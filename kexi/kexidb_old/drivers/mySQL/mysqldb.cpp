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
MySqlDB::queryRecord(QString querystatement, bool /*buffer*/)
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
	return false;
}

bool
MySqlDB::connect(QString host, QString user, QString password, QString socket, QString port, QString db, bool create)
{
	kdDebug() << "MySqlDB::connect(QString host, QString user, QString password, QString db)" << endl;
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
			return false;
		m_connectedDB = true;
		return true;
	}
	else
	{
		kdDebug() << "MySqlDB::connect(db): retrying..." << endl;
		try
		{
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
		catch(KexiDBError &err)
		{
			throw err;
			return false;
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
	if(!uhQuery(statement))
	{
		throw KexiDBError(0, mysql_error(m_mysql));
		return false;
	}
}

bool
MySqlDB::uhQuery(QString statement)
{
	const char *query = statement.latin1();
	if(mysql_real_query(m_mysql, query, strlen(query)) == 0)
	{
		if(mysql_errno(m_mysql) == 0)
			return true;
	}

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
	kdDebug() << "MySqlDB::alterField: DataType: " << getNativeDataType(dtype) << "ColumnType: " << dtype << endl;
	QString qstr = "ALTER TABLE " + table + " CHANGE " + field + " " + newFieldName;
	qstr += " " + createDefinition(newFieldName, dtype, length, precision, constraints, binary,
		unsignedType, defaultVal);

	kdDebug() << "MySqlDB::alterField: Query: " << qstr << endl;
	return uhQuery(qstr);
}

bool
MySqlDB::createField(const QString& table, const QString& field, KexiDBField::ColumnType dtype,
	int length, int precision, KexiDBField::ColumnConstraints constraints, bool binary,
	bool unsignedType, const QString& defaultVal)
{
	kdDebug() << "MySqlDB::createField: Table: " << table << " Field: " << field << endl;
	kdDebug() << "MySqlDB::createField: DataType: " << getNativeDataType(dtype) << "ColumnType: " << dtype << endl;
	QString qstr = "ALTER TABLE " + table + " ADD " + field;
	qstr += " " + createDefinition(field, dtype, length, precision, constraints, binary, unsignedType, defaultVal);

	kdDebug() << "MySqlDB::createField: Query: " << qstr << endl;
	return uhQuery(qstr);
}

QString
MySqlDB::createDefinition(const QString& /*field*/, KexiDBField::ColumnType dtype, int length, int precision,
 KexiDBField::ColumnConstraints constraints, bool binary, bool unsignedType, const QString& defaultVal)
{
	QString qstr = getNativeDataType(dtype);
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
		case KexiDBField::SQLLastType:
			break;
	}

	if(constraints & KexiDBField::CCNotNull)
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

	if(constraints & KexiDBField::CCAutoInc)
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

		while((field = mysql_fetch_field(result)))
		{
			KexiDBField* f = new KexiDBField(field->table);
			f->setName(field->name);
			f->setColumnType(getInternalDataType(field->type));
			f->setLength(field->length);
			f->setPrecision(field->decimals);
			f->setUnsigned(field->flags & UNSIGNED_FLAG);
			f->setBinary(field->flags & BINARY_FLAG);
			f->setDefaultValue(field->def);
			f->setAutoIncrement(field->flags & AUTO_INCREMENT_FLAG);
			f->setPrimaryKey(field->flags & PRI_KEY_FLAG);
			f->setUniqueKey(field->flags & UNIQUE_KEY_FLAG);
			f->setNotNull(field->flags & NOT_NULL_FLAG);
			dbStruct.append(f);
		}

		mysql_free_result(result);
	}

	return dbStruct;
}

QString
MySqlDB::getNativeDataType(const KexiDBField::ColumnType& t)
{
	switch(t)
	{
		case KexiDBField::SQLLongVarchar:
			return "TEXT";
		case KexiDBField::SQLVarchar:
			return "VARCHAR";
		case KexiDBField::SQLInteger:
			return "INTEGER";
		case KexiDBField::SQLSmallInt:
			return "SMALLINT";
		case KexiDBField::SQLTinyInt:
			return "TINYINT";
		case KexiDBField::SQLNumeric:
			return "NUMERIC";
		case KexiDBField::SQLDouble:
			return "DOUBLE";
		case KexiDBField::SQLBigInt:
			return "BIGINT";
		case KexiDBField::SQLDecimal:
			return "DECIMAL";
		case KexiDBField::SQLFloat:
			return "FLOAT";
		case KexiDBField::SQLBinary:
			return "BLOB";
		case KexiDBField::SQLLongVarBinary:
			return "LONGBLOB";
		case KexiDBField::SQLVarBinary:
			return "BLOB";
		case KexiDBField::SQLDate:
			return "DATE";
		case KexiDBField::SQLTime:
			return "TIME";
		case KexiDBField::SQLTimeStamp:
			return "TIMESTAMP";
		case KexiDBField::SQLBoolean:
			return "BOOL";
		case KexiDBField::SQLInterval:
			return "ENUM";
		case KexiDBField::SQLInvalid:
		case KexiDBField::SQLLastType:
			return QString::null;
	}

	return QString::null;
}

KexiDBField::ColumnType
MySqlDB::getInternalDataType(int t)
{
	switch(t)
	{
		case FIELD_TYPE_NULL:
			return KexiDBField::SQLInvalid;
		case FIELD_TYPE_INT24:
		case FIELD_TYPE_LONGLONG:
			return KexiDBField::SQLBigInt;
		case FIELD_TYPE_NEWDATE:
		case FIELD_TYPE_DATE:
			return KexiDBField::SQLDate;
		case FIELD_TYPE_DECIMAL:
			return KexiDBField::SQLDecimal;
		case FIELD_TYPE_DOUBLE:
			return KexiDBField::SQLDouble;
		case FIELD_TYPE_FLOAT:
			return KexiDBField::SQLFloat;
		case FIELD_TYPE_LONG:
		case FIELD_TYPE_YEAR:
			return KexiDBField::SQLInteger;
		case FIELD_TYPE_SHORT:
			return KexiDBField::SQLSmallInt;
		case FIELD_TYPE_TIME:
			return KexiDBField::SQLTime;
		case FIELD_TYPE_DATETIME:
		case FIELD_TYPE_TIMESTAMP:
			return KexiDBField::SQLTimeStamp;
		case FIELD_TYPE_TINY:
			return KexiDBField::SQLTinyInt;
		case FIELD_TYPE_TINY_BLOB:
		case FIELD_TYPE_MEDIUM_BLOB:
		case FIELD_TYPE_LONG_BLOB:
		case FIELD_TYPE_BLOB:
			return KexiDBField::SQLVarBinary;
		case FIELD_TYPE_VAR_STRING:
		case FIELD_TYPE_STRING:
		case FIELD_TYPE_SET:
		case FIELD_TYPE_ENUM:
			return KexiDBField::SQLVarchar;

	}
	return KexiDBField::SQLInvalid;
}

bool
MySqlDB::alterField(const KexiDBField& changedField, unsigned int index,
	KexiDBTableStruct fields)
{
	kdDebug() << "MySqlDB::alterField: Table: " << changedField.table() << " Field: " << fields.at(index)->name() << endl;
	kdDebug() << "MySqlDB::alterField: DataType: " << getNativeDataType(
		changedField.sqlType()) << "ColumnType: " << changedField.sqlType() << endl;
	QString qstr = "ALTER TABLE " + changedField.table() + " CHANGE " +
		fields.at(index)->name() + " " + changedField.name();
	qstr += " " + createDefinition(changedField, index, fields);
	kdDebug() << "MySqlDB::alterField: Query: " << qstr << endl;
	bool ok = uhQuery(qstr);

	if(ok)
	{
		ok = changeKeys(changedField, index, fields);
	}

	return ok;
}

bool
MySqlDB::createField(const KexiDBField& newField, KexiDBTableStruct fields,
	bool createTable)
{
	kdDebug() << "MySqlDB::createField: Table: " << newField.table() << " Field: " << newField.name() << endl;
	kdDebug() << "MySqlDB::createField: DataType: " << getNativeDataType(
		newField.sqlType()) << "ColumnType: " << newField.sqlType() << endl;
	QString qstr;

	if(!createTable)
	{
		qstr = "ALTER TABLE " + newField.table() + " ADD " + newField.name();
		qstr += " " + createDefinition(newField, -1, fields);
	}
	else
	{
		qstr = "CREATE TABLE " + newField.table() + " (" + newField.name();
		qstr += " " + createDefinition(newField, -1, fields);
		qstr += ")";
	}

	kdDebug() << "MySqlDB::createField: Query: " << qstr << endl;
	bool ok = uhQuery(qstr);

	if(ok)
	{
		ok = changeKeys(newField, -1, fields);
	}

	return ok;
}

QString
MySqlDB::createDefinition(const KexiDBField& field, int index,
	KexiDBTableStruct fields)
{
	QString qstr = getNativeDataType(field.sqlType());
	bool allowUnsigned = false;

	switch(field.sqlType())
	{
		case KexiDBField::SQLInteger:
		case KexiDBField::SQLSmallInt:
		case KexiDBField::SQLTinyInt:
		case KexiDBField::SQLBigInt:
			allowUnsigned = true;
		case KexiDBField::SQLVarchar:
			qstr += "(" + QString::number(field.length()) + ")";
			break;
		case KexiDBField::SQLDecimal:
		case KexiDBField::SQLFloat:
		case KexiDBField::SQLDouble:
		case KexiDBField::SQLNumeric:
			allowUnsigned = true;
			qstr += "(" + QString::number(field.length()) + "," + QString::number(field.precision()) + ")";
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
		case KexiDBField::SQLLastType:
			break;
	}

	if((field.constraints() & KexiDBField::CCNotNull) || field.primary_key())
	{
		qstr += " NOT NULL";
	}
	else
	{
		qstr += " NULL";
	}

	if(field.binary() && (field.sqlType() == KexiDBField::SQLVarchar))
	{
		qstr += " BINARY";
	}

	if(field.unsignedType() && allowUnsigned)
	{
		qstr += " UNSIGNED";
	}

	if(!field.defaultValue().toString().isEmpty())
	{
		qstr += " DEFAULT " + field.defaultValue().toString();
	}

	if(field.constraints() & KexiDBField::CCAutoInc)
	{
		qstr += " AUTO_INCREMENT";
	}

	return qstr;
}

bool
MySqlDB::changeKeys(const KexiDBField& field, int index,
	KexiDBTableStruct fields)
{
	bool noPrimary = false;
	QString qstr = "ALTER TABLE " + field.table();

	if(index >= 0)
	{
		if(field.primary_key() == fields.at(index)->primary_key())
		{
			noPrimary = true;
		}
	}

	if(!noPrimary)
	{
		qstr += " DROP PRIMARY KEY";
		QString fstr;
		int i = 0, j = 0;

		for(KexiDBField* f = fields.first(); f; f = fields.next())
		{
			if((index != i) && (f->primary_key()))
			{
				if(j > 0)
				{
					fstr += ",";
				}
				else
				{
					j++;
				}

				fstr += f->name();
			}

			i++;
		}

		if(field.primary_key())
		{
			if(j > 0)
			{
				fstr += ",";
			}

			fstr += field.name();
		}

		if(!fstr.isEmpty())
		{
			qstr += ", ADD PRIMARY KEY(" + fstr + ")";
		}
	}

	if(!noPrimary)
	{
		kdDebug() << "MySqlDB::changeKeys: Query: " << qstr << endl;
		return uhQuery(qstr);
	}

	return true;
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
