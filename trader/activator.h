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
