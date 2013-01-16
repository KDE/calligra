/***************************************************************************
 * kexidbcursor.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2006 by Sebastian Sauer (mail@dipe.org)
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

#include <db/tableschema.h>
#include <db/queryschema.h>

#include <kdebug.h>

using namespace Scripting;

KexiDBCursor::KexiDBCursor(QObject* parent, ::KexiDB::Cursor* cursor, bool owner)
        : QObject(parent)
        , m_cursor(cursor)
        , m_owner(owner)
{
    setObjectName("KexiDBCursor");
}

KexiDBCursor::~KexiDBCursor()
{
    clearBuffers();
    if (m_owner) {
        m_cursor->close();
        delete m_cursor;
    }
}

void KexiDBCursor::clearBuffers()
{
    QMap<qint64, Record*>::ConstIterator
    it(m_modifiedrecords.constBegin()), end(m_modifiedrecords.constEnd());
    for (; it != end; ++it)
        delete it.value();
    m_modifiedrecords.clear();
}

bool KexiDBCursor::open()
{
    return m_cursor->open();
}
bool KexiDBCursor::isOpened()
{
    return m_cursor->isOpened();
}
bool KexiDBCursor::reopen()
{
    return m_cursor->reopen();
}
bool KexiDBCursor::close()
{
    return m_cursor->close();
}

bool KexiDBCursor::moveFirst()
{
    return m_cursor->moveFirst();
}
bool KexiDBCursor::moveLast()
{
    return m_cursor->moveLast();
}
bool KexiDBCursor::movePrev()
{
    return m_cursor->movePrev();
}
bool KexiDBCursor::moveNext()
{
    return m_cursor->moveNext();
}

bool KexiDBCursor::bof()
{
    return m_cursor->bof();
}
bool KexiDBCursor::eof()
{
    return m_cursor->eof();
}

int KexiDBCursor::at()
{
    return m_cursor->at();
}
uint KexiDBCursor::fieldCount()
{
    return m_cursor->fieldCount();
}

QVariant KexiDBCursor::value(uint index)
{
    return m_cursor->value(index);
}

bool KexiDBCursor::setValue(uint index, QVariant value)
{
    ::KexiDB::QuerySchema* query = m_cursor->query();
    if (! query) {
        kDebug() << "Invalid query in KexiDBCursor::setValue index=" << index << " value=" << value;
        return false;
    }

    ::KexiDB::QueryColumnInfo* column = query->fieldsExpanded().at(index);
    if (! column) {
        kDebug() << "Invalid column in KexiDBCursor::setValue index=" << index << " value=" << value;
        return false;
    }

    const qint64 position = m_cursor->at();
    if (! m_modifiedrecords.contains(position))
        m_modifiedrecords.insert(position, new Record(m_cursor));
    m_modifiedrecords[position]->buffer->insert(*column, value);
    return true;
}

bool KexiDBCursor::save()
{
    if (m_modifiedrecords.count() < 1)
        return true;

    //It is needed to close the cursor before we are able to update the rows
    //since else the database could be locked (e.g. at the case of SQLite a
    //KexiDB: Object ERROR: 6: SQLITE_LOCKED would prevent updating).
    //Maybe it works fine with other drivers like MySQL or Postqre?
    m_cursor->close();

    bool ok = true;
    QMap<qint64, Record*>::ConstIterator
    it(m_modifiedrecords.constBegin()), end(m_modifiedrecords.constEnd());
    for (; it != end; ++it) {
        bool b = m_cursor->updateRow(it.value()->rowdata, * it.value()->buffer, m_cursor->isBuffered());
        if (ok) {
            ok = b;
            //break;
        }
    }
    //m_cursor->close();
    clearBuffers();
    return ok;
}

#include "kexidbcursor.moc"

