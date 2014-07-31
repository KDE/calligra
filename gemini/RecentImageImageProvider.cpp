/* This file is part of the KDE project
 * Copyright (C) 2012 Boudewijn Rempt <boud@valdyas.org>
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

#include "RecentImageImageProvider.h"
#include <QFile>
#include <QImage>
#include <QImageReader>
#include <QPainter>

#include <KoStore.h>
#include <KoDocument.h>
#include <KoDocumentEntry.h>
#include <KoPart.h>
#include <KMimeTypeTrader>
#include <KMimeType>
#include <kio/previewjob.h>

RecentImageImageProvider::RecentImageImageProvider()
    : QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
}

QImage RecentImageImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    int width = 512;
    int height = 512;
    if(id.toLower().endsWith("odt") || id.toLower().endsWith("doc") || id.toLower().endsWith("docx"))
        width *= 0.72413793;
    else
        height *= 0.72413793;

    if (size) {
        *size = QSize(width, height);
    }

    QSize sz(requestedSize.width() > 0 ? requestedSize.width() : width,
             requestedSize.height() > 0 ? requestedSize.height() : height);

    QFile f(id);
    QImage thumbnail(sz, QImage::Format_ARGB32_Premultiplied);
    thumbnail.fill(Qt::white);

    if (f.exists()) {
        // try to use any embedded thumbnail
        KoStore *store = KoStore::createStore(id, KoStore::Read);

        bool thumbnailFound = false;
        if (store &&
            // ODF thumbnail?
            (store->open(QLatin1String("Thumbnails/thumbnail.png")) ||
            // old KOffice/Calligra thumbnail?
            store->open(QLatin1String("preview.png")) ||
            // OOXML?
            store->open(QLatin1String("docProps/thumbnail.jpeg")))) {
            // Hooray! No long delay for the user...
            const QByteArray thumbnailData = store->read(store->size());

            QImage thumbnailImage;
            if (thumbnailImage.loadFromData(thumbnailData) ){//&&
                //thumbnailImage.width() >= width && thumbnailImage.height() >= height) {
                // put a white background behind the thumbnail
                // as lots of old(?) OOo files have thumbnails with transparent background
                thumbnail = QImage(thumbnailImage.size(), QImage::Format_RGB32);
                thumbnail.fill(QColor(Qt::white).rgb());
                QPainter p(&thumbnail);
                p.drawImage(QPoint(0, 0), thumbnailImage);
                p.end();
                thumbnail = thumbnail.scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                delete store;
                thumbnailFound = true;
            }
        }

         if(!thumbnailFound) {
//            qDebug() << "Thumbnail not found embedded in" << id;
//             // load document and render the thumbnail ourselves
//             const QString mimetype = KMimeType::findByPath(id)->name();
//             QString error;
//             KoDocumentEntry documentEntry = KoDocumentEntry::queryByMimeType(mimetype);
//             KoPart* part = documentEntry.createKoPart(&error);
// 
//             if(part) {
//                 KoDocument* document = part->document();
// 
//                 // prepare the document object
//                 document->setCheckAutoSaveFile(false);
//                 document->setAutoErrorHandlingEnabled(false); // don't show message boxes
// 
//                 // load the document content
//                 KUrl url;
//                 url.setPath(id);
//                 if (document->openUrl(url)) {
//                     while(document->isLoading()) {
//                         qApp->processEvents();
//                     }
// 
//                     // render the page on a bigger pixmap and use smoothScale,
//                     // looks better than directly scaling with the QPainter (malte)
//                     const bool usePassedSize = (width > sz.width() && height > sz.height());
//                     const QSize size = usePassedSize ? QSize(width, height) : sz;
//                     thumbnail = document->generatePreview(size).toImage();
// 
//                     document->closeUrl();
//                 }
//                 delete document;
//             }
         }
    }
    return thumbnail;
}
