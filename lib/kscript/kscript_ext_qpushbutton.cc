#include "kscript_ext_qpushbutton.h"
#include "kscript_ext_qrect.h"
#include "kscript_context.h"
#include "kscript_util.h"
#include <stdio.h>
#include <qpushbutton.h>

#define WIDGET ((QPushButton*)object())
#define CHECKTYPE( context, v, type ) if ( !checkType( context, v, type ) ) return FALSE;
#define RETURN_LEFTEXPR( n, value ) if ( name == n ) { KSValue::Ptr ptr = value; ptr->setMode( KSValue::LeftExpr ); return ptr; }
#define RETURN_RIGHTEXPR( n, value ) if ( name == n ) { return value; }
#define CHECK_LEFTEXPR( context, name ) if ( context.leftExpr() ) return KSObject::member( context, name );
#define SET_PROP( __n, __expr, __t ) if ( name == __n ) { CHECKTYPE( context, v, __t ); __expr; }

KSClass_QPushButton::KSClass_QPushButton( KSModule* m ) : KSScriptClass( m, "QPushButton", 0 )
{
  nameSpace()->insert( "QPushButton", new KSValue( (KSBuiltinMethod)&KSObject_QPushButton::ksQPushButton ) );
  nameSpace()->insert( "show", new KSValue( (KSBuiltinMethod)&KSObject_QPushButton::ksQPushButton_show ) );
  nameSpace()->insert( "destroy", new KSValue( (KSBuiltinMethod)&KSObject_QPushButton::ksQPushButton_delete ) );
}

KSScriptObject* KSClass_QPushButton::createObject( KSClass* c )
{
  return new KSObject_QPushButton( c );
}

KSObject_QPushButton::KSObject_QPushButton( KSClass* c ) : KSObject_QWidget( c ) // was: KS_Qt_Object
{
}

bool KSObject_QPushButton::ksQPushButton( KSContext& context )
{
  printf("QPushButton\n");

  if ( !checkDoubleConstructor( context, "QPushButton" ) )
    return false;

  // TODO: check parameters

  setObject( new QPushButton( 0 ) );

  printf("QPushButton 2\n");

  return true;
}

bool KSObject_QPushButton::ksQPushButton_show( KSContext& context )
{
  printf("QPushButton::show\n");

  if ( !checkLive( context, "QPushButton::show" ) )
    return false;

  if ( !KSUtil::checkArgumentsCount( context, 0, "QPushButton::QPushButton" ) )
    return false;

  QPushButton* w = (QPushButton*)object();
  w->show();

  return true;
}

bool KSObject_QPushButton::ksQPushButton_delete( KSContext& context )
{
  printf("QPushButton::delete\n");

  if ( !KSUtil::checkArgumentsCount( context, 0, "QPushButton::delete" ) )
    return false;

  if ( !object() )
    return true;

  delete object();
  setObject( 0 );

  return true;
}

KSValue::Ptr KSObject_QPushButton::member( KSContext& context, const QString& name )
{
  RETURN_RIGHTEXPR( "width", new KSValue( WIDGET->width() ) );
  RETURN_RIGHTEXPR( "height", new KSValue( WIDGET->height() ) );
  RETURN_RIGHTEXPR( "x", new KSValue( WIDGET->x() ) );
  RETURN_RIGHTEXPR( "y", new KSValue( WIDGET->y() ) );

  CHECK_LEFTEXPR( context, name );

  RETURN_LEFTEXPR( "caption", new KSValue( WIDGET->caption() ) );
  RETURN_LEFTEXPR( "geometry", new KSValue( new KSObject_QRect( WIDGET->geometry() ) ) );

  return KS_Qt_Object::member( context, name );
}

bool KSObject_QPushButton::setMember( KSContext& context, const QString& name, KSValue* v )
{
  SET_PROP( "caption", WIDGET->setCaption( v->stringValue() ), StringType )
  SET_PROP( "geometry", WIDGET->setGeometry( *KSObject_QRect::convert( v ) ), RectType )
  else
    return KS_Qt_Object::setMember( context, name, v );

  return TRUE;
}
