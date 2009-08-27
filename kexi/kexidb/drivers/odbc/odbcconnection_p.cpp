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

#include "odbcconnection_p.h"

// Qt includes
#include <QByteArray>
#include <QStringList>
#include <QFile>

// KDE Includes
#include <KDebug>

// KexiDB includes
#include <kexidb/connectiondata.h>
#include <odbcconnection.h>

using namespace NAMESPACE;

// TODO: This should probably be made a class static function of ODBCConnection
// and should take a pointer to ODBCConnection as the first argument.

/* ************************************************************************** */
ODBCConnectionInternal::ODBCConnectionInternal(KexiDB::Connection* connection)
        : ConnectionInternal(connection)
{
}

ODBCConnectionInternal::~ODBCConnectionInternal()
{
}

void ODBCConnectionInternal::storeResult()
{
}

/* ************************************************************************** */
/*! Connects to the ODBC data source with given user using the specified
    password.
 */
bool ODBCConnectionInternal::db_connect(const KexiDB::ConnectionData& data)
{
    ODBCConnection* odbcConnection = dynamic_cast<ODBCConnection*>( this->connection );
    if ( !odbcConnection )
        return false;

    SQLRETURN returnStatus;
    returnStatus = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &envHandle);
    if ( !SQL_SUCCEEDED(returnStatus) ) {
        KexiDBDrvDbg <<"Couldn't allocate environment handle";
        return false;
    }

    SQLSetEnvAttr(envHandle, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);

    //We'll use ODBC 3.5 by default, so just get connection handle
    returnStatus = SQLAllocHandle(SQL_HANDLE_DBC, envHandle, &connectionHandle);
    if ( !SQL_SUCCEEDED(returnStatus) ) {
        ODBCConnection::extractError( odbcConnection, envHandle, SQL_HANDLE_ENV );
        SQLFreeHandle(SQL_HANDLE_ENV, envHandle);
        return false;
    }

    returnStatus = SQLConnect(connectionHandle, (unsigned char*) data.hostName.toLatin1().constData(),
                        data.hostName.length(), (unsigned char*) data.userName.toLatin1().constData(),
                        data.userName.length(), (unsigned char*) data.password.toLatin1().constData(),
                        data.password.length());

    if ( !SQL_SUCCEEDED(returnStatus ) ) {
        ODBCConnection::extractError( odbcConnection , connectionHandle, SQL_HANDLE_DBC );
        db_disconnect();
        return false;
    }

    return true;
}

/*! Disconnects from the database.
 */
bool ODBCConnectionInternal::db_disconnect()
{
    KexiDBDrvDbg << "ODBCConnection::disconnect()";

    ODBCConnection* odbcConnection = dynamic_cast<ODBCConnection*>( this->connection );
    if ( !odbcConnection )
        return false;

    SQLRETURN returnStatus = SQLDisconnect( connectionHandle );
    if ( !SQL_SUCCEEDED(returnStatus ) ) {
        ODBCConnection::extractError( odbcConnection , connectionHandle, SQL_HANDLE_DBC );
    }

    SQLFreeHandle(SQL_HANDLE_DBC, connectionHandle);
    SQLFreeHandle(SQL_HANDLE_ENV, envHandle);
    return true;
}

/* ************************************************************************** */
/*! Selects dbName as the active database so it can be used.
 */
// bool ODBCConnectionInternal::useDatabase(const QString &dbName)
// {
// //TODO is here escaping needed?
//     //return executeSQL("USE " + dbName);
//     return true;
// }

/*! Executes the given SQL statement on the server.
 */
bool ODBCConnectionInternal::executeSQL(const QString& statement)
{
    SQLRETURN returnStatus;

    ODBCConnection* odbcConnection = dynamic_cast<ODBCConnection*>( this->connection );
    if ( !odbcConnection )
        return false;

    QByteArray queryStr(statement.toUtf8());
    const char *query = queryStr.constData();

    SQLHSTMT statementHandle;
    // allocate a statement handle
    returnStatus = SQLAllocHandle( SQL_HANDLE_STMT, connectionHandle, &statementHandle );
    if ( !SQL_SUCCEEDED( returnStatus ) ) {
        ODBCConnection::extractError( odbcConnection, connectionHandle, SQL_HANDLE_DBC );
        return false;
    }

    kDebug() << QString::fromLatin1(query) ;
    returnStatus = SQLExecDirect( statementHandle, (SQLCHAR*)query, SQL_NTS );
    if ( !SQL_SUCCEEDED( returnStatus ) ) {
        ODBCConnection::extractError( odbcConnection , statementHandle, SQL_HANDLE_STMT );
        return false;
    }

    SQLFreeHandle( SQL_HANDLE_STMT, statementHandle );
    return true;
}

// QString ODBCConnectionInternal::escapeIdentifier(const QString& str) const
// {
//     // TODO
//     return str;
// }

//--------------------------------------

ODBCCursorData::ODBCCursorData(KexiDB::Connection* connection)
        : ODBCConnectionInternal(connection)
{

}

ODBCCursorData::~ODBCCursorData()
{
}

