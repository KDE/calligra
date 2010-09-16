/* This file is part of the KDE project
   Copyright 2007 Sascha Pfau <MrPeacock@gmail.com>
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2006 Ariya Hidayat <ariya@kde.org>

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

#include "TestStatisticalFunctions.h"

#include <math.h>

#include "qtest_kde.h"

#include <CellStorage.h>
#include <Formula.h>
#include <Map.h>
#include <Sheet.h>

#include "TestKspreadCommon.h"

using namespace KSpread;

// NOTE: we do not compare the numbers _exactly_ because it is difficult
// to get one "true correct" expected values for the functions due to:
//  - different algorithms among spreadsheet programs
//  - precision limitation of floating-point number representation
//  - accuracy problem due to propagated error in the implementation
#define CHECK_EVAL(x,y) QCOMPARE(TestDouble(x,y,6),y)
#define CHECK_EVAL_SHORT(x,y) QCOMPARE(TestDouble(x,y,10),y)
#define CHECK_ARRAY(x,y) QCOMPARE(TestArray(x,y,10),true)
#define CHECK_ARRAY_NOSIZE(x,y) QCOMPARE(TestArray(x,y,10,false),true)
#define ROUND(x) (roundf(1e15 * x) / 1e15)

bool TestStatisticalFunctions::TestArray(const QString& formula, const QString& _Array, int accuracy, bool checkSize = true)
{
    // define epsilon
    double epsilon = DBL_EPSILON * pow(10.0, (double)(accuracy));

    Value Array = evaluate(_Array);
//   kDebug()<<"Array = "<<Array;

    Value result = evaluate(formula);

    // test match size
    if (checkSize)
        if (Array.rows() != result.rows() || Array.columns() != result.columns()) {
            kDebug() << "Array size do not match";
            return false;
        }

    // if checkSize is disabled the count of Array array could be lower than result array
    for (int e = 0; e < (int)Array.count(); e++) {
        kDebug() << "check element (" << e << ") " << (double)Array.element(e).asFloat() << " " << (double)result.element(e).asFloat();
        bool res = (long double) fabsl(Array.element(e).asFloat() - result.element(e).asFloat()) < epsilon;
        if (!res) {
            kDebug() << "check failed -->" << "Element(" << e << ") " << (double)Array.element(e).asFloat() << " to" << (double) result.element(e).asFloat() << "  diff =" << (double)(Array.element(e).asFloat() - result.element(e).asFloat());
            return false;
        }
    }
    // test passed
    return true;
}

Value TestStatisticalFunctions::TestDouble(const QString& formula, const Value& v2, int accuracy)
{
    double epsilon = DBL_EPSILON * pow(10.0, (double)(accuracy));

    Formula f(m_map->sheet(0)); // bind to test case data set
    QString expr = formula;
    if (expr[0] != '=')
        expr.prepend('=');
    f.setExpression(expr);
    Value result = f.eval();

    bool res = fabs(v2.asFloat() - result.asFloat()) < epsilon;

    if (!res)
        kDebug(36002) << "check failed -->" << "Epsilon =" << epsilon << "" << (double)v2.asFloat() << " to" << (double)result.asFloat() << "  diff =" << (double)(v2.asFloat() - result.asFloat());
//   else
//     kDebug(36002)<<"check -->" <<"  diff =" << v2.asFloat()-result.asFloat();
    if (res)
        return v2;
    else
        return result;
}

// round to get at most 15-digits number
static Value RoundNumber(const Value& v)
{
    if (v.isNumber()) {
        double d = numToDouble(v.asFloat());
        if (fabs(d) < DBL_EPSILON)
            d = 0.0;
        return Value(ROUND(d));
    } else
        return v;
}

Value TestStatisticalFunctions::evaluate(const QString& formula)
{
    Formula f(m_map->sheet(0));
    QString expr = formula;
    if (expr[0] != '=')
        expr.prepend('=');
    f.setExpression(expr);
    Value result = f.eval();

#if 0
    // this magically generates the CHECKs
    printf("  CHECK_EVAL( \"%s\",  %15g) );\n", qPrintable(formula), result.asFloat());
#endif

    return RoundNumber(result);
}

void TestStatisticalFunctions::initTestCase()
{
    FunctionModuleRegistry::instance()->loadFunctionModules();
    m_map = new Map(0 /*no Doc*/);
    m_map->addNewSheet();
    Sheet* sheet = m_map->sheet(0);
    CellStorage* storage = sheet->cellStorage();

    //
    // Test case data set
    //


    // A19:A29
    storage->setValue(1, 19, Value(1));
    storage->setValue(1, 20, Value(2));
    storage->setValue(1, 21, Value(4));
    storage->setValue(1, 22, Value(8));
    storage->setValue(1, 23, Value(16));
    storage->setValue(1, 24, Value(32));
    storage->setValue(1, 25, Value(64));
    storage->setValue(1, 26, Value(128));
    storage->setValue(1, 27, Value(256));
    storage->setValue(1, 28, Value(512));
    storage->setValue(1, 29, Value(1024));
    storage->setValue(1, 30, Value(2048));
    storage->setValue(1, 31, Value(4096));


    // B3:B17
    storage->setValue(2, 3, Value("7"));
    storage->setValue(2, 4, Value(2));
    storage->setValue(2, 5, Value(3));
    storage->setValue(2, 6, Value(true));
    storage->setValue(2, 7, Value("Hello"));
    // B8 leave empty
    storage->setValue(2, 9, Value::errorDIV0());
    storage->setValue(2, 10, Value(0));
    storage->setValue(2, 11, Value(3));
    storage->setValue(2, 12, Value(4));
    storage->setValue(2, 13, Value("2005-0131T01:00:00"));
    storage->setValue(2, 14, Value(1));
    storage->setValue(2, 15, Value(2));
    storage->setValue(2, 16, Value(3));
    storage->setValue(2, 17, Value(4));
    CHECK_EVAL("AVEDEV(1;2;3;4)",              Value(1));

    // C4:C6
    storage->setValue(3, 4, Value(4));
    storage->setValue(3, 5, Value(5));
    storage->setValue(3, 6, Value(7));
    // C11:C17
    storage->setValue(3, 11, Value(5));
    storage->setValue(3, 12, Value(6));
    storage->setValue(3, 13, Value(8));
    storage->setValue(3, 14, Value(4));
    storage->setValue(3, 15, Value(3));
    storage->setValue(3, 16, Value(2));
    storage->setValue(3, 17, Value(1));
    // C19:C31
    storage->setValue(3, 19, Value(0));
    storage->setValue(3, 20, Value(5));
    storage->setValue(3, 21, Value(2));
    storage->setValue(3, 22, Value(5));
    storage->setValue(3, 23, Value(3));
    storage->setValue(3, 24, Value(4));
    storage->setValue(3, 25, Value(4));
    storage->setValue(3, 26, Value(0));
    storage->setValue(3, 27, Value(8));
    storage->setValue(3, 28, Value(1));
    storage->setValue(3, 29, Value(9));
    storage->setValue(3, 30, Value(6));
    storage->setValue(3, 31, Value(2));
    // C51:C57
    storage->setValue(3, 51, Value(7));
    storage->setValue(3, 52, Value(9));
    storage->setValue(3, 53, Value(11));
    storage->setValue(3, 54, Value(12));
    storage->setValue(3, 55, Value(15));
    storage->setValue(3, 56, Value(17));
    storage->setValue(3, 57, Value(19));


    // D51:D57
    storage->setValue(4, 51, Value(100));
    storage->setValue(4, 52, Value(105));
    storage->setValue(4, 53, Value(104));
    storage->setValue(4, 54, Value(108));
    storage->setValue(4, 55, Value(111));
    storage->setValue(4, 56, Value(120));
    storage->setValue(4, 57, Value(133));

    // F19:F26
    storage->setValue(6, 19, Value(20));
    storage->setValue(6, 20, Value(5));
    storage->setValue(6, 21, Value(-20));
    storage->setValue(6, 22, Value(-60));
    storage->setValue(6, 23, Value(75));
    storage->setValue(6, 24, Value(-29));
    storage->setValue(6, 25, Value(20));
    storage->setValue(6, 26, Value(30));
    // F51:F60
    storage->setValue(6, 51, Value(3));
    storage->setValue(6, 52, Value(4));
    storage->setValue(6, 53, Value(5));
    storage->setValue(6, 54, Value(2));
    storage->setValue(6, 55, Value(3));
    storage->setValue(6, 56, Value(4));
    storage->setValue(6, 57, Value(5));
    storage->setValue(6, 58, Value(6));
    storage->setValue(6, 59, Value(4));
    storage->setValue(6, 60, Value(7));


    // G51:G60
    storage->setValue(7, 51, Value(23));
    storage->setValue(7, 52, Value(24));
    storage->setValue(7, 53, Value(25));
    storage->setValue(7, 54, Value(22));
    storage->setValue(7, 55, Value(23));
    storage->setValue(7, 56, Value(24));
    storage->setValue(7, 57, Value(25));
    storage->setValue(7, 58, Value(26));
    storage->setValue(7, 59, Value(24));
    storage->setValue(7, 60, Value(27));

    // H19:H31
    storage->setValue(8, 19, Value("2005-03-12"));
    storage->setValue(8, 20, Value("2002-02-03"));
    storage->setValue(8, 21, Value("2005-03-08"));
    storage->setValue(8, 22, Value("1991-03-27"));
    storage->setValue(8, 23, Value("1967-07-05"));
    storage->setValue(8, 24, Value("1912-12-23"));
    storage->setValue(8, 25, Value("1992-02-06"));
    storage->setValue(8, 26, Value("1934-07-04"));
    storage->setValue(8, 27, Value("1909-01-08"));
    storage->setValue(8, 28, Value("1989-11-28"));
    storage->setValue(8, 29, Value("2000-02-22"));
    storage->setValue(8, 30, Value("2004-03-29"));
    storage->setValue(8, 31, Value("1946-07-13"));

    // I19:I31
    storage->setValue(9, 19, Value(13));
    storage->setValue(9, 20, Value(12));
    storage->setValue(9, 21, Value(11));
    storage->setValue(9, 22, Value(10));
    storage->setValue(9, 23, Value(9));
    storage->setValue(9, 24, Value(8));
    storage->setValue(9, 25, Value(7));
    storage->setValue(9, 26, Value(6));
    storage->setValue(9, 27, Value(5));
    storage->setValue(9, 28, Value(4));
    storage->setValue(9, 29, Value(3));
    storage->setValue(9, 30, Value(2));
    storage->setValue(9, 31, Value(1));
}

void TestStatisticalFunctions::testAVEDEV()
{
    // ODF-tests
    CHECK_EVAL("AVEDEV(1;2;3;4)",              Value(1));    //
}

void TestStatisticalFunctions::testAVERAGE()
{
    // ODF-tests
    CHECK_EVAL("AVERAGE(2; 4)",                Value(3));    //
}

void TestStatisticalFunctions::testAVERAGEA()
{
    // ODF-tests
    CHECK_EVAL("AVERAGEA(2; 4)",               Value(3));    //
    CHECK_EVAL("AVERAGEA(TRUE(); FALSE(); 5)", Value(2));    //
}

void TestStatisticalFunctions::testBETADIST()
{
    // ODF-tests

    // Cumulative tests
    CHECK_EVAL("BETADIST( 0  ; 3; 4)",           Value(0));           //
    CHECK_EVAL("BETADIST( 0.5; 3; 4)",           Value(0.656250));    //
    CHECK_EVAL("BETADIST( 0.9; 4; 3)",           Value(0.984150));    //
    CHECK_EVAL("BETADIST( 2  ; 3; 4)",           Value(1));           // constraints x > b should be 1 if cumulative
    CHECK_EVAL("BETADIST(-1  ; 3; 4)",           Value(0));           // constraints x < a

    CHECK_EVAL_SHORT("BETADIST(1.5;3;4;1;2)",    evaluate("BETADIST(0.5;3;4)"));    // diff = -2.27021e-09
    CHECK_EVAL_SHORT("BETADIST(2;3;4;1;3)",      evaluate("BETADIST(0.5;3;4)"));    // diff = -2.27021e-09

    // last parameter FALSE (non - Cumulative)
    CHECK_EVAL("BETADIST( 0  ;3;4;0;1;FALSE())", Value(0));               //
    CHECK_EVAL("BETADIST( 0.5;3;4;0;1;FALSE())", Value(0.0005208333));    // 0.000521
    CHECK_EVAL("BETADIST( 0.9;4;3;0;1;FALSE())", Value(0.0001215000));    // 0.000122
    CHECK_EVAL("BETADIST( 2  ;3;4;0;1;FALSE())", Value(0));               // constraints x > b should be 0 if non-cumulative
    CHECK_EVAL("BETADIST(-1  ;3;4;0;1;FALSE())", Value(0));               // constraints x < a

    CHECK_EVAL("BETADIST(1.5;3;4;1;2;FALSE())",  evaluate("BETADIST(0.5;3;4;0;1;FALSE())"));    //
    CHECK_EVAL("BETADIST(2  ;3;4;1;3;FALSE())",  evaluate("BETADIST(0.5;3;4;0;1;FALSE())"));    //
}

void TestStatisticalFunctions::testBETAINV()
{
    // ODF-tests
    CHECK_EVAL("BETADIST(BETAINV(0;3;4);3;4)",           Value(0));      //
    CHECK_EVAL("BETADIST(BETAINV(0.1;3;4);3;4)",         Value(0.1));    //
    CHECK_EVAL("BETADIST(BETAINV(0.3;3;4);3;4)",         Value(0.3));    //
    CHECK_EVAL("BETADIST(BETAINV(0.5;4;3);4;3)",         Value(0.5));    //
    CHECK_EVAL("BETADIST(BETAINV(0.7;4;3);4;3)",         Value(0.7));    //
    CHECK_EVAL("BETADIST(BETAINV(1;3;4);3;4)",           Value(1));      //
    CHECK_EVAL("BETADIST(BETAINV(0;3;4;1;3);3;4;1;3)",   Value(0));      //
    CHECK_EVAL("BETADIST(BETAINV(0.1;3;4;1;3);3;4;1;3)", Value(0.1));    //
    CHECK_EVAL("BETADIST(BETAINV(0.3;3;4;1;3);3;4;1;3)", Value(0.3));    //
    CHECK_EVAL("BETADIST(BETAINV(0.5;4;3;1;3);4;3;1;3)", Value(0.5));    //
    CHECK_EVAL("BETADIST(BETAINV(0.7;4;3;1;3);4;3;1;3)", Value(0.7));    //
    CHECK_EVAL("BETADIST(BETAINV(1;3;4;1;3);3;4;1;3)",   Value(1));      //
}

void TestStatisticalFunctions::testBINOMDIST()
{
    // bettersolution.com
    CHECK_EVAL("BINOMDIST(10;10;  1  ;0)", Value(1));               // Prob.=100% - all trials successful
    CHECK_EVAL("BINOMDIST(9 ; 1; 10  ;0)", Value(0));               // Prob. of -exactly- 9 trials successful is 0 then
    CHECK_EVAL("BINOMDIST(10;10;  0.1;1)", Value(1));               // Sum of probabilities of 0..10 hits is 1.
//     CHECK_EVAL("BINOMDIST(4 ;10;  0.4;1)", Value( 0.6331032576 ) ); // Some random values.
    // my tests
    CHECK_EVAL_SHORT("BINOMDIST(4 ;10;  0.4;1)", Value(0.6331032576));    // Some random values.
    CHECK_EVAL_SHORT("BINOMDIST(5 ;10;  0.4;1)", Value(0.8337613824));    // Some random values.
    CHECK_EVAL_SHORT("BINOMDIST(6 ;10;  0.4;1)", Value(0.9452381184));    // Some random values.
    CHECK_EVAL_SHORT("BINOMDIST(4 ;10;  0.2;1)", Value(0.9672065024));    // Some random values.
    CHECK_EVAL_SHORT("BINOMDIST(5 ;10;  0.2;1)", Value(0.9936306176));    // Some random values.
    CHECK_EVAL_SHORT("BINOMDIST(6 ;10;  0.2;1)", Value(0.9991356416));    // Some random values.
}

void TestStatisticalFunctions::testCHIDIST()
{
    // bettersolution.com
    CHECK_EVAL("CHIDIST( 18.307;10)",      Value(0.0500005892));    //
    CHECK_EVAL("CHIDIST(      2;2)",       Value(0.3678794412));    //
    CHECK_EVAL("CHIDIST(     -1;2)",       Value(1));               // constraint x<0 TODO EXCEL return #NUM!
//     CHECK_EVAL("CHIDIST(     4;\"texr\")", Value::VALUE()    ); // TODO
}

void TestStatisticalFunctions::testCONFIDENCE()
{
    // ODF-tests
    CHECK_EVAL("CONFIDENCE(0.5 ; 1;1)", Value(0.67448975));      //
    CHECK_EVAL("CONFIDENCE(0.25; 1;1)", Value(1.1503493804));    //
    CHECK_EVAL("CONFIDENCE(0.5 ; 4;1)", Value(2.6979590008));    // Multiplying stddev by X multiplies result by X.
    CHECK_EVAL("CONFIDENCE(0.5 ; 1;4)", Value(0.3372448751));    // Multiplying count by X*X divides result by X.

    // check constraints
    CHECK_EVAL("CONFIDENCE(-0.5; 1;4)", Value::errorNUM());    // 0 < alpha < 1
    CHECK_EVAL("CONFIDENCE( 1.5; 1;4)", Value::errorNUM());    // 0 < alpha < 1
    CHECK_EVAL("CONFIDENCE( 0.5;-1;4)", Value::errorNUM());    // stddev > 0
    CHECK_EVAL("CONFIDENCE( 0.5; 1;0)", Value::errorNUM());    // size >= 1
}

void TestStatisticalFunctions::testCORREL()
{
    //  Cell | Value      Cell | Value
    // ------+------     ------+------
    //   B14 |  1          C14 |  4
    //   B15 |  2          C15 |  3
    //   B16 |  3          C16 |  2
    //   B17 |  4          C17 |  1

    // ODF-tests
    CHECK_EVAL("CORREL(B14:B17;B14:B17)", Value(1));               // Perfect positive correlation given identical sequences
    CHECK_EVAL("CORREL(B14:B17;C14:C17)", Value(-1));              // Perfect negative correlation given reversed sequences
    CHECK_EVAL("CORREL(1;2)",             Value::errorNUM());      // Each list must contain at least 2 values
    CHECK_EVAL("CORREL(B14:B16;B15:B16)", Value::errorNUM());      // The length of each list must be equal
}

void TestStatisticalFunctions::testCOVAR()
{
    //  Cell | Value      Cell | Value
    // ------+------     ------+------
    //   B14 |  1          C14 |  4
    //   B15 |  2          C15 |  3
    //   B16 |  3          C16 |  2
    //   B17 |  4          C17 |  1

    // ODF-tests
    CHECK_EVAL("COVAR(C11:C17;C11:C17)", Value(4.9795918367));     //
    CHECK_EVAL("COVAR(B14:B17;C14:C17)", Value(-1.25));            //
    CHECK_EVAL("COVAR(B14:B17;C13:C17)", Value::errorNUM());       // TODO should we check for "array sizes don't match" or "value counts" in array?.
}

void TestStatisticalFunctions::testDEVSQ()
{
    // ODF-tests
    CHECK_EVAL("DEVSQ(4)",         Value(0));                // One value - no deviation.
    CHECK_EVAL("DEVSQ(5;5;5;5)",   Value(0));                // Identical values - no deviation.
    CHECK_EVAL("DEVSQ(2;4)",       Value(2));                // Each value deviates by 1.
    CHECK_EVAL("DEVSQ(-5;5;-1;1)", Value(52));               // Average=0 must work properly.
    CHECK_EVAL("DEVSQ(C11:C17)",   Value(34.8571428571));    // Test values.
    CHECK_EVAL("DEVSQ(B14:B17)",   Value(5.00));             // Test values.
    CHECK_EVAL("DEVSQ(B14)",       Value(0));                // One value - no deviation.
}

// void TestStatisticalFunctions::testDEVSQA()
// {
//     // no test available
// }

void TestStatisticalFunctions::testEXPONDIST()
{
    // ODF-tests
    CHECK_EVAL("EXPONDIST( 1;1;TRUE())",   Value(0.6321205588));    //
    CHECK_EVAL("EXPONDIST( 2;2;TRUE())",   Value(0.9816843611));    //
    CHECK_EVAL("EXPONDIST( 0;1;TRUE())",   Value(0));               //
    CHECK_EVAL("EXPONDIST(-1;1;TRUE())",   Value(0));               // constraint x<0

    CHECK_EVAL("EXPONDIST( 1;1;FALSE())",  Value(0.3678794412));    //
    CHECK_EVAL("EXPONDIST( 2;2;FALSE())",  Value(0.0366312778));    //
    CHECK_EVAL("EXPONDIST( 0;1;FALSE())",  Value(1));               //
    CHECK_EVAL("EXPONDIST(-1;1;FALSE())",  Value(0));               // constraint x<0

    // test disabled, because 3rd param. is not opt.!
    //CHECK_EVAL("EXPONDIST(1;1)", evaluate("EXPONDIST(1;1;TRUE())") );
}

void TestStatisticalFunctions::testFDIST()
{
    // ODF-tests

    // cumulative
    CHECK_EVAL("FDIST( 1;4;5)", Value(0.5143428033));       //
    CHECK_EVAL("FDIST( 2;5;4)", Value(0.7392019723));       //
    CHECK_EVAL("FDIST( 0;4;5)", Value(0));                  //
    CHECK_EVAL("FDIST(-1;4;5)", Value(0));                  //

    CHECK_EVAL_SHORT("FDIST( 1;4;5;TRUE())", evaluate("FDIST(1;4;5)"));  // diff = -1.39644e-09

    // non-cumulative
    CHECK_EVAL("FDIST( 1;4;5;FALSE())", Value(0.3976140792));    //
    CHECK_EVAL("FDIST( 2;5;4;FALSE())", Value(0.1540004108));    //
    CHECK_EVAL("FDIST( 0;4;5;FALSE())", Value(0));               //
    CHECK_EVAL("FDIST(-1;4;5;FALSE())", Value(0));               //
}

void TestStatisticalFunctions::testFINV()
{
    // ODF-tests
    CHECK_EVAL("FDIST(FINV(0.1;3;4);3;4)", Value(0.1));    //
    CHECK_EVAL("FDIST(FINV(0.3;3;4);3;4)", Value(0.3));    //
    CHECK_EVAL("FDIST(FINV(0.5;3;4);3;4)", Value(0.5));    //
    CHECK_EVAL("FDIST(FINV(0.7;3;4);3;4)", Value(0.7));    //
    CHECK_EVAL("FDIST(FINV(0.0;3;4);3;4)", Value(0.0));    //
}

void TestStatisticalFunctions::testFISHER()
{
    // ODF-tests
    CHECK_EVAL("FISHER(0)",                        Value(0));             // Fisher of 0.
    CHECK_EVAL("FISHER((EXP(1)-1)/(EXP(1)+1))",    Value(0.5));           // Argument chosen so that ln=1
    CHECK_EVAL_SHORT("FISHER(0.5)",                Value(0.54930614));    // TODO - be more precise - Some random value.
    CHECK_EVAL("FISHER(0.47)+FISHER(-0.47)",       Value(0));             // Function is symetrical.
}

void TestStatisticalFunctions::testFISHERINV()
{
    // ODF-tests
    CHECK_EVAL("FISHERINV(0)",                     Value(0));             // Fisherinv of 0.
    CHECK_EVAL("FISHERINV(LN(2))",                 Value(0.6));           // e^(2*ln(2))=4
    CHECK_EVAL("FISHERINV(FISHER(0.5))",           Value(0.5));           // Some random value.
    CHECK_EVAL("FISHERINV(0.47)+FISHERINV(-0.47)", Value(0));             // Function is symetrical.
}

void TestStatisticalFunctions::testFREQUENCY()
{
    Value result(Value::Array);
    result.setElement(0, 0, Value(3));
    result.setElement(0, 1, Value(2));
    result.setElement(0, 2, Value(4));
    result.setElement(0, 3, Value(1));
    CHECK_EVAL("FREQUENCY({1;2;3;4;5;6;7;8;9;10};{3|5|9})", result);
    // the second arg has to be a column vector
    CHECK_EVAL("ISERROR(FREQUENCY({1;2;3;4;5;6;7;8;9;10};{3;5;9}))", Value(true));
    // an empty second arg returns the overall number count
    CHECK_EVAL("FREQUENCY({1;2;3;4;5;6;7;8;9;10};)", Value(10));
}

void TestStatisticalFunctions::testFTEST()
{
    // TODO - be more precise
    // ODF-tests
    CHECK_EVAL_SHORT("FTEST(B14:B17; C14:C17)", Value(1.0));            // Same data (second reversed),
    CHECK_EVAL_SHORT("FTEST(B14:B15; C13:C14)", Value(0.311916521));    // Significantly different variances,
    // so less likely to come from same data set.
}

void TestStatisticalFunctions::testGAMMADIST()
{
    // bettersolution.com non-cumulative
    CHECK_EVAL("GAMMADIST(10 ;9;2;FALSE())",      Value(0.0326390197));    //

    // bettersolution.com cumulative
    CHECK_EVAL("GAMMADIST(10 ;9;2;TRUE())",       Value(0.0680936347));    //
    CHECK_EVAL("GAMMADIST(10 ;10;5;TRUE())",      Value(0.0000464981));    // Bettersolution = 0 .rounded?
    CHECK_EVAL("GAMMADIST(7 ;5;1;TRUE())",        Value(0.8270083921));    // TODO NOK / Bettersolution = 1

    // bettersolution.com constraints
    CHECK_EVAL("GAMMADIST(10 ;9;0;TRUE())",       Value::errorNUM());      // beta = 0 not allowed
    CHECK_EVAL("GAMMADIST(10 ;-2;2;TRUE())",      Value::errorNUM());      // was wird getestet? alpha
    CHECK_EVAL("GAMMADIST(-1 ;9;2;TRUE())",       Value::errorNUM());      // NOK
    CHECK_EVAL("GAMMADIST(7 ;\"text\";1;TRUE())", Value::errorVALUE());    // text not allowed
    CHECK_EVAL("GAMMADIST(7 ;5;\"text\";TRUE())", Value::errorVALUE());    // text not allowed

    // ODF-tests non-cumulative
    CHECK_EVAL("GAMMADIST(0  ;3;4;FALSE())",      Value(0));
    CHECK_EVAL("GAMMADIST(0.5;3;4;FALSE())",      Value(0.0017236268));    //
    CHECK_EVAL("GAMMADIST(9  ;4;3;FALSE())",      Value(0.0746806026));    // ODF-Specs -> 0.0666979468 should be 0,0746
    CHECK_EVAL("GAMMADIST(0  ;3;4;FALSE())",      Value(0));
    CHECK_EVAL("GAMMADIST(9  ;4;3;FALSE())",      Value(0.0746806026));    // TODO check ODF-Specs -> 0.390661

    // ODF-tests cumulative
    CHECK_EVAL("GAMMADIST(0.5;3;4;TRUE())",       Value(0.0002964775));    //
    CHECK_EVAL("GAMMADIST(9  ;4;3;TRUE())",       Value(0.3527681112));
    CHECK_EVAL("GAMMADIST(-1 ;4;3;TRUE())",       Value(0));               // neg. x return always 0
    CHECK_EVAL("GAMMADIST(-1 ;3;4;FALSE())",      Value(0));               // neg. x return always 0

    // various tests cumulative
    CHECK_EVAL("GAMMADIST(9 ;9;2;TRUE())",        Value(0.0402573125));    //
    CHECK_EVAL("GAMMADIST(9 ;8;2;TRUE())",        Value(0.0865864716));    //
}

void TestStatisticalFunctions::testGAMMAINV()
{
    // ODF-tests
    CHECK_EVAL("GAMMADIST(GAMMAINV(0.1;3;4);3;4;1)",     Value(0.1));    //
    CHECK_EVAL("GAMMADIST(GAMMAINV(0.3;3;4);3;4;1)",     Value(0.3));    //
    CHECK_EVAL("GAMMADIST(GAMMAINV(0.5;3;4);3;4;1)",     Value(0.5));    //
    CHECK_EVAL("GAMMADIST(GAMMAINV(0.7;3;4);3;4;1)",     Value(0.7));    //
    CHECK_EVAL("GAMMADIST(GAMMAINV(0  ;3;4);3;4;1)",     Value(0));      //
}

void TestStatisticalFunctions::testGAUSS()
{
    // ODF-tests
    CHECK_EVAL("GAUSS(0)",     Value(0));                // Mean of one value.
    CHECK_EVAL("GAUSS(1)",     Value(0.341344746));      // Multiple equivalent values.
    // my test
    CHECK_EVAL("GAUSS(-0.25)", Value(-0.0987063257));    // check neg. values. test for fixes gauss_func
}

void TestStatisticalFunctions::testGROWTH()
{
    // constraints
    CHECK_EVAL("GROWTH({}; C19:C23; 1)",          Value::errorNA());  // empty knownY matrix
    CHECK_EVAL("GROWTH({5.0;\"a\"}; C19:C23; 1)", Value::errorNA());  // knownY matrix constains chars

    // ODF-tests
    CHECK_ARRAY("GROWTH( A19:A23; C19:C23; 1 )",          "{2.5198420998}");  // with offset
    CHECK_ARRAY("GROWTH( A19:A23; C19:C23; 1; FALSE() )", "{1.4859942891}");  // without offset

    // http://www.techonthenet.com/excel/formulas/growth.php
    CHECK_ARRAY("GROWTH({4;5;6};{10;20;30};{15;30;45})", "{4.4569483434;6.0409611796;8.1879369384}");  //
    CHECK_ARRAY("GROWTH({4;5;6};{10;20;30})",            "{4.0273074534;4.9324241487;6.0409611796}");  //
    CHECK_ARRAY_NOSIZE("GROWTH({4;5;6})",                "{4.0273074534}");                            //
}

void TestStatisticalFunctions::testGEOMEAN()
{
    // ODF-tests
    CHECK_EVAL("GEOMEAN(7)",           Value(7));               // Mean of one value.
    CHECK_EVAL("GEOMEAN(5;5;5;5)",     Value(5));               // Multiple equivalent values.
    CHECK_EVAL("GEOMEAN(2;8;2;8)",     Value(4));               // Some values.
    CHECK_EVAL("GEOMEAN(8;0;8;8;8;8)", Value::errorNUM());      // Error if there is a 0 in the range.
    CHECK_EVAL("GEOMEAN(C11)",         Value(5));               // One value, range.
    CHECK_EVAL("GEOMEAN(C11:C17)",     Value(3.4451109418));    // Some values, range.
    CHECK_EVAL("GEOMEAN(B14:B17)",     Value(2.2133638394));    // Some values, range.
}

void TestStatisticalFunctions::testHARMEAN()
{
    // ODF-tests
    CHECK_EVAL("HARMEAN(7)",           Value(7));               // Mean of one value.
    CHECK_EVAL("HARMEAN(4;4;4;4)",     Value(4));               // Multiple equivalent values.
    CHECK_EVAL("HARMEAN(2;4;4)",       Value(3));               // Some values.
    CHECK_EVAL("HARMEAN(8;0;8;8;8;8)", Value::errorNUM());      // Error if there is a 0 in the range.
    CHECK_EVAL("HARMEAN(C11)",         Value(5));               // One value, range.
    CHECK_EVAL("HARMEAN(C11:C17)",     Value(2.7184466019));    // Some values, range.
    CHECK_EVAL("HARMEAN(B14:B17)",     Value(1.92));            // Some values, range.
}

void TestStatisticalFunctions::testHYPGEOMDIST()
{
    // ODF-tests
    CHECK_EVAL("HYPGEOMDIST( 2  ;3;3;6;FALSE())", Value(0.45));        // If an urn contains 3 red balls and 3 green balls, the probability
    // that 2 red balls will be selected after 3 selections without replacement.
    // (0.45=27/60).
    CHECK_EVAL("HYPGEOMDIST( 2  ;3;3;6)",         Value(0.45));        // The default for cumulative is FALSE().
    CHECK_EVAL("HYPGEOMDIST( 0  ;3;3;6)",         Value(0.05));        // There is a small (5%) chance of selecting only green balls.
    CHECK_EVAL("HYPGEOMDIST( 2  ;3;3;6;TRUE())",  Value(0.95));        // The probability of selecting at most two red balls (i.e 0, 1 or 2).
    CHECK_EVAL("HYPGEOMDIST( 4  ;3;3;6)",         Value::errorNUM());  // X must be <= M
    CHECK_EVAL("HYPGEOMDIST( 2.8;3;3;6)",         Value(0.45));        // Non-integers are truncated.
    CHECK_EVAL("HYPGEOMDIST(-2  ;3;3;6)",         Value::errorNUM());  // Values must be >= 0.
    CHECK_EVAL("HYPGEOMDIST( 0  ;0;0;0)",         Value(1));           //
}

void TestStatisticalFunctions::testINTERCEPT()
{
    // bettersolution.com
    CHECK_EVAL_SHORT("INTERCEPT({2;3;9;1;8};{6;5;11;7;5})", Value(0.048387097));    // TODO - be more precise
//     CHECK_EVAL_SHORT("INTERCEPT({2;4;6};{6;3;8})",          Value( 2.21053     ) ); // TODO - be more precise
    CHECK_EVAL("INTERCEPT({2;3;9};{6;5;11;7;5})",     Value::errorNUM());     //
    CHECK_EVAL("INTERCEPT(\"text\";{6;5;11;7;5})",    Value::errorNUM());     // text is not allowed
}

void TestStatisticalFunctions::testKURT()
{
    // TODO check function

    // ODF-tests
    CHECK_EVAL("KURT(C20:C25)",     Value(-0.446162998));    //
    CHECK_EVAL("KURT(C20:C23;4;4)", Value(-0.446162998));    //
}

void TestStatisticalFunctions::testLARGE()
{
    //  Cell | Value | N'th
    // ------+-------+------
    //   B14 |   1   |   3
    //   B15 |   2   |   2
    //   B16 |   3   |   1

    // ODF-tests
    CHECK_EVAL("LARGE(B14:B16;1)", Value(3));           //
    CHECK_EVAL("LARGE(B14:B16;3)", Value(1));           //
    CHECK_EVAL("LARGE(B14:B16;4)", Value::errorNUM());  // N is greater than the length of the list
}

void TestStatisticalFunctions::testLEGACYCHIDIST()
{
    // ODF-tests LEGACY.CHIDIST
    CHECK_EVAL("LEGACYCHIDIST(-1;2)", Value(1));    // constraint x<0
    CHECK_EVAL("LEGACYCHIDIST( 0;2)", Value(1));    // constraint x=0
    CHECK_EVAL("LEGACYCHIDIST( 2;2)", Value(0.3678794412));    //
    CHECK_EVAL("LEGACYCHIDIST( 4;4)", Value(0.4060058497));    //
}

void TestStatisticalFunctions::testLEGACYCHIINV()
{
    // ODF-tests LEGACY.CHIINV
    CHECK_EVAL("LEGACYCHIDIST(LEGACYCHIINV(0.1;3);3)",   Value(0.1));    //
    CHECK_EVAL("LEGACYCHIDIST(LEGACYCHIINV(0.3;3);3)",   Value(0.3));    //
    CHECK_EVAL("LEGACYCHIDIST(LEGACYCHIINV(0.5;3);3)",   Value(0.5));    //
    CHECK_EVAL("LEGACYCHIDIST(LEGACYCHIINV(0.7;3);3)",   Value(0.7));    //
    CHECK_EVAL("LEGACYCHIDIST(LEGACYCHIINV(0.9;3);3)",   Value(0.9));    //
    CHECK_EVAL("LEGACYCHIDIST(LEGACYCHIINV(0.1;20);20)", Value(0.1));    //
    CHECK_EVAL("LEGACYCHIDIST(LEGACYCHIINV(0.3;20);20)", Value(0.3));    //
    CHECK_EVAL("LEGACYCHIDIST(LEGACYCHIINV(0.5;20);20)", Value(0.5));    //
    CHECK_EVAL("LEGACYCHIDIST(LEGACYCHIINV(0.7;20);20)", Value(0.7));    //
    CHECK_EVAL("LEGACYCHIDIST(LEGACYCHIINV(0.9;20);20)", Value(0.9));    //
    CHECK_EVAL("LEGACYCHIDIST(LEGACYCHIINV(1.0;20);20)", Value(1.0));    //
}

void TestStatisticalFunctions::testLEGACYFDIST()
{
    // ODF-tests
    CHECK_EVAL("LEGACYFDIST( 1;4;5)", Value(0.4856571967));    //
    CHECK_EVAL("LEGACYFDIST( 2;5;4)", Value(0.2607980277));    //
    CHECK_EVAL("LEGACYFDIST( 0;4;5)", Value(1));               //
    CHECK_EVAL("LEGACYFDIST(-1;4;5)", Value::errorNUM());      //
}

void TestStatisticalFunctions::testLEGACYFINV()
{
    // ODF-tests
    CHECK_EVAL("LEGACYFDIST(LEGACYFINV(0.1;3;4);3;4)", Value(0.1));    //
    CHECK_EVAL("LEGACYFDIST(LEGACYFINV(0.3;3;4);3;4)", Value(0.3));    //
    CHECK_EVAL("LEGACYFDIST(LEGACYFINV(0.5;3;4);3;4)", Value(0.5));    //
    CHECK_EVAL("LEGACYFDIST(LEGACYFINV(0.7;3;4);3;4)", Value(0.7));    //
    CHECK_EVAL("LEGACYFDIST(LEGACYFINV(1.0;3;4);3;4)", Value(1.0));    //
}

void TestStatisticalFunctions::testLOGINV()
{
    // TODO check function

    // ODF-tests
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.1;0;1);0;1;TRUE())", Value(0.1));    //
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.3;0;1);0;1;TRUE())", Value(0.3));    //
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.5;0;1);0;1;TRUE())", Value(0.5));    //
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.7;0;1);0;1;TRUE())", Value(0.7));    //
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.9;0;1);0;1;TRUE())", Value(0.9));    //
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.1;1;4);1;4;TRUE())", Value(0.1));    //
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.3;1;4);1;4;TRUE())", Value(0.3));    //
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.5;1;4);1;4;TRUE())", Value(0.5));    //
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.7;1;4);1;4;TRUE())", Value(0.7));    //
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.9;1;4);1;4;TRUE())", Value(0.9));    //
    CHECK_EVAL("LOGINV(0.5)",                             Value(1));      //
}

void TestStatisticalFunctions::testLOGNORMDIST()
{
    // TODO - implement cumulative calculation
    //      - check definition cumulative/non-cumulative and constraints

    // ODF-tests

    // cumulative
    CHECK_EVAL("LOGNORMDIST(1)",              Value(0.5));             //
    CHECK_EVAL("LOGNORMDIST(1;1;4)",          Value(0.4012936743));    //
    CHECK_EVAL("LOGNORMDIST(1;0;1;TRUE())",   Value(0.5));             //
    CHECK_EVAL("LOGNORMDIST(1;1;4;TRUE())",   Value(0.4012936743));    //
    CHECK_EVAL("LOGNORMDIST(1;-1;4;TRUE())",  Value(0.5987063257));    //
//     CHECK_EVAL("LOGNORMDIST(2;-1;4;TRUE())",  Value( 0.663957 ) ); // ??????
    CHECK_EVAL("LOGNORMDIST(3;0;1;TRUE())",   Value(0.8640313924));    //
    CHECK_EVAL("LOGNORMDIST(100;0;1;TRUE())", Value(0.9999979394));    //
    CHECK_EVAL("LOGNORMDIST(-1;0;1;TRUE())",  Value(0));               // constraint x<0 returns 0

    // non-cumulative
//     CHECK_EVAL("LOGNORMDIST( 1; 0; 1;FALSE())", Value( 0.398942 ) ); //
//     CHECK_EVAL("LOGNORMDIST( 1; 1; 4;FALSE())", Value( 0.096667 ) ); //
//     CHECK_EVAL("LOGNORMDIST( 1;-1; 4;FALSE())", Value( 0.096667 ) ); //
//     CHECK_EVAL("LOGNORMDIST( 2;-1; 4;FALSE())", Value( 0.045595 ) ); //
//     CHECK_EVAL("LOGNORMDIST(-1; 0; 1;FALSE())", Value::errorNUM() ); // constraint failure
//     CHECK_EVAL("LOGNORMDIST( 1; 0;-1;FALSE())", Value::errorNUM() ); // constraint failure
}

void TestStatisticalFunctions::testMAX()
{
    //  Cell | Value      Cell | Value
    // ------+------     ------+------
    //   B3  |  "7"        C14 |  4
    //   B4  |  2          C15 |  3
    //   B5  |  3          C16 |  2
    //   B6  |  true       C17 |  1
    //   B7  |  "Hello"
    //   B8  |
    //   B9  | DIV/0


    // ODF-tests
    CHECK_EVAL("MAX(2;4;1;-8)", Value(4));             // Negative numbers are smaller than positive numbers.
    CHECK_EVAL("MAX(B4:B5)",    Value(3));             // The maximum of (2,3) is 3.
//     CHECK_EVAL("ISNA(MAXA(NA())", Value(true)); // nline errors are propagated.
    CHECK_EVAL("MAX(B3:B5)",    Value(3));             // Strings are not converted to numbers and are ignored.
    CHECK_EVAL("MAX(-1;B7)",    Value(-1));            // Strings are not converted to numbers and are ignored.
    CHECK_EVAL("MAX(B3:B9)",    Value::errorVALUE());  // TODO check function - Errors inside ranges are NOT ignored.
}

void TestStatisticalFunctions::testMAXA()
{
    // ODF-tests
    CHECK_EVAL("MAXA(2;4;1;-8)", Value(4));             // Negative numbers are smaller than positive numbers.
    CHECK_EVAL("MAXA(B4:B5)",    Value(3));             // The maximum of (2,3) is 3.
//     CHECK_EVAL("ISNA(MAXA(NA())", Value(true)); // Inline errors are propagated.

// TODO check function - inline Text must be converted, but not Text in Cells
//     CHECK_EVAL("MAXA(B3:B5)",    Value(          3 ) ); // Cell text is converted to 0.

    CHECK_EVAL("MAXA(-1;B7)",    Value(0));             // Cell text is converted to 0.
    CHECK_EVAL("MAXA(\"a\")",    Value::errorVALUE());  // Text inline is NOT ignored.
    CHECK_EVAL("MAXA(B3:B9)",    Value::errorVALUE());  // TODO check function - Errors inside ranges are NOT ignored.
    CHECK_EVAL("MAXA(B6:B7)",    Value(1));             // Logicals are considered numbers.
}

void TestStatisticalFunctions::testMEDIAN()
{
    // ODF-tests
    CHECK_EVAL("=MEDIAN(10.5;7.2)",        Value(8.85));
    CHECK_EVAL("=MEDIAN(7.2;200;5.4;45)",  Value(26.1));
    CHECK_EVAL("=MEDIAN(7.2;200;5.4;8.1)", Value(7.65));
    CHECK_EVAL("=MEDIAN(1;3;13;14;15)",    Value(13.0));
    CHECK_EVAL("=MEDIAN(1;3;13;14;15;35)", Value(13.5));
    // Bug 148574: MEDIAN function gives incorrect results
    CHECK_EVAL("=MEDIAN(1;2;3)",     Value(2));
    CHECK_EVAL("=MEDIAN(1;2;3;4;5)", Value(3));
}

void TestStatisticalFunctions::testMIN()
{
    // ODF-tests
    CHECK_EVAL("MIN(2;4;1;-8)", Value(-8));          // Negative numbers are smaller than positive numbers.
    CHECK_EVAL("MIN(B4:B5)",    Value(2));           // The minimum of (2,3) is 2.
    CHECK_EVAL("MIN(B3)",       Value(0));           // If no numbers are provided in all ranges, MIN returns 0
    CHECK_EVAL("MIN(\"a\")",    Value::errorNUM());  // Non-numbers inline are NOT ignored.
    CHECK_EVAL("MIN(B3:B5)",    Value(2));           // Cell text is not converted to numbers and is ignored.
}

void TestStatisticalFunctions::testMINA()
{
    // ODF-tests
    CHECK_EVAL("MINA(2;4;1;-8)", Value(-8));          // Negative numbers are smaller than positive numbers.
    CHECK_EVAL("MINA(B4:B5)",    Value(2));           // The minimum of (2,3) is 2.
    CHECK_EVAL("MINA(1;B7)",     Value(0));           // Cell text is converted to 0.
    CHECK_EVAL("MINA(\"a\")",    Value::errorNUM());  // Cell text inline is NOT ignored.

// TODO check function - inline Text must be converted, but not Text in Cells
//     CHECK_EVAL("MINA(B3:B5)",    Value(        0 ) ); // Cell text is converted to 0.

    CHECK_EVAL("MINA(B6:C6)",    Value(1));           // The value "True" is considered equivalent to 1.
}

void TestStatisticalFunctions::testMODE()
{
    // ODF-tests
    CHECK_EVAL("MODE(F51:F60)",                                 Value(4));           //
    CHECK_EVAL("MODE(G51;G52;G53;G54;G55;G56;G57;G58;G59;G60)", Value(24));          //
    CHECK_EVAL("MODE(1;2;3;4;5;6;7;8;9;10)",                    Value::errorNUM());  //
}

void TestStatisticalFunctions::testNEGBINOMDIST()
{
    // ODF-test
//     CHECK_EVAL("NEGBINOMDIST(F20;I29;H6)", Value( 0.000130947 ) ); //

    // bettersolutions.com
    CHECK_EVAL("NEGBINOMDIST( 0;1; 0.25)", Value(0.25));            //
    CHECK_EVAL("NEGBINOMDIST( 0;1; 0.5)",  Value(0.5));             //
    CHECK_EVAL("NEGBINOMDIST( 1;6; 0.5)",  Value(0.046875));        //
    CHECK_EVAL("NEGBINOMDIST(10;5; 0.25)", Value(0.0550486604));    //
    CHECK_EVAL("NEGBINOMDIST(10;5;-4)",    Value::errorNUM());      //
//     CHECK_EVAL("NEGBINOMDIST(10;"text";0.25)", Value::NUM() ); //
}

void TestStatisticalFunctions::testNORMDIST()
{
    // ODF-tests
    CHECK_EVAL("NORMDIST(0;1;4;TRUE())",         Value(0.4012936743));    //
    CHECK_EVAL("NORMDIST(0;0;1;FALSE())",        Value(0.3989422804));    //
    CHECK_EVAL("NORMDIST(0;0;1;TRUE())",         Value(0.5));             //
    CHECK_EVAL("NORMDIST(0;1;4;FALSE())",        Value(0.0966670292));    //
    CHECK_EVAL("NORMDIST(0;-1;4;FALSE())",       Value(0.0966670292));    //
    CHECK_EVAL("NORMDIST(0;-1;4;TRUE())",        Value(0.5987063257));    //
    CHECK_EVAL("NORMDIST(1;-1;4;FALSE())",       Value(0.0880163317));    //
    CHECK_EVAL("NORMDIST(1;-1;4;TRUE())",        Value(0.6914624613));    //
    CHECK_EVAL("NORMDIST(1.281552;0;1;TRUE())",  Value(0.9000000762));    //
    CHECK_EVAL("NORMDIST(0;-1.281552;1;TRUE())", Value(0.9000000762));    //
    CHECK_EVAL("NORMDIST(0;0;-1;FALSE())",       Value::errorNUM());      //
}

void TestStatisticalFunctions::testNORMINV()
{
    // ODF-tests
    CHECK_EVAL("NORMDIST(NORMINV(0.1;0;1);0;1;TRUE())", Value(0.1));    //
    CHECK_EVAL("NORMDIST(NORMINV(0.3;0;1);0;1;TRUE())", Value(0.3));    //
    CHECK_EVAL("NORMDIST(NORMINV(0.5;0;1);0;1;TRUE())", Value(0.5));    //
    CHECK_EVAL("NORMDIST(NORMINV(0.7;0;1);0;1;TRUE())", Value(0.7));    //
    CHECK_EVAL("NORMDIST(NORMINV(0.9;0;1);0;1;TRUE())", Value(0.9));    //
    CHECK_EVAL("NORMDIST(NORMINV(0.1;1;4);1;4;TRUE())", Value(0.1));    //
    CHECK_EVAL("NORMDIST(NORMINV(0.3;1;4);1;4;TRUE())", Value(0.3));    //
    CHECK_EVAL("NORMDIST(NORMINV(0.5;1;4);1;4;TRUE())", Value(0.5));    //
    CHECK_EVAL("NORMDIST(NORMINV(0.7;1;4);1;4;TRUE())", Value(0.7));    //
    CHECK_EVAL("NORMDIST(NORMINV(0.9;1;4);1;4;TRUE())", Value(0.9));    //
}

void TestStatisticalFunctions::testPEARSON()
{
    //  Cell | Value       Cell | Value       Cell | Value       Cell | Value
    // ------+-------     ------+-------     ------+-------     ------+-------
    //  A19  |    1        C19  |  0           C51 |   7          D51 |  100
    //  A20  |    2        C20  |  5           C51 |   9          D52 |  105
    //  A21  |    4        C21  |  2           C53 |  11          D53 |  104
    //  A22  |    8        C22  |  5           C54 |  12          D54 |  108
    //  A23  |   16        C23  |  3           C55 |  15          D55 |  111
    //  A24  |   32        C24  |  4           C56 |  17          D56 |  120
    //  A25  |   64        C25  |  4           C57 |  19          D57 |  133
    //  A26  |  128        C26  |  0
    //  A27  |  256        C27  |  8
    //  A28  |  512        C28  |  1
    //  A29  | 1024        C29  |  9
    //  A30  | 2048        C30  |  6
    //  A31  | 4096        C31  |  2

    // ODF-tests
    CHECK_EVAL_SHORT("PEARSON(A19:A31;C19:C31)", Value(0.045989147));    //
    CHECK_EVAL_SHORT("PEARSON(C51:C57;D51:D57)", Value(0.930164207));    //
    CHECK_EVAL("PEARSON(C51:C57;D51:D56)", Value::errorNUM());     //
}

void TestStatisticalFunctions::testPERCENTILE()
{
    // ODF-tests
    CHECK_EVAL("PERCENTILE(A19:A31;0.38)",          Value(24.96));        //
    CHECK_EVAL("PERCENTILE(A19:A31;0.95)",          Value(2867.2));       //
    CHECK_EVAL("PERCENTILE(A19:A31;0.05)",          Value(1.6));          //

    // my tests
    CHECK_EVAL("PERCENTILE(A10:A15;-0.1)",          Value::errorVALUE()); //
    CHECK_EVAL("PERCENTILE(A19:A25;1.1)",           Value::errorVALUE()); //

}

void TestStatisticalFunctions::testPERMUT()
{
    // ODF-tests
    CHECK_EVAL("PERMUT(2;2)",     Value(2));           // =2!/(2-2)!
    CHECK_EVAL("PERMUT(4;2)",     Value(12));          // =4!/(4-2)!
    CHECK_EVAL("PERMUT(4.3;2.1)", Value(12));          // =PERMUT(4;2)
    CHECK_EVAL("PERMUT(-4;2)",    Value::errorNUM());  //
    CHECK_EVAL("PERMUT(4;-2)",    Value::errorNUM());  //
}

void TestStatisticalFunctions::testPHI()
{
    //  Cell | Value
    // ------+-------
    //   C23 |   3
    //       |

    // ODF-tests
    CHECK_EVAL_SHORT("PHI(C23/10)",  Value(0.381387815));    // TODO - be more precise /
    CHECK_EVAL_SHORT("PHI(-C23/10)", Value(0.381387815));    // TODO - be more precise /
    CHECK_EVAL_SHORT("PHI(0)",       Value(0.398942280));    // TODO - be more precise /
}

void TestStatisticalFunctions::testPOISSON()
{
    // ODF-tests
    CHECK_EVAL_SHORT("POISSON(0;1;FALSE())", Value(0.367880));    // TODO - be more precise /
    CHECK_EVAL_SHORT("POISSON(0;2;FALSE())", Value(0.135335));    // TODO - be more precise /
}

void TestStatisticalFunctions::testRANK()
{
    //  Cell | Value
    // ------+------
    //  A19  |   1
    //  A20  |   2
    //  A21  |   4
    //  A22  |   8
    //  A23  |  16
    //  A24  |  32
    //  A25  |  64

    // ODF-tests
    CHECK_EVAL("RANK(A20;A19:A25;1)", Value(2));    // ascending
    CHECK_EVAL("RANK(A25;A19:A25;0)", Value(1));    // descending
    CHECK_EVAL("RANK(A21;A19:A25  )", Value(5));    // ommitted equals descending order
}

void TestStatisticalFunctions::testRSQ()
{
    // ODF-tests
    CHECK_EVAL("RSQ(H19:H31;I19:I31)", Value(0.075215010));    //
    CHECK_EVAL("RSQ(H19:H31;I19:I30)", Value::errorNA());      // array does not have the same size
}

void TestStatisticalFunctions::testQUARTILE()
{
    // flag:
    //  0 equals MIN()
    //  1 25th percentile
    //  2 50th percentile equals MEDIAN()
    //  3 75th percentile
    //  4 equals MAX()

    // ODF-tests
    CHECK_EVAL("QUARTILE(A19:A25;3)",            Value(24));            //
    CHECK_EVAL("QUARTILE(F19:F26;1)",            Value(-22.25));        //
    CHECK_EVAL("QUARTILE(A10:A15;2)",            Value::errorVALUE());  //
    CHECK_EVAL("QUARTILE(A19:A25;5)",            Value::errorVALUE());  // flag > 4
    CHECK_EVAL("QUARTILE(F19:F26;1.5)",          Value(-22.25));        // 1.5 rounded down to 1
    CHECK_EVAL("QUARTILE({1;2;4;8;16;32;64};3)", Value(24));            //

    // my tests
    CHECK_EVAL("QUARTILE(A19:A25;0)",            Value(1));             // MIN()
    CHECK_EVAL("QUARTILE(A19:A25;4)",            Value(64));            // MAX()

}

void TestStatisticalFunctions::testSKEW()
{
    // ODF-tests
    CHECK_EVAL_SHORT("SKEW( 1; 2; 4 )", Value(0.935219));    // TODO - be more precise / Expectation value: 2.333333
    // Standard deviation: 1.257525
    // Third central moment: 0.740741
    CHECK_EVAL_SHORT("SKEW(A19:A23)",   Value(1.325315));    // TODO - be more precise /
    CHECK_EVAL("SKEW( 1; 2 )",    Value::errorNUM());  // At least three numbers.
}

void TestStatisticalFunctions::testSKEWP()
{
    // ODF-tests
    CHECK_EVAL_SHORT("SKEWP( 1; 2; 4 )", Value(0.381802));    // TODO - be more precise / Expectation value: 2.333333
    // Standard deviation: 1.247219
    // Third central moment: 0.740741
    CHECK_EVAL_SHORT("SKEWP(A19:A23)",   Value(0.889048));    // TODO - be more precise /
    CHECK_EVAL("SKEW( 1; 2 )",    Value::errorNUM());  // At least three numbers.
}

void TestStatisticalFunctions::testSLOPE()
{
    // ODF-tests
    CHECK_EVAL("SLOPE(B4:B5;C4:C5)",     Value(1));           //
    CHECK_EVAL_SHORT("SLOPE(A19:A24;A26:A31)", Value(0.007813));    // TODO - be more precise /
}

void TestStatisticalFunctions::testSMALL()
{
    // ODF-tests
    CHECK_EVAL("SMALL(B14:B16;1)", Value(1));           //
    CHECK_EVAL("SMALL(B14:B16;3)", Value(3));           //
    CHECK_EVAL("SMALL(B14:B16;4)", Value::errorNUM());  // N is greater than the length of the list
}

void TestStatisticalFunctions::testSTANDARDIZE()
{
    // ODF-tests
    CHECK_EVAL("STANDARDIZE( 1; 2.5; 0.1 )", Value(-15));         //
    CHECK_EVAL("STANDARDIZE( -1; -2; 2 )",   Value(0.5));         //
    CHECK_EVAL("STANDARDIZE( 1; 1; 0 )",     Value::errorNUM());  // N is greater than the length of the list
}

void TestStatisticalFunctions::testSTDEV()
{
    // ODF-tests
    CHECK_EVAL("STDEV(2;4)/SQRT(2)",        Value(1));               // The sample standard deviation of (2;4) is SQRT(2).
    CHECK_EVAL("STDEV(B4:B5)*SQRT(2)",      Value(1));               // The sample standard deviation of (2;3) is 1/SQRT(2).
    CHECK_EVAL("STDEV(B3:B5)*SQRT(2)",      Value(1));               // Strings are not converted to numbers and are ignored.
    CHECK_EVAL("STDEV({10000000001;10000000002;"
               "10000000003;10000000004;10000000005;"
               "10000000006;10000000007;10000000008;"
               "10000000009;10000000010})", Value(3.0276503541));    // Ensure that implementations use a reasonably stable way of calculating STDEV.
    CHECK_EVAL("STDEV(1)",                  Value::errorNUM());      // At least two numbers must be included
}

void TestStatisticalFunctions::testSTDEVA()
{
    // ODF-tests
    CHECK_EVAL("STDEVA(2;4)/SQRT(2)",      Value(1));           // The sample standard deviation of (2;4) is SQRT(2).
    CHECK_EVAL_SHORT("STDEVA(B5:C6)",            Value(2.581989));    // TODO - be more precise / Logicals (referenced) are converted to numbers.
    CHECK_EVAL_SHORT("STDEVA( TRUE();FALSE() )", Value(0.707107));    // TODO - be more precise / Logicals (inlined) are converted to numbers.
    CHECK_EVAL("STDEVA(1)",                Value::errorNUM());  // Logicals (inlined) are converted to numbers.
}

void TestStatisticalFunctions::testSTDEVP()
{
    // ODF-tests
    CHECK_EVAL("STDEVP(2;4)",     Value(1));    // The standard deviation of the set for (2;4) is 1.
    CHECK_EVAL("STDEVP(B4:B5)*2", Value(1));    // The standard deviation of the set for (2;3) is 0.5.
    CHECK_EVAL("STDEVP(B3:B5)*2", Value(1));    // Strings are not converted to numbers and are ignored.
    CHECK_EVAL("STDEVP(1)",       Value(0));    // STDEVP(1) is 0.
}

void TestStatisticalFunctions::testSTDEVPA()
{
    // ODF-tests
    CHECK_EVAL("STDEVPA(2;4)",            Value(1));           // The sample standard deviation of (2;4) is 1.
    CHECK_EVAL_SHORT("STDEVPA(B5:C6)",          Value(2.236068));    // TODO - be more precise / Logicals (referenced) are converted to numbers.
    CHECK_EVAL("STDEVPA(TRUE();FALSE())", Value(0.5));         // Logicals (inlined) are converted to numbers.
}

void TestStatisticalFunctions::testSTEYX()
{
    // ODF-tests
    CHECK_EVAL_SHORT("STEYX(C19:C23;A19:A23)", Value(2.370953));    // TODO - be more precise
    CHECK_EVAL("STEYX(A19:A23;A25:A29)", Value(0));           //
    CHECK_EVAL("STEYX(B4:B5;C4:C5)",     Value::errorNUM());  // at least three number per sequence
}

void TestStatisticalFunctions::testSUMPRODUCT()
{
    CHECK_EVAL("SUMPRODUCT(C19:C23;A19:A23)", Value(106));
    CHECK_EVAL("SUMPRODUCT(C19:C23^2;2*A19:A23)", Value(820));
}

void TestStatisticalFunctions::testTDIST()
{
    // mode
    // 1 = one tailed distribution
    // 2 = two tailed distribution

    // ODF-tests
    CHECK_EVAL("TDIST( 0.5; 1; 1 )",  Value(0.3524163823));    // ODF-specs -> 0.352416
    CHECK_EVAL_SHORT("TDIST( -1.5; 2; 2 )", Value(0.272393));    //  TODO - be more precise / OOo-2.3.0 returns error!!!
    CHECK_EVAL("TDIST( 0.5; 5; 1 )",  Value(0.3191494358));    // ODF-specs -> 0.319149
    CHECK_EVAL("TDIST( 1; 1; 3 )",    Value::errorNUM());      // mode = { 1; 2 }
    CHECK_EVAL("TDIST( 1; 0; 1 )",    Value::errorNUM());      // degreeOfFreedom >= 1
}

void TestStatisticalFunctions::testTINV()
{
    // TODO - be more precise

    // ODF-tests
    CHECK_EVAL("TINV( 1; 2 )",               Value(0));           // p=1 -> t=0
    CHECK_EVAL_SHORT("TINV( 0.5; 2 )",       Value(0.816497));    //
    CHECK_EVAL("TDIST( TINV(0.25;3); 3;2 )", Value(0.25));        //
    CHECK_EVAL("TDIST( TINV(0.5 ;3); 3;2 )", Value(0.5));         //
    CHECK_EVAL("TDIST( TINV(0.75;3); 3;2 )", Value(0.75));        //
    CHECK_EVAL("TDIST( 2; 3 )",              Value::errorNUM());  // 0 <= probability <= 1
    CHECK_EVAL("TDIST( 1; 0 )",              Value::errorNUM());  // degreeOfFreedom >= 1
}

void TestStatisticalFunctions::testTREND()
{
    //  Cell | Value      Cell | Value
    // ------+------     ------+------
    //  A19  |   1        C19  |  0
    //  A20  |   2        C20  |  5
    //  A21  |   4        C21  |  2
    //  A22  |   8        C22  |  5
    //  A23  |  16        C23  |  3

    CHECK_ARRAY("TREND(A19:A23; C19:C23; 1)     ", "{4.7555555555}");  // with    offset
    CHECK_ARRAY("TREND(A19:A23; C19:C23; 1; 0 ) ", "{1.6825396825}");  // without offset
}

void TestStatisticalFunctions::testTRIMMEAN()
{
    // ODF-tests
    CHECK_EVAL("TRIMMEAN(A19:A23; 0.8 )",      Value(4));               // cutOff = 2
    CHECK_EVAL("TRIMMEAN(A19:A23; 0.6 )",      Value(4.6666666666));    // cutOff = FLOOR(5 * 0.6/ 2) = FLOOR(1.5) = 1;
    // result = 14 / 3
    CHECK_EVAL("TRIMMEAN(A19:A23; 0.19 )",     Value(6.2));             // cutOff = 0
    CHECK_EVAL("TRIMMEAN(A19:A23; 0.999999 )", Value(4));               // cutOff = 2
    CHECK_EVAL("TRIMMEAN(A19:A23; 1)",         Value::errorNUM());      // 0 <= cutOffFraction < 1
}

void TestStatisticalFunctions::testTTEST()
{
    // ODF-tests
    CHECK_EVAL("TTEST(A19:A23;A24:A28; 1; 1 )", Value(0.0427206184));    //
    CHECK_EVAL("TTEST(A19:A23;A24:A28; 2; 1 )", Value(0.0854412368));    //
    CHECK_EVAL("TTEST(A19:A23;A24:A28; 1; 2 )", Value(0.0294544970));    //
    CHECK_EVAL("TTEST(A19:A23;A24:A28; 1; 3 )", Value(0.0462125526));    //
    CHECK_EVAL("TTEST(A19:A23;A24:A29; 1; 1 )", Value::errorNUM());      // same amount of numbers for paired samples
    CHECK_EVAL("TTEST(A19:A19;A24:A24; 1; 3 )", Value::errorNUM());      // two numbers at least for each sequence
}

void TestStatisticalFunctions::testVAR()
{
    // ODF-tests
    CHECK_EVAL("VAR(2;4)",     Value(2));           // The sample variance of (2;4) is 2.
    CHECK_EVAL("VAR(B4:B5)*2", Value(1));           // The sample variance of (2;3) is 0.5.
    CHECK_EVAL("VAR(B3:B5)*2", Value(1));           // Strings are not converted to numbers and are ignored.
    CHECK_EVAL("VAR(1)",       Value::errorNUM());  // At least two numbers must be included
}

void TestStatisticalFunctions::testVARA()
{
    // ODF-tests
    CHECK_EVAL("VARA(2;4)",            Value(2));               // The sample variance of (2;4) is 2.
    CHECK_EVAL("VARA(B5:C6)",          Value(6.6666666667));    // Logicals (referenced) are converted to numbers.
    CHECK_EVAL("VARA(TRUE();FALSE())", Value(0.5));             // Logicals (inlined) are converted to numbers.
    CHECK_EVAL("VARA(1)",              Value::errorNUM());      // Two numbers at least.
}

void TestStatisticalFunctions::testVARIANCE()
{
    // same as VAR

    // ODF-tests
    CHECK_EVAL("VARIANCE(2;4)",     Value(2));           // The sample variance of (2;4) is 2.
    CHECK_EVAL("VARIANCE(B4:B5)*2", Value(1));           // The sample variance of (2;3) is 0.5.
    CHECK_EVAL("VARIANCE(B3:B5)*2", Value(1));           // Strings are not converted to numbers and are ignored.
    CHECK_EVAL("VARIANCE(1)",       Value::errorNUM());  // At least two numbers must be included
}

void TestStatisticalFunctions::testVARP()
{
    //  Cell | Value
    // ------+-------
    //   B3  |  "7"
    //   B4  |   2
    //   B5  |   3

    // ODF-tests
    CHECK_EVAL("VARP(2;4)",     Value(1));    // The variance of the set for (2;4) is 1.
    CHECK_EVAL("VARP(B4:B5)*4", Value(1));    // The variance of the set for (2;3) is 0.25.
    CHECK_EVAL("VARP(B3:B5)*4", Value(1));    // Strings are not converted to numbers and are ignored.
}

void TestStatisticalFunctions::testVARPA()
{
    //  Cell | Value      Cell | Value
    // ------+------     ------+------
    //   B5  |   3         C5  |  5
    //   B6  | true        C6  |  7

    // ODF-tests
    CHECK_EVAL("VARPA(2;4)",            Value(1));       // The sample variance of (2;4) is 1.
    CHECK_EVAL("VARPA(B5:C6)",          Value(5));       // Logicals (referenced) are converted to numbers.
    CHECK_EVAL("VARPA(TRUE();FALSE())", Value(0.25));    // Logicals (inlined) are converted to numbers.
}

void TestStatisticalFunctions::testWEIBULL()
{
    // TODO - be more precise

    // ODF-tests
    CHECK_EVAL_SHORT("WEIBULL(  2; 3; 4; 0 )", Value(0.165468));    // pdf
    CHECK_EVAL_SHORT("WEIBULL(  2; 3; 4; 1 )", Value(0.117503));    // cdf
    CHECK_EVAL_SHORT("WEIBULL( -1; 3; 4; 0 )", Value::errorNUM());  // value >= 0
    CHECK_EVAL_SHORT("WEIBULL(  2; 0; 4; 0 )", Value::errorNUM());  // alpha > 0
    CHECK_EVAL_SHORT("WEIBULL(  2; 3; 0; 0 )", Value::errorNUM());  // beta > 0
}

void TestStatisticalFunctions::testZTEST()
{
    // ODF-tests
    CHECK_EVAL("ZTEST(B4:C5; 3.5      )", Value(0));               // mean = average, estimated standard deviation: fits well
    CHECK_EVAL("ZTEST(B4:C5; 3  ; 2   )", Value(0.3829249225));    // mean near average, standard deviation greater than estimate: probable
    CHECK_EVAL("ZTEST(B4:C5; 4  ; 0.5 )", Value(0.9544997361));    // mean near the average, but small deviation: not probable
    CHECK_EVAL("ZTEST(B4:C5; 5        )", Value(0.9798632484));    // mean at a border value, standard deviation ~ 1,3: nearly improbable
    CHECK_EVAL("ZTEST(B4:C5; 5  ; 0.1 )", Value(1));               // mean at a border value, small standard deviation: improbable
}

void TestStatisticalFunctions::cleanupTestCase()
{
    delete m_map;
}

QTEST_KDEMAIN(TestStatisticalFunctions, GUI)

#include "TestStatisticalFunctions.moc"
