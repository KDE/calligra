/*
   This file is part of the KDE project
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
//unixODBC Includes
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

//QT Includes
#include <qfile.h>
#include <qdir.h>

//KDE Includes
#include <kgenericfactory.h>
#include <kdebug.h>

//Kexi Includes
#include <kexidb/driver.h>
#include <kexidb/cursor.h>
#include <kexidb/error.h>

//Local Includes
#include "odbcconnection.h"

using namespace KexiDB;

//! @internal
class ODBCConnectionPrivate
{
public:
    ConnectionData connData;
    QString currentDB;
    SQLHENV envHandle;
    SQLHDBC connectionHandle;
};

ODBCConnection::ODBCConnection(Driver *driver, ConnectionData &conn_data)
        : Connection(driver, conn_data)
{
    d = new ODBCConnectionPrivate;
    //d->connData = conn_data;
}

Cursor* ODBCConnection::prepareQuery(const QString& statement, uint cursor_options)
{
    Q_UNUSED(statement);
    Q_UNUSED(cursor_options);
    return 0;
}

QString ODBCConnection::escapeString(const QString& str) const
{
    return str;
}

QByteArray ODBCConnection::escapeString(const QByteArray& str) const
{
    return str;
}

bool ODBCConnection::drv_connect()
{
    long result;

    result = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &d->envHandle);
    if (result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO)
        return false;

    //We'll use ODBC 3.5 by default, so just get connection handle
    result = SQLAllocHandle(SQL_HANDLE_DBC, d->envHandle, &d->connectionHandle);
    if (result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_ENV, d->envHandle);
        return false;
    }

    result = SQLConnect(d->connectionHandle, (unsigned char*) d->connData.hostName.toLatin1(),
                        d->connData.hostName.length(), (unsigned char*) d->connData.userName.toLatin1(),
                        d->connData.userName.length(), (unsigned char*) d->connData.password.toLatin1(),
                        d->connData.password.length());
    if (result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_DBC, d->connectionHandle);
        SQLFreeHandle(SQL_HANDLE_ENV, d->envHandle);
        return false;
    }

    return true;
}

bool ODBCConnection::drv_disconnect()
{
    SQLDisconnect(d->connectionHandle);
    SQLFreeHandle(SQL_HANDLE_DBC, d->connectionHandle);
    SQLFreeHandle(SQL_HANDLE_ENV, d->envHandle);
    return true;
}

bool ODBCConnection::drv_getDatabasesList(QStringList &)
{
    return false;
}

bool ODBCConnection::drv_createDatabase(const QString &)
{
    return false;
}

bool ODBCConnection::drv_useDatabase(const QString &)
{
    return false;
}

bool ODBCConnection::drv_closeDatabase()
{
    return false;
}

bool ODBCConnection::drv_dropDatabase(const QString &)
{
    return false;
}

bool ODBCConnection::drv_executeSQL(const QString &)
{
    return false;
}

ODBCConnection::~ODBCConnection()
{
    drv_disconnect();
    destroy();
    delete d;
}

#include "odbcconnection.moc"

