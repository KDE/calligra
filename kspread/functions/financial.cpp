/* This file is part of the KDE project

   Copyright (C) 2002,2006 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>
   Copyright (C) 2002 Norbert Andres <nandres@web.de>
   Copyright (C) 1999-2000 Laurent Montel <montel@kde.org>
   Copyright (C) 1999-2000 Laurent Montel <montel@kde.org>
   Copyright (C) 1999-2000 Jonathan Singer <jsinger@genome.wi.mit.edu>
   Copyright (C) 1998-1999 Torben Weis <weis@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

// built-in financial functions

#include <math.h>

#include "Functions.h"
#include "functions/helper.h"
#include "ValueCalc.h"
#include "ValueConverter.h"

#include <klocale.h>
#include <kcalendarsystem.h>

using namespace KSpread;

// prototypes (sorted)
Value func_accrint (valVector args, ValueCalc *calc, FuncExtra *);
Value func_accrintm (valVector args, ValueCalc *calc, FuncExtra *);
Value func_compound (valVector args, ValueCalc *calc, FuncExtra *);
Value func_continuous (valVector args, ValueCalc *calc, FuncExtra *);
Value func_coupnum (valVector args, ValueCalc *calc, FuncExtra *);
Value func_cumipmt (valVector args, ValueCalc *calc, FuncExtra *);
Value func_cumprinc (valVector args, ValueCalc *calc, FuncExtra *);
Value func_db (valVector args, ValueCalc *calc, FuncExtra *);
Value func_ddb (valVector args, ValueCalc *calc, FuncExtra *);
Value func_disc (valVector args, ValueCalc *calc, FuncExtra *);
Value func_dollarde (valVector args, ValueCalc *calc, FuncExtra *);
Value func_dollarfr (valVector args, ValueCalc *calc, FuncExtra *);
Value func_duration (valVector args, ValueCalc *calc, FuncExtra *);
Value func_effective (valVector args, ValueCalc *calc, FuncExtra *);
Value func_euro (valVector args, ValueCalc *calc, FuncExtra *);
Value func_euroconvert (valVector args, ValueCalc *calc, FuncExtra *);
Value func_fv (valVector args, ValueCalc *calc, FuncExtra *);
Value func_fv_annuity (valVector args, ValueCalc *calc, FuncExtra *);
Value func_intrate (valVector args, ValueCalc *calc, FuncExtra *);
Value func_ipmt (valVector args, ValueCalc *calc, FuncExtra *);
Value func_ispmt (valVector args, ValueCalc *calc, FuncExtra *);
Value func_level_coupon (valVector args, ValueCalc *calc, FuncExtra *);
Value func_nominal (valVector args, ValueCalc *calc, FuncExtra *);
Value func_nper (valVector args, ValueCalc *calc, FuncExtra *);
Value func_pmt (valVector args, ValueCalc *calc, FuncExtra *);
Value func_ppmt (valVector args, ValueCalc *calc, FuncExtra *);
Value func_pv (valVector args, ValueCalc *calc, FuncExtra *);
Value func_pv_annuity (valVector args, ValueCalc *calc, FuncExtra *);
Value func_received (valVector args, ValueCalc *calc, FuncExtra *);
Value func_sln (valVector args, ValueCalc *calc, FuncExtra *);
Value func_syd (valVector args, ValueCalc *calc, FuncExtra *);
Value func_tbilleq (valVector args, ValueCalc *calc, FuncExtra *);
Value func_tbillprice (valVector args, ValueCalc *calc, FuncExtra *);
Value func_tbillyield (valVector args, ValueCalc *calc, FuncExtra *);
Value func_zero_coupon (valVector args, ValueCalc *calc, FuncExtra *);

// registers all financial functions
void RegisterFinancialFunctions()
{
  FunctionRepository* repo = FunctionRepository::self();
  Function *f;

  f = new Function ("ACCRINT", func_accrint);
  f->setParamCount (6, 7);
  repo->add (f);
  f = new Function ("ACCRINTM", func_accrintm);
  f->setParamCount (3, 5);
  repo->add (f);
  f = new Function ("COMPOUND", func_compound);
  f->setParamCount (4);
  repo->add (f);
  f = new Function ("CONTINUOUS", func_continuous);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("COUPNUM", func_coupnum);
  f->setParamCount (3, 5);
  repo->add (f);
  f = new Function ("CUMIPMT", func_cumipmt);
  f->setParamCount (6);
  repo->add (f);
  f = new Function ("CUMPRINC", func_cumprinc);
  f->setParamCount (6);
  repo->add (f);
  f = new Function ("DB", func_db);
  f->setParamCount (4, 5);
  repo->add (f);
  f = new Function ("DDB", func_ddb);
  f->setParamCount (4, 5);
  repo->add (f);
  f = new Function ("DISC", func_disc);
  f->setParamCount (4, 5);
  repo->add (f);
  f = new Function ("DOLLARDE", func_dollarde);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("DOLLARFR", func_dollarfr);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("DURATION", func_duration);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("EFFECT", func_effective);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("EFFECTIVE", func_effective);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("EURO", func_euro);  // KSpread-specific, Gnumeric-compatible
  f->setParamCount (1);
  repo->add (f);
  f = new Function ("EUROCONVERT", func_euroconvert); 
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("FV", func_fv);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("FV_ANNUITY", func_fv_annuity);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("INTRATE", func_intrate);
  f->setParamCount (4, 5);
  repo->add (f);
  f = new Function ("IPMT", func_ipmt);
  f->setParamCount (4, 6);
  repo->add (f);
  f = new Function ("ISPMT", func_ispmt);
  f->setParamCount (4);
  repo->add (f);
  f = new Function ("LEVEL_COUPON", func_level_coupon);
  f->setParamCount (5);
  repo->add (f);
  f = new Function ("NOMINAL", func_nominal);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("NPER", func_nper);
  f->setParamCount (3, 5);
  repo->add (f);
  f = new Function ("PMT", func_pmt);
  f->setParamCount (3, 5);
  repo->add (f);
  f = new Function ("PPMT", func_ppmt);
  f->setParamCount (4, 6);
  repo->add (f);
  f = new Function ("PV", func_pv);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("PV_ANNUITY", func_pv_annuity);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("RECEIVED", func_received);
  f->setParamCount (4, 5);
  repo->add (f);
  f = new Function ("SLN", func_sln);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("SYD", func_syd);
  f->setParamCount (4);
  repo->add (f);
  f = new Function ("TBILLEQ", func_tbilleq);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("TBILLPRICE", func_tbillprice);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("TBILLYIELD", func_tbillyield);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("ZERO_COUPON", func_zero_coupon);
  f->setParamCount (3);
  repo->add (f);
}

static Value getPay (ValueCalc *calc, Value rate,
    Value nper, Value pv, Value fv, Value type)
{
  Value pvif, fvifa;

  if (calc->isZero (rate)) return Value::errorVALUE();

  //pvif  = pow( 1 + rate, nper );
  //fvifa = ( pvif - 1 ) / rate;
  pvif = calc->pow (calc->add (rate, 1), nper);
  fvifa = calc->div (calc->sub (pvif, 1), rate);

  // ( -pv * pvif - fv ) / ( ( 1.0 + rate * type ) * fvifa );
  Value val1 = calc->sub (calc->mul (calc->mul (Value(-1), pv), pvif), fv);
  Value val2 = calc->mul (calc->add (Value(1.0), calc->mul (rate, type)),
      fvifa);
  return calc->div (val1, val2);
}

static Value getPrinc (ValueCalc *calc, Value start,
    Value pay, Value rate, Value period)
{
  // val1 = pow( 1 + rate, period )
  Value val1 = calc->pow (calc->add (rate, 1), period);
  // val2 = start * val1
  Value val2 = calc->mul (start, val1);
  // val3 = pay * ( ( val1 - 1 ) / rate )
  Value val3 = calc->mul (pay, calc->div (calc->sub (val1, Value(1)), rate));
  // result = val2 + val3
  return calc->add (val2, val3);
}

// helper for IPMT and CUMIPMT
static Value helper_ipmt( ValueCalc* calc, Value rate, Value per, Value nper, Value pv, Value fv, Value type )
{
    const Value payment = getPay (calc, rate, nper, pv, fv, type);
    const Value ineg = getPrinc (calc, pv, payment, rate, calc->sub (per, Value(1)));
    // -ineg * rate
    return calc->mul (calc->mul (ineg, Value(-1)), rate);
}

// Function: COUPNUM - taken from GNUMERIC
Value func_coupnum (valVector args, ValueCalc *calc, FuncExtra *)
{
  // dates and integers only - don't need high-precision for this
  QDate settlement = calc->conv()->asDate (args[0]).asDate( calc->doc() );
  QDate maturity = calc->conv()->asDate (args[1]).asDate( calc->doc() );
  int   frequency = calc->conv()->asInteger (args[2]).asInteger();
  int   basis = 0;
  bool  eom   = true;
  if (args.count() > 3)
    basis = calc->conv()->asInteger (args[3]).asInteger();
  if (args.count() == 5)
    eom = calc->conv()->asBoolean (args[4]).asBoolean();

  if (basis < 0 || basis > 5 || ( frequency == 0 ) || ( 12 % frequency != 0 )
      || settlement.daysTo( maturity ) <= 0)
    return Value::errorVALUE();

  double result;
  QDate cDate( maturity );

  int months = maturity.month() - settlement.month()
    + 12 * ( maturity.year() - settlement.year() );

  cDate = calc->conv()->locale()->calendar()->addMonths (cDate, -months);

  if ( eom && maturity.daysInMonth() == maturity.day() )
  {
    while( cDate.daysInMonth() != cDate.day() )
      cDate.addDays( 1 );
  }

  if ( settlement.day() >= cDate.day() )
    --months;

  result = ( 1 + months / ( 12 / frequency ) );

  return Value (result);
}

// Function: ACCRINT
Value func_accrint (valVector args, ValueCalc *calc, FuncExtra *)
{
  QDate maturity = calc->conv()->asDate (args[0]).asDate( calc->doc() );
  QDate firstInterest = calc->conv()->asDate (args[1]).asDate( calc->doc() );
  QDate settlement = calc->conv()->asDate (args[2]).asDate( calc->doc() );

  Value rate = args[3];
  Value par = args[4];
  int frequency = calc->conv()->asInteger (args[5]).asInteger();

  int basis = 0;
  if (args.count() == 7)
    basis = calc->conv()->asInteger (args[6]).asInteger();

  if ( basis < 0 || basis > 4 || (calc->isZero (Value(frequency))) ||
      (12 % frequency != 0))
    return Value::errorVALUE();

  if ( ( settlement.daysTo( firstInterest ) < 0 )
       || ( firstInterest.daysTo( maturity ) > 0 ) )
    return Value::errorVALUE();

  double d = daysBetweenDates (maturity, settlement, basis);
  double y = daysPerYear (maturity, basis);

  if ( d < 0 || y <= 0 || calc->lower (par, Value(0)) || calc->lower (rate, Value(0)) ||
      calc->isZero (rate))
    return Value::errorVALUE();

  Value coeff = calc->div (calc->mul (par, rate), frequency);
  double n = d / y;

  return calc->mul (coeff, n * frequency);
}

// Function: ACCRINTM
Value func_accrintm (valVector args, ValueCalc *calc, FuncExtra *)
{
  QDate issue = calc->conv()->asDate (args[0]).asDate( calc->doc() );
  QDate maturity = calc->conv()->asDate (args[1]).asDate( calc->doc() );
  Value rate = args[2];

  Value par = Value(1000);
  int basis = 0;
  if (args.count() > 3)
    par = args[3];
  if (args.count() == 5)
    basis = calc->conv()->asInteger (args[4]).asInteger ();

  double d = daysBetweenDates (issue, maturity, basis);
  double y = daysPerYear (issue, basis);

  if (d < 0 || y <= 0 || calc->isZero (par) || calc->isZero (rate) ||
      calc->lower (par, Value(0)) || calc->lower (rate, Value(0)) || basis < 0 || basis > 4)
    return Value::errorVALUE();

  // par*date * d/y
  return calc->mul (calc->mul (par, rate), d / y);
}

// Function: CUMIPMT
Value func_cumipmt(valVector args, ValueCalc *calc, FuncExtra *)
{
    const Value rate = args[0];
    if ( rate.asFloat() <= 0.0 )
        return Value::errorVALUE();
    const Value nper = args[1];
    const int periods = nper.asInteger();
    if ( periods <= 0 )
        return Value::errorVALUE();
    const Value pv = args[2];
    if ( pv.asFloat() <= 0.0 )
        return Value::errorVALUE();
    const Value v1( calc->conv()->asInteger(args[3]) );
    if ( v1.isError() )
        return Value::errorVALUE();
    const int start = v1.asInteger();
    if ( start <= 0 || start > periods )
        return Value::errorVALUE();
    const Value v2( calc->conv()->asInteger(args[4]) );
    if ( v2.isError() )
        return Value::errorVALUE();
    const int end = v2.asInteger();
    if ( end < start || end > periods )
        return Value::errorVALUE();
    const Value type( calc->conv()->asInteger(args[5]) );
    if ( type.isError() )
        return Value::errorVALUE();

    Value result(0.0);
    for ( int per = start; per <= end; ++per )
        result = calc->add( result, helper_ipmt( calc, rate, Value(per), nper, pv, Value(0.0), type ) );
    return result;
}

// Function: CUMPRINC
Value func_cumprinc(valVector args, ValueCalc *calc, FuncExtra *)
{
    const Value rate = args[0];
    if ( rate.asFloat() <= 0.0 )
        return Value::errorVALUE();
    const Value nper = args[1];
    const int periods = nper.asInteger();
    if ( periods <= 0 )
        return Value::errorVALUE();
    const Value pv = args[2];
    if ( pv.asFloat() <= 0.0 )
        return Value::errorVALUE();
    const Value v1( calc->conv()->asInteger(args[3]) );
    if ( v1.isError() )
        return Value::errorVALUE();
    const int start = v1.asInteger();
    if ( start <= 0 || start > periods )
        return Value::errorVALUE();
    const Value v2( calc->conv()->asInteger(args[4]) );
    if ( v2.isError() )
        return Value::errorVALUE();
    const int end = v2.asInteger();
    if ( end < start || end > periods )
        return Value::errorVALUE();
    const Value type( calc->conv()->asInteger(args[5]) );
    if ( type.isError() )
        return Value::errorVALUE();

    const Value pay = getPay(calc, rate, nper, pv, Value(0.0), type);
    const Value cumipmt = func_cumipmt(args, calc, 0);
    return calc->sub( calc->mul(pay, Value(end-start+1)), cumipmt);
}

// Function: DISC
Value func_disc (valVector args, ValueCalc *calc, FuncExtra *)
{
  QDate settlement = calc->conv()->asDate (args[0]).asDate( calc->doc() );
  QDate maturity = calc->conv()->asDate (args[1]).asDate( calc->doc() );

  Value par = args[2];
  Value redemp = args[3];

  int basis = 0;
  if (args.count() == 5)
    basis = calc->conv()->asInteger (args[4]).asInteger();

  double y = daysPerYear (settlement, basis);
  double d = daysBetweenDates (settlement, maturity, basis);

  if ( y <= 0 || d <= 0 || basis < 0 || basis > 4 || calc->isZero (redemp) )
    return Value(false);

  // (redemp - par) / redemp * (y / d)
  return calc->mul (calc->div (calc->sub (redemp, par), redemp), y / d);
}


// Function: TBILLPRICE
Value func_tbillprice (valVector args, ValueCalc *calc, FuncExtra *)
{
  QDate settlement = calc->conv()->asDate (args[0]).asDate( calc->doc() );
  QDate maturity = calc->conv()->asDate (args[1]).asDate( calc->doc() );

  Value discount = args[2];

  double days = settlement.daysTo( maturity );

  if (settlement > maturity || calc->lower (discount, Value(0)) || days > 265)
    return Value::errorVALUE();

  // (discount * days) / 360.0
  Value val = calc->div (calc->mul (discount, days), 360.0);
  // 100 * (1.0 - val);
  return calc->mul (calc->sub (Value(1.0), val), Value(100));
}

// Function: TBILLYIELD
Value func_tbillyield (valVector args, ValueCalc *calc, FuncExtra *)
{
  QDate settlement = calc->conv()->asDate (args[0]).asDate( calc->doc() );
  QDate maturity = calc->conv()->asDate (args[1]).asDate( calc->doc() );

  Value rate = args[2];

  double days = settlement.daysTo( maturity );

  if (settlement > maturity || calc->isZero (rate) || calc->lower (rate, Value(0))
      || days > 265)
    return Value::errorVALUE();

  // (100.0 - rate) / rate * (360.0 / days);
  return calc->mul (calc->div (calc->sub (Value(100.0), rate), rate), 360.0 / days);
}

// Function: TBILLEQ
Value func_tbilleq (valVector args, ValueCalc *calc, FuncExtra *)
{
  QDate settlement = calc->conv()->asDate (args[0]).asDate( calc->doc() );
  QDate maturity = calc->conv()->asDate (args[1]).asDate( calc->doc() );

  Value discount = args[2];

  double days = settlement.daysTo( maturity );

  if (settlement > maturity || calc->lower (discount, Value(0)) || days > 265)
    return Value::errorVALUE();

  // 360 - discount*days
  Value divisor = calc->sub (Value(360.0), calc->mul (discount, days));
  if (calc->isZero (divisor))
    return Value::errorVALUE();

  // 365.0 * discount / divisor
  return calc->mul (calc->div (discount, divisor), Value(356.0));
}

// Function: RECEIVED
Value func_received (valVector args, ValueCalc *calc, FuncExtra *)
{

  QDate settlement = calc->conv()->asDate (args[0]).asDate( calc->doc() );
  QDate maturity = calc->conv()->asDate (args[1]).asDate( calc->doc() );

  Value investment = args[2];
  Value discount = args[3];

  int basis = 0;
  if (args.count() == 5)
    basis = calc->conv()->asInteger (args[4]).asInteger();

  double d = daysBetweenDates( settlement, maturity, basis );
  double y = daysPerYear( settlement, basis );

  if ( d <= 0 || y <= 0 || basis < 0 || basis > 4 )
    return Value(false);

  // 1.0 - ( discount * d / y )
  Value x = calc->sub (Value(1.0), (calc->mul (discount, d / y)));

  if (calc->isZero (x))
    return Value::errorVALUE();
  return calc->div (investment, x);
}

// Function: DOLLARDE
Value func_dollarde (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value d = args[0];
  Value f = args[1];

  if (!calc->greater (f, Value(0)))
    return Value::errorVALUE();

  Value tmp = f;
  int n = 0;
  while (calc->greater (tmp, Value(0)))
  {
    tmp = calc->div (tmp, Value(10));
    ++n;
  }

  Value fl = calc->roundDown (d);
  Value r = calc->sub (d, fl);

  // fl + (r * pow(10.0, n) / f)
  return calc->add (fl, calc->div (calc->mul (r, pow (10.0, n)), f));
}

// Function: DOLLARFR
Value func_dollarfr (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value d = args[0];
  Value f = args[1];

  if (!calc->greater (f, Value(0)))
    return Value::errorVALUE();

  Value tmp = f;
  int n = 0;
  while (calc->greater (tmp, Value(0)))
  {
    tmp = calc->div (tmp, 10);
    ++n;
  }

  Value fl = calc->roundDown (d);
  Value r = calc->sub (d, fl);

  // fl + ((r * f) / pow (10.0, n));
  return calc->add (fl, calc->div (calc->mul (r, f), Value(pow (10.0, n))));
}

/// *** TODO continue here ***

// Function: INTRATE
Value func_intrate (valVector args, ValueCalc *calc, FuncExtra *)
{
  QDate settlement = calc->conv()->asDate (args[0]).asDate( calc->doc() );
  QDate maturity = calc->conv()->asDate (args[1]).asDate( calc->doc() );

  Value invest = args[2];
  Value redemption = args[3];

  int basis = 0;
  if (args.count() == 5)
    basis = calc->conv()->asInteger (args[4]).asInteger();

  double d = daysBetweenDates (settlement, maturity, basis);
  double y = daysPerYear (settlement, basis);

  if ( d <= 0 || y <= 0 || calc->isZero (invest) || basis < 0 || basis > 4 )
    return Value::errorVALUE();

  // (redemption - invest) / invest * (y / d)
  return calc->mul (calc->div (calc->sub (redemption, invest), invest), y/d);
}


// Function: DURATION
Value func_duration (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value rate = args[0];
  Value pv   = args[1];
  Value fv   = args[2];

  if (!calc->greater (rate, Value(0.0)))
    return Value::errorVALUE();
  if (calc->isZero (fv) || calc->isZero (pv))
    return Value::errorDIV0();

  if (calc->lower (calc->div (fv, pv), Value(0)))
    return Value::errorVALUE();

  // log(fv / pv) / log(1.0 + rate)
  return calc->div (calc->ln (calc->div (fv, pv)),
      calc->ln (calc->add (rate, Value(1.0))));
}

// Function: PMT
Value func_pmt (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value rate = args[0];
  Value nper = args[1];
  Value pv   = args[2];
  Value fv = Value(0.0);
  Value type = Value(0);
  if (args.count() > 3) fv = args[3];
  if (args.count() == 5) type = args[4];

  return getPay (calc, rate, nper, pv, fv, type);
}

// Function: NPER
Value func_nper (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value rate = args[0];
  Value pmt  = args[1];
  Value pv   = args[2];
  Value fv = Value(0.0);
  Value type = Value(0);
  if (args.count() > 3) fv = args[3];
  if (args.count() == 5) type = args[4];

  if (!calc->greater (rate, Value(0.0)))
    return Value::errorVALUE();

  // taken from Gnumeric
  // v = 1.0 + rate * type
  // d1 = pmt * v - fv * rate
  // d2 = pmt * v - pv * rate
  // res = d1 / d2;
  Value v = calc->add (calc->mul (rate, type), 1.0);
  Value d1 = calc->sub (calc->mul (pmt, v), calc->mul (fv, rate));
  Value d2 = calc->add (calc->mul (pmt, v), calc->mul (pv, rate));
  Value res = calc->div (d1, d2);

  if (!calc->greater (res, Value(0.0)))  // res must be >0
    return Value::errorVALUE();

  // ln (res) / ln (rate + 1.0)
  return calc->div (calc->ln (res), calc->ln (calc->add (rate, Value(1.0))));
}

// Function: ISPMT
Value func_ispmt (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value rate = args[0];
  Value per  = args[1];
  Value nper = args[2];
  Value pv   = args[3];

  if (calc->lower (per, Value(1)) || calc->greater (per, nper))
    return Value::errorVALUE();

  // d = -pv * rate
  Value d = calc->mul (calc->mul (pv, Value(-1)), rate);

  // d - (d / nper * per)
  return calc->sub (d, calc->mul (calc->div (d, nper), per));
}

// Function: IPMT
Value func_ipmt (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value rate = args[0];
  Value per  = args[1];
  Value nper = args[2];
  Value pv   = args[3];

  Value fv = Value(0.0);
  Value type = Value(0);
  if (args.count() > 4) fv = args[4];
  if (args.count() == 6) type = args[5];

  return helper_ipmt( calc, rate, per, nper, pv, fv, type);
}

// Function: PPMT
// Uses IPMT.
Value func_ppmt (valVector args, ValueCalc *calc, FuncExtra *)
{
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

  Value rate = args[0];
  Value per  = args[1];
  Value nper = args[2];
  Value pv   = args[3];
  Value fv = Value(0.0);
  Value type = Value(0);
  if (args.count() > 4) fv = args[4];
  if (args.count() == 6) type = args[5];

  Value pay  = getPay (calc, rate, nper, pv, fv, type);
  Value ipmt = func_ipmt (args, calc, 0);
  return calc->sub (pay, ipmt);
}

// Function: FV
/* Returns future value, given current value, interest rate and time */
Value func_fv (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value present = args[0];
  Value interest = args[1];
  Value periods = args[2];

  // present * pow (1 + interest, periods)
  return calc->mul (present, calc->pow (calc->add (interest, 1), periods));
}

// Function: compound
/* Returns value after compounded interest, given principal, rate, periods
per year and year */
 Value func_compound (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value principal = args[0];
  Value interest = args[1];
  Value periods = args[2];
  Value years = args[3];

  // principal * pow(1+ (interest / periods), periods*years);
  Value base = calc->add (calc->div (interest, periods), 1);
  return calc->mul (principal, calc->pow (base, calc->mul (periods, years)));
}

// Function: continuous
/* Returns value after continuous compounding of interest, given principal,
rate and years */
Value func_continuous (valVector args, ValueCalc *calc, FuncExtra *)
{
  // If you still don't understand this, let me know!  ;-)  jsinger@leeta.net
  Value principal = args[0];
  Value interest = args[1];
  Value years = args[2];

  // principal * exp(interest * years)
  return calc->mul (principal, calc->exp (calc->mul (interest, years)));
}

// Function: PV
Value func_pv (valVector args, ValueCalc *calc, FuncExtra *)
{
/* Returns presnt value, given future value, interest rate and years */
  Value future = args[0];
  Value interest = args[1];
  Value periods = args[2];

  // future / pow(1+interest, periods)
  return calc->div (future, calc->pow (calc->add (interest, 1), periods));
}

// Function: PV_annuity
Value func_pv_annuity (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value amount = args[0];
  Value interest = args[1];
  Value periods = args[2];

  // recpow = 1 / pow (1 + interest, periods)
  // result = amount * (1 - recpow) / interest;
  Value recpow;
  recpow = calc->div (Value(1), calc->pow (calc->add (interest, Value(1)), periods));
  return calc->mul (amount, calc->div (calc->sub (Value(1), recpow), interest));
}

// Function: FV_annnuity
Value func_fv_annuity (valVector args, ValueCalc *calc, FuncExtra *)
{
  /* Returns future value of an annuity or cash flow, given payment, interest
     rate and periods */

  Value amount = args[0];
  Value interest = args[1];
  Value periods = args[2];

  // pw = pow (1 + interest, periods)
  // result = amount * ((pw - 1) / interest)
  Value pw = calc->pow (calc->add (interest, 1), periods);
  return calc->mul (amount, calc->div (calc->sub (pw, 1), interest));
}

// Function: effective
Value func_effective (valVector args, ValueCalc *calc, FuncExtra *)
{
  // Returns effective interest rate given nominal rate and periods per year

  Value nominal = args[0];
  Value periods = args[1];

  // base = 1 + (nominal / periods)
  // result = pow (base, periods) - 1
  Value base = calc->add (calc->div (nominal, periods), 1);
  return calc->sub (calc->pow (base, periods), 1);
}

// Function: zero_coupon
Value func_zero_coupon (valVector args, ValueCalc *calc, FuncExtra *)
{
  // Returns effective interest rate given nominal rate and periods per year

  Value face = args[0];
  Value rate = args[1];
  Value years = args[2];

  // face / pow(1 + rate, years)
  return calc->div (face, calc->pow (calc->add (rate, 1), years));
}

// Function: level_coupon
Value func_level_coupon (valVector args, ValueCalc *calc, FuncExtra *)
{
  // Returns effective interest rate given nominal rate and periods per year
  Value face = args[0];
  Value coupon_rate = args[1];
  Value coupon_year = args[2];
  Value years = args[3];
  Value market_rate = args[4];

  Value coupon, interest, pw, pv_annuity;
  // coupon = coupon_rate * face / coupon_year
  // interest = market_rate / coupon_year
  // pw = pow(1 + interest, years * coupon_year)
  // pv_annuity = (1 - 1 / pw) / interest
  // result = coupon * pv_annuity + face / pw
  coupon = calc->mul (coupon_rate, calc->div (face, coupon_year));
  interest = calc->div (market_rate, coupon_year);
  pw = calc->pow (calc->add (interest, Value(1)), calc->mul (years, coupon_year));
  pv_annuity = calc->div (calc->sub (Value(1), calc->div (Value(1), pw)), interest);
  return calc->add (calc->mul (coupon, pv_annuity), calc->div (face, pw));
}

// Function: nominal
Value func_nominal (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value effective = args[0];
  Value periods = args[1];
  
  // sentinel checks
  if (calc->isZero (periods))
    return Value::errorDIV0();
  if (!calc->greater (periods, Value(0.0)))
    return Value::errorVALUE();
  if (calc->isZero (effective))
    return Value::errorVALUE();
  if(!calc->greater (effective, Value(0.0)))
    return Value::errorVALUE();
    
  // pw = pow (effective + 1, 1 / periods)
  // result = periods * (pw - 1);
  Value pw;
  pw = calc->pow (calc->add (effective, Value(1)), calc->div (Value(1), periods));
  return calc->mul (periods, calc->sub (pw, Value(1)));
}

// Function: SLN
/* straight-line depreciation for a single period */
Value func_sln (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value cost = args[0];
  Value salvage_value = args[1];
  Value life = args[2];

  // sentinel check
  if (!calc->greater (life, Value(0.0)))
    return Value::errorVALUE();

  // (cost - salvage_value) / life
  return calc->div (calc->sub (cost, salvage_value), life);
}

// Function: SYD
/* sum-of-years digits depreciation */
Value func_syd (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value cost = args[0];
  Value salvage_value = args[1];
  Value life = args[2];
  Value period = args[3];

  // sentinel check
  if (!calc->greater (life, Value(0.0)))
    return Value::errorVALUE();

  // v1 = cost - salvage_value
  // v2 = life - period + 1
  // v3 = life * (life + 1.0)
  // result = (v1 * v2 * 2) / v3
  Value v1, v2, v3;
  v1 = calc->sub (cost, salvage_value);
  v2 = calc->add (calc->sub (life, period), 1);
  v3 = calc->mul (life, calc->add (life, 1.0));
  return calc->div (calc->mul (calc->mul (v1, v2), 2), v3);
}

// Function: DB
/* fixed-declining depreciation */
Value func_db (valVector args, ValueCalc *calc, FuncExtra *)
{
  // This function doesn't support extended datatypes, it simply
  // converts everything to double - because it does quite a bit
  // of computing, and, well, I'm lazy to convert it all (Tomas)
  double cost = calc->conv()->asFloat (args[0]).asFloat();
  double salvage = calc->conv()->asFloat (args[1]).asFloat();
  double life = calc->conv()->asFloat (args[2]).asFloat();
  double period = calc->conv()->asFloat (args[3]).asFloat();
  double month = 12;
  if (args.count() == 5)
    month = calc->conv()->asFloat (args[4]).asFloat();

  // sentinel check
  if (cost == 0 || life <= 0.0 || period == 0)
    return Value::errorNUM ();

  if (calc->lower (calc->div (Value(salvage), Value(cost)), Value(0)))
    return Value::errorNUM ();

  double rate = 1000 * (1 - pow( (salvage/cost), (1/life) ));
  rate = floor( rate + 0.5 )  / 1000;

  double total = cost * rate * month / 12;

  if( period == 1 )
    return Value (total);

  for (int i = 1; i < life; ++i)
    if (i == period - 1)
      return Value (rate * (cost-total));
    else total += rate * (cost-total);

  return Value ((cost-total) * rate * (12-month)/12);
}

// Function: DDB
/* depreciation per period */
Value func_ddb (valVector args, ValueCalc *calc, FuncExtra *)
{
  double cost = calc->conv()->asFloat (args[0]).asFloat();
  double salvage = calc->conv()->asFloat (args[1]).asFloat();
  double life = calc->conv()->asFloat (args[2]).asFloat();
  double period = calc->conv()->asFloat (args[3]).asFloat();
  double factor = 2;
  if (args.count() == 5)
    factor = calc->conv()->asFloat (args[4]).asFloat();

  if ( cost < 0.0 || salvage < 0.0 || life <= 0.0 || period < 0.0 || factor < 0.0 )
    return Value::errorVALUE();

  double result = 0.0;

  // depreciation is the value between two periods
  double invrate = (life-factor)/life;
  double current = (period==1) ? invrate : pow (invrate, period);
  double previous = current / invrate;
  current *= cost;
  previous *= cost;
  result = previous - current;

  // should not be more than the salvage
  if(current < salvage)
    result = previous - salvage;

  // can't be negative
  if(result < 0.0)
    result = 0.0;
    
  return Value(result);
}


static double helper_eurofactor(const QString& currency)
{
  QString cur = currency.toUpper();
  double result = -1;

  if( cur == "ATS" ) result = 13.7603;       // Austria
  else if( cur == "BEF" ) result = 40.3399;  // Belgium
  else if( cur == "DEM" ) result = 1.95583;  // Germany
  else if( cur == "ESP" ) result = 166.386;  // Spain
  else if( cur == "EUR" ) result = 1.0;      // Euro
  else if( cur == "FIM" ) result = 5.94573;  // Finland
  else if( cur == "FRF" ) result = 6.55957;  // France
  else if( cur == "GRD" ) result = 340.75;   // Greece
  else if( cur == "IEP" ) result = 0.787564; // Ireland
  else if( cur == "ITL" ) result = 1936.27;  // Italy
  else if( cur == "LUX" ) result = 40.3399;  // Luxembourg
  else if( cur == "NLG" ) result = 2.20371;  // Netherlands
  else if( cur == "PTE" ) result = 200.482;  // Portugal
  
  return result;
} 

// Function: EURO
Value func_euro (valVector args, ValueCalc *calc, FuncExtra *)
{
  QString currency = calc->conv()->asString (args[0]).asString();
  double result = helper_eurofactor(currency);
  if( result < 0 )
    return Value::errorNUM();

  return Value (result);
}

// EUROCONVERT(number,source,target)
Value func_euroconvert (valVector args, ValueCalc *calc, FuncExtra *)
{
  double number = calc->conv()->asFloat (args[0]).asFloat();
  QString source = calc->conv()->asString (args[1]).asString();
  QString target = calc->conv()->asString (args[2]).asString();
  
  double factor1 = helper_eurofactor(source);
  double factor2 = helper_eurofactor(target);

  if( factor1 < 0 )
    return Value::errorNUM();
  if( factor2 < 0 )
    return Value::errorNUM();
    
  double result = number * factor2 / factor1;

  return Value (result);
}
