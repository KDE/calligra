#include "kscript_ext_qwidget.h"
#include "kscript_ext_qstructs.h"
#include "kscript_context.h"
#include "kscript_util.h"

#include <qwidget.h>

#undef WIDGET
#define WIDGET ((QWidget*)object())
#undef CHECKTYPE
#define CHECKTYPE( context, v, type ) if ( !checkType( context, v, type ) ) return FALSE;
#define RETURN_LEFTEXPR( n, value ) if ( name == n ) { KSValue::Ptr ptr = value; ptr->setMode( KSValue::LeftExpr ); return ptr; }
#define RETURN_RIGHTEXPR( n, value ) if ( name == n ) { return value; }
#define CHECK_LEFTEXPR( context, name ) if ( context.leftExpr() ) return KSObject::member( context, name );
#define SET_PROP( __n, __expr, __t ) if ( name == __n ) { CHECKTYPE( context, v, __t ); __expr; return TRUE; }

KSClass_QWidget::KSClass_QWidget( KSModule* m, const char* name ) : KSClass_QObject( m, name )
{
  nameSpace()->insert( "QWidget", new KSValue( (KSBuiltinMethod)&KSObject_QWidget::ksQWidget ) );
  nameSpace()->insert( "show", new KSValue( (KSBuiltinMethod)&KSObject_QWidget::ksQWidget_show ) );
}

KSScriptObject* KSClass_QWidget::createObject( KSClass* c )
{
  return new KSObject_QWidget( c );
}

KSObject_QWidget::KSObject_QWidget( KSClass* c ) : KS_Qt_Object( c )
{
}

bool KSObject_QWidget::ksQWidget( KSContext& context )
{
  qDebug("QWidget count=%i\n", count);

  if ( !checkDoubleConstructor( context, "QWidget" ) )
    return false;

  QWidget* parent = 0;
  QString name;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( args.count() >= 1 )
  {
      if ( !checkArguments( context, context.value(), "QWidget::QWidget", KS_Qt_Object::WidgetType ) )
	  return FALSE;
      parent = KSObject_QWidget::convert( args[0] );
  }
  if ( args.count() >= 2 )
  {
      if ( !checkArguments( context, context.value(), "QWidget::QWidget", KS_Qt_Object::StringType ) )
	  return FALSE;
      name = args[1]->stringValue();
  }
  if ( args.count() > 2 )
  {
      KSUtil::tooFewArgumentsError( context, "QWidget::QWidget" );
      return FALSE;
  }

  setObject( new QWidget( parent, name.latin1() ) );

  qDebug("QWidget end count=%i\n", count);

  return true;
}

bool KSObject_QWidget::ksQWidget_show( KSContext& context )
{
  qDebug("QWidget::show\n");

  if ( !checkLive( context, "QWidget::show" ) )
    return false;

  if ( !KSUtil::checkArgumentsCount( context, 0, "QWidget::QWidget" ) )
    return false;

  QWidget* w = (QWidget*)object();
  w->show();

  return true;
}

/*
bool KSObject_QWidget::ksQWidget_delete( KSContext& context )
{
  qDebug("QWidget::delete\n");

  if ( !KSUtil::checkArgumentsCount( context, 0, "QWidget::delete" ) )
    return false;

  if ( !object() )
    return true;

  delete object();
  setObject( 0 );

  return true;
}
*/
KSValue::Ptr KSObject_QWidget::member( KSContext& context, const QString& name )
{
  RETURN_RIGHTEXPR( "width", new KSValue( WIDGET->width() ) );
  RETURN_RIGHTEXPR( "height", new KSValue( WIDGET->height() ) );
  RETURN_RIGHTEXPR( "x", new KSValue( WIDGET->x() ) );
  RETURN_RIGHTEXPR( "y", new KSValue( WIDGET->y() ) );

  CHECK_LEFTEXPR( context, name );

  RETURN_LEFTEXPR( "caption", new KSValue( WIDGET->caption() ) );
  RETURN_LEFTEXPR( "geometry", KSQt::Rect::convert( context, WIDGET->geometry() ) );

  return KS_Qt_Object::member( context, name );
}

bool KSObject_QWidget::setMember( KSContext& context, const QString& name, const KSValue::Ptr& v )
{
  SET_PROP( "caption", WIDGET->setCaption( v->stringValue() ), StringType )
  SET_PROP( "geometry", WIDGET->setGeometry( KSQt::Rect::convert( context, v ) ), RectType )

  return KS_Qt_Object::setMember( context, name, v );
}
