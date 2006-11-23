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

#include <kdebug.h>
//Added by qt3to4:
//#include <Q3ValueList>

using namespace Kross::KexiDB;

KexiDBFieldList::KexiDBFieldList(::KexiDB::FieldList* fieldlist)
    : QObject()
    , m_fieldlist(fieldlist)
{
    setObjectName("KexiDBFieldList");

/*
    this->addFunction0< Kross::Api::Variant >("fieldCount", this, &KexiDBFieldList::fieldCount);
    this->addFunction1< KexiDBField, Kross::Api::Variant >("field", this, &KexiDBFieldList::field);
    this->addFunction1< KexiDBField, Kross::Api::Variant >("fieldByName", this, &KexiDBFieldList::fieldByName);

    this->addFunction0< Kross::Api::List >("fields", this, &KexiDBFieldList::fields);

    this->addFunction1< Kross::Api::Variant, KexiDBField >("hasField", this, &KexiDBFieldList::hasField);
    this->addFunction0< Kross::Api::Variant >("names", this, &KexiDBFieldList::names);

    this->addFunction1< void, KexiDBField >("addField", this, &KexiDBFieldList::addField);
    this->addFunction2< void, Kross::Api::Variant, KexiDBField >("insertField", this, &KexiDBFieldList::insertField);
    this->addFunction1< void, KexiDBField >("removeField", this, &KexiDBFieldList::removeField);
    this->addFunction0< void >("clear", this, &KexiDBFieldList::clear);
    this->addFunction1< void, KexiDBFieldList >("setFields", this, &KexiDBFieldList::setFields);

    this->addFunction1< KexiDBFieldList, Kross::Api::Variant >("subList", this, &KexiDBFieldList::subList);
*/
}

KexiDBFieldList::~KexiDBFieldList()
{
}

#if 0
uint KexiDBFieldList::fieldCount() {
    return m_fieldlist->fieldCount();
}

KexiDBField* KexiDBFieldList::field(uint index) {
    ::KexiDB::Field* field = m_fieldlist->field(index);
    return field ? new KexiDBField(field) : 0;
}

KexiDBField* KexiDBFieldList::fieldByName(const QString& name) {
    ::KexiDB::Field* field = m_fieldlist->field(name);
    return field ? new KexiDBField(field) : 0;
}

Kross::Api::List* KexiDBFieldList::fields() {
    return new Kross::Api::ListT<KexiDBField>( *m_fieldlist->fields() );
}

bool KexiDBFieldList::hasField(KexiDBField* field) { return m_fieldlist->hasField( field->field() ); }
const QStringList KexiDBFieldList::names() const { return m_fieldlist->names(); }
void KexiDBFieldList::addField(KexiDBField* field) { m_fieldlist->addField( field->field() ); }
void KexiDBFieldList::insertField(uint index, KexiDBField* field) { m_fieldlist->insertField(index, field->field()); }
void KexiDBFieldList::removeField(KexiDBField* field) { m_fieldlist->removeField( field->field() ); }
void KexiDBFieldList::clear() { m_fieldlist->clear(); }

void KexiDBFieldList::setFields(KexiDBFieldList* fieldlist) {
    m_fieldlist->clear();
    ::KexiDB::FieldList* fl = fieldlist->fieldlist();
    for(::KexiDB::Field::ListIterator it = *fl->fields(); it.current(); ++it)
        m_fieldlist->addField( it.current() );
}

KexiDBFieldList* KexiDBFieldList::subList(Q3ValueList<QVariant> list) {
    Q3ValueList<QVariant>::ConstIterator it( list.constBegin() ), end( list.constEnd() );
    QStringList sl;
    for(; it != end; ++it) sl.append( (*it).toString() );
    ::KexiDB::FieldList* fl = m_fieldlist->subList(sl);
    return fl ? new Kross::KexiDB::KexiDBFieldList(fl) : 0;
}
#endif
