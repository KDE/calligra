/* This file is part of the KDE project
   Copyright 2006 Ariya Hidayat <ariya@kde.org>
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

#include "TestFinancialFunctions.h"

// NOTE: we do not compare the numbers _exactly_ because it is difficult
// to get one "true correct" expected values for the functions due to:
//  - different algorithms among spreadsheet programs
//  - precision limitation of floating-point number representation
//  - accuracy problem due to propagated error in the implementation
#define CHECK_EVAL(x,y) QCOMPARE(evaluate(x),RoundNumber(y))
#define CHECK_EVAL_SHORT(x,y) QCOMPARE(evaluateShort(x),RoundNumberShort(y))

// round to get at most 10-digits number
static Value RoundNumber(double f)
{
  return Value( QString::number(f, 'g', 10) );
}

// round to get at most 10-digits number
static Value RoundNumber(const Value& v)
{
  if(v.isNumber())
    return Value( QString::number(numToDouble(v.asFloat()), 'g', 10) );
  else
    return v;
}

// round to get at most 6-digits number
static Value RoundNumberShort(const Value& v)
{
  if(v.isNumber())
    return Value( QString::number(numToDouble(v.asFloat()), 'g', 5) );
  else
    return v;
}

Value TestFinancialFunctions::evaluate(const QString& formula)
{
  Formula f;
  QString expr = formula;
  if ( expr[0] != '=' )
    expr.prepend( '=' );
  f.setExpression( expr );
  Value result = f.eval();

#if 0
  // this magically generates the CHECKs
  printf("  CHECK_EVAL( \"%s\",  %.14e) );\n", qPrintable(formula), result.asFloat());
#endif

  return RoundNumber(result);
}

Value TestFinancialFunctions::evaluateShort(const QString& formula)
{
  Formula f;
  QString expr = formula;
  if ( expr[0] != '=' )
    expr.prepend( '=' );
  f.setExpression( expr );
  Value result = f.eval();

  return RoundNumberShort(result);
}

// ACCRINT
void TestFinancialFunctions::testACCRINT()
{
  // kspread
  CHECK_EVAL_SHORT( "ACCRINT( \"02/28/2001\"; \"08/31/2001\"; \"05/01/2001\" ;  0.1; 1000; 2; 0 )", Value( 16.944 ) ); 
}

// Fixed-declining balance depreciation
// DB(cost, salvage, life, period, month)
void TestFinancialFunctions::testDB()
{
  // Excel example: http://office.microsoft.com/en-us/excel/HP100623551033.aspx
  CHECK_EVAL( "DB(1000000; 100000; 6; 1; 7)", 186083.3333333333 ) ;
  CHECK_EVAL( "DB(1000000; 100000; 6; 2; 7)", 259639.4166666667 ) ;
  CHECK_EVAL( "DB(1000000; 100000; 6; 3; 7)", 176814.4427500000 ) ;
  CHECK_EVAL( "DB(1000000; 100000; 6; 4; 7)", 120410.6355127500 ) ;
  CHECK_EVAL( "DB(1000000; 100000; 6; 5; 7)", 81999.64278418274 ) ;
  CHECK_EVAL( "DB(1000000; 100000; 6; 6; 7)", 55841.75673602846 ) ;
  CHECK_EVAL( "DB(1000000; 100000; 6; 7; 7)", 15845.09847384807 ) ;

  // http://www.vni.com/products/imsl/jmsl/v30/api/com/imsl/finance/dbEx1.html
  CHECK_EVAL( "DB(2500; 500; 3; 1; 6)",  518.750000000000 );
  CHECK_EVAL( "DB(2500; 500; 3; 2; 6)",  822.218750000000 );
  CHECK_EVAL( "DB(2500; 500; 3; 3; 6)",  480.997968750000 );
  CHECK_EVAL( "DB(2500; 500; 3; 4; 6)",  140.691905859375 );

  // test cases in OpenFormula specification
  CHECK_EVAL( "DB(4000;500;4;2)",  963.90 );
  CHECK_EVAL( "DB(4000;500;4;2;2)",  1510.65 );
  CHECK_EVAL( "DB(4000;500;4;5)",  0.0 );
  CHECK_EVAL( "DB(0;500;4;2)",  Value::errorNUM() );
  CHECK_EVAL( "DB(4000;-500;4;2)",  Value::errorNUM() );
  CHECK_EVAL( "DB(4000;500;0;0)",  Value::errorNUM() );
  CHECK_EVAL( "DB(4000;500;2;0)",  Value::errorNUM() );
}

// Double declining balance depreciation
// DDB(cost, salvage, life, period, factor)
void TestFinancialFunctions::testDDB()
{
  // Excel example: http://office.microsoft.com/en-us/excel/HP100623561033.aspx
  CHECK_EVAL( "DDB(2400; 300; 10*365; 1; 2)", 1.31506849315065 ) ;
  CHECK_EVAL( "DDB(2400; 300; 10*12; 1; 2)", 40.0 ) ;
  CHECK_EVAL( "DDB(2400; 300; 10; 1; 2)", 480.0 ) ;
  CHECK_EVAL( "DDB(2400; 300; 10; 2; 1.5)", 306 ) ;
  CHECK_EVAL( "DDB(2400; 300; 10; 10; 2)", 22.1225472000002 ) ;

  // http://www.vni.com/products/imsl/jmsl/v30/api/com/imsl/finance/ddbEx1.html
  CHECK_EVAL( "DDB(2500; 500; 24; 1; 2)",  208.333333333333 );
  CHECK_EVAL( "DDB(2500; 500; 24; 2; 2)",  190.972222222222 );
  CHECK_EVAL( "DDB(2500; 500; 24; 3; 2)",  175.057870370370 );
  CHECK_EVAL( "DDB(2500; 500; 24; 4; 2)",  160.469714506173 );
  CHECK_EVAL( "DDB(2500; 500; 24; 5; 2)",  147.097238297325 );
  CHECK_EVAL( "DDB(2500; 500; 24; 6; 2)",  134.839135105881 );
  CHECK_EVAL( "DDB(2500; 500; 24; 7; 2)",  123.602540513725 );
  CHECK_EVAL( "DDB(2500; 500; 24; 8; 2)",  113.302328804248 );
  CHECK_EVAL( "DDB(2500; 500; 24; 9; 2)",  103.860468070560 );
  CHECK_EVAL( "DDB(2500; 500; 24; 10; 2)",  95.2054290646802 );
  CHECK_EVAL( "DDB(2500; 500; 24; 11; 2)",  87.2716433092901 );
  CHECK_EVAL( "DDB(2500; 500; 24; 12; 2)",  79.9990063668494 );
  CHECK_EVAL( "DDB(2500; 500; 24; 13; 2)",  73.3324225029452 );
  CHECK_EVAL( "DDB(2500; 500; 24; 14; 2)",  67.2213872943665 );
  CHECK_EVAL( "DDB(2500; 500; 24; 15; 2)",  61.6196050198359 );
  CHECK_EVAL( "DDB(2500; 500; 24; 16; 2)",  56.4846379348497 );
  CHECK_EVAL( "DDB(2500; 500; 24; 17; 2)",  51.7775847736120 );
  CHECK_EVAL( "DDB(2500; 500; 24; 18; 2)",  47.4627860424778 );
  CHECK_EVAL( "DDB(2500; 500; 24; 19; 2)",  22.0906464672553 );
  CHECK_EVAL( "DDB(2500; 500; 24; 20; 2)",  0 );
  CHECK_EVAL( "DDB(2500; 500; 24; 21; 2)",  0 );
  CHECK_EVAL( "DDB(2500; 500; 24; 22; 2)",  0 );
  CHECK_EVAL( "DDB(2500; 500; 24; 23; 2)",  0 );
  CHECK_EVAL( "DDB(2500; 500; 24; 24; 2)",  0 );

  // test cases in OpenFormula specification
  CHECK_EVAL( "DDB(4000; 500; 4; 2; 2)", 1000 ) ;
  CHECK_EVAL( "DDB(4000; 500; 4; 2)", 1000 ) ;
  CHECK_EVAL( "DDB(1100; 100; 5; 5; 2.3 )", 0 ) ;

  // try default factor (=2)
  CHECK_EVAL( "DDB(2400; 300; 10*12; 1)", 40.0 ) ;
  CHECK_EVAL( "DDB(2400; 300; 10; 1)", 480.0 ) ;
  CHECK_EVAL( "DDB(2500; 500; 24; 22)",  0 );
  CHECK_EVAL( "DDB(2500; 500; 24; 23)",  0 );
  CHECK_EVAL( "DDB(2500; 500; 24; 24)",  0 );

  // factor > life
  CHECK_EVAL( "DDB(2400; 300; 10; 0.8; 20)", 2100 );
  CHECK_EVAL( "DDB(2400; 300; 10; 1.0; 20)", 2100 );
  CHECK_EVAL( "DDB(2400; 300; 10; 1.2; 20)", 0 );

  // factor is fraction
  CHECK_EVAL( "DDB(2400; 300; 10; 2; 2.5)", 450 );
  CHECK_EVAL( "DDB(2400; 300; 10; 2; 1.5)", 306 );

  // period is fraction
  CHECK_EVAL( "DDB(2400; 300; 10; 6.7; 2)", 134.5408487904432 );
  CHECK_EVAL( "DDB(2400; 300; 10; 7.7; 2)", 107.6326790323546 );
}

// DISC
void TestFinancialFunctions::testDISC()
{
  // basis | day-count basis
  //-------+-----------------------------------
  //   0   |  US (NASD) 30/360
  //   1   |  Actual/actual (Euro), also known as AFB
  //   2   |  Actual/360
  //   3   |  Actual/365
  //   4   |  European 30/360

  CHECK_EVAL_SHORT( "DISC( DATE(2004;02;29); date(2009;01;01); 95000; 100000; 0)", Value (  0.010339 ) ); 
//    CHECK_EVAL_SHORT( "DISC( DATE(2004;02;29); date(2009;01;01); 95000; 100000; 1)", Value (  0.010333 ) ); // NOK (0.010332) 
  CHECK_EVAL_SHORT( "DISC( DATE(2004;02;29); date(2009;01;01); 95000; 100000; 2)", Value (  0.010181 ) );
  CHECK_EVAL_SHORT( "DISC( DATE(2004;02;29); date(2009;01;01); 95000; 100000; 3)", Value (  0.010322 ) );
  CHECK_EVAL_SHORT( "DISC( DATE(2004;02;29); date(2009;01;01); 95000; 100000; 4)", Value (  0.010333 ) );
  CHECK_EVAL_SHORT( "DISC( DATE(2006;01;01); date(2008;01;01);   200;    100; 3)", Value ( -0.500000 ) );
  CHECK_EVAL_SHORT( "DISC( DATE(2006;01;01); date(2005;07;01); 95000; 100000; 4)", Value (    false  ) );
}

// DOLLARDE
void TestFinancialFunctions::testDOLLARDE()
{
  // http://publib.boulder.ibm.com/infocenter/iadthelp/v7r0/index.jsp?topic=/com.businessobjects.integration.eclipse.designer.doc/designer/Functions68.html
  CHECK_EVAL_SHORT( "DOLLARDE(   1.1 ; 8)" , Value ( 1.125     ) ); // 
  CHECK_EVAL_SHORT( "DOLLARDE(   2.13;16)" , Value ( 2.8125    ) ); // 
  CHECK_EVAL_SHORT( "DOLLARDE(   2.45;16)" , Value ( 4.8125    ) ); //  
  CHECK_EVAL_SHORT( "DOLLARDE(   1.16; 8)" , Value ( 1.2       ) ); // 

  // http://www.bettersolutions.com/excel/EDH113/LR849116511.htm
  CHECK_EVAL_SHORT( "DOLLARDE(   1.1 ; 2)" , Value ( 1.5       ) ); // 
  CHECK_EVAL_SHORT( "DOLLARDE(   1.25; 5)" , Value ( 1.5       ) ); // 
  CHECK_EVAL_SHORT( "DOLLARDE(   5.08; 4)" , Value ( 5.2       ) ); // 
  CHECK_EVAL_SHORT( "DOLLARDE(   5.24; 4)" , Value ( 5.6       ) ); // 
  CHECK_EVAL_SHORT( "DOLLARDE( 100.24; 4)" , Value ( 100.6     ) ); // 
  CHECK_EVAL_SHORT( "DOLLARFR(DOLLARDE( 101.2; 4);4)", Value ( 101.2 ) ); // for- and backward 

  // ODF 
  CHECK_EVAL_SHORT( "DOLLARDE(   1.1; 4)"  , Value ( 1.25      ) ); // 
  CHECK_EVAL_SHORT( "DOLLARDE(   1.1; 3)"  , Value ( 1.333333  ) ); // 
  CHECK_EVAL_SHORT( "DOLLARDE(  -1.1;10)"  , Value (-1.1       ) ); // 
  CHECK_EVAL_SHORT( "DOLLARDE(   1.0; 5)"  , Value ( 1         ) ); //
  CHECK_EVAL_SHORT( "DOLLARDE(   1.1;10)"  , Value ( 1.1       ) ); //
  CHECK_EVAL_SHORT( "DOLLARDE(   1.1; 0)"  , Value::errorVALUE() ); //   
}

// DOLLARFR
void TestFinancialFunctions::testDOLLARFR()
{
  // my tests
  CHECK_EVAL_SHORT( "DOLLARFR(    1.1 ;  9)" , Value ( 1.09     ) ); //
  CHECK_EVAL_SHORT( "DOLLARFR(    1.1 ; 11)" , Value ( 1.011    ) ); //
  CHECK_EVAL_SHORT( "DOLLARFR(    1.1 ; 10)" , Value ( 1.1      ) ); //
 
  // http://www.bettersolutions.com/excel/EDH113/QR810212321.htm
  CHECK_EVAL_SHORT( "DOLLARFR(    1.125 ; 8)" , Value ( 1.1     ) ); //
  CHECK_EVAL_SHORT( "DOLLARFR(    1.5   ; 2)" , Value ( 1.1     ) ); //
  CHECK_EVAL_SHORT( "DOLLARFR(    1.5   ; 8)" , Value ( 1.4     ) ); //
  CHECK_EVAL_SHORT( "DOLLARFR(    1.5   ; 5)" , Value ( 1.25    ) ); //
  
  // ODF 
  CHECK_EVAL_SHORT( "DOLLARFR(    1.1 ;10)" , Value ( 1.1       ) ); //
  CHECK_EVAL_SHORT( "DOLLARFR(    1.25; 4)" , Value ( 1.1       ) ); // 
  CHECK_EVAL_SHORT( "DOLLARFR(-1.33333; 3)" , Value ( -1.099999 ) ); // ODF specs error (1.1) must be -1.1
  CHECK_EVAL_SHORT( "DOLLARFR(     1.0; 5)" , Value ( 1         ) ); //
  CHECK_EVAL_SHORT( "DOLLARFR(     1.1; 0)" , Value::errorVALUE() ); // 
}

// DURATION
void TestFinancialFunctions::testDURATION()
{
  CHECK_EVAL_SHORT( "DURATION( DATE(1998;01;01);  DATE(2006;01;01); 0.08; 0.09; 2; 1 )" , Value( 5.994 ) ); // TODO check
}

// PDURATION
void TestFinancialFunctions::testPDURATION()
{
  CHECK_EVAL_SHORT( "PDURATION( 0.1; 10; 100 )" , Value( 24.158858 ) ); //
}

// Euro conversion
// EURO(currency)
void TestFinancialFunctions::testEURO()
{
  CHECK_EVAL( "EURO(\"ATS\")", 13.7603 );
  CHECK_EVAL( "EURO(\"BEF\")", 40.3399 );
  CHECK_EVAL( "EURO(\"DEM\")", 1.95583 );
  CHECK_EVAL( "EURO(\"ESP\")", 166.386 );
  CHECK_EVAL( "EURO(\"EUR\")", 1.0 );
  CHECK_EVAL( "EURO(\"FIM\")", 5.94573 );
  CHECK_EVAL( "EURO(\"FRF\")", 6.55957 );
  CHECK_EVAL( "EURO(\"GRD\")", 340.75 );
  CHECK_EVAL( "EURO(\"IEP\")", 0.787564 );
  CHECK_EVAL( "EURO(\"ITL\")", 1936.27 );
  CHECK_EVAL( "EURO(\"LUX\")", 40.3399 );
  CHECK_EVAL( "EURO(\"NLG\")", 2.20371 );
  CHECK_EVAL( "EURO(\"PTE\")", 200.482 );

  // should still work with lowercase
  CHECK_EVAL( "EURO(\"ats\")", 13.7603 );
  CHECK_EVAL( "EURO(\"bef\")", 40.3399 );
  CHECK_EVAL( "EURO(\"dem\")", 1.95583 );
  CHECK_EVAL( "EURO(\"esp\")", 166.386 );
  CHECK_EVAL( "EURO(\"eur\")", 1.0 );
  CHECK_EVAL( "EURO(\"fim\")", 5.94573 );
  CHECK_EVAL( "EURO(\"frf\")", 6.55957 );
  CHECK_EVAL( "EURO(\"grd\")", 340.75 );
  CHECK_EVAL( "EURO(\"iep\")", 0.787564 );
  CHECK_EVAL( "EURO(\"itl\")", 1936.27 );
  CHECK_EVAL( "EURO(\"lux\")", 40.3399 );
  CHECK_EVAL( "EURO(\"nlg\")", 2.20371 );
  CHECK_EVAL( "EURO(\"pte\")", 200.482 );

  // should still work with mixed-case
  CHECK_EVAL( "EURO(\"Ats\")", 13.7603 );
  CHECK_EVAL( "EURO(\"Bef\")", 40.3399 );
  CHECK_EVAL( "EURO(\"Dem\")", 1.95583 );
  CHECK_EVAL( "EURO(\"Esp\")", 166.386 );
  CHECK_EVAL( "EURO(\"Eur\")", 1.0 );
  CHECK_EVAL( "EURO(\"Fim\")", 5.94573 );
  CHECK_EVAL( "EURO(\"Frf\")", 6.55957 );
  CHECK_EVAL( "EURO(\"GrD\")", 340.75 );
  CHECK_EVAL( "EURO(\"IeP\")", 0.787564 );
  CHECK_EVAL( "EURO(\"Itl\")", 1936.27 );
  CHECK_EVAL( "EURO(\"luX\")", 40.3399 );
  CHECK_EVAL( "EURO(\"nlG\")", 2.20371 );
  CHECK_EVAL( "EURO(\"ptE\")", 200.482 );

  CHECK_EVAL( "EURO(\"NOMANSLAND\")", Value::errorNUM() );
}

// Currency conversion using Euro
// EUROCONVERT(number,source,target)
void TestFinancialFunctions::testEUROCONVERT()
{
  // 1 Euro to ...
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"ATS\")", 13.7603 );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"BEF\")", 40.3399 );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"DEM\")", 1.95583 );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"ESP\")", 166.386 );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"EUR\")", 1.0 );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"FIM\")", 5.94573 );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"FRF\")", 6.55957 );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"GRD\")", 340.75 );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"IEP\")", 0.787564 );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"ITL\")", 1936.27 );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"LUX\")", 40.3399 );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"NLG\")", 2.20371 );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"PTE\")", 200.482 );

  // identity
  CHECK_EVAL( "EUROCONVERT(1;\"BEF\";\"bef\")", 1.0 );
  CHECK_EVAL( "EUROCONVERT(1;\"DEM\";\"dem\")", 1.0 );
  CHECK_EVAL( "EUROCONVERT(1;\"ESP\";\"esp\")", 1.0 );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"eur\")", 1.0 );
  CHECK_EVAL( "EUROCONVERT(1;\"FIM\";\"fim\")", 1.0 );
  CHECK_EVAL( "EUROCONVERT(1;\"FRF\";\"frf\")", 1.0 );
  CHECK_EVAL( "EUROCONVERT(1;\"GRD\";\"grd\")", 1.0 );
  CHECK_EVAL( "EUROCONVERT(1;\"IEP\";\"iep\")", 1.0 );
  CHECK_EVAL( "EUROCONVERT(1;\"ITL\";\"itl\")", 1.0 );
  CHECK_EVAL( "EUROCONVERT(1;\"LUX\";\"lux\")", 1.0 );
  CHECK_EVAL( "EUROCONVERT(1;\"NLG\";\"nlg\")", 1.0 );
  CHECK_EVAL( "EUROCONVERT(1;\"PTE\";\"pte\")", 1.0 );

  // all other combinations
  CHECK_EVAL( "EUROCONVERT(   2; \"ATS\"; \"bef\" )", 2*40.3399/13.7603 );
  CHECK_EVAL( "EUROCONVERT(   3; \"ATS\"; \"dem\" )", 3*1.95583/13.7603 );
  CHECK_EVAL( "EUROCONVERT(   4; \"ATS\"; \"esp\" )", 4*166.386/13.7603 );
  CHECK_EVAL( "EUROCONVERT(   5; \"ATS\"; \"eur\" )", 5*1/13.7603 );
  CHECK_EVAL( "EUROCONVERT(   6; \"ATS\"; \"fim\" )", 6*5.94573/13.7603 );
  CHECK_EVAL( "EUROCONVERT(   7; \"ATS\"; \"frf\" )", 7*6.55957/13.7603 );
  CHECK_EVAL( "EUROCONVERT(   8; \"ATS\"; \"grd\" )", 8*340.75/13.7603 );
  CHECK_EVAL( "EUROCONVERT(   9; \"ATS\"; \"iep\" )", 9*0.787564/13.7603 );
  CHECK_EVAL( "EUROCONVERT(  10; \"ATS\"; \"itl\" )", 10*1936.27/13.7603 );
  CHECK_EVAL( "EUROCONVERT(  11; \"ATS\"; \"lux\" )", 11*40.3399/13.7603 );
  CHECK_EVAL( "EUROCONVERT(  12; \"ATS\"; \"nlg\" )", 12*2.20371/13.7603 );
  CHECK_EVAL( "EUROCONVERT(  13; \"ATS\"; \"pte\" )", 13*200.482/13.7603 );
  CHECK_EVAL( "EUROCONVERT(  14; \"BEF\"; \"ats\" )", 14*13.7603/40.3399 );
  CHECK_EVAL( "EUROCONVERT(  15; \"BEF\"; \"dem\" )", 15*1.95583/40.3399 );
  CHECK_EVAL( "EUROCONVERT(  16; \"BEF\"; \"esp\" )", 16*166.386/40.3399 );
  CHECK_EVAL( "EUROCONVERT(  17; \"BEF\"; \"eur\" )", 17*1/40.3399 );
  CHECK_EVAL( "EUROCONVERT(  18; \"BEF\"; \"fim\" )", 18*5.94573/40.3399 );
  CHECK_EVAL( "EUROCONVERT(  19; \"BEF\"; \"frf\" )", 19*6.55957/40.3399 );
  CHECK_EVAL( "EUROCONVERT(  20; \"BEF\"; \"grd\" )", 20*340.75/40.3399 );
  CHECK_EVAL( "EUROCONVERT(  21; \"BEF\"; \"iep\" )", 21*0.787564/40.3399 );
  CHECK_EVAL( "EUROCONVERT(  22; \"BEF\"; \"itl\" )", 22*1936.27/40.3399 );
  CHECK_EVAL( "EUROCONVERT(  23; \"BEF\"; \"lux\" )", 23*40.3399/40.3399 );
  CHECK_EVAL( "EUROCONVERT(  24; \"BEF\"; \"nlg\" )", 24*2.20371/40.3399 );
  CHECK_EVAL( "EUROCONVERT(  25; \"BEF\"; \"pte\" )", 25*200.482/40.3399 );
  CHECK_EVAL( "EUROCONVERT(  26; \"DEM\"; \"ats\" )", 26*13.7603/1.95583 );
  CHECK_EVAL( "EUROCONVERT(  27; \"DEM\"; \"bef\" )", 27*40.3399/1.95583 );
  CHECK_EVAL( "EUROCONVERT(  28; \"DEM\"; \"esp\" )", 28*166.386/1.95583 );
  CHECK_EVAL( "EUROCONVERT(  29; \"DEM\"; \"eur\" )", 29*1/1.95583 );
  CHECK_EVAL( "EUROCONVERT(  30; \"DEM\"; \"fim\" )", 30*5.94573/1.95583 );
  CHECK_EVAL( "EUROCONVERT(  31; \"DEM\"; \"frf\" )", 31*6.55957/1.95583 );
  CHECK_EVAL( "EUROCONVERT(  32; \"DEM\"; \"grd\" )", 32*340.75/1.95583 );
  CHECK_EVAL( "EUROCONVERT(  33; \"DEM\"; \"iep\" )", 33*0.787564/1.95583 );
  CHECK_EVAL( "EUROCONVERT(  34; \"DEM\"; \"itl\" )", 34*1936.27/1.95583 );
  CHECK_EVAL( "EUROCONVERT(  35; \"DEM\"; \"lux\" )", 35*40.3399/1.95583 );
  CHECK_EVAL( "EUROCONVERT(  36; \"DEM\"; \"nlg\" )", 36*2.20371/1.95583 );
  CHECK_EVAL( "EUROCONVERT(  37; \"DEM\"; \"pte\" )", 37*200.482/1.95583 );
  CHECK_EVAL( "EUROCONVERT(  38; \"ESP\"; \"ats\" )", 38*13.7603/166.386 );
  CHECK_EVAL( "EUROCONVERT(  39; \"ESP\"; \"bef\" )", 39*40.3399/166.386 );
  CHECK_EVAL( "EUROCONVERT(  40; \"ESP\"; \"dem\" )", 40*1.95583/166.386 );
  CHECK_EVAL( "EUROCONVERT(  41; \"ESP\"; \"eur\" )", 41*1/166.386 );
  CHECK_EVAL( "EUROCONVERT(  42; \"ESP\"; \"fim\" )", 42*5.94573/166.386 );
  CHECK_EVAL( "EUROCONVERT(  43; \"ESP\"; \"frf\" )", 43*6.55957/166.386 );
  CHECK_EVAL( "EUROCONVERT(  44; \"ESP\"; \"grd\" )", 44*340.75/166.386 );
  CHECK_EVAL( "EUROCONVERT(  45; \"ESP\"; \"iep\" )", 45*0.787564/166.386 );
  CHECK_EVAL( "EUROCONVERT(  46; \"ESP\"; \"itl\" )", 46*1936.27/166.386 );
  CHECK_EVAL( "EUROCONVERT(  47; \"ESP\"; \"lux\" )", 47*40.3399/166.386 );
  CHECK_EVAL( "EUROCONVERT(  48; \"ESP\"; \"nlg\" )", 48*2.20371/166.386 );
  CHECK_EVAL( "EUROCONVERT(  49; \"ESP\"; \"pte\" )", 49*200.482/166.386 );
  CHECK_EVAL( "EUROCONVERT(  50; \"EUR\"; \"ats\" )", 50*13.7603/1 );
  CHECK_EVAL( "EUROCONVERT(  51; \"EUR\"; \"bef\" )", 51*40.3399/1 );
  CHECK_EVAL( "EUROCONVERT(  52; \"EUR\"; \"dem\" )", 52*1.95583/1 );
  CHECK_EVAL( "EUROCONVERT(  53; \"EUR\"; \"esp\" )", 53*166.386/1 );
  CHECK_EVAL( "EUROCONVERT(  54; \"EUR\"; \"fim\" )", 54*5.94573/1 );
  CHECK_EVAL( "EUROCONVERT(  55; \"EUR\"; \"frf\" )", 55*6.55957/1 );
  CHECK_EVAL( "EUROCONVERT(  56; \"EUR\"; \"grd\" )", 56*340.75/1 );
  CHECK_EVAL( "EUROCONVERT(  57; \"EUR\"; \"iep\" )", 57*0.787564/1 );
  CHECK_EVAL( "EUROCONVERT(  58; \"EUR\"; \"itl\" )", 58*1936.27/1 );
  CHECK_EVAL( "EUROCONVERT(  59; \"EUR\"; \"lux\" )", 59*40.3399/1 );
  CHECK_EVAL( "EUROCONVERT(  60; \"EUR\"; \"nlg\" )", 60*2.20371/1 );
  CHECK_EVAL( "EUROCONVERT(  61; \"EUR\"; \"pte\" )", 61*200.482/1 );
  CHECK_EVAL( "EUROCONVERT(  62; \"FIM\"; \"ats\" )", 62*13.7603/5.94573 );
  CHECK_EVAL( "EUROCONVERT(  63; \"FIM\"; \"bef\" )", 63*40.3399/5.94573 );
  CHECK_EVAL( "EUROCONVERT(  64; \"FIM\"; \"dem\" )", 64*1.95583/5.94573 );
  CHECK_EVAL( "EUROCONVERT(  65; \"FIM\"; \"esp\" )", 65*166.386/5.94573 );
  CHECK_EVAL( "EUROCONVERT(  66; \"FIM\"; \"eur\" )", 66*1/5.94573 );
  CHECK_EVAL( "EUROCONVERT(  67; \"FIM\"; \"frf\" )", 67*6.55957/5.94573 );
  CHECK_EVAL( "EUROCONVERT(  68; \"FIM\"; \"grd\" )", 68*340.75/5.94573 );
  CHECK_EVAL( "EUROCONVERT(  69; \"FIM\"; \"iep\" )", 69*0.787564/5.94573 );
  CHECK_EVAL( "EUROCONVERT(  70; \"FIM\"; \"itl\" )", 70*1936.27/5.94573 );
  CHECK_EVAL( "EUROCONVERT(  71; \"FIM\"; \"lux\" )", 71*40.3399/5.94573 );
  CHECK_EVAL( "EUROCONVERT(  72; \"FIM\"; \"nlg\" )", 72*2.20371/5.94573 );
  CHECK_EVAL( "EUROCONVERT(  73; \"FIM\"; \"pte\" )", 73*200.482/5.94573 );
  CHECK_EVAL( "EUROCONVERT(  74; \"FRF\"; \"ats\" )", 74*13.7603/6.55957 );
  CHECK_EVAL( "EUROCONVERT(  75; \"FRF\"; \"bef\" )", 75*40.3399/6.55957 );
  CHECK_EVAL( "EUROCONVERT(  76; \"FRF\"; \"dem\" )", 76*1.95583/6.55957 );
  CHECK_EVAL( "EUROCONVERT(  77; \"FRF\"; \"esp\" )", 77*166.386/6.55957 );
  CHECK_EVAL( "EUROCONVERT(  78; \"FRF\"; \"eur\" )", 78*1/6.55957 );
  CHECK_EVAL( "EUROCONVERT(  79; \"FRF\"; \"fim\" )", 79*5.94573/6.55957 );
  CHECK_EVAL( "EUROCONVERT(  80; \"FRF\"; \"grd\" )", 80*340.75/6.55957 );
  CHECK_EVAL( "EUROCONVERT(  81; \"FRF\"; \"iep\" )", 81*0.787564/6.55957 );
  CHECK_EVAL( "EUROCONVERT(  82; \"FRF\"; \"itl\" )", 82*1936.27/6.55957 );
  CHECK_EVAL( "EUROCONVERT(  83; \"FRF\"; \"lux\" )", 83*40.3399/6.55957 );
  CHECK_EVAL( "EUROCONVERT(  84; \"FRF\"; \"nlg\" )", 84*2.20371/6.55957 );
  CHECK_EVAL( "EUROCONVERT(  85; \"FRF\"; \"pte\" )", 85*200.482/6.55957 );
  CHECK_EVAL( "EUROCONVERT(  86; \"GRD\"; \"ats\" )", 86*13.7603/340.75 );
  CHECK_EVAL( "EUROCONVERT(  87; \"GRD\"; \"bef\" )", 87*40.3399/340.75 );
  CHECK_EVAL( "EUROCONVERT(  88; \"GRD\"; \"dem\" )", 88*1.95583/340.75 );
  CHECK_EVAL( "EUROCONVERT(  89; \"GRD\"; \"esp\" )", 89*166.386/340.75 );
  CHECK_EVAL( "EUROCONVERT(  90; \"GRD\"; \"eur\" )", 90*1/340.75 );
  CHECK_EVAL( "EUROCONVERT(  91; \"GRD\"; \"fim\" )", 91*5.94573/340.75 );
  CHECK_EVAL( "EUROCONVERT(  92; \"GRD\"; \"frf\" )", 92*6.55957/340.75 );
  CHECK_EVAL( "EUROCONVERT(  93; \"GRD\"; \"iep\" )", 93*0.787564/340.75 );
  CHECK_EVAL( "EUROCONVERT(  94; \"GRD\"; \"itl\" )", 94*1936.27/340.75 );
  CHECK_EVAL( "EUROCONVERT(  95; \"GRD\"; \"lux\" )", 95*40.3399/340.75 );
  CHECK_EVAL( "EUROCONVERT(  96; \"GRD\"; \"nlg\" )", 96*2.20371/340.75 );
  CHECK_EVAL( "EUROCONVERT(  97; \"GRD\"; \"pte\" )", 97*200.482/340.75 );
  CHECK_EVAL( "EUROCONVERT(  98; \"IEP\"; \"ats\" )", 98*13.7603/0.787564 );
  CHECK_EVAL( "EUROCONVERT(  99; \"IEP\"; \"bef\" )", 99*40.3399/0.787564 );
  CHECK_EVAL( "EUROCONVERT( 100; \"IEP\"; \"dem\" )", 100*1.95583/0.787564 );
  CHECK_EVAL( "EUROCONVERT( 101; \"IEP\"; \"esp\" )", 101*166.386/0.787564 );
  CHECK_EVAL( "EUROCONVERT( 102; \"IEP\"; \"eur\" )", 102*1/0.787564 );
  CHECK_EVAL( "EUROCONVERT( 103; \"IEP\"; \"fim\" )", 103*5.94573/0.787564 );
  CHECK_EVAL( "EUROCONVERT( 104; \"IEP\"; \"frf\" )", 104*6.55957/0.787564 );
  CHECK_EVAL( "EUROCONVERT( 105; \"IEP\"; \"grd\" )", 105*340.75/0.787564 );
  CHECK_EVAL( "EUROCONVERT( 106; \"IEP\"; \"itl\" )", 106*1936.27/0.787564 );
  CHECK_EVAL( "EUROCONVERT( 107; \"IEP\"; \"lux\" )", 107*40.3399/0.787564 );
  CHECK_EVAL( "EUROCONVERT( 108; \"IEP\"; \"nlg\" )", 108*2.20371/0.787564 );
  CHECK_EVAL( "EUROCONVERT( 109; \"IEP\"; \"pte\" )", 109*200.482/0.787564 );
  CHECK_EVAL( "EUROCONVERT( 110; \"ITL\"; \"ats\" )", 110*13.7603/1936.27 );
  CHECK_EVAL( "EUROCONVERT( 111; \"ITL\"; \"bef\" )", 111*40.3399/1936.27 );
  CHECK_EVAL( "EUROCONVERT( 112; \"ITL\"; \"dem\" )", 112*1.95583/1936.27 );
  CHECK_EVAL( "EUROCONVERT( 113; \"ITL\"; \"esp\" )", 113*166.386/1936.27 );
  CHECK_EVAL( "EUROCONVERT( 114; \"ITL\"; \"eur\" )", 114*1/1936.27 );
  CHECK_EVAL( "EUROCONVERT( 115; \"ITL\"; \"fim\" )", 115*5.94573/1936.27 );
  CHECK_EVAL( "EUROCONVERT( 116; \"ITL\"; \"frf\" )", 116*6.55957/1936.27 );
  CHECK_EVAL( "EUROCONVERT( 117; \"ITL\"; \"grd\" )", 117*340.75/1936.27 );
  CHECK_EVAL( "EUROCONVERT( 118; \"ITL\"; \"iep\" )", 118*0.787564/1936.27 );
  CHECK_EVAL( "EUROCONVERT( 119; \"ITL\"; \"lux\" )", 119*40.3399/1936.27 );
  CHECK_EVAL( "EUROCONVERT( 120; \"ITL\"; \"nlg\" )", 120*2.20371/1936.27 );
  CHECK_EVAL( "EUROCONVERT( 121; \"ITL\"; \"pte\" )", 121*200.482/1936.27 );
  CHECK_EVAL( "EUROCONVERT( 122; \"LUX\"; \"ats\" )", 122*13.7603/40.3399 );
  CHECK_EVAL( "EUROCONVERT( 123; \"LUX\"; \"bef\" )", 123*40.3399/40.3399 );
  CHECK_EVAL( "EUROCONVERT( 124; \"LUX\"; \"dem\" )", 124*1.95583/40.3399 );
  CHECK_EVAL( "EUROCONVERT( 125; \"LUX\"; \"esp\" )", 125*166.386/40.3399 );
  CHECK_EVAL( "EUROCONVERT( 126; \"LUX\"; \"eur\" )", 126*1/40.3399 );
  CHECK_EVAL( "EUROCONVERT( 127; \"LUX\"; \"fim\" )", 127*5.94573/40.3399 );
  CHECK_EVAL( "EUROCONVERT( 128; \"LUX\"; \"frf\" )", 128*6.55957/40.3399 );
  CHECK_EVAL( "EUROCONVERT( 129; \"LUX\"; \"grd\" )", 129*340.75/40.3399 );
  CHECK_EVAL( "EUROCONVERT( 130; \"LUX\"; \"iep\" )", 130*0.787564/40.3399 );
  CHECK_EVAL( "EUROCONVERT( 131; \"LUX\"; \"itl\" )", 131*1936.27/40.3399 );
  CHECK_EVAL( "EUROCONVERT( 132; \"LUX\"; \"nlg\" )", 132*2.20371/40.3399 );
  CHECK_EVAL( "EUROCONVERT( 133; \"LUX\"; \"pte\" )", 133*200.482/40.3399 );
  CHECK_EVAL( "EUROCONVERT( 134; \"NLG\"; \"ats\" )", 134*13.7603/2.20371 );
  CHECK_EVAL( "EUROCONVERT( 135; \"NLG\"; \"bef\" )", 135*40.3399/2.20371 );
  CHECK_EVAL( "EUROCONVERT( 136; \"NLG\"; \"dem\" )", 136*1.95583/2.20371 );
  CHECK_EVAL( "EUROCONVERT( 137; \"NLG\"; \"esp\" )", 137*166.386/2.20371 );
  CHECK_EVAL( "EUROCONVERT( 138; \"NLG\"; \"eur\" )", 138*1/2.20371 );
  CHECK_EVAL( "EUROCONVERT( 139; \"NLG\"; \"fim\" )", 139*5.94573/2.20371 );
  CHECK_EVAL( "EUROCONVERT( 140; \"NLG\"; \"frf\" )", 140*6.55957/2.20371 );
  CHECK_EVAL( "EUROCONVERT( 141; \"NLG\"; \"grd\" )", 141*340.75/2.20371 );
  CHECK_EVAL( "EUROCONVERT( 142; \"NLG\"; \"iep\" )", 142*0.787564/2.20371 );
  CHECK_EVAL( "EUROCONVERT( 143; \"NLG\"; \"itl\" )", 143*1936.27/2.20371 );
  CHECK_EVAL( "EUROCONVERT( 144; \"NLG\"; \"lux\" )", 144*40.3399/2.20371 );
  CHECK_EVAL( "EUROCONVERT( 145; \"NLG\"; \"pte\" )", 145*200.482/2.20371 );
  CHECK_EVAL( "EUROCONVERT( 146; \"PTE\"; \"ats\" )", 146*13.7603/200.482 );
  CHECK_EVAL( "EUROCONVERT( 147; \"PTE\"; \"bef\" )", 147*40.3399/200.482 );
  CHECK_EVAL( "EUROCONVERT( 148; \"PTE\"; \"dem\" )", 148*1.95583/200.482 );
  CHECK_EVAL( "EUROCONVERT( 149; \"PTE\"; \"esp\" )", 149*166.386/200.482 );
  CHECK_EVAL( "EUROCONVERT( 150; \"PTE\"; \"eur\" )", 150*1/200.482 );
  CHECK_EVAL( "EUROCONVERT( 151; \"PTE\"; \"fim\" )", 151*5.94573/200.482 );
  CHECK_EVAL( "EUROCONVERT( 152; \"PTE\"; \"frf\" )", 152*6.55957/200.482 );
  CHECK_EVAL( "EUROCONVERT( 153; \"PTE\"; \"grd\" )", 153*340.75/200.482 );
  CHECK_EVAL( "EUROCONVERT( 154; \"PTE\"; \"iep\" )", 154*0.787564/200.482 );
  CHECK_EVAL( "EUROCONVERT( 155; \"PTE\"; \"itl\" )", 155*1936.27/200.482 );
  CHECK_EVAL( "EUROCONVERT( 156; \"PTE\"; \"lux\" )", 156*40.3399/200.482 );
  CHECK_EVAL( "EUROCONVERT( 157; \"PTE\"; \"nlg\" )", 157*2.20371/200.482 );
}

// INTRATE
void TestFinancialFunctions::testINTRATE()
{
  // ODF
  CHECK_EVAL_SHORT( "INTRATE( DATE(2002; 6;8); DATE(1995;10;5); 100000; 200000; 0 )" , Value::errorVALUE() ); // Settlement date must be before the maturity date.
  CHECK_EVAL_SHORT( "INTRATE( DATE(2002; 6;8); DATE(2002; 6;8); 100000; 200000; 0 )" , Value::errorVALUE() ); // Settlement date must be before the maturity date.
  CHECK_EVAL_SHORT( "INTRATE( DATE(1995;10;5); DATE(2002; 6;8); 100000; 200000; 50)" , Value::errorVALUE() ); // Unknown Basis returns Error.
  CHECK_EVAL_SHORT( "INTRATE( DATE(1995;10;5); DATE(2002; 6;8); 100000; 200000; 0 )" , Value( 0.14981 ) );    // An example of INTRATE.
  CHECK_EVAL_SHORT( "INTRATE( DATE(1995;10;5); DATE(2002; 6;8); 100000; 200000    )" , Value( 0.14981 ) );    // Basis defaults to 0.
  CHECK_EVAL_SHORT( "INTRATE( DATE(1995;10;5); DATE(2002; 6;8); 100000; 200000; 1 )" , Value( 0.14971 ) );    //
  CHECK_EVAL_SHORT( "INTRATE( DATE(1995;10;5); DATE(2002; 6;8); 100000; 200000; 2 )" , Value( 0.14766 ) );    //
  CHECK_EVAL_SHORT( "INTRATE( DATE(1995;10;5); DATE(2002; 6;8); 100000; 200000; 3 )" , Value( 0.14971 ) );    //
  CHECK_EVAL_SHORT( "INTRATE( DATE(1995;10;5); DATE(2002; 6;8); 100000; 200000; 4 )" , Value( 0.14981 ) );    //
}

// ISPMT
void TestFinancialFunctions::testISPMT()
{
  // ODF
  CHECK_EVAL_SHORT( "ISPMT(5%/12;12;360;100000)", Value( -402.78 ) ); // A trivial example of ISPMT.  A 100000 unit investment with an
                                                                      // annual interest rate of 5% and a 30 year term has an interest payment 
                                                                      // of 402.78 units in month 12.
}

// Level-coupon bond
// LEVEL_COUPON(faceValue; couponRate; couponsPerYear; years; marketRate)
void TestFinancialFunctions::testLEVELCOUPON()
{
  CHECK_EVAL( "LEVEL_COUPON(1000; .13; 1; 4; .1)",   1095.0959633904788 );
  CHECK_EVAL( "LEVEL_COUPON(1000; .13; 2; 4; .1)",   1096.9481913913939 );
  CHECK_EVAL( "LEVEL_COUPON(1000; .10; 1; 10; .25)",  464.4245094400000 );
  CHECK_EVAL( "LEVEL_COUPON(1000; .12; 1; 10; .25)",  535.8345748480000 );
  CHECK_EVAL( "LEVEL_COUPON(1000; .20; 1; 10; .25)",  821.4748364800000 );
}

// Yearly nominal interest rate
// NOMINAL(effectiveRate, periods)
void TestFinancialFunctions::testNOMINAL()
{
  CHECK_EVAL( "NOMINAL(13.5%; 12)",  0.1273031669590416 );
  CHECK_EVAL( "NOMINAL(13.5%; 12)",  0.1273031669590416 );
  CHECK_EVAL( "NOMINAL(25%; 12)",  0.2252311814580734 );
  CHECK_EVAL( "NOMINAL(25%; 4)",  0.2294850537622564 );
  CHECK_EVAL( "NOMINAL(20%; 12)",  0.1837136459967743 );
  CHECK_EVAL( "NOMINAL(10%; 12)",  0.0956896851468452 );

  // rate must be positive
  CHECK_EVAL( "NOMINAL(0; 12)", Value::errorVALUE());

  // periods must be positive
  CHECK_EVAL( "NOMINAL(10%; 0)", Value::errorDIV0());
  CHECK_EVAL( "NOMINAL(10%; -1)", Value::errorVALUE());
  CHECK_EVAL( "NOMINAL(10%; -2)", Value::errorVALUE());

  // test cases in OpenFormula specification
  CHECK_EVAL( "NOMINAL(8%;4)", 0.0777061876330940 );
  CHECK_EVAL( "NOMINAL(12.5%;12)", 0.118362966638538 );
  CHECK_EVAL( "NOMINAL(1%;2)",  0.00997512422417790 );
}

// NPER
void TestFinancialFunctions::testNPER()
{
  // ODF
  CHECK_EVAL_SHORT( "NPER(5% ;-100;1000)",       Value( 14.2067     ) ); // A trivial example of NPER.
  CHECK_EVAL_SHORT( "NPER(5% ;-100;1000;100)",   Value( 15.2067     ) ); // A trivial example of NPER with non-zero FV.
  CHECK_EVAL_SHORT( "NPER(5% ;-100;1000;100;1)", Value( 14.2067     ) ); // A trivial example of NPER with non-zero FV and PayType.
  CHECK_EVAL_SHORT( "NPER(0  ;-100;1000)",       Value( 10.0000     ) ); // TODO Rate can be zero.
  CHECK_EVAL_SHORT( "NPER(-1%;-100;1000)",       Value( 9.483283066 ) ); // TODO Rate can be negative.
}

// Net present value
// NPV(rate, values)
void TestFinancialFunctions::testNPV()
{
    CHECK_EVAL( "NPV(100%; 4; 5; 7)", 4.125 );
    CHECK_EVAL( "NPV(10%; 100; 200)", 256.198347107438 );
}

// PMT
void TestFinancialFunctions::testPMT()
{
  // ODF
  CHECK_EVAL_SHORT( "PMT(5%;12;1000)",       Value( -112.82541  ) ); // A trivial example of PMT.
  CHECK_EVAL_SHORT( "PMT(5%;12;1000;100)",   Value( -119.107951 ) ); // A trivial example of PMT with non-zero FV.
  CHECK_EVAL_SHORT( "PMT(5%;12;1000;100;1)", Value( -113.43614  ) ); // A trivial example of PMT with non-zero FV and PayType.
  CHECK_EVAL_SHORT( "PMT(0;10;1000)",        Value( -100.00000  ) ); // TODO Rate can be zero.
}

// PRICEMAT
void TestFinancialFunctions::testPRICEMAT()
{
  // ODF
  CHECK_EVAL_SHORT( "PRICEMAT(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);6%;5%  )", Value( 103.819218241  ) ); // Without Basis parameter
  CHECK_EVAL_SHORT( "PRICEMAT(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);6%;5%;1)", Value( 103.824693325  ) ); // With Basis=1
  CHECK_EVAL_SHORT( "PRICEMAT(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);6%;5%;2)", Value( 103.858482159  ) ); // With Basis=2
  CHECK_EVAL_SHORT( "PRICEMAT(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);6%;5%;3)", Value( 103.824693325  ) ); // With Basis=3
  CHECK_EVAL_SHORT( "PRICEMAT(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);6%;5%;4)", Value( 103.817732653  ) ); // With Basis=4
  CHECK_EVAL_SHORT( "PRICEMAT(DATE(1990;6;1);DATE(1992;12;31);DATE(1990;1;1);3%;2%;0)", Value( 102.395007924  ) ); //
}

// Straight-line depreciation
// SLN(cost, salvage, life)
void TestFinancialFunctions::testSLN()
{
  // Excel example: http://office.microsoft.com/en-us/excel/HP100623811033.aspx
  CHECK_EVAL( "SLN(30000; 7500; 10)", 2250.0 ) ;

  // http://www.vni.com/products/imsl/jmsl/v30/api/com/imsl/finance/slnEx1.html
  CHECK_EVAL( "SLN(2500; 500; 24)", 83.3333333333333 ) ;

  // http://www.gnome.org/projects/gnumeric/doc/gnumeric-SLN.shtml
  CHECK_EVAL( "SLN(10000; 700; 10)", 930 );

  // test cases in OpenFormula specification
  CHECK_EVAL( "SLN(4000;500;4)", 875);
}

// Sum-of-years' digits depreciation
// SYD(cost, salvage, life, period)
void TestFinancialFunctions::testSYD()
{
  // Excel example: http://office.microsoft.com/en-us/excel/HP100623821033.aspx
  CHECK_EVAL( "SYD(30000; 7500; 10; 1)",  4090.909090909090 ) ;
  CHECK_EVAL( "SYD(30000; 7500; 10; 10)", 409.0909090909090 ) ;

  // http://www.vni.com/products/imsl/jmsl/v30/api/com/imsl/finance/sydEx1.html
  CHECK_EVAL( "SYD(25000; 5000; 15; 14)", 333.3333333333333 ) ;

  // http://www.gnome.org/projects/gnumeric/doc/gnumeric-SYD.shtml
  CHECK_EVAL( "SYD(5000; 200; 5; 2)", 1280 );

  // test cases in OpenFormula specification
  CHECK_EVAL( "SYD(4000;500;4;2)", 1050 );
}

// TBILLPRICE
void TestFinancialFunctions::testTBILLPRICE()
{
  // ODF
  CHECK_EVAL_SHORT( "TBILLPRICE(DATE(1996;1;1);DATE(1996;2;1);5%)" ,     Value( 99.56944444 ) ); //
  CHECK_EVAL_SHORT( "TBILLPRICE(DATE(1995;12;31);DATE(1996;2;1);5%)" ,   Value( 99.55555556 ) ); //
  CHECK_EVAL_SHORT( "TBILLPRICE(DATE(1995;12;31);DATE(1996;7;1);5%)" ,   Value( 97.47222222 ) ); // ODF specs 97.45833333 OOo-2.2.1 97.47222222 
  CHECK_EVAL_SHORT( "TBILLPRICE(DATE(1995;12;31);DATE(1996;12;31);5%)" , Value( 94.98611111 ) ); // ODF specs 94.91666667 OOo-2.2.1 94.98611111
  CHECK_EVAL_SHORT( "TBILLPRICE(DATE(1996;1;1);DATE(1996;6;30);5%)" ,    Value( 97.50000000 ) ); // ODF specs 97.48611111 OOo-2.2.1 97.50000000
  CHECK_EVAL_SHORT( "TBILLPRICE(DATE(1996;1;1);DATE(1996;7;1);5%)" ,     Value( 97.48611111 ) ); // ODF specs 97.47222222 OOo-2.2.1 97.48611111
  CHECK_EVAL_SHORT( "TBILLPRICE(DATE(1996;1;1);DATE(1996;12;31);5%)" ,   Value( Value::errorVALUE() ) ); // ODF specs 94.93055556 OOo-2.2.1 Err:502
  CHECK_EVAL_SHORT( "TBILLPRICE(DATE(1996;1;1);DATE(1997;1;1);5%)" ,     Value( 94.98611111 ) ); // ODF specs 94.91666667 OOo-2.2.1 94.98611111
  CHECK_EVAL_SHORT( "TBILLPRICE(DATE(1996;7;1);DATE(1997;7;1);5%)" ,     Value( 94.98611111 ) ); // ODF specs 94.93055556 OOo-2.2.1 94.98611111
}

// YIELDDISC
void TestFinancialFunctions::testYIELDDISC()
{
  // ODF
  CHECK_EVAL_SHORT( "YIELDDISC(DATE(1990;6;1);DATE(1990;12;31);941.66667;1000)", Value( 0.106194684  ) ); // Without Basis parameter
  CHECK_EVAL_SHORT( "YIELDDISC(DATE(1990;6;1);DATE(1990;12;31);941.64384;1000;1)", Value( 0.106238821  ) ); // With Basis=1
  CHECK_EVAL_SHORT( "YIELDDISC(DATE(1990;6;1);DATE(1990;12;31);940.83333;1000;2)", Value( 0.107807168  ) ); // With Basis=2
  CHECK_EVAL_SHORT( "YIELDDISC(DATE(1990;6;1);DATE(1990;12;31);941.64384;1000;3)", Value( 0.106238821  ) ); // With Basis=3
  CHECK_EVAL_SHORT( "YIELDDISC(DATE(1990;6;1);DATE(1990;12;31);941.94444;1000;4)", Value( 0.105657842  ) ); // With Basis=4
  CHECK_EVAL_SHORT( "YIELDDISC(DATE(1990;1;1);DATE(1990;12;31);97.08219;100;1)", Value( 0.051522942  ) ); //
  CHECK_EVAL_SHORT( "YIELDDISC(DATE(1990;6;1);DATE(1990;6;30);99.75833;100;4)", Value( 0.030073091  ) ); //
}

// Zero-coupon (pure discount) bond
// ZERO_COUPON(faceValue; rate; years)
void TestFinancialFunctions::testZEROCOUPON()
{
  CHECK_EVAL( "ZERO_COUPON(1000;.1;20)",  148.6436280241434531 );
  CHECK_EVAL( "ZERO_COUPON(1000;.2;20)",  26.0840533045888456 );
  CHECK_EVAL( "ZERO_COUPON(1000;.15/12;10)",  883.1809261539680165 );
  CHECK_EVAL( "ZERO_COUPON(1000;.25;1)",  800 );
}


QTEST_KDEMAIN(TestFinancialFunctions, GUI)

#include "TestFinancialFunctions.moc"
