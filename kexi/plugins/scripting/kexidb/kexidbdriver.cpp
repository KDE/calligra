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
#include <qptrlist.h>
#include <kdebug.h>

#include <kexidb/connection.h>

using namespace Kross::KexiDB;

KexiDBDriver::KexiDBDriver(::KexiDB::Driver* driver)
    : Kross::Api::Class<KexiDBDriver>("KexiDBDriver", KexiDBDriverManager::self())
    , m_driver(driver)
{

    // Return true if this driver is valid else false.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,bool> >
        ("isValid", driver, &::KexiDB::Driver::isValid );

    // The drivers major versionnumber.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,int> >
        ("versionMajor", driver, &::KexiDB::Driver::versionMajor );

    // The drivers minor versionnumber.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,int> >
        ("versionMinor", driver, &::KexiDB::Driver::versionMinor );

    // Driver-specific SQL string escaping. For example the " or ' char may
    // need to be escaped for values used within SQL-statements.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,QString>, Kross::Api::ProxyValue<Kross::Api::Variant,const QString&> >
        ("escapeString", driver, (QString(::KexiDB::Driver::*)(const QString&)const) &::KexiDB::Driver::escapeString);

    // Returns true if this driver is file-based.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,bool> >
        ("isFileDriver", driver, &::KexiDB::Driver::isFileDriver );

    // Return a name of MIME type of files handled by this driver if it is a
    // file-based database's driver otherwise returns null string.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,QString> >
        ("fileDBDriverMimeType", driver, &::KexiDB::Driver::fileDBDriverMimeType );

    // Returns true if the passed string is a system object's name, eg. name
    // of build-in system table that cannot be used or created by a user.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,bool>, Kross::Api::ProxyValue<Kross::Api::Variant,const QString&> >
        ("isSystemObjectName", driver, &::KexiDB::Driver::isSystemObjectName );

    // Returns true if the passed string is a system database's name, eg. name
    // of build-in, system database that cannot be used or created by a user.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,bool>, Kross::Api::ProxyValue<Kross::Api::Variant,const QString&> >
        ("isSystemDatabaseName", driver, &::KexiDB::Driver::isSystemDatabaseName );

    // Returns true if the passed string is a system field's name, build-in
    // system field that cannot be used or created by a user.
    this->addProxyFunction< Kross::Api::ProxyValue<Kross::Api::Variant,bool>, Kross::Api::ProxyValue<Kross::Api::Variant,const QString&> >
        ("isSystemFieldName", driver, &::KexiDB::Driver::isSystemFieldName );

    // The as second argument passed string got escaped to be usuable within
    // a SQL-statement and those escaped string got returned by the method.
    // The first argument defines the fieldtype to what we should escape the
    // second argument to.
    this->addProxyFunction<
        Kross::Api::ProxyValue<Kross::Api::Variant,QString>,
        Kross::Api::ProxyValue<Kross::Api::Variant,const QString&>,
        Kross::Api::ProxyValue<Kross::Api::Variant,const QVariant&>
        > ("valueToSQL", driver, (QString(::KexiDB::Driver::*)(const QString&,const QVariant&)const) &::KexiDB::Driver::valueToSQL );

    // Create a new KexiDBConnection object and return it.
    this->addProxyFunction<
        Kross::Api::ProxyValue<Kross::KexiDB::KexiDBConnection, ::KexiDB::Connection* >,
        Kross::Api::ProxyValue<Kross::KexiDB::KexiDBConnectionData, ::KexiDB::ConnectionData& > >
        ("createConnection", driver, &::KexiDB::Driver::createConnection );

    // Return a list of KexiDBConnection objects.
    this->addProxyFunction< 
        Kross::Api::ProxyValue< Kross::Api::ListT<Kross::KexiDB::KexiDBConnection, ::KexiDB::Connection>, const QPtrList< ::KexiDB::Connection> > >
        ("connectionsList", driver, &::KexiDB::Driver::connectionsList );
}

KexiDBDriver::~KexiDBDriver()
{
}

const QString KexiDBDriver::getClassName() const
{
    return "Kross::KexiDB::KexiDBDriver";
}

