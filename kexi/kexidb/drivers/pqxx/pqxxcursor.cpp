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
#include <kexidb/global.h>

#include <klocale.h>
#include <kdebug.h>

using namespace KexiDB;


unsigned int pqxxSqlCursor_trans_num=0; //!< debug helper

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
		
	QCString cur_name;
	//Set up a transaction
	try
	{
		//m_tran = new pqxx::work(*my_conn, "cursor_open");
		cur_name.sprintf("cursor_transaction%d", pqxxSqlCursor_trans_num++);
		
		m_tran = new pqxx::nontransaction(*my_conn, (const char*)cur_name);

		m_res = new pqxx::result(m_tran->exec(statement.utf8()));
		m_tran->commit();
		KexiDBDrvDbg << "pqxxSqlCursor::drv_open: trans. commited: " << cur_name <<endl;

		//We should now be placed before the first row, if any
		m_fieldCount = m_res->columns();
//js		m_opened=true;
		m_afterLast=false;
		m_records_in_buf = m_res->size();
		m_buffering_completed = true;
		return true;
	}
	catch (const std::exception &e)
    	{
			setError(ERR_DB_SPECIFIC,e.what());
			KexiDBDrvDbg << "pqxxSqlCursor::drv_open:exception - " << e.what() << endl;
    	}
	catch(...)
    	{
    		setError();
    	}
	delete m_tran;
	m_tran = 0;
	KexiDBDrvDbg << "pqxxSqlCursor::drv_open: trans. rolled back! - " << cur_name <<endl;
	return false;
}

//==================================================================================
//Delete objects
bool pqxxSqlCursor::drv_close()
{
//js	m_opened=false;

	delete m_res;
	m_res = 0;
		
	delete m_tran;
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
QVariant pqxxSqlCursor::value(uint pos)
{
	if (pos < m_fieldCount)
		return pValue(pos);
	else
		return QVariant();
}

//==================================================================================
//Return the value for a given column for the current record - Private const version
QVariant pqxxSqlCursor::pValue(uint pos) const
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

	KexiDB::Field *f = m_fieldsExpanded ? m_fieldsExpanded->at(pos)->field : 0;
	//from most to least frequently used types:
	if (!f || f->isTextType())
	{
		return QVariant((*m_res)[at()][pos].c_str());
	}
	else if (f->isIntegerType())
	{
		return QVariant((*m_res)[at()][pos].as(int()));
	}
	else if (f->isFPNumericType())
	{
		return QVariant((*m_res)[at()][pos].as(double()));
	}
	
	return QVariant((*m_res)[at()][pos].c_str());
}

//==================================================================================
//Return the current record as a char**
//who'd have thought we'd be using char** in this day and age :o)
const char** pqxxSqlCursor::rowData() const
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
void pqxxSqlCursor::storeCurrentRow(RowData &data) const
{
KexiDBDrvDbg << "pqxxSqlCursor::storeCurrentRow: POSITION IS " << (long)m_at<< endl;

if (!m_res->size()>0)
	return;

data.reserve(m_fieldCount);

for( uint i=0; i<m_fieldCount; i++)
{
	data[i] = pValue(i);
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
