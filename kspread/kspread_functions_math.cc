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

// built-in math functions

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <kdebug.h>

#include <koscript_parser.h>
#include <koscript_util.h>
#include <koscript_func.h>
#include <koscript_synext.h>

#include <kspread_cell.h>
#include <kspread_doc.h>
#include <kspread_functions.h>
#include <kspread_map.h>
#include <kspread_table.h>
#include <kspread_util.h>

namespace math_local
{
  bool gWorking = false;
}

using namespace math_local;

// prototypes
bool kspreadfunc_abs( KSContext& context );
bool kspreadfunc_ceil( KSContext& context );
bool kspreadfunc_count( KSContext& context );
bool kspreadfunc_countif( KSContext& context );
bool kspreadfunc_cur( KSContext& context );
bool kspreadfunc_div( KSContext& context );
bool kspreadfunc_eps( KSContext& context );
bool kspreadfunc_even( KSContext& context );
bool kspreadfunc_exp( KSContext& context );
bool kspreadfunc_fact( KSContext& context );
bool kspreadfunc_factdouble( KSContext& context );
bool kspreadfunc_fib( KSContext& context );
bool kspreadfunc_floor( KSContext& context );
bool kspreadfunc_gcd( KSContext & context );
bool kspreadfunc_int( KSContext& context );
bool kspreadfunc_inv( KSContext& context );
bool kspreadfunc_lcm( KSContext & context );
bool kspreadfunc_ln( KSContext& context );
bool kspreadfunc_log( KSContext& context );
bool kspreadfunc_logn( KSContext& context );
bool kspreadfunc_max( KSContext& context );
bool kspreadfunc_min( KSContext& context );
bool kspreadfunc_mod( KSContext& context );
bool kspreadfunc_mult( KSContext& context );
bool kspreadfunc_multinomial( KSContext& context );
bool kspreadfunc_odd( KSContext& context );
bool kspreadfunc_pow( KSContext& context );
bool kspreadfunc_quotient( KSContext& context );
bool kspreadfunc_product( KSContext& context );
bool kspreadfunc_rand( KSContext& context );
bool kspreadfunc_randbetween( KSContext& context );
bool kspreadfunc_rootn( KSContext& context );
bool kspreadfunc_round( KSContext& context );
bool kspreadfunc_rounddown( KSContext& context );
bool kspreadfunc_roundup( KSContext& context );
bool kspreadfunc_sign( KSContext& context );
bool kspreadfunc_sqrt( KSContext& context );
bool kspreadfunc_sqrtpi( KSContext& context );
bool kspreadfunc_sum( KSContext& context );
bool kspreadfunc_sumsq( KSContext& context );
bool kspreadfunc_trunc( KSContext& context );


bool kspreadfunc_multipleOP( KSContext& context );
bool kspreadfunc_subtotal( KSContext& context );

// registers all math functions
void KSpreadRegisterMathFunctions()
{
  gWorking = false;
  KSpreadFunctionRepository* repo = KSpreadFunctionRepository::self();

  repo->registerFunction( "MULTIPLEOPERATIONS", kspreadfunc_multipleOP );
  repo->registerFunction( "SUBTOTAL",    kspreadfunc_subtotal );
  repo->registerFunction( "ABS",         kspreadfunc_abs );
  repo->registerFunction( "CEIL",        kspreadfunc_ceil );
  repo->registerFunction( "COUNT",       kspreadfunc_count );
  repo->registerFunction( "COUNTIF",     kspreadfunc_countif );
  repo->registerFunction( "CUR",         kspreadfunc_cur );
  repo->registerFunction( "DIV",         kspreadfunc_div );
  repo->registerFunction( "EPS",         kspreadfunc_eps );
  repo->registerFunction( "EVEN",        kspreadfunc_even );
  repo->registerFunction( "EXP",         kspreadfunc_exp );
  repo->registerFunction( "FACT",        kspreadfunc_fact );
  repo->registerFunction( "FACTDOUBLE",  kspreadfunc_factdouble ); 
  repo->registerFunction( "FIB",         kspreadfunc_fib ); // KSpread-specific, like Quattro-Pro's FIB
  repo->registerFunction( "FLOOR",       kspreadfunc_floor );
  repo->registerFunction( "GCD",         kspreadfunc_gcd );
  repo->registerFunction( "INT",         kspreadfunc_int );
  repo->registerFunction( "INV",         kspreadfunc_inv );
  repo->registerFunction( "LCD",         kspreadfunc_gcd ); // obsolete, use GCD instead, remove in 1.4
  repo->registerFunction( "LCM",         kspreadfunc_lcm );
  repo->registerFunction( "LN",          kspreadfunc_ln );
  repo->registerFunction( "LOG",         kspreadfunc_log );
  repo->registerFunction( "LOGN",        kspreadfunc_logn );
  repo->registerFunction( "MAX",         kspreadfunc_max );
  repo->registerFunction( "MIN",         kspreadfunc_min );
  repo->registerFunction( "MOD",         kspreadfunc_mod );
  repo->registerFunction( "MULTIPLY",    kspreadfunc_mult );
  repo->registerFunction( "MULTINOMIAL", kspreadfunc_multinomial );
  repo->registerFunction( "ODD",         kspreadfunc_odd );
  repo->registerFunction( "POW",         kspreadfunc_pow );
  repo->registerFunction( "POWER",       kspreadfunc_pow );
  repo->registerFunction( "QUOTIENT",    kspreadfunc_quotient );
  repo->registerFunction( "PRODUCT",     kspreadfunc_product );
  repo->registerFunction( "RAND",        kspreadfunc_rand );
  repo->registerFunction( "RANDBETWEEN", kspreadfunc_randbetween );
  repo->registerFunction( "ROOTN",       kspreadfunc_rootn );
  repo->registerFunction( "ROUND",       kspreadfunc_round );
  repo->registerFunction( "ROUNDDOWN",   kspreadfunc_rounddown );
  repo->registerFunction( "ROUNDUP",     kspreadfunc_roundup );
  repo->registerFunction( "SIGN",        kspreadfunc_sign );
  repo->registerFunction( "SQRT",        kspreadfunc_sqrt );
  repo->registerFunction( "SQRTPI",      kspreadfunc_sqrtpi );
  repo->registerFunction( "SUM",         kspreadfunc_sum );
  repo->registerFunction( "SUMSQ",       kspreadfunc_sumsq );
  repo->registerFunction( "TRUNC",       kspreadfunc_trunc );
}

/*********************************************************************
 *
 * Helper function to avoid problems with rounding floating point
 * values. Idea for this kind of solution taken from Openoffice.
 *
 *********************************************************************/

static bool approx_equal (double a, double b)
{
  if ( a == b )
    return TRUE;
  double x = a - b;
  return (x < 0.0 ? -x : x)  <  ((a < 0.0 ? -a : a) * DBL_EPSILON);
}

// Function: SQRT
bool kspreadfunc_sqrt( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "sqrt", true ) )
    return false;
  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( sqrt( val ) ) );

  return true;
}

// Function: SQRTPI
bool kspreadfunc_sqrtpi( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "SQRTPI", true ) )
    return false;
  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  if( val < 0 ) return false;

  context.setValue( new KSValue( sqrt( val * M_PI ) ) );
  return true;
}





// Function: ROOTN
bool kspreadfunc_rootn( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "SQRTn", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;

  context.setValue( new KSValue( exp( log(args[0]->doubleValue())/args[1]->intValue() ) ) );

  return true;
}

// Function: CUR
bool kspreadfunc_cur( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "CUR", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  context.setValue( new KSValue( exp( log(args[0]->doubleValue())/3)) );

  return true;
}

// Function: ABS
bool kspreadfunc_abs( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ABS", true ) || !KSUtil::checkArgumentsCount( context, 1, "ABS", true ))
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( fabs(val ) ) );

  return true;
}

// Function: exp
bool kspreadfunc_exp( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "exp",true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( exp( val ) ) );

  return true;
}

// Function: ceil
bool kspreadfunc_ceil( KSContext& context )
{
  kdDebug() << "Ceil" << endl;
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ceil", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if (approx_equal(floor(args[0]->doubleValue()),args[0]->doubleValue()))
    context.setValue( new KSValue(args[0]->doubleValue()));
  else
    context.setValue( new KSValue( ceil( args[0]->doubleValue() ) ) );

  return true;
}

// Function: floor
bool kspreadfunc_floor( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "floor", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( floor( val ) ) );

  return true;
}

// Function: ln
bool kspreadfunc_ln( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "ln", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( log( val ) ) );

  return true;
}

// Function: LOGn
bool kspreadfunc_logn( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "LOGn", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;

  context.setValue( new KSValue( log( args[0]->doubleValue() ) /log( args[1]->doubleValue() )) );

  return true;
}

// Function: log
bool kspreadfunc_log( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "log", true ) )
    return false;

  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ))
	return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( log10( val ) ) );

  return true;
}

static bool kspreadfunc_sum_helper( KSContext& context, QValueList<KSValue::Ptr>& args, double& result )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_sum_helper( context, (*it)->listValue(), result ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      {
      result += (*it)->doubleValue();
      }
    /*else if ( !KSUtil::checkType( context, *it, KSValue::Empty, true ) )
      return false;*/
  }

  return true;
}

// Function: sum
bool kspreadfunc_sum( KSContext& context )
{
  double result = 0.0;
  bool b = kspreadfunc_sum_helper( context, context.value()->listValue(), result );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

static bool kspreadfunc_product_helper( KSContext & context,
                                        QValueList<KSValue::Ptr> & args,
                                        double & result )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_product_helper( context, (*it)->listValue(), result ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
    {
      result *= (*it)->doubleValue();
    }
  }

  return true;
}

// Function: product
bool kspreadfunc_product( KSContext& context )
{
  double result = 1.0;
  bool b = kspreadfunc_product_helper( context,
                                       context.value()->listValue(),
                                       result );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

static int kspreadfunc_div_helper( KSContext & context,
                                   QValueList<KSValue::Ptr> & args,
                                   double & result )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  result = (*it)->doubleValue();
  ++it;

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_product_helper( context, (*it)->listValue(), result ) )
        return 0;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
    {
      double val =(*it)->doubleValue();

      if (val == 0)
        return -1;

      result /= val;
    }
  }

  return 1;
}

// Function: DIV
bool kspreadfunc_div( KSContext& context )
{
  double result = 0.0;
  int b = kspreadfunc_div_helper( context,
                                  context.value()->listValue(),
                                  result );

  if ( b == 1 )
    context.setValue( new KSValue( result ) );
  else if ( b == -1 )
    context.setValue( new KSValue( i18n("#DIV/0") ) );
  else
    return false;

  return true;
}

static bool kspreadfunc_sumsq_helper( KSContext& context, QValueList<KSValue::Ptr>& args, double& result )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_sum_helper( context, (*it)->listValue(), result ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      {
      result += ((*it)->doubleValue()*(*it)->doubleValue());
      }
    else if ( !KSUtil::checkType( context, *it, KSValue::Empty, true ) )
      return false;
  }

  return true;
}

// Function: SUMSQ
bool kspreadfunc_sumsq( KSContext& context )
{
  double result = 0.0;
  bool b = kspreadfunc_sumsq_helper( context, context.value()->listValue(), result );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

static bool kspreadfunc_max_helper( KSContext& context, QValueList<KSValue::Ptr>& args, double& result,int& inter)
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {

      if ( !kspreadfunc_max_helper( context, (*it)->listValue(), result,inter ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      {
      if(inter == 0)
      {
        result=(*it)->doubleValue();
        inter=1;
      }
      if(result <  (*it)->doubleValue())
        result =(*it)->doubleValue();
    }
  }

  return true;
}

// Function: MAX
bool kspreadfunc_max( KSContext& context )
{
  double result = 0.0;

  //init first element
  int inter=0;

  bool b = kspreadfunc_max_helper( context, context.value()->listValue(), result ,inter );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

static int kspreadfunc_gcd_gcd(int value1, int value2)
{
  // start with the lower value.
  int n = (value1 <= value2 ? value1 : value2);

  // check if this is already the result
  if ((value1 % n == 0) && (value2 % n == 0))
  {
    return n;
  }
  // to save time: start with n = n / 2
  n = (int) (n / 2);

  while ((value1 % n != 0) || (value2 % n != 0))
  {
    --n;
  }

  return n;
}

static bool kspreadfunc_gcd_helper( KSContext & context,
                                    QValueList<KSValue::Ptr>& args,
                                    int & result)
{
  QValueList<KSValue::Ptr>::Iterator it  = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  // at first get the smallest value to start with
  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_gcd_helper( context, (*it)->listValue(), result ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::IntType, true ) )
    {
      int val = (*it)->intValue();

      if (val == 0)
      {
        result = 0;
        return true;
      }

      if ((result == 0) || (val < result))
        result = val;
    }
  }

  it = args.begin();

  // calculate the LCD:
  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_gcd_helper( context, (*it)->listValue(), result ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::IntType, true ) )
    {
      int n = kspreadfunc_gcd_gcd(result, (*it)->intValue());

      if (n != result)
      {
        result = n;
        it = args.begin();

        continue;
      }
    }
  }

  if (result < 0)
    result *= -1;

  context.setValue(new KSValue(result));

  return true;
}

// Function: GCD
bool kspreadfunc_gcd( KSContext & context )
{
  int result = 0;

  bool b = kspreadfunc_gcd_helper(context, context.value()->listValue(),
				  result);

  if (b)
    context.setValue(new KSValue(result));

  return b;
}

static bool kspreadfunc_lcm_helper( KSContext & context,
				    QValueList<KSValue::Ptr> & args,
				    double & result,
				    double & max,
				    double & inter,
				    int & signs)
{
  // calculating the LCM:
  // algorithm: choose the biggest value in the list
  // take this as interval for increasing the numbers you check with
  // go through the list, check every value if your proposal is a multiple
  // if not, increase your proposal value and start again.

  // when you have found your LCM: check if it has the right sign:
  // if the number of (-) signs in the list is , the result is positive
  // otherwise negative.

  QValueList<KSValue::Ptr>::Iterator it  = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  // first loop: check for the biggest value
  // and count number of (-) signs:
  for (; it != end; ++it)
  {
    if (KSUtil::checkType(context, *it, KSValue::ListType, false))
    {
      if (!kspreadfunc_lcm_helper(context, (*it)->listValue(), result, max, inter, signs))
        return false;
    }
    else if (KSUtil::checkType(context, *it, KSValue::DoubleType, true))
    {
      double d = (double) (*it)->doubleValue();

      if (d < 0)
	++signs;

      if (result < d)
	result = d;
    }
  }

  inter = result;
  it    = args.begin();

  while (true)
  {
    if (KSUtil::checkType(context, *it, KSValue::ListType, false))
    {
      if (!kspreadfunc_lcm_helper( context, (*it)->listValue(), result, max, inter, signs))
	return false;
    }
    else if (KSUtil::checkType(context, *it, KSValue::DoubleType, true))
    {
      double d = inter / (*it)->doubleValue();

      // if it is not a multiple, increase you proposal value and start again
      if (!approx_equal(d, floor(d)))
      {
	inter += result;

	it = args.begin();
	continue;
      }
    } // end else if

    // ... otherwise check the next value if any
    ++it;

    if (it == end)
      break;
  }

  result = inter;

  // check if we have the correct sign (-/+)
  if (signs > 0)
  {
    if ((result < 0) && (signs % 2 == 0))
      result *= -1;
    else if ((result > 0) && (signs % 2 != 0))
      result *= -1;
  }

  return true;
}

// Function: lcm
bool kspreadfunc_lcm( KSContext & context )
{
  double result = 0.0;
  double max    = 1.0;
  double inter  = 0.0;
  int    signs  = 0;

  bool b = kspreadfunc_lcm_helper(context, context.value()->listValue(),
				  result, max, inter, signs);

  if (b)
    context.setValue(new KSValue(result));

  return b;

}

static bool kspreadfunc_min_helper( KSContext& context, QValueList<KSValue::Ptr>& args, double& result,int& inter)
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {

      if ( !kspreadfunc_min_helper( context, (*it)->listValue(), result,inter ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      {
      if(inter == 0)
      {
        result=(*it)->doubleValue();
        inter=1;
      }
      if(result >  (*it)->doubleValue())
        result =(*it)->doubleValue();
    }
  }

  return true;
}

// Function: MIN
bool kspreadfunc_min( KSContext& context )
{
  double result = 0.0;

  //init first element
  int inter=0;

  bool b = kspreadfunc_min_helper( context, context.value()->listValue(), result ,inter );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

static bool kspreadfunc_mult_helper( KSContext& context, QValueList<KSValue::Ptr>& args, double& result )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_mult_helper( context, (*it)->listValue(), result ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      result *= (*it)->doubleValue();
  }

  return true;
}

// Function: MULT
bool kspreadfunc_mult( KSContext& context )
{
  double result = 1.0;
  bool b = kspreadfunc_mult_helper( context, context.value()->listValue(), result );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

// Function: INT
bool kspreadfunc_int( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "INT", true ) )
    return false;
  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
    if(!KSUtil::checkType( context, args[0], KSValue::Empty, true ) )
      return false;
    }
  else
    val=args[0]->doubleValue();

  context.setValue( new KSValue( floor( val ) ) );
  return true;
}

// Function: QUOTIENT
bool kspreadfunc_quotient( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "QUOTIENT", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;

  double num = args[0]->doubleValue();
  double den = args[1]->doubleValue();

  if( den == 0 ) return false;

  context.setValue( new KSValue( (int)(num/den) ) );
  return true;
}


// Function: eps
bool kspreadfunc_eps( KSContext& context )
{
// #### This should adjust according to the actual number system used (float, double, long double, ...)
    if( !KSUtil::checkArgumentsCount( context, 0, "eps", true ) )
      return false;

    context.setValue( new KSValue(DBL_EPSILON));
    return true;
}

// Function: rand
bool kspreadfunc_rand( KSContext& context )
{
    // QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 0, "rand", true ) )
      return false;

    context.setValue( new KSValue((double) rand()/(RAND_MAX + 1.0)));
    return true;
}

// Function: RANDBETWEEN
bool kspreadfunc_randbetween( KSContext& context )
{
    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 2, "RANDBETWEEN", true ) )
      return false;
    if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
        return false;
    if( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
        return false;
    if(args[0]->intValue()>args[1]->intValue())
        {
        context.setValue( new KSValue(i18n("Err")));
        return true;
        }

    context.setValue( new KSValue((double)(((double)args[1]->intValue()-(double)args[0]->intValue())*rand()/RAND_MAX+((double)args[0]->intValue()))));

    return true;
}

// Function: POW
bool kspreadfunc_pow( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "pow",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;

  context.setValue( new KSValue( pow( args[0]->doubleValue(),args[1]->doubleValue() ) ) );

  return true;
}

// Function: MOD
bool kspreadfunc_mod( KSContext& context )
{
  double result=0;
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "MOD",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if( (int)args[1]->doubleValue()!=0)
  {
        result=(int)args[0]->doubleValue() % (int)args[1]->doubleValue();
        context.setValue( new KSValue(  result  ) );
  }
  else
  {
        context.setValue( new KSValue( i18n("#DIV/0") ) );

  }
  return true;
}

// Function: fact
bool kspreadfunc_fact( KSContext& context )
{
  double result;
  QString tmp;
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context,1, "fact",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  result=util_fact((double)args[0]->intValue(),0);
  //In util_fact function val must be positive
  tmp=i18n("Err");
  if(result==-1)
        context.setValue( new KSValue(tmp));
  else
        context.setValue( new KSValue(result ));

  return true;
}

// Function: FACTDOUBLE
bool kspreadfunc_factdouble( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "FACTDOUBLE",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  int number = args[0]->intValue();
  if( number < 0 )
    return false;

  double result = 1;
  for( int n = number; n > 0; n -= 2 )
    result *= n;

  context.setValue( new KSValue( result ) );

  return true;
}

// Function: MULTINOMIAL
bool kspreadfunc_multinomial( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  double num = 0, den = 1;

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::IntType, false ) )
    {
      int val = (*it)->intValue();
      if( val < 0 ) return false;
      num += val;
      den *= util_fact( val, 0 );
    }
  }

  num = util_fact( num, 0 );
  double result = num / den;

  context.setValue( new KSValue( result ) );
  return true;
}

// Function: sign
bool kspreadfunc_sign( KSContext& context )
{
  int value=0;
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "sign", true ) )
    return false;
  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if ( !KSUtil::checkType( context, args[0], KSValue::Empty, true ) )
	return false;
    }
  val=args[0]->doubleValue();

  if(val>0)
    value=1;
  else if(val<0)
    value=-1;
  else if(val==0)
    value=0;

  context.setValue( new KSValue( value ) );

  return true;
}

// Function: INV
bool kspreadfunc_inv( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "INV",true ) )
    return false;
  double val=0.0;
  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    {
      if ( !KSUtil::checkType( context, args[0], KSValue::Empty, true ) )
	return false;
    }
  val=args[0]->doubleValue();


  context.setValue( new KSValue( val*(-1) ) );

  return true;
}

// Function: ROUNDDOWN
bool kspreadfunc_rounddown( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  double result=0;
  int digits=0;
  if ( !KSUtil::checkArgumentsCount( context, 2, "ROUNDDOWN", true ) )
        {
        //just 1 argument => number of decimal =0 by default
        if ( !KSUtil::checkArgumentsCount( context, 1, "ROUNDDOWN", true ) )
                return false;
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
            return false;
        digits=0;
        }
  else
        {
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
                return false;
        if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
                return false;
        digits=args[1]->intValue();
        }
  result=args[0]->doubleValue()*pow(10.0, digits);
  context.setValue( new KSValue( floor( result )/pow(10.0, digits) ) );

  return true;
}

// Function: ROUNDUP
bool kspreadfunc_roundup( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  double result=0;
  int digits=0;
  if ( !KSUtil::checkArgumentsCount( context, 2, "ROUNDUP", true ) )
        {
        //just 1 argument => number of decimal =0 by default
        if ( !KSUtil::checkArgumentsCount( context, 1, "ROUNDUP", true ) )
                return false;
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
            return false;
        digits=0;
        }
  else
        {
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
                return false;
        if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
                return false;
        digits=args[1]->intValue();
        }
  // This is not correct solution for problem with floating point numbers and probably
  // will fail in platforms where float and double lenghts are same.
  if (approx_equal(floor(args[0]->doubleValue()*::pow(10,digits)), args[0]->doubleValue()*::pow(10,digits)))
      result = args[0]->doubleValue();
  else
      result=floor(args[0]->doubleValue()*::pow(10,digits)+1)/::pow(10,digits);
  context.setValue( new KSValue( result) );

  return true;
}

// Function: ROUND
bool kspreadfunc_round( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  double result=0;
  int digits=0;
  if ( !KSUtil::checkArgumentsCount( context, 2, "ROUND", true ) )
        {
        //just 1 argument => number of decimal =0 by default
        if ( !KSUtil::checkArgumentsCount( context, 1, "ROUND", true ) )
                return false;
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
            return false;
        digits=0;
        }
  else
        {
        if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
                return false;
        if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
                return false;
        digits=args[1]->intValue();
        }
  result=floor(args[0]->doubleValue()*pow(10.0,digits)+0.5)/pow(10.0,digits);
  context.setValue( new KSValue( result) );

  return true;
}

// Function: EVEN
bool kspreadfunc_even( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1, "EVEN",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  double result;
  double val=args[0]->doubleValue();
  int sign=1;
  if(val<0)
        {
        sign=-1;
        val=-val;
        }
  if (approx_equal(val, floor(val)))
    val = floor(val);
 double valsup=ceil( val );
 if(fmod(valsup,2.0)==0)
        {
        if(val>valsup)
                result=(int)(sign*(valsup+2));
        else
                result=(int)(sign*valsup);
        }
 else
        {
        result=(int)(sign*(valsup+1));
        }
  context.setValue( new KSValue(result));

  return true;
}

// Function: ODD
bool kspreadfunc_odd( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context,1, "ODD",true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  double result;
  double valsup;
  int sign=1;
  double val= args[0]->doubleValue();
  if (val < 0)
        {
        sign = -1;
        val = -val;
        }
  if (approx_equal(val, floor(val)))
    val = floor(val);
  valsup = ceil(val);
  if (fmod(valsup, 2.0) == 1)
        {
        if (val > valsup)
                result=(int) (sign * (valsup + 2));
        else
                result=(int) (sign * valsup);
        }
  else
        result=(int) (sign * (valsup + 1));

 context.setValue( new KSValue(result));

  return true;
}

static bool kspreadfunc_count_helper( KSContext& context, QValueList<KSValue::Ptr>& args, double& result)
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
      {

	if ( !kspreadfunc_count_helper( context, (*it)->listValue(), result ) )
	  return false;
      }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      {
	result++;
      }
  }

  return true;
}

bool kspreadfunc_trunc( KSContext & context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  int precision = 0;

  if( KSUtil::checkArgumentsCount( context, 2, "TRUNC", false ) )
  {
    if( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
      return false;

    precision = args[1]->intValue();
  }
  else
  {
    if ( !KSUtil::checkArgumentsCount( context, 1, "TRUNC", true ) )
      return false;
  }

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  double result = args[0]->doubleValue();

  int factor = (int) pow( 10, precision );

  result = floor( result * factor ) / factor;

  context.setValue( new KSValue( result ) );

  return true;
}

// Function: COUNT
bool kspreadfunc_count( KSContext& context )
{
  double result = 0.0;

  bool b = kspreadfunc_count_helper( context, context.value()->listValue(), result );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

static int kspreadfunc_countif_helper( KSContext& context, KSValue* value, 
  const QString& criteria )
{
  if( KSUtil::checkType( context, value, KSValue::DoubleType, false ) )
  {
    bool ok = false;
    double num = KGlobal::locale()->readNumber( criteria, &ok );
    if( !ok ) return 0;
    return num == value->doubleValue() ? 1 : 0;
  }

  if( KSUtil::checkType( context, value, KSValue::StringType, false ) )
  {
    return criteria == value->stringValue() ? 1 : 0;
  }

  if( KSUtil::checkType( context, value, KSValue::BoolType, false ) )
  {
    bool criteria_bool = criteria.lower() == "true";
    if( !value && (criteria.lower() != "false") ) return 0;
    return criteria_bool == value->boolValue() ? 1 : 0;
  }

  if( KSUtil::checkType( context, value, KSValue::ListType, false ) )
  {
    QValueList<KSValue::Ptr>& args = value->listValue();
    QValueList<KSValue::Ptr>::Iterator it = args.begin();
    QValueList<KSValue::Ptr>::Iterator end = args.end();
    
    int count = 0;
    for( ; it != end; ++it )
       if ( !kspreadfunc_countif_helper( context, *it, criteria ) ) count++;
 
    return count;
  }

  return 0;   
}
 
// Function: COUNTIF
// This is a lot simplified !!
// Not really Excel-compatible as it understands only "equals to" relation
bool kspreadfunc_countif( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "COUNTIF", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::StringType, true ) )
    return false;

  KSValue* value = args[0];
  QString criteria = args[1]->stringValue();

  int result = kspreadfunc_countif_helper( context, value, criteria );

  context.setValue( new KSValue( result ) );
  return true;
}

// Function: FIB
bool kspreadfunc_fib( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  if ( !KSUtil::checkArgumentsCount( context, 1, "FIB", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

/*
Lucas' formula for the nth Fibonacci number F(n) is given by

         ((1+sqrt(5))/2)^n - ((1-sqrt(5))/2)^n
  F(n) = ------------------------------------- .
                         sqrt(5)

*/
  double n = args[0]->doubleValue();

  double s = sqrt(5.0);
  double result = ( pow((1+s)/2,n) - pow((1-s)/2,n) ) / s;

  context.setValue( new KSValue( result ) );
  return true;
}

// Function: MULTIPLEOPERATIONS
bool kspreadfunc_multipleOP( KSContext& context )
{
  if (gWorking)
    return true;

  gWorking = true;

  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  QValueList<KSValue::Ptr>& extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 5, "MULTIPLEOPERATIONS", true ) )
  {
    gWorking = false;
    return false;
  }

  // 0: cell must contain formula with double/int result
  // 0, 1, 2, 3, 4: must contain integer/double
  for (int i = 0; i < 5; ++i)
  {
    if ( !KSUtil::checkType( context, args[i], KSValue::DoubleType, true ) )
    {
      gWorking = false;
      return false;
    }
  }

  ((KSpreadInterpreter *) context.interpreter() )->document()->emitBeginOperation();

  double oldCol = args[1]->doubleValue();
  double oldRow = args[3]->doubleValue();
  kdDebug() << "Old values: Col: " << oldCol << ", Row: " << oldRow << endl;

  KSpreadCell * cell;
  KSpreadTable * table = ((KSpreadInterpreter *) context.interpreter() )->table();

  KSpreadPoint point( extra[1]->stringValue() );  
  KSpreadPoint point2( extra[3]->stringValue() );
  KSpreadPoint point3( extra[0]->stringValue() );

  if ( ( args[1]->doubleValue() != args[2]->doubleValue() )
       || ( args[3]->doubleValue() != args[4]->doubleValue() ) )
  {
    cell = table->cellAt( point.pos.x(), point.pos.y() );
    cell->setValue( args[2]->doubleValue() );
    kdDebug() << "Setting value " << args[2]->doubleValue() << " on cell " << point.pos.x() 
              << ", " << point.pos.y() << endl;

    cell = table->cellAt( point2.pos.x(), point.pos.y() );
    cell->setValue( args[4]->doubleValue() );
    kdDebug() << "Setting value " << args[4]->doubleValue() << " on cell " << point2.pos.x() 
              << ", " << point2.pos.y() << endl;
  }

  KSpreadCell * cell1 = table->cellAt( point3.pos.x(), point3.pos.y() );
  cell1->calc( false );

  double d = cell1->valueDouble();
  kdDebug() << "Cell: " << point3.pos.x() << "; " << point3.pos.y() << " with value "
            << d << endl;

  kdDebug() << "Resetting old values" << endl;
 
  cell = table->cellAt( point.pos.x(), point.pos.y() );
  cell->setValue( oldCol );

  cell = table->cellAt( point2.pos.x(), point2.pos.y() );
  cell->setValue( oldRow );

  cell1->calc( false );

  ((KSpreadInterpreter *) context.interpreter() )->document()->emitEndOperation();

  context.setValue( new KSValue( (double) d ) );
 
  gWorking = false;
  return true;
}

static bool kspreadfunc_average_helper( KSContext & context, QValueList<KSValue::Ptr> & args, double & result,int & number)
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_average_helper( context, (*it)->listValue(), result, number) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
    {
      result += (*it)->doubleValue();
      number++;
    }
  }

  return true;
}

static bool kspreadfunc_variance_helper( KSContext& context, QValueList<KSValue::Ptr>& args, double& result, double avera)
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_variance_helper( context, (*it)->listValue(), result ,avera) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
      {
      result += ( (*it)->doubleValue() - avera ) * ( (*it)->doubleValue() - avera );
      }
  }

  return true;
}

static bool kspreadfunc_stddev_helper( KSContext& context, QValueList<KSValue::Ptr>& args, double& result,double& avera)
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_stddev_helper( context, (*it)->listValue(), result ,avera) )
        return false;

    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
    {
      result += (((*it)->doubleValue()-avera)*((*it)->doubleValue()-avera));
    }
  }

  return true;
}

// Function: SUBTOTAL:
bool kspreadfunc_subtotal( KSContext & context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  QValueList<KSValue::Ptr>& extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "SUBTOTAL", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::IntType, true ) )
    return false;

  // create a new list
  QValueList<KSValue::Ptr> * list = new QValueList<KSValue::Ptr>;
  int function = args[0]->intValue();
  
  KSValue * c = 0;
  KSpreadCell  * cell = 0;
  KSpreadTable * table = ((KSpreadInterpreter *) context.interpreter() )->table();
  KSpreadMap * map = ((KSpreadInterpreter *) context.interpreter() )->document()->map();

  kdDebug() << "Range: " << extra[1]->stringValue() << endl;

  KSpreadRange range ( extra[1]->stringValue(), map, table );
  if ( !range.isValid() )
  {
    KSpreadPoint point( extra[1]->stringValue(), map, table );
    
    if ( !point.isValid() )
      return false;

    range.range = QRect( point.pos.x(), point.pos.y(),
                         point.pos.x(), point.pos.y() );

  }
   
  KSValue * l = new KSValue( KSValue::ListType );
  int count = 0;
  int countA = 0;
  double sum = 0.0;
  double max = 0.0;

  if ( function == 6 ) // product
    sum = 1.0;

  int x = range.range.left();
  int y = range.range.top();
  int bottom = range.range.bottom();
  
  for ( ; y <= bottom; ++y )
  {
    cell = table->cellAt( x, y );
    if ( cell->isDefault() || cell->text().find( "SUBTOTAL", 0, false ) != -1 )
      continue;
    
    ++count;
    if ( cell->isNumeric() )
    {
      ++countA;
      if ( function == 1 || function == 9 || function == 7 || function == 8
           || function == 10 || function == 11 )
        sum += cell->valueDouble();
      else if ( function == 4 )
      {
        if ( countA == 1 )
          max = cell->valueDouble();
        else
        if ( cell->valueDouble() > max )
          max = cell->valueDouble();
      }
      else if ( function == 5 )
      {
        if ( countA == 1 )
          max = cell->valueDouble();
        else
        if ( cell->valueDouble() < max )
          max = cell->valueDouble();
      }
      else if ( function == 6 )
        sum *= cell->valueDouble();
    }


    c = new KSValue( cell->valueDouble() );
    l->listValue().append( c );
  }
  list->append ( l );


  double result  = 0.0;
  double average = sum / countA;

  switch( function )
  {
   case 1: // Average
    context.setValue( new KSValue( average ) );
    break;
   case 2: // Count
    context.setValue( new KSValue( count ) );
    break;
   case 3: // CountA (count without blanks)
    context.setValue( new KSValue( countA ) );
    break;
   case 4: // MAX
    context.setValue( new KSValue( max ) );
    break;
   case 5: // Min
    context.setValue( new KSValue( max ) );
    break;
   case 6: // Product
    context.setValue( new KSValue( sum ) );
    break;
   case 7: // StDev
    kspreadfunc_stddev_helper( context, *list, result, average );
    context.setValue( new KSValue( sqrt( result / ((double) (countA - 1) ) ) ) );
    break;
   case 8: // StDevP
    kspreadfunc_stddev_helper( context, *list, result, average );
    context.setValue( new KSValue( sqrt( result / countA ) ) );
    break;
   case 9: // Sum
    context.setValue( new KSValue( sum ) );
    break;
   case 10: // Var
    kspreadfunc_variance_helper( context, *list, result, average );
    context.setValue( new KSValue( (double)(result / (countA - 1)) ) );
    break;
   case 11: // VarP
    kspreadfunc_variance_helper( context, *list, result, average );
    context.setValue( new KSValue( (double)(result / countA) ) );
    break;
   default:
    return false;
  }

  return true;
}

