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
#include "field.h"

#ifndef PURE_QT
#include <klocale.h>
#include <klineeditdlg.h>
#else
#include <compat_tools.h>
#include <qinputdialog.h>
#endif

#include <kdebug.h>

#include <property.h>

namespace Kudesigner
{

Field::Field( int x, int y, int width, int height, Canvas *canvas, bool reg )
        : Label( x, y, width, height, canvas )
{
    QMap<QString, QVariant> m;

    props.setGroupDescription( "Field", i18n( "Field" ) );
    props.addProperty( new Property( "Field", i18n( "Field to Display" ), i18n( "Field Name" ), "", KOProperty::String ), "Field" );

    m[ i18n( "String" ) ] = "0";
    m[ i18n( "Integer" ) ] = "1";
    m[ i18n( "Float" ) ] = "2";
    m[ i18n( "Date" ) ] = "3";
    m[ i18n( "Currency" ) ] = "4";
    props.addProperty( new Property( "DataType", i18n( "Data Type" ), "d", m, "0", KOProperty::String ), "Field" );
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
    props.addProperty( new Property( "DateFormat", i18n( "Date format" ), i18n( "Date format" ), m, "11", KOProperty::String ), "Field" );
    m.clear();

    props.addProperty( new Property( "Precision", i18n( "Number of Digits After Comma" ), i18n( "Number of Digits After Comma" ), 2, KOProperty::Integer ), "Field" );

    //TODO: make currency locale-aware
    props.addProperty( new Property( "Currency", i18n( "Currency Symbol" ), i18n( "Currency Symbol" ), 32, KOProperty::Symbol ), "Field" );

    props.addProperty( new Property( "NegValueColor", i18n( "Negative Value Color" ), i18n( "Negative Value Color" ), QColor( 0, 0, 0 ),  KOProperty::Color ), "Field" );

    props.addProperty( new Property( "CommaSeparator", i18n( "Comma Separator" ), i18n( "Comma Separator" ), 44, KOProperty::Symbol ), "Field" );

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

#ifndef PURE_QT

    QString sText = KLineEditDlg::getText( i18n( "Change Field" ),
                                           "Enter field name:", sValue , &accepted );
#else

    QString sText = QInputDialog::getText( i18n( "Change Field" ),
                                           "Enter field name:", QLineEdit::Normal, sValue , &accepted );
#endif

    if ( accepted )
        props[ "Field" ].setValue( sText );

}

}
