#include "kscript_func.h"
#include "kscript_context.h"
#include "kscript_value.h"
#include "kscript_object.h"
#include "kscript_parsenode.h"
#include "kscript_util.h"
#include "kscript_proxy.h"

#include <kapp.h>
#include <dcopclient.h>
#include <klocale.h>
#include <kglobal.h>

#include <iostream.h>

bool KSScriptFunction::call( KSContext& context )
{
  return m_node->eval( context );
}

bool KSScriptFunction::isSignal() const
{
  if ( m_node && m_node->getType() == signal_dcl )
    return true;
  return false;
}

static bool ksfunc_mid( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    uint len = 0xffffffff;
    if ( KSUtil::checkArgumentsCount( context, 3, "mid", false ) )
    {
	if( KSUtil::checkType( context, args[2], KSValue::DoubleType, false ) )
	    len = (uint) args[2]->doubleValue();
	else if( KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
	    len = (uint) args[2]->intValue();
	else
	    return false;
    }
    else if ( !KSUtil::checkArgumentsCount( context, 2, "mid", true ) )
	return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
	return false;

    if( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
	return false;
    int pos = args[1]->intValue();

    QString tmp = args[0]->stringValue().mid( pos, len );
    context.setValue( new KSValue(tmp));
    return true;
}

static bool ksfunc_time( KSContext& context )
{
    KSUtil::checkArgs( context, "s", "time", TRUE );

    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    QTime t = KGlobal::locale()->readTime( args[0]->stringValue() );
    if ( !t.isValid() )
    {
	QString tmp( i18n("Non valid time format: %1") );
	context.setException( new KSException( "ParsingError", tmp.arg( args[0]->stringValue() ), -1 ) );
	return false;
    }

    context.setValue( new KSValue( t ) );

    return true;
}

static bool ksfunc_date( KSContext& context )
{
    KSUtil::checkArgs( context, "s", "date", TRUE );

    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    QDate t = KGlobal::locale()->readDate( args[0]->stringValue() );
    if ( !t.isValid() )
    {
	QString tmp( i18n("Non valid date format: %1") );
	context.setException( new KSException( "ParsingError", tmp.arg( args[0]->stringValue() ), -1 ) );
	return false;
    }

    context.setValue( new KSValue( t ) );

    return true;
}

/**
 * Like QString::length for strings.
 * Returns the length of lists or maps, too.
 */
static bool ksfunc_length( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "length", true ) )
    return false;

  if ( KSUtil::checkType( context, args[0], KSValue::ListType, false ) )
  {
    context.setValue( new KSValue( (KScript::Long) args[0]->listValue().count() ) );
  }
  else if ( KSUtil::checkType( context, args[0], KSValue::MapType, false ) )
  {
    context.setValue( new KSValue( (KScript::Long) args[0]->mapValue().count() ) );
  }
  else if ( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
  {
    context.setValue( new KSValue( (KScript::Long) args[0]->stringValue().length() ) );
  }
  else
  {
    QString tmp( i18n("Can not calculate length of a %1 value") );
    context.setException( new KSException( "CastingError", tmp.arg( args[0]->typeName() ), -1 ) );
    return false;
  }

  return true;
}

static bool ksfunc_lower( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgs( context, args, "s", "lower", TRUE ) )
	return FALSE;

    context.setValue( new KSValue( args[0]->stringValue().lower() ) );

    return TRUE;
}

static bool ksfunc_upper( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgs( context, args, "s", "lower", TRUE ) )
	return FALSE;

    context.setValue( new KSValue( args[0]->stringValue().upper() ) );

    return TRUE;
}

static bool ksfunc_isEmpty( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 1, "isEmpty", true ) )
	return false;

    if ( KSUtil::checkType( context, args[0], KSValue::ListType, false ) )
    {
	context.setValue( new KSValue( args[0]->listValue().isEmpty() ) );
    }
    else if ( KSUtil::checkType( context, args[0], KSValue::MapType, false ) )
    {
	context.setValue( new KSValue( args[0]->mapValue().isEmpty() ) );
    }
    else if ( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
    {
	context.setValue( new KSValue( args[0]->stringValue().isEmpty() ) );
    }
    else
    {
	QString tmp( i18n("Can not determine emptiness of a %1 value") );
	context.setException( new KSException( "CastingError", tmp.arg( args[0]->typeName() ), -1 ) );
	return false;
    }

    return true;
}

/**
 * Like QString::toInt
 */
static bool ksfunc_toInt( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "toInt", true ) )
    return false;

  if ( KSUtil::checkType( context, args[0], KSValue::DoubleType, false ) )
  {
    context.setValue( new KSValue( (KScript::Long)args[0]->doubleValue() ) );
    return true;
  }
  else if ( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
  {
    bool ok;
    KScript::Long l = args[0]->stringValue().toLong( &ok );
    if ( ok )
    {
      context.setValue( new KSValue( l ) );
      return true;
    }
  }

  QString tmp( i18n("Can not calulcate a numerical value from a %1 value") );
  context.setException( new KSException( "CastingError", tmp.arg( args[0]->typeName() ), -1 ) );
  return false;
}

/**
 * Like QString::toFloat
 */
static bool ksfunc_toFloat( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "toFloat", true ) )
    return false;

  if ( KSUtil::checkType( context, args[0], KSValue::IntType, false ) )
  {
    context.setValue( new KSValue( (KScript::Double)args[0]->intValue() ) );
    return true;
  }
  else if ( KSUtil::checkType( context, args[0], KSValue::StringType, false ) )
  {
    bool ok;
    KScript::Double l = args[0]->stringValue().toDouble( &ok );
    if ( ok )
    {
      context.setValue( new KSValue( l ) );
      return true;
    }
  }

  QString tmp( i18n("Can not calulcate a floating point value from a %1 value") );
  context.setException( new KSException( "CastingError", tmp.arg( args[0]->typeName() ), -1 ) );
  return false;
}

/**
 * Like QString::arg
 *
 * Syntax: arg( string, value )
 * Syntax: string.arg( value )
 */
static bool ksfunc_arg( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 2, "arg", true ) )
	return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType, TRUE ) )
	return FALSE;

    QString str = args[0]->stringValue();

    if ( KSUtil::checkType( context, args[1], KSValue::StringType, FALSE ) )
	context.setValue( new KSValue( str.arg( args[1]->stringValue() ) ) );
    else if ( KSUtil::checkType( context, args[1], KSValue::IntType, FALSE ) )
	context.setValue( new KSValue( str.arg( args[1]->intValue() ) ) );
    else if ( KSUtil::checkType( context, args[1], KSValue::DoubleType, FALSE ) )
	context.setValue( new KSValue( str.arg( args[1]->doubleValue() ) ) );
    else if ( KSUtil::checkType( context, args[1], KSValue::CharType, FALSE ) )
	context.setValue( new KSValue( str.arg( args[1]->charValue() ) ) );
    else context.setValue( new KSValue( str.arg( args[1]->toString( context ) ) ) );

    return TRUE;
}

static bool ksfunc_stringListSplit( KSContext &context )
{
  QValueList<KSValue::Ptr> &args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "arg", true ) );

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, TRUE ) )
    return false;

  QString sep = args[0]->stringValue();
  QString str = args[1]->stringValue();

  QStringList strLst = QStringList::split( sep, str );

  KSValue *v = new KSValue( KSValue::ListType );

  QStringList::ConstIterator it = strLst.begin();
  QStringList::ConstIterator end = strLst.end();
  for (; it != end; ++it )
    v->listValue().append( new KSValue( *it ) );

  context.setValue( v );
  return true;
}

static bool ksfunc_connect( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "connect", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::MethodType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::MethodType, true ) )
    return false;

  QString sig = args[0]->methodValue()->name();
  KSObject* o = args[0]->methodValue()->object()->objectValue();

  args[1]->methodValue()->function()->ref();
  if ( !o->connect( sig, args[1]->methodValue()->object()->objectValue(), args[1]->methodValue()->function() ) )
  {
    QString tmp( i18n("The method %1 is no signal") );
    context.setException( new KSException( "NoSignal", tmp.arg( sig ), -1 ) );
    return false;
  }

  return true;
}

static bool ksfunc_print( KSContext& context )
{
  // We know that the context always holds a list of parameters
  QValueList<KSValue::Ptr>::Iterator it = context.value()->listValue().begin();
  QValueList<KSValue::Ptr>::Iterator end = context.value()->listValue().end();

  if ( it == end )
    cout << endl;

  for( ; it != end; ++it )
    cout << (*it)->toString( context ).latin1();

  // context.value()->clear();
  context.setValue( 0 );

  return true;
}

static bool ksfunc_println( KSContext& context )
{
  // We know that the context always holds a list of parameters
  QValueList<KSValue::Ptr>::Iterator it = context.value()->listValue().begin();
  QValueList<KSValue::Ptr>::Iterator end = context.value()->listValue().end();

  if ( it == end )
    cout << endl;

  for( ; it != end; ++it )
    cout << (*it)->toString( context ).latin1() << endl;

  // context.value()->clear();
  context.setValue( 0 );

  return true;
}

static bool ksfunc_application( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( args.count() < 1 )
    {
      KSUtil::tooFewArgumentsError( context, "Application" );
      return false;
    }

    if ( args.count() > 2 )
    {
      KSUtil::tooManyArgumentsError( context, "Application" );
      return false;
    }

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType ) )
	return false;

    QCString objId;

    if ( args.count() == 2 )
    {
      if ( !KSUtil::checkType( context, args[1], KSValue::StringType ) )
	return false;

      objId = args[1]->stringValue().latin1();
    }

    context.setValue( new KSValue( new KSProxy( args[0]->stringValue().latin1(), objId ) ) );

    return TRUE;
}

static bool ksfunc_startApplication( KSContext& context )
{
    if ( !KSUtil::checkArgumentsCount( context, 2, "startApplication" ) )
	return false;

    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType ) )
	return false;
    if ( !KSUtil::checkType( context, args[1], KSValue::StringType ) )
	return false;

    QString error;
    QCString dcopService;
    int res = KApplication::startServiceByDesktopName( args[0]->stringValue(),
						       QString::null, &error, &dcopService );

    if ( res != 0 )
    {
      qDebug( "klauncher error: %s", error.ascii() );
      return false;
    }

    context.setValue( new KSValue( new KSProxy( dcopService, args[1]->stringValue().latin1() ) ) );

    return TRUE;
}

static bool ksfunc_dcopCall( KSContext &context )
{
  QValueList<KSValue::Ptr> &args = context.value()->listValue();

  if ( args.count() < 3 )
  {
    KSUtil::tooFewArgumentsError( context, "dcopCall" );
    return false;
  }

  if ( args.count() > 4 )
  {
    KSUtil::tooManyArgumentsError( context, "dcopCall" );
    return false;
  }

  //  if ( !KSUtil::checkArgs( context, args, "sss", "dcopCall" ) )
  //    return false;

  QCString app = args[0]->stringValue().latin1();
  QCString obj = args[1]->stringValue().latin1();
  QCString fun = args[2]->stringValue().latin1();

  QByteArray data;
  QDataStream str( data, IO_WriteOnly );
  /*
  if ( args.count() >= 4 )
  {
    for ( int i = 3; i < args.count(); i++ )
    {
      KSProxy::pack( context, str, args[i] );
      if ( context.exception() )
        return false;
    }
  }
  */
  if ( args.count() == 4 )
  {
    QValueList<KSValue::Ptr> &lst = args[3]->listValue();
    QValueList<KSValue::Ptr>::Iterator it = lst.begin();
    QValueList<KSValue::Ptr>::Iterator end = lst.end();
    for (; it != end; ++it )
    {
      KSProxy::pack( context, str, *it );
      if ( context.exception() )
        return false;
    }

    KSProxy::pack( context, str, args[3] );
    if ( context.exception() )
      return false;
  }

  QByteArray replyData;
  QCString replyType;

  return kapp->dcopClient()->call( app, obj, fun, data, replyType, replyData );
}

KSModule::Ptr ksCreateModule_KScript( KSInterpreter* interp )
{
  KSModule::Ptr module = new KSModule( interp, "kscript" );

  module->addObject( "time", new KSValue( new KSBuiltinFunction( module, "time", ksfunc_time ) ) );
  module->addObject( "date", new KSValue( new KSBuiltinFunction( module, "date", ksfunc_date ) ) );
  module->addObject( "print", new KSValue( new KSBuiltinFunction( module, "print", ksfunc_print ) ) );
  module->addObject( "println", new KSValue( new KSBuiltinFunction( module, "println", ksfunc_println ) ) );
  module->addObject( "connect", new KSValue( new KSBuiltinFunction( module, "connect", ksfunc_connect ) ) );
  module->addObject( "length", new KSValue( new KSBuiltinFunction( module, "length", ksfunc_length ) ) );
  module->addObject( "arg", new KSValue( new KSBuiltinFunction( module, "arg", ksfunc_arg ) ) );
  module->addObject( "mid", new KSValue( new KSBuiltinFunction( module, "mid", ksfunc_mid ) ) );
  module->addObject( "upper", new KSValue( new KSBuiltinFunction( module, "upper", ksfunc_upper ) ) );
  module->addObject( "lower", new KSValue( new KSBuiltinFunction( module, "lower", ksfunc_lower ) ) );
  module->addObject( "isEmpty", new KSValue( new KSBuiltinFunction( module, "isEmpty", ksfunc_isEmpty ) ) );
  module->addObject( "toInt", new KSValue( new KSBuiltinFunction( module, "toInt", ksfunc_toInt ) ) );
  module->addObject( "toFloat", new KSValue( new KSBuiltinFunction( module, "toFloat", ksfunc_toFloat ) ) );
  module->addObject( "stringListSplit", new KSValue( new KSBuiltinFunction( module, "stringListSplit", ksfunc_stringListSplit ) ) );
  module->addObject( "findApplication", new KSValue( new KSBuiltinFunction( module, "findApplication", ksfunc_application ) ) );
  module->addObject( "startApplication", new KSValue( new KSBuiltinFunction( module, "startApplication", ksfunc_startApplication ) ) );
  module->addObject( "dcopCall", new KSValue( new KSBuiltinFunction( module, "dcopCall", ksfunc_dcopCall ) ) );

  return module;
}
