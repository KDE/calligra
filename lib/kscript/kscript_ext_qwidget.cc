#include "kscript_ext_qwidget.h"
#include "kscript_ext_qrect.h"
#include "kscript_context.h"
#include "kscript_util.h"
#include <stdio.h>
#include <qwidget.h>

#define WIDGET ((QWidget*)object())
#define CHECKTYPE( context, v, type ) if ( !checkType( context, v, type ) ) return FALSE;
#define RETURN_LEFTEXPR( n, value ) if ( name == n ) { KSValue::Ptr ptr = value; ptr->setMode( KSValue::LeftExpr ); return ptr; }
#define RETURN_RIGHTEXPR( n, value ) if ( name == n ) { return value; }
#define CHECK_LEFTEXPR( context, name ) if ( context.leftExpr() ) return KSObject::member( context, name );
#define SET_PROP( __n, __expr, __t ) if ( name == __n ) { CHECKTYPE( context, v, __t ); __expr; }

KSClass_QWidget::KSClass_QWidget( KSModule* m ) : KSScriptClass( m, "QWidget", 0 )
{
  nameSpace()->insert( "QWidget", new KSValue( (KSBuiltinMethod)&KSObject_QWidget::ksQWidget ) );
  nameSpace()->insert( "show", new KSValue( (KSBuiltinMethod)&KSObject_QWidget::ksQWidget_show ) );
  nameSpace()->insert( "destroy", new KSValue( (KSBuiltinMethod)&KSObject_QWidget::ksQWidget_delete ) );
}

KSScriptObject* KSClass_QWidget::createObject( KSClass* c )
{
  return new KSObject_QWidget( c );
}

KSObject_QWidget::KSObject_QWidget( KSClass* c ) : KS_Qt_Object( c )
{
}

bool KSObject_QWidget::ksQWidget( KSContext& context )
{
  printf("QWidget\n");

  if ( !checkDoubleConstructor( context, "QWidget" ) )
    return false;

  // TODO: check parameters

  setObject( new QWidget( 0 ) );

  printf("QWidget 2\n");

  return true;
}

bool KSObject_QWidget::ksQWidget_show( KSContext& context )
{
  printf("QWidget::show\n");

  if ( !checkLive( context, "QWidget::show" ) )
    return false;

  if ( !KSUtil::checkArgumentsCount( context, 0, "QWidget::QWidget" ) )
    return false;

  QWidget* w = (QWidget*)object();
  w->show();

  return true;
}

bool KSObject_QWidget::ksQWidget_delete( KSContext& context )
{
  printf("QWidget::delete\n");

  if ( !KSUtil::checkArgumentsCount( context, 0, "QWidget::delete" ) )
    return false;

  if ( !object() )
    return true;

  delete object();
  setObject( 0 );

  return true;
}

KSValue::Ptr KSObject_QWidget::member( KSContext& context, const QString& name )
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

bool KSObject_QWidget::setMember( KSContext& context, const QString& name, KSValue* v )
{
  SET_PROP( "caption", WIDGET->setCaption( v->stringValue() ), StringType )
  SET_PROP( "geometry", WIDGET->setGeometry( *KSObject_QRect::convert( v ) ), RectType )
  else
    return KS_Qt_Object::setMember( context, name, v );

  return TRUE;
}
