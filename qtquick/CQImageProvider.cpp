/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Shantanu Tushar <shantanu@kde.org>
 * Copyright (C) 2013 Sujith Haridasan <sujith.h@gmail.com>
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
 *
 */

#include "CQImageProvider.h"


const char CQImageProvider::identificationString[] = "cqimage";
CQImageProvider *CQImageProvider::s_imageProvider = 0;

CQImageProvider::CQImageProvider()
    : QDeclarativeImageProvider(Image)
{
}

CQImageProvider::~CQImageProvider()
{

}

QImage CQImageProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    if (m_images.contains(id)) {
        QImage image = m_images.value(id);
        *size = image.size();
        return requestedSize.isValid() ? image.scaled(requestedSize, Qt::KeepAspectRatioByExpanding) : image;
    }
    *size = QSize();
    return QImage();
}

void CQImageProvider::addImage(const QString& id, const QImage& image)
{
    m_images.insert(id, image);
}

bool CQImageProvider::containsId(const QString& id)
{
    return m_images.contains(id);
}

void CQImageProvider::clearCache()
{
    m_images.clear();
}
