/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
Copyright (C) 2003   Joseph Wenninger<jowenn@kde.org>

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

#ifndef MYSQLCONNECTION_H
#define MYSQLCONNECTION_H

#include <qstringlist.h>

#include <kexidb/connection.h>
#include "mysqlcursor.h"
#include <qdict.h>

//#define BOOL bool

typedef struct st_mysql MYSQL;

class MySqlRecordSet;
class MySqlResult;
/*!
 * should overwrite kexiDB/kexiDB
 * all other members ar done by the
 * baseclass
 */

namespace KexiDB {

class KEXIDB_MYSQL_DRIVER_EXPORT MySqlConnection : public Connection
{
	Q_OBJECT


        public:
                ~MySqlConnection();

                virtual Cursor* prepareQuery( const QString& statement = QString::null, uint cursor_options = 0 );
                virtual Cursor* prepareQuery( QuerySchema& query, uint cursor_options = 0 );
                virtual QString escapeString( const QString& str) const;
                virtual QCString escapeString( const QCString& str) const;

        protected:
		friend class MySqlDriver;
                /*! Used by driver */
                MySqlConnection( Driver *driver, const ConnectionData &conn_data );

                virtual bool drv_connect();
                virtual bool drv_disconnect();
                virtual bool drv_getDatabasesList( QStringList &list );
                virtual bool drv_createDatabase( const QString &dbName = QString::null );
                virtual bool drv_useDatabase( const QString &dbName = QString::null );
                virtual bool drv_closeDatabase();
                virtual bool drv_dropDatabase( const QString &dbName = QString::null );
                virtual bool drv_executeSQL( const QString& statement );
                virtual Q_ULLONG drv_lastInsertRowID();

	private:
		friend class MySqlCursor;
		MYSQL		*m_mysql;

#if 0
	public:
		MySqlDB(QObject *parent=0, const char *name="mysq", const QStringList &args=QStringList());
		~MySqlDB();

		virtual KexiDBError *latestError();

		/*!
		 *  connect to mysql-database
		 */

//		int		connect(const char *host, const char *user, const char *passwd,
//					const char *db, unsigned int port = 0, const char *unix_socket = 0, unsigned int client_flag = 0);
		QString		driverName()   const;
		QStringList	databases();
		virtual bool isSystemDatabase(QString &dbName);
		QStringList	tableNames();
		unsigned long	affectedRows() const;
		KexiDBTableStruct	structure(const QString& table) const;
		QString	nativeDataType(const KexiDBField::ColumnType& t) const;

		QString escapeName(const QString &tn);
		
	public _slots_:
		KexiDBRecordSet	*queryRecord(const QString& querystatement, bool buffer=false);

		bool		connect(const QString& host, const QString& user, const QString& password,
										const QString& socket, const QString& port);
		bool		connect(const QString& host, const QString& user, const QString& password,
										const QString& socket, const QString& port,
										const QString& db, bool create = false);

		void		load(QString, bool) { }
		const KexiDBTable* const table(const QString &name);

		/*!
		 * execute a query
		 */
		bool		query(const QString& statement);

		/**
		 * us that function if you don't want to catch exceptions :)
		 */
		bool		uhQuery(const QString& statement);
//		int		realQuery(const char *statement, unsigned int length);

		QString		escape(const QString &str);
		QString		escape(const QByteArray& str);
		bool alterField(const KexiDBField& changedField,
			unsigned int index, KexiDBTableStruct fields);
		bool createField(const KexiDBField& newField,
			KexiDBTableStruct fields, bool createTable = false);

		/*!
		 *  friendy mode
		 */
		MySqlResult	*getResult();

		/*!
		 * mysql_store_result
		 */
		MySqlResult	*storeResult();
		MySqlResult	*useResult();

		unsigned long	lastAuto();

		static KexiDBField::ColumnType getInternalDataType(int t);

	protected:
		int		reconnect();
		void		initCheckUpdate();

		MYSQL		*m_mysql;
		bool		m_connected;
		bool		m_connectedDB;
		unsigned int    m_port;
		QString		m_socket;
		unsigned int	m_client_flag;

		QString		m_host;
		QString		m_user;
		QString		m_password;

	private:
		QString createDefinition(const QString& field, KexiDBField::ColumnType dtype, int length, int precision,
			KexiDBField::ColumnConstraints constraints, bool binary, bool unsignedType, const QString& defaultVal);
		QString createDefinition(const KexiDBField& field,
			int index, KexiDBTableStruct fields);
		bool changeKeys(const KexiDBField& field,
			int index, KexiDBTableStruct fields);

		KexiDBTable * createTableDef(const QString& name);
		KexiDBError m_error;

		QDict<KexiDBTable> m_tableDefs;
#endif
};

}

#endif
