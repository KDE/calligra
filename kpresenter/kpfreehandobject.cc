// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2001 Toshitaka Fujioka <fujioka@kde.org>

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
#include "KPFreehandObjectIface.h"
#include "kpfreehandobject.h"
#include "kpresenter_utils.h"
#include <kozoomhandler.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <qdom.h>

#include <kdebug.h>
#include <math.h>
using namespace std;

KPFreehandObject::KPFreehandObject()
    : KPPointObject()
{
}

KPFreehandObject::KPFreehandObject( const KoPointArray &_points, const KoSize &_size,
                                    const QPen &_pen, LineEnd _lineBegin, LineEnd _lineEnd )
    : KPPointObject( _pen, _lineBegin, _lineEnd )
{
    points = KoPointArray( _points );
    ext = _size;
}

KPFreehandObject &KPFreehandObject::operator=( const KPFreehandObject & )
{
    return *this;
}

DCOPObject* KPFreehandObject::dcopObject()
{
    if ( !dcop )
        dcop = new KPFreehandObjectIface( this );
    return dcop;
}

QDomDocumentFragment KPFreehandObject::save( QDomDocument& doc,double offset )
{
    return KPPointObject::save( doc, offset );
}

double KPFreehandObject::load( const QDomElement &element )
{
    return KPPointObject::load( element );
}

void KPFreehandObject::paint( QPainter* _painter,KoZoomHandler*_zoomHandler,
                              bool /*drawingShadow*/, bool drawContour )
{
    int _w = pen.width();

    QPen pen2;
    if ( drawContour ) {
        pen2 = QPen( Qt::black, 1, Qt::DotLine );
        _painter->setRasterOp( Qt::NotXorROP );
    }
    else {
        pen2 = pen;
        pen2.setWidth( _zoomHandler->zoomItX( pen.width() ) );
    }
    _painter->setPen( pen2 );

    QPointArray pointArray = points.zoomPointArray( _zoomHandler, _w );

    _painter->drawPolyline( pointArray );


    if ( lineBegin != L_NORMAL && !drawContour && !isClosed()) {
        QPoint startPoint;
        bool first = true;
        QPointArray::ConstIterator it1;
        for ( it1 = pointArray.begin(); it1 != pointArray.end(); ++it1 ) {
            if ( first ) {
                startPoint = (*it1);
                first = false;
            }

            QPoint point = (*it1);
            if ( startPoint != point ) {
                float angle = KoPoint::getAngle( startPoint, point );
                drawFigureWithOffset( lineBegin, _painter, startPoint, pen2.color(), _w, angle,_zoomHandler );

                break;
            }
        }
    }

    if ( lineEnd != L_NORMAL && !drawContour && !isClosed()) {
        QPoint endPoint;
        bool last = true;
        QPointArray::ConstIterator it2 = pointArray.end();
        for ( it2 = it2 - 1; it2 != pointArray.begin(); --it2 ) {
            if ( last ) {
                endPoint = (*it2);
                last = false;
            }

            QPoint point = (*it2);
            if ( endPoint != point ) {
                float angle = KoPoint::getAngle( endPoint, point );
                drawFigureWithOffset( lineEnd, _painter, endPoint, pen2.color(), _w, angle,_zoomHandler );

                break;
            }
        }
    }
}
