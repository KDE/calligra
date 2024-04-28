// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.0-only

#include "TestBitopsFunctions.h"

#include "TestKspreadCommon.h"

#include <QTest>

void TestBitopsFunctions::initTestCase()
{
    KLocalizedString::setApplicationDomain("calligrasheets");
    FunctionModuleRegistry::instance()->loadFunctionModules();
}

// because we may need to promote expected value from integer to float
#define CHECK_EVAL(x, y)                                                                                                                                       \
    {                                                                                                                                                          \
        Value z(y);                                                                                                                                            \
        QCOMPARE(evaluate(x, z), (z));                                                                                                                         \
    }

Value TestBitopsFunctions::evaluate(const QString &formula, Value &ex)
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

    // Note: We cast to int64_t because some compilers (msvc) may define large integer constants as unsigned,
    //       but Value has no ctor for unsigned integers.

    // test of 31-bit value
    CHECK_EVAL("BITAND(2147483641; 2147483637)", Value(static_cast<int64_t>(2147483633)));
    // test of 32-bit value
    CHECK_EVAL("BITAND(4294967289.0; 4294967285.0)", Value(static_cast<int64_t>(4294967281)));
    // test of 32-bit value
    CHECK_EVAL("BITAND(4294967289; 4294967285)", Value(static_cast<int64_t>(4294967281)));
    // test of 48 bit value
    CHECK_EVAL("BITAND(281474976710649 ; 281474976710645)", Value(static_cast<int64_t>(281474976710641)));
    // test of 48 bit value
    CHECK_EVAL("BITAND(281474976710655; 281474976710655)", Value(static_cast<int64_t>(281474976710655)));
    // test of 48 bit value
    CHECK_EVAL("BITAND(281474976710655; 281474976710655)<>281474976710656", Value(true));
}

void TestBitopsFunctions::testBITOR()
{
    // basic check of all four bit combinations
    CHECK_EVAL("BITOR(12;10)", Value(14));
    // test using an all-zero combo
    CHECK_EVAL("BITOR(7;0)", Value(7));

    // Note: We cast to int64_t because some compilers (msvc) may define large integer constants as unsigned,
    //       but Value has no ctor for unsigned integers.

    // test of 31-bit value
    CHECK_EVAL("BITOR(2147483641; 2147483637)", Value(static_cast<int64_t>(2147483645)));
    // test of 32-bit value
    CHECK_EVAL("BITOR(4294967289.0; 4294967285.0)", Value(static_cast<int64_t>(4294967293)));
    // test of 32-bit value
    CHECK_EVAL("BITOR(4294967289; 4294967285)", Value(static_cast<int64_t>(4294967293)));
    // test of 48 bit value
    CHECK_EVAL("BITOR(281474976710649; 281474976710645)", Value(static_cast<int64_t>(281474976710653)));
    // test of 48 bit value
    CHECK_EVAL("BITOR(281474976710655; 281474976710655)", Value(static_cast<int64_t>(281474976710655)));
    // test of 48 bit value
    CHECK_EVAL("BITOR(281474976710655; 281474976710655)<>281474976710656", Value(true));
}

void TestBitopsFunctions::testBITXOR()
{
    // basic check of all four bit combinations
    CHECK_EVAL("BITXOR(12;10)", Value(6));
    // test using an all-zero combo
    CHECK_EVAL("BITXOR(7;0)", Value(7));

    // Note: We cast to int64_t because some compilers (msvc) may define large integer constants as unsigned,
    //       but Value has no ctor for unsigned integers.

    // test of 31-bit value
    CHECK_EVAL("BITXOR(2147483641; 2)", Value(static_cast<int64_t>(2147483643)));
    // test of 32-bit value
    CHECK_EVAL("BITXOR(4294967289.0; 2.0)", Value(static_cast<int64_t>(4294967291)));
    // test of 32-bit value
    CHECK_EVAL("BITXOR(4294967289; 2)", Value(static_cast<int64_t>(4294967291)));
    // test of 48 bit value
    CHECK_EVAL("BITXOR(281474976710649 ; 2)", Value(static_cast<int64_t>(281474976710651)));
    // test of 48 bit value
    CHECK_EVAL("BITXOR(281474976710655; 0)", Value(static_cast<int64_t>(281474976710655)));
    // test of 48 bit value
    CHECK_EVAL("BITXOR(281474976710655; 0)<>281474976710656", Value(true));
}

void TestBitopsFunctions::testBITLSHIFT()
{
    CHECK_EVAL("BITLSHIFT(63;2)", Value(252));
    CHECK_EVAL("BITLSHIFT(63;0)", Value(63));
    CHECK_EVAL("BITLSHIFT(63;-2)", Value(15));
    CHECK_EVAL("BITLSHIFT(1;47)", Value(static_cast<int64_t>(140737488355328)));

    // Note: We cast to int64_t because some compilers (msvc) may define large integer constants as unsigned,
    //       but Value has no ctor for unsigned integers.

    // test for 31 bits
    CHECK_EVAL("BITLSHIFT(2147483641; 0)", Value(static_cast<int64_t>(2147483641)));
    // test for 32 bits
    CHECK_EVAL("BITLSHIFT(4294967289; 0)", Value(static_cast<int64_t>(4294967289)));
    // test for 48 bits
    CHECK_EVAL("BITLSHIFT(281474976710649; 0)", Value(static_cast<int64_t>(281474976710649)));
}

void TestBitopsFunctions::testBITRSHIFT()
{
    CHECK_EVAL("BITRSHIFT(63;2)", Value(15));
    CHECK_EVAL("BITRSHIFT(63;0)", Value(63));
    CHECK_EVAL("BITRSHIFT(63;-2)", Value(252));
    CHECK_EVAL("BITRSHIFT(63;48)", Value(0));

    // Note: We cast to int64_t because some compilers (msvc) may define large integer constants as unsigned,
    //       but Value has no ctor for unsigned integers.

    // test for 31 bits
    CHECK_EVAL("BITRSHIFT(2147483641; 0)", Value(static_cast<int64_t>(2147483641)));
    // test for 32 bits
    CHECK_EVAL("BITRSHIFT(4294967289; 0)", Value(static_cast<int64_t>(4294967289)));
    // test for 48 bits
    CHECK_EVAL("BITRSHIFT(281474976710649 ; 0)", Value(static_cast<int64_t>(281474976710649)));
}

QTEST_MAIN(TestBitopsFunctions)
