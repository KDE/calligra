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

#include "kpcubicbeziercurveobject.h"
#include "KPCubicbeziercurveObjectIface.h"
#include <kpresenter_utils.h>
#include <kozoomhandler.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <qdom.h>
#include "koPointArray.h"
#include <kdebug.h>

#include <math.h>
using namespace std;

/******************************************************************/
/* Class: KPCubicBezierCurveObject                                */
/******************************************************************/

/*================ default constructor ===========================*/
KPCubicBezierCurveObject::KPCubicBezierCurveObject()
    : KPShadowObject()
{
    lineBegin = L_NORMAL;
    lineEnd = L_NORMAL;
}


/*================== overloaded constructor ======================*/
KPCubicBezierCurveObject::KPCubicBezierCurveObject( const KoPointArray &_controlPoints,
						    const KoPointArray &_allPoints,
                                                    const KoSize &_size, const QPen &_pen,
						    LineEnd _lineBegin, LineEnd _lineEnd )
    : KPShadowObject( _pen )
{
    controlPoints = KoPointArray( _controlPoints );
    origControlPoints = KoPointArray( _controlPoints );

    allPoints = KoPointArray( _allPoints );
    origAllPoints = KoPointArray( _allPoints );

    origSize = _size;
    lineBegin = _lineBegin;
    lineEnd = _lineEnd;
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


/*========================= save =================================*/
QDomDocumentFragment KPCubicBezierCurveObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment = KPShadowObject::save( doc,offset );
    if ( !controlPoints.isNull() ) {
        QDomElement elemPoints = doc.createElement( "POINTS" );
	KoPointArray::ConstIterator it;
        for ( it = controlPoints.begin(); it != controlPoints.end(); ++it ) {
            QDomElement elemPoint = doc.createElement( "Point" );
            KoPoint point = (*it);
            elemPoint.setAttribute( "point_x", point.x() );
            elemPoint.setAttribute( "point_y", point.y() );

            elemPoints.appendChild( elemPoint );
        }
        fragment.appendChild( elemPoints );
    }

    if ( lineBegin != L_NORMAL )
        fragment.appendChild( KPObject::createValueElement( "LINEBEGIN", static_cast<int>( lineBegin ), doc ) );

    if ( lineEnd != L_NORMAL )
        fragment.appendChild( KPObject::createValueElement( "LINEEND", static_cast<int>( lineEnd ), doc ) );

    return fragment;
}

/*========================== load ================================*/
double KPCubicBezierCurveObject::load(const QDomElement &element)
{
    double offset=KPShadowObject::load( element );
    QDomElement e = element.namedItem( "POINTS" ).toElement();
    if ( !e.isNull() ) {
        QDomElement elemPoint = e.firstChild().toElement();
        unsigned int index = 0;
        while ( !elemPoint.isNull() ) {
            if ( elemPoint.tagName() == "Point" ) {
                double tmpX = 0;
                double tmpY = 0;
                if( elemPoint.hasAttribute( "point_x" ) )
                    tmpX = elemPoint.attribute( "point_x" ).toDouble();
                if( elemPoint.hasAttribute( "point_y" ) )
                    tmpY = elemPoint.attribute( "point_y" ).toDouble();

                controlPoints.putPoints( index, 1, tmpX,tmpY );
            }
            elemPoint = elemPoint.nextSibling().toElement();
            ++index;
        }
        origControlPoints = controlPoints;
        allPoints = getCubicBezierPointsFrom( controlPoints );
        origAllPoints = allPoints;
        origSize = ext;
    }

    e = element.namedItem( "LINEBEGIN" ).toElement();
    if( !e.isNull() ) {
        int tmp = 0;
        if( e.hasAttribute( "value" ) )
            tmp = e.attribute( "value" ).toInt();
        lineBegin = static_cast<LineEnd>( tmp );
    }

    e = element.namedItem( "LINEEND" ).toElement();
    if( !e.isNull() ) {
        int tmp = 0;
        if( e.hasAttribute( "value" ) )
            tmp = e.attribute( "value" ).toInt();
        lineEnd = static_cast<LineEnd>( tmp );
    }
    return offset;
}

/*======================== paint =================================*/
void KPCubicBezierCurveObject::paint( QPainter* _painter, KoZoomHandler*_zoomHandler,
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
                drawFigure( lineBegin, _painter, _zoomHandler->unzoomPoint( startPoint ), pen.color(), _w, angle,_zoomHandler );

                break;
            }
        }
    }

    if ( lineEnd != L_NORMAL && !drawContour &&!isClosed()) {
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
                drawFigure( lineEnd, _painter, _zoomHandler->unzoomPoint( endPoint ), pen.color(), _w, angle, _zoomHandler);

                break;
            }
        }
    }
}

void KPCubicBezierCurveObject::setSize( double _width, double _height )
{
    KPObject::setSize( _width, _height );

    double fx = ext.width() / origSize.width();
    double fy = ext.height() / origSize.height();

    updatePoints( fx, fy );
}

void KPCubicBezierCurveObject::resizeBy( const KoSize &_size )
{
    resizeBy( _size.width(), _size.height() );
}

void KPCubicBezierCurveObject::resizeBy( double _dx, double _dy )
{
    KPObject::resizeBy( _dx, _dy );

    double fx = ext.width() / origSize.width();
    double fy = ext.height() / origSize.height();

    updatePoints( fx, fy );
}

void KPCubicBezierCurveObject::updatePoints( double _fx, double _fy )
{
    int index = 0;
    KoPointArray tmpPoints;
    KoPointArray::ConstIterator it;
    for ( it = origAllPoints.begin(); it != origAllPoints.end(); ++it ) {
        KoPoint point = (*it);
        double tmpX = point.x() * _fx;
        double tmpY = point.y() * _fy;

        tmpPoints.putPoints( index, 1, tmpX,tmpY );
        ++index;
    }
    allPoints = tmpPoints;

    index = 0;
    tmpPoints = KoPointArray();
    for ( it = origControlPoints.begin(); it != origControlPoints.end(); ++it ) {
        KoPoint point = (*it);
        double tmpX = point.x() * _fx;
        double tmpY = point.y() * _fy;

        tmpPoints.putPoints( index, 1, tmpX,tmpY );
        ++index;
    }
    controlPoints = tmpPoints;
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
                _cubicBezierPoint.putPoints( 0, 4, _firstX,_firstY, _secondX,_secondY, _thirdX,_thirdY, _fourthX,_fourthY );
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
    KoPointArray tmpPoints;
    int index = 0;
    if ( horizontal )
    {
	KoPointArray::ConstIterator it;
        double horiz = getSize().height()/2;
        for ( it = origControlPoints.begin(); it != origControlPoints.end(); ++it )
        {
            KoPoint point = (*it);
            if ( point.y()> horiz )
                tmpPoints.putPoints( index, 1, point.x(),point.y()- 2*(point.y()-horiz) );
            else
                tmpPoints.putPoints( index, 1, point.x(),point.y()+ 2*(horiz - point.y()) );
            ++index;
        }
        origControlPoints = tmpPoints;

        index=0;
        for ( it = origAllPoints.begin(); it != origAllPoints.end(); ++it )
        {
            KoPoint point = (*it);
            if ( point.y()> horiz )
                tmpPoints.putPoints( index, 1, point.x(),point.y()- 2*(point.y()-horiz) );
            else
                tmpPoints.putPoints( index, 1, point.x(),point.y()+ 2*(horiz - point.y()) );
            ++index;
        }
        origAllPoints = tmpPoints;

    }
    else
    {
        KoPointArray::ConstIterator it;
        double vert = getSize().width()/2;
        for ( it = origControlPoints.begin(); it != origControlPoints.end(); ++it )
        {
            KoPoint point = (*it);
            if ( point.x()> vert )
                tmpPoints.putPoints( index, 1, point.x()- 2*(point.x()-vert), point.y() );
            else
                tmpPoints.putPoints( index, 1, point.x()+ 2*(vert - point.x()),point.y() );
            ++index;
        }
        origControlPoints = tmpPoints;

        index = 0;
        for ( it = origAllPoints.begin(); it != origAllPoints.end(); ++it )
        {
            KoPoint point = (*it);
            if ( point.y()> vert )
                tmpPoints.putPoints( index, 1, point.x()- 2*(point.x()-vert), point.y() );
            else
                tmpPoints.putPoints( index, 1, point.x()+ 2*(vert - point.x()),point.y() );
            ++index;
        }
        origAllPoints = tmpPoints;

    }
    updatePoints( 1.0, 1.0 );
}

void KPCubicBezierCurveObject::closeObject(bool _close)
{
    origAllPoints=getCloseObject( origAllPoints, _close, isClosed() );
    updatePoints( 1.0, 1.0 );
}

bool KPCubicBezierCurveObject::isClosed()const
{
    return (origAllPoints.at(0)==origAllPoints.at(origAllPoints.count()-1));
}
