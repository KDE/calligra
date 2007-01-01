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
	CHECK_EVAL( "DDB(2400; 300; 10; 10; 2)", Value((double)22.12254720000009911) ) ;
	
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
  CHECK_EVAL( "DDB(2500; 500; 24; 24; 2)", Value((double) -162.08839416072669337));
  
  // try default factor (=2)
  CHECK_EVAL( "DDB(2400; 300; 10*12; 1)", Value((double)40.0) ) ;
  CHECK_EVAL( "DDB(2400; 300; 10; 1)", Value((double)480.0) ) ;
  CHECK_EVAL( "DDB(2500; 500; 24; 22)", Value((double) 33.51189479397749));
  CHECK_EVAL( "DDB(2500; 500; 24; 23)", Value((double) 30.719236894479383437));
  CHECK_EVAL( "DDB(2500; 500; 24; 24)", Value((double) -162.08839416072669337));
}

// Euro conversion
// EURO(currency)
void FinancialFunctionsTest::testEURO()
{
  CHECK_EVAL( "EURO(\"ATS\")", Value(13.7603) );
  CHECK_EVAL( "EURO(\"BEF\")", Value(40.3399) );
  CHECK_EVAL( "EURO(\"DEM\")", Value(1.95583) );
  CHECK_EVAL( "EURO(\"ESP\")", Value(166.386) );
  CHECK_EVAL( "EURO(\"EUR\")", Value(1.0) );
  CHECK_EVAL( "EURO(\"FIM\")", Value(5.94573) );
  CHECK_EVAL( "EURO(\"FRF\")", Value(6.55957) );
  CHECK_EVAL( "EURO(\"GRD\")", Value(340.75) );
  CHECK_EVAL( "EURO(\"IEP\")", Value(0.787564) );
  CHECK_EVAL( "EURO(\"ITL\")", Value(1936.27) );
  CHECK_EVAL( "EURO(\"LUX\")", Value(40.3399) );
  CHECK_EVAL( "EURO(\"NLG\")", Value(2.20371) );
  CHECK_EVAL( "EURO(\"PTE\")", Value(200.482) );
  
  // should still work with lowercase 
  CHECK_EVAL( "EURO(\"ats\")", Value(13.7603) );
  CHECK_EVAL( "EURO(\"bef\")", Value(40.3399) );
  CHECK_EVAL( "EURO(\"dem\")", Value(1.95583) );
  CHECK_EVAL( "EURO(\"esp\")", Value(166.386) );
  CHECK_EVAL( "EURO(\"eur\")", Value(1.0) );
  CHECK_EVAL( "EURO(\"fim\")", Value(5.94573) );
  CHECK_EVAL( "EURO(\"frf\")", Value(6.55957) );
  CHECK_EVAL( "EURO(\"grd\")", Value(340.75) );
  CHECK_EVAL( "EURO(\"iep\")", Value(0.787564) );
  CHECK_EVAL( "EURO(\"itl\")", Value(1936.27) );
  CHECK_EVAL( "EURO(\"lux\")", Value(40.3399) );
  CHECK_EVAL( "EURO(\"nlg\")", Value(2.20371) );
  CHECK_EVAL( "EURO(\"pte\")", Value(200.482) );

  // should still work with mixed-case
  CHECK_EVAL( "EURO(\"Ats\")", Value(13.7603) );
  CHECK_EVAL( "EURO(\"Bef\")", Value(40.3399) );
  CHECK_EVAL( "EURO(\"Dem\")", Value(1.95583) );
  CHECK_EVAL( "EURO(\"Esp\")", Value(166.386) );
  CHECK_EVAL( "EURO(\"Eur\")", Value(1.0) );
  CHECK_EVAL( "EURO(\"Fim\")", Value(5.94573) );
  CHECK_EVAL( "EURO(\"Frf\")", Value(6.55957) );
  CHECK_EVAL( "EURO(\"GrD\")", Value(340.75) );
  CHECK_EVAL( "EURO(\"IeP\")", Value(0.787564) );
  CHECK_EVAL( "EURO(\"Itl\")", Value(1936.27) );
  CHECK_EVAL( "EURO(\"luX\")", Value(40.3399) );
  CHECK_EVAL( "EURO(\"nlG\")", Value(2.20371) );
  CHECK_EVAL( "EURO(\"ptE\")", Value(200.482) );

  CHECK_EVAL( "EURO(\"NOMANSLAND\")", Value::errorNUM() );
}

// Currency conversion using Euro
// EUROCONVERT(number,source,target)
void FinancialFunctionsTest::testEUROCONVERT()
{
  // 1 Euro to ...
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"ATS\")", Value((double)13.7603) );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"BEF\")", Value((double)40.3399) );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"DEM\")", Value((double)1.95583) );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"ESP\")", Value((double)166.386) );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"EUR\")", Value((double)1.0) );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"FIM\")", Value((double)5.94573) );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"FRF\")", Value((double)6.55957) );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"GRD\")", Value((double)340.75) );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"IEP\")", Value((double)0.787564) );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"ITL\")", Value((double)1936.27) );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"LUX\")", Value((double)40.3399) );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"NLG\")", Value((double)2.20371) );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"PTE\")", Value((double)200.482) );
  
  // identity
  CHECK_EVAL( "EUROCONVERT(1;\"BEF\";\"bef\")", Value((double)1.0) );
  CHECK_EVAL( "EUROCONVERT(1;\"DEM\";\"dem\")", Value((double)1.0) );
  CHECK_EVAL( "EUROCONVERT(1;\"ESP\";\"esp\")", Value((double)1.0) );
  CHECK_EVAL( "EUROCONVERT(1;\"EUR\";\"eur\")", Value((double)1.0) );
  CHECK_EVAL( "EUROCONVERT(1;\"FIM\";\"fim\")", Value((double)1.0) );
  CHECK_EVAL( "EUROCONVERT(1;\"FRF\";\"frf\")", Value((double)1.0) );
  CHECK_EVAL( "EUROCONVERT(1;\"GRD\";\"grd\")", Value((double)1.0) );
  CHECK_EVAL( "EUROCONVERT(1;\"IEP\";\"iep\")", Value((double)1.0) );
  CHECK_EVAL( "EUROCONVERT(1;\"ITL\";\"itl\")", Value((double)1.0) );
  CHECK_EVAL( "EUROCONVERT(1;\"LUX\";\"lux\")", Value((double)1.0) );
  CHECK_EVAL( "EUROCONVERT(1;\"NLG\";\"nlg\")", Value((double)1.0) );
  CHECK_EVAL( "EUROCONVERT(1;\"PTE\";\"pte\")", Value((double)1.0) );
  
  // all other combinations
  CHECK_EVAL( "EUROCONVERT(   2; \"ATS\"; \"bef\" )", Value((double)2*40.3399/13.7603) );
  CHECK_EVAL( "EUROCONVERT(   3; \"ATS\"; \"dem\" )", Value((double)3*1.95583/13.7603) );
  CHECK_EVAL( "EUROCONVERT(   4; \"ATS\"; \"esp\" )", Value((double)4*166.386/13.7603) );
  CHECK_EVAL( "EUROCONVERT(   5; \"ATS\"; \"eur\" )", Value((double)5*1/13.7603) );
  CHECK_EVAL( "EUROCONVERT(   6; \"ATS\"; \"fim\" )", Value((double)6*5.94573/13.7603) );
  CHECK_EVAL( "EUROCONVERT(   7; \"ATS\"; \"frf\" )", Value((double)7*6.55957/13.7603) );
  CHECK_EVAL( "EUROCONVERT(   8; \"ATS\"; \"grd\" )", Value((double)8*340.75/13.7603) );
  CHECK_EVAL( "EUROCONVERT(   9; \"ATS\"; \"iep\" )", Value((double)9*0.787564/13.7603) );
  CHECK_EVAL( "EUROCONVERT(  10; \"ATS\"; \"itl\" )", Value((double)10*1936.27/13.7603) );
  CHECK_EVAL( "EUROCONVERT(  11; \"ATS\"; \"lux\" )", Value((double)11*40.3399/13.7603) );
  CHECK_EVAL( "EUROCONVERT(  12; \"ATS\"; \"nlg\" )", Value((double)12*2.20371/13.7603) );
  CHECK_EVAL( "EUROCONVERT(  13; \"ATS\"; \"pte\" )", Value((double)13*200.482/13.7603) );
  CHECK_EVAL( "EUROCONVERT(  14; \"BEF\"; \"ats\" )", Value((double)14*13.7603/40.3399) );
  CHECK_EVAL( "EUROCONVERT(  15; \"BEF\"; \"dem\" )", Value((double)15*1.95583/40.3399) );
  CHECK_EVAL( "EUROCONVERT(  16; \"BEF\"; \"esp\" )", Value((double)16*166.386/40.3399) );
  CHECK_EVAL( "EUROCONVERT(  17; \"BEF\"; \"eur\" )", Value((double)17*1/40.3399) );
  CHECK_EVAL( "EUROCONVERT(  18; \"BEF\"; \"fim\" )", Value((double)18*5.94573/40.3399) );
  CHECK_EVAL( "EUROCONVERT(  19; \"BEF\"; \"frf\" )", Value((double)19*6.55957/40.3399) );
  CHECK_EVAL( "EUROCONVERT(  20; \"BEF\"; \"grd\" )", Value((double)20*340.75/40.3399) );
  CHECK_EVAL( "EUROCONVERT(  21; \"BEF\"; \"iep\" )", Value((double)21*0.787564/40.3399) );
  CHECK_EVAL( "EUROCONVERT(  22; \"BEF\"; \"itl\" )", Value((double)22*1936.27/40.3399) );
  CHECK_EVAL( "EUROCONVERT(  23; \"BEF\"; \"lux\" )", Value((double)23*40.3399/40.3399) );
  CHECK_EVAL( "EUROCONVERT(  24; \"BEF\"; \"nlg\" )", Value((double)24*2.20371/40.3399) );
  CHECK_EVAL( "EUROCONVERT(  25; \"BEF\"; \"pte\" )", Value((double)25*200.482/40.3399) );
  CHECK_EVAL( "EUROCONVERT(  26; \"DEM\"; \"ats\" )", Value((double)26*13.7603/1.95583) );
  CHECK_EVAL( "EUROCONVERT(  27; \"DEM\"; \"bef\" )", Value((double)27*40.3399/1.95583) );
  CHECK_EVAL( "EUROCONVERT(  28; \"DEM\"; \"esp\" )", Value((double)28*166.386/1.95583) );
  CHECK_EVAL( "EUROCONVERT(  29; \"DEM\"; \"eur\" )", Value((double)29*1/1.95583) );
  CHECK_EVAL( "EUROCONVERT(  30; \"DEM\"; \"fim\" )", Value((double)30*5.94573/1.95583) );
  CHECK_EVAL( "EUROCONVERT(  31; \"DEM\"; \"frf\" )", Value((double)31*6.55957/1.95583) );
  CHECK_EVAL( "EUROCONVERT(  32; \"DEM\"; \"grd\" )", Value((double)32*340.75/1.95583) );
  CHECK_EVAL( "EUROCONVERT(  33; \"DEM\"; \"iep\" )", Value((double)33*0.787564/1.95583) );
  CHECK_EVAL( "EUROCONVERT(  34; \"DEM\"; \"itl\" )", Value((double)34*1936.27/1.95583) );
  CHECK_EVAL( "EUROCONVERT(  35; \"DEM\"; \"lux\" )", Value((double)35*40.3399/1.95583) );
  CHECK_EVAL( "EUROCONVERT(  36; \"DEM\"; \"nlg\" )", Value((double)36*2.20371/1.95583) );
  CHECK_EVAL( "EUROCONVERT(  37; \"DEM\"; \"pte\" )", Value((double)37*200.482/1.95583) );
  CHECK_EVAL( "EUROCONVERT(  38; \"ESP\"; \"ats\" )", Value((double)38*13.7603/166.386) );
  CHECK_EVAL( "EUROCONVERT(  39; \"ESP\"; \"bef\" )", Value((double)39*40.3399/166.386) );
  CHECK_EVAL( "EUROCONVERT(  40; \"ESP\"; \"dem\" )", Value((double)40*1.95583/166.386) );
  CHECK_EVAL( "EUROCONVERT(  41; \"ESP\"; \"eur\" )", Value((double)41*1/166.386) );
  CHECK_EVAL( "EUROCONVERT(  42; \"ESP\"; \"fim\" )", Value((double)42*5.94573/166.386) );
  CHECK_EVAL( "EUROCONVERT(  43; \"ESP\"; \"frf\" )", Value((double)43*6.55957/166.386) );
  CHECK_EVAL( "EUROCONVERT(  44; \"ESP\"; \"grd\" )", Value((double)44*340.75/166.386) );
  CHECK_EVAL( "EUROCONVERT(  45; \"ESP\"; \"iep\" )", Value((double)45*0.787564/166.386) );
  CHECK_EVAL( "EUROCONVERT(  46; \"ESP\"; \"itl\" )", Value((double)46*1936.27/166.386) );
  CHECK_EVAL( "EUROCONVERT(  47; \"ESP\"; \"lux\" )", Value((double)47*40.3399/166.386) );
  CHECK_EVAL( "EUROCONVERT(  48; \"ESP\"; \"nlg\" )", Value((double)48*2.20371/166.386) );
  CHECK_EVAL( "EUROCONVERT(  49; \"ESP\"; \"pte\" )", Value((double)49*200.482/166.386) );
  CHECK_EVAL( "EUROCONVERT(  50; \"EUR\"; \"ats\" )", Value((double)50*13.7603/1) );
  CHECK_EVAL( "EUROCONVERT(  51; \"EUR\"; \"bef\" )", Value((double)51*40.3399/1) );
  CHECK_EVAL( "EUROCONVERT(  52; \"EUR\"; \"dem\" )", Value((double)52*1.95583/1) );
  CHECK_EVAL( "EUROCONVERT(  53; \"EUR\"; \"esp\" )", Value((double)53*166.386/1) );
  CHECK_EVAL( "EUROCONVERT(  54; \"EUR\"; \"fim\" )", Value((double)54*5.94573/1) );
  CHECK_EVAL( "EUROCONVERT(  55; \"EUR\"; \"frf\" )", Value((double)55*6.55957/1) );
  CHECK_EVAL( "EUROCONVERT(  56; \"EUR\"; \"grd\" )", Value((double)56*340.75/1) );
  CHECK_EVAL( "EUROCONVERT(  57; \"EUR\"; \"iep\" )", Value((double)57*0.787564/1) );
  CHECK_EVAL( "EUROCONVERT(  58; \"EUR\"; \"itl\" )", Value((double)58*1936.27/1) );
  CHECK_EVAL( "EUROCONVERT(  59; \"EUR\"; \"lux\" )", Value((double)59*40.3399/1) );
  CHECK_EVAL( "EUROCONVERT(  60; \"EUR\"; \"nlg\" )", Value((double)60*2.20371/1) );
  CHECK_EVAL( "EUROCONVERT(  61; \"EUR\"; \"pte\" )", Value((double)61*200.482/1) );
  CHECK_EVAL( "EUROCONVERT(  62; \"FIM\"; \"ats\" )", Value((double)62*13.7603/5.94573) );
  CHECK_EVAL( "EUROCONVERT(  63; \"FIM\"; \"bef\" )", Value((double)63*40.3399/5.94573) );
  CHECK_EVAL( "EUROCONVERT(  64; \"FIM\"; \"dem\" )", Value((double)64*1.95583/5.94573) );
  CHECK_EVAL( "EUROCONVERT(  65; \"FIM\"; \"esp\" )", Value((double)65*166.386/5.94573) );
  CHECK_EVAL( "EUROCONVERT(  66; \"FIM\"; \"eur\" )", Value((double)66*1/5.94573) );
  CHECK_EVAL( "EUROCONVERT(  67; \"FIM\"; \"frf\" )", Value((double)67*6.55957/5.94573) );
  CHECK_EVAL( "EUROCONVERT(  68; \"FIM\"; \"grd\" )", Value((double)68*340.75/5.94573) );
  CHECK_EVAL( "EUROCONVERT(  69; \"FIM\"; \"iep\" )", Value((double)69*0.787564/5.94573) );
  CHECK_EVAL( "EUROCONVERT(  70; \"FIM\"; \"itl\" )", Value((double)70*1936.27/5.94573) );
  CHECK_EVAL( "EUROCONVERT(  71; \"FIM\"; \"lux\" )", Value((double)71*40.3399/5.94573) );
  CHECK_EVAL( "EUROCONVERT(  72; \"FIM\"; \"nlg\" )", Value((double)72*2.20371/5.94573) );
  CHECK_EVAL( "EUROCONVERT(  73; \"FIM\"; \"pte\" )", Value((double)73*200.482/5.94573) );
  CHECK_EVAL( "EUROCONVERT(  74; \"FRF\"; \"ats\" )", Value((double)74*13.7603/6.55957) );
  CHECK_EVAL( "EUROCONVERT(  75; \"FRF\"; \"bef\" )", Value((double)75*40.3399/6.55957) );
  CHECK_EVAL( "EUROCONVERT(  76; \"FRF\"; \"dem\" )", Value((double)76*1.95583/6.55957) );
  CHECK_EVAL( "EUROCONVERT(  77; \"FRF\"; \"esp\" )", Value((double)77*166.386/6.55957) );
  CHECK_EVAL( "EUROCONVERT(  78; \"FRF\"; \"eur\" )", Value((double)78*1/6.55957) );
  CHECK_EVAL( "EUROCONVERT(  79; \"FRF\"; \"fim\" )", Value((double)79*5.94573/6.55957) );
  CHECK_EVAL( "EUROCONVERT(  80; \"FRF\"; \"grd\" )", Value((double)80*340.75/6.55957) );
  CHECK_EVAL( "EUROCONVERT(  81; \"FRF\"; \"iep\" )", Value((double)81*0.787564/6.55957) );
  CHECK_EVAL( "EUROCONVERT(  82; \"FRF\"; \"itl\" )", Value((double)82*1936.27/6.55957) );
  CHECK_EVAL( "EUROCONVERT(  83; \"FRF\"; \"lux\" )", Value((double)83*40.3399/6.55957) );
  CHECK_EVAL( "EUROCONVERT(  84; \"FRF\"; \"nlg\" )", Value((double)84*2.20371/6.55957) );
  CHECK_EVAL( "EUROCONVERT(  85; \"FRF\"; \"pte\" )", Value((double)85*200.482/6.55957) );
  CHECK_EVAL( "EUROCONVERT(  86; \"GRD\"; \"ats\" )", Value((double)86*13.7603/340.75) );
  CHECK_EVAL( "EUROCONVERT(  87; \"GRD\"; \"bef\" )", Value((double)87*40.3399/340.75) );
  CHECK_EVAL( "EUROCONVERT(  88; \"GRD\"; \"dem\" )", Value((double)88*1.95583/340.75) );
  CHECK_EVAL( "EUROCONVERT(  89; \"GRD\"; \"esp\" )", Value((double)89*166.386/340.75) );
  CHECK_EVAL( "EUROCONVERT(  90; \"GRD\"; \"eur\" )", Value((double)90*1/340.75) );
  CHECK_EVAL( "EUROCONVERT(  91; \"GRD\"; \"fim\" )", Value((double)91*5.94573/340.75) );
  CHECK_EVAL( "EUROCONVERT(  92; \"GRD\"; \"frf\" )", Value((double)92*6.55957/340.75) );
  CHECK_EVAL( "EUROCONVERT(  93; \"GRD\"; \"iep\" )", Value((double)93*0.787564/340.75) );
  CHECK_EVAL( "EUROCONVERT(  94; \"GRD\"; \"itl\" )", Value((double)94*1936.27/340.75) );
  CHECK_EVAL( "EUROCONVERT(  95; \"GRD\"; \"lux\" )", Value((double)95*40.3399/340.75) );
  CHECK_EVAL( "EUROCONVERT(  96; \"GRD\"; \"nlg\" )", Value((double)96*2.20371/340.75) );
  CHECK_EVAL( "EUROCONVERT(  97; \"GRD\"; \"pte\" )", Value((double)97*200.482/340.75) );
  CHECK_EVAL( "EUROCONVERT(  98; \"IEP\"; \"ats\" )", Value((double)98*13.7603/0.787564) );
  CHECK_EVAL( "EUROCONVERT(  99; \"IEP\"; \"bef\" )", Value((double)99*40.3399/0.787564) );
  CHECK_EVAL( "EUROCONVERT( 100; \"IEP\"; \"dem\" )", Value((double)100*1.95583/0.787564) );
  CHECK_EVAL( "EUROCONVERT( 101; \"IEP\"; \"esp\" )", Value((double)101*166.386/0.787564) );
  CHECK_EVAL( "EUROCONVERT( 102; \"IEP\"; \"eur\" )", Value((double)102*1/0.787564) );
  CHECK_EVAL( "EUROCONVERT( 103; \"IEP\"; \"fim\" )", Value((double)103*5.94573/0.787564) );
  CHECK_EVAL( "EUROCONVERT( 104; \"IEP\"; \"frf\" )", Value((double)104*6.55957/0.787564) );
  CHECK_EVAL( "EUROCONVERT( 105; \"IEP\"; \"grd\" )", Value((double)105*340.75/0.787564) );
  CHECK_EVAL( "EUROCONVERT( 106; \"IEP\"; \"itl\" )", Value((double)106*1936.27/0.787564) );
  CHECK_EVAL( "EUROCONVERT( 107; \"IEP\"; \"lux\" )", Value((double)107*40.3399/0.787564) );
  CHECK_EVAL( "EUROCONVERT( 108; \"IEP\"; \"nlg\" )", Value((double)108*2.20371/0.787564) );
  CHECK_EVAL( "EUROCONVERT( 109; \"IEP\"; \"pte\" )", Value((double)109*200.482/0.787564) );
  CHECK_EVAL( "EUROCONVERT( 110; \"ITL\"; \"ats\" )", Value((double)110*13.7603/1936.27) );
  CHECK_EVAL( "EUROCONVERT( 111; \"ITL\"; \"bef\" )", Value((double)111*40.3399/1936.27) );
  CHECK_EVAL( "EUROCONVERT( 112; \"ITL\"; \"dem\" )", Value((double)112*1.95583/1936.27) );
  CHECK_EVAL( "EUROCONVERT( 113; \"ITL\"; \"esp\" )", Value((double)113*166.386/1936.27) );
  CHECK_EVAL( "EUROCONVERT( 114; \"ITL\"; \"eur\" )", Value((double)114*1/1936.27) );
  CHECK_EVAL( "EUROCONVERT( 115; \"ITL\"; \"fim\" )", Value((double)115*5.94573/1936.27) );
  CHECK_EVAL( "EUROCONVERT( 116; \"ITL\"; \"frf\" )", Value((double)116*6.55957/1936.27) );
  CHECK_EVAL( "EUROCONVERT( 117; \"ITL\"; \"grd\" )", Value((double)117*340.75/1936.27) );
  CHECK_EVAL( "EUROCONVERT( 118; \"ITL\"; \"iep\" )", Value((double)118*0.787564/1936.27) );
  CHECK_EVAL( "EUROCONVERT( 119; \"ITL\"; \"lux\" )", Value((double)119*40.3399/1936.27) );
  CHECK_EVAL( "EUROCONVERT( 120; \"ITL\"; \"nlg\" )", Value((double)120*2.20371/1936.27) );
  CHECK_EVAL( "EUROCONVERT( 121; \"ITL\"; \"pte\" )", Value((double)121*200.482/1936.27) );
  CHECK_EVAL( "EUROCONVERT( 122; \"LUX\"; \"ats\" )", Value((double)122*13.7603/40.3399) );
  CHECK_EVAL( "EUROCONVERT( 123; \"LUX\"; \"bef\" )", Value((double)123*40.3399/40.3399) );
  CHECK_EVAL( "EUROCONVERT( 124; \"LUX\"; \"dem\" )", Value((double)124*1.95583/40.3399) );
  CHECK_EVAL( "EUROCONVERT( 125; \"LUX\"; \"esp\" )", Value((double)125*166.386/40.3399) );
  CHECK_EVAL( "EUROCONVERT( 126; \"LUX\"; \"eur\" )", Value((double)126*1/40.3399) );
  CHECK_EVAL( "EUROCONVERT( 127; \"LUX\"; \"fim\" )", Value((double)127*5.94573/40.3399) );
  CHECK_EVAL( "EUROCONVERT( 128; \"LUX\"; \"frf\" )", Value((double)128*6.55957/40.3399) );
  CHECK_EVAL( "EUROCONVERT( 129; \"LUX\"; \"grd\" )", Value((double)129*340.75/40.3399) );
  CHECK_EVAL( "EUROCONVERT( 130; \"LUX\"; \"iep\" )", Value((double)130*0.787564/40.3399) );
  CHECK_EVAL( "EUROCONVERT( 131; \"LUX\"; \"itl\" )", Value((double)131*1936.27/40.3399) );
  CHECK_EVAL( "EUROCONVERT( 132; \"LUX\"; \"nlg\" )", Value((double)132*2.20371/40.3399) );
  CHECK_EVAL( "EUROCONVERT( 133; \"LUX\"; \"pte\" )", Value((double)133*200.482/40.3399) );
  CHECK_EVAL( "EUROCONVERT( 134; \"NLG\"; \"ats\" )", Value((double)134*13.7603/2.20371) );
  CHECK_EVAL( "EUROCONVERT( 135; \"NLG\"; \"bef\" )", Value((double)135*40.3399/2.20371) );
  CHECK_EVAL( "EUROCONVERT( 136; \"NLG\"; \"dem\" )", Value((double)136*1.95583/2.20371) );
  CHECK_EVAL( "EUROCONVERT( 137; \"NLG\"; \"esp\" )", Value((double)137*166.386/2.20371) );
  CHECK_EVAL( "EUROCONVERT( 138; \"NLG\"; \"eur\" )", Value((double)138*1/2.20371) );
  CHECK_EVAL( "EUROCONVERT( 139; \"NLG\"; \"fim\" )", Value((double)139*5.94573/2.20371) );
  CHECK_EVAL( "EUROCONVERT( 140; \"NLG\"; \"frf\" )", Value((double)140*6.55957/2.20371) );
  CHECK_EVAL( "EUROCONVERT( 141; \"NLG\"; \"grd\" )", Value((double)141*340.75/2.20371) );
  CHECK_EVAL( "EUROCONVERT( 142; \"NLG\"; \"iep\" )", Value((double)142*0.787564/2.20371) );
  CHECK_EVAL( "EUROCONVERT( 143; \"NLG\"; \"itl\" )", Value((double)143*1936.27/2.20371) );
  CHECK_EVAL( "EUROCONVERT( 144; \"NLG\"; \"lux\" )", Value((double)144*40.3399/2.20371) );
  CHECK_EVAL( "EUROCONVERT( 145; \"NLG\"; \"pte\" )", Value((double)145*200.482/2.20371) );
  CHECK_EVAL( "EUROCONVERT( 146; \"PTE\"; \"ats\" )", Value((double)146*13.7603/200.482) );
  CHECK_EVAL( "EUROCONVERT( 147; \"PTE\"; \"bef\" )", Value((double)147*40.3399/200.482) );
  CHECK_EVAL( "EUROCONVERT( 148; \"PTE\"; \"dem\" )", Value((double)148*1.95583/200.482) );
  CHECK_EVAL( "EUROCONVERT( 149; \"PTE\"; \"esp\" )", Value((double)149*166.386/200.482) );
  CHECK_EVAL( "EUROCONVERT( 150; \"PTE\"; \"eur\" )", Value((double)150*1/200.482) );
  CHECK_EVAL( "EUROCONVERT( 151; \"PTE\"; \"fim\" )", Value((double)151*5.94573/200.482) );
  CHECK_EVAL( "EUROCONVERT( 152; \"PTE\"; \"frf\" )", Value((double)152*6.55957/200.482) );
  CHECK_EVAL( "EUROCONVERT( 153; \"PTE\"; \"grd\" )", Value((double)153*340.75/200.482) );
  CHECK_EVAL( "EUROCONVERT( 154; \"PTE\"; \"iep\" )", Value((double)154*0.787564/200.482) );
  CHECK_EVAL( "EUROCONVERT( 155; \"PTE\"; \"itl\" )", Value((double)155*1936.27/200.482) );
  CHECK_EVAL( "EUROCONVERT( 156; \"PTE\"; \"lux\" )", Value((double)156*40.3399/200.482) );
  CHECK_EVAL( "EUROCONVERT( 157; \"PTE\"; \"nlg\" )", Value((double)157*2.20371/200.482) );
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
