/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team
                           www.koffice.org/kspread

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

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

#include <sys/utsname.h>

// Function: INFO
bool kspreadfunc_info( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "INFO", true ) )
    return false;

  if( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;

  QString type = args[0]->stringValue().lower();

  if ( type == "directory" )
  { 
    context.setValue( new KSValue( QDir::currentDirPath() ) );
    return true;
  }

  else if ( type == "release" )
  { 
    context.setValue( new KSValue( QString( VERSION ) ) );
    return true;
  }

  else if ( type == "numfile" )
  {
    context.setValue( new KSValue( (int)KSpreadDoc::documents().count() ) );
    return true;
  }

  else if ( type == "recalc" )
  {
    QString result;
    if ( ( (KSpreadInterpreter *) context.interpreter() )->document()->delayCalculation() )
      result = i18n( "Manual" );
    else
      result = i18n( "Automatic" );
    context.setValue( new KSValue( result ) );
    return true;
  } 

  else if (type == "memavail")
  {
  }

  else if (type == "memused")
  {
  }

  else if (type == "origin")
  {
  }

  else if (type == "system")
  {
    struct utsname name;
    if( uname( &name ) >= 0 )
    {
       context.setValue( new KSValue( QString( name.sysname ) ) );
       return true;
    }   
  }

  else if (type == "totmem")
  {
  }

  else if (type == "osversion")
  {
    struct utsname name;
    if( uname( &name ) >= 0 )
    {
       QString os = QString("%1 %2 (%3)").arg( name.sysname ).
         arg( name.release ).arg( name.machine );
       context.setValue( new KSValue( os ) );
       return true;
    }
  }

  return false;
}

// Function: ISBLANK
bool kspreadfunc_isblank( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ISBLANK", true ) )
    return false;

  bool result = false;

  if ( KSUtil::checkType( context, args[0], KSValue::Empty, false ) ) 
    result = true;

  // the following part is needed becase empty cell returns 0.0 instead
  // of KSValue::Empty. this is due to bug in many function implementation
  // until someone can fix this, leave it as it is (ariya, 16.05.2002)
  if ( KSUtil::checkType( context, args[0], KSValue::DoubleType, false ) ) 
    result = args[0]->doubleValue() == 0.0;

  if ( KSUtil::checkType( context, args[0], KSValue::StringType, false ) ) 
    result = args[0]->stringValue().isEmpty();

  context.setValue( new KSValue( result ) );
  return true;
}

// Function: ISLOGICAL
bool kspreadfunc_islogical( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ISLOGICAL", true ) )
    return false;

  bool logic = KSUtil::checkType( context, args[0], KSValue::BoolType, true );
  context.setValue( new KSValue(logic));
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

static bool kspreadfunc_countblank_helper( KSContext & context, 
                                           QValueList<KSValue::Ptr> & args, 
                                           int & result)
{
  QValueList<KSValue::Ptr>::Iterator it  = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {

      if ( !kspreadfunc_countblank_helper( context, (*it)->listValue(), result) )
        return false;
    }
    else
    if ( KSUtil::checkType( context, args[0], KSValue::Empty, true ) ) 
    {
      ++result;
    }
    else
    if ( KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) ) 
    {
      KScript::Double d = args[0]->doubleValue();
      if (d == 0.0)
        ++result;
    }
    else
    if ( KSUtil::checkType( context, args[0], KSValue::StringType, true ) ) 
    {
      QString s = args[0]->stringValue();
      if ( s.isEmpty() || s.stripWhiteSpace().isEmpty() )
        ++result;
    }
  }

  return true;
}

bool kspreadfunc_countblank( KSContext & context )
{
  int result = 0;

  bool b = kspreadfunc_countblank_helper( context, context.value()->listValue(), result);

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

// Function: TYPE
bool kspreadfunc_type( KSContext & context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "TYPE", true ) )
    return false;

  if ( KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
  {
    context.setValue( new KSValue( 1 ) );
    return true;
  }

  if ( KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) 
       || KSUtil::checkType( context, args[0], KSValue::IntType, true )
       || KSUtil::checkType( context, args[0], KSValue::DateType, true )
       || KSUtil::checkType( context, args[0], KSValue::TimeType, true ) )
  {
    context.setValue( new KSValue( 2 ) );
    return true;
  }

  if ( KSUtil::checkType( context, args[0], KSValue::BoolType, true ) )
  {
    context.setValue( new KSValue( 4 ) );
    return true;
  }

  if ( KSUtil::checkType( context, args[0], KSValue::ListType, true ) )
  {
    context.setValue( new KSValue( 64 ) );
    return true;
  }

  // TODO: for errors we need direct access to the cell
  //  if ( cell->hasError() )
  //    context.setValue( new KSValue( 16 ) );
  
  return true;
}

bool kspreadfunc_filename( KSContext & context )
{
  context.setValue( new KSValue( ( (KSpreadInterpreter *) context.interpreter() )->document()->url().prettyURL() ) );

  return true;
}

bool kspreadfunc_version( KSContext & context )
{
  context.setValue( new KSValue( QString( VERSION ) ) );

  return true;
}

// Function: N
bool kspreadfunc_n( KSContext & context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "N", true ) )
    return false;

  if ( KSUtil::checkType( context, args[0], KSValue::DoubleType, false ) )
  {
    context.setValue( new KSValue( args[0]->doubleValue() ) );
    return true;
  }

  if ( KSUtil::checkType( context, args[0], KSValue::IntType, false ) )
  {
    context.setValue( new KSValue( args[0]->intValue() ) );
    return true;
  }

  if ( KSUtil::checkType( context, args[0], KSValue::BoolType, false ) )
  {
    context.setValue( new KSValue( args[0]->boolValue() ) );
    return true;
  }

  if ( KSUtil::checkType( context, args[0], KSValue::DateType, false ) )
  {
    QDate date = args[0]->dateValue();
    QDate ref = QDate( 1900, 1, 1 );
    long serialno = -date.daysTo( ref ) + 2;
    context.setValue( new KSValue( serialno ) );
    return true;
  }

  context.setValue( new KSValue( (int)0 ) );
  return true;
 
}
