
// NOT YET FUNCTIONAL, ONLY FOR TESTING

#include "kscript_ext_qcheckbox.h"
#include "kscript_ext_qrect.h"
#include "kscript_context.h"
#include "kscript_util.h"
#include <stdio.h>
#include <qcheckbox.h>

#define WIDGET ((QCheckBox*)object())
#define CHECKTYPE( context, v, type ) if ( !checkType( context, v, type ) ) return FALSE;
#define RETURN_LEFTEXPR( n, value ) if ( name == n ) { KSValue::Ptr ptr = value; ptr->setMode( KSValue::LeftExpr ); return ptr; }
#define RETURN_RIGHTEXPR( n, value ) if ( name == n ) { return value; }
#define CHECK_LEFTEXPR( context, name ) if ( context.leftExpr() ) return KSObject::member( context, name );
#define SET_PROP( __n, __expr, __t ) if ( name == __n ) { CHECKTYPE( context, v, __t ); __expr; }

KSClass_QCheckBox::KSClass_QCheckBox( KSModule* m ) : KSScriptClass( m, "QCheckBox", 0 )
{
  nameSpace()->insert( "QCheckBox", new KSValue( (KSBuiltinMethod)&KSObject_QCheckBox::ksQCheckBox ) );
  nameSpace()->insert( "show", new KSValue( (KSBuiltinMethod)&KSObject_QCheckBox::ksQCheckBox_show ) );
  nameSpace()->insert( "destroy", new KSValue( (KSBuiltinMethod)&KSObject_QCheckBox::ksQCheckBox_delete ) );
}

KSScriptObject* KSClass_QCheckBox::createObject( KSClass* c )
{
  return new KSObject_QCheckBox( c );
}

KSObject_QCheckBox::KSObject_QCheckBox( KSClass* c ) : KSObject_QWidget( c ) // was: KS_Qt_Object
{
}

bool KSObject_QCheckBox::ksQCheckBox( KSContext& context )
{
  printf("QCheckBox\n");

  if ( !checkDoubleConstructor( context, "QCheckBox" ) )
    return false;

  // TODO: check parameters

  setObject( new QCheckBox( 0 ) );

  printf("QCheckBox 2\n");

  return true;
}

bool KSObject_QCheckBox::ksQCheckBox_show( KSContext& context )
{
  printf("QCheckBox::show\n");

  if ( !checkLive( context, "QCheckBox::show" ) )
    return false;

  if ( !KSUtil::checkArgumentsCount( context, 0, "QCheckBox::QCheckBox" ) )
    return false;

  QCheckBox* w = (QCheckBox*)object();
  w->show();

  return true;
}

bool KSObject_QCheckBox::ksQCheckBox_delete( KSContext& context )
{
  printf("QCheckBox::delete\n");

  if ( !KSUtil::checkArgumentsCount( context, 0, "QCheckBox::delete" ) )
    return false;

  if ( !object() )
    return true;

  delete object();
  setObject( 0 );

  return true;
}

KSValue::Ptr KSObject_QCheckBox::member( KSContext& context, const QString& name )
{
  RETURN_RIGHTEXPR( "width", new KSValue( WIDGET->width() ) );
  RETURN_RIGHTEXPR( "height", new KSValue( WIDGET->height() ) );
  RETURN_RIGHTEXPR( "x", new KSValue( WIDGET->x() ) );
  RETURN_RIGHTEXPR( "y", new KSValue( WIDGET->y() ) );
  RETURN_RIGHTEXPR( "checked", new KSValue( WIDGET->isChecked() ) );

  CHECK_LEFTEXPR( context, name );

  RETURN_LEFTEXPR( "text", new KSValue( WIDGET->text() ) );
  RETURN_LEFTEXPR( "geometry", new KSValue( new KSObject_QRect( WIDGET->geometry() ) ) );

  return KS_Qt_Object::member( context, name );
}

bool KSObject_QCheckBox::setMember( KSContext& context, const QString& name, KSValue* v )
{
  SET_PROP( "text", WIDGET->setText( v->stringValue() ), StringType )
  SET_PROP( "geometry", WIDGET->setGeometry( *KSObject_QRect::convert( v ) ), RectType )
  else
    return KS_Qt_Object::setMember( context, name, v );

  return TRUE;
}
