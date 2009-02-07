/*
   This file is part of the KDE project
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
#include "odbcconnection_p.h"
#include "odbccursor.h"
#include "odbcpreparedstatement.h"
#include "odbcdriver.h"
#include "odbcqueryunit.h"
#include "odbcsqlqueryunit.h"
#include "odbctablesqueryunit.h"

using namespace KexiDB;

void ODBCConnection::extractError(ODBCConnection* connection, SQLHANDLE handle, SQLSMALLINT type)
{
    SQLINTEGER i = 0;
    SQLINTEGER native;
    SQLCHAR state[ 7 ];
    SQLCHAR text[256];
    SQLSMALLINT len;
    SQLRETURN ret;

    do
    {
      ret = SQLGetDiagRec(type, handle, ++i, state, &native, text, sizeof(text), &len );
      if (SQL_SUCCEEDED(ret)) {
          // send the native error code and the text
          connection->setODBCError( native, QString::fromLatin1((const char*)state), QString::fromLatin1((const char*)text));
          KexiDBDrvDbg<<( const char * )state<<":"<<i<<":"<<native<<":"<<( const char* )text;
      }
    } while( ret == SQL_SUCCESS );
}

ODBCConnection::ODBCConnection(Driver *driver, ConnectionData &conn_data)
        : Connection(driver, conn_data)
         , d(new ODBCConnectionInternal(this))
{
}

Cursor* ODBCConnection::prepareQuery(const QString& statement, uint cursor_options)
{
    ODBCSQLQueryUnit* queryUnit = new ODBCSQLQueryUnit( this );
    return new ODBCCursor(this, queryUnit, statement, cursor_options);
}

Cursor* ODBCConnection::prepareQuery(QuerySchema& query, uint cursor_options)
{
    ODBCSQLQueryUnit* queryUnit = new ODBCSQLQueryUnit( this );
    return new ODBCCursor(this, queryUnit, query, cursor_options);
}

QString ODBCConnection::escapeString(const QString& str) const
{
    // TODO : escape strings
    return str;
}

QByteArray ODBCConnection::escapeString(const QByteArray& str) const
{
    // TODO : escape strings
    return str;
}

bool ODBCConnection::drv_connect(KexiDB::ServerVersionInfo& version)
{
    const bool ok = d->db_connect(*data());
    if (!ok)
        return false;

    SQLCHAR stringVal[1024];
    SQLSMALLINT stringLen;
    SQLRETURN returnStatus;

    // get the database name
    returnStatus = SQLGetInfo( d->connectionHandle, SQL_DATABASE_NAME, stringVal, sizeof(stringVal ), &stringLen );
    if ( !SQL_SUCCEEDED(returnStatus) ) {
        ODBCConnection::extractError( this , d->connectionHandle , SQL_HANDLE_DBC );
        d->db_disconnect();
        return false;
    }
    version.string = QString::fromLatin1( ( const char* )stringVal );

    returnStatus = SQLGetInfo( d->connectionHandle, SQL_SERVER_NAME, stringVal, sizeof(stringVal ), &stringLen );
    if ( !SQL_SUCCEEDED(returnStatus) ) {
        ODBCConnection::extractError( this , d->connectionHandle , SQL_HANDLE_DBC );
        d->db_disconnect();
        return false;
    }
    version.string += QString::fromLatin1( ( const char* )stringVal );

    // get the database version. the database version retrieved is of the form ##.##.####, where the first two digits are the major version, the next two digits are the minor version, and the last four digits are the release version.
    returnStatus = SQLGetInfo( d->connectionHandle,  SQL_DBMS_VER , stringVal, sizeof(stringVal ), &stringLen );
    if ( !SQL_SUCCEEDED(returnStatus) ) {
        ODBCConnection::extractError( this , d->connectionHandle , SQL_HANDLE_DBC );
        d->db_disconnect();
        return false;
    }

    QString versionString = QString::fromLatin1( ( const char* )stringVal );
    QRegExp versionRe("(\\d+)\\.(\\d+)\\.(\\d+)");
    if ( versionRe.exactMatch(versionString) ) {
        version.major = versionRe.cap(1).toInt();
        version.minor = versionRe.cap(2).toInt();
        version.release = versionRe.cap(3).toInt();
    }

    ODBCDriver* odbcDriver = dynamic_cast<ODBCDriver*>(driver());
    if ( odbcDriver )
        return odbcDriver->updateDriverInfo( this );
    else
        return false;
}

bool ODBCConnection::drv_disconnect()
{
    return d->db_disconnect();
}

bool ODBCConnection::drv_createDatabase(const QString& )
{
    return false;
}

bool ODBCConnection::drv_useDatabase(const QString& , bool* /*cancelled*/, MessageHandler* /*msgHandler*/)
{
    return true;
}

bool ODBCConnection::drv_closeDatabase()
{
    return true;
}

bool ODBCConnection::drv_dropDatabase(const QString &)
{
    return true;
}

bool ODBCConnection::drv_executeSQL(const QString& statement)
{
    return d->executeSQL(statement);
}

bool ODBCConnection::drv_getTablesList(QStringList &list)
{
   ODBCTablesQueryUnit* queryUnit = new ODBCTablesQueryUnit( this );
   Cursor* cursor = new ODBCCursor( this, queryUnit );

   if ( !cursor->open() || !cursor->moveFirst() ) {
       return false;
   }

   while ( !cursor->eof() ) {
       // the third column is the table name.
       QString tableName = cursor->value( 2 ).toString();
       list<<tableName;
        if (!cursor->moveNext() && cursor->error()) {
            setError(cursor);
            deleteCursor(cursor);
            return false;
        }
   }

   return deleteCursor( cursor );
}

quint64 ODBCConnection::drv_lastInsertRowID()
{
    // TODO: One huge hack. Will work for mysql ( and perhaps sybase too )
    QString queryToExecute;
    ODBCDriver* odbcDriver = dynamic_cast<ODBCDriver*>(driver());
    if ( odbcDriver )
        queryToExecute = odbcDriver->getQueryForOID();

    int rowId;
    querySingleNumber( queryToExecute , rowId);

    return rowId;
}

bool ODBCConnection::drv_getDatabasesList(QStringList &list)
{
    KexiDBDrvDbg << "ODBCConnection::drv_getDatabasesList()";
    // we return the "Data Sources" list here, actually.
    SQLCHAR dsn[256];
    SQLCHAR desc[256];
    SQLSMALLINT dsnRet;
    SQLSMALLINT descRet;
    SQLUSMALLINT direction;
    SQLRETURN returnStatus;

    direction = SQL_FETCH_FIRST;
    while(SQL_SUCCEEDED(returnStatus = SQLDataSources(d->envHandle, direction,
                                             dsn, sizeof(dsn), &dsnRet,
                                             desc, sizeof(desc), &descRet))) {
        direction = SQL_FETCH_NEXT;
        KexiDBDrvDbg << ( const char* )dsn <<" - "<< ( const char* )desc;
        list << QString::fromLatin1((const char*)dsn);
    }

    return true;
}

bool ODBCConnection::drv_containsTable(const QString& tableName)
{
   ODBCTablesQueryUnit* queryUnit = new ODBCTablesQueryUnit( this );
   queryUnit->setFilter( tableName );
   Cursor* cursor = new ODBCCursor( this, queryUnit );

   if ( !cursor->open() ) {
       setError( cursor );
       return false;
   }

   bool recordExists = true;
   // we are expecting one record :)
   if ( !cursor->moveFirst() ) {
       recordExists = false;
   }
   deleteCursor( cursor );

   return recordExists;
}

ODBCConnection::~ODBCConnection()
{
    drv_disconnect();
    destroy();
    delete d;
}

void ODBCConnection::setODBCError( int nativeErrorCode, const QString& odbcErrorCode, const QString& errorMessage) {
    d->nativeErrorCode = nativeErrorCode;
    d->odbcErrorText = errorMessage;
    d->odbcErrorCode = odbcErrorCode;
}

QString ODBCConnection::serverErrorMsg() {
    return d->odbcErrorText;
}

int ODBCConnection::serverResult() {
    return d->nativeErrorCode;
}

QString ODBCConnection::serverResultName() {
    return d->odbcErrorCode;
}

PreparedStatement::Ptr ODBCConnection::prepareStatement(PreparedStatement::StatementType type,
        FieldList& fields)
{
    return KSharedPtr<PreparedStatement>(new ODBCPreparedStatement(type, *d, fields));
}

#include "odbcconnection.moc"

