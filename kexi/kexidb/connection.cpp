/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#include <kexidb/connection.h>

#include <kexidb/error.h>
#include <kexidb/connectiondata.h>
#include <kexidb/driver.h>
#include <kexidb/driver_p.h>
#include <kexidb/schemadata.h>
#include <kexidb/tableschema.h>
#include <kexidb/transaction.h>
#include <kexidb/cursor.h>
#include <kexidb/global.h>
#include <kexidb/roweditbuffer.h>
#include <kexidb/utils.h>

#include <qfileinfo.h>
#include <qguardedptr.h>

#include <klocale.h>
#include <kdebug.h>

#include <assert.h>

#include "kexi_utils.h"

namespace KexiDB {

class ConnectionPrivate
{
	public:
		ConnectionPrivate(Connection *conn) 
		 : m_conn(conn)
		 , m_versionMajor(-1)
		 , m_versionMinor(-1)
		 , m_dont_remove_transactions(false)
		 , m_skip_databaseExists_check_in_useDatabase(false)
		{
		}
		~ConnectionPrivate() { }

		Connection *m_conn;

		/*! Default transaction handle. 
		If transactions are supported: Any operation on database (e.g. inserts)
		that is started without specifing transaction context, will be performed
		in the context of this transaction. */
		Transaction m_default_trans;
		QValueList<Transaction> m_transactions;

		//! Version information for this connection.
		int m_versionMajor;
		int m_versionMinor;

		//! true if rollbackTransaction() and commitTransaction() shouldn't remove 
		//! the transaction object from m_transactions list; used by closeDatabase()
		bool m_dont_remove_transactions : 1; 
		
		//! used to avoid endless recursion between useDatabase() and databaseExists()
		//! when useTemporaryDatabaseIfNeeded() works
		bool m_skip_databaseExists_check_in_useDatabase : 1;

		void errorInvalidDBContents(const QString& details) {
			m_conn->setError( ERR_INVALID_DATABASE_CONTENTS, i18n("Invalid database contents. ")+details);
		}

};

}

using namespace KexiDB;

//! static: list of internal KexiDB system table names 
QStringList KexiDB_kexiDBSystemTableNames;
		
Connection::Connection( Driver *driver, ConnectionData &conn_data )
	: QObject()
	,KexiDB::Object()
	,m_data(&conn_data)
	,d(new ConnectionPrivate(this))
	,m_driver(driver)
	,m_is_connected(false)
	,m_autoCommit(true)
	,m_tables_byname(101, false)
	,m_queries_byname(101, false)
	,m_destructor_started(false)
{
	m_tables.setAutoDelete(true);
	m_tables_byname.setAutoDelete(false);//m_tables is owner, not me
	m_kexiDBSystemtables.setAutoDelete(true);//only system tables
	m_queries.setAutoDelete(true);
	m_queries_byname.setAutoDelete(false);//m_queries is owner, not me
	m_cursors.setAutoDelete(true);
//	d->m_transactions.setAutoDelete(true);
	//reasonable sizes: TODO
	m_tables.resize(101);
	m_queries.resize(101);
	m_cursors.resize(101);
//	d->m_transactions.resize(101);//woohoo! so many transactions?
}

void Connection::destroy()
{
	disconnect();
	//do not allow the driver to touch me: I will kill myself.
	m_driver->m_connections.take( this );
}

Connection::~Connection()
{
	m_destructor_started = true;
	KexiDBDbg << "Connection::~Connection()" << endl;
	delete d;
/*	if (m_driver) {
		if (m_is_connected) {
			//delete own table schemas
			m_tables.clear();
			//delete own cursors:
			m_cursors.clear();
		}
		//do not allow the driver to touch me: I will kill myself.
		m_driver->m_connections.take( this );
	}*/
}

bool Connection::connect()
{
	clearError();
	if (m_is_connected) {
		setError(ERR_ALREADY_CONNECTED, i18n("Connection already established.") );
		return false;
	}

	m_is_connected = drv_connect();
	return m_is_connected;
}

bool Connection::isDatabaseUsed()
{
	return !m_usedDatabase.isEmpty() && m_is_connected && drv_isDatabaseUsed();
}

void Connection::clearError()
{
	Object::clearError();
	m_sql = QString::null;
}

bool Connection::disconnect()
{
	clearError();
	if (!m_is_connected)
		return true;
	
	if (!closeDatabase())
		return false;

	bool ok = drv_disconnect();
	if (ok)
		m_is_connected = false;
	return ok;
}

bool Connection::checkConnected()
{
	if (m_is_connected) {
		clearError();
		return true;
	}
	setError(ERR_NO_CONNECTION, i18n("Not connected to the database server.") );
	return false;
}

bool Connection::checkIsDatabaseUsed()
{
	if (isDatabaseUsed()) {
		clearError();
		return true;
	}
	setError(ERR_NO_DB_USED, i18n("Currently no database is used.") );
	return false;
}

QStringList Connection::databaseNames(bool also_system_db)
{
	KexiDBDbg << "Connection::databaseNames("<<also_system_db<<")"<< endl;
	if (!checkConnected())
		return QStringList();

	QString tmpdbName;
	//some engines need to have opened any database before executing "create database"
	if (!useTemporaryDatabaseIfNeeded(tmpdbName))
		return QStringList();
		
	QStringList list, non_system_list;
	
	bool ret = drv_getDatabasesList( list );
	
	if (!tmpdbName.isEmpty()) {
		//whatever result is - now we have to close temporary opened database:
		if (!closeDatabase())
			return QStringList();
	}
	
	if (!ret)
		return QStringList();
	
	if (also_system_db)
		return list;
	//filter system databases:
	for (QStringList::Iterator it = list.begin(); it!=list.end(); ++it) {
		KexiDBDbg << "Connection::databaseNames(): " << *it << endl;
		if (!m_driver->isSystemDatabaseName(*it)) {
			KexiDBDbg << "add " << endl;
			non_system_list << (*it);
		}
	}
	return non_system_list;
}

bool Connection::drv_getDatabasesList( QStringList &list )
{
	list.clear();
	return true;
}

bool Connection::drv_databaseExists( const QString &dbName, bool ignoreErrors )
{
	QStringList list = databaseNames(true);//also system
	if (error()) {
		return false;
	}

	if (list.find( dbName )==list.end()) {
		if (!ignoreErrors)
			setError(ERR_OBJECT_NOT_EXISTING, i18n("The database \"%1\" does not exist.").arg(dbName));
		return false;
	}

	return true;
}

bool Connection::databaseExists( const QString &dbName, bool ignoreErrors )
{
	KexiDBDbg << "Connection::databaseExists(" << dbName << "," << ignoreErrors << ")" << endl;
	if (!checkConnected())
		return false;
	clearError();

	if (m_driver->isFileDriver()) {
		//for file-based db: file must exists and be accessible
//js: moved from useDatabase():
		QFileInfo file(dbName);
		if (!file.exists() || ( !file.isFile() && !file.isSymLink()) ) {
			if (!ignoreErrors)
				setError(ERR_OBJECT_NOT_EXISTING, i18n("Database file \"%1\" does not exist.").arg(m_data->fileName()) );
			return false;
		}
		if (!file.isReadable()) {
			if (!ignoreErrors)
				setError(ERR_ACCESS_RIGHTS, i18n("Database file \"%1\" is not readable.").arg(m_data->fileName()) );
			return false;
		}
		if (!file.isWritable()) {
			if (!ignoreErrors)
				setError(ERR_ACCESS_RIGHTS, i18n("Database file \"%1\" is not writable.").arg(m_data->fileName()) );
			return false;
		}
		return true;
	}

	QString tmpdbName;
	//some engines need to have opened any database before executing "create database"
	d->m_skip_databaseExists_check_in_useDatabase = true;
	if (!useTemporaryDatabaseIfNeeded(tmpdbName))
		return false;
	d->m_skip_databaseExists_check_in_useDatabase = false;

	bool ret = drv_databaseExists(dbName, ignoreErrors);
	
	if (!tmpdbName.isEmpty()) {
		//whatever result is - now we have to close temporary opened database:
		if (!closeDatabase())
			return false;
	}
	
	return ret;
}

#define createDatabase_CLOSE \
	{ if (!closeDatabase()) { \
		setError(i18n("Database \"%1\" created but could not be closed after creation.").arg(dbName) ); \
		return false; \
	} }

#define createDatabase_ERROR \
	{ createDatabase_CLOSE; return false; }

/*! See doc/dev/kexidb_issues.txt document, chapter "Table schema, query schema, etc. storage"
 for database schema documentation (detailed description of kexi__* 'system' tables).
*/
bool Connection::createDatabase( const QString &dbName )
{
	if (!checkConnected())
		return false;

	if (databaseExists( dbName )) {
		setError(ERR_OBJECT_EXISTS, i18n("Database \"%1\" already exists.").arg(dbName) );
		return false;
	}
	if (m_driver->isSystemDatabaseName( dbName )) {
		setError(ERR_SYSTEM_NAME_RESERVED, i18n("Cannot create database \"%1\". This name is reserved for system database.").arg(dbName) );
		return false;
	}
	if (m_driver->isFileDriver()) {
		//update connection data if filename differs
		m_data->setFileName( dbName );
	}

	QString tmpdbName;
	//some engines need to have opened any database before executing "create database"
	if (!useTemporaryDatabaseIfNeeded(tmpdbName))
		return false;
	
	//low-level create
	if (!drv_createDatabase( dbName )) {
		setError(i18n("Error creating database \"%1\" on the server.").arg(dbName) );
		closeDatabase();//sanity
		return false;
	}
	
	if (!tmpdbName.isEmpty()) {
		//whatever result is - now we have to close temporary opened database:
		if (!closeDatabase())
			return false;
	}

	if (!tmpdbName.isEmpty() || !m_driver->m_isDBOpenedAfterCreate) {
		//db need to be opened
		if (!useDatabase( dbName, false/*not yet kexi compatible!*/ )) {
			setError(i18n("Database \"%1\" created but could not be opened.").arg(dbName) );
			return false;
		}
	}
	else {
		//just for the rule
		m_usedDatabase = dbName;
	}
		
	Transaction trans;
	if (m_driver->transactionsSupported()) {
		trans = beginTransaction();
		if (!trans.active())
			return false;
	}
//not needed since closeDatabase() rollbacks transaction: TransactionGuard trans_g(this);
//	if (error())
//		return false;
	
	//-create system tables schema objects
	if (!setupKexiDBSystemSchema())
		return false;
	
	//-physically create system tables
	TableSchema *ts=m_kexiDBSystemtables.first();
	while (ts) {
		if (!drv_createTable( ts->name() ))
			createDatabase_ERROR;
		ts = m_kexiDBSystemtables.next();
	}

	//-create default part info
	if (!(ts = tableSchema("kexi__parts")))
		createDatabase_ERROR;
	FieldList *fl = ts->subList("p_id", "p_name", "p_mime", "p_url");
	if (!fl)
		createDatabase_ERROR;
	if (!insertRecord(*fl, QVariant(1), QVariant("Tables"), QVariant("kexi/table"), QVariant("http://koffice.org/kexi/")))
		createDatabase_ERROR;
	if (!insertRecord(*fl, QVariant(2), QVariant("Queries"), QVariant("kexi/query"), QVariant("http://koffice.org/kexi/")))
		createDatabase_ERROR;

	//-insert KexiDB version info:
	TableSchema *t_db = tableSchema("kexi__db");
	if (!t_db)
		createDatabase_ERROR;
	if ( !insertRecord(*t_db, "kexidb_major_ver", KexiDB::versionMajor())
		|| !insertRecord(*t_db, "kexidb_minor_ver", KexiDB::versionMinor()))
		createDatabase_ERROR;

	if (trans.active() && !commitTransaction(trans))
		createDatabase_ERROR;
	
	createDatabase_CLOSE;
	return true;
}

#undef createDatabase_CLOSE
#undef createDatabase_ERROR

bool Connection::useDatabase( const QString &dbName, bool kexiCompatible )
{
	if (!checkConnected())
		return false;
	
	if (dbName.isEmpty())
		return false;
	QString my_dbName = dbName;
//	if (my_dbName.isEmpty()) {
//		const QStringList& db_lst = databaseNames();
//		if (!db_lst.isEmpty())
//			my_dbName = db_lst.first();
//	}
	if (m_usedDatabase == my_dbName)
		return true; //already used

	if (!d->m_skip_databaseExists_check_in_useDatabase) {
	 	if (!databaseExists(my_dbName))
			return false; //database must exist
	}

	if (!m_usedDatabase.isEmpty() && !closeDatabase()) //close db if already used
		return false;

	m_usedDatabase = "";
	
	if (!drv_useDatabase( my_dbName )) {
		return false;
	}
	
	//-create system tables schema objects
	if (!setupKexiDBSystemSchema())
		return false;

	if (kexiCompatible && my_dbName.lower()!=anyAvailableDatabaseName().lower()) {
		//-get global database information
		int num;
		static QString notfound_str = i18n("\"%1\" database property not found");
		if (!querySingleNumber(
			"select db_value from kexi__db where db_property=\"kexidb_major_ver\"", num)) {
			d->errorInvalidDBContents(notfound_str.arg("kexidb_major_ver"));
			return false;
		}
		d->m_versionMajor = num;
		if (!querySingleNumber(
			"select db_value from kexi__db where db_property=\"kexidb_minor_ver\"", num)) {
			d->errorInvalidDBContents(notfound_str.arg("kexidb_minor_ver"));
			return false;
		}
		d->m_versionMinor = num;

		//** error if major version does not match
		if (m_driver->versionMajor()!=KexiDB::versionMajor()) {
			setError(ERR_INCOMPAT_DATABASE_VERSION, 
				i18n("Database version (%1) does not match Kexi application's version (%2)")
				.arg( QString("%1.%2").arg(versionMajor()).arg(versionMinor()) )
				.arg( QString("%1.%2").arg(KexiDB::versionMajor()).arg(KexiDB::versionMinor()) ) );
			return false;
		}
		if (m_driver->versionMinor()!=KexiDB::versionMinor()) {
			//js TODO: COMPATIBILITY CODE HERE!
			//js TODO: CONVERSION CODE HERE (or signal that conversion is needed)
		}
	}
	m_usedDatabase = my_dbName;
	return true;
}

bool Connection::closeDatabase()
{
	if (m_usedDatabase.isEmpty())
		return true; //no db used
	if (!checkConnected())
		return true;

	bool ret = true;

/*! \todo (js) add CLEVER algorithm here for nested transactions */
	if (m_driver->transactionsSupported()) {
		//rollback all transactions
		QValueList<Transaction>::iterator it;
		d->m_dont_remove_transactions=true; //lock!
		for (it=d->m_transactions.begin(); it!= d->m_transactions.end(); ++it) {
			if (!rollbackTransaction(*it)) {//rollback as much as you can, don't stop on prev. errors
				ret = false;
			}
			else {
				KexiDBDbg << "Connection::closeDatabase(): transaction rolled back!" << endl;
				KexiDBDbg << "Connection::closeDatabase(): trans.refcount==" << 
				 ((*it).m_data ? QString::number((*it).m_data->refcount) : "(null)") << endl;
			}
		}
		d->m_dont_remove_transactions=false; //unlock!
		d->m_transactions.clear(); //free trans. data
	}

	//delete own cursors:
	m_cursors.clear();
	//delete own schemas
	m_tables.clear();
	m_kexiDBSystemtables.clear();
	m_queries.clear();
			
	if (!drv_closeDatabase())
		return false;

	m_usedDatabase = "";
	KexiDBDbg << "Connection::closeDatabase(): " << ret << endl;
	return ret;
}

bool Connection::useTemporaryDatabaseIfNeeded(QString &tmpdbName)
{
	if (!m_driver->isFileDriver() && m_driver->beh->USING_DATABASE_REQUIRED_TO_CONNECT
	 && !isDatabaseUsed()) {
		//we have no db used, but it is required by engine to have used any!
		tmpdbName = anyAvailableDatabaseName();
		if (tmpdbName.isEmpty()) {
			setError(ERR_NO_DB_USED, i18n("Cannot find any database for temporary connection.") );
			return false;
		}
		if (!useDatabase(tmpdbName, false)) {
			setError(errorNum(), i18n("Error during starting temporary connection using \"%1\" database name.")
				.arg(tmpdbName) );
			return false;
		}
	}
	return true;
}

bool Connection::dropDatabase( const QString &dbName )
{
	if (!checkConnected())
		return false;
	
	QString dbToDrop;
	if (dbName.isEmpty() && m_usedDatabase.isEmpty()) {
		if (!m_driver->isFileDriver() 
		 || (m_driver->isFileDriver() && m_data->fileName().isEmpty()) ) {
			setError(ERR_NO_NAME_SPECIFIED, i18n("Cannot drop database - name not specified.") );
			return false;
		}
		//this is a file driver so reuse previously passed filename
		dbToDrop = m_data->fileName();
	}
	else {
		if (dbName.isEmpty()) {
			dbToDrop = m_usedDatabase;
		} else {
			if (m_driver->isFileDriver()) //lets get full path
				dbToDrop = QFileInfo(dbName).absFilePath();
			else
				dbToDrop = dbName;
		}
	}

	if (dbToDrop.isEmpty()) {
		setError(ERR_NO_NAME_SPECIFIED, i18n("Cannot delete database - name not specified.") );
		return false;
	}

	if (m_driver->isSystemDatabaseName( dbToDrop )) {
		setError(ERR_SYSTEM_NAME_RESERVED, i18n("Cannot delete system database \"%1\".").arg(dbToDrop) );
		return false;
	}
	
	if (isDatabaseUsed() && m_usedDatabase == dbToDrop) {
		//we need to close database because cannot drop used this database
		if (!closeDatabase())
			return false;
	}
	
	QString tmpdbName;
	//some engines need to have opened any database before executing "drop database"
	if (!useTemporaryDatabaseIfNeeded(tmpdbName))
		return false;
	
	//ok, now we have access to dropping
	bool ret = drv_dropDatabase( dbToDrop );
	
	if (!tmpdbName.isEmpty()) {
		//whatever result is - now we have to close temporary opened database:
		if (!closeDatabase())
			return false;
	}
	return ret;
}

QStringList Connection::tableNames(bool also_system_tables)
{
	QStringList list;
	
	if (!isDatabaseUsed())
		return list;

	Cursor *c = executeQuery(QString(
	 "select o_name from kexi__objects where o_type=%1").arg(KexiDB::TableObjectType));
	if (!c)
		return list;
	for (c->moveFirst(); !c->eof(); c->moveNext())
	{
		QString tname = c->value(0).toString(); //kexi__objects.o_name
		if (Kexi::isIdentifier( tname )) {
			list.append(tname);
		}
	}

	deleteCursor(c);

	if (also_system_tables) {
		list += Connection::kexiDBSystemTableNames();
	}
	return list;
}

//! \todo (js): this will depend on KexiDB lib version
const QStringList& Connection::kexiDBSystemTableNames()
{
	if (KexiDB_kexiDBSystemTableNames.isEmpty()) {
		KexiDB_kexiDBSystemTableNames
		<< "kexi__objects"
		<< "kexi__objectdata"
		<< "kexi__fields"
		<< "kexi__querydata"
		<< "kexi__queryfields"
		<< "kexi__querytables"
		<< "kexi__db"
		;
	}
	return KexiDB_kexiDBSystemTableNames;
}

int Connection::versionMajor() const
{
	return d->m_versionMinor;
}

int Connection::versionMinor() const
{
	return d->m_versionMinor;
}

QValueList<int> Connection::queryIds()
{
	return objectIds(KexiDB::QueryObjectType);
}

QValueList<int> Connection::objectIds(int objType)
{
	QValueList<int> list;
	
	if (!isDatabaseUsed())
		return list;

	Cursor *c = executeQuery(QString("select o_id, o_name from kexi__objects where o_type=%1").arg(objType));
	if (!c)
		return list;
	for (c->moveFirst(); !c->eof(); c->moveNext())
	{
		QString tname = c->value(1).toString(); //kexi__objects.o_name
		if (Kexi::isIdentifier( tname )) {
			list.append(c->value(0).toInt()); //kexi__objects.o_id
		}
	}

	deleteCursor(c);

	return list;
	
/*	switch (objType) {
	case KexiDB::TableObject:
		return tableNames();
	case KexiDB::QueryObject:
		return queryNames();
	default: ;
	}
	return list;*/
}

#if 0
inline QString Connection::internal_valueToSQL( Field::Type ftype, const QVariant& v )
{
	if (v.isNull())
		return "NULL";
	switch (ftype) {
		case Field::Byte:
		case Field::ShortInteger:
		case Field::Integer:
		case Field::Float:
		case Field::Double:
		case Field::BigInteger:
			return v.toString();
//TODO: here special encoding method needed
		case Field::Boolean:
			return QString::number(v.toInt()); //0 or 1
		case Field::Date:
		case Field::Time:
			return QString("\"")+v.toString()+"\"";
		case Field::DateTime:
			return QString("\"") + v.toDate().toString( Qt::ISODate ) + " " + v.toTime().toString( Qt::ISODate ) +"\"";
		case Field::Text:
		case Field::LongText: {
			QString s = v.toString();
//js: TODO: for sqlite we use single ' chars, what with other engines?
			return QString("'")+s.replace( '"', "\\\"" ) + "'"; 
		}
		case Field::BLOB:
//TODO: here special encoding method needed
			return QString("'")+v.toString()+"'";
		case Field::InvalidType:
			return "!INVALIDTYPE!";
		default:
			KexiDBDbg << "Connection_valueToSQL(): UNKNOWN!" << endl;
			return QString::null;
	}
	return QString::null;
}

QString Connection::valueToSQL( const Field::Type ftype, const QVariant& v ) const
{
	kdDebug() << "valueToSQL(" << (m_driver ? m_driver->sqlTypeName(ftype) : "??") << ", " << Connection_valueToSQL( ftype, v ) <<")" << endl;
	return Connection_valueToSQL( ftype, v );
}

QString Connection::valueToSQL( const Field *field, const QVariant& v ) const
{
	kdDebug() << "valueToSQL(" << (m_driver ? ( field ? m_driver->sqlTypeName(field->type()): "!field") : "??") << ", " << Connection_valueToSQL( (field ? field->type() : Field::InvalidType), v ) <<")" << endl;
	return Connection_valueToSQL( (field ? field->type() : Field::InvalidType), v );
}
#endif

QString Connection::createTableStatement( const KexiDB::TableSchema& tableSchema ) const
{
	QString sql = "CREATE TABLE " + tableSchema.name() + " (";
	bool first=true;
	Field::ListIterator it( tableSchema.m_fields );
	Field *field;
	for (;(field = it.current())!=0; ++it) {
		if (first)
			first = false;
		else
			sql += ", ";
		QString v = field->m_name + " ";
		if (field->isAutoIncrement() && m_driver->beh->SPECIAL_AUTO_INCREMENT_DEF) {
			v += m_driver->beh->AUTO_INCREMENT_FIELD_OPTION;
		}
		else {
			if (field->isUnsigned())
				v += (m_driver->beh->UNSIGNED_TYPE_KEYWORD + " ");
			v += m_driver->m_typeNames[field->m_type];
			if (field->m_length>0)
				v += QString("(%1)").arg(field->m_length);
			if (field->isAutoIncrement())
				v += (" " + m_driver->beh->AUTO_INCREMENT_FIELD_OPTION);
	//TODO: here is automatically a single-field key created
			if (field->isPrimaryKey())
				v += " PRIMARY KEY";
			if (!field->isPrimaryKey() && field->isUniqueKey())
				v += " UNIQUE";
#ifndef Q_WS_WIN
#warning IS this ok for all engines?: if (!field->isAutoIncrement() && !field->isPrimaryKey() && field->isNotNull()) 
#endif
			if (!field->isAutoIncrement() && !field->isPrimaryKey() && field->isNotNull()) 
				v += " NOT NULL"; //only add not null option if no autocommit is set
			if (field->defaultValue().isValid())
				v += QString(" DEFAULT ") + m_driver->valueToSQL( field, field->m_defaultValue );
		}
		sql += v;
	}
	sql += ")";
	return sql;
}

//yeah, it is very efficient:
#define C_A(a) , const QVariant& c ## a

#define V_A0 m_driver->valueToSQL( tableSchema.field(0), c0 )
#define V_A(a) +","+m_driver->valueToSQL( \
	tableSchema.field(a) ? tableSchema.field(a)->type() : Field::Text, c ## a )

#define C_INS_REC(args, vals) \
	bool Connection::insertRecord(KexiDB::TableSchema &tableSchema args) {\
		KexiDBDbg<<"******** "<< QString("INSERT INTO ") + tableSchema.name() + " VALUES (" + vals + ")" <<endl; \
		return drv_executeSQL( \
		 QString("INSERT INTO ") + tableSchema.name() + " VALUES (" + vals + ")" \
		); \
	}

#define C_INS_REC_ALL \
C_INS_REC( C_A(0), V_A0 ) \
C_INS_REC( C_A(0) C_A(1), V_A0 V_A(1) ) \
C_INS_REC( C_A(0) C_A(1) C_A(2), V_A0 V_A(1) V_A(2) ) \
C_INS_REC( C_A(0) C_A(1) C_A(2) C_A(3), V_A0 V_A(1) V_A(2) V_A(3) ) \
C_INS_REC( C_A(0) C_A(1) C_A(2) C_A(3) C_A(4), V_A0 V_A(1) V_A(2) V_A(3) V_A(4) ) \
C_INS_REC( C_A(0) C_A(1) C_A(2) C_A(3) C_A(4) C_A(5), V_A0 V_A(1) V_A(2) V_A(3) V_A(4) V_A(5) ) \
C_INS_REC( C_A(0) C_A(1) C_A(2) C_A(3) C_A(4) C_A(5) C_A(6), V_A0 V_A(1) V_A(2) V_A(3) V_A(4) V_A(5) V_A(6) ) \
C_INS_REC( C_A(0) C_A(1) C_A(2) C_A(3) C_A(4) C_A(5) C_A(6) C_A(7), V_A0 V_A(1) V_A(2) V_A(3) V_A(4) V_A(5) V_A(6) V_A(7) )

C_INS_REC_ALL

#undef V_A0
#undef V_A
#undef C_INS_REC

#define V_A0 value += m_driver->valueToSQL( flist->first(), c0 );
#define V_A( a ) value += ("," + m_driver->valueToSQL( flist->next(), c ## a ));
//#define V_ALAST( a ) valueToSQL( flist->last(), c ## a )

#define C_INS_REC(args, vals) \
	bool Connection::insertRecord(FieldList& fields args) \
	{ \
		QString value; \
		Field::List *flist = fields.fields(); \
		vals \
		return drv_executeSQL( \
			QString("INSERT INTO ") + \
		((fields.fields()->first() && fields.fields()->first()->table()) ? fields.fields()->first()->table()->name() : "??") \
		+ "(" + fields.sqlFieldsList() + ") VALUES (" + value + ")" \
		); \
	}

C_INS_REC_ALL

#undef C_A
#undef V_A
#undef V_ALAST
#undef C_INS_REC
#undef C_INS_REC_ALL

bool Connection::insertRecord(TableSchema &tableSchema, QValueList<QVariant>& values)
{
	Field::List *fields = tableSchema.fields();
	Field *f = fields->first();
	QString s_val; 
	QValueList<QVariant>::iterator it = values.begin();
	int i=0;
	while (f && (it!=values.end())) {
		if (!s_val.isEmpty())
			s_val += ",";
		s_val += m_driver->valueToSQL( f, *it );
		KexiDBDbg << "val" << i++ << ": " << m_driver->valueToSQL( f, *it ) << endl;
		++it;
		f=fields->next();
	}
	KexiDBDbg<<"******** "<< 
	(QString("INSERT INTO ") + tableSchema.name() + " VALUES (" + s_val + ")") <<endl;
	
	return drv_executeSQL(
		QString("INSERT INTO ") + tableSchema.name() + " VALUES (" + s_val + ")"
	);
}

bool Connection::insertRecord(FieldList& fields, QValueList<QVariant>& values)
{
	Field::List *flist = fields.fields();
	Field *f = flist->first();
	if (!f)
		return false;

	QString s_val; 
	QValueList<QVariant>::iterator it = values.begin();
	int i=0;
	while (f && (it!=values.end())) {
		if (!s_val.isEmpty())
			s_val += ",";
		s_val += m_driver->valueToSQL( f, *it );
		KexiDBDbg << "val" << i++ << ": " << m_driver->valueToSQL( f, *it ) << endl;
		++it;
		f=flist->next();
	}
	
	return drv_executeSQL(
		QString("INSERT INTO ") + flist->first()->table()->name() + "("
		 + fields.sqlFieldsList() + ") VALUES (" + s_val + ")"
	);
}

QString Connection::selectStatement( KexiDB::QuerySchema& querySchema ) const
{
	if (querySchema.fieldCount()<1)
		return QString::null;

	if (!querySchema.statement().isEmpty())
		return querySchema.statement();

	QString sql;
	Field::List *fields = querySchema.fields();
	for (Field *f = fields->first(); f; f = fields->next()) {
		if (sql.isEmpty())
			sql = "SELECT ";
		else
			sql += ", ";
		if (f->isQueryAsterisk()) {
			if (static_cast<QueryAsterisk*>(f)->isSingleTableAsterisk()) //single-table *
				sql += (f->table()->name() + ".*");
			else //all-tables *
				sql += "*";
		}
		else {
			if (!f->table()) //sanity check
				return QString::null;
			sql += (f->table()->name() + "." + f->name());
		}
	}
	sql += " FROM ";
	TableSchema::List* tables = querySchema.tables();
	if (!tables || tables->isEmpty()) //sanity check
		return QString::null;
	
	QString s_from;
	for (TableSchema *table = tables->first(); table; table = tables->next()) {
		s_from += table->name();
		if(table != tables->getLast())
			s_from += ", ";
	}
	sql += s_from;

//! \todo (js) add WHERE and other sql parts

	return sql;
}

QString Connection::selectStatement( KexiDB::TableSchema& tableSchema ) const
{
	return selectStatement( *tableSchema.query() );
}

#define createTable_ERR \
	{ KexiDBDbg << "Connection::createTable(): ERROR!" <<endl; \
	  rollbackAutoCommitTransaction(trans); \
	  return false; }

Field* Connection::findSystemFieldName(KexiDB::FieldList* fieldlist)
{
	Field *f = fieldlist->fields()->first();
	while (f) {
		if (m_driver->isSystemFieldName( f->name() ))
			return f;
		f = fieldlist->fields()->next();
	}
	return 0;
}

int Connection::lastInsertedAutoIncValue(const QString& aiFieldName, const QString& tableName)
{
	int row_id = drv_lastInsertRowID();
	KexiDB::RowData rdata;
	if (row_id<=0 || !querySingleRecord(
	 QString("select ")+aiFieldName+" from "+tableName+" where "+m_driver->beh->ROW_ID_FIELD_NAME
	 +"="+QString::number(row_id), rdata)) {
		KexiDBDbg << "Connection::lastInsertedAutoIncValue(): row_id<=0 || !querySingleRecord()" << endl;
	 	return -1;
	}
	return rdata[0].toInt();
}

int Connection::lastInsertedAutoIncValue(const QString& aiFieldName, const KexiDB::TableSchema& table)
{
	return lastInsertedAutoIncValue(aiFieldName,table.name());
}

bool Connection::createTable( KexiDB::TableSchema* tableSchema )
{
	if (!tableSchema || !checkIsDatabaseUsed())
		return false;

	//check if there are any fields
	if (tableSchema->fieldCount()<1) {
		setError(ERR_CANNOT_CREATE_EMPTY_OBJECT, i18n("Cannot create table without fields."));
		return false;
	}
	if (m_driver->isSystemObjectName( tableSchema->name() )) {
		setError(ERR_SYSTEM_NAME_RESERVED, i18n("System name \"%1\" cannot be used as table name.")
			.arg(tableSchema->name()));
		return false;
	}
	{
		Field *sys_field = findSystemFieldName(tableSchema);
		if (sys_field) {
			setError(ERR_SYSTEM_NAME_RESERVED, 
				i18n("System name \"%1\" cannot be used as one of fields in \"%2\" table.")
				.arg(sys_field->name()).arg(tableSchema->name()));
			return false;
		}
	}
	
	Transaction trans;
	if (!beginAutoCommitTransaction(trans))
		return false;
	
	if (!drv_createTable(*tableSchema))
		createTable_ERR;
	
	//add schema info to kexi__* tables
	if (!storeObjectSchemaData( *tableSchema, true ))
		createTable_ERR;
/*	TableSchema *ts = m_tables_byname["kexi__objects"];
	if (!ts)
		return false;
		
	FieldList *fl = ts->subList("o_type", "o_name", "o_caption", "o_help");
	if (!fl)
		return false;
		
	if (!insertRecord(*fl, QVariant(tableSchema->type()), QVariant(tableSchema->name()),
		QVariant(tableSchema->caption()), QVariant(tableSchema->description()) ))
		createTable_ERR;
	
	delete fl;*/
	
//	if (!insertRecord(*ts, QVariant()/*autoinc*/, QVariant(tableSchema->type()), QVariant(tableSchema->name()),
//		QVariant(tableSchema->caption()), QVariant(tableSchema->description())))
//		createTable_ERR;
/*	int obj_id = lastInsertedAutoIncValue("o_id",*ts);
	if (obj_id<=0)
		createTable_ERR;
	KexiDBDbg << "######## obj_id == " << obj_id << endl;*/
	
	TableSchema *ts = m_tables_byname["kexi__fields"];
	if (!ts)
		return false;
	FieldList *fl = ts->subList(
		"t_id",
		"f_type",
		"f_name",
		"f_length",
		"f_precision",
		"f_constraints",
		"f_options",
		"f_default",
		"f_order",
		"f_caption",
		"f_help"
	);
	if (!fl)
		return false;
	
	Field::List *fields = tableSchema->fields();
	Field *f = fields->first();
	int order = 0;
	while (f) {
		QValueList<QVariant> vals;
		vals
		<< QVariant(tableSchema->id())//obj_id)
		<< QVariant(f->type())
		<< QVariant(f->name())
		<< QVariant(f->length())
		<< QVariant(f->precision())
		<< QVariant(f->constraints())
		<< QVariant(f->options())
		<< QVariant(f->defaultValue())
		<< QVariant(f->order())
		<< QVariant(f->caption())
		<< QVariant(f->description());
		
		if (!insertRecord(*fl, vals ))
			createTable_ERR;
			
		f = fields->next();
		order++;
	}
	delete fl;
		
	//store objects locally:
	m_tables.insert(tableSchema->m_id, tableSchema);
	m_tables_byname.insert(tableSchema->m_name.lower(), tableSchema);
			
	return commitAutoCommitTransaction(trans);
}

//! internal
bool Connection::removeObject( uint objId )
{
	//remove table schema from kexi__* tables
	TableSchema *ts = m_tables_byname["kexi__objects"];
	if (!KexiDB::deleteRow(*this, ts, "o_id", objId)) //schema entry
		return false;
	ts = m_tables_byname["kexi__objectdata"];
	if (!KexiDB::deleteRow(*this, ts, "o_id", objId)) //schema entry
		return false;
	return true;
}

bool Connection::dropTable( KexiDB::TableSchema* tableSchema )
{
	if (!tableSchema)
		return false;

	//sanity checks:
	if (m_driver->isSystemObjectName( tableSchema->name() )) {
		setError(ERR_SYSTEM_NAME_RESERVED, i18n("Table \"%1\" cannot be removed.")
			.arg(tableSchema->name()));
		return false;
	}
//js TODO DO THIS INSIDE TRANSACTION!!!!

	m_sql = "DROP TABLE " + tableSchema->name();
	if (!drv_executeSQL(m_sql))
		return false;

	TableSchema *ts = m_tables_byname["kexi__fields"];
	if (!KexiDB::deleteRow(*this, ts, "t_id", tableSchema->id())) //field entries
		return false;

	//remove table schema from kexi__objects table
	if (!removeObject( tableSchema->id() )) {
		return false;
	}

//TODO(js): update any structure (e.g. query) that depend on this table!
	m_tables_byname.remove(tableSchema->name().lower());
	m_tables.remove(tableSchema->id());
	return true;
}

bool Connection::dropTable( const QString& table )
{
	TableSchema* ts = tableSchema( table );
	if (!ts) {
		return false;
	}
	return dropTable(ts);
}

bool Connection::dropQuery( KexiDB::QuerySchema* querySchema )
{
	if (!querySchema)
		return false;

//js TODO DO THIS INSIDE TRANSACTION!!!!
	TableSchema *ts = m_tables_byname["kexi__querydata"];
	if (!KexiDB::deleteRow(*this, ts, "q_id", querySchema->id()))
		return false;

	ts = m_tables_byname["kexi__queryfields"];
	if (!KexiDB::deleteRow(*this, ts, "q_id", querySchema->id()))
		return false;

	ts = m_tables_byname["kexi__querytables"];
	if (!KexiDB::deleteRow(*this, ts, "q_id", querySchema->id()))
		return false;

	//remove query schema from kexi__objects table
	if (!removeObject( querySchema->id() )) {
		return false;
	}
	
//TODO(js): update any structure that depend on this table!
	m_queries_byname.remove(querySchema->name().lower());
	m_queries.remove(querySchema->id());
	return true;
}

bool Connection::drv_createTable( const KexiDB::TableSchema& tableSchema )
{
	m_sql = createTableStatement(tableSchema);
	KexiDBDbg<<"******** "<<m_sql<<endl;
	return drv_executeSQL(m_sql);
}

bool Connection::drv_createTable( const QString& tableSchemaName )
{
	TableSchema *ts = m_tables_byname[tableSchemaName];
	if (!ts)
		return false;
	return drv_createTable(*ts);
}

bool Connection::beginAutoCommitTransaction(Transaction &trans)
{
	if (!m_autoCommit)
		return true;
		
	// commit current transaction (if present) for drivers 
	// that allow single transaction per connection
	if (m_driver->m_features & Driver::SingleTransactions) {
		if (!commitTransaction(d->m_default_trans, true)) 
			return false; //we have real error
	}
	else if (!(m_driver->m_features & Driver::MultipleTransactions)) {
		return true; //no trans. supported at all - just return
	}
	trans=beginTransaction();
	return !error();
}

bool Connection::commitAutoCommitTransaction(const Transaction& trans)
{
	if (trans.isNull() || !m_driver->transactionsSupported())
		return true;
	return commitTransaction(trans, true);
}

bool Connection::rollbackAutoCommitTransaction(const Transaction& trans)
{
	if (trans.isNull() || !m_driver->transactionsSupported())
		return true;
	return rollbackTransaction(trans);
}

#define SET_ERR_TRANS_NOT_SUPP \
	{ setError(ERR_UNSUPPORTED_DRV_FEATURE, \
	 i18n("Transactions are not supported for \"%1\" driver.").arg(m_driver->name() )); }

#define SET_BEGIN_TR_ERROR \
	 { if (!error()) \
		setError(ERR_ROLLBACK_OR_COMMIT_TRANSACTION, i18n("Begin transaction failed")); }

Transaction Connection::beginTransaction()
{
	if (!isDatabaseUsed())
		return Transaction::null;
	Transaction trans;
	if (m_driver->m_features & Driver::SingleTransactions) {
		if (d->m_default_trans.active()) {
			setError(ERR_TRANSACTION_ACTIVE, i18n("Transaction already started.") );
			return Transaction::null;
		}
		if (!(trans.m_data = drv_beginTransaction())) {
			SET_BEGIN_TR_ERROR;
			return Transaction::null;
		}
		d->m_default_trans = trans;
		d->m_transactions.append(trans);
		return d->m_default_trans;
	}
	else if (m_driver->m_features & Driver::MultipleTransactions) {
		if (!(trans.m_data = drv_beginTransaction())) {
			SET_BEGIN_TR_ERROR;
			return Transaction::null;
		}
		d->m_transactions.append(trans);
		return trans;
	}
	
	SET_ERR_TRANS_NOT_SUPP;
	return Transaction::null;
}

bool Connection::commitTransaction(const Transaction trans, bool ignore_inactive)
{
	if (!isDatabaseUsed())
		return false;
	if (!m_driver->transactionsSupported()) {
		SET_ERR_TRANS_NOT_SUPP;
		return false;
	}
	Transaction t = trans;
	if (!t.active()) { //try default tr.
		if (!d->m_default_trans.active()) {
			if (ignore_inactive)
				return true;
			setError(ERR_NO_TRANSACTION_ACTIVE, i18n("Transaction not started.") );
			return false;
		}
		t = d->m_default_trans;
		d->m_default_trans = Transaction::null; //now: no default tr.
	}
	bool ret = drv_commitTransaction(t.m_data);
	if (t.m_data)
		t.m_data->m_active = false; //now this transaction if inactive
	if (!d->m_dont_remove_transactions) //true=transaction obj will be later removed from list
		d->m_transactions.remove(t);
	if (!ret && !error())
		setError(ERR_ROLLBACK_OR_COMMIT_TRANSACTION, i18n("Error on commit transaction"));
	return ret;
}

bool Connection::rollbackTransaction(const Transaction trans, bool ignore_inactive)
{
	if (!isDatabaseUsed())
		return false;
	if (!m_driver->transactionsSupported()) {
		SET_ERR_TRANS_NOT_SUPP;
		return false;
	}
	Transaction t = trans;
	if (!t.active()) { //try default tr.
		if (!d->m_default_trans.active()) {
			if (ignore_inactive)
				return true;
			setError(ERR_NO_TRANSACTION_ACTIVE, i18n("Transaction not started.") );
			return false;
		}
		t = d->m_default_trans;
		d->m_default_trans = Transaction::null; //now: no default tr.
	}
	bool ret = drv_rollbackTransaction(t.m_data);
	if (t.m_data)
		t.m_data->m_active = false; //now this transaction if inactive
	if (!d->m_dont_remove_transactions) //true=transaction obj will be later removed from list
		d->m_transactions.remove(t);
	if (!ret && !error())
		setError(ERR_ROLLBACK_OR_COMMIT_TRANSACTION, i18n("Error on rollback transaction"));
	return ret;
}

#undef SET_ERR_TRANS_NOT_SUPP
#undef SET_BEGIN_TR_ERROR

/*bool Connection::duringTransaction()
{
	return drv_duringTransaction();
}*/

Transaction& Connection::defaultTransaction() const
{
	return d->m_default_trans;
}

void Connection::setDefaultTransaction(const Transaction& trans)
{
	if (!checkIsDatabaseUsed())
		return;
	if (!trans.active() || !m_driver->transactionsSupported())
		return;
	d->m_default_trans = trans;
}

const QValueList<Transaction>& Connection::transactions()
{
	return d->m_transactions;
}

bool Connection::autoCommit() const
{
	return m_autoCommit;
}

bool Connection::setAutoCommit(bool on)
{
	if (m_autoCommit == on)
		return true;
	if (!drv_setAutoCommit(on))
		return false;
	m_autoCommit = on;
	return true;
}

TransactionData* Connection::drv_beginTransaction()
{
	if (!drv_executeSQL( "BEGIN" ))
		return 0;
	return new TransactionData(this);
}

bool Connection::drv_commitTransaction(TransactionData *)
{
	return drv_executeSQL( "COMMIT" );
}

bool Connection::drv_rollbackTransaction(TransactionData *)
{
	return drv_executeSQL( "ROLLBACK" );
}

bool Connection::drv_setAutoCommit(bool /*on*/)
{
	return true;
}

Cursor* Connection::executeQuery( const QString& statement, uint cursor_options )
{
	if (statement.isEmpty())
		return 0;
	Cursor *c = prepareQuery( statement, cursor_options );
	if (!c)
		return 0;
	if (!c->open()) {//err - kill that
		setError(c);
		delete c;
		return 0;
	}
	return c;
}

Cursor* Connection::executeQuery( QuerySchema& query, uint cursor_options )
{
	Cursor *c = prepareQuery( query, cursor_options );
	if (!c)
		return 0;
	if (!c->open()) {//err - kill that
		setError(c);
		delete c;
		return 0;
	}
	return c;
}

Cursor* Connection::executeQuery( TableSchema& table, uint cursor_options )
{
	return executeQuery( *table.query(), cursor_options );
}

/*Cursor* Connection::prepareQuery( QuerySchema& query, uint cursor_options )
{
	Cursor *c = prepareQuery( query, cursor_options );
	if (!c)
		return 0;
	if (!c->open()) {//err - kill that
		setError(c);
		delete c;
		return 0;
	}
	return c;
}*/

Cursor* Connection::prepareQuery( TableSchema& table, uint cursor_options )
{
	return prepareQuery( *table.query(), cursor_options );
}

bool Connection::deleteCursor(Cursor *cursor)
{
	if (!cursor)
		return false;
	if (cursor->connection()!=this) {//illegal call
		KexiDBDbg << "Connection::deleteCursor(): WARNING! Cannot delete the cursor not owned by the same connection!" << endl;
		return false;
	}
	bool ret = cursor->close();
	delete cursor;
	return ret;
}

bool Connection::setupObjectSchemaData( const KexiDB::RowData &data, SchemaData &sdata )
{
	//not found: retrieve schema
/*	KexiDB::Cursor *cursor;
	if (!(cursor = executeQuery( QString("select * from kexi__objects where o_id='%1'").arg(objId) )))
		return false;
	if (!cursor->moveFirst()) {
		deleteCursor(cursor);
		return false;
	}*/
	//if (!ok) {
		//deleteCursor(cursor);
		//return 0;
//	}
	bool ok;
	sdata.m_id = data[0].toInt(&ok);
	if (!ok) {
		return false;
	}
	sdata.m_name = data[2].toString();
	if (!Kexi::isIdentifier( sdata.m_name )) {
		setError(ERR_INVALID_IDENTIFIER, i18n("Invalid object name \"%1\"").arg(sdata.m_name));
		return false;
	}
	sdata.m_caption = data[3].toString();
	sdata.m_desc = data[4].toString();
	
	KexiDBDbg<<"@@@ Connection::setupObjectSchemaData() == " << sdata.schemaDataDebugString() << endl;
	return true;
}

bool Connection::setupObjectSchemaData( int objectID, SchemaData &sdata )
{
	RowData data;
	if (!querySingleRecord(QString("select o_id, o_type, o_name, o_caption, o_desc from kexi__objects where o_id=%1").arg(objectID), data))
		return false;
	return setupObjectSchemaData( data, sdata );
}

bool Connection::findObjectSchemaData( int objectType, const QString& objectName, SchemaData &sdata )
{
	RowData data;
	if (!querySingleRecord(QString("select o_id, o_type, o_name, o_caption, o_desc from kexi__objects where o_type=%1 and o_name=%2")
		.arg(objectType).arg(m_driver->valueToSQL(Field::Text, objectName)), data))
		return false;
	return setupObjectSchemaData( data, sdata );
}

bool Connection::storeObjectSchemaData( SchemaData &sdata, bool newObject )
{
	TableSchema *ts = m_tables_byname["kexi__objects"];
	if (!ts)
		return false;
	if (newObject) {
		FieldList *fl = ts->subList("o_type", "o_name", "o_caption", "o_desc");
		if (!fl)
			return false;
		if (!insertRecord(*fl, QVariant(sdata.type()), QVariant(sdata.name()),
			QVariant(sdata.caption()), QVariant(sdata.description()) ))
			return false;
		//fetch newly assigned ID
		int obj_id = lastInsertedAutoIncValue("o_id",*ts);
		KexiDBDbg << "######## obj_id == " << obj_id << endl;
		if (obj_id<=0)
			return false;
		sdata.m_id = obj_id;
		return true;
	}
	FieldList *fl = ts->subList("o_id", "o_type", "o_name", "o_caption", "o_desc");
	if (!fl)
		return false;
	return drv_executeSQL(QString("update kexi__objects set o_type=%2, o_caption=%3, o_desc=%4 where o_id=%1")
		.arg(sdata.id()).arg(sdata.type())
		.arg(m_driver->valueToSQL(KexiDB::Field::Text, sdata.caption()))
		.arg(m_driver->valueToSQL(KexiDB::Field::Text, sdata.description())) );
}

bool Connection::querySingleRecord(const QString& sql, KexiDB::RowData &data)
{
	KexiDB::Cursor *cursor;
	m_sql = sql;
	if (!(cursor = executeQuery( m_sql ))) {
		KexiDBDbg << "Connection::querySingleRecord(): !executeQuery()" << endl;
		return false;
	}
	if (!cursor->moveFirst() || cursor->eof()) {
		KexiDBDbg << "Connection::querySingleRecord(): !cursor->moveFirst() || cursor->eof()" << endl;
		deleteCursor(cursor);
		return false;
	}
	cursor->storeCurrentRow(data);
	return deleteCursor(cursor);
}

bool Connection::querySingleString(const QString& sql, QString &value)
{
	KexiDB::Cursor *cursor;
	m_sql = sql;
	if (!(cursor = executeQuery( m_sql ))) {
		KexiDBDbg << "Connection::querySingleRecord(): !executeQuery()" << endl;
		return false;
	}
	if (!cursor->moveFirst() || cursor->eof()) {
		KexiDBDbg << "Connection::querySingleRecord(): !cursor->moveFirst() || cursor->eof()" << endl;
		deleteCursor(cursor);
		return false;
	}
	value = cursor->value(0).toString();
	return deleteCursor(cursor);
}

bool Connection::querySingleNumber(const QString& sql, int &number)
{
	static QString str;
	static bool ok;
	if (!querySingleString(sql, str))
		return false;
	number = str.toInt(&ok);
	return ok;
}

bool Connection::resultExists(const QString& sql, bool &success)
{
	KexiDB::Cursor *cursor;
	m_sql = sql;
	if (!(cursor = executeQuery( m_sql ))) {
		KexiDBDbg << "Connection::querySingleRecord(): !executeQuery()" << endl;
		success = false;
		return false;
	}
	success = true;
	if (!cursor->moveFirst() || cursor->eof()) {
		KexiDBDbg << "Connection::querySingleRecord(): !cursor->moveFirst() || cursor->eof()" << endl;
		deleteCursor(cursor);
		return false;
	}
	return true;
}

KexiDB::TableSchema* Connection::setupTableSchema( const KexiDB::RowData &data )//KexiDB::Cursor *table_cur )
{
	TableSchema *t = new TableSchema( this );
	if (!setupObjectSchemaData( data, *t )) {
		delete t;
		return 0;
	}
/*	if (!deleteCursor(table_cur)) {
		delete t;
		return 0;
	}*/
	
	KexiDB::Cursor *cursor;
	if (!(cursor = executeQuery( 
		QString("select t_id, f_type, f_name, f_length, f_precision, f_constraints, f_options, f_order, f_caption, f_help"
		" from kexi__fields where t_id=%1 order by f_order").arg(t->m_id) ))) {
		return 0;
	}
	if (!cursor->moveFirst()) {
		deleteCursor(cursor);
		return 0;
	}
	bool ok;
	while (!cursor->eof()) {
		KexiDBDbg<<"@@@ f_name=="<<cursor->value(2).asCString()<<endl;

		int f_type = cursor->value(1).toInt(&ok);
		if (!ok)
			break;
		int f_len = cursor->value(3).toInt(&ok);
		if (!ok)
			break;
		int f_prec = cursor->value(4).toInt(&ok);
		if (!ok)
			break;
		int f_constr = cursor->value(5).toInt(&ok);
		if (!ok)
			break;
		int f_opts = cursor->value(6).toInt(&ok);
		if (!ok)
			break;
		
		if (!Kexi::isIdentifier( cursor->value(2).asString() )) {
			setError(ERR_INVALID_IDENTIFIER, i18n("Invalid object name \"%1\"")
				.arg( cursor->value(2).asString() ));
			ok=false;
			break;
		}

		Field *f = new Field(
			cursor->value(2).asString(), (Field::Type)f_type, f_constr, f_len, f_prec, f_opts );
		f->setDefaultValue( cursor->value(7).toCString() );
		f->m_caption = cursor->value(9).asString();
		f->m_desc = cursor->value(10).asString();
		t->addField(f);
		cursor->moveNext();
	}

	if (!ok) {//error:
		deleteCursor(cursor);
		delete t;
		return 0;
	}

	if (!deleteCursor(cursor)) {
		delete t;
		return 0;
	}
	//store locally:
	m_tables.insert(t->m_id, t);
	m_tables_byname.insert(t->m_name.lower(), t);
	return t;
}

TableSchema* Connection::tableSchema( const QString& tableName )
{
	QString m_tableName = tableName.lower();
	TableSchema *t = m_tables_byname[m_tableName];
	if (t)
		return t;
	//not found: retrieve schema
	RowData data;
	if (!querySingleRecord(QString("select o_id, o_type, o_name, o_caption, o_desc from kexi__objects where o_name='%1' and o_type=%2")
			.arg(m_tableName).arg(KexiDB::TableObjectType), data))
		return 0;
	
	return setupTableSchema(data);
}

TableSchema* Connection::tableSchema( const int tableId )
{
	TableSchema *t = m_tables[tableId];
	if (t)
		return t;
	//not found: retrieve schema
	RowData data;
	if (!querySingleRecord(QString("select o_id, o_type, o_name, o_caption, o_desc from kexi__objects where o_id=%1").arg(tableId), data))
		return 0;
	
	return setupTableSchema(data);
}

QuerySchema* Connection::querySchema( const int queryId )
{
	QuerySchema *q = m_queries[queryId];
	if (q)
		return q;
	//not found: retrieve schema
//	RowData queryobject_data, querydata_data;
//	if (!querySingleRecord(QString("select * from kexi__objects where o_id=%1").arg(queryId), queryobject_data))
//		return 0;

	q = new QuerySchema();
	if (!setupObjectSchemaData( queryId, *q )) {
		delete q;
		return 0;
	}
	
	//TODO: retrieve rest of query schema............
//	if (!querySingleRecord(QString("select * from kexi_querydata where q_id='%1'").arg(queryId), data))
	

//	KexiDB::Cursor *cursor;
//	if (!(cursor = executeQuery( QString("select * from kexi__objects where o_id='%1'").arg(queryId) )))
//		return 0;
//	if (!cursor->moveFirst()) {
//		deleteCursor(cursor);
//		return 0;
//	}
//	bool ok;
//	int q_id = cursor->value(0).toInt(&ok);
//	if (!ok || q_id!=queryId) {
//		deleteCursor(cursor);
//		return 0;
//	}
//	q = new QuerySchema( q_id, this );
//	q->m_id = q_id;
//	KexiDBDbg<<"@@@ t_id=="<<t->m_id<<" t_name="<<cursor->value(1).asCString()<<endl;
	m_queries.insert(q->m_id, q);
	m_queries_byname.insert(q->m_name, q);
	return q;	
}

TableSchema* Connection::newKexiDBSystemTableSchema(const QString& tsname)
{
	TableSchema *ts = new TableSchema(tsname.lower());
	ts->setKexiDBSystem(true);
	m_kexiDBSystemtables.append(ts);
	m_tables_byname.insert(ts->name(),ts);
	return ts;
}
	
bool Connection::setupKexiDBSystemSchema()
{
	if (!m_kexiDBSystemtables.isEmpty())
		return true; //already set up
	
	TableSchema *t_objects = newKexiDBSystemTableSchema("kexi__objects");
	t_objects->addField( new Field("o_id", Field::Integer, Field::PrimaryKey | Field::AutoInc, Field::Unsigned) )
	.addField( new Field("o_type", Field::Byte, 0, Field::Unsigned) )
	.addField( new Field("o_name", Field::Text) )
	.addField( new Field("o_caption", Field::Text ) )
	.addField( new Field("o_desc", Field::LongText ) );

	TableSchema *t_objectdata = newKexiDBSystemTableSchema("kexi__objectdata");
	t_objectdata->addField( new Field("o_id", Field::Integer, Field::NotNull, Field::Unsigned) )
	.addField( new Field("o_data", Field::BLOB) )
	.addField( new Field("o_sub_id", Field::Text) );

	TableSchema *t_fields = newKexiDBSystemTableSchema("kexi__fields");
	t_fields->addField( new Field("t_id", Field::Integer, 0, Field::Unsigned) )
	.addField( new Field("f_type", Field::Byte, 0, Field::Unsigned) )
	.addField( new Field("f_name", Field::Text ) )
	.addField( new Field("f_length", Field::Integer ) )
	.addField( new Field("f_precision", Field::Integer ) )
	.addField( new Field("f_constraints", Field::Integer ) )
	.addField( new Field("f_options", Field::Integer ) )
	.addField( new Field("f_default", Field::Text ) )
	//these are additional properties:
	.addField( new Field("f_order", Field::Integer ) )
	.addField( new Field("f_caption", Field::Text ) )
	.addField( new Field("f_help", Field::LongText ) );

	TableSchema *t_querydata = newKexiDBSystemTableSchema("kexi__querydata");
	t_querydata->addField( new Field("q_id", Field::Integer, 0, Field::Unsigned) )
	.addField( new Field("q_sql", Field::LongText ) )
	.addField( new Field("q_valid", Field::Boolean ) );

	TableSchema *t_queryfields = newKexiDBSystemTableSchema("kexi__queryfields");
	t_queryfields->addField( new Field("q_id", Field::Integer, 0, Field::Unsigned) )
	.addField( new Field("f_order", Field::Integer ) )
	.addField( new Field("f_id", Field::Integer ) )
	.addField( new Field("f_tab_asterisk", Field::Integer, 0, Field::Unsigned) )
	.addField( new Field("f_alltab_asterisk", Field::Boolean) );

	TableSchema *t_querytables = newKexiDBSystemTableSchema("kexi__querytables");
	t_querytables->addField( new Field("q_id", Field::Integer, 0, Field::Unsigned) )
	.addField( new Field("t_id", Field::Integer, 0, Field::Unsigned) )
	.addField( new Field("t_order", Field::Integer, 0, Field::Unsigned) );

	TableSchema *t_db = newKexiDBSystemTableSchema("kexi__db");
	t_db->addField( new Field("db_property", Field::Text, Field::NoConstraints, Field::NoOptions, 32 ) )
	.addField( new Field("db_value", Field::LongText ) );

	TableSchema *t_parts = newKexiDBSystemTableSchema("kexi__parts");
	t_parts->addField( new Field("p_id", Field::Integer, Field::PrimaryKey | Field::AutoInc, Field::Unsigned) )
	.addField( new Field("p_name", Field::Text) )
	.addField( new Field("p_mime", Field::Text ) )
	.addField( new Field("p_url", Field::Text ) );

	return true;
}

void Connection::removeMe(TableSchema *ts)
{
	if (ts && !m_destructor_started) {
		m_tables.take(ts->id());
		m_tables.take(ts->id());
		m_tables_byname.take(ts->name());
	}
}

QString Connection::anyAvailableDatabaseName()
{
	if (!m_availableDatabaseName.isEmpty()) {
		return m_availableDatabaseName;
	}
	return m_driver->beh->ALWAYS_AVAILABLE_DATABASE_NAME;
}

void Connection::setAvailableDatabaseName(const QString& dbName)
{
	m_availableDatabaseName = dbName;
}

bool Connection::updateRow(QuerySchema &query, RowData& data, RowEditBuffer& buf)
{
	KexiDBDrvDbg << "Connection::updateRow.." << endl;
	clearError();
	//--get PKEY
	if (buf.dbBuffer().isEmpty()) {
		KexiDBDrvDbg << " -- NO CHANGES DATA!" << endl;
		return true;
	}
	if (!query.parentTable()) {
		KexiDBDrvDbg << " -- NO PARENT TABLE!" << endl;
		return false;
	}
	IndexSchema *pkey = query.parentTable()->primaryKey();
	if (!pkey || pkey->fields()->isEmpty()) {
		KexiDBDrvDbg << " -- NO PARENT TABLE's PKEY!" << endl;
//js TODO: hmm, perhaps we can try to update without using PKEY?
		return false;
	}
	//update the record:
	m_sql = "UPDATE " + query.parentTable()->name() + " SET ";
	QString sqlset, sqlwhere;
	KexiDB::RowEditBuffer::DBMap b = buf.dbBuffer();
	for (KexiDB::RowEditBuffer::DBMap::ConstIterator it=b.begin();it!=b.end();++it) {
		if (!sqlset.isEmpty())
			sqlset+=",";
		sqlset += (it.key()->name() + "=" + m_driver->valueToSQL(it.key(),it.data()));
	}
	QValueVector<uint> pkeyFieldsOrder = query.pkeyFieldsOrder();
	if (pkey->fieldCount()>0) {
		uint i=0;
		for (Field::ListIterator it = pkey->fieldsIterator(); it.current(); i++, ++it) {
			if (!sqlwhere.isEmpty())
				sqlwhere+=" AND ";
			QVariant val = data[ pkeyFieldsOrder[i] ];
			if (val.isNull() || !val.isValid()) {
				setError(ERR_UPDATE_NULL_PKEY_FIELD, i18n("Primary key's field \"%1\" cannot be empty.").arg(it.current()->name()));
//js todo: pass the field's name somewhere!
				return false;
			}
			sqlwhere += ( it.current()->name() + "=" 
				+ m_driver->valueToSQL( it.current(), val ) );
		}
	}
	m_sql += (sqlset + " WHERE " + sqlwhere);
	KexiDBDrvDbg << " -- SQL == " << m_sql << endl;

	bool res = drv_executeSQL(m_sql);

	if (!res) {
		setError(ERR_UPDATE_SERVER_ERROR, i18n("Row updating on the server failed."));
		return false;
	}
	//success: now also assign new value in memory:
	QMap<Field*,uint> fieldsOrder = query.fieldsOrder();
	for (KexiDB::RowEditBuffer::DBMap::ConstIterator it=b.begin();it!=b.end();++it) {
		data[ fieldsOrder[it.key()] ] = it.data();
	}

	return res;
}

bool Connection::insertRow(QuerySchema &query, RowData& data, RowEditBuffer& buf)
{
	KexiDBDrvDbg << "Connection::updateRow.." << endl;
	clearError();
	//--get PKEY
	if (buf.dbBuffer().isEmpty()) {
		KexiDBDrvDbg << " -- NO CHANGES DATA!" << endl;
		return true;
	}
	if (!query.parentTable()) {
		KexiDBDrvDbg << " -- NO PARENT TABLE!" << endl;
		return false;
	}
	IndexSchema *pkey = query.parentTable()->primaryKey();
	if (!pkey || pkey->fields()->isEmpty())
		KexiDBDrvDbg << " -- WARNING: NO PARENT TABLE's PKEY" << endl;

	//insert the record:
	m_sql = "INSERT INTO " + query.parentTable()->name() + " (";
	QString sqlcols, sqlvals;
	KexiDB::RowEditBuffer::DBMap b = buf.dbBuffer();
	for (KexiDB::RowEditBuffer::DBMap::ConstIterator it=b.begin();it!=b.end();++it) {
		if (!sqlcols.isEmpty()) {
			sqlcols+=",";
			sqlvals+=",";
		}
		sqlcols += it.key()->name();
		sqlvals += m_driver->valueToSQL(it.key(),it.data());
	}
	m_sql += (sqlcols + ") VALUES (" + sqlvals + ")");
	KexiDBDrvDbg << " -- SQL == " << m_sql << endl;

	bool res = drv_executeSQL(m_sql);

	if (!res) {
		setError(ERR_INSERT_SERVER_ERROR, i18n("Row inserting on the server failed."));
	return false;
	}
	//success: now also assign new value in memory:
	QMap<Field*,uint> fieldsOrder = query.fieldsOrder();
	for (KexiDB::RowEditBuffer::DBMap::ConstIterator it=b.begin();it!=b.end();++it) {
		data[ fieldsOrder[it.key()] ] = it.data();
	}

	//fetch autoincremented values
	Field::List *aif_list = query.parentTable()->autoIncrementFields();
	if (pkey && !aif_list->isEmpty()) {
		//now only if PKEY is present:
		//js TODO more...
		Field *id_field = aif_list->first();
		int last_id = lastInsertedAutoIncValue(id_field->name(), query.parentTable()->name());
		if (last_id==-1) {
			//err...
			return false;
		}
		KexiDB::RowData aif_data;
		if (!querySingleRecord(QString("SELECT ")+ FieldList::sqlFieldsList( aif_list ) + " FROM " 
			+ query.parentTable()->name() + " WHERE "+ id_field->name() + "=" + QString::number(last_id),
			aif_data))
		{
			//err...
			return false;
		}
		Field::ListIterator f_it(*aif_list);
		Field *f;
		for (uint i=0; (f = f_it.current()); ++f_it, i++) {
			kdDebug() << "Connection::insertRow(): AUTOINCREMENTED FIELD " << f->name() << " == " 
				<< aif_data[i].toInt() << endl;
			data[ fieldsOrder[ f ] ] = aif_data[i];
		}
	}
	return true;
}

bool Connection::deleteRow(QuerySchema &query, RowData& data)
{
	KexiDBDrvDbg << "Connection::deleteRow.." << endl;
	clearError();
	if (!query.parentTable()) {
		KexiDBDrvDbg << " -- NO PARENT TABLE!" << endl;
		return false;
	}
	IndexSchema *pkey = query.parentTable()->primaryKey();
	if (!pkey || pkey->fields()->isEmpty())
		KexiDBDrvDbg << " -- WARNING: NO PARENT TABLE's PKEY" << endl;
	
	//update the record:
	m_sql = "DELETE FROM " + query.parentTable()->name() + " WHERE ";
	QString sqlwhere;
	QValueVector<uint> pkeyFieldsOrder = query.pkeyFieldsOrder();
	if (pkey->fieldCount()>0) {
		uint i=0;
		for (Field::ListIterator it = pkey->fieldsIterator(); it.current(); i++, ++it) {
			if (!sqlwhere.isEmpty())
				sqlwhere+=" AND ";
			QVariant val = data[ pkeyFieldsOrder[i] ];
			if (val.isNull() || !val.isValid()) {
				setError(ERR_DELETE_NULL_PKEY_FIELD, i18n("Primary key's field \"%1\" cannot be empty.").arg(it.current()->name()));
//js todo: pass the field's name somewhere!
				return false;
			}
			sqlwhere += ( it.current()->name() + "=" 
				+ m_driver->valueToSQL( it.current(), val ) );
		}
	}
	m_sql += sqlwhere;
	KexiDBDrvDbg << " -- SQL == " << m_sql << endl;

	bool res = drv_executeSQL(m_sql);

	if (!res) {
		setError(ERR_DELETE_SERVER_ERROR, i18n("Row deleting on the server failed."));
		return false;
	}

	return res;
}

#include "connection.moc"
