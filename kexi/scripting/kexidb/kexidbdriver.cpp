/***************************************************************************
 * kexidbdriver.cpp
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

#include "kexidbdriver.h"
#include "kexidbdrivermanager.h"

#include "kexidbconnection.h"
#include "kexidbconnectiondata.h"

#include <qvaluelist.h>
#include <klocale.h>
#include <kdebug.h>

#include <kexidb/connection.h>

using namespace Kross::KexiDB;

KexiDBDriver::KexiDBDriver(KexiDBDriverManager* drivermanager, ::KexiDB::Driver* driver)
    : Kross::Api::Class<KexiDBDriver>("KexiDBDriver", drivermanager)
    , m_driver(driver)
{
    addFunction("versionMajor", &KexiDBDriver::versionMajor,
        Kross::Api::ArgumentList(),
        i18n("Return the major version number of this driver.")
    );
    addFunction("versionMinor", &KexiDBDriver::versionMinor,
        Kross::Api::ArgumentList(),
        i18n("Return the minor version number of this driver.")
    );
    addFunction("escapeString", &KexiDBDriver::escapeString,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Return a driver-specific escaped SQL string.")
    );
    addFunction("createConnection", &KexiDBDriver::createConnection,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBConnectionData"),
        i18n("Create a new KexiDBConnection object and return it.")
    );
    addFunction("connectionList", &KexiDBDriver::connectionList,
        Kross::Api::ArgumentList(),
        i18n("Return a list of KexiDBConnection objects.")
    );
}

KexiDBDriver::~KexiDBDriver()
{
}

const QString KexiDBDriver::getClassName() const
{
    return "Kross::KexiDB::KexiDBDriver";
}

const QString KexiDBDriver::getDescription() const
{
    return i18n("KexiDB::Driver wrapper to access the generic "
                "database abstraction functionality KexiDB spends.");
}

::KexiDB::Driver* KexiDBDriver::driver()
{
    if(! m_driver)
        throw Kross::Api::RuntimeException(i18n("KexiDB::Driver is NULL."));
    if(m_driver->error())
        throw Kross::Api::RuntimeException(i18n("KexiDB::Driver error: %1").arg(m_driver->errorMsg()));
    return m_driver;
}

Kross::Api::Object* KexiDBDriver::versionMajor(Kross::Api::List*)
{
    return Kross::Api::Variant::create(driver()->versionMajor(),
           "Kross::KexiDB::Driver::versionMajor::Int");
}

Kross::Api::Object* KexiDBDriver::versionMinor(Kross::Api::List*)
{
    return Kross::Api::Variant::create(driver()->versionMinor(),
           "Kross::KexiDB::Driver::versionMinor::Int");
}

Kross::Api::Object* KexiDBDriver::createConnection(Kross::Api::List* args)
{
    KexiDBConnectionData* data =
        Kross::Api::Object::fromObject<KexiDBConnectionData>(args->item(0));
    QGuardedPtr< ::KexiDB::Connection > connection = driver()->createConnection( *(data->getConnectionData()) );
    if(! connection)
        throw Kross::Api::RuntimeException("Failed to create connection.");
    return new KexiDBConnection(this, connection, data);
}

Kross::Api::Object* KexiDBDriver::connectionList(Kross::Api::List*)
{
    QValueList<Object*> list;
    QPtrList< ::KexiDB::Connection > connectionlist = driver()->connectionsList();
    ::KexiDB::Connection* connection;
    for(connection = connectionlist.first(); connection; connection = connectionlist.next())
        list.append( new KexiDBConnection(this, connection) );
    return Kross::Api::List::create(list,
           "Kross::KexiDB::Driver::connectionList::List");
}

Kross::Api::Object* KexiDBDriver::escapeString(Kross::Api::List* args)
{
    return Kross::Api::Variant::create(
           driver()->escapeString( Kross::Api::Variant::toString(args->item(0)) ),
           "Kross::KexiDB::DriverManager::escapeString::String");
}

