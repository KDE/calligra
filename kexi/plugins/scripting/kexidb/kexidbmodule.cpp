/***************************************************************************
 * kexidbmodule.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2006 by Sebastian Sauer (mail@dipe.org)
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

#include "kexidbmodule.h"
#include "kexidbconnection.h"
#include "kexidbdriver.h"
#include "kexidbconnectiondata.h"
#include "kexidbfield.h"
#include "kexidbschema.h"

#include <kexidb/driver.h>
#include <kexidb/connectiondata.h>
#include <kexidb/field.h>
#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>

#include <kdebug.h>
#include <kmimetype.h>
#include <kconfiggroup.h>

// The as version() published versionnumber of this kross-module.
#define KROSS_KEXIDB_VERSION 1

extern "C"
{
    /**
     * Exported an loadable function as entry point to use
     * the \a KexiDBModule.
     */
    KDE_EXPORT QObject* krossmodule()
    {
        return new Scripting::KexiDBModule();
    }
}

using namespace Scripting;

KexiDBModule::KexiDBModule(QObject* parent)
    : QObject(parent)
{
    kDebug() << "Kross::KexiDB::KexiDBModule Ctor" << endl;
    setObjectName("KexiDB");
}

KexiDBModule::~KexiDBModule()
{
    kDebug() << "Kross::KexiDB::KexiDBModule Dtor" << endl;
}

int KexiDBModule::version()
{
    return KROSS_KEXIDB_VERSION;
}

const QStringList KexiDBModule::driverNames()
{
    return m_drivermanager.driverNames();
}

QObject* KexiDBModule::driver(const QString& drivername)
{
    QPointer< ::KexiDB::Driver > driver = m_drivermanager.driver(drivername); // caching is done by the DriverManager
    if(! driver) {
        kDebug() << QString("KexiDB::Driver No such driver '%1'").arg(drivername) << endl;
        return 0;
    }
    if(driver->error()) {
        kDebug() << QString("KexiDB::Driver error for drivername '%1': %2").arg(drivername).arg(driver->errorMsg()) << endl;
        return 0;
    }
    return new KexiDBDriver(this, driver);
}

const QString KexiDBModule::lookupByMime(const QString& mimetype)
{
    return m_drivermanager.lookupByMime(mimetype);
}

const QString KexiDBModule::mimeForFile(const QString& filename)
{
    QString mimename = KMimeType::findByFileContent( filename )->name();
    if(mimename.isEmpty() || mimename=="application/octet-stream" || mimename=="text/plain")
        mimename = KMimeType::findByUrl(filename)->name();
    return mimename;
}

QObject* KexiDBModule::createConnectionData()
{
    return new KexiDBConnectionData(this, new ::KexiDB::ConnectionData(), true);
}

QObject* KexiDBModule::createConnectionDataByFile(const QString& filename)
{
    //! @todo reuse the original code!

    QString mimename = KMimeType::findByFileContent(filename)->name();
    if(mimename.isEmpty() || mimename=="application/octet-stream" || mimename=="text/plain")
        mimename = KMimeType::findByUrl(filename)->name();

    if(mimename == "application/x-kexiproject-shortcut" || mimename == "application/x-kexi-connectiondata") {
        KConfig _config(filename, KConfig::NoGlobals);

        QString groupkey;
        foreach(QString s, _config.groupList()) {
            if(s.toLower()!="file information") {
                groupkey = s;
                break;
            }
        }
        if(groupkey.isNull()) {
            kDebug() << "No groupkey in KexiDBModule::createConnectionDataByFile filename=" << filename << endl;
            return 0;
        }

	KConfigGroup config(&_config, groupkey);
        //QString type( config.readEntry("type", "database").toLower() );
        //bool isDatabaseShortcut = (type == "database");

        ::KexiDB::ConnectionData* data = new ::KexiDB::ConnectionData();
        int version = config.readEntry("version", 2); //KexiDBShortcutFile_version
        data->setFileName(QString());
        data->caption = config.readEntry("caption");
        data->description = config.readEntry("comment");
        QString dbname = config.readEntry("name");
        data->driverName = config.readEntry("engine");
        data->hostName = config.readEntry("server");
        data->port = config.readEntry("port", 0);
        data->useLocalSocketFile = config.readEntry("useLocalSocketFile", false);
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

        KexiDBConnectionData* c = new KexiDBConnectionData(this, data, true);
        c->setDatabaseName(dbname);
        return c;
    }

    QString const drivername = m_drivermanager.lookupByMime(mimename);
    if(drivername.isEmpty()) {
        kDebug() << "No driver in KexiDBModule::createConnectionDataByFile filename=" << filename << " mimename=" << mimename << endl;
        return 0;
    }

    ::KexiDB::ConnectionData* data = new ::KexiDB::ConnectionData();
    data->setFileName(filename);
    data->driverName = drivername;
    return new KexiDBConnectionData(this, data, true);
}

QObject* KexiDBModule::field()
{
    return new KexiDBField(this, new ::KexiDB::Field(), true);
}

QObject* KexiDBModule::tableSchema(const QString& tablename)
{
    return new KexiDBTableSchema(this, new ::KexiDB::TableSchema(tablename), true);
}

QObject* KexiDBModule::querySchema()
{
    return new KexiDBQuerySchema(this, new ::KexiDB::QuerySchema(), true);
}

QObject* KexiDBModule::connectionWrapper(QObject* connection)
{
    ::KexiDB::Connection* c = dynamic_cast< ::KexiDB::Connection* >(connection);
    return c ? new KexiDBConnection(c) : 0;
}

#include "kexidbmodule.moc"
