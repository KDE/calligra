/* This file is part of the KDE project
   Copyright (C) 2008 Julia Sanchez-Simon <hithwen@gmail.com>
   Copyright (C) 2008 Miguel Angel Aragüez-Rey <fizban87@gmail.com>

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
 Boston, MA 02110-1301, USA.
*/

#include "oracleconnection.h"
#include "oracledriver.h"
#include "oraclepreparedstatement.h"
#include <kgenericfactory.h>
#include <kdebug.h>
//#include <oci.h>
//#include <occi.h>

using namespace KexiDB;

OracleConnection::OracleConnection(Driver *driver, ConnectionData &conn_data)
	: Connection(driver,conn_data)
	, d(new OracleConnectionInternal(this))
	,active(false)
{
}

OracleConnection::~OracleConnection()
{
	destroy();
}

bool OracleConnection::drv_connect(ServerVersionInfo& version)
{
	QString versionString; // Version string from server

	// Connect (using d) and retrieve server version info
	const bool ok = d->db_connect(*data());
	if(!ok)
		return false;

	// Get server version info
	try{
	  versionString = d->getServerVersion();
	  QRegExp versionRe("(\\d+)\\.(\\d+)\\.(\\d+)");
	  version.major = versionRe.cap(1).toInt();
	  version.minor = versionRe.cap(2).toInt();
	  version.release = versionRe.cap(3).toInt();
	  return true;
	 }
	 catch ( ea)
	 {
	  KexiDBDrvDbg <<ea.getMessage().c_str();
	  return false;
	 }
}

bool OracleConnection::drv_disconnect()
{
  return d->db_disconnect();
}

//////////////////////////////////////////////////////////
// Down here is incomplete code, so it won't work (surely)


// TODO: Do we need this?
Cursor* OracleConnection::prepareQuery(const QString& statement, uint cursor_options)
{
	return new OracleCursor(this,statement,cursor_options);
}

// TODO: Do we need this?
Cursor* OracleConnection::prepareQuery( QuerySchema& query, uint cursor_options )
{
	return new OracleCursor( this, query, cursor_options );
}


// TODO: See below (this piece of code should be removed)

bool OracleConnection::drv_getDatabasesList( QStringList &list )
/*
 As databases doesnt exist in Oracle we should redefine the concept
 on purpose of cheating kexi.
 The only database wich can exist is the current user.
 That database exists if it contains kexi tables.
*/
{
	KexiDBDrvDbg <<endl;
	QString user;
	try{
		d->rs=d->stmt->executeQuery("SELECT user FROM DUAL");
		d->rs->next();
		user=QString(d->rs->getString(1).c_str());
		d->stmt->closeResultSet(d->rs);
		
		d->rs=d->stmt->executeQuery("select COUNT(*) from user_tables where table_name like \'KEXI__%\'");
	  d->rs->next();
	  
	  if (d->rs->getInt(1)>0) list.append(user);
		d->stmt->closeResultSet(d->rs);
		
		KexiDBDrvDbg <<list<<endl;
		return true;
	}
	catch ( ea)
  {
       //d->errno=ea.getErrorCode();
       KexiDBDrvDbg << "OracleConnection::drv_getDatabasesList error: "
       							<<ea.what()<<"\n";
       return(false);
  }
  
}

// TODO: Understand this
/**
 * Our server does not offer a list of databases (as just only one exists)
 * So, I am not sure about the semantics of this, should every database exist?
 * Or should not exist any at all?
 * Choices are: return true, return false or default to parent behaviour
 */
bool OracleConnection::drv_createDatabase( const QString &dbName) {
	KexiDBDrvDbg << dbName << endl;
	QString user;
	int res;
	try{
		d->rs=d->stmt->executeQuery("SELECT user FROM DUAL");
		if(d->rs->next()) user=QString(d->rs->getString(1).c_str());
		d->stmt->closeResultSet(d->rs);
		res=!user.compare(dbName);
    return res;
	}
	catch (ea)
	{
	  KexiDBDrvDbg << ea.getMessage().c_str()<< endl;
	  return(false);
	}
}

bool OracleConnection::drv_databaseExists( const QString &dbName, bool /*ignoreErrors*/ )
{
	QString user;
	int res;
	try{
	  if(active) return true;
	  
		d->rs=d->stmt->executeQuery("SELECT user FROM DUAL");
		if(d->rs->next()) user=QString(d->rs->getString(1).c_str());
		d->stmt->closeResultSet(d->rs);
		res=!user.compare(dbName);
	  
	  /*d->rs=d->stmt->executeQuery("select COUNT(*) from user_tables where table_name like \'KEXI__%\'");
	  d->rs->next();
	  res=res && (d->rs->getInt(1)>0);
	  d->stmt->closeResultSet(d->rs);*/
	  KexiDBDrvDbg << dbName <<":"<<res<< endl;
    return res;
	}
	catch ( ea)
	{
	  KexiDBDrvDbg << "OracleConnection::drv_dataBaseExists: "<< ea.what()<< endl;
	  return(false);
	}
}


// TODO: How should we use this?
/**
 * Since there is just one big database with many schemas,
 * this could be used for user switching
 * or even disconnecting and connecting to other servers.
 */
bool OracleConnection::drv_useDatabase(const QString &dbName,
																	 bool *cancelled, MessageHandler* msgHandler)
{
KexiDBDrvDbg <<endl;
	Q_UNUSED(cancelled);
	Q_UNUSED(msgHandler);
//TODO is here escaping needed?
	if( d->useDatabase(dbName))
	{
	  active=true;
	  return true;
	}
	return false;
}

/**
 * Closing means disconnecting
 * By now, this does nothing
 */
bool OracleConnection::drv_closeDatabase()
{
  KexiDBDrvDbg <<endl;
  active=false;
	return true;
}

/**
 * We cannot do that unless we mean users.
 */
bool OracleConnection::drv_dropDatabase( const QString& /*dbName*/)
/*
TODO
Drop the database acording our refeinition should be droping all KEXI__ tables
*/
{
	KexiDBDrvDbg <<"Oracle does not provide such functionality";
	return true;
}

TransactionData* OracleConnection::drv_beginTransaction()
{
	/*Extract from Oracle documentation:
	 As soon as you connect to the oralce database, a transaction begins.
	 Once the transaction begins, every SQL DML statement you issue subsequently 
	 becomes a part of this transaction (DDL are autocommited).
	 A transaction ends when you disconnect from the database,
	 or when you issue a COMMIT or ROLLBACK command. 
	*/
	/*
	 Autonomus transactions must be defined in PL/SQL blocks and 
	 has no link to the calling transaction,
	 so only commited data can be shared by both transactions.
	 */
	return new TransactionData(this);
}
bool OracleConnection::drv_setAutoCommit(bool on)
{
	try{
	  d->stmt->setAutoCommit(on);
	  return true;
	}catch ( ea){
	  KexiDBDrvDbg <<ea.getMessage().c_str();
	  return false;
	}
}
            
bool OracleConnection::drv_executeSQL( const QString& statement )
{
    return d->executeSQL(statement);
}

/**
 * RowID must be stored somewhere (read OracleConnectionInternal) or there must 
 * be a way to get it, else, return false
 */
Q_ULLONG OracleConnection::drv_lastInsertRowID()
{
  KexiDBDrvDbg <<endl;
	return false;
}

/**
 * Return result from server.
 */
int OracleConnection::serverResult()
{
	return d->errno;
}

/**
 * Returns an empty string.
 */
QString OracleConnection::serverResultName()
{
	return QString::null;
}

/**
 * Clears result from server.
 */
void OracleConnection::drv_clearServerResult()
{
	if (!d)
		return;
	d->errno = 0;
	d->errmsg="";
}

/**
 * Returns error message from server.
 */
QString OracleConnection::serverErrorMsg()
{
	return d->errmsg;
}

/**
 * Finds out if a given table exists
 */
bool OracleConnection::drv_containsTable( const QString &tableName )
{
  KexiDBDrvDbg<<endl;
	bool success;
	return resultExists(QString("SELECT TABLE_NAME FROM ALL_TABLES WHERE TABLE_NAME LIKE \'"+tableName.upper()+"\'")
		.arg(driver()->escapeString(tableName)), success) && success;
}

/**
 * Gets a list with all tables
 */
bool OracleConnection::drv_getTablesList( QStringList &list )
{
  KexiDBDrvDbg<<endl;
	KexiDB::Cursor *cursor;
	if (!(cursor = executeQuery( "SELECT TABLE_NAME FROM ALL_TABLES" ))) {
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

/**
 * What is this?
 */
PreparedStatement::Ptr OracleConnection::prepareStatement(PreparedStatement::StatementType type,
	FieldList& fields)
{
    KexiDBDrvDbg<<endl;
		return KSharedPtr<PreparedStatement>(new OraclePreparedStatement(type, *d, fields) );
	//return KSharedPtr<PreparedStatement>( new OraclePreparedStatement(type, *d, fields) );
}
