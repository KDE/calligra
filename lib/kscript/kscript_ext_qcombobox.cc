// We want to access the signals
#include <qobjectdefs.h>
#ifdef signals
#undef signals
#define signals public
#endif

#include "kscript_ext_qcombobox.h"
#include "kscript_context.h"
#include "kscript_util.h"

#include <qcombobox.h>

#undef WIDGET
#define WIDGET ((QComboBox*)object())
#undef CHECKTYPE
#define CHECKTYPE( context, v, type ) if ( !checkType( context, v, type ) ) return FALSE;
#define RETURN_LEFTEXPR( n, value ) if ( name == n ) { KSValue::Ptr ptr = value; ptr->setMode( KSValue::LeftExpr ); return ptr; }
#define RETURN_RIGHTEXPR( n, value ) if ( name == n ) { return value; }
#define CHECK_LEFTEXPR( context, name ) if ( context.leftExpr() ) return KSObject::member( context, name );
#define SET_PROP( __n, __expr, __t ) if ( name == __n ) { CHECKTYPE( context, v, __t ); __expr; return TRUE; }

KSClass_QComboBox::KSClass_QComboBox( KSModule* m, const char* name ) : KSClass_QWidget( m, name )
{
  nameSpace()->insert( "QComboBox", new KSValue( (KSBuiltinMethod)&KSObject_QComboBox::ksQComboBox ) );
  nameSpace()->insert( "activated", new KSValue( (KSBuiltinMethod)&KSObject_QComboBox::ksQComboBox_activated ) );
  nameSpace()->insert( "insertItem", new KSValue( (KSBuiltinMethod)&KSObject_QComboBox::ksQComboBox_insertItem ) );
  nameSpace()->insert( "clear", new KSValue( (KSBuiltinMethod)&KSObject_QComboBox::ksQComboBox_clear ) );

  addQtSignal( "activated" );
}

KSScriptObject* KSClass_QComboBox::createObject( KSClass* c )
{
  return new KSObject_QComboBox( c );
}

// ------------------------------------------------------

KSObject_QComboBox::KSObject_QComboBox( KSClass* c ) : KSObject_QWidget( c )
{
}

bool KSObject_QComboBox::ksQComboBox( KSContext& context )
{
  qDebug("QComboBox\n");

  if ( !checkDoubleConstructor( context, "QComboBox" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  QWidget* parent = 0;
  QString name;

  if ( args.count() >= 1 )
  {
      if ( !checkArguments( context, context.value(), "QComboBox::QComboBox", KS_Qt_Object::WidgetType ) )
	  return FALSE;
      parent = KSObject_QWidget::convert( args[0] );
  }
  if ( args.count() >= 2 )
  {
      if ( !checkArguments( context, context.value(), "QComboBox::QComboBox", KS_Qt_Object::StringType ) )
	  return FALSE;
      name = args[1]->stringValue();
  }
  if ( args.count() > 2 )
  {
      KSUtil::tooFewArgumentsError( context, "QComboBox::QComboBox" );
      return FALSE;
  }

  setObject( new QComboBox( parent, name.latin1() ) );

  qDebug("QComboBox 2\n");

  return true;
}

void KSObject_QComboBox::setObject( QObject* obj )
{
    if ( obj )
    {
	KS_Qt_Callback::self()->connect( obj, SIGNAL( activated( int ) ),
					 SLOT( activated( int ) ), this, "activated" );
        KS_Qt_Callback::self()->connect( obj, SIGNAL( activated( const QString& ) ),
                                         SLOT( activated( const QString& ) ), this, "activated" );
    }

    KSObject_QWidget::setObject( obj );
}

bool KSObject_QComboBox::ksQComboBox_activated( KSContext& context )
{
  if ( !checkLive( context, "QComboBox::activated" ) )
    return false;

  if ( !KSUtil::checkArgumentsCount( context, 0, "QComboBox::activated" ) )
    return false;

// TODO get argument and pass it to Qt-Signal
//  WIDGET->activated();

  return true;
}

bool KSObject_QComboBox::ksQComboBox_insertItem( KSContext& context )
{
  if ( !checkLive( context, "QComboBox::insertItem" ) )
    return false;

  qDebug("QComboBox::insertItem\n");

  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  QString text;
  int id = -1;

  if ( args.count() >= 1 )
  {
      if ( !checkArguments( context, context.value(), "QComboBox::insertItem", KS_Qt_Object::StringType ) )
	  return FALSE;
      text = args[0]->stringValue();
  }
  if ( args.count() >= 2 )
  {
      if ( !checkArguments( context, context.value(), "QComboBox::insertItem", KS_Qt_Object::IntType ) )
	  return FALSE;
      id = args[1]->intValue();
  }
  if ( args.count() > 2 || args.count() == 0 )
  {
      KSUtil::tooFewArgumentsError( context, "QComboBox::insertItem" );
      return FALSE;
  }

  qDebug("QComboBox::insertItem 2 \n");

  QComboBox* w = (QComboBox*)object();
  w->insertItem( text, id );

  return true;

  qDebug("QComboBox::insertItem 3\n");
}

bool KSObject_QComboBox::ksQComboBox_clear( KSContext &context )
{
  if ( !checkLive( context, "QComboBox::clear" ) )
    return false;

  QComboBox *w = static_cast<QComboBox *>( object() );
  w->clear();

  return true;
}

KSValue::Ptr KSObject_QComboBox::member( KSContext& context, const QString& name )
{
//  CHECK_LEFTEXPR( context, name );

  return KSObject_QWidget::member( context, name );
}

bool KSObject_QComboBox::setMember( KSContext& context, const QString& name, const KSValue::Ptr& v )
{
  return KSObject_QWidget::setMember( context, name, v );
}
