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
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#include "propertyserializer.h"

#include <qcolor.h>

namespace Kudesigner
{

PropertySerializer::PropertySerializer()
{}

PropertySerializer::~PropertySerializer()
{}

QString PropertySerializer::toString( Property *prop )
{
    QVariant val = prop->value();
    switch ( prop->type() )
    {
    case KOProperty::Color:
        return QString( "%1,%2,%3" ).arg( val.toColor().red() ).arg( val.toColor().green() ).arg( val.toColor().blue() );
    default:
        return val.toString();
    }
}

QVariant PropertySerializer::fromString( Property *prop, const QString &str )
{
    switch ( prop->type() )
    {
    case KOProperty::Color:
        return QVariant( QColor( str.section( ',', 0, 0 ).toInt(),
                                 str.section( ',', 1, 1 ).toInt(),
                                 str.section( ',', 2, 2 ).toInt() ) );
    default:
        return QVariant( str );
    }
}

}
