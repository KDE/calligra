/* This file is part of the KDE project
   Copyright (C) 2000, 2001 Thomas Zander <zander@kde.org>

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

#include "koborder.h"
#include <klocale.h>
#include <qdom.h>
#include <qapplication.h>
#include <qpalette.h>
#include <qpainter.h>
#include <kdebug.h>
#include "kozoomhandler.h"
#include "kotextformat.h"

KoBorder::KoBorder()
    : color(), style( SOLID ), ptWidth( 1 ) { }

KoBorder::KoBorder( const QColor & c, BorderStyle s, double width )
    : color( c ), style( s ), ptWidth( width ) { }

bool KoBorder::operator==( const KoBorder _brd ) const {
    return ( style == _brd.style && color == _brd.color && ptWidth == _brd.ptWidth );
}

bool KoBorder::operator!=( const KoBorder _brd ) const {
    return ( style != _brd.style || color != _brd.color || ptWidth != _brd.ptWidth );
}

QPen KoBorder::borderPen( const KoBorder & _brd, int width, QColor defaultColor )
{
    QPen pen( _brd.color, width );
    if ( !_brd.color.isValid() )
        pen.setColor( defaultColor );

    switch ( _brd.style ) {
    case KoBorder::SOLID:
        pen.setStyle( SolidLine );
        break;
    case KoBorder::DASH:
        pen.setStyle( DashLine );
        break;
    case KoBorder::DOT:
        pen.setStyle( DotLine );
        break;
    case KoBorder::DASH_DOT:
        pen.setStyle( DashDotLine );
        break;
    case KoBorder::DASH_DOT_DOT:
        pen.setStyle( DashDotDotLine );
        break;
    }

    return pen;
}

KoBorder KoBorder::loadBorder( const QDomElement & elem )
{
    KoBorder bd;
    if ( elem.hasAttribute("red") )
    {
        int r = elem.attribute("red").toInt();
        int g = elem.attribute("green").toInt();
        int b = elem.attribute("blue").toInt();
        bd.color.setRgb( r, g, b );
    }
    bd.style = static_cast<BorderStyle>( elem.attribute("style").toInt() );
    bd.ptWidth = elem.attribute("width").toInt();
    return bd;
}

void KoBorder::save( QDomElement & elem ) const
{
    if (color.isValid()) {
        elem.setAttribute("red", color.red());
        elem.setAttribute("green", color.green());
        elem.setAttribute("blue", color.blue());
    }
    elem.setAttribute("style", static_cast<int>( style ));
    elem.setAttribute("width", ptWidth);
}

KoBorder::BorderStyle KoBorder::getStyle( const QString &style )
{
    if ( style == "___ ___ __" )
        return KoBorder::DASH;
    if ( style == "_ _ _ _ _ _" )
        return KoBorder::DOT;
    if ( style == "___ _ ___ _" )
        return KoBorder::DASH_DOT;
    if ( style == "___ _ _ ___" )
        return KoBorder::DASH_DOT_DOT;

    // default
    return KoBorder::SOLID;
}

QString KoBorder::getStyle( const BorderStyle &style )
{
    switch ( style )
    {
    case KoBorder::SOLID:
        return "_________";
    case KoBorder::DASH:
        return "___ ___ __";
    case KoBorder::DOT:
        return "_ _ _ _ _ _";
    case KoBorder::DASH_DOT:
        return "___ _ ___ _";
    case KoBorder::DASH_DOT_DOT:
        return "___ _ _ ___";
    }

    // Keep compiler happy.
    return "_________";
}

int KoBorder::zoomWidthX( double ptWidth, KoZoomHandler * zoomHandler, int minborder )
{
    // If a border was set, then zoom it and apply a minimum of 1, so that it's always visible.
    // If no border was set, apply minborder ( 0 for paragraphs, 1 for frames )
    return ptWidth > 0 ? QMAX( 1, zoomHandler->zoomItX( ptWidth ) /*applies qRound*/ ) : minborder;
}

int KoBorder::zoomWidthY( double ptWidth, KoZoomHandler * zoomHandler, int minborder )
{
    // If a border was set, then zoom it and apply a minimum of 1, so that it's always visible.
    // If no border was set, apply minborder ( 0 for paragraphs, 1 for frames )
    return ptWidth > 0 ? QMAX( 1, zoomHandler->zoomItY( ptWidth ) /*applies qRound*/ ) : minborder;
}

void KoBorder::drawBorders( QPainter& painter, KoZoomHandler * zoomHandler, QRect rect, KoBorder leftBorder, KoBorder rightBorder, KoBorder topBorder, KoBorder bottomBorder, int minborder, QPen defaultPen )
{
    int topBorderWidth = zoomWidthY( topBorder.ptWidth, zoomHandler, minborder );
    int bottomBorderWidth = zoomWidthY( bottomBorder.ptWidth, zoomHandler, minborder );
    int leftBorderWidth = zoomWidthX( leftBorder.ptWidth, zoomHandler, minborder );
    int rightBorderWidth = zoomWidthX( rightBorder.ptWidth, zoomHandler, minborder );

    //kdDebug() << "KoBorder::drawBorders top=" << topBorderWidth << " bottom=" << bottomBorderWidth
    //          << " left=" << leftBorderWidth << " right=" << rightBorderWidth << endl;

    QColor defaultColor = KoTextFormat::defaultTextColor( &painter );

    if ( topBorderWidth > 0 )
    {
        if ( topBorder.ptWidth > 0 )
            painter.setPen( KoBorder::borderPen( topBorder, topBorderWidth, defaultColor ) );
        else
            painter.setPen( defaultPen );
        int y = rect.top() - topBorderWidth + topBorderWidth/2;
        painter.drawLine( rect.left()-leftBorderWidth, y, rect.right()+rightBorderWidth, y );
    }
    if ( bottomBorderWidth > 0 )
    {
        if ( bottomBorder.ptWidth > 0 )
            painter.setPen( KoBorder::borderPen( bottomBorder, bottomBorderWidth, defaultColor ) );
        else
            painter.setPen( defaultPen );
        int y = rect.bottom() + bottomBorderWidth - bottomBorderWidth/2;
        painter.drawLine( rect.left()-leftBorderWidth, y, rect.right()+rightBorderWidth, y );
    }
    if ( leftBorderWidth > 0 )
    {
        if ( leftBorder.ptWidth > 0 )
            painter.setPen( KoBorder::borderPen( leftBorder, leftBorderWidth, defaultColor ) );
        else
            painter.setPen( defaultPen );
        int x = rect.left() - leftBorderWidth + leftBorderWidth/2;
        painter.drawLine( x, rect.top()-topBorderWidth, x, rect.bottom()+bottomBorderWidth );
    }
    if ( rightBorderWidth > 0 )
    {
        if ( rightBorder.ptWidth > 0 )
            painter.setPen( KoBorder::borderPen( rightBorder, rightBorderWidth, defaultColor ) );
        else
            painter.setPen( defaultPen );
        int x = rect.right() + rightBorderWidth - rightBorderWidth/2;
        painter.drawLine( x, rect.top()-topBorderWidth, x, rect.bottom()+bottomBorderWidth );
    }
}
