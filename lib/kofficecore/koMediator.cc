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

#include <string.h>
#include <stdlib.h>
#include <CORBA.h>
#include <algorithm>
#include <mico/impl.h>
#include <mico/util.h>
#include <mico/template_impl.h>
#include "koMediator.h"


/****************************** ObjectRec ****************************/


ObjectRec::ObjectRec (CORBA::Object_ptr local,
                      CORBA::Object_ptr remote,
		      const CORBA::BOA::ReferenceData &id,
		      ImplRec *irec,
                      ObjectState state)
{
    _local_obj = local;
    _remote_obj = remote;
    _id = id;
    _state = state;
    _server = 0;
    _impl = irec;
}

ObjectRec::~ObjectRec ()
{
    CORBA::release (_local_obj);
    CORBA::release (_remote_obj);
}

CORBA::Object_ptr
ObjectRec::local_obj ()
{
    return _local_obj;
}

CORBA::Object_ptr
ObjectRec::remote_obj ()
{
    return _remote_obj;
}

void
ObjectRec::remote_obj (CORBA::Object_ptr remote)
{
    CORBA::release (_remote_obj);
    _remote_obj = remote;
}

ObjectState
ObjectRec::state () const
{
    return _state;
}

void
ObjectRec::state (ObjectState state)
{
    _state = state;
}

ServerRec *
ObjectRec::server ()
{
    return _server;
}

void
ObjectRec::server (ServerRec *s)
{
    _server = s;
}

ImplRec *
ObjectRec::impl ()
{
    return _impl;
}

void
ObjectRec::impl (ImplRec *i)
{
    _impl = i;
}

const CORBA::BOA::ReferenceData &
ObjectRec::id () const
{
    return _id;
}


/****************************** ServerRec ****************************/


ServerRec::ServerId ServerRec::_theid = 0;

ServerRec::ServerRec (CORBA::OAMediator::ServerId id,
		      ImplRec *irec, ServerState state)
{
    _proc = 0;
    _id = id;
    _impl = irec;
    _state = state;
}

ServerRec::ServerRec (const char *args, CORBA::OAMediator::ServerId id,
		      ImplRec *irec, ServerState state)
{
    _id = id;
    _impl = irec;
    _state = state;
    _proc = new MICO::UnixProcess (args, this);
}

ServerRec::~ServerRec ()
{
    if (_proc) {
	MICO::Process *p = _proc;
	_proc = 0;
	delete p;
    }
}

ServerRec::ServerId
ServerRec::new_id ()
{
    if (_theid == 0)
	_theid = 1;
    return _theid++;
}

ServerRec::ServerId
ServerRec::id () const
{
    return _id;
}

ServerState
ServerRec::state () const
{
    return _state;
}

void
ServerRec::state (ServerState state)
{
    _state = state;
}

CORBA::OAServer_ptr
ServerRec::oaserver ()
{
    return _oaserver;
}

void
ServerRec::oaserver (CORBA::OAServer_ptr oaserver)
{
    _oaserver = CORBA::OAServer::_duplicate (oaserver);
}

MICO::Process *
ServerRec::process ()
{
    return _proc;
}

void
ServerRec::callback (MICO::Process *proc, MICO::ProcessCallback::Event ev)
{
    assert (ev == MICO::ProcessCallback::Exited);

    if (!_proc)
	// in destructor ...
	return;

    if (!proc->exit_status())
	_impl->server_crash (this);
    else
	_impl->server_exit (this);
}


/****************************** ImplRec ****************************/


ImplRec::ImplRec (MediatorImpl *med, CORBA::ImplementationDef_ptr impl)
    : _invqueue (med, med->_orbnc())
{
    _impldef = CORBA::ImplementationDef::_duplicate (impl);
    _med = med;

    CORBA::ORB_ptr orb = _med->_orbnc();
    CORBA::Object_var obj;
    CORBA::String_var s;

    // get interface repository IOR
    obj = orb->resolve_initial_references ("ImplementationRepository");
    assert (!CORBA::is_nil (obj));

    s = orb->object_to_string (obj);
    _std_args  = "-ORBImplRepoIOR ";
    _std_args += s;

    // get implementation repository IOR
    obj = orb->resolve_initial_references ("InterfaceRepository");
    assert (!CORBA::is_nil (obj));

    s = orb->object_to_string (obj);
    _std_args += " -ORBIfaceRepoIOR ";
    _std_args += s;

    // get mediator IOR
    s = orb->object_to_string (_med);
    _std_args += " -OARemoteIOR ";
    _std_args += s;

    // get impl name
    s = _impldef->name();
    _std_args += " -OAImplName ";
    _std_args += s;

    // cache activation mode 
    _amode = _impldef->mode();
}

ImplRec::~ImplRec ()
{
    // delete all objects
    for (MapObjectRec::iterator i0 = _objs.begin(); i0 != _objs.end(); ++i0)
        delete (*i0).second;

    // delete all pending invocations
    _invqueue.fail ();

    // kill all servers
    for (MapIdServer::iterator i1 = _servers.begin(); i1 != _servers.end(); ++i1)
	delete (*i1).second;
}

ImplRec *
ImplRec::create (MediatorImpl *med, CORBA::ImplementationDef_ptr impl)
{
    CORBA::ImplementationDef::ActivationMode amode = impl->mode();

    switch (amode) {
    case CORBA::ImplementationDef::ActivatePersistent:
	return new PersistentImplRec (med, impl);
	
    case CORBA::ImplementationDef::ActivateShared:
	return new SharedImplRec (med, impl);

    case CORBA::ImplementationDef::ActivateUnshared:
	return new UnsharedImplRec (med, impl);

    case CORBA::ImplementationDef::ActivatePerMethod:
	return new PermethodImplRec (med, impl);

    default:
        return 0;
    }
}

CORBA::ImplementationDef_ptr
ImplRec::impl ()
{
    return _impldef;
}

void
ImplRec::add_request (MICO::ReqQueueRec *inv)
{
    _invqueue.add (inv);
}

CORBA::Boolean
ImplRec::must_queue (MsgId id)
{
    if (_invqueue.size() == 0)
        return FALSE;
    if (_invqueue.iscurrent (id))
        return FALSE;
    /*
     * there are still invocations in the queue, queue to preserve
     * invocation oder
     */
    return TRUE;
}

void
ImplRec::add_object (ObjectRec *obj)
{
    assert (_objs.count (obj->local_obj()) == 0);
    _objs[obj->local_obj()] = obj;
    obj->impl (this);
}

void
ImplRec::del_object (ObjectRec *obj)
{
    _objs.erase (obj->local_obj());
}

ObjectRec *
ImplRec::find_object (CORBA::Object_ptr obj)
{
    MapObjectRec::iterator i = _objs.find (obj);
    return i != _objs.end() ? (*i).second : 0;
}

ObjectRec *
ImplRec::find_object (const char *repoid,
		      const CORBA::ORB::ObjectTag &tag)
{
    for (MapObjectRec::iterator i = _objs.begin(); i != _objs.end(); ++i) {
        // XXX do a more flexible repoid compare ...
        ObjectRec *orec = (*i).second;
        if (!strcmp (orec->local_obj()->_repoid(), repoid) &&
	    (tag.length() == 0 || tag == orec->id()))
            return orec;
    }
    return 0;
}

void
ImplRec::server_exit (ServerRec *sv)
{
    // orphan all objects
    for (MapObjectRec::iterator i = _objs.begin(); i != _objs.end(); ++i) {
	ObjectRec *orec = (*i).second;
	if (orec->server() == sv) {
	    orec->state (ObjectOrphaned);
	    orec->server (0);
	}
    }

    // remove server
    _servers.erase (sv->id());
    delete sv;

    exec_requests ();
}

void
ImplRec::server_crash (ServerRec *sv)
{
    /*
     * a server belonging to this implementation exited
     * abonormally, cancel all outstanding invocations
     * for this implementation.
     * already issued invocations will result in some
     * SystemException because the connection to the server
     * will break down.
     */
    CORBA::String_var name = _impldef->name();
    CORBA::String_var cmd = _impldef->command();
    cerr << "*** server exited abnormally or could not be run:" << endl
	 << "    name: " << name.in() << endl
	 << " command: " << cmd.in() << endl;

    // make all pending invokes fail ...
    _invqueue.fail ();

    server_exit (sv);
}

ServerRec *
ImplRec::find_server (ServerId svid)
{
    MapIdServer::iterator i = _servers.find (svid);
    return i != _servers.end() ? (*i).second : 0;
}

void
ImplRec::exec_requests ()
{
    // schedule reexecution of pending requests ...
    _invqueue.exec_later ();
}

void
ImplRec::create_obj (ObjectRec *orec, ServerId svid)
{
    ServerRec *sv = find_server (svid);
    assert (sv);

    add_object (orec);
    orec->server (sv);
    orec->state (ObjectInactive);
}

void
ImplRec::restore_obj (ObjectRec *orec, ServerId svid)
{
    ServerRec *sv = find_server (svid);
    assert (sv);

    orec->server (sv);
    orec->state (ObjectInactive);
}

void
ImplRec::activate_obj (ObjectRec *orec, ServerId svid)
{
    orec->state (ObjectActive);
    exec_requests ();
}

void
ImplRec::deactivate_obj (ObjectRec *orec, ServerId svid)
{
    ObjectState ostate = orec->state();
    orec->state (ObjectInactive);
    if (ostate == ObjectActive)
	orec->server()->oaserver()->obj_inactive (orec->remote_obj());

    exec_requests ();
}

void
ImplRec::migrate_obj (ObjectRec *orec, ServerId svid, ImplRec *irec)
{
    if (this == irec)
	return;

    // orphan object and move it to new implementation
    orec->state (ObjectOrphaned);
    orec->server (0);
    del_object (orec);
    irec->add_object (orec);

    /*
     * now comes the tricky part: some of the invocations queued to the
     * old implementation might affect the moved object. these have to
     * be moved to the new implementation. this is done by reissuing all
     * the queued requests; the BOAServer will then schedule all the requests
     * to the right implementation ...
     * we cannot do this here, instead we do the same trick as in running().
     */
    exec_requests ();
}

void
ImplRec::orphan_obj (ObjectRec *orec, ServerId svid)
{
    orec->state (ObjectOrphaned);
    exec_requests ();
}

void
ImplRec::dispose_obj (ObjectRec *orec, ServerId svid)
{
    del_object (orec);
    delete orec;

    exec_requests ();
}

void
ImplRec::create_impl (CORBA::OAServer_ptr oaserv, ServerId &svid)
{
    ServerRec *sv = find_server (svid);
    if (!sv) {
	// happens for servers not activated by mediator
	svid = ServerRec::new_id();
	sv = new ServerRec (svid, this, ServerInactive);
	_servers[svid] = sv;
    }

    sv->oaserver (oaserv);
    sv->state (ServerInactive);
}

void
ImplRec::activate_impl (ServerId svid)
{
    ServerRec *sv = find_server (svid);
    assert (sv);

    sv->state (ServerActive);
    exec_requests ();
}

void
ImplRec::deactivate_impl (ServerId svid)
{
    ServerRec *sv = find_server (svid);
    assert (sv);

    ServerState ostate = sv->state ();
    sv->state (ServerInactive);
    if (ostate == ServerActive || ostate == ServerInactive)
	sv->oaserver()->impl_inactive ();

    exec_requests ();
}

void
ImplRec::dispose_impl (ServerId svid)
{
    ServerRec *sv = find_server (svid);
    assert (sv);

    // orphan all objects
    for (MapObjectRec::iterator i = _objs.begin(); i != _objs.end(); ++i) {
	ObjectRec *orec = (*i).second;
	if (orec->server() == sv) {
	    orec->state (ObjectOrphaned);
	    orec->server (0);
	}
    }
    sv->state (ServerStopped);

    exec_requests ();
}

CORBA::OAMediator::ObjSeq *
ImplRec::get_restore_objs (ServerId svid)
{
    CORBA::OAMediator::ObjSeq *robjs = new CORBA::OAMediator::ObjSeq;
    CORBA::ULong nrobjs = 0;

    for (MapObjectRec::iterator i = _objs.begin(); i != _objs.end(); ++i) {
	ObjectRec *orec = (*i).second;
	if (orec->state() == ObjectRestoring && orec->server()->id() == svid) {
	    robjs->length (nrobjs+1);
	    (*robjs)[nrobjs] = CORBA::Object::_duplicate (orec->local_obj());
	    ++nrobjs;
	}
    }
    return robjs;
}

const ImplRec::MapIdServer &
ImplRec::servers () const
{
    return _servers;
}

string
ImplRec::server_args ()
{
    CORBA::String_var s = _impldef->command();

    string cmd = (const char *)s;
    cmd += " ";
    cmd += _std_args;

    return cmd;
}

ServerRec *
ImplRec::create_server ()
{
    ServerId svid = ServerRec::new_id();
    
    string cmds = server_args();
    cmds += " -OAServerId ";
    cmds += xdec (svid);

    ServerRec *sv = new ServerRec (cmds.c_str(), svid, this, ServerStarted);
    _servers[svid] = sv;

    return sv;
}


/************************** PersistentImplRec ************************/


PersistentImplRec::PersistentImplRec (MediatorImpl *med,
				      CORBA::ImplementationDef_ptr impl)
    : ImplRec (med, impl)
{
}

PersistentImplRec::~PersistentImplRec ()
{
}

CORBA::OAMediator::ObjSeq *
PersistentImplRec::restore_objects (ServerRec *sv)
{
    CORBA::OAMediator::ObjSeq *robjs = new CORBA::OAMediator::ObjSeq;
    CORBA::ULong nrobjs = 0;

    for (MapObjectRec::iterator i = _objs.begin(); i != _objs.end(); ++i) {
	ObjectRec *orec = (*i).second;
	if (orec->state() == ObjectOrphaned) {
	    orec->server (sv);
	    orec->state (ObjectRestoring);

	    robjs->length (nrobjs+1);
	    (*robjs)[nrobjs] = CORBA::Object::_duplicate (orec->local_obj());
	    ++nrobjs;
	}
    }
    return robjs;
}

PolicyStatus
PersistentImplRec::before_invoke (ObjectRec *orec, CORBA::ORBRequest *,
				  CORBA::Principal_ptr)
{
    switch (orec->state()) {
    case ObjectOrphaned:
	if (_servers.empty()) {
	    // cannot activate servers ...
	    return PolicyAbort;
	} else {
	    ServerRec *sv = (*_servers.begin()).second;
	    if (sv->state() == ServerActive) {
		CORBA::OAMediator::ObjSeq *robjs = restore_objects (sv);
		sv->oaserver()->restore_request (*robjs);
		delete robjs;
	    }
	}
	return PolicyRetry;

    case ObjectActive:
	if (orec->server()->state() == ServerActive)
	    return PolicyContinue;
	return PolicyRetry;

    default:
	return PolicyRetry;
    }
}

PolicyStatus
PersistentImplRec::after_invoke (ObjectRec *orec, CORBA::ORBRequest *,
				 CORBA::Principal_ptr)
{
    return PolicyContinue;
}

PolicyStatus
PersistentImplRec::before_bind (const char *repoid,
				const CORBA::ORB::ObjectTag &tag)
{
    ObjectRec *orec = find_object (repoid, tag);
    if (orec)
	return before_invoke (orec, 0, 0);

    if (!_objs.empty()) {
	orec = (*_objs.begin()).second;
	return before_invoke (orec, 0, 0);
    }

    if (_servers.empty())
	// cannot activate servers ...
	return PolicyAbort;

    ServerRec *sv = (*_servers.begin()).second;
    if (sv->state() == ServerActive)
	return PolicyContinue;

    return PolicyRetry;
}


/************************** SharedImplRec ************************/


SharedImplRec::SharedImplRec (MediatorImpl *med,
			      CORBA::ImplementationDef_ptr impl)
    : ImplRec (med, impl)
{
}

SharedImplRec::~SharedImplRec ()
{
}

CORBA::OAMediator::ObjSeq *
SharedImplRec::restore_objects (ServerRec *sv)
{
    CORBA::OAMediator::ObjSeq *robjs = new CORBA::OAMediator::ObjSeq;
    CORBA::ULong nrobjs = 0;

    for (MapObjectRec::iterator i = _objs.begin(); i != _objs.end(); ++i) {
	ObjectRec *orec = (*i).second;
	if (orec->state() == ObjectOrphaned) {
	    orec->server (sv);
	    orec->state (ObjectRestoring);

	    robjs->length (nrobjs+1);
	    (*robjs)[nrobjs] = CORBA::Object::_duplicate (orec->local_obj());
	    ++nrobjs;
	}
    }
    return robjs;
}

PolicyStatus
SharedImplRec::before_invoke (ObjectRec *orec, CORBA::ORBRequest *,
			      CORBA::Principal_ptr)
{
    switch (orec->state()) {
    case ObjectOrphaned:
	if (_servers.empty()) {
	    ServerRec *sv = create_server ();
	    CORBA::OAMediator::ObjSeq_var robjs = restore_objects (sv);

	    CORBA::Boolean r = sv->process()->run();
	    assert (r);
	} else {
	    ServerRec *sv = (*_servers.begin()).second;
	    if (sv->state() == ServerActive) {
		CORBA::OAMediator::ObjSeq *robjs = restore_objects (sv);
		sv->oaserver()->restore_request (*robjs);
		delete robjs;
	    }
	}
	return PolicyRetry;

    case ObjectActive:
	if (orec->server()->state() == ServerActive)
	    return PolicyContinue;
	return PolicyRetry;

    default:
	return PolicyRetry;
    }
}

PolicyStatus
SharedImplRec::after_invoke (ObjectRec *orec, CORBA::ORBRequest *,
			     CORBA::Principal_ptr)
{
    return PolicyContinue;
}

PolicyStatus
SharedImplRec::before_bind (const char *repoid,
			    const CORBA::ORB::ObjectTag &tag)
{
    ObjectRec *orec = find_object (repoid, tag);
    if (orec)
	return before_invoke (orec, 0, 0);

    if (!_objs.empty()) {
	orec = (*_objs.begin()).second;
	return before_invoke (orec, 0, 0);
    }

    if (_servers.empty()) {
	ServerRec *sv = create_server ();

	CORBA::Boolean r = sv->process()->run();
	assert (r);

	return PolicyRetry;
    }

    ServerRec *sv = (*_servers.begin()).second;
    if (sv->state() == ServerActive)
	return PolicyContinue;

    return PolicyRetry;
}


/************************** UnsharedImplRec ************************/


UnsharedImplRec::UnsharedImplRec (MediatorImpl *med,
				  CORBA::ImplementationDef_ptr impl)
    : ImplRec (med, impl)
{
}

UnsharedImplRec::~UnsharedImplRec ()
{
}

ServerRec *
UnsharedImplRec::find_empty_server (ServerRec *except_this_one)
{
    set<ServerRec *, less<ServerRec *> > used_servers;

    for (ImplRec::MapObjectRec::iterator i0 = _objs.begin();
	 i0 != _objs.end(); ++i0) {
        if ((*i0).second->state() != ObjectOrphaned)
            used_servers.insert ((*i0).second->server());
    }

    for (ImplRec::MapIdServer::iterator i1 = _servers.begin();
	 i1 != _servers.end(); ++i1) {
        if ((*i1).second != except_this_one &&
	    used_servers.count ((*i1).second) == 0)
            return (*i1).second;
    }
    return 0;
}

ServerRec *
UnsharedImplRec::find_active_server ()
{
    for (MapIdServer::iterator i = _servers.begin(); i != _servers.end(); ++i){
        if ((*i).second->state() == ServerActive)
            return (*i).second;
    }
    return 0;
}

PolicyStatus
UnsharedImplRec::before_invoke (ObjectRec *orec, CORBA::ORBRequest *,
				CORBA::Principal_ptr)
{
    switch (orec->state()) {
    case ObjectOrphaned: {
        ServerRec *sv = find_empty_server (orec->server());
        if (!sv) {
	    sv = create_server ();

            orec->server (sv);
            orec->state (ObjectRestoring);

	    CORBA::Boolean r = sv->process()->run();
	    assert (r);
	} else if (sv->state() == ServerActive) {
            orec->server (sv);
            orec->state (ObjectRestoring);

            CORBA::OAMediator::ObjSeq robjs;
            robjs.length (1);
            robjs[0] = CORBA::Object::_duplicate (orec->local_obj());

            sv->oaserver()->restore_request (robjs);
	}
	return PolicyRetry;
    }
    case ObjectActive:
	if (orec->server()->state() == ServerActive)
	    return PolicyContinue;
	return PolicyRetry;

    default:
	return PolicyRetry;
    }
}

PolicyStatus
UnsharedImplRec::after_invoke (ObjectRec *orec, CORBA::ORBRequest *,
			       CORBA::Principal_ptr)
{
    return PolicyContinue;
}

PolicyStatus
UnsharedImplRec::before_bind (const char *repoid,
			      const CORBA::ORB::ObjectTag &tag)
{
    ObjectRec *orec = find_object (repoid, tag);
    if (orec)
	return before_invoke (orec, 0, 0);

    if (!_objs.empty()) {
	orec = (*_objs.begin()).second;
	return before_invoke (orec, 0, 0);
    }

    if (_servers.empty()) {
	ServerRec *sv = create_server ();

	CORBA::Boolean r = sv->process()->run();
	assert (r);

	return PolicyRetry;
    }

    if (find_active_server ())
        return PolicyContinue;

    return PolicyRetry;
}


/************************** PermethodImplRec ************************/


PermethodImplRec::PermethodImplRec (MediatorImpl *med,
				    CORBA::ImplementationDef_ptr impl)
    : ImplRec (med, impl)
{
}

PermethodImplRec::~PermethodImplRec ()
{
}

ServerRec *
PermethodImplRec::find_active_server ()
{
    for (MapIdServer::iterator i = _servers.begin(); i != _servers.end(); ++i){
        if ((*i).second->state() == ServerActive)
            return (*i).second;
    }
    return 0;
}

PolicyStatus
PermethodImplRec::before_invoke (ObjectRec *orec, CORBA::ORBRequest *,
				 CORBA::Principal_ptr)
{
    switch (orec->state()) {
    case ObjectOrphaned: {
        ServerRec *sv = create_server ();

        orec->server (sv);
        orec->state (ObjectRestoring);

        CORBA::Boolean r = sv->process()->run();
        assert (r);

	return PolicyRetry;
    }
    case ObjectActive:
	if (orec->server()->state() == ServerActive)
	    return PolicyContinue;
	return PolicyRetry;

    default:
	return PolicyRetry;
    }
}

PolicyStatus
PermethodImplRec::after_invoke (ObjectRec *orec, CORBA::ORBRequest *,
				CORBA::Principal_ptr)
{
    orec->server()->state (ServerInactive);
    return PolicyContinue;
}

PolicyStatus
PermethodImplRec::before_bind (const char *repoid,
			       const CORBA::ORB::ObjectTag &tag)
{
    ObjectRec *orec = find_object (repoid, tag);
    if (orec)
	return before_invoke (orec, 0, 0);

    if (!_objs.empty()) {
	orec = (*_objs.begin()).second;
	return before_invoke (orec, 0, 0);
    }


    if (_servers.empty()) {
	ServerRec *sv = create_server ();

	CORBA::Boolean r = sv->process()->run();
	assert (r);

	return PolicyRetry;
    }

    if (find_active_server ())
        return PolicyContinue;

    return PolicyRetry;
}


/****************************** MediatorImpl ****************************/


MediatorImpl::MediatorImpl ()
{
    CORBA::Object_var obj
        = _orbnc()->resolve_initial_references ("ImplementationRepository");
    assert (!CORBA::is_nil (obj));

    _imr = CORBA::ImplRepository::_narrow (obj);
    assert (!CORBA::is_nil (_imr));

    _theid = 1;

    _orbnc()->register_oa (this);
}

MediatorImpl::~MediatorImpl ()
{
    _orbnc()->unregister_oa (this);

    for (ListImpl::iterator i = _impls.begin(); i != _impls.end(); ++i)
        delete *i;
}

ImplRec *
MediatorImpl::find_impl (CORBA::ImplementationDef_ptr impl)
{
    for (ListImpl::iterator i = _impls.begin(); i != _impls.end(); ++i) {
        if (impl->_is_equivalent ((*i)->impl()))
            return *i;
    }
    return 0;
}

ImplRec *
MediatorImpl::find_impl (ServerId svid)
{
    for (ListImpl::iterator i = _impls.begin(); i != _impls.end(); ++i) {
        if ((*i)->find_server (svid))
            return *i;
    }
    return 0;
}

ImplRec *
MediatorImpl::find_impl (const char *name)
{
    for (ListImpl::iterator i = _impls.begin(); i != _impls.end(); ++i) {
        CORBA::String_var impl_name
            = (*i)->impl()->name();
        if (!strcmp (name, impl_name))
            return *i;
    }
    return 0;
}

ObjectRec *
MediatorImpl::find_obj (CORBA::Object_ptr obj)
{
    for (ListImpl::iterator i = _impls.begin(); i != _impls.end(); ++i) {
        ObjectRec *orec = (*i)->find_object (obj);
        if (orec)
            return orec;
    }
    return 0;
}

void
MediatorImpl::unique_id (vector<CORBA::Octet> &id)
{
    if (++_theid == 0)
	_theid = 1;

    CORBA::ULong l = _theid;
    while (l) {
	id.push_back ((CORBA::Octet)l);
	l >>= 8;
    }

    id.push_back ('O');
    id.push_back ('A');
    id.push_back ('D');

    vector<CORBA::Octet> hostid = MICO::InetAddress::hostid();
    id.insert (id.end(), hostid.begin(), hostid.end());

    l = OSMisc::getpid();
    id.push_back ((CORBA::Octet)l);
    id.push_back ((CORBA::Octet)(l >>= 8));
    id.push_back ((CORBA::Octet)(l >>= 8));
    id.push_back ((CORBA::Octet)(l >>= 8));
}

const char *
MediatorImpl::get_oaid () const
{
    return "mico-oa-mediator";
}

CORBA::Boolean
MediatorImpl::has_object (CORBA::Object_ptr obj)
{
    return !!find_obj (obj);
}

CORBA::Boolean
MediatorImpl::is_local () const
{
    return FALSE;
}

CORBA::Boolean
MediatorImpl::invoke (MsgId id, CORBA::Object_ptr obj,
		      CORBA::ORBRequest *req,
		      CORBA::Principal_ptr pr,
		      CORBA::Boolean response_exp)
{
    ObjectRec *orec = find_obj (obj);
    if (!orec) {
	/*
	 * object has been deleted inbetween the call to has_object()
	 * and now, because we had to queue the invocation ...
	 */
        CORBA::OBJECT_NOT_EXIST ex;
	req->set_out_args (&ex);
	_orbnc()->answer_invoke (id, CORBA::InvokeSysEx,
				 CORBA::Object::_nil(), req);
	return TRUE;
    }
    ImplRec *irec = orec->impl();

    if (irec->must_queue (id)) {
        // queue the invocation for later
        irec->add_request (
	    new MICO::ReqQueueRec (id, req, obj, pr, response_exp));
	return TRUE;
    }

    PolicyStatus pstat = irec->before_invoke (orec, req, pr);
    switch (pstat) {
    case PolicyAbort: {
        CORBA::OBJ_ADAPTER ex;
	req->set_out_args (&ex);
	_orbnc()->answer_invoke (id, CORBA::InvokeSysEx,
				 CORBA::Object::_nil(), req);
	return TRUE;
    }
    case PolicyRetry:
        // queue the invocation for later
        irec->add_request (
	    new MICO::ReqQueueRec (id, req, obj, pr, response_exp));
	return TRUE;

    case PolicyContinue:
	break;

    default:
	assert (0);
    }

    // do the invocation (on the remote object)
    MsgId orbid = _orbnc()->new_msgid();
    if (response_exp)
	_requests[orbid] = id;
    _orbnc()->invoke_async (orec->remote_obj(), req, pr, response_exp,
			    this, orbid);

    pstat = irec->after_invoke (orec, req, pr);
    switch (pstat) {
    case PolicyContinue:
	break;

    default:
	assert (0);
    }
    return TRUE;
}

CORBA::Boolean
MediatorImpl::bind (MsgId id, const char *repoid,
		    const CORBA::ORB::ObjectTag &tag,
		    CORBA::Address *addr)
{
    if (addr && !addr->is_local())
        return FALSE;

    CORBA::ImplRepository::ImplDefSeq_var ims = _imr->find_by_repoid (repoid);

    /*
     * make sure all implementations that implement object with
     * the requested repoid have an active server ...
     */
    for (CORBA::ULong i0 = 0; i0 < ims->length(); ++i0) {
        if (ims[i0]->mode() != CORBA::ImplementationDef::ActivateShared &&
	    ims[i0]->mode() != CORBA::ImplementationDef::ActivateUnshared &&
	    ims[i0]->mode() != CORBA::ImplementationDef::ActivatePerMethod &&
	    ims[i0]->mode() != CORBA::ImplementationDef::ActivatePersistent &&
	    ims[i0]->mode() != CORBA::ImplementationDef::ActivateLibrary) {
	  continue;
	}

	ImplRec *irec = find_impl (ims[i0]);
	if (!irec) {
	    irec = ImplRec::create (this, ims[i0]);
	    assert (irec);
	    _impls.push_back (irec);
	}

        if (irec->must_queue (id)) {
	    irec->add_request (new MICO::ReqQueueRec (id, repoid, tag));
	    return TRUE;
        }

	PolicyStatus pstat = irec->before_bind (repoid, tag);
	switch (pstat) {
	case PolicyAbort:
	    _orbnc()->answer_bind (id, CORBA::LocateUnknown,
				   CORBA::Object::_nil());
	    return TRUE;
	    
	case PolicyRetry:
	    irec->add_request (new MICO::ReqQueueRec (id, repoid, tag));
	    return TRUE;

	case PolicyContinue:
	    break;

	default:
	    assert (0);
	}
    }

    /*
     * all implementations that implement objects with requested
     * repoid have a running server. send a bind "broadcast" ...
     */

    /*
     * allocate slots for all bind requests, so that
     * we can decide in the callback() whether there are more
     * outstanding requests or not.
     */

    vector<MsgId> msgids;
    CORBA::ULong count = 0;

    for (CORBA::ULong i1 = 0; i1 < ims->length(); ++i1) {
        if (ims[i1]->mode() != CORBA::ImplementationDef::ActivateShared &&
	    ims[i1]->mode() != CORBA::ImplementationDef::ActivateUnshared &&
	    ims[i1]->mode() != CORBA::ImplementationDef::ActivatePerMethod &&
	    ims[i1]->mode() != CORBA::ImplementationDef::ActivatePersistent &&
	    ims[i1]->mode() != CORBA::ImplementationDef::ActivateLibrary) {
	  continue;
	}

	ImplRec *irec = find_impl (ims[i1]);
	assert (irec);

	const ImplRec::MapIdServer &svs = irec->servers();
	ImplRec::MapIdServer::const_iterator j;
	for (j = svs.begin(); j != svs.end(); ++j) {
            if ((*j).second->state() == ServerActive) {
                // only active servers ...
		msgids.push_back (_orbnc()->new_msgid());
		_requests[msgids.back()] = id;
	    }
	    count++;
	}
    }

    if (!count) {
      /*
       * No servers available
       */
      return FALSE;
    }

    /*
     * now send the requests
     */
    for (CORBA::ULong i2 = 0; i2 < ims->length(); ++i2) {
        if (ims[i2]->mode() != CORBA::ImplementationDef::ActivateShared &&
	    ims[i2]->mode() != CORBA::ImplementationDef::ActivateUnshared &&
	    ims[i2]->mode() != CORBA::ImplementationDef::ActivatePerMethod &&
	    ims[i2]->mode() != CORBA::ImplementationDef::ActivatePersistent &&
	    ims[i2]->mode() != CORBA::ImplementationDef::ActivateLibrary) {
	  continue;
	}

	ImplRec *irec = find_impl (ims[i2]);
	assert (irec);

	const ImplRec::MapIdServer &svs = irec->servers();
	ImplRec::MapIdServer::const_iterator j;
	for (j = svs.begin(); j != svs.end(); ++j) {
            if ((*j).second->state() != ServerActive)
                // only active servers ...
                continue;

	    CORBA::OAServer_ptr oaserv = (*j).second->oaserver();
	    assert (!CORBA::is_nil (oaserv));

	    CORBA::Address *addr = (CORBA::Address *)oaserv->_ior()->addr();
	    assert (addr);

	    assert (msgids.size() > 0);
	    MsgId orbid = _orbnc()->bind_async (repoid, tag, addr, this,
						msgids.back());
	    msgids.pop_back();

	    assert (orbid != 0);
	}
    }
    return TRUE;
}

CORBA::Boolean
MediatorImpl::locate (MsgId id, CORBA::Object_ptr obj)
{
    assert (find_obj (obj));

    // XXX do a forward if impl is persistent ???
    // XXX pass on locate to impl BOA ???

    _orbnc()->answer_locate (id, CORBA::LocateHere, CORBA::Object::_nil());
    return TRUE;
}

CORBA::Object_ptr
MediatorImpl::skeleton (CORBA::Object_ptr)
{
    assert (0);
    return CORBA::Object::_nil ();
}

void
MediatorImpl::cancel (MsgId id)
{
    int again = 1;
    while (again) {
	again = 0;
	MapIdId::iterator i;
	for (i = _requests.begin(); i != _requests.end(); ++i) {
	    if ((*i).second == id) {
		_orbnc()->cancel ((*i).first);
		_requests.erase (i);
		again = 1;
		break;
	    }
	}
    }
}

void
MediatorImpl::shutdown (CORBA::Boolean wait_for_completion)
{
    // XXX how to wait for completion ??
    _orbnc()->answer_shutdown (this);
}

void
MediatorImpl::answer_invoke (CORBA::ULong, CORBA::Object_ptr,
			     CORBA::ORBRequest *, CORBA::InvokeStatus)
{
    assert (0);
}

void
MediatorImpl::copy_code_sets (CORBA::IOR *ior2, CORBA::IOR *ior1)
{
    CORBA::IORProfile *prof1 = 0, *prof2 = 0;
    CORBA::MultiComponent *mc1 = 0, *mc2 = 0;
    CORBA::Component *csc1 = 0, *csc2 = 0;

    prof1 = ior1->profile (CORBA::IORProfile::TAG_MULTIPLE_COMPONENTS, TRUE);
    if (prof1) {
	mc1 = ((MICO::MultiCompProfile *)prof1)->components();
	csc1 = mc1->component (CORBA::Component::TAG_CODE_SETS);
    }

    prof2 = ior2->profile (CORBA::IORProfile::TAG_MULTIPLE_COMPONENTS, TRUE);
    if (csc1 && !prof2) {
	prof2 = new MICO::MultiCompProfile (CORBA::MultiComponent());
	ior2->add_profile (prof2);
    }
    if (prof2) {
	mc2 = ((MICO::MultiCompProfile *)prof2)->components();
	csc2 = mc2->component (CORBA::Component::TAG_CODE_SETS);
	if (csc2) {
	    mc2->del_component (csc2);
	    delete csc2;
	}
    }
    if (csc1)
	mc2->add_component (csc1->clone());
}

void
MediatorImpl::create_obj (CORBA::Object_ptr remote_obj,
			  const CORBA::OAMediator::RefData& id,
			  CORBA::Object_ptr& local_obj,
			  CORBA::OAMediator::ServerId svid)
{
    ImplRec *irec = find_impl (svid);
    assert (irec);

    vector<CORBA::Octet> key;
    unique_id (key);

    CORBA::IOR *ior = new CORBA::IOR (*_orbnc()->ior_template());
    ior->objectkey (&key[0], key.size());
    ior->objid (remote_obj->_repoid());

    /*
     * make sure local object has the same code sets like
     * remote object by copying the MultiCompProfile.
     *
     * XXX we have to be able to convert between our native code
     * set and the code set selected by the client ...
     */
    copy_code_sets (ior, remote_obj->_ior());

    local_obj = new CORBA::Object (ior);

    ObjectRec *orec = new ObjectRec (CORBA::Object::_duplicate (local_obj),
				     CORBA::Object::_duplicate (remote_obj),
				     id, irec, ObjectOrphaned);
    irec->create_obj (orec, svid);
}

void
MediatorImpl::restore_obj (CORBA::Object_ptr remote_obj,
			   CORBA::Object_ptr& local_obj,
			   CORBA::OAMediator::RefData*& id,
			   CORBA::OAMediator::ServerId svid)
{
    ObjectRec *orec = find_obj (local_obj);
    assert (orec);

    ImplRec *irec = orec->impl();
    assert (irec);

    orec->remote_obj (CORBA::Object::_duplicate (remote_obj));
    id = new RefData (orec->id());

    irec->restore_obj (orec, svid);
}

void
MediatorImpl::activate_obj (CORBA::Object_ptr objref,
			    CORBA::OAMediator::ServerId svid)
{
    ObjectRec *orec = find_obj (objref);
    assert (orec);

    orec->impl()->activate_obj (orec, svid);
}

void
MediatorImpl::deactivate_obj (CORBA::Object_ptr objref,
			      CORBA::OAMediator::ServerId svid)
{
    ObjectRec *orec = find_obj (objref);
    assert (orec);

    orec->impl()->deactivate_obj (orec, svid);
}

void
MediatorImpl::migrate_obj (CORBA::Object_ptr objref,
			   CORBA::OAMediator::ServerId svid,
			   CORBA::ImplementationDef_ptr impl)
{
    /*
     * XXX perhaps do a check whether 'impl' implements objects of kind
     * 'objref' ...
     */
    ObjectRec *orec = find_obj (objref);
    assert (orec);

    ImplRec *irec = find_impl (impl);
    if (!irec) {
	irec = ImplRec::create (this, impl);
	assert (irec);
	_impls.push_back (irec);
    }
    orec->impl()->migrate_obj (orec, svid, irec);
}

void
MediatorImpl::orphan_obj (CORBA::Object_ptr objref,
                          CORBA::OAMediator::ServerId svid)
{
    ObjectRec *orec = find_obj (objref);
    assert (orec);
    orec->impl()->orphan_obj (orec, svid);
}

void
MediatorImpl::dispose_obj (CORBA::Object_ptr objref,
			   CORBA::OAMediator::ServerId svid)
{
    ObjectRec *orec = find_obj (objref);
    if (orec)
	orec->impl()->dispose_obj (orec, svid);
}

void
MediatorImpl::create_impl (CORBA::ImplementationDef_ptr impl,
			   CORBA::OAServer_ptr server,
			   CORBA::OAMediator::ServerId &id)
{
    ImplRec *irec = find_impl (impl);
    if (!irec) {
	// this happens for servers not activated by the mediator
	irec = ImplRec::create (this, impl);
	assert (irec);
	_impls.push_back (irec);
    }
    irec->create_impl (server, id);
}

void
MediatorImpl::activate_impl (CORBA::OAMediator::ServerId id)
{
    ImplRec *irec = find_impl (id);
    assert (irec);

    irec->activate_impl (id);
}

void
MediatorImpl::deactivate_impl (CORBA::OAMediator::ServerId id)
{
    ImplRec *irec = find_impl (id);
    assert (irec);

    irec->deactivate_impl (id);
}

void
MediatorImpl::dispose_impl (CORBA::OAMediator::ServerId id)
{
    ImplRec *irec = find_impl (id);
    assert (irec);

    irec->dispose_impl (id);
}

CORBA::OAMediator::ObjSeq*
MediatorImpl::get_restore_objs (CORBA::OAMediator::ServerId id)
{
    ImplRec *irec = find_impl (id);
    if (!irec)
	return new CORBA::OAMediator::ObjSeq;

    return irec->get_restore_objs (id);
}

CORBA::Boolean
MediatorImpl::force_activation (CORBA::ImplementationDef_ptr impl)
{
    ImplRec *irec = find_impl (impl);
    if (!irec) {
	irec = ImplRec::create (this, impl);
	if (!irec) {
	  return FALSE;
	}
	_impls.push_back (irec);
    }

    PolicyStatus pstat = irec->before_bind ("", CORBA::ORB::ObjectTag());
    switch (pstat) {
    case PolicyAbort:
	return FALSE;
	    
    case PolicyContinue:
    case PolicyRetry:
	return TRUE;

    default:
	assert (0);
	return FALSE;
    }
}

void
MediatorImpl::callback (CORBA::ORB_ptr, MsgId id,
			CORBA::ORBCallback::Event ev)
{
    switch (ev) {
    case CORBA::ORBCallback::Invoke: {
	// an invocation completed ...
	CORBA::ORBRequest *req;
	CORBA::Object_var obj;
	CORBA::InvokeStatus stat = _orbnc()->get_invoke_reply (id, obj, req);

	MapIdId::iterator i = _requests.find (id);
	assert (i != _requests.end());

	_orbnc()->answer_invoke ((*i).second, stat, obj, req);

	_requests.erase (i);
	break;
    }
    case CORBA::ORBCallback::Bind: {
	// a bind completed ...
	CORBA::Object_var obj;
	CORBA::LocateStatus stat = _orbnc()->get_bind_reply (id, obj);

	MapIdId::iterator i = _requests.find (id);
	assert (i != _requests.end());

	MsgId id2 = (*i).second;
	_requests.erase (i);

	if (stat == CORBA::LocateHere) {
	    // found matching object ...
	    _orbnc()->answer_bind (id2, stat, obj);
	    // cancel all binds resulting from the same bind "broadcast"
	    cancel (id2);
	} else {
	    /*
	     * not found, see if there are more outstanding requests
	     * from the same bind "broadcast" ...
	     */
	    for (i = _requests.begin(); i != _requests.end(); ++i) {
		if ((*i).second == id2)
		    break;
	    }

	    if (i == _requests.end()) {
		// ... its the last bind request; answer anyway
		_orbnc()->answer_bind (id2, stat, obj);
	    }

	    // ignore, there are more outstanding bind requests ...
	}
	break;
    }
    default:
	assert (0);
    }
}

