// We want to access the signals
#include <qobjectdefs.h>
#ifdef signals
#undef signals
#define signals public
#endif

#include "kscript_ext_qlineedit.h"
#include "kscript_context.h"
#include "kscript_util.h"

#include <qlineedit.h>

#undef WIDGET
#define WIDGET ((QLineEdit*)object())
#undef CHECKTYPE
#define CHECKTYPE( context, v, type ) if ( !checkType( context, v, type ) ) return FALSE;
#define RETURN_LEFTEXPR( n, value ) if ( name == n ) { KSValue::Ptr ptr = value; ptr->setMode( KSValue::LeftExpr ); return ptr; }
#define RETURN_RIGHTEXPR( n, value ) if ( name == n ) { return value; }
#define CHECK_LEFTEXPR( context, name ) if ( context.leftExpr() ) return KSObject::member( context, name );
#define SET_PROP( __n, __expr, __t ) if ( name == __n ) { CHECKTYPE( context, v, __t ); __expr; return TRUE; }

KSClass_QLineEdit::KSClass_QLineEdit( KSModule* m, const char* name ) : KSClass_QWidget( m, name )
{
  nameSpace()->insert( "QLineEdit", new KSValue( (KSBuiltinMethod)&KSObject_QLineEdit::ksQLineEdit ) );
  nameSpace()->insert( "setText", new KSValue( (KSBuiltinMethod)&KSObject_QLineEdit::ksQLineEdit_setText ) );
  nameSpace()->insert( "textChanged", new KSValue( (KSBuiltinMethod)&KSObject_QLineEdit::ksQLineEdit_textChanged ) );
  nameSpace()->insert( "returnPressed", new KSValue( (KSBuiltinMethod)&KSObject_QLineEdit::ksQLineEdit_returnPressed ) );
  addQtSignal( "textChanged" );
}

KSScriptObject* KSClass_QLineEdit::createObject( KSClass* c )
{
  return new KSObject_QLineEdit( c );
}

// ------------------------------------------------------

KSObject_QLineEdit::KSObject_QLineEdit( KSClass* c ) : KSObject_QWidget( c )
{
}

bool KSObject_QLineEdit::ksQLineEdit( KSContext& context )
{
  qDebug("QLineEdit\n");

  if ( !checkDoubleConstructor( context, "QLineEdit" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  QWidget* parent = 0;
  QString name;

  if ( args.count() >= 1 )
  {
      if ( !checkArguments( context, context.value(), "QLineEdit::QLineEdit", KS_Qt_Object::WidgetType ) )
	  return FALSE;
      parent = KSObject_QWidget::convert( args[0] );
  }
  if ( args.count() >= 2 )
  {
      if ( !checkArguments( context, context.value(), "QLineEdit::QLineEdit", KS_Qt_Object::StringType ) )
	  return FALSE;
      name = args[1]->stringValue();
  }
  if ( args.count() > 2 )
  {
      KSUtil::tooFewArgumentsError( context, "QLineEdit::QLineEdit" );
      return FALSE;
  }

  setObject( new QLineEdit( parent, name.latin1() ) );

  return true;
}

void KSObject_QLineEdit::setObject( QObject* obj )
{
    if ( obj )
	KS_Qt_Callback::self()->connect( obj, SIGNAL( textChanged( const QString& ) ),
					 SLOT( textChanged( const QString& ) ), this, "textChanged" );

    KSObject_QWidget::setObject( obj );
}

bool KSObject_QLineEdit::ksQLineEdit_setText( KSContext& context )
{
  qDebug("QLineEdit::setText\n");

  if ( !checkLive( context, "QLineEdit::setText" ) )
    return false;

  if ( !KSUtil::checkArgumentsCount( context, 1, "QLineEdit::setText" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !checkType( context, args[0], KS_Qt_Object::StringType ) )
      return false;

  QLineEdit* w = (QLineEdit*)object();
  w->setText( args[0]->stringValue() );

  return true;
}

bool KSObject_QLineEdit::ksQLineEdit_textChanged( KSContext& context )
{
  if ( !checkLive( context, "QLineEdit::textChanged" ) )
    return false;

  if ( !KSUtil::checkArgumentsCount( context, 1, "QLineEdit::textChanged" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !checkType( context, args[0], KS_Qt_Object::StringType ) )
      return false;

  QLineEdit* w = (QLineEdit*)object();
  w->textChanged( args[0]->stringValue() );

  return true;
}

bool KSObject_QLineEdit::ksQLineEdit_returnPressed( KSContext& context )
{
  if ( !checkLive( context, "QLineEdit::returnPressed" ) )
    return false;

  if ( !KSUtil::checkArgumentsCount( context, 0, "QLineEdit::returnPressed" ) )
    return false;

  QLineEdit* w = (QLineEdit*)object();
  w->returnPressed();

  return true;
}


KSValue::Ptr KSObject_QLineEdit::member( KSContext& context, const QString& name )
{
/*
  CHECK_LEFTEXPR( context, name );

  RETURN_LEFTEXPR( "text", new KSValue( WIDGET->text() ) );
*/
  return KSObject_QWidget::member( context, name );
}

bool KSObject_QLineEdit::setMember( KSContext& context, const QString& name, const KSValue::Ptr& v )
{
//  SET_PROP( "text", WIDGET->setText( v->stringValue() ), StringType );

  return KSObject_QWidget::setMember( context, name, v );
}
