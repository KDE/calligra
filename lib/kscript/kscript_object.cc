#include "kscript_object.h"
#include "kscript_struct.h"
#include "kscript_util.h"
#include "kscript.h"

KSObject::KSObject( KSClass* c ) : QShared(), m_class( c )
{
  m_status = Alive;

  QValueList<KSValue::Ptr> super;
  c->allSuperClasses( super );

  QValueList<KSValue::Ptr>::Iterator it = super.fromLast();
  for( ; it != super.end(); --it )
    m_scope.pushNamespace( (*it)->classValue()->nameSpace() );
  m_scope.pushNamespace( c->nameSpace() );

  // This has to be the last one!
  m_scope.pushNamespace( &m_space );

  // Register member functions
  m_space.insert( "disconnect", new KSValue( &KSObject::disconnect ) );
  m_space.insert( "isA", new KSValue( &KSObject::isA ) );
  m_space.insert( "inherits", new KSValue( &KSObject::inherits ) );
}

KSObject::~KSObject()
{
    qDebug("KSObject::~KSObject");
    destructor();
}

bool KSObject::kill()
{
    bool b = destructor();

    return b;
}

bool KSObject::destructor()
{
    if ( m_status == Dead )
	return TRUE;

    qDebug("KSObject::destructor");
    QListIterator<KSObject> it( m_sender );
    for( ; it.current(); ++it )
	it.current()->disconnect( this );
    m_sender.clear();

    disconnect();

    m_status = Dead;
	
    return true;
}

bool KSObject::connect( const QString& sig, KSObject* r, KSValue* s )
{
  // Find the signal object
  KSObject::SignalList* l = findSignal( sig, true );
  if ( !l )
    return false;

  Signal so;
  so.m_receiver = r;
  so.m_slot = s;
  l->append( so );
  so.m_receiver->appendSender( this );

  return true;
}

void KSObject::disconnect()
{
  SignalMap::Iterator mit = m_signals.begin();
  for( ; mit != m_signals.end(); ++mit )
  {
    SignalList::Iterator lit = (*mit).begin();
    for( ; lit != (*mit).end(); ++lit )
      (*lit).m_receiver->removeSender( this );
  }

  m_signals.clear();
}

void KSObject::disconnect( KSObject* o )
{
  SignalMap::Iterator mit = m_signals.begin();
  for( ; mit != m_signals.end(); ++mit )
  {
    SignalList::Iterator lit = (*mit).begin();
    while( lit != (*mit).end() )
    {
      if ( o == (*lit).m_receiver )
      {
	(*lit).m_receiver->removeSender( this );
	lit = (*mit).remove( lit );
      }
      else
	++lit;
    }
  }
}

void KSObject::disconnect( KSObject* o, KSValue* _slot )
{
  SignalMap::Iterator mit = m_signals.begin();
  for( ; mit != m_signals.end(); ++mit )
  {
    SignalList::Iterator lit = (*mit).begin();
    while( lit != (*mit).end() )
    {
      if ( o == (*lit).m_receiver && ((*lit).m_slot) == *_slot )
      {
	(*lit).m_receiver->removeSender( this );
	lit = (*mit).remove( lit );
      }
      else
	++lit;
    }
  }
}

bool KSObject::disconnect( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  // 2 Parameters
  if ( KSUtil::checkArgumentsCount( context, 2, "Object::disconnect", false ) )
  {
    if ( !KSUtil::checkType( context, args[0], KSValue::StringType ) )
      return false;

    SignalList* l = findSignal( args[0]->stringValue() );
    if ( !l )
    {
      if ( !m_class->hasSignal( args[0]->stringValue() ) )
      {
	QString tmp("The signal %1 does not exist in class %2");
	context.setException( new KSException( "UnknownSignal", tmp.arg( args[0]->stringValue() ).arg( m_class->name() ), -1 ) );
	return false;
      }
      else
	// No connections in existence right now
	return true;
    }

    if ( KSUtil::checkType( context, args[1], KSValue::MethodType, false ) )
    {
      KSObject* o = args[1]->methodValue()->object()->objectValue();
      KSValue* _slot = args[1]->methodValue()->function();

      SignalList::Iterator lit = l->begin();
      while( lit != l->end() )
      {
	if ( o == (*lit).m_receiver && (*lit).m_slot == *_slot )
        {
	  (*lit).m_receiver->removeSender( this );
	  lit = l->remove( lit );
	}
	else
	  ++lit;
      }
    }
    else if ( KSUtil::checkType( context, args[1], KSValue::ObjectType, false ) )
    {
      KSObject* o = args[1]->objectValue();
      SignalList::Iterator lit = l->begin();
      while( lit != l->end() )
      {
	if ( o == (*lit).m_receiver )
        {
	  (*lit).m_receiver->removeSender( this );
	  lit = l->remove( lit );
	}
	else
	  ++lit;
      }
    }
    else
    {
      KSUtil::castingError( context, args[1], KSValue::MethodType );
      return false;
    }
  }
  // 1 Parameter
  else if ( KSUtil::checkArgumentsCount( context, 1, "Object::connect", false ) )
  {
    if ( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
    {
      SignalList* l = findSignal( args[0]->stringValue() );
      if ( !l )
      {
	if ( !m_class->hasSignal( args[0]->stringValue() ) )
        {
	  QString tmp("The signal %1 does not exist in class %2");
	  context.setException( new KSException( "UnknownSignal", tmp.arg( args[0]->stringValue() ).arg( m_class->name() ), -1 ) );
	  return false;
	}
	else
	  // No connections in existence right now
	  return true;
      }

      l->clear();
    }
    else if ( KSUtil::checkType( context, args[0], KSValue::MethodType, false ) )
    {
      disconnect( args[0]->methodValue()->object()->objectValue(),
		  args[0]->methodValue()->function() );
    }
    else if ( KSUtil::checkType( context, args[0], KSValue::ObjectType ) )
    {
      disconnect( args[0]->objectValue() );
    }
    else
      return false;
  }
  // No Parameters
  else if ( KSUtil::checkArgumentsCount( context, 0, "Object::disconnect", true ) )
  {
    disconnect();
  }
  else
    return false;

  return true;
}

KSObject::SignalList* KSObject::findSignal( const QString& name, bool _insert )
{
  SignalMap::Iterator it = m_signals.find( name );
  if ( it == m_signals.end() )
  {
    if ( !_insert )
      return 0;

    if ( !m_class->hasSignal( name ) )
      return 0;
    SignalList l;
    it = m_signals.insert( name, l );
  }

  return &*it;
}

bool KSObject::emitSignal( const QString& name, KSContext& context )
{
  ASSERT( context.value() && context.value()->type() == KSValue::ListType );

  SignalList* l = findSignal( name );
  // Nobody registered for that signal until now
  if ( !l )
    return true;

  // Remove our namespaces
  KSSubScope* scope = context.scope()->popLocalScope();

  // Save the parameters
  QValueList<KSValue::Ptr> params = context.value()->listValue();

  SignalList::Iterator it = l->begin();
  for( ; it != l->end(); ++it )
  {
    // Refill the parameters for 2nd, 3nd, ... call.
    if ( it != l->begin() )
	context.setValue( new KSValue( params ) );

    (*it).m_slot->ref();
    (*it).m_receiver->ref();
    KSMethod method( (*it).m_receiver->module(), new KSValue( (*it).m_receiver ), (*it).m_slot );

    if ( !method.call( context ) )
    {
      // Resume namespaces
      context.scope()->pushLocalScope( scope );
      return false;
    }
  }

  // Now get rid of the parameters
  params.clear();

  // Resume namespaces
  context.scope()->pushLocalScope( scope );

  // Lets have at least a <none> as return value
  if ( !context.value() )
    context.setValue( KSValue::null() );

  return true;
}

KSValue::Ptr KSObject::member( KSContext& context, const QString& name )
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
	QString tmp( "Unknown symbol '%1' in object of class '%2'" );
	context.setException( new KSException( "UnknownName", tmp.arg( name ).arg( getClass()->name() ) ) );
	return 0;
    }

    v->ref();
    return KSValue::Ptr( v );
}

bool KSObject::setMember( KSContext&, const QString& name, const KSValue::Ptr& v )
{
    // If no such member is present then it is created
    KSValue* x = m_scope.object( name, true );
    ASSERT( x );
    *x = *v;

    return true;
}

/*****************************************
 * API functions of a pure KScript Object
 *****************************************/

bool KSObject::isA( KSContext& context )
{
    if ( !KSUtil::checkArgumentsCount( context, 0, "Object::isA" ) )
	return false;

    context.setValue( new KSValue( m_class->fullName() ) );

    return true;
}

bool KSObject::inherits( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgs( context, args, "s", "QObject::inherits" ) )
	return FALSE;

    // Is the name scoped ?
    QString name = args[0]->stringValue();
    if ( name.find( ":" ) == -1 )
    {
	name.prepend( ":" );
	name.prepend( context.module()->name() );
    }
    context.setValue( new KSValue( getClass()->inherits( name.latin1() ) ) );

    return true;
}

/*******************************************************
 *
 * KSScriptObject
 *
 *******************************************************/

bool KSScriptObject::destructor()
{
  if ( status() == Dead )
      return TRUE;

  qDebug("KSScriptObject::destructor");

  if ( !KSObject::destructor() )
    return false;

  // Get context
  KSContext& context = getClass()->module()->interpreter()->context();
  // Switch to our modules namespace
  context.scope()->pushModule( getClass()->module() );

  // All superclasses
  QValueList<KSValue::Ptr> super;
  getClass()->allSuperClasses( super );
  // Call our most specific destructor, too
  getClass()->ref();
  super.append( new KSValue( getClass() ) );

  // Call all destructors
  QValueList<KSValue::Ptr>::Iterator it = super.fromLast();
  for( ; it != super.end(); --it )
  {
    KSNamespace::Iterator it2 = (*it)->classValue()->nameSpace()->find( "delete" );
    if ( it2 != (*it)->classValue()->nameSpace()->end() )
    {
      // Create the parameter list for the destructor function
      KSValue::Ptr v = new KSValue( KSValue::ListType );
      ref(); ref();
      v->listValue().append( new KSValue( this ) );
      KSContext l( context );
      l.setValue( v );

      qDebug("Calling destructor %s\n",(*it)->classValue()->name().ascii() );
      if ( (*it2)->type() == KSValue::FunctionType )
      {
	if ( !(*it2)->functionValue()->call( l ) )
	{
	  context.scope()->popModule();
	  context.setException( l );
	  return false;
	}
      }
      else if ( (*it2)->type() == KSValue::BuiltinMethodType )
      {
	if ( !( (this->*((*it2)->builtinMethodValue()))( l ) ) )
	{
	  context.scope()->popModule();
	  context.setException( l );
	  return false;
	}
      }
      else
	ASSERT( 0 );
    }
  }

  context.scope()->popModule();
  return true;
}

KSScriptObject::~KSScriptObject()
{
  if ( !getClass()->module()->interpreter()->context().exception() && status() == Alive )
    destructor();
  qDebug("KSScriptObject::~KSScriptObject");
}

/*******************************************************
 *
 * KSMethod
 *
 *******************************************************/

bool KSMethod::call( KSContext& context )
{
  ASSERT( context.value() && context.value()->type() == KSValue::ListType );

  if ( m_func->type() == KSValue::FunctionType )
  {
    context.value()->listValue().prepend( m_object );
    if ( !m_func->functionValue()->call( context ) )
      return false;
  }
  else if ( m_func->type() == KSValue::BuiltinMethodType )
  {
    KSBuiltinMethod m = m_func->builtinMethodValue();
    KSObject* o = m_object->objectValue();
    if ( ! (o->*m)( context ) )
      return false;
  }
  else if ( m_func->type() == KSValue::StructBuiltinMethodType )
  {
    KSStructBuiltinMethod m = m_func->structBuiltinMethodValue();
    KSStruct* o = m_object->structValue();
    if ( ! (o->*m)( context, name() ) )
      return false;
  }
  else if ( m_func->type() == KSValue::ProxyBuiltinMethodType )
  {
    KSProxyBuiltinMethod m = m_func->proxyBuiltinMethodValue();
    KSProxy* o = m_object->proxyValue();
    if ( ! (o->*m)( context, name() ) )
      return false;
  }

  return true;
}

/*******************************************************
 *
 * KSProperty
 *
 *******************************************************/

bool KSProperty::set( KSContext& context, const KSValue::Ptr& v )
{
    if ( m_obj )
	return m_obj->setMember( context, m_name, v );
    else if ( m_struct )
	return m_struct->setMember( context, m_name, v );
    else if ( m_proxy )
	return m_proxy->setMember( context, m_name, v );
    else if ( m_module )
	return m_module->setMember( context, m_name, v );
    else
	ASSERT( 0 );

    // Never reached
    return false;
}
