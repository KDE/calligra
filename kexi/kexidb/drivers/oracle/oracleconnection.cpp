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
#include <string>
//#include <oci.h>
//#include <occi.h>

using namespace KexiDB;
//using namespace oracle::occi;
using namespace std;

OracleConnection::OracleConnection(Driver *driver, ConnectionData &conn_data)
	: Connection(driver,conn_data)
	, d(new OracleConnectionInternal(this))
	, active(false)
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
	 catch ( &ea)
	 {
	  KexiDBDrvDbg <<ea.what();
	  return false;
	 }
}

bool OracleConnection::drv_disconnect()
{
  return d->db_disconnect();
}

// TODO: Do we need this?
Cursor* OracleConnection::prepareQuery
                                 (const QString& statement, uint cursor_options)
{
	return new OracleCursor(this,statement,cursor_options);
}

// TODO: Do we need this?
Cursor* OracleConnection::prepareQuery(QuerySchema& query, uint cursor_options )
{
	return new OracleCursor( this, query, cursor_options );
}

/*
 As databases doesnt exist in Oracle we should redefine the concept
 on purpose of cheating kexi.
 The only database wich can exist is the current user.
 That database exists if it contains kexi tables.
*/
bool OracleConnection::drv_getDatabasesList( QStringList &list )
{
	KexiDBDrvDbg <<endl;
	QString user;
	try{
		d->rs=d->stmt->executeQuery("SELECT user FROM DUAL");
		d->rs->next();
		user=QString(d->rs->getString(1).c_str());
		d->stmt->closeResultSet(d->rs);
		d->rs=0;
		
		d->rs=d->stmt->executeQuery
		     ("select COUNT(*) from user_tables where table_name like \'KEXI__%\'");
	  d->rs->next();
	  
	  if (d->rs->getInt(1)>0) list.append(user);
		d->stmt->closeResultSet(d->rs);
		d->rs=0;
		
		//KexiDBDrvDbg <<list<<endl;
		return true;
	}
	catch (&ea)
  {
       //d->errno=ea.getErrorCode();
       KexiDBDrvDbg << "error: "<<ea.what()<<"\n";
       return(false);
  }
  
}

// TODO: Understand this
/**
 * Oracle doesnt offer a list of db (the equivalent would be user schemas)
 * So,it returns true if dbName is the current Schema
 */
bool OracleConnection::drv_createDatabase( const QString &dbName) {
	KexiDBDrvDbg << dbName << endl;
	QString user;
	int res;

	try
	{
		d->rs=d->stmt->executeQuery("SELECT user FROM DUAL");
		if(d->rs->next()) user=QString(d->rs->getString(1).c_str());
		d->stmt->closeResultSet(d->rs);
		d->rs=0;
		res=!user.compare(dbName);
		if(res)
		{
		  //d->stmt->execute
		  //  ("CREATE TABLE KEXI__AUX (ID NUMBER PRIMARY KEY,CREATION TIMESTAMP)");
		  d->createSequences();  
		}
    return res;
	}
	catch (&ea)
	{
	  KexiDBDrvDbg << ea.getMessage().c_str()<< endl;
	  return(false);
	}
}

bool OracleConnection::drv_databaseExists
                                 ( const QString &dbName, bool /*ignoreErrors*/)
{
	QString user;
	int res;
	try
	{
	  if(active) return true;
	  
		d->rs=d->stmt->executeQuery("SELECT user FROM DUAL");
		if(d->rs->next()) user=QString(d->rs->getString(1).c_str());
		d->stmt->closeResultSet(d->rs);
		d->rs=0;
		res=!user.compare(dbName);
	  //KexiDBDrvDbg << dbName <<":"<<res<< endl;
    return res;
	}
	catch ( &ea)
	{
	  KexiDBDrvDbg << ea.what()<< endl;
	  return(false);
	}
}

/*
Checks if we have all the needed triggers. This is not checked in 
drv_createdatabase because createdatabase from connection calls the drv before
creating the tables of the project.
*/
bool OracleConnection::drv_useDatabase(const QString &dbName,
																	 bool *cancelled, MessageHandler* msgHandler)
{
  KexiDBDrvDbg <<endl;
	Q_UNUSED(cancelled);
	Q_UNUSED(msgHandler);
	bool createtg;
//TODO is here escaping needed?
	if( d->useDatabase(dbName))
	{
	  try
	  {
	    //KexiDBDrvDbg<<"Counting triggers"<<endl; 
	    d->rs=d->stmt->executeQuery
	   ("SELECT COUNT(*) FROM USER_OBJECTS WHERE OBJECT_NAME LIKE 'KEXI__TG__%'");
	    createtg=!d->rs->next() || d->rs->getInt(1)<(SYSTABLES +1);
	    d->stmt->closeResultSet(d->rs);
	    d->rs=0;
	    
	    /*d->rs=d->stmt->executeQuery
	    ("SELECT COUNT(*) FROM USER_OBJECTS WHERE OBJECT_NAME LIKE 'KEXI__SEQ%'");
	    createsq=!d->rs->next() || d->rs->getInt(1)<SYSTABLES;
	    d->stmt->closeResultSet(d->rs);
	    d->rs=0;
	    */
	  }
	  catch(&ea)
	  {
	    KexiDBDrvDbg <<ea.what()<<endl;
	    return false;
	  }
	  //if(createsq)  d->createSequences();
	  if(createtg){
	    d->createTriggers();
	  }

	  active=true;
	  return true;
	}
	return false;
}

/**
 * As there are no databases inside an oracle database you cannot close it 
 * (aka: close the session) without disconnecting.
 */
bool OracleConnection::drv_closeDatabase()
{
  KexiDBDrvDbg <<endl;
  active=false;
	return true;
}

/*
Drops the database acording our refeinition is droping all KEXI__ tables,
secuences and triggers
*/
bool OracleConnection::drv_dropDatabase( const QString& /*dbName*/)
{
	KexiDBDrvDbg <<endl;
	string dropkexi ="DECLARE\n";
	string droptg="DECLARE\n";
	string dropsq="DECLARE\n";
	dropkexi=dropkexi+"CURSOR C_KEXI IS SELECT * FROM USER_TABLES\n"              
	                 +"WHERE TABLE_NAME LIKE 'KEXI__%';\n"                        
	                 +"BEGIN\n"                                                   
	                 +"FOR V_KEXI IN C_KEXI LOOP\n"                               
	                 +"EXECUTE IMMEDIATE 'DROP TABLE ' || V_KEXI.TABLE_NAME;\n"   
                   +"END LOOP;\n"                                                   
                   +"END;\n"; 
                   
  droptg=droptg    +"CURSOR C_KEXI IS SELECT * FROM USER_OBJECTS\n"              
	                 +"WHERE OBJECT_NAME LIKE 'KEXI__TG__%';\n"                        
	                 +"BEGIN\n"                                                   
	                 +"FOR V_KEXI IN C_KEXI LOOP\n"                               
	                 +"EXECUTE IMMEDIATE 'DROP TRIGGER ' || V_KEXI.OBJECT_NAME;\n"   
                   +"END LOOP;\n"                                                   
                   +"END;\n";   
                   
  dropsq=dropsq   +"CURSOR C_KEXI IS SELECT * FROM USER_OBJECTS\n"              
	                +"WHERE OBJECT_NAME LIKE 'KEXI__SEQ%';\n"                        
	                +"BEGIN\n"                                                   
	                +"FOR V_KEXI IN C_KEXI LOOP\n"                               
	                +"EXECUTE IMMEDIATE 'DROP SEQUENCE ' || V_KEXI.OBJECT_NAME;\n"   
                  +"END LOOP;\n"                                                   
                  +"END;\n";                   
	try{
		d->stmt->execute(dropkexi);
		d->stmt->execute(droptg);
		d->stmt->execute(dropsq);
	  return true;
	}
	catch (&ea)
	{
	  KexiDBDrvDbg <<ea.what()<<endl;
	  return false;
	}
}
/*
  Extract from Oracle documentation:
	As soon as you connect to the oralce database, a transaction begins.
	Once the transaction begins, every SQL DML statement you issue subsequently 
	becomes a part of this transaction (DDL are autocommited).
	A transaction ends when you disconnect from the database,
	or when you issue a COMMIT or ROLLBACK command. 

	Autonomus transactions must be defined in PL/SQL blocks and 
	has no link to the calling transaction,
	so only commited data can be shared by both transactions.
*/
TransactionData* OracleConnection::drv_beginTransaction()
{
	return new TransactionData(this);
}
bool OracleConnection::drv_setAutoCommit(bool on)
{
	try{
	  d->stmt->setAutoCommit(on);
	  KexiDBDrvDbg <<":true"<<endl;
	  return true;
	}catch ( &ea){
	  KexiDBDrvDbg <<ea.getMessage().c_str();
	  KexiDBDrvDbg <<":false"<<endl;
	  return false;
	}
}
            
bool OracleConnection::drv_executeSQL( const QString& statement )
{
    return d->executeSQL(statement);
}

bool OracleConnection::drv_createTable( const TableSchema& tableSchema )
{
	m_sql = createTableStatement(tableSchema);
	KexiDBDbg<<"******** "<<m_sql<<endl;
  //KexiDBDrvDbg << tableSchema.name() << endl;
	return (executeSQL(m_sql) &&
	        executeSQL("ALTER TABLE "+tableSchema.name()+" ADD ROW_ID NUMBER"));
}

bool OracleConnection::drv_afterInsert(const QString& table, FieldList& fields)
{
   //Q_UNUSED(table);
   Q_UNUSED(fields);
   /*Field* row_id=new Field("ROW_ID", 
                            Field::BigInteger,
                            Field::Unique,
                            Field::Unsigned,
                            0,
                            0,
                            "KEXI__SEQ__ROW_ID.NEXTVAL",
                            QString(),
                            QString(),
                            0
                          );
   fields.insertField(0, row_id);
   */
   QString stat=QString("UPDATE "+table
             +" SET ROW_ID=KEXI__SEQ__ROW_ID.NEXTVAL WHERE ROW_ID IS NULL");
             
   return (d->executeSQL(stat)/*&&d->executeSQL("COMMIT")*/);

}

/**
 * RowID in Oracle is not a number, is an alphanumeric string
 */
Q_ULLONG OracleConnection::drv_lastInsertRowID()
{
  KexiDBDrvDbg << endl;
  int res;
  try
  {
    d->rs=d->stmt->executeQuery
    ("SELECT LAST_NUMBER-1 from user_sequences WHERE SEQUENCE_NAME='KEXI__SEQ__ROW_ID'");
    if(d->rs->next()) res=d->rs->getInt(1);
    d->stmt->closeResultSet(d->rs);
    d->rs=0;
    return res;
    
  }
  catch(&ea)
  {
    KexiDBDrvDbg<<ea.what()<<endl;
  }
  return -1;
}


int OracleConnection::serverResult()
{
	return d->errno;
}

QString OracleConnection::serverResultName()
{
	return QString::null;
}

void OracleConnection::drv_clearServerResult()
{
	if (!d) return;
	d->errno = 0;
	d->errmsg="";
}

QString OracleConnection::serverErrorMsg()
{
	return d->errmsg;
}

/*
 * Finds out if a given table exists
 */
bool OracleConnection::drv_containsTable( const QString &tableName )
{
  KexiDBDrvDbg<<endl;
	bool success;
	return resultExists(QString("SELECT TABLE_NAME FROM ALL_TABLES WHERE TABLE_NAME LIKE %1")
		.arg(driver()->escapeString(tableName).upper()), success) && success;
}

/**
 * Gets a list with all tables
 */
bool OracleConnection::drv_getTablesList( QStringList &list )
{
  KexiDBDrvDbg<<endl;
	KexiDB::Cursor *cursor;
	if (!(cursor = executeQuery( "SELECT TABLE_NAME FROM ALL_TABLES" ))) 
	{
	  return false;
  }
	list.clear();
	cursor->moveFirst();
	
	while (!cursor->eof() && !cursor->error()) 
	{
		list += cursor->value(0).toString();
		cursor->moveNext();
	}
	if (cursor->error()) 
	{
		deleteCursor(cursor);
		return false;
	}
	return deleteCursor(cursor);
}

/**
 * What is this?
 */
PreparedStatement::Ptr OracleConnection::prepareStatement
                      (PreparedStatement::StatementType type, FieldList& fields)
{
    KexiDBDrvDbg<<endl;
		return KSharedPtr<PreparedStatement>
		                              (new OraclePreparedStatement(type,*d,fields));
}

