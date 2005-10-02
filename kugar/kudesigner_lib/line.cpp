/* This file is part of the KDE project
 Copyright (C) 2002-2004 Alexander Dymo <adymo@mksat.net>

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
#include "line.h"

#include <klocale.h>

#include <qpainter.h>

#include <koproperty/property.h>

#include "band.h"

namespace Kudesigner
{

Line::Line( int x, int y, int width, int height, Canvas *canvas ) :
        ReportItem( x, y, width, height, canvas )
{
    props.setGroupDescription( "Line", i18n( "Line" ) );
    props.addProperty( new Property( "X1", x, i18n( "X1" ), i18n( "X1" ), KoProperty::Integer ), "Line" );

    props.addProperty( new Property( "Y1", y, i18n( "Y1" ), i18n( "Y1" ), KoProperty::Integer ), "Line" );

    props.addProperty( new Property( "X2", x + width, i18n( "X2" ), i18n( "X2" ), KoProperty::Integer ), "Line" );

    props.addProperty( new Property( "Y2", y + height, i18n( "Y2" ), i18n( "Y2" ), KoProperty::Integer ), "Line" );

    props.addProperty( new Property( "Color", QColor( 0, 0, 0 ), i18n( "Color" ), i18n( "Color" ), KoProperty::Color ), "Line" );

    props.addProperty( new Property( "Width", 1, i18n( "Width" ), i18n( "Width" ), KoProperty::Integer ), "Line" );

    props.addProperty( new Property( "Style", 1, i18n( "Line Style" ), i18n( "Line Style" ), KoProperty::LineStyle ), "Line" );
}

QString Line::getXml()
{
    return "\t\t<Line" + ReportItem::getXml() + " />\n";
}

QPen Line::getPenForShape()
{
    PenStyle style = SolidLine;
    switch ( props[ "Style" ].value().toInt() )
    {
    case 0:
        style = NoPen;
        break;
    case 1:
        style = SolidLine;
        break;
    case 2:
        style = DashLine;
        break;
    case 3:
        style = DotLine;
        break;
    case 4:
        style = DashDotLine;
        break;
    case 5:
        style = DashDotDotLine;
        break;
    }
    return QPen( QColor( props[ "Color" ].value().toColor() ),
                 props[ "Width" ].value().toInt(), style );
}

void Line::draw( QPainter &painter )
{
    //update dimensions
    if ( !section() )
        return ;
    setX( props[ "X1" ].value().toInt() + section() ->x() );
    setY( props[ "Y1" ].value().toInt() + section() ->y() );
    setSize( props[ "X2" ].value().toInt() - props[ "X1" ].value().toInt(),
             props[ "Y2" ].value().toInt() - props[ "Y1" ].value().toInt() );

    //draw border and background
    painter.setPen( getPenForShape() );
    painter.setBrush( QColor( 0, 0, 0 ) );
    painter.drawLine( props[ "X1" ].value().toInt() + ( int ) section() ->x(),
                      props[ "Y1" ].value().toInt() + ( int ) section() ->y(),
                      props[ "X2" ].value().toInt() + ( int ) section() ->x(),
                      props[ "Y2" ].value().toInt() + ( int ) section() ->y() );

    painter.setPen( QColor( 0, 0, 0 ) );
    painter.setBrush( QColor( 0, 0, 0 ) );
    //    painter.drawRect(topLeftResizableRect());
    //    painter.drawRect(topRightResizableRect());
    //    painter.drawRect(bottomLeftResizableRect());
    painter.drawRect( bottomRightResizableRect() );

    //    drawHolders(painter);
}

void Line::setSection( Band *section )
{
    props[ "X1" ].setValue( ( int ) ( x() - section->x() ) );
    props[ "Y1" ].setValue( ( int ) ( y() - section->y() ) );
    parentSection = section;
}

void Line::updateGeomProps()
{
    props[ "X1" ].setValue( ( int ) ( x() - section() ->x() ) );
    props[ "Y1" ].setValue( ( int ) ( y() - section() ->y() ) );
    props[ "X2" ].setValue( ( int ) ( x() - section() ->x() + width() ) );
    props[ "Y2" ].setValue( ( int ) ( y() - section() ->y() + height() ) );

    //    if (width() )
}

}
