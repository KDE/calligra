/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#if 0

#define WITH_CORBA
#include <CORBA.h>

#include "kscript_interface.h"
#include "kscript_proxy.h"
#include "kscript_util.h"

extern CORBA::ORB* orb();

/**********************************
 *
 * KSAttribute
 *
 **********************************/

KSAttribute::KSAttribute( KSModule* m, const QString& name, KSAttribute::Access a, const KSTypeCode::Ptr& tc )
{
  m_module = m;
  m_name = name;
  m_typecode = tc;
  m_access = a;
}

void KSAttribute::set( KSContext&, KSProxy*, KSValue* )
{
  // TODO
}

KSValue::Ptr KSAttribute::get( KSContext&, KSProxy* )
{
  // TODO
    return 0; // to avoid warnings
}

/**********************************
 *
 * KSInterface
 *
 **********************************/

KSInterface::KSInterface( KSModule* m, const QString& _name )
{
  m_name = _name;
  m_module = m;

  // Register member functions
  m_space.insert( "isA", new KSValue( &KSProxy::isA ) );
  m_space.insert( "inherits", new KSValue( &KSProxy::inherits ) );
}

bool KSInterface::constructor( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, m_name + "::" + m_name ) )
    return false;

  if ( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
  {
    // TODO: We must avoid this with certain security policies.
    // Embedded KOffice scripts for example may not talk to any CORBA
    // object in the system -> we dont allow to use IORs at all.
    CORBA::Object_ptr obj = orb()->string_to_object( args[0]->stringValue() );
    if ( CORBA::is_nil( obj ) )
    {
      QString tmp( i18n("The IOR seems to be malformed or invalid") );
      context.setException( new KSException( "InvalidIOR", tmp.arg( args[0]->stringValue() ) ) );
      return false;
    }

    context.setValue( new KSValue( new KSProxy( this, obj ) ) );
    obj->_deref();

    return true;
  }
  else if ( !KSUtil::checkType( context, args[0], KSValue::ProxyType ) )
    return false;

  context.setValue( new KSValue( new KSProxy( args[0]->proxyValue() ) ) );

  return true;
}

KSProxy* KSInterface::constructor( void* obj )
{
  return new KSProxy( this, obj );
}

bool KSInterface::allSuperInterfaces( QValueList<KSValue::Ptr>& lst )
{
  QValueList<KSValue::Ptr>::Iterator it = m_superInterfaces.begin();
  for( ; it != m_superInterfaces.end(); ++it )
  {
    lst.append( *it );
    if ( ! (*it)->type() == KSValue::InterfaceType )
      return false;

    (*it)->interfaceValue()->allSuperInterfaces( lst );
  }

  return true;
}

KSValue::Ptr KSInterface::member( KSContext& context, const QString& name )
{
  KSNamespace::Iterator it = m_space.find( name );
  if ( it == m_space.end() )
  {
    QString tmp( i18n("Unknown symbol '%1' in object of module '%2'") );
    context.setException( new KSException( "UnknownName", tmp.arg( name ).arg( m_name ) ) );
    return 0;
  }

  return it.data();
}

#endif
