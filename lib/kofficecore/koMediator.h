// -*- c++ -*-
/*
 *  MICO --- a free CORBA implementation
 *  Copyright (C) 1997-98 Kay Roemer & Arno Puder
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Send comments and/or bug reports to:
 *                 mico@informatik.uni-frankfurt.de
 */


#ifndef __ko_mediator_h__
#define __ko_mediator_h__

#include <CORBA.h>
#include <mico/impl.h>

//-------------------------

class ObjectRec;
class ServerRec;
class ImplRec;
class MediatorImpl;

//-------------------------

enum ObjectState {
    ObjectOrphaned,
    ObjectRestoring,
    ObjectInactive,
    ObjectActive
};

class ObjectRec {
    CORBA::Object_ptr _local_obj;
    CORBA::Object_ptr _remote_obj;
    CORBA::BOA::ReferenceData _id;
    ObjectState _state;
    ServerRec *_server;
    ImplRec *_impl;
public:
    ObjectRec (CORBA::Object_ptr local,
	       CORBA::Object_ptr remote,
	       const CORBA::BOA::ReferenceData &id,
	       ImplRec *impl, ObjectState);
    ~ObjectRec ();

    CORBA::Object_ptr local_obj ();
    CORBA::Object_ptr remote_obj ();
    void remote_obj (CORBA::Object_ptr);

    ObjectState state () const;
    void state (ObjectState);

    ServerRec *server ();
    void server (ServerRec *);

    ImplRec *impl ();
    void impl (ImplRec *);

    const CORBA::BOA::ReferenceData &id () const;
};

//-------------------------

enum ServerState {
    ServerStopped,
    ServerStarted,
    ServerInactive,
    ServerActive
};

class ServerRec : public MICO::ProcessCallback {
public:
    typedef CORBA::OAMediator::ServerId ServerId;
private:
    CORBA::OAServer_var _oaserver;
    ServerId _id;
    MICO::Process *_proc;
    ImplRec *_impl;
    ServerState _state;
    static ServerId _theid;
public:
    ServerRec (CORBA::OAMediator::ServerId, ImplRec *, ServerState);
    ServerRec (const char *args, CORBA::OAMediator::ServerId, ImplRec *,
	       ServerState);
    ~ServerRec ();

    static ServerId new_id ();

    ServerId id () const;

    ServerState state () const;
    void state (ServerState);

    CORBA::OAServer_ptr oaserver ();
    void oaserver (CORBA::OAServer_ptr);

    MICO::Process *process ();

    // process callback 
    virtual void callback (MICO::Process *, MICO::ProcessCallback::Event);
};

//-------------------------

enum PolicyStatus {
    PolicyAbort,
    PolicyRetry,
    PolicyContinue
};

class ImplRec {
public:
    typedef CORBA::ULong MsgId;
    typedef CORBA::OAMediator::ServerId ServerId;
    typedef map<ServerId, ServerRec *, less<ServerId> > MapIdServer;

    struct objcomp : binary_function<CORBA::Object *, CORBA::Object *, bool> {
	bool operator() (CORBA::Object *o1, CORBA::Object *o2) const
	{
	    CORBA::Long l1, l2;
	    // XXX this assumes the keys are the same for all profiles ...
	    const CORBA::Octet *k1 = o1->_ior()->profile()->objectkey (l1);
	    const CORBA::Octet *k2 = o2->_ior()->profile()->objectkey (l2);
	    if (l1 != l2)
		return l1 < l2;
	    return memcmp (k1, k2, l1) < 0;
	}
    };
    typedef map<CORBA::Object *, ObjectRec *, objcomp> MapObjectRec;
protected:
    MapIdServer _servers;
    MapObjectRec _objs;
    MediatorImpl *_med;
    MICO::RequestQueue _invqueue;
    CORBA::ImplementationDef_var _impldef;
    CORBA::ImplementationDef::ActivationMode _amode;
    string _std_args;

    string server_args ();
    ServerRec *create_server ();
public:
    static ImplRec *create (MediatorImpl *, CORBA::ImplementationDef_ptr);

    ImplRec (MediatorImpl *, CORBA::ImplementationDef_ptr);
    virtual ~ImplRec ();

    void server_exit (ServerRec *);
    void server_crash (ServerRec *);
    ServerRec *find_server (ServerId);
    const MapIdServer &servers () const;

    CORBA::ImplementationDef_ptr impl ();

    void add_request (MICO::ReqQueueRec *);
    void exec_requests ();
    CORBA::Boolean must_queue (MsgId);

    void add_object (ObjectRec *);
    void del_object (ObjectRec *);
    ObjectRec *find_object (CORBA::Object_ptr obj);
    ObjectRec *find_object (const char *repoid,
			    const CORBA::ORB::ObjectTag &tag);
    
    void create_obj (ObjectRec *, ServerId);
    void restore_obj (ObjectRec *, ServerId);
    void activate_obj (ObjectRec *, ServerId);
    void deactivate_obj (ObjectRec *, ServerId);
    void migrate_obj (ObjectRec *, ServerId, ImplRec *);
    void dispose_obj (ObjectRec *, ServerId);
    void orphan_obj (ObjectRec *, ServerId);

    void create_impl (CORBA::OAServer_ptr, ServerId &);
    void activate_impl (ServerId);
    void deactivate_impl (ServerId);
    void dispose_impl (ServerId);

    CORBA::OAMediator::ObjSeq *get_restore_objs (ServerId);

    // activation policy
    virtual PolicyStatus before_invoke (ObjectRec *, CORBA::ORBRequest *,
					CORBA::Principal_ptr) = 0;
    virtual PolicyStatus after_invoke (ObjectRec *, CORBA::ORBRequest *,
				       CORBA::Principal_ptr) = 0;
    virtual PolicyStatus before_bind (const char *repoid,
				      const CORBA::ORB::ObjectTag &tag) = 0;
};

class PersistentImplRec : public ImplRec {
    CORBA::OAMediator::ObjSeq *restore_objects (ServerRec *);
public:
    PersistentImplRec (MediatorImpl *, CORBA::ImplementationDef_ptr);
    ~PersistentImplRec ();
    
    PolicyStatus before_invoke (ObjectRec *, CORBA::ORBRequest *,
    				CORBA::Principal_ptr);
    PolicyStatus after_invoke (ObjectRec *, CORBA::ORBRequest *,
    			       CORBA::Principal_ptr);
    PolicyStatus before_bind (const char *repoid,
			      const CORBA::ORB::ObjectTag &tag);
};

class SharedImplRec : public ImplRec {
    CORBA::OAMediator::ObjSeq *restore_objects (ServerRec *);
public:
    SharedImplRec (MediatorImpl *, CORBA::ImplementationDef_ptr);
    ~SharedImplRec ();
    
    PolicyStatus before_invoke (ObjectRec *, CORBA::ORBRequest *,
    				CORBA::Principal_ptr);
    PolicyStatus after_invoke (ObjectRec *, CORBA::ORBRequest *,
    			       CORBA::Principal_ptr);
    PolicyStatus before_bind (const char *repoid,
			      const CORBA::ORB::ObjectTag &tag);
};

class UnsharedImplRec : public ImplRec {
    ServerRec *find_empty_server (ServerRec *except_this_one);
    ServerRec *find_active_server ();
public:
    UnsharedImplRec (MediatorImpl *, CORBA::ImplementationDef_ptr);
    ~UnsharedImplRec ();
    
    PolicyStatus before_invoke (ObjectRec *, CORBA::ORBRequest *,
    				CORBA::Principal_ptr);
    PolicyStatus after_invoke (ObjectRec *, CORBA::ORBRequest *,
    			       CORBA::Principal_ptr);
    PolicyStatus before_bind (const char *repoid,
			      const CORBA::ORB::ObjectTag &tag);
};

class PermethodImplRec : public ImplRec {
    ServerRec *find_active_server ();
public:
    PermethodImplRec (MediatorImpl *, CORBA::ImplementationDef_ptr);
    ~PermethodImplRec ();
    
    PolicyStatus before_invoke (ObjectRec *, CORBA::ORBRequest *,
    				CORBA::Principal_ptr);
    PolicyStatus after_invoke (ObjectRec *, CORBA::ORBRequest *,
    			       CORBA::Principal_ptr);
    PolicyStatus before_bind (const char *repoid,
			      const CORBA::ORB::ObjectTag &tag);
};

//-------------------------

class MediatorImpl : public CORBA::ObjectAdapter,
		     public CORBA::OAMediator_skel,
		     public CORBA::ORBCallback {
public:
    typedef CORBA::OAMediator::ServerId ServerId;
    typedef list<ImplRec *> ListImpl;
    typedef map<MsgId, MsgId, less<MsgId> > MapIdId;
private:
    CORBA::ImplRepository_var _imr;
    CORBA::ULong _theid;
    ListImpl _impls;
    MapIdId _requests;

    ImplRec *find_impl (CORBA::ImplementationDef_ptr);
    ImplRec *find_impl (const char *name);
    ImplRec *find_impl (ServerId);
    ObjectRec *find_obj (CORBA::Object_ptr);
    void unique_id (vector<CORBA::Octet> &);

    void copy_code_sets (CORBA::IOR *to, CORBA::IOR *from);
public:
    MediatorImpl ();
    ~MediatorImpl ();

    // ObjectAdapter stuff 
    const char *get_oaid () const;
    CORBA::Boolean has_object (CORBA::Object_ptr);
    CORBA::Boolean is_local () const;
    
    CORBA::Boolean invoke (MsgId, CORBA::Object_ptr,
    			   CORBA::ORBRequest *,
    			   CORBA::Principal_ptr,
    			   CORBA::Boolean response_exp = TRUE);
    CORBA::Boolean bind (MsgId, const char *repoid,
    			 const CORBA::ORB::ObjectTag &,
    			 CORBA::Address *addr);
    CORBA::Boolean locate (MsgId, CORBA::Object_ptr);
    CORBA::Object_ptr skeleton (CORBA::Object_ptr);
    void cancel (MsgId);
    void shutdown (CORBA::Boolean wait_for_completion);
    
    void answer_invoke (CORBA::ULong, CORBA::Object_ptr,
			CORBA::ORBRequest *,
			CORBA::InvokeStatus);

    // RemoteBOA stuff
    void create_obj (CORBA::Object_ptr objref,
    		     const CORBA::OAMediator::RefData& id,
    		     CORBA::Object_ptr& remote_objref,
    		     CORBA::OAMediator::ServerId svid);
    void restore_obj (CORBA::Object_ptr objref,
    		      CORBA::Object_ptr& remote_objref,
    		      CORBA::OAMediator::RefData*& id,
    		      CORBA::OAMediator::ServerId svid);
    void activate_obj (CORBA::Object_ptr objref,
    		       CORBA::OAMediator::ServerId svid);
    void deactivate_obj (CORBA::Object_ptr objref,
    			 CORBA::OAMediator::ServerId svid);
    void migrate_obj (CORBA::Object_ptr objref,
    		      CORBA::OAMediator::ServerId svid,
    		      CORBA::ImplementationDef_ptr impl);
    void orphan_obj (CORBA::Object_ptr objref,
                     CORBA::OAMediator::ServerId svid);
    void dispose_obj (CORBA::Object_ptr objref,
    		      CORBA::OAMediator::ServerId svid);
    void create_impl (CORBA::ImplementationDef_ptr impl,
    		      CORBA::OAServer_ptr server,
    		      CORBA::OAMediator::ServerId &id);
    void activate_impl (CORBA::OAMediator::ServerId id);
    void deactivate_impl (CORBA::OAMediator::ServerId id);
    void dispose_impl (CORBA::OAMediator::ServerId id);
    CORBA::OAMediator::ObjSeq* get_restore_objs (
	CORBA::OAMediator::ServerId id);

    // admin interface
    CORBA::Boolean force_activation (CORBA::ImplementationDef_ptr impl);

    // ORBCallback
    void callback (CORBA::ORB_ptr,
		   MsgId,
		   CORBA::ORBCallback::Event);
};


#endif // __mediator_h__
