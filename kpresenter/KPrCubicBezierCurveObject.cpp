// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2001 Toshitaka Fujioka <fujioka@kde.org>
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

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

#include "KPrCubicBezierCurveObject.h"
#include "KPrCubicBezierCurveObjectIface.h"
#include "KPrUtils.h"
#include <KoTextZoomHandler.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <qdom.h>
#include "KoPointArray.h"
#include <kdebug.h>

#include <math.h>
using namespace std;

KPCubicBezierCurveObject::KPCubicBezierCurveObject()
    : KPPointObject()
{
}

KPCubicBezierCurveObject::KPCubicBezierCurveObject( const KoPointArray &_controlPoints,
                                                    const KoPointArray &_allPoints,
                                                    const KoSize &_size, const KPPen &_pen,
                                                    LineEnd _lineBegin, LineEnd _lineEnd )
    : KPPointObject( _pen, _lineBegin, _lineEnd )
{
    points = KoPointArray( _controlPoints );
    allPoints = KoPointArray( _allPoints );

    ext = _size;
}

KPCubicBezierCurveObject &KPCubicBezierCurveObject::operator=( const KPCubicBezierCurveObject & )
{
    return *this;
}

DCOPObject* KPCubicBezierCurveObject::dcopObject()
{
    if ( !dcop )
        dcop = new KPCubicBezierCurveObjectIface( this );
    return dcop;
}

bool KPCubicBezierCurveObject::saveOasisObjectAttributes( KPOasisSaveContext &sc ) const
{
    kdDebug()<<"bool KPCubicBezierCurveObject::saveOasis( KoXmlWriter & xmlWriter ) not implemented\n";
    return true;
}

const char * KPCubicBezierCurveObject::getOasisElementName() const
{
    return "draw:custom-shape";
}

void KPCubicBezierCurveObject::loadOasis( const QDomElement &element, KoOasisContext & context,  KPRLoadingInfo* info )
{
    //todo
    //we use draw:path

    //load marker
    loadOasisMarker( context );
}

QDomDocumentFragment KPCubicBezierCurveObject::save( QDomDocument& doc, double offset )
{
    return KPPointObject::save( doc,offset );
}

double KPCubicBezierCurveObject::load(const QDomElement &element)
{
    double offset = KPPointObject::load( element );

    allPoints = getCubicBezierPointsFrom( points );

    return offset;
}

void KPCubicBezierCurveObject::updatePoints( double _fx, double _fy )
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

KoPointArray KPCubicBezierCurveObject::getCubicBezierPointsFrom( const KoPointArray &_pointArray )
{
    if ( _pointArray.isNull() )
        return _pointArray;

    KoPointArray _points( _pointArray );
    KoPointArray _allPoints;
    unsigned int pointCount = _points.count();

    if ( pointCount == 2 ) { // line
        _allPoints = _points;
    }
    else { // cubic bezier curve
        KoPointArray tmpPointArray;
        unsigned int _tmpIndex = 0;
        unsigned int count = 0;
        while ( count < pointCount ) {
            if ( pointCount >= ( count + 4 ) ) { // for cubic bezier curve
                double _firstX = _points.at( count ).x();
                double _firstY = _points.at( count ).y();

                double _fourthX = _points.at( count + 1 ).x();
                double _fourthY = _points.at( count + 1 ).y();

                double _secondX = _points.at( count + 2 ).x();
                double _secondY = _points.at( count + 2 ).y();

                double _thirdX = _points.at( count + 3 ).x();
                double _thirdY = _points.at( count + 3 ).y();

                KoPointArray _cubicBezierPoint;
                _cubicBezierPoint.putPoints( 0, 4, _firstX,_firstY, _secondX,_secondY,
                                             _thirdX,_thirdY, _fourthX,_fourthY );
                _cubicBezierPoint = _cubicBezierPoint.cubicBezier();

                KoPointArray::ConstIterator it;
                for ( it = _cubicBezierPoint.begin(); it != _cubicBezierPoint.end(); ++it ) {
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

void KPCubicBezierCurveObject::flip(bool horizontal )
{
    KPPointObject::flip( horizontal );

    KoPointArray tmpPoints;
    int index = 0;
    if ( ! horizontal )
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

void KPCubicBezierCurveObject::closeObject(bool _close)
{
    allPoints = getCloseObject( allPoints, _close, isClosed() );
}

bool KPCubicBezierCurveObject::isClosed()const
{
    return ( allPoints.at(0) == allPoints.at(allPoints.count()-1) );
}


KoPointArray KPCubicBezierCurveObject::getDrawingPoints() const
{
  return allPoints;
}
