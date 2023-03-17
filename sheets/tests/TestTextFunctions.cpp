// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.0-only

#include "TestTextFunctions.h"

#include "TestKspreadCommon.h"
#include "engine/MapBase.h"
#include "engine/SheetBase.h"
#include <engine/CalculationSettings.h>
#include "engine/Localization.h"

void TestTextFunctions::initTestCase()
{
    KLocalizedString::setApplicationDomain("calligrasheets");
    FunctionModuleRegistry::instance()->loadFunctionModules();

    m_map = new MapBase;
    m_map->addNewSheet();
    SheetBase* sheet = m_map->sheet(0);
    sheet->setSheetName("Sheet1");

    // FIXME
    const_cast<Localization*>(m_map->calculationSettings()->locale())->setLanguage("en_US");  // needed for decimals
}

#define CHECK_EVAL(x,y) { Value z(y); QCOMPARE(evaluate(x,z),(z)); }
// CHECK_EVAL has the side effect that the value 'y' is modified, e.g due to localization
// Use CHECK_VALUE2 where this this is a problem
#define CHECK_EVAL2(x,y,r) { Value z(y); QCOMPARE(evaluate(x,z),r); }

Value TestTextFunctions::evaluate(const QString& formula, Value& ex)
{
    Formula f(m_map->sheet(0));
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

void TestTextFunctions::testASC()
{
    // TODO reactivate after function is implemented
//     CHECK_EVAL( "ASC(\"ＡＢＣ\")", Value( "ABC" ) );
//     CHECK_EVAL( "ASC(\"アイウ\")", Value( "ｧｨｩ" ) );
}

void TestTextFunctions::testCHAR()
{
    CHECK_EVAL("CHAR(65)", Value("A"));
    CHECK_EVAL("CHAR(60)", Value("<"));
    CHECK_EVAL("CHAR(97)", Value("a"));
    CHECK_EVAL("CHAR(126)", Value("~"));
    CHECK_EVAL("CHAR(32)", Value(" "));

    // newline
    CHECK_EVAL("LEN(CHAR(10))", Value(1));
    // number has to be >=0
    CHECK_EVAL("CHAR(-1)", Value::errorNUM());
}

void TestTextFunctions::testCLEAN()
{
    CHECK_EVAL("CLEAN(\"Text\")", Value("Text"));
    CHECK_EVAL("CLEAN(CHAR(7)&\"Tex\"&CHAR(8)&\"t\"&CHAR(9))", Value("Text"));
    CHECK_EVAL("CLEAN(\"Hi there\")", Value("Hi there"));
}

void TestTextFunctions::testCODE()
{
    CHECK_EVAL("CODE(\"A\")", Value(65));
    CHECK_EVAL("CODE(\"0\")>0", Value(true));
    CHECK_EVAL("CODE(\"Text\")=CODE(\"T\")", Value(true));
}

void TestTextFunctions::testCONCATENATE()
{
    CHECK_EVAL("CONCATENATE(\"Hi \"; \"there\")", Value("Hi there"));
    CHECK_EVAL("CONCATENATE(\"A\"; \"B\"; \"C\")", Value("ABC"));
    CHECK_EVAL("CONCATENATE(2;3)", Value("23"));
    CHECK_EVAL("CONCATENATE(23)", Value("23"));
}

void TestTextFunctions::testEXACT()
{
    CHECK_EVAL("EXACT(\"A\";\"A\")",  Value(true));
    CHECK_EVAL("EXACT(\"A\";\"a\")",  Value(false));
    CHECK_EVAL("EXACT(1;1)",  Value(true));
    CHECK_EVAL("EXACT((1/3)*3;1)",  Value(true));
    CHECK_EVAL("EXACT(TRUE();TRUE())",  Value(true));
    CHECK_EVAL("EXACT(\"1\";2)",  Value(false));
    CHECK_EVAL("EXACT(\"h\";1)",  Value(false));
    CHECK_EVAL("EXACT(\"1\";1)",  Value(true));
    CHECK_EVAL("EXACT(\" 1\";1)",  Value(false));
}

void TestTextFunctions::testFIND()
{
    CHECK_EVAL("FIND(\"b\";\"abcabc\")", Value(2));
    CHECK_EVAL("FIND(\"b\";\"abcabcabc\"; 3)", Value(5));
    CHECK_EVAL("FIND(\"b\";\"ABC\";1)", Value::errorVALUE());
    CHECK_EVAL("FIND(\"b\";\"bbbb\")", Value(1));
    CHECK_EVAL("FIND(\"b\";\"bbbb\";2)", Value(2));
    CHECK_EVAL("FIND(\"b\";\"bbbb\";2.9)", Value(2));
    CHECK_EVAL("FIND(\"b\";\"bbbb\";0)", Value::errorVALUE());
    CHECK_EVAL("FIND(\"b\";\"bbbb\";0.9)", Value::errorVALUE());
}

void TestTextFunctions::testFIXED()
{
    // localize the expected value
    const Localization *loc = m_map->calculationSettings()->locale();
    Value expected("12" + loc->thousandsSeparator() + "345" + loc->decimalSymbol() + "000");

    CHECK_EVAL2("FIXED(12345;3)", Value("12,345.000"), expected);
    CHECK_EVAL("ISTEXT(FIXED(12345;3))", Value(true));
    CHECK_EVAL2("FIXED(12345;3;FALSE())", Value("12,345.000"), expected);
    CHECK_EVAL2("FIXED(12345;3.95;FALSE())", Value("12,345.000"), expected);

    expected = Value("12345" + loc->decimalSymbol() + "0000");
    CHECK_EVAL2("FIXED(12345;4;TRUE())", Value("12345.0000"), expected);
    expected = Value("123" + loc->decimalSymbol() + "5");
    CHECK_EVAL2("FIXED(123.45;1)", Value("123.5"), expected);
    CHECK_EVAL("FIXED(125.45; -1)", Value("130"));
    CHECK_EVAL("FIXED(125.45; -1.1)", Value("130"));
    CHECK_EVAL("FIXED(125.45; -1.9)", Value("130"));
    CHECK_EVAL("FIXED(125.45; -2)", Value("100"));
    CHECK_EVAL("FIXED(125.45; -2.87)", Value("100"));
    CHECK_EVAL("FIXED(125.45; -3)", Value("0"));
    CHECK_EVAL("FIXED(125.45; -4)", Value("0"));
    CHECK_EVAL("FIXED(125.45; -5)", Value("0"));
}

void TestTextFunctions::testJIS()
{
    // TODO reactivate after function is implemented
//     CHECK_EVAL( "JIS(\"ABC\")", Value( "ＡＢＣ") );
//     CHECK_EVAL( "JIS(\"ｧｨｩ\")", Value( "アイウ" ) );
}

void TestTextFunctions::testLEFT()
{
    CHECK_EVAL("LEFT(\"Hello\";2)", Value("He"));
    CHECK_EVAL("LEFT(\"Hello\")", Value("H"));
    CHECK_EVAL("LEFT(\"Hello\";20)", Value("Hello"));
    CHECK_EVAL("LEFT(\"Hello\";0)", Value(""));
    CHECK_EVAL("LEFT(\"\";4)", Value(""));
    CHECK_EVAL("LEFT(\"xxx\";-0.1)", Value::errorVALUE());
    CHECK_EVAL("LEFT(\"Hello\";2^15-1)", Value("Hello"));
    CHECK_EVAL("LEFT(\"Hello\";2.9)", Value("He"));
}

void TestTextFunctions::testLEN()
{
    CHECK_EVAL("LEN(\"Hi there\")", Value(8));
    CHECK_EVAL("LEN(\"\")", Value(0));
    CHECK_EVAL("LEN(55)", Value(2));
}

void TestTextFunctions::testLOWER()
{
    CHECK_EVAL("LOWER(\"HELLObc7\")", Value("hellobc7"));
}

void TestTextFunctions::testMID()
{
    CHECK_EVAL("MID(\"123456789\";5;3)", Value("567"));
    CHECK_EVAL("MID(\"123456789\";20;3)", Value(""));
    CHECK_EVAL("MID(\"123456789\";-1;0)", Value::errorVALUE());
    CHECK_EVAL("MID(\"123456789\";1;0)", Value(""));
    CHECK_EVAL("MID(\"123456789\";2.9;1)", Value("2"));
    CHECK_EVAL("MID(\"123456789\";2;2.9)", Value("23"));
    CHECK_EVAL("MID(\"123456789\";5)", Value("56789"));
}

void TestTextFunctions::testNUMBERVALUE()
{
    // TODO this is not actually supported at this time
    CHECK_EVAL( "NUMBERVALUE(\"6\"; \".\")", Value( 6 ) ); // VALUE converts text to numbers (unlike N).
    CHECK_EVAL( "NUMBERVALUE(\"300.5\"; \".\")", Value( 300.5 ) ); // Period works.
/*
    CHECK_EVAL( "NUMBERVALUE(\"300,5\"; \",\")", Value( 300.5 ) ); // Comma works
    CHECK_EVAL( "NUMBERVALUE(\"6,000.5\"; \".\")", Value( 6000.5 ) ); // Period + thousands work.
    CHECK_EVAL( "NUMBERVALUE(\"6.000,5\"; \",\")", Value( 6000.5 ) ); // Comma + thousands work.
    CHECK_EVAL( "NUMBERVALUE(\"3!456!000*567\"; \"*\"; \"!\")", Value( 3456000.567 ) ); // Thousands separator works
    CHECK_EVAL( "NUMBERVALUE(\"+6,000.5\"; \".\")", Value( 6000.5 ) ); // Positive sign
    CHECK_EVAL( "NUMBERVALUE(\"-6,000.5\"; \".\")", Value( -6000.5 ) ); // Negative sign
*/
}

void TestTextFunctions::testPROPER()
{
    CHECK_EVAL("PROPER(\"hello there\")", Value("Hello There"));
    CHECK_EVAL("PROPER(\"HELLO THERE\")", Value("Hello There"));
    CHECK_EVAL("PROPER(\"HELLO.THERE\")", Value("Hello.There"));
}

void TestTextFunctions::testREPLACE()
{
    CHECK_EVAL("REPLACE(\"123456789\";5;3;\"Q\")", Value("1234Q89"));
    CHECK_EVAL("REPLACE(\"123456789\";5;0;\"Q\")", Value("1234Q56789"));
}

void TestTextFunctions::testREPT()
{
    CHECK_EVAL("REPT(\"X\";3)",  Value("XXX"));
    CHECK_EVAL("REPT(\"XY\";2)",  Value("XYXY"));
    CHECK_EVAL("REPT(\"X\";2.9)",  Value("XX"));
    CHECK_EVAL("REPT(\"XY\";2.9)",  Value("XYXY"));
    CHECK_EVAL("REPT(\"X\";0)",  Value(""));
    CHECK_EVAL("REPT(\"XYZ\";0)",  Value(""));
    CHECK_EVAL("REPT(\"X\";-1)",  Value::errorVALUE());
    CHECK_EVAL("REPT(\"XYZ\";-0.1)",  Value::errorVALUE());
}

void TestTextFunctions::testRIGHT()
{
    CHECK_EVAL("RIGHT(\"Hello\";2)", Value("lo"));
    CHECK_EVAL("RIGHT(\"Hello\")", Value("o"));
    CHECK_EVAL("RIGHT(\"Hello\";20)", Value("Hello"));
    CHECK_EVAL("RIGHT(\"Hello\";0)", Value(""));
    CHECK_EVAL("RIGHT(\"\";4)", Value(""));
    CHECK_EVAL("RIGHT(\"xxx\";-1)", Value::errorVALUE());
    CHECK_EVAL("RIGHT(\"xxx\";-0.1)", Value::errorVALUE());
    CHECK_EVAL("RIGHT(\"Hello\";2^15-1)", Value("Hello"));
    CHECK_EVAL("RIGHT(\"Hello\";2.9)", Value("lo"));
}

void TestTextFunctions::testSEARCH()
{
    CHECK_EVAL("=SEARCH(\"b\";\"abcabc\")", Value(2));
    CHECK_EVAL("=SEARCH(\"b\";\"abcabcabc\"; 3)", Value(5));
    CHECK_EVAL("=SEARCH(\"b\";\"ABC\";1)", Value(2));
    CHECK_EVAL("=SEARCH(\"c?a\";\"abcabcda\")", Value(6));
    CHECK_EVAL("=SEARCH(\"e*o\";\"yes and no\")", Value(2));
    CHECK_EVAL("=SEARCH(\"b*c\";\"abcabcabc\")", Value(2));
}

void TestTextFunctions::testSUBSTITUTE()
{
    CHECK_EVAL("SUBSTITUTE(\"121212\";\"2\";\"ab\")", Value("1ab1ab1ab"));
    CHECK_EVAL("SUBSTITUTE(\"121212\";\"2\";\"ab\";2)", Value("121ab12"));
    CHECK_EVAL("SUBSTITUTE(\"Hello\";\"x\";\"ab\")", Value("Hello"));
    CHECK_EVAL("SUBSTITUTE(\"xyz\";\"\";\"ab\")", Value("xyz"));
    CHECK_EVAL("SUBSTITUTE(\"\";\"\";\"ab\")", Value(""));
    CHECK_EVAL("SUBSTITUTE(\"Hello\"; \"H\"; \"J\"; 0)", Value::errorVALUE());
    CHECK_EVAL("SUBSTITUTE(\"Hello\"; \"H\"; \"J\"; 1)", Value("Jello"));
    CHECK_EVAL("SUBSTITUTE(\"fo\"\"o\";\"o\";\"a\")", Value("fa\"a"));
}

void TestTextFunctions::testT()
{
    CHECK_EVAL("T(\"Hi\")", Value("Hi"));
    CHECK_EVAL("T(5)",      Value(""));
}

void TestTextFunctions::testTRIM()
{
    CHECK_EVAL("TRIM(\" Hi \")", Value("Hi"));
    CHECK_EVAL("LEN(TRIM(\"H\" & \" \" & \" \" & \"I\"))", Value(3));
}

void TestTextFunctions::testUNICHAR()
{
    CHECK_EVAL("UNICHAR(65)", Value("A"));
    CHECK_EVAL("UNICHAR(8364)", Value(QChar(8364)));
}

void TestTextFunctions::testUNICODE()
{
    QChar euro(8364);

    CHECK_EVAL("UNICODE(\"A\")", Value(65));
    CHECK_EVAL("UNICODE(\"AB€C\")", Value(65));
    CHECK_EVAL(QString("UNICODE(\"%1\")").arg(euro), Value(8364));
    CHECK_EVAL(QString("UNICODE(\"%1F\")").arg(euro), Value(8364));
}

void TestTextFunctions::testUPPER()
{
    CHECK_EVAL("UPPER(\"Habc7\")", Value("HABC7"));
}

void TestTextFunctions::testROT13()
{
    CHECK_EVAL("ROT13(\"KSpread\")", Value("XFcernq"));
    CHECK_EVAL("ROT13(\"XFcernq\")", Value("KSpread"));
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.DATEFUNCTIONS.GETROT13(\"KSpread\")", Value("XFcernq"));
    CHECK_EVAL("COM.SUN.STAR.SHEET.ADDIN.DATEFUNCTIONS.GETROT13(\"XFcernq\")", Value("KSpread"));
}

void TestTextFunctions::testBAHTTEXT()
{
    Value r;
    r = evaluate("BAHTTEXT(23)", r);
    CHECK_EVAL("BAHTTEXT(23)", r);
    CHECK_EVAL("COM.MICROSOFT.BAHTTEXT(23)", r);
}

void TestTextFunctions::testTEXT()
{
    CHECK_EVAL("TEXT(71)", Value("71"));
    // this is not currently supported
//    CHECK_EVAL("TEXT(TIME(13;10;43);\"hh:mm\")", Value("13:10"));
}

void TestTextFunctions::cleanupTestCase()
{
    delete m_map;
}


QTEST_MAIN(TestTextFunctions)
