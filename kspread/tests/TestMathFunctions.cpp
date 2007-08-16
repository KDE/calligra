/* This file is part of the KDE project
   Copyright 2007 Ariya Hidayat <ariya@kde.org>
   Copyright 2007 Sascha Pfau <MrPeacock@gmail.com>

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

#include "TestKspreadCommon.h"

#include "TestMathFunctions.h"

// NOTE: we do not compare the numbers _exactly_ because it is difficult
// to get one "true correct" expected values for the functions due to:
//  - different algorithms among spreadsheet programs
//  - precision limitation of floating-point number representation
//  - accuracy problem due to propagated error in the implementation
#define CHECK_EVAL(x,y) QCOMPARE(evaluate(x),RoundNumber(y))
#define ROUND(x) (roundf(1e15 * x) / 1e15)

// round to get at most 15-digits number
static Value RoundNumber(double f)
{
  return Value( ROUND(f) );
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

Value TestMathFunctions::evaluate(const QString& formula)
{
  Formula f;
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

void TestMathFunctions::testABS()
{
  CHECK_EVAL( "ABS(0)", 0 );
  CHECK_EVAL( "ABS(-1)", 1 );
  CHECK_EVAL( "ABS(-2)", 2 );
  CHECK_EVAL( "ABS(-3)", 3 );
  CHECK_EVAL( "ABS(-4)", 4 );
  CHECK_EVAL( "ABS(1)", 1 );
  CHECK_EVAL( "ABS(2)", 2 );
  CHECK_EVAL( "ABS(3)", 3 );
  CHECK_EVAL( "ABS(4)", 4 );

  CHECK_EVAL( "ABS(1/0)", Value::errorDIV0() );

}

void TestMathFunctions::testACOS()
{
  // ODF-tests
  CHECK_EVAL( "ACOS(SQRT(2)/2)*4/PI()",    1.0 ); // arc cosine of SQRT(2)/2 is PI()/4 radians.
  CHECK_EVAL( "ACOS(TRUE())",              0.0 ); // TRUE() is 1 if inline.
  CHECK_EVAL( "ACOS(-1.0)/PI()",           1.0 ); // The result must be between 0.0 and PI().
  CHECK_EVAL( "ACOS(2.0)", Value::errorVALUE() ); // The argument must be between -1.0 and 1.0.
}

void TestMathFunctions::testACOSH()
{
  // ODF-tests
  CHECK_EVAL( "ACOSH(1)", 0           ); //
  CHECK_EVAL( "ACOSH(2)", 1.316957897 ); // 
}

void TestMathFunctions::testACOT()
{
  // ODF-tests
  CHECK_EVAL( "ACOT(0)-PI()/2", 0     ); //
}

void TestMathFunctions::testACOTH()
{
  // ODF-tests
  CHECK_EVAL( "ACOTH(2)", 0.549306144 ); //
}

void TestMathFunctions::testASIN()
{
  // ODF-tests
  CHECK_EVAL( "ASIN(SQRT(2)/2)*4/PI()",  1.0 ); // arc sine of SQRT(2)/2 is PI()/4 radians.
  CHECK_EVAL( "ASIN(TRUE())*2/PI()",     1.0 ); // TRUE() is 1 if inline.
  CHECK_EVAL( "ASIN(-1)*2/PI()",        -1.0 ); // The result must be between -PI()/2 and PI()/2.
  CHECK_EVAL( "ASIN(2)", Value::errorVALUE() ); // The argument must be between -1.0 and 1.0.
}

void TestMathFunctions::testASINH()
{
  // ODF-tests
  CHECK_EVAL( "ASINH(0)", 0           ); // 
  CHECK_EVAL( "ASINH(1)", 0.881373587 ); //
}

void TestMathFunctions::testATAN()
{
  // ODF-tests
  CHECK_EVAL( "ATAN(1)*4/PI()", 1        ); // arc tangent of 1 is PI()/4 radians.
  CHECK_EVAL( "ATAN(-1.0e16)", -1.570796 ); // Check if ATAN gives reasonably accurate results, 
                                            // and that slightly negative values as input produce numbers near -£k/2.
}

void TestMathFunctions::testATAN2()
{
  // ODF-tests
  CHECK_EVAL( "ATAN2(1;1)*4/PI()",         1 ); // arc tangent of 1.0/1.0 is PI()/4 radians.
  CHECK_EVAL( "ATAN2(1;-1)*4/PI()",       -1 ); // Location of sign makes a difference.
  CHECK_EVAL( "ATAN2(-1;1)*4/PI()",        3 ); // Location of sign makes a difference.
  CHECK_EVAL( "ATAN2(-1;-1)*4/PI()",      -3 ); // Location of sign makes a difference.
  CHECK_EVAL( "SIGN(ATAN2(-1.0;0.001))",   1 ); // If y is small, it's still important
  CHECK_EVAL( "SIGN(ATAN2(-1.0;-0.001))", -1 ); // If y is small, it's still important
  CHECK_EVAL( "ATAN2(-1.0;0)/PI()",        1 ); // By definition ATAN2(-1,0) should give PI() rather than -PI().
}

void TestMathFunctions::testCEIL()
{
  CHECK_EVAL( "CEIL(0)", 0 );

  CHECK_EVAL( "CEIL(0.1)", 1 );
  CHECK_EVAL( "CEIL(0.01)", 1 );
  CHECK_EVAL( "CEIL(0.001)", 1 );
  CHECK_EVAL( "CEIL(0.0001)", 1 );
  CHECK_EVAL( "CEIL(0.00001)", 1 );
  CHECK_EVAL( "CEIL(0.000001)", 1 );
  CHECK_EVAL( "CEIL(0.0000001)", 1 );

  CHECK_EVAL( "CEIL(1.1)", 2 );
  CHECK_EVAL( "CEIL(1.01)", 2 );
  CHECK_EVAL( "CEIL(1.001)", 2 );
  CHECK_EVAL( "CEIL(1.0001)", 2 );
  CHECK_EVAL( "CEIL(1.00001)", 2 );
  CHECK_EVAL( "CEIL(1.000001)", 2 );
  CHECK_EVAL( "CEIL(1.0000001)", 2 );

  CHECK_EVAL( "CEIL(-0.1)", 0 );
  CHECK_EVAL( "CEIL(-0.01)", 0 );
  CHECK_EVAL( "CEIL(-0.001)", 0 );
  CHECK_EVAL( "CEIL(-0.0001)", 0 );
  CHECK_EVAL( "CEIL(-0.00001)", 0 );
  CHECK_EVAL( "CEIL(-0.000001)", 0 );
  CHECK_EVAL( "CEIL(-0.0000001)", 0 );


  CHECK_EVAL( "CEIL(-1.1)", -1 );
  CHECK_EVAL( "CEIL(-1.01)", -1 );
  CHECK_EVAL( "CEIL(-1.001)", -1 );
  CHECK_EVAL( "CEIL(-1.0001)", -1 );
  CHECK_EVAL( "CEIL(-1.00001)", -1 );
  CHECK_EVAL( "CEIL(-1.000001)", -1 );
  CHECK_EVAL( "CEIL(-1.0000001)", -1 );
}

void TestMathFunctions::testCEILING()
{
  CHECK_EVAL( "CEILING(0; 0.1)", 0 );
  CHECK_EVAL( "CEILING(0; 0.2)", 0 );
  CHECK_EVAL( "CEILING(0; 1.0)", 0 );
  CHECK_EVAL( "CEILING(0; 10.0)", 0 );

  CHECK_EVAL( "CEILING(0.1; 0.2)", 0.2 );
  CHECK_EVAL( "CEILING(0.1; 0.4)", 0.4 );
  CHECK_EVAL( "CEILING(1.1; 0.2)", 1.2 );

  // because can't divide by 0
  CHECK_EVAL( "CEILING(1; 0)", Value::errorDIV0() );
  CHECK_EVAL( "CEILING(2; 0)", Value::errorDIV0() );

  // but this one should be just fine !
  CHECK_EVAL( "CEILING(0; 0)", 0 );

  // different sign does not make sense
  CHECK_EVAL( "CEILING(-1; 2)", Value::errorNUM() );
  CHECK_EVAL( "CEILING(1; -2)", Value::errorNUM() );
}

void TestMathFunctions::testFACT()
{
  CHECK_EVAL( "FACT(0)", 1 );
  CHECK_EVAL( "FACT(1)", 1 );
  CHECK_EVAL( "FACT(2)", 2 );
  CHECK_EVAL( "FACT(3)", 6 );
  CHECK_EVAL( "FACT(-1)", Value::errorNUM() );
  CHECK_EVAL( "FACT(\"xyzzy\")", Value::errorNUM() );
}

void TestMathFunctions::testFACTDOUBLE()
{
  CHECK_EVAL( "FACTDOUBLE(0)", 1 );
  CHECK_EVAL( "FACTDOUBLE(1)", 1 );
  CHECK_EVAL( "FACTDOUBLE(7)", 105 );
  CHECK_EVAL( "FACTDOUBLE(6)", 48 );
  CHECK_EVAL( "FACTDOUBLE(-1)", Value::errorNUM() );
  CHECK_EVAL( "FACTDOUBLE(\"xyzzy\")", Value::errorNUM() );
}

void TestMathFunctions::testFIB()
{
  CHECK_EVAL( "FIB(1)", 1 );
  CHECK_EVAL( "FIB(2)", 1 );
  CHECK_EVAL( "FIB(3)", 2 );
  CHECK_EVAL( "FIB(4)", 3 );
  CHECK_EVAL( "FIB(5)", 5 );
  CHECK_EVAL( "FIB(6)", 8 );
  CHECK_EVAL( "FIB(7)", 13 );
  CHECK_EVAL( "FIB(8)", 21 );
  CHECK_EVAL( "FIB(9)", 34 );
  CHECK_EVAL( "FIB(10)", 55 );

  // large number
  CHECK_EVAL( "FIB(100)", 3.54224848179263E+20 );
  CHECK_EVAL( "FIB(200)", 2.80571172992512E+41 );
  CHECK_EVAL( "FIB(300)", 2.22232244629423E+62 );
  CHECK_EVAL( "FIB(400)", 1.76023680645016E+83 );
  CHECK_EVAL( "FIB(500)", 1.394232245617E+104 );
  CHECK_EVAL( "FIB(600)", 1.10433070572954E+125 );

  // invalid
  CHECK_EVAL( "FIB(0)", Value::errorNUM() );
  CHECK_EVAL( "FIB(-1)", Value::errorNUM() );
  CHECK_EVAL( "FIB(\"text\")", Value::errorVALUE() );
}

void TestMathFunctions::testGAMMA()
{
  // ODF-Tests
  CHECK_EVAL( "GAMMA(1.00)", 1.0000000000 );
  CHECK_EVAL( "GAMMA(1.10)", 0.9513507700 );
  CHECK_EVAL( "GAMMA(1.50)", 0.8862269255 );
}

void TestMathFunctions::testGAMMALN()
{
  // ODF-Tests
  CHECK_EVAL( "GAMMALN(1.00)",  0.0000000000 );
  CHECK_EVAL( "GAMMALN(2.00)",  0.0000000000 );
  CHECK_EVAL( "GAMMALN(3.00)",  0.6931471806 );
  CHECK_EVAL( "GAMMALN(1.50)", -0.1207822376 );
}

void TestMathFunctions::testGCD()
{
  CHECK_EVAL( "GCD(5;15;25)", 5 );
  CHECK_EVAL( "GCD(2;3)", 1 );
  CHECK_EVAL( "GCD(18;24)", 6 );
  CHECK_EVAL( "GCD(18.1;24.1)", 6 );
  CHECK_EVAL( "GCD(1.1;2.2)", 1 );
  CHECK_EVAL( "GCD(18.9;24.9)", 6 );
  CHECK_EVAL( "GCD(7)",  7 );
  CHECK_EVAL( "GCD(5;0)", 5 );
  CHECK_EVAL( "GCD(0;0)", 0 );
  CHECK_EVAL( "GCD(-2;3)", Value::errorNUM() );
  CHECK_EVAL( "GCD(2;-4)", Value::errorNUM() );
  CHECK_EVAL( "GCD(-2;-4)", Value::errorNUM() );
}

void TestMathFunctions::testLCM()
{
  CHECK_EVAL( "LCM(5;15;25)", 75 );
  CHECK_EVAL( "LCM(2;3)", 6 );
  CHECK_EVAL( "LCM(18;12)", 36 );
  CHECK_EVAL( "LCM(12;18)", 36 );
  CHECK_EVAL( "LCM(12.1;18.1)", 36 );
  CHECK_EVAL( "LCM(18.1;12.1)", 36 );
  CHECK_EVAL( "LCM(18.9;12.9)", 36 );
  CHECK_EVAL( "LCM(7)", 7 );
  CHECK_EVAL( "LCM(5;0)", 0 );
  CHECK_EVAL( "LCM(-2;4)", Value::errorNUM() );
  CHECK_EVAL( "LCM(2;-4)", Value::errorNUM() );
  CHECK_EVAL( "LCM(-2;-4)", Value::errorNUM() );
}

void TestMathFunctions::testLOG()
{
  CHECK_EVAL( "LOG(1;10)", 0 );
  CHECK_EVAL( "LOG(1;EXP(1))", 0 );
  CHECK_EVAL( "LOG(10;10)", 1 );
  CHECK_EVAL( "LOG(EXP(1);EXP(1))", 1 );
  CHECK_EVAL( "LOG(10)", 1 );
  CHECK_EVAL( "LOG(8*8*8;8)", 3 );
  CHECK_EVAL( "LOG(0;10)", Value::errorNUM() );
  CHECK_EVAL( "LOG(\"foo\";10)", Value::errorNUM() );
  CHECK_EVAL( "LOG(2;\"foo\")", Value::errorNUM() );
  CHECK_EVAL( "LOG(NA();10)", Value::errorNA() );
  CHECK_EVAL( "LOG(10;NA())", Value::errorNA() );
  CHECK_EVAL( "LOG(NA();NA())", Value::errorNA() );
}

void TestMathFunctions::testLOG10()
{
    CHECK_EVAL( "LOG10(1)",  0 );
    CHECK_EVAL( "LOG10(10)",  1 );
    CHECK_EVAL( "LOG10(100)", 2 );
    CHECK_EVAL( "LOG10(0)", Value::errorNUM() );
    CHECK_EVAL( "LOG10(\"H\")", Value::errorNUM() );
    CHECK_EVAL( "LOG10(-2)",  Value::errorNUM() );
}

void TestMathFunctions::testMDETERM()
{
    CHECK_EVAL( "MDETERM({2;4|3;5})", -2 );
    CHECK_EVAL( "MDETERM({2;4})", Value::errorVALUE() );
    CHECK_EVAL( "MDETERM({2;4|3;6})", 0 );
    CHECK_EVAL( "MDETERM(2)", 2 );
}

void TestMathFunctions::testMINVERSE()
{
    Value value( Value::Array );
    value.setElement( 0, 0, Value( -2.5 ) );
    value.setElement( 1, 0, Value(  2.0 ) );
    value.setElement( 0, 1, Value(  1.5 ) );
    value.setElement( 1, 1, Value( -1.0 ) );
    CHECK_EVAL( "MINVERSE({2;4|3;5})", value );              // simply invertible
    value.setElement( 0, 0, Value(  5.0 ) );
    value.setElement( 1, 0, Value(  1.0 ) );
    value.setElement( 2, 0, Value( -2.0 ) );
    value.setElement( 0, 1, Value( -1.0 ) );
    value.setElement( 1, 1, Value( -1.0 ) );
    value.setElement( 2, 1, Value(  1.0 ) );
    value.setElement( 0, 1, Value( -2.0 ) );
    value.setElement( 1, 1, Value(  1.0 ) );
    value.setElement( 2, 1, Value(  0.0 ) );
    CHECK_EVAL( "MINVERSE({1;2;1|2;4;3|3;7;4}", value );     // fails without pivoting
    CHECK_EVAL( "MINVERSE({2;4})", Value::errorVALUE() );    // non-square matrix
    CHECK_EVAL( "MINVERSE({2;4|3;6})", Value::errorDIV0() ); // singular matrix
    CHECK_EVAL( "MINVERSE(2)", Value( 0.5 ) );               // one elementary matrix
}

void TestMathFunctions::testMMULT()
{
    CHECK_EVAL( "MMULT({2;4|3;5};{2;4|3;5})", evaluate( "{16.0;28.0|21.0;37.0}" ) );
}

void TestMathFunctions::testMUNIT()
{
    CHECK_EVAL( "MUNIT(2)", evaluate( "{1;0|0;1}" ) );
    CHECK_EVAL( "MUNIT(3)", evaluate( "{1;0;0|0;1;0|0;0;1}" ) );
}

void TestMathFunctions::testODD()
{
    CHECK_EVAL( "ODD(5)", 5 );
    CHECK_EVAL( "ODD(-5)", -5 );
    CHECK_EVAL( "ODD(2)",  3 );
    CHECK_EVAL( "ODD(0.3)", 1 );
    CHECK_EVAL( "ODD(-2)",  -3 );
    CHECK_EVAL( "ODD(-0.3)",  -1 );
    CHECK_EVAL( "ODD(0)", 1 );
}

QTEST_KDEMAIN(TestMathFunctions, GUI)

#include "TestMathFunctions.moc"
