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

#include <mysql/mysql.h>

#include <qvariant.h>

#include <kgenericfactory.h>
#include <kdebug.h>

#include "../../kexiDB/kexidbresult.h"

#include "mysqldb.h"
#include "mysqlresult.h"

K_EXPORT_COMPONENT_FACTORY(keximysqlinterface, KGenericFactory<MySqlDB>( "mysqlinterface" ));

MySqlDB::MySqlDB(QObject *parent, const char *name, const QStringList &args) : KexiDB(parent, name)
{
	kdDebug() << "MySqlDB::MySqlDB()" << endl;
	
	m_mysql = 0;
	m_mysql = mysql_init(m_mysql);
	m_connected = false;
	m_connectedDB = false;
	
}

bool
MySqlDB::connect(QString host, QString user, QString password)
{
	kdDebug() << "MySqlDB::connect(QString host, QString user, QString password)" << endl;
	m_mysql = mysql_connect(m_mysql, host.latin1(), user.latin1(), password.latin1());
	if(mysql_errno(m_mysql) == 0)
	{
		m_connected = true;
		m_host = host;
		m_user = user;
		m_password = password;
		return true;
	}
	else
	{
		return false;
	}
}

bool
MySqlDB::connect(QString host, QString user, QString password, QString db)
{
	kdDebug() << "MySqlDB::connect(QString host, QString user, QString password, QString db)" << endl;
	if(m_connected && host == m_host && user == m_user && password == m_password)
	{
		//simple change to db:
		query("use "+db);
		m_connectedDB = true;
		return true;
	}
	else
	{
		if(connect(host, user, password))
		{
			query("use "+db);
			m_connectedDB = true;
			return true;
		}
		
	}

	return false;
}

QStringList
MySqlDB::databases()
{
	kdDebug() << "MySqlDB::databases()" << endl;
	QStringList s;

	query("show databases");
	KexiDBResult *result = storeResult();

	if(!result)
		return s;

	while(result->next())
	{
		s.append(result->value(0).toString());
	}

	delete result;
	return s;
}

QStringList
MySqlDB::tables()
{
	if(!m_connectedDB)
		return QStringList();

	QStringList s;

	query("show tables");
	KexiDBResult *result = storeResult();

	if(!result)
		return s;

	while(result->next())
	{
		s.append(result->value(0).toString());
	}

	delete result;
	return s;
}

int
MySqlDB::query(QString statement)
{
	const char *query = statement.latin1();
	if(mysql_real_query(m_mysql, query, strlen(query)) == 0)
		return 0;
}

KexiDBResult*
MySqlDB::storeResult()
{
	kdDebug() << "MySqlDB::storeResult(): error: " << mysql_error(m_mysql) << endl;
	MYSQL_RES *res = mysql_store_result(m_mysql);
	if(res)
	{
		kdDebug() << "MySqlDB::storeResult(): wow, got a result!!!" << endl;
		return new MySqlResult(res, this);
	}
	else
	{
		return 0;
	}
}

KexiDBResult*
MySqlDB::useResult()
{
	kdDebug() << "MySqlDB::useResult(): error: " << mysql_error(m_mysql) << endl;
	kdDebug() << "MySqlDB::useResult(): info: " << mysql_info(m_mysql) << endl;
	MYSQL_RES *res = mysql_use_result(m_mysql);
	kdDebug() << "MySqlDB::useResult(): d1" << endl;
	if(res)
	{
		kdDebug() << "MySqlDB::useResult(): d2" << endl;
		MySqlResult *result = new MySqlResult(res, this);
		kdDebug() << "MySqlDB::useResulg(): d3" << endl;
		return result;
	}
	else
	{
		kdDebug() << "MySqlDB::useResult(): not enough data" << endl;
		return 0;
	}
}

unsigned long
MySqlDB::affectedRows()
{
	return 0;
}

MySqlDB::~MySqlDB()
{
	if(m_connected)
	{
		mysql_close(m_mysql);
	}
	m_mysql = 0;
}

#include "mysqldb.moc"
