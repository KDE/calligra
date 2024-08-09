/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Shantanu Tushar <shantanu@kde.org>
 * SPDX-FileCopyrightText: 2013 Sujith Haridasan <sujith.h@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#include "CQImageProvider.h"

const char CQImageProvider::identificationString[] = "cqimage";
CQImageProvider *CQImageProvider::s_imageProvider = 0;

CQImageProvider::CQImageProvider()
    : QQuickImageProvider(Image)
{
}

CQImageProvider::~CQImageProvider()
{
}

QImage CQImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    if (m_images.contains(id)) {
        QImage image = m_images.value(id);
        *size = image.size();
        return requestedSize.isValid() ? image.scaled(requestedSize, Qt::KeepAspectRatioByExpanding) : image;
    }
    *size = QSize();
    return QImage();
}

void CQImageProvider::addImage(const QString &id, const QImage &image)
{
    m_images.insert(id, image);
}

bool CQImageProvider::containsId(const QString &id)
{
    return m_images.contains(id);
}

void CQImageProvider::clearCache()
{
    m_images.clear();
}
