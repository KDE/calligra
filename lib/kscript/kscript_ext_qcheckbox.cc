#include "kscript_ext_qcheckbox.h"
#include "kscript_context.h"
#include "kscript_util.h"
#include <stdio.h>
#include <qcheckbox.h>

#undef WIDGET
#define WIDGET ((QCheckBox*)object())
#undef CHECKTYPE
#define CHECKTYPE( context, v, type ) if ( !checkType( context, v, type ) ) return FALSE;
#define RETURN_LEFTEXPR( n, value ) if ( name == n ) { KSValue::Ptr ptr = value; ptr->setMode( KSValue::LeftExpr ); return ptr; }
#define RETURN_RIGHTEXPR( n, value ) if ( name == n ) { return value; }
#define CHECK_LEFTEXPR( context, name ) if ( context.leftExpr() ) return KSObject::member( context, name );
#define SET_PROP( __n, __expr, __t ) if ( name == __n ) { CHECKTYPE( context, v, __t ); __expr; return TRUE; }

KSClass_QCheckBox::KSClass_QCheckBox( KSModule* m, const char* name ) : KSClass_QButton( m, name )
{
  nameSpace()->insert( "QCheckBox", new KSValue( (KSBuiltinMethod)&KSObject_QCheckBox::ksQCheckBox ) );
}

KSScriptObject* KSClass_QCheckBox::createObject( KSClass* c )
{
  return new KSObject_QCheckBox( c );
}

KSObject_QCheckBox::KSObject_QCheckBox( KSClass* c ) : KSObject_QButton( c )
{
}

bool KSObject_QCheckBox::ksQCheckBox( KSContext& context )
{
  qDebug("QCheckBox\n");

  if ( !checkDoubleConstructor( context, "QCheckBox" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  QWidget* parent = 0;
  QString name;

  if ( args.count() >= 1 )
  {
      if ( !checkArguments( context, context.value(), "QCheckBox::QCheckBox", KS_Qt_Object::WidgetType ) )
	  return FALSE;
      parent = KSObject_QWidget::convert( args[0] );
  }
  if ( args.count() >= 2 )
  {
      if ( !checkArguments( context, context.value(), "QCheckBox::QCheckBox", KS_Qt_Object::StringType ) )
	  return FALSE;
      name = args[1]->stringValue();
  }
  if ( args.count() > 2 )
  {
      KSUtil::tooFewArgumentsError( context, "QCheckBox::QCheckBox" );
      return FALSE;
  }

  setObject( new QCheckBox( parent, name.latin1() ) );

  qDebug("QCheckBox 2\n");

  return true;
}

KSValue::Ptr KSObject_QCheckBox::member( KSContext& context, const QString& name )
{
  CHECK_LEFTEXPR( context, name );

  RETURN_LEFTEXPR( "checked", new KSValue( WIDGET->isChecked() ) );
  RETURN_LEFTEXPR( "text", new KSValue( WIDGET->text() ) );

  return KSObject_QButton::member( context, name );
}

bool KSObject_QCheckBox::setMember( KSContext& context, const QString& name, const KSValue::Ptr& v )
{
  SET_PROP( "text", WIDGET->setText( v->stringValue() ), StringType )
  SET_PROP( "checked", WIDGET->setChecked( v->boolValue() ), BoolType )

  return KSObject_QButton::setMember( context, name, v );
}
