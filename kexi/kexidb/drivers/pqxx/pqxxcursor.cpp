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
//
pqxxSqlCursor::pqxxSqlCursor(KexiDB::Connection* conn, const QString& statement, uint options):
	Cursor(conn,statement, options)
{
my_conn = static_cast<pqxxSqlConnection*>(conn)->m_pqxxsql;
m_options = Buffered;
}

//==================================================================================
//
pqxxSqlCursor::pqxxSqlCursor(Connection* conn, QuerySchema& query, uint options )
	: Cursor( conn, query, options )
{
my_conn = static_cast<pqxxSqlConnection*>(conn)->m_pqxxsql;
m_options = Buffered;
}

//==================================================================================
//
pqxxSqlCursor::~pqxxSqlCursor()
{
	close();
}

//==================================================================================
//
bool pqxxSqlCursor::drv_open(const QString& statement)
{
	kdDebug() << "pqxxSqlCursor::drv_open:" << statement << endl;

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
		kdDebug() << "pqxxSqlCursor::drv_open:exception - " << e.what() << endl;
        	return false;
    	}
}

//==================================================================================
//
bool pqxxSqlCursor::drv_close()
{
	m_fieldCount=0;
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
//
void pqxxSqlCursor::drv_getNextRecord()
{
kdDebug() << "pqxxSqlCursor::drv_getNextRecord, size is " <<m_res->size() << " Current Position is " << (long)at() << endl;
if(at() < m_res->size())
{	
	m_result = FetchOK;
}
else
{
	m_result = FetchEnd;
}	
#if 0
m_at++;
if (m_at <= m_res->size())
{
	m_beforeFirst=false;
	m_validRecord=true;
	m_afterLast=false;
	kdDebug() << "NEW POSITION IS " << (long)m_at << endl;
	return true;
}
else
{
	m_at=-1;
	m_validRecord = false;
	m_afterLast = true;
	return false;
}
#endif
}


//==================================================================================
//Move pointer to the previous record
void pqxxSqlCursor::drv_getPrevRecord()
{
kdDebug() << "pqxxSqlCursor::drv_getPrevRecord" << endl;

if(at() > 0 && at() <= m_res->size())
{	
	m_result = FetchOK;
}


m_result = FetchOK;
#if 0
if (m_at > 0)
{
	m_beforeFirst=false;
	m_validRecord=true;
	m_afterLast=false;
	return true;
}
else
{
	return false;
}
#endif
#if 0
	try
	{
		if (m_cur-=1)
		{
			m_res = m_cur->Fetch(1);
			m_beforeFirst=false;
			m_validRecord=true;
			m_afterLast=false;
			return true;
		}
		else
		{
			m_validRecord = false;
			m_afterLast = true;
			return false;
		}
	}
	catch (const std::exception &e)
    	{
		setError(ERR_DB_SPECIFIC,e.what());
		kdDebug() << "EXCEPTION: pqxxSqlCursor::drv_getPrevRecord - " << e.what() << endl;
		m_validRecord = false;
        	return false;
    	}
#endif
}

//==================================================================================
//
QVariant pqxxSqlCursor::value(int pos) const
{
	kdDebug() << "VALUE AT " << pos << endl;
	if (!m_res->size() > 0)
	{
		kdDebug() << "pqxxSqlCursor::value - ERROR: result size not greater than 0" << endl;
		return QVariant();
	}

	if (pos>=m_fieldCount)
	{
		kdDebug() << "pqxxSqlCursor::value - ERROR: requested position is greater than the number of fields" << endl;
		return QVariant();
	}

	kdDebug() << "IS: " << (*m_res)[at()][pos].c_str() << endl;

	return QVariant(QString::fromUtf8((*m_res)[at()][pos].c_str()));
}

//==================================================================================
//Return the current record as a char**
const char** pqxxSqlCursor::recordData() const
{
	kdDebug() << "pqxxSqlCursor::recordData" << endl;
	return NULL;
}

//==================================================================================
//Store the current record in [data]
void pqxxSqlCursor::storeCurrentRecord(RecordData &data) const
{
kdDebug() << "pqxxSqlCursor::storeCurrentRecord: POSITION IS " << (long)m_at<< endl;

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