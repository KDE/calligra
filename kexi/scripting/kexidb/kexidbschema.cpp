/***************************************************************************
 * kexidbschema.cpp
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


#include "kexidbschema.h"
#include "kexidbfieldlist.h"
//#include "kexidbconnection.h"
//#include "kexidbdriver.h"

#include "../api/variant.h"
//#include "../api/exception.h"

//#include <qvaluelist.h>
//#include <klocale.h>
//#include <kdebug.h>

using namespace Kross::KexiDB;

/***************************************************************************
 *KexiDBSchema
 */

template<class T>
KexiDBSchema<T>::KexiDBSchema<T>(const QString& name, ::KexiDB::SchemaData* schema, ::KexiDB::FieldList* fieldlist)
    : Kross::Api::Class<T>(name)
    , m_schema(schema)
    , m_fieldlist(fieldlist)
{
    addFunction("name", &KexiDBSchema<T>::name,
        Kross::Api::ArgumentList(),
        ""
    );
    addFunction("setName", &KexiDBSchema<T>::setName,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        ""
    );
    addFunction("caption", &KexiDBSchema<T>::caption,
        Kross::Api::ArgumentList(),
        ""
    );
    addFunction("setCaption", &KexiDBSchema<T>::setCaption,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        ""
    );
    addFunction("description", &KexiDBSchema<T>::description,
        Kross::Api::ArgumentList(),
        ""
    );
    addFunction("setDescription", &KexiDBSchema<T>::setDescription,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        ""
    );
    addFunction("fieldlist", &KexiDBSchema<T>::fieldlist,
        Kross::Api::ArgumentList(),
        ""
    );
}

template<class T>
KexiDBSchema<T>::~KexiDBSchema<T>()
{
}

template<class T>
Kross::Api::Object* KexiDBSchema<T>::name(Kross::Api::List*)
{
    return Kross::Api::Variant::create(m_schema->name(),
           "Kross::KexiDB::KexiDBSchema::name::String");
}

template<class T>
Kross::Api::Object* KexiDBSchema<T>::setName(Kross::Api::List* args)
{
    m_schema->setName(Kross::Api::Variant::toString(args->item(0)));
    return name(args);
}

template<class T>
Kross::Api::Object* KexiDBSchema<T>::caption(Kross::Api::List*)
{
    return Kross::Api::Variant::create(m_schema->caption(),
           "Kross::KexiDB::KexiDBSchema::caption::String");
}

template<class T>
Kross::Api::Object* KexiDBSchema<T>::setCaption(Kross::Api::List* args)
{
    m_schema->setCaption(Kross::Api::Variant::toString(args->item(0)));
    return caption(args);
}

template<class T>
Kross::Api::Object* KexiDBSchema<T>::description(Kross::Api::List*)
{
    return Kross::Api::Variant::create(m_schema->description(),
           "Kross::KexiDB::KexiDBSchema::description::String");
}

template<class T>
Kross::Api::Object* KexiDBSchema<T>::setDescription(Kross::Api::List* args)
{
    m_schema->setDescription(Kross::Api::Variant::toString(args->item(0)));
    return description(args);
}

template<class T>
Kross::Api::Object* KexiDBSchema<T>::fieldlist(Kross::Api::List*)
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
}

KexiDBTableSchema::~KexiDBTableSchema()
{
}

const QString KexiDBTableSchema::getClassName() const
{
    return "Kross::KexiDB::KexiDBTableSchema";
}

const QString KexiDBTableSchema::getDescription() const
{
    return "";
}

::KexiDB::TableSchema* KexiDBTableSchema::tableschema()
{
    return static_cast< ::KexiDB::TableSchema* >(m_schema);
}

/***************************************************************************
 * KexiDBQuerySchema
 */

KexiDBQuerySchema::KexiDBQuerySchema(::KexiDB::QuerySchema* queryschema)
    : KexiDBSchema<KexiDBQuerySchema>("KexiDBQuerySchema", queryschema, queryschema)
{
    addFunction("statement", &KexiDBQuerySchema::statement,
        Kross::Api::ArgumentList(),
        ""
    );
    addFunction("setStatement", &KexiDBQuerySchema::setStatement,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        ""
    );
}

KexiDBQuerySchema::~KexiDBQuerySchema()
{
}

const QString KexiDBQuerySchema::getClassName() const
{
    return "Kross::KexiDB::KexiDBQuerySchema";
}

const QString KexiDBQuerySchema::getDescription() const
{
    return "";
}

::KexiDB::QuerySchema* KexiDBQuerySchema::queryschema()
{
    return static_cast< ::KexiDB::QuerySchema* >(m_schema);
}

Kross::Api::Object* KexiDBQuerySchema::statement(Kross::Api::List*)
{
    return Kross::Api::Variant::create(
           static_cast< ::KexiDB::QuerySchema* >(m_schema)->statement(),
           "Kross::KexiDB::KexiDBQuerySchema::statement::String");
}

Kross::Api::Object* KexiDBQuerySchema::setStatement(Kross::Api::List* args)
{
    static_cast< ::KexiDB::QuerySchema* >(m_schema)->setStatement(
        Kross::Api::Variant::toString(args->item(0))
    );
    return statement(args);
}

