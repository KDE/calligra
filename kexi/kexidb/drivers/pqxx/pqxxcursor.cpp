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
	Cursor(conn,statement, options),m_numFields(0)
{
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

	pqxxSqlConnection *conn=(pqxxSqlConnection*)m_conn;
	if ( (!conn)  || (!conn->m_pqxxsql))
	{
		//should never happen, but who knows
		setError(ERR_NO_CONNECTION,i18n("No connection for cursor open operation specified"));
		return false;
	}

	//Set up a transaction
	try
	{
		m_tran = new pqxx::transaction<pqxx::serializable>(*(conn->m_pqxxsql));

		m_cur = new pqxx::Cursor(*m_tran, statement.utf8(), "cur", 1);
		//m_tran->commit();
		kdDebug() << "pqxxSqlCursor::drv_open - Created Cursor:" << statement << endl;

		if (m_cur->size() == 0)
		{
			//There were  results
			kdDebug() << "pqxxSqlCursor::drv_open - There were no  results" << endl;
			return false;
		}
		else
		{
			//We should now be placed before the first row, if any
			kdDebug() << "pqxxSqlCursor::drv_open - There are " << m_cur->size() << " results" << endl;
			m_opened=true;
			m_afterLast=false;
			return true;
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
	delete &m_res;
	m_numFields=0;
	m_opened=false;
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
		(*m_cur) >> m_res;

		if (m_res.size() > 0)
		{
			//kdDebug() << "pqxxSqlCursor::drv_getNextRecord - Fetch" << endl;
			//m_res = m_cur->Fetch(1)
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
kdDebug() << "pqxxSqlCursor::value at pos:" << pos << endl;
	if (m_res.size() == 0)
		return QVariant();

	if (pos>=m_numFields)
		return QVariant();

//	if (m_res[0][pos]==0)
//		return QVariant();

	return QVariant(QString::fromUtf8(m_res[0][pos].c_str()));
}

//==================================================================================
//
const char** pqxxSqlCursor::recordData() const
{
kdDebug() << "pqxxSqlCursor::recordData" << endl;
}

//==================================================================================
//
void pqxxSqlCursor::storeCurrentRecord(RecordData &data) const
{
kdDebug() << "pqxxSqlCursor::storeCurrentRecord" << endl;
}
