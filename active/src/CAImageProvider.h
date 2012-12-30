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

#ifndef CAIMAGEPROVIDER_H
#define CAIMAGEPROVIDER_H

#include <QDeclarativeImageProvider>
#include <QHash>

class CAImageProvider : public QDeclarativeImageProvider
{
public:
    static const char *identificationString;
    class Singleton;

    virtual ~CAImageProvider();
    static CAImageProvider *instance();
    virtual QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize);

    void addImage(const QString& id, const QImage &image);
    bool containsId(const QString &id);
private:
    CAImageProvider();
    QHash<QString, QImage> m_images;
};

#endif // CAIMAGEPROVIDER_H
