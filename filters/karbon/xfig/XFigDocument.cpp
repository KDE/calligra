/*  This file is part of the Calligra project, made within the KDE community.

    Copyright 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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
    Boston, MA 02110-1301, USA.
*/

#include "XFigDocument.h"


void
XFigBoxObject::setPoints(const QVector<XFigPoint>& points)
{
    if (points.count()!=5) {
        return;
    }

    // format does not specify in which order the corner points are given,
    // so just get the box from all points
    const XFigPoint firstPoint = points.at(0);
    qint32 minX = firstPoint.x();
    qint32 maxX = firstPoint.x();
    qint32 minY = firstPoint.y();
    qint32 maxY = firstPoint.y();
    for (int i = 1; i < 5; ++i) {
        const XFigPoint point = points.at(i);

        const qint32 x = point.x();
        if (x < minX) {
            minX = x;
        } else if (maxX < x) {
            maxX = x;
        }

        const qint32 y = point.y();
        if (y < minY) {
            minY = y;
        } else if (maxY < y) {
            maxY = y;
        }
    }

    m_UpperLeftCorner = XFigPoint(minX, minY);
    m_Width = (maxX - minX + 1);
    m_Height = (maxY - minY + 1);
}

static void
fillColorTable( QHash<int, QColor>& colorTable )
{
    static const unsigned int colorValues[24] = {
        // four shades of blue (dark to lighter)
        0x000090, 0x0000b0, 0x0000d0, 0x87ceff,
        // three shades of green (dark to lighter)
        0x009000, 0x00b000, 0x00d000, 0x009090,
        // three shades of cyan (dark to lighter)
        0x00b0b0, 0x00d0d0, 0x900000, 0xb00000,
        // three shades of red (dark to lighter)
        0xd00000, 0x900090, 0xb000b0, 0xd000d0,
        // three shades of magenta (dark to lighter)
        0x803000, 0xa04000, 0xc06000, 0xff8080,
        // three shades of brown (dark to lighter)
        0xffa0a0, 0xffc0c0, 0xffe0e0,
        // gold
        0xffd700
    };

    colorTable.insert(0, QColor(Qt::black));
    colorTable.insert(1, QColor(Qt::blue));
    colorTable.insert(2, QColor(Qt::green));
    colorTable.insert(3, QColor(Qt::cyan));
    colorTable.insert(4, QColor(Qt::red));
    colorTable.insert(5, QColor(Qt::magenta));
    colorTable.insert(6, QColor(Qt::yellow));
    colorTable.insert(7, QColor(Qt::white));

    for (int i = 8; i < 32; i++)
        colorTable.insert(i, QColor(colorValues[i-8]));
}


XFigDocument::XFigDocument()
  : m_PageOrientation(XFigPageOrientationUnknown)
  , m_CoordSystemOriginType(XFigCoordSystemOriginTypeUnknown)
  , m_UnitType(XFigUnitTypeUnknown)
  , m_PageSizeType(XFigPageSizeUnknown)
  , m_Resolution(1200)
{
    fillColorTable( m_ColorTable );
}

const QColor*
XFigDocument::color(int id) const
{
    QHash<int, QColor>::ConstIterator it = m_ColorTable.constFind(id);

    return (it!=m_ColorTable.constEnd()) ? &it.value() : 0;
}
