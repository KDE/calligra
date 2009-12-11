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

#include <kmessagebox.h>

#include <KoFilterChain.h>
#include <KoStore.h>
#include <kgenericfactory.h>
#include <KoDocument.h>
#include <exportsizedia.h>

#include "KPrDocument.h"
#include "KPrView.h"
#include "KPrCanvas.h"
#include "genericimageexport.h"

typedef KGenericFactory<GenericImageExport> GenericImageExportFactory;
K_EXPORT_COMPONENT_FACTORY(libkpresentergenericimageexport, GenericImageExportFactory("genericimageexport"))

GenericImageExport::GenericImageExport(QObject *parent, const QStringList&)
        : KoFilter(parent)
{
}

GenericImageExport::~GenericImageExport()
{
}


KoFilter::ConversionStatus
GenericImageExport::convert(const QByteArray& from, const QByteArray& to)
{
    KoDocument * document = m_chain->inputDocument();

    if (!document)
        return KoFilter::StupidError;

    if (strcmp(document->className(), "KPrDocument") != 0) {
        kWarning() << "document isn't a KPrDocument but a "
        << document->className();
        return KoFilter::NotImplemented;
    }

    // Check for proper conversion.
    if (from != "application/x-kpresenter" ||
            (to != "image/bmp" &&
             to != "image/jpeg" &&
             to != "video/x-mng" &&
             to != "image/png" &&
             to != "image/x-xbitmap" &&
             to != "image/x-xpixmap")) {
        kWarning() << "Invalid mimetypes " << to << " " << from;
        return KoFilter::NotImplemented;
    }
    KPrDocument * kpresenterdoc = const_cast<KPrDocument *>(static_cast<const KPrDocument *>(document));

    if (kpresenterdoc->mimeType() != "application/x-kpresenter") {
        kWarning() << "Invalid document mimetype " << kpresenterdoc->mimeType();
        return KoFilter::NotImplemented;
    }

    KoPageLayout layoutPage = kpresenterdoc->pageLayout();
    width =  int(layoutPage.ptWidth);
    height = int(layoutPage.ptHeight);
    bool ret = false;
    ExportSizeDia  *exportDialog = new ExportSizeDia(width, height, 0);
    if (exportDialog->exec()) {
        width  = exportDialog->width();
        height = exportDialog->height();
        ret = true;
    }
    delete exportDialog;
    if (ret) {
        KPrView* view = kpresenterdoc->views().isEmpty() ? 0 : static_cast<KPrView*>(kpresenterdoc->views().first());
        if (view) { // no view if embedded document
            KPrCanvas * canvas = view->getCanvas();
            canvas->drawPageInPix(pixmap, view->getCurrPgNum() - 1, 0, true, width, height);
        } else { //when it's embedded we use just it.
            pixmap = QPixmap(width, height);
            QPainter  painter(&pixmap);
            kpresenterdoc->paintContent(painter, pixmap.rect(), false);
        }
        if (!saveImage(m_chain->outputFile(), to))
            return KoFilter::CreationError;
        return KoFilter::OK;
    }
    return KoFilter::UserCancelled;
}

bool GenericImageExport::saveImage(const QString& fileName, const QByteArray& to)
{
    const char * format = NULL;
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

