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

#include "connection.h"
#include "error.h"
#include "connectiondata.h"
#include "driver.h"
#include "table.h"
#include "transaction.h"
#include "cursor.h"

#include <qfileinfo.h>

#include <klocale.h>
#include <kdebug.h>

#include <assert.h>

using namespace KexiDB;

Connection::Connection( Driver *driver, const ConnectionData &conn_data )
	: QObject()
	,KexiDB::Object()
	,m_driver(driver)
	,m_data(conn_data)
	,m_is_connected(false)
	,m_transaction(false)
{
	m_tables.setAutoDelete(true);
	m_cursors.setAutoDelete(true);
	//reasonable sizes
	m_tables.resize(101);
	m_cursors.resize(101);
}

void Connection::destroy()
{
	disconnect();
	//do not allow the driver to touch me: I will kill myself.
	m_driver->m_connections.take( this );
}

Connection::~Connection()
{
	qDebug("Connection::~Connection()");
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
	//delete own table schemas
	m_tables.clear();

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

bool Connection::databaseExists( const QString &dbName )
{
	if (!checkConnected())
		return false;
	clearError();
	if (m_driver->isFileDriver()) {
		QFileInfo file(dbName);
		return file.exists();
	}
	QStringList list = databaseNames();
	if (error()) {
		clearError();
		return false;
	}
	return list.find( dbName )!=list.end();
}

bool Connection::createDatabase( const QString &dbName )
{
	if (!checkConnected())
		return false;

	if (databaseExists( dbName )) {
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
	if (!drv_createDatabase( dbName ))
		return false;
	

	Transaction trans(this);
	if (error())
		return false;
	//create system tables
	KexiDB::Table t_tables("kexi__tables");
	t_tables.addField( new Field("t_id", Field::Integer, Field::PrimaryKey | Field::AutoInc, Field::Unsigned) );
	t_tables.addField( new  Field("t_name", Field::Text) );
	if (!drv_createTable( t_tables ))
		return false;

	KexiDB::Table t_fields("kexi__fields");
	t_fields.addField( new Field("t_id", Field::Integer, 0, Field::Unsigned) );
	t_fields.addField( new Field("f_type", Field::Byte, 0, Field::Unsigned) );
	t_fields.addField( new Field("f_name", Field::Text ) );
	t_fields.addField( new Field("f_length", Field::Integer ) );
	t_fields.addField( new Field("f_precision", Field::Integer ) );
	t_fields.addField( new Field("f_constraints", Field::Integer ) );
	t_fields.addField( new Field("f_options", Field::Integer ) );
	t_fields.addField( new Field("f_default", Field::Text ) );
//	t_fields.addField( Field("f_notnull", Field::Boolean ) );
//	t_fields.addField( Field("f_required", Field::Boolean ) );
//	t_fields.addField( Field("f_auto", Field::Boolean ) );
	//these are additional properties:
	t_fields.addField( new Field("f_order", Field::Integer ) );
	t_fields.addField( new Field("f_caption", Field::Text ) );
	t_fields.addField( new Field("f_help", Field::LongText ) );
	if (!drv_createTable( t_fields ))
		return false;

	commitTransaction();
	return true;
}

bool Connection::useDatabase( const QString &dbName )
{
	if (!checkConnected())
		return false;

	if (m_usedDatabase == dbName)
		return true; //already used

	if (m_driver->isFileDriver()) {
		//for file-based db: file must exists and be accessible
		QFileInfo file(dbName);
		if (!file.exists() || ( !file.isFile() && !file.isSymLink()) ) {
			setError(ERR_OBJECT_NOT_EXISTING, i18n("Database file '%1' does not exist.").arg(m_data.fileName()) );
			return false;
		}
		if (!file.isReadable()) {
			setError(ERR_ACCESS_RIGHTS, i18n("Database file '%1' is not readable.").arg(m_data.fileName()) );
			return false;
		}
		if (!file.isWritable()) {
			setError(ERR_ACCESS_RIGHTS, i18n("Database file '%1' is not writable.").arg(m_data.fileName()) );
			return false;
		}
		//update connection data if filename differs
		m_data.setFileName( dbName );
	}
	bool ok = drv_useDatabase( dbName );
	if (ok)
		m_usedDatabase = dbName;
	return ok;
}

bool Connection::closeDatabase()
{
	if (m_usedDatabase.isEmpty())
		return true; //no db used

	if (!checkConnected())
		return false;

	if (!drv_closeDatabase())
		return false;

	m_usedDatabase = "";
	kdDebug() << "Connection::closeDatabase() ok" << endl;
	return true;
}

bool Connection::dropDatabase( const QString &dbName )
{
	clearError();
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
	//TODO
	return QStringList();
}

QString Connection::valueToSQL( const Field::Type ftype, QVariant& v )
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
			return QString("\"")+s.replace( '"', "\\\"" ) + "\"";
		}case Field::BLOB:
//TODO: here special encoding method needed
			return QString("\"")+v.toString()+"\"";
		default:
			return QString::null;
	}
	return QString::null;
}

QString Connection::createTableStatement( const KexiDB::Table& table )
{
	QString sql = "CREATE TABLE " + table.name() + " (";
	bool first=true;
	Field::ListIterator it( table.m_fields );
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

bool Connection::drv_createTable( const KexiDB::Table& table )
{
	QString sql = createTableStatement(table);
	kdDebug()<<"******** "<<sql<<endl;
	return drv_executeSQL(sql);
}

bool Connection::beginTransaction()
{
	if (!(m_driver->m_features & Driver::Transactions))
		return true;
	if (m_transaction) {
		setError(ERR_TRANSACTION_ACTIVE, i18n("Transaction already started.") );
		return false;
	}
	if (drv_beginTransaction()) {
		m_transaction = true;
		return true;
	}
	return false;
}

bool Connection::commitTransaction()
{
	if (!(m_driver->m_features & Driver::Transactions))
		return true;
	if (!m_transaction) {
		setError(ERR_NO_TRANSACTION_ACTIVE, i18n("Transaction not started.") );
		return false;
	}
	bool ret = drv_commitTransaction();
	m_transaction = false;
	return ret;
}

bool Connection::rollbackTransaction()
{
	if (!(m_driver->m_features & Driver::Transactions))
		return true;
	if (!m_transaction) {
		setError(ERR_NO_TRANSACTION_ACTIVE, i18n("Transaction not started.") );
		return false;
	}
	bool ret = drv_rollbackTransaction();
	m_transaction = false;
	return ret;
}

bool Connection::duringTransaction()
{
	return drv_duringTransaction();
}

bool Connection::drv_beginTransaction()
{
	return drv_executeSQL( "BEGIN" );
}

bool Connection::drv_commitTransaction()
{
	return drv_executeSQL( "COMMIT" );
}

bool Connection::drv_rollbackTransaction()
{
	return drv_executeSQL( "ROLLBACK" );
}

bool Connection::drv_duringTransaction()
{
	return m_transaction;
}

Cursor* Connection::executeQuery( const QString& statement )
{
	if (statement.isEmpty())
		return 0;
	Cursor *c = prepareQuery( statement );
	if (!c)
		return 0;
	if (!c->open()) {//err - kill that
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

Table* Connection::tableSchema( const QString& tableName )
{
	Table *t = m_tables[tableName];
	if (t)
		return t;
	//not found: retrieve schema
	KexiDB::Cursor *cursor;
	if (!(cursor = executeQuery( QString("select * from kexi__tables where t_name='%1'").arg(tableName) )))
		return 0;
	if (!cursor->moveFirst()) {
		deleteCursor(cursor);
		return 0;
	}
	bool ok;
	int t_id = cursor->value(0).toInt(&ok);
	if (!ok) {
		deleteCursor(cursor);
		return 0;
	}
	t = new Table( tableName, this );
	t->m_id = t_id;
	kdDebug()<<"@@@ t_id=="<<t->m_id<<" t_name="<<cursor->value(1).asCString()<<endl;

	deleteCursor(cursor);

	if (!(cursor = executeQuery( QString("select * from kexi__fields where t_id='%1' order by f_order").arg(t->m_id) )))
		return 0;
	if (!cursor->moveFirst()) {
		deleteCursor(cursor);
		return 0;
	}
	while (!cursor->eof()) {
		kdDebug()<<"@@@ t_name=="<<cursor->value(2).asCString()<<endl;

		int f_type = cursor->value(1).toInt(&ok);
		if (!ok) { 
			deleteCursor(cursor);
			return 0;
		}
		int f_len = cursor->value(3).toInt(&ok);
		if (!ok) {
			deleteCursor(cursor);
			return 0;
		}
		int f_prec = cursor->value(4).toInt(&ok);
		if (!ok) {
			deleteCursor(cursor);
			return 0;
		}
		int f_constr = cursor->value(5).toInt(&ok);
		if (!ok) {
			deleteCursor(cursor);
			return 0;
		}
		int f_opts = cursor->value(6).toInt(&ok);
		if (!ok) {
			deleteCursor(cursor);
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

	m_tables.insert(tableName, t);
	return t;
}

#include "connection.moc"
