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

#include <KDbDriver>
#include <KDbConnectionData>
#include <KDbField>
#include <KDbTableSchema>
#include <KDbQuerySchema>

#include <KConfigGroup>

#include <QDebug>
#include <QMimeDatabase>

// The as version() published versionnumber of this kross-module.
#define KROSS_KEXIDB_VERSION 1

extern "C"
{
    /**
     * Exported an loadable function as entry point to use
     * the \a KexiDBModule.
     */
    KDE_EXPORT QObject* krossmodule() {
        return new Scripting::KexiDBModule();
    }
}

using namespace Scripting;

KexiDBModule::KexiDBModule(QObject* parent)
        : QObject(parent)
{
    qDebug();
    setObjectName("KexiDB");
}

KexiDBModule::~KexiDBModule()
{
    qDebug();
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
    QPointer< KDbDriver > driver = m_drivermanager.driver(drivername); // caching is done by the DriverManager
    if (! driver) {
        qWarning() << "No such driver '%1'" << drivername;
        return 0;
    }
    if (driver->error()) {
        qWarning() << "Error for drivername" << drivername << driver->errorMsg();
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
    QMimeDatabase db;
    QString mimename = db.mimeTypeForFile(filename, QMimeDatabase::MatchContent).name();
    if (mimename.isEmpty() || mimename == "application/octet-stream" || mimename == "text/plain") {
        mimename = db.mimeTypeForUrl(filename).name();
    }
    return mimename;
}

QObject* KexiDBModule::createConnectionData()
{
    return new KexiDBConnectionData(this, new KDbConnectionData(), true);
}

QObject* KexiDBModule::createConnectionDataByFile(const QString& filename)
{
    //! @todo reuse the original code!
    QMimeDatabase db;
    QString mimename = db.mimeTypeForFile(filename, QMimeDatabase::MatchContent).name();
    if (mimename.isEmpty() || mimename == "application/octet-stream" || mimename == "text/plain") {
        mimename = db.mimeTypeForUrl(filename).name();
    }
    if (mimename == "application/x-kexiproject-shortcut" || mimename == "application/x-kexi-connectiondata") {
        KConfig _config(filename, KConfig::NoGlobals);

        QString groupkey;
        foreach(const QString &s, _config.groupList()) {
            if (s.toLower() != "file information") {
                groupkey = s;
                break;
            }
        }
        if (groupkey.isNull()) {
            qDebug() << "No groupkey, filename=" << filename;
            return 0;
        }

        KConfigGroup config(&_config, groupkey);
        KDbConnectionData* data = new KDbConnectionData();
        int version = config.readEntry("version", 2); //KexiDBShortcutFile_version
        data->setDatabaseName(QString());
        data->setCaption(config.readEntry("caption"));
        data->setDescription(config.readEntry("comment"));
        QString dbname = config.readEntry("name");
        data->setDriverId(config.readEntry("engine"));
        data->setHostName(config.readEntry("server"));
        data->setPort(config.readEntry("port", 0));
        data->setUseLocalSocketFile(config.readEntry("useLocalSocketFile", false));
        data->setLocalSocketFileName(config.readEntry("localSocketFile"));

        if (version >= 2 && config.hasKey("encryptedPassword")) {
            QString password(config.readEntry("encryptedPassword"));
            uint len = password.length();
            for (uint i = 0; i < len; i++) {
                password[i] = QChar(password[i].unicode() - 47 - i);
            }
            data->setPassword(password);
        }
        if (data->password().isEmpty()) {
            data->setPassword(config.readEntry("password"));
        }

        data->setSavePassword(!data->password().isEmpty());
        data->setUserName(config.readEntry("user"));

        KexiDBConnectionData* c = new KexiDBConnectionData(this, data, true);
        c->setDatabaseName(dbname);
        return c;
    }

    const QStringList driverIds = m_drivermanager.driverIdsForMimeType(mimename);
    if (driverIds.isEmpty()) {
        qDebug() << "No driver, filename=" << filename << "mimename=" << mimename;
        return 0;
    }

    KDbConnectionData* data = new KDbConnectionData();
    data->setDatabaseName(filename);
    //! @todo there can be more than one driver
    data->setDriverId(driverIds.first());
    return new KexiDBConnectionData(this, data, true);
}

QObject* KexiDBModule::field()
{
    return new KexiDBField(this, new KDbField(), true);
}

QObject* KexiDBModule::tableSchema(const QString& tablename)
{
    return new KexiDBTableSchema(this, new KDbTableSchema(tablename), true);
}

QObject* KexiDBModule::querySchema()
{
    return new KexiDBQuerySchema(this, new KDbQuerySchema(), true);
}

QObject* KexiDBModule::connectionWrapper(QObject* connection)
{
    KDbConnection* c = dynamic_cast< KDbConnection* >(connection);
    return c ? new KexiDBConnection(c) : 0;
}

