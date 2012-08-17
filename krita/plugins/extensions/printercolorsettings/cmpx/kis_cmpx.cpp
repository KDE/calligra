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

#include <KoColorProfile.h>
#include <QtCore>
#include <KoColorSpaceRegistry.h>
#include <kis_image.h>

#include <stdlib.h>

KisCmpx::KisCmpx()
{
    m_colormanager = cmpxAPI_initialize();
}

int KisCmpx::setPrinter(const QPrinter *printer)
{
    int error = 0;
  
    QString name = printer->printerName();
    const char *printerName = strdup(name.toLocal8Bit());
    
    error = cmpxAPI_setCurrentPrinter(&m_colormanager, printerName);
    
    return error;
}

int KisCmpx::setPdfFile(const QString pdfFile)
{
    int error = 0;
    const char *fileName = strdup(pdfFile.toLocal8Bit());
    
    error = cmpxAPI_setPdfFile(&m_colormanager, fileName);
        
    return error;
}

int KisCmpx::setProfile(const KoColorProfile *profile)
{
    int error = 0;
    
    if (profile) {    
        QString name = profile->fileName();    
        const char *profilePath = strdup(name.toLocal8Bit());
    
        error = cmpxAPI_setProfile(&m_colormanager, profilePath);
    } else {
        error = 1;
    }
        
    return error;
}

QString KisCmpx::printerName(void)
{
    int error = 0;
    QString printerName = "";
    char *name = new char[256];
    
    error = cmpxAPI_getCurrentPrinter(m_colormanager, &name);
    
    if (!error) {
        printerName = QString(name);
    }
    
    delete[] name;
    
    return printerName;
}

QString KisCmpx::profileName(void)
{
    int error = 0;
    QString profileName = "";
    char *name = new char[256];
    
    error = cmpxAPI_getProfile(m_colormanager, &name, CMPX_ICC_STRING_NAME); 
    
    if (!error) {
        profileName = KoColorSpaceRegistry::instance()->profileAlias(QString(name));
    }
    
    delete[] name;
    
    return profileName;  
}

QString KisCmpx::pdfFile(void)
{
    int error = 0;
    QString pdfFileName = "";
    char *name = new char[256];
    
    error = cmpxAPI_getCurrentPrinter(m_colormanager, &name);
    
    if (!error) {
        pdfFileName = QString(name);
    }
    
    delete[] name;
    
    return pdfFileName;
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

        if (currentPrinterName == printerName) {
            ++printerFound;
        }
    }
    
    if (printerFound) {
        printer = new QPrinter(printerList.at(index), QPrinter::ScreenResolution);    
    }
    
    return printer;
}

QFuture<void> KisCmpx::setAutoProfile()
{
    // Automatic profile selection takes a little time at the moment.
    return QtConcurrent::run(cmpxAPI_setAutoProfile, &m_colormanager);    
}

// TODO Handle case with no profile (or 'target printing')
QString KisCmpx::renderSpoolPdf(KisImageWSP image, 
                                const KoColorProfile* profile)
{
    QString pdfFileName;
    pdfFileName = convertImageToPdf(image, "/tmp/kis_cmpx_spool.pdf");
    
    setPdfFile(pdfFileName);
    setProfile(profile);
    
    int renderError = cmpxAPI_setSpoolPdf(&m_colormanager, CMPX_RENDERMODE_NORMAL);
    
    if (renderError) 
        pdfFileName = "";
    
    return pdfFileName;
}

QImage KisCmpx::renderPreviewImage(KisImageWSP image, const KoColorProfile *profile)
{
    QImage previewImage;
  
    int error = 0;    
    char *renderedPreviewFileName = 0;
    
    // NOTE: Where sould temporary spool files go?
    QString tempPdf = convertImageToPdf(image, "/tmp/kis_cmpx_rendererd.pdf");
    
    setPdfFile(tempPdf);
    setProfile(profile);
    
    renderedPreviewFileName = new char[256];  
        
    error = cmpxAPI_getPreviewImage(m_colormanager, &renderedPreviewFileName, 1, 0);   
    
    if (!error) {
        previewImage = QImage(QString(renderedPreviewFileName), 0);
    }
    
    delete[] renderedPreviewFileName;
    
    return previewImage;
}

QString KisCmpx::convertImageToPdf(KisImageWSP image, const QString name)
{
    QString pdfFileName;
    
    if (!name.endsWith(".pdf", Qt::CaseInsensitive)) 
        pdfFileName.append(name + ".pdf");
    else 
        pdfFileName.append(name);
      
    QRect r = image->bounds();        
    QImage convertedImage = image->convertToQImage(0, 0, r.width(), r.height(), 0);
    
    QPrinter output;
    output.setOutputFormat(QPrinter::PdfFormat);
    output.setOutputFileName(pdfFileName);
    
    QPainter painter(&output);
    painter.drawImage(r, convertedImage);
    
    return pdfFileName;
}

KisCmpx::~KisCmpx()
{
    cmpxAPI_close(&m_colormanager);
}