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
#include <qpoint.h>
#include <qcolor.h>
#include <qsize.h>
#include <kozoomhandler.h>
#include <koPoint.h>
/*========================== draw a figure =======================*/
void drawFigure( LineEnd figure, QPainter* painter, const KoPoint &coord, const QColor &color,int  _w, float angle, KoZoomHandler*_zoomHandler)
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
        painter->translate( _zoomHandler->zoomItX(coord.x()), _zoomHandler->zoomItY( coord.y()) );
        painter->rotate( angle );
        painter->scale( 1, 1 );
        painter->fillRect( _zoomHandler->zoomItX(-3 - _w / 2),_zoomHandler->zoomItY( -3 - _h / 2),_zoomHandler->zoomItX( 6 + _w),_zoomHandler->zoomItY( 6 + _h), color );
        painter->restore();
    } break;
    case L_CIRCLE:
    {
        painter->save();
        painter->translate( _zoomHandler->zoomItX(coord.x()), _zoomHandler->zoomItY(coord.y()) );
        painter->setBrush( color );
        painter->drawEllipse( _zoomHandler->zoomItX(-3 - _w / 2), _zoomHandler->zoomItY(-3 - _w / 2),_zoomHandler->zoomItX( 6 + _w), _zoomHandler->zoomItY(6 + _w) );
        painter->restore();
    } break;
    case L_ARROW:
    {
        QPoint p1( -5 - _w / 2, -3 - _w / 2 );
        QPoint p2( 5 + _w / 2, 0 );
        QPoint p3( -5 - _w / 2, 3 + _w / 2 );
        QPointArray pArray( 3 );
        pArray.setPoint( 0, _zoomHandler->zoomPoint(p1) );
        pArray.setPoint( 1, _zoomHandler->zoomPoint(p2) );
        pArray.setPoint( 2, _zoomHandler->zoomPoint(p3) );

        painter->save();
        painter->translate( _zoomHandler->zoomItX(coord.x()),_zoomHandler->zoomItY( coord.y()) );
        painter->rotate( angle );
        painter->scale( 1, 1 );
        painter->setBrush( color );
        painter->drawPolygon( pArray );
        painter->restore();
    } break;
    default: break;
    }
}
//todo used kozoomhandled
/*================== get bounding with of figure =================*/
KoSize getBoundingSize( LineEnd figure, double _w, KoZoomHandler*_zoomHandler )
{
    switch ( figure )
    {
    case L_SQUARE:
    {
        int _h = (int)_w;
        if ( _h % 2 == 0 ) _h--;
        return KoSize( _zoomHandler->zoomItX( 10 + _w), _zoomHandler->zoomItY( 10 + _h) );
    } break;
    case L_CIRCLE:
        return KoSize(  _zoomHandler->zoomItX(10 + _w), _zoomHandler->zoomItY(10 + _w) );
        break;
    case L_ARROW:
        return KoSize( _zoomHandler->zoomItX( 14 + _w),_zoomHandler->zoomItY( 14 + _w) );
        break;
    default: break;
    }

    return KoSize( 0, 0 );
}
