/* This file is part of the KDE libraries
    Copyright (C) 1998 Torben Weis <weis@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include "koIMR.h"
#include "koMediator.h"

#include <klocale.h>
#include <kapp.h>

#include <opApplication.h>
#include "koScanParts.h"
#include <qmsgbox.h>

/**
 * This code is inspired by micos "imr" implementation.
 * The original code is copyrighted by 
 * (c) 1997 Kay Roemer & Arno Puder
 *
 * Modifications and extensions done by Torben Weis
 * (c) 1998 Torben Weis <weis@kde.org>
 */

CORBA::OAMediator_ptr med;

void imr_init()
{
  med = new MediatorImpl;
}

bool imr_create( const char* _name, const char* _mode, const char *_exec, QStrList &_repoids, CORBA::ImplRepository_ptr _imr )
{
  CORBA::ImplRepository::ImplDefSeq_var impls = _imr->find_by_name( _name );
  if ( impls->length() > 0 )
  {
    cerr << "there is already a server called '"
	 << _name << "', please choose a unique name!"
	 << endl;
    return false;
  }

  CORBA::ImplementationDef::ActivationMode mode;
  if ( strcmp( _mode, "persistent" ) == 0 )
    mode = CORBA::ImplementationDef::ActivatePersistent;
  else if ( strcmp( _mode, "shared") == 0 )
    mode = CORBA::ImplementationDef::ActivateShared;
  else if ( strcmp( _mode, "unshared") == 0 )
    mode = CORBA::ImplementationDef::ActivateUnshared;
  else if ( strcmp( _mode, "permethod") == 0 )
    mode = CORBA::ImplementationDef::ActivatePerMethod;
  else if ( strcmp( _mode, "library") == 0 )
    mode = CORBA::ImplementationDef::ActivateLibrary;
  else
  {
    cerr << "invalid activation mode: " << _mode << endl;
    cerr << "valid modes are: persistent, shared, unshared, "
	 << "permethod, library"
	 << endl;
    return false;
  }

  CORBA::ImplementationDef::RepoIdList repoids;
  repoids.length( _repoids.count() );
  for ( unsigned int i = 0; i < _repoids.count(); i++ )
    repoids[i] = (const char*)_repoids.at(i);

  CORBA::ImplementationDef_var impl = _imr->create( mode, repoids, _name, _exec );
  assert (!CORBA::is_nil (impl));
  return 0;
}

CORBA::Object_ptr imr_activate( const char *_server, CORBA::ImplRepository_ptr _imr, const char *_addr )
{
  /* CORBA::ImplRepository::ImplDefSeq_var impls = _imr->find_by_name( _server );
  if ( impls->length() == 0 )
  {
    cout << "no such server: " << args[0] << endl;
    return 0L;
  }
  assert (impls->length() == 1);

  CORBA::ORB_var orb = CORBA::ORB_instance ("mico-local-orb");
  CORBA::Object_var obj;
  if (args.size() > 1)
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
    cout << "error: cannot activate server " << args[0] << endl;
    return 0L;
  }
    return 0;
    */


    CORBA::ImplRepository_var imr;
    if ( _imr == 0L )
    {    
      CORBA::Object_var obj = opapp_orb->resolve_initial_references ("ImplementationRepository");
      imr = CORBA::ImplRepository::_narrow( obj );
      assert( !CORBA::is_nil( imr ) );
      _imr = imr;
    }

    CORBA::ImplRepository::ImplDefSeq_var impls
	= _imr->find_by_name( _server );
    if ( impls->length() == 0 )
    {
      cout << "no such server: " << _server << endl;
      return CORBA::Object::_nil();
    }

    assert (impls->length() == 1);
    CORBA::ImplementationDef::RepoIdList_var repoids = impls[0]->repoids();
    if ( repoids->length() == 0 )
    {
      cout << "server " << _server << " implements no objects?!" << endl;
      return CORBA::Object::_nil();
    }

    CORBA::ORB::ObjectTag_var tag = CORBA::ORB::string_to_tag( _server );

    CORBA::ORB_var orb = CORBA::ORB_instance ("mico-local-orb");
    CORBA::Object_var obj;
    if ( _addr )
    {
	// try given address
	obj = orb->bind( repoids[0], tag, _addr );
    }
    if (CORBA::is_nil (obj))
    {
	// try address of the impl repo
	const CORBA::Address *addr = _imr->_ior()->addr();
	obj = orb->bind (repoids[0], tag, addr->stringify().c_str());
    }
    if ( CORBA::is_nil( obj ) )
    {
	// try default addresses 
	obj = orb->bind (repoids[0], tag );
    }
    if ( CORBA::is_nil( obj ) )
    {
      cout << "could not activate server: " << _server << endl;
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

KOffice::Document_ptr imr_createDocByServerName( const char *_server_name )
{
  CORBA::Object_var obj = imr_activate( _server_name );
  if ( CORBA::is_nil( obj ) )
  {
    QString tmp;
    tmp.sprintf( i18n("Could not start server %s" ), _server_name );
    QMessageBox::critical( (QWidget*)0L, i18n("KSpread Error"), tmp, i18n( "Ok" ) );
    return 0L;
  }
  
  KOffice::DocumentFactory_var factory = KOffice::DocumentFactory::_narrow( obj );
  assert( !CORBA::is_nil( factory ) );
  KOffice::Document_ptr doc = factory->create();
  assert( !CORBA::is_nil( doc ) );
  
  return doc;
}

KOffice::Document_ptr imr_createDocByMimeType( const char *_mime_type )
{
  QListIterator<KoPartEntry> it( *g_plstPartEntries );
  for( ; it.current(); ++it )
  {
    if ( it.current()->supports( _mime_type ) )
      return imr_createDocByServerName( it.current()->name() );
  }

  return 0L;
}
