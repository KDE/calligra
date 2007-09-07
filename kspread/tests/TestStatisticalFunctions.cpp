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
#include <Doc.h>
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
#define ROUND(x) (roundf(1e15 * x) / 1e15)

Value TestStatisticalFunctions::TestDouble(const QString& formula, const Value& v2, int accuracy)
{
  double epsilon = DBL_EPSILON*pow(10.0,(double)(accuracy));

  Formula f(m_doc->map()->sheet(0)); // bind to test case data set 
  QString expr = formula;
  if ( expr[0] != '=' )
    expr.prepend( '=' );
  f.setExpression( expr );
  Value result = f.eval();

  bool res = fabs(v2.asFloat()-result.asFloat())<epsilon;

  if (!res)
    kDebug(36002)<<"check failed -->" <<"Epsilon =" << epsilon <<"" << v2.asFloat() <<" to" << result.asFloat() <<"  diff =" << v2.asFloat()-result.asFloat();
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
  if(v.isNumber())
  {
    double d = numToDouble(v.asFloat());
    if(fabs(d) < DBL_EPSILON)
      d = 0.0;
    return Value( ROUND(d) );
  }
  else
    return v;
}

Value TestStatisticalFunctions::evaluate(const QString& formula)
{
  Formula f(m_doc->map()->sheet(0));
  QString expr = formula;
  if ( expr[0] != '=' )
    expr.prepend( '=' );
  f.setExpression( expr );
  Value result = f.eval();

#if 0
  // this magically generates the CHECKs
  printf("  CHECK_EVAL( \"%s\",  %15g) );\n", qPrintable(formula), result.asFloat());
#endif

  return RoundNumber(result);
}

void TestStatisticalFunctions::initTestCase()
{
    m_doc = new Doc();
    m_doc->map()->addNewSheet();
    Sheet* sheet = m_doc->map()->sheet(0);
    CellStorage* storage = sheet->cellStorage();

    // Test case data set

    // A19:A29
    storage->setValue(1,19, Value(    1 ) );
    storage->setValue(1,20, Value(    2 ) );
    storage->setValue(1,21, Value(    4 ) );
    storage->setValue(1,22, Value(    8 ) );
    storage->setValue(1,23, Value(   16 ) );
    storage->setValue(1,24, Value(   32 ) );
    storage->setValue(1,25, Value(   64 ) );
    storage->setValue(1,26, Value(  128 ) );
    storage->setValue(1,27, Value(  256 ) );
    storage->setValue(1,28, Value(  512 ) );
    storage->setValue(1,29, Value( 1024 ) );
    storage->setValue(1,30, Value( 2048 ) );
    storage->setValue(1,31, Value( 4096 ) );

    // B3:B17
    storage->setValue(2, 3, Value(     "7"   ) );
    storage->setValue(2, 4, Value(      2    ) );
    storage->setValue(2, 5, Value(      3    ) );
    storage->setValue(2, 6, Value(    true   ) );
    storage->setValue(2, 7, Value(   "Hello" ) );
    // B8 leave empty
    storage->setValue(2, 9, Value::errorDIV0() );
    storage->setValue(2,10, Value(      0    ) );
    storage->setValue(2,11, Value(      3    ) );
    storage->setValue(2,12, Value(      4    ) );
    storage->setValue(2,13, Value( "2005-0131T01:00:00" ));
    storage->setValue(2,14, Value(      1    ) );
    storage->setValue(2,15, Value(      2    ) );
    storage->setValue(2,16, Value(      3    ) );
    storage->setValue(2,17, Value(      4    ) );

    // C4:C6
    storage->setValue(3, 4, Value( 4 ) );
    storage->setValue(3, 5, Value( 5 ) );
    storage->setValue(3, 6, Value( 7 ) );

    // C11:C17
    storage->setValue(3,11, Value( 5 ) );
    storage->setValue(3,12, Value( 6 ) );
    storage->setValue(3,13, Value( 8 ) );
    storage->setValue(3,14, Value( 4 ) );
    storage->setValue(3,15, Value( 3 ) );
    storage->setValue(3,16, Value( 2 ) );
    storage->setValue(3,17, Value( 1 ) );

    // C19:25
    storage->setValue(3,19, Value( 0 ) );
    storage->setValue(3,20, Value( 5 ) );
    storage->setValue(3,21, Value( 2 ) );
    storage->setValue(3,22, Value( 5 ) );
    storage->setValue(3,23, Value( 3 ) );
    storage->setValue(3,24, Value( 4 ) );
    storage->setValue(3,25, Value( 4 ) );

    // F51:F60
    storage->setValue(6,51, Value( 3 ) );
    storage->setValue(6,52, Value( 4 ) );
    storage->setValue(6,53, Value( 5 ) );
    storage->setValue(6,54, Value( 2 ) );
    storage->setValue(6,55, Value( 3 ) );
    storage->setValue(6,56, Value( 4 ) );
    storage->setValue(6,57, Value( 5 ) );
    storage->setValue(6,58, Value( 6 ) );
    storage->setValue(6,59, Value( 4 ) );
    storage->setValue(6,60, Value( 7 ) );

    // G51:G60
    storage->setValue(7,51, Value( 23 ) );
    storage->setValue(7,52, Value( 24 ) );
    storage->setValue(7,53, Value( 25 ) );
    storage->setValue(7,54, Value( 22 ) );
    storage->setValue(7,55, Value( 23 ) );
    storage->setValue(7,56, Value( 24 ) );
    storage->setValue(7,57, Value( 25 ) );
    storage->setValue(7,58, Value( 26 ) );
    storage->setValue(7,59, Value( 24 ) );
    storage->setValue(7,60, Value( 27 ) );
}

void TestStatisticalFunctions::testAVERAGEA()
{
    // ODF-tests
    CHECK_EVAL("AVERAGEA(2; 4)",               Value( 3 ) ); //
    CHECK_EVAL("AVERAGEA(TRUE(); FALSE(); 5)", Value( 2 ) ); //
}

void TestStatisticalFunctions::testBETADIST()
{
    // ODF-tests
    CHECK_EVAL("BETADIST(0;3;4)",               Value(        0 ) ); //
    CHECK_EVAL("BETADIST(0.5;3;4)",             Value( 0.656250 ) ); //
    CHECK_EVAL("BETADIST(0.9;4;3)",             Value( 0.984150 ) ); //
    CHECK_EVAL("BETADIST(1.5;3;4;1;2)",         Value( 0.656250 ) ); //
    CHECK_EVAL("BETADIST(2;3;4;1;3)",           Value( 0.656250 ) ); //
    CHECK_EVAL("BETADIST(0;3;4;0;1;FALSE())",   Value(        0 ) ); // TODO check BOOL
    CHECK_EVAL("BETADIST(0.5;3;4;0;1;FALSE())", Value( 0.000521 ) ); //
    CHECK_EVAL("BETADIST(0.9;4;3;0;1;FALSE())", Value( 0.000122 ) ); //
    CHECK_EVAL("BETADIST(1.5;3;4;1;2;FALSE())", Value( 0.000521 ) ); //
    CHECK_EVAL("BETADIST(2;3;4;1;3;FALSE())",   Value( 0.000521 ) ); //
    CHECK_EVAL("BETADIST(2;3;4)",               Value(        1 ) ); //
    CHECK_EVAL("BETADIST(-1;3;4)",              Value(        0 ) ); //
    CHECK_EVAL("BETADIST(2;3;4;0;1;FALSE())",   Value(        0 ) ); //
    CHECK_EVAL("BETADIST(-1;3;4;0;1;FALSE())",  Value(        0 ) ); //
}

void TestStatisticalFunctions::testCONFIDENCE()
{
    // ODF-tests
    CHECK_EVAL("CONFIDENCE(0.5 ; 1;1)", Value( 0.67448975   ) ); //
    CHECK_EVAL("CONFIDENCE(0.25; 1;1)", Value( 1.1503493804 ) ); //
    CHECK_EVAL("CONFIDENCE(0.5 ; 4;1)", Value( 2.6979590008 ) ); // Multiplying stddev by X multiplies result by X.
    CHECK_EVAL("CONFIDENCE(0.5 ; 1;4)", Value( 0.3372448751 ) ); // Multiplying count by X*X divides result by X.
    
    // check constraints
    CHECK_EVAL("CONFIDENCE(-0.5; 1;4)", Value::errorNUM()   ); // 0 < alpha < 1
    CHECK_EVAL("CONFIDENCE( 1.5; 1;4)", Value::errorNUM()   ); // 0 < alpha < 1
    CHECK_EVAL("CONFIDENCE( 0.5;-1;4)", Value::errorNUM()   ); // stddev > 0
    CHECK_EVAL("CONFIDENCE( 0.5; 1;0)", Value::errorNUM()   ); // size >= 1
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
    CHECK_EVAL("CORREL(B14:B17;B14:B17)", Value(            1 ) ); // Perfect positive correlation given identical sequences
    CHECK_EVAL("CORREL(B14:B17;C14:C17)", Value(           -1 ) ); // Perfect negative correlation given reversed sequences
    CHECK_EVAL("CORREL(1;2)",             Value::errorNUM()     ); // Each list must contain at least 2 values
    CHECK_EVAL("CORREL(B14:B16;B15:B16)", Value::errorNUM()     ); // The length of each list must be equal
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
    CHECK_EVAL("COVAR(C11:C17;C11:C17)", Value(  4.9795918367 ) ); // 
    CHECK_EVAL("COVAR(B14:B17;C14:C17)", Value( -1.25         ) ); // 
    CHECK_EVAL("COVAR(B14:B17;C13:C17)", Value::errorNUM()      ); // TODO should we check for "array sizes don't match" or "value counts" in array?.
}

void TestStatisticalFunctions::testDEVSQ()
{
    // ODF-tests
    CHECK_EVAL("DEVSQ(4)",         Value(             0 ) ); // One value - no deviation.
    CHECK_EVAL("DEVSQ(5;5;5;5)",   Value(             0 ) ); // Identical values - no deviation.
    CHECK_EVAL("DEVSQ(2;4)",       Value(             2 ) ); // Each value deviates by 1.
    CHECK_EVAL("DEVSQ(-5;5;-1;1)", Value(            52 ) ); // Average=0 must work properly.
    CHECK_EVAL("DEVSQ(C11:C17)",   Value( 34.8571428571 ) ); // Test values.
    CHECK_EVAL("DEVSQ(B14:B17)",   Value(  5.00         ) ); // Test values.
    CHECK_EVAL("DEVSQ(B14)",       Value(             0 ) ); // One value - no deviation.
}

// void TestStatisticalFunctions::testDEVSQA()
// {
//     // no test available
// }

void TestStatisticalFunctions::testEXPONDIST()
{
    // TODO - be more precise

    // ODF-tests
    CHECK_EVAL_SHORT("EXPONDIST(1;1;TRUE())",   Value( 0.632121 ) ); //
    CHECK_EVAL_SHORT("EXPONDIST(2;2;TRUE())",   Value( 0.981684 ) ); //
    CHECK_EVAL_SHORT("EXPONDIST(0;1;TRUE())",   Value(        0 ) ); // 
    CHECK_EVAL_SHORT("EXPONDIST(-1;1;TRUE())",  Value(        0 ) ); // 
    CHECK_EVAL_SHORT("EXPONDIST(1;1;FALSE())",  Value( 0.367879 ) ); //
    CHECK_EVAL_SHORT("EXPONDIST(2;2;FALSE())",  Value( 0.036631 ) ); //
    CHECK_EVAL_SHORT("EXPONDIST(0;1;FALSE())",  Value(        1 ) ); //
    CHECK_EVAL_SHORT("EXPONDIST(-1;1;FALSE())", Value(        0 ) ); //
//     CHECK_EVAL_SHORT("EXPONDIST(1;1)",          Value( 0.632121 ) ); // TODO check logical[=true]? - =EXPONDIST(1;1;TRUE())
}

void TestStatisticalFunctions::testFDIST()
{
    // ODF-tests
    CHECK_EVAL_SHORT("FDIST(1;4;5)",          Value( 0.514343 ) ); //
    CHECK_EVAL_SHORT("FDIST(2;5;4)",          Value( 0.739202 ) ); //
    CHECK_EVAL_SHORT("FDIST(0;4;5)",          Value(        0 ) ); // 
    CHECK_EVAL_SHORT("FDIST(-1;4;5)",         Value(        0 ) ); // 
    CHECK_EVAL_SHORT("FDIST(1;4;5;FALSE())",  Value( 0.397614 ) ); //
    CHECK_EVAL_SHORT("FDIST(2;5;4;FALSE())",  Value( 0.154000 ) ); //
    CHECK_EVAL_SHORT("FDIST(0;4;5;FALSE())",  Value(        0 ) ); //
    CHECK_EVAL_SHORT("FDIST(-1;4;5;FALSE())", Value(        0 ) ); //
    CHECK_EVAL_SHORT("FDIST(1;4;5;TRUE())",   Value( 0.514343 ) ); // =FDIST(1;4;5)
}

void TestStatisticalFunctions::testLEGACYFDIST()
{
    // ODF-tests
    CHECK_EVAL_SHORT("LEGACYFDIST(1;4;5)",          Value( 0.485657 ) ); //
    CHECK_EVAL_SHORT("LEGACYFDIST(2;5;4)",          Value( 0.260798 ) ); //
    CHECK_EVAL_SHORT("LEGACYFDIST(0;4;5)",          Value(        1 ) ); // 
    CHECK_EVAL_SHORT("LEGACYFDIST(-1;4;5)",         Value::errorNUM() ); // 
}

void TestStatisticalFunctions::testFISHER()
{
    // ODF-tests
    CHECK_EVAL("FISHER(0)",                        Value(          0 ) ); // Fisher of 0.
    CHECK_EVAL("FISHER((EXP(1)-1)/(EXP(1)+1))",    Value( 0.5        ) ); // Argument chosen so that ln=1
    CHECK_EVAL_SHORT("FISHER(0.5)",                Value( 0.54930614 ) ); // TODO - be more precise - Some random value.
    CHECK_EVAL("FISHER(0.47)+FISHER(-0.47)",       Value(          0 ) ); // Function is symetrical.
}

void TestStatisticalFunctions::testFISHERINV()
{
    // ODF-tests
    CHECK_EVAL("FISHERINV(0)",                     Value(          0 ) ); // Fisherinv of 0.
    CHECK_EVAL("FISHERINV(LN(2))",                 Value( 0.6        ) ); // e^(2*ln(2))=4
    CHECK_EVAL("FISHERINV(FISHER(0.5))",           Value( 0.5        ) ); // Some random value.
    CHECK_EVAL("FISHERINV(0.47)+FISHERINV(-0.47)", Value(          0 ) ); // Function is symetrical.
}

void TestStatisticalFunctions::testFREQUENCY()
{
    Value result( Value::Array );
    result.setElement( 0, 0, Value( 3 ) );
    result.setElement( 0, 1, Value( 2 ) );
    result.setElement( 0, 2, Value( 4 ) );
    result.setElement( 0, 3, Value( 1 ) );
    CHECK_EVAL( "FREQUENCY({1;2;3;4;5;6;7;8;9;10};{3|5|9})", result );
    // the second arg has to be a column vector
    CHECK_EVAL( "ISERROR(FREQUENCY({1;2;3;4;5;6;7;8;9;10};{3;5;9}))", Value(true) );
    // an empty second arg returns the overall number count
    CHECK_EVAL( "FREQUENCY({1;2;3;4;5;6;7;8;9;10};)", Value( 10 ) );
}

// void TestStatisticalFunctions::testFTEST()
// {
//     // ODF-tests
//     CHECK_EVAL("FTEST(B14:B17; C14:C17)", Value(           1 ) ); // Same data (second reversed),
//     CHECK_EVAL("FTEST(B14:B15; C13:C14)", Value( 0.311916521 ) ); // Significantly different variances,
//                                                                   // so less likely to come from same data set.
// }

void TestStatisticalFunctions::testGAMMADIST()
{
    // ODF-tests
    CHECK_EVAL("GAMMADIST(0  ;3;4)",         Value(        0 ) );
    CHECK_EVAL("GAMMADIST(0.5;3;4)",         Value( 0.001724 ) ); // TODO res=Value::error()
    CHECK_EVAL("GAMMADIST(9  ;4;3)",         Value( 0.066698 ) );
    CHECK_EVAL("GAMMADIST(0  ;3;4;FALSE())", Value(        0 ) );
    CHECK_EVAL("GAMMADIST(0.5;3;4;FALSE())", Value( 0.000296 ) );
    CHECK_EVAL("GAMMADIST(9  ;4;3;FALSE())", Value( 0.390661 ) );
    CHECK_EVAL("GAMMADIST(0  ;3;4;TRUE())",  evaluate( "GAMMADIST(  0;3;4)" ) );
    CHECK_EVAL("GAMMADIST(0.5;3;4;TRUE())",  evaluate( "GAMMADIST(0.5;3;4)" ) );
    CHECK_EVAL("GAMMADIST(9  ;4;3;TRUE())",  evaluate( "GAMMADIST(  9;4;3)" ) );
    CHECK_EVAL("GAMMADIST(-1 ;4;3;TRUE())",  Value( 0 ));
    CHECK_EVAL("GAMMADIST(-1 ;3;4;FALSE())", Value( 0 ));
}

void TestStatisticalFunctions::testGAUSS()
{
    // ODF-tests
    CHECK_EVAL("GAUSS(0)",     Value(            0  ) ); // Mean of one value.
    CHECK_EVAL("GAUSS(1)",     Value(  0.341344746  ) ); // Multiple equivalent values.
    // my test
    CHECK_EVAL("GAUSS(-0.25)", Value( -0.0987063257 ) ); // check neg. values. test for fixes gauss_func
}

void TestStatisticalFunctions::testGEOMEAN()
{
    // ODF-tests
    CHECK_EVAL("GEOMEAN(7)",           Value(            7 ) ); // Mean of one value.
    CHECK_EVAL("GEOMEAN(5;5;5;5)",     Value(            5 ) ); // Multiple equivalent values.
    CHECK_EVAL("GEOMEAN(2;8;2;8)",     Value(            4 ) ); // Some values.
    CHECK_EVAL("GEOMEAN(8;0;8;8;8;8)", Value::errorNUM()     ); // Error if there is a 0 in the range.
    CHECK_EVAL("GEOMEAN(C11)",         Value(            5 ) ); // One value, range.
    CHECK_EVAL("GEOMEAN(C11:C17)",     Value( 3.4451109418 ) ); // Some values, range.
    CHECK_EVAL("GEOMEAN(B14:B17)",     Value( 2.2133638394 ) ); // Some values, range.
}

void TestStatisticalFunctions::testHARMEAN()
{
    // ODF-tests
    CHECK_EVAL("HARMEAN(7)",           Value(            7 ) ); // Mean of one value.
    CHECK_EVAL("HARMEAN(4;4;4;4)",     Value(            4 ) ); // Multiple equivalent values.
    CHECK_EVAL("HARMEAN(2;4;4)",       Value(            3 ) ); // Some values.
    CHECK_EVAL("HARMEAN(8;0;8;8;8;8)", Value::errorNUM()     ); // Error if there is a 0 in the range.
    CHECK_EVAL("HARMEAN(C11)",         Value(            5 ) ); // One value, range.
    CHECK_EVAL("HARMEAN(C11:C17)",     Value( 2.7184466019 ) ); // Some values, range.
    CHECK_EVAL("HARMEAN(B14:B17)",     Value( 1.92         ) ); // Some values, range.
}

void TestStatisticalFunctions::testHYPGEOMDIST()
{
    // ODF-tests 
    CHECK_EVAL("HYPGEOMDIST( 2  ;3;3;6;FALSE())", Value(     0.45 ) ); // If an urn contains 3 red balls and 3 green balls, the probability 
                                                                       // that 2 red balls will be selected after 3 selections without replacement. 
                                                                       // (0.45=27/60).
    CHECK_EVAL("HYPGEOMDIST( 2  ;3;3;6)",         Value(     0.45 ) ); // The default for cumulative is FALSE().
    CHECK_EVAL("HYPGEOMDIST( 0  ;3;3;6)",         Value(     0.05 ) ); // There is a small (5%) chance of selecting only green balls.
    CHECK_EVAL("HYPGEOMDIST( 2  ;3;3;6;TRUE())",  Value(     0.95 ) ); // The probability of selecting at most two red balls (i.e 0, 1 or 2).
    CHECK_EVAL("HYPGEOMDIST( 4  ;3;3;6)",         Value::errorNUM() ); // X must be <= M
    CHECK_EVAL("HYPGEOMDIST( 2.8;3;3;6)",         Value(     0.45 ) ); // Non-integers are truncated.
    CHECK_EVAL("HYPGEOMDIST(-2  ;3;3;6)",         Value::errorNUM() ); // Values must be >= 0.
    CHECK_EVAL("HYPGEOMDIST( 0  ;0;0;0)",         Value(     1    ) ); //
}

void TestStatisticalFunctions::testKURT()
{
    // TODO check function 

    // ODF-tests
    CHECK_EVAL("KURT(C20:C25)",     Value( -0.446162998 ) ); //
    CHECK_EVAL("KURT(C20:C23;4;4)", Value( -0.446162998 ) ); //
}

void TestStatisticalFunctions::testLARGE()
{
    //  Cell | Value | N'th
    // ------+-------+------
    //   B14 |   1   |   3
    //   B15 |   2   |   2
    //   B16 |   3   |   1

    // ODF-tests
    CHECK_EVAL("LARGE(B14:B16;1)", Value( 3 ) );        //
    CHECK_EVAL("LARGE(B14:B16;3)", Value( 1 ) );        //
    CHECK_EVAL("LARGE(B14:B16;4)", Value::errorNUM() ); // N is greater than the length of the list
}

void TestStatisticalFunctions::testLOGINV()
{
    // TODO check function 

    // ODF-tests
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.1;0;1);0;1;TRUE())", Value( 0.1 ) ); //
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.3;0;1);0;1;TRUE())", Value( 0.3 ) ); //
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.5;0;1);0;1;TRUE())", Value( 0.5 ) ); //
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.7;0;1);0;1;TRUE())", Value( 0.7 ) ); //
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.9;0;1);0;1;TRUE())", Value( 0.9 ) ); //
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.1;1;4);1;4;TRUE())", Value( 0.1 ) ); //
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.3;1;4);1;4;TRUE())", Value( 0.3 ) ); //
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.5;1;4);1;4;TRUE())", Value( 0.5 ) ); //
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.7;1;4);1;4;TRUE())", Value( 0.7 ) ); //
    CHECK_EVAL("LOGNORMDIST(LOGINV(0.9;1;4);1;4;TRUE())", Value( 0.9 ) ); //
    CHECK_EVAL("LOGINV(0.5)",                             Value( 1   ) ); //
}

void TestStatisticalFunctions::testLOGNORMDIST()
{
    // TODO - implement cumulative calculation

    // ODF-tests
    CHECK_EVAL("LOGNORMDIST(1)",              Value( 0.5      ) ); //
    CHECK_EVAL("LOGNORMDIST(1;1;4)",          Value( 0.401294 ) ); //
    CHECK_EVAL("LOGNORMDIST(1;0;1;FALSE())",  Value( 0.398942 ) ); //
    CHECK_EVAL("LOGNORMDIST(1;0;1;TRUE())",   Value( 0.5      ) ); //
    CHECK_EVAL("LOGNORMDIST(1;1;4;FALSE())",  Value( 0.096667 ) ); //
    CHECK_EVAL("LOGNORMDIST(1;1;4;TRUE())",   Value( 0.401294 ) ); //
    CHECK_EVAL("LOGNORMDIST(1;-1;4;FALSE())", Value( 0.096667 ) ); //
    CHECK_EVAL("LOGNORMDIST(1;-1;4;TRUE())",  Value( 0.598706 ) ); //
    CHECK_EVAL("LOGNORMDIST(2;-1;4;FALSE())", Value( 0.045595 ) ); //
    CHECK_EVAL("LOGNORMDIST(2;-1;4;TRUE())",  Value( 0.663957 ) ); //
    CHECK_EVAL("LOGNORMDIST(3;0;1;TRUE())",   Value( 0.864031 ) ); //
    CHECK_EVAL("LOGNORMDIST(100;0;1;TRUE())", Value( 0.999998 ) ); //
    CHECK_EVAL("LOGNORMDIST(-1;0;1;FALSE())", Value::errorNUM() ); // constraint failure
    CHECK_EVAL("LOGNORMDIST(-1;0;1;TRUE())",  Value( 0        ) ); //
    CHECK_EVAL("LOGNORMDIST(1;0;-1;FALSE())", Value( 0.864031 ) ); // constraint failure
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
    CHECK_EVAL("MAX(2;4;1;-8)", Value(          4 ) ); // Negative numbers are smaller than positive numbers.
    CHECK_EVAL("MAX(B4:B5)",    Value(          3 ) ); // The maximum of (2,3) is 3.
//     CHECK_EVAL("ISNA(MAXA(NA())", Value(true)); // nline errors are propagated.
    CHECK_EVAL("MAX(B3:B5)",    Value(          3 ) ); // Strings are not converted to numbers and are ignored.
    CHECK_EVAL("MAX(-1;B7)",    Value(         -1 ) ); // Strings are not converted to numbers and are ignored.
    CHECK_EVAL("MAX(B3:B9)",    Value::errorVALUE() ); // TODO check function - Errors inside ranges are NOT ignored.
}

void TestStatisticalFunctions::testMAXA()
{
    // ODF-tests
    CHECK_EVAL("MAXA(2;4;1;-8)", Value(          4 ) ); // Negative numbers are smaller than positive numbers.
    CHECK_EVAL("MAXA(B4:B5)",    Value(          3 ) ); // The maximum of (2,3) is 3.
//     CHECK_EVAL("ISNA(MAXA(NA())", Value(true)); // Inline errors are propagated.

// TODO check function - inline Text must be converted, but not Text in Cells
//     CHECK_EVAL("MAXA(B3:B5)",    Value(          3 ) ); // Cell text is converted to 0.

    CHECK_EVAL("MAXA(-1;B7)",    Value(          0 ) ); // Cell text is converted to 0.
    CHECK_EVAL("MAXA(\"a\")",    Value::errorVALUE() ); // Text inline is NOT ignored.
    CHECK_EVAL("MAXA(B3:B9)",    Value::errorVALUE() ); // TODO check function - Errors inside ranges are NOT ignored.
    CHECK_EVAL("MAXA(B6:B7)",    Value(          1 ) ); // Logicals are considered numbers.
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
    CHECK_EVAL("MIN(2;4;1;-8)", Value(       -8 ) ); // Negative numbers are smaller than positive numbers.
    CHECK_EVAL("MIN(B4:B5)",    Value(        2 ) ); // The minimum of (2,3) is 2.
    CHECK_EVAL("MIN(B3)",       Value(        0 ) ); // If no numbers are provided in all ranges, MIN returns 0
    CHECK_EVAL("MIN(\"a\")",    Value::errorNUM() ); // Non-numbers inline are NOT ignored.
    CHECK_EVAL("MIN(B3:B5)",    Value(        2 ) ); // Cell text is not converted to numbers and is ignored.
}

void TestStatisticalFunctions::testMINA()
{
    // ODF-tests
    CHECK_EVAL("MINA(2;4;1;-8)", Value(       -8 ) ); // Negative numbers are smaller than positive numbers.
    CHECK_EVAL("MINA(B4:B5)",    Value(        2 ) ); // The minimum of (2,3) is 2.
    CHECK_EVAL("MINA(1;B7)",     Value(        0 ) ); // Cell text is converted to 0.
    CHECK_EVAL("MINA(\"a\")",    Value::errorNUM() ); // Cell text inline is NOT ignored.

// TODO check function - inline Text must be converted, but not Text in Cells
//     CHECK_EVAL("MINA(B3:B5)",    Value(        0 ) ); // Cell text is converted to 0.

    CHECK_EVAL("MINA(B6:C6)",    Value(        1 ) ); // The value "True" is considered equivalent to 1.
}

void TestStatisticalFunctions::testMODE()
{
    // ODF-tests
    CHECK_EVAL("MODE(F51:F60)",                                 Value(        4 ) ); //
    CHECK_EVAL("MODE(G51;G52;G53;G54;G55;G56;G57;G58;G59;G60)", Value(       24 ) ); //
    CHECK_EVAL("MODE(1;2;3;4;5;6;7;8;9;10)",                    Value::errorNUM() ); //
}

void TestStatisticalFunctions::testNEGBINOMDIST()
{
    // ODF-test
//     CHECK_EVAL("NEGBINOMDIST(F20;I29;H6)", Value( 0.000130947 ) ); //

    // bettersolutions.com
    CHECK_EVAL("NEGBINOMDIST( 0;1; 0.25)", Value( 0.25         ) ); //
    CHECK_EVAL("NEGBINOMDIST( 0;1; 0.5)",  Value( 0.5          ) ); //
    CHECK_EVAL("NEGBINOMDIST( 1;6; 0.5)",  Value( 0.046875     ) ); //
    CHECK_EVAL("NEGBINOMDIST(10;5; 0.25)", Value( 0.0550486604 ) ); //
    CHECK_EVAL("NEGBINOMDIST(10;5;-4)",    Value::errorNUM()     ); //
//     CHECK_EVAL("NEGBINOMDIST(10;"text";0.25)", Value::NUM() ); //
}

void TestStatisticalFunctions::testNORMDIST()
{
    // ODF-tests
    CHECK_EVAL("NORMDIST(0;1;4;TRUE())",         Value( 0.4012936743 ) ); //
    CHECK_EVAL("NORMDIST(0;0;1;FALSE())",        Value( 0.3989422804 ) ); //
    CHECK_EVAL("NORMDIST(0;0;1;TRUE())",         Value( 0.5          ) ); //
    CHECK_EVAL("NORMDIST(0;1;4;FALSE())",        Value( 0.0966670292 ) ); //
    CHECK_EVAL("NORMDIST(0;-1;4;FALSE())",       Value( 0.0966670292 ) ); //
    CHECK_EVAL("NORMDIST(0;-1;4;TRUE())",        Value( 0.5987063257 ) ); //
    CHECK_EVAL("NORMDIST(1;-1;4;FALSE())",       Value( 0.0880163317 ) ); //
    CHECK_EVAL("NORMDIST(1;-1;4;TRUE())",        Value( 0.6914624613 ) ); //
    CHECK_EVAL("NORMDIST(1.281552;0;1;TRUE())",  Value( 0.9000000762 ) ); //
    CHECK_EVAL("NORMDIST(0;-1.281552;1;TRUE())", Value( 0.9000000762 ) ); //
    CHECK_EVAL("NORMDIST(0;0;-1;FALSE())",       Value::errorNUM()     ); //
}

void TestStatisticalFunctions::testNORMINV()
{
    // ODF-tests
    CHECK_EVAL("NORMDIST(NORMINV(0.1;0;1);0;1;TRUE())", Value( 0.1 ) ); //
    CHECK_EVAL("NORMDIST(NORMINV(0.3;0;1);0;1;TRUE())", Value( 0.3 ) ); //
    CHECK_EVAL("NORMDIST(NORMINV(0.5;0;1);0;1;TRUE())", Value( 0.5 ) ); //
    CHECK_EVAL("NORMDIST(NORMINV(0.7;0;1);0;1;TRUE())", Value( 0.7 ) ); //
    CHECK_EVAL("NORMDIST(NORMINV(0.9;0;1);0;1;TRUE())", Value( 0.9 ) ); //
    CHECK_EVAL("NORMDIST(NORMINV(0.1;1;4);1;4;TRUE())", Value( 0.1 ) ); //
    CHECK_EVAL("NORMDIST(NORMINV(0.3;1;4);1;4;TRUE())", Value( 0.3 ) ); //
    CHECK_EVAL("NORMDIST(NORMINV(0.5;1;4);1;4;TRUE())", Value( 0.5 ) ); //
    CHECK_EVAL("NORMDIST(NORMINV(0.7;1;4);1;4;TRUE())", Value( 0.7 ) ); //
    CHECK_EVAL("NORMDIST(NORMINV(0.9;1;4);1;4;TRUE())", Value( 0.9 ) ); //
}

void TestStatisticalFunctions::testPERMUT()
{
    // ODF-tests
    CHECK_EVAL("PERMUT(2;2)",     Value(      0.1 ) ); //
    CHECK_EVAL("PERMUT(4;2)",     Value(      0.3 ) ); //
    CHECK_EVAL("PERMUT(4.3;2.1)", Value(      0.3 ) ); // =PERMUT(4;2)
    CHECK_EVAL("PERMUT(-4;2)",    Value::errorNUM() ); //
    CHECK_EVAL("PERMUT(4;-2)",    Value::errorNUM() ); //
}

void TestStatisticalFunctions::testPHI()
{
    //  Cell | Value
    // ------+-------
    //   C23 |   3   
    //       |   

    // ODF-tests
    CHECK_EVAL_SHORT("PHI(C23/10)",  Value( 0.381387815 ) ); // TODO - be more precise /
    CHECK_EVAL_SHORT("PHI(-C23/10)", Value( 0.381387815 ) ); // TODO - be more precise /
    CHECK_EVAL_SHORT("PHI(0)",       Value( 0.398942280 ) ); // TODO - be more precise /
}

void TestStatisticalFunctions::testPOISSON()
{
    // ODF-tests
    CHECK_EVAL_SHORT("POISSON(0;1;FALSE())", Value( 0.367880 ) ); // TODO - be more precise /
    CHECK_EVAL_SHORT("POISSON(0;2;FALSE())", Value( 0.135335 ) ); // TODO - be more precise /
}

void TestStatisticalFunctions::testSKEW()
{
    // ODF-tests
    CHECK_EVAL_SHORT("SKEW( 1; 2; 4 )", Value( 0.935219 ) ); // TODO - be more precise / Expectation value: 2.333333
                                                       // Standard deviation: 1.257525
                                                       // Third central moment: 0.740741
    CHECK_EVAL_SHORT("SKEW(A19:A23)",   Value( 1.325315 ) ); // TODO - be more precise /
    CHECK_EVAL("SKEW( 1; 2 )",    Value::errorNUM() ); // At least three numbers.
}

void TestStatisticalFunctions::testSLOPE()
{
    // ODF-tests
    CHECK_EVAL("SLOPE(B4:B5;C4:C5)",     Value( 1        ) ); // 
    CHECK_EVAL_SHORT("SLOPE(A19:A24;A26:A31)", Value( 0.007813 ) ); // TODO - be more precise /
}

void TestStatisticalFunctions::testSMALL()
{
    // ODF-tests
    CHECK_EVAL("SMALL(B14:B16;1)", Value(        1 ) ); // 
    CHECK_EVAL("SMALL(B14:B16;3)", Value(        3 ) ); //
    CHECK_EVAL("SMALL(B14:B16;4)", Value::errorNUM() ); // N is greater than the length of the list
}

void TestStatisticalFunctions::testSTANDARDIZE()
{
    // ODF-tests
    CHECK_EVAL("STANDARDIZE( 1; 2.5; 0.1 )", Value(      -15 ) ); // 
    CHECK_EVAL("STANDARDIZE( -1; -2; 2 )",   Value(      0.5 ) ); //
    CHECK_EVAL("STANDARDIZE( 1; 1; 0 )",     Value::errorNUM() ); // N is greater than the length of the list
}

void TestStatisticalFunctions::testSTDEV()
{
    // ODF-tests
    CHECK_EVAL("STDEV(2;4)/SQRT(2)",        Value(       1 ) ); // The sample standard deviation of (2;4) is SQRT(2).
    CHECK_EVAL("STDEV(B4:B5)*SQRT(2)",      Value(       1 ) ); // The sample standard deviation of (2;3) is 1/SQRT(2).
    CHECK_EVAL("STDEV(B3:B5)*SQRT(2)",      Value(       1 ) ); // Strings are not converted to numbers and are ignored.
    CHECK_EVAL("STDEV({10000000001;10000000002;"
               "10000000003;10000000004;10000000005;"
               "10000000006;10000000007;10000000008;"
               "10000000009;10000000010})", Value( 3.027650 ) ); // Ensure that implementations use a reasonably stable way of calculating STDEV.
    CHECK_EVAL("STDEV(1)",                  Value::errorNUM() ); // At least two numbers must be included
}

void TestStatisticalFunctions::testSTDEVA()
{
    // ODF-tests
    CHECK_EVAL("STDEVA(2;4)/SQRT(2)",      Value(        1 ) ); // The sample standard deviation of (2;4) is SQRT(2).
    CHECK_EVAL_SHORT("STDEVA(B5:C6)",            Value( 2.581989 ) ); // TODO - be more precise / Logicals (referenced) are converted to numbers.
    CHECK_EVAL_SHORT("STDEVA( TRUE();FALSE() )", Value( 0.707107 ) ); // TODO - be more precise / Logicals (inlined) are converted to numbers.
    CHECK_EVAL("STDEVA(1)",                Value::errorNUM() ); // Logicals (inlined) are converted to numbers.
}

void TestStatisticalFunctions::testSTDEVP()
{
    // ODF-tests
    CHECK_EVAL("STDEVP(2;4)",     Value( 1 ) ); // The standard deviation of the set for (2;4) is 1.
    CHECK_EVAL("STDEVP(B4:B5)*2", Value( 1 ) ); // The standard deviation of the set for (2;3) is 0.5.
    CHECK_EVAL("STDEVP(B3:B5)*2", Value( 1 ) ); // Strings are not converted to numbers and are ignored.
    CHECK_EVAL("STDEVP(1)",       Value( 0 ) ); // STDEVP(1) is 0.
}

void TestStatisticalFunctions::testSTDEVPA()
{
    // ODF-tests
    CHECK_EVAL("STDEVPA(2;4)",            Value( 1        ) ); // The sample standard deviation of (2;4) is 1.
    CHECK_EVAL_SHORT("STDEVPA(B5:C6)",          Value( 2.236068 ) ); // TODO - be more precise / Logicals (referenced) are converted to numbers.
    CHECK_EVAL("STDEVPA(TRUE();FALSE())", Value( 0.5      ) ); // Logicals (inlined) are converted to numbers.
}

void TestStatisticalFunctions::testSTEYX()
{
    // ODF-tests
    CHECK_EVAL_SHORT("STEYX(C19:C23;A19:A23)", Value( 2.370953 ) ); // TODO - be more precise
    CHECK_EVAL("STEYX(A19:A23;A25:A29)", Value( 0        ) ); //
    CHECK_EVAL("STEYX(B4:B5;C4:C5)",     Value::errorNUM() ); // at least three number per sequence
}

void TestStatisticalFunctions::testTDIST()
{
    // mode
    // 1 = one tailed distribution
    // 2 = two tailed distribution

    // ODF-tests
    CHECK_EVAL_SHORT("TDIST( 0.5; 1; 1 )",  Value( 0.352416 ) ); // TODO - be more precise
    CHECK_EVAL_SHORT("TDIST( -1.5; 2; 2 )", Value( 0.272393 ) ); // TODO - be more precise
    CHECK_EVAL_SHORT("TDIST( 0.5; 5; 1 )",  Value( 0.319149 ) ); // TODO - be more precise 
    CHECK_EVAL("TDIST( 1; 1; 3 )",    Value::errorNUM() ); // mode = { 1; 2 }
    CHECK_EVAL("TDIST( 1; 0; 1 )",    Value::errorNUM() ); // degreeOfFreedom >= 1
}

void TestStatisticalFunctions::testTTEST()
{
    // ODF-tests
    CHECK_EVAL("TTEST(A19:A23;A24:A28; 1; 1 )", Value( 1        ) ); //
    CHECK_EVAL("TTEST(A19:A23;A24:A28; 2; 1 )", Value( 0.085441 ) ); //
    CHECK_EVAL("TTEST(A19:A23;A24:A28; 1; 2 )", Value( 0.029454 ) ); // 
    CHECK_EVAL("TTEST(A19:A23;A24:A28; 1; 3 )", Value( 0.046213 ) ); //
    CHECK_EVAL("TTEST(A19:A23;A24:A29; 1; 1 )", Value::errorNUM() ); // same amount of numbers for paired samples
    CHECK_EVAL("TTEST(A19:A19;A24:A24; 1; 3 )", Value::errorNUM() ); // two numbers at least for each sequence
}

void TestStatisticalFunctions::testVAR()
{
    // ODF-tests
    CHECK_EVAL("VAR(2;4)",     Value(        2 ) ); // The sample variance of (2;4) is 2.
    CHECK_EVAL("VAR(B4:B5)*2", Value(        1 ) ); // The sample variance of (2;3) is 0.5.
    CHECK_EVAL("VAR(B3:B5)*2", Value(        1 ) ); // Strings are not converted to numbers and are ignored.
    CHECK_EVAL("VAR(1)",       Value::errorNUM() ); // At least two numbers must be included
}

void TestStatisticalFunctions::testVARA()
{
    // ODF-tests
    CHECK_EVAL("VARA(2;4)",            Value(            2 ) ); // The sample variance of (2;4) is 2.
    CHECK_EVAL("VARA(B5:C6)",          Value( 6.6666666667 ) ); // Logicals (referenced) are converted to numbers.
    CHECK_EVAL("VARA(TRUE();FALSE())", Value(          0.5 ) ); // Logicals (inlined) are converted to numbers.
    CHECK_EVAL("VARA(1)",              Value::errorNUM()     ); // Two numbers at least.
}

void TestStatisticalFunctions::testVARP()
{
    //  Cell | Value
    // ------+-------
    //   B3  |  "7"   
    //   B4  |   2
    //   B5  |   3

    // ODF-tests
    CHECK_EVAL("VARP(2;4)",     Value( 1 ) ); // The variance of the set for (2;4) is 1.
    CHECK_EVAL("VARP(B4:B5)*4", Value( 1 ) ); // The variance of the set for (2;3) is 0.25.
    CHECK_EVAL("VARP(B3:B5)*4", Value( 1 ) ); // Strings are not converted to numbers and are ignored.
}

void TestStatisticalFunctions::testVARPA()
{
    //  Cell | Value      Cell | Value
    // ------+------     ------+------
    //   B5  |   3         C5  |  5
    //   B6  | true        C6  |  7

    // ODF-tests
    CHECK_EVAL("VARPA(2;4)",            Value( 1   ) ); // The sample variance of (2;4) is 1.
    CHECK_EVAL("VARPA(B5:C6)",          Value( 5   ) ); // Logicals (referenced) are converted to numbers.
    CHECK_EVAL("VARPA(TRUE();FALSE())", Value( 0.25 ) ); // Logicals (inlined) are converted to numbers.
}

void TestStatisticalFunctions::testWEIBULL()
{
    // TODO - be more precise

    // ODF-tests
    CHECK_EVAL_SHORT("WEIBULL(  2; 3; 4; 0 )", Value( 0.165468 ) ); // pdf
    CHECK_EVAL_SHORT("WEIBULL(  2; 3; 4; 1 )", Value( 0.117503 ) ); // cdf
    CHECK_EVAL_SHORT("WEIBULL( -1; 3; 4; 0 )", Value::errorNUM() ); // value >= 0
    CHECK_EVAL_SHORT("WEIBULL(  2; 0; 4; 0 )", Value::errorNUM() ); // alpha > 0
    CHECK_EVAL_SHORT("WEIBULL(  2; 3; 0; 0 )", Value::errorNUM() ); // beta > 0
}

void TestStatisticalFunctions::testZTEST()
{
    // ODF-tests
    CHECK_EVAL("ZTEST(B4:C5; 3.5      )", Value( 0            ) ); // mean = average, estimated standard deviation: fits well
    CHECK_EVAL("ZTEST(B4:C5; 3  ; 2   )", Value( 0.3829249225 ) ); // mean near average, standard deviation greater than estimate: probable
    CHECK_EVAL("ZTEST(B4:C5; 4  ; 0.5 )", Value( 0.9544997361 ) ); // mean near the average, but small deviation: not probable
    CHECK_EVAL("ZTEST(B4:C5; 5        )", Value( 0.9798632484 ) ); // mean at a border value, standard deviation ~ 1,3: nearly improbable
    CHECK_EVAL("ZTEST(B4:C5; 5  ; 0.1 )", Value( 1            ) ); // mean at a border value, small standard deviation: improbable
}

void TestStatisticalFunctions::cleanupTestCase()
{
    delete m_doc;
}

QTEST_KDEMAIN(TestStatisticalFunctions, GUI)

#include "TestStatisticalFunctions.moc"
