// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002 Toshitaka Fujioka <fujioka@kde.org>

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

#include "kpclosedlineobject.h"
#include "kpgradient.h"
#include <kozoomhandler.h>
#include <kdebug.h>
#include <qbitmap.h>
#include <qregion.h>
#include <qdom.h>
#include <qpicture.h>
#include <qpainter.h>
#include <kooasiscontext.h>
#include <math.h>
#include <koUnit.h>
using namespace std;

KPClosedLineObject::KPClosedLineObject()
    : KP2DObject()
{
    redrawPix = false;
}

KPClosedLineObject::KPClosedLineObject( const KoPointArray &_points, const KoSize &_size, const QPen &_pen, const QBrush &_brush,
                                        FillType _fillType, const QColor &_gColor1, const QColor &_gColor2, BCType _gType,
                                        bool _unbalanced, int _xfactor, int _yfactor, const QString _typeString )
    : KP2DObject( _pen, _brush, _fillType, _gColor1, _gColor2, _gType, _unbalanced, _xfactor, _yfactor )
{
    points = KoPointArray( _points );
    ext = _size;
    typeString = _typeString;

    redrawPix = false;

    if ( fillType == FT_GRADIENT ) {
        gradient = new KPGradient( gColor1, gColor2, gType, unbalanced, xfactor, yfactor );
        redrawPix = true;
    }
    else
        gradient = 0;
}

KPClosedLineObject &KPClosedLineObject::operator=( const KPClosedLineObject & )
{
    return *this;
}

#if 0
DCOPObject* KPClosedLineObject::dcopObject()
{
    if ( !dcop )
        dcop = new KPClosedLineObjectIface( this );
    return dcop;
}
#endif

QDomDocumentFragment KPClosedLineObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment = KP2DObject::save( doc, offset );

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

bool KPClosedLineObject::saveOasis( KoXmlWriter &xmlWriter, KoSavingContext& context, int indexObj )
{
    xmlWriter.startElement( "draw:polygon" );
    xmlWriter.addAttribute( "draw:style-name", KP2DObject::saveOasisBackgroundStyle( xmlWriter, context.mainStyles(), indexObj ) );

    QString listOfPoint;
    int maxX=0;
    int maxY=0;
    KoPointArray::ConstIterator it;
    for ( it = points.begin(); it != points.end(); ++it ) {
        int tmpX = 0;
        int tmpY = 0;
        tmpX = ( int ) ( KoUnit::toMM( ( *it ).x() )*100 );
        tmpY = ( int ) ( KoUnit::toMM( ( *it ).y() )*100 );
        if ( !listOfPoint.isEmpty() )
            listOfPoint += QString( " %1,%2" ).arg( tmpX ).arg( tmpY );
        else
            listOfPoint = QString( "%1,%2" ).arg( tmpX ).arg( tmpY );
        maxX = QMAX( maxX, tmpX );
        maxY = QMAX( maxY, tmpY );
    }
    xmlWriter.addAttribute("draw:points", listOfPoint );
    xmlWriter.addAttribute("svg:viewBox", QString( "0 0 %1 %2" ).arg( maxX ).arg( maxY ) );

    if( !objectName.isEmpty())
        xmlWriter.addAttribute( "draw:name", objectName );
    xmlWriter.endElement();
    return true;
}

double KPClosedLineObject::load( const QDomElement &element )
{
    double offset = KP2DObject::load( element );

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

void KPClosedLineObject::setSize( double _width, double _height )
{
    KoSize origSize( ext );
    KPObject::setSize( _width, _height );

    double fx = ext.width() / origSize.width();
    double fy = ext.height() / origSize.height();

    updatePoints( fx, fy );
}

void KPClosedLineObject::updatePoints( double _fx, double _fy )
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

void KPClosedLineObject::setFillType( FillType _fillType )
{
    fillType = _fillType;

    if ( fillType == FT_BRUSH && gradient ) {
        delete gradient;
        gradient = 0;
    }

    if ( fillType == FT_GRADIENT && !gradient ) {
        gradient = new KPGradient( gColor1, gColor2, gType, unbalanced, xfactor, yfactor );
        redrawPix = true;
    }
}

void KPClosedLineObject::paint( QPainter* _painter,KoZoomHandler*_zoomHandler,
                                bool drawingShadow, bool drawContour )
{
    int _w = ( pen.style() == Qt::NoPen ) ? 1 : pen.width();

    if ( drawContour ) {
        QPointArray pointArray2 = points.zoomPointArray( _zoomHandler );
        QPen pen3( Qt::black, 1, Qt::DotLine );
        _painter->setPen( pen3 );
        _painter->setRasterOp( Qt::NotXorROP );
        _painter->drawPolygon( pointArray2 );
        return;
    }

    QPointArray pointArray = points.zoomPointArray( _zoomHandler, _w );
    QPen pen2( pen );
    pen2.setWidth( _zoomHandler->zoomItX( pen.width() ) );

    if ( drawingShadow || fillType == FT_BRUSH || !gradient ) {
        _painter->setPen( pen2 );
        _painter->setBrush( brush );
        _painter->drawPolygon( pointArray );
    }
    else {
        QSize size( _zoomHandler->zoomSize( ext ) );
        if ( redrawPix || gradient->size() != size )
        {
            redrawPix = false;
            gradient->setSize( size );
            QRegion clipregion( pointArray );
            pix.resize( size );
            pix.fill( Qt::white );

            QPainter p;
            p.begin( &pix );
            p.setClipRegion( clipregion );
            p.drawPixmap( 0, 0, gradient->pixmap() );
            p.end();

            pix.setMask( pix.createHeuristicMask() );
        }

        QRect _rect = pointArray.boundingRect();
        _painter->drawPixmap( 0, 0, pix, 0, 0, _rect.width(), _rect.height() );

        _painter->setPen( pen2 );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawPolygon( pointArray );
    }
}

void KPClosedLineObject::flip( bool horizontal )
{
    KP2DObject::flip( horizontal );
    // flip the points
    KoPointArray tmpPoints;
    int index = 0;
    if ( horizontal )
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

    if ( fillType == FT_GRADIENT ) {
        redrawPix = true;
    }
}

void KPClosedLineObject::loadOasis( const QDomElement &element, KoOasisContext & context, KPRLoadingInfo *info )
{
    kdDebug()<<"void KPClosedLineObject::loadOasis( const QDomElement &element )***********\n";
    KP2DObject::loadOasis( element,context, info );
    //load point.
    QStringList ptList = QStringList::split(' ', element.attribute("draw:points"));

    QString pt_x, pt_y;
    double tmp_x, tmp_y;
    unsigned int index = 0;
    for (QStringList::Iterator it = ptList.begin(); it != ptList.end(); ++it)
    {
        tmp_x = (*it).section(',',0,0).toInt() / 100;
        tmp_y = (*it).section(',',1,1).toInt() / 100;

        pt_x.setNum(tmp_x);
        pt_x+="mm";

        pt_y.setNum(tmp_y);
        pt_y+="mm";

        points.putPoints( index, 1, KoUnit::parseValue(pt_x),KoUnit::parseValue(pt_y) );
        ++index;
    }
}

KoSize KPClosedLineObject::getRealSize() const {
    KoSize size( ext );
    KoPoint realOrig( orig );
    KoPointArray p( points );
    getRealSizeAndOrigFromPoints( p, angle, size, realOrig );
    return size;
}

KoPoint KPClosedLineObject::getRealOrig() const {
    KoSize size( ext );
    KoPoint realOrig( orig );
    KoPointArray p( points );
    getRealSizeAndOrigFromPoints( p, angle, size, realOrig );
    return realOrig;
}
