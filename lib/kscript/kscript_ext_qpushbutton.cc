// We want to access the signals
#include <qobjectdefs.h>
#ifdef signals
#undef signals
#define signals public
#endif

#include "kscript_ext_qpushbutton.h"
#include "kscript_ext_qrect.h"
#include "kscript_context.h"
#include "kscript_util.h"

#include <qpushbutton.h>

#define WIDGET ((QPushButton*)object())
#define CHECKTYPE( context, v, type ) if ( !checkType( context, v, type ) ) return FALSE;
#define RETURN_LEFTEXPR( n, value ) if ( name == n ) { KSValue::Ptr ptr = value; ptr->setMode( KSValue::LeftExpr ); return ptr; }
#define RETURN_RIGHTEXPR( n, value ) if ( name == n ) { return value; }
#define CHECK_LEFTEXPR( context, name ) if ( context.leftExpr() ) return KSObject::member( context, name );
#define SET_PROP( __n, __expr, __t ) if ( name == __n ) { CHECKTYPE( context, v, __t ); __expr; return TRUE; }

KSClass_QPushButton::KSClass_QPushButton( KSModule* m, const char* name ) : KSClass_QWidget( m, name )
{
  nameSpace()->insert( "QPushButton", new KSValue( (KSBuiltinMethod)&KSObject_QPushButton::ksQPushButton ) );
  nameSpace()->insert( "setText", new KSValue( (KSBuiltinMethod)&KSObject_QPushButton::ksQPushButton_setText ) );
  nameSpace()->insert( "clicked", new KSValue( (KSBuiltinMethod)&KSObject_QPushButton::ksQPushButton_clicked ) );

  addQtSignal( "clicked" );
}

KSScriptObject* KSClass_QPushButton::createObject( KSClass* c )
{
  return new KSObject_QPushButton( c );
}

// ------------------------------------------------------

KSObject_QPushButton::KSObject_QPushButton( KSClass* c ) : KSObject_QWidget( c )
{
}

bool KSObject_QPushButton::ksQPushButton( KSContext& context )
{
  qDebug("QPushButton\n");

  if ( !checkDoubleConstructor( context, "QPushButton" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();
    
  QWidget* parent = 0;
  QString name;

  if ( args.count() >= 1 )
  {
      if ( !checkArguments( context, context.value(), "QPushButton::QPushButton", KS_Qt_Object::WidgetType ) )
	  return FALSE;
      parent = KSObject_QWidget::convert( args[0] );
  }
  if ( args.count() >= 2 )
  {
      if ( !checkArguments( context, context.value(), "QPushButton::QPushButton", KS_Qt_Object::StringType ) )
	  return FALSE;
      name = args[1]->stringValue();
  }
  if ( args.count() > 2 )
  {
      KSUtil::tooFewArgumentsError( context, "QPushButton::QPushButton" );
      return FALSE;
  }

  setObject( new QPushButton( parent, name ) );

  qDebug("QPushButton 2\n");

  return true;
}

void KSObject_QPushButton::setObject( QObject* obj )
{
    if ( obj )
	KS_Qt_Callback::self()->connect( obj, SIGNAL( clicked() ),
					 SLOT( clicked() ), this, "clicked" );

    KSObject_QWidget::setObject( obj );
}

bool KSObject_QPushButton::ksQPushButton_setText( KSContext& context )
{
  qDebug("QPushButton::setText\n");

  if ( !checkLive( context, "QPushButton::setText" ) )
    return false;

  if ( !KSUtil::checkArgumentsCount( context, 1, "QPushButton::setText" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !checkType( context, args[0], KS_Qt_Object::StringType ) )
      return false;

  QPushButton* w = (QPushButton*)object();
  w->setText( args[0]->stringValue() );

  return true;
}

bool KSObject_QPushButton::ksQPushButton_clicked( KSContext& context )
{
  if ( !checkLive( context, "QPushButton::clicked" ) )
    return false;

  if ( !KSUtil::checkArgumentsCount( context, 0, "QPushButton::clicked" ) )
    return false;

  WIDGET->clicked();

  return true;
}

KSValue::Ptr KSObject_QPushButton::member( KSContext& context, const QString& name )
{
  CHECK_LEFTEXPR( context, name );

  RETURN_LEFTEXPR( "text", new KSValue( WIDGET->text() ) );

  return KSObject_QWidget::member( context, name );
}

bool KSObject_QPushButton::setMember( KSContext& context, const QString& name, const KSValue::Ptr& v )
{
  SET_PROP( "text", WIDGET->setText( v->stringValue() ), StringType )

  return KSObject_QWidget::setMember( context, name, v );
}
