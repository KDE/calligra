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

// built-in trigonometric functions

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <kdebug.h>

#include <koscript_parser.h>
#include <koscript_util.h>
#include <koscript_func.h>
#include <koscript_synext.h>

#include "kspread_functions.h"
#include "kspread_util.h"


// prototypes (sort alphabetically)
bool kspreadfunc_acos( KSContext& context );
bool kspreadfunc_acosh( KSContext& context );
bool kspreadfunc_acot( KSContext& context );
bool kspreadfunc_asinh( KSContext& context );
bool kspreadfunc_asin( KSContext& context );
bool kspreadfunc_atan( KSContext& context );
bool kspreadfunc_atan2( KSContext& context );
bool kspreadfunc_atanh( KSContext& context );
bool kspreadfunc_cos( KSContext& context );
bool kspreadfunc_cosh( KSContext& context );
bool kspreadfunc_degrees( KSContext& context );
bool kspreadfunc_radians( KSContext& context );
bool kspreadfunc_sin( KSContext& context );
bool kspreadfunc_sinh( KSContext& context );
bool kspreadfunc_tan( KSContext& context );
bool kspreadfunc_tanh( KSContext& context );
bool kspreadfunc_pi( KSContext& context );

// registers all trigonometric functions
void KSpreadRegisterTrigFunctions()
{
  KSpreadFunctionRepository* repo = KSpreadFunctionRepository::self();

  repo->registerFunction( "ACOS",   kspreadfunc_acos );
  repo->registerFunction( "ACOSH",  kspreadfunc_acosh );
  repo->registerFunction( "ACOT",  kspreadfunc_acot );
  repo->registerFunction( "ASIN",   kspreadfunc_asin );
  repo->registerFunction( "ASINH",  kspreadfunc_asinh );
  repo->registerFunction( "ATAN",   kspreadfunc_atan );
  repo->registerFunction( "ATAN2",  kspreadfunc_atan2 );
  repo->registerFunction( "ATANH",  kspreadfunc_atanh );
  repo->registerFunction( "COS",    kspreadfunc_cos );
  repo->registerFunction( "COSH",   kspreadfunc_cosh );
  repo->registerFunction( "DEGREE", kspreadfunc_degrees );  // backward-compatible, remove in 1.4
  repo->registerFunction( "DEGREES",kspreadfunc_degrees );
  repo->registerFunction( "RADIAN", kspreadfunc_radians ); // backwared-compatible, remove in 1.4
  repo->registerFunction( "RADIANS",kspreadfunc_radians );
  repo->registerFunction( "SIN",    kspreadfunc_sin );
  repo->registerFunction( "SINH",   kspreadfunc_sinh );
  repo->registerFunction( "TAN",    kspreadfunc_tan );
  repo->registerFunction( "TANH",   kspreadfunc_tanh );
  repo->registerFunction( "PI",     kspreadfunc_pi );
}


// Function: sin
bool kspreadfunc_sin( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "sin", true ) )
    return false;
  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if( !KSUtil::checkType( context, args[0], KSValue::Empty, true ) )
	return false;
    }
  val=args[0]->doubleValue();

  context.setValue( new KSValue( sin( val ) ) );

  return true;
}

// Function: cos
bool kspreadfunc_cos( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "cos", true ) )
    return false;
  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( cos( val ) ) );

  return true;
}

// Function: tan
bool kspreadfunc_tan( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "tan", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();
  context.setValue( new KSValue( tan(val) ) );

  return true;
}

// Function: atan
bool kspreadfunc_atan( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "atan", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( atan( val ) ) );

  return true;
}

// Function: asin
bool kspreadfunc_asin( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "asin", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( asin( val ) ) );

  return true;
}

// Function: acos
bool kspreadfunc_acos( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "acos", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();


  context.setValue( new KSValue( acos( val ) ) );

  return true;
}

bool kspreadfunc_acot( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "acot", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();


  context.setValue( new KSValue( M_PI/2 - atan( val ) ) );

  return true;
}


// Function: asinh
bool kspreadfunc_asinh( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "asinh", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( asinh( val ) ) );

  return true;
}

// Function: acosh
bool kspreadfunc_acosh( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "acosh", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( acosh( val ) ) );

  return true;
}

// Function: atanh
bool kspreadfunc_atanh( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "atanh", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();


  context.setValue( new KSValue( atanh( val ) ) );

  return true;
}

// Function: tanh
bool kspreadfunc_tanh( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "tanh", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( tanh( val ) ) );

  return true;
}

// Function: sinh
bool kspreadfunc_sinh( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "sinh", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( sinh( val ) ) );

  return true;
}

// Function: cosh
bool kspreadfunc_cosh( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "cosh", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( cosh( val ) ) );

  return true;
}

// Function: DEGREES
bool kspreadfunc_degrees( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "degrees", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( (val*180)/M_PI  ));

  return true;
}

// Function: RADIANS
bool kspreadfunc_radians( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "radians", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();


  context.setValue( new KSValue( (val*M_PI )/180  ));

  return true;
}

// Function: PI
bool kspreadfunc_pi( KSContext& context )
{
    // QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 0, "PI", true ) )
      return false;

    context.setValue( new KSValue(M_PI));
    return true;
}

// Function: atan2
bool kspreadfunc_atan2( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "atan2", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  context.setValue( new KSValue( atan2( args[1]->doubleValue(),args[0]->doubleValue() ) ) );

  return true;
}
