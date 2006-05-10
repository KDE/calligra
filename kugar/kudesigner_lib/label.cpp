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
#include "label.h"

#include <klocale.h>
#include <kinputdialog.h>

#include <QMap>
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
    QMap<QString, QString> m;

    props.addProperty( new Property( "Text", i18n( "Text" ), i18n( "Text to Display" ), i18n( "Text" ), KoProperty::String ) );

    props.setGroupDescription( "Geometry", i18n( "Geometry" ) );
    props.addProperty( new Property( "X", x, i18n( "X" ), i18n( "X value" ), KoProperty::Integer ), "Geometry" );

    props.addProperty( new Property( "Y", y, i18n( "Y" ), i18n( "Y value" ), KoProperty::Integer ), "Geometry" );

    props.addProperty( new Property( "Width", width, i18n( "Width" ), i18n( "Width" ), KoProperty::Integer ), "Geometry" );

    props.addProperty( new Property( "Height", height, i18n( "Height" ), i18n( "Height" ), KoProperty::Integer ), "Geometry" );

    props.addProperty( new Property( "BackgroundColor", QColor( 255, 255, 255 ), i18n( "Background Color" ), i18n( "Background Color" ), KoProperty::Color ) );

    props.addProperty( new Property( "ForegroundColor", QColor( 0, 0, 0 ), i18n( "Foreground Color" ), i18n( "Foreground Color" ), KoProperty::Color ) );

    props.setGroupDescription( "BorderStyle", i18n( "Border Style" ) );
    props.addProperty( new Property( "BorderColor", QColor( 0, 0, 0 ), i18n( "Color" ), i18n( "Border Color" ), KoProperty::Color ), "BorderStyle" );

    props.addProperty( new Property( "BorderWidth", 1, i18n( "Width" ), i18n( "Border Width" ), KoProperty::Integer ), "BorderStyle" );

    props.addProperty( new Property( "BorderStyle", 1, i18n( "Line" ), i18n( "Border Style" ), KoProperty::LineStyle ), "BorderStyle" );

    props.setGroupDescription( "DrawBorder", i18n( "Border" ) );
    props.addProperty( new Property( "DrawTop", QVariant( false, 3 ), i18n( "Top" ), i18n( "Draw Top Border" ), KoProperty::Boolean ), "DrawBorder" );
    props.addProperty( new Property( "DrawBottom", QVariant( false, 3 ), i18n( "Bottom" ), i18n( "Draw Bottom Border" ), KoProperty::Boolean ), "DrawBorder" );
    props.addProperty( new Property( "DrawLeft", QVariant( false, 3 ), i18n( "Left" ), i18n( "Draw Left Border" ), KoProperty::Boolean ), "DrawBorder" );
    props.addProperty( new Property( "DrawRight", QVariant( false, 3 ), i18n( "Right" ), i18n( "Draw Right Border" ), KoProperty::Boolean ), "DrawBorder" );


    props.setGroupDescription( "Font", i18n( "Font" ) );
    props.addProperty( new Property( "FontFamily", QApplication::font(), i18n( "Family" ), i18n( "Font Family" ), KoProperty::Font ), "Font" );

    props.addProperty( new Property( "FontSize", QApplication::font().pointSize(), i18n( "Size" ), i18n( "Font Size" ), KoProperty::Integer ), "Font" );

    m[ i18n( "Light" ) ] = "25";
    m[ i18n( "Normal" ) ] = "50";
    m[ i18n( "DemiBold" ) ] = "63";
    m[ i18n( "Bold" ) ] = "75";
    m[ i18n( "Black" ) ] = "87";
    props.addProperty( new Property( "FontWeight", m.values(), m.keys(), "50", i18n( "Weight" ), i18n( "Weight" ) ), "Font" );
    m.clear();

    m[ i18n( "Regular" ) ] = "0";
    m[ i18n( "Italic" ) ] = "1";
    props.addProperty( new Property( "FontItalic", m.values(), m.keys(), "0", i18n( "Italic" ), i18n( "Italic" ) ), "Font" );
    m.clear();

    m[ i18n( "Left" ) ] = "0";
    m[ i18n( "Center" ) ] = "1";
    m[ i18n( "Right" ) ] = "2";
    props.addProperty( new Property( "HAlignment", m.values(), m.keys(),
                                     QString( "" ).isRightToLeft() ? "2" : "0", i18n( "HAlignment" ), i18n( "HAlignment" ) ) );
    m.clear();

    m[ i18n( "Top" ) ] = "0";
    m[ i18n( "Middle" ) ] = "1";
    m[ i18n( "Bottom" ) ] = "2";
    props.addProperty( new Property( "VAlignment", m.values(), m.keys(), "1", i18n( "VAlignment" ), i18n( "VAlignment" ) ) );
    m.clear();

    m[ i18n( "False" ) ] = "0";
    m[ i18n( "True" ) ] = "1";
    props.addProperty( new Property( "WordWrap", m.values(), m.keys(), "0", i18n( "Word wrap" ), i18n( "Word wrap" ) ) );
}

int Label::getTextAlignment()
{
    int result = 0;
    switch ( props[ "HAlignment" ].value().toInt() )
    {
    case 0:
        result = Qt::AlignLeft;
        break;
    case 1:
        result = Qt::AlignHCenter;
        break;
    case 2:
        result = Qt::AlignRight;
        break;
    default:
        result = Qt::AlignHCenter;
    }
    switch ( props[ "VAlignment" ].value().toInt() )
    {
    case 0:
        result = result | Qt::AlignTop;
        break;
    case 1:
        result = result | Qt::AlignVCenter;
        break;
    case 2:
        result = result | Qt::AlignBottom;
        break;
    default:
        result = result | Qt::AlignVCenter;
    }
    return result;
}

int Label::getTextWrap()
{
    switch ( props[ "WordWrap" ].value().toInt() )
    {
    case 0:
        return Qt::SingleLine;
        break;
    case 1:
        return Qt::WordBreak;
        break;
    default:
        return Qt::SingleLine;
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
    QColor color = props[ "ForegroundColor" ].value().value<QColor>();
	return QPen(color );
}

QPen Label::getPenForShape()
{
		Qt::PenStyle style = Qt::SolidLine;
    switch ( props[ "BorderStyle" ].value().toInt() )
    {
    case 0:
        style = Qt::NoPen;
        break;
    case 1:
        style = Qt::SolidLine;
        break;
    case 2:
        style = Qt::DashLine;
        break;
    case 3:
        style = Qt::DotLine;
        break;
    case 4:
        style = Qt::DashDotLine;
        break;
    case 5:
        style = Qt::DashDotDotLine;
        break;
    }

	QColor color = props[ "BorderColor" ].value().value<QColor>();
    return QPen( color ,
                 props[ "BorderWidth" ].value().toInt(), style );
}

QBrush Label::getBrush()
{
	QColor color = props[ "BackgroundColor" ].value().value<QColor>();
    return QBrush( color );
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
    painter.setPen( Qt::NoPen );
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

    QString sText = KInputDialog::getText( i18n( "Change Label" ),
                                           "Enter label name:", props[ "Text" ].value().toString(),
                                           &accepted );

    if ( accepted )
        props[ "Text" ].setValue( sText );
}

}
