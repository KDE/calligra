/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
Daniel Molkentin <molkentin@kde.org>
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

#ifdef Q_WS_WIN
# include <config-win.h>
#endif
#include <mysql.h>
#define BOOL bool

#include <qvariant.h>
#include <qfile.h>
#include <qdict.h>

#include <kgenericfactory.h>
#include <kdebug.h>

#include "mysqldriver.h"
#include "mysqlconnection.h"
#include "mysqlcursor.h"
#include <kexidb/error.h>
using namespace KexiDB;

MySqlConnection::MySqlConnection( Driver *driver, const ConnectionData &conn_data )
	:Connection(driver,conn_data)
	,m_mysql(0) {

	m_is_connected=false;
}


MySqlConnection::~MySqlConnection() {
	destroy();
}

bool MySqlConnection::drv_connect()
{
	if (m_is_connected) {
		setError(ERR_ALREADY_CONNECTED,i18n("Connection has already been opened"));
		return false;
	}
	if (!(m_mysql = mysql_init(m_mysql)))
		return false;

        KexiDBDrvDbg << "MySqlConnection::connect()" << endl;
	QString socket;
	if (m_data.hostName.isEmpty() || (m_data.hostName=="localhost")) {
		if (m_data.fileName().isEmpty()) {
	                QStringList sockets;
	                sockets.append("/var/lib/mysql/mysql.sock");
	                sockets.append("/var/run/mysqld/mysqld.sock");
        	        sockets.append("/tmp/mysql.sock");
	
	                for(QStringList::Iterator it = sockets.begin(); it != sockets.end(); it++)
        	        {
                	        if(QFile(*it).exists()) {
                        	        socket = (*it);
					break;
				}
	                }
        	} else socket=m_data.fileName();
	}


        mysql_real_connect(m_mysql, m_data.hostName.local8Bit(), m_data.userName.local8Bit(), 
		m_data.password.local8Bit(), 0,
                m_data.port, socket.local8Bit(), 0);
        if(mysql_errno(m_mysql) == 0)
        {
                m_is_connected = true;
                return true;
        }
	
	QString err = mysql_error(m_mysql); //store error msg, if any - can be destroyed after disconenct()
	drv_disconnect();
	setError(ERR_DB_SPECIFIC,err);
	return false;
}

bool MySqlConnection::drv_disconnect()
{
	if(m_is_connected)
	{
		mysql_close(m_mysql);
	}
	m_mysql = 0;

        KexiDBDrvDbg << "MySqlConnection::disconnect()" << endl;
        return true;
}


Cursor* MySqlConnection::prepareQuery( const QString& statement, uint cursor_options ) {
	return new MySqlCursor(this,statement,cursor_options);
}

Cursor* MySqlConnection::prepareQuery( QuerySchema& query, uint cursor_options ) {
	return new MySqlCursor( this, query, cursor_options );
}

QString MySqlConnection::escapeString(const QString& str) const {
	return QString();//TODO
}

QCString MySqlConnection::escapeString(const QCString& str) const {
	return QCString();//TODO
}
                
void MySqlConnection::escapeString( const QString& str, char *target ) {
}


bool MySqlConnection::drv_getDatabasesList( QStringList &list ) {
        KexiDBDrvDbg << "MySqlConnection::drv_getDatabasesList()" << endl;

	list.clear();

	if (!m_is_connected) {
		setError(ERR_NO_CONNECTION,i18n("Database list cannot be retrieved: Connection not open"));
		return false;
	}

	MYSQL_RES *res;

        if((res=mysql_list_dbs(m_mysql,0)) != 0)
        {
		MYSQL_ROW  row;
		while ( (row = mysql_fetch_row(res))!=0) {
			list<<QString(row[0]);
		}
              	mysql_free_result(res);
                return true;
        }

	setError(ERR_DB_SPECIFIC,mysql_error(m_mysql));
	return false;
}
	

bool MySqlConnection::drv_createDatabase( const QString &dbName) {
	return false; //TODO
}

bool MySqlConnection::drv_useDatabase( const QString &dbName) {
//TODO is here escaping needed?
	return drv_executeSQL("use "+dbName);
}

bool MySqlConnection::drv_closeDatabase() {
//TODO free resources 
//As far as I know, mysql doesn't support that
	return true;
}

bool MySqlConnection::drv_dropDatabase( const QString &dbName) {
//TODO is here escaping needed
	return drv_executeSQL("drop database "+dbName);
}
                
bool MySqlConnection::drv_executeSQL( const QString& statement ) {
        if (!m_is_connected) {
                setError(ERR_NO_CONNECTION,i18n("Database command can't be executed, because there is no open connection"));
                return false;
        }

	if (statement.isEmpty()) {
//add errormsg ?
		return false;
	}
	
	QCString queryStr=statement.utf8();
	const char *query=queryStr;
        if(mysql_real_query(m_mysql, query, strlen(query)) == 0)
        {
		return true;
        }
        
        setError(ERR_DB_SPECIFIC,mysql_error(m_mysql));
        return false;
}

Q_ULLONG MySqlConnection::drv_lastInsertRowID()
{
#ifndef Q_WS_WIN
#warning TODO
#endif
	return (Q_ULLONG)mysql_insert_id(m_mysql);
}













#if 0
MySqlDB::MySqlDB(QObject *parent, const char *name, const QStringList &) : KexiDB(parent, name)
{
	KexiDBDrvDbg << "MySqlDB::MySqlDB()" << endl;

	m_driverName = "mySQL"
	m_isFileDriver=false;
	m_features=CursorForward;
	m_typeNames.resize(Field::LastType + 1);

        m_typeNames.resize(Field::LastType + 1);
        m_typeNames[Field::Byte]="UNSIGNED TINYINT";
        m_typeNames[Field::ShortInteger]="SMALLINT";
        m_typeNames[Field::Integer]="INT";
        m_typeNames[Field::BigInteger]="BIGINT";
        m_typeNames[Field::Boolean]="BOOLEAN";
        m_typeNames[Field::Date]="DATE";
        m_typeNames[Field::DateTime]="DATETIME";
        m_typeNames[Field::Time]="TIME";
        m_typeNames[Field::Float]="FLOAT";
        m_typeNames[Field::Double]="DOUBLE";
        m_typeNames[Field::Text]="TEXT";
        m_typeNames[Field::LongText]="LONGTEXT";
        m_typeNames[Field::BLOB]="BLOB"; 



	m_mysql = 0;
	m_mysql = mysql_init(m_mysql);
	m_connected = false;
	m_connectedDB = false;

}


KexiDBRecordSet*
MySqlDB::queryRecord(const QString& querystatement, bool buffer)
{
	m_error.setup(0);
	KexiDBDrvDbg << "MySqlDB::queryRecord()" << endl;
	if (query(querystatement)) {
		MYSQL_RES *res;
		if(!buffer)
			res = mysql_use_result(m_mysql);
		else
			res = mysql_store_result(m_mysql);

		if(res)
		{
			MySqlRecord *rec = new MySqlRecord(res, this, "record", false);
			return rec;
		}
	}
	else
	{
		KexiDBDrvDbg << "MySqlDB::queryRecord(): error..." << endl;
		KexiDBDrvDbg << "MySqlDB::queryRecord(): cause:"<<m_error.message() <<endl;
	}

	return 0;
}

bool
MySqlDB::connect(const QString& host, const QString& user, const QString& password,
								 const QString& socket, const QString& port)
{
	KexiDBDrvDbg << "MySqlDB::connect(" << host << "," << user << "," << password << ")" << endl;

	QString msocket = socket;
	if(msocket.isEmpty())
	{
		QStringList sockets;
		sockets.append("/var/lib/mysql/mysql.sock");
		sockets.append("/var/run/mysqld/mysqld.sock");
		sockets.append("/tmp/mysql.sock");

		for(QStringList::Iterator it = sockets.begin(); it != sockets.end(); it++)
		{
			if(QFile(*it).exists())
				msocket = (*it);
		}
	}

	mysql_real_connect(m_mysql, host.local8Bit(), user.local8Bit(), password.local8Bit(), 0,
		port.toUInt(), msocket.local8Bit(), 0);
	if(mysql_errno(m_mysql) == 0)
	{
		m_connected = true;
		m_host = host;
		m_user = user;
		m_password = password;
		m_port = port.toUInt();
		m_socket = msocket;
		return true;
	}
	else
	{
		m_error=KexiDBError(1, mysql_error(m_mysql));
	}

	KexiDBDrvDbg << "MySqlDB::connect(...) failed: " << mysql_error(m_mysql) << endl;
	return false;
}

bool
MySqlDB::connect(const QString& host, const QString& user, const QString& password,
								 const QString& socket, const QString& port, const QString& db, bool create)
{
	KexiDBDrvDbg << "MySqlDB::connect(QString host, QString user, QString password, QString db)" << endl;
	if(m_connected && host == m_host && user == m_user && password == m_password && socket == m_socket
		&& port.toUInt() == m_port)
	{
		KexiDBDrvDbg << "MySqlDB::connect(db): already connected" << endl;

		//create new database if needed
		if(create)
		{
			query("create database " + db);
		}
		//simple change to db:
		query("use "+db);
		KexiDBDrvDbg << "MySqlDB::connect(db): errno: " << mysql_error(m_mysql) << endl;
		if(mysql_errno(m_mysql) != 0)
			return false;
		m_connectedDB = true;
		return true;
	}
	else
	{
		KexiDBDrvDbg << "MySqlDB::connect(db): retrying..." << endl;
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
MySqlDB::databases()
{
	KexiDBDrvDbg << "MySqlDB::databases()" << endl;
	QStringList s;

	const_cast<MySqlDB*>(this)->query("show databases");
	MySqlResult *result = const_cast<MySqlDB*>(this)->storeResult();

	if(!result)
		return s;

	KexiDBDrvDbg << "field name: " << result->fieldInfo(0)->name() << endl;
	while(result->next())
	{
		s.append(result->value(0).toString());
	}

	delete result;
	return s;
}

bool MySqlDB::isSystemDatabase(QString &dbName)
{
	return dbName=="mysql";
}

QStringList
MySqlDB::tableNames()
{
	if(!m_connectedDB)
		return QStringList();

	QStringList s;

	query("show tables");
	MySqlResult *result = storeResult();

	if(!result)
		return s;

	while(result->next())
	{
		s.append(result->value(0).toString());
//    KexiDBDrvDbg << "* tableNames():" << result->value(0).toString() << endl;
	}

	delete result;

	m_tableDefs.clear();
	for(QStringList::const_iterator it=s.begin();it!=s.end();++it) {
		m_tableDefs.insert((*it),createTableDef(*it));
	}

	return s;
}

const KexiDBTable * const MySqlDB::table(const QString& name) {
	return m_tableDefs[name];
}


KexiDBTable * MySqlDB::createTableDef(const QString& name)
{
	KexiDBDrvDbg<<"MySQLDB::createTableDef: entered"<<endl;

	if(!m_connectedDB)
		return 0;

	KexiDBDrvDbg<<"MySQLDB::createTableDef: connection exists"<<endl;

	KexiDBDrvDbg<<"MySQLDB::createTableDef: querying"<< ("select * from "+escapeName(name)+" limit 0")<<endl;

	query("select * from "+escapeName(name)+" limit 0");
	MySqlResult *result = storeResult();

	if(!result)
		return 0;

	KexiDBDrvDbg<<"MySQLDB::createTableDef: there is a result"<<endl;

	KexiDBTable *t=new KexiDBTable(name);

	KexiDBField *f = 0;
	int i = 0;
	while ((f = result->fieldInfo(i++)) != 0)
	{
		t->addField(*f);
		//should we support other unique keys here too ?
		if (f->primary_key()) t->addPrimaryKey(f->name());
		KexiDBDrvDbg<<"MySQLDB::createTableDef: addField:"<<f->name()<<endl;
	}

	delete result;

	return t;
}

bool
MySqlDB::query(const QString& statement)
{
	m_error.setup(0);
//	if(!m_connected)
//		return false;
	if(!uhQuery(statement))
	{
		m_error.setup(1, mysql_error(m_mysql));
		KexiDBDrvDbg<<m_error.message();
		return false;
	}
	return true;
}

bool
MySqlDB::uhQuery(const QString& statement)
{
	const char *query = statement.latin1();
//??(js)	const char *query = encode(statement);
	if(mysql_real_query(m_mysql, query, strlen(query)) == 0)
	{
		if(mysql_errno(m_mysql) == 0)
			return true;
	}

	return false;
}

MySqlResult*
MySqlDB::getResult()
{
	return useResult();
}

MySqlResult*
MySqlDB::storeResult()
{
	KexiDBDrvDbg << "MySqlDB::storeResult(): error: " << mysql_error(m_mysql) << endl;
	MYSQL_RES *res = mysql_store_result(m_mysql);
	if(res)
	{
		KexiDBDrvDbg << "MySqlDB::storeResult(): wow, got a result!!!" << endl;
		return new MySqlResult(res, this);
	}
	else
	{
		return 0;
	}
}

MySqlResult*
MySqlDB::useResult()
{
	KexiDBDrvDbg << "MySqlDB::useResult(): error: " << mysql_error(m_mysql) << endl;
	KexiDBDrvDbg << "MySqlDB::useResult(): info: " << mysql_info(m_mysql) << endl;
	MYSQL_RES *res = mysql_use_result(m_mysql);
	KexiDBDrvDbg << "MySqlDB::useResult(): d1" << endl;
	if(res)
	{
		KexiDBDrvDbg << "MySqlDB::useResult(): d2" << endl;
		MySqlResult *result = new MySqlResult(res, this);
		KexiDBDrvDbg << "MySqlDB::useResulg(): d3" << endl;
		return result;
	}
	else
	{
		KexiDBDrvDbg << "MySqlDB::useResult(): not enough data" << endl;
		return 0;
	}
}

unsigned long
MySqlDB::affectedRows() const
{
	return 0;
}

QString
MySqlDB::driverName() const
{
	return QString::fromLatin1("mySQL");
}

QString
MySqlDB::escape(const QString &str)
{
//	QCString val(encode(str));
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


KexiDBTableStruct
MySqlDB::structure(const QString& table) const
{
	KexiDBTableStruct dbStruct;
	MYSQL_RES* result= mysql_list_fields(m_mysql, table.local8Bit().data(), 0);
	KexiDBDrvDbg << "MySqlDB::structure: Get fields..." << endl;

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
MySqlDB::nativeDataType(const KexiDBField::ColumnType& t) const
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
	KexiDBDrvDbg << "MySqlDB::alterField: Table: " << changedField.table() << " Field: " << fields.at(index)->name() << endl;
	KexiDBDrvDbg << "MySqlDB::alterField: DataType: " << nativeDataType(
		changedField.sqlType()) << "ColumnType: " << changedField.sqlType() << endl;
	QString qstr = "ALTER TABLE `" + changedField.table() + "` CHANGE `" +
		fields.at(index)->name() + "` `" + changedField.name();
	qstr += "` " + createDefinition(changedField, index, fields);
	KexiDBDrvDbg << "MySqlDB::alterField: Query: " << qstr << endl;
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
	KexiDBDrvDbg << "MySqlDB::createField: Table: " << newField.table() << " Field: " << newField.name() << endl;
	KexiDBDrvDbg << "MySqlDB::createField: DataType: " << nativeDataType(
		newField.sqlType()) << "ColumnType: " << newField.sqlType() << endl;
	QString qstr;

	if(!createTable)
	{
		qstr = "ALTER TABLE `" + newField.table() + "` ADD `" + newField.name();
		qstr += "` " + createDefinition(newField, -1, fields);
	}
	else
	{
		qstr = "CREATE TABLE `" + newField.table() + "` (`" + newField.name();
		qstr += "` " + createDefinition(newField, -1, fields);
		qstr += ")";
	}

	KexiDBDrvDbg << "MySqlDB::createField: Query: " << qstr << endl;
	bool ok = uhQuery(qstr);

	if(ok)
	{
		ok = changeKeys(newField, -1, fields);
	}

	//<js> This is a fresh created table: add its def to our set of tabledefs:
	//TODO: tableNames() do this on as a side effect -THIS IS BAD IMPL.- fix tableNames()
	if (ok && createTable) {
               m_tableDefs.insert(newField.table(),createTableDef(newField.table()));
	}
	//</js>


	return ok;
}

QString
MySqlDB::createDefinition(const KexiDBField& field, int index,
	KexiDBTableStruct fields)
{
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
			qstr += "(" + QString::number((field.length()==0)?255:field.length()) + ")";
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
		qstr += " AUTO_INCREMENT PRIMARY KEY";
	}

	return qstr;
}

bool
MySqlDB::changeKeys(const KexiDBField& field, int index,
	KexiDBTableStruct fields)
{
	bool noPrimary = false;
	QString qstr = "ALTER TABLE `" + field.table() + "`";

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

				fstr += "`" + f->name() + "`";
			}

			i++;
		}

		if(field.primary_key())
		{
			if(j > 0)
			{
				fstr += ",";
			}

			fstr += "`" + field.name() + "`";
		}

		if(!fstr.isEmpty())
		{
			qstr += ", ADD PRIMARY KEY(" + fstr + ")";
		}
	}

	if(!noPrimary)
	{
		KexiDBDrvDbg << "MySqlDB::changeKeys: Query: " << qstr << endl;
		return uhQuery(qstr);
	}

	return true;
}

KexiDBError *MySqlDB	::latestError() {
	return &m_error;
}

MySqlDB::~MySqlDB()
{
	if(m_connected)
	{
		mysql_close(m_mysql);
	}
	m_mysql = 0;
}

QString MySqlDB::escapeName(const QString &tn)
{
	QString en;

	en = "`" + tn + "`";
	return en;
}

#endif

#include "mysqlconnection.moc"
