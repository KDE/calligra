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
#include <kozoomhandler.h>
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
KPPolylineObject::KPPolylineObject(  const KoPointArray &_points, const KoSize &_size, const QPen &_pen,
                                     LineEnd _lineBegin, LineEnd _lineEnd )
    : KPObject(), pen( _pen )
{
    points = KoPointArray( _points );
    origPoints = KoPointArray( _points );
    origSize = _size;
    lineBegin = _lineBegin;
    lineEnd = _lineEnd;
}

KPPolylineObject &KPPolylineObject::operator=( const KPPolylineObject & )
{
    return *this;
}

/*========================= save =================================*/
QDomDocumentFragment KPPolylineObject::save( QDomDocument& doc, int offset )
{
    QDomDocumentFragment fragment = KPObject::save( doc, offset );
    fragment.appendChild( KPObject::createPenElement( "PEN", pen, doc ) );
    if ( !points.isNull() ) {
        QDomElement elemPoints = doc.createElement( "POINTS" );
	KoPointArray::ConstIterator it;
        for ( it = points.begin(); it != points.end(); ++it ) {
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
int KPPolylineObject::load(const QDomElement &element)
{
    int offset=KPObject::load( element );
    QDomElement e = element.namedItem( "PEN" ).toElement();
    if ( !e.isNull() )
        setPen( KPObject::toPen( e ) );

    e = element.namedItem( "POINTS" ).toElement();
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
    return offset;
}

/*========================= draw =================================*/
void KPPolylineObject::draw( QPainter *_painter, KoZoomHandler*_zoomHandler, bool drawSelection )
{
    double ox = orig.x();
    double oy = orig.y();
    double ow = ext.width();
    double oh = ext.height();

    _painter->save();

    if ( shadowDistance > 0 ) {
        QPen tmpPen( pen );
        pen.setColor( shadowColor );

        if ( angle == 0 ) {
            double sx = ox;
            double sy = oy;
            getShadowCoords( sx, sy, _zoomHandler );

            _painter->translate( _zoomHandler->zoomItX(sx), _zoomHandler->zoomItY(sy) );
            paint( _painter,_zoomHandler );
        }
        else {
            _painter->translate( _zoomHandler->zoomItX(ox), _zoomHandler->zoomItY(oy) );

            KoRect br = KoRect( 0, 0, ow, oh );
            double pw = br.width();
            double ph = br.height();
            KoRect rr = br;
            double yPos = -rr.y();
            double xPos = -rr.x();
            rr.moveTopLeft( KoPoint( -rr.width() / 2, -rr.height() / 2 ) );

            double sx = 0;
            double sy = 0;
            getShadowCoords( sx, sy,_zoomHandler );

            QWMatrix m;
            m.translate( pw / 2, ph / 2 );
            m.rotate( angle );
            m.translate( rr.left() + xPos + _zoomHandler->zoomItX(sx), rr.top() + yPos + _zoomHandler->zoomItY(sy) );

            _painter->setWorldMatrix( m, true );
            paint( _painter,_zoomHandler );
        }

        pen = tmpPen;
    }

    _painter->restore();

    _painter->save();
    _painter->translate( _zoomHandler->zoomItX(ox), _zoomHandler->zoomItY(oy) );

    if ( angle == 0 )
        paint( _painter,_zoomHandler );
    else {
        KoRect br = KoRect( 0, 0, ow, oh );
        double pw = br.width();
        double ph = br.height();
        KoRect rr = br;
        double yPos = -rr.y();
        double xPos = -rr.x();
        rr.moveTopLeft( KoPoint( -rr.width() / 2, -rr.height() / 2 ) );

        QWMatrix m;
        m.translate( pw / 2, ph / 2 );
        m.rotate( angle );
        m.translate( rr.left() + xPos, rr.top() + yPos );

        _painter->setWorldMatrix( m, true );
        paint( _painter,_zoomHandler );
    }

    _painter->restore();

    KPObject::draw( _painter, _zoomHandler, drawSelection );
}

/*===================== get angle ================================*/
float KPPolylineObject::getAngle( const QPoint &p1, const QPoint &p2 )
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
void KPPolylineObject::paint( QPainter* _painter,KoZoomHandler*_zoomHandler )
{
    int _w = _zoomHandler->zoomItX(pen.width());
    QPen pen2(pen);
    pen2.setWidth(_w);

    QPointArray pointArray = points.zoomPointArray( _zoomHandler, _w );

    _painter->setPen( pen2 );
    _painter->drawPolyline( pointArray );

    if ( lineBegin != L_NORMAL ) {
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
                float angle = getAngle( startPoint, point );
                drawFigure( lineBegin, _painter, startPoint, pen2.color(), _w, angle,_zoomHandler );

                break;
            }
        }
    }

    if ( lineEnd != L_NORMAL ) {
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
                float angle = getAngle( endPoint, point );
                drawFigure( lineEnd, _painter, endPoint, pen2.color(), _w, angle,_zoomHandler );

                break;
            }
        }
    }
}

void KPPolylineObject::setSize( double _width, double _height )
{
    KPObject::setSize( _width, _height );

    double fx = (double)( (double)ext.width() / (double)origSize.width() );
    double fy = (double)( (double)ext.height() / (double)origSize.height() );

    updatePoints( fx, fy );
}

void KPPolylineObject::resizeBy( const KoSize &_size )
{
    resizeBy( _size.width(), _size.height() );
}

void KPPolylineObject::resizeBy( double _dx, double _dy )
{
    KPObject::resizeBy( _dx, _dy );

    double fx = (double)( (double)ext.width() / (double)origSize.width() );
    double fy = (double)( (double)ext.height() / (double)origSize.height() );

    updatePoints( fx, fy );
}

void KPPolylineObject::updatePoints( double _fx, double _fy )
{
    int index = 0;
    KoPointArray tmpPoints;
    KoPointArray::ConstIterator it;
    for ( it = origPoints.begin(); it != origPoints.end(); ++it ) {
        KoPoint point = (*it);
        double tmpX = (int)( (double)point.x() * _fx );
        double tmpY = (int)( (double)point.y() * _fy );

        tmpPoints.putPoints( index, 1, tmpX,tmpY );
        ++index;
    }
    points = tmpPoints;
}


