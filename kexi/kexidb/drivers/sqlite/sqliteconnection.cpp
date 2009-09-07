/* This file is part of the KDE project
   Copyright (C) 2003-2006 Jarosław Staniek <staniek@kde.org>

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

#include "sqliteconnection.h"
#include "sqliteconnection_p.h"
#include "sqlitecursor.h"
#include "sqlitepreparedstatement.h"

#include "sqlite.h"

#include "kexisql.h" //for isReadOnly()

#include <kexidb/driver.h>
#include <kexidb/cursor.h>
#include <kexidb/error.h>
#include <kexiutils/utils.h>

#include <qfile.h>
#include <qdir.h>
#include <qregexp.h>

#include <KDebug>
#include <KLocale>

//remove debug
#undef KexiDBDrvDbg
#define KexiDBDrvDbg if (0) kDebug()

using namespace KexiDB;

SQLiteConnectionInternal::SQLiteConnectionInternal(Connection *connection)
        : ConnectionInternal(connection)
        , data(0)
        , data_owned(true)
        , errmsg_p(0)
        , res(SQLITE_OK)
        , result_name(0)
{
}

SQLiteConnectionInternal::~SQLiteConnectionInternal()
{
    if (data_owned && data) {
        free(data);
        data = 0;
    }
//sqlite_freemem does this if (errmsg) {
//  free( errmsg );
//  errmsg = 0;
// }
}

void SQLiteConnectionInternal::storeResult()
{
    if (errmsg_p) {
        errmsg = errmsg_p;
        sqlite_free(errmsg_p);
        errmsg_p = 0;
    }
    errmsg = (data && res != SQLITE_OK) ? sqlite3_errmsg(data) : 0;
}

/*! Used by driver */
SQLiteConnection::SQLiteConnection(Driver *driver, ConnectionData &conn_data)
        : Connection(driver, conn_data)
        , d(new SQLiteConnectionInternal(this))
{
}

SQLiteConnection::~SQLiteConnection()
{
    KexiDBDrvDbg << "SQLiteConnection::~SQLiteConnection()";
    //disconnect if was connected
// disconnect();
    destroy();
    delete d;
    KexiDBDrvDbg << "SQLiteConnection::~SQLiteConnection() ok";
}

bool SQLiteConnection::drv_connect(KexiDB::ServerVersionInfo& version)
{
    KexiDBDrvDbg << "SQLiteConnection::connect()";
    version.string = QString(SQLITE_VERSION); //defined in sqlite3.h
    QRegExp re("(\\d+)\\.(\\d+)\\.(\\d+)");
    if (re.exactMatch(version.string)) {
        version.major = re.cap(1).toUInt();
        version.minor = re.cap(2).toUInt();
        version.release = re.cap(3).toUInt();
    }
    return true;
}

bool SQLiteConnection::drv_disconnect()
{
    KexiDBDrvDbg << "SQLiteConnection::disconnect()";
    return true;
}

bool SQLiteConnection::drv_getDatabasesList(QStringList &list)
{
    //this is one-db-per-file database
    list.append(data()->fileName());   //more consistent than dbFileName() ?
    return true;
}

bool SQLiteConnection::drv_containsTable(const QString &tableName)
{
    bool success;
    return resultExists(QString("select name from sqlite_master where type='table' and name LIKE %1")
                        .arg(driver()->escapeString(tableName)), success) && success;
}

bool SQLiteConnection::drv_getTablesList(QStringList &list)
{
    KexiDB::Cursor *cursor;
    m_sql = "select lower(name) from sqlite_master where type='table'";
    if (!(cursor = executeQuery(m_sql))) {
        KexiDBWarn << "Connection::drv_getTablesList(): !executeQuery()";
        return false;
    }
    list.clear();
    cursor->moveFirst();
    while (!cursor->eof() && !cursor->error()) {
        list += cursor->value(0).toString();
        cursor->moveNext();
    }
    if (cursor->error()) {
        deleteCursor(cursor);
        return false;
    }
    return deleteCursor(cursor);
}

bool SQLiteConnection::drv_createDatabase(const QString &dbName)
{
    // SQLite creates a new db is it does not exist
    return drv_useDatabase(dbName);
#if 0
    d->data = sqlite_open(QFile::encodeName(data()->fileName()), 0/*mode: unused*/,
                          &d->errmsg_p);
    d->storeResult();
    return d->data != 0;
#endif
}

bool SQLiteConnection::drv_useDatabase(const QString &dbName, bool *cancelled,
                                       MessageHandler* msgHandler)
{
    Q_UNUSED(dbName);
// KexiDBDrvDbg << "drv_useDatabase(): " << data()->fileName();
    //TODO: perhaps allow to use sqlite3_open16() as well for SQLite ~ 3.3 ?
//! @todo add option (command line or in kexirc?)
    int exclusiveFlag = Connection::isReadOnly() ? SQLITE_OPEN_READONLY : SQLITE_OPEN_WRITE_LOCKED; // <-- shared read + (if !r/o): exclusive write
//! @todo add option
    int allowReadonly = 1;
    const bool wasReadOnly = Connection::isReadOnly();

    d->res = sqlite3_open(
                 //QFile::encodeName( data()->fileName() ),
                 data()->fileName().toUtf8().constData(), /* unicode expected since SQLite 3.1 */
                 &d->data,
                 exclusiveFlag,
                 allowReadonly /* If 1 and locking fails, try opening in read-only mode */
             );
    d->storeResult();

    if (d->res == SQLITE_OK && cancelled && !wasReadOnly && allowReadonly && isReadOnly()) {
        //opened as read only, ask
        if (KMessageBox::Continue !=
                askQuestion(
                    i18n("Do you want to open file \"%1\" as read-only?",
                         QDir::convertSeparators(data()->fileName()))
                    + "\n\n"
                    + i18n("The file is probably already open on this or another computer.") + " "
                    + i18n("Could not gain exclusive access for writing the file."),
                    KMessageBox::WarningContinueCancel, KMessageBox::Continue,
                    KGuiItem(i18n("Open As Read-Only"), "document-open"), KStandardGuiItem::cancel(),
                    "askBeforeOpeningFileReadOnly", KMessageBox::Notify, msgHandler)) {
            clearError();
            if (!drv_closeDatabase())
                return false;
            *cancelled = true;
            return false;
        }
    }

    if (d->res == SQLITE_CANTOPEN_WITH_LOCKED_READWRITE) {
        setError(ERR_ACCESS_RIGHTS,
                 i18n("The file is probably already open on this or another computer.") + "\n\n"
                 + i18n("Could not gain exclusive access for reading and writing the file.") + " "
                 + i18n("Check the file's permissions and whether it is already opened and locked by another application."));
    } else if (d->res == SQLITE_CANTOPEN_WITH_LOCKED_WRITE) {
        setError(ERR_ACCESS_RIGHTS,
                 i18n("The file is probably already open on this or another computer.") + "\n\n"
                 + i18n("Could not gain exclusive access for writing the file.") + " "
                 + i18n("Check the file's permissions and whether it is already opened and locked by another application."));
    }
    return d->res == SQLITE_OK;
}

bool SQLiteConnection::drv_closeDatabase()
{
    if (!d->data)
        return false;

    const int res = sqlite_close(d->data);
    if (SQLITE_OK == res) {
        d->data = 0;
        return true;
    }
    if (SQLITE_BUSY == res) {
#if 0 //this is ANNOYING, needs fixing (by closing cursors or waiting)
        setError(ERR_CLOSE_FAILED, i18n("Could not close busy database."));
#else
        return true;
#endif
    }
    return false;
}

bool SQLiteConnection::drv_dropDatabase(const QString &dbName)
{
    Q_UNUSED(dbName); // Each database is one single SQLite file.
    const QString filename = data()->fileName();
    if (QFile(filename).exists() && !QDir().remove(filename)) {
        setError(ERR_ACCESS_RIGHTS, i18n("Could not remove file \"%1\".",
                                         QDir::convertSeparators(filename)) + " "
                 + i18n("Check the file's permissions and whether it is already opened and locked by another application."));
        return false;
    }
    return true;
}

//CursorData* SQLiteConnection::drv_createCursor( const QString& statement )
Cursor* SQLiteConnection::prepareQuery(const QString& statement, uint cursor_options)
{
    return new SQLiteCursor(this, statement, cursor_options);
}

Cursor* SQLiteConnection::prepareQuery(QuerySchema& query, uint cursor_options)
{
    return new SQLiteCursor(this, query, cursor_options);
}

bool SQLiteConnection::drv_executeSQL(const QString& statement)
{
// KexiDBDrvDbg << "SQLiteConnection::drv_executeSQL(" << statement << ")";
// QCString st(statement.length()*2);
// st = escapeString( statement.local8Bit() ); //?
#ifdef SQLITE_UTF8
    d->temp_st = statement.toUtf8();
#else
    d->temp_st = statement.toLocal8Bit(); //latin1 only
#endif

#ifdef KEXI_DEBUG_GUI
    KexiUtils::addKexiDBDebug(QString("ExecuteSQL (SQLite): ") + statement);
#endif

    d->res = sqlite_exec(
                 d->data,
                 (const char*)d->temp_st,
                 0/*callback*/,
                 0,
                 &d->errmsg_p);
    d->storeResult();
#ifdef KEXI_DEBUG_GUI
    KexiUtils::addKexiDBDebug(d->res == SQLITE_OK ? "  Success" : "  Failure");
#endif
    return d->res == SQLITE_OK;
}

quint64 SQLiteConnection::drv_lastInsertRowID()
{
    return (quint64)sqlite_last_insert_rowid(d->data);
}

int SQLiteConnection::serverResult()
{
    return d->res == 0 ? Connection::serverResult() : d->res;
}

QString SQLiteConnection::serverResultName()
{
    return Connection::serverResultName();
}

void SQLiteConnection::drv_clearServerResult()
{
    if (!d)
        return;
    d->res = SQLITE_OK;
}

QString SQLiteConnection::serverErrorMsg()
{
    return d->errmsg.isEmpty() ? Connection::serverErrorMsg() : d->errmsg;
}

PreparedStatement::Ptr SQLiteConnection::prepareStatement(PreparedStatement::StatementType type,
        FieldList& fields)
{
    return KSharedPtr<PreparedStatement>(new SQLitePreparedStatement(type, *d, fields));
}

bool SQLiteConnection::isReadOnly() const
{
    return (d->data ? sqlite3_is_readonly(d->data) : false)
           || Connection::isReadOnly();
}

#include "sqliteconnection.moc"
