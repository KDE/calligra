/* This file is part of the KDE project
   Copyright (C) 2002-2004 Rob Buis <buis@kde.org>
   Copyright (C) 2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2004 Nicolas Goutte <nicolasg@snafu.de>
   Copyright (C) 2005 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>
   Copyright (C) 2005-2006 David Faure <faure@kde.org>
   Copyright (C) 2006 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2006 Christian Mueller <cmueller@gmx.de>
   Copyright (C) 2007-2008 Jan Hambrecht <jaham@gmx.net>

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

#include "PngExport.h"
#include "PngExportOptionsWidget.h"

#include <KarbonDocument.h>
#include <KarbonPart.h>

#include <KoShapePainter.h>
#include <KoFilter.h>
#include <KoFilterChain.h>
#include <KoFilterManager.h>
#include <KoZoomHandler.h>

#include <kgenericfactory.h>
#include <KDialog>
#include <QImage>

typedef KGenericFactory<PngExport> PngExportFactory;
K_EXPORT_COMPONENT_FACTORY(libkarbonpngexport, PngExportFactory("kofficefilters"))


PngExport::PngExport(QObject*parent, const QStringList&)
        : KoFilter(parent)
{
}

KoFilter::ConversionStatus
PngExport::convert(const QByteArray& from, const QByteArray& to)
{
    if (to != "image/png" || from != "application/vnd.oasis.opendocument.graphics") {
        return KoFilter::NotImplemented;
    }

    KoDocument * document = m_chain->inputDocument();
    if (! document)
        return KoFilter::ParsingError;

    KarbonPart * karbonPart = dynamic_cast<KarbonPart*>(document);
    if (! karbonPart)
        return KoFilter::WrongFormat;

    KoShapePainter painter;
    painter.setShapes(karbonPart->document().shapes());

    // get the bounding rect of the content
    QRectF shapesRect = painter.contentRect();
    // get the size on point
    QSizeF pointSize = shapesRect.size();
    // get the size in pixel (100% zoom)
    KoZoomHandler zoomHandler;
    QSize pixelSize = zoomHandler.documentToView(pointSize).toSize();
    QColor backgroundColor(Qt::white);

    if (! m_chain->manager()->getBatchMode()) {
        PngExportOptionsWidget * widget = new PngExportOptionsWidget(pointSize);
        widget->setUnit(karbonPart->unit());
        widget->setBackgroundColor(backgroundColor);

        KDialog dlg;
        dlg.setCaption(i18n("PNG Export Options"));
        dlg.setButtons(KDialog::Ok | KDialog::Cancel);
        dlg.setMainWidget(widget);
        if (dlg.exec() != QDialog::Accepted)
            return KoFilter::UserCancelled;

        pixelSize = widget->pixelSize();
        backgroundColor = widget->backgroundColor();
    }
    QImage image(pixelSize, QImage::Format_ARGB32);

    // draw the background of the image
    image.fill(backgroundColor.rgba());

    // paint the shapes
    painter.paint(image);
    image.save(m_chain->outputFile(), "PNG");

    return KoFilter::OK;
}

#include "PngExport.moc"

