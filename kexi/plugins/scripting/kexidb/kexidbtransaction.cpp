/***************************************************************************
 * kexidbtransaction.cpp
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


#include "kexidbtransaction.h"
#include "kexidbconnection.h"

//#include <kdebug.h>

using namespace Kross::KexiDB;

KexiDBTransaction::KexiDBTransaction(::KexiDB::Transaction& transaction)
    : QObject()
    , m_transaction(transaction)
{
    setObjectName("KexiDBTransaction");
/*
    this->addFunction0< Kross::Api::Variant >("isActive", this, &KexiDBTransaction::isActive);
    this->addFunction0< Kross::Api::Variant >("isNull", this, &KexiDBTransaction::isNull);
*/
}

KexiDBTransaction::~KexiDBTransaction()
{
}

::KexiDB::Transaction& KexiDBTransaction::transaction()
{
    return m_transaction;
}

#if 0
bool KexiDBTransaction::isActive() const
{
    return m_transaction.active();
}

bool KexiDBTransaction::isNull() const
{
    return m_transaction.isNull();
}
#endif
