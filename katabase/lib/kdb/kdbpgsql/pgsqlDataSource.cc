#include "pgsqlDataSource.h"
#include <kded_utils.h>

PGSQLDataSource::PGSQLDataSource()
  : KOMAggregate( this )
{
  m_bInitialized = FALSE;
  
  KDB::PROPINFO infoHost;
  
  infoHost.wstrName = CORBA::wstring_dup( L"init/host" );
  infoHost.wstrDescr = CORBA::wstring_dup( L"The hostname of the data store server" );
  infoHost.strType = CORBA::string_dup( "string" );
  infoHost.mode = KDB::READWRITE;
  
  m_infos.insert( "init/host", infoHost );
  
  KDB::PROPINFO infoDB;
  
  infoDB.wstrName = CORBA::wstring_dup( L"init/database" );
  infoDB.wstrDescr = CORBA::wstring_dup( L"The database being accessed" );
  infoDB.strType = CORBA::string_dup( "string" );
  infoDB.mode = KDB::READWRITE;
  
  m_infos.insert( "init/database", infoDB );
}


PGSQLDataSource::~PGSQLDataSource()
{
}


void PGSQLDataSource::Initialize()
{
  // we check if the "init/host" and "init/database" properties
  // are set
  
  KDB::PROP_var varHostProp = new KDB::PROP;
  KDB::PROP_var varDBProp = new KDB::PROP;
  
  varHostProp->wstrName = CORBA::wstring_dup( L"init/host" );
  varDBProp->wstrName = CORBA::wstring_dup( L"init/database" );
  
  GetProperty( varHostProp );
  GetProperty( varDBProp );
  
  if ( varHostProp->status != KDB::OK ) {
    
    // the host property was not set
    
    KDB::PropertyMissing exc;
    exc.wstrName = CORBA::wstring_dup( L"init/host" );
    
    mico_throw( exc );
    
  }
  
  if ( varDBProp->status != KDB::OK ) {
    
    // the database property was not set
    
    KDB::PropertyMissing exc;
    exc.wstrName = CORBA::wstring_dup( L"init/database" );
    
    mico_throw( exc );
    
  }
  
  // now we set those properties readonly
  
  QString qstrHost = WString2QString( L"init/host" );
  QString qstrDB = WString2QString( L"init/database" );
  
  SetPropertyMode( qstrHost, KDB::READ );
  SetPropertyMode( qstrDB, KDB::READ );
  
  m_bInitialized = TRUE;
}


void PGSQLDataSource::UnInitialize()
{
  // now we set those properties back to normal
  
  QString qstrHost = WString2QString( L"init/host" );
  QString qstrDB = WString2QString( L"init/database" );
  
  SetPropertyMode( qstrHost, KDB::READWRITE );
  SetPropertyMode( qstrDB, KDB::READWRITE );
  
  m_bInitialized = FALSE;
}

KDB::ISession_ptr PGSQLDataSource::CreateSession()
{
  return NULL;
}




