// We want to access the signals
#include <qobjectdefs.h>
#ifdef signals
#undef signals
#define signals public
#endif

#include "kscript_ext_qvboxlayout.h"
#include "kscript_context.h"
#include "kscript_util.h"

#include <qlayout.h>

#undef WIDGET
#define WIDGET ((QVBoxLayout*)object())
#undef CHECKTYPE
#define CHECKTYPE( context, v, type ) if ( !checkType( context, v, type ) ) return FALSE;
#define RETURN_LEFTEXPR( n, value ) if ( name == n ) { KSValue::Ptr ptr = value; ptr->setMode( KSValue::LeftExpr ); return ptr; }
#define RETURN_RIGHTEXPR( n, value ) if ( name == n ) { return value; }
#define CHECK_LEFTEXPR( context, name ) if ( context.leftExpr() ) return KSObject::member( context, name );
#define SET_PROP( __n, __expr, __t ) if ( name == __n ) { CHECKTYPE( context, v, __t ); __expr; return TRUE; }

KSClass_QVBoxLayout::KSClass_QVBoxLayout( KSModule* m, const char* name ) : KSClass_QObject( m, name )
{
  nameSpace()->insert( "QVBoxLayout", new KSValue( (KSBuiltinMethod)&KSObject_QVBoxLayout::ksQVBoxLayout ) );
  nameSpace()->insert( "addWidget", new KSValue( (KSBuiltinMethod)&KSObject_QVBoxLayout::ksQVBoxLayout_addWidget ) );
}

KSScriptObject* KSClass_QVBoxLayout::createObject( KSClass* c )
{
  return new KSObject_QVBoxLayout( c );
}

// ------------------------------------------------------

KSObject_QVBoxLayout::KSObject_QVBoxLayout( KSClass* c ) : KS_Qt_Object( c )
{
}

bool KSObject_QVBoxLayout::ksQVBoxLayout( KSContext& context )
{
  qDebug("QVBoxLayout\n");

  if ( !checkDoubleConstructor( context, "QVBoxLayout" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  QWidget* parent = 0;
  QString name;

  if ( args.count() >= 1 )
  {
      if ( !checkArguments( context, context.value(), "QVBoxLayout::QVBoxLayout", KS_Qt_Object::WidgetType ) )
	  return FALSE;
      parent = KSObject_QWidget::convert( args[0] );
  }
  if ( args.count() >= 2 )
  {
      if ( !checkArguments( context, context.value(), "QVBoxLayout::QVBoxLayout", KS_Qt_Object::StringType ) )
	  return FALSE;
      name = args[1]->stringValue();
  }
  if ( args.count() > 2 )
  {
      KSUtil::tooFewArgumentsError( context, "QVBoxLayout::QVBoxLayout" );
      return FALSE;
  }

  setObject( new QVBoxLayout( parent, 6, 6, name ) );

  return true;
}

bool KSObject_QVBoxLayout::ksQVBoxLayout_addWidget( KSContext& context )
{
  qDebug("QVBoxLayout::addWidget\n");

  if ( !checkLive( context, "QVBoxLayout::addWidget" ) )
    return false;

  if ( !KSUtil::checkArgumentsCount( context, 1, "QVBoxLayout::addWidget" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !checkType( context, args[0], KS_Qt_Object::WidgetType ) )
      return false;

  WIDGET->addWidget( KSObject_QWidget::convert( args[0] ) );

  return true;
}


KSValue::Ptr KSObject_QVBoxLayout::member( KSContext& context, const QString& name )
{
  CHECK_LEFTEXPR( context, name );

  return KS_Qt_Object::member( context, name );
}

bool KSObject_QVBoxLayout::setMember( KSContext& context, const QString& name, const KSValue::Ptr& v )
{
  return KS_Qt_Object::setMember( context, name, v );
}
