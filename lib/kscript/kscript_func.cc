#include "kscript_func.h"
#include "kscript_context.h"
#include "kscript_value.h"
#include "kscript_object.h"
#include "kscript_parsenode.h"
#include "kscript_util.h"

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

bool ksfunc_length( KSContext& context )
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

bool ksfunc_toInt( KSContext& context )
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

bool ksfunc_toFloat( KSContext& context )
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

bool ksfunc_connect( KSContext& context )
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

bool ksfunc_print( KSContext& context )
{
  // We know that the context always holds a list of parameters
  QValueList<KSValue::Ptr>::Iterator it = context.value()->listValue().begin();
  QValueList<KSValue::Ptr>::Iterator end = context.value()->listValue().end();

  if ( it == end )
    cout << endl;

  for( ; it != end; ++it )
    cout << (*it)->toString() << endl;

  // context.value()->clear();
  context.setValue( 0 );

  return true;
}

KSModule::Ptr ksCreateModule_KScript( KSInterpreter* interp )
{
  KSModule::Ptr module = new KSModule( interp, "kscript" );

  module->addObject( "print", new KSValue( new KSBuiltinFunction( module, "print", ksfunc_print ) ) );
  module->addObject( "connect", new KSValue( new KSBuiltinFunction( module, "connect", ksfunc_connect ) ) );
  module->addObject( "length", new KSValue( new KSBuiltinFunction( module, "length", ksfunc_length ) ) );
  module->addObject( "toInt", new KSValue( new KSBuiltinFunction( module, "toInt", ksfunc_toInt ) ) );
  module->addObject( "toFloat", new KSValue( new KSBuiltinFunction( module, "toFloat", ksfunc_toFloat ) ) );

  return module;
}
