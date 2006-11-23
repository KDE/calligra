/***************************************************************************
 * kexidbcursor.cpp
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

#include "kexidbcursor.h"
#include "kexidbconnection.h"

#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>

#include <kdebug.h>

using namespace Kross::KexiDB;

KexiDBCursor::KexiDBCursor(::KexiDB::Cursor* cursor)
    : QObject()
    , m_cursor(cursor)
{
    setObjectName("KexiDBCursor");

/*
    this->addFunction0<Kross::Api::Variant>("open", this, &KexiDBCursor::open );
    this->addFunction0<Kross::Api::Variant>("isOpened", this, &KexiDBCursor::isOpened );
    this->addFunction0<Kross::Api::Variant>("reopen", this, &KexiDBCursor::reopen );
    this->addFunction0<Kross::Api::Variant>("close", this, &KexiDBCursor::close );
    this->addFunction0<Kross::Api::Variant>("moveFirst", this, &KexiDBCursor::moveFirst );
    this->addFunction0<Kross::Api::Variant>("moveLast", this, &KexiDBCursor::moveLast );
    this->addFunction0<Kross::Api::Variant>("movePrev", this, &KexiDBCursor::movePrev );
    this->addFunction0<Kross::Api::Variant>("moveNext", this, &KexiDBCursor::moveNext );
    this->addFunction0<Kross::Api::Variant>("bof", this, &KexiDBCursor::bof );
    this->addFunction0<Kross::Api::Variant>("eof", this, &KexiDBCursor::eof );
    this->addFunction0<Kross::Api::Variant>("at", this, &KexiDBCursor::at );
    this->addFunction0<Kross::Api::Variant>("fieldCount", this, &KexiDBCursor::fieldCount );
    this->addFunction1<Kross::Api::Variant, Kross::Api::Variant>("value", this, &KexiDBCursor::value );
    this->addFunction2<Kross::Api::Variant, Kross::Api::Variant, Kross::Api::Variant>("setValue", this, &KexiDBCursor::setValue );
    this->addFunction0<Kross::Api::Variant>("save", this, &KexiDBCursor::save );
*/
}

KexiDBCursor::~KexiDBCursor()
{
    ///@todo check ownership
    //delete m_cursor;
#if 0
    clearBuffers();
#endif
}

#if 0
void KexiDBCursor::clearBuffers()
{
    QMap<Q_LLONG, Record*>::ConstIterator
        it( m_modifiedrecords.constBegin() ), end( m_modifiedrecords.constEnd() );
    for( ; it != end; ++it)
        delete it.data();
    m_modifiedrecords.clear();
}

bool KexiDBCursor::open() { return m_cursor->open(); }
bool KexiDBCursor::isOpened() { return m_cursor->isOpened(); }
bool KexiDBCursor::reopen() { return m_cursor->reopen(); }
bool KexiDBCursor::close() { return m_cursor->close(); }

bool KexiDBCursor::moveFirst() { return m_cursor->moveFirst(); }
bool KexiDBCursor::moveLast() { return m_cursor->moveLast(); }
bool KexiDBCursor::movePrev() { return m_cursor->movePrev(); }
bool KexiDBCursor::moveNext() { return m_cursor->moveNext(); }

bool KexiDBCursor::bof() { return m_cursor->bof(); }
bool KexiDBCursor::eof() { return m_cursor->eof(); }

Q_LLONG KexiDBCursor::at() { return m_cursor->at(); }
uint KexiDBCursor::fieldCount() { return m_cursor->fieldCount(); }

QVariant KexiDBCursor::value(uint index)
{
    return m_cursor->value(index);
}

bool KexiDBCursor::setValue(uint index, QVariant value)
{
    ::KexiDB::QuerySchema* query = m_cursor->query();
    if(! query) {
        kDebug() << "Invalid query in KexiDBCursor::setValue index=" << index << " value=" << value << endl;
        return false;
    }

    ::KexiDB::QueryColumnInfo* column = query->fieldsExpanded().at(index);
    if(! column) {
        kDebug() << "Invalid column in KexiDBCursor::setValue index=" << index << " value=" << value << endl;
        return false;
    }

    const Q_LLONG position = m_cursor->at();
    if(! m_modifiedrecords.contains(position))
        m_modifiedrecords.replace(position, new Record(m_cursor));
    m_modifiedrecords[position]->buffer->insert(*column, value);
    return true;
}

bool KexiDBCursor::save()
{
    if(m_modifiedrecords.count() < 1)
        return true;

    //It is needed to close the cursor before we are able to update the rows
    //since else the database could be locked (e.g. at the case of SQLite a
    //KexiDB: Object ERROR: 6: SQLITE_LOCKED would prevent updating).
    //Maybe it works fine with other drivers like MySQL or Postqre?
    m_cursor->close();

    bool ok = true;
    QMap<Q_LLONG, Record*>::ConstIterator
        it( m_modifiedrecords.constBegin() ), end( m_modifiedrecords.constEnd() );
    for( ; it != end; ++it) {
        bool b = m_cursor->updateRow(it.data()->rowdata, * it.data()->buffer, m_cursor->isBuffered());
        if(ok) {
            ok = b;
            //break;
        }
    }
    //m_cursor->close();
    clearBuffers();
    return ok;
}
#endif
