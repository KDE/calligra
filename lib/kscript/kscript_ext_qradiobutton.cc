
// NOT YET FUNCTIONAL, ONLY FOR TESTING

#include "kscript_ext_qradiobutton.h"
#include "kscript_ext_qrect.h"
#include "kscript_context.h"
#include "kscript_util.h"
#include <stdio.h>
#include <qradiobutton.h>

#define WIDGET ((QRadioButton*)object())
#define CHECKTYPE( context, v, type ) if ( !checkType( context, v, type ) ) return FALSE;
#define RETURN_LEFTEXPR( n, value ) if ( name == n ) { KSValue::Ptr ptr = value; ptr->setMode( KSValue::LeftExpr ); return ptr; }
#define RETURN_RIGHTEXPR( n, value ) if ( name == n ) { return value; }
#define CHECK_LEFTEXPR( context, name ) if ( context.leftExpr() ) return KSObject::member( context, name );
#define SET_PROP( __n, __expr, __t ) if ( name == __n ) { CHECKTYPE( context, v, __t ); __expr; }

KSClass_QRadioButton::KSClass_QRadioButton( KSModule* m ) : KSScriptClass( m, "QRadioButton", 0 )
{
  nameSpace()->insert( "QRadioButton", new KSValue( (KSBuiltinMethod)&KSObject_QRadioButton::ksQRadioButton ) );
  nameSpace()->insert( "show", new KSValue( (KSBuiltinMethod)&KSObject_QRadioButton::ksQRadioButton_show ) );
  nameSpace()->insert( "destroy", new KSValue( (KSBuiltinMethod)&KSObject_QRadioButton::ksQRadioButton_delete ) );
}

KSScriptObject* KSClass_QRadioButton::createObject( KSClass* c )
{
  return new KSObject_QRadioButton( c );
}

KSObject_QRadioButton::KSObject_QRadioButton( KSClass* c ) : KSObject_QWidget( c ) // was: KS_Qt_Object
{
}

bool KSObject_QRadioButton::ksQRadioButton( KSContext& context )
{
  printf("QRadioButton\n");

  if ( !checkDoubleConstructor( context, "QRadioButton" ) )
    return false;

  // TODO: check parameters

  setObject( new QRadioButton( "Test", 0 ) );

  printf("QRadioButton 2\n");

  return true;
}

bool KSObject_QRadioButton::ksQRadioButton_show( KSContext& context )
{
  printf("QRadioButton::show\n");

  if ( !checkLive( context, "QRadioButton::show" ) )
    return false;

  if ( !KSUtil::checkArgumentsCount( context, 0, "QRadioButton::QRadioButton" ) )
    return false;

  QRadioButton* w = (QRadioButton*)object();
  w->show();

  return true;
}

bool KSObject_QRadioButton::ksQRadioButton_delete( KSContext& context )
{
  printf("QRadioButton::delete\n");

  if ( !KSUtil::checkArgumentsCount( context, 0, "QRadioButton::delete" ) )
    return false;

  if ( !object() )
    return true;

  delete object();
  setObject( 0 );

  return true;
}

KSValue::Ptr KSObject_QRadioButton::member( KSContext& context, const QString& name )
{
  RETURN_RIGHTEXPR( "width", new KSValue( WIDGET->width() ) );
  RETURN_RIGHTEXPR( "height", new KSValue( WIDGET->height() ) );
  RETURN_RIGHTEXPR( "x", new KSValue( WIDGET->x() ) );
  RETURN_RIGHTEXPR( "y", new KSValue( WIDGET->y() ) );

  CHECK_LEFTEXPR( context, name );

  RETURN_LEFTEXPR( "text", new KSValue( WIDGET->text() ) );
  RETURN_LEFTEXPR( "geometry", new KSValue( new KSObject_QRect( WIDGET->geometry() ) ) );

  return KS_Qt_Object::member( context, name );
}

bool KSObject_QRadioButton::setMember( KSContext& context, const QString& name, KSValue* v )
{
  SET_PROP( "text", WIDGET->setText( v->stringValue() ), StringType )
  SET_PROP( "geometry", WIDGET->setGeometry( *KSObject_QRect::convert( v ) ), RectType )
  else
    return KS_Qt_Object::setMember( context, name, v );

  return TRUE;
}
