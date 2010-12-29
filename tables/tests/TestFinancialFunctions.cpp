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
#include "TestFinancialFunctions.h"

#include "TestKspreadCommon.h"

void TestFinancialFunctions::initTestCase()
{
    FunctionModuleRegistry::instance()->loadFunctionModules();
}

// NOTE: we do not compare the numbers _exactly_ because it is difficult
// to get one "true correct" expected values for the functions due to:
//  - different algorithms among spreadsheet programs
//  - precision limitation of floating-point number representation
//  - accuracy problem due to propagated error in the implementation

#define CHECK_EVAL(x,y) QCOMPARE(TestDouble(x,y,6),y)
#define CHECK_EVAL_SHORT(x,y) QCOMPARE(TestDouble(x,y,11),y)

static Value TestDouble(const QString& formula, const Value& v2, int accuracy)
{
    double epsilon = DBL_EPSILON * pow(10.0, (double)(accuracy));

    Formula f;
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

// ACCRINT
void TestFinancialFunctions::testACCRINT()
{
    // odf test
    CHECK_EVAL("ACCRINT( \"1992-12-01\"; \"1993-06-01\"; \"1993-07-01\";  0.055; 100 ; 2; 0 ) ", Value(3.2083333333));
    CHECK_EVAL("ACCRINT( \"2001-02-28\"; \"2001-08-31\";\"2001-05-01\";  0.1  ; 1000; 2; 0 )",
               Value(16.9444444444));    // A security is issued on 2.28.2001.
    // First interest is set for 8.31.2001. The settlement date is 5.1.2001.
    // The Rate is 0.1 or 10% and Par is 1000 currency units. Interest is paid
    // half-yearly (frequency is 2). The basis is the US method (0). How much interest has accrued?
    CHECK_EVAL("ACCRINT( \"2004-02-01\"; \"2004-04-01\"; \"2004-05-01\"; 0.1; 1000; 4; 0 )", Value(24.7222222222));     // leap year, quaterly, US (NASD) 30/360
    CHECK_EVAL_SHORT("ACCRINT( \"2004-02-01\"; \"2004-04-01\"; \"2004-05-01\"; 0.1; 1000; 4; 1 )", Value(24.590164));         // leap year, quaterly, actual/acual
    CHECK_EVAL("ACCRINT( \"2004-02-01\"; \"2004-04-01\"; \"2004-05-01\"; 0.1; 1000; 4; 2 )", Value(25));                // leap year, quaterly, actual/360
    CHECK_EVAL_SHORT("ACCRINT( \"2004-02-01\"; \"2004-04-01\"; \"2004-05-01\"; 0.1; 1000; 4; 3 )", Value(24.657534));         // leap year, quaterly, actual/365
    CHECK_EVAL("ACCRINT( \"2004-02-01\"; \"2004-04-01\"; \"2004-05-01\"; 0.1; 1000; 4; 4 )", Value(25));                // leap year, quaterly, European 30/360
    CHECK_EVAL("ACCRINT( \"2004-02-01\"; \"2004-04-01\"; \"2004-05-01\"; 0.1; 1000; 1 )",    Value(24.7222222222));     // leap year, annual, US (NASD) 30/360
    CHECK_EVAL("ACCRINT( \"2004-02-01\"; \"2004-04-01\"; \"2004-05-01\"; 0.1; 1000; 2 )",    Value(24.7222222222));     // leap year, semiannual, US 30/360
    // alternate function name
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETACCRINT( \"1992-12-01\";\"1993-06-01\";\"1993-07-01\";0.055;100;2;0)", Value(3.2083333333));
}

// ACCRINTM
void TestFinancialFunctions::testACCRINTM()
{
    // kspread
    CHECK_EVAL_SHORT("ACCRINTM( \"2001-04-01\"; \"2001-06-15\"; 0.1; 1000; 3 )", Value(20.5479454));
    CHECK_EVAL_SHORT("ACCRINTM( \"2004-02-01\"; \"2004-05-01\"; 0.1; 1000; 0 )", Value(24.722222));      // leap year, US (NASD) 30/360
    CHECK_EVAL_SHORT("ACCRINTM( \"2004-02-01\"; \"2004-05-01\"; 0.1; 1000; 1 )", Value(24.590164));      // leap year, actual/actual
    CHECK_EVAL_SHORT("ACCRINTM( \"2004-02-01\"; \"2004-05-01\"; 0.1; 1000; 2 )", Value(25.0));           // leap year, actual/360
    CHECK_EVAL_SHORT("ACCRINTM( \"2004-02-01\"; \"2004-05-01\"; 0.1; 1000; 3 )", Value(24.657534));      // leap year, actual/365
    CHECK_EVAL_SHORT("ACCRINTM( \"2004-02-01\"; \"2004-05-01\"; 0.1; 1000; 4 )", Value(25.0));           // leap year, European 30/360
    // alternate function name
    CHECK_EVAL_SHORT("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETACCRINTM( \"2001-04-01\"; \"2001-06-15\"; 0.1; 1000; 3 )", Value(20.5479454));
}

// AMORDEGRC
void TestFinancialFunctions::testAMORDEGRC()
{
    // Excel Formen und Funktionen
    CHECK_EVAL("AMORDEGRC( 2400; 34199; 34334; 300; 1; 0.15; 1 )" , Value(775));

    // bettersolution.com
    CHECK_EVAL("AMORDEGRC( 50000; \"2003-01-01\"; \"2003-12-31\"; 500; 1; 0.15; 1 )" , Value(11738));     //
    CHECK_EVAL("AMORDEGRC( 50000; \"2003-01-01\"; \"2003-12-31\"; 500; 2; 0.15; 1 )" , Value(7336));      //
    CHECK_EVAL("AMORDEGRC( 50000; \"2003-01-01\"; \"2003-12-31\"; 500; 3; 0.15; 1 )" , Value(4585));      //
    CHECK_EVAL("AMORDEGRC( 50000; \"2003-01-01\"; \"2003-12-31\"; 500; 4; 0.15; 1 )" , Value(2866));      //
//   CHECK_EVAL_SHORT( "AMORDEGRC( 50000; \"2003-01-01\"; \"2003-12-31\"; 500; 5; 0.15; 1 )" , Value(  2388 ) ); // TODO check KSpread -> 1791

    // odf tests
    CHECK_EVAL("AMORDEGRC( 1000; \"2006-02-01\"; \"2006-12-31\"; 10; 0; 0.1; 1 )"  , Value(228));     // the first period (10 years life time)
    CHECK_EVAL("AMORDEGRC( 1000; \"2006-02-01\"; \"2006-12-31\"; 10; 8; 0.1; 1 )"  , Value(26));      // (specs. 52) the period before last (10 years)
    CHECK_EVAL("AMORDEGRC( 1000; \"2006-02-01\"; \"2006-12-31\"; 10; 9; 0.1; 1 )"  , Value(19));      // (specs. 52) the last period (10 years life time)
    CHECK_EVAL("AMORDEGRC( 1000; \"2006-02-01\"; \"2006-12-31\"; 10; 10; 0.1; 1 )" , Value(15));      // (specs. 15) - beyond life time (10 years life time)
    CHECK_EVAL("AMORDEGRC( 1000; \"2006-02-01\"; \"2006-12-31\"; 10; 0; 0.25; 1 )" , Value(342));     // the first period (4 years life time)
    CHECK_EVAL("AMORDEGRC( 1000; \"2006-02-01\"; \"2006-12-31\"; 10; 0; 0.1; 0 )"  , Value(229));     // leap year, US (NASD) 30/360
    CHECK_EVAL("AMORDEGRC( 1000; \"2006-02-01\"; \"2006-12-31\"; 10; 0; 0.1; 1 )"  , Value(228));     // leap year, actual/actual
    CHECK_EVAL("AMORDEGRC( 1000; \"2006-02-01\"; \"2006-12-31\"; 10; 0; 0.1; 2 )"  , Value(231));     // (specs 232) leap year, actual/360
    CHECK_EVAL("AMORDEGRC( 1000; \"2006-02-01\"; \"2006-12-31\"; 10; 0; 0.1; 3 )"  , Value(228));     // leap year, actual/365
    CHECK_EVAL("AMORDEGRC( 1000; \"2006-02-01\"; \"2006-12-31\"; 10; 0; 0.1; 4 )"  , Value(228));     // leap year, European 30/360
    
    // alternate function name
    CHECK_EVAL_SHORT("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETAMORDEGRC(2400;34199;34334;300;1;0.15;1)", Value(775));
}

// AMORLINC
void TestFinancialFunctions::testAMORLINC()
{
    CHECK_EVAL_SHORT("AMORLINC( 1000; \"2004-02-01\"; \"2004-12-31\"; 10; 0; 0.1; 1 )" , Value(91.2568306011));     // the first period (10 years life time)
    CHECK_EVAL_SHORT("AMORLINC( 1000; \"2006-02-01\"; \"2006-12-31\"; 10; 0; 0.1; 3 )" , Value(91.2328767123));     // leap year, actual/365
    CHECK_EVAL_SHORT("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETAMORLINC(1000;\"2004-02-01\";\"2004-12-31\";10;0;0.1;1)", Value(91.2568306011));
}

// COMPOUND
void TestFinancialFunctions::testCOMPOUND()
{
    // kspread
    CHECK_EVAL_SHORT("COMPOUND(5000;0.12;4;5)", Value(9030.556173));
}

// CONTINUOUS
void TestFinancialFunctions::testCONTINUOUS()
{
    // kspread
    CHECK_EVAL_SHORT("CONTINUOUS(1000;0.1;1)", Value(1105.17091808));
}

// COUPNUM
void TestFinancialFunctions::testCOUPNUM()
{
    // ODF
    CHECK_EVAL_SHORT("COUPNUM( \"2004-01-01\"; \"2007-01-01\"; 1; 1 )", Value(3));      // Annual
    CHECK_EVAL_SHORT("COUPNUM( \"2004-01-01\"; \"2007-01-01\"; 2; 1 )", Value(6));      // Semiannual
    CHECK_EVAL_SHORT("COUPNUM( \"2004-01-01\"; \"2007-01-01\"; 4; 1 )", Value(12));     // Quarterly
    CHECK_EVAL_SHORT("COUPNUM( \"2004-02-01\"; \"2009-01-01\"; 4; 0 )", Value(20));     //
    CHECK_EVAL_SHORT("COUPNUM( \"2004-02-01\"; \"2009-01-01\"; 4; 1 )", Value(20));     //
    CHECK_EVAL_SHORT("COUPNUM( \"2004-02-01\"; \"2009-01-01\"; 4; 2 )", Value(20));     //
    CHECK_EVAL_SHORT("COUPNUM( \"2004-02-01\"; \"2009-01-01\"; 4; 3 )", Value(20));     //
    CHECK_EVAL_SHORT("COUPNUM( \"2004-02-01\"; \"2009-01-01\"; 4; 4 )", Value(20));     //
    // alternate function name
    CHECK_EVAL_SHORT("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETCOUPNUM(\"2004-01-01\";\"2007-01-01\";1;1)", Value(3));
}

// CUMIPMT
void TestFinancialFunctions::testCUMIPMT()
{
    // ODF
    CHECK_EVAL_SHORT("CUMIPMT( 0.06/12; 5*12; 100000; 5; 12; 0 )",  Value(-3562.187023));            // maturity at the end of a period
    CHECK_EVAL_SHORT("CUMIPMT( 0.06/12; 5*12; 100000; 5; 12; 1 )",  Value(-3544.464699));            // maturity at the beginning of a period
    CHECK_EVAL_SHORT("CUMIPMT( 0.06/12; 5*12; 100000; 0; 0; 0 )",   Value(Value::errorVALUE()));     // start > 0; end > 0
    CHECK_EVAL_SHORT("CUMIPMT( 0.06/12; 5*12; 100000; 5; 61; 0 )",  Value(Value::errorVALUE()));     // end > periods
    CHECK_EVAL_SHORT("CUMIPMT( 0.06/12; 5*12; 100000; 15; 12; 0 )", Value(Value::errorVALUE()));     // start > end
    // alternate function name
    CHECK_EVAL_SHORT("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETCUMIPMT(0.06/12;5*12;100000;5;12;0)", Value(-3562.187023));
}

// CUMPRINC
void TestFinancialFunctions::testCUMPRINC()
{
    // ODF
    CHECK_EVAL_SHORT("CUMPRINC( 0.06/12; 5*12; 100000; 5; 12; 0 )", Value(-11904.054201));           // maturity at the end of a period
    CHECK_EVAL_SHORT("CUMPRINC( 0.06/12; 5*12; 100000; 5; 12; 1 )", Value(-11844.830051));           // maturity at the beginning of a period
    CHECK_EVAL_SHORT("CUMPRINC( 0.06/12; 5*12; 100000; 0;  0; 0 )", Value(Value::errorVALUE()));     // start > 0; end > 0
    CHECK_EVAL_SHORT("CUMPRINC( 0.06/12; 5*12; 100000; 5; 61; 0 )", Value(Value::errorVALUE()));     // end > periods
    CHECK_EVAL_SHORT("CUMPRINC( 0.06/12; 5*12; 100000;15; 12; 0 )", Value(Value::errorVALUE()));     // start > end
    // alternate function name
    CHECK_EVAL_SHORT("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETCUMPRINC(0.06/12;5*12;100000;5;12;0)", Value(-11904.054201));
}

// Fixed-declining balance depreciation
// DB(cost, salvage, life, period, month)
void TestFinancialFunctions::testDB()
{
    // Excel example: http://office.microsoft.com/en-us/excel/HP100623551033.aspx
    CHECK_EVAL("DB(1000000; 100000; 6; 1; 7)", Value(186083.3333333333));
    CHECK_EVAL("DB(1000000; 100000; 6; 2; 7)", Value(259639.4166666667));
    CHECK_EVAL("DB(1000000; 100000; 6; 3; 7)", Value(176814.4427500000));
    CHECK_EVAL("DB(1000000; 100000; 6; 4; 7)", Value(120410.6355127500));
    CHECK_EVAL("DB(1000000; 100000; 6; 5; 7)", Value(81999.64278418274));
    CHECK_EVAL("DB(1000000; 100000; 6; 6; 7)", Value(55841.75673602846));
    CHECK_EVAL("DB(1000000; 100000; 6; 7; 7)", Value(15845.09847384807));

    // http://www.vni.com/products/imsl/jmsl/v30/api/com/imsl/finance/dbEx1.html
    CHECK_EVAL("DB(2500; 500; 3; 1; 6)", Value(518.750000000000));
    CHECK_EVAL("DB(2500; 500; 3; 2; 6)", Value(822.218750000000));
    CHECK_EVAL("DB(2500; 500; 3; 3; 6)", Value(480.997968750000));
    CHECK_EVAL("DB(2500; 500; 3; 4; 6)", Value(140.691905859375));

    // test cases in OpenFormula specification
    CHECK_EVAL("DB(4000;500;4;2)",   Value(963.90));
    CHECK_EVAL("DB(4000;500;4;2;2)", Value(1510.65));
    CHECK_EVAL("DB(4000;500;4;5)",   Value(0.0));
    CHECK_EVAL("DB(0;500;4;2)",      Value(Value::errorNUM()));
    CHECK_EVAL("DB(4000;-500;4;2)",  Value(Value::errorNUM()));
    CHECK_EVAL("DB(4000;500;0;0)",   Value(Value::errorNUM()));
    CHECK_EVAL("DB(4000;500;2;0)",   Value(Value::errorNUM()));
}

// Double declining balance depreciation
// DDB(cost, salvage, life, period, factor)
void TestFinancialFunctions::testDDB()
{
    // Excel example: http://office.microsoft.com/en-us/excel/HP100623561033.aspx
    CHECK_EVAL("DDB(2400; 300; 10*365; 1; 2)", Value(1.31506849315065));
    CHECK_EVAL("DDB(2400; 300; 10*12; 1; 2)",  Value(40.0));
    CHECK_EVAL("DDB(2400; 300; 10; 1; 2)",     Value(480.0));
    CHECK_EVAL("DDB(2400; 300; 10; 2; 1.5)",   Value(306));
    CHECK_EVAL("DDB(2400; 300; 10; 10; 2)",    Value(22.1225472000002));

    // http://www.vni.com/products/imsl/jmsl/v30/api/com/imsl/finance/ddbEx1.html
    CHECK_EVAL("DDB(2500; 500; 24; 1; 2)",  Value(208.333333333333));
    CHECK_EVAL("DDB(2500; 500; 24; 2; 2)",  Value(190.972222222222));
    CHECK_EVAL("DDB(2500; 500; 24; 3; 2)",  Value(175.057870370370));
    CHECK_EVAL("DDB(2500; 500; 24; 4; 2)",  Value(160.469714506173));
    CHECK_EVAL("DDB(2500; 500; 24; 5; 2)",  Value(147.097238297325));
    CHECK_EVAL("DDB(2500; 500; 24; 6; 2)",  Value(134.839135105881));
    CHECK_EVAL("DDB(2500; 500; 24; 7; 2)",  Value(123.602540513725));
    CHECK_EVAL("DDB(2500; 500; 24; 8; 2)",  Value(113.302328804248));
    CHECK_EVAL("DDB(2500; 500; 24; 9; 2)",  Value(103.860468070560));
    CHECK_EVAL("DDB(2500; 500; 24; 10; 2)", Value(95.2054290646802));
    CHECK_EVAL("DDB(2500; 500; 24; 11; 2)", Value(87.2716433092901));
    CHECK_EVAL("DDB(2500; 500; 24; 12; 2)", Value(79.9990063668494));
    CHECK_EVAL("DDB(2500; 500; 24; 13; 2)", Value(73.3324225029452));
    CHECK_EVAL("DDB(2500; 500; 24; 14; 2)", Value(67.2213872943665));
    CHECK_EVAL("DDB(2500; 500; 24; 15; 2)", Value(61.6196050198359));
    CHECK_EVAL("DDB(2500; 500; 24; 16; 2)", Value(56.4846379348497));
    CHECK_EVAL("DDB(2500; 500; 24; 17; 2)", Value(51.7775847736120));
    CHECK_EVAL("DDB(2500; 500; 24; 18; 2)", Value(47.4627860424778));
    CHECK_EVAL("DDB(2500; 500; 24; 19; 2)", Value(22.0906464672553));
    CHECK_EVAL("DDB(2500; 500; 24; 20; 2)", Value(0));
    CHECK_EVAL("DDB(2500; 500; 24; 21; 2)", Value(0));
    CHECK_EVAL("DDB(2500; 500; 24; 22; 2)", Value(0));
    CHECK_EVAL("DDB(2500; 500; 24; 23; 2)", Value(0));
    CHECK_EVAL("DDB(2500; 500; 24; 24; 2)", Value(0));

    // test cases in OpenFormula specification
    CHECK_EVAL("DDB(4000; 500; 4; 2; 2)",   Value(1000));
    CHECK_EVAL("DDB(4000; 500; 4; 2)",      Value(1000));
    CHECK_EVAL("DDB(1100; 100; 5; 5; 2.3 )", Value(0));

    // try default factor (=2)
    CHECK_EVAL("DDB(2400; 300; 10*12; 1)", Value(40.0));
    CHECK_EVAL("DDB(2400; 300; 10; 1)",   Value(480.0));
    CHECK_EVAL("DDB(2500; 500; 24; 22)",  Value(0));
    CHECK_EVAL("DDB(2500; 500; 24; 23)",  Value(0));
    CHECK_EVAL("DDB(2500; 500; 24; 24)",  Value(0));

    // factor > life
    CHECK_EVAL("DDB(2400; 300; 10; 0.8; 20)", Value(2100));
    CHECK_EVAL("DDB(2400; 300; 10; 1.0; 20)", Value(2100));
    CHECK_EVAL("DDB(2400; 300; 10; 1.2; 20)", Value(0));

    // factor is fraction
    CHECK_EVAL("DDB(2400; 300; 10; 2; 2.5)", Value(450));
    CHECK_EVAL("DDB(2400; 300; 10; 2; 1.5)", Value(306));

    // period is fraction
    CHECK_EVAL("DDB(2400; 300; 10; 6.7; 2)", Value(134.5408487904432));
    CHECK_EVAL("DDB(2400; 300; 10; 7.7; 2)", Value(107.6326790323546));
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

    CHECK_EVAL_SHORT("DISC( DATE(2004;02;29); date(2009;01;01); 95000; 100000; 0)", Value(0.010339));
    CHECK_EVAL_SHORT("DISC( DATE(2004;02;29); date(2009;01;01); 95000; 100000; 1)", Value(0.010333));       // NOK (0.010332)
    CHECK_EVAL_SHORT("DISC( DATE(2004;02;29); date(2009;01;01); 95000; 100000; 2)", Value(0.010181));
    CHECK_EVAL_SHORT("DISC( DATE(2004;02;29); date(2009;01;01); 95000; 100000; 3)", Value(0.010322));
    CHECK_EVAL_SHORT("DISC( DATE(2004;02;29); date(2009;01;01); 95000; 100000; 4)", Value(0.010333));
    CHECK_EVAL_SHORT("DISC( DATE(2006;01;01); date(2008;01;01);   200;    100; 3)", Value(-0.500000));
    CHECK_EVAL_SHORT("DISC( DATE(2006;01;01); date(2005;07;01); 95000; 100000; 4)", Value(false));
    CHECK_EVAL_SHORT("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETDISC( DATE(2004;02;29); date(2009;01;01); 95000; 100000; 0)", Value(0.010339));
}

// DOLLARDE
void TestFinancialFunctions::testDOLLARDE()
{
    // http://publib.boulder.ibm.com/infocenter/iadthelp/v7r0/index.jsp?topic=/com.businessobjects.integration.eclipse.designer.doc/designer/Functions68.html
    CHECK_EVAL_SHORT("DOLLARDE(   1.1 ; 8)" , Value(1.125));          //
    CHECK_EVAL_SHORT("DOLLARDE(   2.13;16)" , Value(2.8125));         //
    CHECK_EVAL_SHORT("DOLLARDE(   2.45;16)" , Value(4.8125));         //
    CHECK_EVAL_SHORT("DOLLARDE(   1.16; 8)" , Value(1.2));            //

    // http://www.bettersolutions.com/excel/EDH113/LR849116511.htm
    CHECK_EVAL_SHORT("DOLLARDE(   1.1 ; 2)" , Value(1.5));            //
    CHECK_EVAL_SHORT("DOLLARDE(   1.25; 5)" , Value(1.5));            //
    CHECK_EVAL_SHORT("DOLLARDE(   5.08; 4)" , Value(5.2));            //
    CHECK_EVAL_SHORT("DOLLARDE(   5.24; 4)" , Value(5.6));            //
    CHECK_EVAL_SHORT("DOLLARDE( 100.24; 4)" , Value(100.6));          //
    CHECK_EVAL_SHORT("DOLLARFR(DOLLARDE( 101.2; 4);4)", Value(101.2));      // for- and backward

    // ODF
    CHECK_EVAL_SHORT("DOLLARDE(   1.1; 4)"  , Value(1.25));           //
    CHECK_EVAL_SHORT("DOLLARDE(   1.1; 3)"  , Value(1.333333));       //
    CHECK_EVAL_SHORT("DOLLARDE(  -1.1;10)"  , Value(-1.1));           //
    CHECK_EVAL_SHORT("DOLLARDE(   1.0; 5)"  , Value(1));              //
    CHECK_EVAL_SHORT("DOLLARDE(   1.1;10)"  , Value(1.1));            //
    CHECK_EVAL_SHORT("DOLLARDE(   1.1; 0)"  , Value::errorVALUE());   //

    // alternate function name
    CHECK_EVAL_SHORT("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETDOLLARDE(1.1;8)" , Value(1.125));
}

// DOLLARFR
void TestFinancialFunctions::testDOLLARFR()
{
    // my tests
    CHECK_EVAL_SHORT("DOLLARFR(    1.1 ;  9)" , Value(1.09));          //
    CHECK_EVAL_SHORT("DOLLARFR(    1.1 ; 11)" , Value(1.011));         //
    CHECK_EVAL_SHORT("DOLLARFR(    1.1 ; 10)" , Value(1.1));           //

    // http://www.bettersolutions.com/excel/EDH113/QR810212321.htm
    CHECK_EVAL_SHORT("DOLLARFR(    1.125 ; 8)" , Value(1.1));          //
    CHECK_EVAL_SHORT("DOLLARFR(    1.5   ; 2)" , Value(1.1));          //
    CHECK_EVAL_SHORT("DOLLARFR(    1.5   ; 8)" , Value(1.4));          //
    CHECK_EVAL_SHORT("DOLLARFR(    1.5   ; 5)" , Value(1.25));         //

    // ODF
    CHECK_EVAL_SHORT("DOLLARFR(    1.1 ;10)" , Value(1.1));            //
    CHECK_EVAL_SHORT("DOLLARFR(    1.25; 4)" , Value(1.1));            //
    CHECK_EVAL_SHORT("DOLLARFR(-1.33333; 3)" , Value(-1.099999));      // ODF specs error (1.1) must be -1.1
    CHECK_EVAL_SHORT("DOLLARFR(     1.0; 5)" , Value(1));              //
    CHECK_EVAL_SHORT("DOLLARFR(     1.1; 0)" , Value::errorVALUE());   //
    
    // alternate function name
    CHECK_EVAL_SHORT("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETDOLLARFR(    1.1 ;  9)" , Value(1.09));
}

// DURATION
void TestFinancialFunctions::testDURATION()
{
    // kspread
    CHECK_EVAL("DURATION( 0.1; 1000; 2000 )" , Value(7.2725408973));     //
    // alternate function name
    CHECK_EVAL_SHORT("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETDURATION( 0.1; 1000; 2000 )" , Value(7.2725408973));
}

// DURATION_ADD
void TestFinancialFunctions::testDURATION_ADD()
{
    CHECK_EVAL("DURATION_ADD( \"1998-01-01\";  \"2006-01-01\"; 0.08; 0.09; 2; 1 )" , Value(5.9937749555));     //
}

// EFFECT
void TestFinancialFunctions::testEFFECT()
{
    // kspread
    CHECK_EVAL_SHORT("EFFECT(0.08;12)", Value(0.083));
    // alternate function name
    CHECK_EVAL_SHORT("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETEFFECT(0.08;12)", Value(0.083));
}

// Euro conversion
// EURO(currency)
void TestFinancialFunctions::testEURO()
{
    CHECK_EVAL("EURO(\"ATS\")", Value(13.7603));
    CHECK_EVAL("EURO(\"BEF\")", Value(40.3399));
    CHECK_EVAL("EURO(\"DEM\")", Value(1.95583));
    CHECK_EVAL("EURO(\"ESP\")", Value(166.386));
    CHECK_EVAL("EURO(\"EUR\")", Value(1.0));
    CHECK_EVAL("EURO(\"FIM\")", Value(5.94573));
    CHECK_EVAL("EURO(\"FRF\")", Value(6.55957));
    CHECK_EVAL("EURO(\"GRD\")", Value(340.75));
    CHECK_EVAL("EURO(\"IEP\")", Value(0.787564));
    CHECK_EVAL("EURO(\"ITL\")", Value(1936.27));
    CHECK_EVAL("EURO(\"LUX\")", Value(40.3399));
    CHECK_EVAL("EURO(\"NLG\")", Value(2.20371));
    CHECK_EVAL("EURO(\"PTE\")", Value(200.482));

    // should still work with lowercase
    CHECK_EVAL("EURO(\"ats\")", Value(13.7603));
    CHECK_EVAL("EURO(\"bef\")", Value(40.3399));
    CHECK_EVAL("EURO(\"dem\")", Value(1.95583));
    CHECK_EVAL("EURO(\"esp\")", Value(166.386));
    CHECK_EVAL("EURO(\"eur\")", Value(1.0));
    CHECK_EVAL("EURO(\"fim\")", Value(5.94573));
    CHECK_EVAL("EURO(\"frf\")", Value(6.55957));
    CHECK_EVAL("EURO(\"grd\")", Value(340.75));
    CHECK_EVAL("EURO(\"iep\")", Value(0.787564));
    CHECK_EVAL("EURO(\"itl\")", Value(1936.27));
    CHECK_EVAL("EURO(\"lux\")", Value(40.3399));
    CHECK_EVAL("EURO(\"nlg\")", Value(2.20371));
    CHECK_EVAL("EURO(\"pte\")", Value(200.482));

    // should still work with mixed-case
    CHECK_EVAL("EURO(\"Ats\")", Value(13.7603));
    CHECK_EVAL("EURO(\"Bef\")", Value(40.3399));
    CHECK_EVAL("EURO(\"Dem\")", Value(1.95583));
    CHECK_EVAL("EURO(\"Esp\")", Value(166.386));
    CHECK_EVAL("EURO(\"Eur\")", Value(1.0));
    CHECK_EVAL("EURO(\"Fim\")", Value(5.94573));
    CHECK_EVAL("EURO(\"Frf\")", Value(6.55957));
    CHECK_EVAL("EURO(\"GrD\")", Value(340.75));
    CHECK_EVAL("EURO(\"IeP\")", Value(0.787564));
    CHECK_EVAL("EURO(\"Itl\")", Value(1936.27));
    CHECK_EVAL("EURO(\"luX\")", Value(40.3399));
    CHECK_EVAL("EURO(\"nlG\")", Value(2.20371));
    CHECK_EVAL("EURO(\"ptE\")", Value(200.482));

    CHECK_EVAL("EURO(\"NOMANSLAND\")", Value::errorNUM());
}

// Currency conversion using Euro
// EUROCONVERT(number,source,target)
void TestFinancialFunctions::testEUROCONVERT()
{
    // 1 Euro to ...
    CHECK_EVAL("EUROCONVERT(1;\"EUR\";\"ATS\")", Value(13.7603));
    CHECK_EVAL("EUROCONVERT(1;\"EUR\";\"BEF\")", Value(40.3399));
    CHECK_EVAL("EUROCONVERT(1;\"EUR\";\"DEM\")", Value(1.95583));
    CHECK_EVAL("EUROCONVERT(1;\"EUR\";\"ESP\")", Value(166.386));
    CHECK_EVAL("EUROCONVERT(1;\"EUR\";\"EUR\")", Value(1.0));
    CHECK_EVAL("EUROCONVERT(1;\"EUR\";\"FIM\")", Value(5.94573));
    CHECK_EVAL("EUROCONVERT(1;\"EUR\";\"FRF\")", Value(6.55957));
    CHECK_EVAL("EUROCONVERT(1;\"EUR\";\"GRD\")", Value(340.75));
    CHECK_EVAL("EUROCONVERT(1;\"EUR\";\"IEP\")", Value(0.787564));
    CHECK_EVAL("EUROCONVERT(1;\"EUR\";\"ITL\")", Value(1936.27));
    CHECK_EVAL("EUROCONVERT(1;\"EUR\";\"LUX\")", Value(40.3399));
    CHECK_EVAL("EUROCONVERT(1;\"EUR\";\"NLG\")", Value(2.20371));
    CHECK_EVAL("EUROCONVERT(1;\"EUR\";\"PTE\")", Value(200.482));

    // identity
    CHECK_EVAL("EUROCONVERT(1;\"BEF\";\"bef\")", Value(1.0));
    CHECK_EVAL("EUROCONVERT(1;\"DEM\";\"dem\")", Value(1.0));
    CHECK_EVAL("EUROCONVERT(1;\"ESP\";\"esp\")", Value(1.0));
    CHECK_EVAL("EUROCONVERT(1;\"EUR\";\"eur\")", Value(1.0));
    CHECK_EVAL("EUROCONVERT(1;\"FIM\";\"fim\")", Value(1.0));
    CHECK_EVAL("EUROCONVERT(1;\"FRF\";\"frf\")", Value(1.0));
    CHECK_EVAL("EUROCONVERT(1;\"GRD\";\"grd\")", Value(1.0));
    CHECK_EVAL("EUROCONVERT(1;\"IEP\";\"iep\")", Value(1.0));
    CHECK_EVAL("EUROCONVERT(1;\"ITL\";\"itl\")", Value(1.0));
    CHECK_EVAL("EUROCONVERT(1;\"LUX\";\"lux\")", Value(1.0));
    CHECK_EVAL("EUROCONVERT(1;\"NLG\";\"nlg\")", Value(1.0));
    CHECK_EVAL("EUROCONVERT(1;\"PTE\";\"pte\")", Value(1.0));

    // all other combinations
    CHECK_EVAL("EUROCONVERT(   2; \"ATS\"; \"bef\" )", Value(2*40.3399 / 13.7603));
    CHECK_EVAL("EUROCONVERT(   3; \"ATS\"; \"dem\" )", Value(3*1.95583 / 13.7603));
    CHECK_EVAL("EUROCONVERT(   4; \"ATS\"; \"esp\" )", Value(4*166.386 / 13.7603));
    CHECK_EVAL("EUROCONVERT(   5; \"ATS\"; \"eur\" )", Value(5*1 / 13.7603));
    CHECK_EVAL("EUROCONVERT(   6; \"ATS\"; \"fim\" )", Value(6*5.94573 / 13.7603));
    CHECK_EVAL("EUROCONVERT(   7; \"ATS\"; \"frf\" )", Value(7*6.55957 / 13.7603));
    CHECK_EVAL("EUROCONVERT(   8; \"ATS\"; \"grd\" )", Value(8*340.75 / 13.7603));
    CHECK_EVAL("EUROCONVERT(   9; \"ATS\"; \"iep\" )", Value(9*0.787564 / 13.7603));
    CHECK_EVAL("EUROCONVERT(  10; \"ATS\"; \"itl\" )", Value(10*1936.27 / 13.7603));
    CHECK_EVAL("EUROCONVERT(  11; \"ATS\"; \"lux\" )", Value(11*40.3399 / 13.7603));
    CHECK_EVAL("EUROCONVERT(  12; \"ATS\"; \"nlg\" )", Value(12*2.20371 / 13.7603));
    CHECK_EVAL("EUROCONVERT(  13; \"ATS\"; \"pte\" )", Value(13*200.482 / 13.7603));
    CHECK_EVAL("EUROCONVERT(  14; \"BEF\"; \"ats\" )", Value(14*13.7603 / 40.3399));
    CHECK_EVAL("EUROCONVERT(  15; \"BEF\"; \"dem\" )", Value(15*1.95583 / 40.3399));
    CHECK_EVAL("EUROCONVERT(  16; \"BEF\"; \"esp\" )", Value(16*166.386 / 40.3399));
    CHECK_EVAL("EUROCONVERT(  17; \"BEF\"; \"eur\" )", Value(17*1 / 40.3399));
    CHECK_EVAL("EUROCONVERT(  18; \"BEF\"; \"fim\" )", Value(18*5.94573 / 40.3399));
    CHECK_EVAL("EUROCONVERT(  19; \"BEF\"; \"frf\" )", Value(19*6.55957 / 40.3399));
    CHECK_EVAL("EUROCONVERT(  20; \"BEF\"; \"grd\" )", Value(20*340.75 / 40.3399));
    CHECK_EVAL("EUROCONVERT(  21; \"BEF\"; \"iep\" )", Value(21*0.787564 / 40.3399));
    CHECK_EVAL("EUROCONVERT(  22; \"BEF\"; \"itl\" )", Value(22*1936.27 / 40.3399));
    CHECK_EVAL("EUROCONVERT(  23; \"BEF\"; \"lux\" )", Value(23*40.3399 / 40.3399));
    CHECK_EVAL("EUROCONVERT(  24; \"BEF\"; \"nlg\" )", Value(24*2.20371 / 40.3399));
    CHECK_EVAL("EUROCONVERT(  25; \"BEF\"; \"pte\" )", Value(25*200.482 / 40.3399));
    CHECK_EVAL("EUROCONVERT(  26; \"DEM\"; \"ats\" )", Value(26*13.7603 / 1.95583));
    CHECK_EVAL("EUROCONVERT(  27; \"DEM\"; \"bef\" )", Value(27*40.3399 / 1.95583));
    CHECK_EVAL("EUROCONVERT(  28; \"DEM\"; \"esp\" )", Value(28*166.386 / 1.95583));
    CHECK_EVAL("EUROCONVERT(  29; \"DEM\"; \"eur\" )", Value(29*1 / 1.95583));
    CHECK_EVAL("EUROCONVERT(  30; \"DEM\"; \"fim\" )", Value(30*5.94573 / 1.95583));
    CHECK_EVAL("EUROCONVERT(  31; \"DEM\"; \"frf\" )", Value(31*6.55957 / 1.95583));
    CHECK_EVAL("EUROCONVERT(  32; \"DEM\"; \"grd\" )", Value(32*340.75 / 1.95583));
    CHECK_EVAL("EUROCONVERT(  33; \"DEM\"; \"iep\" )", Value(33*0.787564 / 1.95583));
    CHECK_EVAL("EUROCONVERT(  34; \"DEM\"; \"itl\" )", Value(34*1936.27 / 1.95583));
    CHECK_EVAL("EUROCONVERT(  35; \"DEM\"; \"lux\" )", Value(35*40.3399 / 1.95583));
    CHECK_EVAL("EUROCONVERT(  36; \"DEM\"; \"nlg\" )", Value(36*2.20371 / 1.95583));
    CHECK_EVAL("EUROCONVERT(  37; \"DEM\"; \"pte\" )", Value(37*200.482 / 1.95583));
    CHECK_EVAL("EUROCONVERT(  38; \"ESP\"; \"ats\" )", Value(38*13.7603 / 166.386));
    CHECK_EVAL("EUROCONVERT(  39; \"ESP\"; \"bef\" )", Value(39*40.3399 / 166.386));
    CHECK_EVAL("EUROCONVERT(  40; \"ESP\"; \"dem\" )", Value(40*1.95583 / 166.386));
    CHECK_EVAL("EUROCONVERT(  41; \"ESP\"; \"eur\" )", Value(41*1 / 166.386));
    CHECK_EVAL("EUROCONVERT(  42; \"ESP\"; \"fim\" )", Value(42*5.94573 / 166.386));
    CHECK_EVAL("EUROCONVERT(  43; \"ESP\"; \"frf\" )", Value(43*6.55957 / 166.386));
    CHECK_EVAL("EUROCONVERT(  44; \"ESP\"; \"grd\" )", Value(44*340.75 / 166.386));
    CHECK_EVAL("EUROCONVERT(  45; \"ESP\"; \"iep\" )", Value(45*0.787564 / 166.386));
    CHECK_EVAL("EUROCONVERT(  46; \"ESP\"; \"itl\" )", Value(46*1936.27 / 166.386));
    CHECK_EVAL("EUROCONVERT(  47; \"ESP\"; \"lux\" )", Value(47*40.3399 / 166.386));
    CHECK_EVAL("EUROCONVERT(  48; \"ESP\"; \"nlg\" )", Value(48*2.20371 / 166.386));
    CHECK_EVAL("EUROCONVERT(  49; \"ESP\"; \"pte\" )", Value(49*200.482 / 166.386));
    CHECK_EVAL("EUROCONVERT(  50; \"EUR\"; \"ats\" )", Value(50*13.7603 / 1));
    CHECK_EVAL("EUROCONVERT(  51; \"EUR\"; \"bef\" )", Value(51*40.3399 / 1));
    CHECK_EVAL("EUROCONVERT(  52; \"EUR\"; \"dem\" )", Value(52*1.95583 / 1));
    CHECK_EVAL("EUROCONVERT(  53; \"EUR\"; \"esp\" )", Value(53*166.386 / 1));
    CHECK_EVAL("EUROCONVERT(  54; \"EUR\"; \"fim\" )", Value(54*5.94573 / 1));
    CHECK_EVAL("EUROCONVERT(  55; \"EUR\"; \"frf\" )", Value(55*6.55957 / 1));
    CHECK_EVAL("EUROCONVERT(  56; \"EUR\"; \"grd\" )", Value(56*340.75 / 1));
    CHECK_EVAL("EUROCONVERT(  57; \"EUR\"; \"iep\" )", Value(57*0.787564 / 1));
    CHECK_EVAL("EUROCONVERT(  58; \"EUR\"; \"itl\" )", Value(58*1936.27 / 1));
    CHECK_EVAL("EUROCONVERT(  59; \"EUR\"; \"lux\" )", Value(59*40.3399 / 1));
    CHECK_EVAL("EUROCONVERT(  60; \"EUR\"; \"nlg\" )", Value(60*2.20371 / 1));
    CHECK_EVAL("EUROCONVERT(  61; \"EUR\"; \"pte\" )", Value(61*200.482 / 1));
    CHECK_EVAL("EUROCONVERT(  62; \"FIM\"; \"ats\" )", Value(62*13.7603 / 5.94573));
    CHECK_EVAL("EUROCONVERT(  63; \"FIM\"; \"bef\" )", Value(63*40.3399 / 5.94573));
    CHECK_EVAL("EUROCONVERT(  64; \"FIM\"; \"dem\" )", Value(64*1.95583 / 5.94573));
    CHECK_EVAL("EUROCONVERT(  65; \"FIM\"; \"esp\" )", Value(65*166.386 / 5.94573));
    CHECK_EVAL("EUROCONVERT(  66; \"FIM\"; \"eur\" )", Value(66*1 / 5.94573));
    CHECK_EVAL("EUROCONVERT(  67; \"FIM\"; \"frf\" )", Value(67*6.55957 / 5.94573));
    CHECK_EVAL("EUROCONVERT(  68; \"FIM\"; \"grd\" )", Value(68*340.75 / 5.94573));
    CHECK_EVAL("EUROCONVERT(  69; \"FIM\"; \"iep\" )", Value(69*0.787564 / 5.94573));
    CHECK_EVAL("EUROCONVERT(  70; \"FIM\"; \"itl\" )", Value(70*1936.27 / 5.94573));
    CHECK_EVAL("EUROCONVERT(  71; \"FIM\"; \"lux\" )", Value(71*40.3399 / 5.94573));
    CHECK_EVAL("EUROCONVERT(  72; \"FIM\"; \"nlg\" )", Value(72*2.20371 / 5.94573));
    CHECK_EVAL("EUROCONVERT(  73; \"FIM\"; \"pte\" )", Value(73*200.482 / 5.94573));
    CHECK_EVAL("EUROCONVERT(  74; \"FRF\"; \"ats\" )", Value(74*13.7603 / 6.55957));
    CHECK_EVAL("EUROCONVERT(  75; \"FRF\"; \"bef\" )", Value(75*40.3399 / 6.55957));
    CHECK_EVAL("EUROCONVERT(  76; \"FRF\"; \"dem\" )", Value(76*1.95583 / 6.55957));
    CHECK_EVAL("EUROCONVERT(  77; \"FRF\"; \"esp\" )", Value(77*166.386 / 6.55957));
    CHECK_EVAL("EUROCONVERT(  78; \"FRF\"; \"eur\" )", Value(78*1 / 6.55957));
    CHECK_EVAL("EUROCONVERT(  79; \"FRF\"; \"fim\" )", Value(79*5.94573 / 6.55957));
    CHECK_EVAL("EUROCONVERT(  80; \"FRF\"; \"grd\" )", Value(80*340.75 / 6.55957));
    CHECK_EVAL("EUROCONVERT(  81; \"FRF\"; \"iep\" )", Value(81*0.787564 / 6.55957));
    CHECK_EVAL("EUROCONVERT(  82; \"FRF\"; \"itl\" )", Value(82*1936.27 / 6.55957));
    CHECK_EVAL("EUROCONVERT(  83; \"FRF\"; \"lux\" )", Value(83*40.3399 / 6.55957));
    CHECK_EVAL("EUROCONVERT(  84; \"FRF\"; \"nlg\" )", Value(84*2.20371 / 6.55957));
    CHECK_EVAL("EUROCONVERT(  85; \"FRF\"; \"pte\" )", Value(85*200.482 / 6.55957));
    CHECK_EVAL("EUROCONVERT(  86; \"GRD\"; \"ats\" )", Value(86*13.7603 / 340.75));
    CHECK_EVAL("EUROCONVERT(  87; \"GRD\"; \"bef\" )", Value(87*40.3399 / 340.75));
    CHECK_EVAL("EUROCONVERT(  88; \"GRD\"; \"dem\" )", Value(88*1.95583 / 340.75));
    CHECK_EVAL("EUROCONVERT(  89; \"GRD\"; \"esp\" )", Value(89*166.386 / 340.75));
    CHECK_EVAL("EUROCONVERT(  90; \"GRD\"; \"eur\" )", Value(90*1 / 340.75));
    CHECK_EVAL("EUROCONVERT(  91; \"GRD\"; \"fim\" )", Value(91*5.94573 / 340.75));
    CHECK_EVAL("EUROCONVERT(  92; \"GRD\"; \"frf\" )", Value(92*6.55957 / 340.75));
    CHECK_EVAL("EUROCONVERT(  93; \"GRD\"; \"iep\" )", Value(93*0.787564 / 340.75));
    CHECK_EVAL("EUROCONVERT(  94; \"GRD\"; \"itl\" )", Value(94*1936.27 / 340.75));
    CHECK_EVAL("EUROCONVERT(  95; \"GRD\"; \"lux\" )", Value(95*40.3399 / 340.75));
    CHECK_EVAL("EUROCONVERT(  96; \"GRD\"; \"nlg\" )", Value(96*2.20371 / 340.75));
    CHECK_EVAL("EUROCONVERT(  97; \"GRD\"; \"pte\" )", Value(97*200.482 / 340.75));
    CHECK_EVAL("EUROCONVERT(  98; \"IEP\"; \"ats\" )", Value(98*13.7603 / 0.787564));
    CHECK_EVAL("EUROCONVERT(  99; \"IEP\"; \"bef\" )", Value(99*40.3399 / 0.787564));
    CHECK_EVAL("EUROCONVERT( 100; \"IEP\"; \"dem\" )", Value(100*1.95583 / 0.787564));
    CHECK_EVAL("EUROCONVERT( 101; \"IEP\"; \"esp\" )", Value(101*166.386 / 0.787564));
    CHECK_EVAL("EUROCONVERT( 102; \"IEP\"; \"eur\" )", Value(102*1 / 0.787564));
    CHECK_EVAL("EUROCONVERT( 103; \"IEP\"; \"fim\" )", Value(103*5.94573 / 0.787564));
    CHECK_EVAL("EUROCONVERT( 104; \"IEP\"; \"frf\" )", Value(104*6.55957 / 0.787564));
    CHECK_EVAL("EUROCONVERT( 105; \"IEP\"; \"grd\" )", Value(105*340.75 / 0.787564));
    CHECK_EVAL("EUROCONVERT( 106; \"IEP\"; \"itl\" )", Value(106*1936.27 / 0.787564));
    CHECK_EVAL("EUROCONVERT( 107; \"IEP\"; \"lux\" )", Value(107*40.3399 / 0.787564));
    CHECK_EVAL("EUROCONVERT( 108; \"IEP\"; \"nlg\" )", Value(108*2.20371 / 0.787564));
    CHECK_EVAL("EUROCONVERT( 109; \"IEP\"; \"pte\" )", Value(109*200.482 / 0.787564));
    CHECK_EVAL("EUROCONVERT( 110; \"ITL\"; \"ats\" )", Value(110*13.7603 / 1936.27));
    CHECK_EVAL("EUROCONVERT( 111; \"ITL\"; \"bef\" )", Value(111*40.3399 / 1936.27));
    CHECK_EVAL("EUROCONVERT( 112; \"ITL\"; \"dem\" )", Value(112*1.95583 / 1936.27));
    CHECK_EVAL("EUROCONVERT( 113; \"ITL\"; \"esp\" )", Value(113*166.386 / 1936.27));
    CHECK_EVAL("EUROCONVERT( 114; \"ITL\"; \"eur\" )", Value(114*1 / 1936.27));
    CHECK_EVAL("EUROCONVERT( 115; \"ITL\"; \"fim\" )", Value(115*5.94573 / 1936.27));
    CHECK_EVAL("EUROCONVERT( 116; \"ITL\"; \"frf\" )", Value(116*6.55957 / 1936.27));
    CHECK_EVAL("EUROCONVERT( 117; \"ITL\"; \"grd\" )", Value(117*340.75 / 1936.27));
    CHECK_EVAL("EUROCONVERT( 118; \"ITL\"; \"iep\" )", Value(118*0.787564 / 1936.27));
    CHECK_EVAL("EUROCONVERT( 119; \"ITL\"; \"lux\" )", Value(119*40.3399 / 1936.27));
    CHECK_EVAL("EUROCONVERT( 120; \"ITL\"; \"nlg\" )", Value(120*2.20371 / 1936.27));
    CHECK_EVAL("EUROCONVERT( 121; \"ITL\"; \"pte\" )", Value(121*200.482 / 1936.27));
    CHECK_EVAL("EUROCONVERT( 122; \"LUX\"; \"ats\" )", Value(122*13.7603 / 40.3399));
    CHECK_EVAL("EUROCONVERT( 123; \"LUX\"; \"bef\" )", Value(123*40.3399 / 40.3399));
    CHECK_EVAL("EUROCONVERT( 124; \"LUX\"; \"dem\" )", Value(124*1.95583 / 40.3399));
    CHECK_EVAL("EUROCONVERT( 125; \"LUX\"; \"esp\" )", Value(125*166.386 / 40.3399));
    CHECK_EVAL("EUROCONVERT( 126; \"LUX\"; \"eur\" )", Value(126*1 / 40.3399));
    CHECK_EVAL("EUROCONVERT( 127; \"LUX\"; \"fim\" )", Value(127*5.94573 / 40.3399));
    CHECK_EVAL("EUROCONVERT( 128; \"LUX\"; \"frf\" )", Value(128*6.55957 / 40.3399));
    CHECK_EVAL("EUROCONVERT( 129; \"LUX\"; \"grd\" )", Value(129*340.75 / 40.3399));
    CHECK_EVAL("EUROCONVERT( 130; \"LUX\"; \"iep\" )", Value(130*0.787564 / 40.3399));
    CHECK_EVAL("EUROCONVERT( 131; \"LUX\"; \"itl\" )", Value(131*1936.27 / 40.3399));
    CHECK_EVAL("EUROCONVERT( 132; \"LUX\"; \"nlg\" )", Value(132*2.20371 / 40.3399));
    CHECK_EVAL("EUROCONVERT( 133; \"LUX\"; \"pte\" )", Value(133*200.482 / 40.3399));
    CHECK_EVAL("EUROCONVERT( 134; \"NLG\"; \"ats\" )", Value(134*13.7603 / 2.20371));
    CHECK_EVAL("EUROCONVERT( 135; \"NLG\"; \"bef\" )", Value(135*40.3399 / 2.20371));
    CHECK_EVAL("EUROCONVERT( 136; \"NLG\"; \"dem\" )", Value(136*1.95583 / 2.20371));
    CHECK_EVAL("EUROCONVERT( 137; \"NLG\"; \"esp\" )", Value(137*166.386 / 2.20371));
    CHECK_EVAL("EUROCONVERT( 138; \"NLG\"; \"eur\" )", Value(138*1 / 2.20371));
    CHECK_EVAL("EUROCONVERT( 139; \"NLG\"; \"fim\" )", Value(139*5.94573 / 2.20371));
    CHECK_EVAL("EUROCONVERT( 140; \"NLG\"; \"frf\" )", Value(140*6.55957 / 2.20371));
    CHECK_EVAL("EUROCONVERT( 141; \"NLG\"; \"grd\" )", Value(141*340.75 / 2.20371));
    CHECK_EVAL("EUROCONVERT( 142; \"NLG\"; \"iep\" )", Value(142*0.787564 / 2.20371));
    CHECK_EVAL("EUROCONVERT( 143; \"NLG\"; \"itl\" )", Value(143*1936.27 / 2.20371));
    CHECK_EVAL("EUROCONVERT( 144; \"NLG\"; \"lux\" )", Value(144*40.3399 / 2.20371));
    CHECK_EVAL("EUROCONVERT( 145; \"NLG\"; \"pte\" )", Value(145*200.482 / 2.20371));
    CHECK_EVAL("EUROCONVERT( 146; \"PTE\"; \"ats\" )", Value(146*13.7603 / 200.482));
    CHECK_EVAL("EUROCONVERT( 147; \"PTE\"; \"bef\" )", Value(147*40.3399 / 200.482));
    CHECK_EVAL("EUROCONVERT( 148; \"PTE\"; \"dem\" )", Value(148*1.95583 / 200.482));
    CHECK_EVAL("EUROCONVERT( 149; \"PTE\"; \"esp\" )", Value(149*166.386 / 200.482));
    CHECK_EVAL("EUROCONVERT( 150; \"PTE\"; \"eur\" )", Value(150*1 / 200.482));
    CHECK_EVAL("EUROCONVERT( 151; \"PTE\"; \"fim\" )", Value(151*5.94573 / 200.482));
    CHECK_EVAL("EUROCONVERT( 152; \"PTE\"; \"frf\" )", Value(152*6.55957 / 200.482));
    CHECK_EVAL("EUROCONVERT( 153; \"PTE\"; \"grd\" )", Value(153*340.75 / 200.482));
    CHECK_EVAL("EUROCONVERT( 154; \"PTE\"; \"iep\" )", Value(154*0.787564 / 200.482));
    CHECK_EVAL("EUROCONVERT( 155; \"PTE\"; \"itl\" )", Value(155*1936.27 / 200.482));
    CHECK_EVAL("EUROCONVERT( 156; \"PTE\"; \"lux\" )", Value(156*40.3399 / 200.482));
    CHECK_EVAL("EUROCONVERT( 157; \"PTE\"; \"nlg\" )", Value(157*2.20371 / 200.482));
}

// FV
void TestFinancialFunctions::testFV()
{
    // ODF
    CHECK_EVAL("FV(10%;12;-100;100)" , Value(1824.5855390489));     // A trivial example of FV.
}

// FVSCHEDULE
void TestFinancialFunctions::testFVSCHEDULE()
{
    // ODF
    CHECK_EVAL_SHORT("FVSCHEDULE(1000000; {0.03; 0.04; 0.05})" , Value(1124760));     // A trivial example of FVSCHEDULE.
    // alternate function name
    CHECK_EVAL_SHORT("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETFVSCHEDULE(1000000; {0.03; 0.04; 0.05})" , Value(1124760));
}

// INTRATE
void TestFinancialFunctions::testINTRATE()
{
    // ODF
    CHECK_EVAL_SHORT("INTRATE( DATE(2002; 6;8); DATE(1995;10;5); 100000; 200000; 0 )" , Value::errorVALUE());     // Settlement date must be before the maturity date.
    CHECK_EVAL_SHORT("INTRATE( DATE(2002; 6;8); DATE(2002; 6;8); 100000; 200000; 0 )" , Value::errorVALUE());     // Settlement date must be before the maturity date.
    CHECK_EVAL_SHORT("INTRATE( DATE(1995;10;5); DATE(2002; 6;8); 100000; 200000; 50)" , Value::errorVALUE());     // Unknown Basis returns Error.
    CHECK_EVAL_SHORT("INTRATE( DATE(1995;10;5); DATE(2002; 6;8); 100000; 200000; 0 )" , Value(0.1498127341));     // An example of INTRATE.
    CHECK_EVAL_SHORT("INTRATE( DATE(1995;10;5); DATE(2002; 6;8); 100000; 200000    )" , Value(0.1498127341));     // Basis defaults to 0.
    CHECK_EVAL_SHORT("INTRATE( DATE(1995;10;5); DATE(2002; 6;8); 100000; 200000; 1 )" , Value(0.1497128794));     //
    CHECK_EVAL_SHORT("INTRATE( DATE(1995;10;5); DATE(2002; 6;8); 100000; 200000; 2 )" , Value(0.1476620180));     //
    CHECK_EVAL_SHORT("INTRATE( DATE(1995;10;5); DATE(2002; 6;8); 100000; 200000; 3 )" , Value(0.1497128794));     //
    CHECK_EVAL_SHORT("INTRATE( DATE(1995;10;5); DATE(2002; 6;8); 100000; 200000; 4 )" , Value(0.1498127341));     //
    // alternate function name
    CHECK_EVAL_SHORT("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETINTRATE( DATE(1995;10;5); DATE(2002; 6;8); 100000; 200000; 0 )" , Value(0.1498127341));
}

// IPMT
void TestFinancialFunctions::testIPMT()
{
    // ODF
    CHECK_EVAL_SHORT("IPMT(5%/12;10;360;100000)", Value(-412.0850243));            // An example of IPMT.  The interest payment on a 100000 unit loan
    // in the 10th month of a 30 year loan at 5% annual interest.
    CHECK_EVAL_SHORT("IPMT(5%/12;10;360;100000;0;1)", Value(-410.3751278993));     // Payments at the beginning of each period.

    // The total payment is the principle plus the interest.
    CHECK_EVAL_SHORT("PPMT(5%/12;10;360;100000)+IPMT(5%/12;10;360;100000)-PMT(5%/12;360;100000)", Value(0));
}

// ISPMT
void TestFinancialFunctions::testISPMT()
{
    // betersolutions
    CHECK_EVAL("ISPMT(10%/12;1 ;36;8000000)", Value(-64814.8148148148));      //
    CHECK_EVAL("ISPMT(10%   ;1 ;3 ;8000000)", Value(-533333.3333333333));     //

    // ODF
    CHECK_EVAL("ISPMT(5%/12;12;360;100000)",  Value(-402.7777777778));        // A trivial example of ISPMT.  A 100000 unit investment with an
    // annual interest rate of 5% and a 30 year term has an interest payment
    // of 402.78 units in month 12.
}

// Level-coupon bond
// LEVEL_COUPON(faceValue; couponRate; couponsPerYear; years; marketRate)
void TestFinancialFunctions::testLEVELCOUPON()
{
    CHECK_EVAL("LEVEL_COUPON(1000; .13; 1; 4; .1)",   Value(1095.0959633904788));
    CHECK_EVAL("LEVEL_COUPON(1000; .13; 2; 4; .1)",   Value(1096.9481913913939));
    CHECK_EVAL("LEVEL_COUPON(1000; .10; 1; 10; .25)", Value(464.4245094400000));
    CHECK_EVAL("LEVEL_COUPON(1000; .12; 1; 10; .25)", Value(535.8345748480000));
    CHECK_EVAL("LEVEL_COUPON(1000; .20; 1; 10; .25)", Value(821.4748364800000));
}

// MDURATION
void TestFinancialFunctions::testMDURATION()
{
    CHECK_EVAL("MDURATION(\"2004-02-01\"; \"2004-05-31\"; 0.08; 0.09; 2; 0)" , Value(0.3189792663));     // These tests go over a leap year day,
    // and intentionally end on May 31, which
    // illustrates the differences between
    // many bases
    // alternate function name
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETMDURATION(\"2004-02-01\"; \"2004-05-31\"; 0.08; 0.09; 2; 0)" , Value(0.3189792663));
}

// MIRR
void TestFinancialFunctions::testMIRR()
{
    // ODF
    CHECK_EVAL("MIRR({100;200;-50;300;-200}; 5%; 6%)",     Value(0.342823387842));

    // bettersolutions.com
    CHECK_EVAL("MIRR({-10;30;20;10;20};0.1;0.12)",         Value(0.7712844619));
    CHECK_EVAL("MIRR({-100;30;30;30;30};0.1;1)",           Value(0.4564753151));
    CHECK_EVAL("MIRR({-50;20;40;70};10/100;12/100)",       Value(0.4090837902));
    CHECK_EVAL("MIRR({-5;1;2;3;4};10/100;0.12)",           Value(0.2253901556));
    CHECK_EVAL("MIRR({1000;1100;1200;1500;1600};10%;12%)", Value(Value::errorDIV0()));
}

// Yearly nominal interest rate
// NOMINAL(effectiveRate, periods)
void TestFinancialFunctions::testNOMINAL()
{
    CHECK_EVAL("NOMINAL(13.5%; 12)", Value(0.1273031669590416));
    CHECK_EVAL("NOMINAL(13.5%; 12)", Value(0.1273031669590416));
    CHECK_EVAL("NOMINAL(25%; 12)",   Value(0.2252311814580734));
    CHECK_EVAL("NOMINAL(25%; 4)",    Value(0.2294850537622564));
    CHECK_EVAL("NOMINAL(20%; 12)",   Value(0.1837136459967743));
    CHECK_EVAL("NOMINAL(10%; 12)",   Value(0.0956896851468452));

    // rate must be positive
    CHECK_EVAL("NOMINAL(0; 12)",     Value::errorVALUE());

    // periods must be positive
    CHECK_EVAL("NOMINAL(10%; 0)",    Value::errorDIV0());
    CHECK_EVAL("NOMINAL(10%; -1)",   Value::errorVALUE());
    CHECK_EVAL("NOMINAL(10%; -2)",   Value::errorVALUE());

    // test cases in OpenFormula specification
    CHECK_EVAL("NOMINAL(8%;4)",      Value(0.0777061876330940));
    CHECK_EVAL("NOMINAL(12.5%;12)",  Value(0.118362966638538));
    CHECK_EVAL("NOMINAL(1%;2)",      Value(0.00997512422417790));
    
    // alternate function name
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETNOMINAL(8%;4)",      Value(0.0777061876330940));
}

// NPER
void TestFinancialFunctions::testNPER()
{
    // ODF
    CHECK_EVAL_SHORT("NPER(5% ;-100;1000)",       Value(14.2067));         // A trivial example of NPER.
    CHECK_EVAL_SHORT("NPER(5% ;-100;1000;100)",   Value(15.2067));         // A trivial example of NPER with non-zero FV.
    CHECK_EVAL_SHORT("NPER(5% ;-100;1000;100;1)", Value(14.2067));         // A trivial example of NPER with non-zero FV and PayType.
    CHECK_EVAL_SHORT("NPER(0  ;-100;1000)",       Value(10.0000));         // TODO Rate can be zero.
    CHECK_EVAL_SHORT("NPER(-1%;-100;1000)",       Value(9.483283066));     // TODO Rate can be negative.
}

// Net present value
// NPV(rate, values)
void TestFinancialFunctions::testNPV()
{
    CHECK_EVAL("NPV(100%; 4; 5; 7)", Value(4.125));
    CHECK_EVAL("NPV(10%; 100; 200)", Value(256.198347107438));
}

// ODDLPRICE
void TestFinancialFunctions::testODDLPRICE()
{
    // ODF tests. All results are taken from OOo-2.2.1 instead of results from ODF-specs
    // TODO it seems that frequency are not considered
    // CHECK_EVAL_SHORT( "ODDLPRICE(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);3%;5%;100;2)",      Value( 90.9975570033  ) ); //
    // CHECK_EVAL_SHORT( "ODDLPRICE(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);3%;5%;100;1;0)",    Value( 90.9975570033  ) ); // f=1, b=0
    // CHECK_EVAL_SHORT( "ODDLPRICE(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);3%;5%;100;2;0)",    Value( 90.9975570033  ) ); // f=2, b=0
    // CHECK_EVAL_SHORT( "ODDLPRICE(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);3%;5%;100;4;0)",    Value( 90.9975570033  ) ); // f=4, b=0
    // CHECK_EVAL_SHORT( "ODDLPRICE(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);2%;1.5%;100;1;1)",  Value( 102.5120875338 ) ); // f=1, b=1
    // CHECK_EVAL_SHORT( "ODDLPRICE(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);2%;1.5%;100;2;1)",  Value( 102.5120875338 ) ); // f=2, b=1
    //CHECK_EVAL_SHORT( "ODDLPRICE(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);2%;1.5%;100;4;1)",  Value( 102.5120875338 ) ); // f=4, b=1
    //CHECK_EVAL_SHORT( "ODDLPRICE(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);2%;1.5%;100;1;2)",  Value( 102.5444975699 ) ); // f=1, b=2 specs 102.512087534
    //CHECK_EVAL_SHORT( "ODDLPRICE(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);2%;1.5%;100;2;2)",  Value( 102.5444975699 ) ); // f=2, b=2
    //CHECK_EVAL_SHORT( "ODDLPRICE(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);2%;1.5%;100;4;2)",  Value( 102.5444975699 ) ); // f=4, b=2
    //CHECK_EVAL_SHORT( "ODDLPRICE(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);3%;5%;1000;1;3)",   Value( 794.4972058550 ) ); // f=1, b=3 specs 794.575995564
    //CHECK_EVAL_SHORT( "ODDLPRICE(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);3%;5%;1000;2;3)",   Value( 794.4972058550 ) ); // f=2, b=3 specs 794.671729071
    //CHECK_EVAL_SHORT( "ODDLPRICE(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);3%;5%;1000;4;3)",   Value( 794.4972058550 ) ); // f=4, b=3 specs 794.684531308
    //CHECK_EVAL_SHORT( "ODDLPRICE(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);2%;1.5%;1000;1;4)", Value( 932.992137337  ) ); // f=1, b=4
    //CHECK_EVAL_SHORT( "ODDLPRICE(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);2%;1.5%;1000;2;4)", Value( 932.992137337  ) ); // f=2, b=4
    //CHECK_EVAL_SHORT( "ODDLPRICE(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);2%;1.5%;1000;4;4)", Value( 932.992137337  ) ); // f=4, b=4
}

// ODDLYIELD
void TestFinancialFunctions::testODDLYIELD()
{
    // ODF tests

    // Basis 0
//   CHECK_EVAL_SHORT( "ODDLYIELD(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);3%;91;100 ;2  )", Value( 4.997775351/100.0 ) ); // Without Basis parameter
//   CHECK_EVAL_SHORT( "ODDLYIELD(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);3%;91;100 ;1;0)", Value( 4.997775351/100.0 ) ); // With Frequency=1 and Basis=0
//   CHECK_EVAL_SHORT( "ODDLYIELD(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);3%;91;100 ;2;0)", Value( 4.997775351/100.0 ) ); // With Frequency=2 and Basis=0
//   CHECK_EVAL_SHORT( "ODDLYIELD(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);3%;91;100 ;4;0)", Value( 4.997775351/100.0 ) ); // With Frequency=4 and Basis=0

    // Basis 1
    // CHECK_EVAL_SHORT( "ODDLYIELD(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);2%;103;100 ;1;1)", Value( 1.408788601/100.0 ) ); // With Frequency=1 and Basis=1
//   CHECK_EVAL_SHORT( "ODDLYIELD(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);2%;103;100 ;2;1)", Value( 1.408379719/100.0 ) ); // With Frequency=2 and Basis=1
//   CHECK_EVAL_SHORT( "ODDLYIELD(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);2%;103;100 ;4;1)", Value( 1.408325114/100.0 ) ); // With Frequency=4 and Basis=1

    // Basis 2
//   CHECK_EVAL_SHORT( "ODDLYIELD(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);2%;103;100;1;2)", Value( 1.408788601/100.0 ) ); // With Frequency=1 and Basis=2
    // CHECK_EVAL_SHORT( "ODDLYIELD(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);2%;103;100 ;4;2)", Value( 1.408379719/100.0 ) ); // With Frequency=2 and Basis=2
    // CHECK_EVAL_SHORT( "ODDLYIELD(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);2%;103;100 ;2;2)", Value( 1.408325114/100.0 ) ); // With Frequency=4 and Basis=2

    // Basis 3
    // CHECK_EVAL_SHORT( "ODDLYIELD(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);3%;795;1000;1;3)", Value( 4.987800402/100.0 ) ); // With Frequency=1 and Basis=3
    // CHECK_EVAL_SHORT( "ODDLYIELD(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);3%;795;1000;2;3)", Value( 4.990550494/100.0 ) ); // With Frequency=2 and Basis=3
    // CHECK_EVAL_SHORT( "ODDLYIELD(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);3%;795;1000;4;3)", Value( 4.990918451/100.0 ) ); // With Frequency=4 and Basis=3

    // Basis 4
    // CHECK_EVAL_SHORT( "ODDLYIELD(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);2%;933;1000;1;4)", Value( 1.499836493/100.0 ) ); // With Frequency=1 and Basis=4
    // CHECK_EVAL_SHORT( "ODDLYIELD(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);2%;933;1000;2;4)", Value( 1.499836493/100.0 ) ); // With Frequency=2 and Basis=4
    // CHECK_EVAL_SHORT( "ODDLYIELD(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);2%;933;1000;4;4)", Value( 1.499836493/100.0 ) ); // With Frequency=4 and Basis=4
}

// PDURATION
void TestFinancialFunctions::testPDURATION()
{
    // is DURATION in kspread
    CHECK_EVAL_SHORT("PDURATION( 0.1; 10; 100 )" , Value(24.158858));      // simple use case
    CHECK_EVAL_SHORT("PDURATION( 0.1; 100; 10 )" , Value(-24.158858));     // curentValue > desiredValue
    CHECK_EVAL_SHORT("PDURATION(   0; 10;  11 )" , Value::errorVALUE());   // rate > 0
    CHECK_EVAL_SHORT("PDURATION( 0.1;  0;  11 )" , Value::errorVALUE());   // currentValue > 0
    CHECK_EVAL_SHORT("PDURATION( 0.1; 10;   0 )" , Value::errorVALUE());   // desiredValue > 0
}

// PMT
void TestFinancialFunctions::testPMT()
{
    // ODF
    CHECK_EVAL_SHORT("PMT(5%;12;1000)",       Value(-112.8254100208));     // A trivial example of PMT.
    CHECK_EVAL_SHORT("PMT(5%;12;1000;100)",   Value(-119.1079510229));     // A trivial example of PMT with non-zero FV.
    CHECK_EVAL_SHORT("PMT(5%;12;1000;100;1)", Value(-113.4361438313));     // A trivial example of PMT with non-zero FV and PayType.
    CHECK_EVAL_SHORT("PMT(0;10;1000)",        Value(-100.00000));      // TODO Rate can be zero.
}

// PPMT
void TestFinancialFunctions::testPPMT()
{
    // bettersolution.com
    CHECK_EVAL("PPMT(10%/12;1;24;2000)",      Value(-75.6231860084));        // A simple test case
    CHECK_EVAL("PPMT(8%;10;10;200000)",       Value(-27598.0534624214));     // A simple test case

    // ODF
    CHECK_EVAL("PPMT(3%;1;12;100)",           Value(-7.0462085473));       // A simple test case
    CHECK_EVAL("PPMT(8%;5;24;10000;0)",       Value(-203.7735140493));     // With nPer=5 and Future=0
    CHECK_EVAL("PPMT(8%;10;24;10000;2000)",   Value(-359.2921746011));     // With nPer=10 and Future=2000
    CHECK_EVAL("PPMT(8%;10;24;10000;2000;1)", Value(-332.6779394454));     // With Type=1

    // these tests seems to be wrong in specs. remove superflous parameter "1".
    CHECK_EVAL("PPMT(3%;1;12;100;200)",       Value(-21.1386256419));      // With future value
    CHECK_EVAL("PPMT(3%;1;12;100;200;1)",     Value(-20.5229375164));      // With future value and type
}

// PRICEMAT
void TestFinancialFunctions::testPRICEMAT()
{
    // ODF - TODO expand to 10 signif.
    CHECK_EVAL_SHORT("PRICEMAT(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);6%;5%  )", Value(103.819218241));     // Without Basis parameter
    CHECK_EVAL_SHORT("PRICEMAT(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);6%;5%;1)", Value(103.824693325));     // With Basis=1 specs 103.824693325
    CHECK_EVAL_SHORT("PRICEMAT(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);6%;5%;2)", Value(103.858482159));     // With Basis=2
    CHECK_EVAL_SHORT("PRICEMAT(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);6%;5%;3)", Value(103.824693325));     // With Basis=3
    CHECK_EVAL_SHORT("PRICEMAT(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);6%;5%;4)", Value(103.819218241));     // With Basis=4
    CHECK_EVAL_SHORT("PRICEMAT(DATE(1990;6;1);DATE(1992;12;31);DATE(1990;1;1);3%;2%;0)", Value(102.395007924));     //
    CHECK_EVAL_SHORT("PRICEMAT(DATE(1990;6;1);DATE(1992;12;31);DATE(1990;1;1);5%;3%;2)", Value(104.709020052));     //
    // alternate function name
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETPRICEMAT(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1);6%;5%  )", Value(103.819218241));
}

// PV
void TestFinancialFunctions::testPV()
{
    // TODO check type > 1, check div0
    // ODF
    CHECK_EVAL_SHORT("PV(10%;12;-100;100)", Value(649.5061005186));      // A trivial example of PV.
}

// PV_ANNUITY
void TestFinancialFunctions::testPV_ANNUITY()
{
    // kspread
    CHECK_EVAL_SHORT("PV_ANNUITY(1000;0.05;5)", Value(4329.47667063));
}

// RECEIVED
void TestFinancialFunctions::testRECEIVED()
{
    // ODF
    CHECK_EVAL_SHORT("RECEIVED(DATE(1990;6;1);DATE(1990;12;31);10000;5%)" , Value(10300.4291845494));     // Without Basis parameter
    // alternate function name
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETRECEIVED(DATE(1990;6;1);DATE(1990;12;31);10000;5%)" , Value(10300.4291845494));
}

// RRI
void TestFinancialFunctions::testRRI()
{
    CHECK_EVAL_SHORT("RRI(1;100;200)" ,     Value(1));                       // A trivial example of RRI.
    CHECK_EVAL_SHORT("RRI(12;5000;10000)" , Value(0.05946309436));           // RRI, practical example
    CHECK_EVAL_SHORT("RRI(12;10000;5000)" , Value(-0.056125687));            // If future value is less than present value, resultant rate is negative
    CHECK_EVAL_SHORT("RRI(0;100;200)" ,     Value(Value::errorVALUE()));     // N must be greater than 0.
}

// Straight-line depreciation
// SLN(cost, salvage, life)
void TestFinancialFunctions::testSLN()
{
    // Excel example: http://office.microsoft.com/en-us/excel/HP100623811033.aspx
    CHECK_EVAL("SLN(30000; 7500; 10)", Value(2250.0));

    // http://www.vni.com/products/imsl/jmsl/v30/api/com/imsl/finance/slnEx1.html
    CHECK_EVAL("SLN(2500; 500; 24)", Value(83.3333333333333));

    // http://www.gnome.org/projects/gnumeric/doc/gnumeric-SLN.shtml
    CHECK_EVAL("SLN(10000; 700; 10)", Value(930));

    // test cases in OpenFormula specification
    CHECK_EVAL("SLN(4000;500;4)", Value(875));
}

// Sum-of-years' digits depreciation
// SYD(cost, salvage, life, period)
void TestFinancialFunctions::testSYD()
{
    // Excel example: http://office.microsoft.com/en-us/excel/HP100623821033.aspx
    CHECK_EVAL("SYD(30000; 7500; 10; 1)",  Value(4090.909090909090));
    CHECK_EVAL("SYD(30000; 7500; 10; 10)", Value(409.0909090909090));

    // http://www.vni.com/products/imsl/jmsl/v30/api/com/imsl/finance/sydEx1.html
    CHECK_EVAL("SYD(25000; 5000; 15; 14)", Value(333.3333333333333));

    // http://www.gnome.org/projects/gnumeric/doc/gnumeric-SYD.shtml
    CHECK_EVAL("SYD(5000; 200; 5; 2)", Value(1280));

    // test cases in OpenFormula specification
    CHECK_EVAL("SYD(4000;500;4;2)", Value(1050));
}

// TBILLEQ
void TestFinancialFunctions::testTBILLEQ()
{
    // TODO check function, check OOo-2.2.1

    // ODF
    CHECK_EVAL("TBILLEQ(DATE(1996;01;01);DATE(1996;02;01);5%)", Value(0.0509136560));            //
    CHECK_EVAL("TBILLEQ(DATE(1995;12;31);DATE(1996;02;01);5%)", Value(0.0509207589));            // specs 0.050920759
    CHECK_EVAL("TBILLEQ(DATE(1995;12;31);DATE(1996;07;01);5%)", Value(0.0520091194));            // specs 0.052016531
    CHECK_EVAL("TBILLEQ(DATE(1995;12;31);DATE(1996;12;31);5%)", Value(Value::errorVALUE()));     // specs 0.053409423 OOo-2.2.1 Error(#VALUE!) 361 days
    CHECK_EVAL("TBILLEQ(DATE(1996;01;01);DATE(1996;06;30);5%)", Value(0.0519943020));            // specs 0.052001710
    CHECK_EVAL("TBILLEQ(DATE(1996;01;01);DATE(1996;07;01);5%)", Value(0.0520017096));            // specs 0.052009119
    CHECK_EVAL("TBILLEQ(DATE(1996;01;01);DATE(1996;12;31);5%)", Value(0.0533625731));            // specs 0.053401609
    CHECK_EVAL("TBILLEQ(DATE(1996;01;01);DATE(1997;01;01);5%)", Value(Value::errorVALUE()));     // specs 0.053409423 OOo-2.2.1 Error(#VALUE!) days 361
    CHECK_EVAL("TBILLEQ(DATE(1996;07;01);DATE(1997;07;01);5%)", Value(Value::errorVALUE()));     // specs 0.053401609 OOo-2.2.1 Error(#VALUE!) days 361
    // alternate function name
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETTBILLEQ(DATE(1996;01;01);DATE(1996;02;01);5%)", Value(0.0509136560));
}

// TBILLPRICE
void TestFinancialFunctions::testTBILLPRICE()
{
    // ODF
    CHECK_EVAL("TBILLPRICE(DATE(1996;01;01);DATE(1996;02;01);5%)", Value(99.5694444444));     //
    CHECK_EVAL("TBILLPRICE(DATE(1995;12;31);DATE(1996;02;01);5%)", Value(99.5555555555));     //
    CHECK_EVAL("TBILLPRICE(DATE(1995;12;31);DATE(1996;07;01);5%)", Value(97.4722222222));     // ODF specs 97.45833333 OOo-2.2.1 97.47222222
    CHECK_EVAL("TBILLPRICE(DATE(1995;12;31);DATE(1996;12;31);5%)", Value(94.9861111111));     // ODF specs 94.91666667 OOo-2.2.1 94.98611111
    CHECK_EVAL("TBILLPRICE(DATE(1996;01;01);DATE(1996;06;30);5%)", Value(97.5000000000));     // ODF specs 97.48611111 OOo-2.2.1 97.50000000
    CHECK_EVAL("TBILLPRICE(DATE(1996;01;01);DATE(1996;07;01);5%)", Value(97.4861111111));     // ODF specs 97.47222222 OOo-2.2.1 97.48611111
    CHECK_EVAL("TBILLPRICE(DATE(1996;01;01);DATE(1996;12;31);5%)", Value(Value::errorVALUE()));     // ODF specs 94.93055556 OOo-2.2.1 Err:502
    CHECK_EVAL("TBILLPRICE(DATE(1996;01;01);DATE(1997;01;01);5%)", Value(94.9861111111));     // ODF specs 94.91666667 OOo-2.2.1 94.98611111
    CHECK_EVAL("TBILLPRICE(DATE(1996;07;01);DATE(1997;07;01);5%)", Value(94.9861111111));     // ODF specs 94.93055556 OOo-2.2.1 94.98611111
    // alternate function name
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETTBILLPRICE(DATE(1996;01;01);DATE(1996;02;01);5%)", Value(99.5694444444));
}

// TBILLYIELD
void TestFinancialFunctions::testTBILLYIELD()
{
    // ODF tests. All results are taken from OOo-2.2.1 instead of results from ODF-specs

    // new implementation uses day360(US) to get daydiff. TODO check if we should test against 361 days instead 360 in function to get Error away
    CHECK_EVAL("TBILLYIELD(DATE(1996;01;01);DATE(1996;02;01);99.57)", Value(0.0501511337));            //
    CHECK_EVAL("TBILLYIELD(DATE(1995;12;31);DATE(1996;02;01);99.56)", Value(0.0497187626));            //
    CHECK_EVAL("TBILLYIELD(DATE(1995;12;31);DATE(1996;07;01);97.46)", Value(0.0515511576));            // specs 0.0512695
    CHECK_EVAL("TBILLYIELD(DATE(1995;12;31);DATE(1996;12;31);94.92)", Value(Value::errorVALUE()));     // specs 0.0526414 OOo-2.2.1 Error(#VALUE!)
    CHECK_EVAL("TBILLYIELD(DATE(1996;01;01);DATE(1996;06;30);97.49)", Value(0.0514924608));            // specs 0.0512080
    CHECK_EVAL("TBILLYIELD(DATE(1996;01;01);DATE(1996;07;01);97.47)", Value(0.0516265948));            // specs 0.0513429
    CHECK_EVAL("TBILLYIELD(DATE(1996;01;01);DATE(1996;12;31);94.93)", Value(Value::errorVALUE()));     // specs 0.0526762 OOo-2.2.1 Error(#VALUE!)
    CHECK_EVAL("TBILLYIELD(DATE(1996;01;01);DATE(1997;01;01);94.92)", Value(Value::errorVALUE()));     // specs 0.0526414 OOo-2.2.1 Error(#VALUE!)
    CHECK_EVAL("TBILLYIELD(DATE(1996;07;01);DATE(1997;07;01);94.93)", Value(Value::errorVALUE()));     // specs 0.0526762 OOo-2.2.1 Error(#VALUE!)
    // alternate function name
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETTBILLYIELD(DATE(1996;01;01);DATE(1996;02;01);99.57)", Value(0.0501511337));
}

// VDB
void TestFinancialFunctions::testVDB()
{
    // ODF
    CHECK_EVAL("VDB(10000;600;10;0    ;0.875;1.5)", Value(1312.50));
    CHECK_EVAL("VDB(10000;600;10;0.875;1.875;1.5)", Value(1303.125));
    CHECK_EVAL("VDB(10000;600;10;1.875;2.875;1.5)", Value(1107.65625));
    CHECK_EVAL("VDB(10000;600;10;2.875;3.875;1.5)", Value(941.5078125));
    CHECK_EVAL("VDB(10000;600;10;3.875;4.875;1.5)", Value(800.2816406250));
    CHECK_EVAL("VDB(10000;600;10;4.875;5.875;1.5)", Value(767.7910823171));
    CHECK_EVAL("VDB(10000;600;10;5.875;6.875;1.5)", Value(767.410625));
    CHECK_EVAL("VDB(10000;600;10;6.875;7.875;1.5)", Value(767.410625));
    CHECK_EVAL("VDB(10000;600;10;7.875;8.875;1.5)", Value(767.410625));
    CHECK_EVAL("VDB(10000;600;10;8.875;9.875;1.5)", Value(767.410625));
    CHECK_EVAL("VDB(10000;600;10;9.875;10   ;1.5)", Value(95.9263281250));
}

// XIRR
void TestFinancialFunctions::testXIRR()
{
    // ODF
    CHECK_EVAL_SHORT("XIRR( {-20000;4000;12000;8000}; {date(2000;01;01); date(2000;06;01); date(2000;12;30); date(2001;03;01)} )", Value(0.2115964));     //
    CHECK_EVAL_SHORT("XIRR( {-20000;25000};           {date(2000;01;01); date(2001;01;01)} )",                                     Value(0.2492381));     //
    CHECK_EVAL_SHORT("XIRR( {-10000;4000;12000};      {date(2000;01;01); date(2002;06;01); date(2004;01;01)} )",                   Value(0.1405418));     //
    // alternate function name
    CHECK_EVAL_SHORT("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETXIRR( {-20000;25000};{date(2000;01;01); date(2001;01;01)} )", Value(0.2492381));
}

// XNPV
void TestFinancialFunctions::testXNPV()
{
    // bettersolution.com
    CHECK_EVAL("XNPV(0.1;  {-1000;2000;3000};       {date(2005;01;01); date(2005;01;10); date(2005;01;15)})" , Value(3984.3581140636));         //

    // with dates {date(2005;01;01); date(2005;03;01); date(2005;10;30); date(2006;02;15)}
    CHECK_EVAL("XNPV(0.09; {-10000;2750;4250;3250}; {38353;38412;38655;38763})",                               Value(-380.3891178530));         //
    CHECK_EVAL("XNPV(30;   {-10000;2750;4250;3250}; {38353;38412;38655;38763})",                               Value(-8104.7862519770));        //
    CHECK_EVAL("XNPV(-30;  {-10000;2750;4250;3250}; {38353;38412;38655;38763})",                               Value(Value::errorNUM()));       //
    CHECK_EVAL("XNPV(0.09; {-10000;2750};           {date(2005;01;01); date(2005;01;10); date(2005;01;15)})",  Value(Value::errorNUM()));       //
    CHECK_EVAL("XNPV(0.1;  {-1000;2000;3000};       {\"fail\"; date(2005;01;10); date(2005;01;15)})",          Value(Value::errorVALUE()));     //

    // alternate function name
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETXNPV(0.09; {-10000;2750;4250;3250}; {38353;38412;38655;38763})",Value(-380.3891178530));
}

// YIELDDISC
void TestFinancialFunctions::testYIELDDISC()
{
    // ODF
    CHECK_EVAL("YIELDDISC(DATE(1990;06;01);DATE(1990;12;31);941.66667;1000   )", Value(0.1061946838));     // Without Basis parameter
    CHECK_EVAL("YIELDDISC(DATE(1990;06;01);DATE(1990;12;31);941.64384;1000; 1)", Value(0.1061972566));     // With Basis=1 specs 0.106238821 OOo-2.2.1 0.1061972566
    CHECK_EVAL("YIELDDISC(DATE(1990;06;01);DATE(1990;12;31);940.83333;1000; 2)", Value(0.1062887575));     // With Basis=2 specs 0.107807168 OOo-2.2.1 0.1062887575
    CHECK_EVAL("YIELDDISC(DATE(1990;06;01);DATE(1990;12;31);941.64384;1000; 3)", Value(0.1061972566));     // With Basis=3 specs 0.106238821 OOo-2.2.1 0.1061972566
    CHECK_EVAL("YIELDDISC(DATE(1990;06;01);DATE(1990;12;31);941.94444;1000; 4)", Value(0.1061633823));     // With Basis=4 specs 0.105657842 OOo-2.2.1 0.1061633823
    CHECK_EVAL("YIELDDISC(DATE(1990;01;01);DATE(1990;12;31);97.08219;100;   1)", Value(0.0301376180));     // specs 0.051522942 OOo-2.2.1 0.0301376180
    CHECK_EVAL("YIELDDISC(DATE(1990;06;01);DATE(1990;06;30);99.75833;100;   4)", Value(0.0300730914));     //
    // alternate function name
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETYIELDDISC(DATE(1990;06;01);DATE(1990;12;31);941.66667;1000   )", Value(0.1061946838));
}

// YIELDMAT
void TestFinancialFunctions::testYIELDMAT()
{
    // ODF
    CHECK_EVAL_SHORT("YIELDMAT(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1); 6%;103.819218241  )", Value(0.050000000));      // Without Basis parameter
    CHECK_EVAL_SHORT("YIELDMAT(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1); 6%;103.824693325;1)", Value(0.050000000));      // With Basis=1
//   CHECK_EVAL_SHORT( "YIELDMAT(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1); 6%;103.858482159;2)", Value( 0.050000000  ) ); // With Basis=2
    CHECK_EVAL_SHORT("YIELDMAT(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1); 6%;103.824693325;3)", Value(0.050000000));      // With Basis=3
//   CHECK_EVAL_SHORT( "YIELDMAT(DATE(1990;6;1);DATE(1992;12;31);DATE(1990;1;1); 6%;103.817732653;4)", Value( 0.050000000  ) ); // With Basis=4 NOK diff = 0.0074805
    CHECK_EVAL_SHORT("YIELDMAT(DATE(1990;6;1);DATE(1992;12;31);DATE(1990;1;1); 3%;102.395007924;0)", Value(0.020000000));      // With Basis=0
//   CHECK_EVAL_SHORT( "YIELDMAT(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1); 5%;102.967175933;2)", Value( 0.030000000  ) ); // With Basis=2 NOK diff = -0.0126036

    // alternate function name
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETYIELDMAT(DATE(1990;6;1);DATE(1995;12;31);DATE(1990;1;1); 6%;103.819218241  )", Value(0.050000000));
}

// Zero-coupon (pure discount) bond
// ZERO_COUPON(faceValue; rate; years)
void TestFinancialFunctions::testZEROCOUPON()
{
    CHECK_EVAL("ZERO_COUPON(1000;.1;20)",     Value(148.6436280241434531));
    CHECK_EVAL("ZERO_COUPON(1000;.2;20)",     Value(26.0840533045888456));
    CHECK_EVAL("ZERO_COUPON(1000;.15/12;10)", Value(883.1809261539680165));
    CHECK_EVAL("ZERO_COUPON(1000;.25;1)",     Value(800));
}


QTEST_KDEMAIN(TestFinancialFunctions, GUI)

#include "TestFinancialFunctions.moc"
