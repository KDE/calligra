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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

// built-in statistical functions

#include "functions.h"
#include "valuecalc.h"
#include "valueconverter.h"

// needed for MODE
#include <qmap.h>

using namespace KSpread;

// prototypes (sorted!)
KSpreadValue func_arrang (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_average (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_averagea (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_avedev (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_betadist (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_bino (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_chidist (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_combin (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_confidence (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_correl_pop (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_covar (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_devsq (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_devsqa (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_expondist (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_fdist (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_fisher (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_fisherinv (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_gammadist (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_gammaln (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_gauss (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_geomean (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_harmean (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_hypgeomdist (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_kurtosis_est (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_kurtosis_pop (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_large (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_loginv (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_lognormdist (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_median (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_mode (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_negbinomdist (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_normdist (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_norminv (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_normsinv (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_phi (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_poisson (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_skew_est (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_skew_pop (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_small (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_standardize (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_stddev (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_stddeva (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_stddevp (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_stddevpa (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_stdnormdist (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_sumproduct (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_sumx2py2 (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_sumx2my2 (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_sumxmy2 (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_tdist (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_variance (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_variancea (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_variancep (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_variancepa (valVector args, ValueCalc *calc, FuncExtra *);
KSpreadValue func_weibull (valVector args, ValueCalc *calc, FuncExtra *);

typedef QValueList<double> List;

// registers all statistical functions
void KSpreadRegisterStatisticalFunctions()
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

void awSkew (ValueCalc *c, KSpreadValue &res, KSpreadValue val, KSpreadValue p)
{
  KSpreadValue avg = p.element (0, 0);
  KSpreadValue stdev = p.element (1, 0);
  // (val - avg) / stddev
  KSpreadValue d = c->div (c->sub (val, avg), stdev);
  // res += d*d*d
  res = c->add (res, c->mul (d, c->mul (d, d)));
}

void awSumInv (ValueCalc *c, KSpreadValue &res, KSpreadValue val, KSpreadValue)
{
  // res += 1/value
  res = c->add (res, c->div (1.0, val));
}

void awAveDev (ValueCalc *c, KSpreadValue &res, KSpreadValue val,
    KSpreadValue p)
{
  // res += abs (val - p)
  res = c->add (res, c->abs (c->sub (val, p)));
}

void awKurtosis (ValueCalc *c, KSpreadValue &res, KSpreadValue val,
    KSpreadValue p)
{
  KSpreadValue avg = p.element (0, 0);
  KSpreadValue stdev = p.element (1, 0);
  //d = (val - avg ) / stdev
  KSpreadValue d = c->div (c->sub (val, avg), stdev);
  // res += d^4
  res = c->add (res, c->pow (d, 4));
}


KSpreadValue func_skew_est (valVector args, ValueCalc *calc, FuncExtra *)
{
  int number = calc->count (args);
  KSpreadValue avg = calc->avg (args);
  if (number < 3)
    return KSpreadValue::errorVALUE();

  KSpreadValue res = calc->stddev (args, avg);
  if (res.isZero())
    return KSpreadValue::errorVALUE();
  
  KSpreadValue params (2, 1);
  params.setElement (0, 0, avg);
  params.setElement (1, 0, res);
  KSpreadValue tskew;
  calc->arrayWalk (args, tskew, awSkew, params);

  // ((tskew * number) / (number-1)) / (number-2)
  return calc->div (calc->div (calc->mul (tskew, number), number-1), number-2);
}

KSpreadValue func_skew_pop (valVector args, ValueCalc *calc, FuncExtra *)
{
  int number = calc->count (args);
  KSpreadValue avg = calc->avg (args);
  if (number < 1)
    return KSpreadValue::errorVALUE();
  
  KSpreadValue res = calc->stddevP (args, avg);
  if (res.isZero())
    return KSpreadValue::errorVALUE();
  
  KSpreadValue params (2, 1);
  params.setElement (0, 0, avg);
  params.setElement (1, 0, res);
  KSpreadValue tskew;
  calc->arrayWalk (args, tskew, awSkew, params);
  
  // tskew / number
  return calc->div (tskew, number);
}

class ContentSheet : public QMap<double, int> {};

void func_mode_helper (KSpreadValue range, ValueCalc *calc, ContentSheet &sh)
{
  if (!range.isArray())
  {
    double d = calc->conv()->asFloat (range).asFloat();
    sh[d]++;
    return;
  }
  
  for (unsigned int row = 0; row < range.rows(); ++row)
    for (unsigned int col = 0; col < range.columns(); ++col) {
      KSpreadValue v = range.element (col, row);
      if (v.isArray())
        func_mode_helper (v, calc, sh);
      else {
        double d = calc->conv()->asFloat (v).asFloat();
        sh[d]++;
      }
    }
}

KSpreadValue func_mode (valVector args, ValueCalc *calc, FuncExtra *)
{
  // does NOT support anything other than doubles !!!
  ContentSheet sh;
  for (unsigned int i = 0; i < args.count(); ++i)
    func_mode_helper (args[i], calc, sh);
  
  // retrieve value with max.count
  int maxcount = 0;
  double max = 0.0;
  ContentSheet::iterator it;
  for (it = sh.begin(); it != sh.end(); ++it)
    if (it.data() > maxcount) {
      max = it.key();
      maxcount = it.data();
    }
  return KSpreadValue (max);
}

KSpreadValue func_covar_helper (KSpreadValue range1, KSpreadValue range2,
    ValueCalc *calc, KSpreadValue avg1, KSpreadValue avg2)
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
    return KSpreadValue::errorVALUE();
    
  KSpreadValue result = 0.0;
  for (int row = 0; row < rows; ++row)
    for (int col = 0; col < cols; ++col) {
      KSpreadValue v1 = range1.element (col, row);
      KSpreadValue v2 = range2.element (col, row);
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

KSpreadValue func_covar (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue avg1 = calc->avg (args[0]);
  KSpreadValue avg2 = calc->avg (args[1]);
  int number = calc->count (args[0]);
  int number2 = calc->count (args[1]);
  
  if (number2 <= 0 || number2 != number)
    return KSpreadValue::errorVALUE();

  KSpreadValue covar = func_covar_helper (args[0], args[1], calc, avg1, avg2);
  return calc->div (covar, number);
}

KSpreadValue func_correl_pop (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue covar = func_covar (args, calc, 0);
  KSpreadValue stdevp1 = calc->stddevP (args[0]);
  KSpreadValue stdevp2 = calc->stddevP (args[1]);
  
  if (calc->isZero (stdevp1) || calc->isZero (stdevp2))
    return KSpreadValue::errorDIV0();

  // covar / (stdevp1 * stdevp2)
  return calc->div (covar, calc->mul (stdevp1, stdevp2));
}

void func_array_helper (KSpreadValue range, ValueCalc *calc,
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
      KSpreadValue v = range.element (col, row);
      if (v.isArray ())
        func_array_helper (v, calc, array, number);
      else {
        array << calc->conv()->asFloat (v).asFloat();
        ++number;
      }
    }
}

KSpreadValue func_large (valVector args, ValueCalc *calc, FuncExtra *)
{
  // does NOT support anything other than doubles !!!
  int k = calc->conv()->asInteger (args[1]).asInteger();
  if ( k < 1 )
    return false;

  List array;
  int number = 1;
  
  func_array_helper (args[0], calc, array, number);

  if ( k > number )
    return KSpreadValue::errorVALUE();

  qHeapSort (array);
  double d = *array.at (number - k - 1);
  return KSpreadValue (d);
}

KSpreadValue func_small (valVector args, ValueCalc *calc, FuncExtra *)
{
  // does NOT support anything other than doubles !!!
  int k = calc->conv()->asInteger (args[1]).asInteger();
  if ( k < 1 )
    return false;

  List array;
  int number = 1;
  
  func_array_helper (args[0], calc, array, number);

  if ( k > number )
    return KSpreadValue::errorVALUE();

  qHeapSort (array);
  double d = *array.at (k - 1);
  return KSpreadValue (d);
}

KSpreadValue func_geomean (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue count = calc->count (args);
  KSpreadValue prod = calc->product (args, 1.0);
  if (calc->isZero (count))
    return KSpreadValue::errorDIV0();
  return calc->pow (prod, calc->div (1.0, count));
}

KSpreadValue func_harmean (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue count = calc->count (args);
  if (calc->isZero (count))
    return KSpreadValue::errorDIV0();
  KSpreadValue suminv;
  calc->arrayWalk (args, suminv, awSumInv, 0);
  return calc->div (suminv, count);
}

KSpreadValue func_loginv (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue p = args[0];
  KSpreadValue m = args[1];
  KSpreadValue s = args[2];

  if (calc->lower (p, 0) || calc->greater (p, 1))
    return KSpreadValue::errorVALUE();
  
  if (!calc->greater (s, 0))
    return KSpreadValue::errorVALUE();

  KSpreadValue result = 0.0;
  if (calc->equal (p, 1))   //p==1
    result = KSpreadValue::errorVALUE();
  else if (calc->greater (p, 0)) {   //p>0
    KSpreadValue gaussInv = calc->gaussinv (p);
    // exp (gaussInv * s + m)
    result = calc->exp (calc->add (calc->mul (s, gaussInv), m));
  }

  return result;
}

KSpreadValue func_devsq (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue res;
  calc->arrayWalk (args, res, calc->awFunc ("devsq"), calc->avg (args, false));
  return res;
}

KSpreadValue func_devsqa (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue res;
  calc->arrayWalk (args, res, calc->awFunc ("devsqa"), calc->avg (args));
  return res;
}

KSpreadValue func_kurtosis_est (valVector args, ValueCalc *calc, FuncExtra *)
{
  int count = calc->count (args);
  if (count < 4)
    return KSpreadValue::errorVALUE();

  KSpreadValue avg = calc->avg (args);
  KSpreadValue devsq;
  calc->arrayWalk (args, devsq, calc->awFunc ("devsqa"), avg);

  if (devsq.isZero ())
    return KSpreadValue::errorDIV0();

  KSpreadValue params (2, 1);
  params.setElement (0, 0, avg);
  params.setElement (1, 0, devsq);
  KSpreadValue x4;
  calc->arrayWalk (args, x4, awKurtosis, params);

  double den = (double) (count - 2) * (count - 3);
  double nth = (double) count * (count + 1) / ((count - 1) * den);
  double t = 3.0 * (count - 1) * (count - 1) / den;

  // res = x4 * nth - t
  return calc->sub (calc->mul (x4, nth), t);
}

KSpreadValue func_kurtosis_pop (valVector args, ValueCalc *calc, FuncExtra *)
{
  int count = calc->count (args);
  if (count < 4)
    return KSpreadValue::errorVALUE();

  KSpreadValue avg = calc->avg (args);
  KSpreadValue devsq;
  calc->arrayWalk (args, devsq, calc->awFunc ("devsqa"), avg);

  if (devsq.isZero ())
    return KSpreadValue::errorDIV0();

  KSpreadValue params (2, 1);
  params.setElement (0, 0, avg);
  params.setElement (1, 0, devsq);
  KSpreadValue x4;
  calc->arrayWalk (args, x4, awKurtosis, params);
  
  // x4 / count - 3
  return calc->sub (calc->div (x4, count), 3);
}

KSpreadValue func_standardize (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue x = args[0];
  KSpreadValue m = args[1];
  KSpreadValue s = args[2];

  if (!calc->greater (s, 0))  // s must be >0
    return KSpreadValue::errorVALUE();

  // (x - m) / s
  return calc->div (calc->sub (x, m), s);
}

KSpreadValue func_hypgeomdist (valVector args, ValueCalc *calc, FuncExtra *)
{
  int x = calc->conv()->asInteger (args[0]).asInteger();
  int n = calc->conv()->asInteger (args[1]).asInteger();
  int M = calc->conv()->asInteger (args[2]).asInteger();
  int N = calc->conv()->asInteger (args[3]).asInteger();

  if ( x < 0 || n < 0 || M < 0 || N < 0 )
    return KSpreadValue::errorVALUE();

  if ( x > M || n > N )
    return KSpreadValue::errorVALUE();

  KSpreadValue d1 = calc->combin (M, x);
  KSpreadValue d2 = calc->combin (N - M, n - x);
  KSpreadValue d3 = calc->combin (N, n);

  // d1 * d2 / d3
  return calc->div (calc->mul (d1, d2), d3);
}

KSpreadValue func_negbinomdist (valVector args, ValueCalc *calc, FuncExtra *)
{
  int x = calc->conv()->asInteger (args[0]).asInteger();
  int r = calc->conv()->asInteger (args[1]).asInteger();
  KSpreadValue p = args[2];

  if ((x + r - 1) <= 0)
    return KSpreadValue::errorVALUE();
  if (calc->lower (p, 0) || calc->greater (p, 1))
    return KSpreadValue::errorVALUE();

  KSpreadValue d1 = calc->combin (x + r - 1, r - 1);
  // d2 = pow (p, r) * pow (1 - p, x)
  KSpreadValue d2 = calc->mul (calc->pow (p, r),
      calc->pow (calc->sub (1, p), x));

  return calc->mul (d1, d2);
}

// Function: permut
KSpreadValue func_arrang (valVector args, ValueCalc *calc, FuncExtra *)
{ 
  KSpreadValue n = args[0];
  KSpreadValue m = args[1];
  if (calc->lower (n, m))  // problem if n<m
    return KSpreadValue::errorVALUE();

  if (calc->lower (m, 0))  // problem if m<0  (n>=m so that's okay)
    return KSpreadValue::errorVALUE();

  // fact(n) / (fact(n-m)
  return calc->fact (n, calc->sub (n, m));
}

// Function: average
KSpreadValue func_average (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->avg (args, false);
}

// Function: averagea
KSpreadValue func_averagea (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->avg (args);
}

// Function: avedev
KSpreadValue func_avedev (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue result;
  calc->arrayWalk (args, result, awAveDev, calc->avg (args));
  return result;
}

// Function: median
KSpreadValue func_median (valVector args, ValueCalc *calc, FuncExtra *)
{
  // does NOT support anything other than doubles !!!
  List array;
  int number = 1;
  
  for (unsigned int i = 0; i < args.count(); ++i)
    func_array_helper (args[i], calc, array, number);

  qHeapSort (array);
  double d = *array.at (number / 2 + number % 2);
  return KSpreadValue (d);
}

// Function: variance
KSpreadValue func_variance (valVector args, ValueCalc *calc, FuncExtra *)
{
  int count = calc->count (args, false);
  if (count < 2)
    return KSpreadValue::errorVALUE();

  KSpreadValue result = func_devsq (args, calc, 0);
  return calc->div (result, count-1);
}

// Function: vara
KSpreadValue func_variancea (valVector args, ValueCalc *calc, FuncExtra *)
{
  int count = calc->count (args);
  if (count < 2)
    return KSpreadValue::errorVALUE();

  KSpreadValue result = func_devsqa (args, calc, 0);
  return calc->div (result, count-1);
}

// Function: varp
KSpreadValue func_variancep (valVector args, ValueCalc *calc, FuncExtra *)
{
  int count = calc->count (args, false);
  if (count == 0)
    return KSpreadValue::errorVALUE();

  KSpreadValue result = func_devsq (args, calc, 0);
  return calc->div (result, count);
}

// Function: varpa
KSpreadValue func_variancepa (valVector args, ValueCalc *calc, FuncExtra *)
{
  int count = calc->count (args);
  if (count == 0)
    return KSpreadValue::errorVALUE();

  KSpreadValue result = func_devsqa (args, calc, 0);
  return calc->div (result, count);
}

// Function: stddev
KSpreadValue func_stddev (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->stddev (args, false);
}

// Function: stddeva
KSpreadValue func_stddeva (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->stddev (args);
}

// Function: stddevp
KSpreadValue func_stddevp (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->stddevP (args, false);
}

// Function: stddevpa
KSpreadValue func_stddevpa (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->stddevP (args);
}

// Function: combin
KSpreadValue func_combin (valVector args, ValueCalc *calc, FuncExtra *)
{
  return calc->combin (args[0], args[1]);
}

// Function: bino
KSpreadValue func_bino (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue n = args[0];
  KSpreadValue m = args[1];
  KSpreadValue comb = calc->combin (n, m);
  KSpreadValue prob = args[2];
  
  if (calc->lower (prob,0) || calc->greater (prob,1))
    return KSpreadValue::errorVALUE();
    
  // result = comb * pow (prob, m) * pow (1 - prob, n - m)
  KSpreadValue pow1 = calc->pow (prob, m);
  KSpreadValue pow2 = calc->pow (calc->sub (1, prob), calc->sub (n, m));
  return calc->mul (comb, calc->mul (pow1, pow2));
}

// Function: phi
KSpreadValue func_phi (valVector args, ValueCalc *calc, FuncExtra *)
//distribution function for a standard normal distribution
{
  return calc->phi (args[0]);
}

// Function: gauss
KSpreadValue func_gauss (valVector args, ValueCalc *calc, FuncExtra *)
{
  //returns the integral values of the standard normal cumulative distribution
  return calc->gauss (args[0]);
}

// Function: gammadist
KSpreadValue func_gammadist (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue x = args[0];
  KSpreadValue alpha = args[1];
  KSpreadValue beta = args[2];
  int kum = calc->conv()->asInteger (args[3]).asInteger();  // 0 or 1
  
  KSpreadValue result;

  if (calc->lower (x, 0.0) || (!calc->greater (alpha, 0.0)) ||
      (!calc->greater (beta, 0.0)))
    return KSpreadValue::errorVALUE();
  
  if (kum == 0) {  //density
    KSpreadValue G = calc->GetGamma (alpha);
    // result = pow (x, alpha - 1.0) / exp (x / beta) / pow (beta, alpha) / G
    KSpreadValue pow1 = calc->pow (x, calc->sub (alpha, 1.0));
    KSpreadValue pow2 = calc->exp (calc->div (x, beta));
    KSpreadValue pow3 = calc->pow (beta, alpha);
    result = calc->div (calc->div (calc->div (pow1, pow2), pow3), G);
  }
  else
    result = calc->GetGammaDist (x, alpha, beta);

  return KSpreadValue (result);
}

// Function: betadist
KSpreadValue func_betadist (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue x = args[0];
  KSpreadValue alpha = args[1];
  KSpreadValue beta = args[2];
  
  KSpreadValue fA = 0.0;
  KSpreadValue fB = 1.0;
  if (args.count() > 3) fA = args[3];
  if (args.count() == 5) fB = args[4];

  //x < fA || x > fB || fA == fB || alpha <= 0.0 || beta <= 0.0
  if (calc->lower (x, fA) || calc->greater (x, fB) || calc->equal (fA, fB) ||
      (!calc->greater (alpha, 0.0)) || (!calc->greater (beta, 0.0)))
    return KSpreadValue::errorVALUE();
  
  // xx = (x - fA) / (fB - fA)  // scaling
  KSpreadValue xx = calc->div (calc->sub (x, fA), calc->sub (fB, fA));

  return calc->GetBeta (xx, alpha, beta);
}

// Function: fisher
KSpreadValue func_fisher (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the Fisher transformation for x

  // 0.5 * ln ((1.0 + fVal) / (1.0 - fVal))
  KSpreadValue fVal = args[0];
  KSpreadValue num = calc->div (calc->add (fVal, 1.0), calc->sub (1.0, fVal));
  return calc->mul (calc->ln (num), 0.5);
}

// Function: fisherinv
KSpreadValue func_fisherinv (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the inverse of the Fisher transformation for x
  
  KSpreadValue fVal = args[0];
  // (exp (2.0 * fVal) - 1.0) / (exp (2.0 * fVal) + 1.0)
  KSpreadValue ex = calc->exp (calc->mul (fVal, 2.0));
  return calc->div (calc->sub (ex, 1.0), calc->add (ex, 1.0));
}

// Function: normdist
KSpreadValue func_normdist (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the normal cumulative distribution
  KSpreadValue x = args[0];
  KSpreadValue mue = args[1];
  KSpreadValue sigma = args[2];
  KSpreadValue k = args[3];

  if (!calc->greater (sigma, 0.0))
    return KSpreadValue::errorVALUE();

  // (x - mue) / sigma
  KSpreadValue Y = calc->div (calc->sub (x, mue), sigma);
  if (calc->isZero (k))   // density
    return calc->div (calc->phi (Y), sigma);
  else          // distribution
    return calc->add (calc->gauss (Y), 0.5);
}

// Function: lognormdist
KSpreadValue func_lognormdist (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the cumulative lognormal distribution
  KSpreadValue x = args[0];
  KSpreadValue mue = args[1];
  KSpreadValue sigma = args[2];

  if (!calc->greater (sigma, 0.0) || (!calc->greater (x, 0.0)))
    return KSpreadValue::errorVALUE();
  
  // (ln(x) - mue) / sigma
  KSpreadValue Y = calc->div (calc->sub (calc->ln (x), mue), sigma);
  return calc->add (calc->gauss (Y), 0.5);
}

// Function: normsdist
KSpreadValue func_stdnormdist (valVector args, ValueCalc *calc, FuncExtra *)
{
  //returns the cumulative lognormal distribution, mue=0, sigma=1
  return calc->add (calc->gauss (args[0]), 0.5);
}

// Function: expondist
KSpreadValue func_expondist (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the exponential distribution
  KSpreadValue x = args[0];
  KSpreadValue lambda = args[1];
  KSpreadValue kum = args[2];

  KSpreadValue result = 0.0;

  if (!calc->greater (lambda, 0.0))
    return KSpreadValue::errorVALUE();
  
  // ex = exp (-lambda * x)
  KSpreadValue ex = calc->exp (calc->mul (calc->mul (lambda, -1), x));
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
KSpreadValue func_weibull (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the Weibull distribution

  KSpreadValue x = args[0];
  KSpreadValue alpha = args[1];
  KSpreadValue beta = args[2];
  KSpreadValue kum = args[3];

  KSpreadValue result;

  if ((!calc->greater (alpha, 0.0)) || (!calc->greater (beta, 0.0)) ||
      calc->lower (x, 0.0))
    return KSpreadValue::errorVALUE();
  
  // ex = exp (-pow (x / beta, alpha))
  KSpreadValue ex;
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
KSpreadValue func_normsinv (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the inverse of the standard normal cumulative distribution
  
  KSpreadValue x = args[0];
  if (!(calc->greater (x, 0.0) && calc->lower (x, 1.0)))
    return KSpreadValue::errorVALUE();
  
  return calc->gaussinv (x);
}

// Function: norminv
KSpreadValue func_norminv (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the inverse of the normal cumulative distribution
  KSpreadValue x = args[0];
  KSpreadValue mue = args[1];
  KSpreadValue sigma = args[2];

  if (!calc->greater (sigma, 0.0))
    return KSpreadValue::errorVALUE();
  if (!(calc->greater (x, 0.0) && calc->lower (x, 1.0)))
    return KSpreadValue::errorVALUE();

  // gaussinv (x)*sigma + mue
  return calc->add (calc->mul (calc->gaussinv (x), sigma), mue);
}

// Function: gammaln
KSpreadValue func_gammaln (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the natural logarithm of the gamma function
  
  if (calc->greater (args[0], 0.0))
    return calc->GetLogGamma (args[0]);
  return KSpreadValue::errorVALUE();
}

// Function: poisson
KSpreadValue func_poisson (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the Poisson distribution

  KSpreadValue x = args[0];
  KSpreadValue lambda = args[1];
  KSpreadValue kum = args[2];

  // lambda < 0.0 || x < 0.0
  if (calc->lower (lambda, 0.0) || calc->lower (x, 0.0))
    return KSpreadValue::errorVALUE();
  
  KSpreadValue result;

  // ex = exp (-lambda)
  KSpreadValue ex = calc->exp (calc->mul (lambda, -1));

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
      KSpreadValue fFak = 1.0;
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
KSpreadValue func_confidence (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the confidence interval for a population mean
  KSpreadValue alpha = args[0];
  KSpreadValue sigma = args[1];
  KSpreadValue n = args[2];

  // sigma <= 0.0 || alpha <= 0.0 || alpha >= 1.0 || n < 1
  if ((!calc->greater (sigma, 0.0)) || (!calc->greater (alpha, 0.0)) ||
      (!calc->lower (alpha, 1.0)) || calc->lower (n, 1))
    return KSpreadValue::errorVALUE();
    
  // g = gaussinv (1.0 - alpha / 2.0)
  KSpreadValue g = calc->gaussinv (calc->sub (1.0, calc->div (alpha, 2.0)));
  // g * sigma / sqrt (n)
  return calc->div (calc->mul (g, sigma), calc->sqrt (n));
}

// Function: tdist
KSpreadValue func_tdist (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the t-distribution
  
  KSpreadValue T = args[0];
  KSpreadValue fDF = args[1];
  int flag = calc->conv()->asInteger (args[2]).asInteger();

  if (calc->lower (fDF, 1) || calc->lower (T, 0.0) || (flag != 1 && flag != 2))
    return KSpreadValue::errorVALUE();

  // arg = fDF / (fDF + T * T)
  KSpreadValue arg = calc->div (fDF, calc->add (fDF, calc->sqr (T)));
  
  KSpreadValue R;
  R = calc->mul (calc->GetBeta (arg, calc->div (fDF, 2.0), 0.5), 0.5);
  
  if (flag == 1)
    return R;
  return calc->mul (R, 2);   // flag is 2 here
}

// Function: fdist
KSpreadValue func_fdist (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the f-distribution

  KSpreadValue x = args[0];
  KSpreadValue fF1 = args[1];
  KSpreadValue fF2 = args[2];

  // x < 0.0 || fF1 < 1 || fF2 < 1 || fF1 >= 1.0E10 || fF2 >= 1.0E10
  if (calc->lower (x, 0.0) || calc->lower (fF1, 1) || calc->lower (fF2, 1) ||
      (!calc->lower (fF1, 1.0E10)) || (!calc->lower (fF2, 1.0E10)))
    return KSpreadValue::errorVALUE();

  // arg = fF2 / (fF2 + fF1 * x)
  KSpreadValue arg = calc->div (fF2, calc->add (fF2, calc->mul (fF1, x)));
  // alpha = fF2/2.0
  KSpreadValue alpha = calc->div (fF2, 2.0);
  // beta = fF1/2.0
  KSpreadValue beta = calc->div (fF1, 2.0);
  return calc->GetBeta (arg, alpha, beta);
}

// Function: chidist
KSpreadValue func_chidist (valVector args, ValueCalc *calc, FuncExtra *) {
  //returns the chi-distribution
  
  KSpreadValue fChi = args[0];
  KSpreadValue fDF = args[1];

  // fDF < 1 || fDF >= 1.0E5 || fChi < 0.0
  if (calc->lower (fDF, 1) || (!calc->lower (fDF, 1.0E5)) ||
      calc->lower (fChi, 0.0))
    return KSpreadValue::errorVALUE();

  // 1.0 - GetGammaDist (fChi / 2.0, fDF / 2.0, 1.0)
  return calc->sub (1.0, calc->GetGammaDist (calc->div (fChi, 2.0),
      calc->div (fDF, 2.0), 1.0));
}


// two-array-walk functions used in the two-sum functions

void tawSumproduct (ValueCalc *c, KSpreadValue &res, KSpreadValue v1,
    KSpreadValue v2) {
  // res += v1*v2
  res = c->add (res, c->mul (v1, v2));  
}

void tawSumx2py2 (ValueCalc *c, KSpreadValue &res, KSpreadValue v1,
    KSpreadValue v2) {
  // res += sqr(v1)+sqr(v2)
  res = c->add (res, c->add (c->sqr (v1), c->sqr (v2)));
}

void tawSumx2my2 (ValueCalc *c, KSpreadValue &res, KSpreadValue v1,
    KSpreadValue v2) {
  // res += sqr(v1)-sqr(v2)
  res = c->add (res, c->sub (c->sqr (v1), c->sqr (v2)));
}

void tawSumxmy2 (ValueCalc *c, KSpreadValue &res, KSpreadValue v1,
    KSpreadValue v2) {
  // res += sqr(v1-v2)
  res = c->add (res, c->sqr (c->sub (v1, v2)));
  
}

// Function: sumproduct
KSpreadValue func_sumproduct (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue result;
  calc->twoArrayWalk (args[0], args[1], result, tawSumproduct);
  return result;
}

// Function: sumx2py2
KSpreadValue func_sumx2py2 (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue result;
  calc->twoArrayWalk (args[0], args[1], result, tawSumx2py2);
  return result;
}

// Function: sumx2my2
KSpreadValue func_sumx2my2 (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue result;
  calc->twoArrayWalk (args[0], args[1], result, tawSumx2my2);
  return result;
}

// Function: sum2xmy
KSpreadValue func_sumxmy2 (valVector args, ValueCalc *calc, FuncExtra *)
{
  KSpreadValue result;
  calc->twoArrayWalk (args[0], args[1], result, tawSumxmy2);
  return result;
}
