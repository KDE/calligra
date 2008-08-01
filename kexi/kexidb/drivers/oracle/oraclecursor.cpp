/**
 * @author Julia Sanchez-Simon <hithwen@gmail.com>
 * @author Miguel Angel Aragüez-Rey <fizban87@gmail.com>
 * @date   20/jul/2008
 */

/* This file is part of the KDE project
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

#include <kexidb/error.h>
#include <kexidb/utils.h>
#include <klocale.h>
#include <kdebug.h>
#include <limits.h>
#include "oraclecursor.h"
#include <vector>
//#include <occi.h>

#define BOOL bool
using namespace std;
using namespace KexiDB;
using namespace oracle::occi;

//Cursor can be defined in two ways:
OracleCursor::OracleCursor(KexiDB::Connection* conn, const QString& statement, uint cursor_options)
	: Cursor(conn,statement,cursor_options)
	, d( new OracleCursorData(conn) )
{
//Done
	m_options &= !Buffered;
  //Description of different conn vars :p
  //Stolen                                  Param ConnectionInternal
	d->oraconn 	= static_cast<OracleConnection*>(conn)->d->oraconn;
  d->env   		= static_cast<OracleConnection*>(conn)->d->env;
  d->rs   		= static_cast<OracleConnection*>(conn)->d->rs;
  d->stmt 		= static_cast<OracleConnection*>(conn)->d->stmt;
    
	KexiDBDrvDbg << "OracleCursor: constructor for query statement" << endl;
}

OracleCursor::OracleCursor(Connection* conn, QuerySchema& query, uint options )
   : Cursor( conn, query, options )
      , d( new OracleCursorData(conn) )
{
//Done
	m_options |= Buffered;
    d->oraconn= static_cast<OracleConnection*>(conn)->d->oraconn;
    d->env  	= static_cast<OracleConnection*>(conn)->d->env;
    d->rs 		= static_cast<OracleConnection*>(conn)->d->rs;
    d->stmt 	= static_cast<OracleConnection*>(conn)->d->stmt;
	KexiDBDrvDbg << "OracleCursor: constructor for query statement2" << endl;
}

OracleCursor::~OracleCursor() {
	close();
}

bool OracleCursor::drv_open() {
//Done, but there are thigs I dunno understand (yet ^^)
   QString count="select count(*) from("+m_sql+")";
KexiDBDrvDbg <<m_sql<<endl;
   try{
      d->rs=d->stmt->executeQuery(count.latin1());
      if(d->rs->next()) d->numRows=d->rs->getInt(1);//Numer of rows
      //Oracle doesnt provide a method to count ¬¬
      d->stmt->closeResultSet(d->rs);
      d->rs=d->stmt->executeQuery(m_sql.latin1());
      
      vector<MetaData> md = d->rs->getColumnListMetaData();
      m_fieldCount=md.size();//Number of columns
      d->types=vector<int>(m_fieldCount);
      d->lengths=vector<unsigned long>(m_fieldCount); 
   
 //KexiDBDrvDbg <<"Iniciatig Metadata"<<endl;   
      for(int i=0; i<m_fieldCount;i++){
         d->lengths[i]=md[i].getInt(MetaData::ATTR_DATA_SIZE);
         d->types[i]=md[i].getInt(MetaData::ATTR_DATA_TYPE);
         if(d->types[i]==8) d->rs->setMaxColumnSize(i+1,2000);//Long
      }
 //KexiDBDrvDbg <<"(2)"<<endl;
      m_at=0;
      d->rs->next();
 //KexiDBDrvDbg <<"(3)"<<endl;
      m_opened=true;
      m_records_in_buf = d->numRows; 
      m_buffering_completed = true;
      m_afterLast=false;
      KexiDBDrvDbg <<"DRV OPENED"<<endl;
      return true;
      
   }catch (ea){
      KexiDBDrvDbg << ea.what()<<endl;
      setError(ERR_DB_SPECIFIC,QString::fromUtf8(ea.getMessage().c_str()));
      return false;
   }

}

bool OracleCursor::drv_close() {
//Done! 
   KexiDBDrvDbg <<endl;
   if(d->rs){
      try{
        d->stmt->closeResultSet(d->rs);
        d->rs=0;
      }catch (SQLException ea){
        KexiDBDrvDbg <<ea.getMessage().c_str()<<endl;
        return false;
      }
   }
   //KexiDBDrvDbg <<"(1)"<<endl;
   d->lengths.~vector<unsigned long>();
   d->types.~vector<int>();
   m_opened=false;
   d->numRows=0;
   return true;
}

bool OracleCursor::moveFirst() {
//Done?  
    KexiDBDrvDbg <<endl; 
   if(d->rs->next()) return true;
   return false;
}

void OracleCursor::drv_getNextRecord() {
      KexiDBDrvDbg <<endl;
      if(d->rs->status()){
         m_result=FetchOK;
      }
      else if(at()>=d->numRows){
         m_result = FetchEnd;
      }
      else {
         m_result = FetchError;
      } 
   
}

QVariant OracleCursor::value(uint pos) {
//Done?   
//whats a QVariant?
   //-->QVariant makes to types what Ditto makes to pokemon
      //so... QVariant=types*pokemon/Ditto
//What is this function supposed to do?
   //-->Returns the value stored in the column number i (counting from 0)
   
KexiDBDrvDbg <<"(1)"<<endl;          
	if (!d->rs->status() || pos>=m_fieldCount)
		return QVariant();
    //MetaData md=(d->rs->getColumnListMetaData())[pos+1];
    int t=d->types[pos];
  
KexiDBDrvDbg <<"(2)"<<endl;       
   KexiDB::Field *f = (m_fieldsExpanded && pos<m_fieldsExpanded->count())
		? m_fieldsExpanded->at(pos)->field : 0;	
//! @todo js: use MYSQL_FIELD::type here!
   //Oracle ResultSet counts from 1
   //return KexiDB::cstringToVariant(d->rs[pos+1], f, d->lengths[pos]);
   
KexiDBDrvDbg <<"(3)"<<endl;
   if (t==1||t==5||t==8||t==9||t==94||t==96||t==104){//text
      return QVariant( d->rs->getString(pos+1).c_str());
   }else if (t==3||t==6||t==2){//Numeric
      return QVariant(d->rs->getDouble(pos+1));
									    //,md.getInt(MetaData::ATTR_PRECISION) );
   }else if (t==113) {//blob
      return QByteArray((const char*)&d->rs->getBlob(pos+1),d->lengths[pos]);
   }
KexiDBDrvDbg <<"(4)"<<endl;
//! @todo date/time?
	//default
   return QVariant(d->rs->getString(pos+1).c_str());
}

/* Not as with sqlite, the DB library doenst returns all values as
   strings. So we cannot use cstringtoVariant, isn't it?
 */
bool OracleCursor::drv_storeCurrentRow(RecordData& data) const
{
	KexiDBDrvDbg << ": Position is " << (long)m_at<< endl;
	if (d->numRows<=0)
		return false;

//! @todo    see SQLiteCursor::storeCurrentRow()
	vector<MetaData> md=d->rs->getColumnListMetaData();
	int t;
	
	data.resize(m_fieldCount);
	const uint fieldsExpandedCount = m_fieldsExpanded ?
																	 m_fieldsExpanded->count() : UINT_MAX;
	const uint realCount = QMIN(fieldsExpandedCount, m_fieldCount);
	
	for( uint i=0; i<realCount; i++) 
	{
		//md=d->rs->getColumnListMetaData()[i+1];
		t=md[i+1].getInt(MetaData::ATTR_DATA_TYPE);
		
		Field *f = m_fieldsExpanded ? m_fieldsExpanded->at(i)->field : 0;
		if (m_fieldsExpanded && !f)
			continue;
		if (t==1||t==5||t==9||t==94||t==96||t==104)//text
			data[i] = QVariant( d->rs->getString(i+1).c_str() );
		else if (t==3||t==6||t==2)//Numeric
			data[i] = QVariant(d->rs->getDouble(i+1));
											//,md[i+1].getInt(MetaData::ATTR_PRECISION));
		else if (t==113) {//blob
			data[i]=QByteArray((char *) &d->rs->getBlob(i+1),d->lengths[i]);
		}else{
//! @todo date/time?
	//default
			data[i]= QVariant(d->rs->getString(i+1).c_str());
		}
	}
	return true;
}

void OracleCursor::drv_appendCurrentRecordToBuffer() {KexiDBDrvDbg <<endl;}


void OracleCursor::drv_bufferMovePointerNext() {
  KexiDBDrvDbg <<endl;  
   try{
      d->rs->next();
   }catch ( ea){
      //cout<<ea.what();
      m_result = FetchError;
   }   
}

void OracleCursor::drv_bufferMovePointerPrev() {
   KexiDBDrvDbg << "Operation NOT AVAILABLE" << endl;
}


void OracleCursor::drv_bufferMovePointerTo(Q_LLONG to) {
    KexiDBDrvDbg <<"("<<to<<"/"<<d->numRows<<")"<<endl;
    Q_LLONG pos=to-m_at;
    for(int i=0; i<pos;i++){
       d->rs->next();
    }
}

const char** OracleCursor::rowData() const {
   //returns current record data
   //If its a number size is 22
   /*!@todo Calculate precision and scale if its a number*/
   /*int n=d->rs->getColumnListMetaData().size();
   char** info=(char**)malloc(n*sizeof(char*));
   int typeNumber;
   int typeSize;
   char* ts = (char *)malloc(10*sizeof(char)); 
   for(int i=0;i<n;i++){
typeNumber=d->rs->getColumnListMetaData()[i].getInt(MetaData::ATTR_DATA_TYPE);
      info[i]=decodeDataType(typeNumber);
typesize=d->rs->getColumnListMetaData()[i].getInt(MetaData::ATTR_DATA_SIZE);
      sprintf(ts,"%d",typeSize);
      strcat(info[i],"(");
      strcat(info[i],ts);
      strcat(info[i],")");
   }
   return(info);*/
	return NULL;
}

int OracleCursor::serverResult()
{
	return d->errno;
}

QString OracleCursor::serverResultName()
{
	return QString::null;
}

void OracleCursor::drv_clearServerResult()
{
  KexiDBDrvDbg <<endl;
 d->errno=0;
 d->errmsg="";
	/*if (d && d->rs){
        d->stmt->closeResultSet(d->rs);
        d->rs=0;
    }
    */
}

QString OracleCursor::serverErrorMsg()
{
 //Description of last operation's error/result
	return d->errmsg;
}
