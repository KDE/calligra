/***************************************************************************
 * variant.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "variant.h"

#include <kdebug.h>

using namespace Kross::Api;

Variant::Variant(const QVariant& value, const QString& name)
    : Value<Variant, QVariant>(value, name)
{
}

Variant::~Variant()
{
}

const QString Variant::getClassName() const
{
    return "Kross::Api::Variant";
}

const QString Variant::toString()
{
    return getValue().toString();
}

const QString Variant::getVariantType(Object::Ptr object)
{
    QVariant value = toVariant(object);
    switch( value.type() ) {

        case QVariant::CString:
        case QVariant::String:
            return "Kross::Api::Variant::String";

        case QVariant::Map:
            return "Kross::Api::Variant::Dict";

        case QVariant::StringList:
        case QVariant::List:
            return "Kross::Api::Variant::List";

        case QVariant::Double:
            //return "Kross::Api::Variant::Double";
        case QVariant::UInt: 
            //return "Kross::Api::Variant::UInt"; // python isn't able to differ between int and uint :-(
        case QVariant::LongLong:
        case QVariant::ULongLong:
        case QVariant::Int:
            return "Kross::Api::Variant::Integer";

        case QVariant::Bool:
            return "Kross::Api::Variant::Bool";

        default: //Date, Time, DateTime, ByteArray, BitArray, Rect, Size, Color, Invalid, etc.
            return "Kross::Api::Variant";
    }
}

const QVariant& Variant::toVariant(Object::Ptr object)
{
    return Object::fromObject<Variant>( object.data() )->getValue();
}

const QString Variant::toString(Object::Ptr object)
{
    QVariant variant = toVariant(object);
    if(variant.type() != QVariant::String && variant.type() != QVariant::CString)
        throw Exception::Ptr( new Exception(QString("Kross::Api::Variant::String expected, but got %1.").arg(variant.typeName()).latin1()) );
    return variant.toString();
}

uint Variant::toUInt(Object::Ptr object)
{
    QVariant variant = toVariant(object);
    //TODO check for QVariant::UInt ?!
    bool ok;
    uint i = variant.toUInt(&ok);
    if(! ok)
        throw Exception::Ptr( new Exception(QString("Kross::Api::Variant::UInt expected, but got %1.").arg(variant.typeName()).latin1()) );
    return i;
}

Q_LLONG Variant::toLLONG(Object::Ptr object)
{
    QVariant variant = toVariant(object);
    bool ok;
    Q_LLONG l = variant.toLongLong(&ok);
    if(! ok)
        throw Exception::Ptr( new Exception(QString("Kross::Api::Variant::LLONG expected, but got %1.").arg(variant.typeName()).latin1()) );
    return l;
}

Q_ULLONG Variant::toULLONG(Object::Ptr object)
{
    QVariant variant = toVariant(object);
    bool ok;
    Q_ULLONG l = variant.toULongLong(&ok);
    if(! ok)
        throw Exception::Ptr( new Exception(QString("Kross::Api::Variant::ULLONG expected, but got %1.").arg(variant.typeName()).latin1()) );
    return l;
}

bool Variant::toBool(Object::Ptr object)
{
    QVariant variant = toVariant(object);
    if(variant.type() != QVariant::Bool &&
       variant.type() != QVariant::LongLong &&
       variant.type() != QVariant::ULongLong &&
       variant.type() != QVariant::Int &&
       variant.type() != QVariant::UInt
    )
        throw Exception::Ptr( new Exception(QString("Kross::Api::Variant::Bool expected, but got %1.").arg(variant.typeName()).latin1()) );
    return variant.toBool();
}

QValueList<QVariant> Variant::toList(Object::Ptr object)
{
    QVariant variant = toVariant(object);
    if(variant.type() != QVariant::List)
        throw Exception::Ptr( new Exception(QString("Kross::Api::Variant::List expected, but got %1.").arg(variant.typeName()).latin1()) );
    return variant.toList();
}
