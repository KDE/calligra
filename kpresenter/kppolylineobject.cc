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
#include "KPPolyLineObjectIface.h"

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
    : KPShadowObject()
{
    lineBegin = L_NORMAL;
    lineEnd = L_NORMAL;
}

DCOPObject* KPPolylineObject::dcopObject()
{
    if ( !dcop )
	dcop = new KPPolyLineObjectIface( this );
    return dcop;
}


/*================== overloaded constructor ======================*/
KPPolylineObject::KPPolylineObject(  const KoPointArray &_points, const KoSize &_size,
				     const QPen &_pen, LineEnd _lineBegin, LineEnd _lineEnd )
    : KPShadowObject( _pen )
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
QDomDocumentFragment KPPolylineObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment = KPShadowObject::save( doc, offset );
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
double KPPolylineObject::load(const QDomElement &element)
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

/*======================== paint =================================*/
void KPPolylineObject::paint( QPainter* _painter,KoZoomHandler*_zoomHandler,
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

    if ( lineBegin != L_NORMAL && !drawContour &&!isClosed()) {
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
                drawFigure( lineBegin, _painter, _zoomHandler->unzoomPoint( startPoint ), pen2.color(), _w, angle,_zoomHandler );

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
                drawFigure( lineEnd, _painter, _zoomHandler->unzoomPoint( endPoint ), pen2.color(), _w, angle,_zoomHandler );

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


void KPPolylineObject::flip(bool horizontal )
{
    KoPointArray tmpPoints;
    int index = 0;
    if ( horizontal )
    {
	KoPointArray::ConstIterator it;
        double horiz = getSize().height()/2;
        for ( it = origPoints.begin(); it != origPoints.end(); ++it ) {
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
        for ( it = origPoints.begin(); it != origPoints.end(); ++it ) {
            KoPoint point = (*it);
            if ( point.x()> vert )
                tmpPoints.putPoints( index, 1, point.x()- 2*(point.x()-vert), point.y() );
            else
                tmpPoints.putPoints( index, 1, point.x()+ 2*(vert - point.x()),point.y() );
            ++index;
        }

    }
    origPoints = tmpPoints;
    updatePoints( 1.0, 1.0 );

}

void KPPolylineObject::closeObject(bool _close)
{
    origPoints=getCloseObject( origPoints, _close, isClosed() );
    updatePoints( 1.0, 1.0 );
}

bool KPPolylineObject::isClosed()const
{
    return (origPoints.at(0)==origPoints.at(origPoints.count()-1));
}
