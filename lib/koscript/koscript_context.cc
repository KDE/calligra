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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "koscript_context.h"
#include "koscript_parsenode.h"
#include "koscript_property.h"

#include <stdio.h>

/*************************************************
 *
 * KSContext
 *
 *************************************************/

KSContext::KSContext()
{
  m_bLeftExpr = false;
  m_bReturning = false;
}

KSContext::KSContext( KSContext& c, bool leftexpr )
{
  setScope( c );
  m_bLeftExpr = leftexpr;
  m_bReturning = false;
}

KSContext::~KSContext()
{
  // You may NOT change the order of the following two lines!
  m_exception = 0;
  m_value = 0;
  m_extraData = 0;
}

/*************************************************
 *
 * KSException
 *
 *************************************************/

KSException::KSException( const QString& _type, const KSValue::Ptr& _ptr, int _line )
{
  m_type = new KSValue( _type );
  m_value = _ptr;
  if ( _line >= 0 )
    m_lines.append( _line );
}

KSException::KSException( const QString& _type, const QString& _val, int _line )
{
  m_type = new KSValue( _type );
  m_value = new KSValue( _val );
  if ( _line >= 0 )
    m_lines.append( _line );
}

KSException::KSException( const KSValue::Ptr& _type, const KSValue::Ptr& _ptr, int _line )
{
  m_type = _type;
  m_value = _ptr;
  if ( _line >= 0 )
    m_lines.append( _line );
}

void KSException::print( KSContext& context )
{
  printf("%s\n",toString( context ).local8Bit().data());
}

QString KSException::toString( KSContext& context )
{
  QString out("Exception '%1'\n%3\n");

  if ( m_value )
    out = out.arg( m_type->toString( context ) ).arg( m_value->toString( context ) );
  else
    out = out.arg( m_type->toString( context ) ).arg( "" );

  QValueList<int>::ConstIterator it = lines().begin();
  for( ; it != lines().end(); ++it )
  {
    QString l( "\nLine %1" );
    l = l.arg( *it );
    out += l;
  }

  return out;
}

/*************************************************
 *
 * KSScope
 *
 *************************************************/

KSScope::KSScope( const KSNamespace* globalSpace, KSModule* module )
{
  Q_ASSERT( globalSpace );

  m_module = module;
  m_globalSpace = globalSpace;
  if ( m_module )
    m_moduleSpace = m_module->nameSpace();
  else
    m_moduleSpace = 0;
  m_localScope = 0;
}

KSScope::KSScope( const KSScope& s ) : QShared()
{
  m_module = s.m_module;
  m_globalSpace = s.m_globalSpace;
  m_moduleSpace = s.m_moduleSpace;
  m_localScope = s.m_localScope;
}

KSValue* KSScope::object( const QString& name, bool _insert )
{
  int len = name.length();

  // Look in global namespace ?
  if ( len > 2 && name[0] == ':' && name[1] == ':' )
  {
    // TODO
  }
  else if ( name.find( ':' ) == -1 )
  {
    if ( m_localScope )
    {
      KSValue *v = m_localScope->object( name, false );
      if ( v )
        return v;
    }

    if ( m_moduleSpace )
    {
      KSNamespace::Iterator it = m_moduleSpace->find( name );
      if ( it != m_moduleSpace->end() )
        return it.data();
    }

    KSNamespace::ConstIterator it = m_globalSpace->find( name );
    if ( it != m_globalSpace->end() )
    {
      KSSharedPtr<KSValue> ptr( it.data() );
      return ptr;
    }

    // let's search again, now using case-insensitive comparison
    // this way, function names in KSpread become case-insensitive
    if( m_moduleSpace )
    {
      KSNamespace::ConstIterator it = m_moduleSpace->begin();
      for( ; it != m_moduleSpace->end(); ++it )
        if( it.key().lower() == name.lower() )
          {
            KSSharedPtr<KSValue> ptr( it.data() );
            return ptr;
          }
      }

    it = m_globalSpace->begin();
    for( ; it != m_globalSpace->end(); ++it )
      if( it.key().lower() == name.lower() )
        {
          KSSharedPtr<KSValue> ptr( it.data() );
          return ptr;
        }

    if ( !_insert )
      return 0;

    KSValue::Ptr v = new KSValue();
    v->setMode( KSValue::LeftExpr );
    addObject( name, v );
    return v;
  }
  else
  {
    // TODO
  }

  return 0;
}

void KSScope::addObject( const QString& name, const KSValue::Ptr& value )
{
  if( m_localScope )
    m_localScope->addObject( name, value );
  else if( m_module )
    m_module->addObject( name, value );
  else
    Q_ASSERT( 0 );
}

/*************************************************
 *
 * KSModule
 *
 *************************************************/

KSModule::KSModule( KSInterpreter* i, const QString& name, KSParseNode* code )
  : QShared(), m_name( name ), m_code( code ), m_interpreter( i )
{
}

KSModule::~KSModule()
{
  if ( m_code )
    delete m_code;
}

void KSModule::addObject( const QString& name, const KSValue::Ptr& v )
{
  m_space.insert( name, v );
}

void KSModule::removeObject( const QString& name )
{
  m_space.remove( name );
}


KSValue::Ptr KSModule::member( KSContext& context, const QString& name )
{
  KSNamespace::Iterator it = m_space.find( name );
  if ( it == m_space.end() )
  {
    if ( context.leftExpr() )
    {
        this->ref();
        KSValue::Ptr ptr( new KSValue( new KSProperty( this, name ) ) );
        ptr->setMode( KSValue::LeftExpr );
        return ptr;
        // KSValue::Ptr ptr( new KSValue );
        // ptr->setMode( KSValue::LeftExpr );
        // return ptr;
    }

    QString tmp( "Unknown symbol '%1' in object of module '%2'" );
    context.setException( new KSException( "UnknownName", tmp.arg( name ).arg( m_name ) ) );
    return 0;
  }

  return it.data();
}

bool KSModule::setMember( KSContext&, const QString& name, const KSValue::Ptr& v )
{
  m_space.insert( name, v );

  return true;
}

bool KSModule::eval( KSContext& context )
{
  if ( !m_code )
    return false;

  return m_code->eval( context );
}

KSValue* KSModule::object( const QString& name )
{
  KSNamespace::Iterator nit = m_space.find( name );
  if ( nit != m_space.end() )
    return nit.data();

  return 0;
}

void KSModule::setCode( KSParseNode* node )
{
    if ( m_code )
        delete m_code;

    m_code = node;
}

/**************************************************
 *
 * KSSubScope
 *
 **************************************************/

KSValue* KSSubScope::object( const QString& name, bool insert )
{
  QPtrListIterator<KSNamespace> it( m_spaces );
  it.toLast();
  for( ; it.current(); --it )
  {
    KSNamespace::Iterator nit = it.current()->find( name );
    if ( nit != it.current()->end() )
      return nit.data();
  }

  if ( !insert )
    return 0;

  KSValue* v = new KSValue();
  v->setMode( KSValue::LeftExpr );
  addObject( name, v );
  return v;
}

void KSSubScope::addObject( const QString& name, const KSValue::Ptr& value )
{
  m_spaces.getLast()->insert( name, value );
}
