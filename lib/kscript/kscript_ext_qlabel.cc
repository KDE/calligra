
// NOT YET FUNCTIONAL, ONLY FOR TESTING

#include "kscript_ext_qlabel.h"
#include "kscript_ext_qrect.h"
#include "kscript_context.h"
#include "kscript_util.h"
#include <stdio.h>
#include <qlabel.h>

#define WIDGET ((QLabel*)object())
#define CHECKTYPE( context, v, type ) if ( !checkType( context, v, type ) ) return FALSE;
#define RETURN_LEFTEXPR( n, value ) if ( name == n ) { KSValue::Ptr ptr = value; ptr->setMode( KSValue::LeftExpr ); return ptr; }
#define RETURN_RIGHTEXPR( n, value ) if ( name == n ) { return value; }
#define CHECK_LEFTEXPR( context, name ) if ( context.leftExpr() ) return KSObject::member( context, name );
#define SET_PROP( __n, __expr, __t ) if ( name == __n ) { CHECKTYPE( context, v, __t ); __expr; }

KSClass_QLabel::KSClass_QLabel( KSModule* m ) : KSScriptClass( m, "QLabel", 0 )
{
  nameSpace()->insert( "QLabel", new KSValue( (KSBuiltinMethod)&KSObject_QLabel::ksQLabel ) );
  nameSpace()->insert( "show", new KSValue( (KSBuiltinMethod)&KSObject_QLabel::ksQLabel_show ) );
  nameSpace()->insert( "destroy", new KSValue( (KSBuiltinMethod)&KSObject_QLabel::ksQLabel_delete ) );
}

KSScriptObject* KSClass_QLabel::createObject( KSClass* c )
{
  return new KSObject_QLabel( c );
}

KSObject_QLabel::KSObject_QLabel( KSClass* c ) : KSObject_QWidget( c ) // was: KS_Qt_Object
{
}

bool KSObject_QLabel::ksQLabel( KSContext& context )
{
  printf("QLabel\n");

  if ( !checkDoubleConstructor( context, "QLabel" ) )
    return false;

  // TODO: check parameters

  setObject( new QLabel( "Test", 0 ) );

  printf("QLabel 2\n");

  return true;
}

bool KSObject_QLabel::ksQLabel_show( KSContext& context )
{
  printf("QLabel::show\n");

  if ( !checkLive( context, "QLabel::show" ) )
    return false;

  if ( !KSUtil::checkArgumentsCount( context, 0, "QLabel::QLabel" ) )
    return false;

  QLabel* w = (QLabel*)object();
  w->show();

  return true;
}

bool KSObject_QLabel::ksQLabel_delete( KSContext& context )
{
  printf("QLabel::delete\n");

  if ( !KSUtil::checkArgumentsCount( context, 0, "QLabel::delete" ) )
    return false;

  if ( !object() )
    return true;

  delete object();
  setObject( 0 );

  return true;
}

KSValue::Ptr KSObject_QLabel::member( KSContext& context, const QString& name )
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

bool KSObject_QLabel::setMember( KSContext& context, const QString& name, KSValue* v )
{
  SET_PROP( "text", WIDGET->setText( v->stringValue() ), StringType )
  SET_PROP( "geometry", WIDGET->setGeometry( *KSObject_QRect::convert( v ) ), RectType )
  else
    return KSObject_QWidget::setMember( context, name, v );

  return TRUE;
}
