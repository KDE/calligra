/* This file is part of the KDE project
   Copyright (C) 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

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

#include "CoverImage.h"

#include <KoStore.h>
#include <KoXmlWriter.h>
#include <KoStoreDevice.h>

#include <QMimeDatabase>
#include <QFile>
#include <QPair>

#include "AuthorDebug.h"

#define coverPath "Pictures/coverImage."

CoverImage::CoverImage()
{
}

bool CoverImage::saveCoverImage(KoStore *store, KoXmlWriter *manifestWriter, QPair<QString, QByteArray> coverData)
{
    // There is no cover to save.
    if (coverData.first.isEmpty())
        return true;

    if (!store->open(coverPath + coverData.first)) {
        debugAuthor << "Unable to open"<<coverPath + coverData.first;
        return false;
    }

    KoStoreDevice device(store);
    device.write(coverData.second, coverData.second.size());
    store->close();

    const QString mimetype(QMimeDatabase().mimeTypeForFile(coverPath + coverData.first).name());
    manifestWriter->addManifestEntry(coverPath + coverData.first, mimetype);

    return true;
}

QPair<QString, QByteArray> CoverImage::readCoverImage(QString path)
{
    QFile file (path);
    if (!file.open(QIODevice::ReadOnly)) {
        debugAuthor << "Unable to open" << path;
    }
    QByteArray data = file.readAll();

    QPair<QString, QByteArray> coverData;
    //more flexible, allow to catch all kind of extantions like .png AND .jpeg for example
    coverData.first = path.split('.').last(); //was : "right(3);"
    coverData.second = data;

    file.close();
    return coverData;
}
