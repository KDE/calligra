#include "kscript_util.h"
#include "kscript_context.h"

bool KSUtil::checkArgumentsCount( KSContext& context, uint count, const QString& name, bool fatal )
{
  ASSERT( context.value() && context.value()->type() == KSValue::ListType );

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( args.count() == count )
    return true;

  if ( !fatal )
    return false;

  if ( args.count() < count )
    tooFewArgumentsError( context, name );
  else
    tooManyArgumentsError( context, name );

  return false;
}

bool KSUtil::checkType( KSContext& context, KSValue* v, KSValue::Type t, bool fatal )
{
  if ( !v->cast( t ) )
  {
    if ( !fatal )
      return false;

    castingError( context, v, t );
    return false;
  }

  return true;
}

void KSUtil::castingError( KSContext& context, const QString& from, const QString& to )
{
  QString tmp( "From %1 to %2" );
  context.setException( new KSException( "CastingError", tmp.arg( from ).arg( to ), -1 ) );
}

void KSUtil::castingError( KSContext& context, KSValue* v, KSValue::Type t )
{
  QString tmp( "From %1 to %2" );
  context.setException( new KSException( "CastingError", tmp.arg( v->typeName() ).arg( KSValue::typeToName( t ) ), -1 ) );
}

void KSUtil::argumentsMismatchError( KSContext& context, const QString& methodname )
{
  QString tmp( "Arguments did not match the methods %1 parameter list" );
  context.setException( new KSException( "ParameterMismatch", tmp.arg( methodname ) ) );
}

void KSUtil::tooFewArgumentsError( KSContext& context, const QString& methodname )
{
  QString tmp( "Too few arguments for method %1" );
  context.setException( new KSException( "TooFewArguments", tmp.arg( methodname ), -1 ) );
}

void KSUtil::tooManyArgumentsError( KSContext& context, const QString& methodname )
{
  QString tmp( "Too many arguments  for method %1" );
  context.setException( new KSException( "TooManyArguments", tmp.arg( methodname ), -1 ) );
}
