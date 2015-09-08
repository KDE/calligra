/* This file is part of the KDE project
   Copyright (C) 2010 by Nokia

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

#include "writer.h"

#include "KoGenStyles.h"

Writer::Writer(KoXmlWriter& xmlWriter, KoGenStyles& kostyles,
               bool stylesxml_)
        : xOffset(0),
        yOffset(0),
        scaleX(1),
        scaleY(1),
        g_rotation(0),
        g_flipH(0),
        g_flipV(0),
        xml(xmlWriter),
        styles(kostyles),
        stylesxml(stylesxml_)
{
}

Writer Writer::transform(const QRectF& oldCoords, const QRectF &newCoords) const
{
    Writer w(xml, styles, stylesxml);
    w.xOffset = xOffset + oldCoords.x() * scaleX;
    w.yOffset = yOffset + oldCoords.y() * scaleY;
    w.scaleX = scaleX * oldCoords.width() / newCoords.width();
    w.scaleY = scaleY * oldCoords.height() / newCoords.height();
    w.xOffset -= w.scaleX * newCoords.x();
    w.yOffset -= w.scaleY * newCoords.y();
    w.g_rotation = g_rotation;
    w.g_flipH = g_flipH;
    w.g_flipV = g_flipV;
    return w;
}

qreal Writer::vLength(qreal length) const
{
    return length*scaleY;
}

qreal Writer::hLength(qreal length) const
{
    return length*scaleX;
}

qreal Writer::vOffset(qreal offset) const
{
    return yOffset + offset*scaleY;
}

qreal Writer::hOffset(qreal offset) const
{
    return xOffset + offset*scaleX;
}
