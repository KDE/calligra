// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002 Toshitaka Fujioka <fujioka@kde.org>
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

#include "KPrClosedLineObject.h"
#include "KPrPointObject.h"
#include "KPrGradient.h"
#include "KPrSVGPathParser.h"
#include <KoTextZoomHandler.h>
#include <kdebug.h>
#include <qbitmap.h>
#include <qregion.h>
#include <QRegExp>
#include <qdom.h>
#include <q3picture.h>
#include <qpainter.h>
//Added by qt3to4:
#include <Q3PointArray>
#include <KoOasisContext.h>
#include <math.h>
#include <KoUnit.h>
#include <KoXmlNS.h>
#include <KoDom.h>

using namespace std;

KPrClosedLineObject::KPrClosedLineObject()
    : KPr2DObject()
{
}

KPrClosedLineObject::KPrClosedLineObject( const KoPointArray &_points, const KoSize &_size, const KoPen &_pen, const QBrush &_brush,
                                        FillType _fillType, const QColor &_gColor1, const QColor &_gColor2, BCType _gType,
                                        bool _unbalanced, int _xfactor, int _yfactor, const QString _typeString )
    : KPr2DObject( _pen, _brush, _fillType, _gColor1, _gColor2, _gType, _unbalanced, _xfactor, _yfactor )
{
    points = KoPointArray( _points );
    ext = _size;
    typeString = _typeString;
}

KPrClosedLineObject::KPrClosedLineObject( const KPrPointObject &object )
: KPr2DObject( object.getPen(), Qt::NoBrush, FT_BRUSH, QColor(), QColor(), BCT_PLAIN, false, 0, 0 )
{
    ext = object.getSize();
    orig = object.getOrig();
    objectName = object.getObjectName();
    points = object.getPoints().copy();
    points.putPoints( points.count(), 1, points.at( 0 ).x(), points.at( 0 ).y() );
    switch ( object.getType() )
    {
        case OT_FREEHAND:
            typeString = i18n( "Closed Freehand" );
            break;
        case OT_POLYLINE:
            typeString = i18n( "Closed Polyline" );
            break;
        case OT_CUBICBEZIERCURVE:
            typeString = i18n( "Closed Cubic Bezier Curve" );
            break;
        case OT_QUADRICBEZIERCURVE:
            typeString = i18n( "Closed Quadric Bezier Curve" );
            break;
        default:    
            break;
    }
            
}

KPrClosedLineObject &KPrClosedLineObject::operator=( const KPrClosedLineObject & )
{
    return *this;
}

#if 0
DCOPObject* KPrClosedLineObject::dcopObject()
{
    if ( !dcop )
        dcop = new KPClosedLineObjectIface( this );
    return dcop;
}
#endif

QDomDocumentFragment KPrClosedLineObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment = KPr2DObject::save( doc, offset );

    QDomElement elemObjectsName = doc.createElement( "OBJECTSNAME" );

    elemObjectsName.setAttribute( "NAME", typeString );

    fragment.appendChild( elemObjectsName );

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

    return fragment;
}

bool KPrClosedLineObject::saveOasisObjectAttributes( KPOasisSaveContext &sc ) const
{
    KPrShadowObject::saveOasisDrawPoints( points, sc );
    return true;
}

const char * KPrClosedLineObject::getOasisElementName() const
{
    return "draw:polygon";
}


double KPrClosedLineObject::load( const QDomElement &element )
{
    double offset = KPr2DObject::load( element );

    QDomElement e = element.namedItem( "OBJECTSNAME" ).toElement();
    if ( !e.isNull() ) {
        if ( e.hasAttribute( "NAME" ) )
            typeString = e.attribute( "NAME" );
    }

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
    }
    return offset;
}

void KPrClosedLineObject::setSize( double _width, double _height )
{
    KoSize origSize( ext );
    KPrObject::setSize( _width, _height );

    double fx = ext.width() / origSize.width();
    double fy = ext.height() / origSize.height();

    updatePoints( fx, fy );
}

void KPrClosedLineObject::updatePoints( double _fx, double _fy )
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

void KPrClosedLineObject::paint( QPainter* _painter,KoTextZoomHandler*_zoomHandler,
                                int /* pageNum */, bool drawingShadow, bool drawContour )
{
    int _w = ( pen.style() == Qt::NoPen ) ? 1 : int( pen.pointWidth() );

    if ( drawContour ) {
        Q3PointArray pointArray2 = points.zoomPointArray( _zoomHandler );
        QPen pen3( Qt::black, 1, Qt::DotLine );
        _painter->setPen( pen3 );
#warning "kde4: port it"		
        //_painter->setRasterOp( Qt::NotXorROP );
        _painter->drawPolygon( pointArray2 );
        return;
    }

    Q3PointArray pointArray = points.zoomPointArray( _zoomHandler, _w );
    QPen pen2 = pen.zoomedPen( _zoomHandler );

    if ( drawingShadow || getFillType() == FT_BRUSH || !gradient ) {
        _painter->setPen( pen2 );
        _painter->setBrush( getBrush() );
        _painter->drawPolygon( pointArray );
    }
    else {
        QSize size( _zoomHandler->zoomSizeOld( ext ) );
        if ( m_redrawGradientPix || gradient->size() != size )
        {
            m_redrawGradientPix = false;
            gradient->setSize( size );
            QRegion clipregion( pointArray );
            m_gradientPix.resize( size );
            m_gradientPix.fill( Qt::white );

            QPainter p;
            p.begin( &m_gradientPix );
            p.setClipRegion( clipregion );
            p.drawPixmap( 0, 0, gradient->pixmap() );
            p.end();

            m_gradientPix.setMask( m_gradientPix.createHeuristicMask() );
        }

        QRect _rect = pointArray.boundingRect();
        _painter->drawPixmap( 0, 0, m_gradientPix, 0, 0, _rect.width(), _rect.height() );

        _painter->setPen( pen2 );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawPolygon( pointArray );
    }
}

void KPrClosedLineObject::flip( bool horizontal )
{
    KPr2DObject::flip( horizontal );
    // flip the points
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

void KPrClosedLineObject::loadOasis( const QDomElement &element, KoOasisContext & context, KPrLoadingInfo *info )
{
    kDebug()<<"void KPrClosedLineObject::loadOasis( const QDomElement &element )***********\n";
    KPr2DObject::loadOasis( element,context, info );
    QString tag( element.tagName() );
    if ( tag == "polygon" )
    {
        KPrShadowObject::loadOasisDrawPoints( points, element, context, info );
    }
    else if ( tag == "path" ) // this is used to load closed draw:path objects
    {
        QString d = element.attributeNS( KoXmlNS::svg, "d", QString::null);
        kDebug(33001) << "path d: " << d << endl;

        KPrSVGPathParser parser;
        points = parser.getPoints( d, true );
        loadOasisApplyViewBox( element, points );
    }
    else if ( tag == "custom-shape" )
    {
        QDomElement enhancedGeometry = KoDom::namedItemNS( element, KoXmlNS::draw, "enhanced-geometry" );

        if ( !enhancedGeometry.isNull() )
        {
            QString d = enhancedGeometry.attributeNS( KoXmlNS::draw, "enhanced-path", QString::null );
            QRegExp rx( "^([0-9 MLZ]+)N$" );
            if ( rx.search( d ) != -1 )
            {
                d = rx.cap( 1 );
                kDebug(33001) << "enhanced-path d: " << d << endl;

                KPrSVGPathParser parser;
                points = parser.getPoints( d, true );
                loadOasisApplyViewBox( enhancedGeometry, points );
            }
        }
    }
    else
    {
        kDebug(33001) << "KPrClosedLineObject::loadOasis unsupported tag" << endl;
    }
}

KoSize KPrClosedLineObject::getRealSize() const {
    KoSize size( ext );
    KoPoint realOrig( orig );
    KoPointArray p( points );
    getRealSizeAndOrigFromPoints( p, angle, size, realOrig );
    return size;
}

KoPoint KPrClosedLineObject::getRealOrig() const {
    KoSize size( ext );
    KoPoint realOrig( orig );
    KoPointArray p( points );
    getRealSizeAndOrigFromPoints( p, angle, size, realOrig );
    return realOrig;
}
