// We want to access the signals
#include <qobjectdefs.h>
#ifdef signals
#undef signals
#define signals public
#endif

#include "kscript_ext_qboxlayout.h"
#include "kscript_context.h"
#include "kscript_util.h"

#include <qlayout.h>

#undef OBJECT
#define OBJECT ((QBoxLayout*)object())
#undef CHECKTYPE
#define CHECKTYPE( context, v, type ) if ( !checkType( context, v, type ) ) return FALSE;
#define RETURN_LEFTEXPR( n, value ) if ( name == n ) { KSValue::Ptr ptr = value; ptr->setMode( KSValue::LeftExpr ); return ptr; }
#define RETURN_RIGHTEXPR( n, value ) if ( name == n ) { return value; }
#define CHECK_LEFTEXPR( context, name ) if ( context.leftExpr() ) return KSObject::member( context, name );
#define SET_PROP( __n, __expr, __t ) if ( name == __n ) { CHECKTYPE( context, v, __t ); __expr; return TRUE; }

KSClass_QBoxLayout::KSClass_QBoxLayout( KSModule* m, const char* name ) : KSClass_QObject( m, name )
{
  nameSpace()->insert( "QBoxLayout", new KSValue( (KSBuiltinMethod)&KSObject_QBoxLayout::ksQBoxLayout ) );
  nameSpace()->insert( "addWidget", new KSValue( (KSBuiltinMethod)&KSObject_QBoxLayout::ksQBoxLayout_addWidget ) );
  nameSpace()->insert( "addLayout", new KSValue( (KSBuiltinMethod)&KSObject_QBoxLayout::ksQBoxLayout_addLayout ) );
}

KSScriptObject* KSClass_QBoxLayout::createObject( KSClass* c )
{
  return new KSObject_QBoxLayout( c );
}

// ------------------------------------------------------

KSObject_QBoxLayout::KSObject_QBoxLayout( KSClass* c ) : KS_Qt_Object( c )
{
}

bool KSObject_QBoxLayout::ksQBoxLayout( KSContext& context )
{
/*  qDebug("QBoxLayout\n");

  if ( !checkDoubleConstructor( context, "QBoxLayout" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  QWidget* parent = 0;
  QString name;

  if ( args.count() >= 1 )
  {
      if ( !checkArguments( context, context.value(), "QBoxLayout::QBoxLayout", KS_Qt_Object::WidgetType ) )
	  return FALSE;
      parent = KSObject_QWidget::convert( args[0] );
  }
  if ( args.count() >= 2 )
  {
      if ( !checkArguments( context, context.value(), "QBoxLayout::QBoxLayout", KS_Qt_Object::StringType ) )
	  return FALSE;
      name = args[1]->stringValue();
  }
  if ( args.count() > 2 )
  {
      KSUtil::tooFewArgumentsError( context, "QBoxLayout::QBoxLayout" );
      return FALSE;
  }

  setObject( new QBoxLayout( parent, 6, 6, name ) );*/

  return false;
}

bool KSObject_QBoxLayout::ksQBoxLayout_addWidget( KSContext& context )
{
  qDebug("QBoxLayout::addWidget\n");

  if ( !checkLive( context, "QBoxLayout::addWidget" ) )
    return false;

  if ( !KSUtil::checkArgumentsCount( context, 1, "QBoxLayout::addWidget" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !checkType( context, args[0], KS_Qt_Object::WidgetType ) )
      return false;

  OBJECT->addWidget( KSObject_QWidget::convert( args[0] ) );

  return true;
}

bool KSObject_QBoxLayout::ksQBoxLayout_addLayout( KSContext& context )
{
  qDebug("QBoxLayout::addLayout\n");

  if ( !checkLive( context, "QBoxLayout::addLayout" ) )
    return false;

  if ( !KSUtil::checkArgumentsCount( context, 1, "QBoxLayout::addLayout" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSObject_QBoxLayout::convert( args[0] ) )
      return false;

  OBJECT->addLayout( KSObject_QBoxLayout::convert( args[0] ) );

  return true;
}

KSValue::Ptr KSObject_QBoxLayout::member( KSContext& context, const QString& name )
{
  CHECK_LEFTEXPR( context, name );

  return KS_Qt_Object::member( context, name );
}

bool KSObject_QBoxLayout::setMember( KSContext& context, const QString& name, const KSValue::Ptr& v )
{
  return KS_Qt_Object::setMember( context, name, v );
}
