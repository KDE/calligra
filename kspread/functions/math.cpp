/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team <koffice-devel@kde.org>
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>
   Copyright 2007 Sascha Pfau <MrPeacock@gmail.com>
   Copyright (C) 2010 Carlos Licea <carlos@kdab.com>
   Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
     Contact: Suresh Chande suresh.chande@nokia.com

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// built-in math functions

// needed for RANDBINOM and so
#include <math.h>

#include <kdebug.h>
#include <KGenericFactory>
#include <KLocale>

#include "FunctionModuleRegistry.h"
#include "Functions.h"
#include "MathModule.h"
#include "ValueCalc.h"
#include "ValueConverter.h"

// needed for SUBTOTAL:
#include "Cell.h"
#include "Sheet.h"
#include "RowColumnFormat.h"

// needed by MDETERM and MINVERSE
#include <Eigen/LU>

using namespace KSpread;

// RANDBINOM and RANDNEGBINOM won't support arbitrary precision

// prototypes
Value func_abs(valVector args, ValueCalc *calc, FuncExtra *);
Value func_ceil(valVector args, ValueCalc *calc, FuncExtra *);
Value func_ceiling(valVector args, ValueCalc *calc, FuncExtra *);
Value func_count(valVector args, ValueCalc *calc, FuncExtra *);
Value func_counta(valVector args, ValueCalc *calc, FuncExtra *);
Value func_countblank(valVector args, ValueCalc *calc, FuncExtra *);
Value func_countif(valVector args, ValueCalc *calc, FuncExtra *);
Value func_cur(valVector args, ValueCalc *calc, FuncExtra *);
Value func_div(valVector args, ValueCalc *calc, FuncExtra *);
Value func_eps(valVector args, ValueCalc *calc, FuncExtra *);
Value func_even(valVector args, ValueCalc *calc, FuncExtra *);
Value func_exp(valVector args, ValueCalc *calc, FuncExtra *);
Value func_fact(valVector args, ValueCalc *calc, FuncExtra *);
Value func_factdouble(valVector args, ValueCalc *calc, FuncExtra *);
Value func_fib(valVector args, ValueCalc *calc, FuncExtra *);
Value func_floor(valVector args, ValueCalc *calc, FuncExtra *);
Value func_gamma(valVector args, ValueCalc *calc, FuncExtra *);
Value func_gcd(valVector args, ValueCalc *calc, FuncExtra *);
Value func_int(valVector args, ValueCalc *calc, FuncExtra *);
Value func_inv(valVector args, ValueCalc *calc, FuncExtra *);
Value func_kproduct(valVector args, ValueCalc *calc, FuncExtra *);
Value func_lcm(valVector args, ValueCalc *calc, FuncExtra *);
Value func_ln(valVector args, ValueCalc *calc, FuncExtra *);
Value func_log2(valVector args, ValueCalc *calc, FuncExtra *);
Value func_log10(valVector args, ValueCalc *calc, FuncExtra *);
Value func_logn(valVector args, ValueCalc *calc, FuncExtra *);
Value func_max(valVector args, ValueCalc *calc, FuncExtra *);
Value func_maxa(valVector args, ValueCalc *calc, FuncExtra *);
Value func_mdeterm(valVector args, ValueCalc *calc, FuncExtra *);
Value func_min(valVector args, ValueCalc *calc, FuncExtra *);
Value func_mina(valVector args, ValueCalc *calc, FuncExtra *);
Value func_minverse(valVector args, ValueCalc* calc, FuncExtra*);
Value func_mmult(valVector args, ValueCalc *calc, FuncExtra *);
Value func_mod(valVector args, ValueCalc *calc, FuncExtra *);
Value func_mround(valVector args, ValueCalc *calc, FuncExtra *);
Value func_mult(valVector args, ValueCalc *calc, FuncExtra *);
Value func_multinomial(valVector args, ValueCalc *calc, FuncExtra *);
Value func_munit(valVector args, ValueCalc* calc, FuncExtra*);
Value func_odd(valVector args, ValueCalc *calc, FuncExtra *);
Value func_pow(valVector args, ValueCalc *calc, FuncExtra *);
Value func_quotient(valVector args, ValueCalc *calc, FuncExtra *);
Value func_product(valVector args, ValueCalc *calc, FuncExtra *);
Value func_rand(valVector args, ValueCalc *calc, FuncExtra *);
Value func_randbetween(valVector args, ValueCalc *calc, FuncExtra *);
Value func_randbernoulli(valVector args, ValueCalc *calc, FuncExtra *);
Value func_randbinom(valVector args, ValueCalc *calc, FuncExtra *);
Value func_randexp(valVector args, ValueCalc *calc, FuncExtra *);
Value func_randnegbinom(valVector args, ValueCalc *calc, FuncExtra *);
Value func_randnorm(valVector args, ValueCalc *calc, FuncExtra *);
Value func_randpoisson(valVector args, ValueCalc *calc, FuncExtra *);
Value func_rootn(valVector args, ValueCalc *calc, FuncExtra *);
Value func_round(valVector args, ValueCalc *calc, FuncExtra *);
Value func_rounddown(valVector args, ValueCalc *calc, FuncExtra *);
Value func_roundup(valVector args, ValueCalc *calc, FuncExtra *);
Value func_seriessum(valVector args, ValueCalc *calc, FuncExtra *);
Value func_sign(valVector args, ValueCalc *calc, FuncExtra *);
Value func_sqrt(valVector args, ValueCalc *calc, FuncExtra *);
Value func_sqrtpi(valVector args, ValueCalc *calc, FuncExtra *);
Value func_subtotal(valVector args, ValueCalc *calc, FuncExtra *);
Value func_sum(valVector args, ValueCalc *calc, FuncExtra *);
Value func_suma(valVector args, ValueCalc *calc, FuncExtra *);
Value func_sumif(valVector args, ValueCalc *calc, FuncExtra *);
Value func_sumsq(valVector args, ValueCalc *calc, FuncExtra *);
Value func_transpose(valVector args, ValueCalc *calc, FuncExtra *);
Value func_trunc(valVector args, ValueCalc *calc, FuncExtra *);


// Value func_multipleOP (valVector args, ValueCalc *calc, FuncExtra *);


KSPREAD_EXPORT_FUNCTION_MODULE("math", MathModule)


MathModule::MathModule(QObject* parent, const QVariantList&)
    : FunctionModule(parent)
{
    Function *f;

    /*
      f = new Function ("MULTIPLEOPERATIONS", func_multipleOP);
  add(f);
    */

    // functions that don't take array parameters
    f = new Function("ABS",           func_abs);
  add(f);
    f = new Function("CEIL",          func_ceil);
  add(f);
    f = new Function("CEILING",       func_ceiling);
    f->setParamCount(1, 2);
  add(f);
    f = new Function("CUR",           func_cur);
  add(f);
    f = new Function("EPS",           func_eps);
    f->setParamCount(0);
  add(f);
    f = new Function("EVEN",          func_even);
  add(f);
    f = new Function("EXP",           func_exp);
  add(f);
    f = new Function("FACT",          func_fact);
  add(f);
    f = new Function("FACTDOUBLE",    func_factdouble);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETFACTDOUBLE");
    add(f);
    f = new Function("FIB",           func_fib);  // KSpread-specific, like Quattro-Pro's FIB
  add(f);
    f = new Function("FLOOR",         func_floor);
    f->setParamCount(1, 3);
  add(f);
    f = new Function("GAMMA",         func_gamma);
  add(f);
    f = new Function("INT",           func_int);
  add(f);
    f = new Function("INV",           func_inv);
  add(f);
    f = new Function("LN",            func_ln);
  add(f);
    f = new Function("LOG",           func_logn);
    f->setParamCount(1, 2);
  add(f);
    f = new Function("LOG2",          func_log2);
  add(f);
    f = new Function("LOG10",         func_log10);
  add(f);
    f = new Function("LOGN",          func_logn);
    f->setParamCount(2);
  add(f);
    f = new Function("MOD",           func_mod);
    f->setParamCount(2);
  add(f);
    f = new Function("MROUND",        func_mround);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETMROUND");
    f->setParamCount(2);
  add(f);
    f = new Function("MULTINOMIAL",   func_multinomial);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETMULTINOMIAL");
    f->setParamCount(1, -1);
  add(f);
    f = new Function("ODD",           func_odd);
  add(f);
    f = new Function("POW",         func_pow);
    f->setParamCount(2);
  add(f);
    f = new Function("POWER",         func_pow);
    f->setParamCount(2);
  add(f);
    f = new Function("QUOTIENT",      func_quotient);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETQUOTIENT");
    f->setParamCount(2);
  add(f);
    f = new Function("RAND",          func_rand);
    f->setParamCount(0);
  add(f);
    f = new Function("RANDBERNOULLI", func_randbernoulli);
  add(f);
    f = new Function("RANDBETWEEN",   func_randbetween);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETRANDBETWEEN");
    f->setParamCount(2);
  add(f);
    f = new Function("RANDBINOM",     func_randbinom);
    f->setParamCount(2);
  add(f);
    f = new Function("RANDEXP",       func_randexp);
  add(f);
    f = new Function("RANDNEGBINOM",  func_randnegbinom);
    f->setParamCount(2);
  add(f);
    f = new Function("RANDNORM",      func_randnorm);
    f->setParamCount(2);
  add(f);
    f = new Function("RANDPOISSON",   func_randpoisson);
  add(f);
    f = new Function("ROOTN",         func_rootn);
    f->setParamCount(2);
  add(f);
    f = new Function("ROUND",         func_round);
    f->setParamCount(1, 2);
  add(f);
    f = new Function("ROUNDDOWN",     func_rounddown);
    f->setParamCount(1, 2);
  add(f);
    f = new Function("ROUNDUP",       func_roundup);
    f->setParamCount(1, 2);
  add(f);
    f = new Function("SIGN",          func_sign);
  add(f);
    f = new Function("SQRT",          func_sqrt);
  add(f);
    f = new Function("SQRTPI",        func_sqrtpi);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETSQRTPI");
    add(f);
    f = new Function("TRUNC",         func_trunc);
    f->setParamCount(1, 2);
  add(f);

    // functions that operate over arrays
    f = new Function("COUNT",         func_count);
    f->setParamCount(1, -1);
    f->setAcceptArray();
  add(f);
    f = new Function("COUNTA",        func_counta);
    f->setParamCount(1, -1);
    f->setAcceptArray();
  add(f);
    f = new Function("COUNTBLANK",    func_countblank);
    f->setParamCount(1, -1);
    f->setAcceptArray();
  add(f);
    f = new Function("COUNTIF",       func_countif);
    f->setParamCount(2);
    f->setAcceptArray();
    f->setNeedsExtra(true);
  add(f);
    f = new Function("DIV",           func_div);
    f->setParamCount(1, -1);
    f->setAcceptArray();
  add(f);
    f = new Function("G_PRODUCT",     func_kproduct);  // Gnumeric compatibility
    f->setParamCount(1, -1);
    f->setAcceptArray();
  add(f);
    f = new Function("GCD",           func_gcd);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETGCD");
    f->setParamCount(1, -1);
    f->setAcceptArray();
  add(f);
    f = new Function("KPRODUCT",      func_kproduct);
    f->setParamCount(1, -1);
    f->setAcceptArray();
  add(f);
    f = new Function("LCM",           func_lcm);
    f->setAlternateName("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETLCM");
    f->setParamCount(1, -1);
    f->setAcceptArray();
  add(f);
    f = new Function("MAX",           func_max);
    f->setParamCount(1, -1);
    f->setAcceptArray();
  add(f);
    f = new Function("MAXA",          func_maxa);
    f->setParamCount(1, -1);
    f->setAcceptArray();
  add(f);
    f = new Function("MDETERM",          func_mdeterm);
    f->setParamCount(1);
    f->setAcceptArray();
  add(f);
    f = new Function("MIN",           func_min);
    f->setParamCount(1, -1);
    f->setAcceptArray();
  add(f);
    f = new Function("MINA",          func_mina);
    f->setParamCount(1, -1);
    f->setAcceptArray();
  add(f);
    f = new Function("MINVERSE",         func_minverse);
    f->setParamCount(1);
    f->setAcceptArray();
  add(f);
    f = new Function("MMULT",          func_mmult);
    f->setParamCount(2);
    f->setAcceptArray();
  add(f);
    f = new Function("MULTIPLY",      func_product);   // same as PRODUCT
    f->setParamCount(1, -1);
    f->setAcceptArray();
  add(f);
    f = new Function("MUNIT",         func_munit);
    f->setParamCount(1);
  add(f);
    f = new Function("PRODUCT",       func_product);
    f->setParamCount(1, -1);
    f->setAcceptArray();
  add(f);
    f = new Function("SERIESSUM",     func_seriessum);
    f->setParamCount(3, -1);
    f->setAcceptArray();
  add(f);
    f = new Function("SUM",           func_sum);
    f->setParamCount(1, -1);
    f->setAcceptArray();
  add(f);
    f = new Function("SUMA",          func_suma);
    f->setParamCount(1, -1);
    f->setAcceptArray();
  add(f);
    f = new Function("SUBTOTAL",      func_subtotal);
    f->setParamCount(2);
    f->setAcceptArray();
    f->setNeedsExtra(true);
  add(f);
    f = new Function("SUMIF",         func_sumif);
    f->setParamCount(2, 3);
    f->setAcceptArray();
    f->setNeedsExtra(true);
    add(f);
    f = new Function("SUMSQ",         func_sumsq);
    f->setParamCount(1, -1);
    f->setAcceptArray();
  add(f);
    f = new Function("TRANSPOSE",     func_transpose);
    f->setParamCount(1);
    f->setAcceptArray();
  add(f);
}

QString MathModule::descriptionFileName() const
{
    return QString("math.xml");
}


// Function: SQRT
Value func_sqrt(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value arg = args[0];
    if (calc->gequal(arg, Value(0.0)))
        return calc->sqrt(arg);
    else
        return Value::errorVALUE();
}

// Function: SQRTPI
Value func_sqrtpi(valVector args, ValueCalc *calc, FuncExtra *)
{
    // sqrt (val * PI)
    Value arg = args[0];
    if (calc->gequal(arg, Value(0.0)))
        return calc->sqrt(calc->mul(args[0], calc->pi()));
    else
        return Value::errorVALUE();
}

// Function: ROOTN
Value func_rootn(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->pow(args[0], calc->div(Value(1), args[1]));
}

// Function: CUR
Value func_cur(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->pow(args[0], Value(1.0 / 3.0));
}

// Function: ABS
Value func_abs(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->abs(args[0]);
}

// Function: exp
Value func_exp(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->exp(args[0]);
}

// Function: ceil
Value func_ceil(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->roundUp(args[0], Value(0));
}

// Function: ceiling
Value func_ceiling(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value number = args[0];
    Value res;
    if (args.count() == 2)
        res = args[1];
    else
        res = calc->gequal(number, Value(0.0)) ? Value(1.0) : Value(-1.0);

    // short-circuit, and allow CEILING(0;0) to give 0 (which is correct)
    // instead of DIV0 error
    if (calc->isZero(number))
        return Value(0.0);

    if (calc->isZero(res))
        return Value::errorDIV0();

    Value d = calc->div(number, res);
    if (calc->greater(Value(0), d))
        return Value::errorNUM();

    Value rud = calc->roundDown(d);
    if (calc->approxEqual(rud, d))
        d = calc->mul(rud, res);
    else
        d = calc->mul(calc->roundUp(d), res);

    return d;
}

// Function: FLOOR
Value func_floor(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (calc->approxEqual(args[0], Value(0.0)))
        return Value(0);
    Number number = args[0].asFloat();

    Number significance;
    if (args.count() >= 2) { // we have the optional "significance" argument
        significance = args[1].asFloat();
        // Sign of number and significance must match.
        if (calc->gequal(args[0], Value(0.0)) != calc->gequal(args[1], Value(0.0)))
            return Value::errorVALUE();
    } else // use 1 or -1, depending on the sign of the first argument
        significance = calc->gequal(args[0], Value(0.0)) ? 1.0 : -1.0;
    if (calc->approxEqual(Value(significance), Value(0.0)))
        return Value(0);

    const bool mode = (args.count() == 3) ? (args[2].asFloat() != 0.0) : false;

    Number result;
    if (mode) // round towards zero
        result = ((int)(number / significance)) * significance;
    else { // round towards negative infinity
        result = number / significance; // always positive, because signs match
        if (calc->gequal(args[0], Value(0.0))) // positive values
            result = floor(result) * significance;
        else // negative values
            result = ceil(result) * significance;
    }
    return Value(result);
}

// Function: GAMMA
Value func_gamma(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->GetGamma(args[0]);
}

// Function: ln
Value func_ln(valVector args, ValueCalc *calc, FuncExtra *)
{
    if ((args [0].isNumber() == false) || args[0].asFloat() <= 0)
        return Value::errorNUM();
    return calc->ln(args[0]);
}

// Function: LOGn
Value func_logn(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (args [0].isError())
        return args [0];
    if (args [0].isEmpty())
        return Value::errorNUM();
    if (args [0].isNumber() == false)
        return Value::errorVALUE();
    if (args[0].asFloat() <= 0)
        return Value::errorNUM();
    if (args.count() == 2) {
        if (args [1].isError())
            return args [1];
        if (args [1].isEmpty())
            return Value::errorNUM();
        if (args [1].isNumber() == false)
            return Value::errorVALUE();
        if (args [1].asFloat() <= 0)
            return Value::errorNUM();
        return calc->log(args[0], args[1]);
    } else
        return calc->log(args[0]);
}

// Function: LOG2
Value func_log2(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->log(args[0], Value(2.0));
}

// Function: LOG10
Value func_log10(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (args [0].isError())
        return args [0];
    if ((args [0].isNumber() == false) || (args[0].asFloat() <= 0))
        return Value::errorNUM();
    return calc->log(args[0]);
}

// Function: sum
Value func_sum(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->sum(args, false);
}

// Function: suma
Value func_suma(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->sum(args, true);
}

// Function: SUMIF
Value func_sumif(valVector args, ValueCalc *calc, FuncExtra *e)
{
    Value checkRange = args[0];
    QString condition = calc->conv()->asString(args[1]).asString();
    Condition cond;
    calc->getCond(cond, Value(condition));

    if (args.count() == 3) {
        Cell sumRangeStart(e->sheet, e->ranges[2].col1, e->ranges[2].row1);
        return calc->sumIf(sumRangeStart, checkRange, cond);
    } else {
        return calc->sumIf(checkRange, cond);
    }
}

// Function: product
Value func_product(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->product(args, Value(0.0));
}

// Function: seriessum
Value func_seriessum(valVector args, ValueCalc *calc, FuncExtra *)
{
    double fX = calc->conv()->asFloat(args[0]).asFloat();
    double fN = calc->conv()->asFloat(args[1]).asFloat();
    double fM = calc->conv()->asFloat(args[2]).asFloat();

    if (fX == 0.0 && fN == 0.0)
        return Value::errorNUM();

    double res = 0.0;

    if (fX != 0.0) {

        for (unsigned int i = 0 ; i < args[3].count(); i++) {
            res += args[3].element(i).asFloat() * pow(fX, fN);
            fN += fM;
        }
    }

    return Value(res);
}

// Function: kproduct
Value func_kproduct(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->product(args, Value(1.0));
}

// Function: DIV
Value func_div(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value val = args[0];
    for (int i = 1; i < args.count(); ++i) {
        val = calc->div(val, args[i]);
        if (val.isError())
            return val;
    }
    return val;
}

// Function: SUMSQ
Value func_sumsq(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value res;
    calc->arrayWalk(args, res, calc->awFunc("sumsq"), Value(0));
    return res;
}

// Function: MAX
Value func_max(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value m = calc->max(args, false);
    return m.isEmpty() ? Value(0.0) : m;
}

// Function: MAXA
Value func_maxa(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value m = calc->max(args);
    return m.isEmpty() ? Value(0.0) : m;
}

// Function: MIN
Value func_min(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value m = calc->min(args, false);
    return m.isEmpty() ? Value(0.0) : m;
}

// Function: MINA
Value func_mina(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value m = calc->min(args, true);
    return m.isEmpty() ? Value(0.0) : m;
}

// Function: INT
Value func_int(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->conv()->asInteger(args[0]);
}

// Function: QUOTIENT
Value func_quotient(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (calc->isZero(args[1]))
        return Value::errorDIV0();

    double res = calc->conv()->toFloat(calc->div(args[0], args[1]));
    if (res < 0)
        res = ceil(res);
    else
        res = floor(res);

    return Value(res);
}


// Function: eps
Value func_eps(valVector, ValueCalc *calc, FuncExtra *)
{
    return calc->eps();
}

Value func_randexp(valVector args, ValueCalc *calc, FuncExtra *)
{
    // -1 * d * log (random)
    return calc->mul(calc->mul(args[0], Value(-1)), calc->random());
}

Value func_randbinom(valVector args, ValueCalc *calc, FuncExtra *)
{
    // this function will not support arbitrary precision

    double d  = numToDouble(calc->conv()->toFloat(args[0]));
    int    tr = calc->conv()->toInteger(args[1]);

    if (d < 0 || d > 1)
        return Value::errorVALUE();

    if (tr < 0)
        return Value::errorVALUE();

    // taken from gnumeric
    double x = pow(1 - d, tr);
    double r = (double) rand() / (RAND_MAX + 1.0);
    double t = x;
    int i = 0;

    while (r > t) {
        x *= (((tr - i) * d) / ((1 + i) * (1 - d)));
        i++;
        t += x;
    }

    return Value(i);
}

Value func_randnegbinom(valVector args, ValueCalc *calc, FuncExtra *)
{
    // this function will not support arbitrary precision

    double d  = numToDouble(calc->conv()->toFloat(args[0]));
    int    f = calc->conv()->toInteger(args[1]);

    if (d < 0 || d > 1)
        return Value::errorVALUE();

    if (f < 0)
        return Value::errorVALUE();


    // taken from Gnumeric
    double x = pow(d, f);
    double r = (double) rand() / (RAND_MAX + 1.0);
    double t = x;
    int i = 0;

    while (r > t) {
        x *= (((f + i) * (1 - d)) / (1 + i)) ;
        i++;
        t += x;
    }

    return Value(i);
}

Value func_randbernoulli(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value rnd = calc->random();
    return Value(calc->greater(rnd, args[0]) ? 1.0 : 0.0);
}

Value func_randnorm(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value mu = args[0];
    Value sigma = args[1];

    //using polar form of the Box-Muller transformation
    //refer to http://www.taygeta.com/random/gaussian.html for more info

    Value x1, x2, w;
    do {
        // x1,x2 = 2 * random() - 1
        x1 = calc->random(2.0);
        x2 = calc->random(2.0);
        x1 = calc->sub(x1, 1);
        x1 = calc->sub(x2, 1);
        w = calc->add(calc->sqr(x1), calc->sqr(x2));
    } while (calc->gequal(w, Value(1.0)));    // w >= 1.0

    //sqrt ((-2.0 * log (w)) / w) :
    w = calc->sqrt(calc->div(calc->mul(Value(-2.0), calc->ln(w)), w));
    Value res = calc->mul(x1, w);

    res = calc->add(calc->mul(res, sigma), mu);    // res*sigma + mu
    return res;
}

Value func_randpoisson(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (calc->lower(args[0], Value(0)))
        return Value::errorVALUE();

    // taken from Gnumeric...
    Value x = calc->exp(calc->mul(Value(-1), args[0]));     // e^(-A)
    Value r = calc->random();
    Value t = x;
    int i = 0;

    while (calc->greater(r, t)) {    // r > t
        x = calc->mul(x, calc->div(args[0], i + 1));    // x *= (A/(i+1))
        t = calc->add(t, x);     //t += x
        i++;
    }

    return Value(i);
}

// Function: rand
Value func_rand(valVector, ValueCalc *calc, FuncExtra *)
{
    return calc->random();
}

// Function: RANDBETWEEN
Value func_randbetween(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value v1 = args[0];
    Value v2 = args[1];
    if (calc->greater(v2, v1)) {
        v1 = args[1];
        v2 = args[0];
    }
    return calc->add(v1, calc->random(calc->sub(v2, v1)));
}

// Function: POW
Value func_pow(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->pow(args[0], args[1]);
}

// Function: MOD
Value func_mod(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->mod(args[0], args[1]);
}

// Function: fact
Value func_fact(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (args[0].isInteger() || args[0].asInteger() > 0)
        return calc->fact(args[0]);
    else
        return Value::errorNUM();
}

// Function: FACTDOUBLE
Value func_factdouble(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (args[0].isInteger() || args[0].asInteger() > 0)
        return calc->factDouble(args[0]);
    else
        return Value::errorNUM();
}

// Function: MULTINOMIAL
Value func_multinomial(valVector args, ValueCalc *calc, FuncExtra *)
{
    // (a+b+c)! / a!b!c!  (any number of params possible)
    Value num = Value(0), den = Value(1);
    for (int i = 0; i < args.count(); ++i) {
        num = calc->add(num, args[i]);
        den = calc->mul(den, calc->fact(args[i]));
    }
    num = calc->fact(num);
    return calc->div(num, den);
}

// Function: sign
Value func_sign(valVector args, ValueCalc *calc, FuncExtra *)
{
    return Value(calc->sign(args[0]));
}

// Function: INV
Value func_inv(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->mul(args[0], -1);
}

// Function: MROUND
Value func_mround(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value d = args[0];
    Value m = args[1];

    // signs must be the same
    if ((calc->greater(d, Value(0)) && calc->lower(m, Value(0)))
            || (calc->lower(d, Value(0)) && calc->greater(m, Value(0))))
        return Value::errorVALUE();

    int sign = 1;

    if (calc->lower(d, Value(0))) {
        sign = -1;
        d = calc->mul(d, Value(-1));
        m = calc->mul(m, Value(-1));
    }

    // from gnumeric:
    Value mod = calc->mod(d, m);
    Value div = calc->sub(d, mod);

    Value result = div;
    if (calc->gequal(mod, calc->div(m, Value(2))))  // mod >= m/2
        result = calc->add(result, m);      // result += m
    result = calc->mul(result, sign);     // add the sign

    return result;
}

// Function: ROUNDDOWN
Value func_rounddown(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (args.count() == 2) {
        if (calc->greater(args[0], 0.0))
            return calc->roundDown(args[0], args[1]);
        else
            return calc->roundUp(args[0], args[1]);
    }

    if (calc->greater(args[0], 0.0))
        return calc->roundDown(args[0], 0);
    else
        return calc->roundUp(args[0], 0);
}

// Function: ROUNDUP
Value func_roundup(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (args.count() == 2) {
        if (calc->greater(args[0], 0.0))
            return calc->roundUp(args[0], args[1]);
        else
            return calc->roundDown(args[0], args[1]);
    }

    if (calc->greater(args[0], 0.0))
        return calc->roundUp(args[0], 0);
    else
        return calc->roundDown(args[0], 0);
}

// Function: ROUND
Value func_round(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (args.count() == 2)
        return calc->round(args[0], args[1]);
    return calc->round(args[0], 0);
}

// Function: EVEN
Value func_even(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (calc->greater(args[0], 0.0)) {
        const Value value = calc->roundUp(args[0], 0);
        return calc->isZero(calc->mod(value, Value(2))) ? value : calc->add(value, Value(1));
    } else {
        const Value value = calc->roundDown(args[0], 0);
        return calc->isZero(calc->mod(value, Value(2))) ? value : calc->sub(value, Value(1));
    }
}

// Function: ODD
Value func_odd(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (calc->gequal(args[0], Value(0))) {
        const Value value = calc->roundUp(args[0], 0);
        return calc->isZero(calc->mod(value, Value(2))) ? calc->add(value, Value(1)) : value;
    } else {
        const Value value = calc->roundDown(args[0], 0);
        return calc->isZero(calc->mod(value, Value(2))) ? calc->add(value, Value(-1)) : value;
    }
}

Value func_trunc(valVector args, ValueCalc *calc, FuncExtra *)
{
    Q_UNUSED(calc)
    Number result = args[0].asFloat();
    if (args.count() == 2)
        result = result * ::pow(10, (int)args[1].asInteger());
    result = (args[0].asFloat() < 0) ? -(qint64)(-result) : (qint64)result;
    if (args.count() == 2)
        result = result * ::pow(10, -(int)args[1].asInteger());
    return Value(result);
}

// Function: COUNT
Value func_count(valVector args, ValueCalc *calc, FuncExtra *)
{
    return Value(calc->count(args, false));
}

// Function: COUNTA
Value func_counta(valVector args, ValueCalc *calc, FuncExtra *)
{
    return Value(calc->count(args));
}

// Function: COUNTBLANK
Value func_countblank(valVector args, ValueCalc *, FuncExtra *)
{
    int cnt = 0;
    for (int i = 0; i < args.count(); ++i)
        if (args[i].isArray()) {
            int rows = args[i].rows();
            int cols = args[i].columns();
            for (int r = 0; r < rows; ++r)
                for (int c = 0; c < cols; ++c)
                    if (args[i].element(c, r).isEmpty())
                        cnt++;
        } else
            if (args[i].isEmpty())
                cnt++;
    return Value(cnt);
}

// Function: COUNTIF
Value func_countif(valVector args, ValueCalc *calc, FuncExtra *e)
{
    // the first parameter must be a reference
    if ((e->ranges[0].col1 == -1) || (e->ranges[0].row1 == -1))
        return Value::errorNA();

    Value range = args[0];
    QString condition = calc->conv()->asString(args[1]).asString();

    Condition cond;
    calc->getCond(cond, Value(condition));

    return Value(calc->countIf(range, cond));
}

// Function: FIB
Value func_fib(valVector args, ValueCalc *calc, FuncExtra *)
{
    /*
    Lucas' formula for the nth Fibonacci number F(n) is given by

             ((1+sqrt(5))/2)^n - ((1-sqrt(5))/2)^n
      F(n) = ------------------------------------- .
                             sqrt(5)

    */
    Value n = args[0];
    if (!n.isNumber())
        return Value::errorVALUE();

    if (!calc->greater(n, Value(0.0)))
        return Value::errorNUM();

    Value s = calc->sqrt(Value(5.0));
    // u1 = ((1+sqrt(5))/2)^n
    Value u1 = calc->pow(calc->div(calc->add(Value(1), s), Value(2)), n);
    // u2 = ((1-sqrt(5))/2)^n
    Value u2 = calc->pow(calc->div(calc->sub(Value(1), s), Value(2)), n);

    Value result = calc->div(calc->sub(u1, u2), s);
    return result;
}

static Value func_gcd_helper(const Value &val, ValueCalc *calc)
{
    Value res(0);
    if (!val.isArray())
        return val;
    for (uint row = 0; row < val.rows(); ++row)
        for (uint col = 0; col < val.columns(); ++col) {
            Value v = val.element(col, row);
            if (v.isArray())
                v = func_gcd_helper(v, calc);
            res = calc->gcd(res, calc->roundDown(v));
        }
    return res;
}

// Function: GCD
Value func_gcd(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value result = Value(0);
    for (int i = 0; i < args.count(); ++i) {
        if (args[i].isArray()) {
            result = calc->gcd(result, func_gcd_helper(args[i], calc));
        } else {
            if (args[i].isNumber() && args[i].asInteger() >= 0) {
                result = calc->gcd(result, calc->roundDown(args[i]));
            } else {
                return Value::errorNUM();
            }
        }
    }
    return result;
}

static Value func_lcm_helper(const Value &val, ValueCalc *calc)
{
    Value res = Value(0);
    if (!val.isArray())
        return val;
    for (unsigned int row = 0; row < val.rows(); ++row)
        for (unsigned int col = 0; col < val.columns(); ++col) {
            Value v = val.element(col, row);
            if (v.isArray())
                v = func_lcm_helper(v, calc);
            res = calc->lcm(res, calc->roundDown(v));
        }
    return res;
}

// Function: lcm
Value func_lcm(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value result = Value(0);
    for (int i = 0; i < args.count(); ++i) {
        if (args[i].isArray()) {
            result = calc->lcm(result, func_lcm_helper(args[i], calc));
        } else {
            if (args[i].isNumber() == false) {
                return Value::errorNUM();
            } else {
                // its a number
                if (args[i].asInteger() < 0) {
                    return Value::errorNUM();
                } else if (args[i].asInteger() == 0) {
                    return Value(0);
                } else { // number > 0
                    result = calc->lcm(result, calc->roundDown(args[i]));
                }
            }
        }
    }
    return result;
}

static Eigen::MatrixXd convert(const Value& matrix, ValueCalc *calc)
{
    const int rows = matrix.rows(), cols = matrix.columns();
    Eigen::MatrixXd eMatrix(rows, cols);
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            eMatrix(row, col) = numToDouble(calc->conv()->toFloat(matrix.element(col, row)));
        }
    }
    return eMatrix;
}

static Value convert(const Eigen::MatrixXd& eMatrix)
{
    const int rows = eMatrix.rows(), cols = eMatrix.cols();
    Value matrix(Value::Array);
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            matrix.setElement(col, row, Value(eMatrix(row, col)));
        }
    }
    return matrix;
}

// Function: MDETERM
Value func_mdeterm(valVector args, ValueCalc* calc, FuncExtra*)
{
    Value matrix = args[0];
    if (matrix.columns() != matrix.rows() || matrix.rows() < 1)
        return Value::errorVALUE();

    const Eigen::MatrixXd eMatrix = convert(matrix, calc);

    return Value(eMatrix.determinant());
}

// Function: MINVERSE
Value func_minverse(valVector args, ValueCalc* calc, FuncExtra*)
{
    Value matrix = args[0];
    if (matrix.columns() != matrix.rows() || matrix.rows() < 1)
        return Value::errorVALUE();

    Eigen::MatrixXd eMatrix = convert(matrix, calc),
                              eMatrixInverse(eMatrix.rows(), eMatrix.cols());
    Eigen::LU<Eigen::MatrixXd> lu(eMatrix);
    if (lu.isInvertible()) {
        lu.computeInverse(&eMatrixInverse);
        return convert(eMatrixInverse);
    } else
        return Value::errorDIV0();
}

// Function: mmult
Value func_mmult(valVector args, ValueCalc *calc, FuncExtra *)
{
    const Eigen::MatrixXd eMatrix1 = convert(args[0], calc);
    const Eigen::MatrixXd eMatrix2 = convert(args[1], calc);

    if (eMatrix1.cols() != eMatrix2.rows())    // row/column counts must match
        return Value::errorVALUE();

    return convert(eMatrix1 * eMatrix2);
}

// Function: MUNIT
Value func_munit(valVector args, ValueCalc* calc, FuncExtra*)
{
    const int dim = calc->conv()->asInteger(args[0]).asInteger();
    if (dim < 1)
        return Value::errorVALUE();
    Value result(Value::Array);
    for (int row = 0; row < dim; ++row)
        for (int col = 0; col < dim; ++col)
            result.setElement(col, row, Value(col == row ? 1 : 0));
    return result;
}

// Function: SUBTOTAL
// This function requires access to the Sheet and so on, because
// it needs to check whether cells contain the SUBTOTAL formula or not ...
// Cells containing a SUBTOTAL formula must be ignored.
Value func_subtotal(valVector args, ValueCalc *calc, FuncExtra *e)
{
    int function = calc->conv()->asInteger(args[0]).asInteger();
    Value range = args[1];
    int r1 = -1, c1 = -1, r2 = -1, c2 = -1;
    if (e) {
        r1 = e->ranges[1].row1;
        c1 = e->ranges[1].col1;
        r2 = e->ranges[1].row2;
        c2 = e->ranges[1].col2;
    }

    // exclude manually hidden rows. http://tools.oasis-open.org/issues/browse/OFFICE-2030
    bool excludeHiddenRows = false;
    if(function > 100) {
        excludeHiddenRows = true;
        function = function % 100; // translate e.g. 106 to 6.
    }

    // run through the cells in the selected range
    Value empty;
    if ((r1 > 0) && (c1 > 0) && (r2 > 0) && (c2 > 0)) {
        for (int r = r1; r <= r2; ++r) {
            const bool setAllEmpty = excludeHiddenRows && e->sheet->rowFormat(r)->isHidden();
            for (int c = c1; c <= c2; ++c) {
                // put an empty value to all cells in a hidden row
                if(setAllEmpty) {
                    range.setElement(c - c1, r - r1, empty);
                    continue;
                }
                Cell cell(e->sheet, c, r);
                // put an empty value to the place of all occurrences of the SUBTOTAL function
                if (!cell.isDefault() && cell.isFormula() && cell.userInput().indexOf("SUBTOTAL", 0, Qt::CaseInsensitive) != -1)
                    range.setElement(c - c1, r - r1, empty);
            }
        }
    }

    // Good. Now we can execute the necessary function on the range.
    Value res;
  QSharedPointer<Function> f;
    valVector a;
    switch (function) {
    case 1: // Average
        res = calc->avg(range, false);
        break;
    case 2: // Count
        res = Value(calc->count(range, false));
        break;
    case 3: // CountA
        res = Value(calc->count(range));
        break;
    case 4: // MAX
        res = calc->max(range, false);
        break;
    case 5: // Min
        res = calc->min(range, false);
        break;
    case 6: // Product
        res = calc->product(range, Value(0.0), false);
        break;
    case 7: // StDev
        res = calc->stddev(range, false);
        break;
    case 8: // StDevP
        res = calc->stddevP(range, false);
        break;
    case 9: // Sum
        res = calc->sum(range, false);
        break;
    case 10: // Var
        f = FunctionRepository::self()->function("VAR");
        if (!f) return Value::errorVALUE();
        a.resize(1);
        a[0] = range;
        res = f->exec(a, calc, 0);
        break;
    case 11: // VarP
        f = FunctionRepository::self()->function("VARP");
        if (!f) return Value::errorVALUE();
        a.resize(1);
        a[0] = range;
        res = f->exec(a, calc, 0);
        break;
    default:
        return Value::errorVALUE();
    }
    return res;
}

// Function: TRANSPOSE
Value func_transpose(valVector args, ValueCalc *calc, FuncExtra *)
{
    Q_UNUSED(calc);
    Value matrix = args[0];
    const int cols = matrix.columns();
    const int rows = matrix.rows();

    Value transpose(Value::Array);
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            if (!matrix.element(col, row).isEmpty())
                transpose.setElement(row, col, matrix.element(col, row));
        }
    }
    return transpose;
}

/*
Commented out.
Absolutely no idea what this thing is supposed to do.
To anyone who would enable this code: it still uses koscript calls - you need
to convert it to the new style prior to uncommenting.

// Function: MULTIPLEOPERATIONS
Value func_multipleOP (valVector args, ValueCalc *calc, FuncExtra *)
{
  if (gCell)
  {
    context.setValue( new KSValue( ((Interpreter *) context.interpreter() )->cell()->value().asFloat() ) );
    return true;
  }

  gCell = ((Interpreter *) context.interpreter() )->cell();

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

  //  ((Interpreter *) context.interpreter() )->document()->emitBeginOperation();

  double oldCol = args[1]->doubleValue();
  double oldRow = args[3]->doubleValue();
  kDebug() <<"Old values: Col:" << oldCol <<", Row:" << oldRow;

  Cell * cell;
  Sheet * sheet = ((Interpreter *) context.interpreter() )->sheet();

  Point point( extra[1]->stringValue() );
  Point point2( extra[3]->stringValue() );
  Point point3( extra[0]->stringValue() );

  if ( ( args[1]->doubleValue() != args[2]->doubleValue() )
       || ( args[3]->doubleValue() != args[4]->doubleValue() ) )
  {
    cell = Cell( sheet, point.pos.x(), point.pos.y() );
    cell->setValue( args[2]->doubleValue() );
    kDebug() <<"Setting value" << args[2]->doubleValue() <<" on cell" << point.pos.x()
              << ", " << point.pos.y() << endl;

    cell = Cell( sheet, point2.pos.x(), point.pos.y() );
    cell->setValue( args[4]->doubleValue() );
    kDebug() <<"Setting value" << args[4]->doubleValue() <<" on cell" << point2.pos.x()
              << ", " << point2.pos.y() << endl;
  }

  Cell * cell1 = Cell( sheet, point3.pos.x(), point3.pos.y() );
  cell1->calc( false );

  double d = cell1->value().asFloat();
  kDebug() <<"Cell:" << point3.pos.x() <<";" << point3.pos.y() <<" with value"
            << d << endl;

  kDebug() <<"Resetting old values";

  cell = Cell( sheet, point.pos.x(), point.pos.y() );
  cell->setValue( oldCol );

  cell = Cell( sheet, point2.pos.x(), point2.pos.y() );
  cell->setValue( oldRow );

  cell1->calc( false );

  // ((Interpreter *) context.interpreter() )->document()->emitEndOperation();

  context.setValue( new KSValue( (double) d ) );

  gCell = 0;
  return true;
}

*/

#include "MathModule.moc"
