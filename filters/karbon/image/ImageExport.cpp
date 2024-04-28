/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2004 Rob Buis <buis@kde.org>
   SPDX-FileCopyrightText: 2002 Lennart Kudling <kudling@kde.org>
   SPDX-FileCopyrightText: 2002 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 2004 Nicolas Goutte <nicolasg@snafu.de>
   SPDX-FileCopyrightText: 2005 Tim Beaulen <tbscope@gmail.com>
   SPDX-FileCopyrightText: 2005 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2005-2006 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2006 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2006 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2006 Christian Mueller <cmueller@gmx.de>
   SPDX-FileCopyrightText: 2007-2008, 2012 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2019 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ImageExport.h"
#include "ImageExportOptionsWidget.h"

#include <KarbonDocument.h>
#include <KoDocument.h>

#include <KoDialog.h>
#include <KoFilter.h>
#include <KoFilterChain.h>
#include <KoFilterManager.h>
#include <KoPAPageBase.h>
#include <KoShapePainter.h>
#include <KoUnit.h>
#include <KoZoomHandler.h>

#include <KPluginFactory>

#include <QImage>

K_PLUGIN_FACTORY_WITH_JSON(PngExportFactory, "calligra_filter_karbon2image.json", registerPlugin<ImageExport>();)

ImageExport::ImageExport(QObject *parent, const QVariantList &)
    : KoFilter(parent)
{
}

KoFilter::ConversionStatus ImageExport::convert(const QByteArray &from, const QByteArray &to)
{
    if (from != KoOdf::mimeType(KoOdf::Graphics)) {
        return KoFilter::BadMimeType;
    }

    QString format;

    if (to == "image/png") {
        format = "PNG";
    } else if (to == "image/jpeg") {
        format = "JPEG";
    } else {
        return KoFilter::BadMimeType;
    }

    KarbonDocument *doc = dynamic_cast<KarbonDocument *>(m_chain->inputDocument());
    if (!doc) {
        return KoFilter::InternalError;
    }

    KoPAPageBase *page = doc->pages().first();

    KoShapePainter painter;
    painter.setShapes(page->shapes());

    // get the bounding rect of the content
    QRectF shapesRect = painter.contentRect();
    // get the size in point
    QSizeF pointSize = shapesRect.size();
    // get the size in pixel (100% zoom)
    KoZoomHandler zoomHandler;
    QSize pixelSize = zoomHandler.documentToView(pointSize).toSize();
    // transparent white by default
    QColor backgroundColor(QColor(255, 255, 255, 0));

    if (!m_chain->manager()->getBatchMode()) {
        QApplication::restoreOverrideCursor();
        ImageExportOptionsWidget *widget = new ImageExportOptionsWidget(doc);
        widget->setUnit(doc->unit());
        widget->setBackgroundColor(backgroundColor);
        widget->enableBackgroundOpacity(format == "PNG");

        KoDialog dlg;
        dlg.setCaption(i18n("%1 Export Options", format));
        dlg.setButtons(KoDialog::Ok | KoDialog::Cancel);
        dlg.setMainWidget(widget);
        int result = dlg.exec();
        QApplication::setOverrideCursor(Qt::BusyCursor);
        if (result != QDialog::Accepted) {
            return KoFilter::UserCancelled;
        }
        pixelSize = widget->pixelSize();
        backgroundColor = widget->backgroundColor();

        page = widget->page();
        if (!page) {
            return KoFilter::InternalError;
        }
        painter.setShapes(page->shapes());
    }

    QImage image(pixelSize, QImage::Format_ARGB32);

    // draw the background of the image
    image.fill(backgroundColor.rgba());

    // paint the shapes
    painter.paint(image);

    if (!image.save(m_chain->outputFile(), format.toLatin1())) {
        return KoFilter::CreationError;
    }

    return KoFilter::OK;
}

#include "ImageExport.moc"
