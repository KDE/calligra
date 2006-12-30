/* This file is part of the KDE project
   Copyright 2006 Ariya Hidayat <ariya@kde.org>

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

#include <math.h>

#include "qtest_kde.h"

#include <Formula.h>
#include <Value.h>

#include "FinancialFunctionsTest.h"

using namespace KSpread;

#define CHECK_EVAL(x,y) QCOMPARE(evaluate(x),y)

Value FinancialFunctionsTest::evaluate(const QString& formula)
{
  Formula f;
  QString expr = formula;
  if ( expr[0] != '=' )
    expr.prepend( '=' );
  f.setExpression( expr );
  Value result = f.eval();

#if 0
  // this magically generates the CHECKs
  printf("  CHECK_EVAL( \"%s\", Value((double) %.16f) );\n", qPrintable(formula), result.asFloat());
#endif

  return result;
}

namespace QTest 
{
  template<>
  char *toString(const Value& value)
  {
    QString message;
    QTextStream ts( &message, QIODevice::WriteOnly );
    if( value.isFloat() )
      ts << QString::number(value.asFloat(), 'g', 20);
    else  
      ts << value;
    return qstrdup(message.toLatin1());
  }
}

// Fixed-declining balance depreciation
// DB(cost, salvage, life, period, month)
void FinancialFunctionsTest::testDB()
{
  // Excel example: http://office.microsoft.com/en-us/excel/HP100623551033.aspx
	CHECK_EVAL( "DB(1000000; 100000; 6; 1; 7)", Value((double)186083.33333333334303) ) ;
	CHECK_EVAL( "DB(1000000; 100000; 6; 2; 7)", Value((double)259639.41666666665697) ) ;
	CHECK_EVAL( "DB(1000000; 100000; 6; 3; 7)", Value((double)176814.44275000001653) ) ;
	CHECK_EVAL( "DB(1000000; 100000; 6; 4; 7)", Value((double)120410.63551274998463) ) ;
	CHECK_EVAL( "DB(1000000; 100000; 6; 5; 7)", Value((double)81999.642784182738978) ) ;
	CHECK_EVAL( "DB(1000000; 100000; 6; 6; 7)", Value((double)55841.756736028459272) ) ;
	CHECK_EVAL( "DB(1000000; 100000; 6; 7; 7)", Value((double)15845.09847384807108) ) ;
	
	// http://www.vni.com/products/imsl/jmsl/v30/api/com/imsl/finance/dbEx1.html
  CHECK_EVAL( "DB(2500; 500; 3; 1; 6)", Value((double) 518.75000000000000));
  CHECK_EVAL( "DB(2500; 500; 3; 2; 6)", Value((double) 822.21875000000000));
  CHECK_EVAL( "DB(2500; 500; 3; 3; 6)", Value((double) 480.99796874999998));
  CHECK_EVAL( "DB(2500; 500; 3; 4; 6)", Value((double) 140.69190585937500));
}

// Double declining balance depreciation
// DDB(cost, salvage, life, period, factor)
void FinancialFunctionsTest::testDDB()
{
	// Excel example: http://office.microsoft.com/en-us/excel/HP100623561033.aspx
	CHECK_EVAL( "DDB(2400; 300; 10*365; 1; 2)", Value((double)1.3150684931506848585) ) ;
	CHECK_EVAL( "DDB(2400; 300; 10*12; 1; 2)", Value((double)40.0) ) ;
	CHECK_EVAL( "DDB(2400; 300; 10; 1; 2)", Value((double)480.0) ) ;
	CHECK_EVAL( "DDB(2400; 300; 10; 2; 1.5)", Value((double)306) ) ;
	//CHECK_EVAL( "DDB(2400; 300; 10; 10; 2)", Value((double)22.12254720000015595360) ) ;
	
	// http://www.vni.com/products/imsl/jmsl/v30/api/com/imsl/finance/ddbEx1.html
  CHECK_EVAL( "DDB(2500; 500; 24; 1; 2)", Value((double) 208.33333333333331));
  CHECK_EVAL( "DDB(2500; 500; 24; 2; 2)", Value((double) 190.97222222222220));
  CHECK_EVAL( "DDB(2500; 500; 24; 3; 2)", Value((double) 175.05787037037035));
  CHECK_EVAL( "DDB(2500; 500; 24; 4; 2)", Value((double) 160.46971450617283));
  CHECK_EVAL( "DDB(2500; 500; 24; 5; 2)", Value((double) 147.09723829732508));
  CHECK_EVAL( "DDB(2500; 500; 24; 6; 2)", Value((double) 134.83913510588133));
  CHECK_EVAL( "DDB(2500; 500; 24; 7; 2)", Value((double) 123.60254051372456));
  CHECK_EVAL( "DDB(2500; 500; 24; 8; 2)", Value((double) 113.30232880424752));
  CHECK_EVAL( "DDB(2500; 500; 24; 9; 2)", Value((double) 103.86046807056023));
  CHECK_EVAL( "DDB(2500; 500; 24; 10; 2)", Value((double) 95.20542906468020));
  CHECK_EVAL( "DDB(2500; 500; 24; 11; 2)", Value((double) 87.27164330929020));
  CHECK_EVAL( "DDB(2500; 500; 24; 12; 2)", Value((double) 79.99900636684936));
  CHECK_EVAL( "DDB(2500; 500; 24; 13; 2)", Value((double) 73.33242250294525));
  CHECK_EVAL( "DDB(2500; 500; 24; 14; 2)", Value((double) 67.22138729436648));
  CHECK_EVAL( "DDB(2500; 500; 24; 15; 2)", Value((double) 61.619605019835944404));
  CHECK_EVAL( "DDB(2500; 500; 24; 16; 2)", Value((double) 56.484637934849615704));
  CHECK_EVAL( "DDB(2500; 500; 24; 17; 2)", Value((double) 51.77758477361215));
  CHECK_EVAL( "DDB(2500; 500; 24; 18; 2)", Value((double) 47.46278604247781));
  CHECK_EVAL( "DDB(2500; 500; 24; 19; 2)", Value((double) 43.50755387227133));
  CHECK_EVAL( "DDB(2500; 500; 24; 20; 2)", Value((double) 39.881924382915386218));
  CHECK_EVAL( "DDB(2500; 500; 24; 21; 2)", Value((double) 36.55843068433910));
  CHECK_EVAL( "DDB(2500; 500; 24; 22; 2)", Value((double) 33.51189479397749));
  CHECK_EVAL( "DDB(2500; 500; 24; 23; 2)", Value((double) 30.719236894479383437));
  CHECK_EVAL( "DDB(2500; 500; 24; 24; 2)", Value((double) 28.159300486606106517));
}

// Euro conversion
// EURO(currency)
void FinancialFunctionsTest::testEURO()
{
  CHECK_EVAL( "EURO(\"ATS\")", Value(13.7603) );
  CHECK_EVAL( "EURO(\"BEF\")", Value(40.3399) );
  CHECK_EVAL( "EURO(\"DEM\")", Value(1.95583) );
  CHECK_EVAL( "EURO(\"ESP\")", Value(166.386) );
  CHECK_EVAL( "EURO(\"FIM\")", Value(5.94573) );
  CHECK_EVAL( "EURO(\"FRF\")", Value(6.55957) );
  CHECK_EVAL( "EURO(\"GRD\")", Value(340.75) );
  CHECK_EVAL( "EURO(\"IEP\")", Value(0.787564) );
  CHECK_EVAL( "EURO(\"ITL\")", Value(1936.27) );
  CHECK_EVAL( "EURO(\"LUX\")", Value(40.3399) );
  CHECK_EVAL( "EURO(\"NLG\")", Value(2.20371) );
  CHECK_EVAL( "EURO(\"PTE\")", Value(200.482) );

  CHECK_EVAL( "EURO(\"NOMANSLAND\")", Value::errorVALUE() );
}

// Level-coupon bond
// LEVEL_COUPON(faceValue; couponRate; couponsPerYear; years; marketRate)
void FinancialFunctionsTest::testLEVELCOUPON()
{
  CHECK_EVAL( "LEVEL_COUPON(1000; .13; 1; 4; .1)", Value((double) 1095.0959633904788006));
  CHECK_EVAL( "LEVEL_COUPON(1000; .13; 2; 4; .1)", Value((double) 1096.9481913913939479));
  CHECK_EVAL( "LEVEL_COUPON(1000; .10; 1; 10; .25)", Value((double) 464.4245094400000085));
  CHECK_EVAL( "LEVEL_COUPON(1000; .12; 1; 10; .25)", Value((double) 535.8345748479999884));
  CHECK_EVAL( "LEVEL_COUPON(1000; .20; 1; 10; .25)", Value((double) 821.4748364800000218));
}

// Yearly nominal interest rate
// NOMINAL(effectiveRate, periods)
void FinancialFunctionsTest::testNOMINAL()
{
  CHECK_EVAL( "NOMINAL(13.5%; 12)", Value((double) 0.1273031669590416));
  CHECK_EVAL( "NOMINAL(13.5%; 12)", Value((double) 0.1273031669590416));
  CHECK_EVAL( "NOMINAL(25%; 12)", Value((double) 0.2252311814580734));
  CHECK_EVAL( "NOMINAL(25%; 4)", Value((double) 0.2294850537622564));
  CHECK_EVAL( "NOMINAL(20%; 12)", Value((double) 0.1837136459967743));
  CHECK_EVAL( "NOMINAL(10%; 12)", Value((double) 0.0956896851468452));
  
  // rate must be positive
  CHECK_EVAL( "NOMINAL(0; 12)", Value::errorVALUE());

  // periods must be positive
  CHECK_EVAL( "NOMINAL(10%; 0)", Value::errorDIV0());
  CHECK_EVAL( "NOMINAL(10%; -1)", Value::errorVALUE());
  CHECK_EVAL( "NOMINAL(10%; -2)", Value::errorVALUE());
}

// Straight-line depreciation
// SLN(cost, salvage, life)
void FinancialFunctionsTest::testSLN()
{
	// Excel example: http://office.microsoft.com/en-us/excel/HP100623811033.aspx
	CHECK_EVAL( "SLN(30000; 7500; 10)", Value((double)2250.0) ) ;
	
	// http://www.vni.com/products/imsl/jmsl/v30/api/com/imsl/finance/slnEx1.html
	CHECK_EVAL( "SLN(2500; 500; 24)", Value((double)83.333333333333328596) ) ;
	
	// http://www.gnome.org/projects/gnumeric/doc/gnumeric-SLN.shtml
	CHECK_EVAL( "SLN(10000; 700; 10)", Value((double)930) );
}

// Sum-of-years' digits depreciation
// SYD(cost, salvage, life, period)
void FinancialFunctionsTest::testSYD()
{
	// Excel example: http://office.microsoft.com/en-us/excel/HP100623821033.aspx
	CHECK_EVAL( "SYD(30000; 7500; 10; 1)", Value((double)4090.9090909090909918) ) ;
	CHECK_EVAL( "SYD(30000; 7500; 10; 10)", Value((double)409.09090909090906507) ) ;
	
	// http://www.vni.com/products/imsl/jmsl/v30/api/com/imsl/finance/sydEx1.html
	CHECK_EVAL( "SYD(25000; 5000; 15; 14)", Value((double)333.3333333333333) ) ;
	
	// http://www.gnome.org/projects/gnumeric/doc/gnumeric-SYD.shtml
	CHECK_EVAL( "SYD(5000; 200; 5; 2)", Value((double)1280) );
}

// Zero-coupon (pure discount) bond
// ZERO_COUPON(faceValue; rate; years)
void FinancialFunctionsTest::testZEROCOUPON()
{
  CHECK_EVAL( "ZERO_COUPON(1000;.1;20)", Value((double) 148.6436280241434531));
  CHECK_EVAL( "ZERO_COUPON(1000;.2;20)", Value((double) 26.0840533045888456));
  CHECK_EVAL( "ZERO_COUPON(1000;.15/12;10)", Value((double) 883.1809261539680165));
  CHECK_EVAL( "ZERO_COUPON(1000;.25;1)", Value((double) 800));
}

QTEST_KDEMAIN(FinancialFunctionsTest, GUI)
#include "FinancialFunctionsTest.moc"
