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

#include <qvaluelist.h>
#include <kdebug.h>

#include <kexidb/connection.h>

using namespace Kross::KexiDB;

KexiDBDriver::KexiDBDriver(::KexiDB::Driver* driver)
    : Kross::Api::Class<KexiDBDriver>("KexiDBDriver", KexiDBDriverManager::self())
    , m_driver(driver)
{
    addFunction("versionMajor", &KexiDBDriver::versionMajor);
    addFunction("versionMinor", &KexiDBDriver::versionMinor);

    addFunction("escapeString", &KexiDBDriver::escapeString,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));
    addFunction("valueToSQL", &KexiDBDriver::valueToSQL,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::Variant"));

    addFunction("createConnection", &KexiDBDriver::createConnection,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBConnectionData"));
    addFunction("connectionList", &KexiDBDriver::connectionList);
}

KexiDBDriver::~KexiDBDriver()
{
}

const QString KexiDBDriver::getClassName() const
{
    return "Kross::KexiDB::KexiDBDriver";
}

::KexiDB::Driver* KexiDBDriver::driver()
{
    if(! m_driver)
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("KexiDB::Driver is NULL.")) );
    if(m_driver->error())
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("KexiDB::Driver error: %1").arg(m_driver->errorMsg())) );
    return m_driver;
}

Kross::Api::Object::Ptr KexiDBDriver::versionMajor(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(driver()->versionMajor(),
           "Kross::KexiDB::Driver::versionMajor::Int");
}

Kross::Api::Object::Ptr KexiDBDriver::versionMinor(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(driver()->versionMinor(),
           "Kross::KexiDB::Driver::versionMinor::Int");
}

Kross::Api::Object::Ptr KexiDBDriver::createConnection(Kross::Api::List::Ptr args)
{
    KexiDBConnectionData* data =
        Kross::Api::Object::fromObject<KexiDBConnectionData>(args->item(0));
    QGuardedPtr< ::KexiDB::Connection > connection = driver()->createConnection( *(data->getConnectionData()) );
    if(! connection)
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception("Failed to create connection.") );
    return new KexiDBConnection(connection, this, data);
}

Kross::Api::Object::Ptr KexiDBDriver::connectionList(Kross::Api::List::Ptr)
{
    QValueList<Kross::Api::Object::Ptr> list;
    QPtrList< ::KexiDB::Connection > connectionlist = driver()->connectionsList();
    ::KexiDB::Connection* connection;
    for(connection = connectionlist.first(); connection; connection = connectionlist.next())
        list.append( new KexiDBConnection(connection, this) );
    return new Kross::Api::List(list,
           "Kross::KexiDB::Driver::connectionList::List");
}

Kross::Api::Object::Ptr KexiDBDriver::escapeString(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           driver()->escapeString( Kross::Api::Variant::toString(args->item(0)) ),
           "Kross::KexiDB::DriverManager::escapeString::String");
}

Kross::Api::Object::Ptr KexiDBDriver::valueToSQL(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           driver()->valueToSQL(
               (uint)::KexiDB::Field::typeForString(Kross::Api::Variant::toString(args->item(0))),
               Kross::Api::Variant::toVariant(args->item(1))
           ),
           "Kross::KexiDB::DriverManager::valueToSQL::String");
}

