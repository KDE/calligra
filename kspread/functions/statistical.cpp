/* This file is part of the KDE project
   Copyright (C) 1998-2002 The KSpread Team <koffice-devel@kde.org>
   Copyright (C) 2005 Tomas Mecir <mecirt@gmail.com>
   Copyright (C) 2007 Sascha Pfau <MrPeacock@gmail.com>

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

// built-in statistical functions
#include "StatisticalModule.h"

#include "Function.h"
#include "FunctionModuleRegistry.h"
#include "ValueCalc.h"
#include "ValueConverter.h"

#include <Formula.h>

#include <kdebug.h>
#include <KGenericFactory>
#include <KLocale>

// needed for MODE
#include <QList>
#include <QMap>

using namespace KSpread;

// prototypes (sorted!)
Value func_arrang(valVector args, ValueCalc *calc, FuncExtra *);
Value func_average(valVector args, ValueCalc *calc, FuncExtra *);
Value func_averagea(valVector args, ValueCalc *calc, FuncExtra *);
Value func_avedev(valVector args, ValueCalc *calc, FuncExtra *);
Value func_betadist(valVector args, ValueCalc *calc, FuncExtra *);
Value func_betainv(valVector args, ValueCalc *calc, FuncExtra *);
Value func_bino(valVector args, ValueCalc *calc, FuncExtra *);
Value func_binomdist(valVector args, ValueCalc *calc, FuncExtra *);
Value func_chidist(valVector args, ValueCalc *calc, FuncExtra *);
Value func_combin(valVector args, ValueCalc *calc, FuncExtra *);
Value func_combina(valVector args, ValueCalc *calc, FuncExtra *);
Value func_confidence(valVector args, ValueCalc *calc, FuncExtra *);
Value func_correl_pop(valVector args, ValueCalc *calc, FuncExtra *);
Value func_covar(valVector args, ValueCalc *calc, FuncExtra *);
Value func_devsq(valVector args, ValueCalc *calc, FuncExtra *);
Value func_devsqa(valVector args, ValueCalc *calc, FuncExtra *);
Value func_expondist(valVector args, ValueCalc *calc, FuncExtra *);
Value func_fdist(valVector args, ValueCalc *calc, FuncExtra *);
Value func_finv(valVector args, ValueCalc *calc, FuncExtra *);
Value func_fisher(valVector args, ValueCalc *calc, FuncExtra *);
Value func_fisherinv(valVector args, ValueCalc *calc, FuncExtra *);
Value func_frequency(valVector args, ValueCalc *calc, FuncExtra *);
Value func_ftest(valVector args, ValueCalc *calc, FuncExtra *);
Value func_gammadist(valVector args, ValueCalc *calc, FuncExtra *);
Value func_gammainv(valVector args, ValueCalc *calc, FuncExtra *);
Value func_gammaln(valVector args, ValueCalc *calc, FuncExtra *);
Value func_gauss(valVector args, ValueCalc *calc, FuncExtra *);
Value func_growth(valVector args, ValueCalc *calc, FuncExtra *);
Value func_geomean(valVector args, ValueCalc *calc, FuncExtra *);
Value func_harmean(valVector args, ValueCalc *calc, FuncExtra *);
Value func_hypgeomdist(valVector args, ValueCalc *calc, FuncExtra *);
Value func_intercept(valVector args, ValueCalc *calc, FuncExtra *);
Value func_kurtosis_est(valVector args, ValueCalc *calc, FuncExtra *);
Value func_kurtosis_pop(valVector args, ValueCalc *calc, FuncExtra *);
Value func_large(valVector args, ValueCalc *calc, FuncExtra *);
Value func_legacychidist(valVector args, ValueCalc *calc, FuncExtra *);
Value func_legacychiinv(valVector args, ValueCalc *calc, FuncExtra *);
Value func_legacyfdist(valVector args, ValueCalc *calc, FuncExtra *);
Value func_legacyfinv(valVector args, ValueCalc *calc, FuncExtra *);
Value func_loginv(valVector args, ValueCalc *calc, FuncExtra *);
Value func_lognormdist(valVector args, ValueCalc *calc, FuncExtra *);
Value func_median(valVector args, ValueCalc *calc, FuncExtra *);
Value func_mode(valVector args, ValueCalc *calc, FuncExtra *);
Value func_negbinomdist(valVector args, ValueCalc *calc, FuncExtra *);
Value func_normdist(valVector args, ValueCalc *calc, FuncExtra *);
Value func_norminv(valVector args, ValueCalc *calc, FuncExtra *);
Value func_normsinv(valVector args, ValueCalc *calc, FuncExtra *);
Value func_percentile(valVector args, ValueCalc *calc, FuncExtra *);
Value func_phi(valVector args, ValueCalc *calc, FuncExtra *);
Value func_poisson(valVector args, ValueCalc *calc, FuncExtra *);
Value func_rank(valVector args, ValueCalc *calc, FuncExtra *);
Value func_rsq(valVector args, ValueCalc *calc, FuncExtra *);
Value func_quartile(valVector args, ValueCalc *calc, FuncExtra *);
Value func_skew_est(valVector args, ValueCalc *calc, FuncExtra *);
Value func_skew_pop(valVector args, ValueCalc *calc, FuncExtra *);
Value func_slope(valVector args, ValueCalc *calc, FuncExtra *);
Value func_small(valVector args, ValueCalc *calc, FuncExtra *);
Value func_standardize(valVector args, ValueCalc *calc, FuncExtra *);
Value func_stddev(valVector args, ValueCalc *calc, FuncExtra *);
Value func_stddeva(valVector args, ValueCalc *calc, FuncExtra *);
Value func_stddevp(valVector args, ValueCalc *calc, FuncExtra *);
Value func_stddevpa(valVector args, ValueCalc *calc, FuncExtra *);
Value func_stdnormdist(valVector args, ValueCalc *calc, FuncExtra *);
Value func_steyx(valVector args, ValueCalc *calc, FuncExtra *);
Value func_sumproduct(valVector args, ValueCalc *calc, FuncExtra *);
Value func_sumx2py2(valVector args, ValueCalc *calc, FuncExtra *);
Value func_sumx2my2(valVector args, ValueCalc *calc, FuncExtra *);
Value func_sumxmy2(valVector args, ValueCalc *calc, FuncExtra *);
Value func_tdist(valVector args, ValueCalc *calc, FuncExtra *);
Value func_tinv(valVector args, ValueCalc *calc, FuncExtra *);
Value func_trend(valVector args, ValueCalc *calc, FuncExtra *);
Value func_trimmean(valVector args, ValueCalc *calc, FuncExtra *);
Value func_ttest(valVector args, ValueCalc *calc, FuncExtra *);
Value func_variance(valVector args, ValueCalc *calc, FuncExtra *);
Value func_variancea(valVector args, ValueCalc *calc, FuncExtra *);
Value func_variancep(valVector args, ValueCalc *calc, FuncExtra *);
Value func_variancepa(valVector args, ValueCalc *calc, FuncExtra *);
Value func_weibull(valVector args, ValueCalc *calc, FuncExtra *);
Value func_ztest(valVector args, ValueCalc *calc, FuncExtra *);

typedef QList<double> List;


KSPREAD_EXPORT_FUNCTION_MODULE("statistical", StatisticalModule)


StatisticalModule::StatisticalModule(QObject* parent, const QVariantList&)
        : FunctionModule(parent)
{
    Function *f;

    f = new Function("AVEDEV", func_avedev);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("AVERAGE", func_average);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("AVERAGEA", func_averagea);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("BETADIST", func_betadist);
    f->setParamCount(3, 6);
    add(f);
    f = new Function("BETAINV", func_betainv);
    f->setParamCount(3, 5);
    add(f);
    f = new Function("BINO", func_bino);
    f->setParamCount(3);
    add(f);
    f = new Function("BINOMDIST", func_binomdist);
    f->setParamCount(4);
    add(f);
    f = new Function("CHIDIST", func_chidist);
    f->setParamCount(2);
    add(f);
    f = new Function("COMBIN", func_combin);
    f->setParamCount(2);
    add(f);
    f = new Function("COMBINA", func_combina);
    f->setParamCount(2);
    add(f);
    f = new Function("CONFIDENCE", func_confidence);
    f->setParamCount(3);
    add(f);
    f = new Function("CORREL", func_correl_pop);
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("COVAR", func_covar);
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("DEVSQ", func_devsq);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("DEVSQA", func_devsqa);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("EXPONDIST", func_expondist);
    f->setParamCount(3);
    add(f);
    f = new Function("FDIST", func_fdist);
    f->setParamCount(3, 4);
    add(f);
    f = new Function("FINV", func_finv);
    f->setParamCount(3);
    add(f);
    f = new Function("FISHER", func_fisher);
    add(f);
    f = new Function("FISHERINV", func_fisherinv);
    add(f);
    f = new Function("FREQUENCY", func_frequency);
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("FTEST", func_ftest);
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("GAMMADIST", func_gammadist);
    f->setParamCount(4);
    add(f);
    f = new Function("GAMMAINV", func_gammainv);
    f->setParamCount(3);
    add(f);
    f = new Function("GAMMALN", func_gammaln);
    add(f);
    f = new Function("GAUSS", func_gauss);
    add(f);
    f = new Function("GROWTH", func_growth);
    f->setParamCount(1, 4);
    f->setAcceptArray();
    add(f);
    f = new Function("GEOMEAN", func_geomean);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("HARMEAN", func_harmean);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("HYPGEOMDIST", func_hypgeomdist);
    f->setParamCount(4, 5);
    add(f);
    f = new Function("INTERCEPT", func_intercept);
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("INVBINO", func_bino);   // same as BINO, for 1.4 compat
    add(f);
    f = new Function("KURT", func_kurtosis_est);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("KURTP", func_kurtosis_pop);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("LARGE", func_large);
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("LEGACYCHIDIST", func_legacychidist);
    f->setParamCount(2);
    add(f);
    f = new Function("LEGACYCHIINV", func_legacychiinv);
    f->setParamCount(2);
    add(f);
    f = new Function("LEGACYFDIST", func_legacyfdist);
    f->setParamCount(3);
    add(f);
    f = new Function("LEGACYFINV", func_legacyfinv);
    f->setParamCount(3);
    add(f);
    // this is meant to be a copy of the function NORMSDIST.
    // required for OpenFormula compliance.
    f = new Function("LEGACYNORMSDIST", func_stdnormdist);
    add(f);
    // this is meant to be a copy of the function NORMSINV.
    // required for OpenFormula compliance.
    f = new Function("LEGACYNORMSINV", func_normsinv);
    add(f);
    f = new Function("LOGINV", func_loginv);
    f->setParamCount(1, 3);
    add(f);
    f = new Function("LOGNORMDIST", func_lognormdist);
    f->setParamCount(1, 4);
    add(f);
    f = new Function("MEDIAN", func_median);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("MODE", func_mode);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("NEGBINOMDIST", func_negbinomdist);
    f->setParamCount(3);
    add(f);
    f = new Function("NORMDIST", func_normdist);
    f->setParamCount(4);
    add(f);
    f = new Function("NORMINV", func_norminv);
    f->setParamCount(3);
    add(f);
    f = new Function("NORMSDIST", func_stdnormdist);
    add(f);
    f = new Function("NORMSINV", func_normsinv);
    add(f);
    f = new Function("PEARSON", func_correl_pop);
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("PERCENTILE", func_percentile);
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("PERMUT", func_arrang);
    f->setParamCount(2);
    add(f);
    f = new Function("PHI", func_phi);
    add(f);
    f = new Function("POISSON", func_poisson);
    f->setParamCount(3);
    add(f);
    f = new Function("RANK", func_rank);
    f->setParamCount(2, 3);
    f->setAcceptArray();
    add(f);
    f = new Function("RSQ", func_rsq);
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("QUARTILE", func_quartile);
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("SKEW", func_skew_est);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("SKEWP", func_skew_pop);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("SLOPE", func_slope);
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("SMALL", func_small);
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("STANDARDIZE", func_standardize);
    f->setParamCount(3);
    add(f);
    f = new Function("STDEV", func_stddev);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("STDEVA", func_stddeva);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("STDEVP", func_stddevp);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("STDEVPA", func_stddevpa);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("STEYX", func_steyx);
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("SUM2XMY", func_sumxmy2);  // deprecated, use SUMXMY2
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("SUMXMY2", func_sumxmy2);
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("SUMPRODUCT", func_sumproduct);
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("SUMX2PY2", func_sumx2py2);
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("SUMX2MY2", func_sumx2my2);
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("TDIST", func_tdist);
    f->setParamCount(3);
    add(f);
    f = new Function("TINV", func_tinv);
    f->setParamCount(2);
    add(f);
    f = new Function("TREND", func_trend);
    f->setParamCount(1, 4);
    f->setAcceptArray();
    add(f);
    f = new Function("TRIMMEAN", func_trimmean);
    f->setParamCount(2);
    f->setAcceptArray();
    add(f);
    f = new Function("TTEST", func_ttest);
    f->setParamCount(4);
    f->setAcceptArray();
    add(f);
    f = new Function("VARIANCE", func_variance);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("VAR", func_variance);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("VARP", func_variancep);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("VARA", func_variancea);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("VARPA", func_variancepa);
    f->setParamCount(1, -1);
    f->setAcceptArray();
    add(f);
    f = new Function("WEIBULL", func_weibull);
    f->setParamCount(4);
    add(f);
    f = new Function("ZTEST", func_ztest);
    f->setParamCount(2, 3);
    f->setAcceptArray();
    add(f);
}

QString StatisticalModule::descriptionFileName() const
{
    return QString("statistical.xml");
}


///////////////////////////////////////////////////////////
//
// helper functions
//
///////////////////////////////////////////////////////////

//
// helper: array_helper
//
void func_array_helper(Value range, ValueCalc *calc,
                       List &array, int &number)
{
    if (!range.isArray()) {
        array << numToDouble(calc->conv()->toFloat(range));
        ++number;
        return;
    }

    for (unsigned int row = 0; row < range.rows(); ++row)
        for (unsigned int col = 0; col < range.columns(); ++col) {
            Value v = range.element(col, row);
            if (v.isArray())
                func_array_helper(v, calc, array, number);
            else {
                array << numToDouble(calc->conv()->toFloat(v));
                ++number;
            }
        }
}


//
// helper: covar_helper
//
Value func_covar_helper(Value range1, Value range2,
                        ValueCalc *calc, Value avg1, Value avg2)
{
    // two arrays -> cannot use arrayWalk
    if ((!range1.isArray()) && (!range2.isArray()))
        // (v1-E1)*(v2-E2)
        return calc->mul(calc->sub(range1, avg1), calc->sub(range2, avg2));

    int rows = range1.rows();
    int cols = range1.columns();
    int rows2 = range2.rows();
    int cols2 = range2.columns();
    if ((rows != rows2) || (cols != cols2))
        return Value::errorVALUE();

    Value result(0.0);
    for (int row = 0; row < rows; ++row)
        for (int col = 0; col < cols; ++col) {
            Value v1 = range1.element(col, row);
            Value v2 = range2.element(col, row);
            if (v1.isArray() || v2.isArray())
                result = calc->add(result,
                                   func_covar_helper(v1, v2, calc, avg1, avg2));
            else
                // result += (v1-E1)*(v2-E2)
                result = calc->add(result, calc->mul(calc->sub(v1, avg1),
                                                     calc->sub(v2, avg2)));
        }

    return result;
}


//
// helper: GetValue - Returns result of a formula.
//
static double GetValue(const QString& formula, const double x)
{
    Formula f;
    QString expr = formula;
    if (expr[0] != '=')
        expr.prepend('=');

    expr.replace(QString("x"), QString::number(x, 'g', 12));

    //kDebug()<<"expression"<<expr;
    f.setExpression(expr);
    Value result = f.eval();

    return result.asFloat();
}

//
// helper: IterateInverse - Returns the unknown value
//
static Value IterateInverse(const double unknown, const QString& formula, double x0, double x1, bool& convergenceError)
{
    convergenceError = false; // reset error flag
    double eps = 1.0E-7;      // define Epsilon

    kDebug() << "searching for " << unknown << " in interval x0=" << x0 << " x1=" << x1;

    if (x0 > x1)
        kDebug() << "IterateInverse: wrong interval";

    double f0 = unknown - GetValue(formula, x0);
    double f1 = unknown - GetValue(formula, x1);

    kDebug() << " f(" << x0 << ") =" << f0;
    kDebug() << " f(" << x1 << ") =" << f1;

    double xs;
    int i;
    for (i = 0; i < 1000 && f0*f1 > 0.0; i++) {
        if (fabs(f0) <= fabs(f1)) {
            xs = x0;
            x0 += 2.0 * (x0 - x1);
            if (x0 < 0.0)
                x0 = 0.0;
            x1 = xs;
            f1 = f0;
            f0 = unknown - GetValue(formula, x0);
        } else {
            xs = x1;
            x1 += 2.0 * (x1 - x0);
            x0 = xs;
            f0 = f1;
            f1 = unknown - GetValue(formula, x1);
        }
    }

    if (f0 == 0.0)
        return Value(x0);
    if (f1 == 0.0)
        return Value(x1);

    // simple iteration
    //kDebug()<<"simple iteration f0="<<f0<<" f1="<<f1;

    double x00 = x0;
    double x11 = x1;
    double fs = 0.0;
    for (i = 0; i < 100; i++) {
        xs = 0.5 * (x0 + x1);
        if (fabs(f1 - f0) >= eps) {
            fs = unknown - GetValue(formula, xs);
            if (f0*fs <= 0.0) {
                x1 = xs;
                f1 = fs;
            } else {
                x0 = xs;
                f0 = fs;
            }
        } else {
            //add one step of regula falsi to improve precision
            if (x0 != x1) {
                double regxs = (f1 - f0) / (x1 - x0);
                if (regxs != 0.0) {
                    double regx = x1 - f1 / regxs;
                    if (regx >= x00 && regx <= x11) {
                        double regfs = unknown - GetValue(formula, regx);
                        if (fabs(regfs) < fabs(fs))
                            xs = regx;
                    }
                }
            }
            return Value(xs);
        }
        // kDebug()<<"probe no. "<<i<<" : "<<xs<<" error diff ="<<fs;
    }

    // error no convergence - set flag
    convergenceError = true;
    return Value(0.0);
}

//
// helper: mode_helper
//
class ContentSheet : public QMap<double, int> {};

void func_mode_helper(Value range, ValueCalc *calc, ContentSheet &sh)
{
    if (!range.isArray()) {
        double d = numToDouble(calc->conv()->toFloat(range));
        sh[d]++;
        return;
    }

    for (unsigned int row = 0; row < range.rows(); ++row)
        for (unsigned int col = 0; col < range.columns(); ++col) {
            Value v = range.element(col, row);
            if (v.isArray())
                func_mode_helper(v, calc, sh);
            else {
                double d = numToDouble(calc->conv()->toFloat(v));
                sh[d]++;
            }
        }
}

///////////////////////////////////////////////////////////
//
// array-walk functions used in this file
//
///////////////////////////////////////////////////////////

void awSkew(ValueCalc *c, Value &res, Value val, Value p)
{
    Value avg = p.element(0, 0);
    Value stdev = p.element(1, 0);
    // (val - avg) / stddev
    Value d = c->div(c->sub(val, avg), stdev);
    // res += d*d*d
    res = c->add(res, c->mul(d, c->mul(d, d)));
}

void awSumInv(ValueCalc *c, Value &res, Value val, Value)
{
    // res += 1/value
    res = c->add(res, c->div(Value(1.0), val));
}

void awAveDev(ValueCalc *c, Value &res, Value val,
              Value p)
{
    // res += abs (val - p)
    res = c->add(res, c->abs(c->sub(val, p)));
}

void awKurtosis(ValueCalc *c, Value &res, Value val,
                Value p)
{
    Value avg = p.element(0, 0);
    Value stdev = p.element(1, 0);
    //d = (val - avg) / stdev
    Value d = c->div(c->sub(val, avg), stdev);
    // res += d^4
    res = c->add(res, c->pow(d, 4));
}

///////////////////////////////////////////////////////////
//
// two-array-walk functions used in the two-sum functions
//
///////////////////////////////////////////////////////////

void tawSumproduct(ValueCalc *c, Value &res, Value v1,
                   Value v2)
{
    // res += v1*v2
    res = c->add(res, c->mul(v1, v2));
}

void tawSumx2py2(ValueCalc *c, Value &res, Value v1,
                 Value v2)
{
    // res += sqr(v1)+sqr(v2)
    res = c->add(res, c->add(c->sqr(v1), c->sqr(v2)));
}

void tawSumx2my2(ValueCalc *c, Value &res, Value v1,
                 Value v2)
{
    // res += sqr(v1)-sqr(v2)
    res = c->add(res, c->sub(c->sqr(v1), c->sqr(v2)));
}

void tawSumxmy2(ValueCalc *c, Value &res, Value v1,
                Value v2)
{
    // res += sqr(v1-v2)
    res = c->add(res, c->sqr(c->sub(v1, v2)));
}

void tawSumxmy(ValueCalc *c, Value &res, Value v1,
               Value v2)
{
    // res += (v1-v2)
    res = c->add(res, c->sub(v1, v2));
}

///////////////////////////////////////////////////////////
//
// functions used in this file
//
///////////////////////////////////////////////////////////

//
// Function: permut
//
Value func_arrang(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value n = args[0];
    Value m = args[1];
    if (calc->lower(n, m))   // problem if n<m
        return Value::errorVALUE();

    if (calc->lower(m, Value(0)))   // problem if m<0  (n>=m so that's okay)
        return Value::errorVALUE();

    // fact(n) / (fact(n-m)
    return calc->fact(n, calc->sub(n, m));
}

//
// Function: average
//
Value func_average(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->avg(args, false);
}

//
// Function: averagea
//
Value func_averagea(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->avg(args);
}

//
// Function: avedev
//
Value func_avedev(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value result;
    calc->arrayWalk(args, result, awAveDev, calc->avg(args));
    return calc->div(result, calc->count(args));
}

//
// Function: betadist
//
Value func_betadist(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value x     = args[0];
    Value alpha = args[1];
    Value beta  = args[2];

    // default values parameter 4 - 6
    Value fA(0.0);
    Value fB(1.0);
    bool kum = true;

    if (args.count() > 3) fA = args[3];
    if (args.count() > 4) fB = args[4];
    if (args.count() > 5)
        kum = calc->conv()->asInteger(args[5]).asInteger();   // 0 or 1

    // constraints x < fA || x > fB || fA == fB || alpha <= 0.0 || beta <= 0.0
    if (calc->lower(x, fA) || calc->equal(fA, fB) ||
            (!calc->greater(alpha, 0.0)) || !calc->greater(beta, 0.0))
        return Value(0.0);

    // constraints  x > b
    if (calc->greater(x, fB)) {
        if (kum)
            return Value(1.0);
        else
            return Value(0.0);
    }

    // scale = (x - fA) / (fB - fA)  // prescaling
    Value scale = calc->div(calc->sub(x, fA), calc->sub(fB, fA));

    if (kum)
        return calc->GetBeta(scale, alpha, beta);
    else {
        Value res = calc->div(calc->mul(calc->GetGamma(alpha), calc->GetGamma(beta)),
                              calc->GetGamma(calc->add(alpha, beta)));
        Value b1  = calc->pow(scale, calc->sub(alpha, Value(1.0)));
        Value b2  = calc->pow(calc->sub(Value(1.0), scale), calc->sub(beta, Value(1.0)));

        return calc->mul(calc->mul(res, b1), b2);
    }
}

//
// Function: betainv
//
Value func_betainv(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value p     = args[0];
    Value alpha = args[1];
    Value beta  = args[2];

    // default values parameter 4 - 6
    Value fA(0.0);
    Value fB(1.0);

    if (args.count() > 3) fA = args[3];
    if (args.count() > 4) fB = args[4];

    Value result;

    // constraints
    if (calc->lower(alpha, 0.0) || calc->lower(beta, 0.0) ||
            calc->greater(p, 1.0)   || calc->lower(p, 0.0)     || calc->equal(fA, fB))
        return Value::errorVALUE();

    bool convergenceError;

    // create formula string
    QString formula = QString("BETADIST(x;%1;%2)").arg((double)alpha.asFloat()).arg((double)beta.asFloat());

    result = IterateInverse(p.asFloat(), formula , 0.0, 1.0, convergenceError);

    if (convergenceError)
        return Value::errorVALUE();

    result = calc->add(fA, calc->mul(result, calc->sub(fB, fA)));

    return result;
}

//
// Function: bino
//
// kspread version
//
Value func_bino(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value n = args[0];
    Value m = args[1];
    Value comb = calc->combin(n, m);
    Value prob = args[2];

    if (calc->lower(prob, Value(0)) || calc->greater(prob, Value(1)))
        return Value::errorVALUE();

    // result = comb * pow (prob, m) * pow (1 - prob, n - m)
    Value pow1 = calc->pow(prob, m);
    Value pow2 = calc->pow(calc->sub(1, prob), calc->sub(n, m));
    return calc->mul(comb, calc->mul(pow1, pow2));
}

//
// Function: binomdist
//
// binomdist ( x, n, p, bool cumulative )
//
Value func_binomdist(valVector args, ValueCalc *calc, FuncExtra *)
{
    // TODO using approxfloor
    double x = calc->conv()->asFloat(args[0]).asFloat();
    double n = calc->conv()->asFloat(args[1]).asFloat();
    double p = calc->conv()->asFloat(args[2]).asFloat();
    bool kum = calc->conv()->asInteger(args[3]).asInteger();

    kDebug() << "x= " << x << " n= " << n << " p= " << p;

    // check constraints
    if (n < 0.0 || x < 0.0 || x > n || p < 0.0 || p > 1.0)
        return Value::errorVALUE();

    double res;
    double factor;
    double q;

    if (kum) {
        // calculation of binom-distribution
        kDebug() << "calc distribution";
        if (n == x)
            res = 1.0;
        else {
            q = 1.0 - p;
            factor = pow(q, n);
            if (factor == 0.0) {
                factor = pow(p, n);
                if (factor == 0.0)
                    return Value::errorNA(); //SetNoValue();
                else {
                    res = 1.0 - factor;
                    unsigned long max = (unsigned long)(n - x) - 1;
                    for (unsigned long i = 0; i < max && factor > 0.0; i++) {
                        factor *= (n - i) / (i + 1) * q / p;
                        res -= factor;
                    }
                    if (res < 0.0)
                        res = 0.0;
                }
            } else {
                res = factor;
                unsigned long max = (unsigned long) x;
                for (unsigned long i = 0; i < max && factor > 0.0; i++) {
                    factor *= (n - i) / (i + 1) * p / q;
                    res += factor;
                }
            }
        }
    } else { // density
        kDebug() << "calc density";
        q = 1.0 - p;
        factor = pow(q, n);
        if (factor == 0.0) {
            factor = pow(p, n);
            if (factor == 0.0)
                return Value::errorNA(); //SetNoValue();
            else {
                unsigned long max = (unsigned long)(n - x);
                for (unsigned long i = 0; i < max && factor > 0.0; i++)
                    factor *= (n - i) / (i + 1) * q / p;
                res = factor;
            }
        } else {
            unsigned long max = (unsigned long) x;
            for (unsigned long i = 0; i < max && factor > 0.0; i++)
                factor *= (n - i) / (i + 1) * p / q;
            res = factor;
        }
    }
    return Value(res);
}

//
// Function: chidist
//
// returns the chi-distribution
//
Value func_chidist(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value fChi = args[0];
    Value fDF = args[1];

    // fDF < 1 || fDF >= 1.0E5
    if (calc->lower(fDF, Value(1)) || (!calc->lower(fDF, Value(1.0E5))))
        return Value::errorVALUE();
    // fChi <= 0.0
    if (calc->lower(fChi, Value(0.0)) || (!calc->greater(fChi, Value(0.0))))
        return Value(1.0);

    // 1.0 - GetGammaDist (fChi / 2.0, fDF / 2.0, 1.0)
    return calc->sub(Value(1.0), calc->GetGammaDist(calc->div(fChi, 2.0),
                     calc->div(fDF, 2.0), Value(1.0)));
}

//
// Function: combin
//
Value func_combin(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (calc->lower(args[1], Value(0.0)) || calc->lower(args[1], Value(0.0)) || calc->greater(args[1], args[0]))
        return Value::errorNUM();

    return calc->combin(args[0], args[1]);
}

//
// Function: combina
//
Value func_combina(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (calc->lower(args[1], Value(0.0)) || calc->lower(args[1], Value(0.0)) || calc->greater(args[1], args[0]))
        return Value::errorNUM();

    return calc->combin(calc->sub(calc->add(args[0], args[1]), Value(1.0)), args[1]);
}

//
// Function: confidence
//
// returns the confidence interval for a population mean
//
Value func_confidence(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value alpha = args[0];
    Value sigma = args[1];
    Value n = args[2];

    // sigma <= 0.0 || alpha <= 0.0 || alpha >= 1.0 || n < 1
    if ((!calc->greater(sigma, Value(0.0))) || (!calc->greater(alpha, Value(0.0))) ||
            (!calc->lower(alpha, Value(1.0))) || calc->lower(n, Value(1)))
        return Value::errorVALUE();

    // g = gaussinv (1.0 - alpha / 2.0)
    Value g = calc->gaussinv(calc->sub(Value(1.0), calc->div(alpha, 2.0)));
    // g * sigma / sqrt (n)
    return calc->div(calc->mul(g, sigma), calc->sqrt(n));
}

//
// function: correl_pop
//
Value func_correl_pop(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value covar = func_covar(args, calc, 0);
    Value stdevp1 = calc->stddevP(args[0]);
    Value stdevp2 = calc->stddevP(args[1]);

    if (calc->isZero(stdevp1) || calc->isZero(stdevp2))
        return Value::errorDIV0();

    // covar / (stdevp1 * stdevp2)
    return calc->div(covar, calc->mul(stdevp1, stdevp2));
}

//
// function: covar
//
Value func_covar(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value avg1 = calc->avg(args[0]);
    Value avg2 = calc->avg(args[1]);
    int number = calc->count(args[0]);
    int number2 = calc->count(args[1]);

    if (number2 <= 0 || number2 != number)
        return Value::errorVALUE();

    Value covar = func_covar_helper(args[0], args[1], calc, avg1, avg2);
    return calc->div(covar, number);
}

//
// function: devsq
//
Value func_devsq(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value res;
    calc->arrayWalk(args, res, calc->awFunc("devsq"), calc->avg(args, false));
    return res;
}

//
// function: devsqa
//
Value func_devsqa(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value res;
    calc->arrayWalk(args, res, calc->awFunc("devsqa"), calc->avg(args));
    return res;
}

//
// Function: expondist
//
// returns the exponential distribution
//
Value func_expondist(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value x = args[0];
    Value lambda = args[1];
    Value kum = args[2];

    Value result(0.0);

    if (!calc->greater(lambda, 0.0))
        return Value::errorVALUE();

    // ex = exp (-lambda * x)
    Value ex = calc->exp(calc->mul(calc->mul(lambda, -1), x));
    if (calc->isZero(kum)) {   //density
        if (!calc->lower(x, 0.0))
            // lambda * ex
            result = calc->mul(lambda, ex);
    } else { //distribution
        if (calc->greater(x, 0.0))
            // 1.0 - ex
            result = calc->sub(1.0, ex);
    }
    return result;
}

//
// Function: fdist
//
// returns the f-distribution
//
Value func_fdist(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value x = args[0];
    Value fF1 = args[1];
    Value fF2 = args[2];

    bool kum = true;
    if (args.count() > 3)
        kum = calc->conv()->asInteger(args[3]).asInteger();

    // check constraints
    // x < 0.0 || fF1 < 1 || fF2 < 1 || fF1 >= 1.0E10 || fF2 >= 1.0E10
    if (calc->lower(x, Value(0.0)) || calc->lower(fF1, Value(1)) || calc->lower(fF2, Value(1)) ||
            (!calc->lower(fF1, Value(1.0E10))) || (!calc->lower(fF2, Value(1.0E10))))
        return Value::errorVALUE();

    if (kum) {
        // arg = fF2 / (fF2 + fF1 * x)
        Value arg = calc->div(fF2, calc->add(fF2, calc->mul(fF1, x)));
        // alpha = fF2/2.0
        Value alpha = calc->div(fF2, 2.0);
        // beta = fF1/2.0
        Value beta = calc->div(fF1, 2.0);
        return calc->sub(Value(1), calc->GetBeta(arg, alpha, beta));
    } else {
        // non-cumulative calculation
        if (calc->lower(x, Value(0.0)))
            return Value(0);
        else {
            double res = 0.0;
            double f1 = calc->conv()->asFloat(args[1]).asFloat();
            double f2 = calc->conv()->asFloat(args[2]).asFloat();
            double xx = calc->conv()->asFloat(args[0]).asFloat();


            double tmp1 = (f1 / 2) * log(f1) + (f2 / 2) * log(f2);
            double tmp2 = calc->GetLogGamma(Value((f1 + f2) / 2)).asFloat();
            double tmp3 = calc->GetLogGamma(Value(f1 / 2)).asFloat();
            double tmp4 = calc->GetLogGamma(Value(f2 / 2)).asFloat();

            res = exp(tmp1 + tmp2 - tmp3 - tmp4) * pow(xx, (f1 / 2) - 1) * pow(f2 + f1 * xx, (-f1 / 2) - (f2 / 2));
            return Value(res);
        }
    }
}

//
// Function: finv
//
// returns the inverse f-distribution
//
Value func_finv(valVector args, ValueCalc *, FuncExtra *)
{
    Value p  = args[0];
    Value f1 = args[1];
    Value f2 = args[2];

    Value result;

    //TODO constraints
//   if (  calc->lower(DF, 1.0)  || calc->greater(DF, 1.0E5) ||
//         calc->greater(p, 1.0) || calc->lower(p,0.0)           )
//     return Value::errorVALUE();

    bool convergenceError;

    // create formula string
    QString formula = QString("FDIST(x;%1;%2;1)").arg((double)f1.asFloat()).arg((double)f2.asFloat());

    result = IterateInverse(p.asFloat(), formula , f1.asFloat() * 0.5, f1.asFloat(), convergenceError);

    if (convergenceError)
        return Value::errorVALUE();

    return result;
}

//
// Function: fisher
//
// returns the Fisher transformation for x
//
Value func_fisher(valVector args, ValueCalc *calc, FuncExtra *)
{
    // 0.5 * ln ((1.0 + fVal) / (1.0 - fVal))
    Value fVal = args[0];
    Value num = calc->div(calc->add(fVal, 1.0), calc->sub(1.0, fVal));
    return calc->mul(calc->ln(num), 0.5);
}

//
// Function: fisherinv
//
// returns the inverse of the Fisher transformation for x
//
Value func_fisherinv(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value fVal = args[0];
    // (exp (2.0 * fVal) - 1.0) / (exp (2.0 * fVal) + 1.0)
    Value ex = calc->exp(calc->mul(fVal, 2.0));
    return calc->div(calc->sub(ex, 1.0), calc->add(ex, 1.0));
}

//
// Function: FREQUENCY
//
Value func_frequency(valVector args, ValueCalc*, FuncExtra*)
{
    const Value bins = args[1];
    if (bins.columns() != 1)
        return Value::errorVALUE();

    // create a data vector
    QVector<double> data;
    for (uint v = 0; v < args[0].count(); ++v) {
        if (args[0].element(v).isNumber())
            data.append(numToDouble(args[0].element(v).asFloat()));
    }

    // no intervals given?
    if (bins.isEmpty())
        return Value(data.count());

    // sort the data
    qStableSort(data);

    Value result(Value::Array);
    QVector<double>::ConstIterator begin = data.constBegin();
    QVector<double>::ConstIterator it = data.constBegin();
    for (uint v = 0; v < bins.count(); ++v) {
        if (!bins.element(v).isNumber())
            continue;
        it = qUpperBound(begin, data.constEnd(), bins.element(v).asFloat());
        // exact match?
        if (*it == numToDouble(bins.element(v).asFloat()))
            ++it;
        // add the number of values in this interval to the result
        result.setElement(0, v, Value(static_cast<qint64>(it - begin)));
        begin = it;
    }
    // the remaining values
    result.setElement(0, bins.count(), Value(static_cast<qint64>(data.constEnd() - begin)));

    return result;
}

//
// Function: FTEST
//
// TODO - check if parameters are arrays
Value func_ftest(valVector args, ValueCalc *calc, FuncExtra*)
{
    const Value matrixA = args[0];
    const Value matrixB = args[1];

    double val    = 0.0; // stores current array value
    double countA = 0.0; //
    double countB = 0.0; //
    double sumA   = 0.0, sumSqrA = 0.0;
    double sumB   = 0.0, sumSqrB = 0.0;

    // matrixA
    for (uint v = 0; v < matrixA.count(); ++v) {
        if (matrixA.element(v).isNumber()) {
            countA++;
            val = numToDouble(matrixA.element(v).asFloat());
            sumA    += val;       // add sum
            sumSqrA += val * val; // add square
        }
    }

    // matrixB
    for (uint v = 0; v < matrixB.count(); ++v) {
        if (matrixB.element(v).isNumber()) {
            countB++;
            val = numToDouble(matrixB.element(v).asFloat());
            sumB    += val;       // add sum
            sumSqrB += val * val; // add square
        }
    }

    // check constraints
    if (countA < 2 || countB < 2)
        return Value::errorNA();

    double sA = (sumSqrA - sumA * sumA / countA) / (countA - 1.0);
    double sB = (sumSqrB - sumB * sumB / countB) / (countB - 1.0);

    if (sA == 0.0 || sB == 0.0)
        return Value::errorNA();

    double x, r1, r2;

    if (sA > sB) {
        x  = sA / sB;
        r1 = countA - 1.0;
        r2 = countB - 1.0;
    } else {
        x  = sB / sA;
        r1 = countB - 1.0;
        r2 = countA - 1.0;
    }

    valVector param;
    param.append(Value(x));
    param.append(Value(r1));
    param.append(Value(r2));

    return calc->mul(Value(2.0), func_legacyfdist(param, calc, 0));
}

//
// Function: gammadist
//
Value func_gammadist(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value x     = args[0];
    Value alpha = args[1];
    Value beta  = args[2];
    int kum = calc->conv()->asInteger(args[3]).asInteger();   // 0 or 1

    Value result;

    if (calc->lower(x, 0.0) || (!calc->greater(alpha, 0.0)) ||
            (!calc->greater(beta, 0.0)))
        return Value::errorVALUE();

    if (kum == 0) {  //density
        Value G = calc->GetGamma(alpha);
        // result = pow (x, alpha - 1.0) / exp (x / beta) / pow (beta, alpha) / G
        Value pow1 = calc->pow(x, calc->sub(alpha, 1.0));
        Value pow2 = calc->exp(calc->div(x, beta));
        Value pow3 = calc->pow(beta, alpha);
        result = calc->div(calc->div(calc->div(pow1, pow2), pow3), G);
    } else
        result = calc->GetGammaDist(x, alpha, beta);

    return Value(result);
}

//
// Function: gammainv
//
Value func_gammainv(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value p     = args[0];
    Value alpha = args[1];
    Value beta  = args[2];

    Value result;

    // constraints
    if (calc->lower(alpha, 0.0) || calc->lower(beta, 0.0) ||
            calc->lower(p, 0.0)     || !calc->lower(p, 1.0))
        return Value::errorVALUE();

    bool convergenceError;
    Value start = calc->mul(alpha, beta);

    // create formula string
    QString formula = QString("GAMMADIST(x;%1;%2;1)").arg((double)alpha.asFloat()).arg((double)beta.asFloat());

    result = IterateInverse(p.asFloat(), formula , start.asFloat() * 0.5, start.asFloat(), convergenceError);

    if (convergenceError)
        return Value::errorVALUE();

    return result;
}

//
// Function: gammaln
//
// returns the natural logarithm of the gamma function
//
Value func_gammaln(valVector args, ValueCalc *calc, FuncExtra *)
{
    if (calc->greater(args[0], Value(0.0)))
        return calc->GetLogGamma(args[0]);
    return Value::errorVALUE();
}

//
// Function: gauss
//
Value func_gauss(valVector args, ValueCalc *calc, FuncExtra *)
{
    //returns the integral values of the standard normal cumulative distribution
    return calc->gauss(args[0]);
}

//
// Function: growth
//
// GROWTH ( knownY [; [knownX] [; [newX] [; allowOsset = TRUE() ] ] ] )
//
Value func_growth(valVector args, ValueCalc *calc, FuncExtra *)
{
    kDebug() << "GROWTH"; // Debug
    Value known_Y = args[0];

    // default
    bool withOffset = true;

    if (args.count() > 3)
        withOffset = calc->conv()->asInteger(args[3]).asInteger();

    // check constraints
    if (known_Y.isEmpty()) {
        kDebug() << "known_Y is empty";
        return Value::errorNA();
    }

    // check if array known_Y contains only numbers
    for (uint i = 0; i < known_Y.count(); i++) {
        if (!known_Y.element(i).isNumber()) {
            kDebug() << "count_Y (" << i << ") is non Value";
            return Value::errorNA();
        }
    }

    uint cols_X, cols_Y; // columns in X and Y-Matrix
    uint rows_X, rows_Y; // rows     in X and Y-Matrix
    int M, N;

    // stores count of elements in array
    // int count_Y = 0;
    int count_X = 0;

    //
    int nCase = 0;

    // get size Y-Matrix
    rows_Y = known_Y.rows();
    cols_Y = known_Y.columns();
    kDebug() << "Y has " << rows_Y << " rows";
    kDebug() << "Y has " << cols_Y << " cols";

    // convert all Value in known_Y into log
    for (uint r = 0; r < rows_Y; r++)
        for (uint c = 0; c < cols_Y; c++) {
            kDebug() << "col " << c << " row " << r << " log of Y(" << known_Y.element(c, r) << ") Value=" << calc->log(known_Y.element(c, r)); // Debug
            known_Y.setElement(c, r, calc->ln(known_Y.element(c, r)));
        }

    Value known_X(Value::Array);

    //
    // knownX is given ...
    //
    if (args.count() > 1) {
        //
        // get X-Matrix and print size
        //
        known_X = args[1];

        rows_X = known_Y.rows();
        cols_X = known_X.columns();
        kDebug() << "X has " << rows_X << " rows";
        kDebug() << "X has " << cols_X << " cols";

        //
        // check if array known_X contains only numbers
        //
        for (uint i = 0; i < known_X.count(); i++) {
            if (!known_X.element(i).isNumber()) {
                kDebug() << "count_X (" << i << ") is non Value";
                return Value::errorNA();
            }
        }

        //
        // check for simple regression
        //
        if (cols_X == cols_Y && rows_X == rows_Y)
            nCase = 1;

        else if (cols_Y != 1 && rows_Y != 1) {
            kDebug() << "Y-Matrix only has one row or column";
            return Value::errorNA(); // TODO which errortype VALUE?
        } else if (cols_Y == 1) {
            //
            // row alignment
            //
            if (rows_X != rows_Y) {
                kDebug() << "--> row aligned";
                kDebug() << "row sizes not equal";
                return Value::errorNA();
            } else {
                kDebug() << "--> row aligned";
                nCase = 2; // row alignment
                N = rows_Y;
                M = cols_X;
            }
        }

        //
        // only column alignment left
        //
        else if (cols_X != cols_Y) {
            kDebug() << "--> col aligned";
            kDebug() << "col sizes not equal";
            return Value::errorNA();
        } else {
            kDebug() << "--> col aligned";
            nCase = 3; // column alignment
            N = cols_Y;
            M = rows_X;
        }
    } else
        //
        // if known_X is empty it has to be set to
        // the sequence 1,2,3... n (n number of counts knownY) in one row.
        //
    {
        kDebug() << "fill X-Matrix with 0,1,2,3 .. sequence";
        const int known_Y_count = known_Y.count();
        for (int i = 0; i < known_Y_count; i++)
            known_X.setElement(i, 0, Value(i));

        cols_X = cols_Y;
        rows_X = rows_Y;

        // simple regression
        nCase = 1;
    }

    Value newX(Value::Array);
    uint cols_newX, rows_newX;
    int count_newX;

    if (args.count() < 3) {
        kDebug() << "no newX-Matrix --> copy X-Matrix";
        cols_newX = cols_X;
        rows_newX = rows_X;
        count_newX = count_X;
        newX = known_X;
    } else {
        newX = args[2];
        // get dimensions
        cols_newX = newX.columns();
        rows_newX = newX.rows();

        if ((nCase == 2 && cols_X != cols_newX) || (nCase == 3 && rows_X != rows_newX)) {
            kDebug() << "newX does not fit...";
            return Value::errorNA();
        }

        // check if array newX contains only numbers
        for (uint i = 0; i < newX.count(); i++) {
            if (!newX.element(i).isNumber()) {
                kDebug() << "newX (" << i << ") is non Value";
                return Value::errorNA();
            }
        }
    }

    kDebug() << "known_X = " << known_X;
    kDebug() << "newX = " << newX;
    kDebug() << "newX has " << rows_newX << " rows";
    kDebug() << "newX has " << cols_newX << " cols";

    // create the resulting matrix
    Value res(Value::Array);

    //
    // simple regression
    //
    if (nCase == 1) {
        kDebug() << "Simple regression detected"; // Debug

        double count   = 0.0;
        double sumX    = 0.0;
        double sumSqrX = 0.0;
        double sumY    = 0.0;
        double sumSqrY = 0.0;
        double sumXY   = 0.0;
        double valX, valY;

        //
        // Gehe �ber Matrix Reihen/Spaltenweise
        //
        for (uint c = 0; c < cols_Y; c++) {
            for (uint r = 0; r < rows_Y; r++) {
                valX = known_X.element(c, r).asFloat();
                valY = known_Y.element(c, r).asFloat();
                sumX    += valX;
                sumSqrX += valX * valX;
                sumY    += valY;
                sumSqrY += valY * valY;
                sumXY   += valX * valY;
                count++;
            }
        }

        if (count < 1.0) {
            kDebug() << "count less than 1.0";
            return Value::errorNA();
        } else {
            double f1 = count * sumXY - sumX * sumY;
            double X  = count * sumSqrX - sumX * sumX;
            double b, m;

            if (withOffset) {
                // with offset
                b = sumY / count - f1 / X * sumX / count;
                m = f1 / X;
            } else {
                // without offset
                b = 0.0;
                m = sumXY / sumSqrX;
            }

            //
            // Fill result matrix
            //
            for (uint c = 0; c < cols_newX; c++) {
                for (uint r = 0; r < rows_newX; r++) {
                    double result = 0.0;
                    result = exp(newX.element(c, r).asFloat() * m + b);
                    kDebug() << "res(" << c << "," << r << ") = " << result;
                    res.setElement(c, r, Value(result));
                }
            }
        }
        kDebug() << res;
    } else {
        if (nCase == 2) {
            kDebug() << "column alignment";
        } else {
            kDebug() << "row alignment";
        }
    }

    return (res);   // return array
}

//
// function: geomean
//
Value func_geomean(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value count = Value(calc->count(args));
    Value prod = calc->product(args, Value(1.0));
    if (calc->isZero(count))
        return Value::errorDIV0();
    return calc->pow(prod, calc->div(Value(1.0), count));
}

//
// function: harmean
//
Value func_harmean(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value count(calc->count(args));
    if (calc->isZero(count))
        return Value::errorDIV0();
    Value suminv;
    calc->arrayWalk(args, suminv, awSumInv, Value(0));
    return calc->div(count, suminv);
}

//
// function: hypgeomdist
//
Value func_hypgeomdist(valVector args, ValueCalc *calc, FuncExtra *)
{
    int x = calc->conv()->asInteger(args[0]).asInteger();
    int n = calc->conv()->asInteger(args[1]).asInteger();
    int M = calc->conv()->asInteger(args[2]).asInteger();
    int N = calc->conv()->asInteger(args[3]).asInteger();

    double res = 0.0;

    bool kum = false;
    if (args.count() > 4)
        kum = calc->conv()->asInteger(args[4]).asInteger();

    if (x < 0 || n < 0 || M < 0 || N < 0)
        return Value::errorVALUE();

    if (x > M || n > N)
        return Value::errorVALUE();

    if (kum) {
        for (int i = 0; i < x + 1; i++) {
            Value d1 = calc->combin(M, i);
            Value d2 = calc->combin(N - M, n - i);
            Value d3 = calc->combin(N, n);

            // d1 * d2 / d3
            res += calc->div(calc->mul(d1, d2), d3).asFloat();
        }
    } else {
        Value d1 = calc->combin(M, x);
        Value d2 = calc->combin(N - M, n - x);
        Value d3 = calc->combin(N, n);

        // d1 * d2 / d3
        res =  calc->div(calc->mul(d1, d2), d3).asFloat();
    }

    return Value(res);
}

//
// Function: INTERCEPT
//
Value func_intercept(valVector args, ValueCalc* calc, FuncExtra*)
{
    int numberY = calc->count(args[0]);
    int numberX = calc->count(args[1]);

    if (numberY < 1 || numberX < 1 || numberY != numberX)
        return Value::errorVALUE();

    Value denominator;
    Value avgY = calc->avg(args[0]);
    Value avgX = calc->avg(args[1]);
    Value nominator = func_covar_helper(args[0], args[1], calc, avgY, avgX);
    calc->arrayWalk(args[1], denominator, calc->awFunc("devsq"), avgX);
    // result = Ey - SLOPE * Ex
    return calc->sub(avgY, calc->mul(calc->div(nominator, denominator), avgX));
}

//
// function: kurtosis_est
//
Value func_kurtosis_est(valVector args, ValueCalc *calc, FuncExtra *)
{
    int count = calc->count(args);
    if (count < 4)
        return Value::errorVALUE();

    Value avg = calc->avg(args);

    Value stdev = calc->stddev(args, false);
    if (stdev.isZero())
        return Value::errorDIV0();

    Value params(Value::Array);
    params.setElement(0, 0, avg);
    params.setElement(1, 0, stdev);
    Value x4;
    calc->arrayWalk(args, x4, awKurtosis, params);

    // res = ( n*(n+1)*x4 - 3*(n-1)^3) / ( (n-3)*(n-2)*(n-1) )
    int v1 = count * (count + 1);
    int v2 = 3 * (count - 1) * (count - 1) * (count - 1);
    int v3 = (count - 3) * (count - 2) * (count - 1);
    return calc->div(calc->sub(calc->mul(x4, v1), v2), v3);
}

//
// function: kurtosis_pop
//
Value func_kurtosis_pop(valVector args, ValueCalc *calc, FuncExtra *)
{
    int count = calc->count(args);
    if (count < 4)
        return Value::errorVALUE();

    Value avg = calc->avg(args);
    Value stdev = calc->stddev(args, false);
    if (stdev.isZero())
        return Value::errorDIV0();

    Value params(Value::Array);
    params.setElement(0, 0, avg);
    params.setElement(1, 0, stdev);
    Value x4;
    calc->arrayWalk(args, x4, awKurtosis, params);

    // x4 / count - 3
    return calc->sub(calc->div(x4, count), 3);
}

//
// function: large
//
Value func_large(valVector args, ValueCalc *calc, FuncExtra *)
{
    // does NOT support anything other than doubles !!!
    int k = calc->conv()->asInteger(args[1]).asInteger();
    if (k < 1)
        return Value::errorVALUE();

    List array;
    int number = 1;

    func_array_helper(args[0], calc, array, number);

    if (k >= number || number - k - 1 >= array.count())
        return Value::errorVALUE();

    qSort(array);
    double d = array.at(number - k - 1);
    return Value(d);
}

//
// Function: legacaychidist
//
// returns the chi-distribution
//
Value func_legacychidist(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value fChi = args[0];
    Value fDF = args[1];

    // fDF < 1 || fDF >= 1.0E5
    if (calc->lower(fDF, Value(1)) || (!calc->lower(fDF, Value(1.0E5))))
        return Value::errorVALUE();
    // fChi <= 0.0
    if (calc->lower(fChi, Value(0.0)) || (!calc->greater(fChi, Value(0.0))))
        return Value(1.0);

    // 1.0 - GetGammaDist (fChi / 2.0, fDF / 2.0, 1.0)
    return calc->sub(Value(1.0), calc->GetGammaDist(calc->div(fChi, 2.0),
                     calc->div(fDF, 2.0), Value(1.0)));
}

//
// Function: legacaychiinv
//
// returns the inverse chi-distribution
//
Value func_legacychiinv(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value p  = args[0];
    Value DF = args[1];

    Value result;

    // constraints
    if (calc->lower(DF, 1.0)  || calc->greater(DF, 1.0E5) ||
            calc->greater(p, 1.0) || calc->lower(p, 0.0))
        return Value::errorVALUE();

    bool convergenceError;

    // create formula string
    QString formula = QString("LEGACYCHIDIST(x;%1)").arg((double)DF.asFloat());

    result = IterateInverse(p.asFloat(), formula , DF.asFloat() * 0.5, DF.asFloat(), convergenceError);

    if (convergenceError)
        return Value::errorVALUE();

    return result;
}

//
// Function: legacy.fdist
//
// returns the f-distribution
//
Value func_legacyfdist(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value x = args[0];
    Value fF1 = args[1];
    Value fF2 = args[2];

    // x < 0.0 || fF1 < 1 || fF2 < 1 || fF1 >= 1.0E10 || fF2 >= 1.0E10
    if (calc->lower(x, Value(0.0)) || calc->lower(fF1, Value(1)) || calc->lower(fF2, Value(1)) ||
            (!calc->lower(fF1, Value(1.0E10))) || (!calc->lower(fF2, Value(1.0E10))))
        return Value::errorVALUE();

    // arg = fF2 / (fF2 + fF1 * x)
    Value arg = calc->div(fF2, calc->add(fF2, calc->mul(fF1, x)));
    // alpha = fF2/2.0
    Value alpha = calc->div(fF2, 2.0);
    // beta = fF1/2.0
    Value beta = calc->div(fF1, 2.0);
    return calc->GetBeta(arg, alpha, beta);
}

//
// Function: legacyfinv
//
// returns the inverse legacy f-distribution
//
Value func_legacyfinv(valVector args, ValueCalc *, FuncExtra *)
{
    Value p  = args[0];
    Value f1 = args[1];
    Value f2 = args[2];

    Value result;

    //TODO constraints
//   if (  calc->lower(DF, 1.0)  || calc->greater(DF, 1.0E5) ||
//         calc->greater(p, 1.0) || calc->lower(p,0.0)           )
//     return Value::errorVALUE();

    bool convergenceError;

    // create formula string
    QString formula = QString("LEGACYFDIST(x;%1;%2)").arg((double)f1.asFloat()).arg((double)f2.asFloat());

    result = IterateInverse(p.asFloat(), formula , f1.asFloat() * 0.5, f1.asFloat(), convergenceError);

    if (convergenceError)
        return Value::errorVALUE();

    return result;
}

//
// function: loginv
//
Value func_loginv(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value p = args[0];

    // defaults
    Value m = Value(0.0);
    Value s = Value(1.0);

    if (args.count() > 1)
        m = args[1];
    if (args.count() > 2)
        s = args[2];

    if (calc->lower(p, Value(0)) || calc->greater(p, Value(1)))
        return Value::errorVALUE();

    if (!calc->greater(s, Value(0)))
        return Value::errorVALUE();

    Value result(0.0);
    if (calc->equal(p, Value(1)))    //p==1
        result = Value::errorVALUE();
    else if (calc->greater(p, Value(0))) {    //p>0
        Value gaussInv = calc->gaussinv(p);
        // exp (gaussInv * s + m)
        result = calc->exp(calc->add(calc->mul(s, gaussInv), m));
    }

    return result;
}

//
// Function: lognormdist
//
// returns the cumulative lognormal distribution
//
Value func_lognormdist(valVector args, ValueCalc *calc, FuncExtra *)
{
    // defaults
    Value mue   = Value(0);
    Value sigma = Value(1);
    bool kum = true;

    Value x = args[0];
    if (args.count() > 1)
        mue = args[1];
    if (args.count() > 2)
        sigma = args[2];
    if (args.count() > 3)
        kum = calc->conv()->asInteger(args[3]).asInteger();

    if (!kum) {
        // TODO implement me !!!
        return Value::errorVALUE();

        // check constraints
        if (!calc->greater(sigma, 0.0) || (!calc->greater(x, 0.0)))
            return Value::errorVALUE();
    }
    // non-cumulative
    // check constraints
    if (calc->lower(x, Value(0.0)))
        return Value(0.0);

    // (ln(x) - mue) / sigma
    Value Y = calc->div(calc->sub(calc->ln(x), mue), sigma);
    return calc->add(calc->gauss(Y), 0.5);
}

//
// Function: MEDIAN
//
Value func_median(valVector args, ValueCalc *calc, FuncExtra *)
{
    // does NOT support anything other than doubles !!!
    List array;
    int number = 0;

    for (int i = 0; i < args.count(); ++i)
        func_array_helper(args[i], calc, array, number);

    if (number == 0)
        return Value::errorVALUE();

    qSort(array);
    double d;
    if (number % 2) // odd
        d = array.at((number - 1) / 2);
    else // even
        d = 0.5 * (array.at(number / 2 - 1) + array.at(number / 2));
    return Value(d);
}

//
// function: mode
//
Value func_mode(valVector args, ValueCalc *calc, FuncExtra *)
{
    // does NOT support anything other than doubles !!!
    ContentSheet sh;
    for (int i = 0; i < args.count(); ++i)
        func_mode_helper(args[i], calc, sh);

    // retrieve value with max.count
    int maxcount = 0;
    double max = 0.0;

    ContentSheet::iterator it;

    // check if there is a difference in frequency
    it = sh.begin();
    double last = it.value(); // init last with 1st value
    bool   nodiff = true;     // reset flag

    for (it = sh.begin(); it != sh.end(); ++it) {
        if (it.value() > maxcount) {
            max = it.key();
            maxcount = it.value();
        }
        if (last != it.value())
            nodiff = false; // set flag
    }

    // if no diff return error
    if (nodiff)
        return Value::errorNUM();
    else
        return Value(max);
}

//
// function: negbinomdist
//
Value func_negbinomdist(valVector args, ValueCalc *calc, FuncExtra *)
{
    double x = calc->conv()->asFloat(args[0]).asFloat();
    double r = calc->conv()->asFloat(args[1]).asFloat();
    double p = calc->conv()->asFloat(args[2]).asFloat();

    if (r < 0.0 || x < 0.0 || p < 0.0 || p > 1.0)
        return Value::errorVALUE();

    double q = 1.0 - p;
    double res = pow(p, r);

    for (double i = 0.0; i < x; i++)
        res *= (i + r) / (i + 1.0) * q;

    return Value(res);
//   int x = calc->conv()->asInteger (args[0]).asInteger();
//   int r = calc->conv()->asInteger (args[1]).asInteger();
//   Value p = args[2];
//
//   if ((x + r - 1) <= 0)
//     return Value::errorVALUE();
//   if (calc->lower (p, Value(0)) || calc->greater (p, Value(1)))
//     return Value::errorVALUE();
//
//   Value d1 = calc->combin (x + r - 1, r - 1);
//   // d2 = pow (p, r) * pow (1 - p, x)
//   Value d2 = calc->mul (calc->pow (p, r),
//       calc->pow (calc->sub (Value(1), p), x));
//
//   return calc->mul (d1, d2);
}

//
// Function: normdist
//
// returns the normal cumulative distribution
//
Value func_normdist(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value x = args[0];
    Value mue = args[1];
    Value sigma = args[2];
    Value k = args[3];

    if (!calc->greater(sigma, 0.0))
        return Value::errorVALUE();

    // (x - mue) / sigma
    Value Y = calc->div(calc->sub(x, mue), sigma);
    if (calc->isZero(k))    // density
        return calc->div(calc->phi(Y), sigma);
    else          // distribution
        return calc->add(calc->gauss(Y), 0.5);
}

//
// Function: norminv
//
// returns the inverse of the normal cumulative distribution
//
Value func_norminv(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value x = args[0];
    Value mue = args[1];
    Value sigma = args[2];

    if (!calc->greater(sigma, 0.0))
        return Value::errorVALUE();
    if (!(calc->greater(x, 0.0) && calc->lower(x, 1.0)))
        return Value::errorVALUE();

    // gaussinv (x)*sigma + mue
    return calc->add(calc->mul(calc->gaussinv(x), sigma), mue);
}

//
// Function: normsinv
//
// returns the inverse of the standard normal cumulative distribution
//
Value func_normsinv(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value x = args[0];
    if (!(calc->greater(x, 0.0) && calc->lower(x, 1.0)))
        return Value::errorVALUE();

    return calc->gaussinv(x);
}

//
// Function: percentile
//
// PERCENTILE( data set; alpha )
//
Value func_percentile(valVector args, ValueCalc *calc, FuncExtra*)
{
    double alpha = numToDouble(calc->conv()->toFloat(args[1]));

    // create array - does NOT support anything other than doubles !!!
    List array;
    int number = 0;

    func_array_helper(args[0], calc, array, number);

    // check constraints - number of values must be > 0 and flag >0 <=4
    if (number == 0)
        return Value::errorNA(); // or VALUE?
    if (alpha < -1e-9 || alpha > 1 + 1e-9)
        return Value::errorVALUE();

    // sort values
    qSort(array);

    if (number == 1)
        return Value(array[0]); // only one value
    else {
        double r = alpha * (number - 1);
        int index = ::floor(r);
        double d = r - index;
        return Value(array[index] + d * (array[index+1] - array[index]));
    }
}


//
// Function: phi
//
// distribution function for a standard normal distribution
//
Value func_phi(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->phi(args[0]);
}

//
// Function: poisson
//
// returns the Poisson distribution
//
Value func_poisson(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value x = args[0];
    Value lambda = args[1];
    Value kum = args[2];

    // lambda < 0.0 || x < 0.0
    if (calc->lower(lambda, Value(0.0)) || calc->lower(x, Value(0.0)))
        return Value::errorVALUE();

    Value result;

    // ex = exp (-lambda)
    Value ex = calc->exp(calc->mul(lambda, -1));

    if (calc->isZero(kum)) {    // density
        if (calc->isZero(lambda))
            result = Value(0);
        else
            // ex * pow (lambda, x) / fact (x)
            result = calc->div(calc->mul(ex, calc->pow(lambda, x)), calc->fact(x));
    } else {  // distribution
        if (calc->isZero(lambda))
            result = Value(1);
        else {
            result = Value(1.0);
            Value fFak(1.0);
            qint64 nEnd = calc->conv()->asInteger(x).asInteger();
            for (qint64 i = 1; i <= nEnd; i++) {
                // fFak *= i
                fFak = calc->mul(fFak, (int)i);
                // result += pow (lambda, i) / fFak
                result = calc->add(result, calc->div(calc->pow(lambda, (int)i), fFak));
            }
            result = calc->mul(result, ex);
        }
    }

    return result;
}

//
// Function: rank
//
// rank(rank; ref.;sort order)
Value func_rank(valVector args, ValueCalc *calc, FuncExtra*)
{
    double x = calc->conv()->asFloat(args[0]).asFloat();

    // default
    bool descending = true;

    double count = 1.0;
    double val   = 0.0;
    bool valid = false; // flag

    // opt. parameter
    if (args.count() > 2)
        descending = !calc->conv()->asInteger(args[2]).asInteger();

    // does NOT support anything other than doubles !!!
    List array;
    int number = 0;

    func_array_helper(args[1], calc, array, number);

    // sort array
    qSort(array);

    for (int i = 0; i < array.count(); i++) {
        if (descending)
            val = array[array.count()-count];
        else
            val = array[i];

        //kDebug()<<"count ="<<count<<" val = "<<val<<" x = "<<x;

        if (x == val) {
            valid = true;
            break;
        } else if ((!descending && x > val) || (descending && x < val))
            count++;
    }

    if (valid)
        return Value(count);
    else
        return Value::errorNA();
}

//
// Function: rsq
//
Value func_rsq(valVector args, ValueCalc *calc, FuncExtra*)
{
    const Value matrixA = args[0];
    const Value matrixB = args[1];

    // check constraints - arrays must have the same size
    if (matrixA.count() != matrixB.count())
        return Value::errorVALUE();

    double valA  = 0.0; // stores current array value
    double valB  = 0.0; // stores current array value
    double count = 0.0; // count fields with numbers
    double sumA  = 0.0, sumSqrA = 0.0;
    double sumB  = 0.0, sumSqrB = 0.0;
    double sumAB = 0.0;

    // calc sums
    for (uint v = 0; v < matrixA.count(); ++v) {
        Value vA(calc->conv()->asFloat(matrixA.element(v)));
        Value vB(calc->conv()->asFloat(matrixB.element(v)));

        // TODO add unittest for check
        if (!vA.isError() && !vB.isError()) {// only if numbers are in both fields
            valA = calc->conv()->asFloat(matrixA.element(v)).asFloat();
            valB = calc->conv()->asFloat(matrixB.element(v)).asFloat();
            count++;
            //kDebug()<<"valA ="<<valA<<" valB ="<<valB;

            // value A
            sumA    += valA;        // add sum
            sumSqrA += valA * valA; // add square
            // value B
            sumB    += valB;        // add sum
            sumSqrB += valB * valB; // add square

            sumAB   += valA * valB;
        }
    }

    // check constraints
    if (count < 2)
        return Value::errorNA();
    else
        return Value((count*sumAB   - sumA*sumB) *(count*sumAB   - sumA*sumB) /
                     (count*sumSqrA - sumA*sumA) / (count*sumSqrB - sumB*sumB));
}

//
// Function: quartile
//
// QUARTILE( data set; flag )
//
// flag:
//  0 equals MIN()
//  1 25th percentile
//  2 50th percentile equals MEDIAN()
//  3 75th percentile
//  4 equals MAX()
//
Value func_quartile(valVector args, ValueCalc *calc, FuncExtra*)
{
    int flag = calc->conv()->asInteger(args[1]).asInteger();

    // create array - does NOT support anything other than doubles !!!
    List array;
    int number = 0;

    func_array_helper(args[0], calc, array, number);

    // check constraints - number of values must be > 0 and flag >0 <=4
    if (number == 0)
        return Value::errorNA(); // or VALUE?
    if (flag < 0 || flag > 4)
        return Value::errorVALUE();

    // sort values
    qSort(array);

    if (number == 1)
        return Value(array[0]); // only one value
    else {
        //
        // flag 0 -> MIN()
        //
        if (flag == 0)
            return Value(array[0]);

        //
        // flag 1 -> 25th percentile
        //
        else if (flag == 1) {
            int nIndex = ::floor(0.25 * (number - 1));
            double diff = 0.25 * (number - 1) - ::floor(0.25 * (number - 1));

            if (diff == 0.0)
                return Value(array[nIndex]);
            else
                return Value(array[nIndex] + diff*(array[nIndex+1] - array[nIndex]));
        }

        //
        // flag 2 -> 50th percentile equals MEDIAN()
        //
        else if (flag == 2) {
            if (number % 2 == 0)
                return Value((array[number/2-1] + array[number/2]) / 2.0);
            else
                return Value(array[(number-1)/2]);
        }

        //
        // flag 3 -> 75thpercentile
        //
        else if (flag == 3) {
            int nIndex = ::floor(0.75 * (number - 1));
            double diff = 0.75 * (number - 1) - ::floor(0.75 * (number - 1));

            if (diff == 0.0)
                return Value(array[nIndex]);
            else
                return Value(array[nIndex] + diff*(array[nIndex+1] - array[nIndex]));
        }

        //
        // flag 4 -> equals MAX()
        //
        else
            return Value(array[number-1]);
    }
}


//
// function: skew_est
//
Value func_skew_est(valVector args, ValueCalc *calc, FuncExtra *)
{
    int number = calc->count(args);
    Value avg = calc->avg(args);
    if (number < 3)
        return Value::errorVALUE();

    Value res = calc->stddev(args, avg);
    if (res.isZero())
        return Value::errorVALUE();

    Value params(Value::Array);
    params.setElement(0, 0, avg);
    params.setElement(1, 0, res);
    Value tskew;
    calc->arrayWalk(args, tskew, awSkew, params);

    // ((tskew * number) / (number-1)) / (number-2)
    return calc->div(calc->div(calc->mul(tskew, number), number - 1), number - 2);
}

//
// function: skew_pop
//
Value func_skew_pop(valVector args, ValueCalc *calc, FuncExtra *)
{
    int number = calc->count(args);
    Value avg = calc->avg(args);
    if (number < 1)
        return Value::errorVALUE();

    Value res = calc->stddevP(args, avg);
    if (res.isZero())
        return Value::errorVALUE();

    Value params(Value::Array);
    params.setElement(0, 0, avg);
    params.setElement(1, 0, res);
    Value tskew;
    calc->arrayWalk(args, tskew, awSkew, params);

    // tskew / number
    return calc->div(tskew, (double)number);
}

//
// Function: SLOPE
//
Value func_slope(valVector args, ValueCalc* calc, FuncExtra*)
{
    int numberY = calc->count(args[0]);
    int numberX = calc->count(args[1]);

    if (numberY < 1 || numberX < 1 || numberY != numberX)
        return Value::errorVALUE();

    Value denominator;
    Value avgY = calc->avg(args[0]);
    Value avgX = calc->avg(args[1]);
    Value nominator = func_covar_helper(args[0], args[1], calc, avgY, avgX);
    calc->arrayWalk(args[1], denominator, calc->awFunc("devsq"), avgX);
    return calc->div(nominator, denominator);
}

//
// function: small
//
Value func_small(valVector args, ValueCalc *calc, FuncExtra *)
{
    // does NOT support anything other than doubles !!!
    int k = calc->conv()->asInteger(args[1]).asInteger();
    if (k < 1)
        return Value::errorVALUE();

    List array;
    int number = 1;

    func_array_helper(args[0], calc, array, number);

    if (k > number || k - 1 >= array.count())
        return Value::errorVALUE();

    qSort(array);
    double d = array.at(k - 1);
    return Value(d);
}

//
// function: standardize
//
Value func_standardize(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value x = args[0];
    Value m = args[1];
    Value s = args[2];

    if (!calc->greater(s, Value(0)))   // s must be >0
        return Value::errorVALUE();

    // (x - m) / s
    return calc->div(calc->sub(x, m), s);
}

//
// Function: stddev
//
Value func_stddev(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->stddev(args, false);
}

//
// Function: stddeva
//
Value func_stddeva(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->stddev(args);
}

//
// Function: stddevp
//
Value func_stddevp(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->stddevP(args, false);
}

//
// Function: stddevpa
//
Value func_stddevpa(valVector args, ValueCalc *calc, FuncExtra *)
{
    return calc->stddevP(args);
}

//
// Function: normsdist
//
Value func_stdnormdist(valVector args, ValueCalc *calc, FuncExtra *)
{
    //returns the cumulative lognormal distribution, mue=0, sigma=1
    return calc->add(calc->gauss(args[0]), 0.5);
}

//
// Function: STEYX
//
Value func_steyx(valVector args, ValueCalc* calc, FuncExtra*)
{
    int number = calc->count(args[0]);

    if (number < 1 || number != calc->count(args[1]))
        return Value::errorVALUE();

    Value varX, varY;
    Value avgY = calc->avg(args[0]);
    Value avgX = calc->avg(args[1]);
    Value cov = func_covar_helper(args[0], args[1], calc, avgY, avgX);
    calc->arrayWalk(args[0], varY, calc->awFunc("devsq"), avgY);
    calc->arrayWalk(args[1], varX, calc->awFunc("devsq"), avgX);
    Value numerator = calc->sub(varY, calc->div(calc->sqr(cov), varX));
    Value denominator = calc->sub(Value(number), 2);
    return calc->sqrt(calc->div(numerator, denominator));
}

//
// Function: sumproduct
//
Value func_sumproduct(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value result;
    calc->twoArrayWalk(args[0], args[1], result, tawSumproduct);
    return result;
}

//
// Function: sumx2py2
//
Value func_sumx2py2(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value result;
    calc->twoArrayWalk(args[0], args[1], result, tawSumx2py2);
    return result;
}

//
// Function: sumx2my2
//
Value func_sumx2my2(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value result;
    calc->twoArrayWalk(args[0], args[1], result, tawSumx2my2);
    return result;
}

//
// Function: SUMXMY2
//
Value func_sumxmy2(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value result;
    calc->twoArrayWalk(args[0], args[1], result, tawSumxmy2);
    return result;
}

//
// Function: tdist
//
// returns the t-distribution
//
Value func_tdist(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value T = args[0];
    Value fDF = args[1];
    int flag = calc->conv()->asInteger(args[2]).asInteger();

    if (calc->lower(fDF, Value(1)) || (flag != 1 && flag != 2))
        return Value::errorVALUE();

    // arg = fDF / (fDF + T * T)
    Value arg = calc->div(fDF, calc->add(fDF, calc->sqr(T)));

    Value R;
    R = calc->mul(calc->GetBeta(arg, calc->div(fDF, 2.0), Value(0.5)), 0.5);

    if (flag == 1)
        return R;
    return calc->mul(R, 2);    // flag is 2 here
}

//
// Function: tinv
//
// returns the inverse t-distribution
//
Value func_tinv(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value p  = args[0];
    Value DF = args[1];

    Value result;

    // constraints
    if (calc->lower(DF, 1.0)  || calc->greater(DF, 1.0E5) ||
            calc->greater(p, 1.0) || calc->lower(p, 0.0))
        return Value::errorVALUE();

    bool convergenceError;

    // create formula string
    QString formula = QString("TDIST(x;%1;2)").arg((double)DF.asFloat());

    result = IterateInverse(p.asFloat(), formula , DF.asFloat() * 0.5, DF.asFloat(), convergenceError);

    if (convergenceError)
        return Value::errorVALUE();

    return result;
}

//
// Function: trend
//
// TREND ( knownY [; [knownX] [; [newX] [; allowOsset = TRUE() ] ] ] )
//
// TODO - check do we need 2d arrays?
//
Value func_trend(valVector args, ValueCalc *calc, FuncExtra *)
{
    // default
    bool withOffset = true;

    if (args.count() > 3)
        withOffset = calc->conv()->asInteger(args[3]).asInteger();

    List knownY, knownX, newX;
    int  knownXcount, newXcount;

    //
    // knownX
    //
    if (args[1].isEmpty()) {
        // if knownX is empty it has to be set to the sequence 1,2,3... n (n number of counts knownY)
        for (uint i = 1; i < args[0].count() + 1; i++)
            knownX.append(i);
    } else {
        // check constraints / TODO if 2d array, then we must check dimension row&col?
        if (args[0].count() != args[1].count())
            return Value::errorNUM();

        // copy array to list
        func_array_helper(args[1], calc, knownX, knownXcount);
    }

    //
    // newX
    //
    if (args[2].isEmpty()) {
        for (uint i = 1; i < args[0].count() + 1; i++)
            newX.append(i);
    } else {
        // copy array to list
        func_array_helper(args[2], calc, newX, newXcount);
    }

    // create the resulting matrix
    Value res(Value::Array);

    // arrays for slope und intercept
    Value Y(Value::Array);
    Value X(Value::Array);

    Value sumXX(0.0); // stores sum of xi*xi
    Value sumYX(0.0); // stores sum of yi*xi

    // copy data in arrays
    for (uint i = 0; i < args[0].count(); ++i) {
        X.setElement(i, 0, Value((double)knownX[i]));
        sumXX = calc->add(sumXX, calc->mul(Value((double)knownX[i]), Value((double)knownX[i])));
    }

    for (uint i = 0; i < args[0].count(); ++i) {
        Y.setElement(i, 0, Value(args[0].element(i)));
        // sum yi*xi
        sumYX = calc->add(sumYX, calc->mul(Value(args[0].element(i)), Value((double)knownX[i])));
    }

    // create parameter for func_slope and func_intercept calls
    valVector param;

    param.append(Y);
    param.append(X);

    // a1 = [xy]/[x*x]
    Value a1 = calc->div(sumYX, sumXX);

    // v2 = INTERCEPT = b
    Value v2 = func_intercept(param, calc, 0); // v2 is const, we only need to calc it once

    // fill array up with values
    for (uint i = 0; i < args[2].count(); ++i) {
        Value trend;
        Value v1;

        if (withOffset) {
            v1 = calc->mul(func_slope(param, calc, 0), Value(newX[i]));
            trend = Value(calc->add(v1  , v2));
        } else {
            // b=0
            // x*a1
            trend = calc->mul(a1, Value(newX[i]));
        }

        // set value in res array
        res.setElement(i, 0, trend);
    }

    return (res);   // return array
}

//
// Function: trimmean
//
Value func_trimmean(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value dataSet    = args[0];
    Value cutOffFrac = args[1];

    // constrains 0 <= cutOffFrac < 1
    if (calc->lower(cutOffFrac, Value(0)) || !calc->lower(cutOffFrac, Value(1)))
        return Value::errorVALUE();

    // cutOff = floor( n*cutOffFrac/2)
    int cutOff = floor(calc->div(calc->mul(cutOffFrac , Value((int)dataSet.count())), 2).asFloat());

    double res = 0.0;

    // sort parameter into QList array
    List array;
    int valCount = 0; // stores the number of values in array

    func_array_helper(args[0], calc, array, valCount);

    if (valCount == 0)
        return Value::errorVALUE();

    qSort(array);

    for (int i = cutOff; i < valCount - cutOff ; i++)
        res += array[i];

    res /= (valCount - 2 * cutOff);

    return Value(res);
}

//
// Function TTEST
//
Value func_ttest(valVector args, ValueCalc* calc, FuncExtra*)
{
    Value x = args[0];
    Value y = args[1];
    int mode = calc->conv()->asInteger(args[2]).asInteger();
    int type = calc->conv()->asInteger(args[3]).asInteger();

    int numX = calc->count(x);
    int numY = calc->count(y);

    // check mode parameter
    if (mode < 1 || mode > 2)
        return Value::errorVALUE();
    // check type parameter
    if (type < 1 || type > 3)
        return Value::errorVALUE();
    // check amount of numbers in sequences
    if (numX < 2 || numY < 2 || (type == 1 && numX != numY))
        return Value::errorVALUE();

    Value t;
    Value dof;
    if (type == 1) {
        // paired
        dof = calc->sub(Value(numX), 1);

        Value mean;
        calc->twoArrayWalk(x, y, mean, tawSumxmy);
        mean = calc->div(mean, numX);

        Value sigma;
        calc->twoArrayWalk(x, y, sigma, tawSumxmy2);
        sigma = calc->sqrt(calc->sub(calc->div(sigma, numX), calc->sqr(mean)));

        t = calc->div(calc->mul(mean, calc->sqrt(dof)), sigma);
    } else if (type == 2) {
        // independent, equal variances
        dof = calc->sub(calc->add(Value(numX), Value(numY)), 2);

        Value avgX = calc->avg(x);
        Value avgY = calc->avg(y);
        Value varX, varY;
        calc->arrayWalk(x, varX, calc->awFunc("devsq"), avgX);
        calc->arrayWalk(y, varY, calc->awFunc("devsq"), avgY);
        varX = calc->div(varX, calc->sub(Value(numX), 1));
        varY = calc->div(varY, calc->sub(Value(numX), 1));

        Value numerator = calc->sub(calc->avg(x), calc->avg(y));

        Value denominator = calc->div(varX, numX);
        denominator = calc->add(denominator, calc->div(varY, numY));
        denominator = calc->sqrt(denominator);

        t = calc->div(numerator, denominator);
    } else {
        // independent, unequal variances

        Value avgX = calc->avg(x);
        Value avgY = calc->avg(y);
        Value varX, varY;
        calc->arrayWalk(x, varX, calc->awFunc("devsq"), avgX);
        calc->arrayWalk(y, varY, calc->awFunc("devsq"), avgY);
        varX = calc->div(varX, calc->sub(Value(numX), (double)1));
        varY = calc->div(varY, calc->sub(Value(numX), (double)1));

        Value numerator = calc->add(Value(numX), Value(numY));
        numerator = calc->div(calc->mul(Value(numX), calc->mul(Value(numY), calc->sub(numerator, (double)2))), numerator);
        numerator = calc->mul(calc->sub(calc->avg(x), calc->avg(y)), calc->sqrt(numerator));

        Value denominator = calc->mul(calc->sub(Value(numX), (double)1), varX);
        denominator = calc->add(denominator, calc->mul(calc->sub(Value(numY), (double)1), varY));
        denominator = calc->sqrt(denominator);

        t = calc->div(numerator, denominator);

        // inspired from Gnumeric
        dof = calc->div(Value(1.0), calc->add(Value(1.0), calc->div(calc->mul(varY, numX), calc->mul(varX, numY))));
        dof = calc->div(Value(1.0), calc->add(calc->div(calc->sqr(dof), calc->sub(Value(numX), (double)1)), calc->div(calc->sqr(calc->sub(Value(1), dof)), calc->sub(Value(numY), (double)1))));
    }

    valVector tmp(3);
    tmp.insert(0, t);
    tmp.insert(1, dof);
    tmp.insert(2, Value(mode));
    return func_tdist(tmp, calc, 0);
}

//
// Function: variance
//
Value func_variance(valVector args, ValueCalc *calc, FuncExtra *)
{
    int count = calc->count(args, false);
    if (count < 2)
        return Value::errorVALUE();

    Value result = func_devsq(args, calc, 0);
    return calc->div(result, count - 1);
}

//
// Function: vara
//
Value func_variancea(valVector args, ValueCalc *calc, FuncExtra *)
{
    int count = calc->count(args);
    if (count < 2)
        return Value::errorVALUE();

    Value result = func_devsqa(args, calc, 0);
    return calc->div(result, count - 1);
}

//
// Function: varp
//
Value func_variancep(valVector args, ValueCalc *calc, FuncExtra *)
{
    int count = calc->count(args, false);
    if (count == 0)
        return Value::errorVALUE();

    Value result = func_devsq(args, calc, 0);
    return calc->div(result, count);
}

//
// Function: varpa
//
Value func_variancepa(valVector args, ValueCalc *calc, FuncExtra *)
{
    int count = calc->count(args);
    if (count == 0)
        return Value::errorVALUE();

    Value result = func_devsqa(args, calc, 0);
    return calc->div(result, count);
}

//
// Function: weibull
//
// returns the Weibull distribution
//
Value func_weibull(valVector args, ValueCalc *calc, FuncExtra *)
{
    Value x = args[0];
    Value alpha = args[1];
    Value beta = args[2];
    Value kum = args[3];

    Value result;

    if ((!calc->greater(alpha, 0.0)) || (!calc->greater(beta, 0.0)) ||
            calc->lower(x, 0.0))
        return Value::errorVALUE();

    // ex = exp (-pow (x / beta, alpha))
    Value ex;
    ex = calc->exp(calc->mul(calc->pow(calc->div(x, beta), alpha), -1));
    if (calc->isZero(kum)) {   // density
        // result = alpha / pow(beta,alpha) * pow(x,alpha-1.0) * ex
        result = calc->div(alpha, calc->pow(beta, alpha));
        result = calc->mul(result, calc->mul(calc->pow(x,
                                             calc->sub(alpha, 1)), ex));
    } else   // distribution
        result = calc->sub(1.0, ex);

    return result;
}

//
// Function ZTEST
//
Value func_ztest(valVector args, ValueCalc* calc, FuncExtra*)
{
    int number = calc->count(args[0]);

    if (number < 2)
        return Value::errorVALUE();

    // standard deviation is optional
    Value sigma = (args.count() > 2) ? args[2] : calc->stddev(args[0], false);
    // z = Ex - mu / sigma * sqrt(N)
    Value z = calc->div(calc->mul(calc->sub(calc->avg(args[0]), args[1]), calc->sqrt(Value(number))), sigma);
    // result = 1 - ( NORMDIST(-abs(z),0,1,1) + ( 1 - NORMDIST(abs(z),0,1,1) ) )
    return calc->mul(Value(2.0), calc->gauss(calc->abs(z)));
}

#include "StatisticalModule.moc"
