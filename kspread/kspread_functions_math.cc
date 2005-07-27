/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team
                           www.koffice.org/kspread
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>

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

#include <kdebug.h>
#include <klocale.h>

#include "functions.h"
#include "valuecalc.h"
#include "valueconverter.h"

// needed for RANDBINOM and so
#include <math.h>

using namespace KSpread;

// RANDBINOM and RANDNEGBINOM won't support arbitrary precision

// prototypes
KSpreadValue func_abs (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_ceil (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_ceiling (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_count (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_counta (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_countblank (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_countif (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_cur (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_div (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_eps (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_even (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_exp (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_fact (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_factdouble (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_fib (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_floor (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_gcd (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_int (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_inv (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_kproduct (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_lcm (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_ln (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_log2 (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_log10 (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_logn (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_max (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_min (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_mod (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_mround (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_mult (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_multinomial (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_odd (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_pow (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_quotient (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_product (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_rand (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_randbetween (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_randbernoulli (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_randbinom (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_randexp (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_randnegbinom (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_randnorm (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_randpoisson (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_rootn (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_round (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_rounddown (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_roundup (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_sign (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_sqrt (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_sqrtpi (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_subtotal (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_sum (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_sumif (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_sumsq (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_trunc (valVector args, ValueCalc *calc, FuncExtra *);


// KSpreadValue func_multipleOP (valVector args, ValueCalc *calc, FuncExtra *);

// registers all math functions
void KSpreadRegisterMathFunctions()
{
  FunctionRepository* repo = FunctionRepository::self();
  Function *f;

/*
  f = new Function ("MULTIPLEOPERATIONS", func_multipleOP);
  repo->add (f);
*/

  // functions that don't take array parameters
  f = new Function ("ABS",           func_abs);
  repo->add (f);
  f = new Function ("CEIL",          func_ceil);
  repo->add (f);
  f = new Function ("CEILING",       func_ceiling);
  f->setParamCount (1, 2);
  repo->add (f);
  f = new Function ("CUR",           func_cur);
  repo->add (f);
  f = new Function ("EPS",           func_eps);
  f->setParamCount (0);
  repo->add (f);
  f = new Function ("EVEN",          func_even);
  repo->add (f);
  f = new Function ("EXP",           func_exp);
  repo->add (f);
  f = new Function ("FACT",          func_fact);
  repo->add (f);
  f = new Function ("FACTDOUBLE",    func_factdouble);
  repo->add (f);
  f = new Function ("FIB",           func_fib); // KSpread-specific, like Quattro-Pro's FIB
  repo->add (f);
  f = new Function ("FLOOR",         func_floor);
  repo->add (f);
  f = new Function ("INT",           func_int);
  repo->add (f);
  f = new Function ("INV",           func_inv);
  repo->add (f);
  f = new Function ("LN",            func_ln);
  repo->add (f);
  f = new Function ("LOG",           func_log10);
  repo->add (f);
  f = new Function ("LOG2",          func_log2);
  repo->add (f);
  f = new Function ("LOG10",         func_log10);   // same as LOG
  repo->add (f);
  f = new Function ("LOGN",          func_logn);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("MOD",           func_mod);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("MROUND",        func_mround);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("MULTINOMIAL",   func_multinomial);
  f->setParamCount (1, -1);
  repo->add (f);
  f = new Function ("ODD",           func_odd);
  repo->add (f);
  f = new Function ("POW",           func_pow); // remove in 1.5
  repo->add (f);
  f = new Function ("POWER",         func_pow);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("QUOTIENT",      func_quotient);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("RAND",          func_rand);
  f->setParamCount (0);
  repo->add (f);
  f = new Function ("RANDBERNOULLI", func_randbernoulli);
  repo->add (f);
  f = new Function ("RANDBETWEEN",   func_randbetween);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("RANDBINOM",     func_randbinom);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("RANDEXP",       func_randexp);
  repo->add (f);
  f = new Function ("RANDNEGBINOM",  func_randnegbinom);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("RANDNORM",      func_randnorm);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("RANDPOISSON",   func_randpoisson);
  repo->add (f);
  f = new Function ("ROOTN",         func_rootn);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("ROUND",         func_round);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("ROUNDDOWN",     func_rounddown);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("ROUNDUP",       func_roundup);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("SIGN",          func_sign);
  repo->add (f);
  f = new Function ("SQRT",          func_sqrt);
  repo->add (f);
  f = new Function ("SQRTPI",        func_sqrtpi);
  repo->add (f);
  f = new Function ("TRUNC",         func_trunc);
  f->setParamCount (1, 2);
  repo->add (f);
  
  // functions that operate over arrays
  f = new Function ("COUNT",         func_count);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("COUNTA",        func_counta);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("COUNTBLANK",    func_countblank);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("COUNTIF",       func_countif);
  f->setParamCount (2);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("DIV",           func_div);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("G_PRODUCT",     func_kproduct); // Gnumeric compatibility
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("GCD",           func_gcd);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("KPRODUCT",      func_kproduct);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("LCM",           func_lcm);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("MAX",           func_max);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("MAXA",          func_max);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("MIN",           func_min);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("MINA",          func_min);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("MULTIPLY",      func_product);  // same as PRODUCT
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("PRODUCT",       func_product);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("SUM",           func_sum);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("SUMA",          func_sum);   // same as SUM
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("SUBTOTAL",      func_subtotal);
  f->setParamCount (2);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("SUMIF",         func_sumif);
  f->setParamCount (2, 3);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("SUMSQ",         func_sumsq);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
}

// Function: SQRT
KSpreadValue func_sqrt (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->sqrt (args[0]);
}

// Function: SQRTPI
KSpreadValue func_sqrtpi (valVector args, ValueCalc *calc, FuncExtra *)
{
  // sqrt (val * PI)
  return calc->sqrt (calc->mul (args[0], calc->pi()));
}

// Function: ROOTN
KSpreadValue func_rootn (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->pow (args[0], calc->div (1, args[1]));
}

// Function: CUR
KSpreadValue func_cur (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->pow (args[0], 1.0/3.0);
}

// Function: ABS
KSpreadValue func_abs (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->abs (args[0]);
}

// Function: exp
KSpreadValue func_exp (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->exp (args[0]);
}

// Function: ceil
KSpreadValue func_ceil (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->roundUp (args[0], 0);
}

// Function: ceiling
KSpreadValue func_ceiling (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue number = args[0];
  KSpreadValue res;
  if (args.count() == 2)
    res = args[1];
  else
    res = calc->gequal (number, 0.0) ? 1.0 : -1.0;
  
  if (calc->isZero(res))
    return KSpreadValue::errorDIV0();

  KSpreadValue d = calc->div (number, res);
  if (calc->greater (0, d))
    return KSpreadValue::errorVALUE();

  KSpreadValue rud = calc->roundDown (d);
  if (calc->approxEqual (rud, d))
    d = calc->mul (rud, res);
  else
    d = calc->mul (calc->roundUp (d), res);

  return d;
}

// Function: floor
KSpreadValue func_floor (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->roundDown (args[0], 0);
}

// Function: ln
KSpreadValue func_ln (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->ln (args[0]);
}

// Function: LOGn
KSpreadValue func_logn (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->log (args[0], args[1]);
}

// Function: LOG2
KSpreadValue func_log2 (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->log (args[0], 2.0);
}

// Function: LOG10
KSpreadValue func_log10 (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->log (args[0]);
}

// Function: sum
KSpreadValue func_sum (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue res = 0;
  calc->arrayWalk (args, res, calc->awFunc ("sum"), 0);
  return res;
}

KSpreadValue func_sumif (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue sumRange = args[0];
  QString condition = calc->conv()->asString (args[1]).asString();
  KSpreadValue checkRange = sumRange;
  if (args.count() == 3)
    checkRange = args[2];

  Condition cond;
  calc->getCond (cond, condition);

  return calc->sumIf (sumRange, checkRange, cond);
}

// Function: product
KSpreadValue func_product (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->product (args[0], 0.0);
}

// Function: kproduct
KSpreadValue func_kproduct (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->product (args[0], 1.0);
}

// Function: DIV
KSpreadValue func_div (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue val = args[0];
  for (unsigned int i = 1; i < args.count(); ++i)
  {
    val = calc->div (val, args[i]);
    if (val.isError())
      return val;
  }
  return val;
}

// Function: SUMSQ
KSpreadValue func_sumsq (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue res = 0;
  calc->arrayWalk (args, res, calc->awFunc ("sumsq"), 0);
  return res;
}

// Function: MAX
KSpreadValue func_max (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue res = args[0];
  while (res.isArray()) res = res.element (0,0);
  calc->arrayWalk (args, res, calc->awFunc ("max"), 0);
  return res;
}

// Function: MIN
KSpreadValue func_min (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue res = args[0];
  while (res.isArray()) res = res.element (0,0);
  calc->arrayWalk (args, res, calc->awFunc ("min"), 0);
  return res;
}

// Function: INT
KSpreadValue func_int (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->conv()->asInteger (args[0]);
}

// Function: QUOTIENT
KSpreadValue func_quotient (valVector args, ValueCalc *calc, FuncExtra *)
{
  if (calc->isZero (args[1]))
    return KSpreadValue::errorDIV0();
  return calc->conv()->asInteger (calc->div (args[0], args[1]));
}


// Function: eps
KSpreadValue func_eps (valVector, ValueCalc *calc, FuncExtra *)
{
  return calc->eps ();
}

KSpreadValue func_randexp (valVector args, ValueCalc *calc, FuncExtra *)
{
  // -1 * d * log (random)
  return calc->mul (calc->mul (args[0], -1), calc->random());
}

KSpreadValue func_randbinom (valVector args, ValueCalc *calc, FuncExtra *)
{
  // this function will not support arbitrary precision
  
  double d  = calc->conv()->asFloat (args[0]).asFloat();
  int    tr = calc->conv()->asInteger (args[1]).asInteger();

  if ( d < 0 || d > 1 )
    return KSpreadValue::errorVALUE();

  if ( tr < 0 )
    return KSpreadValue::errorVALUE();

  // taken from gnumeric
  double x = pow(1 - d, tr);
  double r = (double) rand() / ( RAND_MAX + 1.0 );
  double t = x;
  int i = 0;

  while (r > t)
  {
    x *= (((tr - i) * d) / ((1 + i) * (1 - d)));
    i++;
    t += x;
  }

  return KSpreadValue (i);
}

KSpreadValue func_randnegbinom (valVector args, ValueCalc *calc, FuncExtra *)
{
  // this function will not support arbitrary precision
  
  double d  = calc->conv()->asFloat (args[0]).asFloat();
  int    f = calc->conv()->asInteger (args[1]).asInteger();

  if ( d < 0 || d > 1 )
    return KSpreadValue::errorVALUE();

  if ( f < 0 )
    return KSpreadValue::errorVALUE();


  // taken from Gnumeric
  double x = pow(d, f);
  double r = (double) rand() / ( RAND_MAX + 1.0 );
  double t = x;
  int i = 0;

  while (r > t)
  {
    x *= ( ( ( f + i ) * ( 1 - d ) ) / (1 + i) ) ;
    i++;
    t += x;
  }

  return KSpreadValue (i);
}

KSpreadValue func_randbernoulli (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue rnd = calc->random ();
  return KSpreadValue (calc->greater (rnd, args[0]) ? 1.0 : 0.0);
}

KSpreadValue func_randnorm (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue mu = args[0];
  KSpreadValue sigma = args[1];

  //using polar form of the Box-Muller transformation
  //refer to http://www.taygeta.com/random/gaussian.html for more info

  KSpreadValue x1, x2, w;
  do {
    // x1,x2 = 2 * random() - 1
    x1 = calc->random (2.0);
    x2 = calc->random (2.0);
    x1 = calc->sub (x1, 1);
    x1 = calc->sub (x2, 1);
    w = calc->add (calc->sqr(x1), calc->sqr (x2));
  } while (calc->gequal (w, 1.0));   // w >= 1.0

  //sqrt ((-2.0 * log (w)) / w) :
  w = calc->sqrt (calc->div (calc->mul (-2.0, calc->ln (w)), w));
  KSpreadValue res = calc->mul (x1, w);

  res = calc->add (calc->mul (res, sigma), mu);  // res*sigma + mu
  return res;
}

KSpreadValue func_randpoisson (valVector args, ValueCalc *calc, FuncExtra *)
{
  if (calc->lower (args[0], 0))
    return KSpreadValue::errorVALUE();

  // taken from Gnumeric...
  KSpreadValue x = calc->exp (calc->mul (-1, args[0]));   // e^(-A)
  KSpreadValue r = calc->random ();
  KSpreadValue t = x;
  int i = 0;

  while (calc->greater (r, t)) {   // r > t
    x = calc->mul (x, calc->div (args[0], i + 1));  // x *= (A/(i+1))
    t = calc->add (t, x);    //t += x
    i++;
  }

  return KSpreadValue (i);
}

// Function: rand
KSpreadValue func_rand (valVector, ValueCalc *calc, FuncExtra *)
{
  return calc->random ();
}

// Function: RANDBETWEEN
KSpreadValue func_randbetween (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue v1 = args[0];
  KSpreadValue v2 = args[1];
  if (calc->greater (v2, v1)) {
    v1 = args[1];
    v2 = args[0];
  }
  return calc->add (v1, calc->random (calc->sub (v2, v1)));
}

// Function: POW
KSpreadValue func_pow (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->pow (args[0], args[1]);
}

// Function: MOD
KSpreadValue func_mod (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->mod (args[0], args[1]);
}

// Function: fact
KSpreadValue func_fact (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->fact (args[0]);
}

// Function: FACTDOUBLE
KSpreadValue func_factdouble (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->factDouble (args[0]);
}

// Function: MULTINOMIAL
KSpreadValue func_multinomial (valVector args, ValueCalc *calc, FuncExtra *)
{
  // (a+b+c)! / a!b!c!  (any number of params possible)
  KSpreadValue num = 0, den = 1;
  for (unsigned int i = 0; i < args.count(); ++i) {
    num = calc->add (num, args[i]);
    den = calc->mul (den, calc->fact (args[i]));
  }
  num = calc->fact (num);
  return calc->div (num, den);
}

// Function: sign
KSpreadValue func_sign (valVector args, ValueCalc *calc, FuncExtra *)
{
  return KSpreadValue (calc->sign (args[0]));
}

// Function: INV
KSpreadValue func_inv (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->mul (args[0], -1);
}

KSpreadValue func_mround (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue d = args[0];
  KSpreadValue m = args[1];

  // signs must be the same
  if ((calc->greater (d, 0) && calc->lower (m, 0))
      || (calc->lower (d, 0) && calc->greater (m, 0)))
    return KSpreadValue::errorVALUE();

  int sign = 1;

  if (calc->lower (d, 0))
  {
    sign = -1;
    d = calc->mul (d, -1);
    m = calc->mul (m, -1);
  }

  // from gnumeric:
  KSpreadValue mod = calc->mod (d, m);
  KSpreadValue div = calc->sub (d, mod);

  KSpreadValue result = div;
  if (calc->greater (mod, calc->div (m, 2)))  // mod > m/2
    result = calc->add (result, m);     // result += m
  result = calc->mul (result, sign);    // add the sign

  return result;
}

// Function: ROUNDDOWN
KSpreadValue func_rounddown (valVector args, ValueCalc *calc, FuncExtra *)
{
  if (args.count() == 2)
    return calc->roundDown (args[0], args[1]);
  return calc->roundDown (args[0], 0);
}

// Function: ROUNDUP
KSpreadValue func_roundup (valVector args, ValueCalc *calc, FuncExtra *)
{
  if (args.count() == 2)
    return calc->roundUp (args[0], args[1]);
  return calc->roundUp (args[0], 0);
}

// Function: ROUND
KSpreadValue func_round (valVector args, ValueCalc *calc, FuncExtra *)
{
  if (args.count() == 2)
    return calc->round (args[0], args[1]);
  return calc->round (args[0], 0);
}

// Function: EVEN
KSpreadValue func_even (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->isEven (args[0]);
}

// Function: ODD
KSpreadValue func_odd (valVector args, ValueCalc *calc, FuncExtra *)
{
  return (!calc->isEven (args[0]));
}

KSpreadValue func_trunc (valVector args, ValueCalc *calc, FuncExtra *)
{
  if (args.count() == 1)
    return calc->roundDown (args[0]);
  return calc->roundDown (args[0], args[1]);
}

// Function: COUNT
KSpreadValue func_count (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue res = 0;
  calc->arrayWalk (args, res, calc->awFunc ("count"), 0);
  return res;
}

// Function: COUNTA
KSpreadValue func_counta (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue res = 0;
  calc->arrayWalk (args, res, calc->awFunc ("counta"), 0);
  return res;
}

// Function: COUNTBLANK
KSpreadValue func_countblank (valVector args, ValueCalc *calc, FuncExtra *)
{
  // all without non-empty = empty
  return calc->sub (func_count (args, calc, 0), func_counta (args, calc, 0));
}

// Function: COUNTIF
KSpreadValue func_countif (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue range = args[0];
  QString condition = calc->conv()->asString (args[1]).asString();

  Condition cond;
  calc->getCond (cond, condition);

  return calc->countIf (range, cond);
}

// Function: FIB
KSpreadValue func_fib (valVector args, ValueCalc *calc, FuncExtra *)
{
/*
Lucas' formula for the nth Fibonacci number F(n) is given by

         ((1+sqrt(5))/2)^n - ((1-sqrt(5))/2)^n
  F(n) = ------------------------------------- .
                         sqrt(5)

*/
  KSpreadValue n = args[0];
  KSpreadValue s = calc->sqrt (5.0);
  // u1 = ((1+sqrt(5))/2)^n
  KSpreadValue u1 = calc->pow (calc->div (calc->add (1, s), 2), n);
  // u2 = ((1-sqrt(5))/2)^n
  KSpreadValue u2 = calc->pow (calc->div (calc->sub (1, s), 2), n);
  
  KSpreadValue result = calc->div (calc->sub (u1, u2), s);
  return result;
}

static KSpreadValue func_gcd_helper(const KSpreadValue &val, ValueCalc *calc)
{
  KSpreadValue res = 0;
  if (!val.isArray ())
    return val;
  for (unsigned int row = 0; row < val.rows(); ++row)
    for (unsigned int col = 0; col < val.columns(); ++col)
    {
      KSpreadValue v = val.element (col, row);
      if (v.isArray ())
        v = func_gcd_helper (v, calc);
      res = calc->gcd (res, v);
    }
}

// Function: GCD
KSpreadValue func_gcd (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue result = 0;
  for (unsigned int i = 0; i < args.count(); ++i)
    if (args[i].isArray())
      result = calc->gcd (result, func_gcd_helper (args[i], calc));
    else
      result = calc->gcd (result, args[i]);
}

static KSpreadValue func_lcm_helper(const KSpreadValue &val, ValueCalc *calc)
{
  KSpreadValue res = 0;
  if (!val.isArray ())
    return val;
  for (unsigned int row = 0; row < val.rows(); ++row)
    for (unsigned int col = 0; col < val.columns(); ++col)
    {
      KSpreadValue v = val.element (col, row);
      if (v.isArray ())
        v = func_lcm_helper (v, calc);
      res = calc->lcm (res, v);
    }
}

// Function: lcm
KSpreadValue func_lcm (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue result = 0;
  for (unsigned int i = 0; i < args.count(); ++i)
    if (args[i].isArray())
      result = calc->lcm (result, func_lcm_helper (args[i], calc));
    else
      result = calc->lcm (result, args[i]);
}


/** TODO continue here TODO **/
/** SUBTOTAL **/
#warning SUBTOTAL commented out

// Function: SUBTOTAL
// This function requires access to the KSpreadSheet and so on, because
// it needs to check whether cells contain the SUBTOTAL formula or not ...
// Cells containing a SUBTOTAL formula must be ignored.
KSpreadValue func_subtotal (valVector args, ValueCalc *calc, FuncExtra *)
{
/*
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
    func_stddev_helper( context, *list, result, average, false );
    context.setValue( new KSValue( sqrt( result / ((double) (countA - 1) ) ) ) );
    break;
   case 8: // StDevP
    func_stddev_helper( context, *list, result, average, false );
    context.setValue( new KSValue( sqrt( result / countA ) ) );
    break;
   case 9: // Sum
    context.setValue( new KSValue( sum ) );
    break;
   case 10: // Var
    func_variance_helper( context, *list, result, average, false );
    context.setValue( new KSValue( (double)(result / (countA - 1)) ) );
    break;
   case 11: // VarP
    func_variance_helper( context, *list, result, average, false );
    context.setValue( new KSValue( (double)(result / countA) ) );
    break;
   default:
    return false;
  }

  return true;

*/
}

/*
Commented out.
Absolutely no idea what this thing is supposed to do.
To anyone who would enable this code: it still uses koscript calls - you need
to convert it to the new style prior to uncommenting.

// Function: MULTIPLEOPERATIONS
KSpreadValue func_multipleOP (valVector args, ValueCalc *calc, FuncExtra *)
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

*/
