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

using namespace KexiDB;
//using namespace std;

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
	try
	{
	  versionString = d->getServerVersion();
	  QRegExp versionRe("(\\d+)\\.(\\d+)\\.(\\d+)");
	  version.major = versionRe.cap(1).toInt();
	  version.minor = versionRe.cap(2).toInt();
	  version.release = versionRe.cap(3).toInt();
	  return true;
	}
	catch (oracle::occi::SQLException ea)
	{
	  KexiDBDrvDbg <<ea.what();
	  d->errmsg=ea.what();
	  d->errno=ea.getErrorCode();
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
 As databases does not exist in Oracle we should redefine the concept
 on purpose of cheating kexi.
 The only database which can exist is the current user.
 That database exists if it contains kexi tables.
*/
bool OracleConnection::drv_getDatabasesList( QStringList &list )
{
	KexiDBDrvDbg;
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
		
		return true;
	}
	catch (oracle::occi::SQLException ea)
  {
       KexiDBDrvDbg <<ea.what()<<"\n";
       d->errmsg=ea.what();
       d->errno=ea.getErrorCode();
       return false;
  }
  
}

/*
 * Oracle does not offer a list of db (the equivalent would be user schemas)
 * So, it returns true if dbName is the current Schema
 */
bool OracleConnection::drv_createDatabase( const QString &dbName) {
	KexiDBDrvDbg << dbName;
	QString user;
  bool res;
	try
	{
		d->rs=d->stmt->executeQuery("SELECT user FROM DUAL");
		if(d->rs->next()) user=QString(d->rs->getString(1).c_str());
		d->stmt->closeResultSet(d->rs);
		d->rs=0;
		res=(!user.compare(dbName));
		if (res) d->createSequences();
		return res;
	}
	catch (oracle::occi::SQLException ea)
	{
	  KexiDBDrvDbg << ea.what();
	  d->errmsg=ea.what();
    d->errno=ea.getErrorCode();
	  return false;
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
	  //KexiDBDrvDbg << dbName <<":"<<res;
    return res;
	}
	catch ( oracle::occi::SQLException ea)
	{
	  KexiDBDrvDbg << ea.what();
	  d->errmsg=ea.what();
    d->errno=ea.getErrorCode();
	  return(false);
	}
}

bool OracleConnection::drv_useDatabase(const QString &dbName,
																	 bool *cancelled, MessageHandler* msgHandler)
{
  KexiDBDrvDbg;
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

/*
 * As there are no databases inside an oracle database you cannot close it 
 * (aka: close the session) without disconnecting.
 */
bool OracleConnection::drv_closeDatabase()
{
  KexiDBDrvDbg;
  active=false;
	return true;
}

/*
 * Drops the database according our refeinition is droping all KEXI__ tables,
 * secuences and triggers and tables created inside kexi
 */
bool OracleConnection::drv_dropDatabase( const QString& /*dbName*/)
{
	//KexiDBDrvDbg;
	QString drop[4];
	drop[0]= "DECLARE\n";
	drop[1]= "DECLARE\n";
	drop[2]= "DECLARE\n";
	drop[3]= "DECLARE\n";
	
	drop[0]=drop[0]  +"CURSOR C_KEXI IS SELECT * FROM KEXI__OBJECTS\n"              
	                 +"WHERE O_TYPE=1;\n"                        
	                 +"BEGIN\n"                                                   
	                 +"FOR V_KEXI IN C_KEXI LOOP\n"                               
	                 +"EXECUTE IMMEDIATE 'DROP TABLE ' || V_KEXI.O_NAME;\n"   
                   +"END LOOP;\n"                                                   
                   +"END;\n"; 
                   
	drop[1]=drop[1]  +"CURSOR C_KEXI IS SELECT * FROM USER_TABLES\n"              
	                 +"WHERE TABLE_NAME LIKE 'KEXI__%';\n"                        
	                 +"BEGIN\n"                                                   
	                 +"FOR V_KEXI IN C_KEXI LOOP\n"                               
	                 +"EXECUTE IMMEDIATE 'DROP TABLE ' || V_KEXI.TABLE_NAME;\n"   
                   +"END LOOP;\n"                                                   
                   +"END;\n"; 
                   
  drop[2]=drop[2]  +"CURSOR C_KEXI IS SELECT * FROM USER_OBJECTS\n"              
	                 +"WHERE OBJECT_NAME LIKE 'KEXI__TG__%';\n"                        
	                 +"BEGIN\n"                                                   
	                 +"FOR V_KEXI IN C_KEXI LOOP\n"                               
	                 +"EXECUTE IMMEDIATE 'DROP TRIGGER ' || V_KEXI.OBJECT_NAME;\n"   
                   +"END LOOP;\n"                                                   
                   +"END;\n";   
                   
  drop[3]=drop[3] +"CURSOR C_KEXI IS SELECT * FROM USER_OBJECTS\n"              
	                +"WHERE OBJECT_NAME LIKE 'KEXI__SEQ%';\n"                        
	                +"BEGIN\n"                                                   
	                +"FOR V_KEXI IN C_KEXI LOOP\n"                               
	                +"EXECUTE IMMEDIATE 'DROP SEQUENCE ' || V_KEXI.OBJECT_NAME;\n"   
                  +"END LOOP;\n"                                                   
                  +"END;\n";                                              
 for(int i=0; i<4; i++)
 {                               
    try
    {
	    d->stmt->execute(drop[i].latin1());
	  }
	  catch (oracle::occi::SQLException ea)
	  {
	    KexiDBDrvDbg <<ea.what();
	    d->errmsg=ea.what();
      d->errno=ea.getErrorCode();
      if(!d->errno==6550 && !d->errno==942) return false;//nothing to be dropped
  	}
 }
 return true;
}
/*
  Extract from Oracle documentation:
	As soon as you connect to the oralce database, a transaction begins.
	Once the transaction begins, every SQL DML statement you issue subsequently 
	becomes a part of this transaction (DDL are autocommitted).
	A transaction ends when you disconnect from the database,
	or when you issue a COMMIT or ROLLBACK command. 

	Autonomus transactions must be defined in PL/SQL blocks and 
	has no link to the calling transaction,
	so only committed data can be shared by both transactions.
*/
TransactionData* OracleConnection::drv_beginTransaction()
{
	return new TransactionData(this);
}
bool OracleConnection::drv_setAutoCommit(bool on)
{
	try{
	  d->stmt->setAutoCommit(on);
	  KexiDBDrvDbg <<":true";
	  return true;
	}catch ( oracle::occi::SQLException ea){
	  KexiDBDrvDbg <<ea.what();
	  d->errmsg=ea.what();
    d->errno=ea.getErrorCode();
	  return false;
	}
}
            
bool OracleConnection::drv_executeSQL( const QString& statement )
{
  return d->executeSQL(statement);
}

bool OracleConnection::drv_createTable( const KexiDB::TableSchema& tableSchema )
{
  QString tableName = tableSchema.name();
  KexiDBDrvDbg << tableName;
  
  IndexSchema *ind; // Used to ask for the primary key
  QString sequenceName;
	m_sql = createTableStatement(tableSchema);
	//KexiDBDrvDbg <<m_sql;
 
	if(d->executeSQL(m_sql))
	{
	  ind = tableSchema.primaryKey();
    //KexiDBDrvDbg << "Number of fields in table: " << tableSchema.fieldCount();
    //KexiDBDrvDbg << "Number of fields in pk: " << ind->fieldCount();
    // TODO: is it ok to check this way? Would it be better using fieldCount == 0 ?
    if(ind->isPrimaryKey())
    {
      sequenceName = "KEXI__SEQ__" + tableName;
      // Add custom sequence and trigger to manage it
      if(!d->executeSQL("CREATE SEQUENCE "+sequenceName) ||
                                               !d->createTrigger(tableName,ind))
      {
        return false;
      }
    }
    return d->executeSQL("ALTER TABLE " + tableName + " ADD ROW_ID NUMBER");
  }
  return false;
}
/* 
 * Here is we do the count of ROW_IDs
 */
bool OracleConnection::drv_afterInsert(const QString& table, FieldList& fields)
{
  Q_UNUSED(table);
  Q_UNUSED(fields);
  //KexiDBDrvDbg << "Updating ROW_ID on " << table;
  QString stat=QString("UPDATE "+table
            +" SET ROW_ID=KEXI__SEQ__ROW_ID.NEXTVAL WHERE ROW_ID IS NULL");
  if(d->executeSQL(stat))
  {
  //In Oracle DDL sentences are autoconfirmed (AUTOCOMMIT)
  //So I think kexi should reflect it somehow.. like this XD
    if(table.compare("kexi__objects "))
      return d->executeSQL("COMMIT");
    return true;  
  }
  return false;
}

bool OracleConnection::drv_dropTable( const QString& name )
{
  QString trig ="DROP TRIGGER KEXI__TG__"+ escapeIdentifier(name);
  QString seq ="DROP SEQUENCE KEXI__SEQ__"+ escapeIdentifier(name);
  m_sql = "DROP TABLE " + escapeIdentifier(name);

  d->executeSQL(trig);
  d->executeSQL(seq);
  return d->executeSQL(m_sql);
}

/*
 * We need to update al related sequences and triggers when we change the
 * table name
 */
bool OracleConnection::drv_alterTableName
                              (TableSchema& tableSchema, const QString& newName)
{
  QString oldTableName = tableSchema.name();
  IndexSchema *ind = tableSchema.primaryKey();
  QString fieldName;
 
  if(ind->isPrimaryKey())
  {
    d->rs = d->stmt->executeQuery(QString("SELECT KEXI__SEQ__"+oldTableName+".NEXTVAL FROM DUAL").latin1());
    int n = d->rs->getInt(1);
    QString sq="CREATE SEQUENCE KEXI__SEQ__" + escapeIdentifier(newName) 
               +" START WITH " + n;          
    if(!d->executeSQL(sq.latin1())||!d->createTrigger(newName,ind)) return false;
  }
  
  tableSchema.setName(newName);
  if (!d->executeSQL(QString::fromLatin1("ALTER TABLE %1 RENAME TO %2")
    .arg(escapeIdentifier(oldTableName)).arg(escapeIdentifier(newName))))
  {
    tableSchema.setName(oldTableName); //restore old name
    return false;
  }
  
  if(ind->isPrimaryKey())//Now we've changed the name, we can drop sqncs&trggers
  {
    QString droptg="DROP TRIGGER KEXI__TG__"+oldTableName;
    QString dropsq="DROP SEQUENCE KEXI__SEQ__"+oldTableName;
    
    d->executeSQL(droptg.latin1());
    d->executeSQL(dropsq.latin1());
  }
  return true;           
}
/*
 * RowID in Oracle is not a number, is an alphanumeric string, so we have an 
 * extra column on every single table which is called ROW_ID and a global 
 * sequence. The value of ROW_ID is set on drv_afterInsert
 */
Q_ULLONG OracleConnection::drv_lastInsertRowID()
{
  KexiDBDrvDbg;
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
  catch(oracle::occi::SQLException ea)
  {
    KexiDBDrvDbg<<ea.what();
    d->errmsg=ea.what();
    d->errno=ea.getErrorCode();
    return -1;
  }
}

int OracleConnection::serverResult()
{
	return d->errno;
}

QString OracleConnection::serverResultName()
{
	return QString();
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
  KexiDBDrvDbg;
	bool success;
	return resultExists(QString("SELECT TABLE_NAME FROM ALL_TABLES WHERE TABLE_NAME LIKE %1")
		.arg(driver()->escapeString(tableName).upper()), success) && success;
}

/**
 * Gets a list with all tables
 */
bool OracleConnection::drv_getTablesList( QStringList &list )
{
  KexiDBDrvDbg;
	KexiDB::Cursor *cursor;
	if (!(cursor = executeQuery( "SELECT TABLE_NAME FROM USER_TABLES" ))) 
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
    KexiDBDrvDbg;
		return KSharedPtr<PreparedStatement>
		                              (new OraclePreparedStatement(type,*d,fields));
}

