/* This file is part of the KDE project
 * Copyright (C) 2012 KO GmbH. Contact: Boudewijn Rempt <boud@kogmbh.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "IconImageProvider.h"

#include <QtDeclarative/QDeclarativeEngine>
#include <KDE/KIcon>
#include <KDE/KMimeType>

IconImageProvider::IconImageProvider()
    : QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap)
{

}

QPixmap IconImageProvider::requestPixmap( const QString &id, QSize *size, const QSize &requestedSize )
{
    int width = 64;
    int height = 64;

    if(requestedSize.width() > 0) {
        width = requestedSize.width();
    }

    if(requestedSize.height() > 0) {
        height = requestedSize.height();
    }

    QString requestedIcon = id;
    if(id.contains("by-path")) {
        requestedIcon = KMimeType::iconNameForUrl(KUrl(id.right(id.size() - 8)));
    }
    KIcon icon(requestedIcon);

    if(size) {
        *size = QSize(width, height);
    }

    return icon.pixmap(*size);
}
