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

#ifndef COVERIMAGE_H
#define COVERIMAGE_H

#include <QObject>
#include <QPair>

class KoStore;
class KoXmlWriter;

class CoverImage
{
public:
    explicit CoverImage();
    bool saveCoverImage(KoStore *store, KoXmlWriter *manifestWriter, QPair<QString, QByteArray> coverData); // save cover to store.

    /**
     * Give the image @par path and read and save its data.
     */
    QPair<QString, QByteArray> getCoverData(QString path);
};

#endif // COVERIMAGE_H
