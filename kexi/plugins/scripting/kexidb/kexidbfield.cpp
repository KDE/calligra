/***************************************************************************
 * kexidbfield.cpp
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


#include "kexidbfield.h"

#include <api/variant.h>

using namespace Kross::KexiDB;

KexiDBField::KexiDBField(::KexiDB::Field* field)
    : Kross::Api::Class<KexiDBField>("KexiDBField")
    , m_field(field)
{
    addFunction("type", &KexiDBField::type);
    addFunction("setType", &KexiDBField::setType,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("variantType", &KexiDBField::variantType);
    addFunction("setVariantType", &KexiDBField::setVariantType,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("subType", &KexiDBField::subType);
    addFunction("setSubType", &KexiDBField::setSubType,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("typeGroup", &KexiDBField::typeGroup);

    addFunction("isAutoInc", &KexiDBField::isAutoInc);
    addFunction("setAutoInc", &KexiDBField::setAutoInc,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool"));

    addFunction("isUniqueKey", &KexiDBField::isUniqueKey);
    addFunction("setUniqueKey", &KexiDBField::setUniqueKey,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool"));

    addFunction("isPrimaryKey", &KexiDBField::isPrimaryKey);
    addFunction("setPrimaryKey", &KexiDBField::setPrimaryKey,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool"));

    addFunction("isForeignKey", &KexiDBField::isForeignKey);
    addFunction("setForeignKey", &KexiDBField::setForeignKey,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool"));

    addFunction("isNotNull", &KexiDBField::isNotNull);
    addFunction("setNotNull", &KexiDBField::setNotNull,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool"));

    addFunction("isNotEmpty", &KexiDBField::isNotEmpty);
    addFunction("setNotEmpty", &KexiDBField::setNotEmpty,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool"));

    addFunction("isIndexed", &KexiDBField::isIndexed);
    addFunction("setIndexed", &KexiDBField::setIndexed);

    addFunction("isUnsigned", &KexiDBField::isUnsigned);
    addFunction("setUnsigned", &KexiDBField::setUnsigned,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool"));

    addFunction("name", &KexiDBField::name);
    addFunction("setName", &KexiDBField::setName,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("caption", &KexiDBField::caption);
    addFunction("setCaption", &KexiDBField::setCaption,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("description", &KexiDBField::description);
    addFunction("setDescription", &KexiDBField::setDescription,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("length", &KexiDBField::length);
    addFunction("setLength", &KexiDBField::setLength,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::UInt"));

    addFunction("precision", &KexiDBField::precision);
    addFunction("setPrecision", &KexiDBField::setPrecision,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::UInt"));

    addFunction("width", &KexiDBField::width);
    addFunction("setWidth", &KexiDBField::setWidth,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::UInt"));

    addFunction("defaultValue", &KexiDBField::defaultValue);
    addFunction("setDefaultValue", &KexiDBField::setDefaultValue,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant"));
}

KexiDBField::~KexiDBField()
{
}

const QString KexiDBField::getClassName() const
{
    return "Kross::KexiDB::KexiDBField";
}

Kross::Api::Object::Ptr KexiDBField::type(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->typeString());
}

Kross::Api::Object::Ptr KexiDBField::setType(Kross::Api::List::Ptr args)
{
    m_field->setType(
        ::KexiDB::Field::typeForString(Kross::Api::Variant::toString(args->item(0)))
    );
    return type(args);
}

Kross::Api::Object::Ptr KexiDBField::variantType(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->typeString());
}

Kross::Api::Object::Ptr KexiDBField::setVariantType(Kross::Api::List::Ptr args)
{
    m_field->setType(
        ::KexiDB::Field::typeForString(Kross::Api::Variant::toString(args->item(0)))
    );
    return type(args);
}

Kross::Api::Object::Ptr KexiDBField::subType(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->subType());
}

Kross::Api::Object::Ptr KexiDBField::setSubType(Kross::Api::List::Ptr args)
{
    m_field->setSubType(Kross::Api::Variant::toString(args->item(0)));
    return subType(args);
}

Kross::Api::Object::Ptr KexiDBField::typeGroup(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->typeGroupString());
}

Kross::Api::Object::Ptr KexiDBField::isAutoInc(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(QVariant(m_field->isAutoIncrement(),0));
}

Kross::Api::Object::Ptr KexiDBField::setAutoInc(Kross::Api::List::Ptr args)
{
    m_field->setAutoIncrement(Kross::Api::Variant::toBool(args->item(0)));
    return isAutoInc(args);
}

Kross::Api::Object::Ptr KexiDBField::isUniqueKey(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(QVariant(m_field->isUniqueKey(),0));
}

Kross::Api::Object::Ptr KexiDBField::setUniqueKey(Kross::Api::List::Ptr args)
{
    m_field->setUniqueKey(Kross::Api::Variant::toBool(args->item(0)));
    return isUniqueKey(args);
}

Kross::Api::Object::Ptr KexiDBField::isPrimaryKey(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(QVariant(m_field->isPrimaryKey(),0));
}

Kross::Api::Object::Ptr KexiDBField::setPrimaryKey(Kross::Api::List::Ptr args)
{
    m_field->setPrimaryKey(Kross::Api::Variant::toBool(args->item(0)));
    return isPrimaryKey(args);
}

Kross::Api::Object::Ptr KexiDBField::isForeignKey(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(QVariant(m_field->isForeignKey(),0));
}

Kross::Api::Object::Ptr KexiDBField::setForeignKey(Kross::Api::List::Ptr args)
{
    m_field->setForeignKey(Kross::Api::Variant::toBool(args->item(0)));
    return isForeignKey(args);
}

Kross::Api::Object::Ptr KexiDBField::isNotNull(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(QVariant(m_field->isNotNull(),0));
}

Kross::Api::Object::Ptr KexiDBField::setNotNull(Kross::Api::List::Ptr args)
{
    m_field->setNotNull(Kross::Api::Variant::toBool(args->item(0)));
    return isNotNull(args);
}

Kross::Api::Object::Ptr KexiDBField::isNotEmpty(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(QVariant(m_field->isNotEmpty(),0));
}

Kross::Api::Object::Ptr KexiDBField::setNotEmpty(Kross::Api::List::Ptr args)
{
    m_field->setNotEmpty(Kross::Api::Variant::toBool(args->item(0)));
    return isNotEmpty(args);
}

Kross::Api::Object::Ptr KexiDBField::isIndexed(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(QVariant(m_field->isIndexed(),0));
}

Kross::Api::Object::Ptr KexiDBField::setIndexed(Kross::Api::List::Ptr args)
{
    m_field->setIndexed(Kross::Api::Variant::toBool(args->item(0)));
    return isIndexed(args);
}

Kross::Api::Object::Ptr KexiDBField::isUnsigned(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(QVariant(m_field->isUnsigned(),0));
}

Kross::Api::Object::Ptr KexiDBField::setUnsigned(Kross::Api::List::Ptr args)
{
    m_field->setUnsigned(Kross::Api::Variant::toBool(args->item(0)));
    return isUnsigned(args);
}

Kross::Api::Object::Ptr KexiDBField::name(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->name());
}

Kross::Api::Object::Ptr KexiDBField::setName(Kross::Api::List::Ptr args)
{
    m_field->setName(Kross::Api::Variant::toString(args->item(0)));
    return name(args);
}

Kross::Api::Object::Ptr KexiDBField::caption(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->caption());
}

Kross::Api::Object::Ptr KexiDBField::setCaption(Kross::Api::List::Ptr args)
{
    m_field->setCaption(Kross::Api::Variant::toString(args->item(0)));
    return caption(args);
}

Kross::Api::Object::Ptr KexiDBField::description(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->description());
}

Kross::Api::Object::Ptr KexiDBField::setDescription(Kross::Api::List::Ptr args)
{
    m_field->setDescription(Kross::Api::Variant::toString(args->item(0)));
    return description(args);
}

Kross::Api::Object::Ptr KexiDBField::length(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->length());
}

Kross::Api::Object::Ptr KexiDBField::setLength(Kross::Api::List::Ptr args)
{
    m_field->setLength(Kross::Api::Variant::toUInt(args->item(0)));
    return length(args);
}

Kross::Api::Object::Ptr KexiDBField::precision(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->precision());
}

Kross::Api::Object::Ptr KexiDBField::setPrecision(Kross::Api::List::Ptr args)
{
    m_field->setPrecision(Kross::Api::Variant::toUInt(args->item(0)));
    return precision(args);
}

Kross::Api::Object::Ptr KexiDBField::width(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->width());
}

Kross::Api::Object::Ptr KexiDBField::setWidth(Kross::Api::List::Ptr args)
{
    m_field->setWidth(Kross::Api::Variant::toUInt(args->item(0)));
    return width(args);
}

Kross::Api::Object::Ptr KexiDBField::defaultValue(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->defaultValue());
}

Kross::Api::Object::Ptr KexiDBField::setDefaultValue(Kross::Api::List::Ptr args)
{
    m_field->setDefaultValue(Kross::Api::Variant::toVariant(args->item(0)));
    return defaultValue(args);
}

