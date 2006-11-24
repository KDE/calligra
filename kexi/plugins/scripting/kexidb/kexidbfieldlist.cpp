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

KexiDBFieldList::KexiDBFieldList(QObject* parent, ::KexiDB::FieldList* fieldlist, bool owner)
    : QObject(parent)
    , m_fieldlist(fieldlist)
    , m_owner(owner)
{
    setObjectName("KexiDBFieldList");
}

KexiDBFieldList::~KexiDBFieldList()
{
    if( m_owner )
        delete m_fieldlist;
}

uint KexiDBFieldList::fieldCount() {
    return m_fieldlist->fieldCount();
}

QObject* KexiDBFieldList::field(uint index) {
    ::KexiDB::Field* field = m_fieldlist->field(index);
    return field ? new KexiDBField(this, field, false) : 0;
}

QObject* KexiDBFieldList::fieldByName(const QString& name) {
    ::KexiDB::Field* field = m_fieldlist->field(name);
    return field ? new KexiDBField(this, field, false) : 0;
}

#if 0
Kross::Api::List* KexiDBFieldList::fields() {
    return new Kross::Api::ListT<KexiDBField>( *m_fieldlist->fields() );
}
#endif

bool KexiDBFieldList::hasField(QObject* field) {
    KexiDBField* f = dynamic_cast<KexiDBField*>(field);
    return f ? m_fieldlist->hasField( f->field() ) : false;
}

const QStringList KexiDBFieldList::names() const {
    return m_fieldlist->names();
}

bool KexiDBFieldList::addField(QObject* field) {
    KexiDBField* f = dynamic_cast<KexiDBField*>(field);
    if( ! f ) return false;
    m_fieldlist->addField( f->field() );
    return true;
}

bool KexiDBFieldList::insertField(uint index, QObject* field) {
    KexiDBField* f = dynamic_cast<KexiDBField*>(field);
    if( ! f ) return false;
    m_fieldlist->insertField(index, f->field());
    return true;
}

bool KexiDBFieldList::removeField(QObject* field) {
    KexiDBField* f = dynamic_cast<KexiDBField*>(field);
    if( ! f ) return false;
    m_fieldlist->removeField( f->field() );
    return true; // (! m_fieldlist->hasField( f->field() ));
}

void KexiDBFieldList::clear() {
    m_fieldlist->clear();
}

bool KexiDBFieldList::setFields(QObject* fieldlist) {
    KexiDBFieldList* list = dynamic_cast<KexiDBFieldList*>(fieldlist);
    if( ! list ) return false;
    list->clear();
    ::KexiDB::FieldList* fl = list->fieldlist();
    for(::KexiDB::Field::ListIterator it = *fl->fields(); it.current(); ++it)
        m_fieldlist->addField( it.current() );
    return true;
}

#if 0
KexiDBFieldList* KexiDBFieldList::subList(Q3ValueList<QVariant> list) {
    Q3ValueList<QVariant>::ConstIterator it( list.constBegin() ), end( list.constEnd() );
    QStringList sl;
    for(; it != end; ++it) sl.append( (*it).toString() );
    ::KexiDB::FieldList* fl = m_fieldlist->subList(sl);
    return fl ? new Kross::KexiDB::KexiDBFieldList(fl) : 0;
}
#endif

#include "kexidbfieldlist.moc"
