/* This file is part of the KDE project
   Copyright (C) 2005-2007 Tomas Mecir <mecirt@gmail.com>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_VALUECALC
#define KSPREAD_VALUECALC

#include <map>

#include <QVector>

#include "Number.h"
#include "Value.h"

#include "kspread_export.h"

#ifdef max
# undef max
#endif
#ifdef min
# undef min
#endif

namespace KSpread
{
class Cell;
class Doc;
class ValueCalc;
class ValueConverter;

// Condition structures
enum Comp { isEqual, isLess, isGreater, lessEqual, greaterEqual, notEqual };
enum Type { numeric, string };

struct Condition {
    Comp     comp;
    int      index;
    Number   value;
    QString  stringValue;
    Type     type;
};

typedef void (*arrayWalkFunc)(ValueCalc *, Value &result,
                              Value val, Value param);
// A function that can map an array element-wise
typedef Value (ValueCalc::*arrayMapFunc)(const Value &val, const Value &param);

/**
 * \ingroup Value
The ValueCalc class is used to perform all sorts of calculations.

Usage of this class for simpler calculations is deprecated, as we now use
the Number object directly for that. This class is to be used for computations
of more complicated and ranged functions.
*/

class KSPREAD_EXPORT ValueCalc
{
public:
    explicit ValueCalc(ValueConverter* c);

    ValueConverter *conv() {
        return converter;
    }

    const CalculationSettings* settings() const;

    /** basic arithmetic operations */
    Value add(const Value &a, const Value &b);
    Value sub(const Value &a, const Value &b);
    Value mul(const Value &a, const Value &b);
    Value div(const Value &a, const Value &b);
    Value mod(const Value &a, const Value &b);
    Value pow(const Value &a, const Value &b);
    Value sqr(const Value &a);
    Value sqrt(const Value &a);
    Value add(const Value &a, Number b);
    Value sub(const Value &a, Number b);
    Value mul(const Value &a, Number b);
    Value div(const Value &a, Number b);
    Value pow(const Value &a, Number b);
    Value abs(const Value &a);

    /** comparison and related */
    bool isZero(const Value &a);
    bool isEven(const Value &a);
    /** numerical comparison */
    bool equal(const Value &a, const Value &b);
    /** numerical comparison with a little epsilon tolerance */
    bool approxEqual(const Value &a, const Value &b);
    /** numerical comparison */
    bool greater(const Value &a, const Value &b);
    /** numerical comparison - greater or equal */
    bool gequal(const Value &a, const Value &b);
    /** numerical comparison */
    bool lower(const Value &a, const Value &b);
    /** string comparison */
    bool strEqual(const Value &a, const Value &b, bool CS = true);
    /** string comparison */
    bool strGreater(const Value &a, const Value &b, bool CS = true);
    /** string comparison - greater or equal */
    bool strGequal(const Value &a, const Value &b, bool CS = true);
    /** string comparison */
    bool strLower(const Value &a, const Value &b, bool CS = true);
    /** string or numerical comparison */
    bool naturalEqual(const Value &a, const Value &b, bool CS = true);
    /** string or numerical comparison */
    bool naturalGreater(const Value &a, const Value &b, bool CS = true);
    /** string or numerical comparison - greater or equal */
    bool naturalGequal(const Value &a, const Value &b, bool CS = true);
    /** string or numerical comparison */
    bool naturalLower(const Value &a, const Value &b, bool CS = true);
    /** string or numerical comparison - lower or equal */
    bool naturalLequal(const Value &a, const Value &b, bool CS = true);

    int sign(const Value &a);

    // just a quick workaround
    Value add(Number a, const Value& b) {
        return add(Value(a), b);
    }
    Value sub(Number a, const Value& b) {
        return sub(Value(a), b);
    }
    Value mul(Number a, const Value& b) {
        return mul(Value(a), b);
    }
    Value div(Number a, const Value& b) {
        return div(Value(a), b);
    }
    Value pow(Number a, const Value& b) {
        return pow(Value(a), b);
    }

    bool equal(const Value &a, Number b)   {
        return equal(a, Value(b));
    }
    bool greater(const Value &a, Number b) {
        return greater(a, Value(b));
    }
    bool lower(const Value &a, Number b)   {
        return lower(a, Value(b));
    }
    bool equal(Number a, const Value &b)   {
        return equal(Value(a), b);
    }
    bool greater(Number a, const Value &b) {
        return greater(Value(a), b);
    }
    bool lower(Number a, const Value &b)   {
        return lower(Value(a), b);
    }


    /** rounding */
    Value roundDown(const Value &a, const Value &digits);
    Value roundUp(const Value &a, const Value &digits);
    Value round(const Value &a, const Value &digits);
    Value roundDown(const Value &a, int digits = 0);
    Value roundUp(const Value &a, int digits = 0);
    Value round(const Value &a, int digits = 0);

    /** logarithms and exponentials */
    Value log(const Value &number, const Value &base);
    Value log(const Value &number, Number base = 10);
    Value ln(const Value &number);
    Value exp(const Value &number);

    /** constants */
    Value pi();
    Value eps();

    /** random number from <0.0, range) */
    Value random(Number range = 1.0);
    Value random(Value range);

    /** some computational functions */
    Value fact(const Value &which);
    Value fact(const Value &which, const Value &end);
    Value fact(int which, int end = 0);
    /** Number factorial (every other number multiplied) */
    Value factDouble(int which);
    Value factDouble(Value which);

    /** combinations */
    Value combin(int n, int k);
    Value combin(Value n, Value k);

    /** greatest common divisor */
    Value gcd(const Value &a, const Value &b);
    /** lowest common multiplicator */
    Value lcm(const Value &a, const Value &b);

    /** base conversion 10 -> base */
    Value base(const Value &val, int base = 16, int prec = 0, int minLength = 0);
    /** base conversion base -> 10 */
    Value fromBase(const Value &val, int base = 16);

    /** goniometric functions */
    Value sin(const Value &number);
    Value cos(const Value &number);
    Value tg(const Value &number);
    Value cotg(const Value &number);
    Value asin(const Value &number);
    Value acos(const Value &number);
    Value atg(const Value &number);
    Value atan2(const Value &y, const Value &x);

    /** hyperbolic functions */
    Value sinh(const Value &number);
    Value cosh(const Value &number);
    Value tgh(const Value &number);
    Value asinh(const Value &number);
    Value acosh(const Value &number);
    Value atgh(const Value &number);

    /** some statistical stuff
      TODO: we may want to move these over to a separate class or something,
      as the functions are mostly big */
    Value phi(Value x);
    Value gauss(Value xx);
    Value gaussinv(Value xx);
    Value GetGamma(Value _x);
    Value GetLogGamma(Value _x);
    Value GetGammaDist(Value _x, Value _alpha,
                       Value _beta);
    Value GetBeta(Value _x, Value _alpha,
                  Value _beta);

    /** bessel functions - may also end up being separated from here */
    Value besseli(Value v, Value x);
    Value besselj(Value v, Value x);
    Value besselk(Value v, Value x);
    Value besseln(Value v, Value x);

    /** error functions (see: man erf) */
    Value erf(Value x);
    Value erfc(Value x);

    /** array/range walking */
    void arrayWalk(const Value &range, Value &res,
                   arrayWalkFunc func, Value param);
    /** Walk the array in function-like style.
    This method is here to avoid duplication in function handlers. */
    void arrayWalk(QVector<Value> &range, Value &res,
                   arrayWalkFunc func, Value param);
    Value arrayMap(const Value &array, arrayMapFunc func, const Value &param);
    Value twoArrayMap(const Value &array1, arrayMapFunc func, const Value &array2);
    void twoArrayWalk(const Value &a1, const Value &a2,
                      Value &res, arrayWalkFunc func);
    void twoArrayWalk(QVector<Value> &a1,
                      QVector<Value> &a2, Value &res, arrayWalkFunc func);
    arrayWalkFunc awFunc(const QString &name);
    void registerAwFunc(const QString &name, arrayWalkFunc func);

    /** basic range functions */
    // if full is true, A-version is used (means string/bool values included)
    Value sum(const Value &range, bool full = true);
    Value sumsq(const Value &range, bool full = true);
    Value sumIf(const Value &range, const Condition &cond);
    Value sumIf(const Cell &sumRangeStart,
                const Value &checkRange, const Condition &cond);
    int count(const Value &range, bool full = true);
    int countIf(const Value &range, const Condition &cond);
    Value avg(const Value &range, bool full = true);
    Value max(const Value &range, bool full = true);
    Value min(const Value &range, bool full = true);
    Value product(const Value &range, Value init,
                  bool full = true);
    Value stddev(const Value &range, bool full = true);
    Value stddev(const Value &range, Value avg,
                 bool full = true);
    Value stddevP(const Value &range, bool full = true);
    Value stddevP(const Value &range, Value avg,
                  bool full = true);

    /** range functions using value lists */
    Value sum(QVector<Value> range, bool full = true);
    int count(QVector<Value> range, bool full = true);
    Value avg(QVector<Value> range, bool full = true);
    Value max(QVector<Value> range, bool full = true);
    Value min(QVector<Value> range, bool full = true);
    Value product(QVector<Value> range, Value init,
                  bool full = true);
    Value stddev(QVector<Value> range, bool full = true);
    Value stddev(QVector<Value> range, Value avg,
                 bool full = true);
    Value stddevP(QVector<Value> range, bool full = true);
    Value stddevP(QVector<Value> range, Value avg,
                  bool full = true);

    /**
      This method parses the condition in string text to the condition cond.
      It sets the condition's type and value.
    */
    void getCond(Condition &cond, Value val);

    /**
      Returns true if value d matches the condition cond, built with getCond().
      Otherwise, it returns false.
    */
    bool matches(const Condition &cond, Value d);

    /** return formatting for the result, based on formattings of input values */
    Value::Format format(Value a, Value b);

protected:
    ValueConverter* converter;

    /** registered array-walk functions */
    std::map<QString, arrayWalkFunc> awFuncs;
};

inline bool approxEqual(double a, double b)
{
    if (a == b)
        return true;
    double x = a - b;
    return (x < 0.0 ? -x : x)
           < ((a < 0.0 ? -a : a) *(1.0 / (16777216.0 * 16777216.0)));
}

inline double approxFloor(double a)
{
    double b = floor(a);
    // The second approxEqual() is necessary for values that are near the limit
    // of numbers representable with 4 bits stripped off. (#i12446#)
    if (approxEqual(a - 1.0, b) && !approxEqual(a, b))
        return b + 1.0;
    return b;
}

}  //namespace KSpread


#endif // KSPREAD_VALUECALC

