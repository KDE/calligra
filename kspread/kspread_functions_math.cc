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
#include <klocale.h>

#include <koscript_parser.h>
#include <koscript_util.h>
#include <koscript_func.h>
#include <koscript_synext.h>

#include "kspread_cell.h"
#include "kspread_sheet.h"
#include "kspread_interpreter.h"
#include "kspread_doc.h"
#include "kspread_functions.h"
#include "kspread_functions_helper.h"
#include "kspread_util.h"

namespace math_local
{
  KSpreadCell * gCell = 0;
}

using namespace math_local;

// prototypes
bool kspreadfunc_abs( KSContext& context );
bool kspreadfunc_ceil( KSContext& context );
bool kspreadfunc_ceiling( KSContext& context );
bool kspreadfunc_count( KSContext& context );
bool kspreadfunc_counta( KSContext& context );
bool kspreadfunc_countblank( KSContext& context );
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
bool kspreadfunc_kproduct( KSContext& context );
bool kspreadfunc_lcm( KSContext & context );
bool kspreadfunc_ln( KSContext& context );
bool kspreadfunc_log( KSContext& context );
bool kspreadfunc_log2( KSContext& context );
bool kspreadfunc_log10( KSContext& context );
bool kspreadfunc_logn( KSContext& context );
bool kspreadfunc_max( KSContext& context );
bool kspreadfunc_maxa( KSContext& context );
bool kspreadfunc_min( KSContext& context );
bool kspreadfunc_mina( KSContext& context );
bool kspreadfunc_mod( KSContext& context );
bool kspreadfunc_mround( KSContext& context );
bool kspreadfunc_mult( KSContext& context );
bool kspreadfunc_multinomial( KSContext& context );
bool kspreadfunc_odd( KSContext& context );
bool kspreadfunc_pow( KSContext& context );
bool kspreadfunc_quotient( KSContext& context );
bool kspreadfunc_product( KSContext& context );
bool kspreadfunc_rand( KSContext& context );
bool kspreadfunc_randbetween( KSContext& context );
bool kspreadfunc_randbernoulli( KSContext & context );
bool kspreadfunc_randbinom( KSContext & context );
bool kspreadfunc_randexp( KSContext & context );
bool kspreadfunc_randnegbinom( KSContext & context );
bool kspreadfunc_randnorm( KSContext & context );
bool kspreadfunc_randpoisson( KSContext & context );
bool kspreadfunc_rootn( KSContext& context );
bool kspreadfunc_round( KSContext& context );
bool kspreadfunc_rounddown( KSContext& context );
bool kspreadfunc_roundup( KSContext& context );
bool kspreadfunc_sign( KSContext& context );
bool kspreadfunc_sqrt( KSContext& context );
bool kspreadfunc_sqrtpi( KSContext& context );
bool kspreadfunc_subtotal( KSContext& context );
bool kspreadfunc_sum( KSContext& context );
bool kspreadfunc_sumif( KSContext& context );
bool kspreadfunc_suma( KSContext& context );
bool kspreadfunc_sumsq( KSContext& context );
bool kspreadfunc_trunc( KSContext& context );


bool kspreadfunc_multipleOP( KSContext& context );

// registers all math functions
void KSpreadRegisterMathFunctions()
{
  gCell = 0;
  KSpreadFunctionRepository * repo = KSpreadFunctionRepository::self();

  repo->registerFunction( "MULTIPLEOPERATIONS", kspreadfunc_multipleOP );

  repo->registerFunction( "ABS",           kspreadfunc_abs );
  repo->registerFunction( "CEIL",          kspreadfunc_ceil );
  repo->registerFunction( "CEILING",       kspreadfunc_ceiling );
  repo->registerFunction( "COUNT",         kspreadfunc_count );
  repo->registerFunction( "COUNTA",        kspreadfunc_counta );
  repo->registerFunction( "COUNTBLANK",    kspreadfunc_countblank );
  repo->registerFunction( "COUNTIF",       kspreadfunc_countif );
  repo->registerFunction( "CUR",           kspreadfunc_cur );
  repo->registerFunction( "DIV",           kspreadfunc_div );
  repo->registerFunction( "EPS",           kspreadfunc_eps );
  repo->registerFunction( "EVEN",          kspreadfunc_even );
  repo->registerFunction( "EXP",           kspreadfunc_exp );
  repo->registerFunction( "FACT",          kspreadfunc_fact );
  repo->registerFunction( "FACTDOUBLE",    kspreadfunc_factdouble );
  repo->registerFunction( "FIB",           kspreadfunc_fib ); // KSpread-specific, like Quattro-Pro's FIB
  repo->registerFunction( "FLOOR",         kspreadfunc_floor );
  repo->registerFunction( "G_PRODUCT",     kspreadfunc_kproduct ); // Gnumeric compatibility
  repo->registerFunction( "GCD",           kspreadfunc_gcd );
  repo->registerFunction( "INT",           kspreadfunc_int );
  repo->registerFunction( "INV",           kspreadfunc_inv );
  repo->registerFunction( "KPRODUCT",      kspreadfunc_kproduct );
  repo->registerFunction( "LCD",           kspreadfunc_gcd ); // obsolete, use GCD instead, remove in 1.4
  repo->registerFunction( "LCM",           kspreadfunc_lcm );
  repo->registerFunction( "LN",            kspreadfunc_ln );
  repo->registerFunction( "LOG",           kspreadfunc_log );
  repo->registerFunction( "LOG2",          kspreadfunc_log2 );
  repo->registerFunction( "LOG10",         kspreadfunc_log10 );
  repo->registerFunction( "LOGN",          kspreadfunc_logn );
  repo->registerFunction( "MAX",           kspreadfunc_max );
  repo->registerFunction( "MAXA",          kspreadfunc_maxa );
  repo->registerFunction( "MIN",           kspreadfunc_min );
  repo->registerFunction( "MINA",          kspreadfunc_mina );
  repo->registerFunction( "MOD",           kspreadfunc_mod );
  repo->registerFunction( "MROUND",        kspreadfunc_mround );
  repo->registerFunction( "MULTIPLY",      kspreadfunc_mult );
  repo->registerFunction( "MULTINOMIAL",   kspreadfunc_multinomial );
  repo->registerFunction( "ODD",           kspreadfunc_odd );
  repo->registerFunction( "POW",           kspreadfunc_pow ); // remove in 1.5
  repo->registerFunction( "POWER",         kspreadfunc_pow );
  repo->registerFunction( "QUOTIENT",      kspreadfunc_quotient );
  repo->registerFunction( "PRODUCT",       kspreadfunc_product );
  repo->registerFunction( "RAND",          kspreadfunc_rand );
  repo->registerFunction( "RANDBERNOULLI", kspreadfunc_randbernoulli );
  repo->registerFunction( "RANDBETWEEN",   kspreadfunc_randbetween );
  repo->registerFunction( "RANDBINOM",     kspreadfunc_randbinom );
  repo->registerFunction( "RANDEXP",       kspreadfunc_randexp );
  repo->registerFunction( "RANDNEGBINOM",  kspreadfunc_randnegbinom );
  repo->registerFunction( "RANDNORM",      kspreadfunc_randnorm );
  repo->registerFunction( "RANDPOISSON",   kspreadfunc_randpoisson );
  repo->registerFunction( "ROOTN",         kspreadfunc_rootn );
  repo->registerFunction( "ROUND",         kspreadfunc_round );
  repo->registerFunction( "ROUNDDOWN",     kspreadfunc_rounddown );
  repo->registerFunction( "ROUNDUP",       kspreadfunc_roundup );
  repo->registerFunction( "SIGN",          kspreadfunc_sign );
  repo->registerFunction( "SQRT",          kspreadfunc_sqrt );
  repo->registerFunction( "SQRTPI",        kspreadfunc_sqrtpi );
  repo->registerFunction( "SUBTOTAL",      kspreadfunc_subtotal );
  repo->registerFunction( "SUM",           kspreadfunc_sum );
  repo->registerFunction( "SUMIF",         kspreadfunc_sumif );
  repo->registerFunction( "SUMA",          kspreadfunc_suma );
  repo->registerFunction( "SUMSQ",         kspreadfunc_sumsq );
  repo->registerFunction( "TRUNC",         kspreadfunc_trunc );
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
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "CEIL", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( approx_equal( floor( args[0]->doubleValue() ), args[0]->doubleValue() ) )
    context.setValue( new KSValue( args[0]->doubleValue() ) );
  else
    context.setValue( new KSValue( ceil( args[0]->doubleValue() ) ) );

  return true;
}

// Function: ceiling
bool kspreadfunc_ceiling( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  double res;
  double number;

  if ( !KSUtil::checkArgumentsCount( context, 2, "CEILING", true ) )
  {
    if ( !KSUtil::checkArgumentsCount( context, 1, "CEILING", true ) )
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
      return false;

    number = args[0]->doubleValue();

    if ( number >= 0 )
      res = 1.0;
    else
      res = -1.0;
  }
  else
  {
    if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
      return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
      return false;

    number = args[0]->doubleValue();

    res = args[1]->doubleValue();
  }

  if ( res == 0 )
    return false;

  double d = number / res;

  if ( d < 0 )
    return false;

  if ( approx_equal( floor( d ), d ) )
    context.setValue( new KSValue( d * res ) );
  else
    context.setValue( new KSValue( ceil( d ) * res ) );

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

// Function: LOG2
bool kspreadfunc_log2( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "LOG2", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  double d = args[0]->doubleValue();

  if ( d <= 0 )
    return false;

  context.setValue( new KSValue( log( d ) /log( 2.0 ) ) );
  return true;
}

// Function: LOG10
bool kspreadfunc_log10( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "LOG10", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  double d = args[0]->doubleValue();

  if ( d <= 0 )
    return false;

  context.setValue( new KSValue( log10( d ) ) );
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

static bool kspreadfunc_sum_helper( KSContext & context, QValueList<KSValue::Ptr> & args,
                                    double & result, bool aMode )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_sum_helper( context, (*it)->listValue(), result, aMode ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
    {
      result += (*it)->doubleValue();
    }
    else if ( aMode )
    {
      if ( KSUtil::checkType( context, *it, KSValue::StringType, false ) )
      {
        // TODO: needed?
      }
      else
      if ( KSUtil::checkType( context, *it, KSValue::BoolType, false ) )
      {
        result += ( (*it)->boolValue() ? 1.0 : 0.0 );
      }
    }
  }

  return true;
}

// Function: sum
bool kspreadfunc_sum( KSContext & context )
{
  double result = 0.0;
  bool b = kspreadfunc_sum_helper( context, context.value()->listValue(), result, false );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

static bool kspreadfunc_sumif_helper( KSContext & context, KSValue *value,
                                      QValueList<KSValue::Ptr> &args,
                                      KSpreadDB::Condition &cond,
                                      double & result, int &vpos, int &hpos )
{
  if ( KSUtil::checkType( context, value, KSValue::DoubleType, false ) )
  {
    if ( conditionMatches( cond, value->doubleValue() ) ) {
      if( hpos == -1 )
        result += value->doubleValue();
      else {
        result += (args[2]->listValue()[vpos])-> listValue()[hpos]->doubleValue();
        ++hpos;
      }
    }
    return true;
  }

  if ( KSUtil::checkType( context, value, KSValue::ListType, false ) ) {
    QValueList<KSValue::Ptr> values = value->listValue();
    QValueList<KSValue::Ptr>::Iterator it = values.begin();
    QValueList<KSValue::Ptr>::Iterator end = values.end();

    for( ; it != end; ++it ) {
      if( !kspreadfunc_sumif_helper( context, *it, args, cond, result, vpos, hpos ) )
        return false;
      if ( vpos != -1 && KSUtil::checkType( context, *it, KSValue::ListType, false ) ){
        ++vpos;
        hpos = 0;
      }
    }
    return true;
  }
  return false;
}

bool kspreadfunc_sumif( KSContext & context )
{
  double result = 0.0;
  int hpos = -1; int vpos = -1; // these values become >=0 if there's a seperate sumrange
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  // starting sanity checks
  if( KSUtil::checkArgumentsCount( context, 3, "SUMIF", true ) ) {
  
    QValueList<KSValue::Ptr> checkRange = args[0]->listValue();
    QValueList<KSValue::Ptr> sumRange   = args[2]->listValue();

    if( checkRange.count() <= sumRange.count() ) {
      for( uint i = 0 ; i < checkRange.count() ; ++i ) {
        if ( checkRange[i]->listValue().count() > sumRange[i]->listValue().count() )
          return false;
      }
      // the ranges do match, make hpos and vpos > -1
      hpos = 0;
      vpos = 0;
    } else return false;

  } else if ( !KSUtil::checkArgumentsCount( context, 2, "SUMIF", false ) )
      return false;
    
  if( !KSUtil::checkType( context, args[1], KSValue::StringType ) )
    return false;
  // end of sanity checks

  KSValue *value = args[0];
  KSpreadDB::Condition cond;
  getCond( cond, args[1]->stringValue() );

  bool b = kspreadfunc_sumif_helper( context, value, args, cond, result, vpos, hpos );

  if ( b ) {
    context.setValue( new KSValue( result ) );
  }
  return b;
}

// Function: suma
bool kspreadfunc_suma( KSContext & context )
{
  double result = 0.0;
  bool b = kspreadfunc_sum_helper( context, context.value()->listValue(), result, true );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

static bool kspreadfunc_product_helper( KSContext & context,
                                        QValueList<KSValue::Ptr> & args,
                                        double & result, int & number )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_product_helper( context, (*it)->listValue(), result, number ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
    {
      ++number;
      result *= (*it)->doubleValue();
    }
  }

  return true;
}

// Function: product
bool kspreadfunc_product( KSContext& context )
{
  double result = 1.0;
  int number = 0;
  bool b = kspreadfunc_product_helper( context,
                                       context.value()->listValue(),
                                       result, number );

  if ( number == 0 )
    result = 0.0; // Excel specific

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

// Function: product
bool kspreadfunc_kproduct( KSContext& context )
{
  double result = 1.0;
  int number = 0;
  bool b = kspreadfunc_product_helper( context,
                                       context.value()->listValue(),
                                       result, number );

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
  int number = 0;

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {
      if ( !kspreadfunc_product_helper( context, (*it)->listValue(), result, number ) )
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
      if ( !kspreadfunc_sum_helper( context, (*it)->listValue(), result, false ) )
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

static bool kspreadfunc_max_helper( KSContext & context, QValueList<KSValue::Ptr> & args,
                                    double & result, int & inter, int mode )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {

      if ( !kspreadfunc_max_helper( context, (*it)->listValue(), result, inter, mode ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
    {
      if ( inter == 0 )
      {
        result = (*it)->doubleValue();
        inter  = 1;
      }
      if ( result < (*it)->doubleValue() )
        result = (*it)->doubleValue();
    }
    else if ( mode == 1 && KSUtil::checkType( context, *it, KSValue::BoolType, true ) )
    {
      double n = ( (*it)->boolValue() ? 1.0 : 0.0 );
      if ( inter == 0 )
      {
        result = n;
        inter = 1;
      }
      if ( result < n )
        result = n;
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

  bool b = kspreadfunc_max_helper( context, context.value()->listValue(), result, inter, 0 );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

// Function: MAXA
bool kspreadfunc_maxa( KSContext & context )
{
  double result = 0.0;

  //init first element
  int inter = 0;

  bool b = kspreadfunc_max_helper( context, context.value()->listValue(), result, inter, 1 );

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

static bool kspreadfunc_min_helper( KSContext & context, QValueList<KSValue::Ptr> & args,
                                    double & result, int & inter, int mode )
{
  QValueList<KSValue::Ptr>::Iterator it = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  for ( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::ListType, false ) )
    {

      if ( !kspreadfunc_min_helper( context, (*it)->listValue(), result, inter, mode ) )
        return false;
    }
    else if ( KSUtil::checkType( context, *it, KSValue::DoubleType, true ) )
    {
      if ( inter == 0 )
      {
        result = (*it)->doubleValue();
        inter  = 1;
      }
      if ( result > (*it)->doubleValue() )
        result = (*it)->doubleValue();
    }
    else if ( mode == 1 && KSUtil::checkType( context, *it, KSValue::BoolType, true ) )
    {
      double n = ( (*it)->boolValue() ? 1.0 : 0.0 );
      if ( inter == 0 )
      {
        result = n;
        inter  = 1;
      }
      if ( result > n )
        result = n;
    }
  }

  return true;
}

// Function: MIN
bool kspreadfunc_min( KSContext& context )
{
  double result = 0.0;

  //init first element
  int inter = 0;

  bool b = kspreadfunc_min_helper( context, context.value()->listValue(), result, inter, 0 );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

// Function: MINA
bool kspreadfunc_mina( KSContext& context )
{
  double result = 0.0;

  //init first element
  int inter = 0;

  bool b = kspreadfunc_min_helper( context, context.value()->listValue(), result, inter, 1 );

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

bool kspreadfunc_randexp( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "RANDEXP", true ) )
    return false;

  if( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  double d = args[0]->doubleValue();

  d = -1 * d * log( (double) rand() / ( RAND_MAX + 1.0 ) );

  context.setValue( new KSValue( d ) );
  return true;
}

bool kspreadfunc_randbinom( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "RANDBINOM", true ) )
    return false;

  if( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;

  double d  = args[0]->doubleValue();
  int    tr = args[1]->intValue();

  if ( d < 0 || d > 1 )
    return false;

  if ( tr < 0 )
    return false;

  // taken from gnumeric
  double x = pow(1 - d, tr);
  double r = (double) rand() / ( RAND_MAX + 1.0 );
  double t = x;
  double i = 0;

  while (r > t)
  {
    x *= (((tr - i) * d) / ((1 + i) * (1 - d)));
    i += 1;
    t += x;
  }

  context.setValue( new KSValue( i ) );
  return true;
}

bool kspreadfunc_randnegbinom( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "RANDNEGBINOM", true ) )
    return false;

  if( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;

  double d = args[0]->doubleValue();
  int    f = args[1]->intValue();

  if ( d < 0 || d > 1 )
    return false;

  if ( f < 0 )
    return false;

  // taken from Gnumeric
  double x = pow(d, f);
  double r = (double) rand() / ( RAND_MAX + 1.0 );
  double t = x;
  double i = 0;

  while (r > t)
  {
    x *= ( ( ( f + i ) * ( 1 - d ) ) / (1 + i) ) ;
    i += 1;
    t += x;
  }

  context.setValue( new KSValue( i ) );
  return true;
}

bool kspreadfunc_randbernoulli( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "RANDBERNOULLI", true ) )
    return false;

  if( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  double d = args[0]->doubleValue();
  if ( d < 0 || d > 1 )
    return false;

  // taken from Gnumeric
  double r = (double) rand() / ( RAND_MAX + 1.0 );

  context.setValue( new KSValue( ( r <= d ) ? 1.0 : 0.0 ) );
  return true;
}

bool kspreadfunc_randnorm( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "RANDNORM", true ) )
    return false;

  if( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;

  double mu = args[0]->doubleValue();
  double sigma = args[1]->doubleValue();

  //using polar form of the Box-Muller transformation
  //refer to http://www.taygeta.com/random/gaussian.html for more info

  double x1, x2, w;
  do {
    x1 = (double) rand() / (RAND_MAX + 1.0);
    x2 = (double) rand() / (RAND_MAX + 1.0);
    x1 = 2.0 * x1 - 1.0;
    x2 = 2.0 * x2 - 1.0;
    w = x1 * x1 + x2 * x2;
  } while (w >= 1.0);

  w = sqrt ((-2.0 * log (w)) / w);
  double res = x1 * w;

  res = res * sigma + mu;
  context.setValue( new KSValue( res ) );
  return true;
}

bool kspreadfunc_randpoisson( KSContext & context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "RANDPOISSON", true ) )
    return false;

  if( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  double d = args[0]->doubleValue();
  if ( d < 0 )
    return false;

  // taken from Gnumeric...
  double x = exp( -1 * d );
  double r = ( double ) rand() / ( RAND_MAX + 1.0 );
  double t = x;
  double i = 0;

  while ( r > t )
  {
    x *= d / ( i + 1 );
    i += 1;
    t += x;
  }

  context.setValue( new KSValue( i ) );
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
        if( result < 0 ) result += (int)args[1]->doubleValue();
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

bool kspreadfunc_mround( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "MROUND", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;

  double d = args[0]->doubleValue();
  double m = args[1]->doubleValue();

  if ( ( d > 0 && m < 0 )
      || ( d < 0 && m > 0 ) )
    return false;

  int sign = 1;

  if ( d < 0 )
  {
    sign = -1;
    d = -d;
    m = -m;
  }

  // from gnumeric:
  double mod = fmod( d, m );
  double div = d - mod;

  double accuracyLimit = 0.0000003;
  double result = sign * ( div + ( ( mod + accuracyLimit >= m / 2 ) ? m : 0 ) );

  context.setValue( new KSValue( result ) );
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

static bool kspreadfunc_count_helper( KSContext& context, QValueList<KSValue::Ptr> & args, double & result )
{
  QValueList<KSValue::Ptr>::Iterator it  = args.begin();
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
	++result;
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

  // TRUNC(254;-2) is 200
  if( precision < 0 )
  {
      precision = -precision;
      result = floor( result/pow(10.0,precision) ) * pow(10.0,precision);
      precision = 0;
  }

  int factor = (int) pow( 10.0, precision );

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

static bool kspreadfunc_counta_helper( KSContext& context, QValueList<KSValue::Ptr> & args,
                                       QValueList<KSValue::Ptr> & extra, int & resultA )
{
  QValueList<KSValue::Ptr>::Iterator eit  = extra.begin();
  QValueList<KSValue::Ptr>::Iterator eend = extra.end();

  QValueList<KSValue::Ptr>::Iterator it  = args.begin();

  KSpreadMap * map = ((KSpreadInterpreter *) context.interpreter() )->document()->map();
  KSpreadSheet * sheet = ((KSpreadInterpreter *) context.interpreter() )->sheet();
  KSpreadSheet * t = 0;
  KSpreadCell * cell = 0;

  for ( ; eit != eend; ++eit )
  {
    int right = 0;
    int left = 0;
    int bottom = 0;
    int top = 0;
    if ( KSUtil::checkType( context, *eit, KSValue::StringType, true ) )
    {
      KSpreadRange range( (*eit)->stringValue(), map );
      if ( range.range.left() <= 0 || range.range.right() <= 0 )
      {
        KSpreadPoint point( (*eit)->stringValue(), map );
        if ( point.pos.x() <= 0 || point.pos.y() <= 0 )
          return false;

        right  = point.pos.x();
        bottom = point.pos.y();
        left   = right;
        top    = bottom;

        if ( !point.isSheetKnown() )
          t = sheet;
        else
          t = point.sheet;
      }
      else
      {
        right  = range.range.right();
        bottom = range.range.bottom();
        left   = range.range.left();
        top    = range.range.top();

        if ( !range.isSheetKnown() )
          t = sheet;
        else
          t = range.sheet;
      }

      for ( int x = left; x <= right; ++x )
      {
        for ( int y = top; y <= bottom; ++y )
        {
          kdDebug() << "Cell: " << x << ", " << y << endl;
          cell = t->cellAt( x, y );
          if ( !cell->isDefault() && !cell->isEmpty() )
          {
            if ( !cell->strOutText().isEmpty() )
              ++resultA;
          }
        }
      }
    }
    else if ( KSUtil::checkType( context, *it, KSValue::StringType, true ) )
    {
      kdDebug() << "String value" << endl;
      if ( !(*it)->stringValue().isEmpty() )
        ++resultA;
    }
    else if ( !KSUtil::checkType( context, *it, KSValue::Empty, true ) )
    {
      kdDebug() << "Empty" << endl;
      ++resultA;
    }

    ++it;
  }

  return true;
}

// Function: COUNTA
bool kspreadfunc_counta( KSContext& context )
{
  int resultA = 0;

  bool b = kspreadfunc_counta_helper( context, context.value()->listValue(),
                                      context.extraData()->listValue(), resultA );

  if ( b )
    context.setValue( new KSValue( resultA ) );

  return b;
}

static bool kspreadfunc_countblank_helper( KSContext& context, QValueList<KSValue::Ptr> & args,
                                           int & result )
{
  KSpreadMap   * map   = ((KSpreadInterpreter *) context.interpreter() )->document()->map();
  KSpreadSheet * sheet = ((KSpreadInterpreter *) context.interpreter() )->sheet();
  KSpreadCell  * cell  = 0;
  KSpreadSheet * t     = 0;

  QValueList<KSValue::Ptr>::Iterator it  = args.begin();
  QValueList<KSValue::Ptr>::Iterator end = args.end();

  kdDebug() << "Result: " << result << endl;

  for( ; it != end; ++it )
  {
    if ( KSUtil::checkType( context, *it, KSValue::StringType, false ) )
    {
      kdDebug() << "S:" << (*it)->stringValue() << endl;
      int right = 0;
      int left = 0;
      int bottom = 0;
      int top = 0;

      KSpreadRange range( (*it)->stringValue(), map );
      if ( range.range.left() <= 0 || range.range.right() <= 0 )
      {
        KSpreadPoint point( (*it)->stringValue(), map );

        if ( point.pos.x() <= 0 || point.pos.y() <= 0 )
          continue; // not blank, whatever it is...

        right  = point.pos.x();
        bottom = point.pos.y();
        left   = right;
        top    = bottom;

        if ( !point.isSheetKnown() )
          t = sheet;
        else
          t = point.sheet;
      }
      else
      {
        right  = range.range.right();
        bottom = range.range.bottom();
        left   = range.range.left();
        top    = range.range.top();

        if ( !range.isSheetKnown() )
          t = sheet;
        else
          t = range.sheet;
      }

      for ( int x = left; x <= right; ++x )
      {
        for ( int y = top; y <= bottom; ++y )
        {
          kdDebug() << "Cell: " << x << ", " << y << endl;
          cell = t->cellAt( x, y );
          if ( cell->isDefault() || cell->isEmpty() || cell->strOutText().isEmpty() )
          {
            ++result;
          }
        }
      }
    }
  }

  kdDebug() << "Result 2: " << result << endl;

  return true;
}

// Function: COUNTBLANK
bool kspreadfunc_countblank( KSContext& context )
{
  int result = 0;

  kdDebug() << "Countblank: " << endl;

  bool b = kspreadfunc_countblank_helper( context, context.extraData()->listValue(), result );

  if ( b )
    context.setValue( new KSValue( result ) );

  return b;
}

static int kspreadfunc_countif_helper( KSContext& context, KSValue *value,
                                       const QString &criteria )
{
  if( KSUtil::checkType( context, value, KSValue::DoubleType, false ) )
  {
    KSpreadDB::Condition cond;
    getCond( cond, criteria );

    return conditionMatches( cond, value->doubleValue() ) ? 1 : 0;
  }

  if( KSUtil::checkType( context, value, KSValue::StringType, false ) )
  {
    KSpreadDB::Condition cond;
    getCond( cond, criteria );

    return conditionMatches( cond, value->stringValue() ) ? 1 : 0;
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
  if (gCell)
  {
    context.setValue( new KSValue( ((KSpreadInterpreter *) context.interpreter() )->cell()->value().asFloat() ) );
    return true;
  }

  gCell = ((KSpreadInterpreter *) context.interpreter() )->cell();

  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  QValueList<KSValue::Ptr>& extra = context.extraData()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 5, "MULTIPLEOPERATIONS", true ) )
  {
    gCell = 0;
    return false;
  }

  // 0: cell must contain formula with double/int result
  // 0, 1, 2, 3, 4: must contain integer/double
  for (int i = 0; i < 5; ++i)
  {
    if ( !KSUtil::checkType( context, args[i], KSValue::DoubleType, true ) )
    {
      gCell = 0;
      return false;
    }
  }

  //  ((KSpreadInterpreter *) context.interpreter() )->document()->emitBeginOperation();

  double oldCol = args[1]->doubleValue();
  double oldRow = args[3]->doubleValue();
  kdDebug() << "Old values: Col: " << oldCol << ", Row: " << oldRow << endl;

  KSpreadCell * cell;
  KSpreadSheet * sheet = ((KSpreadInterpreter *) context.interpreter() )->sheet();

  KSpreadPoint point( extra[1]->stringValue() );
  KSpreadPoint point2( extra[3]->stringValue() );
  KSpreadPoint point3( extra[0]->stringValue() );

  if ( ( args[1]->doubleValue() != args[2]->doubleValue() )
       || ( args[3]->doubleValue() != args[4]->doubleValue() ) )
  {
    cell = sheet->cellAt( point.pos.x(), point.pos.y() );
    cell->setValue( args[2]->doubleValue() );
    kdDebug() << "Setting value " << args[2]->doubleValue() << " on cell " << point.pos.x()
              << ", " << point.pos.y() << endl;

    cell = sheet->cellAt( point2.pos.x(), point.pos.y() );
    cell->setValue( args[4]->doubleValue() );
    kdDebug() << "Setting value " << args[4]->doubleValue() << " on cell " << point2.pos.x()
              << ", " << point2.pos.y() << endl;
  }

  KSpreadCell * cell1 = sheet->cellAt( point3.pos.x(), point3.pos.y() );
  cell1->calc( false );

  double d = cell1->value().asFloat();
  kdDebug() << "Cell: " << point3.pos.x() << "; " << point3.pos.y() << " with value "
            << d << endl;

  kdDebug() << "Resetting old values" << endl;

  cell = sheet->cellAt( point.pos.x(), point.pos.y() );
  cell->setValue( oldCol );

  cell = sheet->cellAt( point2.pos.x(), point2.pos.y() );
  cell->setValue( oldRow );

  cell1->calc( false );

  // ((KSpreadInterpreter *) context.interpreter() )->document()->emitEndOperation();

  context.setValue( new KSValue( (double) d ) );

  gCell = 0;
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
  KSpreadSheet * sheet = ((KSpreadInterpreter *) context.interpreter() )->sheet();
  KSpreadMap * map = ((KSpreadInterpreter *) context.interpreter() )->document()->map();

  kdDebug() << "Range: " << extra[1]->stringValue() << endl;

  KSpreadRange range ( extra[1]->stringValue(), map, sheet );
  if ( !range.isValid() )
  {
    KSpreadPoint point( extra[1]->stringValue(), map, sheet );

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
    cell = sheet->cellAt( x, y );
    if ( cell->isDefault() || cell->text().find( "SUBTOTAL", 0, false ) != -1 )
      continue;

    ++count;
    if ( cell->value().isNumber() )
    {
      ++countA;
      if ( function == 1 || function == 9 || function == 7 || function == 8
           || function == 10 || function == 11 )
        sum += cell->value().asFloat();
      else if ( function == 4 )
      {
        if ( countA == 1 )
          max = cell->value().asFloat();
        else
        if ( cell->value().asFloat() > max )
          max = cell->value().asFloat();
      }
      else if ( function == 5 )
      {
        if ( countA == 1 )
          max = cell->value().asFloat();
        else
        if ( cell->value().asFloat() < max )
          max = cell->value().asFloat();
      }
      else if ( function == 6 )
        sum *= cell->value().asFloat();
    }


    c = new KSValue( cell->value().asFloat() );
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
    if ( countA == 0 )
      sum = 0.0;       // Excel compatibility :-(
    context.setValue( new KSValue( sum ) );
    break;
   case 7: // StDev
    kspreadfunc_stddev_helper( context, *list, result, average, false );
    context.setValue( new KSValue( sqrt( result / ((double) (countA - 1) ) ) ) );
    break;
   case 8: // StDevP
    kspreadfunc_stddev_helper( context, *list, result, average, false );
    context.setValue( new KSValue( sqrt( result / countA ) ) );
    break;
   case 9: // Sum
    context.setValue( new KSValue( sum ) );
    break;
   case 10: // Var
    kspreadfunc_variance_helper( context, *list, result, average, false );
    context.setValue( new KSValue( (double)(result / (countA - 1)) ) );
    break;
   case 11: // VarP
    kspreadfunc_variance_helper( context, *list, result, average, false );
    context.setValue( new KSValue( (double)(result / countA) ) );
    break;
   default:
    return false;
  }

  return true;
}

