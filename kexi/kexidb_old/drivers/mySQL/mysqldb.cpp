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

#include <kgenericfactory.h>
#include <kdebug.h>

#include "../../kexiDB/kexidbresult.h"

#include "mysqldb.h"

K_EXPORT_COMPONENT_FACTORY(keximysqlinterface, KGenericFactory<MySqlDB>( "mysqlinterface" ));

MySqlDB::MySqlDB(QObject *parent, const char *name, const QStringList &args) : KexiDB(parent, name)
{
	kdDebug() << "MySqlDB::MySqlDB()" << endl;
	
	m_mysql = 0;
	m_mysql = mysql_init(m_mysql);
	m_connected = 0;
	
}

bool
MySqlDB::connect(QString host, QString user, QString password)
{
	kdDebug() << "MySqlDB::connect(QString host, QString user, QString password)" << endl;
	m_mysql = mysql_connect(m_mysql, host.latin1(), user.latin1(), password.latin1());
	kdDebug() << "MySqlDB::connect(host): errno: " << mysql_errno(m_mysql) << endl;
	if(mysql_errno(m_mysql) == 0)
	{
		m_connected = true;
		return true;
	}
	else
	{
		kdDebug() << "MySqlDB::connect(host): error: " << mysql_error(m_mysql) << endl;
		return false;
	}
}

bool
MySqlDB::connect(QString host, QString user, QString password, QString db)
{
	kdDebug() << "MySqlDB::connect(QString host, QString user, QString password, QString db)" << endl;
	MYSQL *nConnect = mysql_connect(m_mysql, host.latin1(), user.latin1(), password.latin1());
	if(!nConnect)
	{
		kdDebug() << "MySqlDB::connect(db): error: " << mysql_error(m_mysql) << endl;
		return false;
	}
	
	m_mysql = nConnect;	
	kdDebug() << "MySqlDB::connect(db): errno: " << mysql_errno(m_mysql) << endl;
	
	if(mysql_errno(m_mysql) == 0)
	{
		m_connected = true;
		return true;
	}
	else
	{
		kdDebug() << "MySqlDB::connect(db): error: " << mysql_error(m_mysql) << endl;
		return false;
	}
}

QStringList
MySqlDB::databases() const
{
	return QStringList();
}

QStringList
MySqlDB::tables() const
{
	return QStringList();
}

int
MySqlDB::query(QString)
{
	return -1;
}

KexiDBResult*
MySqlDB::storeResult()
{
	return new KexiDBResult();
}

KexiDBResult*
MySqlDB::useResult()
{
	return new KexiDBResult();
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
