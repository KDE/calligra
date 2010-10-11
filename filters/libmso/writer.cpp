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

#include <QtCore/QRegExp>

namespace
{
QString format(double v)
{
    static const QString f("%1");
    static const QString e("");
    static const QRegExp r("\\.?0+$");
    return f.arg(v, 0, 'f').replace(r, e);
}

QString mm(double v)
{
    static const QString mm("mm");
    return format(v) + mm;
}
}//namespace

Writer::Writer(KoXmlWriter& xmlWriter, KoGenStyles& kostyles,
               bool stylesxml_)
        : xOffset(0),
        yOffset(0),
        scaleX(1),
        scaleY(1),
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
    return w;
}

qreal Writer::vLength(qreal length)
{
    return length*scaleY;
}

qreal Writer::hLength(qreal length)
{
    return length*scaleX;
}

qreal Writer::vOffset(qreal offset)
{
    return yOffset + offset*scaleY;
}

qreal Writer::hOffset(qreal offset)
{
    return xOffset + offset*scaleX;
}
