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

#include "kexidbconnection.h"
#include "kexidbconnectiondata.h"

//#include <q3valuelist.h>
//#include <q3ptrlist.h>
#include <kdebug.h>

#include <kexidb/connection.h>

using namespace Kross::KexiDB;

KexiDBDriver::KexiDBDriver(QObject* parent, ::KexiDB::Driver* driver)
    : QObject(parent)
    , m_driver(driver)
{
    setObjectName("KexiDBDriver");
}

KexiDBDriver::~KexiDBDriver()
{
}

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

#if 0
KexiDBConnection* KexiDBDriver::createConnection(KexiDBConnectionData* data) { return new KexiDBConnection( m_driver->createConnection(*data) ); }
Q3PtrList< ::KexiDB::Connection > KexiDBDriver::connectionsList() { return m_driver->connectionsList(); }
#endif

#include "kexidbdriver.moc"
