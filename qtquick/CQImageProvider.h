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

#ifndef CQIMAGEPROVIDER_H
#define CQIMAGEPROVIDER_H

#include <QDeclarativeImageProvider>
#include <QHash>

class CQImageProvider : public QDeclarativeImageProvider
{
public:
    static const char identificationString[];
    static CQImageProvider *s_imageProvider;

    CQImageProvider();
    virtual ~CQImageProvider();
    virtual QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize);

    void addImage(const QString& id, const QImage &image);
    bool containsId(const QString &id);
    void clearCache();
private:

    QHash<QString, QImage> m_images;
};

#endif // CQIMAGEPROVIDER_H
