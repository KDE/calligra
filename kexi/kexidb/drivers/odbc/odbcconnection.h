/* This file is part of the KDE project
   Copyright (C) 2004 Matt Rogers <matt.rogers@kdemail.net>

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

class ODBCConnectionPrivate;

namespace KexiDB
{
class Driver;


class ODBCConnection : public Connection
{
    Q_OBJECT

public:
    ~ODBCConnection();

    virtual Cursor* prepareQuery(const QString& statement = QString::null, uint cursor_options = 0);
    virtual QString escapeString(const QString& str) const;
    virtual QByteArray escapeString(const QByteArray& str) const;

protected:
    /*! Used by driver */
    ODBCConnection(Driver *driver,  ConnectionData &conn_data);

    virtual bool drv_connect();

    virtual bool drv_disconnect();

    virtual bool drv_getDatabasesList(QStringList &list);

    /*! Creates new database using connection. Note: Do not pass \a dbName
      arg because for file-based engine (that has one database per connection)
      it is defined during connection. */
    virtual bool drv_createDatabase(const QString &dbName = QString::null);

    /*! Opens existing database using connection. Do not pass \a dbName
      arg because for file-based engine (that has one database per connection)
      it is defined during connection. If you pass it,
      database file name will be changed. */
    virtual bool drv_useDatabase(const QString &dbName = QString::null);

    virtual bool drv_closeDatabase();

    /*! Drops database from the server using connection.
      After drop, database shouldn't be accessible
      anymore, so database file is just removed. See note from drv_useDatabase(). */
    virtual bool drv_dropDatabase(const QString &dbName = QString::null);

    //virtual bool drv_createTable( const KexiDB::Table& table );

    virtual bool drv_executeSQL(const QString& statement);

    friend class ODBCDriver;

private:
    ODBCConnectionPrivate *d;
};



}

#endif
