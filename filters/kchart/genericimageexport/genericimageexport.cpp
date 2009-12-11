/* This file is part of the KDE project
   Copyright (C) 2006 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <QPixmap>
#include <QPainter>
//Added by qt3to4:
#include <QByteArray>

#include <kmessagebox.h>

#include <KoFilterChain.h>
#include <KoStore.h>
#include <kgenericfactory.h>
#include <KoDocument.h>
#include <KoXmlReader.h>
#include <exportsizedia.h>
#include "genericimageexport.h"
#include "KChartPart.h"


typedef KGenericFactory<GenericImageExport> GenericImageExportFactory;
K_EXPORT_COMPONENT_FACTORY(libkchartgenericimageexport, GenericImageExportFactory("genericimageexport"))


GenericImageExport::GenericImageExport(QObject* parent, const QStringList&)
        : KoFilter(parent)
{
}

GenericImageExport::~GenericImageExport()
{
}


KoFilter::ConversionStatus
GenericImageExport::convert(const QByteArray& from, const QByteArray& to)
{
    // Check for proper conversion.
    if (from != "application/x-kchart" ||
            (to != "image/bmp" &&
             to != "image/jpeg" &&
             to != "video/x-mng" &&
             to != "image/png" &&
             to != "image/x-xbitmap" &&
             to != "image/x-xpixmap"))
        return KoFilter::NotImplemented;

    // Read the contents of the KChart file
    KoStoreDevice* storeIn = m_chain->storageFile("root", KoStore::Read);
    if (!storeIn) {
        KMessageBox::error(0, i18n("Failed to read data."),
                           i18n("Export Error"));
        return KoFilter::FileNotFound;
    }

    // Get the XML tree.
    KoXmlDocument  domIn;
    domIn.setContent(storeIn);
    KoXmlElement   docNode = domIn.documentElement();

    // Read the document from the XML tree.
    KChart::KChartPart  kchartDoc;
    if (!kchartDoc.loadXML(domIn, 0)) {
        KMessageBox::error(0, i18n("Malformed XML data."),
                           i18n("Export Error"));
        return KoFilter::WrongFormat;
    }
    width = 500;
    height = 400;
    ExportSizeDia  *exportDialog = new ExportSizeDia(width, height, 0);
    bool ret = false;
    if (exportDialog->exec()) {
        width  = exportDialog->width();
        height = exportDialog->height();
        ret = true;
    }
    delete exportDialog;
    if (ret) {
        pixmap = QPixmap(width, height);
        QPainter  painter(&pixmap);
        kchartDoc.paintContent(painter, pixmap.rect());
        if (!saveImage(m_chain->outputFile(), to))
            return KoFilter::CreationError;
    }
    return KoFilter::OK;
}

bool GenericImageExport::saveImage(const QString& fileName, const QByteArray& to)
{
    const char* format = NULL;
    if (to == "image/bmp")
        format = "XBM";
    else if (to == "image/jpeg")
        format = "JPEG";
    else if (to == "video/x-mng")
        format = "MNG";
    else if (to == "image/png")
        format = "PNG";
    else if (to == "image/x-xbitmap")
        format = "XBM";
    else if (to == "image/x-xpixmap")
        format = "XPM";
    bool ret = pixmap.save(fileName, format);
    // Save the image.
    if (!ret) {
        KMessageBox::error(0, i18n("Failed to write file."),
                           i18n("%1 Export Error", format));
    }
    return ret;
}

#include "genericimageexport.moc"

