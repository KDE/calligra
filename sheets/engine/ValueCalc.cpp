/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008-2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2005-2007 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ValueCalc.h"

#include "CellBase.h"
#include "ValueConverter.h"
#include "CalculationSettings.h"
#include "SheetsDebug.h"

#include <cfloat>

using namespace Calligra::Sheets;


//
// helper: gammaHelper
//
// args[0] = value
// args[1] = & reflect
//
static double GammaHelp(double& x, bool& reflect)
{
    double c[6] = { 76.18009173, -86.50532033    , 24.01409822,
                    -1.231739516,   0.120858003E-2, -0.536382E-5
                  };
    if (x >= 1.0) {
        reflect = false;
        x -= 1.0;
    } else {
        reflect = true;
        x = 1.0 - x;
    }
    double res, anum;
    res = 1.0;
    anum = x;
    for (int i = 0; i < 6; ++i) {
        anum += 1.0;
        res += c[i] / anum;
    }
    res *= 2.506628275; // sqrt(2*PI)

    return res;
}

static Value toValue(Number value) {
    // Can the result be represented as an integer?
    int64_t resint = static_cast<int64_t>(value);
    if ((Number)resint == value)
        return Value(resint);
    else
        return Value(value);
}


// Array-walk functions registered on ValueCalc object

void awSum(ValueCalc *c, Value &res, Value val, Value)
{
    if (val.isError())
        res = val;
    else if ((!val.isEmpty()) && (!val.isBoolean()) && (!val.isString()))
        res = c->add(res, val);
}

void awSumA(ValueCalc *c, Value &res, Value val, Value)
{
    if (!val.isEmpty())
        res = c->add(res, val);
}

void awSumSq(ValueCalc *c, Value &res, Value val, Value)
{
    // removed (!val.isBoolean()) to allow conversion from BOOL to int
    if ((!val.isEmpty()) && (!val.isString()) && (!val.isError()))
        res = c->add(res, c->sqr(val));
}

void awSumSqA(ValueCalc *c, Value &res, Value val, Value)
{
    if (!val.isEmpty())
        res = c->add(res, c->sqr(val));
}

void awCount(ValueCalc *c, Value &res, Value val, Value)
{
    if ((!val.isEmpty()) && (!val.isBoolean()) && (!val.isString()) && (!val.isError()))
        res = c->add(res, 1);
}

void awCountA(ValueCalc *c, Value &res, Value val, Value)
{
    if (!val.isEmpty())
        res = c->add(res, 1);
}

void awMax(ValueCalc *c, Value &res, Value val, Value)
{
    // propagate error values
    if (res.isError())
        return;
    if (val.isError())
        res = val;
    else if ((!val.isEmpty()) && (!val.isBoolean()) && (!val.isString())) {
        if (res.isEmpty()) {
            res = val;
        } else {
            if (c->greater(val, res)) res = val;
        }
    }
}

void awMaxA(ValueCalc *c, Value &res, Value val, Value)
{
    // propagate error values
    if (res.isError())
        return;
    if (val.isError()) {
        res = val;
    } else if (!val.isEmpty()) {
        if (res.isEmpty())
            // convert to number, so that we don't return string/bool
            res = c->conv()->asNumeric(val);
        else if (c->greater(val, res))
            // convert to number, so that we don't return string/bool
            res = c->conv()->asNumeric(val);
    }
}

void awMin(ValueCalc *c, Value &res, Value val, Value)
{
    if ((!val.isEmpty()) && (!val.isBoolean()) && (!val.isString())) {
        if (res.isEmpty())
            res = val;
        else if (c->lower(val, res)) res = val;
    }
}

void awMinA(ValueCalc *c, Value &res, Value val, Value)
{
    if (!val.isEmpty()) {
        if (res.isEmpty())
            // convert to number, so that we don't return string/bool
            res = c->conv()->asNumeric(val);
        else if (c->lower(val, res))
            // convert to number, so that we don't return string/bool
            res = c->conv()->asNumeric(val);
    }
}

void awProd(ValueCalc *c, Value &res, Value val, Value)
{
    if ((!val.isEmpty()) && (!val.isBoolean()) && (!val.isString()) && (!val.isError()))
        res = c->mul(res, val);
}

void awProdA(ValueCalc *c, Value &res, Value val, Value)
{
    if (!val.isEmpty())
        res = c->mul(res, val);
}

// sum of squares of deviations, used to compute standard deviation
void awDevSq(ValueCalc *c, Value &res, Value val,
             Value avg)
{
    if ((!val.isEmpty()) && (!val.isBoolean()) && (!val.isString()) && (!val.isError()))
        res = c->add(res, c->sqr(c->sub(val, avg)));
}

// sum of squares of deviations, used to compute standard deviation
void awDevSqA(ValueCalc *c, Value &res, Value val,
              Value avg)
{
    if (!val.isEmpty())
        res = c->add(res, c->sqr(c->sub(val, avg)));
}


// ***********************
// ****** ValueCalc ******
// ***********************

ValueCalc::ValueCalc(ValueConverter* c): converter(c)
{
    // initialize the random number generator
    srand(time(0));

    // register array-walk functions
    registerAwFunc("sum", awSum);
    registerAwFunc("suma", awSumA);
    registerAwFunc("sumsq", awSumSq);
    registerAwFunc("sumsqa", awSumSqA);
    registerAwFunc("count", awCount);
    registerAwFunc("counta", awCountA);
    registerAwFunc("max", awMax);
    registerAwFunc("maxa", awMaxA);
    registerAwFunc("min", awMin);
    registerAwFunc("mina", awMinA);
    registerAwFunc("prod", awProd);
    registerAwFunc("proda", awProdA);
    registerAwFunc("devsq", awDevSq);
    registerAwFunc("devsqa", awDevSqA);
}

const CalculationSettings* ValueCalc::settings() const
{
    return converter->settings();
}

Value ValueCalc::add(const Value &a, const Value &b)
{
    if (a.isError()) return a;
    if (b.isError()) return b;
    if (a.isArray() || b.isArray())
        return twoArrayMap(a, &ValueCalc::add, b);

    Number aa, bb;
    aa = converter->toFloat(a);
    bb = converter->toFloat(b);
    Value res = toValue(aa + bb);

    if (a.isNumber() || a.isEmpty())
        res.setFormat(format(a, b));

    return res;
}

Value ValueCalc::sub(const Value &a, const Value &b)
{
    if (a.isError()) return a;
    if (b.isError()) return b;
    if (a.isArray() || b.isArray())
        return twoArrayMap(a, &ValueCalc::sub, b);

    Number aa, bb;
    aa = converter->toFloat(a);
    bb = converter->toFloat(b);
    Value res = toValue(aa - bb);

    if (a.isNumber() || a.isEmpty())
        res.setFormat(format(a, b));

    return res;
}

Value ValueCalc::mul(const Value &a, const Value &b)
{
    if (a.isError()) return a;
    if (b.isError()) return b;
    // This operation is only defined for an array if it is multiplied
    // with a number, that however is commutative, thus swap parameters
    // if necessary.
    if (a.isArray() && !b.isArray())
       return arrayMap(a, &ValueCalc::mul, b);
    if (b.isArray() && !a.isArray())
       return arrayMap(b, &ValueCalc::mul, a);

    Number aa, bb;
    aa = converter->toFloat(a);
    bb = converter->toFloat(b);
    Value res = toValue(aa * bb);

    if (a.isNumber() || a.isEmpty())
        res.setFormat(format(a, b));

    return res;
}

Value ValueCalc::div(const Value &a, const Value &b)
{
    if (a.isError()) return a;
    if (b.isError()) return b;
    if (a.isArray() && !b.isArray())
       return arrayMap(a, &ValueCalc::div, b);

    Number aa, bb;
    aa = converter->toFloat(a);
    bb = converter->toFloat(b);
    Value res;
    if (bb == 0.0)
        return Value::errorDIV0();
    else
        res = toValue(aa / bb);

    if (a.isNumber() || a.isEmpty())
        res.setFormat(format(a, b));

    return res;
}

Value ValueCalc::mod(const Value &a, const Value &b)
{
    if (a.isError()) return a;
    if (b.isError()) return b;
    if (a.isArray() && !b.isArray())
       return arrayMap(a, &ValueCalc::mod, b);

    Number aa, bb;
    aa = converter->toFloat(a);
    bb = converter->toFloat(b);
    Value res;
    if (bb == 0.0)
        return Value::errorDIV0();
    else {
        Number m = fmod(aa, bb);
        // the following adjustments are needed by OpenFormula:
        // can't simply use fixed increases/decreases, because the implementation
        // of fmod may differ on various platforms, and we should always return
        // the same results ...
        if ((bb > 0) && (aa < 0))  // result must be positive here
            while (m < 0) m += bb;
        if (bb < 0) { // result must be negative here, but not lower than bb
            // bb is negative, hence the following two are correct
            while (m < bb) m -= bb;  // same as m+=fabs(bb)
            while (m > 0) m += bb;   // same as m-=fabs(bb)
        }

        res = Value(m);
    }

    if (a.isNumber() || a.isEmpty())
        res.setFormat(format(a, b));

    return res;
}

Value ValueCalc::pow(const Value &a, const Value &b)
{
    if (a.isError()) return a;
    if (b.isError()) return b;
    if (a.isArray() && !b.isArray())
       return arrayMap(a, &ValueCalc::pow, b);

    Number aa, bb;
    aa = converter->toFloat(a);
    bb = converter->toFloat(b);
    Value res(::pow(aa, bb));

    if (a.isNumber() || a.isEmpty())
        res.setFormat(format(a, b));

    return res;
}

Value ValueCalc::sqr(const Value &a)
{
    if (a.isError()) return a;
    return mul(a, a);
}

Value ValueCalc::sqrt(const Value &a)
{
    if (a.isError()) return a;
    Value res = Value(::pow((qreal)converter->toFloat(a), 0.5));
    if (a.isNumber() || a.isEmpty())
        res.setFormat(a.format());

    return res;
}

Value ValueCalc::add(const Value &a, Number b)
{
    if (a.isError()) return a;
    Value res = toValue(converter->toFloat(a) + b);

    if (a.isNumber() || a.isEmpty())
        res.setFormat(a.format());

    return res;
}

Value ValueCalc::sub(const Value &a, Number b)
{
    if (a.isError()) return a;
    Value res = toValue(converter->toFloat(a) - b);

    if (a.isNumber() || a.isEmpty())
        res.setFormat(a.format());

    return res;
}

Value ValueCalc::mul(const Value &a, Number b)
{
    if (a.isError()) return a;
    Value res = toValue(converter->toFloat(a) * b);

    if (a.isNumber() || a.isEmpty())
        res.setFormat(a.format());

    return res;
}

Value ValueCalc::div(const Value &a, Number b)
{
    if (a.isError()) return a;
    Value res;
    if (b == 0.0)
        return Value::errorDIV0();

    res = toValue(converter->toFloat(a) / b);

    if (a.isNumber() || a.isEmpty())
        res.setFormat(a.format());

    return res;
}

Value ValueCalc::pow(const Value &a, Number b)
{
    if (a.isError()) return a;
    Value res = Value(::pow(converter->toFloat(a), b));

    if (a.isNumber() || a.isEmpty())
        res.setFormat(a.format());

    return res;
}

Value ValueCalc::abs(const Value &a)
{
    if (a.isError()) return a;
    return toValue(fabs(converter->toFloat(a)));
}

bool ValueCalc::isZero(const Value &a)
{
    if (a.isError()) return false;
    return (converter->toFloat(a) == 0.0);
}

bool ValueCalc::isEven(const Value &a)
{
    if (a.isError()) return false;
    if (gequal(a, Value(0))) {
        return ((converter->toInteger(roundDown(a)) % 2) == 0);
    } else {
        return ((converter->toInteger(roundUp(a)) % 2) == 0);
    }
}

bool ValueCalc::equal(const Value &a, const Value &b)
{
    return (converter->toFloat(a) == converter->toFloat(b));
}

/*********************************************************************
 *
 * Helper function to avoid problems with rounding floating point
 * values. Idea for this kind of solution taken from Openoffice.
 *
 *********************************************************************/
bool ValueCalc::approxEqual(const Value &a, const Value &b)
{
    if ((a.isInteger()) && (b.isInteger())) {
        // avoid possible rounding
        return a.asInteger() == b.asInteger();
    }
    Number aa = converter->toFloat(a);
    Number bb = converter->toFloat(b);
    if (aa == bb)
        return true;
    Number x = aa - bb;
    return (x < 0.0 ? -x : x)  < ((aa < 0.0 ? -aa : aa) * 1e-14);
}

bool ValueCalc::greater(const Value &a, const Value &b)
{
    if ((a.isInteger()) && (b.isInteger())) {
        // avoid possible rounding
        return a.asInteger() > b.asInteger();
    }
    Number aa = converter->toFloat(a);
    Number bb = converter->toFloat(b);
    return (aa > bb);
}

bool ValueCalc::gequal(const Value &a, const Value &b)
{
    return (greater(a, b) || approxEqual(a, b));
}

bool ValueCalc::lower(const Value &a, const Value &b)
{
    return greater(b, a);
}

bool ValueCalc::strEqual(const Value &a, const Value &b, bool CalcS)
{
    QString aa = converter->asString(a).asString();
    QString bb = converter->asString(b).asString();
    if (!CalcS) {
        aa = aa.toLower();
        bb = bb.toLower();
    }
    return (aa == bb);
}

bool ValueCalc::strGreater(const Value &a, const Value &b, bool CalcS)
{
    QString aa = converter->asString(a).asString();
    QString bb = converter->asString(b).asString();
    if (!CalcS) {
        aa = aa.toLower();
        bb = bb.toLower();
    }
    return (aa > bb);
}

bool ValueCalc::strGequal(const Value &a, const Value &b, bool CalcS)
{
    QString aa = converter->asString(a).asString();
    QString bb = converter->asString(b).asString();
    if (!CalcS) {
        aa = aa.toLower();
        bb = bb.toLower();
    }
    return (aa >= bb);
}

bool ValueCalc::strLower(const Value &a, const Value &b, bool CalcS)
{
    return strGreater(b, a, CalcS);
}

bool ValueCalc::naturalEqual(const Value &a, const Value &b, bool CalcS)
{
    if (a.isNumber() && b.isNumber()) return approxEqual(a, b);
    if (a.allowComparison(b)) return a.equal(b, CalcS ? Qt::CaseSensitive : Qt::CaseInsensitive);
    return strEqual(a, b, CalcS);
}

bool ValueCalc::naturalGreater(const Value &a, const Value &b, bool CalcS)
{
    if (a.isNumber() && b.isNumber()) return greater(a, b);
    if (a.allowComparison(b)) return a.greater(b, CalcS ? Qt::CaseSensitive : Qt::CaseInsensitive);
    return strGreater(a, b, CalcS);
}

bool ValueCalc::naturalGequal(const Value &a, const Value &b, bool CalcS)
{
    return (naturalGreater(a, b, CalcS) || naturalEqual(a, b, CalcS));
}

bool ValueCalc::naturalLower(const Value &a, const Value &b, bool CalcS)
{
    return naturalGreater(b, a, CalcS);
}

bool ValueCalc::naturalLequal(const Value &a, const Value &b, bool CalcS)
{
    return (naturalLower(a, b, CalcS) || naturalEqual(a, b, CalcS));
}

Value ValueCalc::roundDown(const Value &a,
                           const Value &digits)
{
    return roundDown(a, converter->asInteger(digits).asInteger());
}

Value ValueCalc::roundUp(const Value &a,
                         const Value &digits)
{
    return roundUp(a, converter->asInteger(digits).asInteger());
}

Value ValueCalc::round(const Value &a,
                       const Value &digits)
{
    return round(a, converter->asInteger(digits).asInteger());
}

Value ValueCalc::roundDown(const Value &a, int digits)
{
    // shift in one direction, round, shift back
    Value val = a;
    if (digits > 0)
        for (int i = 0; i < digits; ++i)
            val = mul(val, 10);
    if (digits < 0)
        for (int i = 0; i > digits; --i)
            val = div(val, 10);

    val = Value(floor(numToDouble(converter->toFloat(val))));

    if (digits > 0)
        for (int i = 0; i < digits; ++i)
            val = div(val, 10);
    if (digits < 0)
        for (int i = 0; i > digits; --i)
            val = mul(val, 10);
    return val;
}

Value ValueCalc::roundUp(const Value &a, int digits)
{
    // shift in one direction, round, shift back
    Value val = a;
    if (digits > 0)
        for (int i = 0; i < digits; ++i)
            val = mul(val, 10);
    if (digits < 0)
        for (int i = 0; i > digits; --i)
            val = div(val, 10);

    val = Value(ceil(numToDouble(converter->toFloat(val))));

    if (digits > 0)
        for (int i = 0; i < digits; ++i)
            val = div(val, 10);
    if (digits < 0)
        for (int i = 0; i > digits; --i)
            val = mul(val, 10);
    return val;
}

Value ValueCalc::round(const Value &a, int digits)
{
    // shift in one direction, round, shift back
    Value val = a;
    if (digits > 0)
        for (int i = 0; i < digits; ++i)
            val = mul(val, 10);
    if (digits < 0)
        for (int i = 0; i > digits; --i)
            val = div(val, 10);

    val = Value((lower(val, 0) ? -1 : 1) * ::round(::fabsl(converter->toFloat(val))));

    if (digits > 0)
        for (int i = 0; i < digits; ++i)
            val = div(val, 10);
    if (digits < 0)
        for (int i = 0; i > digits; --i)
            val = mul(val, 10);
    return val;
}

int ValueCalc::sign(const Value &a)
{
    Number val = converter->toFloat(a);
    if (val == 0) return 0;
    if (val > 0) return 1;
    return -1;
}


Value ValueCalc::log(const Value &number,
                     const Value &base)
{
    Number logbase = converter->toFloat(base);
    if (logbase == 1.0)
        return Value::errorDIV0();
    if (logbase <= 0.0)
        return Value::errorNA();

    logbase = Calligra::Sheets::log(logbase, 10);
    Value res = Value(Calligra::Sheets::log(converter->toFloat(number), 10) / logbase);

    if (number.isNumber() || number.isEmpty())
        res.setFormat(number.format());

    return res;
}

Value ValueCalc::ln(const Value &number)
{
    Value res = Value(::ln(converter->toFloat(number)));

    if (number.isNumber() || number.isEmpty())
        res.setFormat(number.format());

    return res;
}

Value ValueCalc::log(const Value &number, Number base)
{
    if (base <= 0.0)
        return Value::errorNA();
    if (base == 1.0)
        return Value::errorDIV0();

    Number num = converter->toFloat(number);
    Value res = Value(Calligra::Sheets::log(num, base));

    if (number.isNumber() || number.isEmpty())
        res.setFormat(number.format());

    return res;
}

Value ValueCalc::exp(const Value &number)
{
    return Value(::exp(converter->toFloat(number)));
}

Value ValueCalc::pi()
{
    // return PI in double-precision
    // if arbitrary precision gets in, this should be extended to return
    // more if need be
    return Value(M_PI);
}

Value ValueCalc::eps()
{
    // #### This should adjust according to the actual number system used
    // (float, double, long double, ...)
    return Value(DBL_EPSILON);
}

Value ValueCalc::random(Number range)
{
    return Value(range *(double) rand() / (RAND_MAX + 1.0));
}

Value ValueCalc::random(Value range)
{
    return random(converter->toFloat(range));
}

Value ValueCalc::fact(const Value &which)
{
    // we can simply use integers - no one is going to compute factorial of
    // anything bigger than 2^64
    return fact(converter->asInteger(which).asInteger());
}

Value ValueCalc::fact(const Value &which,
                      const Value &end)
{
    // we can simply use integers - no one is going to compute factorial of
    // anything bigger than 2^64
    return fact(converter->asInteger(which).asInteger(),
                converter->asInteger(end).asInteger());
}

Value ValueCalc::fact(int which, int end)
{
    if (which < 0)
        return Value(-1);
    if (which == 0)
        return Value(1);
    Value res = Value(1);
    while (which > end) {
        res = mul(res, which);
        --which;
    }
    return res;
}

Value ValueCalc::factDouble(int which)
{
    if (which < 0)
        return Value(-1);
    if ((which == 0) || (which == 1))
        return Value(1);

    Value res = Value(1);
    while (which > 1) {
        res = mul(res, which);
        which -= 2;
    }
    return res;
}

Value ValueCalc::factDouble(Value which)
{
    return factDouble(converter->asInteger(which).asInteger());
}

Value ValueCalc::combin(int n, int k)
{
    if (n >= 15) {
        Number result = ::exp(Number(lgamma(n + 1) - lgamma(k + 1) - lgamma(n - k + 1)));
        return Value(floor(numToDouble(result + 0.5)));
    } else
        return div(div(fact(n), fact(k)), fact(n - k));
}

Value ValueCalc::combin(Value n, Value k)
{
    int nn = converter->toInteger(n);
    int kk = converter->toInteger(k);
    return combin(nn, kk);
}

Value ValueCalc::gcd(const Value &a, const Value &b)
{
    // Euler's GCD algorithm
    Value aa = round(a);
    Value bb = round(b);

    if (approxEqual(aa, bb)) return aa;

    if (aa.isZero()) return bb;
    if (bb.isZero()) return aa;


    if (greater(aa, bb))
        return gcd(bb, mod(aa, bb));
    else
        return gcd(aa, mod(bb, aa));
}

Value ValueCalc::lcm(const Value &a, const Value &b)
{
    Value aa = round(a);
    Value bb = round(b);

    if (approxEqual(aa, bb)) return aa;

    if (aa.isZero()) return bb;
    if (bb.isZero()) return aa;

    Value g = gcd(aa, bb);
    if (g.isZero())  // GCD is zero for some weird reason
        return mul(aa, bb);

    return div(mul(aa, bb), g);
}

Value ValueCalc::base(const Value &val, int base, int prec, int minLength)
{
    if (prec < 0) prec = 2;
    if ((base < 2) || (base > 36))
        return Value::errorVALUE();

    Number value = converter->toFloat(val);
    QString result = QString::number((int)numToDouble(value), base);
    if (result.length() < minLength)
        result = result.rightJustified(minLength, QChar('0'));

    if (prec > 0) {
        result += '.'; value = value - (int)numToDouble(value);

        int ix;
        for (int i = 0; i < prec; ++i) {
            ix = (int) numToDouble(value * base);
            result += "0123456789abcdefghijklmnopqrstuvwxyz"[ix];
            value = base * (value - (double)ix / base);
        }
    }

    return Value(result.toUpper());
}

Value ValueCalc::fromBase(const Value &val, int base)
{
    QString str = converter->asString(val).asString();
    bool ok;
    int64_t num = (int64_t) str.toLongLong(&ok, base);
    if (ok)
        return Value(num);
    return Value::errorVALUE();
}


Value ValueCalc::sin(const Value &number)
{
    bool ok = true;
    Number n = converter->asFloat(number, &ok).asFloat();
    if (!ok)
        return Value::errorVALUE();

    Value res = Value(::sin(n));

    if (number.isNumber() || number.isEmpty())
        res.setFormat(number.format());

    return res;
}

Value ValueCalc::cos(const Value &number)
{
    bool ok = true;
    Number n = converter->asFloat(number, &ok).asFloat();
    if (!ok)
        return Value::errorVALUE();

    Value res = Value(::cos(n));

    if (number.isNumber() || number.isEmpty())
        res.setFormat(number.format());

    return res;
}

Value ValueCalc::tg(const Value &number)
{
    Value res = Value(::tg(converter->toFloat(number)));

    if (number.isNumber() || number.isEmpty())
        res.setFormat(number.format());

    return res;
}

Value ValueCalc::cotg(const Value &number)
{
    Value res = div(1, Value(::tg(converter->toFloat(number))));

    if (number.isNumber() || number.isEmpty())
        res.setFormat(number.format());

    return res;
}

Value ValueCalc::asin(const Value &number)
{
    bool ok = true;
    Number n = converter->asFloat(number, &ok).asFloat();
    if (!ok)
        return Value::errorVALUE();
    const double d = numToDouble(n);
    if (d < -1.0 || d > 1.0)
        return Value::errorVALUE();

    errno = 0;
    Value res = Value(::asin(n));
    if (errno)
        return Value::errorVALUE();

    if (number.isNumber() || number.isEmpty())
        res.setFormat(number.format());
    return res;
}

Value ValueCalc::acos(const Value &number)
{
    bool ok = true;
    Number n = converter->asFloat(number, &ok).asFloat();
    if (!ok)
        return Value::errorVALUE();
    const double d = numToDouble(n);
    if (d < -1.0 || d > 1.0)
        return Value::errorVALUE();

    errno = 0;
    Value res = Value(::acos(n));
    if (errno)
        return Value::errorVALUE();

    if (number.isNumber() || number.isEmpty())
        res.setFormat(number.format());
    return res;
}

Value ValueCalc::atg(const Value &number)
{
    errno = 0;
    Value res = Value(::atg(converter->toFloat(number)));
    if (errno)
        return Value::errorVALUE();

    if (number.isNumber() || number.isEmpty())
        res.setFormat(number.format());

    return res;
}

Value ValueCalc::atan2(const Value &y, const Value &x)
{
    Number yy = converter->toFloat(y);
    Number xx = converter->toFloat(x);
    return Value(::atan2(yy, xx));
}

Value ValueCalc::sinh(const Value &number)
{
    Value res = Value(::sinh(converter->toFloat(number)));

    if (number.isNumber() || number.isEmpty())
        res.setFormat(number.format());

    return res;
}

Value ValueCalc::cosh(const Value &number)
{
    Value res = Value(::cosh(converter->toFloat(number)));

    if (number.isNumber() || number.isEmpty())
        res.setFormat(number.format());

    return res;
}

Value ValueCalc::tgh(const Value &number)
{
    Value res = Value(::tgh(converter->toFloat(number)));

    if (number.isNumber() || number.isEmpty())
        res.setFormat(number.format());

    return res;
}

Value ValueCalc::asinh(const Value &number)
{
    errno = 0;
    Value res = Value(::asinh(converter->toFloat(number)));
    if (errno)
        return Value::errorVALUE();

    if (number.isNumber() || number.isEmpty())
        res.setFormat(number.format());

    return res;
}

Value ValueCalc::acosh(const Value &number)
{
    errno = 0;
    Value res = Value(::acosh(converter->toFloat(number)));
    if (errno)
        return Value::errorVALUE();

    if (number.isNumber() || number.isEmpty())
        res.setFormat(number.format());

    return res;
}

Value ValueCalc::atgh(const Value &number)
{
    errno = 0;
    Value res = Value(::atgh(converter->toFloat(number)));
    if (errno)
        return Value::errorVALUE();

    if (number.isNumber() || number.isEmpty())
        res.setFormat(number.format());

    return res;
}

Value ValueCalc::phi(Value x)
{
    Value constant(0.39894228040143268);

    // constant * exp(-(x * x) / 2.0);
    Value x2neg = mul(sqr(x), -1);
    return mul(constant, exp(div(x2neg, 2.0)));
}

static double taylor_helper(double* pPolynom, uint nMax, double x)
{
    double nVal = pPolynom[nMax];
    for (int i = nMax - 1; i >= 0; --i) {
        nVal = pPolynom[i] + (nVal * x);
    }
    return nVal;
}


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

Value ValueCalc::gauss(Value xx)
// this is a weird function
{
    double x = converter->toFloat(xx);

    double t0[] = { 0.39894228040143268, -0.06649038006690545,  0.00997355701003582,
                    -0.00118732821548045,  0.00011543468761616, -0.00000944465625950,
                    0.00000066596935163, -0.00000004122667415,  0.00000000227352982,
                    0.00000000011301172,  0.00000000000511243, -0.00000000000021218
                  };
    double t2[] = { 0.47724986805182079,  0.05399096651318805, -0.05399096651318805,
                    0.02699548325659403, -0.00449924720943234, -0.00224962360471617,
                    0.00134977416282970, -0.00011783742691370, -0.00011515930357476,
                    0.00003704737285544,  0.00000282690796889, -0.00000354513195524,
                    0.00000037669563126,  0.00000019202407921, -0.00000005226908590,
                    -0.00000000491799345,  0.00000000366377919, -0.00000000015981997,
                    -0.00000000017381238,  0.00000000002624031,  0.00000000000560919,
                    -0.00000000000172127, -0.00000000000008634,  0.00000000000007894
                  };
    double t4[] = { 0.49996832875816688,  0.00013383022576489, -0.00026766045152977,
                    0.00033457556441221, -0.00028996548915725,  0.00018178605666397,
                    -0.00008252863922168,  0.00002551802519049, -0.00000391665839292,
                    -0.00000074018205222,  0.00000064422023359, -0.00000017370155340,
                    0.00000000909595465,  0.00000000944943118, -0.00000000329957075,
                    0.00000000029492075,  0.00000000011874477, -0.00000000004420396,
                    0.00000000000361422,  0.00000000000143638, -0.00000000000045848
                  };
    double asympt[] = { -1.0, 1.0, -3.0, 15.0, -105.0 };

    double xAbs = fabs(x);
    uint xShort = static_cast<uint>(approxFloor(xAbs)); // approxFloor taken from OOo
    double nVal = 0.0;
    if (xShort == 0)
        nVal = taylor_helper(t0, 11, (xAbs * xAbs)) * xAbs;
    else if ((xShort >= 1) && (xShort <= 2))
        nVal = taylor_helper(t2, 23, (xAbs - 2.0));
    else if ((xShort >= 3) && (xShort <= 4))
        nVal = taylor_helper(t4, 20, (xAbs - 4.0));
    else {
        double phiAbs = numToDouble(converter->toFloat(phi(Value(xAbs))));
        nVal = 0.5 + phiAbs * taylor_helper(asympt, 4, 1.0 / (xAbs * xAbs)) / xAbs;
    }

    if (x < 0.0)
        return Value(-nVal);
    else
        return Value(nVal);
}

Value ValueCalc::gaussinv(Value xx)
// this is a weird function
{
    double x = numToDouble(converter->toFloat(xx));

    double q, t, z;

    q = x - 0.5;

    if (fabs(q) <= .425) {
        t = 0.180625 - q * q;

        z =
            q *
            (
                (
                    (
                        (
                            (
                                (
                                    (
                                        t * 2509.0809287301226727 + 33430.575583588128105
                                    )
                                    * t + 67265.770927008700853
                                )
                                * t + 45921.953931549871457
                            )
                            * t + 13731.693765509461125
                        )
                        * t + 1971.5909503065514427
                    )
                    * t + 133.14166789178437745
                )
                * t + 3.387132872796366608
            )
            /
            (
                (
                    (
                        (
                            (
                                (
                                    (
                                        t * 5226.495278852854561 + 28729.085735721942674
                                    )
                                    * t + 39307.89580009271061
                                )
                                * t + 21213.794301586595867
                            )
                            * t + 5394.1960214247511077
                        )
                        * t + 687.1870074920579083
                    )
                    * t + 42.313330701600911252
                )
                * t + 1.0
            );

    } else {
        if (q > 0)  t = 1 - x;
        else    t = x;

        t =::sqrt(-::log(t));

        if (t <= 5.0) {
            t += -1.6;

            z =
                (
                    (
                        (
                            (
                                (
                                    (
                                        (
                                            t * 7.7454501427834140764e-4 + 0.0227238449892691845833
                                        )
                                        * t + 0.24178072517745061177
                                    )
                                    * t + 1.27045825245236838258
                                )
                                * t + 3.64784832476320460504
                            )
                            * t + 5.7694972214606914055
                        )
                        * t + 4.6303378461565452959
                    )
                    * t + 1.42343711074968357734
                )
                /
                (
                    (
                        (
                            (
                                (
                                    (
                                        (
                                            t * 1.05075007164441684324e-9 + 5.475938084995344946e-4
                                        )
                                        * t + 0.0151986665636164571966
                                    )
                                    * t + 0.14810397642748007459
                                )
                                * t + 0.68976733498510000455
                            )
                            * t + 1.6763848301838038494
                        )
                        * t + 2.05319162663775882187
                    )
                    * t + 1.0
                );

        } else {
            t += -5.0;

            z =
                (
                    (
                        (
                            (
                                (
                                    (
                                        (
                                            t * 2.01033439929228813265e-7 + 2.71155556874348757815e-5
                                        )
                                        * t + 0.0012426609473880784386
                                    )
                                    * t + 0.026532189526576123093
                                )
                                * t + 0.29656057182850489123
                            )
                            * t + 1.7848265399172913358
                        )
                        * t + 5.4637849111641143699
                    )
                    * t + 6.6579046435011037772
                )
                /
                (
                    (
                        (
                            (
                                (
                                    (
                                        (
                                            t * 2.04426310338993978564e-15 + 1.4215117583164458887e-7
                                        )
                                        * t + 1.8463183175100546818e-5
                                    )
                                    * t + 7.868691311456132591e-4
                                )
                                * t + 0.0148753612908506148525
                            )
                            * t + 0.13692988092273580531
                        )
                        * t + 0.59983220655588793769
                    )
                    * t + 1.0
                );

        }

        if (q < 0.0) z = -z;
    }

    return Value(z);
}

//
// GetGamma
//
Value ValueCalc::GetGamma(Value value)
{
    double val = conv()->asFloat(value).asFloat();

    bool reflect;

    double gamma = GammaHelp(val, reflect);

    gamma = ::pow(val + 5.5, val + 0.5) * gamma /::exp(val + 5.5);

    if (reflect)
        gamma = M_PI * val / (gamma*::sin(M_PI * val));

    return Value(gamma);
}

Value ValueCalc::GetLogGamma(Value _x)
{
    double x = numToDouble(converter->toFloat(_x));

    bool bReflect;
    double G = GammaHelp(x, bReflect);
    G = (x + 0.5)*::log(x + 5.5) +::log(G) - (x + 5.5);
    if (bReflect)
        G = ::log(M_PI * x) - G -::log(::sin(M_PI * x));
    return Value(G);
}

// Value ValueCalc::GetGammaDist (Value _x, Value _alpha,
//     Value _beta)
// {
//   double x = numToDouble (converter->toFloat (_x));
//   double alpha = numToDouble (converter->toFloat (_alpha));
//   double beta = numToDouble (converter->toFloat (_beta));
//
//   if (x == 0.0)
//     return Value (0.0);
//
//   x /= beta;
//   double gamma = alpha;
//
//   double c = 0.918938533204672741;
//   double d[10] = {
//     0.833333333333333333E-1,
//     -0.277777777777777778E-2,
//     0.793650793650793651E-3,
//     -0.595238095238095238E-3,
//     0.841750841750841751E-3,
//     -0.191752691752691753E-2,
//     0.641025641025641025E-2,
//     -0.295506535947712418E-1,
//     0.179644372368830573,
//     -0.139243221690590111E1
//   };
//
//   double dx = x;
//   double dgamma = gamma;
//   int maxit = 10000;
//
//   double z = dgamma;
//   double den = 1.0;
//   while ( z < 10.0 ) {
//     den *= z;
//     z += 1.0;
//   }
//
//   double z2 = z*z;
//   double z3 = z*z2;
//   double z4 = z2*z2;
//   double z5 = z2*z3;
//   double a = ( z - 0.5 ) * ::log10(z) - z + c;
//   double b = d[0]/z + d[1]/z3 + d[2]/z5 + d[3]/(z2*z5) + d[4]/(z4*z5) +
//     d[5]/(z*z5*z5) + d[6]/(z3*z5*z5) + d[7]/(z5*z5*z5) + d[8]/(z2*z5*z5*z5);
//   // double g = exp(a+b) / den;
//
//   double sum = 1.0 / dgamma;
//   double term = 1.0 / dgamma;
//   double cut1 = dx - dgamma;
//   double cut2 = dx * 10000000000.0;
//
//   for ( int i=1; i<=maxit; ++i ) {
//     double ai = i;
//     term = dx * term / ( dgamma + ai );
//     sum += term;
//     double cutoff = cut1 + ( cut2 * term / sum );
//     if ( ai > cutoff ) {
//       double t = sum;
//       // return pow( dx, dgamma ) * exp( -dx ) * t / g;
//       return Value (::exp( dgamma * ::log(dx) - dx - a - b ) * t * den);
//     }
//   }
//
//   return Value (1.0);             // should not happen ...
// }

Value ValueCalc::GetGammaDist(Value _x, Value _alpha, Value _beta)
{
    // Algorithm adopted from StatLib (http://lib.stat.cmu.edu/apstat/239):
    // Algorithm AS 239: Chi-Squared and Incomplete Gamma Integral
    // B. L. Shea
    // Applied Statistics, Vol. 37, No. 3 (1988), pp. 466-473

// TODO - normal approx

    double x     = numToDouble(converter->toFloat(_x));       // x
    double alpha = numToDouble(converter->toFloat(_alpha));   // alpha
    double beta  = numToDouble(converter->toFloat(_beta));    // beta

    // debug info
//   debugSheets<<"GetGammaDist( x="<<x<<", alpha="<<alpha<<", beta="<<beta<<" )";

    int lower_tail = 1; //
    int pearson;      // flag is set if pearson was used

    const static double xlarge = 1.0e+37;

    double res; // result
    double pn1, pn2, pn3, pn4, pn5, pn6, a, b, c, an, osum, sum;
    long n;

    x /= beta;
//   debugSheets<<"-> x=x/beta ="<<x;

    // check constraints
    if (x <= 0.0)
        return Value(0.0);

    if (x <= 1.0 || x < alpha) {
        //
        // Result = e^log(alpha * log(x) - x - log( gamma( alpha+1 ) ) )
        //

        pearson = 1; // set flag -> use pearson's series expansion.
        res = alpha * ::log(x) - x - ::log(GetGamma(Value(alpha + 1.0)).asFloat());
//     debugSheets<<"Pearson  res="<<res;

        //                 x           x           x
        // sum = 1.0 + --------- +  --------- * --------- + ...
        //              alpha+1      alpha+1     alpha+2
        c   = 1.0;
        sum = 1.0;
        a = alpha;
        do {
            a += 1.0;
            c *= x / a;
            sum += c;
        } while (c > DBL_EPSILON * sum);
    } else {
        // x >= max( 1, alpha)
        pearson = 0; // clear flag -> use a continued fraction expansion

// TODO use GetLogGamma?
        res = alpha * ::log(x) - x - ::log(GetGamma(Value(alpha)).asFloat());

//     debugSheets<<"Continued fraction expression res="<<res;

        //
        //
        //

        a = 1.0 - alpha;
        b = a + x + 1.0;
        pn1 = 1.0;
        pn2 = x;
        pn3 = x + 1.0;
        pn4 = x * b;
        sum = pn3 / pn4;
        for (n = 1; ; ++n) {
//       debugSheets<<"n="<<n<<" sum="<< sum;
            a += 1.0; // =   n+1 -alpha
            b += 2.0; // = 2(n+1)-alph+x
            an = a * n;
            pn5 = b * pn3 - an * pn1;
            pn6 = b * pn4 - an * pn2;
//       debugSheets<<"a ="<<a<<" an="<<an<<" b="<<b<<" pn5="<<pn5<<" pn6="<<pn6;
            if (fabs(pn6) > 0.0) {
                osum = sum;
                sum = pn5 / pn6;
//         debugSheets<<"sum ="<<sum<<" osum="<<osum;
                if (fabs(osum - sum) <= DBL_EPSILON * fmin(1.0, sum))
                    break;
            }
            pn1 = pn3;
            pn2 = pn4;
            pn3 = pn5;
            pn4 = pn6;
            if (fabs(pn5) >= xlarge) {
                // re-scale the terms in continued fraction if they are large
                debugSheets << "the terms are to large -> rescaleling by " << xlarge;
                pn1 /= xlarge;
                pn2 /= xlarge;
                pn3 /= xlarge;
                pn4 /= xlarge;
            }
        }
    }

    res += ::log(sum);
    lower_tail = (lower_tail == pearson);

    if (lower_tail)
        return Value(::exp(res));
    else
        return Value(-1 * ::expm1(res));
}

Value ValueCalc::GetBeta(Value _x, Value _alpha,
                         Value _beta)
{
//   debugSheets<<"GetBeta: x= " << _x << " alpha= " << _alpha << " beta=" << _beta;
    if (equal(_beta, Value(1.0)))
        return pow(_x, _alpha);
    else if (equal(_alpha, Value(1.0)))
        // 1.0 - pow (1.0-_x, _beta)
        return sub(Value(1.0), pow(sub(Value(1.0), _x), _beta));

    double x = numToDouble(converter->toFloat(_x));
    double alpha = numToDouble(converter->toFloat(_alpha));
    double beta = numToDouble(converter->toFloat(_beta));

    double fEps = 1.0E-8;
    bool bReflect;
    double cf, fA, fB;

    if (x < (alpha + 1.0) / (alpha + beta + 1.0)) {
        bReflect = false;
        fA = alpha;
        fB = beta;
    } else {
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
        b2 = 1.0 - (fA + fB) * x / (fA + 1.0);
        if (b2 == 0.0) {
            a2 = b2;
            fnorm = 1.0;
            cf = 1.0;
        } else {
            a2 = 1.0;
            fnorm = 1.0 / b2;
            cf = a2 * fnorm;
        }
        cfnew = 1.0;
        for (uint j = 1; j <= 100; ++j) {
            rm = (double) j;
            apl2m = fA + 2.0 * rm;
            d2m = rm * (fB - rm) * x / ((apl2m - 1.0) * apl2m);
            d2m1 = -(fA + rm) * (fA + fB + rm) * x / (apl2m * (apl2m + 1.0));
            a1 = (a2 + d2m * a1) * fnorm;
            b1 = (b2 + d2m * b1) * fnorm;
            a2 = a1 + d2m1 * a2 * fnorm;
            b2 = b1 + d2m1 * b2 * fnorm;
            if (b2 != 0.0) {
                fnorm = 1.0 / b2;
                cfnew = a2 * fnorm;
                if (fabs(cf - cfnew) / cf < fEps)
                    j = 101;
                else
                    cf = cfnew;
            }
        }
        if (fB < fEps)
            b1 = 1.0E30;
        else
            b1 = ::exp(numToDouble(GetLogGamma(Value(fA)).asFloat() + GetLogGamma(Value(fB)).asFloat() -
                                   GetLogGamma(Value(fA + fB)).asFloat()));

        cf *= ::pow(x, fA)*::pow(1.0 - x, fB) / (fA * b1);
    }
    if (bReflect)
        return Value(1.0 - cf);
    else
        return Value(cf);
}

// ------------------------------------------------------


/*
 *
 * The code for calculating Bessel functions is taken
 * from CCMATH, a mathematics library source.code.
 *
 * Original copyright follows:
 *
 *  SPDX-FileCopyrightText: 2000 Daniel A. Atkinson All rights reserved.
 *  This code may be redistributed under the terms of the GNU library
 *  public license (LGPL).
 */

static double ccmath_gaml(double x)
{
    double g, h = 0; /* NB must be called with 0<=x<29 */
    for (g = 1.; x < 30. ; g *= x, x += 1.) h = x * x;
    g = (x - .5) * log(x) - x + .918938533204672 - log(g);
    g += (1. - (1. / 6. - (1. / 3. - 1. / (4.*h)) / (7.*h)) / (5.*h)) / (12.*x);
    return g;
}

static double ccmath_psi(int m)
{
    double s = -.577215664901533; int k;
    for (k = 1; k < m ; ++k) s += 1. / k;
    return s;
}

static double ccmath_ibes(double v, double x)
{
    double y, s = 0., t = 0., tp; int p, m;
    y = x - 9.; if (y > 0.) y *= y; tp = v * v * .2 + 25.;
    if (y < tp) {
        x /= 2.; m = (int)x;
        if (x > 0.) s = t = exp(v * log(x) - ccmath_gaml(v + 1.));
        else {
            if (v > 0.) return 0.; else if (v == 0.) return 1.;
        }
        for (p = 1, x *= x;; ++p) {
            t *= x / (p * (v += 1.)); s += t;
            if (p > m && t < 1.e-13*s) break;
        }
    } else {
        double u, a0 = 1.57079632679490;
        s = t = 1. / sqrt(x * a0); x *= 2.; u = 0.;
        for (p = 1, y = .5; (tp = fabs(t)) > 1.e-14 ; ++p, y += 1.) {
            t *= (v + y) * (v - y) / (p * x); if (y > v && fabs(t) >= tp) break;
            if (!(p&1)) s += t; else u -= t;
        }
        x /= 2.; s = cosh(x) * s + sinh(x) * u;
    }
    return s;
}

static double ccmath_kbes(double v, double x)
{
    double y, s, t, tp, f, a0 = 1.57079632679490;
    int p, k, m;
    if (x == 0.) return HUGE_VAL;
    y = x - 10.5; if (y > 0.) y *= y; tp = 25. + .185 * v * v;
    if (y < tp && modf(v + .5, &t) != 0.) {
        y = 1.5 + .5 * v;
        if (x < y) {
            x /= 2.; m = (int)x; tp = t = exp(v * log(x) - ccmath_gaml(v + 1.));
            if (modf(v, &y) == 0.) {
                k = (int)y; tp *= v;
                f = 2.*log(x) - ccmath_psi(1) - ccmath_psi(k + 1);
                t /= 2.; if (!(k&1)) t = -t; s = f * t;
                for (p = 1, x *= x;; ++p) {
                    f -= 1. / p + 1. / (v += 1.);
                    t *= x / (p * v); s += (y = t * f);
                    if (p > m && fabs(y) < 1.e-14) break;
                }
                if (k > 0) {
                    x = -x; s += (t = 1. / (tp * 2.));
                    for (p = 1, --k; k > 0 ; ++p, --k) s += (t *= x / (p * k));
                }
            } else {
                f = 1. / (t * v * 2.); t *= a0 / sin(2.*a0 * v); s = f - t;
                for (p = 1, x *= x, tp = v;; ++p) {
                    t *= x / (p * (v += 1.)); f *= -x / (p * (tp -= 1.));
                    s += (y = f - t); if (p > m && fabs(y) < 1.e-14) break;
                }
            }
        } else {
            double tq, h, w, z, r;
            t = 12. / pow(x, .333); k = (int)(t * t); y = 2.*(x + k);
            m = (int)v; v -= m; tp = v * v - .25; v += 1.; tq = v * v - .25;
            for (s = h = 1., r = f = z = w = 0.; k > 0 ; --k, y -= 2.) {
                t = (y * h - (k + 1) * z) / (k - 1 - tp / k); z = h; f += (h = t);
                t = (y * s - (k + 1) * w) / (k - 1 - tq / k); w = s; r += (s = t);
            }
            t = sqrt(a0 / x) * exp(-x); s *= t / r; h *= t / f; x /= 2.; if (m == 0) s = h;
            for (k = 1; k < m ; ++k) {
                t = v * s / x + h; h = s; s = t; v += 1.;
            }
        }
    } else {
        s = t = sqrt(a0 / x); x *= 2.;
        for (p = 1, y = .5; (tp = fabs(t)) > 1.e-14 ; ++p, y += 1.) {
            t *= (v + y) * (v - y) / (p * x); if (y > v && fabs(t) >= tp) break; s += t;
        }
        s *= exp(-x / 2.);
    }
    return s;
}

static double ccmath_jbes(double v, double x)
{
    double y, s = 0., t = 0., tp; int p, m;
    y = x - 8.5; if (y > 0.) y *= y; tp = v * v / 4. + 13.69;
    if (y < tp) {
        x /= 2.; m = (int)x;
        if (x > 0.) s = t = exp(v * log(x) - ccmath_gaml(v + 1.));
        else {
            if (v > 0.) return 0.; else if (v == 0.) return 1.;
        }
        for (p = 1, x *= -x;; ++p) {
            t *= x / (p * (v += 1.)); s += t;
            if (p > m && fabs(t) < 1.e-13) break;
        }
    } else {
        double u, a0 = 1.57079632679490;
        s = t = 1. / sqrt(x * a0); x *= 2.; u = 0.;
        for (p = 1, y = .5; (tp = fabs(t)) > 1.e-14 ; ++p, y += 1.) {
            t *= (v + y) * (v - y) / (p * x); if (y > v && fabs(t) >= tp) break;
            if (!(p&1)) {
                t = -t; s += t;
            } else u -= t;
        }
        y = x / 2. - (v + .5) * a0; s = cos(y) * s + sin(y) * u;
    }
    return s;
}

static double ccmath_nbes(double v, double x)
{
    double y, s, t, tp, u, f, a0 = 3.14159265358979;
    int p, k, m;
    y = x - 8.5; if (y > 0.) y *= y; tp = v * v / 4. + 13.69;
    if (y < tp) {
        if (x == 0.) return HUGE_VAL;
        x /= 2.; m = (int)x; u = t = exp(v * log(x) - ccmath_gaml(v + 1.));
        if (modf(v, &y) == 0.) {
            k = (int)y; u *= v;
            f = 2.*log(x) - ccmath_psi(1) - ccmath_psi(k + 1);
            t /= a0; x *= -x; s = f * t;
            for (p = 1;; ++p) {
                f -= 1. / p + 1. / (v += 1.);
                t *= x / (p * v); s += (y = t * f); if (p > m && fabs(y) < 1.e-13) break;
            }
            if (k > 0) {
                x = -x; s -= (t = 1. / (u * a0));
                for (p = 1, --k; k > 0 ; ++p, --k) s -= (t *= x / (p * k));
            }
        } else {
            f = 1. / (t * v * a0); t /= tan(a0 * v); s = t - f;
            for (p = 1, x *= x, u = v;; ++p) {
                t *= -x / (p * (v += 1.)); f *= x / (p * (u -= 1.));
                s += (y = t - f); if (p > m && fabs(y) < 1.e-13) break;
            }
        }
    } else {
        x *= 2.; s = t = 2. / sqrt(x * a0); u = 0.;
        for (p = 1, y = .5; (tp = fabs(t)) > 1.e-14 ; ++p, y += 1.) {
            t *= (v + y) * (v - y) / (p * x); if (y > v && fabs(t) > tp) break;
            if (!(p&1)) {
                t = -t; s += t;
            } else u += t;
        }
        y = (x - (v + .5) * a0) / 2.; s = sin(y) * s + cos(y) * u;
    }
    return s;
}


/* ---------- end of CCMATH code ---------- */

template <typename func_ptr>
Value CalcBessel(
    func_ptr       *func,
    ValueConverter *converter,
    Value v, Value x)
{
    double vv = numToDouble(converter->toFloat(v));
    double xx = numToDouble(converter->toFloat(x));
    // vv must be a non-negative integer and <29 for implementation reasons
    // xx must be non-negative
    if (xx >= 0 && vv >= 0 && vv < 29 && vv == floor(vv))
        return Value(func(vv, xx));
    return Value::errorVALUE();
}

Value ValueCalc::besseli(Value v, Value x)
{
    return CalcBessel(ccmath_ibes, converter, v, x);
}

Value ValueCalc::besselj(Value v, Value x)
{
    return CalcBessel(ccmath_jbes, converter, v, x);
}

Value ValueCalc::besselk(Value v, Value x)
{
    return CalcBessel(ccmath_kbes, converter, v, x);
}

Value ValueCalc::besseln(Value v, Value x)
{
    return CalcBessel(ccmath_nbes, converter, v, x);
}


// ------------------------------------------------------

Value ValueCalc::erf(Value x)
{
    return Value(::erf(numToDouble(converter->toFloat(x))));
}

Value ValueCalc::erfc(Value x)
{
    return Value(::erfc(numToDouble(converter->toFloat(x))));
}

// ------------------------------------------------------

void ValueCalc::arrayWalk(const Value &range,
                          Value &res, arrayWalkFunc func, Value param)
{
    if (res.isError()) return;
    if (!range.isArray()) {
        func(this, res, range, param);
        return;
    }

    // iterate over the non-empty entries
    for (uint i = 0; i < range.count(); ++i) {
        Value v = range.element(i);
        if (v.isArray())
            arrayWalk(v, res, func, param);
        else {
            func(this, res, v, param);
            if (res.format() == Value::fmt_None)
                res.setFormat(v.format());
        }
    }
}

void ValueCalc::arrayWalk(QVector<Value> &range,
                          Value &res, arrayWalkFunc func, Value param)
{
    if (res.isError()) return;
    for (int i = 0; i < range.count(); ++i)
        arrayWalk(range[i], res, func, param);
}

Value ValueCalc::arrayMap(const Value &array, arrayMapFunc func, const Value &param)
{
    Value res( Value::Array );
    for (unsigned row = 0; row < array.rows(); ++row) {
        for (unsigned col = 0; col < array.columns(); ++col) {
            Value element = array.element( col, row );
            Value _res = (this->*func)( element, param );
            res.setElement( col, row, _res );
        }
    }
    return res;
}

Value ValueCalc::twoArrayMap(const Value &array1, arrayMapFunc func, const Value &array2)
{
    Value res( Value::Array );
    // Map each element in one array with the respective element in the other array
    unsigned rows = qMax(array1.rows(), array2.rows());
    unsigned columns = qMax(array1.columns(), array2.columns());
    for (unsigned row = 0; row < rows; ++row) {
        for (unsigned col = 0; col < columns; ++col) {
            // Value::element() will return an empty value if element(col, row) does not exist.
            Value element1 = array1.element( col, row );
            Value element2 = array2.element( col, row );
            Value _res = (this->*func)( element1, element2 );
            res.setElement( col, row, _res );
        }
    }
    return res;
}

void ValueCalc::twoArrayWalk(const Value &a1, const Value &a2,
                             Value &res, arrayWalkFunc func)
{
    if (res.isError()) return;
    if (!a1.isArray()) {
        func(this, res, a1, a2);
        return;
    }

    int rows = a1.rows();
    int cols = a1.columns();
    int rows2 = a2.rows();
    int cols2 = a2.columns();
    if ((rows != rows2) || (cols != cols2)) {
        res = Value::errorVALUE();
        return;
    }
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c) {
            Value v1 = a1.element(c, r);
            Value v2 = a2.element(c, r);
            if (v1.isArray() && v2.isArray())
                twoArrayWalk(v1, v2, res, func);
            else {
                func(this, res, v1, v2);
                if (res.format() == Value::fmt_None)
                    res.setFormat(format(v1, v2));
            }
        }
}

void ValueCalc::twoArrayWalk(QVector<Value> &a1,
                             QVector<Value> &a2, Value &res, arrayWalkFunc func)
{
    if (res.isError()) return;
    if (a1.count() != a2.count()) {
        res = Value::errorVALUE();
        return;
    }
    for (int i = 0; i < a1.count(); ++i)
        twoArrayWalk(a1[i], a2[i], res, func);
}

arrayWalkFunc ValueCalc::awFunc(const QString &name)
{
    if (awFuncs.count(name))
        return awFuncs[name];
    return 0;
}

void ValueCalc::registerAwFunc(const QString &name, arrayWalkFunc func)
{
    awFuncs[name] = func;
}

// ------------------------------------------------------

Value ValueCalc::sum(const Value &range, bool full)
{
    Value res(0);
    arrayWalk(range, res, full ? awSumA : awSum, Value(0));
    return res;
}

Value ValueCalc::sum(QVector<Value> range, bool full)
{
    Value res(0);
    arrayWalk(range, res, full ? awSumA : awSum, Value(0));
    return res;
}

// sum of squares
Value ValueCalc::sumsq(const Value &range, bool full)
{
    Value res(0);
    arrayWalk(range, res, full ? awSumSqA : awSumSq, Value(0));
    return res;
}

Value ValueCalc::sumIf(const Value &range, const Condition &cond)
{
    if(range.isError())
        return range;

    if (!range.isArray()) {
        if (matches(cond, range.element(0, 0))) {
            //debugSheets<<"return non array value "<<range;
            return range;
        }
        return Value(0.0);
    }

    //if we are here, we have an array
    Value res(0);
    Value tmp;

    unsigned int rows = range.rows();
    unsigned int cols = range.columns();
    for (unsigned int r = 0; r < rows; ++r)
        for (unsigned int c = 0; c < cols; ++c) {
            Value v = range.element(c, r);

            if (v.isArray())
                tmp = sumIf(v, cond);
            if (tmp.isNumber()) {// only add numbers, no conversion from string allowed
                res = add(res, tmp);
            } else if (matches(cond, v)) {
                if (v.isNumber()) {// only add numbers, no conversion from string allowed
                    //debugSheets<<"add "<<v;
                    res = add(res, v);
                }
            }
        }

    return res;
}

Value ValueCalc::sumIf(const CellBase &sumRangeStart, const Value &range, const Condition &cond)
{
    if(range.isError())
        return range;

    if (!range.isArray()) {
        if (matches(cond, range.element(0, 0))) {
            //debugSheets<<"return non array value "<<range;
            return sumRangeStart.value();
        }
        return Value(0.0);
    }

    //if we are here, we have an array
    Value res(0);
    Value tmp;

    unsigned int rows = range.rows();
    unsigned int cols = range.columns();
    for (unsigned int r = 0; r < rows; ++r)
        for (unsigned int c = 0; c < cols; ++c) {
            Value v = range.element(c, r);
            if (v.isArray())
                return Value::errorVALUE();

            if (matches(cond, v)) {
                Value val = CellBase(sumRangeStart.sheet(), sumRangeStart.column() + c, sumRangeStart.row() + r).value();
                if (val.isNumber()) {// only add numbers, no conversion from string allowed
                    res = add(res, val);
                }
            }
        }

    return res;
}

Value ValueCalc::sumIfs(const CellBase &sumRangeStart, QList<Value> range, QList<Condition> cond, const float limit)
{
    if(range[0].isError())
        return range[0];

    Value res(0);
    Value val;

    unsigned int rows = range[0].rows();
    unsigned int cols = range[0].columns();
    for (unsigned int r = 0; r < rows; ++r) {
        for (unsigned int c = 0; c < cols; ++c) {
            for (unsigned int i = 1; i <= limit ; ++i) {

                if(range[i].isError())
                    return range[0];

                if (!range[i].isArray()) {
                    if (matches(cond[i-1], range[i].element(0, 0))) {
                        return sumRangeStart.value();
                    }
                    return Value(0.0);
                }

                Value v = range[i].element(c, r);
                if (v.isArray()) {
                    return Value::errorVALUE();
                }

                if (!matches(cond[i-1], v)) {
                    val = Value(0.0);
                    break;
                }
                val = range[0].element(c, r);
            }
            if (val.isNumber()) {// only add numbers, no conversion from string allowed
                res = add(res, val);
            }
        }
    }
    return res;
}

Value ValueCalc::averageIf(const Value &range, const Condition &cond)
{
    if(range.isError())
        return range;

    if (!range.isArray()) {
        if (matches(cond, range.element(0, 0))) {
            return range;
        }
        return Value(0.0);
    }

    Value res(0);
    Value tmp;

    unsigned int rows = range.rows();
    unsigned int cols = range.columns();
    unsigned int cnt = 0;
    for (unsigned int r = 0; r < rows; ++r)
        for (unsigned int c = 0; c < cols; ++c) {
            Value v = range.element(c, r);

            if (v.isArray())
                tmp = averageIf(v, cond);
            if (tmp.isNumber()) {// only add numbers, no conversion from string allowed
                res = add(res, tmp);
            } else if (matches(cond, v)) {
                if (v.isNumber()) {// only add numbers, no conversion from string allowed
                    //debugSheets<<"add "<<v;
                    res = add(res, v);
                    ++cnt;
                }
            }
        }

    res = div(res, cnt);
    return res;
}

Value ValueCalc::averageIf(const CellBase &avgRangeStart, const Value &range, const Condition &cond)
{
    if(range.isError())
        return range;

    if (!range.isArray()) {
        if (matches(cond, range.element(0, 0))) {
            return avgRangeStart.value();
        }
        return Value(0.0);
    }

    Value res(0);

    unsigned int rows = range.rows();
    unsigned int cols = range.columns();
    unsigned int cnt = 0;
    for (unsigned int r = 0; r < rows; ++r)
        for (unsigned int c = 0; c < cols; ++c) {
            Value v = range.element(c, r);

            if (v.isArray())
                return Value::errorVALUE();

            if (matches(cond, v)) {
                Value val = CellBase(avgRangeStart.sheet(), avgRangeStart.column() + c, avgRangeStart.row() + r).value();
                if (val.isNumber()) {// only add numbers, no conversion from string allowed
                    //debugSheets<<"add "<<val;
                    res = add(res, val);
                    ++cnt;
                }
            }
        }

    res = div(res, cnt);
    return res;
}

Value ValueCalc::averageIfs(const CellBase &avgRangeStart, QList<Value> range, QList<Condition> cond, const float limit)
{
    if(range[0].isError())
        return range[0];

    Value res(0);
    Value val;

    unsigned int rows = range[0].rows();
    unsigned int cols = range[0].columns();
    unsigned int cnt = 0;
    for (unsigned int r = 0; r < rows; ++r) {
        for (unsigned int c = 0; c < cols; ++c) {
            bool flag = true;
            for (unsigned int i = 1; i <= limit ; ++i) {

                if(range[i].isError())
                    return range[0];

                if (!range[i].isArray()) {
                    if (matches(cond[i-1], range[i].element(0, 0))) {
                        return avgRangeStart.value();
                    }
                    return Value(0.0);
                }

                Value v = range[i].element(c, r);
                if (v.isArray()) {
                    return Value::errorVALUE();
                }

                if (!matches(cond[i-1], v)) {
                    flag = false;
                    val = Value(0.0);
                    break;
                }
                val = range[0].element(c, r);
            }
            if (flag) {
                ++cnt;
                flag = true;
            }
            if (val.isNumber()) {// only add numbers, no conversion from string allowed
                res = add(res, val);
            }
        }
    }
    res = div(res, cnt);
    return res;
}

int ValueCalc::count(const Value &range, bool full)
{
    Value res(0);
    arrayWalk(range, res, full ? awCountA : awCount, Value(0));
    return converter->asInteger(res).asInteger();
}

int ValueCalc::count(QVector<Value> range, bool full)
{
    Value res(0);
    arrayWalk(range, res, full ? awCountA : awCount, Value(0));
    return converter->asInteger(res).asInteger();
}

int ValueCalc::countIf(const Value &range, const Condition &cond)
{
    if (!range.isArray()) {
        if (matches(cond, range))
            return range.isEmpty() ? 0 : 1;
        return 0;
    }

    int res = 0;

    // iterate over the non-empty entries
    for (uint i = 0; i < range.count(); ++i) {
        Value v = range.element(i);

        if (v.isArray())
            res += countIf(v, cond);
        else if (matches(cond, v))
            ++res;
    }

    return res;
}

Value ValueCalc::countIfs(const CellBase &cntRangeStart, QList<Value> range, QList<Condition> cond, const float limit)
{
    if (!range[0].isArray())
        return Value(0.0);

    if(range[0].isError())
        return range[0];

    Value res(0);

    unsigned int rows = range[0].rows();
    unsigned int cols = range[0].columns();
    for (unsigned int r = 0; r < rows; ++r) {
        for (unsigned int c = 0; c < cols; ++c) {
            bool flag = true;
            for (unsigned int i = 0; i <= limit ; ++i) {

                if(range[i].isError())
                    return range[0];

                if (!range[i].isArray()) {
                    if (matches(cond[i], range[i].element(0, 0))) {
                        return cntRangeStart.value();
                    }
                    return Value(0.0);
                }

                Value v = range[i].element(c, r);
                if (v.isArray()) {
                    return Value::errorVALUE();
                }

                if (!matches(cond[i], v)) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                res = add(res, 1);
                flag = true;
            }
        }
    }
    return res;
}

Value ValueCalc::avg(const Value &range, bool full)
{
    int cnt = count(range, full);
    if (cnt)
        return div(sum(range, full), cnt);
    return Value(0.0);
}

Value ValueCalc::avg(QVector<Value> range, bool full)
{
    int cnt = count(range, full);
    if (cnt)
        return div(sum(range, full), cnt);
    return Value(0.0);
}

Value ValueCalc::max(const Value &range, bool full)
{
    Value res;
    arrayWalk(range, res, full ? awMaxA : awMax, Value(0));
    return res;
}

Value ValueCalc::max(QVector<Value> range, bool full)
{
    Value res;
    arrayWalk(range, res, full ? awMaxA: awMax, Value(0));
    return res;
}

Value ValueCalc::min(const Value &range, bool full)
{
    Value res;
    arrayWalk(range, res, full ? awMinA : awMin, Value(0));
    return res;
}

Value ValueCalc::min(QVector<Value> range, bool full)
{
    Value res;
    arrayWalk(range, res, full ? awMinA : awMin, Value(0));
    return res;
}

Value ValueCalc::product(const Value &range, Value init,
                         bool full)
{
    Value res = init;
    if (isZero(init)) { // special handling of a zero, due to excel-compat
        if (count(range, full) == 0)
            return init;
        res = Value(1.0);
    }
    arrayWalk(range, res, full ? awProdA : awProd, Value(0));
    return res;
}

Value ValueCalc::product(QVector<Value> range,
                         Value init, bool full)
{
    Value res = init;
    if (isZero(init)) { // special handling of a zero, due to excel-compat
        if (count(range, full) == 0)
            return init;
        res = Value(1.0);
    }
    arrayWalk(range, res, full ? awProdA : awProd, Value(0));
    return res;
}

Value ValueCalc::stddev(const Value &range, bool full)
{
    return stddev(range, avg(range, full), full);
}

Value ValueCalc::stddev(const Value &range, Value avg,
                        bool full)
{
    Value res;
    int cnt = count(range, full);
    arrayWalk(range, res, full ? awDevSqA : awDevSq, avg);
    return sqrt(div(res, cnt - 1));
}

Value ValueCalc::stddev(QVector<Value> range, bool full)
{
    return stddev(range, avg(range, full), full);
}

Value ValueCalc::stddev(QVector<Value> range,
                        Value avg, bool full)
{
    Value res;
    int cnt = count(range, full);
    arrayWalk(range, res, full ? awDevSqA : awDevSq, avg);
    return sqrt(div(res, cnt - 1));
}

Value ValueCalc::stddevP(const Value &range, bool full)
{
    return stddevP(range, avg(range, full), full);
}

Value ValueCalc::stddevP(const Value &range, Value avg,
                         bool full)
{
    Value res;
    int cnt = count(range, full);
    arrayWalk(range, res, full ? awDevSqA : awDevSq, avg);
    return sqrt(div(res, cnt));
}

Value ValueCalc::stddevP(QVector<Value> range, bool full)
{
    return stddevP(range, avg(range, full), full);
}

Value ValueCalc::stddevP(QVector<Value> range,
                         Value avg, bool full)
{
    Value res;
    int cnt = count(range, full);
    arrayWalk(range, res, full ? awDevSqA : awDevSq, avg);
    return sqrt(div(res, cnt));
}

bool isDate(Value::Format fmt)
{
    if ((fmt == Value::fmt_Date) || (fmt == Value::fmt_DateTime))
        return true;
    return false;
}

Value::Format ValueCalc::format(Value a, Value b)
{
    Value::Format af = a.format();
    Value::Format bf = b.format();

    // operation on two dates should produce a number
    if (isDate(af) && isDate(bf))
        return Value::fmt_Number;

    if ((af == Value::fmt_None) || (af == Value::fmt_Boolean))
        return bf;
    return af;
}

// ------------------------------------------------------

void ValueCalc::getCond(Condition &cond, Value val)
{
    // not a string - we simply take it as a numeric value
    // that also handles floats, logical values, date/time and such
    if (!val.isString()) {
        cond.comp = isEqual;
        cond.type = numeric;
        cond.value = converter->toFloat(val);
        return;
    }
    QString text = converter->asString(val).asString();
    cond.comp = isEqual;
    text = text.trimmed();

    if (text.startsWith(QLatin1String("<="))) {
        cond.comp = lessEqual;
        text.remove(0, 2);
    } else if (text.startsWith(QLatin1String(">="))) {
        cond.comp = greaterEqual;
        text.remove(0, 2);
    } else if (text.startsWith(QLatin1String("!=")) || text.startsWith(QLatin1String("<>"))) {
        cond.comp = notEqual;
        text.remove(0, 2);
    } else if (text.startsWith(QLatin1String("=="))) {
        cond.comp = isEqual;
        text.remove(0, 2);
    } else if (text.startsWith(QLatin1Char('<'))) {
        cond.comp = isLess;
        text.remove(0, 1);
    } else if (text.startsWith(QLatin1Char('>'))) {
        cond.comp = isGreater;
        text.remove(0, 1);
    } else if (text.startsWith(QLatin1Char('='))) {
        cond.comp = isEqual;
        text.remove(0, 1);
    } else { // character comparison
        cond.type = string;
        cond.stringValue = text;
        if (settings()->useWildcards()) { // HOST-USE-WILDCARDS Excel like wildcard matching
            cond.comp = wildcardMatch;
        } else if (settings()->useRegularExpressions()) { // HOST-USE-REGULAR-EXPRESSION ODF like regex matching
            cond.comp = regexMatch;
        } else { // Simple string matching
            cond.comp = stringMatch;
        }
        return;
    }

    text = text.trimmed();

    bool ok = false;
    double d = text.toDouble(&ok);
    if (ok) {
        cond.type = numeric;
        cond.value = d;
    } else {
        cond.type = string;
        cond.stringValue = text;
    }
    //TODO: date values
}

bool ValueCalc::matches(const Condition &cond, Value val)
{
    if (val.isEmpty())
        return false;
    if (cond.type == numeric) {
        Number d = converter->toFloat(val);
        switch (cond.comp) {
        case isEqual:
            if (approxEqual(Value(d), Value(cond.value))) return true;
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

        default:
            break;
        }
    } else {
        QString d = converter->asString(val).asString();
        switch (cond.comp) {
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

        case stringMatch:
            if (d.toLower() == cond.stringValue.toLower()) return true;
            break;

        case regexMatch: {
            QRegExp rx;
            rx.setPattern(cond.stringValue);
            rx.setPatternSyntax(QRegExp::RegExp);
            rx.setCaseSensitivity(Qt::CaseInsensitive);
            if (rx.exactMatch(d)) return true;
        } break;

        case wildcardMatch: {
            QRegExp rx;
            rx.setPattern(cond.stringValue);
            rx.setPatternSyntax(QRegExp::Wildcard);
            rx.setCaseSensitivity(Qt::CaseInsensitive);
            if (rx.exactMatch(d)) return true;
        } break;

        }
    }
    return false;
}

// the days360 method does implement the 30/360days method as used in e.g. the YEARFRAC function
// TODO this duplicates the func_days360, maybe merge the algorithms?
int ValueCalc::days360(int day1, int month1, int year1, bool leapYear1,
                     int day2, int month2, int year2, bool leapYear2,
                     bool usaMethod)
{
    if (usaMethod) { // US method
        if (day1 == 31) {
            day1 = 30;
            if (day2 == 31) {
                day2 = 30;
            }
        }
        else if (day1 == 30 && day2 == 31) {
            day2 = 30;
        }
        else if (month1 == 2 && (day1 == 29 || (day1 == 28 && ! leapYear1))) {
            day1 = 30;
            if (month2 == 2 && (day2 == 29 || (day2 == 28 && ! leapYear2))) {
                day2 = 30;
            }
        }
    } else { // European method
        if (day1 == 31) {
            day1 = 30;
        }
        if (day2 == 31) {
            day2 = 30;
        }
    }
    return day2 + month2 * 30 + year2 * 360 - day1 - month1 * 30 - year1 * 360;
}


// days360
int ValueCalc::days360(const QDate& _date1, const QDate& _date2, bool european)
{
    int day1, month1, year1, day2, month2, year2;

    day1 = _date1.day();
    month1 = _date1.month();
    year1 = _date1.year();

    day2 = _date2.day();
    month2 = _date2.month();
    year2 = _date2.year();

    return days360(day1, month1, year1, QDate::isLeapYear(_date1.year()), day2, month2, year2, QDate::isLeapYear(_date2.year()), !european);
}



Value ValueCalc::yearFrac(const QDate& startDate, const QDate& endDate, int basis)
{
    QDate date1 = startDate;
    QDate date2 = endDate;

    //
    // calculation
    //

    if (date2 < date1) {
        // exchange dates
        QDate Temp1 = date1;
        date1 = date2;
        date2 = Temp1;
    }

    int days = date1.daysTo(date2);

//   debugSheetsFormula <<"date1 =" << date1 <<"    date2 =" << date2 <<"    days =" << days <<"    basis =" << basis;

    long double res = 0;
    long double peryear = 0;
    int nYears = 0;

    switch (basis) {
    case 1: {
        nYears = date2.year() - date1.year() + 1;
        for (int y = date1.year(); y <= date2.year(); ++y) {
            peryear += QDate::isLeapYear(y) ? 366 : 365;
        }
        // less than one year - even if it does span two consequentive years ...
        if (QDate(date1.year() + 1, date1.month(), date1.day()) >= date2) {
            nYears = 1;
            peryear = 365;
            if (QDate::isLeapYear(date1.year()) && date1.month() <= 2) peryear = 366;
            else if (QDate::isLeapYear(date2.year()) && date2.month() > 2) peryear = 366;
            else if (date2.month() == 2 && date2.day() == 29) peryear = 366;
        }
        peryear = peryear / (long double) nYears;
        nYears = 0;
        break;
    }
    case 2: {
        // Actual/360
        peryear = 360;
        break;
    }
    case 3: {
        // Actual/365
        peryear = 365;
        break;
    }
    case 4: {
        // 30/360 Europe

        // calc datedif360 (start, end, Europe)
        days = days360(date1, date2, 1);

        peryear = 360;
        break;
    }
    default: {
        // NASD 30/360
        //basis = 0;

        // calc datedif360 (start, end, US)
        days = days360(date1, date2, 0);

        peryear = 360;
    }
    }

    res = (long double)(nYears) + (long double)days / (long double) peryear;
//   debugSheetsFormula<<"getYearFrac res="<<res;
    return Value(res);
}


