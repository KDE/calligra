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
m_fieldCount = 0;
m_at = 0;
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
		return true;
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
bool pqxxSqlCursor::drv_getNextRecord()
{
kdDebug() << "pqxxSqlCursor::drv_getNextRecord, size is " <<m_res->size() << " Current Position is " << (long)m_at << endl;
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
}


//==================================================================================
//Move pointer to the previous record
bool pqxxSqlCursor::drv_getPrevRecord()
{
kdDebug() << "pqxxSqlCursor::drv_getPrevRecord" << endl;

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

	return QVariant(QString::fromUtf8((*m_res)[m_at-1][pos].c_str()));
}

//==================================================================================
//I giess we return a char** full of the record data
const char** pqxxSqlCursor::recordData() const
{
kdDebug() << "pqxxSqlCursor::recordData" << endl;
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
