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
}

//==================================================================================
//
bool pqxxSqlCursor::drv_open()
{
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

		m_cur = new pqxx::Cursor(*m_tran, m_statement.utf8(), "cur", 1);

		if (m_cur)
		{
			m_res = m_cur->Fetch(1);
			m_numFields = m_res[0].size();
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
}

//==================================================================================
//
bool pqxxSqlCursor::drv_getNextRecord()
{
	if (m_cur+=1)
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

//==================================================================================
//
bool pqxxSqlCursor::drv_getPrevRecord()
{
	if (m_cur+=1)
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
	return false;
}

//==================================================================================
//
QVariant pqxxSqlCursor::value(int pos)
{
	if (!&m_res)
		return QVariant();

	if (pos>=m_numFields)
		return QVariant();

//	if (m_res[0][pos]==0)
//		return QVariant();

	return QVariant(QString::fromUtf8(m_res[0][pos].c_str()));
}

//==================================================================================
//
const char** pqxxSqlCursor::recordData()
{

}