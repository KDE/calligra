// built-in logical functions
#include "kspread_util.h"
#include "kspread_doc.h"
#include "kspread_table.h"

#include <koscript_parser.h>
#include <koscript_util.h>
#include <koscript_func.h>
#include <koscript_synext.h>

#include <stdlib.h>
#include <math.h>
#include <float.h>

// Function: NOT
bool kspreadfunc_not( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "NOT", true ) || !KSUtil::checkArgumentsCount( context, 1, "not", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::BoolType, true ) )
    return false;

  bool toto = !args[0]->boolValue();
  context.setValue( new KSValue(toto));
  return true;
}

bool kspreadfunc_or_helper( KSContext& context, QValueList<KSValue::Ptr>& args, bool& first )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_or_helper( context, (*it)->listValue(), first ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::BoolType, true ) )
      first = (first || (*it)->boolValue());
    else
      return false;
  }

  return true;
}

// Function: OR
bool kspreadfunc_or( KSContext& context )
{
  bool first = false;
  bool b = kspreadfunc_or_helper( context, context.value()->listValue(), first );

  if ( b )
    context.setValue( new KSValue( first ) );

  return b;
}

// Function: NOR
bool kspreadfunc_nor( KSContext& context )
{
  bool first = false;
  bool b = kspreadfunc_or_helper( context, context.value()->listValue(), first );

  if ( b )
    context.setValue( new KSValue( !first ) );

  return b;
}


bool kspreadfunc_and_helper( KSContext& context, QValueList<KSValue::Ptr>& args, bool& first )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_and_helper( context, (*it)->listValue(), first ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::BoolType, true ) )
      first = first && (*it)->boolValue();
    else
      return false;
  }

  return true;
}

// Function: AND
bool kspreadfunc_and( KSContext& context )
{
  bool first = true;
  bool b = kspreadfunc_and_helper( context, context.value()->listValue(), first );

  if ( b )
    context.setValue( new KSValue( first ) );

  return b;
}

// Function: NAND
bool kspreadfunc_nand( KSContext& context )
{
  bool first = true;
  bool b = kspreadfunc_and_helper( context, context.value()->listValue(), first );

  if ( b )
    context.setValue( new KSValue( !first ) );

  return b;
}

static bool kspreadfunc_xor_helper( KSContext& context, QValueList<KSValue::Ptr>& args, bool& first )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_and_helper( context, (*it)->listValue(), first ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::BoolType, true ) )
      first = first ^ (*it)->boolValue();
    else
      return false;
  }

  return true;
}

// Function: XOR
bool kspreadfunc_xor( KSContext& context )
{
  bool first = true;
  bool b = kspreadfunc_xor_helper( context, context.value()->listValue(), first );

  if ( b )
    context.setValue( new KSValue( first ) );

  return b;
}

// Function: IF
bool kspreadfunc_if( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 3, "if", true ) || !KSUtil::checkArgumentsCount( context, 3, "IF", true ))
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::BoolType, true ) )
      return false;

    if (  args[0]->boolValue() == true )
      context.setValue( new KSValue( *(args[1]) ) );
    else
      context.setValue( new KSValue( *(args[2]) ) );

    return true;
}

// Function: ISLOGIC
bool kspreadfunc_islogic( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ISLOGIC", true ) )
    return false;

  bool logic = KSUtil::checkType( context, args[0], KSValue::BoolType, true );
  context.setValue( new KSValue(logic));
  return true;
}

// Function: ISEMPTY
bool kspreadfunc_isempty( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ISEMPTY", true ) )
    return false;

  if ( KSUtil::checkType( context, args[0], KSValue::Empty, true ) ) {
    context.setValue( new KSValue(true) );
    return true;
  }

  if ( KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) ) {
    KScript::Double d = args[0]->doubleValue();
    bool logic = (d==0);
    context.setValue( new KSValue(logic) );
    return true;
  }

  if ( KSUtil::checkType( context, args[0], KSValue::StringType, true ) ) {
    QString s = args[0]->stringValue();
    bool logic = s.isEmpty() || s.stripWhiteSpace().isEmpty();
    context.setValue( new KSValue(logic) );
    return true;
  }
  
  context.setValue( new KSValue(false) );
  return true;
}
	

// Function: ISTEXT
bool kspreadfunc_istext( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ISTEXT", true ) )
    return false;

  bool logic = KSUtil::checkType( context, args[0], KSValue::StringType, true );
  context.setValue( new KSValue(logic));
  return true;
}

// Function: ISNOTTEXT
bool kspreadfunc_isnottext( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ISNOTTEXT", true ) )
    return false;

  bool logic = !KSUtil::checkType( context, args[0], KSValue::StringType, true );
  context.setValue( new KSValue(logic));
  return true;
}

// Function: ISNUM
bool kspreadfunc_isnum( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ISNUM", true ) )
    return false;

  bool logic = KSUtil::checkType( context, args[0], KSValue::DoubleType, true );
  context.setValue( new KSValue(logic));
  return true;
}

// Function: ISTIME
bool kspreadfunc_istime( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ISTIME", true ) )
    return false;
  bool logic = false;

  if (!KSUtil::checkType( context, args[0], KSValue::TimeType, true ))
  {
      if ( KSUtil::checkType( context, args[0], KSValue::StringType, true ))
          KGlobal::locale()->readTime(args[0]->stringValue(), &logic);
      else
          return false;
  }
  else
      logic = true;
  context.setValue( new KSValue(logic));
  return true;
}

// Function: ISDATE
bool kspreadfunc_isdate( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ISDATE", true ) )
    return false;

  bool logic = false;
  if (!KSUtil::checkType( context, args[0], KSValue::DateType, true ))
  {
      if ( KSUtil::checkType( context, args[0], KSValue::StringType, true ))
          KGlobal::locale()->readDate(args[0]->stringValue(), &logic);
      else
          return false;
  }
  else
      logic = true;

  context.setValue( new KSValue(logic));
  return true;
}

// Function: ISODD
bool kspreadfunc_isodd( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1, "ISODD",true ) )
    return false;
  bool result=true;
  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
        result= false;
  if(result)
        {
        //it's a integer => test if it's an odd integer
        if(args[0]->intValue() & 1)
                result=true;
        else
                result=false;
        }

  context.setValue( new KSValue(result));

  return true;
}

// Function: ISEVEN
bool kspreadfunc_iseven( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1, "ISEVEN",true ) )
    return false;
  bool result=true;
  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
        result= false;
  if(result)
        {
        //it's a integer => test if it's an even integer
        if(args[0]->intValue() & 1)
                result=false;
        else
                result=true;
        }

  context.setValue( new KSValue(result));

  return true;
}

