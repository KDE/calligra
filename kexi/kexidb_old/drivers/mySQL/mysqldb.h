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

#include "../../kexiDB/kexidb.h"
#include "../../kexiDB/kexidbresult.h"

typedef struct st_mysql MYSQL;


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
		
		/*!
		 *  connect to mysql-database
		 */
		 
//		int		connect(const char *host, const char *user, const char *passwd,
//					const char *db, unsigned int port = 0, const char *unix_socket = 0, unsigned int client_flag = 0);

		bool		connect(QString host, QString user, QString password);
		bool		connect(QString host, QString user, QString password, QString db);

		QStringList	databases();
		QStringList	tables();
		
		/*!
		 *  get the last error
		 */
		QString		error();
		
		/*!
		 * execute a query
		 */
		int		query(QString statement);
		int		realQuery(const char *statement, unsigned int length);
		
		QString		realEscape(const QString &str);
		QByteArray	realEscape(const QByteArray &a); 

		/*!
		 * mysql_store_result
		 */
		KexiDBResult	*storeResult();
		KexiDBResult	*useResult();
		KexiDBResult	*listProcesses();
		
		int		threadID();
		 
		unsigned long	insertID();
		unsigned long	affectedRows();

	protected:
		int		reconnect();
		void		initCheckUpdate();
		
		MYSQL		*m_mysql;
		bool		m_connected;
		bool		m_connectedDB;
		const char	*m_host;
		const char	*m_user;
		const char	*m_passwd;
		const char	*m_db;
		unsigned int    m_port;
		const char	*m_unix_socket;
		unsigned int	m_client_flag;
};

#endif
