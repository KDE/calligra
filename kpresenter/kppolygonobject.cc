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

#include <kppolygonobject.h>
#include <kpgradient.h>

#include <kdebug.h>
#include <qbitmap.h>
#include <qregion.h>
#include <qdom.h>
#include <qpicture.h>
#include <qpainter.h>

#include <math.h>

using namespace std;

/******************************************************************/
/* Class: KPPolygonObject                                         */
/******************************************************************/

/*================ default constructor ===========================*/
KPPolygonObject::KPPolygonObject()
    : KP2DObject()
{
    redrawPix = false;
}

/*================== overloaded constructor ======================*/
KPPolygonObject::KPPolygonObject( const QPointArray &_points, QSize _size, QPen _pen, QBrush _brush,
                                  FillType _fillType, QColor _gColor1, QColor _gColor2, BCType _gType,
                                  bool _unbalanced, int _xfactor, int _yfactor,
                                  bool _checkConcavePolygon, int _cornersValue, int _sharpnessValue )
    : KP2DObject( _pen, _brush, _fillType, _gColor1, _gColor2, _gType, _unbalanced, _xfactor, _yfactor )
{
    points = QPointArray( _points );
    origPoints = points;
    origSize = _size;

    checkConcavePolygon = _checkConcavePolygon;
    cornersValue = _cornersValue;
    sharpnessValue = _sharpnessValue;

    redrawPix = false;

    if ( fillType == FT_GRADIENT ) {
        gradient = new KPGradient( gColor1, gColor2, gType, QSize( 1, 1 ), unbalanced, xfactor, yfactor );
        redrawPix = true;
        pix.resize( getSize() );
    }
    else
        gradient = 0;
}

/*================================================================*/
KPPolygonObject &KPPolygonObject::operator=( const KPPolygonObject & )
{
    return *this;
}

/*========================= save =================================*/
QDomDocumentFragment KPPolygonObject::save( QDomDocument& doc )
{
    QDomDocumentFragment fragment = KP2DObject::save( doc );

    QDomElement elemSettings = doc.createElement( "SETTINGS" );

    elemSettings.setAttribute( "checkConcavePolygon", static_cast<int>( checkConcavePolygon ) );
    elemSettings.setAttribute( "cornersValue", cornersValue );
    elemSettings.setAttribute( "sharpnessValue", sharpnessValue );

    fragment.appendChild( elemSettings );

    if ( !points.isNull() ) {
        QDomElement elemPoints = doc.createElement( "POINTS" );
	QPointArray::ConstIterator it;
        for ( it = points.begin(); it != points.end(); ++it ) {
            QDomElement elemPoint = doc.createElement( "Point" );
            QPoint point = (*it);
            elemPoint.setAttribute( "point_x", point.x() );
            elemPoint.setAttribute( "point_y", point.y() );

            elemPoints.appendChild( elemPoint );
        }
        fragment.appendChild( elemPoints );
    }

    return fragment;
}

/*========================== load ================================*/
void KPPolygonObject::load( const QDomElement &element )
{
    KP2DObject::load( element );

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
}

/*================================================================*/
void KPPolygonObject::setSize( int _width, int _height )
{
    KPObject::setSize( _width, _height );
    if ( move ) return;

    double fx = (double)( (double)ext.width() / (double)origSize.width() );
    double fy = (double)( (double)ext.height() / (double)origSize.height() );

    updatePoints( fx, fy );

    if ( fillType == FT_GRADIENT && gradient ) {
        gradient->setSize( getSize() );
        redrawPix = true;
        pix.resize( getSize() );
    }
}

void KPPolygonObject::resizeBy( QSize _size )
{
    resizeBy( _size.width(), _size.height() );
}

/*================================================================*/
void KPPolygonObject::resizeBy( int _dx, int _dy )
{
    KPObject::resizeBy( _dx, _dy );
    if ( move ) return;

    double fx = (double)( (double)ext.width() / (double)origSize.width() );
    double fy = (double)( (double)ext.height() / (double)origSize.height() );

    updatePoints( fx, fy );

    if ( fillType == FT_GRADIENT && gradient ) {
        gradient->setSize( getSize() );
        redrawPix = true;
        pix.resize( getSize() );
    }
}

void KPPolygonObject::updatePoints( double _fx, double _fy )
{
    int index = 0;
    QPointArray tmpPoints;
    QPointArray::ConstIterator it;
    for ( it = origPoints.begin(); it != origPoints.end(); ++it ) {
        QPoint point = (*it);
        int tmpX = (int)( (double)point.x() * _fx );
        int tmpY = (int)( (double)point.y() * _fy );

        tmpPoints.putPoints( index, 1, tmpX,tmpY );
        ++index;
    }
    points = tmpPoints;
}

/*================================================================*/
void KPPolygonObject::setFillType( FillType _fillType )
{
    fillType = _fillType;

    if ( fillType == FT_BRUSH && gradient ) {
        delete gradient;
        gradient = 0;
    }

    if ( fillType == FT_GRADIENT && !gradient ) {
        gradient = new KPGradient( gColor1, gColor2, gType, getSize(), unbalanced, xfactor, yfactor );
        redrawPix = true;
        pix.resize( getSize() );
    }
}

/*======================== paint =================================*/
void KPPolygonObject::paint( QPainter* _painter )
{
    int _w = pen.width();

    QPointArray pointArray = points;
    if ( !move && _w > 1 ) {
        double fx = (double)( (double)( ext.width() - _w ) / (double)ext.width() );
        double fy = (double)( (double)( ext.height() - _w ) / (double)ext.height() );

        unsigned int index = 0;
        QPointArray tmpPoints;
        QPointArray::ConstIterator it;
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

    if ( drawShadow || fillType == FT_BRUSH || !gradient ) {
        _painter->setPen( pen );
        _painter->setBrush( brush );
        _painter->drawConvexPolygon( pointArray );
    }
    else {
        if ( redrawPix ) {
            redrawPix = false;
            QRegion clipregion( pointArray );

            pix.fill( Qt::white );

            QPainter p;
            p.begin( &pix );
            p.setClipRegion( clipregion );
            p.drawPixmap( 0, 0, *gradient->getGradient() );
            p.end();

            pix.setMask( pix.createHeuristicMask() );
        }

        QRect _rect = pointArray.boundingRect();
        _painter->drawPixmap( _w/2, _w/2, pix, 0, 0, _rect.width(), _rect.height() );

        _painter->setPen( pen );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawConvexPolygon( pointArray );;

    }
}

void KPPolygonObject::setPolygonSettings( bool _checkConcavePolygon, int _cornersValue, int _sharpnessValue )
{
    checkConcavePolygon = _checkConcavePolygon;
    cornersValue = _cornersValue;
    sharpnessValue = _sharpnessValue;

    drawPolygon();
}

void KPPolygonObject::getPolygonSettings( bool *_checkConcavePolygon, int *_cornersValue, int *_sharpnessValue )
{
    *_checkConcavePolygon = checkConcavePolygon;
    *_cornersValue = cornersValue;
    *_sharpnessValue = sharpnessValue;
}

void KPPolygonObject::drawPolygon()
{
    QRect _rect = points.boundingRect();
    double angle = 2 * M_PI / cornersValue;
    double diameter = static_cast<double>( QMAX( _rect.width(), _rect.height() ) );
    double radius = diameter * 0.5;

    QPointArray _points( checkConcavePolygon ? cornersValue * 2 : cornersValue );
    _points.setPoint( 0, 0, qRound( -radius ) );

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
            _points.setPoint( i, (int)xp, (int)yp );
        }
    }
    else {
        double a = angle;
        for ( int i = 1; i < cornersValue; ++i ) {
            double xp = radius * sin( a );
            double yp = -radius * cos( a );
            a += angle;
            _points.setPoint( i, (int)xp, (int)yp );
        }
    }

    QRect _changRect = _points.boundingRect();
    double fx = (double)( (double)_rect.width() / (double)_changRect.width() );
    double fy = (double)( (double)_rect.height() / (double)_changRect.height() );

    double _diffx = (double)_rect.width() / 2.0;
    double _diffy = (double)_rect.height() / 2.0;

    int _index = 0;
    QPointArray tmpPoints;
    QPointArray::ConstIterator it;
    for ( it = _points.begin(); it != _points.end(); ++it ) {
        QPoint point = (*it);
        int tmpX = (int)( ( (double)point.x() * fx ) + (int)_diffx );
        int tmpY = (int)( ( (double)point.y() * fy ) + (int)_diffy );

        tmpPoints.putPoints( _index, 1, tmpX,tmpY );
        ++_index;
    }

    points = tmpPoints;
    origPoints = points;
    origSize = ext;

    if ( fillType == FT_GRADIENT && gradient ) {
        redrawPix = true;
    }
}
