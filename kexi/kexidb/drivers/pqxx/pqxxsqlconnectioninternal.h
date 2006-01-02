//
// C++ Interface: pqxxsqlconnectioninternal
//
// Description: 
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PQXXSQLCONNECTIONINTERNAL_H
#define PQXXSQLCONNECTIONINTERNAL_H

#include <kexidb/connection_p.h>
#include <pqxx/all.h>

/**
	@author Adam Pigg <adam@piggz.co.uk>
*/
namespace KexiDB
{
class pqxxSqlConnectionInternal : public ConnectionInternal
{
public:
    	pqxxSqlConnectionInternal();

    	~pqxxSqlConnectionInternal();

    	//! stores last result's message
    	virtual void storeResult();

	pqxx::connection* m_pqxxsql;
	pqxx::result* m_res;

	QString errmsg; //!< server-specific message of last operation
	int res; //!< result code of last operation on server
};
}
#endif
