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

//#include <qpointer.h>
#include <kdebug.h>
//#include <kmimetype.h>

#include <kexidb/driver.h>
#include <kexidb/connectiondata.h>
#include <kexidb/field.h>
#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>

using namespace Kross::KexiDB;

KexiDBDriverManager::KexiDBDriverManager(QObject* parent)
    : QObject(parent)
{
    setObjectName("DriverManager");
    /*
    //krossdebug( QString("Kross::KexiDB::KexiDBDriverManager::KexiDBDriverManager()") );

    this->addFunction0< Kross::Api::Variant >("driverNames", this, &KexiDBDriverManager::driverNames);

    this->addFunction1< KexiDBDriver, Kross::Api::Variant >("driver", this, &KexiDBDriverManager::driver);
    this->addFunction1< Kross::Api::Variant, Kross::Api::Variant >("lookupByMime", this, &KexiDBDriverManager::lookupByMime);
    this->addFunction1< Kross::Api::Variant, Kross::Api::Variant >("mimeForFile", this, &KexiDBDriverManager::mimeForFile);

    this->addFunction0< KexiDBConnectionData >("createConnectionData", this, &KexiDBDriverManager::createConnectionData);
    this->addFunction1< KexiDBConnectionData, Kross::Api::Variant >("createConnectionDataByFile", this, &KexiDBDriverManager::createConnectionDataByFile);
    this->addFunction0< KexiDBField >("field", this, &KexiDBDriverManager::field);
    this->addFunction1< KexiDBTableSchema, Kross::Api::Variant >("tableSchema", this, &KexiDBDriverManager::tableSchema);
    this->addFunction0< KexiDBQuerySchema>("querySchema", this, &KexiDBDriverManager::querySchema);
    */
}

KexiDBDriverManager::~KexiDBDriverManager() {
    //krossdebug( QString("Kross::KexiDB::KexiDBDriverManager::~KexiDBDriverManager()") );
}

#if 0
KexiDB::DriverManager& KexiDBDriverManager::driverManager()
{
    if(m_drivermanager.error())
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("KexiDB::DriverManager error: %1").arg(m_drivermanager.errorMsg())) );
    return m_drivermanager;
}

const QStringList KexiDBDriverManager::driverNames() {
    return driverManager().driverNames();
}

KexiDBDriver* KexiDBDriverManager::driver(const QString& drivername) {
    QPointer< ::KexiDB::Driver > driver = driverManager().driver(drivername); // caching is done by the DriverManager
    if(! driver) return 0;
    if(driver->error()) throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("KexiDB::Driver error for drivername '%1': %2").arg(drivername).arg(driver->errorMsg())) );
    return new KexiDBDriver(driver);
}

const QString KexiDBDriverManager::lookupByMime(const QString& mimetype) {
    return driverManager().lookupByMime(mimetype);
}

const QString KexiDBDriverManager::mimeForFile(const QString& filename) {
    QString mimename = KMimeType::findByFileContent( filename )->name();
    if(mimename.isEmpty() || mimename=="application/octet-stream" || mimename=="text/plain")
        mimename = KMimeType::findByURL(filename)->name();
    return mimename;
}

KexiDBConnectionData* KexiDBDriverManager::createConnectionData() {
    return new KexiDBConnectionData( new ::KexiDB::ConnectionData() );
}

KexiDBConnectionData* KexiDBDriverManager::createConnectionDataByFile(const QString& filename) {
    //! @todo reuse the original code!

    QString mimename = KMimeType::findByFileContent(filename)->name();
    if(mimename.isEmpty() || mimename=="application/octet-stream" || mimename=="text/plain")
        mimename = KMimeType::findByURL(filename)->name();

    if(mimename == "application/x-kexiproject-shortcut" || mimename == "application/x-kexi-connectiondata") {
        KConfig config(filename, true, false);
        QString groupkey;
        QStringList groups(config.groupList());
        QStringList::ConstIterator it, end( groups.constEnd() );
        for( it = groups.constBegin(); it != end; ++it) {
            if((*it).lower()!="file information") {
                groupkey = *it;
                break;
            }
        }
        if(groupkey.isNull()) {
            kDebug() << "No groupkey in KexiDBDriverManager::createConnectionDataByFile filename=" << filename << endl;
            return 0;
        }

        config.setGroup(groupkey);
        //QString type( config.readEntry("type", "database").lower() );
        //bool isDatabaseShortcut = (type == "database");

        ::KexiDB::ConnectionData* data = new ::KexiDB::ConnectionData();
        int version = config.readEntry("version", 2); //KexiDBShortcutFile_version
        data->setFileName(QString::null);
        data->caption = config.readEntry("caption");
        data->description = config.readEntry("comment");
        QString dbname = config.readEntry("name");
        data->driverName = config.readEntry("engine");
        data->hostName = config.readEntry("server");
        data->port = config.readEntry("port", 0);
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
    if(! drivername) {
        kDebug() << "No driver in KexiDBDriverManager::createConnectionDataByFile filename=" << filename << " mimename=" << mimename << endl;
        return 0;
    }

    ::KexiDB::ConnectionData* data = new ::KexiDB::ConnectionData();
    data->setFileName(filename);
    data->driverName = drivername;
    return new KexiDBConnectionData(data);
}

KexiDBField* KexiDBDriverManager::field() {
    return new KexiDBField( new ::KexiDB::Field() );
}

KexiDBTableSchema* KexiDBDriverManager::tableSchema(const QString& tablename) {
    return new KexiDBTableSchema( new ::KexiDB::TableSchema(tablename) );
}

KexiDBQuerySchema* KexiDBDriverManager::querySchema() {
    return new KexiDBQuerySchema( new ::KexiDB::QuerySchema() );
}
#endif
