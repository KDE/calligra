/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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

#include "ImageBuilder.h"

#include <QApplication>
#include <QDesktopWidget>

#include <KoColorSpaceRegistry.h>

#include <kis_part2.h>
#include <kis_doc2.h>
#include <kis_image.h>
#include <kis_config.h>
#include <kis_clipboard.h>
#include <kis_layer.h>
#include <kis_painter.h>

ImageBuilder::ImageBuilder(QObject* parent)
{

}

ImageBuilder::~ImageBuilder()
{

}

QString ImageBuilder::createBlankImage(int width, int height, int resolution)
{
    KisPart2 part;
    KisDoc2 doc(&part);
    part.setDocument(&doc);

    doc.newImage("Blank Image", width, height, KoColorSpaceRegistry::instance()->rgb8());
    doc.image()->setResolution(resolution / 72.0, resolution / 72.0);

    return saveDocument(doc);
}

QString ImageBuilder::createImageFromClipboard()
{
    KisPart2 part;
    KisDoc2 doc(&part);
    part.setDocument(&doc);

    KisConfig cfg;
    cfg.setPasteBehaviour(PASTE_ASSUME_MONITOR);

    QSize sz = KisClipboard::instance()->clipSize();
    KisPaintDeviceSP clipDevice = KisClipboard::instance()->clip(QPoint(0,0));

    if (clipDevice) {

        doc.newImage("From Clipboard", sz.width(), sz.height(), clipDevice->colorSpace());

        KisImageWSP image = doc.image();
        if (image && image->root() && image->root()->firstChild()) {
            KisLayer * layer = dynamic_cast<KisLayer*>(image->root()->firstChild().data());
            Q_ASSERT(layer);
            layer->setOpacity(OPACITY_OPAQUE_U8);
            QRect r = clipDevice->exactBounds();

            KisPainter painter;
            painter.begin(layer->paintDevice());
            painter.setCompositeOp(COMPOSITE_COPY);
            painter.bitBlt(QPoint(0, 0), clipDevice, r);
            layer->setDirty(QRect(0, 0, sz.width(), sz.height()));
        }
    }
    else {

        doc.newImage("Blank Image", qApp->desktop()->width(), qApp->desktop()->height(), KoColorSpaceRegistry::instance()->rgb8());
        doc.image()->setResolution(1.0, 1.0);
    }
    return saveDocument(doc);
}

QString ImageBuilder::createImageFromWebcam(int width, int height, int resolution)
{
    return QString();
}

void ImageBuilder::discardImage(const QString& path)
{
    QFile::remove(path);
}

QString ImageBuilder::saveDocument(KisDoc2 &doc)
{
    QString path = QString("%1/kritasketch/%2.kra").arg(QDir::tempPath()).arg(QDateTime::currentDateTime().toMSecsSinceEpoch());
    QDir::temp().mkpath("kritasketch");
    if(doc.documentPart()->saveAs(KUrl(path))) {
        return path;
    }

    qWarning() << "Could not create temporary image! Disk full?";
    return QString();
}

