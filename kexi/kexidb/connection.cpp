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
#include <kexidb/schemadata.h>
#include <kexidb/tableschema.h>
#include <kexidb/transaction.h>
#include <kexidb/cursor.h>
#include <kexidb/global.h>

#include <qfileinfo.h>
#include <qguardedptr.h>

#include <klocale.h>
#include <kdebug.h>

#include <assert.h>


namespace KexiDB {

class ConnectionPrivate
{
	public:
		ConnectionPrivate() { }
		~ConnectionPrivate() { }
		/*! Default transaction handle. 
		If transactions are supported: Any operation on database (e.g. inserts)
		that is started without specifing transaction context, will be performed
		in the context of this transaction. */
		Transaction m_default_trans;
		QValueList<Transaction> m_transactions;
};

}

using namespace KexiDB;
		
Connection::Connection( Driver *driver, const ConnectionData &conn_data )
	: QObject()
	,KexiDB::Object()
	,m_driver(driver)
	,m_data(conn_data)
	,m_is_connected(false)
	,m_autoCommit(true)
	,d(new ConnectionPrivate())
{
	m_tables.setAutoDelete(true);
	m_tables_byname.setAutoDelete(false);//m_tables is owner, not me
	m_queries.setAutoDelete(true);
	m_queries_byname.setAutoDelete(false);//m_queries is owner, not me
	m_cursors.setAutoDelete(true);
//	d->m_transactions.setAutoDelete(true);
	//reasonable sizes: TODO
	m_tables.resize(101);
	m_queries.resize(101);
	m_tables_byname.resize(101);
	m_queries_byname.resize(101);
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

bool Connection::disconnect()
{
	clearError();
	if (!m_is_connected)
		return true;
	
	//delete own cursors:
	m_cursors.clear();
	//delete own schemas
	m_tables.clear();
	m_queries.clear();

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

QStringList Connection::databaseNames()
{
	if (!checkConnected())
		return QStringList();

	QStringList list;

	drv_getDatabasesList( list );
	return list;
}

bool Connection::drv_getDatabasesList( QStringList &list )
{
	list.clear();
	return true;
}

bool Connection::drv_databaseExists( const QString &dbName, bool ignoreErrors )
{
	QStringList list = databaseNames();
	if (error()) {
		return false;
	}

	if (list.find( dbName )==list.end()) {
		if (!ignoreErrors)
			setError(ERR_OBJECT_NOT_EXISTING, i18n("The database '%1' does not exist.").arg(dbName));
		return false;
	}

	return true;
}

bool Connection::databaseExists( const QString &dbName, bool ignoreErrors )
{
	if (!checkConnected())
		return false;
	clearError();

	if (m_driver->isFileDriver()) {
		//for file-based db: file must exists and be accessible
//js: moved from useDatabase():
		QFileInfo file(dbName);
		if (!file.exists() || ( !file.isFile() && !file.isSymLink()) ) {
			if (!ignoreErrors)
				setError(ERR_OBJECT_NOT_EXISTING, i18n("Database file '%1' does not exist.").arg(m_data.fileName()) );
			return false;
		}
		if (!file.isReadable()) {
			if (!ignoreErrors)
				setError(ERR_ACCESS_RIGHTS, i18n("Database file '%1' is not readable.").arg(m_data.fileName()) );
			return false;
		}
		if (!file.isWritable()) {
			if (!ignoreErrors)
				setError(ERR_ACCESS_RIGHTS, i18n("Database file '%1' is not writable.").arg(m_data.fileName()) );
			return false;
		}
	}

	return drv_databaseExists(dbName, ignoreErrors);
}

#define createDatabase_CLOSE \
	{ if (!closeDatabase()) { \
		setError(i18n("Database '%1' created but could not be closed after creation.").arg(dbName) ); \
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
	
	QString my_dbName = dbName;
	const QStringList& db_lst = databaseNames();
	if (my_dbName.isEmpty() && !db_lst.isEmpty())
		my_dbName = db_lst.first();

	if (databaseExists( my_dbName )) {
		setError(ERR_OBJECT_EXISTS, i18n("Database '%1' already exists.").arg(dbName) );
		return false;
	}
	if (m_driver->isFileDriver()) {
		//update connection data if filename differs
		m_data.setFileName( dbName );

		//for file-based db: file must not exists
//		QFileInfo file(conn_data.fileName);
//		if (file.exists()) {
//		}
	}

	//low-level create
	if (!drv_createDatabase( dbName )) {
		setError(i18n("Error creating database '%1' on the server.").arg(dbName) );
		return false;
	}

	if (!m_driver->m_isDBOpenedAfterCreate) {
		//db need to be opened
		if (!useDatabase( dbName )) {
			setError(i18n("Database '%1' created but could not be opened.").arg(dbName) );
			return false;
		}
	}
	else {
		//just for the rule
		m_usedDatabase = my_dbName;
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
	
	//-create system tables
	KexiDB::TableSchema t_objects("kexi__objects");
	t_objects.addField( new Field("o_id", Field::Integer, Field::PrimaryKey | Field::AutoInc, Field::Unsigned) )
	.addField( new Field("o_type", Field::Byte, 0, Field::Unsigned) )
	.addField( new Field("o_name", Field::Text) )
	.addField( new Field("o_caption", Field::Text ) )
	.addField( new Field("o_help", Field::LongText ) );
	if (!drv_createTable( t_objects ))
		createDatabase_ERROR;

	KexiDB::TableSchema t_fields("kexi__fields");
	t_fields.addField( new Field("t_id", Field::Integer, 0, Field::Unsigned) )
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
	if (!drv_createTable( t_fields ))
		createDatabase_ERROR;

	KexiDB::TableSchema t_querydata("kexi__querydata");
	t_querydata.addField( new Field("q_id", Field::Integer, 0, Field::Unsigned) )
	.addField( new Field("q_sql", Field::LongText ) )
	.addField( new Field("q_valid", Field::Boolean ) );
	if (!drv_createTable( t_querydata ))
		createDatabase_ERROR;

	KexiDB::TableSchema t_db("kexi__db");
	t_db.addField( new Field("db_property", Field::Text, Field::NoConstraints, Field::NoOptions, 32 ) )
	.addField( new Field("db_value", Field::LongText ) );
	if (!drv_createTable( t_db ))
		createDatabase_ERROR;

	//insert KexiDB version info:
	//property="kexidb_major_ver", value=<major_version>
	//property="kexidb_minor_ver", value=<minor_version>
	//TODO(js)
	if (   !insertRecord(t_db, "kexidb_major_ver", KexiDB::majorVersion())
		|| !insertRecord(t_db, "kexidb_minor_ver", KexiDB::minorVersion()))
		createDatabase_ERROR;

//not needed	trans_g.commit();
	if (trans.active() && !commitTransaction(trans))
		createDatabase_ERROR;
	
	createDatabase_CLOSE;
	return true;
}

#undef createDatabase_CLOSE
#undef createDatabase_ERROR

bool Connection::useDatabase( const QString &dbName )
{
	if (!checkConnected())
		return false;
	
	QString my_dbName = dbName;
	const QStringList& db_lst = databaseNames();
	if (my_dbName.isEmpty() && !db_lst.isEmpty())
		my_dbName = db_lst.first();
	if (my_dbName.isEmpty())
		return false;

	if (m_usedDatabase == my_dbName)
		return true; //already used

	if (!databaseExists(my_dbName))
		return false; //database must exist

	if (!m_usedDatabase.isEmpty() && !closeDatabase()) //close db if already used
		return false;

	bool ok = drv_useDatabase( my_dbName );
	if (ok)
		m_usedDatabase = my_dbName;
	else
		m_usedDatabase = "";
	return ok;
}

bool Connection::closeDatabase()
{
	if (m_usedDatabase.isEmpty())
		return true; //no db used

	if (!checkConnected())
		return true;

	bool ret = true;

/*! \todo (js) add CLEVER algoright here for nested transactions */
	if (m_driver->transactionsSupported()) {
		//rollback all transactions
		QValueList<Transaction>::iterator it;
		for (it=d->m_transactions.begin(); it!= d->m_transactions.end(); ++it) {
			if (!rollbackTransaction(*it)) {//rollback as much as you can, don't stop on prev. errors
				ret = false;
			}
			else {
				KexiDBDbg << "Connection::closeDatabase(): transaction rolled back!" << endl;
			}
		}
		d->m_transactions.clear(); //free trans. data
	}
		
	if (!drv_closeDatabase())
		return false;

	m_usedDatabase = "";
	KexiDBDbg << "Connection::closeDatabase(): " << ret << endl;
	return ret;
}

bool Connection::dropDatabase( const QString &dbName )
{
	QString dbToDrop;
	if (dbName.isEmpty() && m_usedDatabase.isEmpty()) {
		if (!m_driver->isFileDriver())
		return false;
		//this is a file driver so reuse previously passed filename
		dbToDrop = m_data.m_fileName;
	}
	else
		dbToDrop = m_usedDatabase.isEmpty() ? dbName : m_usedDatabase;

	if (!checkConnected())
		return false;

	//close db if opened
	if (!m_usedDatabase.isEmpty()) {
		if (!closeDatabase())
			return false;
	}

	return drv_dropDatabase( dbToDrop );
}

QStringList Connection::tableNames()
{
	QStringList list;
	
	if (!isDatabaseUsed())
		return list;

	Cursor *c = executeQuery(QString("select * from kexi__objects where o_type=%1").arg(KexiDB::TableObjectType));
	if (!c)
		return list;
	for (c->moveFirst(); !c->eof(); c->moveNext())
	{
		list.append(c->value(2).toString()); //kexi__objects.o_name
	}

	deleteCursor(c);

	return list;
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

	Cursor *c = executeQuery(QString("select * from kexi__objects where o_type=%1").arg(objType));
	if (!c)
		return list;
	for (c->moveFirst(); !c->eof(); c->moveNext())
	{
		list.append(c->value(0).toInt()); //kexi__objects.o_id
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

QString Connection::valueToSQL( const Field::Type ftype, const QVariant& v ) const
{
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
		}case Field::BLOB:
//TODO: here special encoding method needed
			return QString("'")+v.toString()+"'";
		default:
			return QString::null;
	}
	return QString::null;
}

QString Connection::createTableStatement( const KexiDB::TableSchema& tableSchema )
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
		if (field->isUnsigned())
			v += "UNSIGNED ";
		v += m_driver->m_typeNames[field->m_type];
		if (field->m_length>0)
			v += QString("(%1)").arg(field->m_length);
		if (field->isUniqueKey())
			v += " UNIQUE";
//TODO: here is automatically a single-field key created
		if (field->isNotNull())
			v += " NOT NULL";
		if (field->defaultValue().isValid())
			v += QString(" DEFAULT ") + valueToSQL( field->m_type, field->m_defaultValue );
		sql += v;
	}
	sql += ")";
	return sql;
}

//todo
bool Connection::insertRecord(KexiDB::TableSchema &tableSchema, const QVariant& c1, const QVariant& c2)
{
	return drv_executeSQL(
		QString("INSERT INTO ") + tableSchema.name() + " VALUES ("
		+ valueToSQL( tableSchema.field(0)->type(), c1 ) + ","
		+ valueToSQL( tableSchema.field(1)->type(), c2 ) +")"
	);
}

/*QString Connection::insertIntoStatement( const KexiDB::TableSchema& tableSchema )
{
	QString sql = "INSERT INTO " + tableSchema.name() + " (";
	
}*/

QString Connection::queryStatement( const KexiDB::QuerySchema& querySchema )
{
	//todo
	return QString::null;
}

bool Connection::drv_createTable( const KexiDB::TableSchema& tableSchema )
{
	QString sql = createTableStatement(tableSchema);
	KexiDBDbg<<"******** "<<sql<<endl;
	return drv_executeSQL(sql);
}

#define SET_ERR_TRANS_NOT_SUPP \
	{ setError(ERR_UNSUPPORTED_DRV_FEATURE, \
	 i18n("Transactions are not supported for \"%1\" driver.").arg(m_driver->driverName() )); }

#define SET_BEGIN_TR_ERROR \
	 { if (!error()) \
		setError(ERR_ROLLBACK_OR_COMMIT_TRANSACTION, i18n("Begin transaction failed")); }

Transaction Connection::beginTransaction()
{
	if (!checkConnected())
		return Transaction();
	Transaction trans;
	if (m_driver->m_features & Driver::SingleTransactions) {
		if (d->m_default_trans.active()) {
			setError(ERR_TRANSACTION_ACTIVE, i18n("Transaction already started.") );
			return Transaction();
		}
		if (!(trans.m_data = drv_beginTransaction())) {
			SET_BEGIN_TR_ERROR;
			return Transaction();
		}
		d->m_default_trans = trans;
		d->m_transactions.append(trans);
		return trans;
	}
	else if (m_driver->m_features & Driver::MultipleTransactions) {
		if (!(trans.m_data = drv_beginTransaction())) {
			SET_BEGIN_TR_ERROR;
			return Transaction();
		}
		d->m_transactions.append(trans);
		return trans;
	}
	
	SET_ERR_TRANS_NOT_SUPP;
	return Transaction();
}

bool Connection::commitTransaction(const Transaction trans)
{
	if (!checkConnected())
		return false;
	if (!m_driver->transactionsSupported())
		SET_ERR_TRANS_NOT_SUPP;
	Transaction t = trans;
	if (!t.active()) { //try default tr.
		if (!d->m_default_trans.active()) {
			setError(ERR_NO_TRANSACTION_ACTIVE, i18n("Transaction not started.") );
			return false;
		}
		t = d->m_default_trans;
		d->m_default_trans = Transaction(); //now: no default tr.
	}
	bool ret = drv_commitTransaction(t.m_data);
	d->m_transactions.remove(t);
	if (!ret && !error())
		setError(ERR_ROLLBACK_OR_COMMIT_TRANSACTION, i18n("Error on commit transaction"));
	return ret;
}

bool Connection::rollbackTransaction(const Transaction trans)
{
	if (!checkConnected())
		return false;
	if (!m_driver->transactionsSupported())
		SET_ERR_TRANS_NOT_SUPP;
	Transaction t = trans;
	if (!t.active()) { //try default tr.
		if (d->m_default_trans.active()) {
			setError(ERR_NO_TRANSACTION_ACTIVE, i18n("Transaction not started.") );
			return false;
		}
		t = d->m_default_trans;
		d->m_default_trans = Transaction(); //now: no default tr.
	}
	bool ret = drv_rollbackTransaction(t.m_data);
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
	if (!checkConnected())
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

/*bool Connection::drv_duringTransaction()
{
	return m_transaction;
}*/

bool Connection::drv_setAutoCommit(bool on)
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

bool Connection::deleteCursor(Cursor *cursor)
{
	if (!cursor)
		return false;
	if (cursor->connection()!=this) //illegal call
		qWarning("Connection::deleteCursor(): Cannot delete the cursor not owned by the same connection!");
	bool ret = cursor->close();
	delete cursor;
	return ret;
}

bool Connection::setupObjectSchemaData( const KexiDB::RecordData &data, SchemaData &sdata )
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
	sdata.m_caption = data[3].toString();
	sdata.m_helpText = data[4].toString();
	
	KexiDBDbg<<"@@@ Connection::setupObjectSchemaData() == " << sdata.schemaDataDebugString() << endl;
	return true;
}

bool Connection::querySingleRecord(QString sql, KexiDB::RecordData &data)
{
	KexiDB::Cursor *cursor;
	if (!(cursor = executeQuery( sql )))
		return false;
	if (!cursor->moveFirst() || cursor->eof()) {
		deleteCursor(cursor);
		return false;
	}
	cursor->storeCurrentRecord(data);
	return deleteCursor(cursor);
}

KexiDB::TableSchema* Connection::setupTableSchema( const KexiDB::RecordData &data )//KexiDB::Cursor *table_cur )
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
	if (!(cursor = executeQuery( QString("select * from kexi__fields where t_id='%1' order by f_order").arg(t->m_id) )))
		return 0;
	if (!cursor->moveFirst()) {
		deleteCursor(cursor);
		return 0;
	}
	bool ok;
	while (!cursor->eof()) {
		KexiDBDbg<<"@@@ f_name=="<<cursor->value(2).asCString()<<endl;

		int f_type = cursor->value(1).toInt(&ok);
		if (!ok) { 
			deleteCursor(cursor);
			delete t;
			return 0;
		}
		int f_len = cursor->value(3).toInt(&ok);
		if (!ok) {
			deleteCursor(cursor);
			delete t;
			return 0;
		}
		int f_prec = cursor->value(4).toInt(&ok);
		if (!ok) {
			deleteCursor(cursor);
			delete t;
			return 0;
		}
		int f_constr = cursor->value(5).toInt(&ok);
		if (!ok) {
			deleteCursor(cursor);
			delete t;
			return 0;
		}
		int f_opts = cursor->value(6).toInt(&ok);
		if (!ok) {
			deleteCursor(cursor);
			delete t;
			return 0;
		}
		
		Field *f = new Field(
			cursor->value(2).asString(), (Field::Type)f_type, f_constr, f_len, f_prec, f_opts );
		f->setDefaultValue( cursor->value(7).toCString() );
		f->m_caption = cursor->value(9).asString();
		f->m_help = cursor->value(10).asString();
		t->addField(f);
		cursor->moveNext();
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

/*	KexiDB::Cursor *cursor;
	if (!(cursor = executeQuery( QString("select * from kexi__objects where o_name='%1'").arg(m_tableName) )))
		return false;
	if (!cursor->moveFirst() || cursor->eof()) {
		deleteCursor(cursor);
		return 0;
	}*/
	//not found: retrieve schema
	RecordData data;
	if (!querySingleRecord(QString("select * from kexi__objects where o_name='%1'").arg(m_tableName), data))
		return 0;
	
	return setupTableSchema(data);//cursor);
}

TableSchema* Connection::tableSchema( const int tableId )
{
	TableSchema *t = m_tables[tableId];
	if (t)
		return t;
	//not found: retrieve schema
	RecordData data;
	if (!querySingleRecord(QString("select * from kexi__objects where o_id='%1'").arg(tableId), data))
		return 0;
	
	return setupTableSchema(data);
}

QuerySchema* Connection::querySchema( const int queryId )
{
	QuerySchema *q = m_queries[queryId];
	if (q)
		return q;
	//not found: retrieve schema
	RecordData queryobject_data, querydata_data;

	if (!querySingleRecord(QString("select * from kexi__objects where o_id='%1'").arg(queryId), queryobject_data))
		return 0;

	q = new QuerySchema();
	if (!setupObjectSchemaData( queryobject_data, *q )) {
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

#include "connection.moc"
