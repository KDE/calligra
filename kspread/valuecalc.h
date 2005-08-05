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

#ifndef KSPREAD_VALUECALC
#define KSPREAD_VALUECALC

#include "kspread_value.h"

#include <qvaluelist.h>
#include <qvaluevector.h>
#include <map>

class KSpreadDoc;

namespace KSpread {

class ValueCalc;
class ValueConverter;

// Condition structures
enum Comp { isEqual, isLess, isGreater, lessEqual, greaterEqual, notEqual };
enum Type { numeric, string };

struct Condition
{
  Comp     comp;
  int      index;
  double   value;
  QString  stringValue;
  Type     type;
};

typedef QValueList<Condition> ConditionList;

typedef void (*arrayWalkFunc) (ValueCalc *, KSpreadValue &result,
    KSpreadValue val, KSpreadValue param);

/**
The ValueCalc class is used to perform all sorts of calculations.
No other means of calculation should be performed, to achieve
transparency, and to ease addition of new datatypes.

Currently, most functions simply convert data to double and work with that.
The idea is such that after we add support for bigger precision, we only need
to adjust this class and the parsing/formatting/converting classes. All
function implementations will remain exactly the same.

Of course, for some functions, it might be impossible to apply them
on all datatypes, but since all of them can be applied on both
doubles and GnuMP-based numbers, that is not of much concern ;)
*/

class ValueCalc {
 public:
  ValueCalc (ValueConverter* c);

  ValueConverter *conv () { return converter; };
  
  // some functions need access to the document
  void setDoc (KSpreadDoc *d) { _doc = d; };
  KSpreadDoc *doc () { return _doc; };

  /** basic arithmetic operations */
  KSpreadValue add (const KSpreadValue &a, const KSpreadValue &b);
  KSpreadValue sub (const KSpreadValue &a, const KSpreadValue &b);
  KSpreadValue mul (const KSpreadValue &a, const KSpreadValue &b);
  KSpreadValue div (const KSpreadValue &a, const KSpreadValue &b);
  KSpreadValue mod (const KSpreadValue &a, const KSpreadValue &b);
  KSpreadValue pow (const KSpreadValue &a, const KSpreadValue &b);
  KSpreadValue sqr (const KSpreadValue &a);
  KSpreadValue sqrt (const KSpreadValue &a);
  KSpreadValue add (const KSpreadValue &a, double b);
  KSpreadValue sub (const KSpreadValue &a, double b);
  KSpreadValue mul (const KSpreadValue &a, double b);
  KSpreadValue div (const KSpreadValue &a, double b);
  KSpreadValue pow (const KSpreadValue &a, double b);
  KSpreadValue abs (const KSpreadValue &a);

  /** comparison and related */
  bool isZero (const KSpreadValue &a);
  bool isEven (const KSpreadValue &a);
  /** numerical comparison */
  bool equal (const KSpreadValue &a, const KSpreadValue &b);
  /** numerical comparison with a little epsilon tolerance */
  bool approxEqual (const KSpreadValue &a, const KSpreadValue &b);
  /** numerical comparison */
  bool greater (const KSpreadValue &a, const KSpreadValue &b);
  /** numerical comparison - greater or equal */
  bool gequal (const KSpreadValue &a, const KSpreadValue &b);
  /** numerical comparison */
  bool lower (const KSpreadValue &a, const KSpreadValue &b);
  /** string comparison */
  bool strEqual (const KSpreadValue &a, const KSpreadValue &b);
  int sign (const KSpreadValue &a);

  /** rounding */
  KSpreadValue roundDown (const KSpreadValue &a, const KSpreadValue &digits);
  KSpreadValue roundUp (const KSpreadValue &a, const KSpreadValue &digits);
  KSpreadValue round (const KSpreadValue &a, const KSpreadValue &digits);
  KSpreadValue roundDown (const KSpreadValue &a, int digits = 0);
  KSpreadValue roundUp (const KSpreadValue &a, int digits = 0);
  KSpreadValue round (const KSpreadValue &a, int digits = 0);
  
  /** logarithms and exponentials */
  KSpreadValue log (const KSpreadValue &number, const KSpreadValue &base);
  KSpreadValue log (const KSpreadValue &number, double base = 10);
  KSpreadValue ln (const KSpreadValue &number);
  KSpreadValue exp (const KSpreadValue &number);

  /** constants */
  KSpreadValue pi ();
  KSpreadValue eps ();
 
  /** random number from <0.0, range) */
  KSpreadValue random (double range = 1.0);
  KSpreadValue random (KSpreadValue range);
  
  /** some computational functions */
  KSpreadValue fact (const KSpreadValue &which);
  KSpreadValue fact (const KSpreadValue &which, const KSpreadValue &end);
  KSpreadValue fact (int which, int end = 0);
  /** double factorial (every other number multiplied) */
  KSpreadValue factDouble (int which);
  KSpreadValue factDouble (KSpreadValue which);

  /** combinations */
  KSpreadValue combin (int n, int k);
  KSpreadValue combin (KSpreadValue n, KSpreadValue k);
  
  /** greatest common divisor */
  KSpreadValue gcd (const KSpreadValue &a, const KSpreadValue &b);
  /** lowest common multiplicator */
  KSpreadValue lcm (const KSpreadValue &a, const KSpreadValue &b);

  /** base conversion 10 -> base */
  KSpreadValue base (const KSpreadValue &val, int base = 16, int prec = 0);
  /** base conversion base -> 10 */
  KSpreadValue fromBase (const KSpreadValue &val, int base = 16);

  /** goniometric functions */
  KSpreadValue sin (const KSpreadValue &number);
  KSpreadValue cos (const KSpreadValue &number);
  KSpreadValue tg (const KSpreadValue &number);
  KSpreadValue cotg (const KSpreadValue &number);
  KSpreadValue asin (const KSpreadValue &number);
  KSpreadValue acos (const KSpreadValue &number);
  KSpreadValue atg (const KSpreadValue &number);
  KSpreadValue atan2 (const KSpreadValue &y, const KSpreadValue &x);

  /** hyperbolic functions */
  KSpreadValue sinh (const KSpreadValue &number);
  KSpreadValue cosh (const KSpreadValue &number);
  KSpreadValue tgh (const KSpreadValue &number);
  KSpreadValue asinh (const KSpreadValue &number);
  KSpreadValue acosh (const KSpreadValue &number);
  KSpreadValue atgh (const KSpreadValue &number);

  /** some statistical stuff
    TODO: we may want to move these over to a separate class or something,
    as the functions are mostly big */
  KSpreadValue phi (KSpreadValue x);
  KSpreadValue gauss (KSpreadValue xx);
  KSpreadValue gaussinv (KSpreadValue xx);
  KSpreadValue GetGamma (KSpreadValue _x);
  KSpreadValue GetLogGamma (KSpreadValue _x);
  KSpreadValue GetGammaDist (KSpreadValue _x, KSpreadValue _alpha,
    KSpreadValue _beta);
  KSpreadValue GetBeta (KSpreadValue _x, KSpreadValue _alpha,
    KSpreadValue _beta);

  /** bessel functions - may also end up being separated from here */
  KSpreadValue besseli (KSpreadValue v, KSpreadValue x);
  KSpreadValue besselj (KSpreadValue v, KSpreadValue x);
  KSpreadValue besselk (KSpreadValue v, KSpreadValue x);
  KSpreadValue besseln (KSpreadValue v, KSpreadValue x);
  
  /** error functions (see: man erf) */
  KSpreadValue erf (KSpreadValue x);
  KSpreadValue erfc (KSpreadValue x);
  
  /** array/range walking */
  void arrayWalk (const KSpreadValue &range, KSpreadValue &res,
      arrayWalkFunc func, KSpreadValue param);
  /** Walk the array in function-like style.
  This method is here to avoid duplication in function handlers. */
  void arrayWalk (QValueVector<KSpreadValue> &range, KSpreadValue &res,
      arrayWalkFunc func, KSpreadValue param);
  void twoArrayWalk (const KSpreadValue &a1, const KSpreadValue &a2,
      KSpreadValue &res, arrayWalkFunc func);
  void twoArrayWalk (QValueVector<KSpreadValue> &a1,
      QValueVector<KSpreadValue> &a2, KSpreadValue &res, arrayWalkFunc func);
  arrayWalkFunc awFunc (const QString &name);
  void registerAwFunc (const QString &name, arrayWalkFunc func);

  /** basic range functions */
  // if full is true, A-version is used (means string/bool values included)
  KSpreadValue sum (const KSpreadValue &range, bool full = true);
  KSpreadValue sumsq (const KSpreadValue &range, bool full = true);
  KSpreadValue sumIf (const KSpreadValue &range,
      const KSpreadValue &checkRange, const Condition &cond);
  int count (const KSpreadValue &range, bool full = true);
  int countIf (const KSpreadValue &range, const Condition &cond);
  KSpreadValue avg (const KSpreadValue &range, bool full = true);
  KSpreadValue max (const KSpreadValue &range, bool full = true);
  KSpreadValue min (const KSpreadValue &range, bool full = true);
  KSpreadValue product (const KSpreadValue &range, KSpreadValue init,
      bool full = true);
  KSpreadValue stddev (const KSpreadValue &range, bool full = true);
  KSpreadValue stddev (const KSpreadValue &range, KSpreadValue avg,
      bool full = true);
  KSpreadValue stddevP (const KSpreadValue &range, bool full = true);
  KSpreadValue stddevP (const KSpreadValue &range, KSpreadValue avg,
      bool full = true);
  
  /** range functions using value lists */
  KSpreadValue sum (QValueVector<KSpreadValue> range, bool full = true);
  int count (QValueVector<KSpreadValue> range, bool full = true);
  KSpreadValue avg (QValueVector<KSpreadValue> range, bool full = true);
  KSpreadValue max (QValueVector<KSpreadValue> range, bool full = true);
  KSpreadValue min (QValueVector<KSpreadValue> range, bool full = true);
  KSpreadValue product (QValueVector<KSpreadValue> range, KSpreadValue init,
      bool full = true);
  KSpreadValue stddev (QValueVector<KSpreadValue> range, bool full = true);
  KSpreadValue stddev (QValueVector<KSpreadValue> range, KSpreadValue avg,
      bool full = true);
  KSpreadValue stddevP (QValueVector<KSpreadValue> range, bool full = true);
  KSpreadValue stddevP (QValueVector<KSpreadValue> range, KSpreadValue avg,
      bool full = true);

  /**
    This method parses the condition in string text to the condition cond.
    It sets the condition's type and value.
  */
  void getCond (Condition &cond, QString text);
  
  /**  
    Returns true if value d matches the condition cond, built with getCond().
    Otherwise, it returns false.
  */
  bool matches (const Condition &cond, KSpreadValue d);

 protected:
  ValueConverter* converter;
  /** return result formatting, based on these two values */
  KSpreadValue::Format format (KSpreadValue::Format a, KSpreadValue::Format b);
  
  KSpreadDoc *_doc;
  
  /** registered array-walk functions */
  std::map<QString, arrayWalkFunc> awFuncs;
};

}  //namespace KSpread


#endif // KSPREAD_VALUECALC

