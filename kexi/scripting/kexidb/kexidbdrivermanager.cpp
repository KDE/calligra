/***************************************************************************
 * kexidbdrivermanager.cpp
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

#include "kexidbdrivermanager.h"
#include "kexidbdriver.h"
#include "kexidbconnectiondata.h"
#include "kexidbfield.h"
#include "kexidbschema.h"

#include "../api/exception.h"

#include <qguardedptr.h>
#include <klocale.h>
#include <kdebug.h>

#include <kexidb/driver.h>
#include <kexidb/connectiondata.h>
#include <kexidb/field.h>
#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>

using namespace Kross::KexiDB;

KexiDBDriverManager::KexiDBDriverManager()
    : Kross::Api::Class<KexiDBDriverManager>("KexiDBDriverManager")
{
    addFunction("driverNames", &KexiDBDriverManager::driverNames,
        Kross::Api::ArgumentList(),
        i18n("Returns a stringlist of all avaible drivernames.")
    );
    addFunction("driver", &KexiDBDriverManager::driver,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Returns the to name matching KexiDBDriver object.")
    );
    addFunction("lookupByMime", &KexiDBDriverManager::lookupByMime,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Looks up a drivers list by MIME type of database file. "
             "Only file-based database drivers are checked. "
             "The lookup is case insensitive.")
    );

    addFunction("connectionData", &KexiDBDriverManager::connectionData,
        Kross::Api::ArgumentList(),
        i18n("Return a new KexiDBConnectionData object.")
    );
    addFunction("field", &KexiDBDriverManager::field,
        Kross::Api::ArgumentList(),
        i18n("Return a new KexiDBField object.")
    );
    addFunction("tableSchema", &KexiDBDriverManager::tableSchema,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Return a new KexiDBTableSchema object.")
    );
    addFunction("querySchema", &KexiDBDriverManager::querySchema,
        Kross::Api::ArgumentList(),
        i18n("Return a new KexiDBQuerySchema object.")
    );
}

KexiDBDriverManager::~KexiDBDriverManager()
{
}

const QString KexiDBDriverManager::getClassName() const
{
    return "Kross::KexiDB::KexiDBDriverManager";
}

const QString KexiDBDriverManager::getDescription() const
{
    return i18n("KexiDB::DriverManager wrapper for database driver "
                "management, e.g. finding and loading drivers.");
}

KexiDB::DriverManager& KexiDBDriverManager::driverManager()
{
    if(m_drivermanager.error())
        throw Kross::Api::RuntimeException(i18n("KexiDB::DriverManager error: %1").arg(m_drivermanager.errorMsg()));
    return m_drivermanager;
}

Kross::Api::Object* KexiDBDriverManager::driverNames(Kross::Api::List*)
{
    return Kross::Api::Variant::create(driverManager().driverNames(), "Kross::KexiDB::DriverManager::driverNames::StringList");
}

Kross::Api::Object* KexiDBDriverManager::driver(Kross::Api::List* args)
{
    QString drivername = Kross::Api::Variant::toString(args->item(0));
    QGuardedPtr< ::KexiDB::Driver > driver = driverManager().driver(drivername); // caching is done by the DriverManager
    if(! driver)
        throw Kross::Api::AttributeException(i18n("No such KexiDB::Driver object for the defined drivername '%1'.").arg(drivername));
    if(driver->error())
        throw Kross::Api::RuntimeException(i18n("KexiDB::Driver error for drivername '%1': %2").arg(drivername).arg(driver->errorMsg()));
    return new KexiDBDriver(this, driver);
}

Kross::Api::Object* KexiDBDriverManager::lookupByMime(Kross::Api::List* args)
{
    return Kross::Api::Variant::create(
        driverManager().lookupByMime( Kross::Api::Variant::toString(args->item(0)) ),
        "Kross::KexiDB::DriverManager::lookupByMime::String");
}

Kross::Api::Object* KexiDBDriverManager::connectionData(Kross::Api::List*)
{
    return new KexiDBConnectionData( new ::KexiDB::ConnectionData() );
}

Kross::Api::Object* KexiDBDriverManager::field(Kross::Api::List*)
{
    return new KexiDBField( new ::KexiDB::Field() );
}

Kross::Api::Object* KexiDBDriverManager::tableSchema(Kross::Api::List* args)
{
    return new KexiDBTableSchema(
               new ::KexiDB::TableSchema(Kross::Api::Variant::toString(args->item(0)))
           );
}

Kross::Api::Object* KexiDBDriverManager::querySchema(Kross::Api::List*)
{
    return new KexiDBQuerySchema( new ::KexiDB::QuerySchema() );
}

