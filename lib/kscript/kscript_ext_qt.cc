#include "kscript_ext_qt.h"
#include "kscript_ext_qwidget.h"
#include "kscript_ext_qapplication.h"
#include "kscript_ext_qrect.h"
#include "kscript_value.h"
#include "kscript_util.h"

#define CHECKTYPE( context, v, type ) if ( !checkType( context, v, type ) ) return FALSE;

KSModule::Ptr ksCreateModule_Qt( KSInterpreter* interp )
{
  KSModule::Ptr module = new KSModule( interp, "qt" );

  // Add all Qt classes to the module
  module->addObject( "QWidget", new KSValue( new KSClass_QWidget( module ) ) );
  module->addObject( "QApplication", new KSValue( new KSClass_QApplication( module ) ) );
  module->addObject( "QRect", new KSValue( new KSClass_QRect( module ) ) );

  return module;
}

/**********************************************
 *
 * KS_Qt_Callback
 *
 **********************************************/

KS_Qt_Callback* KS_Qt_Callback::s_pSelf = 0;

KS_Qt_Callback* KS_Qt_Callback::self()
{
  if ( !s_pSelf )
    s_pSelf = new KS_Qt_Callback;
  return s_pSelf;
}

void KS_Qt_Callback::connect( QObject* s, KSObject* r )
{
  DestroyCallback c;
  c.m_sender = s;
  c.m_receiver = r;
  m_callbacks.append( c );
  QObject::connect( s, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );
}

void KS_Qt_Callback::connect( QObject* s, const QString& sig, KSObject* r, const KSValue::Ptr& slot )
{
  Connection c;
  c.m_sender = s;
  c.m_signal = sig;
  c.m_receiver = r;
  c.m_slot = slot;
  m_connections.append( c );
}

void KS_Qt_Callback::disconnect( KSObject* r )
{
  QValueList<Connection>::Iterator it =  m_connections.begin();
  while( it != m_connections.end() )
  {
    if ( r == (*it).m_receiver )
      it = m_connections.remove( it );
    else
      ++it;
  }

  QValueList<DestroyCallback>::Iterator it2 = m_callbacks.begin();
  while( it2 != m_callbacks.end() )
  {
    if ( r == (*it2).m_receiver )
      it2 = m_callbacks.remove( it2 );
    else
      ++it2;
  }
}

void KS_Qt_Callback::slotDestroyed()
{
  QObject *s = (QObject*)sender();

  QValueList<DestroyCallback>::Iterator it = m_callbacks.begin();
  while( it != m_callbacks.end() )
  {
    if ( s == (*it).m_sender )
      if ( (*it).m_receiver->status() == KSObject::Alive )
	(*it).m_receiver->kill();
    ++it;
  }
}

/**********************************************
 *
 * KS_Qt_Object
 *
 **********************************************/

KS_Qt_Object::~KS_Qt_Object()
{
  if ( status() == Alive )
    destructor();
}

void KS_Qt_Object::setObject( QObject* o, bool ownership )
{
  if ( m_object )
    KS_Qt_Callback::self()->disconnect( this );

  m_object = o;
  m_ownership = ownership;

  if ( m_object )
    KS_Qt_Callback::self()->connect( m_object, this );
}

bool KS_Qt_Object::destructor()
{
  bool b = KSScriptObject::destructor();

  if ( m_object )
  {
    if ( m_ownership )
      delete m_object;
    m_object = 0;
  }

  KS_Qt_Callback::self()->disconnect( this );

  return b;
}

KSValue::Ptr KS_Qt_Object::member( KSContext& context, const QString& name )
{
  KSValue::Ptr ptr;
  if ( name == "name" )
    ptr = new KSValue( QString( m_object->name() ) );
    
  if ( ptr )
  {
    ptr->setMode( KSValue::LeftExpr );
    return ptr;
  }

  return KSObject::member( context, name );
}

bool KS_Qt_Object::setMember( KSContext& context, const QString& name, KSValue* v )
{
  if ( name == "name" )
  {
    CHECKTYPE( context, v, StringType );
    m_object->setName( v->stringValue() );
    return TRUE;
  }

  return KSObject::setMember( context, name, v );
}

/***********************************************
 *
 * Helper
 *
 ***********************************************/

bool KS_Qt_Object::checkArguments( KSContext& context, KSValue* v, const QString& name, KS_Qt_Object::Type t1,
				   KS_Qt_Object::Type t2, KS_Qt_Object::Type t3,
				   KS_Qt_Object::Type t4, KS_Qt_Object::Type t5, KS_Qt_Object::Type t6 )
{
  QValueList<KSValue::Ptr>::Iterator it = v->listValue().begin();
  QValueList<KSValue::Ptr>::Iterator end = v->listValue().end();

  if ( t1 != NoType )
  {
    if ( it == end )
    {
      KSUtil::tooFewArgumentsError( context, name );
      return false;
    }
    if ( !checkType( context, *it, t1, true ) )
      return false;
    ++it;
  }
  if ( t2 != NoType )
  {
    if ( it == end )
    {
      KSUtil::tooFewArgumentsError( context, name );
      return false;
    }
    if ( !checkType( context, *it, t2, true ) )
      return false;
    ++it;
  }
  if ( t3 != NoType )
  {
    if ( it == end )
    {
      KSUtil::tooFewArgumentsError( context, name );
      return false;
    }
    if ( !checkType( context, *it, t3, true ) )
      return false;
    ++it;
  }
  if ( t4 != NoType )
  {
    if ( it == end )
    {
      KSUtil::tooFewArgumentsError( context, name );
      return false;
    }
    if ( !checkType( context, *it, t4, true ) )
      return false;
    ++it;
  }
  if ( t5 != NoType )
  {
    if ( it == end )
    {
      KSUtil::tooFewArgumentsError( context, name );
      return false;
    }
    if ( !checkType( context, *it, t5, true ) )
      return false;
    ++it;
  }
  if ( t6 != NoType )
  {
    if ( it == end )
    {
      KSUtil::tooFewArgumentsError( context, name );
      return false;
    }
    if ( !checkType( context, *it, t6, true ) )
      return false;
    ++it;
  }

  // Too many parameters ?
  if ( it != end )
  {
    KSUtil::tooManyArgumentsError( context, name );
    return false;
  }

  return true;
}

bool KS_Qt_Object::tryArguments( KSContext& context, KSValue* v, KS_Qt_Object::Type t1,
				 KS_Qt_Object::Type t2, KS_Qt_Object::Type t3,
				 KS_Qt_Object::Type t4, KS_Qt_Object::Type t5, KS_Qt_Object::Type t6 )
{
  QValueList<KSValue::Ptr>::Iterator it = v->listValue().begin();
  QValueList<KSValue::Ptr>::Iterator end = v->listValue().end();

  if ( t1 != NoType )
  {
    if ( it == end )
      return false;
    if ( !checkType( context, *it, t1, false ) );
      return false;
    ++it;
  }
  if ( t2 != NoType )
  {
    if ( it == end )
      return false;
    if ( !checkType( context, *it, t2, false ) );
      return false;
    ++it;
  }
  if ( t3 != NoType )
  {
    if ( it == end )
      return false;
    if ( !checkType( context, *it, t3, false ) );
      return false;
    ++it;
  }
  if ( t4 != NoType )
  {
    if ( it == end )
      return false;
    if ( !checkType( context, *it, t4, false ) );
      return false;
    ++it;
  }
  if ( t5 != NoType )
  {
    if ( it == end )
      return false;
    if ( !checkType( context, *it, t5, false ) );
      return false;
    ++it;
  }
  if ( t6 != NoType )
  {
    if ( it == end )
      return false;
    if ( !checkType( context, *it, t6, false ) );
      return false;
    ++it;
  }

  // Too many parameters ?
  if ( it != end )
    return false;

  return true;
}

bool KS_Qt_Object::checkType( KSContext& context, KSValue* v, KS_Qt_Object::Type type, bool _fatal )
{
  switch( type )
  {
  case StringType:
    return KSUtil::checkType( context, v, KSValue::StringType, _fatal );
  case IntType:
    return KSUtil::checkType( context, v, KSValue::IntType, _fatal );
  case BoolType:
    return KSUtil::checkType( context, v, KSValue::BoolType, _fatal );
  case DoubleType:
    return KSUtil::checkType( context, v, KSValue::DoubleType, _fatal );
  case RectType:
    if ( !KSUtil::checkType( context, v, KSValue::ObjectType, _fatal ) )
      return false;
    if ( v->objectValue()->inherits( "KSObject_QRect" ) )
      return true;
    KSUtil::castingError( context, "Object", "QRect" );
    return false;
  case NoType:
    ASSERT( 0 );
  }

  // never reached
  return false;
}

bool KS_Qt_Object::checkDoubleConstructor( KSContext& context, const QString& name )
{
  if ( m_object )
  {
    QString tmp( "The constructor of the class %1 was called twice." );
    context.setException( new KSException( "ConstructorCalledTwice", tmp.arg( name ), -1 ) );
    return false;
  }

  return true;
}

bool KS_Qt_Object::checkLive( KSContext& context, const QString& name )
{
  if ( !m_object )
  {
    QString tmp( "The method %1 was called before the constructor or after the destructor" );
    context.setException( new KSException( "MethodCalledBeforeConstructor", tmp.arg( name ), -1 ) );
    return false;
  }

  return true;
}


#include "kscript_ext_qt.moc"
#undef CHECKTYPE
