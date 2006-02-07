/***************************************************************************
 * kexidbdrivermanager.cpp
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

#include "kexidbdrivermanager.h"
#include "kexidbdriver.h"
#include "kexidbconnectiondata.h"
#include "kexidbfield.h"
#include "kexidbschema.h"

#include <api/exception.h>

#include <qguardedptr.h>
#include <kdebug.h>
#include <kmimetype.h>

#include <kexidb/driver.h>
#include <kexidb/connectiondata.h>
#include <kexidb/field.h>
#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>

using namespace Kross::KexiDB;

KexiDBDriverManager::KexiDBDriverManager()
    : Kross::Api::Class<KexiDBDriverManager>("DriverManager")
{
    addFunction("driverNames", &KexiDBDriverManager::driverNames);
    addFunction("driver", &KexiDBDriverManager::driver,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));
    addFunction("lookupByMime", &KexiDBDriverManager::lookupByMime,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));
    addFunction("mimeForFile", &KexiDBDriverManager::mimeForFile,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("createConnectionData", &KexiDBDriverManager::createConnectionData);
    addFunction("field", &KexiDBDriverManager::field);
    addFunction("tableSchema", &KexiDBDriverManager::tableSchema,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));
    addFunction("querySchema", &KexiDBDriverManager::querySchema);
}

KexiDBDriverManager::~KexiDBDriverManager()
{
}

const QString KexiDBDriverManager::getClassName() const
{
    return "Kross::KexiDB::KexiDBDriverManager";
}

KexiDB::DriverManager& KexiDBDriverManager::driverManager()
{
    if(m_drivermanager.error())
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("KexiDB::DriverManager error: %1").arg(m_drivermanager.errorMsg())) );
    return m_drivermanager;
}

Kross::Api::Object::Ptr KexiDBDriverManager::driverNames(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(driverManager().driverNames(), "Kross::KexiDB::DriverManager::driverNames::StringList");
}

Kross::Api::Object::Ptr KexiDBDriverManager::driver(Kross::Api::List::Ptr args)
{
    QString drivername = Kross::Api::Variant::toString(args->item(0));
    QGuardedPtr< ::KexiDB::Driver > driver = driverManager().driver(drivername); // caching is done by the DriverManager
    if(! driver)
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("No such KexiDB::Driver object for the defined drivername '%1'.").arg(drivername)) );
    if(driver->error())
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("KexiDB::Driver error for drivername '%1': %2").arg(drivername).arg(driver->errorMsg())) );
    return new KexiDBDriver(driver);
}

Kross::Api::Object::Ptr KexiDBDriverManager::lookupByMime(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
        driverManager().lookupByMime( Kross::Api::Variant::toString(args->item(0)) ),
        "Kross::KexiDB::DriverManager::lookupByMime::String");
}

Kross::Api::Object::Ptr KexiDBDriverManager::mimeForFile(Kross::Api::List::Ptr args)
{
    QString const file = Kross::Api::Variant::toString(args->item(0));
    QString mimename = KMimeType::findByFileContent(file)->name();
    if(mimename.isEmpty() || mimename=="application/octet-stream" || mimename=="text/plain")
        mimename = KMimeType::findByURL(file)->name();
    return new Kross::Api::Variant(mimename);
}

Kross::Api::Object::Ptr KexiDBDriverManager::createConnectionData(Kross::Api::List::Ptr)
{
    return new KexiDBConnectionData( new ::KexiDB::ConnectionData() );
}

Kross::Api::Object::Ptr KexiDBDriverManager::field(Kross::Api::List::Ptr)
{
    return new KexiDBField( new ::KexiDB::Field() );
}

Kross::Api::Object::Ptr KexiDBDriverManager::tableSchema(Kross::Api::List::Ptr args)
{
    return new KexiDBTableSchema(
               new ::KexiDB::TableSchema(Kross::Api::Variant::toString(args->item(0)))
           );
}

Kross::Api::Object::Ptr KexiDBDriverManager::querySchema(Kross::Api::List::Ptr)
{
    return new KexiDBQuerySchema( new ::KexiDB::QuerySchema() );
}

