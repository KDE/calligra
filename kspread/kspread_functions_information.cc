// built-in information functions
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

#include <qdir.h>
#include <config.h>

// Function: INFO
bool kspreadfunc_info( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "INFO", true ) )
    return false;

  if( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;

  QString type = args[0]->stringValue().lower();

  QString result;

  if( type == "directory" )
    result = QDir::currentDirPath();

  if( type == "release" )
    result = VERSION;

  context.setValue( new KSValue( result) );
  return true;
}

// Function: ISBLANK
bool kspreadfunc_isblank( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ISBLANK", true ) )
    return false;

  bool result = false;

  if ( KSUtil::checkType( context, args[0], KSValue::Empty, true ) ) 
    result = true;

  context.setValue( new KSValue( result ) );
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
// FIXME will be obsoleted by ISBLANK
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

