// We want to access the signals
#include <qobjectdefs.h>
#ifdef signals
#undef signals
#define signals public
#endif

#include "kscript_ext_qbuttongroup.h"
#include "kscript_ext_qbutton.h"
#include "kscript_context.h"
#include "kscript_util.h"

#include <qbuttongroup.h>

#undef WIDGET
#define WIDGET ((QButtonGroup*)object())
#undef CHECKTYPE
#define CHECKTYPE( context, v, type ) if ( !checkType( context, v, type ) ) return FALSE;
#define RETURN_LEFTEXPR( n, value ) if ( name == n ) { KSValue::Ptr ptr = value; ptr->setMode( KSValue::LeftExpr ); return ptr; }
#define RETURN_RIGHTEXPR( n, value ) if ( name == n ) { return value; }
#define CHECK_LEFTEXPR( context, name ) if ( context.leftExpr() ) return KSObject::member( context, name );
#define SET_PROP( __n, __expr, __t ) if ( name == __n ) { CHECKTYPE( context, v, __t ); __expr; return TRUE; }

KSClass_QButtonGroup::KSClass_QButtonGroup( KSModule* m, const char* name ) : KSClass_QWidget( m, name )
{
  nameSpace()->insert( "QButtonGroup", new KSValue( (KSBuiltinMethod)&KSObject_QButtonGroup::ksQButtonGroup ) );
  nameSpace()->insert( "insert", new KSValue( (KSBuiltinMethod)&KSObject_QButtonGroup::ksQButtonGroup_insert ) );
  nameSpace()->insert( "remove", new KSValue( (KSBuiltinMethod)&KSObject_QButtonGroup::ksQButtonGroup_remove ) );
  nameSpace()->insert( "clicked", new KSValue( (KSBuiltinMethod)&KSObject_QButtonGroup::ksQButtonGroup_clicked ) );
}

KSScriptObject* KSClass_QButtonGroup::createObject( KSClass* c )
{
  return new KSObject_QButtonGroup( c );
}

// ------------------------------------------------------

KSObject_QButtonGroup::KSObject_QButtonGroup( KSClass* c ) : KSObject_QWidget( c )
{
}

bool KSObject_QButtonGroup::ksQButtonGroup( KSContext& context )
{
  qDebug("QButtonGroup\n");

  if ( !checkDoubleConstructor( context, "QButtonGroup" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  QWidget* parent = 0;
  QString name;

  // TODO: more constructor options, Vertical/Horizontal etc.

  if ( args.count() >= 1 )
  {
      if ( !checkArguments( context, context.value(), "QButtonGroup::QButtonGroup", KS_Qt_Object::WidgetType ) )
	  return FALSE;
      parent = KSObject_QWidget::convert( args[0] );
  }
  if ( args.count() >= 2 )
  {
      if ( !checkArguments( context, context.value(), "QButtonGroup::QButtonGroup", KS_Qt_Object::StringType ) )
	  return FALSE;
      name = args[1]->stringValue();
  }
  if ( args.count() > 2 )
  {
      KSUtil::tooFewArgumentsError( context, "QButtonGroup::QButtonGroup" );
      return FALSE;
  }

  setObject( new QButtonGroup( 0, Qt::Vertical, parent, name ) );

  qDebug("QButtonGroup 2\n");

  return true;
}

void KSObject_QButtonGroup::setObject( QObject* obj )
{
    if ( obj )
	KS_Qt_Callback::self()->connect( obj, SIGNAL( clicked() ),
					 SLOT( clicked() ), this, "clicked" );

    KSObject_QWidget::setObject( obj );
}

bool KSObject_QButtonGroup::ksQButtonGroup_insert( KSContext& context )
{
  qDebug("QButtonGroup::insert\n");

  if ( !checkLive( context, "QButtonGroup::insert" ) )
    return false;

  // TODO: add parameter int id = -1 and remove the checkArgumentsCount line

  if ( !KSUtil::checkArgumentsCount( context, 1, "QButtonGroup::insert" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  QButton* button = 0;
  int id = -1;

  if ( args.count() >= 1 )
  {
      if ( !checkArguments( context, context.value(), "QButtonGroup::QButtonGroup", KS_Qt_Object::WidgetType ) )
	  return FALSE;
      button = KSObject_QButton::convert( args[0] );
  }
  if ( args.count() >= 2 )
  {
      if ( !checkArguments( context, context.value(), "QButtonGroup::QButtonGroup", KS_Qt_Object::IntType ) )
	  return FALSE;
      id = args[1]->intValue();
  }
  if ( args.count() > 2 )
  {
      KSUtil::tooFewArgumentsError( context, "QButtonGroup::QButtonGroup" );
      return FALSE;
  }

  if ( !checkType( context, args[0], KS_Qt_Object::WidgetType ) ) // ButtonType?
      return false;

  QButtonGroup* w = (QButtonGroup*)object();
  w->insert( button );

  return true;
}

bool KSObject_QButtonGroup::ksQButtonGroup_remove( KSContext& context )
{
  qDebug("QButtonGroup::remove\n");
  // TODO

  return true;
}

bool KSObject_QButtonGroup::ksQButtonGroup_clicked( KSContext& context )
{
  if ( !checkLive( context, "QButtonGroup::clicked" ) )
    return false;

  if ( !KSUtil::checkArgumentsCount( context, 0, "QButtonGroup::clicked" ) )
    return false;

  // TODO

  WIDGET->clicked( 0 );

  return true;
}

KSValue::Ptr KSObject_QButtonGroup::member( KSContext& context, const QString& name )
{
  CHECK_LEFTEXPR( context, name );

  RETURN_LEFTEXPR( "title", new KSValue( WIDGET->title() ) );

  return KSObject_QWidget::member( context, name );
}

bool KSObject_QButtonGroup::setMember( KSContext& context, const QString& name, const KSValue::Ptr& v )
{
  SET_PROP( "title", WIDGET->setTitle( v->stringValue() ), StringType )

  return KSObject_QWidget::setMember( context, name, v );
}
