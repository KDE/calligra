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

#include "kpquadricbeziercurveobject.h"
#include "KPQuadricbezierCurveObjectIface.h"
#include "kpresenter_utils.h"

#include <qpainter.h>
#include <qwmatrix.h>
#include <qdom.h>

#include <kdebug.h>
#include <kozoomhandler.h>
#include <math.h>
using namespace std;

KPQuadricBezierCurveObject::KPQuadricBezierCurveObject()
    : KPPointObject()
{
}

KPQuadricBezierCurveObject::KPQuadricBezierCurveObject( const KoPointArray &_controlPoints,
                                                        const KoPointArray &_allPoints,
                                                        const KoSize &_size, const QPen &_pen,
                                                        LineEnd _lineBegin, LineEnd _lineEnd )
    : KPPointObject( _pen, _lineBegin, _lineEnd )
{
    points = KoPointArray( _controlPoints );
    allPoints = KoPointArray( _allPoints );

    ext = _size;
}

DCOPObject* KPQuadricBezierCurveObject::dcopObject()
{
    if ( !dcop )
        dcop = new KPQuadricBezierCurveObjectIface( this );
    return dcop;
}

KPQuadricBezierCurveObject &KPQuadricBezierCurveObject::operator=( const KPQuadricBezierCurveObject & )
{
    return *this;
}

QDomDocumentFragment KPQuadricBezierCurveObject::save( QDomDocument& doc, double offset )
{
    return KPPointObject::save( doc, offset );
}

double KPQuadricBezierCurveObject::load(const QDomElement &element)
{
    double offset = KPPointObject::load( element );

    allPoints = getQuadricBezierPointsFrom( points );

    return offset;
}

void KPQuadricBezierCurveObject::paint( QPainter* _painter,KoZoomHandler*_zoomHandler,
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

    QPointArray pointArray = allPoints.zoomPointArray( _zoomHandler, _w );
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

void KPQuadricBezierCurveObject::updatePoints( double _fx, double _fy )
{
    KPPointObject::updatePoints( _fx, _fy );

    int index = 0;
    KoPointArray tmpPoints;
    KoPointArray::ConstIterator it;
    for ( it = allPoints.begin(); it != allPoints.end(); ++it ) {
        KoPoint point = (*it);
        double tmpX = point.x() * _fx;
        double tmpY = point.y() * _fy;

        tmpPoints.putPoints( index, 1, tmpX,tmpY );
        ++index;
    }
    allPoints = tmpPoints;
}

KoPointArray KPQuadricBezierCurveObject::getQuadricBezierPointsFrom( const KoPointArray &_pointArray )
{
    if ( _pointArray.isNull() )
        return _pointArray;

    KoPointArray _points( _pointArray );
    KoPointArray _allPoints;
    unsigned int pointCount = _points.count();

    if ( pointCount == 2 )  // line
        _allPoints = _points;
    else { // quadric bezier curve
        KoPointArray tmpPointArray;
        unsigned int _tmpIndex = 0;
        unsigned int count = 0;
        while ( count < pointCount ) {
            if ( pointCount >= ( count + 4 ) ) { // for quadric bezier curve
                double _firstX = _points.at( count ).x();
                double _firstY = _points.at( count ).y();

                double _fourthX = _points.at( count + 1 ).x();
                double _fourthY = _points.at( count + 1 ).y();

                double _secondX = _points.at( count + 2 ).x();
                double _secondY = _points.at( count + 2 ).y();

                double _thirdX = _points.at( count + 3 ).x();
                double _thirdY = _points.at( count + 3 ).y();

                KoPointArray _quadricBezierPoint;
                _quadricBezierPoint.putPoints( 0, 4, _firstX,_firstY, _secondX,_secondY, _thirdX,_thirdY, _fourthX,_fourthY );
                _quadricBezierPoint = _quadricBezierPoint.cubicBezier();

                KoPointArray::ConstIterator it;
                for ( it = _quadricBezierPoint.begin(); it != _quadricBezierPoint.end(); ++it ) {
                    KoPoint _point = (*it);
                    tmpPointArray.putPoints( _tmpIndex, 1, _point.x(), _point.y() );
                    ++_tmpIndex;
                }

                count += 4;
            }
            else { // for line
                double _x1 = _points.at( count ).x();
                double _y1 = _points.at( count ).y();

                double _x2 = _points.at( count + 1 ).x();
                double _y2 = _points.at( count + 1 ).y();

                tmpPointArray.putPoints( _tmpIndex, 2, _x1,_y1, _x2,_y2 );
                _tmpIndex += 2;
                count += 2;
            }
        }

        _allPoints = tmpPointArray;
    }

    return _allPoints;
}

void KPQuadricBezierCurveObject::flip( bool horizontal )
{
    KPPointObject::flip( horizontal );
    
    KoPointArray tmpPoints;
    int index = 0;
    if ( horizontal )
    {
        KoPointArray::ConstIterator it;
        double horiz = getSize().height()/2;
        for ( it = allPoints.begin(); it != allPoints.end(); ++it )
        {
            KoPoint point = (*it);
            if ( point.y()> horiz )
                tmpPoints.putPoints( index, 1, point.x(),point.y()- 2*(point.y()-horiz) );
            else
                tmpPoints.putPoints( index, 1, point.x(),point.y()+ 2*(horiz - point.y()) );
            ++index;
        }
    }
    else
    {
        KoPointArray::ConstIterator it;
        double vert = getSize().width()/2;
        for ( it = allPoints.begin(); it != allPoints.end(); ++it )
        {
            KoPoint point = (*it);
            if ( point.y()> vert )
                tmpPoints.putPoints( index, 1, point.x()- 2*(point.x()-vert), point.y() );
            else
                tmpPoints.putPoints( index, 1, point.x()+ 2*(vert - point.x()),point.y() );
            ++index;
        }
    }

    allPoints = tmpPoints;
}

void KPQuadricBezierCurveObject::closeObject(bool _close)
{
    allPoints = getCloseObject( allPoints, _close, isClosed() );
}

bool KPQuadricBezierCurveObject::isClosed()const
{
    return ( allPoints.at(0) == allPoints.at(allPoints.count()-1) );
}
