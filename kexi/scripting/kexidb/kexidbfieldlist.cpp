/***************************************************************************
 * kexidbfieldlist.cpp
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

#include "kexidbfieldlist.h"
#include "kexidbfield.h"

#include "../api/variant.h"
#include "../api/exception.h"

//#include <qvaluelist.h>
//#include <klocale.h>
//#include <kdebug.h>

using namespace Kross::KexiDB;

KexiDBFieldList::KexiDBFieldList(::KexiDB::FieldList* fieldlist)
    : Kross::Api::Class<KexiDBFieldList>("KexiDBFieldList")
    , m_fieldlist(fieldlist)
{
    addFunction("fieldCount", &KexiDBFieldList::fieldCount,
        Kross::Api::ArgumentList(),
        i18n("Return the number of fields.")
    );
    addFunction("field", &KexiDBFieldList::field,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant"),
        i18n("Return the field with the as argument passed number.")
    );
    addFunction("fields", &KexiDBFieldList::fields,
        Kross::Api::ArgumentList(),
        i18n("Return a list of fields.")
    );
    addFunction("hasField", &KexiDBFieldList::hasField,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBField"),
        i18n("Return true if the as argument passed KexiDBField object is in the fieldlist.")
    );
    addFunction("names", &KexiDBFieldList::names,
        Kross::Api::ArgumentList(),
        i18n("Return a stringlist of fieldnames.")
    );
    addFunction("addField", &KexiDBFieldList::addField,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBField"),
        i18n("Add the as argument passed KexiDBField object to the fieldlist.")
    );
    addFunction("insertField", &KexiDBFieldList::insertField,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant::UInt")
            << Kross::Api::Argument("Kross::KexiDB::KexiDBField"),
        i18n("Insert the as second argument passed KexiDBField object "
             "into the fieldlist at the position defined by the as second "
             "argument passed number.")
    );
    addFunction("removeField", &KexiDBFieldList::removeField,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBField"),
        i18n("Remove the as argument passed KexiDBField object from the fieldlist.")
    );
    addFunction("clear", &KexiDBFieldList::clear,
        Kross::Api::ArgumentList(),
        i18n("Remove all KexiDBField objects from the fieldlist.")
    );
}

KexiDBFieldList::~KexiDBFieldList()
{
}

const QString KexiDBFieldList::getClassName() const
{
    return "Kross::KexiDB::KexiDBFieldList";
}

const QString KexiDBFieldList::getDescription() const
{
    return i18n("KexiDB::FieldList wrapper to handle a list of KexiDBField objects.");
}

Kross::Api::Object* KexiDBFieldList::fieldCount(Kross::Api::List*)
{
    return Kross::Api::Variant::create(m_fieldlist->fieldCount(),
           "Kross::KexiDB::KexiDBFieldList::fieldCount::UInt");
}

Kross::Api::Object* KexiDBFieldList::field(Kross::Api::List* args)
{
    QVariant variant = Kross::Api::Variant::toVariant(args->item(0));
    bool ok;
    uint idx = variant.toUInt(&ok);
    return new KexiDBField(
        ok ? m_fieldlist->field(idx) : m_fieldlist->field(variant.toString())
    );
}

Kross::Api::Object* KexiDBFieldList::fields(Kross::Api::List*)
{
    QValueList<Object*> list;
    ::KexiDB::Field::ListIterator it( *m_fieldlist->fields() );
    for(; it.current(); ++it)
        list.append( new KexiDBField(it.current()) );
    return Kross::Api::List::create(list,
           "kross::KexiDB::KexiDBFieldList::fields::List");
}

Kross::Api::Object* KexiDBFieldList::hasField(Kross::Api::List* args)
{
    return Kross::Api::Variant::create(
           m_fieldlist->hasField(
               Kross::Api::Object::fromObject<KexiDBField>(args->item(0))->field()
           ),
           "Kross::KexiDB::KexiDBFieldList::hasField::Bool");
}

Kross::Api::Object* KexiDBFieldList::names(Kross::Api::List*)
{
    return Kross::Api::Variant::create(m_fieldlist->names(),
           "Kross::KexiDB::KexiDBFieldList::fieldCount::StringList");
}

Kross::Api::Object* KexiDBFieldList::addField(Kross::Api::List* args)
{
    m_fieldlist->addField(
        Kross::Api::Object::fromObject<KexiDBField>(args->item(0))->field()
    );
    return Kross::Api::Variant::create(true,
           "Kross::KexiDB::KexiDBFieldList::addField::Bool");
}

Kross::Api::Object* KexiDBFieldList::insertField(Kross::Api::List* args)
{
    m_fieldlist->insertField(
        Kross::Api::Variant::toUInt(args->item(0)),
        Kross::Api::Object::fromObject<KexiDBField>(args->item(1))->field()
    );
    return Kross::Api::Variant::create(true,
           "Kross::KexiDB::KexiDBFieldList::insertField::Bool");
}

Kross::Api::Object* KexiDBFieldList::removeField(Kross::Api::List* args)
{
    m_fieldlist->removeField(
        Kross::Api::Object::fromObject<KexiDBField>(args->item(0))->field()
    );
    return Kross::Api::Variant::create(true,
           "Kross::KexiDB::KexiDBFieldList::removeField::Bool");
}

Kross::Api::Object* KexiDBFieldList::clear(Kross::Api::List*)
{
    m_fieldlist->clear();
    return Kross::Api::Variant::create(true,
           "Kross::KexiDB::KexiDBFieldList::clear::Bool");
}

