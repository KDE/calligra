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
m_fieldCount;
}

//==================================================================================
//
pqxxSqlCursor::~pqxxSqlCursor()
{
	close();
	delete m_cur;
	delete m_tran;
	m_cur = 0;
	m_tran = 0;
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
		m_tran = new pqxx::transaction<pqxx::serializable>(*my_conn);

		m_cur = new pqxx::Cursor(*m_tran, statement.utf8(), "cur", 1);
		//m_tran->commit();
		kdDebug() << "pqxxSqlCursor::drv_open - Created Cursor:" << statement << endl;

		if (*m_cur)
		{
			//We should now be placed before the first row, if any
			m_opened=true;
			m_afterLast=false;
			return true;
		}
		else
		{
			//There were  results
			return false;
		}
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
	m_tran->commit();
	//delete m_cur;
	//delete m_tran;
	return true;
}

//==================================================================================
//
bool pqxxSqlCursor::drv_moveFirst()
{
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
}

//==================================================================================
//
bool pqxxSqlCursor::drv_getNextRecord()
{
kdDebug() << "pqxxSqlCursor::drv_getNextRecord" << endl;

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
}


//==================================================================================
//
bool pqxxSqlCursor::drv_getPrevRecord()
{
kdDebug() << "pqxxSqlCursor::drv_getPrevRecord" << endl;
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
}

//==================================================================================
//
QVariant pqxxSqlCursor::value(int pos) const
{
	if (!m_res.size() > 0)
	{
		kdDebug() << "pqxxSqlCursor::value - ERROR: result size not greater than 0" << endl;
		return QVariant();
	}

	if (pos>=m_fieldCount)
	{
		kdDebug() << "pqxxSqlCursor::value - ERROR: requested position is greater than the number of fields" << endl;
		return QVariant();
	}
//	if (m_res[0][pos]==0)
//		return QVariant();
	kdDebug() << "pqxxSqlCursor::value at pos:" << pos << "is: " << m_res[0][pos].c_str() << endl;

	return QVariant(QString::fromUtf8(m_res[0][pos].c_str()));
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

if (!m_res.size()>0)
	return;

data.reserve(m_fieldCount);

for( int i=0; i<m_fieldCount; i++)
{
	data[i] = QVariant(value(i));
}
}
