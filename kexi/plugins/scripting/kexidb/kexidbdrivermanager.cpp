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

#include <qpointer.h>
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
    addFunction("createConnectionDataByFile", &KexiDBDriverManager::createConnectionDataByFile,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));
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
    QPointer< ::KexiDB::Driver > driver = driverManager().driver(drivername); // caching is done by the DriverManager
    if(! driver)
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("No such KexiDB::Driver object for the defined drivername '%1'.").arg(drivername)) );
    if(driver->error())
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("KexiDB::Driver error for drivername '%1': %2").arg(drivername).arg(driver->errorMsg())) );
    return new KexiDBDriver(driver);
}

Kross::Api::Object::Ptr KexiDBDriverManager::lookupByMime(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
        driverManager().lookupByMime( Kross::Api::Variant::toString(args->item(0)) ));
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

Kross::Api::Object::Ptr KexiDBDriverManager::createConnectionDataByFile(Kross::Api::List::Ptr args)
{
//! @todo reuse the original code!

    QString const file = Kross::Api::Variant::toString(args->item(0));

    QString mimename = KMimeType::findByFileContent(file)->name();
    if(mimename.isEmpty() || mimename=="application/octet-stream" || mimename=="text/plain")
        mimename = KMimeType::findByURL(file)->name();

    if(mimename == "application/x-kexiproject-shortcut" || mimename == "application/x-kexi-connectiondata") {
        KConfig config(file, true, false);
        QString groupkey;
        QStringList groups(config.groupList());
        for(QStringList::Iterator it = groups.begin(); it != groups.end(); ++it) {
            if((*it).lower()!="file information") {
                groupkey = *it;
                break;
            }
        }
        if(groupkey.isNull())
            return 0;

        config.setGroup(groupkey);
        //QString type( config.readEntry("type", "database").lower() );
        //bool isDatabaseShortcut = (type == "database");

        ::KexiDB::ConnectionData* data = new ::KexiDB::ConnectionData();
        int version = config.readNumEntry("version", 2); //KexiDBShortcutFile_version
        data->setFileName(QString::null);
        data->caption = config.readEntry("caption");
        data->description = config.readEntry("comment");
        QString dbname = config.readEntry("name");
        data->driverName = config.readEntry("engine");
        data->hostName = config.readEntry("server");
        data->port = config.readNumEntry("port", 0);
        data->useLocalSocketFile = config.readBoolEntry("useLocalSocketFile", false);
        data->localSocketFileName = config.readEntry("localSocketFile");

        if(version >= 2 && config.hasKey("encryptedPassword")) {
            data->password = config.readEntry("encryptedPassword");
            uint len = data->password.length();
            for (uint i=0; i<len; i++)
                data->password[i] = QChar( data->password[i].unicode() - 47 - i );
        }
        if(data->password.isEmpty())
            data->password = config.readEntry("password");

        data->savePassword = ! data->password.isEmpty();
        data->userName = config.readEntry("user");

        KexiDBConnectionData* c = new KexiDBConnectionData(data);
        c->setDatabaseName(dbname);
        return c;
    }

    QString const drivername = driverManager().lookupByMime(mimename);
    if(! drivername)
        return 0;

    ::KexiDB::ConnectionData* data = new ::KexiDB::ConnectionData();
    data->setFileName(file);
    data->driverName = drivername;
    return new KexiDBConnectionData(data);
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

