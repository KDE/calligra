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
	kdDebug() << "pqxxSqlCursor::drv_open" << endl;

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

	delete m_res;
	delete m_tran;
	m_res = 0;
	m_tran = 0;
	return true;
}

//==================================================================================
//
bool pqxxSqlCursor::drv_moveFirst()
{
m_pos = 0;
#if 0
try
{
	m_cur->BACKWARD_ALL();
}
catch (const std::exception &e)
{
	setError(ERR_DB_SPECIFIC,e.what());
	kdDebug() << "EXCEPTION: pqxxSqlCursor::drv_getNextRecord - " << e.what() << endl;
	m_validRecord = false;
	return false;
}
#endif
return true;
}

//==================================================================================
//
bool pqxxSqlCursor::drv_getNextRecord()
{
kdDebug() << "pqxxSqlCursor::drv_getNextRecord" << endl;
if (m_pos < m_res->size() - 1)
{
	m_pos++;
	return true;
}

return false;

#if 0
	try
	{
		if (*m_cur>>m_res)
		{
			//kdDebug() << "pqxxSqlCursor::drv_getNextRecord - Fetch" << endl;
			//m_res = m_cur->Fetch(1)
			m_fieldCount = m_res.columns(); //Only know this when moved to first record
			m_beforeFirst=false;
			m_validRecord=true;
			m_afterLast=false;

			kdDebug() << "pqxxSqlCursor::drv_getNextRecord - Done" << endl;
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
		kdDebug() << "EXCEPTION: pqxxSqlCursor::drv_getNextRecord - " << e.what() << endl;
		m_validRecord = false;
        	return false;
    	}
#endif
}


//==================================================================================
//
bool pqxxSqlCursor::drv_getPrevRecord()
{
kdDebug() << "pqxxSqlCursor::drv_getPrevRecord" << endl;

if (m_pos > 0)
{
	m_pos--;
	return true;
}
return false;

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

	kdDebug() << "pqxxSqlCursor::value at pos:" << pos << "is: " << (*m_res)[m_pos][pos].c_str() << endl;

	return QVariant(QString::fromUtf8((*m_res)[m_pos][pos].c_str()));
}

//==================================================================================
//
const char** pqxxSqlCursor::recordData() const
{
kdDebug() << "pqxxSqlCursor::recordData" << endl;
}

//==================================================================================
//Store the current record in [data]
void pqxxSqlCursor::storeCurrentRecord(RecordData &data) const
{
kdDebug() << "pqxxSqlCursor::storeCurrentRecord" << endl;

if (!m_res->size()>0)
	return;

data.reserve(m_fieldCount);

for( int i=0; i<m_fieldCount; i++)
{
	data[i] = QVariant(value(i));
}
}
