// We want to access the signals
#include <qobjectdefs.h>
#ifdef signals
#undef signals
#define signals public
#endif

#include "kscript_ext_qlistbox.h"
#include "kscript_context.h"
#include "kscript_util.h"

#include <qlistbox.h>

#undef WIDGET
#define WIDGET ((QListBox*)object())
#undef CHECKTYPE
#define CHECKTYPE( context, v, type ) if ( !checkType( context, v, type ) ) return FALSE;
#define RETURN_LEFTEXPR( n, value ) if ( name == n ) { KSValue::Ptr ptr = value; ptr->setMode( KSValue::LeftExpr ); return ptr; }
#define RETURN_RIGHTEXPR( n, value ) if ( name == n ) { return value; }
#define CHECK_LEFTEXPR( context, name ) if ( context.leftExpr() ) return KSObject::member( context, name );
#define SET_PROP( __n, __expr, __t ) if ( name == __n ) { CHECKTYPE( context, v, __t ); __expr; return TRUE; }

KSClass_QListBox::KSClass_QListBox( KSModule* m, const char* name ) : KSClass_QWidget( m, name )
{
  nameSpace()->insert( "QListBox", new KSValue( (KSBuiltinMethod)&KSObject_QListBox::ksQListBox ) );
  nameSpace()->insert( "selected", new KSValue( (KSBuiltinMethod)&KSObject_QListBox::ksQListBox_selected ) );
  nameSpace()->insert( "insertItem", new KSValue( (KSBuiltinMethod)&KSObject_QListBox::ksQListBox_insertItem ) );
  nameSpace()->insert( "clear", new KSValue( (KSBuiltinMethod)&KSObject_QListBox::ksQListBox_clear ) );

  addQtSignal( "selected" );
}

KSScriptObject* KSClass_QListBox::createObject( KSClass* c )
{
  return new KSObject_QListBox( c );
}

// ------------------------------------------------------

KSObject_QListBox::KSObject_QListBox( KSClass* c ) : KSObject_QWidget( c )
{
}

bool KSObject_QListBox::ksQListBox( KSContext& context )
{
  qDebug("QListBox");

  if ( !checkDoubleConstructor( context, "QListBox" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  QWidget* parent = 0;
  QString name;

  if ( args.count() >= 1 )
  {
      if ( !checkArguments( context, context.value(), "QListBox::QListBox", KS_Qt_Object::WidgetType ) )
	  return FALSE;
      parent = KSObject_QWidget::convert( args[0] );
  }
  if ( args.count() >= 2 )
  {
      if ( !checkArguments( context, context.value(), "QListBox::QListBox", KS_Qt_Object::StringType ) )
	  return FALSE;
      name = args[1]->stringValue();
  }
  if ( args.count() > 2 )
  {
      KSUtil::tooFewArgumentsError( context, "QListBox::QListBox" );
      return FALSE;
  }

  setObject( new QListBox( parent, name.latin1() ) );

  qDebug("QListBox 2\n");

  return true;
}

void KSObject_QListBox::setObject( QObject* obj )
{
    if ( obj )
    {
	KS_Qt_Callback::self()->connect( obj, SIGNAL( selected( int ) ),
					 SLOT( selected( int ) ), this, "selected" );
        KS_Qt_Callback::self()->connect( obj, SIGNAL( selected( const QString& ) ),
                                         SLOT( selected( const QString& ) ), this, "selected" );
    }

    KSObject_QWidget::setObject( obj );
}

bool KSObject_QListBox::ksQListBox_selected( KSContext& context )
{
  if ( !checkLive( context, "QListBox::selected" ) )
    return false;

  if ( !KSUtil::checkArgumentsCount( context, 0, "QListBox::selected" ) )
    return false;

// TODO get argument and pass it to Qt-Signal
//  WIDGET->activated();

  return true;
}

bool KSObject_QListBox::ksQListBox_insertItem( KSContext& context )
{
  if ( !checkLive( context, "QListBox::insertItem" ) )
    return false;

  qDebug("QListBox::insertItem\n");

  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  QString text;
  int id = -1;

  if ( args.count() >= 1 )
  {
      if ( !checkArguments( context, context.value(), "QListBox::insertItem", KS_Qt_Object::StringType ) )
	  return FALSE;
      text = args[0]->stringValue();
  }
  if ( args.count() >= 2 )
  {
      if ( !checkArguments( context, context.value(), "QListBox::insertItem", KS_Qt_Object::IntType ) )
	  return FALSE;
      id = args[1]->intValue();
  }
  if ( args.count() > 2 || args.count() == 0 )
  {
      KSUtil::tooFewArgumentsError( context, "QListBox::insertItem" );
      return FALSE;
  }

  qDebug("QListBox::insertItem 2 \n");

  QListBox* w = (QListBox*)object();
  w->insertItem( text, id );

  return true;

  qDebug("QListBox::insertItem 3\n");
}

bool KSObject_QListBox::ksQListBox_clear( KSContext& context )
{
  if ( !checkLive( context, "QListBox::clear" ) )
    return false;

  qDebug("QListBox::clear\n");

  if ( !KSUtil::checkArgumentsCount( context, 0, "QListBox::clear" ) )
      return false;

  QListBox* w = (QListBox*)object();
  w->clear();

  return TRUE;
}

KSValue::Ptr KSObject_QListBox::member( KSContext& context, const QString& name )
{
//  CHECK_LEFTEXPR( context, name );

  return KSObject_QWidget::member( context, name );
}

bool KSObject_QListBox::setMember( KSContext& context, const QString& name, const KSValue::Ptr& v )
{
  return KSObject_QWidget::setMember( context, name, v );
}
