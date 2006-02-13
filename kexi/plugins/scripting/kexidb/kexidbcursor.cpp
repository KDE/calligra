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

#include <kdebug.h>

using namespace Kross::KexiDB;

KexiDBCursor::KexiDBCursor(KexiDBConnection* connection, ::KexiDB::Cursor* cursor)
    : Kross::Api::Class<KexiDBCursor>("KexiDBCursor", connection)
    , m_cursor(cursor)
{
    // Opens the cursor.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,bool> >
        ("open", m_cursor, &::KexiDB::Cursor::open );

    // Returns true if the cursor is opened else false.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,bool> >
        ("isOpened", m_cursor, &::KexiDB::Cursor::isOpened );

    // Closes and then opens again the same cursor.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,bool> >
        ("reopen", m_cursor, &::KexiDB::Cursor::reopen );

    // Closes previously opened cursor.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,bool> >
        ("close", m_cursor, &::KexiDB::Cursor::close );

    // Moves current position to the first record and retrieves it.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,bool> >
        ("moveFirst", m_cursor, &::KexiDB::Cursor::moveFirst );

    // Moves current position to the last record and retrieves it.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,bool> >
        ("moveLast", m_cursor, &::KexiDB::Cursor::moveLast );

    // Moves current position to the previous record and retrieves it.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,bool> >
        ("movePrev", m_cursor, &::KexiDB::Cursor::movePrev );

    // Moves current position to the next record and retrieves it.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,bool> >
        ("moveNext", m_cursor, &::KexiDB::Cursor::moveNext );

    // Returns true if current position is before first record.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,bool> >
        ("bof", m_cursor, &::KexiDB::Cursor::bof );

    // Returns true if current position is after last record.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,bool> >
        ("eof", m_cursor, &::KexiDB::Cursor::eof );

    // Returns current internal position of the cursor's query. Records 
    // are numbered from 0; the value -1 means that the cursor does not 
    // point to a valid record.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,Q_LLONG> >
        ("at", m_cursor, &::KexiDB::Cursor::at );

    // Returns the number of fields available for this cursor.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,uint> >
        ("fieldCount", m_cursor, &::KexiDB::Cursor::fieldCount );

    // Returns the value stored in the passed column number (counting from 0).
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,QVariant>, Kross::Api::ProxyValue<Kross::Api::Variant,uint> >
        ("value", m_cursor, &::KexiDB::Cursor::value );
}

KexiDBCursor::~KexiDBCursor()
{
    ///@todo check ownership
    //delete m_cursor;
}

const QString KexiDBCursor::getClassName() const
{
    return "Kross::KexiDB::KexiDBCursor";
}

