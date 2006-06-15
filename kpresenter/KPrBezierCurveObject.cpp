// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2001 Toshitaka Fujioka <fujioka@kde.org>
   Copyright (C) 2005-2006 Thorsten Zachmann <zachmann@kde.org>

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

#include "KPrBezierCurveObject.h"
#include "KPrCubicBezierCurveObjectAdaptor.h"
#include "KPrQuadricBezierCurveObjectAdaptor.h"
#include "KPrUtils.h"
#include <KoTextZoomHandler.h>
#include <qpainter.h>
#include <qmatrix.h>
#include <qdom.h>
#include "KoPointArray.h"
#include <kdebug.h>

#include <math.h>
using namespace std;

KPrBezierCurveObject::KPrBezierCurveObject()
: KPrPointObject()
{
}

KPrBezierCurveObject::KPrBezierCurveObject( const KoPointArray &_controlPoints,
                                            const KoPointArray &_allPoints,
                                            const KoSize &_size, const KoPen &_pen,
                                            LineEnd _lineBegin, LineEnd _lineEnd )
: KPrPointObject( _pen, _lineBegin, _lineEnd )
{
    points = KoPointArray( _controlPoints );
    allPoints = KoPointArray( _allPoints );

    ext = _size;
}


KPrBezierCurveObject &KPrBezierCurveObject::operator=( const KPrBezierCurveObject & )
{
    return *this;
}


bool KPrBezierCurveObject::saveOasisObjectAttributes( KPOasisSaveContext &sc ) const
{
    KoRect rect( getRect() );
    sc.xmlWriter.addAttribute("svg:viewBox", QString( "0 0 %1 %2" ).arg( int( rect.width() * 100 ) )
                                                                   .arg( int( rect.height() * 100 ) ) );

    unsigned int pointCount = points.count();
    unsigned int pos = 0;

    QString d;
    d += QString( "M%1 %2" ).arg( int( points.at(pos).x() * 100 ) )
                            .arg( int( points.at(pos).y() * 100 ) );

    while ( pos + 4 <= pointCount )
    {
        d += QString( "C%1 %2 %3 %4 %5 %6" ).arg( int( points.at( pos + 2 ).x() * 100 ) )
                                            .arg( int( points.at( pos + 2 ).y() * 100 ) )
                                            .arg( int( points.at( pos + 3 ).x() * 100 ) )
                                            .arg( int( points.at( pos + 3 ).y() * 100 ) )
                                            .arg( int( points.at( pos + 1 ).x() * 100 ) )
                                            .arg( int( points.at( pos + 1 ).y() * 100 ) );
        pos += 4;
    }

    if ( pos < pointCount )
    {
        d += QString( "L%1 %2" ).arg( int( points.at( pos + 1 ).x() * 100 ) )
                                .arg( int( points.at( pos + 1 ).y() * 100 ) );
    }

    sc.xmlWriter.addAttribute( "svg:d", d );

    return true;
}


const char * KPrBezierCurveObject::getOasisElementName() const
{
    return "draw:path";
}


void KPrBezierCurveObject::loadOasis( const QDomElement &element, KoOasisContext & context,  KPrLoadingInfo* info )
{
    kDebug(33001) << "KPrBezierCurveObject::loadOasis" << endl;
    KPrPointObject::loadOasis( element, context, info );

    allPoints = bezier2polyline( points );

    //load marker
    loadOasisMarker( context );
}

QDomDocumentFragment KPrBezierCurveObject::save( QDomDocument& doc, double offset )
{
    return KPrPointObject::save( doc,offset );
}

double KPrBezierCurveObject::load(const QDomElement &element)
{
    double offset = KPrPointObject::load( element );

    allPoints = bezier2polyline( points );

    return offset;
}

void KPrBezierCurveObject::updatePoints( double _fx, double _fy )
{
    KPrPointObject::updatePoints( _fx, _fy );

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

KoPointArray KPrBezierCurveObject::bezier2polyline( const KoPointArray &bezierPoints )
{
    if ( bezierPoints.isNull() )
        return bezierPoints;

    KoPointArray _points( bezierPoints );
    KoPointArray _allPoints;
    unsigned int pointCount = _points.count();

    if ( pointCount == 2 ) // line
    {
        _allPoints = _points;
    }
    else // cubic bezier curve
    {
        KoPointArray tmpPointArray;
        unsigned int _tmpIndex = 0;
        unsigned int count = 0;
        while ( count < pointCount )
        {
            if ( pointCount >= ( count + 4 ) ) // for cubic bezier curve
            {
                double _firstX = _points.at( count ).x();
                double _firstY = _points.at( count ).y();

                double _fourthX = _points.at( count + 1 ).x();
                double _fourthY = _points.at( count + 1 ).y();

                double _secondX = _points.at( count + 2 ).x();
                double _secondY = _points.at( count + 2 ).y();

                double _thirdX = _points.at( count + 3 ).x();
                double _thirdY = _points.at( count + 3 ).y();

                KoPointArray bezierPoint;
                bezierPoint.putPoints( 0, 4, _firstX,_firstY, _secondX,_secondY,
                                             _thirdX,_thirdY, _fourthX,_fourthY );
                bezierPoint = bezierPoint.cubicBezier();

                KoPointArray::ConstIterator it;
                for ( it = bezierPoint.begin(); it != bezierPoint.end(); ++it )
                {
                    KoPoint _point = (*it);
                    tmpPointArray.putPoints( _tmpIndex, 1, _point.x(), _point.y() );
                    ++_tmpIndex;
                }

                count += 4;
            }
            else // for line
            {
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

void KPrBezierCurveObject::flip(bool horizontal )
{
    KPrPointObject::flip( horizontal );

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


KoPointArray KPrBezierCurveObject::getDrawingPoints() const
{
  return allPoints;
}


KPrCubicBezierCurveObject::KPrCubicBezierCurveObject()
: KPrBezierCurveObject()
{
}


KPrCubicBezierCurveObject::KPrCubicBezierCurveObject( const KoPointArray &_controlPoints,
                                                      const KoPointArray &_allPoints,
                                                      const KoSize & _size, const KoPen &_pen,
                                                      LineEnd _lineBegin, LineEnd _lineEnd )
: KPrBezierCurveObject( _controlPoints, _allPoints, _size, _pen, _lineBegin, _lineEnd )
{
}


KPrObjectAdaptor* KPrCubicBezierCurveObject::dbusObject()
{
    if ( !dbus )
        dbus = new KPrCubicBezierCurveObjectAdaptor( this );
    return dbus;
}


KPrQuadricBezierCurveObject::KPrQuadricBezierCurveObject()
: KPrBezierCurveObject()
{
}


KPrQuadricBezierCurveObject::KPrQuadricBezierCurveObject( const KoPointArray &_controlPoints,
                                                          const KoPointArray &_allPoints,
                                                          const KoSize & _size, const KoPen &_pen,
                                                          LineEnd _lineBegin, LineEnd _lineEnd )
: KPrBezierCurveObject( _controlPoints, _allPoints, _size, _pen, _lineBegin, _lineEnd )
{
}


KPrObjectAdaptor* KPrQuadricBezierCurveObject::dbusObject()
{
    if ( !dbus )
        dbus = new KPrQuadricBezierCurveObjectAdaptor( this );
    return dbus;
}
