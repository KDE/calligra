#include "kscript_ext_qrect.h"
#include "kscript_ext_qt.h"
#include "kscript_context.h"
#include "kscript_util.h"

#include <qrect.h>

#define CHECKTYPE( context, v, type ) if ( !KS_Qt_Object::checkType( context, v, type ) ) return FALSE;
#define RETURN_LEFTEXPR( n, value ) if ( name == n ) { KSValue::Ptr ptr = value; ptr->setMode( KSValue::LeftExpr ); return ptr; }
#define RETURN_RIGHTEXPR( n, value ) if ( name == n ) { return value; }
#define CHECK_LEFTEXPR( context, name ) if ( context.leftExpr() ) return KSObject::member( context, name );
#define SET_PROP( __n, __expr, __t ) if ( name == __n ) { CHECKTYPE( context, v, __t ); __expr; }
#define ARG( v, i ) (v->listValue()[i])

KSClass_QRect* KSClass_QRect::s_pSelf = 0;

KSClass_QRect::KSClass_QRect( KSModule* m ) : KSScriptClass( m, "QRect", 0 )
{
  s_pSelf = this;

  nameSpace()->insert( "QRect", new KSValue( (KSBuiltinMethod)&KSObject_QRect::ksQRect ) );
  nameSpace()->insert( "intersects", new KSValue( (KSBuiltinMethod)&KSObject_QRect::ksQRect_intersects ) );
  nameSpace()->insert( "unite", new KSValue( (KSBuiltinMethod)&KSObject_QRect::ksQRect_unite ) );
}

KSScriptObject* KSClass_QRect::createObject( KSClass* c )
{
  return new KSObject_QRect( c );
}

KSObject_QRect::KSObject_QRect( KSClass* c ) : KSScriptObject( c )
{
}

KSObject_QRect::KSObject_QRect( const QRect& r ) : KSScriptObject( KSClass_QRect::self() ), m_rect( r )
{
}

bool KSObject_QRect::ksQRect( KSContext& context )
{
  return KS_Qt_Object::checkArguments( context, context.value(), "QRect::QRect" );
}

bool KSObject_QRect::ksQRect_unite( KSContext& context )
{
  if ( !KS_Qt_Object::checkArguments( context, context.value(), "QRect::unite", KS_Qt_Object::RectType ) )
    return false;

  context.setValue( new KSValue( new KSObject_QRect( m_rect.unite( *convert( ARG( context.value(), 0 ) ) ) ) ) );
  return true;
}

bool KSObject_QRect::ksQRect_intersects( KSContext& context )
{
  if ( !KS_Qt_Object::checkArguments( context, context.value(), "QRect::intersects", KS_Qt_Object::RectType ) )
    return false;

  context.setValue( new KSValue( m_rect.intersects( *convert( ARG( context.value(), 0 ) ) ) ) );
  return true;
}

KSValue::Ptr KSObject_QRect::member( KSContext& context, const QString& name )
{
  CHECK_LEFTEXPR( context, name );

  RETURN_LEFTEXPR( "width", new KSValue( m_rect.width() ) );
  RETURN_LEFTEXPR( "height", new KSValue( m_rect.height() ) );
  RETURN_LEFTEXPR( "x", new KSValue( m_rect.x() ) );
  RETURN_LEFTEXPR( "y", new KSValue( m_rect.y() ) );
  RETURN_LEFTEXPR( "left", new KSValue( m_rect.left() ) );
  RETURN_LEFTEXPR( "right", new KSValue( m_rect.right() ) );
  RETURN_LEFTEXPR( "top", new KSValue( m_rect.top() ) );
  RETURN_LEFTEXPR( "bottom", new KSValue( m_rect.bottom() ) );

  return KSScriptObject::member( context, name );
}

bool KSObject_QRect::setMember( KSContext& context, const QString& name, KSValue* v )
{
  SET_PROP( "width", m_rect.setWidth( v->intValue() ), KS_Qt_Object::IntType )
  SET_PROP( "height", m_rect.setHeight( v->intValue() ), KS_Qt_Object::IntType )
  SET_PROP( "x", m_rect.setX( v->intValue() ), KS_Qt_Object::IntType )
  SET_PROP( "y", m_rect.setY( v->intValue() ), KS_Qt_Object::IntType )
  SET_PROP( "left", m_rect.setLeft( v->intValue() ), KS_Qt_Object::IntType )
  SET_PROP( "right", m_rect.setRight( v->intValue() ), KS_Qt_Object::IntType )
  SET_PROP( "bottom", m_rect.setBottom( v->intValue() ), KS_Qt_Object::IntType )
  SET_PROP( "top", m_rect.setTop( v->intValue() ), KS_Qt_Object::IntType )
  else
    return KSScriptObject::setMember( context, name, v );

  return true;
}
