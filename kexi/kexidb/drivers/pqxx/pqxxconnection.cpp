/* This file is part of the KDE project
   Copyright (C) 2003 Adam Pigg <adam@piggz.co.uk>

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

#include "pqxxconnection.h"
#include <qvariant.h>
#include <qfile.h>
#include <kdebug.h>
#include <kexidb/error.h>
#include <kexidb/global.h>
#include <klocale.h>
#include <string>
#include "pqxxpreparedstatement.h"
#include "pqxxconnection_p.h"
using namespace KexiDB;

pqxxTransactionData::pqxxTransactionData(Connection *conn, bool nontransaction)
 : TransactionData(conn)
{
	if (nontransaction)
		data = new pqxx::nontransaction(*static_cast<pqxxSqlConnection*>(conn)->d->pqxxsql /* todo: add name? */);
	else
		data = new pqxx::transaction<>(*static_cast<pqxxSqlConnection*>(conn)->d->pqxxsql /* todo: add name? */);
	if (!static_cast<pqxxSqlConnection*>(conn)->m_trans) {
		static_cast<pqxxSqlConnection*>(conn)->m_trans = this;
	}
}

pqxxTransactionData::~pqxxTransactionData()
{
	if (static_cast<pqxxSqlConnection*>(m_conn)->m_trans == this) {
		static_cast<pqxxSqlConnection*>(m_conn)->m_trans = 0;
	}
	delete data;
	data = 0;
}

//==================================================================================

pqxxSqlConnection::pqxxSqlConnection(Driver *driver, ConnectionData &conn_data)
 : Connection(driver,conn_data)
 , d( new pqxxSqlConnectionInternal(this) )
 , m_trans(0)
{
}

//==================================================================================
//Do any tidying up before the object is deleted
pqxxSqlConnection::~pqxxSqlConnection()
{
	//delete m_trans;
	destroy();
	delete d;
}

//==================================================================================
//Return a new query based on a query statment
Cursor* pqxxSqlConnection::prepareQuery( const QString& statement,  uint cursor_options)
{
	Q_UNUSED(cursor_options);
	return new pqxxSqlCursor(this, statement, 1); //Always used buffered cursor
}

//==================================================================================
//Return a new query based on a query object
Cursor* pqxxSqlConnection::prepareQuery( QuerySchema& query, uint cursor_options)
{
	Q_UNUSED(cursor_options);
	return new pqxxSqlCursor(this, query, 1);//Always used buffered cursor
}

//==================================================================================
//Properly escaped a database object name
QString pqxxSqlConnection::escapeName(const QString &name) const
{
	return QString("\"" + name + "\"");
}

//==================================================================================
//Made this a noop
//We tell kexi we are connected, but we wont actually connect until we use a database!
bool pqxxSqlConnection::drv_connect(KexiDB::ServerVersionInfo& version)
{
	KexiDBDrvDbg << "pqxxSqlConnection::drv_connect" << endl;
	version.clear();
	d->version = &version; //remember for later...
#ifdef __GNUC__
#warning pqxxSqlConnection::drv_connect implement setting version info when we drop libpqxx for libpq
#endif
	return true;
}

//==================================================================================
//Made this a noop
//We tell kexi wehave disconnected, but it is actually handled by closeDatabse
bool pqxxSqlConnection::drv_disconnect()
{
	KexiDBDrvDbg << "pqxxSqlConnection::drv_disconnect: " << endl;
	return true;
}

//==================================================================================
//Return a list of database names
bool pqxxSqlConnection::drv_getDatabasesList( QStringList &list )
{
//	KexiDBDrvDbg << "pqxxSqlConnection::drv_getDatabaseList" << endl;

	if (executeSQL("SELECT datname FROM pg_database WHERE datallowconn = TRUE"))
	{
		std::string N;
		for (pqxx::result::const_iterator c = d->res->begin(); c != d->res->end(); ++c)
		{
			// Read value of column 0 into a string N
			c[0].to(N);
			// Copy the result into the return list
			list << QString::fromLatin1 (N.c_str());
		}
		return true;
	}

	return false;
}

//==================================================================================
//Create a new database
bool pqxxSqlConnection::drv_createDatabase( const QString &dbName )
{
	KexiDBDrvDbg << "pqxxSqlConnection::drv_createDatabase: " << dbName << endl;

	if (executeSQL("CREATE DATABASE " + escapeName(dbName)))
		return true;

	return false;
}

//==================================================================================
//Use this as our connection instead of connect
bool pqxxSqlConnection::drv_useDatabase( const QString &dbName, bool *cancelled, 
	MessageHandler* msgHandler )
{
	Q_UNUSED(cancelled);
	Q_UNUSED(msgHandler);
	KexiDBDrvDbg << "pqxxSqlConnection::drv_useDatabase: " << dbName << endl;

	QString conninfo;
	QString socket;
	QStringList sockets;

	if (data()->hostName.isEmpty() || data()->hostName == "localhost")
	{
		if (data()->localSocketFileName.isEmpty())
		{
			sockets.append("/tmp/.s.PGSQL.5432");

			for(QStringList::ConstIterator it = sockets.constBegin(); it != sockets.constEnd(); it++)
			{
				if(QFile(*it).exists())
				{
					socket = (*it);
					break;
				}
			}
		}
		else
		{
			socket=data()->localSocketFileName; //data()->fileName();
		}
	}
	else
	{
		conninfo = "host='" + data()->hostName + "'";
	}

	//Build up the connection string
	if (data()->port == 0)
		data()->port = 5432;

	conninfo += QString::fromLatin1(" port='%1'").arg(data()->port);

	conninfo += QString::fromLatin1(" dbname='%1'").arg(dbName);

	if (!data()->userName.isNull())
		conninfo += QString::fromLatin1(" user='%1'").arg(data()->userName);

	if (!data()->password.isNull())
		conninfo += QString::fromLatin1(" password='%1'").arg(data()->password);

	try
	{
		d->pqxxsql = new pqxx::connection( conninfo.latin1() );
		drv_executeSQL( "SET DEFAULT_WITH_OIDS TO ON" ); //Postgres 8.1 changed the default to no oids but we need them

		if (d->version) {
//! @todo set version using the connection pointer when we drop libpqxx for libpq
		}
		return true;
	}
	catch(const std::exception &e)
	{
		KexiDBDrvDbg << "pqxxSqlConnection::drv_useDatabase:exception - " << e.what() << endl;
		d->errmsg = QString::fromUtf8( e.what() );

	}
	catch(...)
	{
		d->errmsg = i18n("Unknown error.");
	}
	return false;
}

//==================================================================================
//Here we close the database connection
bool pqxxSqlConnection::drv_closeDatabase()
{
	KexiDBDrvDbg << "pqxxSqlConnection::drv_closeDatabase" << endl;
//	if (isConnected())
//	{
	delete d->pqxxsql;
	return true;
//	}
/* js: not needed, right? 
	else
	{
		d->errmsg = "Not connected to database backend";
		d->res = ERR_NO_CONNECTION;
	}
	return false;*/
}

//==================================================================================
//Drops the given database
bool pqxxSqlConnection::drv_dropDatabase( const QString &dbName )
{
	KexiDBDrvDbg << "pqxxSqlConnection::drv_dropDatabase: " << dbName << endl;

	//FIXME Maybe should check that dbname is no the currentdb
	if (executeSQL("DROP DATABASE " + escapeName(dbName)))
		return true;

	return false;
}

//==================================================================================
//Execute an SQL statement
bool pqxxSqlConnection::drv_executeSQL( const QString& statement )
{
//	KexiDBDrvDbg << "pqxxSqlConnection::drv_executeSQL: " << statement << endl;
	bool ok = false;

	// Clear the last result information...
	delete d->res;
	d->res = 0;

//	KexiDBDrvDbg << "About to try" << endl;
	try
	{
		//Create a transaction
		const bool implicityStarted = !m_trans;
		if (implicityStarted)
			(void)new pqxxTransactionData(this, true);

		//		m_trans = new pqxx::nontransaction(*m_pqxxsql);
//		KexiDBDrvDbg << "About to execute" << endl;
		//Create a result object through the transaction
		d->res = new pqxx::result(m_trans->data->exec(std::string(statement.utf8())));
//		KexiDBDrvDbg << "Executed" << endl;
		//Commit the transaction
		if (implicityStarted) {
			pqxxTransactionData *t = m_trans;
			drv_commitTransaction(t);
			delete t;
//			m_trans = 0;
		}

		//If all went well then return true, errors picked up by the catch block
		ok = true;
	}
	catch(const pqxx::sql_error& sqlerr) {
		KexiDBDrvDbg << "pqxxSqlConnection::drv_executeSQL: sql_error exception - " << sqlerr.query().c_str() << endl;
	}
	catch (const pqxx::broken_connection& bcerr) {
		KexiDBDrvDbg << "pqxxSqlConnection::drv_executeSQL: broken_connection exception" << endl;
	}
	catch (const std::exception &e)
	{
		//If an error ocurred then put the error description into _dbError
		d->errmsg = QString::fromUtf8( e.what() );
		KexiDBDrvDbg << "pqxxSqlConnection::drv_executeSQL:exception - " << e.what() << endl;
	}
	catch(...)
	{
		d->errmsg = i18n("Unknown error.");
	}
	//KexiDBDrvDbg << "EXECUTE SQL OK: OID was " << (d->res ? d->res->inserted_oid() : 0) << endl;
	return ok;
}

//==================================================================================
//Return true if currently connected to a database, ignoring the m_is_connected falg.
bool pqxxSqlConnection::drv_isDatabaseUsed() const
{
	if (d->pqxxsql->is_open())
	{
		return true;
	}
	return false;
}

//==================================================================================
//Return the oid of the last insert - only works if sql was insert of 1 row
Q_ULLONG pqxxSqlConnection::drv_lastInsertRowID()
{
	if (d->res)
	{
		pqxx::oid theOid = d->res->inserted_oid();

		if (theOid != pqxx::oid_none)
		{
			return (Q_ULLONG)theOid;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

//<queries taken from pqxxMigrate>
bool pqxxSqlConnection::drv_containsTable( const QString &tableName )
{
	bool success;
	return resultExists(QString("select 1 from pg_class where relkind='r' and relname LIKE %1")
		.arg(driver()->escapeString(tableName)), success) && success;
}

bool pqxxSqlConnection::drv_getTablesList( QStringList &list )
{
	KexiDB::Cursor *cursor;
	m_sql = "select lower(relname) from pg_class where relkind='r'";
	if (!(cursor = executeQuery( m_sql ))) {
		KexiDBDrvWarn << "pqxxSqlConnection::drv_getTablesList(): !executeQuery()" << endl;
		return false;
	}
	list.clear();
	cursor->moveFirst();
	while (!cursor->eof() && !cursor->error()) {
		list += cursor->value(0).toString();
		cursor->moveNext();
	}
	if (cursor->error()) {
		deleteCursor(cursor);
		return false;
	}
	return deleteCursor(cursor);
}
//</taken from pqxxMigrate>

TransactionData* pqxxSqlConnection::drv_beginTransaction()
{
	return new pqxxTransactionData(this, false);
}

bool pqxxSqlConnection::drv_commitTransaction(TransactionData *tdata)
{
	bool result = true;
	try {
		static_cast<pqxxTransactionData*>(tdata)->data->commit();
	}
	catch (const std::exception &e)
	{
		//If an error ocurred then put the error description into _dbError
		d->errmsg = QString::fromUtf8( e.what() );
		result = false;
	}
	catch (...) {
		//! @todo
		setError();
		result = false;
	}
	if (m_trans == tdata)
		m_trans = 0;
	return result;
}

bool pqxxSqlConnection::drv_rollbackTransaction(TransactionData *tdata)
{
	bool result = true;
	try {
		static_cast<pqxxTransactionData*>(tdata)->data->abort();
	}
	catch (const std::exception &e)
	{
		//If an error ocurred then put the error description into _dbError
		d->errmsg = QString::fromUtf8( e.what() );
		
		result = false;
	}
	catch (...) {
		d->errmsg = i18n("Unknown error.");
		result = false;
	}
	if (m_trans == tdata)
		m_trans = 0;
	return result;
}

int pqxxSqlConnection::serverResult()
{
	return d->resultCode;
}

QString pqxxSqlConnection::serverResultName()
{
	return QString::null;
}

void pqxxSqlConnection::drv_clearServerResult()
{
	d->resultCode = 0;
}

QString pqxxSqlConnection::serverErrorMsg()
{
	return d->errmsg;
}

PreparedStatement::Ptr pqxxSqlConnection::prepareStatement(PreparedStatement::StatementType type, 
	FieldList& fields)
{
	return new pqxxPreparedStatement(type, *d, fields);
}
#include "pqxxconnection.moc"
