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

#include "koscript_struct.h"
#include "koscript_util.h"
#include "koscript_property.h"
#include "koscript_method.h"
#include "koscript.h"

#include <klocale.h>

/***************************************************
 *
 * KSStructClass
 *
 ***************************************************/

KSStructClass::KSStructClass( KSModule* m, const QString& name )
  : m_name( name ), m_module( m )
{
  m_space.insert( "isA", new KSValue( &KSStruct::isA ) );
}

bool KSStructClass::constructor( KSContext& context )
{
  context.setValue( new KSValue( constructor() ) );

  return true;
}

KSStruct* KSStructClass::constructor()
{
  return new KSStruct( this );
}

KSValue::Ptr KSStructClass::member( KSContext& context, const QString& name )
{
  KSNamespace::Iterator it = m_space.find( name );
  if ( it == m_space.end() )
  {
    QString tmp( i18n("Unknown symbol '%1' in struct of type %2 of module '%3'") );
    context.setException( new KSException( "UnknownName", tmp.arg( name ).arg( m_name ).arg( module()->name() ) ) );
    return 0;
  }

  return it.data();
}

QString KSStructClass::fullName() const
{
    return ( m_module->name() + ":" + m_name );
}

/***************************************************
 *
 * KSStruct
 *
 ***************************************************/

bool KSStruct::isA( KSContext& context )
{
  if ( !KSUtil::checkArgumentsCount( context, 0, "Struct::isA" ) )
    return false;

  context.setValue( new KSValue( m_class->name() ) );

  return true;
}

KSValue::Ptr KSStruct::member( KSContext& context, const QString& name )
{
  if ( context.leftExpr() )
  {
    this->ref();
    KSValue::Ptr ptr( new KSValue( new KSProperty( this, name ) ) );
    ptr->setMode( KSValue::LeftExpr );
    return ptr;
  }

  KSNamespace::Iterator it = m_space.find( name );
  if ( it != m_space.end() )
    return it.data();

  it = m_class->nameSpace()->find( name );
  if ( it != m_class->nameSpace()->end() )
    return it.data();

  QString tmp( i18n("Unknown symbol '%1' in object of struct '%2'") );
  context.setException( new KSException( "UnknownName", tmp.arg( name ).arg( getClass()->name() ) ) );
  return 0;
}

bool KSStruct::setMember( KSContext& context, const QString& name, const KSValue::Ptr& v )
{
  if ( !m_class->vars().contains( name ) )
  {
    QString tmp( i18n("Unknown symbol '%1' in object of struct '%2'") );
    context.setException( new KSException( "UnknownName", tmp.arg( name ).arg( getClass()->name() ) ) );
    return false;
  }

  m_space.insert( name, v );

  return true;
}

bool KSBuiltinStructClass::hasMethod( const QString& name ) const
{
    return m_methods.contains( name );
}

/***************************************************
 *
 * KSBuiltinStructClass
 *
 ***************************************************/

KSBuiltinStructClass::KSBuiltinStructClass( KSModule* module, const QString& name )
    : KSStructClass( module, name )
{
}

void KSBuiltinStructClass::addMethod( const QString& name, KSBuiltinStructClass::MethodPtr m, const QCString& signature )
{
    Method s;
    s.m_method = m;
    s.m_signature = signature;
    m_methods[ name ] = s;
}

bool KSBuiltinStructClass::call( void* object, KSContext& context, const QString& name )
{
    QMap<QString,Method>::Iterator it = m_methods.find( name );
    Q_ASSERT( it != m_methods.end() );

    if ( !it.data().m_signature.isNull() )
	if ( !KSUtil::checkArgs( context, it.data().m_signature, name, TRUE ) )
	    return FALSE;

    return it.data().m_method( object, context, context.value()->listValue() );
}

/***************************************************
 *
 * KSBuiltinStruct
 *
 ***************************************************/

KSBuiltinStruct::KSBuiltinStruct( KSStructClass* c, void* object )
    : KSStruct( c )
{
    m_object = object;
}

KSBuiltinStruct::~KSBuiltinStruct()
{
    ((KSBuiltinStructClass*)getClass())->destructor( m_object );
}

KSValue::Ptr KSBuiltinStruct::member( KSContext& context, const QString& name )
{
    if ( context.leftExpr() )
    {
	this->ref();
	KSValue::Ptr ptr( new KSValue( new KSProperty( this, name ) ) );
	ptr->setMode( KSValue::LeftExpr );
	return ptr;
    }

    // Is it a method ?
    if ( ((KSBuiltinStructClass*)getClass())->hasMethod( name ) )
	return ( new KSValue( (KSStructBuiltinMethod)&KSBuiltinStruct::call ) );

    // Look in the KSStructClass namespace
    KSNamespace::Iterator it = getClass()->nameSpace()->find( name );
    if ( it != getClass()->nameSpace()->end() )
	return it.data();

    // Is it a variable ?
    if ( getClass()->hasVariable( name ) )
	return( ((KSBuiltinStructClass*)getClass())->property( context, m_object, name ) );

    QString tmp( i18n("Unknown symbol '%1' in object of struct '%2'") );
    context.setException( new KSException( "UnknownName", tmp.arg( name ).arg( getClass()->name() ) ) );
    return 0;
}

bool KSBuiltinStruct::setMember( KSContext& context, const QString& name, const KSValue::Ptr& v )
{
    if ( !getClass()->vars().contains( name ) )
    {
	QString tmp( i18n("Unknown variable '%1' in object of struct '%2'") );
	context.setException( new KSException( "UnknownName", tmp.arg( name ).arg( getClass()->name() ) ) );
	return FALSE;
    }

    bool b = ((KSBuiltinStructClass*)getClass())->setProperty( context, m_object, name, v );

    // Some exception ? -> return
    if ( !b && context.exception() )
	return FALSE;
    // Standard error: The variable is readonly
    if ( !b )
    {
	QString tmp( i18n("The variable '%1' in object of struct '%2' is readonly") );
	context.setException( new KSException( "ReadOnly", tmp.arg( name ).arg( getClass()->name() ) ) );
	return FALSE;
    }

    return TRUE;
}

bool KSBuiltinStruct::call( KSContext& context, const QString& name )
{
    return ((KSBuiltinStructClass*)getClass())->call( m_object, context, name );
}

KSStruct* KSBuiltinStruct::clone()
{
    return ((KSBuiltinStructClass*)getClass())->clone( this );
}

void* KSBuiltinStruct::object()
{
    return m_object;
}

const void* KSBuiltinStruct::object() const
{
    return m_object;
}
