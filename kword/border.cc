/* This file is part of the KDE project
   Copyright (C) 2000 Thomas Zander <zander@earthling.net>

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

#include "border.h"
#include <klocale.h>
#include <qdom.h>
#include <qpainter.h>
#include <kwdoc.h>

Border::Border()
    : color( Qt::black ), style( SOLID ), ptWidth( 1 ) { }

Border::Border( const QColor & c, BorderStyle s, double width )
    : color( c ), style( s ), ptWidth( width ) { }

bool Border::operator==( const Border _brd ) const {
    return ( style == _brd.style && color == _brd.color && ptWidth == _brd.ptWidth );
}

bool Border::operator!=( const Border _brd ) const {
    return ( style != _brd.style || color != _brd.color || ptWidth != _brd.ptWidth );
}

/*================================================================*/
QPen Border::borderPen( const Border & _brd, int width )
{
    QPen pen( _brd.color, width );
    switch ( _brd.style ) {
    case Border::SOLID:
        pen.setStyle( SolidLine );
        break;
    case Border::DASH:
        pen.setStyle( DashLine );
        break;
    case Border::DOT:
        pen.setStyle( DotLine );
        break;
    case Border::DASH_DOT:
        pen.setStyle( DashDotLine );
        break;
    case Border::DASH_DOT_DOT:
        pen.setStyle( DashDotDotLine );
        break;
    }

    return pen;
}

Border Border::loadBorder( const QDomElement & elem )
{
    int r = elem.attribute("red").toInt();
    int g = elem.attribute("green").toInt();
    int b = elem.attribute("blue").toInt();
    Border bd;
    bd.color.setRgb( r, g, b );
    bd.style = static_cast<BorderStyle>( elem.attribute("style").toInt() );
    bd.ptWidth = elem.attribute("width").toInt();
    return bd;
}

void Border::save( QDomElement & elem )
{
    elem.setAttribute("red", color.red());
    elem.setAttribute("green", color.green());
    elem.setAttribute("blue", color.blue());
    elem.setAttribute("style", static_cast<int>( style ));
    elem.setAttribute("width", ptWidth);
}

Border::BorderStyle Border::getStyle( const QString &style )
{
    if ( style == "___ ___ __" )
        return Border::DASH;
    if ( style == "_ _ _ _ _ _" )
        return Border::DOT;
    if ( style == "___ _ ___ _" )
        return Border::DASH_DOT;
    if ( style == "___ _ _ ___" )
        return Border::DASH_DOT_DOT;

    // default
    return Border::SOLID;
}

QString Border::getStyle( const BorderStyle &style )
{
    switch ( style )
    {
    case Border::SOLID:
        return "_________";
    case Border::DASH:
        return "___ ___ __";
    case Border::DOT:
        return "_ _ _ _ _ _";
    case Border::DASH_DOT:
        return "___ _ ___ _";
    case Border::DASH_DOT_DOT:
        return "___ _ _ ___";
    }

    // Keep compiler happy.
    return "_________";
}

int Border::zoomWidthX( double ptWidth, KWDocument * doc, int minborder )
{
    // If a border was set, then zoom it and apply a minimum of 1, so that it's always visible.
    // If no border was set, apply minborder ( 0 for paragraphs, 1 for frames )
    return ptWidth > 0 ? QMAX( 1, doc->zoomItX( ptWidth ) /*applies qRound*/ ) : minborder;
}

int Border::zoomWidthY( double ptWidth, KWDocument * doc, int minborder )
{
    // If a border was set, then zoom it and apply a minimum of 1, so that it's always visible.
    // If no border was set, apply minborder ( 0 for paragraphs, 1 for frames )
    return ptWidth > 0 ? QMAX( 1, doc->zoomItY( ptWidth ) /*applies qRound*/ ) : minborder;
}

void Border::drawBorders( QPainter& painter, KWDocument * doc, QRect rect, Border leftBorder, Border rightBorder, Border topBorder, Border bottomBorder, int minborder, QPen defaultPen )
{
    int topBorderWidth = zoomWidthY( topBorder.ptWidth, doc, minborder );
    int bottomBorderWidth = zoomWidthY( bottomBorder.ptWidth, doc, minborder );
    int leftBorderWidth = zoomWidthX( leftBorder.ptWidth, doc, minborder );
    int rightBorderWidth = zoomWidthX( rightBorder.ptWidth, doc, minborder );

    //kdDebug() << "Border::drawBorders top=" << topBorderWidth << " bottom=" << bottomBorderWidth
    //          << " left=" << leftBorderWidth << " right=" << rightBorderWidth << endl;

    if ( topBorderWidth > 0 )
    {
        if ( topBorder.ptWidth > 0 )
            painter.setPen( Border::borderPen( topBorder, topBorderWidth ) );
        else
            painter.setPen( defaultPen );
        int y = rect.top() - topBorderWidth + topBorderWidth/2;
        painter.drawLine( rect.left()-leftBorderWidth, y, rect.right()+rightBorderWidth, y );
    }
    if ( bottomBorderWidth > 0 )
    {
        if ( bottomBorder.ptWidth > 0 )
            painter.setPen( Border::borderPen( bottomBorder, bottomBorderWidth ) );
        else
            painter.setPen( defaultPen );
        int y = rect.bottom() + bottomBorderWidth - bottomBorderWidth/2;
        painter.drawLine( rect.left()-leftBorderWidth, y, rect.right()+rightBorderWidth, y );
    }
    if ( leftBorderWidth > 0 )
    {
        if ( leftBorder.ptWidth > 0 )
            painter.setPen( Border::borderPen( leftBorder, leftBorderWidth ) );
        else
            painter.setPen( defaultPen );
        int x = rect.left() - leftBorderWidth + leftBorderWidth/2;
        painter.drawLine( x, rect.top()-topBorderWidth, x, rect.bottom()+bottomBorderWidth );
    }
    if ( rightBorderWidth > 0 )
    {
        if ( rightBorder.ptWidth > 0 )
            painter.setPen( Border::borderPen( rightBorder, rightBorderWidth ) );
        else
            painter.setPen( defaultPen );
        int x = rect.right() + rightBorderWidth - rightBorderWidth/2;
        painter.drawLine( x, rect.top()-topBorderWidth, x, rect.bottom()+bottomBorderWidth );
    }
}
