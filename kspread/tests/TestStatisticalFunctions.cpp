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

    // B3:B7
    storage->setValue(2, 3, Value(   "7"   ));
    storage->setValue(2, 4, Value(    2    ));
    storage->setValue(2, 5, Value(    3    ));
    storage->setValue(2, 6, Value(  true   ));
    storage->setValue(2, 7, Value( "Hello" ));
    // B9:B17
    storage->setValue(2, 9, Value::errorDIV0());
    storage->setValue(2,10, Value( 0 ));
    storage->setValue(2,11, Value( 3 ));
    storage->setValue(2,12, Value( 4 ));
    storage->setValue(2,13, Value( "2005-0131T01:00:00" ));
    storage->setValue(2,14, Value( 1 ));
    storage->setValue(2,15, Value( 2 ));
    storage->setValue(2,16, Value( 3 ));
    storage->setValue(2,17, Value( 4 ));

    // C11:C17
    storage->setValue(3,11, Value( 5 ));
    storage->setValue(3,12, Value( 6 ));
    storage->setValue(3,13, Value( 8 ));
    storage->setValue(3,14, Value( 4 ));
    storage->setValue(3,15, Value( 3 ));
    storage->setValue(3,16, Value( 2 ));
    storage->setValue(3,17, Value( 1 ));
}

void TestStatisticalFunctions::testAVERAGEA()
{
    CHECK_EVAL("AVERAGEA(2; 4)",               Value( 3 ) ); //
    CHECK_EVAL("AVERAGEA(TRUE(); FALSE(); 5)", Value( 2 ) ); //
}

void TestStatisticalFunctions::testBETADIST()
{
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

void TestStatisticalFunctions::testGAMMADIST()
{
    CHECK_EVAL("GAMMADIST(0;3;4)",           Value(        0 ) );
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
    CHECK_EVAL("GAUSS(0)", Value(           0 ) ); // Mean of one value.
    CHECK_EVAL("GAUSS(1)", Value( 0.341344746 ) ); // Multiple equivalent values.
}

void TestStatisticalFunctions::testGEOMEAN()
{
    CHECK_EVAL("GEOMEAN(7)",           Value(            7 ) ); // Mean of one value.
    CHECK_EVAL("GEOMEAN(5;5;5;5)",     Value(            5 ) ); // Multiple equivalent values.
    CHECK_EVAL("GEOMEAN(2;8;2;8)",     Value(            4 ) ); // Some values.
    CHECK_EVAL("GEOMEAN(8;0;8;8;8;8)", Value::errorNUM()     ); // Error if there is a 0 in the range.
    CHECK_EVAL("GEOMEAN(C11)",         Value(            5 ) ); // One value, range.
    CHECK_EVAL("GEOMEAN(C11:C17)",     Value( 3.4451109418 ) ); // Some values, range.
    CHECK_EVAL("GEOMEAN(B14:B17)",     Value( 2.2133638394 ) ); // Some values, range.
}

void TestStatisticalFunctions::testMAXA()
{
    CHECK_EVAL("MAXA(2;4;1;-8)", Value(4));
    CHECK_EVAL("MAXA(B4:B5)", Value(3));
//     CHECK_EVAL("ISNA(MAXA(NA())", Value(true));
    CHECK_EVAL("MAXA(B3:B5)", Value(3));
    CHECK_EVAL("MAXA(-1;B7)", Value(0));
    CHECK_EVAL("MAXA(\"a\")", Value::errorVALUE());
    CHECK_EVAL("MAXA(B3:B9)", Value::errorVALUE());
    CHECK_EVAL("MAXA(B6:B7)", Value(1));
}

void TestStatisticalFunctions::testMINA()
{
    CHECK_EVAL("MIN(2;4;1;-8)", Value(-8));
    CHECK_EVAL("MIN(B4:B5)", Value(2));
}

void TestStatisticalFunctions::cleanupTestCase()
{
    delete m_doc;
}

QTEST_KDEMAIN(TestStatisticalFunctions, GUI)

#include "TestStatisticalFunctions.moc"
