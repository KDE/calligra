/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team <koffice-devel@kde.org>
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

// built-in statistical functions

#include "functions.h"
#include "valuecalc.h"
#include "valueconverter.h"

// needed for MODE
#include <qlist.h>
#include <qmap.h>

using namespace KSpread;

// prototypes (sorted!)
Value func_arrang (valVector args, ValueCalc *calc, FuncExtra *);
Value func_average (valVector args, ValueCalc *calc, FuncExtra *);
Value func_averagea (valVector args, ValueCalc *calc, FuncExtra *);
Value func_avedev (valVector args, ValueCalc *calc, FuncExtra *);
Value func_betadist (valVector args, ValueCalc *calc, FuncExtra *);
Value func_bino (valVector args, ValueCalc *calc, FuncExtra *);
Value func_chidist (valVector args, ValueCalc *calc, FuncExtra *);
Value func_combin (valVector args, ValueCalc *calc, FuncExtra *);
Value func_confidence (valVector args, ValueCalc *calc, FuncExtra *);
Value func_correl_pop (valVector args, ValueCalc *calc, FuncExtra *);
Value func_covar (valVector args, ValueCalc *calc, FuncExtra *);
Value func_devsq (valVector args, ValueCalc *calc, FuncExtra *);
Value func_devsqa (valVector args, ValueCalc *calc, FuncExtra *);
Value func_expondist (valVector args, ValueCalc *calc, FuncExtra *);
Value func_fdist (valVector args, ValueCalc *calc, FuncExtra *);
Value func_fisher (valVector args, ValueCalc *calc, FuncExtra *);
Value func_fisherinv (valVector args, ValueCalc *calc, FuncExtra *);
Value func_gammadist (valVector args, ValueCalc *calc, FuncExtra *);
Value func_gammaln (valVector args, ValueCalc *calc, FuncExtra *);
Value func_gauss (valVector args, ValueCalc *calc, FuncExtra *);
Value func_geomean (valVector args, ValueCalc *calc, FuncExtra *);
Value func_harmean (valVector args, ValueCalc *calc, FuncExtra *);
Value func_hypgeomdist (valVector args, ValueCalc *calc, FuncExtra *);
Value func_kurtosis_est (valVector args, ValueCalc *calc, FuncExtra *);
Value func_kurtosis_pop (valVector args, ValueCalc *calc, FuncExtra *);
Value func_large (valVector args, ValueCalc *calc, FuncExtra *);
Value func_loginv (valVector args, ValueCalc *calc, FuncExtra *);
Value func_lognormdist (valVector args, ValueCalc *calc, FuncExtra *);
Value func_median (valVector args, ValueCalc *calc, FuncExtra *);
Value func_mode (valVector args, ValueCalc *calc, FuncExtra *);
Value func_negbinomdist (valVector args, ValueCalc *calc, FuncExtra *);
Value func_normdist (valVector args, ValueCalc *calc, FuncExtra *);
Value func_norminv (valVector args, ValueCalc *calc, FuncExtra *);
Value func_normsinv (valVector args, ValueCalc *calc, FuncExtra *);
Value func_phi (valVector args, ValueCalc *calc, FuncExtra *);
Value func_poisson (valVector args, ValueCalc *calc, FuncExtra *);
Value func_skew_est (valVector args, ValueCalc *calc, FuncExtra *);
Value func_skew_pop (valVector args, ValueCalc *calc, FuncExtra *);
Value func_small (valVector args, ValueCalc *calc, FuncExtra *);
Value func_standardize (valVector args, ValueCalc *calc, FuncExtra *);
Value func_stddev (valVector args, ValueCalc *calc, FuncExtra *);
Value func_stddeva (valVector args, ValueCalc *calc, FuncExtra *);
Value func_stddevp (valVector args, ValueCalc *calc, FuncExtra *);
Value func_stddevpa (valVector args, ValueCalc *calc, FuncExtra *);
Value func_stdnormdist (valVector args, ValueCalc *calc, FuncExtra *);
Value func_sumproduct (valVector args, ValueCalc *calc, FuncExtra *);
Value func_sumx2py2 (valVector args, ValueCalc *calc, FuncExtra *);
Value func_sumx2my2 (valVector args, ValueCalc *calc, FuncExtra *);
Value func_sumxmy2 (valVector args, ValueCalc *calc, FuncExtra *);
Value func_tdist (valVector args, ValueCalc *calc, FuncExtra *);
Value func_variance (valVector args, ValueCalc *calc, FuncExtra *);
Value func_variancea (valVector args, ValueCalc *calc, FuncExtra *);
Value func_variancep (valVector args, ValueCalc *calc, FuncExtra *);
Value func_variancepa (valVector args, ValueCalc *calc, FuncExtra *);
Value func_weibull (valVector args, ValueCalc *calc, FuncExtra *);

typedef QList<double> List;

// registers all statistical functions
void RegisterStatisticalFunctions()
{
  FunctionRepository* repo = FunctionRepository::self();
  Function *f;

  f = new Function ("AVEDEV", func_avedev);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("AVERAGE", func_average);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("AVERAGEA", func_averagea);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("BETADIST", func_betadist);
  f->setParamCount (3, 5);
  repo->add (f);
  f = new Function ("BINO", func_bino);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("CHIDIST", func_chidist);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("COMBIN", func_combin);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("CONFIDENCE", func_confidence);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("CORREL", func_correl_pop);
  f->setParamCount (2);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("COVAR", func_covar);
  f->setParamCount (2);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("DEVSQ", func_devsq);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("DEVSQA", func_devsqa);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("EXPONDIST", func_expondist);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("FDIST", func_fdist);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("FISHER", func_fisher);
  repo->add (f);
  f = new Function ("FISHERINV", func_fisherinv);
  repo->add (f);
  f = new Function ("GAMMADIST", func_gammadist);
  f->setParamCount (4);
  repo->add (f);
  f = new Function ("GAMMALN", func_gammaln);
  repo->add (f);
  f = new Function ("GAUSS", func_gauss);
  repo->add (f);
  f = new Function ("GEOMEAN", func_geomean);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("HARMEAN", func_harmean);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("HYPGEOMDIST", func_hypgeomdist);
  f->setParamCount (4);
  repo->add (f);
  f = new Function ("INVBINO", func_bino);  // same as BINO, for 1.4 compat
  repo->add (f);
  f = new Function ("KURT", func_kurtosis_est);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("KURTP", func_kurtosis_pop);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("LARGE", func_large);
  f->setParamCount (2);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("LOGINV", func_loginv);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("LOGNORMDIST", func_lognormdist);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("MEDIAN", func_median);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("MODE", func_mode);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("NEGBINOMDIST", func_negbinomdist);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("NORMDIST", func_normdist);
  f->setParamCount (4);
  repo->add (f);
  f = new Function ("NORMINV", func_norminv);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("NORMSDIST", func_stdnormdist);
  repo->add (f);
  f = new Function ("NORMSINV", func_normsinv);
  repo->add (f);
  f = new Function ("PEARSON", func_correl_pop);
  f->setParamCount (2);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("PERMUT", func_arrang);
  f->setParamCount (2);
  repo->add (f);
  f = new Function ("PHI", func_phi);
  repo->add (f);
  f = new Function ("POISSON", func_poisson);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("SKEW", func_skew_est);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("SKEWP", func_skew_pop);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("SMALL", func_small);
  f->setParamCount (2);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("STANDARDIZE", func_standardize);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("STDEV", func_stddev);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("STDEVA", func_stddeva);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("STDEVP", func_stddevp);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("STDEVPA", func_stddevpa);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("SUM2XMY", func_sumxmy2);
  f->setParamCount (2);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("SUMPRODUCT", func_sumproduct);
  f->setParamCount (2);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("SUMX2PY2", func_sumx2py2);
  f->setParamCount (2);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("SUMX2MY2", func_sumx2my2);
  f->setParamCount (2);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("TDIST", func_tdist);
  f->setParamCount (3);
  repo->add (f);
  f = new Function ("VARIANCE", func_variance);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("VAR", func_variance);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("VARP", func_variancep);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("VARA", func_variancea);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("VARPA", func_variancepa);
  f->setParamCount (1, -1);
  f->setAcceptArray ();
  repo->add (f);
  f = new Function ("WEIBULL", func_weibull);
  f->setParamCount (4);
  repo->add (f);
}

// array-walk functions used in this file

void awSkew (ValueCalc *c, Value &res, Value val, Value p)
{
  Value avg = p.element (0, 0);
  Value stdev = p.element (1, 0);
  // (val - avg) / stddev
  Value d = c->div (c->sub (val, avg), stdev);
  // res += d*d*d
  res = c->add (res, c->mul (d, c->mul (d, d)));
}

void awSumInv (ValueCalc *c, Value &res, Value val, Value)
{
  // res += 1/value
  res = c->add (res, c->div (1.0, val));
}

void awAveDev (ValueCalc *c, Value &res, Value val,
    Value p)
{
  // res += abs (val - p)
  res = c->add (res, c->abs (c->sub (val, p)));
}

void awKurtosis (ValueCalc *c, Value &res, Value val,
    Value p)
{
  Value avg = p.element (0, 0);
  Value stdev = p.element (1, 0);
  //d = (val - avg ) / stdev
  Value d = c->div (c->sub (val, avg), stdev);
  // res += d^4
  res = c->add (res, c->pow (d, 4));
}


Value func_skew_est (valVector args, ValueCalc *calc, FuncExtra *)
{
  int number = calc->count (args);
  Value avg = calc->avg (args);
  if (number < 3)
    return Value::errorVALUE();

  Value res = calc->stddev (args, avg);
  if (res.isZero())
    return Value::errorVALUE();

  Value params (2, 1);
  params.setElement (0, 0, avg);
  params.setElement (1, 0, res);
  Value tskew;
  calc->arrayWalk (args, tskew, awSkew, params);

  // ((tskew * number) / (number-1)) / (number-2)
  return calc->div (calc->div (calc->mul (tskew, number), number-1), number-2);
}

Value func_skew_pop (valVector args, ValueCalc *calc, FuncExtra *)
{
  int number = calc->count (args);
  Value avg = calc->avg (args);
  if (number < 1)
    return Value::errorVALUE();

  Value res = calc->stddevP (args, avg);
  if (res.isZero())
    return Value::errorVALUE();

  Value params (2, 1);
  params.setElement (0, 0, avg);
  params.setElement (1, 0, res);
  Value tskew;
  calc->arrayWalk (args, tskew, awSkew, params);

  // tskew / number
  return calc->div (tskew, number);
}

class ContentSheet : public QMap<double, int> {};

void func_mode_helper (Value range, ValueCalc *calc, ContentSheet &sh)
{
  if (!range.isArray())
  {
    double d = calc->conv()->asFloat (range).asFloat();
    sh[d]++;
    return;
  }

  for (unsigned int row = 0; row < range.rows(); ++row)
    for (unsigned int col = 0; col < range.columns(); ++col) {
      Value v = range.element (col, row);
      if (v.isArray())
        func_mode_helper (v, calc, sh);
      else {
        double d = calc->conv()->asFloat (v).asFloat();
        sh[d]++;
      }
    }
}

Value func_mode (valVector args, ValueCalc *calc, FuncExtra *)
{
  // does NOT support anything other than doubles !!!
  ContentSheet sh;
  for (int i = 0; i < args.count(); ++i)
    func_mode_helper (args[i], calc, sh);

  // retrieve value with max.count
  int maxcount = 0;
  double max = 0.0;
  ContentSheet::iterator it;
  for (it = sh.begin(); it != sh.end(); ++it)
    if (it.value() > maxcount) {
      max = it.key();
      maxcount = it.value();
    }
  return Value (max);
}

Value func_covar_helper (Value range1, Value range2,
    ValueCalc *calc, Value avg1, Value avg2)
{
  // two arrays -> cannot use arrayWalk
  if ((!range1.isArray()) && (!range2.isArray()))
    // (v1-E1)*(v2-E2)
    return calc->mul (calc->sub (range1, avg1), calc->sub (range2, avg2));

  int rows = range1.rows();
  int cols = range1.columns();
  int rows2 = range2.rows();
  int cols2 = range2.columns();
  if ((rows != rows2) || (cols != cols2))
    return Value::errorVALUE();

  Value result = 0.0;
  for (int row = 0; row < rows; ++row)
    for (int col = 0; col < cols; ++col) {
      Value v1 = range1.element (col, row);
      Value v2 = range2.element (col, row);
      if (v1.isArray() || v2.isArray())
        result = calc->add (result,
            func_covar_helper (v1, v2, calc, avg1, avg2));
      else
        // result += (v1-E1)*(v2-E2)
        result = calc->add (result, calc->mul (calc->sub (v1, avg1),
            calc->sub (v2, avg2)));
    }

  return result;
}

Value func_covar (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value avg1 = calc->avg (args[0]);
  Value avg2 = calc->avg (args[1]);
  int number = calc->count (args[0]);
  int number2 = calc->count (args[1]);

  if (number2 <= 0 || number2 != number)
    return Value::errorVALUE();

  Value covar = func_covar_helper (args[0], args[1], calc, avg1, avg2);
  return calc->div (covar, number);
}

Value func_correl_pop (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value covar = func_covar (args, calc, 0);
  Value stdevp1 = calc->stddevP (args[0]);
  Value stdevp2 = calc->stddevP (args[1]);

  if (calc->isZero (stdevp1) || calc->isZero (stdevp2))
    return Value::errorDIV0();

  // covar / (stdevp1 * stdevp2)
  return calc->div (covar, calc->mul (stdevp1, stdevp2));
}

void func_array_helper (Value range, ValueCalc *calc,
    List &array, int &number)
{
  if (!range.isArray())
  {
    array << calc->conv()->asFloat (range).asFloat();
    ++number;
    return;
  }

  for (unsigned int row = 0; row < range.rows(); ++row)
    for (unsigned int col = 0; col < range.columns(); ++col) {
      Value v = range.element (col, row);
      if (v.isArray ())
        func_array_helper (v, calc, array, number);
      else {
        array << calc->conv()->asFloat (v).asFloat();
        ++number;
      }
    }
}

Value func_large (valVector args, ValueCalc *calc, FuncExtra *)
{
  // does NOT support anything other than doubles !!!
  int k = calc->conv()->asInteger (args[1]).asInteger();
  if ( k < 1 )
    return false;

  List array;
  int number = 1;

  func_array_helper (args[0], calc, array, number);

  if ( k > number )
    return Value::errorVALUE();

  qSort(array);
  double d = array.at(number - k - 1);
  return Value (d);
}

Value func_small (valVector args, ValueCalc *calc, FuncExtra *)
{
  // does NOT support anything other than doubles !!!
  int k = calc->conv()->asInteger (args[1]).asInteger();
  if ( k < 1 )
    return false;

  List array;
  int number = 1;

  func_array_helper (args[0], calc, array, number);

  if ( k > number )
    return Value::errorVALUE();

  qSort(array);
  double d = array.at(k - 1);
  return Value (d);
}

Value func_geomean (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value count = calc->count (args);
  Value prod = calc->product (args, 1.0);
  if (calc->isZero (count))
    return Value::errorDIV0();
  return calc->pow (prod, calc->div (1.0, count));
}

Value func_harmean (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value count = calc->count (args);
  if (calc->isZero (count))
    return Value::errorDIV0();
  Value suminv;
  calc->arrayWalk (args, suminv, awSumInv, 0);
  return calc->div (suminv, count);
}

Value func_loginv (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value p = args[0];
  Value m = args[1];
  Value s = args[2];

  if (calc->lower (p, 0) || calc->greater (p, 1))
    return Value::errorVALUE();

  if (!calc->greater (s, 0))
    return Value::errorVALUE();

  Value result = 0.0;
  if (calc->equal (p, 1))   //p==1
    result = Value::errorVALUE();
  else if (calc->greater (p, 0)) {   //p>0
    Value gaussInv = calc->gaussinv (p);
    // exp (gaussInv * s + m)
    result = calc->exp (calc->add (calc->mul (s, gaussInv), m));
  }

  return result;
}

Value func_devsq (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value res;
  calc->arrayWalk (args, res, calc->awFunc ("devsq"), calc->avg (args, false));
  return res;
}

Value func_devsqa (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value res;
  calc->arrayWalk (args, res, calc->awFunc ("devsqa"), calc->avg (args));
  return res;
}

Value func_kurtosis_est (valVector args, ValueCalc *calc, FuncExtra *)
{
  int count = calc->count (args);
  if (count < 4)
    return Value::errorVALUE();

  Value avg = calc->avg (args);
  Value devsq;
  calc->arrayWalk (args, devsq, calc->awFunc ("devsqa"), avg);

  if (devsq.isZero ())
    return Value::errorDIV0();

  Value params (2, 1);
  params.setElement (0, 0, avg);
  params.setElement (1, 0, devsq);
  Value x4;
  calc->arrayWalk (args, x4, awKurtosis, params);

  double den = (double) (count - 2) * (count - 3);
  double nth = (double) count * (count + 1) / ((count - 1) * den);
  double t = 3.0 * (count - 1) * (count - 1) / den;

  // res = x4 * nth - t
  return calc->sub (calc->mul (x4, nth), t);
}

Value func_kurtosis_pop (valVector args, ValueCalc *calc, FuncExtra *)
{
  int count = calc->count (args);
  if (count < 4)
    return Value::errorVALUE();

  Value avg = calc->avg (args);
  Value devsq;
  calc->arrayWalk (args, devsq, calc->awFunc ("devsqa"), avg);

  if (devsq.isZero ())
    return Value::errorDIV0();

  Value params (2, 1);
  params.setElement (0, 0, avg);
  params.setElement (1, 0, devsq);
  Value x4;
  calc->arrayWalk (args, x4, awKurtosis, params);

  // x4 / count - 3
  return calc->sub (calc->div (x4, count), 3);
}

Value func_standardize (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value x = args[0];
  Value m = args[1];
  Value s = args[2];

  if (!calc->greater (s, 0))  // s must be >0
    return Value::errorVALUE();

  // (x - m) / s
  return calc->div (calc->sub (x, m), s);
}

Value func_hypgeomdist (valVector args, ValueCalc *calc, FuncExtra *)
{
  int x = calc->conv()->asInteger (args[0]).asInteger();
  int n = calc->conv()->asInteger (args[1]).asInteger();
  int M = calc->conv()->asInteger (args[2]).asInteger();
  int N = calc->conv()->asInteger (args[3]).asInteger();

  if ( x < 0 || n < 0 || M < 0 || N < 0 )
    return Value::errorVALUE();

  if ( x > M || n > N )
    return Value::errorVALUE();

  Value d1 = calc->combin (M, x);
  Value d2 = calc->combin (N - M, n - x);
  Value d3 = calc->combin (N, n);

  // d1 * d2 / d3
  return calc->div (calc->mul (d1, d2), d3);
}

Value func_negbinomdist (valVector args, ValueCalc *calc, FuncExtra *)
{
  int x = calc->conv()->asInteger (args[0]).asInteger();
  int r = calc->conv()->asInteger (args[1]).asInteger();
  Value p = args[2];

  if ((x + r - 1) <= 0)
    return Value::errorVALUE();
  if (calc->lower (p, 0) || calc->greater (p, 1))
    return Value::errorVALUE();

  Value d1 = calc->combin (x + r - 1, r - 1);
  // d2 = pow (p, r) * pow (1 - p, x)
  Value d2 = calc->mul (calc->pow (p, r),
      calc->pow (calc->sub (1, p), x));

  return calc->mul (d1, d2);
}

// Function: permut
Value func_arrang (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value n = args[0];
  Value m = args[1];
  if (calc->lower (n, m))  // problem if n<m
    return Value::errorVALUE();

  if (calc->lower (m, 0))  // problem if m<0  (n>=m so that's okay)
    return Value::errorVALUE();

  // fact(n) / (fact(n-m)
  return calc->fact (n, calc->sub (n, m));
}

// Function: average
Value func_average (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->avg (args, false);
}

// Function: averagea
Value func_averagea (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->avg (args);
}

// Function: avedev
Value func_avedev (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value result;
  calc->arrayWalk (args, result, awAveDev, calc->avg (args));
  return result;
}

// Function: median
Value func_median (valVector args, ValueCalc *calc, FuncExtra *)
{
  // does NOT support anything other than doubles !!!
  List array;
  int number = 1;

  for (int i = 0; i < args.count(); ++i)
    func_array_helper (args[i], calc, array, number);

  qSort(array);
  double d = array.at(number / 2 + number % 2);
  return Value (d);
}

// Function: variance
Value func_variance (valVector args, ValueCalc *calc, FuncExtra *)
{
  int count = calc->count (args, false);
  if (count < 2)
    return Value::errorVALUE();

  Value result = func_devsq (args, calc, 0);
  return calc->div (result, count-1);
}

// Function: vara
Value func_variancea (valVector args, ValueCalc *calc, FuncExtra *)
{
  int count = calc->count (args);
  if (count < 2)
    return Value::errorVALUE();

  Value result = func_devsqa (args, calc, 0);
  return calc->div (result, count-1);
}

// Function: varp
Value func_variancep (valVector args, ValueCalc *calc, FuncExtra *)
{
  int count = calc->count (args, false);
  if (count == 0)
    return Value::errorVALUE();

  Value result = func_devsq (args, calc, 0);
  return calc->div (result, count);
}

// Function: varpa
Value func_variancepa (valVector args, ValueCalc *calc, FuncExtra *)
{
  int count = calc->count (args);
  if (count == 0)
    return Value::errorVALUE();

  Value result = func_devsqa (args, calc, 0);
  return calc->div (result, count);
}

// Function: stddev
Value func_stddev (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->stddev (args, false);
}

// Function: stddeva
Value func_stddeva (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->stddev (args);
}

// Function: stddevp
Value func_stddevp (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->stddevP (args, false);
}

// Function: stddevpa
Value func_stddevpa (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->stddevP (args);
}

// Function: combin
Value func_combin (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->combin (args[0], args[1]);
}

// Function: bino
Value func_bino (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value n = args[0];
  Value m = args[1];
  Value comb = calc->combin (n, m);
  Value prob = args[2];

  if (calc->lower (prob,0) || calc->greater (prob,1))
    return Value::errorVALUE();

  // result = comb * pow (prob, m) * pow (1 - prob, n - m)
  Value pow1 = calc->pow (prob, m);
  Value pow2 = calc->pow (calc->sub (1, prob), calc->sub (n, m));
  return calc->mul (comb, calc->mul (pow1, pow2));
}

// Function: phi
Value func_phi (valVector args, ValueCalc *calc, FuncExtra *)
//distribution function for a standard normal distribution
{
  return calc->phi (args[0]);
}

// Function: gauss
Value func_gauss (valVector args, ValueCalc *calc, FuncExtra *)
{
  //returns the integral values of the standard normal cumulative distribution
  return calc->gauss (args[0]);
}

// Function: gammadist
Value func_gammadist (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value x = args[0];
  Value alpha = args[1];
  Value beta = args[2];
  int kum = calc->conv()->asInteger (args[3]).asInteger();  // 0 or 1

  Value result;

  if (calc->lower (x, 0.0) || (!calc->greater (alpha, 0.0)) ||
      (!calc->greater (beta, 0.0)))
    return Value::errorVALUE();

  if (kum == 0) {  //density
    Value G = calc->GetGamma (alpha);
    // result = pow (x, alpha - 1.0) / exp (x / beta) / pow (beta, alpha) / G
    Value pow1 = calc->pow (x, calc->sub (alpha, 1.0));
    Value pow2 = calc->exp (calc->div (x, beta));
    Value pow3 = calc->pow (beta, alpha);
    result = calc->div (calc->div (calc->div (pow1, pow2), pow3), G);
  }
  else
    result = calc->GetGammaDist (x, alpha, beta);

  return Value (result);
}

// Function: betadist
Value func_betadist (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value x = args[0];
  Value alpha = args[1];
  Value beta = args[2];

  Value fA = 0.0;
  Value fB = 1.0;
  if (args.count() > 3) fA = args[3];
  if (args.count() == 5) fB = args[4];

  //x < fA || x > fB || fA == fB || alpha <= 0.0 || beta <= 0.0
  if (calc->lower (x, fA) || calc->greater (x, fB) || calc->equal (fA, fB) ||
      (!calc->greater (alpha, 0.0)) || (!calc->greater (beta, 0.0)))
    return Value::errorVALUE();

  // xx = (x - fA) / (fB - fA)  // scaling
  Value xx = calc->div (calc->sub (x, fA), calc->sub (fB, fA));

  return calc->GetBeta (xx, alpha, beta);
}

// Function: fisher
Value func_fisher (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the Fisher transformation for x

  // 0.5 * ln ((1.0 + fVal) / (1.0 - fVal))
  Value fVal = args[0];
  Value num = calc->div (calc->add (fVal, 1.0), calc->sub (1.0, fVal));
  return calc->mul (calc->ln (num), 0.5);
}

// Function: fisherinv
Value func_fisherinv (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the inverse of the Fisher transformation for x

  Value fVal = args[0];
  // (exp (2.0 * fVal) - 1.0) / (exp (2.0 * fVal) + 1.0)
  Value ex = calc->exp (calc->mul (fVal, 2.0));
  return calc->div (calc->sub (ex, 1.0), calc->add (ex, 1.0));
}

// Function: normdist
Value func_normdist (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the normal cumulative distribution
  Value x = args[0];
  Value mue = args[1];
  Value sigma = args[2];
  Value k = args[3];

  if (!calc->greater (sigma, 0.0))
    return Value::errorVALUE();

  // (x - mue) / sigma
  Value Y = calc->div (calc->sub (x, mue), sigma);
  if (calc->isZero (k))   // density
    return calc->div (calc->phi (Y), sigma);
  else          // distribution
    return calc->add (calc->gauss (Y), 0.5);
}

// Function: lognormdist
Value func_lognormdist (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the cumulative lognormal distribution
  Value x = args[0];
  Value mue = args[1];
  Value sigma = args[2];

  if (!calc->greater (sigma, 0.0) || (!calc->greater (x, 0.0)))
    return Value::errorVALUE();

  // (ln(x) - mue) / sigma
  Value Y = calc->div (calc->sub (calc->ln (x), mue), sigma);
  return calc->add (calc->gauss (Y), 0.5);
}

// Function: normsdist
Value func_stdnormdist (valVector args, ValueCalc *calc, FuncExtra *)
{
  //returns the cumulative lognormal distribution, mue=0, sigma=1
  return calc->add (calc->gauss (args[0]), 0.5);
}

// Function: expondist
Value func_expondist (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the exponential distribution
  Value x = args[0];
  Value lambda = args[1];
  Value kum = args[2];

  Value result = 0.0;

  if (!calc->greater (lambda, 0.0))
    return Value::errorVALUE();

  // ex = exp (-lambda * x)
  Value ex = calc->exp (calc->mul (calc->mul (lambda, -1), x));
  if (calc->isZero (kum)) {  //density
    if (!calc->lower (x, 0.0))
      // lambda * ex
      result = calc->mul (lambda, ex);
  }
  else {  //distribution
    if (calc->greater (x, 0.0))
      // 1.0 - ex
      result = calc->sub (1.0, ex);
  }
  return result;
}

// Function: weibull
Value func_weibull (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the Weibull distribution

  Value x = args[0];
  Value alpha = args[1];
  Value beta = args[2];
  Value kum = args[3];

  Value result;

  if ((!calc->greater (alpha, 0.0)) || (!calc->greater (beta, 0.0)) ||
      calc->lower (x, 0.0))
    return Value::errorVALUE();

  // ex = exp (-pow (x / beta, alpha))
  Value ex;
  ex = calc->exp (calc->mul (calc->pow (calc->div (x, beta), alpha), -1));
  if (calc->isZero (kum))    // density
  {
    // result = alpha / pow(beta,alpha) * pow(x,alpha-1.0) * ex
    result = calc->div (alpha, calc->pow (beta, alpha));
    result = calc->mul (result, calc->mul (calc->pow (x,
        calc->sub (alpha, 1)), ex));
  }
  else    // distribution
    result = calc->sub (1.0, ex);

  return result;
}

// Function: normsinv
Value func_normsinv (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the inverse of the standard normal cumulative distribution

  Value x = args[0];
  if (!(calc->greater (x, 0.0) && calc->lower (x, 1.0)))
    return Value::errorVALUE();

  return calc->gaussinv (x);
}

// Function: norminv
Value func_norminv (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the inverse of the normal cumulative distribution
  Value x = args[0];
  Value mue = args[1];
  Value sigma = args[2];

  if (!calc->greater (sigma, 0.0))
    return Value::errorVALUE();
  if (!(calc->greater (x, 0.0) && calc->lower (x, 1.0)))
    return Value::errorVALUE();

  // gaussinv (x)*sigma + mue
  return calc->add (calc->mul (calc->gaussinv (x), sigma), mue);
}

// Function: gammaln
Value func_gammaln (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the natural logarithm of the gamma function

  if (calc->greater (args[0], 0.0))
    return calc->GetLogGamma (args[0]);
  return Value::errorVALUE();
}

// Function: poisson
Value func_poisson (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the Poisson distribution

  Value x = args[0];
  Value lambda = args[1];
  Value kum = args[2];

  // lambda < 0.0 || x < 0.0
  if (calc->lower (lambda, 0.0) || calc->lower (x, 0.0))
    return Value::errorVALUE();

  Value result;

  // ex = exp (-lambda)
  Value ex = calc->exp (calc->mul (lambda, -1));

  if (calc->isZero (kum)) {   // density
    if (calc->isZero (lambda))
      result = 0;
    else
      // ex * pow (lambda, x) / fact (x)
    result = calc->div (calc->mul (ex, calc->pow (lambda, x)), calc->fact (x));
  }
  else {   // distribution
    if (calc->isZero (lambda))
      result = 1;
    else
    {
      result = 1.0;
      Value fFak = 1.0;
      unsigned long nEnd = calc->conv()->asInteger (x).asInteger();
      for (unsigned long i = 1; i <= nEnd; i++)
      {
        // fFak *= i
        fFak = calc->mul (fFak, i);
        // result += pow (lambda, i) / fFak
        result = calc->add (result, calc->div (calc->pow (lambda, i), fFak));
      }
      result = calc->mul (result, ex);
    }
  }

  return result;
}

// Function: confidence
Value func_confidence (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the confidence interval for a population mean
  Value alpha = args[0];
  Value sigma = args[1];
  Value n = args[2];

  // sigma <= 0.0 || alpha <= 0.0 || alpha >= 1.0 || n < 1
  if ((!calc->greater (sigma, 0.0)) || (!calc->greater (alpha, 0.0)) ||
      (!calc->lower (alpha, 1.0)) || calc->lower (n, 1))
    return Value::errorVALUE();

  // g = gaussinv (1.0 - alpha / 2.0)
  Value g = calc->gaussinv (calc->sub (1.0, calc->div (alpha, 2.0)));
  // g * sigma / sqrt (n)
  return calc->div (calc->mul (g, sigma), calc->sqrt (n));
}

// Function: tdist
Value func_tdist (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the t-distribution

  Value T = args[0];
  Value fDF = args[1];
  int flag = calc->conv()->asInteger (args[2]).asInteger();

  if (calc->lower (fDF, 1) || calc->lower (T, 0.0) || (flag != 1 && flag != 2))
    return Value::errorVALUE();

  // arg = fDF / (fDF + T * T)
  Value arg = calc->div (fDF, calc->add (fDF, calc->sqr (T)));

  Value R;
  R = calc->mul (calc->GetBeta (arg, calc->div (fDF, 2.0), 0.5), 0.5);

  if (flag == 1)
    return R;
  return calc->mul (R, 2);   // flag is 2 here
}

// Function: fdist
Value func_fdist (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the f-distribution

  Value x = args[0];
  Value fF1 = args[1];
  Value fF2 = args[2];

  // x < 0.0 || fF1 < 1 || fF2 < 1 || fF1 >= 1.0E10 || fF2 >= 1.0E10
  if (calc->lower (x, 0.0) || calc->lower (fF1, 1) || calc->lower (fF2, 1) ||
      (!calc->lower (fF1, 1.0E10)) || (!calc->lower (fF2, 1.0E10)))
    return Value::errorVALUE();

  // arg = fF2 / (fF2 + fF1 * x)
  Value arg = calc->div (fF2, calc->add (fF2, calc->mul (fF1, x)));
  // alpha = fF2/2.0
  Value alpha = calc->div (fF2, 2.0);
  // beta = fF1/2.0
  Value beta = calc->div (fF1, 2.0);
  return calc->GetBeta (arg, alpha, beta);
}

// Function: chidist
Value func_chidist (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the chi-distribution

  Value fChi = args[0];
  Value fDF = args[1];

  // fDF < 1 || fDF >= 1.0E5 || fChi < 0.0
  if (calc->lower (fDF, 1) || (!calc->lower (fDF, 1.0E5)) ||
      calc->lower (fChi, 0.0))
    return Value::errorVALUE();

  // 1.0 - GetGammaDist (fChi / 2.0, fDF / 2.0, 1.0)
  return calc->sub (1.0, calc->GetGammaDist (calc->div (fChi, 2.0),
      calc->div (fDF, 2.0), 1.0));
}


// two-array-walk functions used in the two-sum functions

void tawSumproduct (ValueCalc *c, Value &res, Value v1,
    Value v2) {
  // res += v1*v2
  res = c->add (res, c->mul (v1, v2));
}

void tawSumx2py2 (ValueCalc *c, Value &res, Value v1,
    Value v2) {
  // res += sqr(v1)+sqr(v2)
  res = c->add (res, c->add (c->sqr (v1), c->sqr (v2)));
}

void tawSumx2my2 (ValueCalc *c, Value &res, Value v1,
    Value v2) {
  // res += sqr(v1)-sqr(v2)
  res = c->add (res, c->sub (c->sqr (v1), c->sqr (v2)));
}

void tawSumxmy2 (ValueCalc *c, Value &res, Value v1,
    Value v2) {
  // res += sqr(v1-v2)
  res = c->add (res, c->sqr (c->sub (v1, v2)));

}

// Function: sumproduct
Value func_sumproduct (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value result;
  calc->twoArrayWalk (args[0], args[1], result, tawSumproduct);
  return result;
}

// Function: sumx2py2
Value func_sumx2py2 (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value result;
  calc->twoArrayWalk (args[0], args[1], result, tawSumx2py2);
  return result;
}

// Function: sumx2my2
Value func_sumx2my2 (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value result;
  calc->twoArrayWalk (args[0], args[1], result, tawSumx2my2);
  return result;
}

// Function: sum2xmy
Value func_sumxmy2 (valVector args, ValueCalc *calc, FuncExtra *)
{
  Value result;
  calc->twoArrayWalk (args[0], args[1], result, tawSumxmy2);
  return result;
}
