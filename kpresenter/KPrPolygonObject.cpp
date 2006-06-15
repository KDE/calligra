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

#include "KPrPolygonObject.h"
#include "KPrGradient.h"
#include <KoTextZoomHandler.h>
#include <kdebug.h>
#include <qbitmap.h>
#include <qregion.h>
#include <qdom.h>
#include <q3picture.h>
#include <qpainter.h>
//Added by qt3to4:
#include <Q3PointArray>
#include "KPrPolygonObjectAdaptor.h"
#include <KoUnit.h>
#include <KoOasisContext.h>
#include <math.h>
#include <KoXmlNS.h>
#include "KoPen.h"

using namespace std;

KPrPolygonObject::KPrPolygonObject()
    : KPr2DObject()
{
}

KPrPolygonObject::KPrPolygonObject( const KoPointArray &_points, const KoSize &_size,
                                  const KoPen &_pen, const QBrush &_brush,
                                  FillType _fillType, const QColor &_gColor1, const QColor &_gColor2, BCType _gType,
                                  bool _unbalanced, int _xfactor, int _yfactor,
                                  bool _checkConcavePolygon, int _cornersValue, int _sharpnessValue )
    : KPr2DObject( _pen, _brush, _fillType, _gColor1, _gColor2, _gType, _unbalanced, _xfactor, _yfactor )
{
    points = KoPointArray( _points );
    ext = _size;

    checkConcavePolygon = _checkConcavePolygon;
    cornersValue = _cornersValue;
    sharpnessValue = _sharpnessValue;
}

KPrPolygonObject &KPrPolygonObject::operator=( const KPrPolygonObject & )
{
    return *this;
}

KPrObjectAdaptor* KPrPolygonObject::dbusObject()
{
    if ( !dbus )
        dbus = new KPrPolygonObjectAdaptor( this );
    return dbus;
}

bool KPrPolygonObject::saveOasisObjectAttributes( KPOasisSaveContext &sc ) const
{
    sc.xmlWriter.addAttribute( "draw:corners", cornersValue );
    sc.xmlWriter.addAttribute( "draw:concave", checkConcavePolygon ? "true" : "false" );
    if ( checkConcavePolygon )
    {
        sc.xmlWriter.addAttribute( "draw:sharpness", QString( "%1%").arg( sharpnessValue ) );
    }

    return true;
}

const char * KPrPolygonObject::getOasisElementName() const
{
    return "draw:regular-polygon";
}


QDomDocumentFragment KPrPolygonObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment = KPr2DObject::save( doc, offset );

    QDomElement elemSettings = doc.createElement( "SETTINGS" );

    elemSettings.setAttribute( "checkConcavePolygon", static_cast<int>( checkConcavePolygon ) );
    elemSettings.setAttribute( "cornersValue", cornersValue );
    elemSettings.setAttribute( "sharpnessValue", sharpnessValue );

    fragment.appendChild( elemSettings );

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

void KPrPolygonObject::loadOasis( const QDomElement &element, KoOasisContext & context, KPrLoadingInfo *info )
{
    kDebug()<<"void KPrPolygonObject::loadOasis( const QDomElement &element )***********\n";
    KPr2DObject::loadOasis( element,context, info );
    cornersValue = element.attributeNS( KoXmlNS::draw, "corners", QString::null ).toInt();
    checkConcavePolygon = element.attributeNS( KoXmlNS::draw, "concave", QString::null ) == "true";
    sharpnessValue = 0;
    if ( checkConcavePolygon )
    {
        sharpnessValue = element.attributeNS( KoXmlNS::draw, "sharpness", QString::null ).remove( '%').toInt();
    }
    drawPolygon();
}

double KPrPolygonObject::load( const QDomElement &element )
{
    double offset=KPr2DObject::load( element );

    QDomElement e = element.namedItem( "SETTINGS" ).toElement();
    if ( !e.isNull() ) {
        bool _checkConcavePolygon = false;
        int _cornersValue = 3;
        int _sharpnessValue = 0;

        if ( e.hasAttribute( "checkConcavePolygon" ) )
            _checkConcavePolygon = static_cast<bool>( e.attribute( "checkConcavePolygon" ).toInt() );
        if ( e.hasAttribute( "cornersValue" ) )
            _cornersValue = e.attribute( "cornersValue" ).toInt();
        if ( e.hasAttribute( "sharpnessValue" ) )
            _sharpnessValue = e.attribute( "sharpnessValue" ).toInt();

        checkConcavePolygon = _checkConcavePolygon;
        cornersValue = _cornersValue;
        sharpnessValue = _sharpnessValue;
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

void KPrPolygonObject::setSize( double _width, double _height )
{
    KoSize origSize( ext );
    KPrObject::setSize( _width, _height );

    double fx = ext.width() / origSize.width();
    double fy = ext.height() / origSize.height();

    updatePoints( fx, fy );
}

void KPrPolygonObject::updatePoints( double _fx, double _fy )
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

void KPrPolygonObject::paint( QPainter* _painter,KoTextZoomHandler*_zoomHandler,
                             int /* pageNum */, bool drawingShadow, bool drawContour )
{
    int _w = ( pen.style() == Qt::NoPen ) ? 1 : pen.width();//pen.width();

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

void KPrPolygonObject::drawPolygon()
{
    kDebug()<<"void KPrPolygonObject::drawPolygon()***********\n";
    KoRect _rect( 0, 0, ext.width(), ext.height() );
    double angle = 2 * M_PI / cornersValue;
    double diameter = static_cast<double>( qMax( _rect.width(), _rect.height() ) );
    double radius = diameter * 0.5;

    KoPointArray _points( checkConcavePolygon ? cornersValue * 2 : cornersValue );
    _points.setPoint( 0, 0, qRound( -radius ) );

    double xmin = 0;
    double ymin = qRound( -radius );

    if ( checkConcavePolygon ) {
        angle = angle / 2.0;
        double a = angle;
        double r = radius - ( sharpnessValue / 100.0 * radius );
        for ( int i = 1; i < cornersValue * 2; ++i ) {
            double xp, yp;
            if ( i % 2 ) {
                xp =  r * sin( a );
                yp = -r * cos( a );
            }
            else {
                xp = radius * sin( a );
                yp = -radius * cos( a );
            }
            a += angle;
            _points.setPoint( i, xp, yp );
            if (xp < xmin)
                xmin = xp;
            if (yp < ymin)
                ymin = yp;
        }
    }
    else {
        double a = angle;
        for ( int i = 1; i < cornersValue; ++i ) {
            double xp = radius * sin( a );
            double yp = -radius * cos( a );
            a += angle;
            _points.setPoint( i, xp, yp );
            if (xp < xmin)
                xmin = xp;
            if (yp < ymin)
                ymin = yp;
        }
    }

    // calculate the points as offsets to 0,0
    KoRect _changRect = _points.boundingRect();
    double fx = _rect.width() / _changRect.width();
    double fy = _rect.height() / _changRect.height();

    int _index = 0;
    KoPointArray tmpPoints;
    KoPointArray::ConstIterator it;
    for ( it = _points.begin(); it != _points.end(); ++it ) {
        KoPoint point = (*it);
        double tmpX = ( point.x() - xmin) * fx;
        double tmpY = ( point.y() - ymin) * fy;

        tmpPoints.putPoints( _index, 1, tmpX,tmpY );
        ++_index;
    }

    points = tmpPoints;

    if ( getFillType() == FT_GRADIENT && gradient )
        m_redrawGradientPix = true;
}

void KPrPolygonObject::flip( bool horizontal )
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

KoSize KPrPolygonObject::getRealSize() const {
    KoSize size( ext );
    KoPoint realOrig( orig );
    KoPointArray p( points );
    getRealSizeAndOrigFromPoints( p, angle, size, realOrig );
    return size;
}

KoPoint KPrPolygonObject::getRealOrig() const {
    KoSize size( ext );
    KoPoint realOrig( orig );
    KoPointArray p( points );
    getRealSizeAndOrigFromPoints( p, angle, size, realOrig );
    return realOrig;
}
