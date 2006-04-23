/***************************************************************************
*   Copyright (C) 2004 by Alexander Dymo                                  *
*   cloudtemple@mskat.net                                                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU Library General Public License as       *
*   published by the Free Software Foundation; either version 2 of the    *
*   License, or (at your option) any later version.                       *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU Library General Public     *
*   License along with this program; if not, write to the                 *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.             *
***************************************************************************/
#include "propertyserializer.h"

#include <qcolor.h>
#include <qfont.h>

#include <kdebug.h>

namespace Kudesigner
{

PropertySerializer::PropertySerializer()
{}

PropertySerializer::~PropertySerializer()
{}

QString PropertySerializer::toString( Property *prop )
{
    QVariant val = prop->value();
	QColor valColor = val.value<QColor>();
    switch ( prop->type() )
    {
    case KoProperty::Color:
        return QString( "%1,%2,%3" ).arg( valColor.red() ).arg( valColor.green() ).arg( valColor.blue() );
    case KoProperty::Boolean:
        return val.toBool() ? "true" : "false";
    case KoProperty::Font:
        return val.value<QFont>().family();
    default:
        return val.toString();
    }
}

QVariant PropertySerializer::fromString( Property *prop, const QString &str )
{
    switch ( prop->type() )
    {
    case KoProperty::Color:
        return QVariant( QColor( str.section( ',', 0, 0 ).toInt(),
                                 str.section( ',', 1, 1 ).toInt(),
                                 str.section( ',', 2, 2 ).toInt() ) );
    case KoProperty::Integer:
        return QVariant( str.toInt() );
    case KoProperty::Boolean:
        return QVariant( str == "true", 3 );
    case KoProperty::Font:
        return QVariant( QFont( str ) );
    case KoProperty::LineStyle:
        return QVariant( str.toInt() );
    case KoProperty::Symbol:
        return QVariant( str.at( 0 ).latin1() );
    default:
        return QVariant( str );
    }
}

}
