/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
Daniel Molkentin <molkentin@kde.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
*/

#ifndef MYSQLDB_H
#define MYSQLDB_H

#include <qstringlist.h>

#include <kexidb.h>
#include <kexidbresult.h>
#include <kexidberror.h>

typedef struct st_mysql MYSQL;

class MySqlRecord;

/*!
 * should overwrite kexiDB/kexiDB
 * all other members ar done by the
 * baseclass
 */

class MySqlDB : public KexiDB
{
	Q_OBJECT

	public:
		MySqlDB(QObject *parent=0, const char *name="mysq", const QStringList &args=QStringList());
		~MySqlDB();

		virtual KexiDBError *latestError();

		/*!
		 *  connect to mysql-database
		 */

//		int		connect(const char *host, const char *user, const char *passwd,
//					const char *db, unsigned int port = 0, const char *unix_socket = 0, unsigned int client_flag = 0);

	public slots:
		QString		driverName();

		KexiDBRecord	*queryRecord(QString querystatement, bool buffer=false);

		bool		connect(QString host, QString user, QString password, QString socket, QString port);
		bool		connect(QString host, QString user, QString password, QString socket, QString port,
			QString db, bool create = false);

		QStringList	databases();
		QStringList	tables();


		/*!
		 * execute a query
		 */
		bool		query(QString statement);

		/**
		 * us that function if you don't want to catch exceptions :)
		 */
		bool		uhQuery(QString statement);
//		int		realQuery(const char *statement, unsigned int length);

		QString		escape(const QString &str);
		QString		escape(const QByteArray& str);
		bool alterField(const QString& table, const QString& field, const QString& newFieldName,
			KexiDBField::ColumnType dtype, int length, int precision, KexiDBField::ColumnConstraints constraints,
			bool binary, bool unsignedType, const QString& defaultVal);
		bool createField(const QString& table, const QString& field, KexiDBField::ColumnType dtype,
			int length, int precision, KexiDBField::ColumnConstraints constraints, bool binary, bool unsignedType,
			const QString& defaultVal);
		bool alterField(const KexiDBField& changedField,
			unsigned int index, KexiDBTableStruct fields);
		bool createField(const KexiDBField& newField,
			KexiDBTableStruct fields, bool createTable = false);

		/*!
		 *  friendy mode
		 */
		KexiDBResult	*getResult();

		/*!
		 * mysql_store_result
		 */
		KexiDBResult	*storeResult();
		KexiDBResult	*useResult();

		unsigned long	affectedRows();

		unsigned long	lastAuto();

		KexiDBTableStruct	getStructure(const QString& table);
		QString	getNativeDataType(const KexiDBField::ColumnType& t);
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

		KexiDBError m_error;
};

#endif
