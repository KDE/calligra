#include "kscript_struct.h"
#include "kscript_util.h"
#include "kscript_object.h"

/***************************************************
 *
 * KSStructClass
 *
 ***************************************************/

KSStructClass::KSStructClass( KSModule* m, const QString& name, const KSParseNode* n )
  : m_name( name ), m_node( n ), m_module( m )
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
    QString tmp( "Unknown symbol '%1' in struct of type %2 of module '%3'" );
    context.setException( new KSException( "UnknownName", tmp.arg( name ).arg( m_name ).arg( module()->name() ) ) );
    return 0;
  }

  return it.data();
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

  QString tmp( "Unknown symbol '%1' in object of struct '%2'" );
  context.setException( new KSException( "UnknownName", tmp.arg( name ).arg( getClass()->name() ) ) );
  return 0;
}

bool KSStruct::setMember( KSContext& context, const QString& name, const KSValue::Ptr& v )
{
  if ( !m_class->vars().contains( name ) )
  {
    QString tmp( "Unknown symbol '%1' in object of struct '%2'" );
    context.setException( new KSException( "UnknownName", tmp.arg( name ).arg( getClass()->name() ) ) );
    return false;
  }

  m_space.insert( name, v );
  
  return true;
}
