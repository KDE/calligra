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
#include <qfile.h>

#include <kgenericfactory.h>
#include <kdebug.h>

#include <kexidbresult.h>
#include <kexidberror.h>

#include "mysqldb.h"
#include "mysqlresult.h"
#include "mysqlrecord.h"

K_EXPORT_COMPONENT_FACTORY(keximysqlinterface, KGenericFactory<MySqlDB>( "mysqlinterface" ));

MySqlDB::MySqlDB(QObject *parent, const char *name, const QStringList &) : KexiDB(parent, name)
{
	kdDebug() << "MySqlDB::MySqlDB()" << endl;
	
	m_mysql = 0;
	m_mysql = mysql_init(m_mysql);
	m_connected = false;
	m_connectedDB = false;
	
}

KexiDBRecord*
MySqlDB::queryRecord(QString querystatement, bool buffer)
{
	kdDebug() << "MySqlDB::queryRecord()" << endl;
	try
	{
		query(querystatement);
		MYSQL_RES *res = mysql_use_result(m_mysql);
		if(res)
		{
			MySqlRecord *rec = new MySqlRecord(res, this, false);
			return rec;
		}
	}
	catch(KexiDBError &err)
	{
		kdDebug() << "MySqlDB::queryRecord(): abroating..." << endl;
		kdDebug() << "MySqlDB::queryRecord(): cause:"<<err.message() << endl;

		throw err;
	}
	
	return 0;
}

bool
MySqlDB::connect(QString host, QString user, QString password, QString socket, QString port)
{
	kdDebug() << "MySqlDB::connect(" << host << "," << user << "," << password << ")" << endl;
	
	if(socket == "")
	{
		QStringList sockets;
		sockets.append("/var/lib/mysql/mysql.sock");
		sockets.append("/var/run/mysqld/mysqld.sock");
		sockets.append("/tmp/mysql.sock");
		
		for(QStringList::Iterator it = sockets.begin(); it != sockets.end(); it++)
		{
			if(QFile(*it).exists())
				socket = (*it);
		}
	}
	
	mysql_real_connect(m_mysql, host.local8Bit(), user.local8Bit(), password.local8Bit(), 0,
		port.toUInt(), socket.local8Bit(), 0);
	if(mysql_errno(m_mysql) == 0)
	{
		m_connected = true;
		m_host = host;
		m_user = user;
		m_password = password;
		m_port = port.toUInt();
		m_socket = socket;
		return true;
	}
	
	kdDebug() << "MySqlDB::connect(...) failed: " << mysql_error(m_mysql) << endl;
	throw KexiDBError(0, mysql_error(m_mysql));
	return false;
}

bool
MySqlDB::connect(QString host, QString user, QString password, QString socket, QString port, QString db, bool create)
{
	kdDebug() << "MySqlDB::connect(QString host, QString user, QString password, QString db)" << endl;
	kdDebug() << "   host: " << host << endl;
	kdDebug() << "   user: " << user << endl;
	kdDebug() << "   pass: " << password << endl;
	kdDebug() << "   socket: " << socket << endl;
	kdDebug() << "   port: " << port << endl;
	kdDebug() << "   db: " << db << endl;


	if(m_connected && host == m_host && user == m_user && password == m_password && socket == m_socket
		&& port.toUInt() == m_port)
	{
		kdDebug() << "MySqlDB::connect(db): already connected" << endl;

		//create new database if needed
		if(create)
		{
			query("create database " + db);
		}
		//simple change to db:
		query("use "+db);
		kdDebug() << "MySqlDB::connect(db): errno: " << mysql_error(m_mysql) << endl;
		if(mysql_errno(m_mysql) != 0)
		{
			throw KexiDBError(0, mysql_error(m_mysql));
			return false;
		}
		m_connectedDB = true;
		return true;
	}
	else
	{
		kdDebug() << "MySqlDB::connect(db): retrying..." << endl;
		if(connect(host, user, password, socket, port))
		{
			//create new database if needed
			if(create)
			{
				query("create database " + db);
			}

			query("use "+db);
			m_connectedDB = true;
			return true;
		}

	}

	throw KexiDBError(0, mysql_error(m_mysql));
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

	kdDebug() << "field name: " << result->fieldInfo(0)->name() << endl;
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

bool
MySqlDB::query(QString statement)
{
//	if(!m_connected)
//		return false;
	const char *query = statement.latin1();
	if(mysql_real_query(m_mysql, query, strlen(query)) == 0)
	{
		if(mysql_errno(m_mysql) == 0)
			return true;
	}
	throw KexiDBError(0, mysql_error(m_mysql));
	return false;
}

KexiDBResult*
MySqlDB::getResult()
{
	return useResult();
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

QString
MySqlDB::driverName()
{
	return QString::fromLatin1("mySQL");
}

QString
MySqlDB::escape(const QString &str)
{
	char* escaped = (char*) malloc(str.length() * 2 + 2);
	mysql_real_escape_string(m_mysql, escaped, str.local8Bit(), str.length());
	
	QString rval = escaped;
	free(escaped);
	return rval;
}

QString
MySqlDB::escape(const QByteArray& str)
{
	QString rval;
	
	if(str.size() > 0)
	{
		char* escaped = (char*) malloc(str.size() * 2 + 2);
		mysql_real_escape_string(m_mysql, escaped, str.data(), str.size());
		rval = escaped;
		free(escaped);
	}
	else
	{
		rval = "";
	}
	
	return rval;
}

unsigned long
MySqlDB::lastAuto()
{
	return (unsigned long)mysql_insert_id(m_mysql);
}

bool
MySqlDB::alterField(const QString& table, const QString& field, const QString& newFieldName,
	KexiDBField::ColumnType dtype, int length, int precision,
	KexiDBField::ColumnConstraints constraints, bool binary, bool unsignedType,
	const QString& defaultVal)
{
	kdDebug() << "MySqlDB::alterField: Table: " << table << " Field: " << field << endl;
	kdDebug() << "MySqlDB::alterField: DataType: " << MySqlField::sql2string(dtype) << "ColumnType: " << dtype << endl;
	QString qstr = "ALTER TABLE " + table + " CHANGE " + field + " " + newFieldName;
	qstr += " " + createDefinition(newFieldName, dtype, length, precision, constraints, binary,
		unsignedType, defaultVal);
	
	kdDebug() << "MySqlDB::alterField: Query: " << qstr << endl;
	return query(qstr);
}

bool
MySqlDB::createField(const QString& table, const QString& field, KexiDBField::ColumnType dtype,
	int length, int precision, KexiDBField::ColumnConstraints constraints, bool binary,
	bool unsignedType, const QString& defaultVal)
{
	kdDebug() << "MySqlDB::createField: Table: " << table << " Field: " << field << endl;
	kdDebug() << "MySqlDB::createField: DataType: " << MySqlField::sql2string(dtype) << "ColumnType: " << dtype << endl;
	QString qstr = "ALTER TABLE " + table + " ADD " + field;
	qstr += " " + createDefinition(field, dtype, length, precision, constraints, binary, unsignedType, defaultVal);
	
	kdDebug() << "MySqlDB::createField: Query: " << qstr << endl;
	return query(qstr);
}

QString
MySqlDB::createDefinition(const QString& field, KexiDBField::ColumnType dtype, int length, int precision,
 KexiDBField::ColumnConstraints constraints, bool binary, bool unsignedType, const QString& defaultVal)
{
	QString qstr = MySqlField::sql2string(dtype);
	bool allowUnsigned = false;
	
	switch(dtype)
	{
		case KexiDBField::SQLInteger:
		case KexiDBField::SQLSmallInt:
		case KexiDBField::SQLTinyInt:
		case KexiDBField::SQLBigInt:
			allowUnsigned = true;
		case KexiDBField::SQLVarchar:
			qstr += "(" + QString::number(length) + ")";
			break;
		case KexiDBField::SQLDecimal:
		case KexiDBField::SQLFloat:
		case KexiDBField::SQLDouble:
		case KexiDBField::SQLNumeric:
			allowUnsigned = true;
			qstr += "(" + QString::number(length) + "," + QString::number(precision) + ")";
			break;
		case KexiDBField::SQLInvalid:
		case KexiDBField::SQLBinary:
		case KexiDBField::SQLBoolean:
		case KexiDBField::SQLDate:
		case KexiDBField::SQLLongVarBinary:
		case KexiDBField::SQLTime:
		case KexiDBField::SQLTimeStamp:
		case KexiDBField::SQLVarBinary:
		case KexiDBField::SQLInterval:
		case KexiDBField::SQLLongVarchar:
			break;
	}
	
	if(constraints & KexiDBField::NotNull)
	{
		qstr += " NOT NULL";
	}
	else
	{
		qstr += " NULL";
	}
	
	if(binary && (dtype == KexiDBField::SQLVarchar))
	{
		qstr += " BINARY";
	}
	
	if(unsignedType && allowUnsigned)
	{
		qstr += " UNSIGNED";
	}
	
	if(defaultVal != "")
	{
		qstr += " DEFAULT " + defaultVal;
	}
	
	if(constraints & KexiDBField::AutoInc)
	{
		qstr += " AUTO_INCREMENT";
	}
	
	return qstr;
}

KexiDBTableStruct
MySqlDB::getStructure(const QString& table)
{
	KexiDBTableStruct dbStruct;
	MYSQL_RES* result= mysql_list_fields(m_mysql, table.local8Bit().data(), 0);
	kdDebug() << "MySqlDB::getStructure: Get fields..." << endl;
	
	if(result)
	{
		MYSQL_FIELD* field;
		int i = 0;
		
		while((field = mysql_fetch_field(result)))
		{
			dbStruct.append(new MySqlField(field, i++));
		}
		
		mysql_free_result(result);
	}
	
	return dbStruct;
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
