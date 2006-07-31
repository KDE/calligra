/* This file is part of the KDE project
   Copyright (C) 2003-2006 Jaroslaw Staniek <js@iidea.pl>

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

#include <kexidb/connection.h>

#include "error.h"
#include "connection_p.h"
#include "connectiondata.h"
#include "driver.h"
#include "driver_p.h"
#include "schemadata.h"
#include "tableschema.h"
#include "relationship.h"
#include "transaction.h"
#include "cursor.h"
#include "global.h"
#include "roweditbuffer.h"
#include "utils.h"
#include "dbproperties.h"
#include "lookupfieldschema.h"
#include "parser/parser.h"

#include <kexiutils/utils.h>
#include <kexiutils/identifier.h>

#include <qdir.h>
#include <qfileinfo.h>
#include <qguardedptr.h>
#include <qdom.h>

#include <klocale.h>
#include <kdebug.h>

#define KEXIDB_EXTENDED_TABLE_SCHEMA_VERSION 1

namespace KexiDB {

ConnectionInternal::ConnectionInternal(Connection *conn)
 : connection(conn)
{
}

ConnectionInternal::~ConnectionInternal()
{
}

//================================================
//! @internal
class ConnectionPrivate
{
	public:
		ConnectionPrivate(Connection *conn)
		 : conn(conn)
		 , tableSchemaChangeListeners(101)
		 , versionMajor(-1)
		 , versionMinor(-1)
		 , m_parser(0)
		 , dont_remove_transactions(false)
		 , skip_databaseExists_check_in_useDatabase(false)
		 , default_trans_started_inside(false)
		 , isConnected(false)
		 , autoCommit(true)
		{
			tableSchemaChangeListeners.setAutoDelete(true);
			obsoleteQueries.setAutoDelete(true);
		}
		~ConnectionPrivate()
		{
			delete m_parser;
		}

		void errorInvalidDBContents(const QString& details) {
			conn->setError( ERR_INVALID_DATABASE_CONTENTS, i18n("Invalid database contents. ")+details);
		}

		QString strItIsASystemObject() const {
			return i18n("It is a system object.");
		}

		inline Parser *parser() { return m_parser ? m_parser : (m_parser = new Parser(conn)); }

		Connection *conn;

		/*! Default transaction handle.
		If transactions are supported: Any operation on database (e.g. inserts)
		that is started without specifing transaction context, will be performed
		in the context of this transaction. */
		Transaction default_trans;
		QValueList<Transaction> transactions;

		QPtrDict< QPtrList<Connection::TableSchemaChangeListenerInterface> > tableSchemaChangeListeners;

		//! Used in Connection::setQuerySchemaObsolete( const QString& queryName )
		//! to collect obsolete queries. THese are deleted on connection deleting.
		QPtrList<QuerySchema> obsoleteQueries;

		//! Version information for this connection.
		int versionMajor;
		int versionMinor;

		Parser *m_parser;

		//! Database properties
		DatabaseProperties* dbProperties;

		QString availableDatabaseName; //!< used by anyAvailableDatabaseName()

		//! true if rollbackTransaction() and commitTransaction() shouldn't remove
		//! the transaction object from m_transactions list; used by closeDatabase()
		bool dont_remove_transactions : 1;

		//! used to avoid endless recursion between useDatabase() and databaseExists()
		//! when useTemporaryDatabaseIfNeeded() works
		bool skip_databaseExists_check_in_useDatabase : 1;

		/*! Used when single transactions are only supported (Driver::SingleTransactions).
		 True value means default transaction has been started inside connection object
		 (by beginAutoCommitTransaction()), otherwise default transaction has been started outside
		 of the object (e.g. before createTable()), so we shouldn't autocommit the transaction
		 in commitAutoCommitTransaction(). Also, beginAutoCommitTransaction() doesn't restarts
		 transaction if m_default_trans_started_inside is false. Such behaviour allows user to
		 execute a sequence of actions like CREATE TABLE...; INSERT DATA...; within a single transaction
		 and commit it or rollback by hand. */
		bool default_trans_started_inside : 1;

		bool isConnected : 1;

		bool autoCommit : 1;

		/*! True for read only connection. Used especially for file-based drivers. */
		bool readOnly : 1;
};

}//namespace KexiDB

//================================================
using namespace KexiDB;

//! static: list of internal KexiDB system table names
QStringList KexiDB_kexiDBSystemTableNames;

Connection::Connection( Driver *driver, ConnectionData &conn_data )
	: QObject()
	,KexiDB::Object()
	,m_data(&conn_data)
	,m_tables_byname(101, false)
	,m_queries_byname(101, false)
	,m_kexiDBSystemTables(101)
	,d(new ConnectionPrivate(this))
	,m_driver(driver)
	,m_destructor_started(false)
{
	d->dbProperties = new DatabaseProperties(this);
	m_tables.setAutoDelete(true);
	m_tables_byname.setAutoDelete(false);//m_tables is owner, not me
	m_kexiDBSystemTables.setAutoDelete(true);//only system tables
	m_queries.setAutoDelete(true);
	m_queries_byname.setAutoDelete(false);//m_queries is owner, not me
	m_cursors.setAutoDelete(true);
//	d->transactions.setAutoDelete(true);
	//reasonable sizes: TODO
	m_tables.resize(101);
	m_queries.resize(101);
	m_cursors.resize(101);
//	d->transactions.resize(101);//woohoo! so many transactions?
	m_sql.reserve(0x4000);
}

void Connection::destroy()
{
	disconnect();
	//do not allow the driver to touch me: I will kill myself.
	m_driver->d->connections.take( this );
}

Connection::~Connection()
{
	m_destructor_started = true;
//	KexiDBDbg << "Connection::~Connection()" << endl;
	delete d->dbProperties;
	delete d;
	d = 0;
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
	if (d->isConnected) {
		setError(ERR_ALREADY_CONNECTED, i18n("Connection already established.") );
		return false;
	}

	if (!(d->isConnected = drv_connect())) {
		setError(m_driver->isFileDriver() ?
			i18n("Could not open \"%1\" project file.").arg(QDir::convertSeparators(m_data->fileName()))
			: i18n("Could not connect to \"%1\" database server.").arg(m_data->serverInfoString()) );
	}
	return d->isConnected;
}

bool Connection::isDatabaseUsed() const
{
	return !m_usedDatabase.isEmpty() && d->isConnected && drv_isDatabaseUsed();
}

void Connection::clearError()
{
	Object::clearError();
	m_sql = QString::null;
}

bool Connection::disconnect()
{
	clearError();
	if (!d->isConnected)
		return true;

	if (!closeDatabase())
		return false;

	bool ok = drv_disconnect();
	if (ok)
		d->isConnected = false;
	return ok;
}

bool Connection::isConnected() const
{
	return d->isConnected;
}

bool Connection::checkConnected()
{
	if (d->isConnected) {
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
	for (QStringList::ConstIterator it = list.constBegin(); it!=list.constEnd(); ++it) {
		KexiDBDbg << "Connection::databaseNames(): " << *it << endl;
		if (!m_driver->isSystemDatabaseName(*it)) {
			KexiDBDbg << "add " << *it << endl;
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
			setError(ERR_OBJECT_NOT_FOUND, i18n("The database \"%1\" does not exist.").arg(dbName));
		return false;
	}

	return true;
}

bool Connection::databaseExists( const QString &dbName, bool ignoreErrors )
{
//	KexiDBDbg << "Connection::databaseExists(" << dbName << "," << ignoreErrors << ")" << endl;
	if (!checkConnected())
		return false;
	clearError();

	if (m_driver->isFileDriver()) {
		//for file-based db: file must exists and be accessible
//js: moved from useDatabase():
		QFileInfo file(m_data->fileName());
		if (!file.exists() || ( !file.isFile() && !file.isSymLink()) ) {
			if (!ignoreErrors)
				setError(ERR_OBJECT_NOT_FOUND, i18n("Database file \"%1\" does not exist.")
				.arg(QDir::convertSeparators(m_data->fileName())) );
			return false;
		}
		if (!file.isReadable()) {
			if (!ignoreErrors)
				setError(ERR_ACCESS_RIGHTS, i18n("Database file \"%1\" is not readable.")
				.arg(QDir::convertSeparators(m_data->fileName())) );
			return false;
		}
		if (!file.isWritable()) {
			if (!ignoreErrors)
				setError(ERR_ACCESS_RIGHTS, i18n("Database file \"%1\" is not writable.")
				.arg(QDir::convertSeparators(m_data->fileName())) );
			return false;
		}
		return true;
	}

	QString tmpdbName;
	//some engines need to have opened any database before executing "create database"
	const bool orig_skip_databaseExists_check_in_useDatabase = d->skip_databaseExists_check_in_useDatabase;
	d->skip_databaseExists_check_in_useDatabase = true;
	bool ret = useTemporaryDatabaseIfNeeded(tmpdbName);
	d->skip_databaseExists_check_in_useDatabase = orig_skip_databaseExists_check_in_useDatabase;
	if (!ret)
		return false;

	ret = drv_databaseExists(dbName, ignoreErrors);

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


bool Connection::createDatabase( const QString &dbName )
{
	if (!checkConnected())
		return false;

	if (databaseExists( dbName )) {
		setError(ERR_OBJECT_EXISTS, i18n("Database \"%1\" already exists.").arg(dbName) );
		return false;
	}
	if (m_driver->isSystemDatabaseName( dbName )) {
		setError(ERR_SYSTEM_NAME_RESERVED, 
			i18n("Cannot create database \"%1\". This name is reserved for system database.").arg(dbName) );
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

	if (!tmpdbName.isEmpty() || !m_driver->d->isDBOpenedAfterCreate) {
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
	for (QPtrDictIterator<TableSchema> it(m_kexiDBSystemTables); it.current(); ++it) {
		if (!drv_createTable( it.current()->name() ))
 			createDatabase_ERROR;
	}

/* moved to KexiProject...

	//-create default part info
	TableSchema *ts;
	if (!(ts = tableSchema("kexi__parts")))
		createDatabase_ERROR;
	FieldList *fl = ts->subList("p_id", "p_name", "p_mime", "p_url");
	if (!fl)
		createDatabase_ERROR;
	if (!insertRecord(*fl, QVariant(1), QVariant("Tables"), QVariant("kexi/table"), QVariant("http://koffice.org/kexi/")))
		createDatabase_ERROR;
	if (!insertRecord(*fl, QVariant(2), QVariant("Queries"), QVariant("kexi/query"), QVariant("http://koffice.org/kexi/")))
		createDatabase_ERROR;
*/

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

bool Connection::useDatabase( const QString &dbName, bool kexiCompatible, bool *cancelled, MessageHandler* msgHandler )
{
	if (cancelled)
		*cancelled = false;
	KexiDBDbg << "Connection::useDatabase(" << dbName << "," << kexiCompatible <<")" << endl;
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

	if (!d->skip_databaseExists_check_in_useDatabase) {
		if (!databaseExists(my_dbName, false /*don't ignore errors*/))
			return false; //database must exist
	}

	if (!m_usedDatabase.isEmpty() && !closeDatabase()) //close db if already used
		return false;

	m_usedDatabase = "";

	if (!drv_useDatabase( my_dbName, cancelled, msgHandler )) {
		if (cancelled && *cancelled)
			return false;
		QString msg(i18n("Opening database \"%1\" failed.").arg( my_dbName ));
		if (error())
			setError( this, msg );
		else
			setError( msg );
		return false;
	}

	//-create system tables schema objects
	if (!setupKexiDBSystemSchema())
		return false;

	if (kexiCompatible && my_dbName.lower()!=anyAvailableDatabaseName().lower()) {
		//-get global database information
		int num;
		bool ok;
//		static QString notfound_str = i18n("\"%1\" database property not found");
		num = d->dbProperties->value("kexidb_major_ver").toInt(&ok);
		if (!ok)
			return false;
		d->versionMajor = num;
/*		if (true!=querySingleNumber(
			"select db_value from kexi__db where db_property=" + m_driver->escapeString(QString("kexidb_major_ver")), num)) {
			d->errorInvalidDBContents(notfound_str.arg("kexidb_major_ver"));
			return false;
		}*/
		num = d->dbProperties->value("kexidb_minor_ver").toInt(&ok);
		if (!ok)
			return false;
		d->versionMinor = num;
/*		if (true!=querySingleNumber(
			"select db_value from kexi__db where db_property=" + m_driver->escapeString(QString("kexidb_minor_ver")), num)) {
			d->errorInvalidDBContents(notfound_str.arg("kexidb_minor_ver"));
			return false;
		}*/

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
		QValueList<Transaction>::ConstIterator it;
		d->dont_remove_transactions=true; //lock!
		for (it=d->transactions.constBegin(); it!= d->transactions.constEnd(); ++it) {
			if (!rollbackTransaction(*it)) {//rollback as much as you can, don't stop on prev. errors
				ret = false;
			}
			else {
				KexiDBDbg << "Connection::closeDatabase(): transaction rolled back!" << endl;
				KexiDBDbg << "Connection::closeDatabase(): trans.refcount==" <<
				 ((*it).m_data ? QString::number((*it).m_data->refcount) : "(null)") << endl;
			}
		}
		d->dont_remove_transactions=false; //unlock!
		d->transactions.clear(); //free trans. data
	}

	//delete own cursors:
	m_cursors.clear();
	//delete own schemas
	m_tables.clear();
	m_kexiDBSystemTables.clear();
	m_queries.clear();

	if (!drv_closeDatabase())
		return false;

	m_usedDatabase = "";
//	KexiDBDbg << "Connection::closeDatabase(): " << ret << endl;
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
		const bool orig_skip_databaseExists_check_in_useDatabase = d->skip_databaseExists_check_in_useDatabase;
		d->skip_databaseExists_check_in_useDatabase = true;
		bool ret = useDatabase(tmpdbName, false);
		d->skip_databaseExists_check_in_useDatabase = orig_skip_databaseExists_check_in_useDatabase;
		if (!ret) {
			setError(errorNum(), 
				i18n("Error during starting temporary connection using \"%1\" database name.")
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

QStringList Connection::objectNames(int objType, bool* ok)
{
	QStringList list;

	if (!checkIsDatabaseUsed()) {
		if(ok) 
			*ok = false;
		return list;
	}

	QString sql;
	if (objType==KexiDB::AnyObjectType)
		sql = "SELECT o_name FROM kexi__objects";
	else
		sql = QString::fromLatin1("SELECT o_name FROM kexi__objects WHERE o_type=%1").arg(objType);

	Cursor *c = executeQuery(sql);
	if (!c) {
		if(ok) 
			*ok = false;
		return list;
	}

	for (c->moveFirst(); !c->eof(); c->moveNext()) {
		QString name = c->value(0).toString();
		if (KexiUtils::isIdentifier( name )) {
			list.append(name);
		}
	}

	if (!deleteCursor(c)) {
		if(ok)
			*ok = false;
		return list;
	}

	if(ok)
		*ok = true;
	return list;
}

QStringList Connection::tableNames(bool also_system_tables)
{
	bool ok = true;
	QStringList list = objectNames(TableObjectType, &ok);
	if (also_system_tables && ok) {
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
//		<< "kexi__querydata"
//		<< "kexi__queryfields"
//		<< "kexi__querytables"
		<< "kexi__db"
		;
	}
	return KexiDB_kexiDBSystemTableNames;
}

int Connection::versionMajor() const
{
	return d->versionMinor;
}

int Connection::versionMinor() const
{
	return d->versionMinor;
}

DatabaseProperties& Connection::databaseProperties()
{
	return *d->dbProperties;
}

QValueList<int> Connection::queryIds()
{
	return objectIds(KexiDB::QueryObjectType);
}

QValueList<int> Connection::objectIds(int objType)
{
	QValueList<int> list;

	if (!checkIsDatabaseUsed())
		return list;

	Cursor *c = executeQuery(
		QString::fromLatin1("SELECT o_id, o_name FROM kexi__objects WHERE o_type=%1").arg(objType));
	if (!c)
		return list;
	for (c->moveFirst(); !c->eof(); c->moveNext())
	{
		QString tname = c->value(1).toString(); //kexi__objects.o_name
		if (KexiUtils::isIdentifier( tname )) {
			list.append(c->value(0).toInt()); //kexi__objects.o_id
		}
	}

	deleteCursor(c);

	return list;
}

QString Connection::createTableStatement( const KexiDB::TableSchema& tableSchema ) const
{
// Each SQL identifier needs to be escaped in the generated query.
	QString sql;
	sql.reserve(4096);
	sql = "CREATE TABLE " + escapeIdentifier(tableSchema.name()) + " (";
	bool first=true;
	Field::ListIterator it( tableSchema.m_fields );
	Field *field;
	for (;(field = it.current())!=0; ++it) {
		if (first)
			first = false;
		else
			sql += ", ";
		QString v = escapeIdentifier(field->name()) + " ";
		const bool autoinc = field->isAutoIncrement();
		const bool pk = field->isPrimaryKey() || (autoinc && m_driver->beh->AUTO_INCREMENT_REQUIRES_PK);
//TODO: warning: ^^^^^ this allows only ont autonumber per table when AUTO_INCREMENT_REQUIRES_PK==true!
		if (autoinc && m_driver->beh->SPECIAL_AUTO_INCREMENT_DEF) {
			if (pk)
				v += m_driver->beh->AUTO_INCREMENT_TYPE + " " + m_driver->beh->AUTO_INCREMENT_PK_FIELD_OPTION;
			else
				v += m_driver->beh->AUTO_INCREMENT_TYPE + " " + m_driver->beh->AUTO_INCREMENT_FIELD_OPTION;
		}
		else {
			if (autoinc && !m_driver->beh->AUTO_INCREMENT_TYPE.isEmpty())
				v += m_driver->beh->AUTO_INCREMENT_TYPE;
			else
				v += m_driver->sqlTypeName(field->type(), field->precision());

			if (field->isUnsigned())
				v += (" " + m_driver->beh->UNSIGNED_TYPE_KEYWORD);

			if (field->isFPNumericType() && field->precision()>0) {
				if (field->scale()>0)
					v += QString::fromLatin1("(%1,%2)").arg(field->precision()).arg(field->scale());
				else
					v += QString::fromLatin1("(%1)").arg(field->precision());
			}
			else if (field->type()==Field::Text && field->length()>0)
				v += QString::fromLatin1("(%1)").arg(field->length());

			if (autoinc)
				v += (" " +
				(pk ? m_driver->beh->AUTO_INCREMENT_PK_FIELD_OPTION : m_driver->beh->AUTO_INCREMENT_FIELD_OPTION));
			else
	//TODO: here is automatically a single-field key created
				if (pk)
					v += " PRIMARY KEY";
			if (!pk && field->isUniqueKey())
				v += " UNIQUE";
///@todo IS this ok for all engines?: if (!autoinc && !field->isPrimaryKey() && field->isNotNull())
			if (!autoinc && !pk && field->isNotNull())
				v += " NOT NULL"; //only add not null option if no autocommit is set
			if (field->defaultValue().isValid())
				v += QString::fromLatin1(" DEFAULT ") + m_driver->valueToSQL( field, field->defaultValue() );
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

//		KexiDBDbg << "******** " << QString("INSERT INTO ") + 
//			escapeIdentifier(tableSchema.name()) + 
//			" VALUES (" + vals + ")" <<endl; 

#define C_INS_REC(args, vals) \
	bool Connection::insertRecord(KexiDB::TableSchema &tableSchema args) {\
		return executeSQL( \
		 QString("INSERT INTO ") + escapeIdentifier(tableSchema.name()) + " VALUES (" + vals + ")" \
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
		return executeSQL( \
			QString("INSERT INTO ") + \
		((fields.fields()->first() && fields.fields()->first()->table()) ? \
			escapeIdentifier(fields.fields()->first()->table()->name()) : \
			"??") \
		+ "(" + fields.sqlFieldsList(m_driver) + ") VALUES (" + value + ")" \
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
// Each SQL identifier needs to be escaped in the generated query.
	Field::List *fields = tableSchema.fields();
	Field *f = fields->first();
//	QString s_val;
//	s_val.reserve(4096);
	m_sql = QString::null;
	QValueList<QVariant>::ConstIterator it = values.constBegin();
//	int i=0;
	while (f && (it!=values.end())) {
		if (m_sql.isEmpty())
			m_sql = QString("INSERT INTO ") +
				escapeIdentifier(tableSchema.name()) +
				" VALUES (";
		else
			m_sql += ",";
		m_sql += m_driver->valueToSQL( f, *it );
//		KexiDBDbg << "val" << i++ << ": " << m_driver->valueToSQL( f, *it ) << endl;
		++it;
		f=fields->next();
	}
	m_sql += ")";

//	KexiDBDbg<<"******** "<< m_sql << endl;
	return executeSQL(m_sql);
}

bool Connection::insertRecord(FieldList& fields, QValueList<QVariant>& values)
{
// Each SQL identifier needs to be escaped in the generated query.
	Field::List *flist = fields.fields();
	Field *f = flist->first();
	if (!f)
		return false;
//	QString s_val;
//	s_val.reserve(4096);
	m_sql = QString::null;
	QValueList<QVariant>::ConstIterator it = values.constBegin();
//	int i=0;
	while (f && (it!=values.constEnd())) {
		if (m_sql.isEmpty())
			m_sql = QString("INSERT INTO ") +
				escapeIdentifier(flist->first()->table()->name()) + "(" +
				fields.sqlFieldsList(m_driver) + ") VALUES (";
		else
			m_sql += ",";
		m_sql += m_driver->valueToSQL( f, *it );
//		KexiDBDbg << "val" << i++ << ": " << m_driver->valueToSQL( f, *it ) << endl;
		++it;
		f=flist->next();
	}
	m_sql += ")";

	return executeSQL(m_sql);
}

bool Connection::executeSQL( const QString& statement )
{
	m_sql = statement; //remember for error handling
	if (!drv_executeSQL( m_sql )) {
		m_errMsg = QString::null; //clear as this could be most probably jsut "Unknown error" string.
		m_errorSql = statement;
		setError(this, ERR_SQL_EXECUTION_ERROR, i18n("Error while executing SQL statement."));
		return false;
	}
	return true;
}

QString Connection::selectStatement( KexiDB::QuerySchema& querySchema,
	bool alsoRetrieveROWID, int drvEscaping) const
{
//"SELECT FROM ..." is theoretically allowed "
//if (querySchema.fieldCount()<1)
//		return QString::null;
// Each SQL identifier needs to be escaped in the generated query.

	if (!querySchema.statement().isEmpty())
		return querySchema.statement();

//! @todo looking at singleTable is visually nice but a field name can conflict 
//!       with function or variable name...
	Field *f;
	uint number = 0;
	bool singleTable = querySchema.tables()->count() <= 1;
	if (singleTable) {
		//make sure we will have single table:
		for (Field::ListIterator it = querySchema.fieldsIterator(); (f = it.current()); ++it, number++) {
			if (querySchema.isColumnVisible(number) && f->table() && f->table()->lookupFieldSchema( *f )) {
				//uups, no, there's at least one left join
				singleTable = false;
				break;
			}
		}
	}

	QString sql; //final sql string
//unused	QString s_from_additional; //additional tables list needed for lookup fields
	QString s_additional_joins; //additional joins needed for lookup fields
	QString s_additional_fields; //additional fields to append to the fields list
	sql.reserve(4096);
	number = 0;
	for (Field::ListIterator it = querySchema.fieldsIterator(); (f = it.current()); ++it, number++) {
		if (querySchema.isColumnVisible(number)) {
			if (!sql.isEmpty())
				sql += QString::fromLatin1(", ");

			if (f->isQueryAsterisk()) {
				if (!singleTable && static_cast<QueryAsterisk*>(f)->isSingleTableAsterisk()) //single-table *
					sql += escapeIdentifier(f->table()->name(), drvEscaping) +
					       QString::fromLatin1(".*");
				else //all-tables * (or simplified table.* when there's only one table)
					sql += QString::fromLatin1("*");
			}
			else {
				if (f->isExpression()) {
					sql += f->expression()->toString();
				}
				else {
					if (!f->table()) //sanity check
						return QString::null;

					QString tableName;
					int tablePosition = querySchema.tableBoundToColumn(number);
					if (tablePosition>=0)
						tableName = querySchema.tableAlias(tablePosition);
					if (tableName.isEmpty())
						tableName = f->table()->name();

					if (!singleTable) {
						sql += (escapeIdentifier(tableName, drvEscaping) + ".");
					}
					sql += escapeIdentifier(f->name(), drvEscaping);
				}
				QString aliasString = QString(querySchema.columnAlias(number));
				if (!aliasString.isEmpty())
					sql += (QString::fromLatin1(" AS ") + aliasString);
//! @todo add option that allows to omit "AS" keyword
			}
			LookupFieldSchema *lookupFieldSchema = f->table() ? f->table()->lookupFieldSchema( *f ) : 0;
			if (lookupFieldSchema) {
				// Lookup field schema found
				// Now we also need to fetch "visible" value from the lookup table, not only the value of binding.
				// -> build LEFT OUTER JOIN clause for this purpose (LEFT, not INNER because the binding can be broken)
				// "LEFT OUTER JOIN lookupTable ON thisTable.thisField=lookupTable.boundField"
				if (lookupFieldSchema->rowSourceType()==LookupFieldSchema::Table) {
					TableSchema *lookupTable = querySchema.connection()->tableSchema( lookupFieldSchema->rowSource() );
					Field *visibleField = 0;
					Field *boundField = 0;
					if (lookupTable && lookupFieldSchema->boundColumn()>=0 
						&& (uint)lookupFieldSchema->boundColumn() < lookupTable->fieldCount()
						&& (visibleField = lookupTable->field( lookupFieldSchema->visibleColumn()))
						&& (boundField = lookupTable->field( lookupFieldSchema->boundColumn() )))
					{
						//add LEFT OUTER JOIN
						if (!s_additional_joins.isEmpty())
							s_additional_joins += QString::fromLatin1(" ");
						s_additional_joins += QString("LEFT OUTER JOIN %1 ON %2.%3=%4.%5")
							.arg(escapeIdentifier(lookupTable->name(), drvEscaping))
							.arg(escapeIdentifier(f->table()->name(), drvEscaping))
							.arg(escapeIdentifier(f->name(), drvEscaping))
							.arg(escapeIdentifier(lookupTable->name(), drvEscaping))
							.arg(escapeIdentifier(boundField->name(), drvEscaping));

						//add visibleField to the list of SELECTed fields if it is not yes present there
						if (!querySchema.findTableField( visibleField->table()->name()+"."+visibleField->name() )) {
							if (!querySchema.table( visibleField->table()->name() )) {
/* not true
								//table should be added after FROM
								if (!s_from_additional.isEmpty())
									s_from_additional += QString::fromLatin1(", ");
								s_from_additional += escapeIdentifier(visibleField->table()->name(), drvEscaping);
								*/
							}
							if (!s_additional_fields.isEmpty())
								s_additional_fields += QString::fromLatin1(", ");
							s_additional_fields += (escapeIdentifier(visibleField->table()->name(), drvEscaping) + "."
								+ escapeIdentifier(visibleField->name(), drvEscaping));
						}
					}
				}
			}
		}
	}

	//add lookup fields
	if (!s_additional_fields.isEmpty())
		sql += (QString::fromLatin1(", ") + s_additional_fields);

	if (alsoRetrieveROWID) { //append rowid column
		QString s;
		if (!sql.isEmpty())
			s = QString::fromLatin1(", ");
		if (querySchema.masterTable())
			s += (escapeIdentifier(querySchema.masterTable()->name())+".");
		s += m_driver->beh->ROW_ID_FIELD_NAME;
		sql += s;
	}

	sql.prepend("SELECT ");
	TableSchema::List* tables = querySchema.tables();
	if (tables && !tables->isEmpty()) {
		sql += QString::fromLatin1(" FROM ");
		QString s_from;
		TableSchema *table;
		number = 0;
		for (TableSchema::ListIterator it(*tables); (table = it.current());
			++it, number++)
		{
			if (!s_from.isEmpty())
				s_from += QString::fromLatin1(", ");
			s_from += escapeIdentifier(table->name(), drvEscaping);
			QString aliasString = QString(querySchema.tableAlias(number));
			if (!aliasString.isEmpty())
				s_from += (QString::fromLatin1(" AS ") + aliasString);
		}
/*unused	if (!s_from_additional.isEmpty()) {//additional tables list needed for lookup fields
			if (!s_from.isEmpty())
				s_from += QString::fromLatin1(", ");
			s_from += s_from_additional;
		}*/
		sql += s_from;
	}
	QString s_where;
	s_where.reserve(4096);

	//JOINS
	if (!s_additional_joins.isEmpty()) {
		sql += QString::fromLatin1(" ") + s_additional_joins + QString::fromLatin1(" ");
	}

//@todo: we're using WHERE for joins now; use INNER/LEFT/RIGHT JOIN later

	//WHERE
	Relationship *rel;
	bool wasWhere = false; //for later use
	for (Relationship::ListIterator it(*querySchema.relationships()); (rel = it.current()); ++it) {
		if (s_where.isEmpty()) {
			wasWhere = true;
		}
		else
			s_where += QString::fromLatin1(" AND ");
		Field::Pair *pair;
		QString s_where_sub;
		for (QPtrListIterator<Field::Pair> p_it(*rel->fieldPairs()); (pair = p_it.current()); ++p_it) {
			if (!s_where_sub.isEmpty())
				s_where_sub += QString::fromLatin1(" AND ");
			s_where_sub += (
				escapeIdentifier(pair->first->table()->name(), drvEscaping) +
				QString::fromLatin1(".") +
				escapeIdentifier(pair->first->name(), drvEscaping) +
				QString::fromLatin1(" = ")  +
				escapeIdentifier(pair->second->table()->name(), drvEscaping) +
				QString::fromLatin1(".") +
				escapeIdentifier(pair->second->name(), drvEscaping));
		}
		if (rel->fieldPairs()->count()>1) {
			s_where_sub.prepend("(");
			s_where_sub += QString::fromLatin1(")");
		}
		s_where += s_where_sub;
	}
	//EXPLICITY SPECIFIER WHERE EXPRESION
	if (querySchema.whereExpression()) {
		if (wasWhere) {
//TODO: () are not always needed
			s_where = "(" + s_where + ") AND (" + querySchema.whereExpression()->toString() + ")";
		}
		else {
			s_where = querySchema.whereExpression()->toString();
		}
	}
	if (!s_where.isEmpty())
		sql += QString::fromLatin1(" WHERE ") + s_where;
//! \todo (js) add WHERE and other sql parts
	//(use wasWhere here)

	return sql;
}

QString Connection::selectStatement( KexiDB::TableSchema& tableSchema ) const
{
	return selectStatement( *tableSchema.query() );
}

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

Q_ULLONG Connection::lastInsertedAutoIncValue(const QString& aiFieldName, const QString& tableName,
	Q_ULLONG* ROWID)
{
	Q_ULLONG row_id = drv_lastInsertRowID();
	if (ROWID)
		*ROWID = row_id;
	if (m_driver->beh->ROW_ID_FIELD_RETURNS_LAST_AUTOINCREMENTED_VALUE) {
		return row_id;
	}
	RowData rdata;
	if (row_id<=0 || true!=querySingleRecord(
		QString::fromLatin1("SELECT ") + tableName + QString::fromLatin1(".") + aiFieldName + QString::fromLatin1(" FROM ") + tableName
		+ QString::fromLatin1(" WHERE ") + m_driver->beh->ROW_ID_FIELD_NAME + QString::fromLatin1("=") + QString::number(row_id), rdata))
	{
//		KexiDBDbg << "Connection::lastInsertedAutoIncValue(): row_id<=0 || true!=querySingleRecord()" << endl;
	 	return (Q_ULLONG)-1; //ULL;
	}
	return rdata[0].toULongLong();
}

Q_ULLONG Connection::lastInsertedAutoIncValue(const QString& aiFieldName,
	const KexiDB::TableSchema& table, Q_ULLONG* ROWID)
{
	return lastInsertedAutoIncValue(aiFieldName,table.name(), ROWID);
}

//! Creates a Field list for kexi__fields, for sanity. Used by createTable()
static FieldList* createFieldListForKexi__Fields(TableSchema *kexi__fieldsSchema)
{
	if (!kexi__fieldsSchema)
		return 0;
	return kexi__fieldsSchema->subList(
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
}

//! builds a list of values for field's \a f properties. Used by createTable().
void buildValuesForKexi__Fields(QValueList<QVariant>& vals, Field* f)
{
	vals.clear();
	vals
	<< QVariant(f->table()->id())
	<< QVariant(f->type())
	<< QVariant(f->name())
	<< QVariant(f->isFPNumericType() ? f->scale() : f->length())
	<< QVariant(f->isFPNumericType() ? f->precision() : 0)
	<< QVariant(f->constraints())
	<< QVariant(f->options())
	<< QVariant(f->defaultValue())
	<< QVariant(f->order())
	<< QVariant(f->caption())
	<< QVariant(f->description());
}

bool Connection::storeMainFieldSchema(Field *field)
{
	if (!field || !field->table())
		return false;
	FieldList *fl = createFieldListForKexi__Fields(m_tables_byname["kexi__fields"]);
	if (!fl)
		return false;

	QValueList<QVariant> vals;
	buildValuesForKexi__Fields(vals, field);
	QValueList<QVariant>::ConstIterator valsIt = vals.constBegin();
	Field *f;
	bool first = true;
	QString sql = "UPDATE kexi__fields SET ";
	for (Field::ListIterator it( fl->fieldsIterator() ); (f = it.current()); ++it, ++valsIt) {
		sql.append( (first ? QString::null : QString(", ")) +
			f->name() + "=" + m_driver->valueToSQL( f, *valsIt ) );
		if (first)
			first = false;
	}
	delete fl;
	
	sql.append(QString(" WHERE t_id=") + QString::number( field->table()->id() )
		+ " AND f_name=" + m_driver->valueToSQL( Field::Text, field->name() ) );
	return executeSQL( sql );
}

#define createTable_ERR \
	{ KexiDBDbg << "Connection::createTable(): ERROR!" <<endl; \
	  setError(this, i18n("Creating table failed.")); \
	  rollbackAutoCommitTransaction(tg.transaction()); \
	  return false; }
		//setError( errorNum(), i18n("Creating table failed.") + " " + errorMsg()); 

//! Creates a table according to the given schema
/*! Creates a table according to the given TableSchema, adding the table and
    column definitions to kexi__* tables.  Checks that a database is in use,
    that the table name is not that of a system table, and that the schema
    defines at least one column.
    If the table exists, and replaceExisting is true, the table is replaced.
    Otherwise, the table is not replaced.
*/
bool Connection::createTable( KexiDB::TableSchema* tableSchema, bool replaceExisting )
{
	if (!tableSchema || !checkIsDatabaseUsed())
		return false;

	//check if there are any fields
	if (tableSchema->fieldCount()<1) {
		clearError();
		setError(ERR_CANNOT_CREATE_EMPTY_OBJECT, i18n("Cannot create table without fields."));
		return false;
	}
	const bool internalTable = dynamic_cast<InternalTableSchema*>(tableSchema);

	const QString &tableName = tableSchema->name().lower();

	if (!internalTable) {
		if (m_driver->isSystemObjectName( tableName )) {
			clearError();
			setError(ERR_SYSTEM_NAME_RESERVED, i18n("System name \"%1\" cannot be used as table name.")
				.arg(tableSchema->name()));
			return false;
		}

		Field *sys_field = findSystemFieldName(tableSchema);
		if (sys_field) {
			clearError();
			setError(ERR_SYSTEM_NAME_RESERVED,
				i18n("System name \"%1\" cannot be used as one of fields in \"%2\" table.")
				.arg(sys_field->name()).arg(tableName));
			return false;
		}
	}

	bool previousSchemaStillKept = false;

	KexiDB::TableSchema *existingTable = 0;
	if (replaceExisting) {
		//get previous table (do not retrieve, though)
		existingTable = m_tables_byname[tableName];
		if (existingTable) {
			if (existingTable == tableSchema) {
				clearError();
				setError(ERR_OBJECT_EXISTS, 
					i18n("Could not create the same table \"%1\" twice.").arg(tableSchema->name()) );
				return false;
			}
//TODO(js): update any structure (e.g. queries) that depend on this table!
			if (existingTable->id()>0)
				tableSchema->m_id = existingTable->id(); //copy id from existing table
			previousSchemaStillKept = true;
			if (!dropTable( existingTable, false /*alsoRemoveSchema*/ ))
				return false;
		}
	}
	else {
		if (this->tableSchema( tableSchema->name() ) != 0) {
			clearError();
			setError(ERR_OBJECT_EXISTS, i18n("Table \"%1\" already exists.").arg(tableSchema->name()) );
			return false;
		}
	}

/*	if (replaceExisting) {
	//get previous table (do not retrieve, though)
	KexiDB::TableSchema *existingTable = m_tables_byname.take(name);
	if (oldTable) {
	}*/

	TransactionGuard tg;
	if (!beginAutoCommitTransaction(tg))
		return false;

	if (!drv_createTable(*tableSchema))
		createTable_ERR;

	//add schema data to kexi__* tables
	if (!internalTable) {
		//update kexi__objects
		if (!storeObjectSchemaData( *tableSchema, true ))
			createTable_ERR;

		TableSchema *ts = m_tables_byname["kexi__fields"];
		if (!ts)
			return false;
		//for sanity: remove field info (if any) for this table id
		if (!KexiDB::deleteRow(*this, ts, "t_id", tableSchema->id()))
			return false;

		FieldList *fl = createFieldListForKexi__Fields(m_tables_byname["kexi__fields"]);
		if (!fl)
			return false;

//		int order = 0;
		Field *f;
		for (Field::ListIterator it( *tableSchema->fields() ); (f = it.current()); ++it/*, order++*/) {
			QValueList<QVariant> vals;
			buildValuesForKexi__Fields(vals, f);
			if (!insertRecord(*fl, vals ))
				createTable_ERR;
		}
		delete fl;

		if (!storeExtendedTableSchemaData(*tableSchema))
			createTable_ERR;
	}

	//finally:
/*	if (replaceExisting) {
		if (existingTable) {
			m_tables.take(existingTable->id());
			delete existingTable;
		}
	}*/

	bool res = commitAutoCommitTransaction(tg.transaction());

	if (res) {
		if (internalTable) {
			//insert the internal table into structures
			insertInternalTableSchema(tableSchema);
		}
		else {
			if (previousSchemaStillKept) {
				//remove previous table schema
				removeTableSchemaInternal(tableSchema);
			}
			//store one schema object locally:
			m_tables.insert(tableSchema->id(), tableSchema);
			m_tables_byname.insert(tableSchema->name().lower(), tableSchema);
		}
		//ok, this table is not created by the connection
		tableSchema->m_conn = this;
	}
	return res;
}

void Connection::removeTableSchemaInternal(TableSchema *tableSchema)
{
	m_tables_byname.remove(tableSchema->name());
	m_tables.remove(tableSchema->id());
}

bool Connection::removeObject( uint objId )
{
	clearError();
	//remove table schema from kexi__* tables
	if (!KexiDB::deleteRow(*this, m_tables_byname["kexi__objects"], "o_id", objId) //schema entry
		|| !KexiDB::deleteRow(*this, m_tables_byname["kexi__objectdata"], "o_id", objId)) {//data blocks
		setError(ERR_DELETE_SERVER_ERROR, i18n("Could not remove object's data."));
		return false;
	}
	return true;
}

bool Connection::drv_dropTable( const QString& name )
{
	m_sql = "DROP TABLE " + escapeIdentifier(name);
	return executeSQL(m_sql);
}

//! Drops a table corresponding to the name in the given schema
/*! Drops a table according to the name given by the TableSchema, removing the
    table and column definitions to kexi__* tables.  Checks first that the
    table is not a system table.

    TODO: Should check that a database is currently in use? (c.f. createTable)
*/
tristate Connection::dropTable( KexiDB::TableSchema* tableSchema )
{
	return dropTable( tableSchema, true );
}

tristate Connection::dropTable( KexiDB::TableSchema* tableSchema, bool alsoRemoveSchema)
{
// Each SQL identifier needs to be escaped in the generated query.
	clearError();
	if (!tableSchema)
		return false;

	QString errmsg(i18n("Table \"%1\" cannot be removed.\n"));
	//be sure that we handle the correct TableSchema object:
	if (tableSchema->id() < 0
		|| this->tableSchema(tableSchema->name())!=tableSchema
		|| this->tableSchema(tableSchema->id())!=tableSchema)
	{
		setError(ERR_OBJECT_NOT_FOUND, errmsg.arg(tableSchema->name())
			+i18n("Unexpected name or identifier."));
		return false;
	}

	tristate res = closeAllTableSchemaChangeListeners(*tableSchema);
	if (true!=res)
		return res;

	//sanity checks:
	if (m_driver->isSystemObjectName( tableSchema->name() )) {
		setError(ERR_SYSTEM_NAME_RESERVED, errmsg.arg(tableSchema->name()) + d->strItIsASystemObject());
		return false;
	}

	TransactionGuard tg;
	if (!beginAutoCommitTransaction(tg))
		return false;

	//for sanity we're checking if this table exists physically
	if (drv_containsTable(tableSchema->name())) {
		if (!drv_dropTable(tableSchema->name()))
			return false;
	}

	TableSchema *ts = m_tables_byname["kexi__fields"];
	if (!KexiDB::deleteRow(*this, ts, "t_id", tableSchema->id())) //field entries
		return false;

	//remove table schema from kexi__objects table
	if (!removeObject( tableSchema->id() )) {
		return false;
	}

	if (alsoRemoveSchema) {
//! \todo js: update any structure (e.g. queries) that depend on this table!
		tristate res = removeDataBlock( tableSchema->id(), "extended_schema");
		if (!res)
			return false;
		removeTableSchemaInternal(tableSchema);
	}
	return commitAutoCommitTransaction(tg.transaction());
}

tristate Connection::dropTable( const QString& table )
{
	clearError();
	TableSchema* ts = tableSchema( table );
	if (!ts) {
		setError(ERR_OBJECT_NOT_FOUND, i18n("Table \"%1\" does not exist.")
			.arg(table));
		return false;
	}
	return dropTable(ts);
}

tristate Connection::alterTable( TableSchema& tableSchema, TableSchema& newTableSchema )
{
	clearError();
	tristate res = closeAllTableSchemaChangeListeners(tableSchema);
	if (true!=res)
		return res;

	if (&tableSchema == &newTableSchema) {
		setError(ERR_OBJECT_THE_SAME, i18n("Could not alter table \"%1\" using the same table.")
			.arg(tableSchema.name()));
		return false;
	}
//TODO(js): implement real altering
//TODO(js): update any structure (e.g. query) that depend on this table!
	bool ok, empty;
#if 0//TODO ucomment:
	empty = isEmpty( tableSchema, ok ) && ok;
#else
	empty = true;
#endif
	if (empty) {
		ok = createTable(&newTableSchema, true/*replace*/);
	}
	return ok;
}

bool Connection::alterTableName(TableSchema& tableSchema, const QString& newName, bool replace)
{
	clearError();
	if (&tableSchema!=m_tables[tableSchema.id()]) {
		setError(ERR_OBJECT_NOT_FOUND, i18n("Unknown table \"%1\"").arg(tableSchema.name()));
		return false;
	}
	if (newName.isEmpty() || !KexiUtils::isIdentifier(newName)) {
		setError(ERR_INVALID_IDENTIFIER, i18n("Invalid table name \"%1\"").arg(newName));
		return false;
	}
	const QString oldTableName = tableSchema.name();
	const QString newTableName = newName.lower().stripWhiteSpace();
	if (oldTableName.lower().stripWhiteSpace() == newTableName) {
		setError(ERR_OBJECT_THE_SAME, i18n("Could rename table \"%1\" using the same name.")
			.arg(newTableName));
		return false;
	}
//TODO: alter table name for server DB backends!
//TODO: what about objects (queries/forms) that use old name?
//TODO
	const bool destTableExists = this->tableSchema( newName ) != 0;
	if (!replace && destTableExists) {
		setError(ERR_OBJECT_EXISTS,
			i18n("Could not rename table \"%1\" to \"%2\". Table \"%3\" already exists.")
			.arg(tableSchema.name()).arg(newName).arg(newName));
		return false;
	}

//helper:
#define alterTableName_ERR \
		tableSchema.setName(oldTableName) //restore old name

	TransactionGuard tg;
	if (!beginAutoCommitTransaction(tg))
		return false;

	if (!drv_alterTableName(tableSchema, newTableName, replace)) {
		alterTableName_ERR;
		return false;
	}

	// Update kexi__objects
	//TODO
	if (!executeSQL(QString::fromLatin1("UPDATE kexi__objects SET o_name=%1 WHERE o_id=%2")
		.arg(m_driver->escapeString(tableSchema.name())).arg(tableSchema.id())))
	{
		alterTableName_ERR;
		return false;
	}
//TODO what about caption?

	//restore old name: it will be changed soon!
	tableSchema.setName(oldTableName);

	if (!commitAutoCommitTransaction(tg.transaction())) {
		alterTableName_ERR;
		return false;
	}

	//update tableSchema:
	m_tables_byname.take(tableSchema.name());
	tableSchema.setName(newTableName);
	m_tables_byname.insert(tableSchema.name(), &tableSchema);
	return true;
}

bool Connection::drv_alterTableName(TableSchema& tableSchema, const QString& newName, bool replace)
{
	const QString oldTableName = tableSchema.name();
	const bool destTableExists = this->tableSchema( newName ) != 0;

	// drop the table
	if (destTableExists) {
		if (!replace)
			return false;
		if (!drv_dropTable( newName ))
			return false;
	}

	tableSchema.setName(newName);

//helper:
#define drv_alterTableName_ERR \
		tableSchema.setName(oldTableName) //restore old name

	if (!executeSQL(QString::fromLatin1("ALTER TABLE %1 RENAME TO %2")
		.arg(escapeIdentifier(oldTableName)).arg(escapeIdentifier(newName))))
	{
		drv_alterTableName_ERR;
		return false;
	}
	return true;
}

bool Connection::dropQuery( KexiDB::QuerySchema* querySchema )
{
	clearError();
	if (!querySchema)
		return false;

	TransactionGuard tg;
	if (!beginAutoCommitTransaction(tg))
		return false;

/*	TableSchema *ts = m_tables_byname["kexi__querydata"];
	if (!KexiDB::deleteRow(*this, ts, "q_id", querySchema->id()))
		return false;

	ts = m_tables_byname["kexi__queryfields"];
	if (!KexiDB::deleteRow(*this, ts, "q_id", querySchema->id()))
		return false;

	ts = m_tables_byname["kexi__querytables"];
	if (!KexiDB::deleteRow(*this, ts, "q_id", querySchema->id()))
		return false;*/

	//remove query schema from kexi__objects table
	if (!removeObject( querySchema->id() )) {
		return false;
	}

//TODO(js): update any structure that depend on this table!
	m_queries_byname.remove(querySchema->name());
	m_queries.remove(querySchema->id());

	return commitAutoCommitTransaction(tg.transaction());
}

bool Connection::dropQuery( const QString& query )
{
	clearError();
	QuerySchema* qs = querySchema( query );
	if (!qs) {
		setError(ERR_OBJECT_NOT_FOUND, i18n("Query \"%1\" does not exist.")
			.arg(query));
		return false;
	}
	return dropQuery(qs);
}

bool Connection::drv_createTable( const KexiDB::TableSchema& tableSchema )
{
	m_sql = createTableStatement(tableSchema);
	KexiDBDbg<<"******** "<<m_sql<<endl;
	return executeSQL(m_sql);
}

bool Connection::drv_createTable( const QString& tableSchemaName )
{
	TableSchema *ts = m_tables_byname[tableSchemaName];
	if (!ts)
		return false;
	return drv_createTable(*ts);
}

bool Connection::beginAutoCommitTransaction(TransactionGuard &tg)
{
	if ((m_driver->d->features & Driver::IgnoreTransactions)
		|| !d->autoCommit)
	{
		tg.setTransaction( Transaction() );
		return true;
	}

	// commit current transaction (if present) for drivers
	// that allow single transaction per connection
	if (m_driver->d->features & Driver::SingleTransactions) {
		if (d->default_trans_started_inside) //only commit internally started transaction
			if (!commitTransaction(d->default_trans, true)) {
				tg.setTransaction( Transaction() );
				return false; //we have a real error
			}

		d->default_trans_started_inside = d->default_trans.isNull();
		if (!d->default_trans_started_inside) {
			tg.setTransaction( d->default_trans );
			tg.doNothing();
			return true; //reuse externally started transaction
		}
	}
	else if (!(m_driver->d->features & Driver::MultipleTransactions)) {
		tg.setTransaction( Transaction() );
		return true; //no trans. supported at all - just return
	}
	tg.setTransaction( beginTransaction() );
	return !error();
}

bool Connection::commitAutoCommitTransaction(const Transaction& trans)
{
	if (m_driver->d->features & Driver::IgnoreTransactions)
		return true;
	if (trans.isNull() || !m_driver->transactionsSupported())
		return true;
	if (m_driver->d->features & Driver::SingleTransactions) {
		if (!d->default_trans_started_inside) //only commit internally started transaction
			return true; //give up
	}
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
	if (!checkIsDatabaseUsed())
		return Transaction::null;
	Transaction trans;
	if (m_driver->d->features & Driver::IgnoreTransactions) {
		//we're creating dummy transaction data here,
		//so it will look like active
		trans.m_data = new TransactionData(this);
		d->transactions.append(trans);
		return trans;
	}
	if (m_driver->d->features & Driver::SingleTransactions) {
		if (d->default_trans.active()) {
			setError(ERR_TRANSACTION_ACTIVE, i18n("Transaction already started.") );
			return Transaction::null;
		}
		if (!(trans.m_data = drv_beginTransaction())) {
			SET_BEGIN_TR_ERROR;
			return Transaction::null;
		}
		d->default_trans = trans;
		d->transactions.append(trans);
		return d->default_trans;
	}
	if (m_driver->d->features & Driver::MultipleTransactions) {
		if (!(trans.m_data = drv_beginTransaction())) {
			SET_BEGIN_TR_ERROR;
			return Transaction::null;
		}
		d->transactions.append(trans);
		return trans;
	}

	SET_ERR_TRANS_NOT_SUPP;
	return Transaction::null;
}

bool Connection::commitTransaction(const Transaction trans, bool ignore_inactive)
{
	if (!isDatabaseUsed())
		return false;
//	if (!checkIsDatabaseUsed())
		//return false;
	if ( !m_driver->transactionsSupported()
		&& !(m_driver->d->features & Driver::IgnoreTransactions))
	{
		SET_ERR_TRANS_NOT_SUPP;
		return false;
	}
	Transaction t = trans;
	if (!t.active()) { //try default tr.
		if (!d->default_trans.active()) {
			if (ignore_inactive)
				return true;
			clearError();
			setError(ERR_NO_TRANSACTION_ACTIVE, i18n("Transaction not started.") );
			return false;
		}
		t = d->default_trans;
		d->default_trans = Transaction::null; //now: no default tr.
	}
	bool ret = true;
	if (! (m_driver->d->features & Driver::IgnoreTransactions) )
		ret = drv_commitTransaction(t.m_data);
	if (t.m_data)
		t.m_data->m_active = false; //now this transaction if inactive
	if (!d->dont_remove_transactions) //true=transaction obj will be later removed from list
		d->transactions.remove(t);
	if (!ret && !error())
		setError(ERR_ROLLBACK_OR_COMMIT_TRANSACTION, i18n("Error on commit transaction"));
	return ret;
}

bool Connection::rollbackTransaction(const Transaction trans, bool ignore_inactive)
{
	if (!isDatabaseUsed())
		return false;
//	if (!checkIsDatabaseUsed())
//		return false;
	if ( !m_driver->transactionsSupported()
		&& !(m_driver->d->features & Driver::IgnoreTransactions))
	{
		SET_ERR_TRANS_NOT_SUPP;
		return false;
	}
	Transaction t = trans;
	if (!t.active()) { //try default tr.
		if (!d->default_trans.active()) {
			if (ignore_inactive)
				return true;
			clearError();
			setError(ERR_NO_TRANSACTION_ACTIVE, i18n("Transaction not started.") );
			return false;
		}
		t = d->default_trans;
		d->default_trans = Transaction::null; //now: no default tr.
	}
	bool ret = true;
	if (! (m_driver->d->features & Driver::IgnoreTransactions) )
	 	ret = drv_rollbackTransaction(t.m_data);
	if (t.m_data)
		t.m_data->m_active = false; //now this transaction if inactive
	if (!d->dont_remove_transactions) //true=transaction obj will be later removed from list
		d->transactions.remove(t);
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
	return d->default_trans;
}

void Connection::setDefaultTransaction(const Transaction& trans)
{
	if (!isDatabaseUsed())
		return;
//	if (!checkIsDatabaseUsed())
	//	return;
	if ( !(m_driver->d->features & Driver::IgnoreTransactions)
		&& (!trans.active() || !m_driver->transactionsSupported()) )
	{
		return;
	}
	d->default_trans = trans;
}

const QValueList<Transaction>& Connection::transactions()
{
	return d->transactions;
}

bool Connection::autoCommit() const
{
	return d->autoCommit;
}

bool Connection::setAutoCommit(bool on)
{
	if (d->autoCommit == on || m_driver->d->features & Driver::IgnoreTransactions)
		return true;
	if (!drv_setAutoCommit(on))
		return false;
	d->autoCommit = on;
	return true;
}

TransactionData* Connection::drv_beginTransaction()
{
	QString old_sql = m_sql; //don't
	if (!executeSQL( "BEGIN" ))
		return 0;
	return new TransactionData(this);
}

bool Connection::drv_commitTransaction(TransactionData *)
{
	return executeSQL( "COMMIT" );
}

bool Connection::drv_rollbackTransaction(TransactionData *)
{
	return executeSQL( "ROLLBACK" );
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
		KexiDBWarn << "Connection::deleteCursor(): Cannot delete the cursor not owned by the same connection!" << endl;
		return false;
	}
	const bool ret = cursor->close();
	delete cursor;
	return ret;
}

bool Connection::setupObjectSchemaData( const RowData &data, SchemaData &sdata )
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
	if (!KexiUtils::isIdentifier( sdata.m_name )) {
		setError(ERR_INVALID_IDENTIFIER, i18n("Invalid object name \"%1\"").arg(sdata.m_name));
		return false;
	}
	sdata.m_caption = data[3].toString();
	sdata.m_desc = data[4].toString();

//	KexiDBDbg<<"@@@ Connection::setupObjectSchemaData() == " << sdata.schemaDataDebugString() << endl;
	return true;
}

tristate Connection::loadObjectSchemaData( int objectID, SchemaData &sdata )
{
	RowData data;
	if (true!=querySingleRecord(QString::fromLatin1(
		"SELECT o_id, o_type, o_name, o_caption, o_desc FROM kexi__objects where o_id=%1")
		.arg(objectID), data))
		return cancelled;
	return setupObjectSchemaData( data, sdata );
}

tristate Connection::loadObjectSchemaData( int objectType, const QString& objectName, SchemaData &sdata )
{
	RowData data;
	if (true!=querySingleRecord(QString::fromLatin1("SELECT o_id, o_type, o_name, o_caption, o_desc "
		"FROM kexi__objects WHERE o_type=%1 AND lower(o_name)=%2")
		.arg(objectType).arg(m_driver->valueToSQL(Field::Text, objectName.lower())), data))
		return cancelled;
	return setupObjectSchemaData( data, sdata );
}

bool Connection::storeObjectSchemaData( SchemaData &sdata, bool newObject )
{
	TableSchema *ts = m_tables_byname["kexi__objects"];
	if (!ts)
		return false;
	if (newObject) {
		int existingID;
		if (querySingleNumber(QString::fromLatin1(
			"SELECT o_id FROM kexi__objects WHERE o_type=%1 AND lower(o_name)=%2")
			.arg(sdata.type()).arg(m_driver->valueToSQL(Field::Text, sdata.name().lower())), existingID))
		{
			//we already have stored a schema data with the same name and type:
			//just update it's properties as it would be existing object
			sdata.m_id = existingID;
			newObject = false;
		}
	}
	if (newObject) {
		FieldList *fl;
		bool ok;
		if (sdata.id()<=0) {//get new ID
			fl = ts->subList("o_type", "o_name", "o_caption", "o_desc");
			ok = fl!=0;
			if (ok && !insertRecord(*fl, QVariant(sdata.type()), QVariant(sdata.name()),
			QVariant(sdata.caption()), QVariant(sdata.description()) ))
				ok = false;
			delete fl;
			if (!ok)
				return false;
			//fetch newly assigned ID
//! @todo safe to cast it?
			int obj_id = (int)lastInsertedAutoIncValue("o_id",*ts);
			KexiDBDbg << "######## NEW obj_id == " << obj_id << endl;
			if (obj_id<=0)
				return false;
			sdata.m_id = obj_id;
			return true;
		} else {
			fl = ts->subList("o_id", "o_type", "o_name", "o_caption", "o_desc");
			ok = fl!=0;
			if (ok && !insertRecord(*fl, QVariant(sdata.id()), QVariant(sdata.type()), QVariant(sdata.name()),
				QVariant(sdata.caption()), QVariant(sdata.description()) ))
				ok = false;
			delete fl;
			return ok;
		}
	}
	//existing object:
	return executeSQL(QString("UPDATE kexi__objects SET o_type=%2, o_caption=%3, o_desc=%4 WHERE o_id=%1")
		.arg(sdata.id()).arg(sdata.type())
		.arg(m_driver->valueToSQL(KexiDB::Field::Text, sdata.caption()))
		.arg(m_driver->valueToSQL(KexiDB::Field::Text, sdata.description())) );
}

tristate Connection::querySingleRecordInternal(RowData &data, const QString* sql, QuerySchema* query)
{
	Q_ASSERT(sql || query);
//! @todo does not work with non-SQL data sources
	if (sql)
		m_sql = *sql + " LIMIT 1"; // is this safe?
	KexiDB::Cursor *cursor;
	if (!(cursor = sql ? executeQuery( m_sql ) : executeQuery( *query ))) {
		KexiDBDbg << "Connection::querySingleRecord(): !executeQuery()" << endl;
		return false;
	}
	if (!cursor->moveFirst() || cursor->eof()) {
		const tristate result = cursor->error() ? false : cancelled;
		KexiDBDbg << "Connection::querySingleRecord(): !cursor->moveFirst() || cursor->eof()" << endl;
		setError(cursor);
		deleteCursor(cursor);
		return result;
	}
	cursor->storeCurrentRow(data);
	return deleteCursor(cursor);
}

tristate Connection::querySingleRecord(const QString& sql, RowData &data)
{
	return querySingleRecordInternal(data, &sql, 0);
}

tristate Connection::querySingleRecord(QuerySchema& query, RowData &data)
{
	return querySingleRecordInternal(data, 0, &query);
}

bool Connection::checkIfColumnExists(Cursor *cursor, uint column)
{
	if (column >= cursor->fieldCount()) {
		setError(ERR_CURSOR_RECORD_FETCHING, i18n("Column %1 does not exist for the query.").arg(column));
		return false;
	}
	return true;
}

tristate Connection::querySingleString(const QString& sql, QString &value, uint column)
{
	KexiDB::Cursor *cursor;
	m_sql = sql + " LIMIT 1"; // is this safe?;
	if (!(cursor = executeQuery( m_sql ))) {
		KexiDBDbg << "Connection::querySingleRecord(): !executeQuery()" << endl;
		return false;
	}
	if (!cursor->moveFirst() || cursor->eof()) {
		const tristate result = cursor->error() ? false : cancelled;
		KexiDBDbg << "Connection::querySingleRecord(): !cursor->moveFirst() || cursor->eof()" << endl;
		deleteCursor(cursor);
		return result;
	}
	if (!checkIfColumnExists(cursor, column)) {
		deleteCursor(cursor);
		return false;
	}
	value = cursor->value(column).toString();
	return deleteCursor(cursor);
}

tristate Connection::querySingleNumber(const QString& sql, int &number, uint column)
{
	static QString str;
	static bool ok;
	const tristate result = querySingleString(sql, str, column);
	if (result!=true)
		return result;
	number = str.toInt(&ok);
	return ok;
}

bool Connection::queryStringList(const QString& sql, QStringList& list, uint column)
{
	KexiDB::Cursor *cursor;
	clearError();
	m_sql = sql;
	if (!(cursor = executeQuery( m_sql ))) {
		KexiDBDbg << "Connection::queryStringList(): !executeQuery()" << endl;
		return false;
	}
	if (!checkIfColumnExists(cursor, column)) {
		deleteCursor(cursor);
		return false;
	}
	cursor->moveFirst();
	if (cursor->error()) {
		setError(cursor);
		deleteCursor(cursor);
		return false;
	}
	list.clear();
	while (!cursor->eof()) {
		list.append( cursor->value(column).toString() );
		if (!cursor->moveNext() && cursor->error()) {
			setError(cursor);
			deleteCursor(cursor);
			return false;
		}
	}
	return deleteCursor(cursor);
}

bool Connection::resultExists(const QString& sql, bool &success)
{
	KexiDB::Cursor *cursor;
	//optimization
	if (m_driver->beh->SELECT_1_SUBQUERY_SUPPORTED) {
		//this is at least for sqlite
		if (sql.left(6).upper() == "SELECT")
			m_sql = QString("SELECT 1 FROM (") + sql + ") LIMIT 1"; // is this safe?;
		else
			m_sql = sql;
	}
	else {
		if (sql.left(6).upper() == "SELECT")
			m_sql = sql + " LIMIT 1"; //not always safe!
		else
			m_sql = sql;
	}
	if (!(cursor = executeQuery( m_sql ))) {
		KexiDBDbg << "Connection::querySingleRecord(): !executeQuery()" << endl;
		success = false;
		return false;
	}
	if (!cursor->moveFirst() || cursor->eof()) {
		success = !cursor->error();
		KexiDBDbg << "Connection::querySingleRecord(): !cursor->moveFirst() || cursor->eof()" << endl;
		setError(cursor);
		deleteCursor(cursor);
		return false;
	}
	success = deleteCursor(cursor);
	return true;
}

bool Connection::isEmpty( TableSchema& table, bool &success )
{
	return !resultExists( selectStatement( *table.query() ), success );
}

int Connection::resultCount(const QString& sql)
{
	int count = -1; //will be changed only on success of querySingleNumber()
	m_sql = QString::fromLatin1("SELECT COUNT() FROM (") + sql + ")";
	querySingleNumber(m_sql, count);
	return count;
}

//! @internal used by storeExtendedTableSchemaData()
static void addFieldPropertyToExtendedTableSchemaData( 
	Field *f, const char* propertyName, const QVariant& propertyValue, 
	QDomDocument& doc, QDomElement& extendedTableSchemaMainEl, 
	bool& extendedTableSchemaStringIsEmpty,
	bool custom = false )
{
	if (extendedTableSchemaStringIsEmpty) {//init document
		extendedTableSchemaMainEl = doc.createElement("EXTENDED_TABLE_SCHEMA");
		doc.appendChild( extendedTableSchemaMainEl );
		extendedTableSchemaMainEl.setAttribute("version", QString::number(KEXIDB_EXTENDED_TABLE_SCHEMA_VERSION));
		extendedTableSchemaStringIsEmpty = false;
	}
	QDomElement extendedTableSchemaFieldEl = doc.createElement("field");
	extendedTableSchemaMainEl.appendChild( extendedTableSchemaFieldEl );
	extendedTableSchemaFieldEl.setAttribute("name", f->name());
	if (custom)
		extendedTableSchemaFieldEl.setAttribute("custom", "true");
	QDomElement extendedTableSchemaFieldPropertyEl = doc.createElement("property");
	extendedTableSchemaFieldEl.appendChild( extendedTableSchemaFieldPropertyEl );
	extendedTableSchemaFieldPropertyEl.setAttribute("name", propertyName);
	QDomElement extendedTableSchemaFieldPropertyValueEl;
	switch (propertyValue.type()) {
	case QVariant::String:
		extendedTableSchemaFieldPropertyValueEl = doc.createElement("string");
		break;
	case QVariant::CString:
		extendedTableSchemaFieldPropertyValueEl = doc.createElement("cstring");
		break;
	case QVariant::Int:
	case QVariant::Double:
	case QVariant::UInt:
	case QVariant::LongLong:
	case QVariant::ULongLong:
		extendedTableSchemaFieldPropertyValueEl = doc.createElement("number");
		break;
	case QVariant::Bool:
		extendedTableSchemaFieldPropertyValueEl = doc.createElement("bool");
		break;
	default:
//! @todo add more QVariant types
		KexiDBFatal << "addFieldPropertyToExtendedTableSchemaData(): impl. error" << endl;
	}
	extendedTableSchemaFieldPropertyEl.appendChild( extendedTableSchemaFieldPropertyValueEl );
	extendedTableSchemaFieldPropertyValueEl.appendChild( 
		doc.createTextNode( propertyValue.toString() ) );
}

bool Connection::storeExtendedTableSchemaData(TableSchema& tableSchema)
{
	//todo: future: save in older versions if neeed
	QDomDocument doc("EXTENDED_TABLE_SCHEMA");
	QDomElement extendedTableSchemaMainEl;
	bool extendedTableSchemaStringIsEmpty = true;

	//for each field:
	Field *f;
	for (Field::ListIterator it( *tableSchema.fields() ); (f = it.current()); ++it) {
		if (f->visibleDecimalPlaces()>=0 && KexiDB::supportsVisibleDecimalPlacesProperty(f->type())) {
			addFieldPropertyToExtendedTableSchemaData( 
				f, "visibleDecimalPlaces", f->visibleDecimalPlaces(), doc, 
				extendedTableSchemaMainEl, extendedTableSchemaStringIsEmpty );
		}
		// boolean field with "not null"

		// add custom properties
		const Field::CustomPropertiesMap customProperties(f->customProperties());
		foreach( Field::CustomPropertiesMap::ConstIterator, itCustom, customProperties ) {
			addFieldPropertyToExtendedTableSchemaData( 
				f, itCustom.key(), itCustom.data(), doc, 
				extendedTableSchemaMainEl, extendedTableSchemaStringIsEmpty, /*custom*/true );
		}
	}

	// Store extended schema information (see ExtendedTableSchemaInformation in Kexi Wiki)
	if (extendedTableSchemaStringIsEmpty) {
#ifdef KEXI_DEBUG_GUI
		KexiUtils::addAlterTableActionDebug(QString("** Extended table schema REMOVED."));
#endif
		if (!removeDataBlock( tableSchema.id(), "extended_schema"))
			return false;
	}
	else {
		QString docString( doc.toString(1) );
#ifdef KEXI_DEBUG_GUI
		KexiUtils::addAlterTableActionDebug(QString("** Extended table schema set to:\n")+docString);
#endif
		if (!storeDataBlock( tableSchema.id(), docString, "extended_schema" ))
			return false;
	}
	return true;
}

bool Connection::loadExtendedTableSchemaData(TableSchema& tableSchema)
{
#define loadExtendedTableSchemaData_ERR \
	{ setError(i18n("Error while loading extended table schema information.")); \
	  return false; }
#define loadExtendedTableSchemaData_ERR2(details) \
	{ setError(i18n("Error while loading extended table schema information."), details); \
	  return false; }
#define loadExtendedTableSchemaData_ERR3(data) \
	{ setError(i18n("Error while loading extended table schema information."), \
	  i18n("Invalid XML data: ") + data.left(1024) ); \
	  return false; }

	// Load extended schema information, if present (see ExtendedTableSchemaInformation in Kexi Wiki)
	QString extendedTableSchemaString;
	tristate res = loadDataBlock( tableSchema.id(), extendedTableSchemaString, "extended_schema" );
	if (!res)
		loadExtendedTableSchemaData_ERR;
	// extendedTableSchemaString will be just empty if there is no such data block

//<temp. for LookupFieldSchema tests>
	if (tableSchema.name()=="cars") {
		LookupFieldSchema *lookupFieldSchema = new LookupFieldSchema();
		lookupFieldSchema->setRowSourceType(LookupFieldSchema::Table);
		lookupFieldSchema->setRowSource("persons");
		lookupFieldSchema->setBoundColumn(0); //id
		lookupFieldSchema->setVisibleColumn(3); //surname
		tableSchema.setLookupFieldSchema( "owner", lookupFieldSchema );
	}
//</temp. for LookupFieldSchema tests>

	if (extendedTableSchemaString.isEmpty())
		return true;

	QDomDocument doc;
	QString errorMsg;
	int errorLine, errorColumn;
	if (!doc.setContent( extendedTableSchemaString, &errorMsg, &errorLine, &errorColumn ))
		loadExtendedTableSchemaData_ERR2( i18n("Error in XML data: \"%1\" in line %2, column %3.\nXML data: ")
			.arg(errorMsg).arg(errorLine).arg(errorColumn) + extendedTableSchemaString.left(1024));

//! @todo look at the current format version (KEXIDB_EXTENDED_TABLE_SCHEMA_VERSION)
	
	if (doc.doctype().name()!="EXTENDED_TABLE_SCHEMA")
		loadExtendedTableSchemaData_ERR3( extendedTableSchemaString );

	QDomElement docEl = doc.documentElement();
	if (docEl.tagName()!="EXTENDED_TABLE_SCHEMA")
		loadExtendedTableSchemaData_ERR3( extendedTableSchemaString );

	for (QDomNode n = docEl.firstChild(); !n.isNull(); n = n.nextSibling()) {
		QDomElement fieldEl = n.toElement();
		if (fieldEl.tagName()=="field") {
			Field *f = tableSchema.field( fieldEl.attribute("name") );
			if (f) {
				//set properties of the field:
//! @todo more properties
				for (QDomNode propNode = fieldEl.firstChild(); 
					!propNode.isNull(); propNode = propNode.nextSibling())
				{
					QDomElement propEl = propNode.toElement();
					bool ok;
					int intValue;
					if (propEl.tagName()=="property") {
						QCString propertyName = propEl.attribute("name").latin1();
						if (propEl.attribute("custom")=="true") {
							//custom property
							f->setCustomProperty(propertyName, 
								KexiDB::loadPropertyValueFromXML( propEl.firstChild() ));
						}
						else if (propertyName == "visibleDecimalPlaces"
							&& KexiDB::supportsVisibleDecimalPlacesProperty(f->type()))
						{
							intValue = KexiDB::loadIntPropertyValueFromXML( propEl.firstChild(), &ok );
							if (ok)
								f->setVisibleDecimalPlaces(intValue);
						}
//! @todo more properties...
					}
					else if (propEl.tagName()=="lookup-column") {
						LookupFieldSchema *lookupFieldSchema = LookupFieldSchema::loadFromXML(propEl);
						tableSchema.setLookupFieldSchema( f->name(), lookupFieldSchema );
					}
				}
			}
			else {
				KexiDBWarn << "Connection::loadExtendedTableSchemaData(): no such field \"" 
					<< fieldEl.attribute("name") << "\" in table \"" << tableSchema.name() << "\"" << endl;
			}
		}
	}

	return true;
}

KexiDB::TableSchema* Connection::setupTableSchema( const RowData &data )
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
		QString::fromLatin1("SELECT t_id, f_type, f_name, f_length, f_precision, f_constraints, "
			"f_options, f_default, f_order, f_caption, f_help"
			" FROM kexi__fields WHERE t_id=%1 ORDER BY f_order").arg(t->m_id) )))
	{
		delete t;
		return 0;
	}
	if (!cursor->moveFirst()) {
		if (!cursor->error() && cursor->eof()) {
			setError(i18n("Table has no fields defined."));
		}
		deleteCursor(cursor);
		delete t;
		return 0;
	}

	// For each field: load its schema
	bool ok;
	while (!cursor->eof()) {
//		KexiDBDbg<<"@@@ f_name=="<<cursor->value(2).asCString()<<endl;

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

		if (!KexiUtils::isIdentifier( cursor->value(2).asString() )) {
			setError(ERR_INVALID_IDENTIFIER, i18n("Invalid object name \"%1\"")
				.arg( cursor->value(2).asString() ));
			ok=false;
			break;
		}

		Field *f = new Field(
			cursor->value(2).asString(), (Field::Type)f_type, f_constr, f_opts, f_len, f_prec );
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

	if (!loadExtendedTableSchemaData(*t)) {
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
	if (true!=querySingleRecord(QString::fromLatin1(
		"SELECT o_id, o_type, o_name, o_caption, o_desc FROM kexi__objects WHERE lower(o_name)='%1' AND o_type=%2")
			.arg(m_tableName).arg(KexiDB::TableObjectType), data))
		return 0;

	return setupTableSchema(data);
}

TableSchema* Connection::tableSchema( int tableId )
{
	TableSchema *t = m_tables[tableId];
	if (t)
		return t;
	//not found: retrieve schema
	RowData data;
	if (true!=querySingleRecord(QString::fromLatin1(
		"SELECT o_id, o_type, o_name, o_caption, o_desc FROM kexi__objects WHERE o_id=%1")
		.arg(tableId), data))
		return 0;

	return setupTableSchema(data);
}

tristate Connection::loadDataBlock( int objectID, QString &dataString, const QString& dataID )
{
	if (objectID<=0)
		return false;
	return querySingleString(
		QString("SELECT o_data FROM kexi__objectdata WHERE o_id=") + QString::number(objectID)
		+ " AND " + KexiDB::sqlWhere(m_driver, KexiDB::Field::Text, "o_sub_id", dataID),
		dataString );
}

bool Connection::storeDataBlock( int objectID, const QString &dataString, const QString& dataID )
{
	if (objectID<=0)
		return false;
	QString sql(QString::fromLatin1("SELECT kexi__objectdata.o_id FROM kexi__objectdata WHERE o_id=%1").arg(objectID));
	QString sql_sub( KexiDB::sqlWhere(m_driver, KexiDB::Field::Text, "o_sub_id", dataID) );

	bool ok, exists;
	exists = resultExists(sql + " and " + sql_sub, ok);
	if (!ok)
		return false;
	if (exists) {
		return executeSQL( "UPDATE kexi__objectdata SET o_data="
			+ m_driver->valueToSQL( KexiDB::Field::BLOB, dataString )
			+ " WHERE o_id=" + QString::number(objectID) + " AND " + sql_sub );
	}
	return executeSQL(
		QString::fromLatin1("INSERT INTO kexi__objectdata (o_id, o_data, o_sub_id) VALUES (")
		+ QString::number(objectID) +"," + m_driver->valueToSQL( KexiDB::Field::BLOB, dataString )
		+ "," + m_driver->valueToSQL( KexiDB::Field::Text, dataID ) + ")" );
}

bool Connection::removeDataBlock( int objectID, const QString& dataID)
{
	if (objectID<=0)
		return false;
	if (dataID.isEmpty())
		return KexiDB::deleteRow(*this, "kexi__objectdata", "o_id", QString::number(objectID));
	else
		return KexiDB::deleteRow(*this, "kexi__objectdata",
		"o_id", KexiDB::Field::Integer, objectID, "o_sub_id", KexiDB::Field::Text, dataID);
}

KexiDB::QuerySchema* Connection::setupQuerySchema( const RowData &data )
{
	bool ok = true;
	const int objID = data[0].toInt(&ok);
	if (!ok)
		return false;
	QString sqlText;
	if (!loadDataBlock( objID, sqlText, "sql" )) {
		setError(ERR_OBJECT_NOT_FOUND, 
			i18n("Could not find definition for query \"%1\". Removing this query is recommended.")
			.arg(data[2].toString()));
		return 0;
	}
	d->parser()->parse( sqlText );
	KexiDB::QuerySchema *query = d->parser()->query();
	//error?
	if (!query) {
		setError(ERR_SQL_PARSE_ERROR, 
			i18n("<p>Could not load definition for query \"%1\". "
			"SQL statement for this query is invalid:<br><tt>%2</tt></p>\n"
			"<p>You can open this query in Text View and correct it.</p>").arg(data[2].toString())
			.arg(d->parser()->statement()));
		return 0;
	}
	if (!setupObjectSchemaData( data, *query )) {
		delete query;
		return 0;
	}
	m_queries.insert(query->m_id, query);
	m_queries_byname.insert(query->m_name, query);
	return query;
}

QuerySchema* Connection::querySchema( const QString& queryName )
{
	QString m_queryName = queryName.lower();
	QuerySchema *q = m_queries_byname[m_queryName];
	if (q)
		return q;
	//not found: retrieve schema
	RowData data;
	if (true!=querySingleRecord(QString::fromLatin1(
		"SELECT o_id, o_type, o_name, o_caption, o_desc FROM kexi__objects WHERE lower(o_name)='%1' AND o_type=%2")
			.arg(m_queryName).arg(KexiDB::QueryObjectType), data))
		return 0;

	return setupQuerySchema(data);
}

QuerySchema* Connection::querySchema( int queryId )
{
	QuerySchema *q = m_queries[queryId];
	if (q)
		return q;
	//not found: retrieve schema
	clearError();
	RowData data;
	if (true!=querySingleRecord(QString::fromLatin1(
		"SELECT o_id, o_type, o_name, o_caption, o_desc FROM kexi__objects WHERE o_id=%1").arg(queryId), data))
		return 0;

	return setupQuerySchema(data);
}

bool Connection::setQuerySchemaObsolete( const QString& queryName )
{
	QuerySchema* oldQuery = querySchema( queryName );
	if (!oldQuery)
		return false;
	d->obsoleteQueries.append(oldQuery);
	m_queries_byname.take(queryName);
	m_queries.take(oldQuery->id());
	return true;
}

TableSchema* Connection::newKexiDBSystemTableSchema(const QString& tsname)
{
	TableSchema *ts = new TableSchema(tsname.lower());
	insertInternalTableSchema( ts );
	return ts;
}

bool Connection::isInternalTableSchema(const QString& tableName)
{
	return (m_kexiDBSystemTables[ m_tables_byname[tableName] ]) 
		// these are here for compatiblility because we're no longer instantiate 
		// them but can exist in projects created with previous Kexi versions:
		|| tableName=="kexi__final" || tableName=="kexi__useractions";
}

void Connection::insertInternalTableSchema(TableSchema *tableSchema)
{
	tableSchema->setKexiDBSystem(true);
	m_kexiDBSystemTables.insert(tableSchema, tableSchema);
	m_tables_byname.insert(tableSchema->name(), tableSchema);
}

//! Creates kexi__* tables.
bool Connection::setupKexiDBSystemSchema()
{
	if (!m_kexiDBSystemTables.isEmpty())
		return true; //already set up

	TableSchema *t_objects = newKexiDBSystemTableSchema("kexi__objects");
	t_objects->addField( new Field("o_id", Field::Integer, Field::PrimaryKey | Field::AutoInc, Field::Unsigned) )
	.addField( new Field("o_type", Field::Byte, 0, Field::Unsigned) )
	.addField( new Field("o_name", Field::Text) )
	.addField( new Field("o_caption", Field::Text ) )
	.addField( new Field("o_desc", Field::LongText ) );

	t_objects->debug();

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

/*	TableSchema *t_querydata = newKexiDBSystemTableSchema("kexi__querydata");
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
	.addField( new Field("t_order", Field::Integer, 0, Field::Unsigned) );*/

	TableSchema *t_db = newKexiDBSystemTableSchema("kexi__db");
	t_db->addField( new Field("db_property", Field::Text, Field::NoConstraints, Field::NoOptions, 32 ) )
	.addField( new Field("db_value", Field::LongText ) );

/* moved to KexiProject...
	TableSchema *t_parts = newKexiDBSystemTableSchema("kexi__parts");
	t_parts->addField( new Field("p_id", Field::Integer, Field::PrimaryKey | Field::AutoInc, Field::Unsigned) )
	.addField( new Field("p_name", Field::Text) )
	.addField( new Field("p_mime", Field::Text ) )
	.addField( new Field("p_url", Field::Text ) );
*/

/*UNUSED
	TableSchema *t_final = newKexiDBSystemTableSchema("kexi__final");
	t_final->addField( new Field("p_id", Field::Integer, 0, Field::Unsigned) )
	.addField( new Field("property", Field::LongText ) )
	.addField( new Field("value", Field::BLOB) );

	TableSchema *t_useractions = newKexiDBSystemTableSchema("kexi__useractions");
	t_useractions->addField( new Field("p_id", Field::Integer, 0, Field::Unsigned) )
	.addField( new Field("scope", Field::Integer ) )
	.addField( new Field("name", Field::LongText ) )
	.addField( new Field("text", Field::LongText ) )
	.addField( new Field("icon", Field::LongText ) )
	.addField( new Field("method", Field::Integer ) )
	.addField( new Field("arguments", Field::LongText) );
*/
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
	if (!d->availableDatabaseName.isEmpty()) {
		return d->availableDatabaseName;
	}
	return m_driver->beh->ALWAYS_AVAILABLE_DATABASE_NAME;
}

void Connection::setAvailableDatabaseName(const QString& dbName)
{
	d->availableDatabaseName = dbName;
}

bool Connection::updateRow(QuerySchema &query, RowData& data, RowEditBuffer& buf, bool useROWID)
{
// Each SQL identifier needs to be escaped in the generated query.
//	query.debug();

	KexiDBDbg << "Connection::updateRow.." << endl;
	clearError();
	//--get PKEY
	if (buf.dbBuffer().isEmpty()) {
		KexiDBDbg << " -- NO CHANGES DATA!" << endl;
		return true;
	}
	TableSchema *mt = query.masterTable();
	if (!mt) {
		KexiDBWarn << " -- NO MASTER TABLE!" << endl;
		setError(ERR_UPDATE_NO_MASTER_TABLE,
			i18n("Could not update row because there is no master table defined."));
		return false;
	}
	IndexSchema *pkey = (mt->primaryKey() && !mt->primaryKey()->fields()->isEmpty()) ? mt->primaryKey() : 0;
	if (!useROWID && !pkey) {
		KexiDBWarn << " -- NO MASTER TABLE's PKEY!" << endl;
		setError(ERR_UPDATE_NO_MASTER_TABLES_PKEY,
			i18n("Could not update row because master table has no primary key defined."));
//! @todo perhaps we can try to update without using PKEY?
		return false;
	}
	//update the record:
	m_sql = "UPDATE " + escapeIdentifier(mt->name()) + " SET ";
	QString sqlset, sqlwhere;
	sqlset.reserve(1024);
	sqlwhere.reserve(1024);
	KexiDB::RowEditBuffer::DBMap b = buf.dbBuffer();
	for (KexiDB::RowEditBuffer::DBMap::ConstIterator it=b.constBegin();it!=b.constEnd();++it) {
		if (it.key()->field->table()!=mt)
			continue; // skip values for fields outside of the master table (e.g. a "visible value" of the lookup field)
		if (!sqlset.isEmpty())
			sqlset+=",";
		sqlset += (escapeIdentifier(it.key()->field->name()) + "=" +
			m_driver->valueToSQL(it.key()->field,it.data()));
	}
	if (pkey) {
		QValueVector<int> pkeyFieldsOrder = query.pkeyFieldsOrder();
		KexiDBDbg << pkey->fieldCount() << " ? " << query.pkeyFieldsCount() << endl;
		if (pkey->fieldCount() != query.pkeyFieldsCount()) { //sanity check
			KexiDBWarn << " -- NO ENTIRE MASTER TABLE's PKEY SPECIFIED!" << endl;
			setError(ERR_UPDATE_NO_ENTIRE_MASTER_TABLES_PKEY,
				i18n("Could not update row because it does not contain entire master table's primary key."));
			return false;
		}
		if (!pkey->fields()->isEmpty()) {
			uint i=0;
			for (Field::ListIterator it = pkey->fieldsIterator(); it.current(); i++, ++it) {
				if (!sqlwhere.isEmpty())
					sqlwhere+=" AND ";
				QVariant val = data[ pkeyFieldsOrder[i] ];
				if (val.isNull() || !val.isValid()) {
					setError(ERR_UPDATE_NULL_PKEY_FIELD,
						i18n("Primary key's field \"%1\" cannot be empty.").arg(it.current()->name()));
	//js todo: pass the field's name somewhere!
					return false;
				}
				sqlwhere += ( escapeIdentifier(it.current()->name()) + "=" +
					m_driver->valueToSQL( it.current(), val ) );
			}
		}
	}
	else {//use ROWID
		sqlwhere = ( escapeIdentifier(m_driver->beh->ROW_ID_FIELD_NAME) + "="
			+ m_driver->valueToSQL(Field::BigInteger, data[data.size()-1]));
	}
	m_sql += (sqlset + " WHERE " + sqlwhere);
	KexiDBDbg << " -- SQL == " << ((m_sql.length() > 400) ? (m_sql.left(400)+"[.....]") : m_sql) << endl;

	if (!executeSQL(m_sql)) {
		setError(ERR_UPDATE_SERVER_ERROR, i18n("Row updating on the server failed."));
		return false;
	}
	//success: now also assign new value in memory:
	QMap<QueryColumnInfo*,int> fieldsOrder = query.fieldsOrder();
	QMap<QueryColumnInfo*,int>::ConstIterator fieldsOrderIt;
	for (KexiDB::RowEditBuffer::DBMap::ConstIterator it=b.constBegin();it!=b.constEnd();++it) {
		fieldsOrderIt = fieldsOrder.find( it.key() );
		if (fieldsOrderIt == fieldsOrder.constEnd()) {
			KexiDBWarn << "Connection::updateRow(): \"now also assign new value in memory\" step "
				"- could not find item '" << it.key()->aliasOrName() << "'" << endl;
			continue;
		}
		data[ fieldsOrderIt.data() ] = it.data();
	}
	return true;
}

bool Connection::insertRow(QuerySchema &query, RowData& data, RowEditBuffer& buf, bool getROWID)
{
// Each SQL identifier needs to be escaped in the generated query.
	KexiDBDbg << "Connection::updateRow.." << endl;
	clearError();
	//--get PKEY
	/*disabled: there may be empty rows (with autoinc)
	if (buf.dbBuffer().isEmpty()) {
		KexiDBDbg << " -- NO CHANGES DATA!" << endl;
		return true; }*/
	TableSchema *mt = query.masterTable();
	if (!mt) {
		KexiDBWarn << " -- NO MASTER TABLE!" << endl;
		setError(ERR_INSERT_NO_MASTER_TABLE,
			i18n("Could not insert row because there is no master table defined."));
		return false;
	}
	IndexSchema *pkey = (mt->primaryKey() && !mt->primaryKey()->fields()->isEmpty()) ? mt->primaryKey() : 0;
	if (!getROWID && !pkey)
		KexiDBWarn << " -- WARNING: NO MASTER TABLE's PKEY" << endl;

	QString sqlcols, sqlvals;
	sqlcols.reserve(1024);
	sqlvals.reserve(1024);

	//insert the record:
	m_sql = "INSERT INTO " + escapeIdentifier(mt->name()) + " (";
	KexiDB::RowEditBuffer::DBMap b = buf.dbBuffer();

	if (buf.dbBuffer().isEmpty()) {
		if (!getROWID && !pkey) {
			KexiDBWarn << " -- WARNING: MASTER TABLE's PKEY REQUIRED FOR INSERTING EMPTY ROWS: INSERT CANCELLED" << endl;
			setError(ERR_INSERT_NO_MASTER_TABLES_PKEY,
				i18n("Could not insert row because master table has no primary key defined."));
			return false;
		}
		if (pkey) {
			QValueVector<int> pkeyFieldsOrder = query.pkeyFieldsOrder();
//			KexiDBDbg << pkey->fieldCount() << " ? " << query.pkeyFieldsCount() << endl;
			if (pkey->fieldCount() != query.pkeyFieldsCount()) { //sanity check
				KexiDBWarn << " -- NO ENTIRE MASTER TABLE's PKEY SPECIFIED!" << endl;
				setError(ERR_INSERT_NO_ENTIRE_MASTER_TABLES_PKEY,
					i18n("Could not insert row because it does not contain entire master table's primary key.")
					.arg(query.name()));
				return false;
			}
		}
		//at least one value is needed for VALUES section: find it and set to NULL:
		Field *anyField = mt->anyNonPKField();
		if (!anyField) {
			if (!pkey) {
				KexiDBWarn << " -- WARNING: NO FILED AVAILABLE TO SET IT TO NULL" << endl;
				return false;
			}
			else {
				//try to set NULL in pkey field (could not work for every SQL engine!)
				anyField = pkey->fields()->first();
			}
		}
		sqlcols += escapeIdentifier(anyField->name());
		sqlvals += m_driver->valueToSQL(anyField,QVariant()/*NULL*/);
	}
	else {
		for (KexiDB::RowEditBuffer::DBMap::ConstIterator it=b.constBegin();it!=b.constEnd();++it) {
			if (!sqlcols.isEmpty()) {
				sqlcols+=",";
				sqlvals+=",";
			}
			sqlcols += escapeIdentifier(it.key()->field->name());
			sqlvals += m_driver->valueToSQL(it.key()->field,it.data());
		}
	}
	m_sql += (sqlcols + ") VALUES (" + sqlvals + ")");
//	KexiDBDbg << " -- SQL == " << m_sql << endl;

	bool res = executeSQL(m_sql);

	if (!res) {
		setError(ERR_INSERT_SERVER_ERROR, i18n("Row inserting on the server failed."));
		return false;
	}
	//success: now also assign new value in memory:
	QMap<QueryColumnInfo*,int> fieldsOrder = query.fieldsOrder();
	QMap<QueryColumnInfo*,int>::ConstIterator fieldsOrderIt;
	for (KexiDB::RowEditBuffer::DBMap::ConstIterator it=b.constBegin();it!=b.constEnd();++it) {
		fieldsOrderIt = fieldsOrder.find( it.key() );
		if (fieldsOrderIt == fieldsOrder.constEnd()) {
			KexiDBWarn << "Connection::insertRow(): \"now also assign new value in memory\" step "
				"- could not find item '" << it.key()->aliasOrName() << "'" << endl;
			continue;
		}
		data[ fieldsOrderIt.data() ] = it.data();
	}

	//fetch autoincremented values
	QueryColumnInfo::List *aif_list = query.autoIncrementFields();
	Q_ULLONG ROWID = 0;
	if (pkey && !aif_list->isEmpty()) {
		//! @todo now only if PKEY is present, this should also work when there's no PKEY
		QueryColumnInfo *id_columnInfo = aif_list->first();
//! @todo safe to cast it?
		Q_ULLONG last_id = lastInsertedAutoIncValue(
			id_columnInfo->field->name(), id_columnInfo->field->table()->name(), &ROWID);
		if (last_id==(Q_ULLONG)-1 || last_id<=0) {
			//! @todo show error
//! @todo remove just inserted row. How? Using ROLLBACK?
			return false;
		}
		RowData aif_data;
		QString getAutoIncForInsertedValue = QString::fromLatin1("SELECT ")
			+ query.autoIncrementSQLFieldsList(m_driver)
			+ QString::fromLatin1(" FROM ")
			+ escapeIdentifier(id_columnInfo->field->table()->name())
			+ QString::fromLatin1(" WHERE ")
			+ escapeIdentifier(id_columnInfo->field->name()) + "="
			+ QString::number(last_id);
		if (true!=querySingleRecord(getAutoIncForInsertedValue, aif_data)) {
			//! @todo show error
			return false;
		}
		QueryColumnInfo::ListIterator fi_it(*aif_list);
		QueryColumnInfo *fi;
		for (uint i=0; (fi = fi_it.current()); ++fi_it, i++) {
//			KexiDBDbg << "Connection::insertRow(): AUTOINCREMENTED FIELD " << fi->field->name() << " == "
//				<< aif_data[i].toInt() << endl;
			data[ fieldsOrder[ fi ] ] = aif_data[i];
		}
	}
	else {
		ROWID = drv_lastInsertRowID();
//		KexiDBDbg << "Connection::insertRow(): new ROWID == " << (uint)ROWID << endl;
		if (m_driver->beh->ROW_ID_FIELD_RETURNS_LAST_AUTOINCREMENTED_VALUE) {
			KexiDBWarn << "Connection::insertRow(): m_driver->beh->ROW_ID_FIELD_RETURNS_LAST_AUTOINCREMENTED_VALUE" << endl;
			return false;
		}
	}
	if (getROWID) {
//		KexiDBDbg << "Connection::insertRow(): new ROWID == " << (uint)ROWID << endl;
		data[data.size()-1] = ROWID;
	}
	return true;
}

bool Connection::deleteRow(QuerySchema &query, RowData& data, bool useROWID)
{
// Each SQL identifier needs to be escaped in the generated query.
	KexiDBWarn << "Connection::deleteRow.." << endl;
	clearError();
	TableSchema *mt = query.masterTable();
	if (!mt) {
		KexiDBWarn << " -- NO MASTER TABLE!" << endl;
		setError(ERR_DELETE_NO_MASTER_TABLE,
			i18n("Could not delete row because there is no master table defined.")
			.arg(query.name()));
		return false;
	}
	IndexSchema *pkey = (mt->primaryKey() && !mt->primaryKey()->fields()->isEmpty()) ? mt->primaryKey() : 0;

//! @todo allow to delete from a table without pkey
	if (!useROWID && !pkey) {
		KexiDBWarn << " -- WARNING: NO MASTER TABLE's PKEY" << endl;
		setError(ERR_DELETE_NO_MASTER_TABLES_PKEY,
			i18n("Could not delete row because there is no primary key for master table defined."));
		return false;
	}

	//update the record:
	m_sql = "DELETE FROM " + escapeIdentifier(mt->name()) + " WHERE ";
	QString sqlwhere;
	sqlwhere.reserve(1024);

	if (pkey) {
		QValueVector<int> pkeyFieldsOrder = query.pkeyFieldsOrder();
		KexiDBDbg << pkey->fieldCount() << " ? " << query.pkeyFieldsCount() << endl;
		if (pkey->fieldCount() != query.pkeyFieldsCount()) { //sanity check
			KexiDBWarn << " -- NO ENTIRE MASTER TABLE's PKEY SPECIFIED!" << endl;
			setError(ERR_DELETE_NO_ENTIRE_MASTER_TABLES_PKEY,
				i18n("Could not delete row because it does not contain entire master table's primary key."));
			return false;
		}
		uint i=0;
		for (Field::ListIterator it = pkey->fieldsIterator(); it.current(); i++, ++it) {
			if (!sqlwhere.isEmpty())
				sqlwhere+=" AND ";
			QVariant val = data[ pkeyFieldsOrder[i] ];
			if (val.isNull() || !val.isValid()) {
				setError(ERR_DELETE_NULL_PKEY_FIELD, i18n("Primary key's field \"%1\" cannot be empty.")
					.arg(it.current()->name()));
//js todo: pass the field's name somewhere!
				return false;
			}
			sqlwhere += ( escapeIdentifier(it.current()->name()) + "=" +
				m_driver->valueToSQL( it.current(), val ) );
		}
	}
	else {//use ROWID
		sqlwhere = ( escapeIdentifier(m_driver->beh->ROW_ID_FIELD_NAME) + "="
			+ m_driver->valueToSQL(Field::BigInteger, data[data.size()-1]));
	}
	m_sql += sqlwhere;
	KexiDBDbg << " -- SQL == " << m_sql << endl;

	if (!executeSQL(m_sql)) {
		setError(ERR_DELETE_SERVER_ERROR, i18n("Row deletion on the server failed."));
		return false;
	}
	return true;
}

bool Connection::deleteAllRows(QuerySchema &query)
{
	clearError();
	TableSchema *mt = query.masterTable();
	if (!mt) {
		KexiDBWarn << " -- NO MASTER TABLE!" << endl;
		return false;
	}
	IndexSchema *pkey = mt->primaryKey();
	if (!pkey || pkey->fields()->isEmpty())
		KexiDBWarn << "Connection::deleteAllRows -- WARNING: NO MASTER TABLE's PKEY" << endl;

	m_sql = "DELETE FROM " + escapeIdentifier(mt->name());

	KexiDBDbg << " -- SQL == " << m_sql << endl;

	if (!executeSQL(m_sql)) {
		setError(ERR_DELETE_SERVER_ERROR, i18n("Row deletion on the server failed."));
		return false;
	}
	return true;
}

void Connection::registerForTableSchemaChanges(TableSchemaChangeListenerInterface& listener,
	TableSchema &schema)
{
	QPtrList<TableSchemaChangeListenerInterface>* listeners = d->tableSchemaChangeListeners[&schema];
	if (!listeners) {
		listeners = new QPtrList<TableSchemaChangeListenerInterface>();
		d->tableSchemaChangeListeners.insert(&schema, listeners);
	}
//TODO: inefficient
	if (listeners->findRef( &listener )==-1)
		listeners->append( &listener );
}

void Connection::unregisterForTableSchemaChanges(TableSchemaChangeListenerInterface& listener,
	TableSchema &schema)
{
	QPtrList<TableSchemaChangeListenerInterface>* listeners = d->tableSchemaChangeListeners[&schema];
	if (!listeners)
		return;
//TODO: inefficient
	listeners->remove( &listener );
}

void Connection::unregisterForTablesSchemaChanges(TableSchemaChangeListenerInterface& listener)
{
	for (QPtrDictIterator< QPtrList<TableSchemaChangeListenerInterface> > it(d->tableSchemaChangeListeners);
		it.current(); ++it)
	{
		if (-1!=it.current()->find(&listener))
			it.current()->take();
	}
}

QPtrList<Connection::TableSchemaChangeListenerInterface>*
Connection::tableSchemaChangeListeners(TableSchema& tableSchema) const
{
	KexiDBDbg << d->tableSchemaChangeListeners.count() << endl;
	return d->tableSchemaChangeListeners[&tableSchema];
}

tristate Connection::closeAllTableSchemaChangeListeners(TableSchema& tableSchema)
{
	QPtrList<Connection::TableSchemaChangeListenerInterface> *listeners = d->tableSchemaChangeListeners[&tableSchema];
	if (!listeners)
		return true;
	QPtrListIterator<KexiDB::Connection::TableSchemaChangeListenerInterface> tmpListeners(*listeners); //safer copy
	tristate res = true;
	//try to close every window
	for (QPtrListIterator<KexiDB::Connection::TableSchemaChangeListenerInterface> it(tmpListeners);
		it.current() && res==true; ++it)
	{
		res = it.current()->closeListener();
	}
	return res;
}

/*PreparedStatement::Ptr Connection::prepareStatement(PreparedStatement::StatementType, 
		TableSchema&)
{
	//safe?
	return 0;
}*/

void Connection::setReadOnly(bool set)
{
	if (d->isConnected)
		return; //sanity
	d->readOnly = set;
}

bool Connection::isReadOnly() const
{
	return d->readOnly;
}

#include "connection.moc"
