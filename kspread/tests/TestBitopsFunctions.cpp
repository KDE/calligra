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
#include "TestBitopsFunctions.h"

#include "TestKspreadCommon.h"

#include <KStandardDirs>

void TestBitopsFunctions::initTestCase()
{
    FunctionModuleRegistry::instance()->loadFunctionModules();
}

// because we may need to promote expected value from integer to float
#define CHECK_EVAL(x,y) { Value z(y); QCOMPARE(evaluate(x,z),(z)); }

Value TestBitopsFunctions::evaluate(const QString& formula, Value& ex)
{
    Formula f;
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

void TestBitopsFunctions::testBITAND()
{
    // basic check of all four bit combinations
    CHECK_EVAL("BITAND(12;10)", Value(8));
    // test using an all-zero combo
    CHECK_EVAL("BITAND(7;0)", Value(0));
    // test of 31-bit value
    CHECK_EVAL("BITAND(2147483641; 2147483637)", Value(2147483633));
    // test of 32-bit value
    CHECK_EVAL("BITAND(4294967289.0; 4294967285.0)", Value(4294967281LL));
    // test of 32-bit value
    CHECK_EVAL("BITAND(4294967289; 4294967285)", Value(4294967281LL));
    // test of 48 bit value
    CHECK_EVAL("BITAND(281474976710649 ; 281474976710645)",  Value(281474976710641LL));
    // test of 48 bit value
    CHECK_EVAL("BITAND(281474976710655; 281474976710655)",  Value(281474976710655LL));
    // test of 48 bit value
    CHECK_EVAL("BITAND(281474976710655; 281474976710655)<>281474976710656", Value(true));
}


void TestBitopsFunctions::testBITOR()
{
    // basic check of all four bit combinations
    CHECK_EVAL("BITOR(12;10)", Value(14));
    // test using an all-zero combo
    CHECK_EVAL("BITOR(7;0)", Value(7));
    // test of 31-bit value
    CHECK_EVAL("BITOR(2147483641; 2147483637)", Value(2147483645));
    // test of 32-bit value
    CHECK_EVAL("BITOR(4294967289.0; 4294967285.0)", Value(4294967293LL));
    // test of 32-bit value
    CHECK_EVAL("BITOR(4294967289; 4294967285)", Value(4294967293LL));
    // test of 48 bit value
    CHECK_EVAL("BITOR(281474976710649; 281474976710645)",  Value(281474976710653LL));
    // test of 48 bit value
    CHECK_EVAL("BITOR(281474976710655; 281474976710655)",  Value(281474976710655LL));
    // test of 48 bit value
    CHECK_EVAL("BITOR(281474976710655; 281474976710655)<>281474976710656", Value(true));
}

void TestBitopsFunctions::testBITXOR()
{
    // basic check of all four bit combinations
    CHECK_EVAL("BITXOR(12;10)", Value(6));
    // test using an all-zero combo
    CHECK_EVAL("BITXOR(7;0)", Value(7));
    // test of 31-bit value
    CHECK_EVAL("BITXOR(2147483641; 2)", Value(2147483643));
    // test of 32-bit value
    CHECK_EVAL("BITXOR(4294967289.0; 2.0)", Value(4294967291LL));
    // test of 32-bit value
    CHECK_EVAL("BITXOR(4294967289; 2)", Value(4294967291LL));
    // test of 48 bit value
    CHECK_EVAL("BITXOR(281474976710649 ; 2)",  Value(281474976710651LL));
    // test of 48 bit value
    CHECK_EVAL("BITXOR(281474976710655; 0)",  Value(281474976710655LL));
    // test of 48 bit value
    CHECK_EVAL("BITXOR(281474976710655; 0)<>281474976710656", Value(true));
}

void TestBitopsFunctions::testBITLSHIFT()
{
    CHECK_EVAL("BITLSHIFT(63;2)", Value(252));
    CHECK_EVAL("BITLSHIFT(63;0)", Value(63));
    CHECK_EVAL("BITLSHIFT(63;-2)", Value(15));
    CHECK_EVAL("BITLSHIFT(1;47)", Value(140737488355328LL));
    // test for 31 bits
    CHECK_EVAL("BITLSHIFT(2147483641; 0)", Value(2147483641LL));
    // test for 32 bits
    CHECK_EVAL("BITLSHIFT(4294967289; 0)", Value(4294967289LL));
    // test for 48 bits
    CHECK_EVAL("BITLSHIFT(281474976710649; 0)", Value(281474976710649LL));
}

void TestBitopsFunctions::testBITRSHIFT()
{
    CHECK_EVAL("BITRSHIFT(63;2)", Value(15));
    CHECK_EVAL("BITRSHIFT(63;0)", Value(63));
    CHECK_EVAL("BITRSHIFT(63;-2)", Value(252));
    CHECK_EVAL("BITRSHIFT(63;48)", Value(0));
    // test for 31 bits
    CHECK_EVAL("BITRSHIFT(2147483641; 0)", Value(2147483641LL));
    // test for 32 bits
    CHECK_EVAL("BITRSHIFT(4294967289; 0)", Value(4294967289LL));
    // test for 48 bits
    CHECK_EVAL("BITRSHIFT(281474976710649 ; 0)", Value(281474976710649LL));
}

QTEST_KDEMAIN(TestBitopsFunctions, GUI)

#include "TestBitopsFunctions.moc"
