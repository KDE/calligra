/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDB_CONNECTION_H
#define KEXIDB_CONNECTION_H

#include <qobject.h>
#include <qstringlist.h>
#include <qdict.h>
#include <qptrdict.h>

#include <kexidb/object.h>

#include <kexidb/connectiondata.h>
#include <kexidb/table.h>

namespace KexiDB {

class Cursor;
class Driver;

/*! This class represents database connection established with given database source.
*/
class KEXI_DB_EXPORT Connection : public QObject, public KexiDB::Object
{
	Q_OBJECT

	public:

		/*! Opened connection is automatically disconnected and removed 
			from driver's connections list. */
		virtual ~Connection();

		/*! \return parameters that had been used for create this connection. */
		ConnectionData& data() { return m_data; }

		/*! return driver that is source for this connection. */
		Driver* driver() const { return m_driver; }

		/*! Connects to driver with given parameters. 
			\return true if successfull. */
		bool connect();

		/*! return if connection is properly estableshed. */
		bool isConnected() { return m_is_connected; }

		/*! Disconnects to driver with given parameters. 
			\return true if successfull. */
		bool disconnect();

		/*! \return list of database names for opened connection. */
		QStringList databaseNames();

		/*! \return true if database \a dbName exists. */
		bool databaseExists( const QString &dbName );

		/*! Creates new database with name \a dbName, using this connection.
			If database with \a dbName already exists, or other error occured,
			false is returned. */
		bool createDatabase( const QString &dbName );

		/*! Opens existing database \a dbName using this connection. */
		bool useDatabase( const QString &dbName );

		/*! Closes currently used database for this connection */
		bool closeDatabase();

		/*! \return name of currently used database for this connection or empty string
			if there is no used database */
		QString currentDatabase() { return m_usedDatabase; }

		/*! Drop database with name \a dbName, using this connection. */
		bool dropDatabase( const QString &dbName = QString::null );

		/*! \return names of all database tables (except system tables). */
		QStringList tableNames();

		bool beginTransaction();
		bool commitTransaction();
		bool rollbackTransaction();
		bool duringTransaction();

		/*! driver-specific string escaping */
		virtual QString escapeString(const QString& str) = 0;
		virtual QCString escapeString(const QCString& str) = 0;

		virtual Cursor* executeQuery( const QString& statement = QString::null) = 0;

		/*! Deletes cursor \a cursor previously created by functions like executeQuery() 
			for this connection.
			There is an attempt to close the cursor with Cursor::close() if it was opened. 
			Anyway, at last cursor is deleted. 
			\returns true if cursor is properly closed before deletion. */
		bool deleteCursor(Cursor *cursor);

		/*! \return schema of \a tablename table retrieved using connection. */
		Table* tableSchema( const QString& tableName );

		QString valueToSQL( const Field::Type ftype, QVariant& v );

	protected:
		/*! Used by Driver */
		Connection( Driver *driver, const ConnectionData &conn_data );

		/*! For reimplemenation: connects to database
			\return true on success. */
		virtual bool drv_connect() = 0;

		/*! For reimplemenation: disconnects database
			\return true on success. */
		virtual bool drv_disconnect() = 0;

		/*! For reimplemenation: loads list of databases available for connection
			and adds the names to \a list. */
		virtual void drv_getDatabasesList( QStringList &list ) = 0;

		/*! For reimplemenation: creates new database using connection */
		virtual bool drv_createDatabase( const QString &dbName = QString::null ) = 0;

		/*! For reimplemenation: opens existing database using connection */
		virtual bool drv_useDatabase( const QString &dbName = QString::null ) = 0;

		/*! For reimplemenation: closes previously opened database 
			using connection. */
		virtual bool drv_closeDatabase() = 0;

		/*! For reimplemenation: drops database from the server
			using connection. After drop, database shouldn't be accessible 
			anymore. */
		virtual bool drv_dropDatabase( const QString &dbName = QString::null ) = 0;

		QString createTableStatement( const KexiDB::Table& table );

		virtual bool drv_createTable( const KexiDB::Table& table );
		/* Executes query \a statement and returns resulting rows 
			(used mostly for SELECT query). */
//		virtual bool drv_executeQuery( const QString& statement ) = 0;
		/* Executes query \a statement, but without returning resulting 
			rows (used mostly for functional queries). */
		virtual bool drv_executeSQL( const QString& statement ) = 0;

		virtual bool drv_beginTransaction();
		virtual bool drv_commitTransaction();
		virtual bool drv_rollbackTransaction();
		virtual bool drv_duringTransaction();



		/*! Creates cursor data and initializes cursor 
			using \a statement for later data retrieval. */
//		virtual CursorData* drv_createCursor( const QString& statement ) = 0;
		/*! Closes and deletes cursor data. */
//		virtual bool drv_deleteCursor( CursorData *data ) = 0;
		
		/*! Helper: checks if connection is established; 
			if not: error message is set up and false returned */
		bool checkConnected();

		Driver *m_driver;
		ConnectionData m_data;
		QString m_name;
		bool m_is_connected : 1;
		bool m_transaction : 1;

		QString m_usedDatabase; //! database name that is opened now

		//! tables schemas retrieved on demand with tableSchema()
		QDict<Table> m_tables;

		//! cursors created for this connection
		QPtrDict<KexiDB::Cursor> m_cursors;
//		ConnectionInternal* m_internal;
	friend class KexiDB::Driver;
	friend class KexiDB::Cursor;

	private:
		class Private;
		Private *d;
};

} //namespace KexiDB

#endif

