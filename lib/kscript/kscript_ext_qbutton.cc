// We want to access the signals
#include <qobjectdefs.h>
#ifdef signals
#undef signals
#define signals public
#endif

#include "kscript_ext_qbutton.h"
#include "kscript_context.h"
#include "kscript_util.h"

#include <qbutton.h>

#undef WIDGET
#define WIDGET ((QButton*)object())
#undef CHECKTYPE
#define CHECKTYPE( context, v, type ) if ( !checkType( context, v, type ) ) return FALSE;
#define RETURN_LEFTEXPR( n, value ) if ( name == n ) { KSValue::Ptr ptr = value; ptr->setMode( KSValue::LeftExpr ); return ptr; }
#define RETURN_RIGHTEXPR( n, value ) if ( name == n ) { return value; }
#define CHECK_LEFTEXPR( context, name ) if ( context.leftExpr() ) return KSObject::member( context, name );
#define SET_PROP( __n, __expr, __t ) if ( name == __n ) { CHECKTYPE( context, v, __t ); __expr; return TRUE; }

KSClass_QButton::KSClass_QButton( KSModule* m, const char* name ) : KSClass_QWidget( m, name )
{
  nameSpace()->insert( "QButton", new KSValue( (KSBuiltinMethod)&KSObject_QButton::ksQButton ) );
  nameSpace()->insert( "setText", new KSValue( (KSBuiltinMethod)&KSObject_QButton::ksQButton_setText ) );
  nameSpace()->insert( "clicked", new KSValue( (KSBuiltinMethod)&KSObject_QButton::ksQButton_clicked ) );

  addQtSignal( "clicked" );
}

KSScriptObject* KSClass_QButton::createObject( KSClass* c )
{
  return new KSObject_QButton( c );
}

// ------------------------------------------------------

KSObject_QButton::KSObject_QButton( KSClass* c ) : KSObject_QWidget( c )
{
}

bool KSObject_QButton::ksQButton( KSContext& context )
{
  qDebug("QButton\n");

  if ( !checkDoubleConstructor( context, "QButton" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  QWidget* parent = 0;
  QString name;

  if ( args.count() >= 1 )
  {
      if ( !checkArguments( context, context.value(), "QButton::QButton", KS_Qt_Object::WidgetType ) )
	  return FALSE;
      parent = KSObject_QWidget::convert( args[0] );
  }
  if ( args.count() >= 2 )
  {
      if ( !checkArguments( context, context.value(), "QButton::QButton", KS_Qt_Object::StringType ) )
	  return FALSE;
      name = args[1]->stringValue();
  }
  if ( args.count() > 2 )
  {
      KSUtil::tooFewArgumentsError( context, "QButton::QButton" );
      return FALSE;
  }

  setObject( new QButton( parent, name.latin1() ) );

  qDebug("QButton 2\n");

  return true;
}

void KSObject_QButton::setObject( QObject* obj )
{
    if ( obj )
	KS_Qt_Callback::self()->connect( obj, SIGNAL( clicked() ),
					 SLOT( clicked() ), this, "clicked" );

    KSObject_QWidget::setObject( obj );
}

bool KSObject_QButton::ksQButton_setText( KSContext& context )
{
  qDebug("QButton::setText\n");

  if ( !checkLive( context, "QButton::setText" ) )
    return false;

  if ( !KSUtil::checkArgumentsCount( context, 1, "QButton::setText" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !checkType( context, args[0], KS_Qt_Object::StringType ) )
      return false;

  QButton* w = (QButton*)object();
  w->setText( args[0]->stringValue() );

  return true;
}

bool KSObject_QButton::ksQButton_clicked( KSContext& context )
{
  if ( !checkLive( context, "QButton::clicked" ) )
    return false;

  if ( !KSUtil::checkArgumentsCount( context, 0, "QButton::clicked" ) )
    return false;

  WIDGET->clicked();

  return true;
}

KSValue::Ptr KSObject_QButton::member( KSContext& context, const QString& name )
{
  CHECK_LEFTEXPR( context, name );

  RETURN_LEFTEXPR( "text", new KSValue( WIDGET->text() ) );

  return KSObject_QWidget::member( context, name );
}

bool KSObject_QButton::setMember( KSContext& context, const QString& name, const KSValue::Ptr& v )
{
  SET_PROP( "text", WIDGET->setText( v->stringValue() ), StringType )

  return KSObject_QWidget::setMember( context, name, v );
}
