#ifndef __activator_h__
#define __activator_h__

#include <CORBA.h>

class Activator : public virtual POA_PortableServer::ServantActivator
{
public:
  Activator();
  
  PortableServer::Servant incarnate( const PortableServer::ObjectId &,
				     PortableServer::POA_ptr );
  void etherealize( const PortableServer::ObjectId & oid,
		    PortableServer::POA_ptr poa,
		    PortableServer::Servant serv,
		    CORBA::Boolean cleanup_in_progress,
		    CORBA::Boolean remaining_activations);
  
  CORBA::Object_ptr createReference( const char *_implreponame, const char* _repoid );

  CORBA::Object_ptr activate( const char *_implreponame, const char *_repoid,
			      CORBA::ImplRepository_ptr _imr = 0L, const char *_addr = 0L );
 
  static void initStatic( CORBA::ORB_ptr );
  static Activator* self();
  
protected:
  static PortableServer::POA_var s_vPoa;
  static Activator* s_pSelf;
};

#endif
