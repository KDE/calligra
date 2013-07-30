/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "CAImageProvider.h"

#include <kglobal.h>

const char *CAImageProvider::identificationString = "caimage";
CAImageProvider *CAImageProvider::s_imageProvider = 0;

CAImageProvider::CAImageProvider()
    : QDeclarativeImageProvider(Image)
{
}

CAImageProvider::~CAImageProvider()
{

}

QImage CAImageProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    if (m_images.contains(id)) {
        QImage image = m_images.value(id);
        *size = image.size();
        return requestedSize.isValid() ? image.scaled(requestedSize, Qt::KeepAspectRatioByExpanding) : image;
    }
    size = new QSize();
    return QImage();
}

void CAImageProvider::addImage(const QString& id, const QImage& image)
{
    m_images[id] = image;
}

bool CAImageProvider::containsId(const QString& id)
{
    return m_images.contains(id);
}
