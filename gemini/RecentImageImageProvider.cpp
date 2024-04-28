/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "RecentImageImageProvider.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QImageReader>
#include <QPainter>
#include <QProcess>

#include <KoDocument.h>
#include <KoPart.h>
#include <KoStore.h>

RecentImageImageProvider::RecentImageImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage RecentImageImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    int width = 512;
    int height = 512;
    if (id.endsWith(QLatin1String("odt"), Qt::CaseInsensitive) || id.endsWith(QLatin1String("doc"), Qt::CaseInsensitive)
        || id.endsWith(QLatin1String("docx"), Qt::CaseInsensitive)) {
        width *= 0.72413793;
    } else {
        height *= 0.72413793;
    }

    if (size) {
        *size = QSize(width, height);
    }

    QSize sz(requestedSize.width() > 0 ? requestedSize.width() : width, requestedSize.height() > 0 ? requestedSize.height() : height);

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
            if (thumbnailImage.loadFromData(thumbnailData)) { //&&
                // thumbnailImage.width() >= width && thumbnailImage.height() >= height) {
                //  put a white background behind the thumbnail
                //  as lots of old(?) OOo files have thumbnails with transparent background
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

        if (!thumbnailFound) {
            // load document and render the thumbnail ourselves
            QProcess thumbnailer;
            QString thumbnailerProgram = QString("%1%2calligrageminithumbnailhelper").arg(qApp->applicationDirPath()).arg(QDir::separator());
            QStringList arguments;
            arguments << "--in" << id;
            QString thumbFile = id;
            thumbFile.replace("/", "-").replace("\\", "-");
            thumbFile.prepend(QDir::separator()).prepend(QDir::tempPath());
            thumbFile.append(".png");
            arguments << "--out" << thumbFile;
            arguments << "--width" << QString::number(sz.width());
            arguments << "--height" << QString::number(sz.height());
            bool fileExists = QFile::exists(thumbFile);
            if (!fileExists)
                thumbnailer.start(thumbnailerProgram, arguments);
            if (fileExists || thumbnailer.waitForFinished(3000)) {
                thumbnail.load(thumbFile);
                thumbnail = thumbnail.scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            } else {
                // some error, final failure...
                qDebug() << "Failed completely to find a preview for" << id;
            }
        }
    }
    return thumbnail;
}
