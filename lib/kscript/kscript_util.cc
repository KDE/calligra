#include "kscript_util.h"
#include "kscript_context.h"
#include "kscript_struct.h"
#include "kscript_object.h"
#include "kscript_class.h"

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

bool KSUtil::checkType( KSContext& context, const KSValue::Ptr& v, KSValue::Type t, bool fatal )
{
  if ( v->type() != t )
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

bool KSUtil::checkArgs( KSContext& context, const QCString& signature, const QString& method, bool fatal )
{
    // Is the value really a list ?
    if ( !KSUtil::checkType( context, context.value(), KSValue::ListType, TRUE ) )
	return FALSE;
    return KSUtil::checkArgs( context, context.value()->listValue(), signature, method, fatal );
}

bool KSUtil::checkArgs( KSContext& context, const QValueList<KSValue::Ptr>& args,
			const QCString& signature, const QString& method, bool fatal )
{
    uint done = 0;
    uint count = args.count();
    uint len = signature.length();
    uint pos = 0;
    while ( pos < len )
    {
	// Have seen all parameters and we may skip optional parameters
	if ( done == count && signature[pos] == '|' )
	    return TRUE;
	if ( signature[pos] == '|' )
	    ++pos;
	if ( signature[pos] == 'i' )
        {
	    if ( !checkType( context, args[done], KSValue::IntType, fatal ) )
		return FALSE;
	    ++pos;
	}
	else if ( signature[pos] == 'f' )
        {
	    if ( !checkType( context, args[done], KSValue::DoubleType, fatal ) )
		return FALSE;
	    ++pos;
	}
	else if ( signature[pos] == 'b' )
        {
	    if ( !checkType( context, args[done], KSValue::BoolType, fatal ) )
		return FALSE;
	    ++pos;
	}
	else if ( signature[pos] == 's' )
        {
	    if ( !checkType( context, args[done], KSValue::StringType, fatal ) )
		return FALSE;
	    ++pos;
	}
	else if ( signature[pos] == 'c' )
        {
	    if ( !checkType( context, args[done], KSValue::CharType, fatal ) )
		return FALSE;
	    ++pos;
	}
	else if ( signature[pos] == '[' )
        {
	    if ( !checkType( context, args[done], KSValue::ListType, fatal ) )
		return FALSE;
	    ++pos;
	    if ( signature[pos] == ']' ) { }
	    // TODO: check vars in the list
	}
	else if ( signature[pos] == '{' )
        {
	    if ( !checkType( context, args[done], KSValue::MapType, fatal ) )
		return FALSE;
	    ++pos;
	    if ( signature[pos] == '}' ) { }
	    // TODO: check vars in the list
	}
	else if ( signature[pos] == 'S' )
        {
	    if ( !checkType( context, args[done], KSValue::StructType, fatal ) )
		return FALSE;
	    ++pos;
	    uint x = pos;
	    while( signature[pos] != ';' && signature[pos] != 0 )
		++pos;
	    ASSERT( signature[pos] == ';' );
	    if ( args[done]->structValue()->getClass()->fullName() != signature.mid( x, pos - x ).data() )
	    {
		if ( fatal )
		    castingError( context, args[done]->structValue()->getClass()->fullName(),
				  signature.mid( x, pos - x ).data() );
		return FALSE;
	    }
	    ++pos;
	}
	else if ( signature[pos] == 'O' )
        {
	    if ( !checkType( context, args[done], KSValue::ObjectType, fatal ) )
		return FALSE;
	    ++pos;
	    uint x = pos;
	    while( signature[pos] != ';' && signature[pos] != 0 )
		++pos;
	    ASSERT( signature[pos] == ';' );
	    if ( args[done]->objectValue()->getClass()->fullName() != signature.mid( x, pos - x ).data() )
	    {
		if ( fatal )
		    castingError( context, args[done]->objectValue()->getClass()->fullName(),
				  signature.mid( x, pos - x ).data() );
		return FALSE;
	    }
	    ++pos;
	}
	else
	    ASSERT( 0 );
	
	++done;
    }
 
    // Too many arguments ?
    if ( done < count )
    {
	if ( fatal )
	    tooFewArgumentsError( context, method );
	return FALSE;
    }
	    
    return TRUE;
}
