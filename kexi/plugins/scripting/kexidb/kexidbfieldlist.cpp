/***************************************************************************
 * kexidbfieldlist.cpp
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

#include "kexidbfieldlist.h"
#include "kexidbfield.h"

#include <api/variant.h>
#include <api/exception.h>

#include <kdebug.h>

using namespace Kross::KexiDB;

KexiDBFieldList::KexiDBFieldList(::KexiDB::FieldList* fieldlist)
    : Kross::Api::Class<KexiDBFieldList>("KexiDBFieldList")
    , m_fieldlist(fieldlist)
{
    addFunction("fieldCount", &KexiDBFieldList::fieldCount);
    addFunction("field", &KexiDBFieldList::field,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant"));
    addFunction("fields", &KexiDBFieldList::fields);
    addFunction("hasField", &KexiDBFieldList::hasField,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBField"));
    addFunction("names", &KexiDBFieldList::names);

    addFunction("addField", &KexiDBFieldList::addField,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBField"));
    addFunction("insertField", &KexiDBFieldList::insertField,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant::UInt")
            << Kross::Api::Argument("Kross::KexiDB::KexiDBField"));
    addFunction("removeField", &KexiDBFieldList::removeField,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBField"));
    addFunction("clear", &KexiDBFieldList::clear);
    addFunction("setFields", &KexiDBFieldList::setFields);

    addFunction("subList", &KexiDBFieldList::subList);
}

KexiDBFieldList::~KexiDBFieldList()
{
}

const QString KexiDBFieldList::getClassName() const
{
    return "Kross::KexiDB::KexiDBFieldList";
}

Kross::Api::Object::Ptr KexiDBFieldList::fieldCount(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_fieldlist->fieldCount());
}

Kross::Api::Object::Ptr KexiDBFieldList::field(Kross::Api::List::Ptr args)
{
    QVariant variant = Kross::Api::Variant::toVariant(args->item(0));
    bool ok;
    uint idx = variant.toUInt(&ok);
    return new KexiDBField(
        ok ? m_fieldlist->field(idx) : m_fieldlist->field(variant.toString())
    );
}

Kross::Api::Object::Ptr KexiDBFieldList::fields(Kross::Api::List::Ptr)
{
    QValueList<Object::Ptr> list;
    ::KexiDB::Field::ListIterator it( *m_fieldlist->fields() );
    for(; it.current(); ++it)
        list.append( new KexiDBField(it.current()) );
    return new Kross::Api::List(list);
}

Kross::Api::Object::Ptr KexiDBFieldList::hasField(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(QVariant(
               m_fieldlist->hasField( Kross::Api::Object::fromObject<KexiDBField>(args->item(0))->field() )
           ,0));
}

Kross::Api::Object::Ptr KexiDBFieldList::names(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(m_fieldlist->names());
}

Kross::Api::Object::Ptr KexiDBFieldList::addField(Kross::Api::List::Ptr args)
{
    m_fieldlist->addField(
        Kross::Api::Object::fromObject<KexiDBField>(args->item(0))->field()
    );
    return 0;
}

Kross::Api::Object::Ptr KexiDBFieldList::insertField(Kross::Api::List::Ptr args)
{
    m_fieldlist->insertField(
        Kross::Api::Variant::toUInt(args->item(0)),
        Kross::Api::Object::fromObject<KexiDBField>(args->item(1))->field()
    );
    return 0;
}

Kross::Api::Object::Ptr KexiDBFieldList::removeField(Kross::Api::List::Ptr args)
{
    m_fieldlist->removeField(
        Kross::Api::Object::fromObject<KexiDBField>(args->item(0))->field()
    );
    return 0;
}

Kross::Api::Object::Ptr KexiDBFieldList::clear(Kross::Api::List::Ptr)
{
    m_fieldlist->clear();
    return 0;
}

Kross::Api::Object::Ptr KexiDBFieldList::setFields(Kross::Api::List::Ptr args)
{
    ::KexiDB::FieldList* fl = Kross::Api::Object::fromObject<KexiDBFieldList>(args->item(0))->fieldlist();
    m_fieldlist->clear();
    for(::KexiDB::Field::ListIterator it = *fl->fields(); it.current(); ++it)
        m_fieldlist->addField( it.current() );
    return 0;
}

Kross::Api::Object::Ptr KexiDBFieldList::subList(Kross::Api::List::Ptr args)
{
    QStringList sl;
    QValueList<QVariant> list = Kross::Api::Variant::toList( args->item(0) );
    QValueList<QVariant>::ConstIterator it, end( list.constEnd() );
    for( it = list.constBegin(); it != end; ++it)
        sl.append( (*it).toString() );
    ::KexiDB::FieldList* fl = m_fieldlist->subList(sl);
    return fl ? new Kross::KexiDB::KexiDBFieldList(fl) : 0;
}

