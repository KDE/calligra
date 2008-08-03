/* This file is part of the KDE project
   Copyright (C) 2007 Sharan Rao <sharanrao@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this program; see the file COPYING. If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <QVariant>
#include <QFile>
#include <QRegExp>

#include <kgenericfactory.h>
#include <kdebug.h>

#include "sybasedriver.h"
#include "sybaseconnection.h"
#include "sybaseconnection_p.h"
#include "sybasecursor.h"
#include "sybasepreparedstatement.h"
#include <kexidb/error.h>


using namespace KexiDB;

//--------------------------------------------------------------------------

SybaseConnection::SybaseConnection(Driver *driver, ConnectionData &conn_data)
        : Connection(driver, conn_data)
        , d(new SybaseConnectionInternal(this))
{
}

SybaseConnection::~SybaseConnection()
{
    destroy();
}

bool SybaseConnection::drv_connect(KexiDB::ServerVersionInfo& version)
{
    const bool ok = d->db_connect(*data());
    if (!ok)
        return false;

    // we can retrieve the server name and the server version using global variables
    // @@servername
    // @@version

    QString serverVersionString;

    if (!querySingleString("Select @@servername" , version.string)) {
        KexiDBDrvDbg << "Couldn't fetch server name" << endl;
    }

    if (!querySingleString("Select @@version", serverVersionString)) {
        KexiDBDrvDbg << "Couldn't fetch server version" << endl;
    }

    QRegExp versionRe("(\\d+)\\.(\\d+)\\.(\\d+)\\.(\\d+)");
    if (versionRe.exactMatch(serverVersionString)) {
        version.major = versionRe.cap(1).toInt();
        version.minor = versionRe.cap(2).toInt();
        version.release = versionRe.cap(3).toInt();
    }

    return true;
}

bool SybaseConnection::drv_disconnect()
{
    return d->db_disconnect();
}

Cursor* SybaseConnection::prepareQuery(const QString& statement, uint cursor_options)
{
    return new SybaseCursor(this, statement, cursor_options);
}

Cursor* SybaseConnection::prepareQuery(QuerySchema& query, uint cursor_options)
{
    return new SybaseCursor(this, query, cursor_options);
}

bool SybaseConnection::drv_getDatabasesList(QStringList &list)
{
    KexiDBDrvDbg << "SybaseConnection::drv_getDatabasesList()" << endl;

    // select * from master..sysdatabases ?
    // todo: verify.
    return queryStringList("Select name from master..sysdatabases", list) ;
}

bool SybaseConnection::drv_createDatabase(const QString &dbName)
{
    KexiDBDrvDbg << "SybaseConnection::drv_createDatabase: " << dbName << endl;
    // mysql_create_db deprecated, use SQL here.
    if (drv_executeSQL("CREATE DATABASE " + dbName)) {
        // set allow_nulls_by_default option to true
        QString allowNullsQuery = QString("sp_dboption %1, allow_nulls_by_default, true").arg(dbName);
        if (drv_executeSQL(allowNullsQuery.toLatin1().data()))
            return true;
    }
    d->storeResult();
    return false;
}

bool SybaseConnection::drv_useDatabase(const QString &dbName, bool *cancelled, MessageHandler* msgHandler)
{
    Q_UNUSED(cancelled);
    Q_UNUSED(msgHandler);

//TODO is here escaping needed?
    return d->useDatabase(dbName) ;
}

bool SybaseConnection::drv_closeDatabase()
{
// here we disconenct the connection
    return true;
}

bool SybaseConnection::drv_dropDatabase(const QString &dbName)
{

    return drv_executeSQL("drop database " + driver()->escapeString(dbName));
}

bool SybaseConnection::drv_executeSQL(const QString& statement)
{
    return d->executeSQL(statement);
}

quint64 SybaseConnection::drv_lastInsertRowID()
{
    int rowId;
    querySingleNumber("Select @@IDENTITY", rowId);

    return (qint64)rowId;
}

int SybaseConnection::serverResult()
{
    return d->res;
}

QString SybaseConnection::serverResultName()
{

    return QString();
}

void SybaseConnection::drv_clearServerResult()
{
    if (!d)
        return;
    d->res = 0;
}

QString SybaseConnection::serverErrorMsg()
{
    return d->errmsg;
}

bool SybaseConnection::drv_containsTable(const QString &tableName)
{
    bool success;
    return resultExists(QString("select name from sysobjects where type='U' and name=%1")
                        .arg(driver()->escapeString(tableName)), success) && success;
}

bool SybaseConnection::drv_getTablesList(QStringList &list)
{
    return queryStringList("Select name from sysobjects where type='U'", list);
}

PreparedStatement::Ptr SybaseConnection::prepareStatement(PreparedStatement::StatementType type,
        FieldList& fields)
{
    return KSharedPtr<PreparedStatement>(new SybasePreparedStatement(type, *d, fields));
}

bool KexiDB::SybaseConnection::drv_beforeInsert(const QString& table, FieldList& fields)
{

    if (fields.autoIncrementFields()->isEmpty())
        return true;

    // explicit insertion into IDENTITY fields !!
    return drv_executeSQL(QString("SET IDENTITY_INSERT %1 ON").arg(escapeIdentifier(table)));

}

bool KexiDB::SybaseConnection::drv_afterInsert(const QString& table, FieldList& fields)
{
    // should we instead just set a flag when an identity_insert has taken place and only check for that
    // flag here ?

    if (fields.autoIncrementFields()->isEmpty())
        return true;

    // explicit insertion into IDENTITY fields has taken place. Turn off IDENTITY_INSERT
    return drv_executeSQL(QString("SET IDENTITY_INSERT %1 OFF").arg(escapeIdentifier(table)));

}

bool KexiDB::SybaseConnection::drv_beforeUpdate(const QString& table, FieldList& fields)
{
    if (fields.autoIncrementFields()->isEmpty())
        return true;

    // explicit update of IDENTITY fields has taken place.
    return drv_executeSQL(QString("SET IDENTITY_UPDATE %1 ON").arg(escapeIdentifier(table)));
}

bool KexiDB::SybaseConnection::drv_afterUpdate(const QString& table, FieldList& fields)
{
    // should we instead just set a flag when an identity_update has taken place and only check for that
    // flag here ?

    if (fields.autoIncrementFields()->isEmpty())
        return true;

    // explicit insertion into IDENTITY fields has taken place. Turn off IDENTITY_INSERT
    return drv_executeSQL(QString("SET IDENTITY_UPDATE %1 OFF").arg(escapeIdentifier(table)));

}

#include "sybaseconnection.moc"
