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

#include <kppolylineobject.h>
#include <kpresenter_utils.h>

#include <qpainter.h>
#include <qwmatrix.h>
#include <qdom.h>

#include <kdebug.h>

#include <math.h>
using namespace std;

/******************************************************************/
/* Class: KPPolylineObject                                        */
/******************************************************************/

/*================ default constructor ===========================*/
KPPolylineObject::KPPolylineObject()
    : KPObject(), pen()
{
    lineBegin = L_NORMAL;
    lineEnd = L_NORMAL;
}

/*================== overloaded constructor ======================*/
KPPolylineObject::KPPolylineObject(  const QPointArray &_points, QSize _size, QPen _pen,
                                     LineEnd _lineBegin, LineEnd _lineEnd )
    : KPObject(), pen( _pen )
{
    points = QPointArray( _points );
    origPoints = QPointArray( _points );
    origSize = _size;
    lineBegin = _lineBegin;
    lineEnd = _lineEnd;
}

KPPolylineObject &KPPolylineObject::operator=( const KPPolylineObject & )
{
    return *this;
}

/*========================= save =================================*/
QDomDocumentFragment KPPolylineObject::save( QDomDocument& doc )
{
    QDomDocumentFragment fragment = KPObject::save( doc );
    fragment.appendChild( KPObject::createPenElement( "PEN", pen, doc ) );
    if ( !points.isNull() ) {
        QDomElement elemPoints = doc.createElement( "POINTS" );
	QPointArray::Iterator it;
        for ( it = points.begin(); it != points.end(); ++it ) {
            QDomElement elemPoint = doc.createElement( "Point" );
            QPoint point = (*it);
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
void KPPolylineObject::load(const QDomElement &element)
{
    KPObject::load( element );
    QDomElement e = element.namedItem( "PEN" ).toElement();
    if ( !e.isNull() )
        setPen( KPObject::toPen( e ) );

    e = element.namedItem( "POINTS" ).toElement();
    if ( !e.isNull() ) {
        QDomElement elemPoint = e.firstChild().toElement();
        unsigned int index = 0;
        while ( !elemPoint.isNull() ) {
            if ( elemPoint.tagName() == "Point" ) {
                int tmpX = 0;
                int tmpY = 0;
                if( elemPoint.hasAttribute( "point_x" ) )
                    tmpX = elemPoint.attribute( "point_x" ).toInt();
                if( elemPoint.hasAttribute( "point_y" ) )
                    tmpY = elemPoint.attribute( "point_y" ).toInt();

                points.putPoints( index, 1, tmpX,tmpY );
            }
            elemPoint = elemPoint.nextSibling().toElement();
            ++index;
        }
        origPoints = points;
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
}

/*========================= draw =================================*/
void KPPolylineObject::draw( QPainter *_painter, int _diffx, int _diffy )
{
    if ( move ) {
        KPObject::draw( _painter, _diffx, _diffy );
        return;
    }

    int ox = orig.x() - _diffx;
    int oy = orig.y() - _diffy;
    int ow = ext.width();
    int oh = ext.height();

    _painter->save();

    if ( shadowDistance > 0 ) {
        QPen tmpPen( pen );
        pen.setColor( shadowColor );

        if ( angle == 0 ) {
            int sx = ox;
            int sy = oy;
            getShadowCoords( sx, sy );

            _painter->translate( sx, sy );
            paint( _painter );
        }
        else {
            _painter->translate( ox, oy );

            QRect br = QRect( 0, 0, ow, oh );
            int pw = br.width();
            int ph = br.height();
            QRect rr = br;
            int yPos = -rr.y();
            int xPos = -rr.x();
            rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

            int sx = 0;
            int sy = 0;
            getShadowCoords( sx, sy );

            QWMatrix m;
            m.translate( pw / 2, ph / 2 );
            m.rotate( angle );
            m.translate( rr.left() + xPos + sx, rr.top() + yPos + sy );

            _painter->setWorldMatrix( m, true );
            paint( _painter );
        }

        pen = tmpPen;
    }

    _painter->restore();

    _painter->save();
    _painter->translate( ox, oy );

    if ( angle == 0 )
        paint( _painter );
    else {
        QRect br = QRect( 0, 0, ow, oh );
        int pw = br.width();
        int ph = br.height();
        QRect rr = br;
        int yPos = -rr.y();
        int xPos = -rr.x();
        rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

        QWMatrix m;
        m.translate( pw / 2, ph / 2 );
        m.rotate( angle );
        m.translate( rr.left() + xPos, rr.top() + yPos );

        _painter->setWorldMatrix( m, true );
        paint( _painter );
    }

    _painter->restore();

    KPObject::draw( _painter, _diffx, _diffy );
}

/*===================== get angle ================================*/
float KPPolylineObject::getAngle( QPoint p1, QPoint p2 )
{
    float _angle = 0.0;

    if ( p1.x() == p2.x() ) {
        if ( p1.y() < p2.y() )
            _angle = 270.0;
        else
            _angle = 90.0;
    }
    else {
        float x1, x2, y1, y2;

        if ( p1.x() <= p2.x() ) {
            x1 = p1.x(); y1 = p1.y();
            x2 = p2.x(); y2 = p2.y();
        }
        else {
            x2 = p1.x(); y2 = p1.y();
            x1 = p2.x(); y1 = p2.y();
        }

        float m = -( y2 - y1 ) / ( x2 - x1 );
        _angle = atan( m ) * RAD_FACTOR;

        if ( p1.x() < p2.x() )
            _angle = 180.0 - _angle;
        else
            _angle = -_angle;
    }

    return _angle;
}

/*======================== paint =================================*/
void KPPolylineObject::paint( QPainter* _painter )
{
    int _w = pen.width();

    QPointArray pointArray = points;
    if ( !move && _w > 1 ) {
        double fx = (double)( (double)( ext.width() - _w ) / (double)ext.width() );
        double fy = (double)( (double)( ext.height() - _w ) / (double)ext.height() );

        unsigned int index = 0;
        QPointArray tmpPoints;
        QPointArray::Iterator it;
        for ( it = points.begin(); it != points.end(); ++it ) {
            QPoint point = (*it);
            int tmpX = (int)( (double)point.x() * fx );
            int tmpY = (int)( (double)point.y() * fy );

            if ( tmpX == 0 )
                tmpX = _w;
            if ( tmpY == 0 )
                tmpY = _w;

            tmpPoints.putPoints( index, 1, tmpX,tmpY );
            ++index;
        }
        pointArray = tmpPoints;
    }

    _painter->setPen( pen );
    _painter->drawPolyline( pointArray );

    if ( lineBegin != L_NORMAL ) {
        QPoint startPoint;
        bool first = true;
        QPointArray::Iterator it1;
        for ( it1 = pointArray.begin(); it1 != pointArray.end(); ++it1 ) {
            if ( first ) {
                startPoint = (*it1);
                first = false;
            }

            QPoint point = (*it1);
            if ( startPoint != point ) {
                float angle = getAngle( startPoint, point );
                drawFigure( lineBegin, _painter, startPoint, pen.color(), _w, angle );

                break;
            }
        }
    }

    if ( lineEnd != L_NORMAL ) {
        QPoint endPoint;
        bool last = true;
        QPointArray::Iterator it2 = pointArray.end();
        for ( it2 = it2 - 1; it2 != pointArray.begin(); --it2 ) {
            if ( last ) {
                endPoint = (*it2);
                last = false;
            }

            QPoint point = (*it2);
            if ( endPoint != point ) {
                float angle = getAngle( endPoint, point );
                drawFigure( lineEnd, _painter, endPoint, pen.color(), _w, angle );

                break;
            }
        }
    }
}

void KPPolylineObject::setSize( int _width, int _height )
{
    KPObject::setSize( _width, _height );

    double fx = (double)( (double)ext.width() / (double)origSize.width() );
    double fy = (double)( (double)ext.height() / (double)origSize.height() );

    updatePoints( fx, fy );
}

void KPPolylineObject::resizeBy( QSize _size )
{
    resizeBy( _size.width(), _size.height() );
}

void KPPolylineObject::resizeBy( int _dx, int _dy )
{
    KPObject::resizeBy( _dx, _dy );

    double fx = (double)( (double)ext.width() / (double)origSize.width() );
    double fy = (double)( (double)ext.height() / (double)origSize.height() );

    updatePoints( fx, fy );
}

void KPPolylineObject::updatePoints( double _fx, double _fy )
{
    int index = 0;
    QPointArray tmpPoints;
    QPointArray::Iterator it;
    for ( it = origPoints.begin(); it != origPoints.end(); ++it ) {
        QPoint point = (*it);
        int tmpX = (int)( (double)point.x() * _fx );
        int tmpY = (int)( (double)point.y() * _fy );

        tmpPoints.putPoints( index, 1, tmpX,tmpY );
        ++index;
    }
    points = tmpPoints;
}


