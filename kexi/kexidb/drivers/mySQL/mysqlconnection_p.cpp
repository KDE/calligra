/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>
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
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <q3cstring.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>

#include <kdebug.h>

#include "mysqlconnection_p.h"

#include <kexidb/connectiondata.h>

#ifdef MYSQLMIGRATE_H
#define NAMESPACE KexiMigration
#else
#define NAMESPACE KexiDB
#endif

using namespace NAMESPACE;

/* ************************************************************************** */
MySqlConnectionInternal::MySqlConnectionInternal(KexiDB::Connection* connection)
	: ConnectionInternal(connection)
	, mysql(0)
	, mysql_owned(true)
	, res(0)
{
}

MySqlConnectionInternal::~MySqlConnectionInternal()
{
	if (mysql_owned && mysql) {
		mysql_close(mysql);
		mysql = 0;
	}
}

void MySqlConnectionInternal::storeResult()
{
	res = mysql_errno(mysql);
	errmsg = mysql_error(mysql);
}

/* ************************************************************************** */
/*! Connects to the MySQL server on host as the given user using the specified
    password.  If host is "localhost", then a socket on the local file system
    can be specified to connect to the server (several defaults will be tried if
    none is specified).  If the server is on a remote machine, then a port is 
    the port that the remote server is listening on.
 */
//bool MySqlConnectionInternal::db_connect(QCString host, QCString user,
//  QCString password, unsigned short int port, QString socket)
bool MySqlConnectionInternal::db_connect(const KexiDB::ConnectionData& data)
{
	if (!(mysql = mysql_init(mysql)))
		return false;

	KexiDBDrvDbg << "MySqlConnectionInternal::connect()" << endl;
	Q3CString localSocket;
	QString hostName = data.hostName;
	if (hostName.isEmpty() || hostName.lower()=="localhost") {
		if (data.useLocalSocketFile) {
			if (data.localSocketFileName.isEmpty()) {
	//! @todo move the list of default sockets to a generic method
				QStringList sockets;
	#ifndef Q_WS_WIN
				sockets.append("/var/lib/mysql/mysql.sock");
				sockets.append("/var/run/mysqld/mysqld.sock");
				sockets.append("/tmp/mysql.sock");
		
				for(QStringList::ConstIterator it = sockets.constBegin(); it != sockets.constEnd(); it++)
				{
					if(QFile(*it).exists()) {
						localSocket = ((QString)(*it)).local8Bit();
						break;
					}
				}
	#endif
			}
			else
				localSocket = QFile::encodeName(data.localSocketFileName);
		}
		else {
			//we're not using local socket
			hostName = "127.0.0.1"; //this will force mysql to connect to localhost
		}
	}

/*! @todo is latin1() encoding here valid? what about using UTF for passwords? */
	const char *pwd = data.password.isNull() ? 0 : data.password.latin1();
	mysql_real_connect(mysql, hostName.latin1(), data.userName.latin1(), 
		pwd, 0, data.port, localSocket, 0);
	if(mysql_errno(mysql) == 0)
		return true;

	storeResult(); //store error msg, if any - can be destroyed after disconnect()
	db_disconnect();
//	setError(ERR_DB_SPECIFIC,err);
	return false;
}

/*! Disconnects from the database.
 */
bool MySqlConnectionInternal::db_disconnect()
{
	mysql_close(mysql);
	mysql = 0;
	KexiDBDrvDbg << "MySqlConnection::disconnect()" << endl;
	return true;
}

/* ************************************************************************** */
/*! Selects dbName as the active database so it can be used.
 */
bool MySqlConnectionInternal::useDatabase(const QString &dbName) {
//TODO is here escaping needed?
	return executeSQL("USE " + dbName);
}

/*! Executes the given SQL statement on the server.
 */
bool MySqlConnectionInternal::executeSQL(const QString& statement) {
//	KexiDBDrvDbg << "MySqlConnectionInternal::executeSQL: "
//	             << statement << endl;
	Q3CString queryStr=statement.utf8();
	const char *query=queryStr;
	if(mysql_real_query(mysql, query, strlen(query)) == 0)
	{
		return true;
	}

	storeResult();
//	setError(ERR_DB_SPECIFIC,mysql_error(m_mysql));
	return false;
}

QString MySqlConnectionInternal::escapeIdentifier(const QString& str) const {
	return QString(str).replace('`', "'");
}

//--------------------------------------

MySqlCursorData::MySqlCursorData(KexiDB::Connection* connection)
: MySqlConnectionInternal(connection)
, mysqlres(0)
, mysqlrow(0)
, lengths(0)
, numRows(0)
{
	mysql_owned = false;
}

MySqlCursorData::~MySqlCursorData()
{
}

