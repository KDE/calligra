/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kpresenter_utils.h"

#include <qpainter.h>
#include <qpointarray.h>

#include <qsize.h>

/*========================== draw a figure =======================*/
void drawFigure( LineEnd figure, QPainter* painter, QPoint coord, QColor color, int _w, float angle )
{
    painter->setPen( Qt::NoPen );
    painter->setBrush( Qt::NoBrush );

    switch ( figure )
    {
    case L_SQUARE:
    {
        int _h = _w;
        if ( _h % 2 == 0 ) _h--;
        painter->save();
        painter->translate( coord.x(), coord.y() );
        painter->rotate( angle );
        painter->scale( 1, 1 );
        painter->fillRect( -3 - _w / 2, -3 - _h / 2, 6 + _w, 6 + _h, color );
        painter->restore();
    } break;
    case L_CIRCLE:
    {
        painter->save();
        painter->translate( coord.x(), coord.y() );
        painter->setBrush( color );
        painter->drawEllipse( -3 - _w / 2, -3 - _w / 2, 6 + _w, 6 + _w );
        painter->restore();
    } break;
    case L_ARROW:
    {
        QPoint p1( -5 - _w / 2, -3 - _w / 2 );
        QPoint p2( 5 + _w / 2, 0 );
        QPoint p3( -5 - _w / 2, 3 + _w / 2 );
        QPointArray pArray( 3 );
        pArray.setPoint( 0, p1 );
        pArray.setPoint( 1, p2 );
        pArray.setPoint( 2, p3 );

        painter->save();
        painter->translate( coord.x(), coord.y() );
        painter->rotate( angle );
        painter->scale( 1, 1 );
        painter->setBrush( color );
        painter->drawPolygon( pArray );
        painter->restore();
    } break;
    default: break;
    }
}

/*================== get bounding with of figure =================*/
QSize getBoundingSize( LineEnd figure, int _w )
{
    switch ( figure )
    {
    case L_SQUARE:
    {
        int _h = _w;
        if ( _h % 2 == 0 ) _h--;
        return QSize( 10 + _w, 10 + _h );
    } break;
    case L_CIRCLE:
        return QSize( 10 + _w, 10 + _w );
        break;
    case L_ARROW:
        return QSize( 14 + _w, 14 + _w );
        break;
    default: break;
    }

    return QSize( 0, 0 );
}
