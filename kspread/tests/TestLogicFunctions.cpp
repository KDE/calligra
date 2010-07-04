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
#include "TestLogicFunctions.h"

#include "TestKspreadCommon.h"

#include <part/Doc.h>
#include <Map.h>
#include <Sheet.h>

using namespace KSpread;

void TestLogicFunctions::init()
{
    m_doc = new Doc();
    m_doc->map()->addNewSheet();
    m_sheet = m_doc->map()->sheet(0);
}

void TestLogicFunctions::cleanup()
{
    delete m_doc;
}

void TestLogicFunctions::initTestCase()
{
    FunctionModuleRegistry::instance()->loadFunctionModules();
}

// because we may need to promote expected value from integer to float
#define CHECK_EVAL(x,y) { Value z(y); QCOMPARE(evaluate(x,z),(z)); }

Value TestLogicFunctions::evaluate(const QString& formula, Value& ex)
{
    Formula f(m_sheet);
    QString expr = formula;
    if (expr[0] != '=')
        expr.prepend('=');
    f.setExpression(expr);
    Value result = f.eval();

    if (result.isFloat() && ex.isInteger())
        ex = Value(ex.asFloat());
    if (result.isInteger() && ex.isFloat())
        result = Value(result.asFloat());

    return result;
}

void TestLogicFunctions::testAND()
{
    CHECK_EVAL("AND(FALSE();FALSE())", Value(false));
    CHECK_EVAL("AND(FALSE();TRUE())", Value(false));
    CHECK_EVAL("AND(TRUE();FALSE())", Value(false));
    CHECK_EVAL("AND(TRUE();TRUE())", Value(true));
    // errors propogate
    CHECK_EVAL("AND(TRUE();NA())", Value::errorNA());
    CHECK_EVAL("AND(NA();TRUE())", Value::errorNA());
    // Nonzero considered TRUE
    CHECK_EVAL("AND(1;TRUE())", Value(true));
    CHECK_EVAL("AND(2;TRUE())", Value(true));
    // zero considered false
    CHECK_EVAL("AND(0;TRUE())", Value(false));
    // multiple parameters...
    CHECK_EVAL("AND(TRUE();TRUE();TRUE())", Value(true));
    CHECK_EVAL("AND(TRUE();TRUE();FALSE())", Value(false));
    CHECK_EVAL("AND(FALSE();TRUE();TRUE())", Value(false));
    CHECK_EVAL("AND(TRUE();FALSE();TRUE())", Value(false));
    CHECK_EVAL("AND(TRUE();FALSE();FALSE())", Value(false));
    // single parameter
    CHECK_EVAL("AND(TRUE())", Value(true));
    CHECK_EVAL("AND(FALSE())", Value(false));

    // literal non-convertable text should give an error
    //CHECK_EVAL("AND(FALSE();\"a\")", Value::errorVALUE());
}

void TestLogicFunctions::testFALSE()
{
    CHECK_EVAL("FALSE()", Value(false));
    // Applications that implement logical values as 0/1 must map FALSE() to 0
    CHECK_EVAL("IF(ISNUMBER(FALSE());FALSE()=0;FALSE())", Value(false));
    // note that kspread distinguishes between boolean and math
    CHECK_EVAL("FALSE()=0", Value(false));
    CHECK_EVAL("FALSE()=1", Value(false));
    // False converts to 0 in Number context
    CHECK_EVAL("2+FALSE()", Value(2));
}

void TestLogicFunctions::testIF()
{
    CHECK_EVAL("IF(FALSE();7;8)", Value(8));
    CHECK_EVAL("IF(TRUE();7;8)", Value(7));
    CHECK_EVAL("IF(FALSE();7.1;8.2)", Value(8.2));
    CHECK_EVAL("IF(TRUE();7.1;8.2)", Value(7.1));
    CHECK_EVAL("IF(TRUE();\"HI\";8)", Value("HI"));
    CHECK_EVAL("IF(1;7;8)", Value(7));
    CHECK_EVAL("IF(5;7;8)", Value(7));
    CHECK_EVAL("IF(0;7;8)", Value(8));
    // there are a couple of indirect references in the spec test
    // vectors here. Sorry
    CHECK_EVAL("IF(\"x\";7;8)", Value::errorVALUE());
    CHECK_EVAL("IF(\"1\";7;8)", Value::errorVALUE());
    CHECK_EVAL("IF(\"\";7;8)", Value::errorVALUE());
    CHECK_EVAL("IF(FALSE();7)", Value(false));
    CHECK_EVAL("IF(FALSE();7;)", Value(0));
    // Assuming A1 is an empty cell, using it in the following
    // context should be different from passing no argument at all
    CHECK_EVAL("IF(FALSE();7;A1)", Value(Value::Empty));
    CHECK_EVAL("IF(TRUE();4;1/0)", Value(4));
    CHECK_EVAL("IF(FALSE();1/0;5)", Value(5));
}

void TestLogicFunctions::testNOT()
{
    CHECK_EVAL("NOT(FALSE())", Value(true));
    CHECK_EVAL("NOT(TRUE())", Value(false));
    CHECK_EVAL("NOT(1/0)", Value::errorDIV0());
    CHECK_EVAL("NOT(\"a\")", Value::errorVALUE());
}

void TestLogicFunctions::testOR()
{
    CHECK_EVAL("OR(FALSE();FALSE())", Value(false));
    CHECK_EVAL("OR(FALSE();TRUE())", Value(true));
    CHECK_EVAL("OR(TRUE();FALSE())", Value(true));
    CHECK_EVAL("OR(TRUE();TRUE())", Value(true));
    // errors propogate
    CHECK_EVAL("OR(TRUE();NA())", Value::errorNA());
    CHECK_EVAL("OR(NA();TRUE())", Value::errorNA());
    // Nonzero considered TRUE
    CHECK_EVAL("OR(1;TRUE())", Value(true));
    CHECK_EVAL("OR(2;TRUE())", Value(true));
    // zero considered false
    CHECK_EVAL("OR(0;TRUE())", Value(true));
    CHECK_EVAL("OR(0;1)", Value(true));
    CHECK_EVAL("OR(0;0)", Value(false));
    // multiple parameters...
    CHECK_EVAL("OR(TRUE();TRUE();TRUE())", Value(true));
    CHECK_EVAL("OR(FALSE();FALSE();FALSE())", Value(false));
    CHECK_EVAL("OR(TRUE();TRUE();FALSE())", Value(true));
    CHECK_EVAL("OR(FALSE();TRUE();TRUE())", Value(true));
    CHECK_EVAL("OR(TRUE();FALSE();TRUE())", Value(true));
    CHECK_EVAL("OR(TRUE();FALSE();FALSE())", Value(true));
    // single parameter
    CHECK_EVAL("OR(TRUE())", Value(true));
    CHECK_EVAL("OR(FALSE())", Value(false));

    // literal non-convertable text should give an error
    //CHECK_EVAL("OR(TRUE();\"a\")", Value::errorVALUE());
}

void TestLogicFunctions::testTRUE()
{
    CHECK_EVAL("TRUE()", Value(true));
    // Applications that implement logical values as 0/1 must map TRUE() to 1
    CHECK_EVAL("IF(ISNUMBER(TRUE());TRUE()=0;TRUE())", Value(true));
    // note that kspread distinguishes between boolean and math
    CHECK_EVAL("TRUE()=1", Value(false));
    CHECK_EVAL("TRUE()=0", Value(false));
    // False converts to 0 in Number context
    CHECK_EVAL("2+TRUE()", Value(3));
}

void TestLogicFunctions::testXOR()
{
    CHECK_EVAL("XOR(FALSE();FALSE())", Value(false));
    CHECK_EVAL("XOR(FALSE();TRUE())", Value(true));
    CHECK_EVAL("XOR(TRUE();FALSE())", Value(true));
    CHECK_EVAL("XOR(TRUE();TRUE())", Value(false));
    // errors propogate
    CHECK_EVAL("XOR(TRUE();NA())", Value::errorNA());
    CHECK_EVAL("XOR(NA();TRUE())", Value::errorNA());
    CHECK_EVAL("XOR(FALSE();NA())", Value::errorNA());
    CHECK_EVAL("XOR(NA();FALSE())", Value::errorNA());
    // Nonzero considered TRUE
    CHECK_EVAL("XOR(1;TRUE())", Value(false));
    CHECK_EVAL("XOR(3;4)", Value(false));
    CHECK_EVAL("XOR(2;TRUE())", Value(false));
    CHECK_EVAL("XOR(FALSE();1)", Value(true));
    CHECK_EVAL("XOR(2;FALSE())", Value(true));
    // zero considered false
    CHECK_EVAL("XOR(0;TRUE())", Value(true));
    CHECK_EVAL("XOR(0;1)", Value(true));
    CHECK_EVAL("XOR(0;0)", Value(false));
    // multiple parameters...
    CHECK_EVAL("XOR(TRUE();TRUE();TRUE())", Value(false));
    CHECK_EVAL("XOR(FALSE();FALSE();FALSE())", Value(false));
    CHECK_EVAL("XOR(TRUE();TRUE();FALSE())", Value(false));
    CHECK_EVAL("XOR(FALSE();TRUE();TRUE())", Value(false));
    CHECK_EVAL("XOR(TRUE();FALSE();TRUE())", Value(false));
    CHECK_EVAL("XOR(TRUE();FALSE();FALSE())", Value(true));
    CHECK_EVAL("XOR(FALSE();FALSE();TRUE())", Value(true));
    CHECK_EVAL("XOR(FALSE();FALSE();TRUE();FALSE())", Value(true));
    // single parameter
    CHECK_EVAL("XOR(TRUE())", Value(true));
    CHECK_EVAL("XOR(FALSE())", Value(false));

    // literal non-convertable text should give an error
    //CHECK_EVAL("XOR(TRUE();\"a\")", Value::errorVALUE());
}

QTEST_KDEMAIN(TestLogicFunctions, GUI)

#include "TestLogicFunctions.moc"
