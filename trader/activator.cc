#include "activator.h"

#include <mico/throw.h>

PortableServer::POA_var Activator::s_vPoa;
Activator* Activator::s_pSelf = 0L;

void Activator::initStatic( CORBA::ORB_ptr _orb )
{
  if ( s_pSelf != 0L )
    return;
  
  /*
   * Obtain a reference to the RootPOA and its Manager
   */

  CORBA::Object_var poaobj = _orb->resolve_initial_references ("RootPOA");
  PortableServer::POA_var poa = PortableServer::POA::_narrow (poaobj);
  PortableServer::POAManager_var mgr = poa->the_POAManager();

  /*
   * The RootPOA has the USE_ACTIVE_OBJECT_MAP_ONLY policy; to register
   * our ServantManager, we must create our own POA with the
   * USE_SERVANT_MANAGER policy
   */

  CORBA::PolicyList pl;
  pl.length(1);
  pl[0] = poa->create_request_processing_policy (PortableServer::USE_SERVANT_MANAGER);
  s_vPoa = poa->create_POA ("ActivatorPOA", mgr, pl);

  /*
   * Activate our ServantManager
   */

  s_pSelf = new Activator;
  PortableServer::ServantManager_var servref = s_pSelf->_this();
  s_vPoa->set_servant_manager( servref );

  mgr->activate();
}

Activator* Activator::self()
{
  if ( s_pSelf == 0L )
  {
    cerr << "ERROR: You mist initialize the Activator first" << endl;
    assert( 0 );
  }
  
  return s_pSelf;
}

Activator::Activator()
{
}

CORBA::Object_ptr Activator::createReference( const char *_implreponame, const char* _repoid )
{
  PortableServer::ObjectId id;
  unsigned int l = strlen( _implreponame );
  unsigned int r = strlen( _repoid );
  id.length( l + 1 + r );
  for( unsigned int i = 0; i < l; i++ )
    id[i] = _implreponame[i];
  id[l] = 0;
  for( unsigned int i = 0; i < r; i++ )
    id[l+1+i] = _repoid[i];
  
  CORBA::Object_ptr obj = s_vPoa->create_reference_with_id ( id, _repoid );
  assert( !CORBA::is_nil( obj ) );
  
  return obj;
}

PortableServer::Servant Activator::incarnate( const PortableServer::ObjectId & _oid,
					      PortableServer::POA_ptr _poa )
{
  cerr << "Incarnating ...." << endl;
  
  /*
   * Incarnate the object
   */
  char buffer[ _oid.length() + 1 ];
  for( CORBA::ULong l = 0; l < _oid.length(); ++l )
    buffer[ l ] = _oid[ l ];
  buffer[ _oid.length() ] = 0;
  const char* p = buffer + strlen( buffer ) + 1;
  
  cerr << "Incarnating " << buffer << " with id " << p << endl;
  
  CORBA::Object_var obj = activate( buffer, p );
  if ( CORBA::is_nil( obj ) )
    return 0L;
  
  PortableServer::ForwardRequest fw;
  fw.forward_reference = obj;
  mico_throw( fw );
  
  // Never reached
  return 0L;
}

void Activator::etherealize( const PortableServer::ObjectId & oid,
			     PortableServer::POA_ptr poa,
			     PortableServer::Servant serv,
			     CORBA::Boolean cleanup_in_progress,
			     CORBA::Boolean remaining_activations)
{
  /*
   * If there are no remaining activations for that servant (which
   * actually could only happen with the MULTIPLE_ID policy, when
   * one servant can incarnate many objects), delete the account.
   */

  if ( !remaining_activations )
  {
    delete serv;
  }
}

CORBA::Object_ptr Activator::activate( const char *_server, const char *_repoid,
				       CORBA::ImplRepository_ptr _imr, const char *_addr )
{
  CORBA::ImplRepository_var imr;
  if ( _imr == 0L )
  {    
    CORBA::Object_var obj = _orbnc()->resolve_initial_references ("ImplementationRepository");
    imr = CORBA::ImplRepository::_narrow( obj );
    assert( !CORBA::is_nil( imr ) );
    _imr = imr;
  }

  CORBA::ImplRepository::ImplDefSeq_var impls = _imr->find_by_name( _server );
  if ( impls->length() == 0 )
  {
    cout << "no such server: " << _server << endl;
    return 0L;
  }
  assert (impls->length() == 1);

  CORBA::ORB_var orb = CORBA::ORB_instance ("mico-local-orb");
  CORBA::Object_var obj;
  if ( _addr )
  {
    // try given address
    obj = orb->bind ("IDL:omg.org/CORBA/OAMediator:1.0", _addr );
  }
  if (CORBA::is_nil (obj))
  {
    // try address of the impl repo
    const CORBA::Address *addr = imr->_ior()->addr();
    obj = orb->bind ("IDL:omg.org/CORBA/OAMediator:1.0", addr->stringify().c_str());
  }
  if (CORBA::is_nil( obj ) )
  {
    // try default addresses 
    obj = orb->bind ("IDL:omg.org/CORBA/OAMediator:1.0");
  }
  if ( CORBA::is_nil(obj) )
  {
    cout << "error: cannot connect to micod" << endl;
    return 0L;
  }
  CORBA::OAMediator_var oamed = CORBA::OAMediator::_narrow( obj );
  if ( !oamed->force_activation (impls[(CORBA::ULong)0]) )
  {
    cout << "error: cannot activate server " << _server << endl;
    return 0L;
  }

  // Lets bind to the requested server
  CORBA::ORB::ObjectTag_var tag = CORBA::ORB::string_to_tag( _server );
  obj = 0L;
  if( _addr )
    obj = orb->bind ( _repoid, tag, _addr );
  if( CORBA::is_nil( obj ) )
  {
    // try address of the impl repo
    const CORBA::Address *addr = imr->_ior()->addr();
    obj = orb->bind( _repoid, tag, addr->stringify().c_str());
  }
  if (CORBA::is_nil( obj ) )
    obj = orb->bind( _repoid, tag );

  if ( CORBA::is_nil( obj ) )
  {
    cout << "could not bind to server: " << _server << endl;
    cout << "Searching for id: " << _repoid << endl;
    return CORBA::Object::_nil();
  }
  /*
   * a bind() will only run the server the first time, but will not
   * reactivate a server. therefore we make an invocation that will
   * always force (re)activation.
   */
  obj->_non_existent ();

  return CORBA::Object::_duplicate( obj );
}










