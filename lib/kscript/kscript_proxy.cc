#define WITH_CORBA
#include <CORBA.h>

#include "kscript_struct.h"
#include "kscript_util.h"
#include "kscript_object.h"

/***************************************************
 *
 * KSProxy
 *
 ***************************************************/

KSProxy::KSProxy( KSProxy* proxy )
{
  init( proxy->interface(), proxy->object() );
}

KSProxy::KSProxy( KSInterface* iface, void* obj )
{
  init( iface, obj );
}

KSProxy::~KSProxy()
{
  if ( m_object )
    m_object->_deref();
}

void KSProxy::init( KSInterface* iface, void* obj )
{
  m_object = (CORBA::Object*)obj;
  m_object->_ref();
  m_interface = iface;

  QValueList<KSValue::Ptr> super;
  iface->allSuperInterfaces( super );

  QValueList<KSValue::Ptr>::Iterator it = super.fromLast();
  for( ; it != super.end(); --it )
    m_scope.pushNamespace( (*it)->interfaceValue()->nameSpace() );
  m_scope.pushNamespace( iface->nameSpace() );

  // This has to be the last one!
  m_scope.pushNamespace( &m_space );
};

bool KSProxy::isA( KSContext& context )
{
  if ( !KSUtil::checkArgumentsCount( context, 0, "Proxy::isA" ) )
    return false;

  context.setValue( new KSValue( m_interface->name() ) );

  return true;
}

bool KSProxy::inherits( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "Proxy::inherits" ) )
    return false;
  
  if ( !KSUtil::checkType( context, args[0], KSValue::StringType ) )
    return false;

  QString name = args[0]->stringValue();

  QValueList<KSValue::Ptr> super;
  m_interface->allSuperInterfaces( super );
  m_interface->ref();
  super.append( new KSValue( m_interface ) );

  QValueList<KSValue::Ptr>::Iterator it = super.begin();
  for(; it != super.end(); ++it )
    if ( (*it)->interfaceValue()->name() == name )
    {
      context.setValue( new KSValue( true ) );
      return true;
    }

  context.setValue( new KSValue( false ) );

  return true;
}

KSValue::Ptr KSProxy::member( KSContext& context, const QString& name )
{
  if ( context.leftExpr() )
  {
    this->ref();
    KSValue::Ptr ptr( new KSValue( new KSProperty( this, name ) ) );
    ptr->setMode( KSValue::LeftExpr );
    return ptr;
  }

  KSValue *v = m_scope.object( name, false );
  if ( !v )
  {
    QString tmp( "Unknown symbol '%1' in object of proxy '%2'" );
    context.setException( new KSException( "UnknownName", tmp.arg( name ).arg( interface()->name() ) ) );
    return 0;
  }

  v->ref();
  return KSValue::Ptr( v );
}

bool KSProxy::setMember( KSContext&, const QString& name, const KSValue::Ptr& v )
{
  // If no such member is present then it is created
  KSValue* x = m_scope.object( name, true );
  ASSERT( x );
  *x = *v;

  return true;
}
