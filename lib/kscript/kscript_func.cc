#include "kscript_func.h"
#include "kscript_context.h"
#include "kscript_value.h"
#include "kscript_object.h"
#include "kscript_parsenode.h"
#include "kscript_util.h"
#include "kscript_proxy.h"

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
    QString tmp( "Can not calculate length of a %1 value" );
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

  QString tmp( "Can not calulcate a numerical value from a %1 value" );
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

  QString tmp( "Can not calulcate a floating point value from a %1 value" );
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
    QString tmp( "The method %i is no signal" );
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
    cout << (*it)->toString( context ) << endl;

  // context.value()->clear();
  context.setValue( 0 );

  return true;
}

static bool ksfunc_application( KSContext& context )
{
    if ( !KSUtil::checkArgumentsCount( context, 2, "Application" ) )
	return false;

    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkType( context, args[0], KSValue::StringType ) )
	return false;
    if ( !KSUtil::checkType( context, args[1], KSValue::StringType ) )
	return false;

    context.setValue( new KSValue( new KSProxy( args[0]->stringValue().latin1(), args[1]->stringValue().latin1() ) ) );

    return TRUE;
}

KSModule::Ptr ksCreateModule_KScript( KSInterpreter* interp )
{
  KSModule::Ptr module = new KSModule( interp, "kscript" );

  module->addObject( "print", new KSValue( new KSBuiltinFunction( module, "print", ksfunc_print ) ) );
  module->addObject( "connect", new KSValue( new KSBuiltinFunction( module, "connect", ksfunc_connect ) ) );
  module->addObject( "length", new KSValue( new KSBuiltinFunction( module, "length", ksfunc_length ) ) );
  module->addObject( "arg", new KSValue( new KSBuiltinFunction( module, "arg", ksfunc_arg ) ) );
  module->addObject( "mid", new KSValue( new KSBuiltinFunction( module, "mid", ksfunc_mid ) ) );
  module->addObject( "toInt", new KSValue( new KSBuiltinFunction( module, "toInt", ksfunc_toInt ) ) );
  module->addObject( "toFloat", new KSValue( new KSBuiltinFunction( module, "toFloat", ksfunc_toFloat ) ) );
  module->addObject( "findApplication", new KSValue( new KSBuiltinFunction( module, "findApplication", ksfunc_application ) ) );

  return module;
}
