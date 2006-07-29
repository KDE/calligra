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
#include "field.h"

#include <klocale.h>
#include <klineeditdlg.h>

#include <kdebug.h>

#include <koproperty/property.h>

namespace Kudesigner
{

Field::Field( int x, int y, int width, int height, Canvas *canvas, bool reg )
        : Label( x, y, width, height, canvas )
{
    QMap<QString, QString> m;

    props.setGroupDescription( "Field", i18n( "Field" ) );

    props.addProperty( new Property( "Field", "", i18n( "Name" ), i18n( "Field Name" ), KoProperty::String ), "Field" );

    m[ i18n( "String" ) ] = "0";
    m[ i18n( "Integer" ) ] = "1";
    m[ i18n( "Float" ) ] = "2";
    m[ i18n( "Date" ) ] = "3";
    m[ i18n( "Currency" ) ] = "4";
    props.addProperty( new Property( "DataType", m.values(), m.keys(), "0", i18n( "Type" ), i18n( "Data Type" ), KoProperty::String ), "Field" );
    m.clear();

    m[ i18n( "m/d/y" ) ] = "0";
    m[ i18n( "m-d-y" ) ] = "1";
    m[ i18n( "mm/dd/y" ) ] = "2";
    m[ i18n( "mm-dd-y" ) ] = "3";
    m[ i18n( "m/d/yyyy" ) ] = "4";
    m[ i18n( "m-d-yyyy" ) ] = "5";
    m[ i18n( "mm/dd/yyyy" ) ] = "6";
    m[ i18n( "mm-dd-yyyy" ) ] = "7";
    m[ i18n( "yyyy/m/d" ) ] = "8";
    m[ i18n( "yyyy-m-d" ) ] = "9";
    m[ i18n( "dd.mm.yy" ) ] = "10";
    m[ i18n( "dd.mm.yyyy" ) ] = "11";

    //TODO: make date format not hard-coded, use locale settings
    props.addProperty( new Property( "DateFormat", m.values(), m.keys(), "11", i18n( "Date Format" ), i18n( "Date Format" ), KoProperty::String ), "Field" );
    m.clear();

    props.addProperty( new Property( "Precision", 2, i18n( "Precision" ), i18n( "Number of Digits After Comma" ), KoProperty::Integer ), "Field" );

    //TODO: make currency locale-aware
    props.addProperty( new Property( "Currency", 32, i18n( "Currency" ), i18n( "Currency Symbol" ), KoProperty::Symbol ), "Field" );

    props.addProperty( new Property( "NegValueColor", QColor( 0, 0, 0 ), i18n( "Negative Value Color" ), i18n( "Negative Value Color" ),  KoProperty::Color ), "Field" );

    props.addProperty( new Property( "CommaSeparator", 44, i18n( "Comma Separator" ), i18n( "Comma Separator" ), KoProperty::Symbol ), "Field" );

    props.addProperty( new Property( "InputMask", "", i18n( "InputMask" ), i18n( "InputMask" ), KoProperty::String ), "Field" );

    if ( reg )
        registerAs( Rtti_Field );
}

void Field::draw( QPainter &painter )
{
    props[ "Text" ].setValue( "[" + props[ "Field" ].value().toString() + "]" );
    Label::draw( painter );
    props[ "Text" ].setValue( "" );
}

QString Field::getXml()
{
    return "\t\t<Field" + ReportItem::getXml() + " />\n";
}

void Field::fastProperty()
{
    bool accepted;
    QString sValue = props[ "Field" ].value().toString();
    QString sText = KLineEditDlg::getText( i18n( "Change Field" ),
                                           "Enter field name:", sValue , &accepted );

    if ( accepted )
        props[ "Field" ].setValue( sText );

}

}
