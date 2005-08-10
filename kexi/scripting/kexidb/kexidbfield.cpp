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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/


#include "kexidbfield.h"
//#include "kexidbcursor.h"
//#include "kexidbconnection.h"
//#include "kexidbdriver.h"
#include "../api/variant.h"
//#include "../api/exception.h"

//#include <qvaluelist.h>
//#include <kdebug.h>

using namespace Kross::KexiDB;

KexiDBField::KexiDBField(::KexiDB::Field* field)
    : Kross::Api::Class<KexiDBField>("KexiDBField")
    , m_field(field)
{
    addFunction("type", &KexiDBField::type,
        Kross::Api::ArgumentList(),
        QString("Returns the type string for this field, e.g. \"Integer\" for "
             "Integer type.")
    );
    addFunction("setType", &KexiDBField::setType,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        QString("Sets the type string for this field, e.g. \"Integer\" for "
             "Integer type.")
    );
    addFunction("subType", &KexiDBField::subType,
        Kross::Api::ArgumentList(),
        QString("Returns the optional subtype for this field. Subtype is a "
             "string providing additional hint for field's type. E.g. for "
             "BLOB type, it can be a MIME type or certain QVariant type "
             "name, for example: \"QPixmap\", \"QColor\" or \"QFont\".")
    );
    addFunction("setSubType", &KexiDBField::setSubType,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        QString("Sets the optional subtype for this field.")
    );
    addFunction("typeGroup", &KexiDBField::typeGroup,
        Kross::Api::ArgumentList(),
        QString("Returns type group string for this field, e.g. "
             "\"IntegerGroup\" for IntegerGroup type.")
    );

    addFunction("isAutoInc", &KexiDBField::isAutoInc,
        Kross::Api::ArgumentList(),
        QString("Returns true if the field is autoincrement (e.g. integer/numeric).")
    );
    addFunction("setAutoInc", &KexiDBField::setAutoInc,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool"),
        QString("Sets auto increment flag.")
    );
    addFunction("isUniqueKey", &KexiDBField::isUniqueKey,
        Kross::Api::ArgumentList(),
        QString("Returns true if the field is member of single-field unique key.")
    );
    addFunction("setUniqueKey", &KexiDBField::setUniqueKey,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool"),
        QString("Specifies whether the field has single-field unique constraint or not.")
    );
    addFunction("isPrimaryKey", &KexiDBField::isPrimaryKey,
        Kross::Api::ArgumentList(),
        QString("Returns true if the field is member of single-field primary key.")
    );
    addFunction("setPrimaryKey", &KexiDBField::setPrimaryKey,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool"),
        QString("Specifies whether the field is single-field primary key or not.")
    );
    addFunction("isForeignKey", &KexiDBField::isForeignKey,
        Kross::Api::ArgumentList(),
        QString("Returns true if the field is member of single-field foreign key.")
    );
    addFunction("setForeignKey", &KexiDBField::setForeignKey,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool"),
        QString("Sets whether the field has to be declared with single-field foreign key.")
    );
    addFunction("isNotNull", &KexiDBField::isNotNull,
        Kross::Api::ArgumentList(),
        QString("Returns true if the field is not allowed to be null.")
    );
    addFunction("setNotNull", &KexiDBField::setNotNull,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool"),
        QString("Specifies whether the field has single-field unique constraint or not.")
    );
    addFunction("isNotEmpty", &KexiDBField::isNotEmpty,
        Kross::Api::ArgumentList(),
        QString("Returns true if the field is not allowed to be null.")
    );
    addFunction("setNotEmpty", &KexiDBField::setNotEmpty,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool"),
        QString("Specifies whether the field has single-field unique constraint or not.")
    );
    addFunction("isIndexed", &KexiDBField::isIndexed,
        Kross::Api::ArgumentList(),
        QString("Returns true if the field is indexed using single-field database index.")
    );
    addFunction("setIndexed", &KexiDBField::setIndexed,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool"),
        QString("Specifies whether the field is indexed or not.")
    );

    addFunction("name", &KexiDBField::name,
        Kross::Api::ArgumentList(),
        QString("Returns the name of this field.")
    );
    addFunction("setName", &KexiDBField::setName,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        QString("Sets the name of this field.")
    );
    addFunction("caption", &KexiDBField::caption,
        Kross::Api::ArgumentList(),
        QString("Returns the caption of this field.")
    );
    addFunction("setCaption", &KexiDBField::setCaption,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        QString("Sets the caption of this field.")
    );
    addFunction("description", &KexiDBField::description,
        Kross::Api::ArgumentList(),
        QString("Returns the descriptive text for this field.")
    );
    addFunction("setDescription", &KexiDBField::setDescription,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        QString("Set the description for this field.")
    );
    addFunction("length", &KexiDBField::length,
        Kross::Api::ArgumentList(),
        QString("Returns the length of text if the field type is text.")
    );
    addFunction("setLength", &KexiDBField::setLength,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::UInt"),
        QString("Sets the length for this field. Only works for Text Type (not including LongText).")
    );
    addFunction("precision", &KexiDBField::precision,
        Kross::Api::ArgumentList(),
        QString("Returns precision for numeric and other fields that have "
             "both length and precision (floating point types).")
    );
    addFunction("setPrecision", &KexiDBField::setPrecision,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::UInt"),
        QString("Sets the precision for numeric and other fields.")
    );
    addFunction("width", &KexiDBField::width,
        Kross::Api::ArgumentList(),
        QString("Returns the width of this field (usually in pixels or points). "
             "0 (the default) means there is no hint for the width.")
    );
    addFunction("setWidth", &KexiDBField::setWidth,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::UInt"),
        QString("Sets the width of this field.")
    );
    addFunction("defaultValue", &KexiDBField::defaultValue,
        Kross::Api::ArgumentList(),
        QString("Returns the default value this field has.")
    );
    addFunction("setDefaultValue", &KexiDBField::setDefaultValue,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant"),
        QString("Sets the default value this field has.")
    );
}

KexiDBField::~KexiDBField()
{
}

const QString KexiDBField::getClassName() const
{
    return "Kross::KexiDB::KexiDBField";
}

const QString KexiDBField::getDescription() const
{
    return QString("KexiDB::Field wrapper to work with Meta-data for a field.");
}

Kross::Api::Object::Ptr KexiDBField::type(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->typeString(),
           "Kross::KexiDB::KexiDBField::type::String");
}

Kross::Api::Object::Ptr KexiDBField::setType(Kross::Api::List::Ptr args)
{
    m_field->setType(
        ::KexiDB::Field::typeForString(
            Kross::Api::Variant::toString(args->item(0))
        )
    );
    return type(args);
}

Kross::Api::Object::Ptr KexiDBField::subType(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->subType(),
           "Kross::KexiDB::KexiDBField::subType::String");
}

Kross::Api::Object::Ptr KexiDBField::setSubType(Kross::Api::List::Ptr args)
{
    m_field->setSubType(Kross::Api::Variant::toString(args->item(0)));
    return subType(args);
}

Kross::Api::Object::Ptr KexiDBField::typeGroup(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->typeGroupString(),
           "Kross::KexiDB::KexiDBField::typeGroup::String");
}

Kross::Api::Object::Ptr KexiDBField::isAutoInc(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->isAutoIncrement(),
           "Kross::KexiDB::KexiDBField::isAutoInc::Bool");
}

Kross::Api::Object::Ptr KexiDBField::setAutoInc(Kross::Api::List::Ptr args)
{
    m_field->setAutoIncrement(Kross::Api::Variant::toBool(args->item(0)));
    return isAutoInc(args);
}

Kross::Api::Object::Ptr KexiDBField::isUniqueKey(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->isUniqueKey(),
           "Kross::KexiDB::KexiDBField::isUniqueKey::Bool");
}

Kross::Api::Object::Ptr KexiDBField::setUniqueKey(Kross::Api::List::Ptr args)
{
    m_field->setUniqueKey(Kross::Api::Variant::toBool(args->item(0)));
    return isUniqueKey(args);
}

Kross::Api::Object::Ptr KexiDBField::isPrimaryKey(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->isPrimaryKey(),
           "Kross::KexiDB::KexiDBField::isPrimaryKey::Bool");
}

Kross::Api::Object::Ptr KexiDBField::setPrimaryKey(Kross::Api::List::Ptr args)
{
    m_field->setPrimaryKey(Kross::Api::Variant::toBool(args->item(0)));
    return isPrimaryKey(args);
}

Kross::Api::Object::Ptr KexiDBField::isForeignKey(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->isForeignKey(),
           "Kross::KexiDB::KexiDBField::isForeignKey::Bool");
}

Kross::Api::Object::Ptr KexiDBField::setForeignKey(Kross::Api::List::Ptr args)
{
    m_field->setForeignKey(Kross::Api::Variant::toBool(args->item(0)));
    return isForeignKey(args);
}

Kross::Api::Object::Ptr KexiDBField::isNotNull(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->isNotNull(),
           "Kross::KexiDB::KexiDBField::isNotNull::Bool");
}

Kross::Api::Object::Ptr KexiDBField::setNotNull(Kross::Api::List::Ptr args)
{
    m_field->setNotNull(Kross::Api::Variant::toBool(args->item(0)));
    return isNotNull(args);
}

Kross::Api::Object::Ptr KexiDBField::isNotEmpty(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->isNotEmpty(),
           "Kross::KexiDB::KexiDBField::isNotEmpty::Bool");
}

Kross::Api::Object::Ptr KexiDBField::setNotEmpty(Kross::Api::List::Ptr args)
{
    m_field->setNotEmpty(Kross::Api::Variant::toBool(args->item(0)));
    return isNotEmpty(args);
}

Kross::Api::Object::Ptr KexiDBField::isIndexed(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->isIndexed(),
           "Kross::KexiDB::KexiDBField::isIndexed::Bool");
}

Kross::Api::Object::Ptr KexiDBField::setIndexed(Kross::Api::List::Ptr args)
{
    m_field->setIndexed(Kross::Api::Variant::toBool(args->item(0)));
    return isIndexed(args);
}

Kross::Api::Object::Ptr KexiDBField::isUnsigned(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->isUnsigned(),
           "Kross::KexiDB::KexiDBField::isUnsigned::Bool");
}

Kross::Api::Object::Ptr KexiDBField::setUnsigned(Kross::Api::List::Ptr args)
{
    m_field->setUnsigned(Kross::Api::Variant::toBool(args->item(0)));
    return isUnsigned(args);
}

Kross::Api::Object::Ptr KexiDBField::name(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->name(),
           "Kross::KexiDB::KexiDBField::name::String");
}

Kross::Api::Object::Ptr KexiDBField::setName(Kross::Api::List::Ptr args)
{
    m_field->setName(Kross::Api::Variant::toString(args->item(0)));
    return name(args);
}

Kross::Api::Object::Ptr KexiDBField::caption(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->caption(),
           "Kross::KexiDB::KexiDBField::caption::String");
}

Kross::Api::Object::Ptr KexiDBField::setCaption(Kross::Api::List::Ptr args)
{
    m_field->setCaption(Kross::Api::Variant::toString(args->item(0)));
    return caption(args);
}

Kross::Api::Object::Ptr KexiDBField::description(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->description(),
           "Kross::KexiDB::KexiDBField::description::String");
}

Kross::Api::Object::Ptr KexiDBField::setDescription(Kross::Api::List::Ptr args)
{
    m_field->setDescription(Kross::Api::Variant::toString(args->item(0)));
    return description(args);
}

Kross::Api::Object::Ptr KexiDBField::length(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->length(),
           "Kross::KexiDB::KexiDBField::length::UInt");
}

Kross::Api::Object::Ptr KexiDBField::setLength(Kross::Api::List::Ptr args)
{
    m_field->setLength(Kross::Api::Variant::toUInt(args->item(0)));
    return length(args);
}

Kross::Api::Object::Ptr KexiDBField::precision(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->precision(),
           "Kross::KexiDB::KexiDBField::precision::UInt");
}

Kross::Api::Object::Ptr KexiDBField::setPrecision(Kross::Api::List::Ptr args)
{
    m_field->setPrecision(Kross::Api::Variant::toUInt(args->item(0)));
    return precision(args);
}

Kross::Api::Object::Ptr KexiDBField::width(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->width(),
           "Kross::KexiDB::KexiDBField::width::UInt");
}

Kross::Api::Object::Ptr KexiDBField::setWidth(Kross::Api::List::Ptr args)
{
    m_field->setWidth(Kross::Api::Variant::toUInt(args->item(0)));
    return width(args);
}

Kross::Api::Object::Ptr KexiDBField::defaultValue(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_field->defaultValue(),
           "Kross::KexiDB::KexiDBField::defaultValue::Variant");
}

Kross::Api::Object::Ptr KexiDBField::setDefaultValue(Kross::Api::List::Ptr args)
{
    m_field->setDefaultValue(Kross::Api::Variant::toVariant(args->item(0)));
    return defaultValue(args);
}

