#include "pgsqlDataSource.h"
#include <kded_utils.h>

PGSQLDataSource::PGSQLDataSource()
  : KOMAggregate( this )
{
  m_bInitialized = FALSE;
  
  KDB::PROPINFO infoHost;
  
  infoHost.wstrName = "init/host";
  infoHost.wstrDescr = "The hostname of the data store server";
  infoHost.strType = "string";
  infoHost.mode = KDB::READWRITE;
  
  m_infos.insert( "init/host", infoHost );
  
  KDB::PROPINFO infoDB;
  
  infoDB.wstrName = "init/database";
  infoDB.wstrDescr = "The database being accessed";
  infoDB.strType = "string";
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
  
  varHostProp->wstrName = "init/host";
  varDBProp->wstrName = "init/database";
  
  GetProperty( varHostProp );
  GetProperty( varDBProp );
  
  if ( varHostProp->status != KDB::OK ) {
    
    // the host property was not set
    
    KDB::PropertyMissing exc;
    exc.wstrName = "init/host";
    
    mico_throw( exc );
    
  }
  
  if ( varDBProp->status != KDB::OK ) {
    
    // the database property was not set
    
    KDB::PropertyMissing exc;
    exc.wstrName = "init/database";
    
    mico_throw( exc );
    
  }
  
  // now we set those properties readonly
  
  QString qstrHost = "init/host";
  QString qstrDB = "init/database";
  
  SetPropertyMode( qstrHost, KDB::READ );
  SetPropertyMode( qstrDB, KDB::READ );
  
  m_bInitialized = TRUE;
}


void PGSQLDataSource::UnInitialize()
{
  // now we set those properties back to normal
  
  QString qstrHost = "init/host";
  QString qstrDB = "init/database";
  
  SetPropertyMode( qstrHost, KDB::READWRITE );
  SetPropertyMode( qstrDB, KDB::READWRITE );
  
  m_bInitialized = FALSE;
}

KDB::ISession_ptr PGSQLDataSource::CreateSession()
{
  return NULL;
}




