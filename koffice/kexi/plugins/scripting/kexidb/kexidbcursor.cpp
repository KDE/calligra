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

KexiDBCursor::KexiDBCursor(::KexiDB::Cursor* cursor)
    : Kross::Api::Class<KexiDBCursor>("KexiDBCursor")
    , m_cursor(cursor)
{
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
QVariant KexiDBCursor::value(uint index) { return m_cursor->value(index); }
