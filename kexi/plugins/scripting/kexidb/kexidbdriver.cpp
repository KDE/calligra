/***************************************************************************
 * kexidbdriver.cpp
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

#include "kexidbdriver.h"
#include "kexidbdrivermanager.h"

#include "kexidbconnection.h"
#include "kexidbconnectiondata.h"

//#include <q3valuelist.h>
//#include <q3ptrlist.h>
#include <kdebug.h>

#include <kexidb/connection.h>

using namespace Kross::KexiDB;

KexiDBDriver::KexiDBDriver(::KexiDB::Driver* driver)
    : QObject()
    , m_driver(driver)
{
    setObjectName("KexiDBDriver");

/*
    this->addFunction0<Kross::Api::Variant>("isValid", this, &KexiDBDriver::isValid );
    this->addFunction0<Kross::Api::Variant>("versionMajor", this, &KexiDBDriver::versionMajor );
    this->addFunction0<Kross::Api::Variant>("versionMinor", this, &KexiDBDriver::versionMinor );
    this->addFunction1<Kross::Api::Variant, Kross::Api::Variant>("escapeString", this, &KexiDBDriver::escapeString);
    this->addFunction0<Kross::Api::Variant>("isFileDriver", this, &KexiDBDriver::isFileDriver );
    this->addFunction0<Kross::Api::Variant>("fileDBDriverMimeType", this, &KexiDBDriver::fileDBDriverMimeType );
    this->addFunction1<Kross::Api::Variant, Kross::Api::Variant>("isSystemObjectName", this, &KexiDBDriver::isSystemObjectName );
    this->addFunction1<Kross::Api::Variant, Kross::Api::Variant>("isSystemDatabaseName", this, &KexiDBDriver::isSystemDatabaseName );
    this->addFunction1<Kross::Api::Variant, Kross::Api::Variant>("isSystemFieldName", this, &KexiDBDriver::isSystemFieldName );
    this->addFunction2<Kross::Api::Variant, Kross::Api::Variant, Kross::Api::Variant> ("valueToSQL", this, &KexiDBDriver::valueToSQL );

    this->addFunction1<KexiDBConnection, KexiDBConnectionData>("createConnection", this, &KexiDBDriver::createConnection);
    this->addFunction0< Kross::Api::ListT< KexiDBConnection > >("connectionsList", this, &KexiDBDriver::connectionsList);
*/
}

KexiDBDriver::~KexiDBDriver()
{
}

#if 0
bool KexiDBDriver::isValid() { return m_driver->isValid(); }
int KexiDBDriver::versionMajor() { return m_driver->version().major; }
int KexiDBDriver::versionMinor() { return m_driver->version().minor; }
QString KexiDBDriver::escapeString(const QString& s) { return m_driver->escapeString(s); }
bool KexiDBDriver::isFileDriver() { return m_driver->isFileDriver(); }
QString KexiDBDriver::fileDBDriverMimeType() { return m_driver->fileDBDriverMimeType(); }
bool KexiDBDriver::isSystemObjectName(const QString& name) { return m_driver->isSystemObjectName(name); }
bool KexiDBDriver::isSystemDatabaseName(const QString& name) { return m_driver->isSystemDatabaseName(name); }
bool KexiDBDriver::isSystemFieldName(const QString& name) { return m_driver->isSystemFieldName(name); }
QString KexiDBDriver::valueToSQL(const QString& fieldtype, const QVariant& value) { return m_driver->valueToSQL(fieldtype, value); }
KexiDBConnection* KexiDBDriver::createConnection(KexiDBConnectionData* data) { return new KexiDBConnection( m_driver->createConnection(*data) ); }
Q3PtrList< ::KexiDB::Connection > KexiDBDriver::connectionsList() { return m_driver->connectionsList(); }
#endif
