/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>
                      Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003 Joseph Wenninger<jowenn@kde.org>
   Copyright (C) 2004, 2006 Jarosław Staniek <staniek@kde.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "mysqldriver_global.h"
#include <QVariant>
#include <QFile>
#include <QRegExp>

#include <KDebug>
#include <KLocale>

#include "mysqldriver.h"
#include "mysqlconnection.h"
#include "mysqlconnection_p.h"
#include "mysqlcursor.h"
#include "mysqlpreparedstatement.h"
#include <kexidb/error.h>


using namespace KexiDB;

//--------------------------------------------------------------------------

MySqlConnection::MySqlConnection(Driver *driver, ConnectionData &conn_data)
        : Connection(driver, conn_data)
        , d(new MySqlConnectionInternal(this))
{
}

MySqlConnection::~MySqlConnection()
{
    destroy();
    delete d;
}

bool MySqlConnection::drv_connect(KexiDB::ServerVersionInfo& version)
{
    const bool ok = d->db_connect(*data());
    if (!ok)
        return false;

    version.string = mysql_get_host_info(d->mysql);

    //retrieve server version info
#if 0 //this only works for client version >= 4.1 :(
    unsigned long v = mysql_get_server_version(d->mysql);
    // v - a number that represents the MySQL server version in this format
    // = major_version*10000 + minor_version *100 + sub_version
    version.major = v / 10000;
    version.minor = (v - version.major * 10000) / 100;
    version.release = v - version.major * 10000 - version.minor * 100;
#else //better way to get the version info: use 'version' built-in variable:
//! @todo this is hardcoded for now; define api for retrieving variables and use this API...
    QString versionString;
    tristate res = querySingleString("SELECT @@version", versionString, /*column*/0, false /*!addLimitTo1*/);
    QRegExp versionRe("(\\d+)\\.(\\d+)\\.(\\d+)");
    if (res == true && versionRe.exactMatch(versionString)) { // (if querySingleString failed, the version will be 0.0.0...
        version.major = versionRe.cap(1).toInt();
        version.minor = versionRe.cap(2).toInt();
        version.release = versionRe.cap(3).toInt();
    }
#endif
    // Get lower_case_table_name value so we know if there's case sensitivity supported
    // See http://dev.mysql.com/doc/refman/5.0/en/identifier-case-sensitivity.html
    int intLowerCaseTableNames = 0;
    res = querySingleNumber(QLatin1String("SHOW VARIABLES LIKE 'lower_case_table_name'"), intLowerCaseTableNames,
                            0/*col*/, false/* !addLimitTo1 */);
    if (res == false) // sanity
        return false;
    kDebug() << (res == true) << "lower_case_table_name:" << intLowerCaseTableNames;
    d->lowerCaseTableNames = intLowerCaseTableNames > 0;
    return true;
}

bool MySqlConnection::drv_disconnect()
{
    return d->db_disconnect();
}

Cursor* MySqlConnection::prepareQuery(const QString& statement, uint cursor_options)
{
    return new MySqlCursor(this, statement, cursor_options);
}

Cursor* MySqlConnection::prepareQuery(QuerySchema& query, uint cursor_options)
{
    return new MySqlCursor(this, query, cursor_options);
}

bool MySqlConnection::drv_getDatabasesList(QStringList &list)
{
    KexiDBDrvDbg << "MySqlConnection::drv_getDatabasesList()";
    list.clear();
    MYSQL_RES *res;

    if ((res = mysql_list_dbs(d->mysql, 0)) != 0) {
        MYSQL_ROW  row;
        while ((row = mysql_fetch_row(res)) != 0) {
            list << QString(row[0]);
        }
        mysql_free_result(res);
        return true;
    }

    d->storeResult();
// setError(ERR_DB_SPECIFIC,mysql_error(d->mysql));
    return false;
}

bool MySqlConnection::drv_databaseExists(const QString &dbName, bool ignoreErrors)
{
    bool success;
    /* db names can be lower case in mysql */
    const QString storedDbName(d->lowerCaseTableNames ? dbName.toLower() : dbName);
    bool exists = resultExists(
      QString::fromLatin1("SHOW DATABASES LIKE %1")
          .arg(driver()->escapeString(storedDbName)), success);
    if (!exists || !success) {
        if (!ignoreErrors)
            setError(ERR_OBJECT_NOT_FOUND, i18n("The database \"%1\" does not exist.", storedDbName));
        return false;
    }
    return true;
}

bool MySqlConnection::drv_createDatabase(const QString &dbName)
{
    const QString storedDbName(d->lowerCaseTableNames ? dbName.toLower() : dbName);
    KexiDBDrvDbg << "MySqlConnection::drv_createDatabase: " << storedDbName;
    // mysql_create_db deprecated, use SQL here.
    // db names are lower case in mysql
    if (drv_executeSQL(QString::fromLatin1("CREATE DATABASE %1").arg(escapeIdentifier(storedDbName))))
        return true;
    d->storeResult();
    return false;
}

bool MySqlConnection::drv_useDatabase(const QString &dbName, bool *cancelled, MessageHandler* msgHandler)
{
    Q_UNUSED(cancelled);
    Q_UNUSED(msgHandler);
//TODO is here escaping needed?
    const QString storedDbName(d->lowerCaseTableNames ? dbName.toLower() : dbName);
    return d->useDatabase(storedDbName);
}

bool MySqlConnection::drv_closeDatabase()
{
//TODO free resources
//As far as I know, mysql doesn't support that
    return true;
}

bool MySqlConnection::drv_dropDatabase(const QString &dbName)
{
//TODO is here escaping needed
    const QString storedDbName(d->lowerCaseTableNames ? dbName.toLower() : dbName);
    return drv_executeSQL(QString::fromLatin1("DROP DATABASE %1").arg(escapeIdentifier(storedDbName)));
}

bool MySqlConnection::drv_executeSQL(const QString& statement)
{
    return d->executeSQL(statement);
}

quint64 MySqlConnection::drv_lastInsertRowID()
{
    //! @todo
    return (quint64)mysql_insert_id(d->mysql);
}

int MySqlConnection::serverResult()
{
    return d->res;
}

QString MySqlConnection::serverResultName()
{
    return QString();
}

void MySqlConnection::drv_clearServerResult()
{
    if (!d)
        return;
    d->res = 0;
}

QString MySqlConnection::serverErrorMsg()
{
    return d->errmsg;
}

bool MySqlConnection::drv_containsTable(const QString &tableName)
{
    bool success;
    return resultExists(QString("show tables like %1")
                        .arg(driver()->escapeString(tableName)), success) && success;
}

bool MySqlConnection::drv_getTablesList(QStringList &list)
{
    return queryStringList("show tables", list);
}

PreparedStatement::Ptr MySqlConnection::prepareStatement(PreparedStatement::StatementType type,
        FieldList& fields)
{
    return KSharedPtr<PreparedStatement>(new MySqlPreparedStatement(type, *d, fields));
}

#include "mysqlconnection.moc"
