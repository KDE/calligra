/* This file is part of the KDE project
   Copyright (C) 2008 Julia Sanchez-Simon <hithwen@gmail.com>
   Copyright (C) 2008 Miguel Angel Aragüez-Rey <fizban87@gmail.com>

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
#ifndef ORACLECONNECTION_H
#define ORACLECONNECTION_H
#include <kexidb/connection.h>
#include <qstringlist.h>
#include "oraclecursor.h"
#include "oracleconnection_p.h"

namespace KexiDB {

//! Oracle database connection
class OracleConnection : public Connection
{
	Q_OBJECT

	public:
		virtual ~OracleConnection();

// TODO: Do we need this?
		virtual Cursor* prepareQuery( const QString& statement = QString::null, uint cursor_options = 0 );
		virtual Cursor* prepareQuery( QuerySchema& query, uint cursor_options = 0 );
		virtual PreparedStatement::Ptr prepareStatement(PreparedStatement::StatementType type, 
			FieldList& fields);

	protected:
		//! Used by Driver
		OracleConnection(Driver *driver, ConnectionData &conn_data);

		virtual bool drv_connect(KexiDB::ServerVersionInfo& version);
		virtual bool drv_disconnect();

// TODO: Check these
		virtual bool drv_createDatabase( const QString &dbName = QString::null );
		virtual bool drv_useDatabase( const QString &dbName = QString::null, 
                          bool *cancelled = 0, MessageHandler* msgHandler = 0 );
		virtual bool drv_closeDatabase();
		virtual bool drv_dropDatabase( const QString &dbName = QString::null );
// End check

		virtual bool drv_executeSQL( const QString& statement );
		virtual Q_ULLONG drv_lastInsertRowID();

		virtual int serverResult();
		virtual QString serverResultName();
		virtual QString serverErrorMsg();
		virtual void drv_clearServerResult();
		
		//Experiments
		virtual bool drv_databaseExists(const QString &dbName,bool ignoreErrors=true);
		virtual TransactionData* drv_beginTransaction();
		virtual bool drv_setAutoCommit(bool on);
		virtual bool drv_getDatabasesList( QStringList &list );

//TODO: move this somewhere to low level class (MIGRATION?)
		virtual bool drv_getTablesList( QStringList &list );
//TODO: move this somewhere to low level class (MIGRATION?)
		virtual bool drv_containsTable( const QString &tableName );

		OracleConnectionInternal *d;

		friend class OracleDriver;
		friend class OracleCursor;
	private:
		oracle::occi::SQLException ea;
		bool active;
	
}; // class OracleConnection

} // namespace KexiDB

#endif // KEXIDB_ORACLE_CONNECTION_H

