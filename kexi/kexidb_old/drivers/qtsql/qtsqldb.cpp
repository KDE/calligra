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

//#include <mysql/mysql.h>

#include <qvariant.h>
#include <qfile.h>
#include <qstring.h>
//#include <qptrlist.h>
//#include <qsqldatabase.h>

#include <kgenericfactory.h>
#include <kdebug.h>

#include <kexidberror.h>

#include "qtsqldb.h"
#include "qtsqlresult.h"
#include "qtsqlrecord.h"

#define _ME "#######qtsql-mysql: "

K_EXPORT_COMPONENT_FACTORY(kexiqtsqlinterface, KGenericFactory<QtSqlDB>( "qtsqlinterface" ));

QtSqlDB::QtSqlDB(QObject *parent, const char *name, const QStringList &) : KexiDB(parent, name)
{
	kdDebug() << _ME  << "QtSqlDB::QtSqlDB()" << endl;

	m_qtsql = 0;
	m_qtsql = QSqlDatabase::addDatabase( "QMYSQL3" );
//	m_qtsql = mysql_init(m_mysql);
	m_connected = false;
	m_connectedDB = false;

}

QStringList
QtSqlDB::tableNames()
{
	kdDebug() << _ME  << "QtSqlDB::tableNames()" << endl;
	return m_qtsql->tables();
// 	QStringList l;
// /*	QStringList s;
// 	QSqlQuery myquery;
// 	myquery.exec("show tables");
// 	if ( myquery.isActive() )
// 	{
// 		while(myquery.next())
// 		{
// 			s.append(myquery.value(0).toString());
// 		}
// 	}
// 	return s;*/
// 	l.append("holgisTables");
// 	return l;
}

bool
QtSqlDB::isSystemDatabase(QString &dbName)
{
	kdDebug() << _ME  << "QtSqlDB::isSystemDatabase()" << endl;
	return false;
}

QString
QtSqlDB::driverName() const
{
	kdDebug() << _ME  << "QtSqlDB::driverName()" << endl;
	return "holgisDriver";
}

KexiDB::DBType
QtSqlDB::dbType() const
{
	kdDebug() << _ME  << "QtSqlDB::dbType()" << endl;
	return RemoteDB;
}

KexiDBTableStruct
QtSqlDB::structure(const QString& table) const
{
	kdDebug() << _ME  << "QtSqlDB::structure()" << endl;
	KexiDBTableStruct dbStruct;
	return dbStruct;
}

unsigned long
QtSqlDB::affectedRows() const
{
	kdDebug() << _ME  << "QtSqlDB::affectedRows()" << endl;
	return 0;
}

// KexiDBWatcher*
// QtSqlDB::watcher() const
// {
// 	KexiDBWatcher watcher;
// 	return watcher;
// }

const KexiDBTable*
const QtSqlDB::table(const QString&)
{
	kdDebug() << _ME  << "QtSqlDB::table()" << endl;
	KexiDBTable tbl;
	return &tbl;
}

RelationList
QtSqlDB::relations() const
{
	kdDebug() << _ME  << "QtSqlDB::relations()" << endl;
	RelationList list;
	return list;
}

KexiDBRecordSet*
QtSqlDB::queryRecord(const QString& querystatement, bool buffer)
{
	kdDebug() << _ME  << "QtSqlDB::queryRecord() querystatement:" << querystatement << endl;
//	return 0;
	QtSqlRecord* r;
	r = new QtSqlRecord( m_qtsql, this, "holgisQtSqlRecord", querystatement);
//	r->set...

	return r;
//	KexiDBRecordSet rs;
// 	m_error.setup(0);
// 	kdDebug() << "QtSqlDB::queryRecord()" << endl;

// 	if (query(querystatement)) {
// 		MYSQL_RES *res;
// 		if(!buffer)
// 			res = mysql_use_result(m_mysql);
// 		else
// 			res = mysql_store_result(m_mysql);
//
// 		if(res)
// 		{
// 			QtSqlRecord *rec = new QtSqlRecord(res, this, "record", false);
// 			return rec;
// 		}
// 	}
// 	else
// 	{
// 		kdDebug() << "QtSqlDB::queryRecord(): error..." << endl;
// 		kdDebug() << "QtSqlDB::queryRecord(): cause:"<<m_error.message() <<endl;
// 	}

//	return &rs;
}

bool
QtSqlDB::connect(const QString& host, const QString& user, const QString& password,
	 const QString& socket, const QString& port)
{
	kdDebug() << "QtSqlDB::connect(" << host << "," << user << "," << password << ")" << endl;

//        QSqlDatabase *defaultDB = QSqlDatabase::addDatabase( DB_SALES_DRIVER );

	if ( ! m_qtsql )
	{
		kdDebug() << _ME << "addDatabase failed" << endl;
		return false;
	}
	else
	{
		kdDebug() << _ME << "addDatabase( QMYSQL3 ) was successful" << endl;
	}
 	m_qtsql->setDatabaseName( "test" );
 	m_qtsql->setUserName( user );
 	m_qtsql->setHostName( host );
 	m_qtsql->setPassword( password );
        if ( ! m_qtsql->open() ) {
		kdDebug() << _ME << "open failed" << endl;
		return false;
	}
	else
	{
		kdDebug() << _ME << "open() was successful" << endl;
	}
	m_connected = true;
	return true;
// 	if(socket.isEmpty())
// 	{
// 		QStringList sockets;
// 		sockets.append("/var/lib/mysql/mysql.sock");
// 		sockets.append("/var/run/mysqld/mysqld.sock");
// 		sockets.append("/tmp/mysql.sock");
//
// 		for(QStringList::Iterator it = sockets.begin(); it != sockets.end(); it++)
// 		{
// 			if(QFile(*it).exists())
// 				socket = (*it);
// 		}
// 	}

// 	mysql_real_connect(m_mysql, host.local8Bit(), user.local8Bit(), password.local8Bit(), 0,
// 		port.toUInt(), socket.local8Bit(), 0);
// 	if(mysql_errno(m_mysql) == 0)
// 	{
// 		m_connected = true;
// 		m_host = host;
// 		m_user = user;
// 		m_password = password;
// 		m_port = port.toUInt();
// 		m_socket = socket;
// 		return true;
// 	}
// 	else
// 	{
// 		m_error=KexiDBError(1, mysql_error(m_mysql));
// 	}

//	kdDebug() << "QtSqlDB::connect(...) failed: " << mysql_error(m_mysql) << endl;
//	return false;
}

bool
QtSqlDB::connect( const QString& host, const QString& user, const QString& password,
	const QString& socket, const QString& port, const QString& db, bool create)
{
	kdDebug() << "QtSqlDB::connect(QString host, QString user, QString password, QString db)" << endl;
	if(m_connected && host == m_host && user == m_user && password == m_password && socket == m_socket
		&& port.toUInt() == m_port)
	{
		kdDebug() << "QtSqlDB::connect(db): already connected" << endl;

		//create new database if needed
		if(create)
		{
			query("create database " + db);
		}
		//simple change to db:
		query("use "+db);
/*		kdDebug() << "QtSqlDB::connect(db): errno: " << mysql_error(m_mysql) << endl;
		if(mysql_errno(m_mysql) != 0)
			return false;*/
		m_connectedDB = true;
		return true;
	}
	else
	{
		kdDebug() << "QtSqlDB::connect(db): retrying..." << endl;
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
			else
			return false;
	}

	return false;
}

QStringList
QtSqlDB::databases()
{
	// FIXME: only mysql case, move to a subclass somewhen...
	kdDebug() << "QtSqlDB::databases()" << endl;
	QStringList s;
	QSqlQuery myquery;
	myquery.exec("show databases");
	if ( myquery.isActive() )
	{
		while(myquery.next())
		{
			s.append(myquery.value(0).toString());
		}
	}
	return s;
////
// 	kdDebug() << "MySqlDB::databases()" << endl;
// 	QStringList s;
//
// 	const_cast<MySqlDB*>(this)->query("show databases");
// 	MySqlResult *result = const_cast<MySqlDB*>(this)->storeResult();
//
// 	if(!result)
// 		return s;
//
// 	kdDebug() << "field name: " << result->fieldInfo(0)->name() << endl;
// 	while(result->next())
// 	{
// 		s.append(result->value(0).toString());
// 	}
//
// 	delete result;
// 	return s;

////
}

QStringList
QtSqlDB::tables()
{
	kdDebug() << "QtSqlDB::tables()" << endl;
	QStringList s;
	if(!m_connectedDB)
		return s;
		
	QSqlQuery myquery;
	myquery.exec("show tables");
	if ( !myquery.isActive() )
		return s;

	while(myquery.next())
	{
		s.append(myquery.value(0).toString());
	}
	return s;
}

bool
QtSqlDB::query(const QString& statement)
{
	kdDebug() << _ME  << "QtSqlDB::query() statement: _" << statement << "_" << endl;
	bool success;
	m_error.setup(0);
	if(!m_connected)
	{
		kdDebug() << "query: not connected" << endl;
//		return false;
		return true;
	}
//	success = m_query.exec( statement ); // for some reason this doesn't work...

	// so do it the probably slower way...
	QSqlQuery myquery;
	success = myquery.exec( statement );

	if( ! success )
	{
		kdDebug() << "query(): not successfull" << endl;
		m_error.setup(1, m_qtsql->lastError().driverText() );
		return false;
	}
	kdDebug() << "query(): successfull" << endl;
	return true;
}

bool
QtSqlDB::uhQuery(QString statement)
{
	kdDebug() << _ME  << "QtSqlDB::uhQuery() query: _" << statement << "_" << endl;
/*	const char *query = statement.latin1();
	if(mysql_real_query(m_mysql, query, strlen(query)) == 0)
	{
		if(mysql_errno(m_mysql) == 0)
			return true;
	}
*/
	return false;
}

QtSqlResult*
QtSqlDB::getResult()
{
	kdDebug() << _ME  << "QtSqlDB::getResult()" << endl;
	return useResult();
}

QtSqlResult*
QtSqlDB::storeResult()
{
	kdDebug() << _ME  << "QtSqlDB::storeResult()" << endl;
/*	kdDebug() << "QtSqlDB::storeResult(): error: " << mysql_error(m_mysql) << endl;
	MYSQL_RES *res = mysql_store_result(m_mysql);
	if(res)
	{
		kdDebug() << "QtSqlDB::storeResult(): wow, got a result!!!" << endl;
		return new QtSqlResult(res, this);
	}
	else
	{
		return 0;
	}*/
}

QtSqlResult*
QtSqlDB::useResult()
{
	kdDebug() << _ME  << "QtSqlDB::useResult()" << endl;
/*	kdDebug() << "QtSqlDB::useResult(): error: " << mysql_error(m_mysql) << endl;
	kdDebug() << "QtSqlDB::useResult(): info: " << mysql_info(m_mysql) << endl;
	MYSQL_RES *res = mysql_use_result(m_mysql);
	kdDebug() << "QtSqlDB::useResult(): d1" << endl;
	if(res)
	{
		kdDebug() << "QtSqlDB::useResult(): d2" << endl;
		QtSqlResult *result = new QtSqlResult(res, this);
		kdDebug() << "QtSqlDB::useResulg(): d3" << endl;
		return result;
	}
	else
	{
		kdDebug() << "QtSqlDB::useResult(): not enough data" << endl;
		return 0;
	}*/
}

unsigned long
QtSqlDB::affectedRows()
{
	kdDebug() << _ME  << "QtSqlDB::affectedRows()" << endl;
	return 0;
}

QString
QtSqlDB::driverName()
{
	kdDebug() << _ME  << "QtSqlDB::driverName()" << endl;
	return QString::fromLatin1("qtsql");
}

QString
QtSqlDB::escape(const QString &str)
{
	kdDebug() << _ME  << "QtSqlDB::escape(QString)" << endl;
/*	char* escaped = (char*) malloc(str.length() * 2 + 2);
	mysql_real_escape_string(m_mysql, escaped, str.local8Bit(), str.length());

	QString rval = escaped;
	free(escaped);
	return rval;*/
}

QString
QtSqlDB::escape(const QByteArray& str)
{
	kdDebug() << _ME  << "QtSqlDB::escape(QByteArray)" << endl;
/*	QString rval;

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

	return rval;*/
}

QString
QtSqlDB::escapeName(const QString& str)
{
	kdDebug() << _ME  << "QtSqlDB::escapeName(QString) FIXME implement me" << endl;

	QString tmp;
	tmp = QString("`%1`").arg( str );
	return tmp;
}
unsigned long
QtSqlDB::lastAuto()
{
	kdDebug() << _ME  << "QtSqlDB::lastAuto()" << endl;
/*	return (unsigned long)mysql_insert_id(m_mysql);*/
}

bool
QtSqlDB::alterField(const QString& table, const QString& field, const QString& newFieldName,
	KexiDBField::ColumnType dtype, int length, int precision,
	KexiDBField::ColumnConstraints constraints, bool binary, bool unsignedType,
	const QString& defaultVal)
{
	kdDebug() << "QtSqlDB::alterField: Table: " << table << " Field: " << field << endl;
//	kdDebug() << "QtSqlDB::alterField: DataType: " << getNativeDataType(dtype) << "ColumnType: " << dtype << endl;
	kdDebug() << "QtSqlDB::alterField: DataType: " << nativeDataType(dtype) << "ColumnType: " << dtype << endl;
	QString qstr = "ALTER TABLE " + table + " CHANGE " + field + " " + newFieldName;
	qstr += " " + createDefinition(newFieldName, dtype, length, precision, constraints, binary,
		unsignedType, defaultVal);

	kdDebug() << "QtSqlDB::alterField: Query: " << qstr << endl;
//	return uhQuery(qstr);
	return query(qstr);
}

bool
QtSqlDB::createField(const QString& table, const QString& field, KexiDBField::ColumnType dtype,
	int length, int precision, KexiDBField::ColumnConstraints constraints, bool binary,
	bool unsignedType, const QString& defaultVal)
{
	kdDebug() << "QtSqlDB::createField: Table: " << table << " Field: " << field << endl;
	kdDebug() << "QtSqlDB::createField: DataType: " << nativeDataType(dtype) << "ColumnType: " << dtype << endl;
	QString qstr = "ALTER TABLE " + table + " ADD " + field;
	qstr += " " + createDefinition(field, dtype, length, precision, constraints, binary, unsignedType, defaultVal);

	kdDebug() << "QtSqlDB::createField: holgi Query: " << qstr << endl;
//	return uhQuery(qstr);
	return query(qstr);
}

QString
QtSqlDB::createDefinition(const QString& /*field*/, KexiDBField::ColumnType dtype, int length, int precision,
 KexiDBField::ColumnConstraints constraints, bool binary, bool unsignedType, const QString& defaultVal)
{
	kdDebug() << _ME  << "QtSqlDB::()" << endl;
	QString qstr = nativeDataType(dtype);
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

	if(!defaultVal.isEmpty())
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
QtSqlDB::getStructure(const QString& table)
{
	kdDebug() << _ME  << "QtSqlDB::getStructure()" << endl;
/*	KexiDBTableStruct dbStruct;
	MYSQL_RES* result= mysql_list_fields(m_mysql, table.local8Bit().data(), 0);
	kdDebug() << "QtSqlDB::getStructure: Get fields..." << endl;

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

	return dbStruct;*/
}

QString
//QtSqlDB::getNativeDataType(const KexiDBField::ColumnType& t)
QtSqlDB::nativeDataType(const KexiDBField::ColumnType& t) const
{
	kdDebug() << _ME  << "QtSqlDB::nativeDataType() from type: " << t << endl;
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
QtSqlDB::getInternalDataType(int t)
{
	kdDebug() << _ME  << "QtSqlDB::getInternalDataType()" << endl;
/*	switch(t)
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

	}*/
	return KexiDBField::SQLInvalid;
}

bool
QtSqlDB::alterField(const KexiDBField& changedField, unsigned int index,
	KexiDBTableStruct fields)
{
	kdDebug() << "QtSqlDB::alterField: Table: " << changedField.table() << " Field: " << fields.at(index)->name() << endl;
	kdDebug() << "QtSqlDB::alterField: DataType: " << nativeDataType(
		changedField.sqlType()) << "ColumnType: " << changedField.sqlType() << endl;
	QString qstr = "ALTER TABLE " + changedField.table() + " CHANGE " +
		fields.at(index)->name() + " " + changedField.name();
	qstr += " " + createDefinition(changedField, index, fields);
	kdDebug() << "QtSqlDB::alterField: Query: " << qstr << endl;
//	bool ok = uhQuery(qstr);
	bool ok = query(qstr);

	if(ok)
	{
		ok = changeKeys(changedField, index, fields);
	}

	return ok;
}

bool
QtSqlDB::createField(const KexiDBField& newField, KexiDBTableStruct fields,
	bool createTable)
{
	kdDebug() << "QtSqlDB::createField: Table: " << newField.table() << " Field: " << newField.name() << endl;
	kdDebug() << "QtSqlDB::createField: DataType: " << nativeDataType(
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

	kdDebug() << "QtSqlDB::createField: Query: " << qstr << endl;
//	bool ok = uhQuery(qstr);
	bool ok = query(qstr);

	if(ok)
	{
		ok = changeKeys(newField, -1, fields);
	}

	return ok;
}

QString
QtSqlDB::createDefinition(const KexiDBField& field, int index,
	KexiDBTableStruct fields)
{
	kdDebug() << _ME  << "QtSqlDB::createDefinition()" << endl;
	QString qstr = nativeDataType(field.sqlType());
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
QtSqlDB::changeKeys(const KexiDBField& field, int index,
	KexiDBTableStruct fields)
{
	kdDebug() << _ME  << "QtSqlDB::changeKeys()" << endl;
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
		kdDebug() << "QtSqlDB::changeKeys: Query: " << qstr << endl;
		return uhQuery(qstr);
	}

	return true;
}

KexiDBError *QtSqlDB	::latestError()
{
	kdDebug() << _ME  << "QtSqlDB::latestError()" << endl;
	return &m_error;
}

QtSqlDB::~QtSqlDB()
{
	kdDebug() << _ME  << "QtSqlDB::~QtSqlDB()" << endl;
/*	if(m_connected)
	{
		mysql_close(m_mysql);
	}*/
	m_mysql = 0;
}

#include "qtsqldb.moc"
