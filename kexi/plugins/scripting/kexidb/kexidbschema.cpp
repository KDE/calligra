/***************************************************************************
 * kexidbschema.cpp
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


#include "kexidbschema.h"
#include "kexidbfieldlist.h"

#include <api/variant.h>

using namespace Kross::KexiDB;

/***************************************************************************
 *KexiDBSchema
 */

template<class T>
KexiDBSchema<T>::KexiDBSchema(const QString& name, ::KexiDB::SchemaData* schema, ::KexiDB::FieldList* fieldlist)
    : Kross::Api::Class<T>(name)
    , m_schema(schema)
    , m_fieldlist(fieldlist)
{
    addFunction("name", &KexiDBSchema<T>::name);
    addFunction("setName", &KexiDBSchema<T>::setName,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("caption", &KexiDBSchema<T>::caption);
    addFunction("setCaption", &KexiDBSchema<T>::setCaption,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("description", &KexiDBSchema<T>::description);
    addFunction("setDescription", &KexiDBSchema<T>::setDescription,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("fieldlist", &KexiDBSchema<T>::fieldlist);
}

template<class T>
KexiDBSchema<T>::~KexiDBSchema<T>()
{
}

template<class T>
Kross::Api::Object::Ptr KexiDBSchema<T>::name(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_schema->name());
}

template<class T>
Kross::Api::Object::Ptr KexiDBSchema<T>::setName(Kross::Api::List::Ptr args)
{
    m_schema->setName(Kross::Api::Variant::toString(args->item(0)));
    return name(args);
}

template<class T>
Kross::Api::Object::Ptr KexiDBSchema<T>::caption(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_schema->caption());
}

template<class T>
Kross::Api::Object::Ptr KexiDBSchema<T>::setCaption(Kross::Api::List::Ptr args)
{
    m_schema->setCaption(Kross::Api::Variant::toString(args->item(0)));
    return caption(args);
}

template<class T>
Kross::Api::Object::Ptr KexiDBSchema<T>::description(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_schema->description());
}

template<class T>
Kross::Api::Object::Ptr KexiDBSchema<T>::setDescription(Kross::Api::List::Ptr args)
{
    m_schema->setDescription(Kross::Api::Variant::toString(args->item(0)));
    return description(args);
}

template<class T>
Kross::Api::Object::Ptr KexiDBSchema<T>::fieldlist(Kross::Api::List::Ptr)
{
    //TODO cache; pass optional KexiDBFieldList* to our class and return here.
    return new KexiDBFieldList(m_fieldlist);
}

/***************************************************************************
 * KexiDBTableSchema
 */

KexiDBTableSchema::KexiDBTableSchema(::KexiDB::TableSchema* tableschema)
    : KexiDBSchema<KexiDBTableSchema>("KexiDBTableSchema", tableschema, tableschema)
{
    addFunction("query", &KexiDBTableSchema::query);
}

KexiDBTableSchema::~KexiDBTableSchema()
{
}

const QString KexiDBTableSchema::getClassName() const
{
    return "Kross::KexiDB::KexiDBTableSchema";
}

::KexiDB::TableSchema* KexiDBTableSchema::tableschema()
{
    return static_cast< ::KexiDB::TableSchema* >(m_schema);
}

Kross::Api::Object::Ptr KexiDBTableSchema::query(Kross::Api::List::Ptr)
{
    return new KexiDBQuerySchema( tableschema()->query() );
}

/***************************************************************************
 * KexiDBQuerySchema
 */

KexiDBQuerySchema::KexiDBQuerySchema(::KexiDB::QuerySchema* queryschema)
    : KexiDBSchema<KexiDBQuerySchema>("KexiDBQuerySchema", queryschema, queryschema)
{
    addFunction("statement", &KexiDBQuerySchema::statement);
    addFunction("setStatement", &KexiDBQuerySchema::setStatement,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));
}

KexiDBQuerySchema::~KexiDBQuerySchema()
{
}

const QString KexiDBQuerySchema::getClassName() const
{
    return "Kross::KexiDB::KexiDBQuerySchema";
}

::KexiDB::QuerySchema* KexiDBQuerySchema::queryschema()
{
    return static_cast< ::KexiDB::QuerySchema* >(m_schema);
}

Kross::Api::Object::Ptr KexiDBQuerySchema::statement(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(
           static_cast< ::KexiDB::QuerySchema* >(m_schema)->statement() );
}

Kross::Api::Object::Ptr KexiDBQuerySchema::setStatement(Kross::Api::List::Ptr args)
{
    static_cast< ::KexiDB::QuerySchema* >(m_schema)->setStatement(
        Kross::Api::Variant::toString(args->item(0))
    );
    return statement(args);
}

