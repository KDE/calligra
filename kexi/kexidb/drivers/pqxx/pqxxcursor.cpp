//
// C++ Implementation: pqxxsqlcursor
//
// Description: 
//
//
// Author: Adam Pigg <piggz@defiant.piggz.co.uk>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "pqxxcursor.h"
#include "pqxxconnection.h"
#include <kexidb/error.h>
#include <klocale.h>
#include <kdebug.h>

using namespace KexiDB;

//==================================================================================
//Constructor based on query statement
pqxxSqlCursor::pqxxSqlCursor(KexiDB::Connection* conn, const QString& statement, uint options):
	Cursor(conn,statement, options)
{
KexiDBDrvDbg << "PQXXSQLCURSOR: constructor for query statement" << endl;
my_conn = static_cast<pqxxSqlConnection*>(conn)->m_pqxxsql;
m_options = Buffered;
m_res = 0;
m_tran = 0;
}

//==================================================================================
//Constructor base on query object
pqxxSqlCursor::pqxxSqlCursor(Connection* conn, QuerySchema& query, uint options )
	: Cursor( conn, query, options )
{
KexiDBDrvDbg << "PQXXSQLCURSOR: constructor for query schema" << endl;
my_conn = static_cast<pqxxSqlConnection*>(conn)->m_pqxxsql;
m_options = Buffered;
m_res = 0;
m_tran = 0;
}

//==================================================================================
//Destructor
pqxxSqlCursor::~pqxxSqlCursor()
{
	close();
}

//==================================================================================
//Create a cursor result set 
bool pqxxSqlCursor::drv_open(const QString& statement)
{
	KexiDBDrvDbg << "pqxxSqlCursor::drv_open:" << statement << endl;

	if (!my_conn->is_open())
	{
		//should never happen, but who knows
		setError(ERR_NO_CONNECTION,i18n("No connection for cursor open operation specified"));
		return false;
	}

	//Set up a transaction
	try
	{
		//m_tran = new pqxx::work(*my_conn, "cursor_open");
		m_tran = new pqxx::nontransaction(*my_conn, "cursor_transaction");

		m_res = new pqxx::result(m_tran->exec(statement.utf8()));
		m_tran->commit();

		//We should now be placed before the first row, if any
		m_fieldCount = m_res->columns();
		m_opened=true;
		m_afterLast=false;
		m_records_in_buf = m_res->size();
		m_buffering_completed = true;
		return true;m_buffering_completed = true;
	}
	catch (const std::exception &e)
    	{
		setError(ERR_DB_SPECIFIC,e.what());
		KexiDBDrvDbg << "pqxxSqlCursor::drv_open:exception - " << e.what() << endl;
        	return false;
    	}
	catch(...)
    	{
    		setError();
    	}
	return false;
}

//==================================================================================
//Delete objects
bool pqxxSqlCursor::drv_close()
{
	m_opened=false;
	if(m_res != 0)
		delete m_res;
	if(m_tran != 0)
		delete m_tran;

	m_res = 0;
	m_tran = 0;

	return true;
}

//==================================================================================
//Gets the next record...doesnt need to do much, just return fetchend if at end of result set
void pqxxSqlCursor::drv_getNextRecord()
{
KexiDBDrvDbg << "pqxxSqlCursor::drv_getNextRecord, size is " <<m_res->size() << " Current Position is " << (long)at() << endl;
if(at() < m_res->size() && at() >=0)
{	
	m_result = FetchOK;
}
else if (at() >= m_res->size())
{
	m_result = FetchEnd;
}
else
{
	m_result = FetchError;
}
}

//==================================================================================
//Check the current position is within boundaries
void pqxxSqlCursor::drv_getPrevRecord()
{
KexiDBDrvDbg << "pqxxSqlCursor::drv_getPrevRecord" << endl;

if(at() < m_res->size() && at() >=0)
{	
	m_result = FetchOK;
}
else if (at() >= m_res->size())
{
	m_result = FetchEnd;
}
else
{
	m_result = FetchError;
}
}

//==================================================================================
//Return the value for a given column for the current record
QVariant pqxxSqlCursor::value(int pos) const
{
	if (!m_res->size() > 0)
	{
		KexiDBDrvDbg << "pqxxSqlCursor::value - ERROR: result size not greater than 0" << endl;
		return QVariant();
	}

	if (pos>=m_fieldCount)
	{
		KexiDBDrvDbg << "pqxxSqlCursor::value - ERROR: requested position is greater than the number of fields" << endl;
		return QVariant();
	}

	KexiDBDrvDbg << "VALUE AT " << pos << " IS: " << (*m_res)[at()][pos].c_str() << endl;

	return QVariant(QString::fromUtf8((*m_res)[at()][pos].c_str()));
}

//==================================================================================
//Return the current record as a char**
//who'd have thought we'd be using char** in this day and age :o)
const char** pqxxSqlCursor::recordData() const
{
	KexiDBDrvDbg << "pqxxSqlCursor::recordData" << endl;
	
	const char** row;
	
	row = (const char**)malloc(m_res->columns()+1);
	row[m_res->columns()] = NULL;
	if (at() >= 0 && at() < m_res->size())
	{
		for(int i = 0; i < m_res->columns(); i++)
		{
			row[i] = (char*)malloc(strlen((*m_res)[at()][i].c_str())+1);
			strcpy((char*)(*m_res)[at()][i].c_str(), row[i]);
			KexiDBDrvDbg << row[i] << endl;
		}
	}
	else
	{
		KexiDBDrvDbg << "pqxxSqlCursor::recordData: m_at is invalid" << endl;
	}
	return row;
}

//==================================================================================
//Store the current record in [data]
void pqxxSqlCursor::storeCurrentRecord(RecordData &data) const
{
KexiDBDrvDbg << "pqxxSqlCursor::storeCurrentRecord: POSITION IS " << (long)m_at<< endl;

if (!m_res->size()>0)
	return;

data.reserve(m_fieldCount);

for( int i=0; i<m_fieldCount; i++)
{
	data[i] = QVariant(value(i));
}
}

//==================================================================================
//
void pqxxSqlCursor::drv_clearServerResult()
{
#warning TODO: stuff with server results
}

//==================================================================================
//Add the current record to the internal buffer
//Implementation required but no need in this driver
//Result set is a buffer so dont need another
void pqxxSqlCursor::drv_appendCurrentRecordToBuffer()
{

}

//==================================================================================
//Move internal pointer to internal buffer +1
//Implementation required but no need in this driver
void pqxxSqlCursor::drv_bufferMovePointerNext()
{

}

//==================================================================================
//Move internal pointer to internal buffer -1
//Implementation required but no need in this driver
void pqxxSqlCursor::drv_bufferMovePointerPrev()
{

}

//==================================================================================
//Move internal pointer to internal buffer to N
//Implementation required but no need in this driver
void pqxxSqlCursor::drv_bufferMovePointerTo(Q_LLONG to)
{

}
