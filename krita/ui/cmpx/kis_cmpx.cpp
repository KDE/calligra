/*
 *  Copyright (c) 2012 Joseph Simon <jsimon383@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_cmpx.h"

#include <stdlib.h>
#include <QtCore>

#include <KoColorProfile.h>
#include <KoColorSpaceRegistry.h>
#include <kis_image.h>
#include "kis_config.h"

static const char KisCmpxSpoolPdfFile[] = "kis_cmpx_spool.pdf";
static const char KisCmpxPreviewPdfFile[] = "kis_cmpx_preview.pdf";

KisCmpx::KisCmpx() : KoCmpx()
{    
}

int KisCmpx::setPrinter(const QPrinter *printer)
{
    int error = setPrinterName(printer->printerName().toLocal8Bit());    
    return error;
}

int KisCmpx::setPdfFile(const QString pdfFile)
{
    int error = setRenderFileName(pdfFile.toLocal8Bit());        
    return error;
}

int KisCmpx::setProfile(const KoColorProfile *profile)
{
    int error = 0; 
    
    if (profile->isSuitableForPrinting()) {
        QByteArray name = profile->fileName().toLocal8Bit();
        setProfileName(name); 
    } else {
        error = 1;
    }
    
    if (!error) 
        saveProfile();
    
    return error;
}

int KisCmpx::setProfile(const KoColorSpace *colorSpace)
{    
    int error = 0;
    
    const KoColorProfile *profile = colorSpace->profile();
    
    if (profile->isSuitableForPrinting()) {
        QByteArray name = profile->fileName().toLocal8Bit();
        error = setProfileName(name);
    } else {
        error = 1; 
    }
    
    if (!error) 
        saveProfile();
    
    return error;
}

QString KisCmpx::pdfFile(void)
{
    return pdfFileName();
}

const KoColorProfile * KisCmpx::profile()
{    
    QString name = profileName();
    return KoColorSpaceRegistry::instance()->profileByName(name);
}

QPrinter * KisCmpx::printer(void)
{
    QPrinter *printer = 0;    
    int printerFound = 0; 
    int index = 0;
    
    QString currentPrinterName = printerName();
    
    QList<QPrinterInfo> printerList = QPrinterInfo::availablePrinters();    
    
    for (index = 0; index < printerList.count(), !printerFound; index++) {
        QString printerName = printerList.at(index).printerName();

        if (currentPrinterName == printerName) 
            ++printerFound;
    }
    
    if (printerFound) 
        printer = new QPrinter(printerList.at(index), QPrinter::ScreenResolution);    
    
    return printer;
}

QFuture<void> KisCmpx::setAutoProfile()
{
    return setAutoProfile();    
}

QString KisCmpx::renderSpoolPdf(KisImageWSP image, 
                                const KoColorProfile* profile)
{

    QString pdfFile = convertImageToPdf(image, KisCmpxSpoolPdfFile);
    
    int renderError = 0;
    
    setPdfFile(pdfFile);    
    
    if (profile) {
        setProfile(profile);      
        renderError = renderSpool();
    } else {
        renderError = renderTargetPrintSpool();
    }
    
    if (renderError) 
        pdfFile = "";
    else
        pdfFile = pdfFileName();
    
    return pdfFile;
}

QImage KisCmpx::renderPreviewImage(KisImageWSP image, const KoColorProfile *profile)
{
    QImage previewImage;    
    QString tempPdf = convertImageToPdf(image, KisCmpxPreviewPdfFile);
    
    if (!tempPdf.isEmpty()) {
        setPdfFile(tempPdf);
        
        setProfile(profile);
        previewImage = createPreviewImage();
    }
    
    return previewImage;
}

void KisCmpx::saveProfile(void)
{
    KisConfig cfg;
    QString name = KoColorSpaceRegistry::instance()->profileAlias(profileName());
    
    cfg.setPrinterProfile(name);    
}

void KisCmpx::loadProfile(void)
{
}

QString KisCmpx::convertImageToPdf(KisImageWSP image, const QString name)
{
    QString pdfFileName;
    
    if (!name.endsWith(".pdf", Qt::CaseInsensitive)) 
        pdfFileName.append(name + ".pdf");
    else 
        pdfFileName.append(name);
    
    QPrinter *output = printer();
    output->setOutputFormat(QPrinter::PdfFormat);
    output->setOutputFileName(pdfFileName);
    output->setPageMargins(0.0, 0.0, 0.0, 0.0, QPrinter::Point);
    output->setPaperSize(QSizeF(image->width()/ (72.0 * image->xRes()), 
                         image->height()/ (72.0 * image->yRes())), 
                         QPrinter::Inch);
    
    double scaleX = output->resolution() / (72.0 * image->xRes());
    double scaleY = output->resolution() / (72.0 * image->yRes());    
    
    QRect r = image->bounds();        
    QImage convertedImage = image->convertToQImage(0, 0, r.width(), r.height(), profile());    
    
    QPainter painter(output);
    painter.scale(scaleX, scaleY);
    painter.drawImage(r.x(), r.y(), convertedImage, 0, 0, r.width(), r.height());
    
    return pdfFileName;
}

