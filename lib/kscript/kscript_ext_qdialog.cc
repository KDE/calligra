// We want to access the signals
#include <qobjectdefs.h>
#ifdef signals
#undef signals
#define signals public
#endif

#include "kscript_ext_qdialog.h"
#include "kscript_context.h"
#include "kscript_util.h"

#include <qdialog.h>

#undef WIDGET
#define WIDGET ((QDialog*)object())
#undef CHECKTYPE
#define CHECKTYPE( context, v, type ) if ( !checkType( context, v, type ) ) return FALSE;
#define RETURN_LEFTEXPR( n, value ) if ( name == n ) { KSValue::Ptr ptr = value; ptr->setMode( KSValue::LeftExpr ); return ptr; }
#define RETURN_RIGHTEXPR( n, value ) if ( name == n ) { return value; }
#define CHECK_LEFTEXPR( context, name ) if ( context.leftExpr() ) return KSObject::member( context, name );
#define SET_PROP( __n, __expr, __t ) if ( name == __n ) { CHECKTYPE( context, v, __t ); __expr; return TRUE; }

KSClass_QDialog::KSClass_QDialog( KSModule* m, const char* name ) : KSClass_QWidget( m, name )
{
  nameSpace()->insert( "QDialog", new KSValue( (KSBuiltinMethod)&KSObject_QDialog::ksQDialog ) );
  nameSpace()->insert( "show", new KSValue( (KSBuiltinMethod)&KSObject_QDialog::ksQDialog_show ) );
  nameSpace()->insert( "hide", new KSValue( (KSBuiltinMethod)&KSObject_QDialog::ksQDialog_hide ) );
}

KSScriptObject* KSClass_QDialog::createObject( KSClass* c )
{
  return new KSObject_QDialog( c );
}

// ------------------------------------------------------

KSObject_QDialog::KSObject_QDialog( KSClass* c ) : KSObject_QWidget( c )
{
}

bool KSObject_QDialog::ksQDialog( KSContext& context )
{
  qDebug("QDialog\n");

  if ( !checkDoubleConstructor( context, "QDialog" ) )
    return false;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  QWidget* parent = 0;
  QString name;
  bool modal = FALSE;
  // FIX: right now, you can only specify the modal value, if you provided the parent and name values
  // add WFlags
  if ( args.count() >= 1 )
  {
      if ( !checkArguments( context, context.value(), "QDialog::QDialog", KS_Qt_Object::WidgetType ) )
	  return FALSE;
      parent = KSObject_QWidget::convert( args[0] );
  }
  if ( args.count() >= 2 )
  {
      if ( !checkArguments( context, context.value(), "QDialog::QDialog", KS_Qt_Object::StringType ) )
	  return FALSE;
      name = args[1]->stringValue();
  }
  if ( args.count() >= 3 )
  {
      if ( !checkArguments( context, context.value(), "QDialog::QDialog", KS_Qt_Object::BoolType ) )
	  return FALSE;
      modal = args[2]->boolValue();
  }
  if ( args.count() > 4 )
  {
      KSUtil::tooFewArgumentsError( context, "QDialog::QDialog" );
      return FALSE;
  }

  setObject( new QDialog( parent, name.latin1(), modal ) );

  qDebug("QDialog 2\n");

  return true;
}

bool KSObject_QDialog::ksQDialog_show( KSContext& context )
{
  if ( !KSUtil::checkArgumentsCount( context, 0, "QLineEdit::setText" ) )
    return false;
  QDialog* w = (QDialog*)object();
  w->show();
  return true;
}

bool KSObject_QDialog::ksQDialog_hide( KSContext& context )
{
  if ( !KSUtil::checkArgumentsCount( context, 0, "QLineEdit::setText" ) )
    return false;
  QDialog* w = (QDialog*)object();
  w->hide();
  return true;
}

void KSObject_QDialog::setObject( QObject* obj )
{
//    if ( obj )
//	KS_Qt_Callback::self()->connect( obj, SIGNAL( clicked() ),
//					 SLOT( clicked() ), this, "clicked" );

    KSObject_QWidget::setObject( obj );
}

KSValue::Ptr KSObject_QDialog::member( KSContext& context, const QString& name )
{
  CHECK_LEFTEXPR( context, name );

  return KSObject_QWidget::member( context, name );
}

bool KSObject_QDialog::setMember( KSContext& context, const QString& name, const KSValue::Ptr& v )
{
    // SET_PROP( "geometry", WIDGET->setGeometry( *KSObject_QRect::convert( v ) ), RectType) // is reimplemented in QDialog, so we need it here, too

  return KSObject_QWidget::setMember( context, name, v );
}
