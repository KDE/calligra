#include "kscript_ext_qlabel.h"
#include "kscript_context.h"
#include "kscript_util.h"

#include <qlabel.h>

#undef WIDGET
#define WIDGET ((QLabel*)object())
#undef CHECKTYPE
#define CHECKTYPE( context, v, type ) if ( !checkType( context, v, type ) ) return FALSE;
#define RETURN_LEFTEXPR( n, value ) if ( name == n ) { KSValue::Ptr ptr = value; ptr->setMode( KSValue::LeftExpr ); return ptr; }
#define RETURN_RIGHTEXPR( n, value ) if ( name == n ) { return value; }
#define CHECK_LEFTEXPR( context, name ) if ( context.leftExpr() ) return KSObject::member( context, name );
#define SET_PROP( __n, __expr, __t ) if ( name == __n ) { CHECKTYPE( context, v, __t ); __expr; return TRUE; }

KSClass_QLabel::KSClass_QLabel( KSModule* m, const char* name ) : KSClass_QWidget( m, name )
{
  nameSpace()->insert( "QLabel", new KSValue( (KSBuiltinMethod)&KSObject_QLabel::ksQLabel ) );
}

KSScriptObject* KSClass_QLabel::createObject( KSClass* c )
{
  return new KSObject_QLabel( c );
}

KSObject_QLabel::KSObject_QLabel( KSClass* c ) : KSObject_QWidget( c )
{
}

bool KSObject_QLabel::ksQLabel( KSContext& context )
{
  qDebug("QLabel\n");

  if ( !checkDoubleConstructor( context, "QLabel" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  QWidget* parent = 0;
  QString name;

  if ( args.count() >= 1 )
  {
      if ( !checkArguments( context, context.value(), "QLabel::QLabel", KS_Qt_Object::WidgetType ) )
	  return FALSE;
      parent = KSObject_QWidget::convert( args[0] );
  }
  if ( args.count() >= 2 )
  {
      if ( !checkArguments( context, context.value(), "QLabel::QLabel", KS_Qt_Object::StringType ) )
	  return FALSE;
      name = args[1]->stringValue();
  }
  if ( args.count() > 2 )
  {
      KSUtil::tooFewArgumentsError( context, "QLabel::QLabel" );
      return FALSE;
  }

  setObject( new QLabel( parent, name.latin1() ) );

  qDebug("QLabel 2\n");

  return true;
}

KSValue::Ptr KSObject_QLabel::member( KSContext& context, const QString& name )
{
  CHECK_LEFTEXPR( context, name );

  RETURN_LEFTEXPR( "text", new KSValue( WIDGET->text() ) );

  return KSObject_QWidget::member( context, name );
}

bool KSObject_QLabel::setMember( KSContext& context, const QString& name, const KSValue::Ptr& v )
{
  SET_PROP( "text", WIDGET->setText( v->stringValue() ), StringType )

  return KSObject_QWidget::setMember( context, name, v );
}
