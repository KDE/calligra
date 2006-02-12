// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "KPrUtils.h"

#include "KPrObject.h"
#include "KPrGroupObject.h"
#include "KPrFreehandObject.h"
#include "KPrPolylineObject.h"
#include "KPrBezierCurveObject.h"
#include "KPrPieObject.h"

#include <KoXmlNS.h>
#include <KoTextZoomHandler.h>
#include <KoPoint.h>

#include <qpainter.h>
#include <qpointarray.h>
#include <qpoint.h>
#include <qcolor.h>
#include <qsize.h>
#include <qbitmap.h>
#include <qcursor.h>
#include <qdatetime.h>
#include <qdom.h>

void drawFigure( LineEnd figure, QPainter* painter, const KoPoint &coord, const QColor &color,
                 int _w, float angle, KoZoomHandler* _zoomHandler)
{
    painter->save();
    painter->setPen( Qt::NoPen );
    painter->setBrush( Qt::NoBrush );

    switch ( figure )
    {
    case L_SQUARE:
    {
        int _h = _w;
        if ( _h % 2 == 0 ) _h--;
        painter->translate( _zoomHandler->zoomItX(coord.x()), _zoomHandler->zoomItY( coord.y()) );
        painter->rotate( angle );
        painter->scale( 1, 1 );
        painter->fillRect( _zoomHandler->zoomItX( -6 - _w ),
                           _zoomHandler->zoomItY( -3 - _h / 2),
                           _zoomHandler->zoomItX( 6 + _w),
                           _zoomHandler->zoomItY( 6 + _h), color );
    } break;
    case L_CIRCLE:
    {
        painter->translate( _zoomHandler->zoomItX(coord.x()), _zoomHandler->zoomItY(coord.y()) );
        painter->rotate( angle );
        painter->setBrush( color );
        painter->drawEllipse( _zoomHandler->zoomItX( -6 - _w ),
                              _zoomHandler->zoomItY( -3 - _w / 2 ),
                              _zoomHandler->zoomItX( 6 + _w),
                              _zoomHandler->zoomItY(6 + _w) );
    } break;
    case L_ARROW:
    {
        KoPoint p1( -10 - _w, -3 - _w / 2 );
        KoPoint p2( 0 , 0 );
        KoPoint p3( -10 - _w, 3 + _w / 2 );
        QPointArray pArray( 3 );
        pArray.setPoint( 0, _zoomHandler->zoomPoint(p1) );
        pArray.setPoint( 1, _zoomHandler->zoomPoint(p2) );
        pArray.setPoint( 2, _zoomHandler->zoomPoint(p3) );

        painter->translate( _zoomHandler->zoomItX(coord.x()),_zoomHandler->zoomItY( coord.y()) );
        painter->rotate( angle );
        painter->scale( 1, 1 );
        painter->setBrush( color );
        painter->drawPolygon( pArray );
    } break;
    case L_LINE_ARROW:
    {
        painter->translate( _zoomHandler->zoomItX(coord.x()),_zoomHandler->zoomItY( coord.y()) );
        painter->setPen( QPen(color , _zoomHandler->zoomItX( _w )) );
        painter->rotate( angle );
        painter->scale( 1, 1 );
        QPoint p1( _zoomHandler->zoomItX(-10 - _w), _zoomHandler->zoomItY(-3 - _w / 2) );
        QPoint p2( _zoomHandler->zoomItX(0), _zoomHandler->zoomItY(0) );
        QPoint p3( _zoomHandler->zoomItX(-10 - _w), _zoomHandler->zoomItY(3 + _w / 2) );
        painter->drawLine( p2, p1);
        painter->drawLine( p2, p3);
    }break;
    case L_DIMENSION_LINE:
    {
        painter->translate( _zoomHandler->zoomItX(coord.x()),_zoomHandler->zoomItY( coord.y()) );
        painter->setPen( QPen(color , _zoomHandler->zoomItX( _w )) );
        painter->rotate( angle );
        painter->scale( 1, 1 );
        QPoint p1( _zoomHandler->zoomItX(- _w / 2), _zoomHandler->zoomItY(-5 - _w / 2) );
        QPoint p2( _zoomHandler->zoomItX(- _w / 2), _zoomHandler->zoomItY(5 + _w / 2 ) );
        painter->drawLine( p1, p2);
    }break;
    case L_DOUBLE_ARROW:
    {
        painter->translate( _zoomHandler->zoomItX(coord.x()),_zoomHandler->zoomItY( coord.y()) );
        painter->rotate( angle );
        painter->scale( 1, 1 );
        painter->setBrush( color );

        KoPoint p1( -10 - _w , -3 - _w / 2 );
        KoPoint p2( 0, 0 );
        KoPoint p3( -10 - _w, 3 + _w / 2 );

        KoPoint p4( -20 - _w, -3 - _w / 2 );
        KoPoint p5( -10, 0 );
        KoPoint p6( -20 - _w, 3 + _w / 2 );

        QPointArray pArray( 3 );
        pArray.setPoint( 0, _zoomHandler->zoomPoint(p1) );
        pArray.setPoint( 1, _zoomHandler->zoomPoint(p2) );
        pArray.setPoint( 2, _zoomHandler->zoomPoint(p3) );
        painter->drawPolygon( pArray );
        pArray.setPoint( 0, _zoomHandler->zoomPoint(p4) );
        pArray.setPoint( 1, _zoomHandler->zoomPoint(p5) );
        pArray.setPoint( 2, _zoomHandler->zoomPoint(p6) );
        painter->drawPolygon( pArray );

    }break;
    case L_DOUBLE_LINE_ARROW:
    {
        painter->translate( _zoomHandler->zoomItX(coord.x()),_zoomHandler->zoomItY( coord.y()) );
        painter->setPen( QPen(color , _zoomHandler->zoomItX( _w )) );
        painter->rotate( angle );
        painter->scale( 1, 1 );
        QPoint p1( _zoomHandler->zoomItX(-10 - _w), _zoomHandler->zoomItY(-3 - _w / 2) );
        QPoint p2( _zoomHandler->zoomItX(0), _zoomHandler->zoomItY(0) );
        QPoint p3( _zoomHandler->zoomItX(-10 - _w), _zoomHandler->zoomItY(3 + _w / 2) );
        painter->drawLine( p2, p1);
        painter->drawLine( p2, p3);

        p1.setX( _zoomHandler->zoomItX(-20 - _w));
        p2.setX( _zoomHandler->zoomItX(-10));
        p3.setX( _zoomHandler->zoomItX(-20 - _w));
        painter->drawLine( p2, p1);
        painter->drawLine( p2, p3);
    }break;
    default: break;
    }
    painter->restore();
}

void drawFigureWithOffset( LineEnd figure, QPainter* painter, const QPoint &coord, const QColor &color,
                           int  _w, float angle, KoZoomHandler*_zoomHandler)
{
    KoSize diff = getOffset( figure, _w, angle );
    KoPoint offsetPoint(diff.width(), diff.height());
    offsetPoint += _zoomHandler->unzoomPoint( coord );
    drawFigure( figure, painter, offsetPoint, color, _w, angle,_zoomHandler );
}

void drawFigureWithOffset( LineEnd figure, QPainter* painter, const KoPoint &coord, const QColor &color,
                           int w, float angle, KoZoomHandler*_zoomHandler, bool begin )
{
    KoSize diff = getOffset( figure, w, angle );
    KoPoint offsetPoint(diff.width(), diff.height());
    double offsetAngle = angle + ( begin ? 90.0 : -90.0 );
    KoPoint lineOffset( w * cos( offsetAngle * M_PI / 180.0 ) / 2,
                        w * sin( offsetAngle * M_PI / 180.0 ) / 2 );
    offsetPoint += coord + lineOffset;
    drawFigure( figure, painter, offsetPoint, color, w, angle,_zoomHandler );
}

KoSize getBoundingSize( LineEnd figure, int _w, const KoZoomHandler*_zoomHandler )
{
    switch ( figure )
    {
    case L_SQUARE:
    {
        int _h = (int)_w;
        if ( _h % 2 == 0 ) _h--;
        return KoSize( _zoomHandler->zoomItX( 10 + _w), _zoomHandler->zoomItY( 10 + _h) );
    } break;
    case L_CIRCLE:
        return KoSize(  _zoomHandler->zoomItX(10 + _w), _zoomHandler->zoomItY(10 + _w) );
        break;
    case L_ARROW:
        return KoSize( _zoomHandler->zoomItX( 14 + _w),_zoomHandler->zoomItY( 14 + _w) );
        break;
    case L_LINE_ARROW:
        return KoSize( _zoomHandler->zoomItX( 14 + _w),_zoomHandler->zoomItY( 14 + _w) );
        break;
    case L_DIMENSION_LINE:
        return KoSize( _zoomHandler->zoomItX( 14 +_w),_zoomHandler->zoomItY( 14 + _w) );
        break;
    case L_DOUBLE_ARROW:
        return KoSize( _zoomHandler->zoomItX( 28 + _w),_zoomHandler->zoomItY( 14 + _w) );
        break;
    case L_DOUBLE_LINE_ARROW:
        return KoSize( _zoomHandler->zoomItX( 28 + _w),_zoomHandler->zoomItY( 14 + _w) );
        break;
    default: break;
    }

    return KoSize( 0, 0 );
}

KoSize getOffset( LineEnd figure, int _w, float angle )
{
    double x = 0;
    double y = 0;
    switch ( figure )
    {
    case L_ARROW:
    case L_DOUBLE_ARROW:
    {
        x = _w * 2;
        y = _w * 2;
        break;
    }
    case L_SQUARE:
    case L_CIRCLE:
    {
        x = _w + 3;
        y = _w + 3;
        break;
    }
    case L_LINE_ARROW:
    case L_DOUBLE_LINE_ARROW:
    {
        x = _w + 1;
        y = _w + 1;
        break;
    }
    case L_DIMENSION_LINE:
    {
        x = _w / 2;
        y = _w / 2;
        break;
    }
    default: break;
    }

    return KoSize( x * cos( angle * M_PI / 180.0 ), y * sin( angle * M_PI / 180 ) );
}

QString lineEndBeginName( LineEnd type )
{
    switch(type)
    {
    case L_NORMAL:
        return QString("NORMAL");
    case L_ARROW:
        return QString("ARROW");
    case L_SQUARE:
        return QString("SQUARE");
    case L_CIRCLE:
        return QString("CIRCLE");
    case L_LINE_ARROW:
        return QString("LINE_ARROW");
    case L_DIMENSION_LINE:
        return QString("DIMENSION_LINE");
    case L_DOUBLE_ARROW:
        return QString("DOUBLE_ARROW");
    case L_DOUBLE_LINE_ARROW:
        return QString("DOUBLE_LINE_ARROW");
    }
    return QString::null;
}

LineEnd lineEndBeginFromString( const QString & type )
{
    if(type=="NORMAL")
        return L_NORMAL;
    else if(type=="ARROW")
        return L_ARROW;
    else if(type=="SQUARE")
        return L_SQUARE;
    else if(type=="CIRCLE")
        return L_CIRCLE;
    else if(type=="LINE_ARROW")
        return L_LINE_ARROW;
    else if (type=="DIMENSION_LINE")
        return L_DIMENSION_LINE;
    else if (type=="DOUBLE_ARROW")
        return L_DOUBLE_ARROW;
    else if (type=="DOUBLE_LINE_ARROW")
        return L_DOUBLE_LINE_ARROW;
    else
        kdDebug(33001)<<"Error in LineEnd lineEndBeginFromString( const QString & name )\n";
    return L_NORMAL;
}

QString lineEndBeginSvg( LineEnd element )
{
    QString str;
    switch( element )
    {
    case L_NORMAL:
        //nothing
        break;
    case L_ARROW:
        str = "m10 0-10 30h20z";
        break;
    case L_SQUARE:
        str =  "m0 0h10v10h-10z";
        break;
    case L_CIRCLE:
        str = "m462 1118-102-29-102-51-93-72-72-93-51-102-29-102-13-105 13-102 29-106 51-102 72-89 93-72 102-50 102-34 106-9 101 9 106 34 98 50 93 72 72 89 51 102 29 106 13 102-13 105-29 102-51 102-72 93-93 72-98 51-106 29-101 13z";
        break;
    case L_LINE_ARROW:
        str = "m0 2108v17 17l12 42 30 34 38 21 43 4 29-8 30-21 25-26 13-34 343-1532 339 1520 13 42 29 34 39 21 42 4 42-12 34-30 21-42v-39-12l-4 4-440-1998-9-42-25-39-38-25-43-8-42 8-38 25-26 39-8 42z";
        break;
    case L_DIMENSION_LINE:
        str = "m0 0h278 278 280v36 36 38h-278-278-280v-36-36z";
        break;
    case L_DOUBLE_ARROW:
        str = "m737 1131h394l-564-1131-567 1131h398l-398 787h1131z";
        break;
    case L_DOUBLE_LINE_ARROW:
        //this define is not good I don't know how to define it.
        str = "m0 11h312 312h122z";
        break;
    }
    return str;
}

QCursor KPrUtils::penCursor()
{
    static unsigned char pen_bits[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x3a, 0x00, 0x00, 0x7d,
        0x00, 0x80, 0x7e, 0x00, 0x40, 0x7f, 0x00, 0xa0, 0x3f, 0x00, 0xd0, 0x1f,
        0x00, 0xe8, 0x0f, 0x00, 0xf4, 0x07, 0x00, 0xfa, 0x03, 0x00, 0xfd, 0x01,
        0x80, 0xfe, 0x00, 0x40, 0x7f, 0x00, 0xa0, 0x3f, 0x00, 0xf0, 0x1f, 0x00,
        0xd0, 0x0f, 0x00, 0x88, 0x07, 0x00, 0x88, 0x03, 0x00, 0xe4, 0x01, 0x00,
        0x7c, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00};

    QBitmap pen_bitmap( 24, 24, pen_bits, true );
    QBitmap pen_mask( 24, 24 );
    QPixmap pix( pen_bitmap );
    pen_mask = pix.createHeuristicMask( false );
    pix.setMask( pen_mask );

    return QCursor( pix, 1, 22 );
}

QCursor KPrUtils::rotateCursor()
{
    static unsigned char rotate_bits[] = {
        0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0xc0, 0xf3, 0x00, 0xe0, 0xff, 0x01,
        0xf0, 0xe1, 0x03, 0x70, 0x80, 0x03, 0x78, 0x80, 0x07, 0x38, 0x00, 0x07,
        0x38, 0x00, 0x07, 0xff, 0xe1, 0x3f, 0xfa, 0xc4, 0x17, 0x74, 0x8c, 0x0b,
        0x28, 0x3a, 0x05, 0x10, 0x1f, 0x02, 0x00, 0x0c, 0x00, 0x00, 0x08, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    QBitmap rotate_bitmap( 22, 22, rotate_bits, true );
    QBitmap rotate_mask( 22, 22 );
    QPixmap pix( rotate_bitmap );
    rotate_mask = pix.createHeuristicMask( false );
    pix.setMask( rotate_mask );

    return QCursor( pix, 11, 13 );
}

QString saveOasisTimer( int second )
{
    QTime time;
    time = time.addSecs( second );
    QString hours( QString::number( time.hour() ).rightJustify( 2, '0' ) );
    QString ms( QString::number( time.minute() ).rightJustify( 2, '0' ) );
    QString sec( QString::number( time.second() ).rightJustify( 2, '0' ) );


    //ISO8601 chapter 5.5.3.2
    //QDate doesn't encode it as this format.
    QString timeIso = QString( "PT%1H%2M%3S" ).arg( hours ).arg( ms ).arg( sec );
    return timeIso;
}

int loadOasisTimer( const QString & timer )
{
    QString str( timer );
    //convert date duration
    int hour( str.mid( 2, 2 ).toInt() );
    int minute( str.mid( 5, 2 ).toInt() );
    int second( str.mid( 8, 2 ).toInt() );

    return ( second + minute*60 + hour*60*60 );
}


int readOasisSettingsInt( const QDomElement & element )
{
    if ( element.hasAttributeNS( KoXmlNS::config, "type" ) )
    {
        QString type = element.attributeNS( KoXmlNS::config, "type", QString::null );
        QString str = element.text();
        bool b;
        int value = str.toInt( &b );
        return ( b ? value : 0 );
    }
    return 0;
}

double readOasisSettingsDouble( const QDomElement & element )
{
    if ( element.hasAttributeNS( KoXmlNS::config, "type" ) )
    {
        QString type = element.attributeNS( KoXmlNS::config, "type", QString::null );
        QString str = element.text();
        bool b;
        double value = str.toDouble( &b );
        return ( b ? value : 0 );
    }
    return 0.0;
}

bool readOasisSettingsBool( const QDomElement & element )
{
    if ( element.hasAttributeNS( KoXmlNS::config, "type" ) )
    {
        QString type = element.attributeNS( KoXmlNS::config, "type", QString::null );
        QString str = element.text();
        return ( str == "true" ? true : false );
    }
    return false;
}

QString readOasisSettingsString( const QDomElement & element )
{
    if ( element.hasAttributeNS( KoXmlNS::config, "type" ) )
    {
        QString type = element.attributeNS( KoXmlNS::config, "type", QString::null );
        QString str = element.text();
        return str;
    }
    return QString::null;
}
