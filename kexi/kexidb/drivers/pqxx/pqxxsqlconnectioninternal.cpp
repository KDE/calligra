//
// C++ Implementation: pqxxsqlconnectioninternal
//
// Description: 
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "pqxxsqlconnectioninternal.h"
#include <kdebug.h>

using namespace KexiDB;
pqxxSqlConnectionInternal::pqxxSqlConnectionInternal()
 : ConnectionInternal()
{
m_pqxxsql = 0;
m_res = 0;
}


pqxxSqlConnectionInternal::~pqxxSqlConnectionInternal()
{	

}

void pqxxSqlConnectionInternal::storeResult()
{
	errmsg = "";
}