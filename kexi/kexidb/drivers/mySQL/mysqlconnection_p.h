/* This file is part of the KDE project
   Copyright (C) 2004 Martin Ellis <martin.ellis@kdemail.net>

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
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
*/

#ifndef KEXIDB_MYSQLCLIENT_P_H
#define KEXIDB_MYSQLCLIENT_P_H

#ifdef Q_WS_WIN
#include <mysql/config-win.h>
#endif
#include <mysql.h>

typedef struct st_mysql MYSQL;

class QCString;
class QString;

#ifdef MYSQLMIGRATE_H
#define NAMESPACE KexiMigration
#else
#define NAMESPACE KexiDB
#endif

namespace NAMESPACE {

class ConnectionData;

//! Internal MySQL connection data.
/*! Provides a low-level API for accessing MySQL databases, that can
    be shared by any module that needs direct access to the underlying
    database.  Used by the KexiDB and KexiMigration drivers.
 */
class MySqlConnectionInternal
{
	public:
		MySqlConnectionInternal();
		~MySqlConnectionInternal();
		//! Connect to a MySQL database
		bool db_connect(const KexiDB::ConnectionData& data);
		//! Disconnect from the database
		bool db_disconnect();

		//! Select a database that is about to be used
		bool useDatabase(const QString &dbName = QString::null);
		
		//! Execute SQL statement on the database
		bool executeSQL( const QString& statement );

		//! Stores last result's error status
		void storeError();

		//! Escape a table, database or column name
		QString escapeIdentifier(const QString& str) const;

		MYSQL *mysql;
		QString errmsg; //<! server-specific message of last operation
		int res; //<! result code of last operation on server
};


//! Internal MySQL cursor data.
/*! Provides a low-level abstraction for iterating over MySql result sets.
 */

class MySqlCursorData : public MySqlConnectionInternal
{
	public:
		MySqlCursorData()
		: MySqlConnectionInternal()
		, mysqlres(0)
		, mysqlrow(0)
		, lengths(0)
		, numRows(0)
		{
		}
		~MySqlCursorData()
		{
		}

		MYSQL_RES *mysqlres;
		MYSQL_ROW mysqlrow;
		unsigned long *lengths;
		unsigned long numRows;
};

}

#endif
