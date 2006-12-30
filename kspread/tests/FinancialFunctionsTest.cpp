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
  printf("  CHECK_EVAL( \"%s\", Value((double) %.14f);\n", qPrintable(formula), result.asFloat());
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
	CHECK_EVAL( "DDB(2400; 300; 10; 10; 2)", Value((double)22.12254720000015595360) ) ;
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

// Straight-line depreciation
// SLN(cost, salvage, life)
void FinancialFunctionsTest::testSLN()
{
	// Excel example: http://office.microsoft.com/en-us/excel/HP100623811033.aspx
	CHECK_EVAL( "SLN(30000; 7500; 10)", Value((double)2250.0) ) ;
}

// Sum-of-years' digits depreciation
// SYD(cost, salvage, life, period)
void FinancialFunctionsTest::testSYD()
{
	// Excel example: http://office.microsoft.com/en-us/excel/HP100623821033.aspx
	CHECK_EVAL( "SYD(30000; 7500; 10; 1)", Value((double)4090.9090909090909918) ) ;
	CHECK_EVAL( "SYD(30000; 7500; 10; 10)", Value((double)409.09090909090906507) ) ;
}

QTEST_KDEMAIN(FinancialFunctionsTest, GUI)
#include "FinancialFunctionsTest.moc"
