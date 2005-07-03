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
  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.
*/
#include "label.h"

#include <klocale.h>
#include <klineeditdlg.h>

#include <qmap.h>
#include <qpainter.h>

#include <kdebug.h>
#include <klocale.h>

#include <koproperty/property.h>

#include "band.h"

namespace Kudesigner
{

Label::Label( int x, int y, int width, int height, Canvas *canvas )
        : ReportItem( x, y, width, height, canvas )
{
    QMap<QString, QVariant> m;
#warning TODO
#if 0

    props.addProperty( new Property( "Text", i18n( "Text" ), i18n( "Text to Display" ), i18n( "Text" ), KoProperty::String ) );

    props.setGroupDescription( "Geometry", i18n( "Geometry" ) );
    props.addProperty( new Property( "X", i18n( "X" ), i18n( "X value" ), x, KoProperty::Integer ), "Geometry" );

    props.addProperty( new Property( "Y", i18n( "Y" ), i18n( "Y value" ), y, KoProperty::Integer ), "Geometry" );

    props.addProperty( new Property( "Width", i18n( "Width" ), i18n( "Width" ), width, KoProperty::Integer ), "Geometry" );

    props.addProperty( new Property( "Height", i18n( "Height" ), i18n( "Height" ), height, KoProperty::Integer ), "Geometry" );

    props.addProperty( new Property( "BackgroundColor", i18n( "Background Color" ), i18n( "Background Color" ), QColor( 255, 255, 255 ), KoProperty::Color ) );

    props.addProperty( new Property( "ForegroundColor", i18n( "Foreground Color" ), i18n( "Foreground Color" ), QColor( 0, 0, 0 ), KoProperty::Color ) );

    props.setGroupDescription( "BorderStyle", i18n( "Border Style" ) );
    props.addProperty( new Property( "BorderColor", i18n( "Color" ), i18n( "Border Color" ), QColor( 0, 0, 0 ), KoProperty::Color ), "BorderStyle" );

    props.addProperty( new Property( "BorderWidth", i18n( "Width" ), i18n( "Border Width" ), 1, KoProperty::Integer ), "BorderStyle" );

    props.addProperty( new Property( "BorderStyle", i18n( "Line" ), i18n( "Border Style" ), 1, KoProperty::LineStyle ), "BorderStyle" );

    props.setGroupDescription( "DrawBorder", i18n( "Border" ) );
    props.addProperty( new Property( "DrawTop", i18n( "Top" ), i18n( "Draw Top Border" ), QVariant( false, 3 ), KoProperty::Boolean ), "DrawBorder" );
    props.addProperty( new Property( "DrawBottom", i18n( "Bottom" ), i18n( "Draw Bottom Border" ), QVariant( false, 3 ), KoProperty::Boolean ), "DrawBorder" );
    props.addProperty( new Property( "DrawLeft", i18n( "Left" ), i18n( "Draw Left Border" ), QVariant( false, 3 ), KoProperty::Boolean ), "DrawBorder" );
    props.addProperty( new Property( "DrawRight", i18n( "Right" ), i18n( "Draw Right Border" ), QVariant( false, 3 ), KoProperty::Boolean ), "DrawBorder" );


    props.setGroupDescription( "Font", i18n( "Font" ) );
    props.addProperty( new Property( "FontFamily", i18n( "Family" ), i18n( "Font Family" ), QFont( "Times New Roman" ), KoProperty::Font ), "Font" );

    props.addProperty( new Property( "FontSize", i18n( "Size" ), i18n( "Font Size" ), 12, KoProperty::Integer ), "Font" );

    m[ i18n( "Light" ) ] = "25";
    m[ i18n( "Normal" ) ] = "50";
    m[ i18n( "DemiBold" ) ] = "63";
    m[ i18n( "Bold" ) ] = "75";
    m[ i18n( "Black" ) ] = "87";
    props.addProperty( new Property( "FontWeight", i18n( "Weight" ), i18n( "Weight" ), m, "50" ), "Font" );
    m.clear();

    m[ i18n( "Regular" ) ] = "0";
    m[ i18n( "Italic" ) ] = "1";
    props.addProperty( new Property( "FontItalic", i18n( "Italic" ), i18n( "Italic" ), m, "0" ), "Font" );
    m.clear();

    m[ i18n( "Left" ) ] = "0";
    m[ i18n( "Center" ) ] = "1";
    m[ i18n( "Right" ) ] = "2";
    props.addProperty( new Property( "HAlignment", i18n( "HAlignment" ), i18n( "HAlignment" ), m,
                                     QString( "" ).isRightToLeft() ? "2" : "0" ) );
    m.clear();

    m[ i18n( "Top" ) ] = "0";
    m[ i18n( "Middle" ) ] = "1";
    m[ i18n( "Bottom" ) ] = "2";
    props.addProperty( new Property( "VAlignment", i18n( "VAlignment" ), i18n( "VAlignment" ), m, "1" ) );
    m.clear();

    m[ i18n( "False" ) ] = "0";
    m[ i18n( "True" ) ] = "1";
    props.addProperty( new Property( "WordWrap", i18n( "Word wrap" ), i18n( "Word wrap" ), m, "0" ) );
#endif
}

int Label::getTextAlignment()
{
    int result = 0;
    switch ( props[ "HAlignment" ].value().toInt() )
    {
    case 0:
        result = AlignLeft;
        break;
    case 1:
        result = AlignHCenter;
        break;
    case 2:
        result = AlignRight;
        break;
    default:
        result = AlignHCenter;
    }
    switch ( props[ "VAlignment" ].value().toInt() )
    {
    case 0:
        result = result | AlignTop;
        break;
    case 1:
        result = result | AlignVCenter;
        break;
    case 2:
        result = result | AlignBottom;
        break;
    default:
        result = result | AlignVCenter;
    }
    return result;
}

int Label::getTextWrap()
{
    switch ( props[ "WordWrap" ].value().toInt() )
    {
    case 0:
        return SingleLine;
        break;
    case 1:
        return WordBreak;
        break;
    default:
        return SingleLine;
    }
}

QFont Label::getFont()
{
    return QFont( props[ "FontFamily" ].value().toString(),
                  props[ "FontSize" ].value().toInt(),
                  props[ "FontWeight" ].value().toInt(),
                  props[ "FontItalic" ].value().toInt() );
}

QPen Label::getPenForText()
{
    return QPen( QColor( props[ "ForegroundColor" ].value().toColor() ) );
}

QPen Label::getPenForShape()
{
    PenStyle style = SolidLine;
    switch ( props[ "BorderStyle" ].value().toInt() )
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
    return QPen( QColor( props[ "BorderColor" ].value().toColor() ),
                 props[ "BorderWidth" ].value().toInt(), style );
}

QBrush Label::getBrush()
{
    return QBrush( QColor( props[ "BackgroundColor" ].value().toColor() ) );
}

void Label::draw( QPainter &painter )
{
    //update dimensions
    if ( !section() )
        return ;

    setX( props[ "X" ].value().toInt() + section() ->x() );
    setY( props[ "Y" ].value().toInt() + section() ->y() );
    setSize( props[ "Width" ].value().toInt(), props[ "Height" ].value().toInt() );

    //draw border and background
    painter.setBrush( getBrush() );
    painter.setPen( NoPen );
    painter.drawRect( rect() );
    painter.setPen( getPenForShape() );
    if ( props[ "DrawLeft" ].value().toBool() )
        painter.drawLine( rect().topLeft(), rect().bottomLeft() );
    if ( props[ "DrawRight" ].value().toBool() )
        painter.drawLine( rect().topRight(), rect().bottomRight() );
    if ( props[ "DrawTop" ].value().toBool() )
        painter.drawLine( rect().topLeft(), rect().topRight() );
    if ( props[ "DrawBottom" ].value().toBool() )
        painter.drawLine( rect().bottomLeft(), rect().bottomRight() );

    //draw text inside
    painter.setFont( getFont() );
    painter.setPen( getPenForText() );
    painter.drawText( ( int ) x(), ( int ) y(), width(), height(),
                      getTextAlignment() | getTextWrap(),
                      props[ "Text" ].value().toString() );

    //draw resizable region
    //    if (isSelected())
    //        drawHolders(painter);
    //    CanvasReportItem::draw(painter);
}

QString Label::getXml()
{
    return "\t\t<Label" + ReportItem::getXml() + " />\n";
}

void Label::fastProperty()
{
    bool accepted;

    QString sText = KLineEditDlg::getText( i18n( "Change Label" ),
                                           "Enter label name:", props[ "Text" ].value().toString(),
                                           &accepted );

    if ( accepted )
        props[ "Text" ].setValue( sText );
}

}
