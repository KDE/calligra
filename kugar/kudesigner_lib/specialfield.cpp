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
#include "specialfield.h"

#include <klocale.h>

#include <property.h>

namespace Kudesigner
{

SpecialField::SpecialField( int x, int y, int width, int height, Canvas *canvas )
        : Label( x, y, width, height, canvas )
{
    QMap<QString, QVariant> m;

    props.setGroupDescription( "SpecialField", i18n( "Special Field" ) );

    m[ i18n( "Date" ) ] = "0";
    m[ i18n( "PageNumber" ) ] = "1";
    props.addProperty( new Property( "Type", i18n( "Type" ), i18n( "Special Field Type" ), m, "0" ), "SpecialField" );
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
    props.addProperty( new Property( "DateFormat", i18n( "Date Format" ), i18n( "Date Format" ), m, "11" ), "SpecialField" );
}

void SpecialField::draw( QPainter &painter )
{
    props[ "Text" ].setValue( "[" +
                              QString( props[ "Type" ].value().toInt() ? i18n( "PageNo" ) : i18n( "Date" ) )
                              + "]" );
    Label::draw( painter );
}

QString SpecialField::getXml()
{
    return "\t\t<Special" + ReportItem::getXml() + " />\n";
}

}
