#include "trader_main.h"
#include <typerepo.h>
#include "typerepo_impl.h"
#include "activator.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iostream>

#include <kapp.h>

CORBA::ORB_var orb;
CORBA::BOA_var boa;

class TraderLoader : public CORBA::BOAObjectRestorer
{
public:
    TraderLoader() { }
  
    CORBA::Boolean restore( CORBA::Object_ptr _obj )
    {
      if ( strcmp( _obj->_repoid(), "IDL:omg.org/CosTrading/Lookup:1.0" ) == 0 )
      {
	cerr << "restoring ... " << endl;
	(void)new Trader( _obj );
	cerr << "restored ... " << endl;
	return TRUE;
      }
      cout << "cannot restore " << _obj->_repoid() << " objects" << endl;
      return FALSE;
    }

    CORBA::Boolean bind( const char *repoid, const CORBA::ORB::ObjectTag &tag )
    {
      if ( strcmp( repoid, "IDL:omg.org/CosTrading/Lookup:1.0" ) == 0 )
      {
	cerr << "creating Trader" << endl;
	(void)new Trader( tag );
	return TRUE;
      }
      return FALSE;
    }
};

int main( int argc, char **argv )
{
  TraderLoader loader;

  KApplication app( argc, argv );
  
  orb = CORBA::ORB_init( argc, argv, "mico-local-orb" );
  boa = orb->BOA_init (argc, argv, "mico-local-boa");

  Activator::initStatic( orb );

  cout << "Trader running ..." << endl;
  boa->impl_is_ready( CORBA::ImplementationDef::_nil() );
  orb->run();
  return 0;
}

