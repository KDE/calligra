// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2004-2005 Thorsten Zachmann  <zachmann@kde.org>

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

#include "KPrPointObject.h"
#include "KPrUtils.h"
#include "KPrDocument.h"
#include "KPrSVGPathParser.h"
#include <KoTextZoomHandler.h>
#include <KoUnit.h>
#include <qdom.h>
#include <qpainter.h>
//Added by qt3to4:
#include <Q3PointArray>
#include <KoStyleStack.h>
#include <KoOasisContext.h>
#include <KoXmlNS.h>

KPrPointObject::KPrPointObject()
    : KPrShadowObject(), KPrStartEndLine( L_NORMAL, L_NORMAL )
{
}


KPrPointObject::KPrPointObject( const KoPen &_pen, LineEnd _lineBegin, LineEnd _lineEnd )
    : KPrShadowObject( _pen ), KPrStartEndLine(_lineBegin, _lineEnd)
{
}


KoSize KPrPointObject::getRealSize() const
{
    KoSize size( ext );
    KoPoint realOrig( orig );
    KoPointArray p( getDrawingPoints() );
    getRealSizeAndOrigFromPoints( p, angle, size, realOrig );
    return size;
}


KoPoint KPrPointObject::getRealOrig() const
{
    KoSize size( ext );
    KoPoint realOrig( orig );
    KoPointArray p( getDrawingPoints() );
    getRealSizeAndOrigFromPoints( p, angle, size, realOrig );
    return realOrig;
}


void KPrPointObject::loadOasis( const QDomElement &element, KoOasisContext & context,  KPrLoadingInfo* info )
{
    kDebug(33001) << "KPrPointObject::loadOasis" << endl;

    KPrShadowObject::loadOasis( element, context, info );
    QString d = element.attributeNS( KoXmlNS::svg, "d", QString::null);
    kDebug(33001) << "path d: " << d << endl;

    KPrSVGPathParser parser;
    points = parser.getPoints( d, getType() == OT_FREEHAND );
    loadOasisApplyViewBox( element, points );
}


QDomDocumentFragment KPrPointObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment = KPrShadowObject::save( doc, offset );
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

    KPrStartEndLine::save( fragment,doc );

    return fragment;
}

const char * KPrPointObject::getOasisElementName() const
{
    return "draw:custom-shape";
}

void KPrPointObject::loadOasisMarker( KoOasisContext & context )
{
    loadOasisMarkerElement( context, "marker-start", lineBegin );
    loadOasisMarkerElement( context, "marker-end", lineEnd );
}

void KPrPointObject::fillStyle( KoGenStyle& styleObjectAuto, KoGenStyles& mainStyles ) const
{
    KPrShadowObject::fillStyle( styleObjectAuto, mainStyles );
    saveOasisMarkerElement( mainStyles, styleObjectAuto );
}


double KPrPointObject::load( const QDomElement &element )
{
    double offset = KPrShadowObject::load( element );

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
    }
    KPrStartEndLine::load( element );
    return offset;
}


void KPrPointObject::setSize( double _width, double _height )
{
    KoSize origSize( ext );
    KPrObject::setSize( _width, _height );

    double fx = ext.width() / origSize.width();
    double fy = ext.height() / origSize.height();

    updatePoints( fx, fy );
}


void KPrPointObject::flip( bool horizontal )
{
    KPrObject::flip( horizontal );

    KoPointArray tmpPoints;
    int index = 0;
    if ( ! horizontal )
    {
        KoPointArray::ConstIterator it;
        double horiz = getSize().height()/2;
        for ( it = points.begin(); it != points.end(); ++it ) {
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
        for ( it = points.begin(); it != points.end(); ++it ) {
            KoPoint point = (*it);
            if ( point.x()> vert )
                tmpPoints.putPoints( index, 1, point.x()- 2*(point.x()-vert), point.y() );
            else
                tmpPoints.putPoints( index, 1, point.x()+ 2*(vert - point.x()),point.y() );
            ++index;
        }
    }

    points = tmpPoints;
}


void KPrPointObject::paint( QPainter* _painter, KoTextZoomHandler*_zoomHandler,
                           int /* pageNum */, bool /*drawingShadow*/, bool drawContour )
{
    int _w = int( pen.pointWidth() );

    QPen pen2;
    if ( drawContour ) {
        pen2 = QPen( Qt::black, 1, Qt::DotLine );
        _painter->setRasterOp( Qt::NotXorROP );
    }
    else {
        pen2 = pen.zoomedPen( _zoomHandler );
    }
    _painter->setPen( pen2 );

    Q3PointArray pointArray = getDrawingPoints().zoomPointArray( _zoomHandler, _w );
    _painter->drawPolyline( pointArray );

    if ( lineBegin != L_NORMAL && !drawContour ) {
        QPoint startPoint;
        bool first = true;
        Q3PointArray::ConstIterator it1;
        for ( it1 = pointArray.begin(); it1 != pointArray.end(); ++it1 ) {
            if ( first ) {
                startPoint = (*it1);
                first = false;
            }

            QPoint point = (*it1);
            if ( startPoint != point ) {
                float angle = KoPoint::getAngle( KoPoint( startPoint ), KoPoint( point ) );
                drawFigureWithOffset( lineBegin, _painter, startPoint, pen2.color(), _w, angle,_zoomHandler );

                break;
            }
        }
    }

    if ( lineEnd != L_NORMAL && !drawContour ) {
        QPoint endPoint;
        bool last = true;
        Q3PointArray::ConstIterator it2 = pointArray.end();
        for ( it2 = it2 - 1; it2 != pointArray.begin(); --it2 ) {
            if ( last ) {
                endPoint = (*it2);
                last = false;
            }

            QPoint point = (*it2);
            if ( endPoint != point ) {
                float angle = KoPoint::getAngle( KoPoint( endPoint ), KoPoint( point ) );
                drawFigureWithOffset( lineEnd, _painter, endPoint, pen2.color(), _w, angle,_zoomHandler );

                break;
            }
        }
    }
}


void KPrPointObject::updatePoints( double _fx, double _fy )
{
    int index = 0;
    KoPointArray tmpPoints;
    KoPointArray::ConstIterator it;
    for ( it = points.begin(); it != points.end(); ++it ) {
        KoPoint point = (*it);
        double tmpX = point.x() * _fx;
        double tmpY = point.y() * _fy;

        tmpPoints.putPoints( index, 1, tmpX,tmpY );
        ++index;
    }
    points = tmpPoints;
}


KoPointArray KPrPointObject::getDrawingPoints() const
{
  return points;
}
