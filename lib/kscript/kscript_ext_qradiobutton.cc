
// NOT YET FUNCTIONAL, ONLY FOR TESTING

#include "kscript_ext_qradiobutton.h"
#include "kscript_context.h"
#include "kscript_util.h"
#include <stdio.h>
#include <qradiobutton.h>

#undef WIDGET
#define WIDGET ((QRadioButton*)object())
#undef CHECKTYPE
#define CHECKTYPE( context, v, type ) if ( !checkType( context, v, type ) ) return FALSE;
#define RETURN_LEFTEXPR( n, value ) if ( name == n ) { KSValue::Ptr ptr = value; ptr->setMode( KSValue::LeftExpr ); return ptr; }
#define RETURN_RIGHTEXPR( n, value ) if ( name == n ) { return value; }
#define CHECK_LEFTEXPR( context, name ) if ( context.leftExpr() ) return KSObject::member( context, name );
#define SET_PROP( __n, __expr, __t ) if ( name == __n ) { CHECKTYPE( context, v, __t ); __expr; return TRUE; }

KSClass_QRadioButton::KSClass_QRadioButton( KSModule* m, const char* name ) : KSClass_QButton( m, name )
{
  nameSpace()->insert( "QRadioButton", new KSValue( (KSBuiltinMethod)&KSObject_QRadioButton::ksQRadioButton ) );
}

KSScriptObject* KSClass_QRadioButton::createObject( KSClass* c )
{
  return new KSObject_QRadioButton( c );
}

KSObject_QRadioButton::KSObject_QRadioButton( KSClass* c ) : KSObject_QButton( c )
{
}

bool KSObject_QRadioButton::ksQRadioButton( KSContext& context )
{
  qDebug("QRadioButton\n");

  if ( !checkDoubleConstructor( context, "QRadioButton" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  QWidget* parent = 0;
  QString name;

  if ( args.count() >= 1 )
  {
      if ( !checkArguments( context, context.value(), "QRadioButton::QRadioButton", KS_Qt_Object::WidgetType ) )
	  return FALSE;
      parent = KSObject_QWidget::convert( args[0] );
  }
  if ( args.count() >= 2 )
  {
      if ( !checkArguments( context, context.value(), "QRadioButton::QRadioButton", KS_Qt_Object::StringType ) )
	  return FALSE;
      name = args[1]->stringValue();
  }
  if ( args.count() > 2 )
  {
      KSUtil::tooFewArgumentsError( context, "QRadioButton::QRadioButton" );
      return FALSE;
  }

  setObject( new QRadioButton( parent, name.latin1() ) );

  qDebug("QRadioButton 2\n");

  return true;
}

KSValue::Ptr KSObject_QRadioButton::member( KSContext& context, const QString& name )
{
  CHECK_LEFTEXPR( context, name );

  // RETURN_LEFTEXPR( "geometry", new KSValue( new KSObject_QRect( WIDGET->geometry() ) ) );

  return KSObject_QButton::member( context, name );
}

bool KSObject_QRadioButton::setMember( KSContext& context, const QString& name, const KSValue::Ptr& v )
{
  SET_PROP( "text", WIDGET->setText( v->stringValue() ), StringType )

  return KSObject_QButton::setMember( context, name, v );
}
