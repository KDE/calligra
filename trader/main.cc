/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
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

