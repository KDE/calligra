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

// built-in financial functions

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <kdebug.h>

#include <koscript_parser.h>
#include <koscript_util.h>
#include <koscript_func.h>
#include <koscript_synext.h>

#include "kspread_functions.h"
#include "kspread_functions_helper.h"
#include "kspread_util.h"


// prototypes (sorted)
bool kspreadfunc_accrint( KSContext& context );
bool kspreadfunc_accrintm( KSContext& context );
bool kspreadfunc_compound( KSContext& context );
bool kspreadfunc_continuous( KSContext& context );
bool kspreadfunc_coupnum( KSContext& context );
bool kspreadfunc_db( KSContext& context );
bool kspreadfunc_ddb( KSContext& context );
bool kspreadfunc_disc( KSContext& context );
bool kspreadfunc_dollarde( KSContext& context );
bool kspreadfunc_dollarfr( KSContext& context );
bool kspreadfunc_duration( KSContext& context );
bool kspreadfunc_effective( KSContext& context );
bool kspreadfunc_euro( KSContext& context );
bool kspreadfunc_fv( KSContext& context );
bool kspreadfunc_fv_annuity( KSContext& context );
bool kspreadfunc_intrate( KSContext& context );
bool kspreadfunc_ipmt( KSContext& context );
bool kspreadfunc_ispmt( KSContext& context );
bool kspreadfunc_level_coupon( KSContext& context );
bool kspreadfunc_nominal( KSContext& context );
bool kspreadfunc_nper( KSContext& context );
bool kspreadfunc_pmt( KSContext& context );
bool kspreadfunc_ppmt( KSContext& context );
bool kspreadfunc_pv( KSContext& context );
bool kspreadfunc_pv_annuity( KSContext& context );
bool kspreadfunc_received( KSContext& context );
bool kspreadfunc_sln( KSContext& context );
bool kspreadfunc_syd( KSContext& context );
bool kspreadfunc_tbilleq( KSContext& context );
bool kspreadfunc_tbillprice( KSContext& context );
bool kspreadfunc_tbillyield( KSContext& context );
bool kspreadfunc_zero_coupon( KSContext& context );

// registers all financial functions
void KSpreadRegisterFinancialFunctions()
{
  KSpreadFunctionRepository* repo = KSpreadFunctionRepository::self();

  repo->registerFunction( "ACCRINT", kspreadfunc_accrint );
  repo->registerFunction( "ACCRINTM", kspreadfunc_accrintm );
  repo->registerFunction( "COMPOUND", kspreadfunc_compound );
  repo->registerFunction( "CONTINUOUS", kspreadfunc_continuous );
  repo->registerFunction( "COUPNUM", kspreadfunc_coupnum );
  repo->registerFunction( "DB", kspreadfunc_db );
  repo->registerFunction( "DDB", kspreadfunc_ddb );
  repo->registerFunction( "DISC", kspreadfunc_disc );
  repo->registerFunction( "DOLLARDE", kspreadfunc_dollarde );
  repo->registerFunction( "DOLLARFR", kspreadfunc_dollarfr );
  repo->registerFunction( "DURATION", kspreadfunc_duration );
  repo->registerFunction( "EFFECT", kspreadfunc_effective );
  repo->registerFunction( "EFFECTIVE", kspreadfunc_effective );
  repo->registerFunction( "EURO", kspreadfunc_euro );  // KSpread-specific, Gnumeric-compatible
  repo->registerFunction( "FV", kspreadfunc_fv );
  repo->registerFunction( "FV_ANNUITY", kspreadfunc_fv_annuity );
  repo->registerFunction( "INTRATE", kspreadfunc_intrate );
  repo->registerFunction( "IPMT", kspreadfunc_ipmt );
  repo->registerFunction( "ISPMT", kspreadfunc_ispmt );
  repo->registerFunction( "LEVEL_COUPON", kspreadfunc_level_coupon );
  repo->registerFunction( "NOMINAL", kspreadfunc_nominal );
  repo->registerFunction( "NPER", kspreadfunc_nper );
  repo->registerFunction( "PMT", kspreadfunc_pmt );
  repo->registerFunction( "PPMT", kspreadfunc_ppmt );
  repo->registerFunction( "PV", kspreadfunc_pv );
  repo->registerFunction( "PV_ANNUITY", kspreadfunc_pv_annuity );
  repo->registerFunction( "RECEIVED", kspreadfunc_received );
  repo->registerFunction( "SLN", kspreadfunc_sln );
  repo->registerFunction( "SYD", kspreadfunc_syd );
  repo->registerFunction( "TBILLEQ", kspreadfunc_tbilleq );
  repo->registerFunction( "TBILLPRICE", kspreadfunc_tbillprice );
  repo->registerFunction( "TBILLYIELD", kspreadfunc_tbillyield );
  repo->registerFunction( "ZERO_COUPON", kspreadfunc_zero_coupon );
}

static double getPay( double rate, double nper, double pv, double fv, int type )
{
  double pvif, fvifa;

  pvif  = ( pow( 1 + rate, nper ) );
  fvifa = ( pow( 1 + rate, nper ) - 1 ) / rate;

  return ( ( -pv * pvif - fv ) / ( ( 1.0 + rate * type ) * fvifa ) );
}

static double getPrinc( double start, double pay,
                        double rate, double period )
{
  return ( start * pow( 1.0 + rate, period) + pay
           * ( ( pow( 1 + rate, period ) - 1 ) / rate ) );
}

static bool getCoupParameter( KSContext & context, QString const & fName, QDate & settlement,
                              QDate & maturity, int & frequency, int & basis, bool & eom )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 5, fName, true ) )
  {
    if ( !KSUtil::checkArgumentsCount( context, 4, fName, true ) )
    {
      if ( !KSUtil::checkArgumentsCount( context, 3, fName, true ) )
        return false;
    }
    else
    {
      if ( !KSUtil::checkType( context, args[3], KSValue::IntType, true ) )
        return false;
      basis = args[3]->intValue();
    }
  }
  else
  {
    if ( !KSUtil::checkType( context, args[3], KSValue::IntType, true ) )
      return false;
    if ( !KSUtil::checkType( context, args[4], KSValue::BoolType, true ) )
      return false;

    basis = args[3]->intValue();
    eom   = args[4]->boolValue();
  }

  if ( !getDate( context, args[0], settlement ) )
    return false;

  if ( !getDate( context, args[1], maturity ) )
    return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
    return false;

  frequency = args[2]->intValue();

  if (basis < 0 || basis > 5 || ( frequency == 0 ) || ( 12 % frequency != 0 )
      || settlement.daysTo( maturity ) <= 0)
    return false;

  return true;
}

// Function: COUPNUM - taken from GNUMERIC
bool kspreadfunc_coupnum( KSContext & context )
{
  QDate settlement;
  QDate maturity;
  int   frequency;
  int   basis = 0;
  bool  eom   = true;

  if ( !getCoupParameter( context, "COUPNUM", settlement, maturity,
                          frequency, basis, eom ) )
    return false;

  double result;
  QDate cDate( maturity );

  int months = maturity.month() - settlement.month()
    + 12 * ( maturity.year() - settlement.year() );

  subMonths( cDate, months );

  if ( eom && maturity.daysInMonth() == maturity.day() )
  {
    while( cDate.daysInMonth() != cDate.day() )
      cDate.addDays( 1 );
  }

  if ( settlement.day() >= cDate.day() )
    --months;

  result = ( 1 + months / ( 12 / frequency ) );

  context.setValue( new KSValue( result ) );
  return true;
}

// Function: ACCRINT
bool kspreadfunc_accrint( KSContext& context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  int basis = 0;

  if ( !KSUtil::checkArgumentsCount( context, 7, "ACCRINT", true ) )
  {
    if ( !KSUtil::checkArgumentsCount( context, 6, "ACCRINT", true ) )
      return false;
  }
  else
  {
    if ( !KSUtil::checkType( context, args[6], KSValue::IntType, true ) )
      return false;

    basis = args[6]->intValue();
  }

  QDate maturity;
  QDate firstInterest;
  QDate settlement;
  double rate, par;
  double frequency;

  if ( !getDate( context, args[0], maturity ) )
    return false;

  if ( !getDate( context, args[1], firstInterest ) )
    return false;

  if ( !getDate( context, args[2], settlement ) )
    return false;

  if ( !KSUtil::checkType( context, args[3], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[4], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[5], KSValue::DoubleType, true ) )
    return false;

  rate      = args[3]->doubleValue();
  par       = args[4]->doubleValue();
  frequency = (int) args[5]->doubleValue();

  if ( basis < 0 || basis > 4 || ( frequency == 0 ) || ( 12 % (int) frequency != 0 ) )
    return false;

  if ( ( settlement.daysTo( firstInterest ) < 0 )
       || ( firstInterest.daysTo( maturity ) > 0 ) )
    return false;

  double d = daysBetweenDates( maturity, settlement, basis );
  double y = daysPerYear( maturity, basis );

  if ( d < 0 || y <= 0 || par <= 0 || rate <= 0 )
    return false;

  double coeff = par * rate / frequency;
  double n = d / y;

  context.setValue( new KSValue( coeff * frequency * n ) );
  return true;
}

// Function: ACCRINTM
bool kspreadfunc_accrintm( KSContext& context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  int basis  = 0;
  double par = 1000;

  if ( !KSUtil::checkArgumentsCount( context, 5, "ACCRINTM", true ) )
  {
    if ( !KSUtil::checkArgumentsCount( context, 4, "ACCRINTM", true ) )
    {
      if ( !KSUtil::checkArgumentsCount( context, 3, "ACCRINTM", true ) )
        return false;
    }
    else
    {
      if ( !KSUtil::checkType( context, args[3], KSValue::DoubleType, true ) )
        return false;

      par   = args[3]->doubleValue();
    }
  }
  else
  {
    if ( !KSUtil::checkType( context, args[3], KSValue::DoubleType, true ) )
      return false;
    if ( !KSUtil::checkType( context, args[4], KSValue::IntType, true ) )
      return false;

    par   = args[3]->doubleValue();
    basis = args[4]->intValue();
  }

  QDate issue;
  QDate maturity;
  double rate;

  if ( !getDate( context, args[0], issue ) )
    return false;

  if ( !getDate( context, args[1], maturity ) )
    return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;

  rate = args[2]->doubleValue();

  double d = daysBetweenDates( issue, maturity, basis );
  double y = daysPerYear( issue, basis );

  if ( d < 0 || y <= 0 || par <= 0 || rate <= 0 || basis < 0 || basis > 4 )
    return false;

  context.setValue( new KSValue( par * rate * d / y ) );
  return true;
}

// Function: DISC
bool kspreadfunc_disc( KSContext& context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  int basis = 0;

  if ( !KSUtil::checkArgumentsCount( context, 5, "DISC", true ) )
  {
    if ( !KSUtil::checkArgumentsCount( context, 4, "DISC", true ) )
      return false;
  }
  else
  {
    if ( !KSUtil::checkType( context, args[4], KSValue::IntType, true ) )
      return false;

    basis = args[4]->intValue();
  }

  QDate settlement;
  QDate maturity;

  if ( !getDate( context, args[0], settlement ) )
    return false;

  if ( !getDate( context, args[1], maturity ) )
    return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[3], KSValue::DoubleType, true ) )
    return false;

  double par    = args[2]->doubleValue();
  double redemp = args[3]->doubleValue();

  double y = daysPerYear( settlement, basis );
  double d = daysBetweenDates( settlement, maturity, basis );

  if ( y <= 0 || d <= 0 || basis < 0 || basis > 4 || redemp == 0 )
    return false;

  context.setValue( new KSValue( ( redemp - par ) / redemp * ( y / d ) ) );
  return true;
}


// Function: TBILLPRICE
bool kspreadfunc_tbillprice( KSContext& context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "TBILLPRICE", true ) )
    return false;

  QDate settlement;
  QDate maturity;
  double discount;

  if ( !getDate( context, args[0], settlement ) )
    return false;

  if ( !getDate( context, args[1], maturity ) )
    return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;

  discount = args[2]->doubleValue();

  double days = settlement.daysTo( maturity );

  if ( settlement > maturity || discount < 0 || days > 265 )
    return false;

  context.setValue( new KSValue( 100 * ( 1.0 - ( discount * days ) / 360.0 ) ) );
  return true;
}

// Function: TBILLYIELD
bool kspreadfunc_tbillyield( KSContext& context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "TBILLYIELD", true ) )
    return false;

  QDate settlement;
  QDate maturity;
  double rate;

  if ( !getDate( context, args[0], settlement ) )
    return false;

  if ( !getDate( context, args[1], maturity ) )
    return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;

  rate = args[2]->doubleValue();

  double days = settlement.daysTo( maturity );

  if ( settlement > maturity || rate <= 0 || days > 265 )
    return false;

  context.setValue( new KSValue( ( 100.0 - rate ) / rate * ( 360.0 / days ) ) );
  return true;
}

// Function: TBILLEQ
bool kspreadfunc_tbilleq( KSContext& context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "TBILLEQ", true ) )
    return false;

  QDate settlement;
  QDate maturity;
  double discount;

  if ( !getDate( context, args[0], settlement ) )
    return false;

  if ( !getDate( context, args[1], maturity ) )
    return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;

  discount = args[2]->doubleValue();

  double days = settlement.daysTo( maturity );

  if ( settlement > maturity || discount < 0 || days > 265 )
    return false;

  double divisor = 360.0 - discount * days;

  if ( divisor == 0 )
    return false;

  context.setValue( new KSValue( 365.0 * discount / divisor ) );
  return true;
}

// Function: RECEIVED
bool kspreadfunc_received( KSContext& context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  int basis = 0;

  if ( !KSUtil::checkArgumentsCount( context, 5, "RECEIVED", true ) )
  {
    if ( !KSUtil::checkArgumentsCount( context, 4, "RECEIVED", true ) )
      return false;
  }
  else
  {
    if ( !KSUtil::checkType( context, args[4], KSValue::IntType, true ) )
      return false;

    basis = args[4]->intValue();
  }

  QDate settlement;
  QDate maturity;

  if ( !getDate( context, args[0], settlement ) )
    return false;

  if ( !getDate( context, args[1], maturity ) )
    return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[3], KSValue::DoubleType, true ) )
    return false;

  double investment = args[2]->doubleValue();
  double discount   = args[3]->doubleValue();

  double d = daysBetweenDates( settlement, maturity, basis );
  double y = daysPerYear( settlement, basis );

  if ( d <= 0 || y <= 0 || basis < 0 || basis > 4 )
    return false;

  double x = 1.0 - ( discount * d / y );

  if ( x == 0 )
    return false;

  context.setValue( new KSValue( investment / x ) );
  return true;
}

// Function: DOLLARDE
bool kspreadfunc_dollarde( KSContext& context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "DOLLARDE", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;

  double d = args[0]->doubleValue();
  int f = (int) args[1]->intValue();

  if ( f <= 0 )
    return false;

  int tmp = f;
  int n = 0;
  while ( tmp > 0 )
  {
    tmp /= 10;
    ++n;
  }

  double fl = floor( d );
  double r  = d - fl;

  context.setValue( new KSValue( fl + ( r * pow( 10.0, n ) / (double) f ) ) );
  return true;
}

// Function: DOLLARFR
bool kspreadfunc_dollarfr( KSContext& context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "DOLLARFR", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;

  double d = args[0]->doubleValue();
  int f = (int) args[1]->intValue();

  if ( f <= 0 )
    return false;

  int tmp = f;
  int n = 0;
  while ( tmp > 0 )
  {
    tmp /= 10;
    ++n;
  }

  double fl = floor( d );
  double r  = d - fl;

  context.setValue( new KSValue( fl + ( ( r * (double) f ) / pow( 10.0, n ) ) ) );
  return true;
}

// Function: INTRATE
bool kspreadfunc_intrate( KSContext& context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  int basis = 0;

  if ( !KSUtil::checkArgumentsCount( context, 5, "INTRATE", true ) )
  {
    if ( !KSUtil::checkArgumentsCount( context, 4, "INTRATE", true ) )
      return false;
  }
  else
  {
    if ( !KSUtil::checkType( context, args[4], KSValue::IntType, true ) )
      return false;

    basis = args[4]->intValue();
  }

  QDate settlement;
  QDate maturity;
  double investment;
  double redemption;

  if ( !getDate( context, args[0], settlement ) )
    return false;

  if ( !getDate( context, args[1], maturity ) )
    return false;

  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;

  if ( !KSUtil::checkType( context, args[3], KSValue::DoubleType, true ) )
    return false;

  investment = args[2]->doubleValue();
  redemption = args[3]->doubleValue();

  double d = daysBetweenDates( settlement, maturity, basis );
  double y = daysPerYear( settlement, basis );

  if ( d <= 0 || y <= 0 || investment == 0 || basis < 0 || basis > 4 )
    return false;

  double result = ( redemption -investment ) / investment * ( y / d );

  context.setValue( new KSValue( result ) );
  return true;
}


// Function: DURATION
bool kspreadfunc_duration( KSContext& context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "DURATION", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;

  double rate = args[0]->doubleValue();
  double pv   = args[1]->doubleValue();
  double fv   = args[2]->doubleValue();

  if ( rate <= 0.0 )
    return false;
  if ( fv == 0.0 || pv == 0.0 )
    return false;
  if ( fv / pv < 0 )
    return false;

  context.setValue( new KSValue( log( fv / pv ) / log( 1.0 + rate ) ) );
  return true;
}

// Function: PMT
bool kspreadfunc_pmt( KSContext& context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  int type = -1;
  double fv = -1.0;

  if ( !KSUtil::checkArgumentsCount( context, 5, "PMT", false ) )
  {
    type = 0;

    if ( !KSUtil::checkArgumentsCount( context, 4, "PMT", false ) )
    {
      fv = 0.0;

      if ( !KSUtil::checkArgumentsCount( context, 3, "PMT", true ) )
        return false;
    }
  }

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  if ( fv == -1.0 )
  {
    if ( !KSUtil::checkType( context, args[3], KSValue::DoubleType, true ) )
      return false;
    fv = args[3]->doubleValue();
  }
  if ( type == -1 )
  {
    if ( !KSUtil::checkType( context, args[4], KSValue::IntType, true ) )
      return false;
    type = args[4]->intValue();
  }

  double rate = args[0]->doubleValue();
  double nper = args[1]->doubleValue();
  double pv   = args[2]->doubleValue();

  context.setValue( new KSValue( getPay( rate, nper, pv, fv, type ) ) );
  return true;
}

// Function: NPER
bool kspreadfunc_nper( KSContext& context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  double fv = 0.0;
  int type  = 0;

  if ( KSUtil::checkArgumentsCount( context, 5, "NPER", true ) )
  {
    if ( !KSUtil::checkType( context, args[4], KSValue::IntType, true ) )
      return false;
    if ( !KSUtil::checkType( context, args[3], KSValue::DoubleType, true ) )
      return false;

    fv   = args[3]->doubleValue();
    type = args[4]->intValue();
  }
  else
  if ( KSUtil::checkArgumentsCount( context, 4, "NPER", true ) )
  {
    type = 0;
    if ( !KSUtil::checkType( context, args[3], KSValue::DoubleType, true ) )
      return false;

    fv   = args[3]->doubleValue();
  }
  else
  if ( !KSUtil::checkArgumentsCount( context, 3, "NPER", false ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;

  double rate = args[0]->doubleValue();
  double pmt  = args[1]->doubleValue();
  double pv   = args[2]->doubleValue();

  if ( rate <= 0.0 )
    return false;

  // taken from Gnumeric
  double d  = ( pmt * ( 1.0 + rate * type ) - fv * rate );
  double d2 = pv * rate + pmt * ( 1.0 + rate * type );

  double res = d / d2;

  if ( res <= 0.0 )
    return false;

  context.setValue( new KSValue( log( res ) / log( 1.0 + rate ) ) );
  return true;
}

// Function: ISPMT
bool kspreadfunc_ispmt( KSContext& context )
{
  QValueList<KSValue::Ptr> & args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 4, "ISPMT", false ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::IntType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::IntType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[3], KSValue::DoubleType, true ) )
    return false;

  double rate = args[0]->doubleValue();
  int    per  = args[1]->intValue();
  int    nper = args[2]->intValue();
  double pv   = args[3]->doubleValue();

  if ( per < 1 || per > nper )
    return false;

  double d = -pv * rate;

  context.setValue( new KSValue( d - ( d / nper * per ) ) );
  return true;
}

// Function: IPMT
bool kspreadfunc_ipmt( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  double rate;
  double per;
  double nper;
  double pv;
  double fv = 0.0;
  int type  = 0;

  if ( KSUtil::checkArgumentsCount( context, 6, "IPMT", true ) )
  {
    if ( !KSUtil::checkType( context, args[5], KSValue::IntType, true ) )
      return false;
    if ( !KSUtil::checkType( context, args[4], KSValue::DoubleType, true ) )
      return false;
    type = args[5]->intValue();
    fv   = args[4]->doubleValue();
  }
  else
  if ( KSUtil::checkArgumentsCount( context, 5, "IPMT", true ) )
  {
    if ( !KSUtil::checkType( context, args[4], KSValue::DoubleType, true ) )
      return false;

    fv = args[4]->doubleValue();
  }
  else
  if ( !KSUtil::checkArgumentsCount( context, 4, "IPMT", false ) )
    return false;


  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[3], KSValue::DoubleType, true ) )
    return false;

  rate = args[0]->doubleValue();
  per  = args[1]->doubleValue();
  nper = args[2]->doubleValue();
  pv   = args[3]->doubleValue();

  double payment = getPay( rate, nper, pv, fv, type );
  double ipmt    = -getPrinc( pv, payment, rate, per - 1 );

  context.setValue( new KSValue( ipmt * rate ) );
  return true;
}

// Function: FV
/* Returns future value, given current value, interest rate and time */
bool kspreadfunc_fv( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "FV", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;

  double present = args[0]->doubleValue();
  double interest = args[1]->doubleValue();
  double periods = args[2]->doubleValue();

  context.setValue( new KSValue( present * pow(1+interest, periods)));
  return true;
}

// Function: compound
/* Returns value after compounded interest, given principal, rate, periods
per year and year */
 bool kspreadfunc_compound( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 4, "compound", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[3], KSValue::DoubleType, true ) )
    return false;
  double principal = args[0]->doubleValue();
  double interest = args[1]->doubleValue();
  double periods = args[2]->doubleValue();
  double years = args[3]->doubleValue();

  context.setValue( new KSValue( principal * pow(1+(interest/periods),
periods*years)));

  return true;
}

// Function: continuous
/* Returns value after continuous compounding of interest, given prinicpal,
rate and years */
bool kspreadfunc_continuous( KSContext& context )
{
    // If you still don't understand this, let me know!  ;-)  jsinger@leeta.net
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "continuous", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  double principal = args[0]->doubleValue();
  double interest = args[1]->doubleValue();
  double years = args[2]->doubleValue();


  context.setValue( new KSValue( principal * exp(interest * years)));
  return true;
}

// Function: PV
bool kspreadfunc_pv( KSContext& context )
{
/* Returns presnt value, given future value, interest rate and years */
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "PV", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  double future = args[0]->doubleValue();
  double interest = args[1]->doubleValue();
  double periods = args[2]->doubleValue();


  context.setValue( new KSValue( future / pow(1+interest, periods)));
  return true;
}

// Function: PV_annuity
bool kspreadfunc_pv_annuity( KSContext& context )
{
    /* Returns present value of an annuity or cash flow, given payment,
       interest rate,
       periods, initial amount and whether payments are made at the start (TRUE)
       or end of a period */

    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 3, "PV_annuity", true ) )
	return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
	return false;
    if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
	return false;
    if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
	return false;
    double amount = args[0]->doubleValue();
    double interest = args[1]->doubleValue();
    double periods = args[2]->doubleValue();

    double result;
    result = amount * (1 - 1/(pow( (1+interest), periods ))) / interest ;

  context.setValue( new KSValue( result ) );

  return true;
}

// Function: FV_annnuity
bool kspreadfunc_fv_annuity( KSContext& context )
{
    /* Returns future value of an annuity or cash flow, given payment, interest
       rate and periods */

    QValueList<KSValue::Ptr>& args = context.value()->listValue();

    if ( !KSUtil::checkArgumentsCount( context, 3, "FV_annuity", true ) )
	return false;

    if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
	return false;
    if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
	return false;
    if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
	return false;
    double amount= args[0]->doubleValue();
    double interest = args[1]->doubleValue();
    double periods = args[2]->doubleValue();

    double result;

    result = amount * ((pow( (1+interest),periods))/interest - 1/interest)   ;

    context.setValue( new KSValue( result ) );

    return true;
}

// Function: effective
bool kspreadfunc_effective( KSContext& context )
{
/* Returns effective interest rate given nominal rate and periods per year */

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "effective", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  double nominal = args[0]->doubleValue();
  double periods = args[1]->doubleValue();

  context.setValue( new KSValue(  pow( 1 + (nominal/periods), periods )- 1 ) );

  return true;
}

// Function: zero_coupon
bool kspreadfunc_zero_coupon( KSContext& context )
{
/* Returns effective interest rate given nominal rate and periods per year */

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "zero_coupon", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  double face = args[0]->doubleValue();
  double rate = args[1]->doubleValue();
  double years = args[2]->doubleValue();

  context.setValue( new KSValue(  face / pow( (1 + rate), years )  ) );

  return true;
}

// Function: level_coupon
bool kspreadfunc_level_coupon( KSContext& context )
{
/* Returns effective interest rate given nominal rate and periods per year */

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 5, "level_coupon", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[3], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[4], KSValue::DoubleType, true ) )
    return false;
  double face = args[0]->doubleValue();
  double coupon_rate = args[1]->doubleValue();
  double coupon_year = args[2]->doubleValue();
  double years = args[3]->doubleValue();
  double market_rate = args[4]->doubleValue();

  double coupon = coupon_rate * face / coupon_year;
  double interest =  market_rate/coupon_year;
  double pv_annuity = (1 - 1/(pow( (1+interest), (years*coupon_year) ))) / interest ;
  context.setValue( new KSValue( coupon * pv_annuity + (face/ pow( (1+interest), (years*coupon_year) ) ) ) );

  return true;
}

// Function: nominal
bool kspreadfunc_nominal( KSContext& context )
{
/* Returns nominal interest rate given effective rate and periods per year */

  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 2, "nominal", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  double effective = args[0]->doubleValue();
  double periods = args[1]->doubleValue();

  if ( periods == 0.0 ) // Check null
      return false;

  context.setValue( new KSValue( periods * (pow( (effective + 1), (1 / periods) ) -1) ) );

  return true;
}

// Function: SLN
/* straight-line depreciation for a single period */
bool kspreadfunc_sln( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 3, "SLN", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;

  double cost = args[0]->doubleValue();
  double salvage_value = args[1]->doubleValue();
  double life = args[2]->doubleValue();

  // sentinel check
  if( life <= 0.0 ) return false;

  context.setValue( new KSValue( (cost - salvage_value) / life ) );

  return true;
}

// Function: SYD
/* sum-of-years digits depreciation */
bool kspreadfunc_syd( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 4, "SYD", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[3], KSValue::DoubleType, true ) )
    return false;

  double cost = args[0]->doubleValue();
  double salvage_value = args[1]->doubleValue();
  double life = args[2]->doubleValue();
  double period = args[3]->doubleValue();

  // sentinel check
  if( life <= 0.0 ) return false;

  context.setValue( new KSValue( ( ( (cost - salvage_value) * (life - period + 1) * 2) /
     (life * (life + 1.0) ) ) ) ) ;

  return true;
}

// Function: DB
/* fixed-declining depreciation */
bool kspreadfunc_db( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  double month = 12;

  if( KSUtil::checkArgumentsCount( context, 5, "DB", false ) )
  {
    if( !KSUtil::checkType( context, args[4], KSValue::DoubleType, true ) )
      return false;
    month = args[4]->doubleValue();
  }
  else
  {
    if ( !KSUtil::checkArgumentsCount( context, 4, "DB", true ) )
      return false;
  }

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[3], KSValue::DoubleType, true ) )
    return false;

  double cost = args[0]->doubleValue();
  double salvage = args[1]->doubleValue();
  double life = args[2]->doubleValue();
  double period = args[3]->doubleValue();

  // sentinel check
  if( cost == 0 || life <= 0.0 ) return false;
  if( salvage / cost < 0 ) return false;

  double rate = 1000 * (1 - pow( (salvage/cost), (1/life) ));
  rate = floor( rate + 0.5 )  / 1000;

  double total = cost * rate * month / 12;

  if( period == 1 )
  {
    context.setValue( new KSValue( total ) );
    return true;
  }

  for( int i = 1; i < life; ++i )
    if( i == period-1 )
    {
      context.setValue( new KSValue( rate * (cost-total) ) );
      return true;
    }
    else total += rate * (cost-total);

  context.setValue( new KSValue( (cost-total) * rate * (12-month)/12 ) );

  return true;
}

// Function: DDB
/* depreciation per period */
bool kspreadfunc_ddb( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  double factor = 2;

  if( KSUtil::checkArgumentsCount( context, 5, "DB", false ) )
  {
    if( !KSUtil::checkType( context, args[4], KSValue::DoubleType, true ) )
      return false;

    factor = args[4]->doubleValue();
  }
  else
  {
    if ( !KSUtil::checkArgumentsCount( context, 4, "DB", true ) )
      return false;
  }

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[3], KSValue::DoubleType, true ) )
    return false;

  double cost    = args[0]->doubleValue();
  double salvage = args[1]->doubleValue();
  double life    = args[2]->doubleValue();
  double period  = args[3]->doubleValue();
  double total   = 0.0;

  if ( cost < 0.0 || salvage < 0.0 || life <= 0.0 || period < 0.0 || factor < 0.0 )
    return false;

  for( int i = 0; i < life; ++i )
  {
    double periodDep = ( cost - total ) * ( factor / life );
    if ( i == period - 1 )
    {
      context.setValue( new KSValue( periodDep ) );
      return true;
    }
    else
    {
      total += periodDep;
    }
  }

  context.setValue( new KSValue( cost - total - salvage ) );

  return true;
}

bool kspreadfunc_ppmt( KSContext & context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();
  /*
Docs partly copied from OO.
Syntax
PPMT(Rate;Period;NPER;PV;FV;Type)

Rate is the periodic interest rate.
Period is the amortizement period. P=1 for the first and P=NPER for the last period.
NPER is the total number of periods during which annuity is paid.
PV is the present value in the sequence of payments.
FV (optional) is the desired (future) value.
Type (optional) defines the due date. F=1 for payment at the beginning of a period and F=0 for payment at the end of a period.
  */

  double fv = -1.0;
  int type = -1;

  if ( !KSUtil::checkArgumentsCount( context, 6, "PPMT", false ) )
  {
    type = 0;

    if ( !KSUtil::checkArgumentsCount( context, 5, "PPMT", false ) )
    {
      fv = 0.0;

      if ( !KSUtil::checkArgumentsCount( context, 4, "PPMT", true ) )
        return false;
    }
  }

  if ( !KSUtil::checkType( context, args[0], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[1], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[2], KSValue::DoubleType, true ) )
    return false;
  if ( !KSUtil::checkType( context, args[3], KSValue::DoubleType, true ) )
    return false;
  if ( fv == -1 )
  {
    if ( !KSUtil::checkType( context, args[4], KSValue::DoubleType, true ) )
      return false;
    fv = args[4]->doubleValue();
  }
  if ( type == -1 )
  {
    if ( !KSUtil::checkType( context, args[5], KSValue::IntType, true ) )
      return false;
    type = args[5]->intValue();
  }

  double rate   = args[0]->doubleValue();
  double period = args[1]->doubleValue();
  double nper   = args[2]->doubleValue();
  double pv     = args[3]->doubleValue();

  double pay  = getPay( rate, nper, pv, fv, type );
  double ipmt = -getPrinc( pv, pay, rate, period - 1 ) * rate;

  context.setValue( new KSValue( pay - ipmt ) );

  return true;
}

// Function: EURO
bool kspreadfunc_euro( KSContext& context )
{
  QValueList<KSValue::Ptr>& args = context.value()->listValue();

  if ( !KSUtil::checkArgumentsCount( context, 1, "EURO", true ) )
    return false;

  if ( !KSUtil::checkType( context, args[0], KSValue::StringType, true ) )
    return false;

  QString currency = args[0]->stringValue().upper();
  double result = -1;

  if( currency == "ATS" ) result = 13.7603;  // Austria
  else if( currency == "BEF" ) result = 40.3399;  // Belgium
  else if( currency == "DEM" ) result = 1.95583;  // Germany
  else if( currency == "ESP" ) result = 166.386;  // Spain
  else if( currency == "FIM" ) result = 5.94573;  // Finland
  else if( currency == "FRF" ) result = 6.55957;  // France
  else if( currency == "GRD" ) result = 340.75;   // Greece
  else if( currency == "IEP" ) result = 0.787564; // Ireland
  else if( currency == "ITL" ) result = 1936.27;  // Italy
  else if( currency == "LUX" ) result = 40.3399;  // Luxemburg
  else if( currency == "NLG" ) result = 2.20371;  // Nederland
  else if( currency == "PTE" ) result = 200.482;  // Portugal

  if( result <= 0 ) return false;

  context.setValue( new KSValue( result ) );
  return true;
}
