/* This file is part of the KDE project
   Copyright (C) 2009 Sharan Rao <sharanrao@gmail.com>

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

#ifndef KEXIDB_CONN_ODBC_H
#define KEXIDB_CONN_ODBC_H

#include <QStringList>

#include <kexidb/connection.h>
#include <kexidb/tableschema.h>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

namespace KexiDB
{

class ODBCConnectionInternal;

class ODBCConnection : public Connection
{
    Q_OBJECT

public:
    ~ODBCConnection();

    // static function which stores error
    static void extractError(ODBCConnection* connection, SQLHANDLE handle, SQLSMALLINT type);

    virtual Cursor* prepareQuery(const QString& statement = QString::null, uint cursor_options = 0);
    virtual QString escapeString(const QString& str) const;
    virtual QByteArray escapeString(const QByteArray& str) const;
    virtual Cursor* prepareQuery(QuerySchema& query, uint cursor_options);

    PreparedStatement::Ptr prepareStatement(PreparedStatement::StatementType type,
							    FieldList& fields);

    void setODBCError( int nativeErrorCode, const QString& odbcErrorCode ,const QString& errorMessage);

    // reimplemented from KexiDB::Object
    //! Sets the server error message
    virtual QString serverErrorMsg();

    //! Sets the native error code returned by the server
    virtual int serverResult();

    //! Sets the ODBC error code :)
    virtual QString serverResultName();

protected:
    /*! Used by driver */
    ODBCConnection(Driver *driver,  ConnectionData &conn_data);

    virtual bool drv_connect(KexiDB::ServerVersionInfo& version);

    virtual bool drv_disconnect();

    /*! Creates new database using connection. Note: Do not pass \a dbName
      arg because for file-based engine (that has one database per connection)
      it is defined during connection. */
    virtual bool drv_createDatabase(const QString &dbName = QString());

    /*! Opens existing database using connection. Do not pass \a dbName
      arg because for file-based engine (that has one database per connection)
      it is defined during connection. If you pass it,
      database file name will be changed. */
    virtual bool drv_useDatabase(const QString &dbName = QString(), bool *cancelled = 0,
                                 MessageHandler* msgHandler = 0);

    virtual bool drv_closeDatabase();

    /*! Drops database from the server using connection.
      After drop, database shouldn't be accessible
      anymore, so database file is just removed. See note from drv_useDatabase(). */
    virtual bool drv_dropDatabase(const QString &dbName = QString());

    //virtual bool drv_createTable( const KexiDB::Table& table );

    virtual bool drv_executeSQL(const QString& statement);

    virtual quint64 drv_lastInsertRowID();

    virtual bool drv_getDatabasesList(QStringList &list);

//TODO: move this somewhere to low level class (MIGRATION?)
    virtual bool drv_getTablesList(QStringList &list);
//TODO: move this somewhere to low level class (MIGRATION?)
    virtual bool drv_containsTable(const QString &tableName);

private:

    friend class ODBCDriver;
    friend class ODBCCursor;

    ODBCConnectionInternal *d;
};

}

#endif
