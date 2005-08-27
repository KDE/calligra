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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#include "kexidbcursor.h"
#include "kexidbconnection.h"

#include <api/exception.h>

#include <kdebug.h>

using namespace Kross::KexiDB;

KexiDBCursor::KexiDBCursor(KexiDBConnection* connection, ::KexiDB::Cursor* cursor)
    : Kross::Api::Class<KexiDBCursor>("KexiDBCursor", connection)
    , m_cursor(cursor)
{
    addFunction("moveFirst", &KexiDBCursor::moveFirst,
        Kross::Api::ArgumentList(),
        QString("Moves current position to the first record and retrieves it.")
    );
    addFunction("moveLast", &KexiDBCursor::moveLast,
        Kross::Api::ArgumentList(),
        QString("Moves current position to the last record and retrieves it.")
    );
    addFunction("moveNext", &KexiDBCursor::moveNext,
        Kross::Api::ArgumentList(),
        QString("Moves current position to the next record and retrieves it.")
    );
    addFunction("movePrev", &KexiDBCursor::movePrev,
        Kross::Api::ArgumentList(),
        QString("Moves current position to the next record and retrieves it.")
    );
    addFunction("eof", &KexiDBCursor::eof,
        Kross::Api::ArgumentList(),
        QString("Returns true if current position is after last record.")
    );
    addFunction("bof", &KexiDBCursor::bof,
        Kross::Api::ArgumentList(),
        QString("Returns true if current position is before first record.")
    );
    addFunction("at", &KexiDBCursor::at,
        Kross::Api::ArgumentList(),
        QString("Returns current internal position of the cursor's query. "
             "Records are numbered from 0; the value -1 means that "
             "the cursor does not point to a valid record.")
    );
    addFunction("fieldCount", &KexiDBCursor::fieldCount,
        Kross::Api::ArgumentList(),
        QString("Returns the number of fields available for this cursor.")
    );

    addFunction("value", &KexiDBCursor::value,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::UInt"),
        QString("Returns the value stored in column number \a i (counting from 0).")
    );
}

KexiDBCursor::~KexiDBCursor()
{
}

const QString KexiDBCursor::getClassName() const
{
    return "Kross::KexiDB::KexiDBCursor";
}

const QString KexiDBCursor::getDescription() const
{
    return QString("KexiDB::Cursor wrapper to provide database "
                "cursor functionality.");
}

::KexiDB::Cursor* KexiDBCursor::cursor()
{
    if(! m_cursor)
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("KexiDB::Cursor is NULL.")) );
    if(m_cursor->error())
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("KexiDB::Cursor error: %1").arg(m_cursor->errorMsg())) );
    return m_cursor;
}

Kross::Api::Object::Ptr KexiDBCursor::moveFirst(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(cursor()->moveFirst(),
           "Kross::KexiDB::Cursor::moveFirst::Bool");
}

Kross::Api::Object::Ptr KexiDBCursor::moveLast(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(cursor()->moveLast(),
           "Kross::KexiDB::Cursor::moveLast::Bool");
}

Kross::Api::Object::Ptr KexiDBCursor::moveNext(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(cursor()->moveNext(),
           "Kross::KexiDB::Cursor::moveNext::Bool");
}

Kross::Api::Object::Ptr KexiDBCursor::movePrev(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(cursor()->movePrev(),
           "Kross::KexiDB::Cursor::movePrev::Bool");
}

Kross::Api::Object::Ptr KexiDBCursor::eof(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(cursor()->eof(),
           "Kross::KexiDB::Cursor::eof::Bool");
}

Kross::Api::Object::Ptr KexiDBCursor::bof(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(cursor()->bof(),
           "Kross::KexiDB::Cursor::bof::Bool");
}

Kross::Api::Object::Ptr KexiDBCursor::at(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(cursor()->at(),
           "Kross::KexiDB::Cursor::at::LLONG");
}

Kross::Api::Object::Ptr KexiDBCursor::fieldCount(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(cursor()->fieldCount(),
           "Kross::KexiDB::Cursor::fieldCount::UInt");
}

Kross::Api::Object::Ptr KexiDBCursor::value(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           cursor()->value( Kross::Api::Variant::toUInt(args->item(0)) ),
           "Kross::KexiDB::Cursor::value::Variant");
}

