/* This file is part of the KDE project
   Copyright 2007 Brad Hards <bradh@frogmouth.net>

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

#include "TestInformationFunctions.h"

// because we may need to promote expected value from integer to float
#define CHECK_EVAL(x,y) { Value z(y); QCOMPARE(evaluate(x,z),(z)); }

Value TestInformationFunctions::evaluate(const QString& formula, Value& ex)
{
  Formula f;
  QString expr = formula;
  if ( expr[0] != '=' )
    expr.prepend( '=' );
  f.setExpression( expr );
  Value result = f.eval();

  if(result.isFloat() && ex.isInteger())
    ex = Value(ex.asFloat());
  if(result.isInteger() && ex.isFloat())
    result = Value(result.asFloat());

  return result;
}


void TestInformationFunctions::testVALUE()
{
    CHECK_EVAL( "VALUE(\"6\")", Value( 6 ) );
    CHECK_EVAL( "VALUE(\"1E5\")", Value( 100000 ) );
    CHECK_EVAL( "VALUE(\"200%\")",  Value( 2 ) );
    CHECK_EVAL( "VALUE(\"1.5\")", Value( 1.5 ) );
    // Check fractions
    CHECK_EVAL( "VALUE(\"7 1/4\")", Value( 7.25 ) );
    CHECK_EVAL( "VALUE(\"0 1/2\")", Value( 0.5 ) );
    // Check times
    CHECK_EVAL( "VALUE(\"00:00\")", Value( 0 ) );
    CHECK_EVAL( "VALUE(\"02:00\")-2/24", Value( 0 ) );
    // check dates - local dependent
    CHECK_EVAL( "VALUE(\"5/21/06\")=DATE(2006;5;21)", Value( true ) );
    CHECK_EVAL( "VALUE(\"1/2/2005\")=DATE(2005;1;2)", Value( true ) );
}


QTEST_KDEMAIN(TestInformationFunctions, GUI)

#include "TestInformationFunctions.moc"
