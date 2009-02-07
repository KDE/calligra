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

#ifndef KEXIDB_ODBCCLIENT_P_H
#define KEXIDB_ODBCCLIENT_P_H

//ODBC includes
#include <sql.h>
#include <sqlext.h>

// Kexi Includes
#include <kexidb/connection_p.h>

#ifdef ODBCMIGRATE_H
#define NAMESPACE KexiMigration
#else
#define NAMESPACE KexiDB
#endif

namespace KexiDB
{
class ConnectionData;
}

namespace NAMESPACE
{

//! Internal ODBC connection data.
/*! Provides a low-level API for accessing ODBC databases, that can
    be shared by any module that needs direct access to the underlying
    database.  Used by the KexiDB and KexiMigration drivers.
 */
class ODBCConnectionInternal : public KexiDB::ConnectionInternal
{
public:
    ODBCConnectionInternal(KexiDB::Connection* connection);
    virtual ~ODBCConnectionInternal();

    //! Connects to a ODBC database
    bool db_connect(const KexiDB::ConnectionData& data);

    //! Disconnects from the database
    bool db_disconnect();

/*     //! Selects a database that is about to be used */
/*     bool useDatabase(const QString &dbName = QString()); */

    //! Execute SQL statement on the database
    bool executeSQL(const QString& statement);

    //! Stores last operation's result
    virtual void storeResult();

/*     //! Escapes a table, database or column name */
/*     QString escapeIdentifier(const QString& str) const; */

     // TODO. Need to put some stuff here for error handling in ODBC.

    SQLHENV envHandle;
    SQLHDBC connectionHandle;

    QString odbcErrorCode;
    QString odbcErrorText; 
    int nativeErrorCode; //!< result code of last operation on server
};


//! Internal ODBC cursor data.
/*! Provides a low-level abstraction for iterating over ODBC result sets. */
class ODBCCursorData : public ODBCConnectionInternal
{
public:
    ODBCCursorData(KexiDB::Connection* connection);
    virtual ~ODBCCursorData();

    SQLHSTMT statementHandle;
    QString sql;

/*     unsigned long numRows; */
};

}

#endif
