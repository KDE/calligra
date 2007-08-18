/* This file is part of the KDE project
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
#define CHECK_EVAL(x,y) { Value z(RoundNumber(y)); QCOMPARE(evaluate(x,z), (z)); }
#define ROUND(x) (roundf(1e15 * x) / 1e15)

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

Value TestStatisticalFunctions::evaluate(const QString& formula, Value& ex)
{
    Formula f(m_doc->map()->sheet(0));
    QString expr = formula;
    if ( expr[0] != '=' )
        expr.prepend( '=' );
    f.setExpression( expr );
    Value result = f.eval();

    if(result.isFloat() && ex.isInteger())
        ex = Value(ex.asFloat());
    if(result.isInteger() && ex.isFloat())
        result = Value(result.asFloat());

#if 0
  // this magically generates the CHECKs
  printf("  CHECK_EVAL( \"%s\",  %.14e) );\n", qPrintable(formula), result.asFloat());
#endif

    return RoundNumber(result);
}

void TestStatisticalFunctions::initTestCase()
{
    m_doc = new Doc();
    m_doc->map()->addNewSheet();
    Sheet* sheet = m_doc->map()->sheet(0);
    CellStorage* storage = sheet->cellStorage();

    // B3:B7
    storage->setValue(2, 3, Value("7"));
    storage->setValue(2, 4, Value(2));
    storage->setValue(2, 5, Value(3));
    storage->setValue(2, 6, Value(true));
    storage->setValue(2, 7, Value("Hello"));
    // B9
    storage->setValue(2, 9, Value::errorDIV0());
}

void TestStatisticalFunctions::testAVERAGEA()
{
    CHECK_EVAL("AVERAGE(2; 4)", Value(3))
    CHECK_EVAL("AVERAGE(TRUE(); FALSE(); 5)", Value(2));
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
