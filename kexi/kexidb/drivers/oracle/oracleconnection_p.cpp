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

#ifdef ORACLEMIGRATE_H
#define NAMESPACE KexiMigration
#else
#define NAMESPACE KexiDB
#endif

#include <QStringList>
#include <QFile>
#include <kdebug.h>
#include "oracleconnection_p.h"
#include <db/connectiondata.h>
#include <string>

using namespace NAMESPACE;
using namespace oracle::occi;
using namespace std;

/* ************************************************************************** */
OracleConnectionInternal::OracleConnectionInternal(KexiDB::Connection*
connection)
   : ConnectionInternal(connection)
    , env(0)
    , oraconn(0)
    , errno(0)
    , rs(0)
{
	//KexiDBDrvDbg << "OracleConnectionInternal::Constructor: ";
   try{
      env = Environment::createEnvironment();
   }catch (const oracle::occi::SQLException &ea){
      errno=ea.getErrorCode();
      errmsg=strdup(ea.what());
      KexiDBDrvDbg <<errmsg;
   }

}

OracleConnectionInternal::~OracleConnectionInternal()
{
 KexiDBDrvDbg;
 try{
	 Environment::terminateEnvironment(env);
	 env=0;
	 KexiDBDrvDbg;
	}
	catch (const oracle::occi::SQLException &ea){
      errno=ea.getErrorCode();
      errmsg=strdup(ea.what());
      KexiDBDrvDbg <<errmsg;
  }
}
/* ************************************************************************** */
/*! Connects to the Oracle server on host as the given user using the specified
    password. If the server is on a remote machine, then a port is 
    the port that the remote server is listening on.
 */
/*
* MySQL driver-developers: is latin1() encoding here valid? 
* what about using UTF for passwords?
* Julia: This is to add the C '\0' char at the end, QString 
* (as returned by unicode()) is generally not terminated by '\0'
*/
bool OracleConnectionInternal::db_connect(const KexiDB::ConnectionData& data)
{
  QString hostName; 
	QString sid="LCC";
	//sid= data.sid.latin1(); //To be included in the API
	char *port;
	
  if(data.port!=0)
  {
    port=(char*)malloc(10*sizeof(char));
    sprintf(port,"%d",data.port);
  }
  else
  {
    port=strdup("1521");
  }
	if (data.hostName.isEmpty() || data.hostName.lower()=="localhost") {
		//localSocketFile not suported
			hostName = "127.0.0.1"; 
	}
	else
	{
		hostName= data.hostName.latin1();
	}
  QString connectStr=("//"+hostName+":"+port+"/"+sid).latin1();
  try{
    oraconn = env->createConnection(data.userName.latin1(),
    data.password.latin1(),
    connectStr.latin1());
    stmt=oraconn->createStatement();
  }
  catch (const oracle::occi::SQLException &ea)
  {
     errno=ea.getErrorCode();
     errmsg=ea.what();
     KexiDBDrvDbg<<ea.what();
	   return false;
  }
  KexiDBDrvDbg<<"Ok";	
  return true;
}
void OracleConnectionInternal::storeResult(){}
/*! Disconnects from the database.
 */
bool OracleConnectionInternal::db_disconnect()
{
  KexiDBDrvDbg;
  try{
	  oraconn->terminateStatement(stmt);
	  env->terminateConnection(oraconn);
    oraconn=0;
	  return true;
	  }
	 catch (const oracle::occi::SQLException &ea)
	 {
	  errmsg=ea.getMessage().c_str();
	  KexiDBDrvDbg<<errmsg;
	  errno=ea.getErrorCode();
	  return false;
	 }
}

/* ************************************************************************** */
/*! Makes no sense in oracle so cheks if dbname is current user
 */
bool OracleConnectionInternal::useDatabase(const QString &dbName) 
{
  KexiDBDrvDbg;
	QString user; 
	try{
		rs=stmt->executeQuery("SELECT user FROM DUAL");
		if(rs->next()) user=QString(rs->getString(1).c_str());
		stmt->closeResultSet(rs);
		rs=0;
		return !user.compare(dbName);
	}
	catch (const oracle::occi::SQLException &ea)
  {
       errno=ea.getErrorCode();
       errmsg=strdup(ea.what());
       KexiDBDrvDbg <<"OracleConnectionInternal::useDatabase:"<<ea.what();
       return(false);
  }
}

bool OracleConnectionInternal::executeSQL(const QString& statement) {
    //QString stat=QString(statement);
    //stat.replace("VALUES (", "VALUES ( KEXI__SEQ__ROW_ID.NEXTVAL, ");
    KexiDBDrvDbg<<statement;
    //const char *query=statement.utf8();
    try
    {
      stmt->execute(statement.latin1());
      rs=stmt->getResultSet();
      return true;
    }
    catch (const oracle::occi::SQLException &ea)
    {
       errno=ea.getErrorCode();
       errmsg=strdup(ea.what());
       KexiDBDrvDbg<<errmsg;
       return false;
    }
}
QString OracleConnectionInternal::escapeIdentifier(const QString& str) const {
	return QString(str).replace('`', "'").toUpper();
}
QString OracleConnectionInternal::getServerVersion()
{
	try
	{
		return QString(oraconn->getServerVersion().c_str());
	}
	catch (const oracle::occi::SQLException &ea)
  {
       errno=ea.getErrorCode();
       errmsg=strdup(ea.what());
       return(NULL);
  }
}
bool OracleConnectionInternal::createSequences(){
  KexiDBDrvDbg;
  return executeSQL("CREATE SEQUENCE KEXI__SEQ__ROW_ID")&&
         executeSQL("ALTER SEQUENCE KEXI__SEQ__ROW_ID NOCACHE");
}
	
bool OracleConnectionInternal::createTrigger
                                           (QString tableName, IndexSchema* ind)
{
  QString fieldName;
  QString tg="CREATE OR REPLACE TRIGGER KEXI__TG__"+tableName+
             "\nBEFORE INSERT ON "+tableName+
             "\nFOR EACH ROW\n"+
             "BEGIN\n";
  for(int i=0; i<ind->fieldCount(); i++)
  {
      fieldName=ind->field(i)->name();          
      tg=tg+"IF :NEW."+fieldName+" IS NULL THEN\nSELECT KEXI__SEQ__"+tableName
      +".NEXTVAL INTO :NEW."+fieldName+" FROM DUAL;\nEND IF;\n";

  }
  tg=tg+"END;";
  return executeSQL(tg); 
}
//--------------------------------------

OracleCursorData::OracleCursorData(KexiDB::Connection* connection)
: OracleConnectionInternal(connection)
, lengths(0)
, numRows(0)
{}

OracleCursorData::~OracleCursorData(){}

