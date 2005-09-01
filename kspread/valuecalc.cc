/* This file is part of the KDE project
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "valuecalc.h"

#include "valueconverter.h"

#include <kdebug.h>
#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

using namespace KSpread;


// Array-walk functions registered on ValueCalc object

void awSum (ValueCalc *c, KSpreadValue &res, KSpreadValue val, KSpreadValue)
{
  if ((!val.isEmpty()) && (!val.isBoolean()) && (!val.isString()))
    res = c->add (res, val);
}

void awSumA (ValueCalc *c, KSpreadValue &res, KSpreadValue val, KSpreadValue)
{
  if (!val.isEmpty())
    res = c->add (res, val);
}

void awSumSq (ValueCalc *c, KSpreadValue &res, KSpreadValue val, KSpreadValue)
{
  if (!val.isEmpty())
    res = c->add (res, c->sqr (val));
}

void awSumSqA (ValueCalc *c, KSpreadValue &res, KSpreadValue val, KSpreadValue)
{
  if ((!val.isEmpty()) && (!val.isBoolean()) && (!val.isString()))
    res = c->add (res, c->sqr (val));
}

void awCount (ValueCalc *c, KSpreadValue &res, KSpreadValue val, KSpreadValue)
{
  if ((!val.isEmpty()) && (!val.isBoolean()) && (!val.isString()))
    res = c->add (res, 1);
}

void awCountA (ValueCalc *c, KSpreadValue &res, KSpreadValue val, KSpreadValue)
{
  if (!val.isEmpty())
    res = c->add (res, 1);
}

void awMax (ValueCalc *c, KSpreadValue &res, KSpreadValue val, KSpreadValue)
{
  if ((!val.isEmpty()) && (!val.isBoolean()) && (!val.isString()))
    if (res.isEmpty())
      res = val;
    else
      if (c->greater (val, res)) res = val;
}

void awMaxA (ValueCalc *c, KSpreadValue &res, KSpreadValue val, KSpreadValue)
{
  if (!val.isEmpty())
    if (res.isEmpty())
      // convert to number, so that we don't return string/bool
      res = c->conv()->asNumeric (val);
    else
      if (c->greater (val, res))
        // convert to number, so that we don't return string/bool
        res = c->conv()->asNumeric (val);
}

void awMin (ValueCalc *c, KSpreadValue &res, KSpreadValue val, KSpreadValue)
{
  if ((!val.isEmpty()) && (!val.isBoolean()) && (!val.isString()))
    if (res.isEmpty())
      res = val;
    else
      if (c->lower (val, res)) res = val;
}

void awMinA (ValueCalc *c, KSpreadValue &res, KSpreadValue val, KSpreadValue)
{
  if (!val.isEmpty())
    if (res.isEmpty())
      // convert to number, so that we don't return string/bool
      res = c->conv()->asNumeric (val);
    else
      if (c->lower (val, res))
        // convert to number, so that we don't return string/bool
        res = c->conv()->asNumeric (val);
}

void awProd (ValueCalc *c, KSpreadValue &res, KSpreadValue val, KSpreadValue)
{
  if ((!val.isEmpty()) && (!val.isBoolean()) && (!val.isString()))
    res = c->mul (res, val);
}

void awProdA (ValueCalc *c, KSpreadValue &res, KSpreadValue val, KSpreadValue)
{
  if (!val.isEmpty())
    res = c->mul (res, val);
}

// sum of squares of deviations, used to compute standard deviation
void awDevSq (ValueCalc *c, KSpreadValue &res, KSpreadValue val,
    KSpreadValue avg)
{
  if (!val.isEmpty())
    res = c->add (res, c->sqr (c->sub (val, avg)));
}

// sum of squares of deviations, used to compute standard deviation
void awDevSqA (ValueCalc *c, KSpreadValue &res, KSpreadValue val,
    KSpreadValue avg)
{
  if ((!val.isEmpty()) && (!val.isBoolean()) && (!val.isString()))
    res = c->add (res, c->sqr (c->sub (val, avg)));
}


bool isDate (KSpreadValue val) {
  KSpreadValue::Format fmt = val.format();
  if ((fmt == KSpreadValue::fmt_Date) || (fmt == KSpreadValue::fmt_DateTime))
    return true;
  return false;
}

// ***********************
// ****** ValueCalc ******
// ***********************

ValueCalc::ValueCalc (ValueConverter* c): converter( c )
{
  // initialize the random number generator
  srand (time (0));
  
  // register array-walk functions
  registerAwFunc ("sum", awSum);
  registerAwFunc ("suma", awSumA);
  registerAwFunc ("sumsq", awSumSq);
  registerAwFunc ("sumsqa", awSumSqA);
  registerAwFunc ("count", awCount);
  registerAwFunc ("counta", awCountA);
  registerAwFunc ("max", awMax);
  registerAwFunc ("maxa", awMaxA);
  registerAwFunc ("min", awMin);
  registerAwFunc ("mina", awMinA);
  registerAwFunc ("prod", awProd);
  registerAwFunc ("proda", awProdA);
  registerAwFunc ("devsq", awDevSq);
  registerAwFunc ("devsqa", awDevSq);
}

KSpreadValue ValueCalc::add (const KSpreadValue &a, const KSpreadValue &b)
{
  double aa, bb;
  aa = converter->asFloat (a).asFloat();
  bb = converter->asFloat (b).asFloat();
  KSpreadValue res = KSpreadValue (aa + bb);

  if (a.isNumber() || a.isEmpty())
    res.setFormat (format (a.format(), b.format()));
  // operation on two dates should produce a number
  if (isDate(a) && isDate(b))
    res.setFormat (KSpreadValue::fmt_Number);

  return res;
}

KSpreadValue ValueCalc::sub (const KSpreadValue &a, const KSpreadValue &b)
{
  double aa, bb;
  aa = converter->asFloat (a).asFloat();
  bb = converter->asFloat (b).asFloat();
  KSpreadValue res = KSpreadValue (aa - bb);

  if (a.isNumber() || a.isEmpty())
    res.setFormat (format (a.format(), b.format()));
  // operation on two dates should produce a number
  if (isDate(a) && isDate(b))
    res.setFormat (KSpreadValue::fmt_Number);

  return res;
}

KSpreadValue ValueCalc::mul (const KSpreadValue &a, const KSpreadValue &b)
{
  double aa, bb;
  aa = converter->asFloat (a).asFloat();
  bb = converter->asFloat (b).asFloat();
  KSpreadValue res = KSpreadValue (aa * bb);

  if (a.isNumber() || a.isEmpty())
    res.setFormat (format (a.format(), b.format()));
  // operation on two dates should produce a number
  if (isDate(a) && isDate(b))
    res.setFormat (KSpreadValue::fmt_Number);

  return res;
}

KSpreadValue ValueCalc::div (const KSpreadValue &a, const KSpreadValue &b)
{
  double aa, bb;
  aa = converter->asFloat (a).asFloat();
  bb = converter->asFloat (b).asFloat();
  KSpreadValue res;
  if (bb == 0.0)
    return KSpreadValue::errorDIV0();
  else
    res = KSpreadValue (aa / bb);

  if (a.isNumber() || a.isEmpty())
    res.setFormat (format (a.format(), b.format()));
  // operation on two dates should produce a number
  if (isDate(a) && isDate(b))
    res.setFormat (KSpreadValue::fmt_Number);

  return res;
}

KSpreadValue ValueCalc::mod (const KSpreadValue &a, const KSpreadValue &b)
{
  double aa, bb;
  aa = converter->asFloat (a).asFloat();
  bb = converter->asFloat (b).asFloat();
  KSpreadValue res;
  if (bb == 0.0)
    return KSpreadValue::errorDIV0();
  else
    res = KSpreadValue (fmod (aa, bb));

  if (a.isNumber() || a.isEmpty())
    res.setFormat (format (a.format(), b.format()));
  if (isDate(a) && isDate(b))
    res.setFormat (KSpreadValue::fmt_Number);

  return res;
}

KSpreadValue ValueCalc::pow (const KSpreadValue &a, const KSpreadValue &b)
{
  double aa, bb;
  aa = converter->asFloat (a).asFloat();
  bb = converter->asFloat (b).asFloat();
  KSpreadValue res = KSpreadValue (::pow (aa, bb));

  if (a.isNumber() || a.isEmpty())
    res.setFormat (format (a.format(), b.format()));
  // operation on date(s) should produce a number
  if (isDate(a) || isDate(b))
    res.setFormat (KSpreadValue::fmt_Number);

  return res;
}

KSpreadValue ValueCalc::sqr (const KSpreadValue &a)
{
  return mul (a, a);
}

KSpreadValue ValueCalc::sqrt (const KSpreadValue &a)
{
  KSpreadValue res = KSpreadValue (::sqrt (converter->asFloat(a).asFloat()));
  if (a.isNumber() || a.isEmpty())
    res.setFormat (a.format());
  // operation on date(s) should produce a number
  if (isDate(a))
    res.setFormat (KSpreadValue::fmt_Number);

  return res;
}

KSpreadValue ValueCalc::add (const KSpreadValue &a, double b)
{
  KSpreadValue res = KSpreadValue (converter->asFloat(a).asFloat() + b);

  if (a.isNumber() || a.isEmpty())
    res.setFormat (a.format());

  return res;
}

KSpreadValue ValueCalc::sub (const KSpreadValue &a, double b)
{
  KSpreadValue res = KSpreadValue (converter->asFloat(a).asFloat() - b);

  if (a.isNumber() || a.isEmpty())
    res.setFormat (a.format());

  return res;
}

KSpreadValue ValueCalc::mul (const KSpreadValue &a, double b)
{
  KSpreadValue res = KSpreadValue (converter->asFloat(a).asFloat() * b);

  if (a.isNumber() || a.isEmpty())
    res.setFormat (a.format());

  return res;
}

KSpreadValue ValueCalc::div (const KSpreadValue &a, double b)
{
  KSpreadValue res;
  if (b == 0.0)
    return KSpreadValue::errorDIV0();

  res = KSpreadValue (converter->asFloat(a).asFloat() / b);

  if (a.isNumber() || a.isEmpty())
    res.setFormat (a.format());

  return res;
}

KSpreadValue ValueCalc::pow (const KSpreadValue &a, double b)
{
  KSpreadValue res = KSpreadValue (::pow (converter->asFloat(a).asFloat(), b));

  if (a.isNumber() || a.isEmpty())
    res.setFormat (a.format());

  return res;
}

KSpreadValue ValueCalc::abs (const KSpreadValue &a)
{
  return KSpreadValue (fabs (converter->asFloat (a).asFloat()));
}

bool ValueCalc::isZero (const KSpreadValue &a)
{
  return (converter->asFloat (a).asFloat() != 0.0);
}

bool ValueCalc::isEven (const KSpreadValue &a)
{
  return ((converter->asInteger (a).asInteger() % 2) == 0);
}

bool ValueCalc::equal (const KSpreadValue &a, const KSpreadValue &b)
{
  return (converter->asFloat (a).asFloat() == converter->asFloat (b).asFloat());
}

/*********************************************************************
 *
 * Helper function to avoid problems with rounding floating point
 * values. Idea for this kind of solution taken from Openoffice.
 *
 *********************************************************************/
bool ValueCalc::approxEqual (const KSpreadValue &a, const KSpreadValue &b)
{
  double aa = converter->asFloat (a).asFloat();
  double bb = converter->asFloat (b).asFloat();
  if (aa == bb)
    return true;
  double x = aa - bb;
  return (x < 0.0 ? -x : x)  <  ((aa < 0.0 ? -aa : aa) * DBL_EPSILON);
}

bool ValueCalc::strEqual (const KSpreadValue &a, const KSpreadValue &b)
{
  return (converter->asString (a).asString() == converter->asString (b).asString());
}

bool ValueCalc::greater (const KSpreadValue &a, const KSpreadValue &b)
{
  double aa = converter->asFloat (a).asFloat();
  double bb = converter->asFloat (b).asFloat();
  return (aa > bb);
}

bool ValueCalc::gequal (const KSpreadValue &a, const KSpreadValue &b)
{
  return (greater (a,b) || approxEqual (a,b));
}

bool ValueCalc::lower (const KSpreadValue &a, const KSpreadValue &b)
{
  return greater (b, a);
}

KSpreadValue ValueCalc::roundDown (const KSpreadValue &a,
    const KSpreadValue &digits) {
  return roundDown (a, converter->asInteger (digits).asInteger());
}

KSpreadValue ValueCalc::roundUp (const KSpreadValue &a,
    const KSpreadValue &digits) {
  return roundUp (a, converter->asInteger (digits).asInteger());
}

KSpreadValue ValueCalc::round (const KSpreadValue &a,
    const KSpreadValue &digits) {
  return round (a, converter->asInteger (digits).asInteger());
}

KSpreadValue ValueCalc::roundDown (const KSpreadValue &a, int digits)
{
  // shift in one direction, round, shift back
  KSpreadValue val = a;
  if (digits > 0)
    for (int i = 0; i < digits; ++i)
      val = mul (val, 10);
  if (digits < 0)
    for (int i = 0; i < digits; ++i)
      val = div (val, 10);
  
  val = KSpreadValue (floor (converter->asFloat (val).asFloat()));
  
  if (digits > 0)
    for (int i = 0; i < digits; ++i)
      val = div (val, 10);
  if (digits < 0)
    for (int i = 0; i < digits; ++i)
      val = mul (val, 10);
  return val;
}

KSpreadValue ValueCalc::roundUp (const KSpreadValue &a, int digits)
{
  // shift in one direction, round, shift back
  KSpreadValue val = a;
  if (digits > 0)
    for (int i = 0; i < digits; ++i)
      val = mul (val, 10);
  if (digits < 0)
    for (int i = 0; i < digits; ++i)
      val = div (val, 10);
  
  val = KSpreadValue (ceil (converter->asFloat (val).asFloat()));
  
  if (digits > 0)
    for (int i = 0; i < digits; ++i)
      val = div (val, 10);
  if (digits < 0)
    for (int i = 0; i < digits; ++i)
      val = mul (val, 10);
  return val;
}

KSpreadValue ValueCalc::round (const KSpreadValue &a, int digits)
{
  // shift in one direction, round, shift back
  KSpreadValue val = a;
  if (digits > 0)
    for (int i = 0; i < digits; ++i)
      val = mul (val, 10);
  if (digits < 0)
    for (int i = 0; i < digits; ++i)
      val = div (val, 10);
  
  val = KSpreadValue (round (converter->asFloat (val).asFloat()));
  
  if (digits > 0)
    for (int i = 0; i < digits; ++i)
      val = div (val, 10);
  if (digits < 0)
    for (int i = 0; i < digits; ++i)
      val = mul (val, 10);
  return val;
}

int ValueCalc::sign (const KSpreadValue &a)
{
  double val = converter->asFloat (a).asFloat ();
  if (val == 0) return 0;
  if (val > 0) return 1;
  return -1;
}


KSpreadValue ValueCalc::log (const KSpreadValue &number,
    const KSpreadValue &base)
{
  double logbase = converter->asFloat (base).asFloat();
  if (logbase == 1.0)
    return KSpreadValue::errorDIV0();
  if (logbase <= 0.0)
    return KSpreadValue::errorNA();

  logbase = log10 (logbase);
  KSpreadValue res = KSpreadValue (log10 (converter->asFloat (number).asFloat()) / logbase);

  if (number.isNumber() || number.isEmpty())
    res.setFormat (number.format());

  return res;
}

KSpreadValue ValueCalc::ln (const KSpreadValue &number)
{
  KSpreadValue res = KSpreadValue (::log (converter->asFloat (number).asFloat()));

  if (number.isNumber() || number.isEmpty())
    res.setFormat (number.format());

  return res;
}

KSpreadValue ValueCalc::log (const KSpreadValue &number, double base)
{
  if (base <= 0.0)
    return KSpreadValue::errorNA();
  if (base == 1.0)
    return KSpreadValue::errorDIV0();

  double num = converter->asFloat (number).asFloat();
  KSpreadValue res = KSpreadValue (log10 (num) / log10 (base));

  if (number.isNumber() || number.isEmpty())
    res.setFormat (number.format());

  return res;
}

KSpreadValue ValueCalc::exp (const KSpreadValue &number)
{
  return KSpreadValue (::exp (converter->asFloat (number).asFloat()));
}

KSpreadValue ValueCalc::pi ()
{
  // retun PI in double-precision
  // if arbitrary precision gets in, this should be extended to return
  // more if need be
  return KSpreadValue (M_PI);
}

KSpreadValue ValueCalc::eps ()
{
  // #### This should adjust according to the actual number system used
  // (float, double, long double, ...)
  return KSpreadValue (DBL_EPSILON);
}

KSpreadValue ValueCalc::random (double range)
{
  return KSpreadValue (range * (double) rand() / (RAND_MAX + 1.0));
}

KSpreadValue ValueCalc::random (KSpreadValue range)
{
  return random (converter->asFloat (range).asFloat());
}

KSpreadValue ValueCalc::fact (const KSpreadValue &which)
{
  // we can simply use integers - no one is going to compute factorial of
  // anything bigger than 2^32
  return fact (converter->asInteger (which).asInteger());
}

KSpreadValue ValueCalc::fact (const KSpreadValue &which,
    const KSpreadValue &end)
{
  // we can simply use integers - no one is going to compute factorial of
  // anything bigger than 2^32
  return fact (converter->asInteger (which).asInteger(),
      converter->asInteger (end).asInteger ());
}

KSpreadValue ValueCalc::fact (int which, int end) {
  if (which < 0)
    return KSpreadValue (-1);
  if (which == 0)
    return KSpreadValue (1);
  // no multiplication if val==end
  if (which == end)
    return KSpreadValue (1);
    
  return (mul (fact (which-1, end), which));
}

KSpreadValue ValueCalc::factDouble (int which)
{
  if (which < 0)
    return KSpreadValue (-1);
  if ((which == 0) || (which == 1))
    return KSpreadValue (1);
    
  return (mul (factDouble (which-2), which));
}

KSpreadValue ValueCalc::factDouble (KSpreadValue which)
{
  return factDouble (converter->asInteger (which).asInteger());
}

KSpreadValue ValueCalc::combin (int n, int k)
{
  if (n >= 15)
  {
    double result = ::exp(lgamma (n + 1) - lgamma (k + 1) - lgamma (n-k+1));
    return KSpreadValue (floor(result + 0.5));
  }
  else
    return div (div (fact (n), fact (k)), fact (n - k));
}

KSpreadValue ValueCalc::combin (KSpreadValue n, KSpreadValue k)
{
  int nn = converter->asInteger (n).asInteger();
  int kk = converter->asInteger (k).asInteger();
  return combin (nn, kk);
}

KSpreadValue ValueCalc::gcd (const KSpreadValue &a, const KSpreadValue &b)
{
  // Euler's GCD algorithm
  KSpreadValue aa = round (a);
  KSpreadValue bb = round (b);

  if (approxEqual (aa, bb)) return aa;
  
  if (aa.isZero()) return bb;
  if (bb.isZero()) return aa;
  

  if (greater (aa, bb))
    return gcd (bb, mod (aa, bb));
  else
    return gcd (aa, mod (bb, aa));
}

KSpreadValue ValueCalc::lcm (const KSpreadValue &a, const KSpreadValue &b)
{
  KSpreadValue aa = round (a);
  KSpreadValue bb = round (b);

  if (approxEqual (aa, bb)) return aa;
  
  if (aa.isZero()) return bb;
  if (bb.isZero()) return aa;
  
  KSpreadValue g = gcd (aa, bb);
  if (g.isZero())  // GCD is zero for some weird reason
    return mul (aa, bb);

  return div (mul (aa, bb), g);
}

KSpreadValue ValueCalc::base (const KSpreadValue &val, int base, int prec)
{
  if (base == 10) return round (val, prec);
  if (prec < 0) prec = 2;
  if ((base < 2) || (base > 36))
    return KSpreadValue::errorVALUE();

  double value = converter->asFloat (val).asFloat();
  QString result = QString::number ((int)value, base);

  if (prec > 0)
  {
    result += "."; value = value - (int)value;

    int ix;
    for( int i = 0; i < prec; i++ )
    {
      ix = (int) value * base;
      result += "0123456789abcdefghijklmnopqrstuvwxyz"[ix];
      value = base * (value - (double)ix/base);
    }
  }

  return KSpreadValue (result.upper());
}

KSpreadValue ValueCalc::fromBase (const KSpreadValue &val, int base)
{
  QString str = converter->asString (val).asString();
  bool ok;
  double num = str.toLong (&ok, base);
  if (ok)
    return KSpreadValue (num);
  return KSpreadValue::errorVALUE();
}


KSpreadValue ValueCalc::sin (const KSpreadValue &number)
{
  KSpreadValue res = KSpreadValue (::sin (converter->asFloat (number).asFloat()));

  if (number.isNumber() || number.isEmpty())
    res.setFormat (number.format());

  return res;
}

KSpreadValue ValueCalc::cos (const KSpreadValue &number)
{
  KSpreadValue res = KSpreadValue (::cos (converter->asFloat (number).asFloat()));

  if (number.isNumber() || number.isEmpty())
    res.setFormat (number.format());

  return res;
}

KSpreadValue ValueCalc::tg (const KSpreadValue &number)
{
  KSpreadValue res = KSpreadValue (::tan (converter->asFloat (number).asFloat()));

  if (number.isNumber() || number.isEmpty())
    res.setFormat (number.format());

  return res;
}

KSpreadValue ValueCalc::cotg (const KSpreadValue &number)
{
  KSpreadValue res = KSpreadValue (div (1, ::tan (converter->asFloat (number).asFloat())));

  if (number.isNumber() || number.isEmpty())
    res.setFormat (number.format());

  return res;
}

KSpreadValue ValueCalc::asin (const KSpreadValue &number)
{
  errno = 0;
  KSpreadValue res = KSpreadValue (::asin (converter->asFloat (number).asFloat()));
  if (errno)
    return KSpreadValue::errorVALUE();

  if (number.isNumber() || number.isEmpty())
    res.setFormat (number.format());

  return res;
}

KSpreadValue ValueCalc::acos (const KSpreadValue &number)
{
  errno = 0;
  KSpreadValue res = KSpreadValue (::acos (converter->asFloat (number).asFloat()));
  if (errno)
    return KSpreadValue::errorVALUE();

  if (number.isNumber() || number.isEmpty())
    res.setFormat (number.format());

  return res;
}

KSpreadValue ValueCalc::atg (const KSpreadValue &number)
{
  errno = 0;
  KSpreadValue res = KSpreadValue (::atan (converter->asFloat (number).asFloat()));
  if (errno)
    return KSpreadValue::errorVALUE();

  if (number.isNumber() || number.isEmpty())
    res.setFormat (number.format());

  return res;
}

KSpreadValue ValueCalc::atan2 (const KSpreadValue &y, const KSpreadValue &x)
{
  double yy = converter->asFloat (y).asFloat();
  double xx = converter->asFloat (x).asFloat();
  return KSpreadValue (::atan2 (yy, xx));
}

KSpreadValue ValueCalc::sinh (const KSpreadValue &number)
{
  KSpreadValue res = KSpreadValue (::sinh (converter->asFloat (number).asFloat()));

  if (number.isNumber() || number.isEmpty())
    res.setFormat (number.format());

  return res;
}

KSpreadValue ValueCalc::cosh (const KSpreadValue &number)
{
  KSpreadValue res = KSpreadValue (::cosh (converter->asFloat (number).asFloat()));

  if (number.isNumber() || number.isEmpty())
    res.setFormat (number.format());

  return res;
}

KSpreadValue ValueCalc::tgh (const KSpreadValue &number)
{
  KSpreadValue res = KSpreadValue (::tanh (converter->asFloat (number).asFloat()));

  if (number.isNumber() || number.isEmpty())
    res.setFormat (number.format());

  return res;
}

KSpreadValue ValueCalc::asinh (const KSpreadValue &number)
{
  errno = 0;
  KSpreadValue res = KSpreadValue (::asinh (converter->asFloat (number).asFloat()));
  if (errno)
    return KSpreadValue::errorVALUE();

  if (number.isNumber() || number.isEmpty())
    res.setFormat (number.format());

  return res;
}

KSpreadValue ValueCalc::acosh (const KSpreadValue &number)
{
  errno = 0;
  KSpreadValue res = KSpreadValue (::acosh (converter->asFloat (number).asFloat()));
  if (errno)
    return KSpreadValue::errorVALUE();

  if (number.isNumber() || number.isEmpty())
    res.setFormat (number.format());

  return res;
}

KSpreadValue ValueCalc::atgh (const KSpreadValue &number)
{
  errno = 0;
  KSpreadValue res = KSpreadValue (::atanh (converter->asFloat (number).asFloat()));
  if (errno)
    return KSpreadValue::errorVALUE();

  if (number.isNumber() || number.isEmpty())
    res.setFormat (number.format());

  return res;
}

KSpreadValue ValueCalc::phi (KSpreadValue x)
{
  KSpreadValue constant (0.39894228040143268);
  
  // constant * exp(-(x * x) / 2.0);
  KSpreadValue x2neg = mul (sqr (x), -1);
  return mul (constant, exp (div (x2neg, 2.0)));
}

static double taylor_helper (double* pPolynom, uint nMax, double x)
{
  double nVal = pPolynom[nMax];
  for (int i = nMax-1; i >= 0; i--) {
    nVal = pPolynom[i] + (nVal * x);
  }
  return nVal;
}

KSpreadValue ValueCalc::gauss (KSpreadValue xx)
// this is a weird function
{
  double x = converter->asFloat (xx).asFloat();
  
  double t0[] =
    { 0.39894228040143268, -0.06649038006690545,  0.00997355701003582,
     -0.00118732821548045,  0.00011543468761616, -0.00000944465625950,
      0.00000066596935163, -0.00000004122667415,  0.00000000227352982,
      0.00000000011301172,  0.00000000000511243, -0.00000000000021218 };
  double t2[] =
    { 0.47724986805182079,  0.05399096651318805, -0.05399096651318805,
      0.02699548325659403, -0.00449924720943234, -0.00224962360471617,
      0.00134977416282970, -0.00011783742691370, -0.00011515930357476,
      0.00003704737285544,  0.00000282690796889, -0.00000354513195524,
      0.00000037669563126,  0.00000019202407921, -0.00000005226908590,
     -0.00000000491799345,  0.00000000366377919, -0.00000000015981997,
     -0.00000000017381238,  0.00000000002624031,  0.00000000000560919,
     -0.00000000000172127, -0.00000000000008634,  0.00000000000007894 };
  double t4[] =
    { 0.49996832875816688,  0.00013383022576489, -0.00026766045152977,
      0.00033457556441221, -0.00028996548915725,  0.00018178605666397,
     -0.00008252863922168,  0.00002551802519049, -0.00000391665839292,
     -0.00000074018205222,  0.00000064422023359, -0.00000017370155340,
      0.00000000909595465,  0.00000000944943118, -0.00000000329957075,
      0.00000000029492075,  0.00000000011874477, -0.00000000004420396,
      0.00000000000361422,  0.00000000000143638, -0.00000000000045848 };
  double asympt[] = { -1.0, 1.0, -3.0, 15.0, -105.0 };

  double xAbs = fabs(x);
  uint xShort = static_cast<uint>(floor(xAbs));
  double nVal = 0.0;
  if (xShort == 0)
    nVal = taylor_helper(t0, 11, (xAbs * xAbs)) * xAbs;
  else if ((xShort >= 1) && (xShort <= 2))
    nVal = taylor_helper(t2, 23, (xAbs - 2.0));
  else if ((xShort >= 3) && (xShort <= 4))
    nVal = taylor_helper(t4, 20, (xAbs - 4.0));
  else
  {
    double phiAbs = converter->asFloat (phi (xAbs)).asFloat();
    nVal = 0.5 + phiAbs * taylor_helper(asympt, 4, 1.0 / (xAbs * xAbs)) / xAbs;
  }

  if (x < 0.0)
    return KSpreadValue (-nVal);
  else
    return KSpreadValue (nVal);
}

KSpreadValue ValueCalc::gaussinv (KSpreadValue xx)
// this is a weird function
{
  double x = converter->asFloat (xx).asFloat();
  
  double q,t,z;

  q=x-0.5;

  if(fabs(q)<=.425)
  {
    t=0.180625-q*q;

    z=
    q*
    (
      (
        (
          (
            (
              (
                (
                  t*2509.0809287301226727+33430.575583588128105
                )
                *t+67265.770927008700853
              )
              *t+45921.953931549871457
            )
            *t+13731.693765509461125
          )
          *t+1971.5909503065514427
        )
        *t+133.14166789178437745
      )
      *t+3.387132872796366608
    )
    /
    (
      (
        (
          (
            (
              (
                (
                  t*5226.495278852854561+28729.085735721942674
                )
                *t+39307.89580009271061
              )
              *t+21213.794301586595867
            )
            *t+5394.1960214247511077
          )
          *t+687.1870074920579083
        )
        *t+42.313330701600911252
      )
      *t+1.0
    );

  }
  else
  {
    if(q>0)  t=1-x;
    else    t=x;

    t=::sqrt(-::log(t));

    if(t<=5.0)
    {
      t+=-1.6;

      z=
      (
        (
          (
            (
              (
                (
                  (
                    t*7.7454501427834140764e-4+0.0227238449892691845833
                  )
                  *t+0.24178072517745061177
                )
                *t+1.27045825245236838258
              )
              *t+3.64784832476320460504
            )
            *t+5.7694972214606914055
          )
          *t+4.6303378461565452959
        )
        *t+1.42343711074968357734
      )
      /
      (
        (
          (
            (
              (
                (
                  (
                    t*1.05075007164441684324e-9+5.475938084995344946e-4
                  )
                  *t+0.0151986665636164571966
                )
                *t+0.14810397642748007459
              )
              *t+0.68976733498510000455
            )
            *t+1.6763848301838038494
          )
          *t+2.05319162663775882187
        )
        *t+1.0
      );

    }
    else
    {
      t+=-5.0;

      z=
      (
        (
          (
            (
              (
                (
                  (
                    t*2.01033439929228813265e-7+2.71155556874348757815e-5
                  )
                  *t+0.0012426609473880784386
                )
                *t+0.026532189526576123093
              )
              *t+0.29656057182850489123
            )
            *t+1.7848265399172913358
          )
          *t+5.4637849111641143699
        )
        *t+6.6579046435011037772
      )
      /
      (
        (
          (
            (
              (
                (
                  (
                    t*2.04426310338993978564e-15+1.4215117583164458887e-7
                  )
                  *t+1.8463183175100546818e-5
                )
                *t+7.868691311456132591e-4
              )
              *t+0.0148753612908506148525
            )
            *t+0.13692988092273580531
          )
          *t+0.59983220655588793769
        )
        *t+1.0
      );

    }

    if(q<0.0) z=-z;
  }

  return KSpreadValue (z);
}

//helper for GetGamma and GetLogGamma
static double GammaHelp(double& x, bool& bReflect)
{
  double c[6] = {76.18009173, -86.50532033, 24.01409822,
                 -1.231739516, 0.120858003E-2, -0.536382E-5};
  if (x >= 1.0)
    {
      bReflect = false;
      x -= 1.0;
    }
  else
    {
      bReflect = true;
      x = 1.0 - x;
    }
  double s, anum;
  s = 1.0;
  anum = x;
  for (uint i = 0; i < 6; i++)
    {
      anum += 1.0;
      s += c[i]/anum;
    }
  s *= 2.506628275;   // sqrt(2*PI)
  return s;
}

KSpreadValue ValueCalc::GetGamma (KSpreadValue _x)
{
  double x = converter->asFloat (_x).asFloat();

  bool bReflect;
  double G = GammaHelp(x, bReflect);
  G = ::pow(x+5.5,x+0.5)*G/::exp(x+5.5);
  if (bReflect)
    G = M_PI*x/(G*::sin(M_PI*x));
  return KSpreadValue (G);
}

KSpreadValue ValueCalc::GetLogGamma (KSpreadValue _x)
{
  double x = converter->asFloat (_x).asFloat();

  bool bReflect;
  double G = GammaHelp(x, bReflect);
  G = (x+0.5)*::log(x+5.5)+::log(G)-(x+5.5);
  if (bReflect)
    G = ::log(M_PI*x)-G-::log(::sin(M_PI*x));
  return KSpreadValue (G);
}

KSpreadValue ValueCalc::GetGammaDist (KSpreadValue _x, KSpreadValue _alpha,
    KSpreadValue _beta)
{
  double x = converter->asFloat (_x).asFloat();
  double alpha = converter->asFloat (_alpha).asFloat();
  double beta = converter->asFloat (_beta).asFloat();
  
  if (x == 0.0)
    return KSpreadValue (0.0);

  x /= beta;
  double gamma = alpha;

  double c = 0.918938533204672741;
  double d[10] = {
    0.833333333333333333E-1,
    -0.277777777777777778E-2,
    0.793650793650793651E-3,
    -0.595238095238095238E-3,
    0.841750841750841751E-3,
    -0.191752691752691753E-2,
    0.641025641025641025E-2,
    -0.295506535947712418E-1,
    0.179644372368830573,
    -0.139243221690590111E1
  };

  double dx = x;
  double dgamma = gamma;
  int maxit = 10000;

  double z = dgamma;
  double den = 1.0;
  while ( z < 10.0 ) {
    den *= z;
    z += 1.0;
  }

  double z2 = z*z;
  double z3 = z*z2;
  double z4 = z2*z2;
  double z5 = z2*z3;
  double a = ( z - 0.5 ) * ::log(z) - z + c;
  double b = d[0]/z + d[1]/z3 + d[2]/z5 + d[3]/(z2*z5) + d[4]/(z4*z5) +
    d[5]/(z*z5*z5) + d[6]/(z3*z5*z5) + d[7]/(z5*z5*z5) + d[8]/(z2*z5*z5*z5);
  // double g = exp(a+b) / den;

  double sum = 1.0 / dgamma;
  double term = 1.0 / dgamma;
  double cut1 = dx - dgamma;
  double cut2 = dx * 10000000000.0;

  for ( int i=1; i<=maxit; i++ ) {
    double ai = i;
    term = dx * term / ( dgamma + ai );
    sum += term;
    double cutoff = cut1 + ( cut2 * term / sum );
    if ( ai > cutoff ) {
      double t = sum;
      // return pow( dx, dgamma ) * exp( -dx ) * t / g;
      return KSpreadValue (::exp( dgamma * ::log(dx) - dx - a - b ) * t * den);
    }
  }

  return KSpreadValue (1.0);             // should not happen ...
}

KSpreadValue ValueCalc::GetBeta (KSpreadValue _x, KSpreadValue _alpha,
    KSpreadValue _beta)
{
  if (equal (_beta, 1.0))
    return pow (_x, _alpha);
  else if (equal (_alpha, 1.0))
    // 1.0 - pow (1.0-_x, _beta)
    return sub (1.0, pow (sub (1.0, _x), _beta));

  double x = converter->asFloat (_x).asFloat();
  double alpha = converter->asFloat (_alpha).asFloat();
  double beta = converter->asFloat (_beta).asFloat();
    
  double fEps = 1.0E-8;
  bool bReflect;
  double cf, fA, fB;

  if (x < (alpha+1.0)/(alpha+beta+1.0)) {
    bReflect = false;
    fA = alpha;
    fB = beta;
  }
  else {
    bReflect = true;
    fA = beta;
    fB = alpha;
    x = 1.0 - x;
  }
  if (x < fEps)
    cf = 0.0;
  else {
    double a1, b1, a2, b2, fnorm, rm, apl2m, d2m, d2m1, cfnew;
    a1 = 1.0; b1 = 1.0;
    b2 = 1.0 - (fA+fB)*x/(fA+1.0);
    if (b2 == 0.0) {
      a2 = b2;
      fnorm = 1.0;
      cf = 1.0;
    }
    else {
      a2 = 1.0;
      fnorm = 1.0/b2;
      cf = a2*fnorm;
    }
    cfnew = 1.0;
    for (uint j = 1; j <= 100; j++) {
      rm = (double) j;
      apl2m = fA + 2.0*rm;
      d2m = rm*(fB-rm)*x/((apl2m-1.0)*apl2m);
      d2m1 = -(fA+rm)*(fA+fB+rm)*x/(apl2m*(apl2m+1.0));
      a1 = (a2+d2m*a1)*fnorm;
      b1 = (b2+d2m*b1)*fnorm;
      a2 = a1 + d2m1*a2*fnorm;
      b2 = b1 + d2m1*b2*fnorm;
      if (b2 != 0.0) {
        fnorm = 1.0/b2;
        cfnew = a2*fnorm;
        if (fabs(cf-cfnew)/cf < fEps)
          j = 101;
        else
          cf = cfnew;
      }
    }
    if (fB < fEps)
      b1 = 1.0E30;
    else
      b1 = ::exp(GetLogGamma(fA).asFloat()+GetLogGamma(fB).asFloat()-
          GetLogGamma(fA+fB).asFloat());

    cf *= ::pow(x, fA)*::pow(1.0-x,fB)/(fA*b1);
  }
  if (bReflect)
    return KSpreadValue (1.0-cf);
  else
    return KSpreadValue (cf);
}

// ------------------------------------------------------


/*
 *
 * The code for calculating Bessel functions is taken
 * from CCMATH, a mathematics library source.code.
 *
 * Original copyright follows:
 *
 *  Copyright (C)  2000   Daniel A. Atkinson    All rights reserved.
 *  This code may be redistributed under the terms of the GNU library
 *  public license (LGPL).
 */

static double ccmath_gaml(double x)
{ double g,h;
  for(g=1.; x<30. ;g*=x,x+=1.); h=x*x;
  g=(x-.5)*log(x)-x+.918938533204672-log(g);
  g+=(1.-(1./6.-(1./3.-1./(4.*h))/(7.*h))/(5.*h))/(12.*x);
  return g;
}

static double ccmath_psi(int m)
{ double s= -.577215664901533; int k;
  for(k=1; k<m ;++k) s+=1./k;
  return s;
}

static double ccmath_ibes(double v,double x)
{ double y,s,t,tp; int p,m;
  y=x-9.; if(y>0.) y*=y; tp=v*v*.2+25.;
  if(y<tp){ x/=2.; m=x;
    if(x>0.) s=t=exp(v*log(x)-ccmath_gaml(v+1.));
    else{ if(v>0.) return 0.; else if(v==0.) return 1.;}
    for(p=1,x*=x;;++p){ t*=x/(p*(v+=1.)); s+=t;
      if(p>m && t<1.e-13*s) break;
     }
   }
  else{ double u,a0=1.57079632679490;
    s=t=1./sqrt(x*a0); x*=2.; u=0.;
    for(p=1,y=.5; (tp=fabs(t))>1.e-14 ;++p,y+=1.){
      t*=(v+y)*(v-y)/(p*x); if(y>v && fabs(t)>=tp) break;
      if(!(p&1)) s+=t; else u-=t;
     }
    x/=2.; s=cosh(x)*s+sinh(x)*u;
   }
  return s;
}

static double ccmath_kbes(double v,double x)
{ double y,s,t,tp,f,a0=1.57079632679490;
  int p,k,m;
  if(x==0.) return HUGE_VAL;
  y=x-10.5; if(y>0.) y*=y; tp=25.+.185*v*v;
  if(y<tp && modf(v+.5,&t)!=0.){ y=1.5+.5*v;
    if(x<y){ x/=2.; m=x; tp=t=exp(v*log(x)-ccmath_gaml(v+1.));
      if(modf(v,&y)==0.){ k=y; tp*=v;
        f=2.*log(x)-ccmath_psi(1)-ccmath_psi(k+1);
        t/=2.; if(!(k&1)) t= -t; s=f*t;
        for(p=1,x*=x;;++p){ f-=1./p+1./(v+=1.);
          t*=x/(p*v); s+=(y=t*f);
          if(p>m && fabs(y)<1.e-14) break; }
        if(k>0){ x= -x; s+=(t=1./(tp*2.));
          for(p=1,--k; k>0 ;++p,--k) s+=(t*=x/(p*k)); }
       }
      else{ f=1./(t*v*2.); t*=a0/sin(2.*a0*v); s=f-t;
        for(p=1,x*=x,tp=v;;++p){
          t*=x/(p*(v+=1.)); f*= -x/(p*(tp-=1.));
          s+=(y=f-t); if(p>m && fabs(y)<1.e-14) break; }
       }
     }
    else{ double tq,h,w,z,r;
      t=12./pow(x,.333); k=t*t; y=2.*(x+k);
      m=v; v-=m; tp=v*v-.25; v+=1.; tq=v*v-.25;
      for(s=h=1.,r=f=z=w=0.; k>0 ;--k,y-=2.){
        t=(y*h-(k+1)*z)/(k-1-tp/k); z=h; f+=(h=t);
        t=(y*s-(k+1)*w)/(k-1-tq/k); w=s; r+=(s=t);  }
      t=sqrt(a0/x)*exp(-x); s*=t/r; h*=t/f; x/=2.; if(m==0) s=h;
      for(k=1; k<m ;++k){ t=v*s/x+h; h=s; s=t; v+=1.;}
     }
   }
  else{ s=t=sqrt(a0/x); x*=2.;
    for(p=1,y=.5; (tp=fabs(t))>1.e-14 ;++p,y+=1.){
      t*=(v+y)*(v-y)/(p*x); if(y>v && fabs(t)>=tp) break; s+=t; }
    s*=exp(-x/2.);
   }
  return s;
}

static double ccmath_jbes(double v,double x)
{ double y,s,t,tp; int p,m;
  y=x-8.5; if(y>0.) y*=y; tp=v*v/4.+13.69;
  if(y<tp){ x/=2.; m=x;
    if(x>0.) s=t=exp(v*log(x)-ccmath_gaml(v+1.));
    else{ if(v>0.) return 0.; else if(v==0.) return 1.;}
    for(p=1,x*= -x;;++p){ t*=x/(p*(v+=1.)); s+=t;
      if(p>m && fabs(t)<1.e-13) break;
     }
   }
  else{ double u,a0=1.57079632679490;
    s=t=1./sqrt(x*a0); x*=2.; u=0.;
    for(p=1,y=.5; (tp=fabs(t))>1.e-14 ;++p,y+=1.){
      t*=(v+y)*(v-y)/(p*x); if(y>v && fabs(t)>=tp) break;
      if(!(p&1)){ t= -t; s+=t;} else u-=t;
     }
    y=x/2.-(v+.5)*a0; s=cos(y)*s+sin(y)*u;
   }
  return s;
}

static double ccmath_nbes(double v,double x)
{ double y,s,t,tp,u,f,a0=3.14159265358979;
  int p,k,m;
  y=x-8.5; if(y>0.) y*=y; tp=v*v/4.+13.69;
  if(y<tp){ if(x==0.) return HUGE_VAL;
    x/=2.; m=x; u=t=exp(v*log(x)-ccmath_gaml(v+1.));
    if(modf(v,&y)==0.){ k=y; u*=v;
      f=2.*log(x)-ccmath_psi(1)-ccmath_psi(k+1);
      t/=a0; x*= -x; s=f*t;
      for(p=1;;++p){ f-=1./p+1./(v+=1.);
        t*=x/(p*v); s+=(y=t*f); if(p>m && fabs(y)<1.e-13) break; }
      if(k>0){ x= -x; s-=(t=1./(u*a0));
        for(p=1,--k; k>0 ;++p,--k) s-=(t*=x/(p*k)); }
     }
    else{ f=1./(t*v*a0); t/=tan(a0*v); s=t-f;
      for(p=1,x*=x,u=v;;++p){
        t*= -x/(p*(v+=1.)); f*=x/(p*(u-=1.));
        s+=(y=t-f); if(p>m && fabs(y)<1.e-13) break; }
     }
   }
  else{ x*=2.; s=t=2./sqrt(x*a0); u=0.;
    for(p=1,y=.5; (tp=fabs(t))>1.e-14 ;++p,y+=1.){
      t*=(v+y)*(v-y)/(p*x); if(y>v && fabs(t)>tp) break;
      if(!(p&1)){ t= -t; s+=t;} else u+=t;
     }
    y=(x-(v+.5)*a0)/2.; s=sin(y)*s+cos(y)*u;
   }
  return s;
}


/* ---------- end of CCMATH code ---------- */


KSpreadValue ValueCalc::besseli (KSpreadValue v, KSpreadValue x)
{
  double vv = converter->asFloat (v).asFloat ();
  double xx = converter->asFloat (x).asFloat ();
  return KSpreadValue (ccmath_ibes (vv, xx));
}

KSpreadValue ValueCalc::besselj (KSpreadValue v, KSpreadValue x)
{
  double vv = converter->asFloat (v).asFloat ();
  double xx = converter->asFloat (x).asFloat ();
  return KSpreadValue (ccmath_jbes (vv, xx));
}

KSpreadValue ValueCalc::besselk (KSpreadValue v, KSpreadValue x)
{
  double vv = converter->asFloat (v).asFloat ();
  double xx = converter->asFloat (x).asFloat ();
  return KSpreadValue (ccmath_kbes (vv, xx));
}

KSpreadValue ValueCalc::besseln (KSpreadValue v, KSpreadValue x)
{
  double vv = converter->asFloat (v).asFloat ();
  double xx = converter->asFloat (x).asFloat ();
  return KSpreadValue (ccmath_nbes (vv, xx));
}

// ------------------------------------------------------
  
KSpreadValue ValueCalc::erf (KSpreadValue x)
{
  return KSpreadValue (::erf (converter->asFloat (x).asFloat()));
}

KSpreadValue ValueCalc::erfc (KSpreadValue x)
{
  return KSpreadValue (::erfc (converter->asFloat (x).asFloat()));
}

// ------------------------------------------------------

void ValueCalc::arrayWalk (const KSpreadValue &range,
    KSpreadValue &res, arrayWalkFunc func, KSpreadValue param)
{
  if (res.isError()) return;
  if (!range.isArray ())
  {
    func (this, res, range, param);
    return;
  }

  int rows = range.rows ();
  int cols = range.columns ();
  for (int r = 0; r < rows; r++)
    for (int c = 0; c < cols; c++)
    {
      KSpreadValue v = range.element (c, r);
      if (v.isArray())
        arrayWalk (v, res, func, param);
      else {
        func (this, res, v, param);
        if (res.format() == KSpreadValue::fmt_None)
          res.setFormat (v.format());
      }
    }
}

void ValueCalc::arrayWalk (QValueVector<KSpreadValue> &range,
    KSpreadValue &res, arrayWalkFunc func, KSpreadValue param)
{
  if (res.isError()) return;
  for (unsigned int i = 0; i < range.count(); ++i)
    arrayWalk (range[i], res, func, param);
}

void ValueCalc::twoArrayWalk (const KSpreadValue &a1, const KSpreadValue &a2,
    KSpreadValue &res, arrayWalkFunc func)
{
  if (res.isError()) return;
  if (!a1.isArray ())
  {
    func (this, res, a1, a2);
    return;
  }

  int rows = a1.rows ();
  int cols = a1.columns ();
  int rows2 = a2.rows ();
  int cols2 = a2.columns ();
  if ((rows != rows2) || (cols != cols2)) {
    res = KSpreadValue::errorVALUE();
    return;
  }
  for (int r = 0; r < rows; r++)
    for (int c = 0; c < cols; c++)
    {
      KSpreadValue v1 = a1.element (c, r);
      KSpreadValue v2 = a1.element (c, r);
      if (v1.isArray() && v2.isArray())
        twoArrayWalk (v1, v2, res, func);
      else {
        func (this, res, v1, v2);
        if (res.format() == KSpreadValue::fmt_None)
          res.setFormat (format (v1.format(), v2.format()));
      }
    }
}

void ValueCalc::twoArrayWalk (QValueVector<KSpreadValue> &a1,
    QValueVector<KSpreadValue> &a2, KSpreadValue &res, arrayWalkFunc func)
{
  if (res.isError()) return;
  if (a1.count() != a2.count()) {
    res = KSpreadValue::errorVALUE();
    return;
  }
  for (unsigned int i = 0; i < a1.count(); ++i)
    twoArrayWalk (a1[i], a2[i], res, func);
}

arrayWalkFunc ValueCalc::awFunc (const QString &name)
{
  if (awFuncs.count(name))
    return awFuncs[name];
  return 0;
}

void ValueCalc::registerAwFunc (const QString &name, arrayWalkFunc func)
{
  awFuncs[name] = func;
}

// ------------------------------------------------------

KSpreadValue ValueCalc::sum (const KSpreadValue &range, bool full)
{
  KSpreadValue res;
  arrayWalk (range, res, awFunc (full ? "suma" : "sum"), 0);
  return res;
}

KSpreadValue ValueCalc::sum (QValueVector<KSpreadValue> range, bool full)
{
  KSpreadValue res;
  arrayWalk (range, res, awFunc (full ? "suma" : "sum"), 0);
  return res;
}

// sum of squares
KSpreadValue ValueCalc::sumsq (const KSpreadValue &range, bool full)
{
  KSpreadValue res;
  arrayWalk (range, res, awFunc (full ? "sumsqa" : "sumsq"), 0);
  return res;
}

KSpreadValue ValueCalc::sumIf (const KSpreadValue &range,
    const KSpreadValue &checkRange, const Condition &cond)
{
  if (!range.isArray())
  {
    if (matches (cond, checkRange.element (0, 0)))
      return converter->asNumeric (range);
    return KSpreadValue (0.0);
  }

  //if we are here, we have an array
  KSpreadValue res;

  unsigned int rows = range.rows ();
  unsigned int cols = range.columns ();
  for (unsigned int r = 0; r < rows; r++)
    for (unsigned int c = 0; c < cols; c++)
    {
      KSpreadValue v = range.element (c, r);
      KSpreadValue newcheck = v;
      if ((c < checkRange.columns()) && (r < checkRange.rows()))
        newcheck = checkRange.element (c, r);
      
      if (v.isArray())
        res = add (res, sumIf (v, newcheck, cond));
      else
        if (matches (cond, newcheck))
          res = add (res, v);
    }

  return res;
}

int ValueCalc::count (const KSpreadValue &range, bool full)
{
  KSpreadValue res = 0;
  arrayWalk (range, res, awFunc (full ? "counta" : "count"), 0);
  return converter->asInteger (res).asInteger ();
}

int ValueCalc::count (QValueVector<KSpreadValue> range, bool full)
{
  KSpreadValue res = 0;
  arrayWalk (range, res, awFunc (full ? "counta" : "count"), 0);
  return converter->asInteger (res).asInteger ();
}

int ValueCalc::countIf (const KSpreadValue &range, const Condition &cond)
{
  if (!range.isArray())
  {
    if (matches (cond, range))
      return range.isEmpty() ? 0 : 1;
    return 0;
  }

  int res = 0;

  int cols = range.columns ();
  int rows = range.rows ();
  for (int r = 0; r < rows; r++)
    for (int c = 0; c < cols; c++)
    {
      KSpreadValue v = range.element (c, r);
      
      if (v.isArray())
        res += countIf (v, cond);
      else
        if (matches (cond, v))
          res++;
    }

  return res;
}

KSpreadValue ValueCalc::avg (const KSpreadValue &range, bool full)
{
  int cnt = count (range, full);
  if (cnt)
    return div (sum (range, full), cnt);
  return KSpreadValue (0.0);
}

KSpreadValue ValueCalc::avg (QValueVector<KSpreadValue> range, bool full)
{
  int cnt = count (range, full);
  if (cnt)
    return div (sum (range, full), cnt);
  return KSpreadValue (0.0);
}

KSpreadValue ValueCalc::max (const KSpreadValue &range, bool full)
{
  KSpreadValue res;
  arrayWalk (range, res, awFunc (full ? "maxa" : "max"), 0);
  return res;
}

KSpreadValue ValueCalc::max (QValueVector<KSpreadValue> range, bool full)
{
  KSpreadValue res;
  arrayWalk (range, res, awFunc (full ? "maxa" : "max"), 0);
  return res;
}

KSpreadValue ValueCalc::min (const KSpreadValue &range, bool full)
{
  KSpreadValue res;
  arrayWalk (range, res, awFunc (full ? "mina" : "min"), 0);
  return res;
}

KSpreadValue ValueCalc::min (QValueVector<KSpreadValue> range, bool full)
{
  KSpreadValue res;
  arrayWalk (range, res, awFunc (full ? "mina" : "min"), 0);
  return res;
}

KSpreadValue ValueCalc::product (const KSpreadValue &range, KSpreadValue init,
    bool full)
{
  KSpreadValue res = init;
  if (isZero (init))  // special handling of a zero, due to excel-compat
  {
    if (count (range, full) == 0)
      return init;
    res = 1.0;
  }  
  arrayWalk (range, res, awFunc (full ? "proda" : "prod"), 0);
  return res;
}

KSpreadValue ValueCalc::product (QValueVector<KSpreadValue> range,
    KSpreadValue init, bool full)
{
  KSpreadValue res = init;
  if (isZero (init))  // special handling of a zero, due to excel-compat
  {
    if (count (range, full) == 0)
      return init;
    res = 1.0;
  }  
  arrayWalk (range, res, awFunc (full ? "proda" : "prod"), 0);
  return res;
}

KSpreadValue ValueCalc::stddev (const KSpreadValue &range, bool full)
{
  return stddev (range, avg (range, full), full);
}

KSpreadValue ValueCalc::stddev (const KSpreadValue &range, KSpreadValue avg,
    bool full)
{
  KSpreadValue res;
  int cnt = count (range, full);
  arrayWalk (range, res, awFunc (full ? "devsqa" : "devsq"), avg);
  return sqrt (div (res, cnt-1));
}

KSpreadValue ValueCalc::stddev (QValueVector<KSpreadValue> range, bool full)
{
  return stddev (range, avg (range, full), full);
}

KSpreadValue ValueCalc::stddev (QValueVector<KSpreadValue> range,
    KSpreadValue avg, bool full)
{
  KSpreadValue res;
  int cnt = count (range, full);
  arrayWalk (range, res, awFunc (full ? "devsqa" : "devsq"), avg);
  return sqrt (div (res, cnt-1));
}

KSpreadValue ValueCalc::stddevP (const KSpreadValue &range, bool full)
{
  return stddevP (range, avg (range, full), full);
}

KSpreadValue ValueCalc::stddevP (const KSpreadValue &range, KSpreadValue avg,
    bool full)
{
  KSpreadValue res;
  int cnt = count (range, full);
  arrayWalk (range, res, awFunc (full ? "devsqa" : "devsq"), avg);
  return sqrt (div (res, cnt));
}

KSpreadValue ValueCalc::stddevP (QValueVector<KSpreadValue> range, bool full)
{
  return stddevP (range, avg (range, full), full);
}

KSpreadValue ValueCalc::stddevP (QValueVector<KSpreadValue> range,
    KSpreadValue avg, bool full)
{
  KSpreadValue res;
  int cnt = count (range, full);
  arrayWalk (range, res, awFunc (full ? "devsqa" : "devsq"), avg);
  return sqrt (div (res, cnt));
}

KSpreadValue::Format ValueCalc::format (KSpreadValue::Format a,
    KSpreadValue::Format b)
{
  if ((a == KSpreadValue::fmt_None) || (a == KSpreadValue::fmt_Boolean))
    return b;
  return a;
}


// ------------------------------------------------------

void ValueCalc::getCond (Condition &cond, QString text)
{
  cond.comp = isEqual;
  text = text.stripWhiteSpace();

  if ( text.startsWith( "<=" ) )
  {
    cond.comp = lessEqual;
    text = text.remove( 0, 2 );
  }
  else if ( text.startsWith( ">=" ) )
  {
    cond.comp = greaterEqual;
    text = text.remove( 0, 2 );
  }
  else if ( text.startsWith( "!=" ) || text.startsWith( "<>" ) )
  {
    cond.comp = notEqual;
    text = text.remove( 0, 2 );
  }
  else if ( text.startsWith( "==" ) )
  {
    cond.comp = isEqual;
    text = text.remove( 0, 2 );
  }
  else if ( text.startsWith( "<" ) )
  {
    cond.comp = isLess;
    text = text.remove( 0, 1 );
  }
  else if ( text.startsWith( ">" ) )
  {
    cond.comp = isGreater;
    text = text.remove( 0, 1 );
  }
  else if ( text.startsWith( "=" ) )
  {
    cond.comp = isEqual;
    text = text.remove( 0, 1 );
  }

  text = text.stripWhiteSpace();

  bool ok = false;
  double d = text.toDouble( &ok );
  if ( ok )
  {
    cond.type = numeric;
    cond.value = d;
    kdDebug() << "Numeric: " << d << ", Op: " << cond.comp << endl;
  }
  else
  {
    cond.type = string;
    cond.stringValue = text;
    kdDebug() << "String: " << text << ", Op: " << cond.comp << endl;
  }
}

bool ValueCalc::matches (const Condition &cond, KSpreadValue val)
{
  if (cond.type == numeric) {
    double d = converter->asFloat (val).asFloat();
    switch ( cond.comp )
    {
      case isEqual:
      if (approxEqual (d, cond.value)) return true;
      break;
  
      case isLess:
      if (d < cond.value) return true;
      break;
  
      case isGreater:
      if (d > cond.value) return true;
      break;
  
      case lessEqual:
      if (d <= cond.value) return true;
      break;
  
      case greaterEqual:
      if (d >= cond.value) return true;
      break;
  
      case notEqual:
      if (d != cond.value) return true;
      break;
    }
  } else {
    QString d = converter->asString (val).asString();
    switch ( cond.comp )
    {
      case isEqual:
      if (d == cond.stringValue) return true;
      break;
  
      case isLess:
      if (d < cond.stringValue) return true;
      break;
  
      case isGreater:
      if (d > cond.stringValue) return true;
      break;
  
      case lessEqual:
      if (d <= cond.stringValue) return true;
      break;
  
      case greaterEqual:
      if (d >= cond.stringValue) return true;
      break;
  
      case notEqual:
      if (d != cond.stringValue) return true;
      break;
    }
  }
  return false;
}

