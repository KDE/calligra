/* This file is part of the KDE project
   Copyright 2004,2007 Ariya Hidayat <ariya@kde.org>

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
#include "TestFormula.h"

#include <klocale.h>

#include "TestKspreadCommon.h"

using namespace KSpread;

static char encodeTokenType(const Token& token)
{
    char result = '?';
    switch (token.type()) {
    case Token::Boolean:    result = 'b'; break;
    case Token::Integer:    result = 'i'; break;
    case Token::Float:      result = 'f'; break;
    case Token::Operator:   result = 'o'; break;
    case Token::Cell:       result = 'c'; break;
    case Token::Range:      result = 'r'; break;
    case Token::Identifier: result = 'x'; break;
    default: break;
    }
    return result;
}

#if 0 // not used?
static QString describeTokenCodes(const QString& tokenCodes)
{
    QString result;

    if (tokenCodes.isEmpty())
        result = "(invalid)";
    else
        for (int i = 0; i < tokenCodes.length(); i++) {
            switch (tokenCodes[i].unicode()) {
            case 'b': result.append("Boolean"); break;
            case 'i': result.append("integer"); break;
            case 'f': result.append("float"); break;
            case 'o': result.append("operator"); break;
            case 'c': result.append("cell"); break;
            case 'r': result.append("range"); break;
            case 'x': result.append("identifier"); break;
            default:  result.append("unknown"); break;
            }
            if (i < tokenCodes.length() - 1) result.append(", ");
        }

    return result.prepend("{").append("}");
}
#endif

#define CHECK_TOKENIZE(x,y) QCOMPARE(tokenizeFormula(x), QString(y))

static QString tokenizeFormula(const QString& formula)
{
    Formula f;
    QString expr = formula;
    expr.prepend('=');
    f.setExpression(expr);
    Tokens tokens = f.tokens();

    QString resultCodes;
    if (tokens.valid())
        for (int i = 0; i < tokens.count(); i++)
            resultCodes.append(encodeTokenType(tokens[i]));

    return resultCodes;
}


// because we may need to promote expected value from integer to float
#define CHECK_EVAL(x,y) { Value z(y); QCOMPARE(evaluate(x,z),(z)); }

Value TestFormula::evaluate(const QString& formula, Value& ex)
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

void TestFormula::initTestCase()
{
    FunctionModuleRegistry::instance()->loadFunctionModules();
}

void TestFormula::testTokenizer()
{
    // simple, single-token formulas
    CHECK_TOKENIZE("True", "x");
    CHECK_TOKENIZE("False", "x");
    CHECK_TOKENIZE("36", "i");
    CHECK_TOKENIZE("0", "i");
    CHECK_TOKENIZE("3.14159", "f");
    CHECK_TOKENIZE(".25", "f");
    CHECK_TOKENIZE("1e-9", "f");
    CHECK_TOKENIZE("2e3", "f");
    CHECK_TOKENIZE(".3333e0", "f");

    // cell/range/identifier
    CHECK_TOKENIZE("A1", "c");
    CHECK_TOKENIZE("Sheet1!A1", "c");
    CHECK_TOKENIZE("'Sheet1'!A1", "c");
    CHECK_TOKENIZE("'Sheet One'!A1", "c");
    CHECK_TOKENIZE("2006!A1", "c");
    CHECK_TOKENIZE("2006bak!A1", "c");
    CHECK_TOKENIZE("2006bak2!A1", "c");
    CHECK_TOKENIZE("'2006bak2'!A1", "c");
    CHECK_TOKENIZE("A1:B100", "r");
    CHECK_TOKENIZE("Sheet1!A1:B100", "r");
    CHECK_TOKENIZE("'Sheet One'!A1:B100", "r");
    CHECK_TOKENIZE("SIN", "x");

    // log2 and log10 are cell references and function identifiers
    CHECK_TOKENIZE("LOG2", "c");
    CHECK_TOKENIZE("LOG10:11", "r");
    CHECK_TOKENIZE("LOG2(2)", "xoio");
    CHECK_TOKENIZE("LOG10(10)", "xoio");

    // operators
    CHECK_TOKENIZE("+", "o");
    CHECK_TOKENIZE("-", "o");
    CHECK_TOKENIZE("*", "o");
    CHECK_TOKENIZE("/", "o");
    CHECK_TOKENIZE("+", "o");
    CHECK_TOKENIZE("^", "o");
    CHECK_TOKENIZE("(", "o");
    CHECK_TOKENIZE(")", "o");
    CHECK_TOKENIZE(",", "o");
    CHECK_TOKENIZE(";", "o");
    CHECK_TOKENIZE("=", "o");
    CHECK_TOKENIZE("<", "o");
    CHECK_TOKENIZE(">", "o");
    CHECK_TOKENIZE("<=", "o");
    CHECK_TOKENIZE(">=", "o");
    CHECK_TOKENIZE("%", "o");

    // commonly used formulas
    CHECK_TOKENIZE("A1+A2", "coc");
    CHECK_TOKENIZE("2.5*B1", "foc");
    CHECK_TOKENIZE("SUM(A1:Z10)", "xoro");
    CHECK_TOKENIZE("MAX(Sheet1!Sales)", "xoro");
    CHECK_TOKENIZE("-ABS(A1)", "oxoco");

    // should be correctly parsed though they are nonsense (can't be evaluated)
    CHECK_TOKENIZE("0E0.5", "ff");
    CHECK_TOKENIZE("B3 D4:D5 Sheet1!K1", "crc");
    CHECK_TOKENIZE("SIN A1", "xc");
    CHECK_TOKENIZE("SIN A1:A20", "xr");

    // invalid formulas, can't be parsed correctly
    CHECK_TOKENIZE("+1.23E", QString());

    // empty parameter
    CHECK_TOKENIZE("IF(A1;A2;)", "xococoo");

    // function cascade
    CHECK_TOKENIZE("SUM(ABS(-1);ABS(-1))", "xoxooiooxooioo");
}

void TestFormula::testConstant()
{
    // simple constants
    CHECK_EVAL("0", Value(0));
    CHECK_EVAL("1", Value(1));
    CHECK_EVAL("-1", Value(-1));
    CHECK_EVAL("3.14e7", Value(3.14e7));
    CHECK_EVAL("3.14e-7", Value(3.14e-7));

    // String constants (from Odf 1.2 spec)
    CHECK_EVAL("\"Hi\"", Value("Hi"));
    CHECK_EVAL("\"Hi\"\"t\"", Value("Hi\"t"));
    CHECK_EVAL("\"\\n\"", Value("\\n"));

    // Constant errors
    CHECK_EVAL("#N/A", Value::errorNA());
    CHECK_EVAL("#DIV/0!", Value::errorDIV0());
    CHECK_EVAL("#NAME?", Value::errorNAME());
    CHECK_EVAL("#NULL!", Value::errorNULL());
    CHECK_EVAL("#NUM!", Value::errorNUM());
    CHECK_EVAL("#REF!", Value::errorREF());
    CHECK_EVAL("#VALUE!", Value::errorVALUE());

}

void TestFormula::testWhitespace()
{
    CHECK_EVAL("=ROUND(  10.1  ;  0  )", Value(10));
    CHECK_EVAL("= ROUND(10.1;0)", Value(10));
    CHECK_EVAL(" =ROUND(10.1;0)", Value::errorPARSE());
    CHECK_EVAL("= ( ROUND( 9.8 ; 0 )  +  ROUND( 9.8 ; 0 ) ) ", Value(20));
    CHECK_EVAL("=(ROUND(9.8;0) ROUND(9.8;0))", Value::errorPARSE());
}

void TestFormula::testInvalid()
{
    // Basic operations always throw errors if one of the values
    // is invalid. This is the difference to SUM and co.
    CHECK_EVAL("a+0", Value::errorVALUE());
    CHECK_EVAL("0-z", Value::errorVALUE());
    CHECK_EVAL("a*b", Value::errorVALUE());
    CHECK_EVAL("u/2", Value::errorVALUE());
}

void TestFormula::testUnary()
{
    // unary minus
    CHECK_EVAL("-1", Value(-1));
    CHECK_EVAL("--1", Value(1));
    CHECK_EVAL("---1", Value(-1));
    CHECK_EVAL("----1", Value(1));
    CHECK_EVAL("-----1", Value(-1));
    CHECK_EVAL("5-1", Value(4));
    CHECK_EVAL("5--1", Value(6));
    CHECK_EVAL("5---1", Value(4));
    CHECK_EVAL("5----1", Value(6));
    CHECK_EVAL("5-----1", Value(4));
    CHECK_EVAL("5-----1*2.5", Value(2.5));
    CHECK_EVAL("5------1*2.5", Value(7.5));
    CHECK_EVAL("-SIN(0)", Value(0));
    CHECK_EVAL("1.1-SIN(0)", Value(1.1));
    CHECK_EVAL("1.2--SIN(0)", Value(1.2));
    CHECK_EVAL("1.3---SIN(0)", Value(1.3));
    CHECK_EVAL("-COS(0)", Value(-1));
    CHECK_EVAL("1.1-COS(0)", Value(0.1));
    CHECK_EVAL("1.2--COS(0)", Value(2.2));
    CHECK_EVAL("1.3---COS(0)", Value(0.3));
}

void TestFormula::testBinary()
{
    // simple binary operation
    CHECK_EVAL("0+0", Value(0));
    CHECK_EVAL("1+1", Value(2));

    // power operator is left associative
    CHECK_EVAL("2^3", Value(8));
    CHECK_EVAL("2^3^2", Value(64));

    // lead to division by zero
    CHECK_EVAL("0/0", Value::errorDIV0());
    CHECK_EVAL("1/0", Value::errorDIV0());
    CHECK_EVAL("-4/0", Value::errorDIV0());
    CHECK_EVAL("(2*3)/(6-2*3)", Value::errorDIV0());
    CHECK_EVAL("1e3+7/0", Value::errorDIV0());
    CHECK_EVAL("2^(99/0)", Value::errorDIV0());

}

void TestFormula::testOperators()
{
    // no parentheses, checking operator precendences
    CHECK_EVAL("14+3*77", Value(245));
    CHECK_EVAL("14-3*77", Value(-217));
    CHECK_EVAL("26*4+81", Value(185));
    CHECK_EVAL("26*4-81", Value(23));
    CHECK_EVAL("30-45/3", Value(15));
    CHECK_EVAL("45+45/3", Value(60));
    CHECK_EVAL("4+3*2-1", Value(9));
}

void TestFormula::testComparison()
{
    // compare numbers
    CHECK_EVAL("6>5", Value(true));
    CHECK_EVAL("6<5", Value(false));
    CHECK_EVAL("2=2", Value(true));
    CHECK_EVAL("2=22", Value(false));
    CHECK_EVAL("=3=3.0001", Value(false));
    // compare booleans
    CHECK_EVAL("=TRUE()=FALSE()", Value(false));
    CHECK_EVAL("=TRUE()=TRUE()", Value(true));
    CHECK_EVAL("=FALSE()=FALSE()", Value(true));
    // compare strings
    CHECK_EVAL("=\"Hi\"=\"Bye\"", Value(false));
    CHECK_EVAL("=\"5\"=5", Value(false));
    CHECK_EVAL("=\"Hi\"=\"HI\"", Value(false));
    CHECK_EVAL("b>a", Value(true));
    CHECK_EVAL("b<aa", Value(false));
    CHECK_EVAL("c<d", Value(true));
    CHECK_EVAL("cc>d", Value(false));
    // compare dates
    CHECK_EVAL("=DATE(2001;12;12)>DATE(2001;12;11)", Value(true));
    CHECK_EVAL("=DATE(2001;12;12)<DATE(2001;12;11)", Value(false));
    CHECK_EVAL("=DATE(1999;01;01)=DATE(1999;01;01)", Value(true));
    CHECK_EVAL("=DATE(1998;01;01)=DATE(1999;01;01)", Value(false));
    // errors cannot be compared
    CHECK_EVAL("=NA()=NA()", Value::errorNA());
    CHECK_EVAL("=NA()>NA()", Value::errorNA());
    CHECK_EVAL("#DIV/0!>0", Value::errorDIV0());
    CHECK_EVAL("5<#VALUE!", Value::errorVALUE());
    CHECK_EVAL("#DIV/0!=#DIV/0!", Value::errorDIV0());
}

void TestFormula::testString()
{
    // string expansion ...
    CHECK_EVAL("\"2\"+5", Value(7));
    CHECK_EVAL("2+\"5\"", Value(7));
    CHECK_EVAL("\"2\"+\"5\"", Value(7));
}

void TestFormula::testFunction()
{
    // function with no arguments
    CHECK_EVAL("TRUE()", Value(true));

    //the built-in sine function
    CHECK_EVAL("SIN(0)", Value(0));
    CHECK_EVAL("2+sin(\"2\"-\"2\")", Value(2));
    CHECK_EVAL("\"1\"+sin(\"0\")", Value(1));

    // function cascades
    CHECK_EVAL("SUM(ABS( 1);ABS( 1))", Value(2));
    CHECK_EVAL("SUM(ABS( 1);ABS(-1))", Value(2));
    CHECK_EVAL("SUM(ABS(-1);ABS( 1))", Value(2));
    CHECK_EVAL("SUM(ABS(-1);ABS(-1))", Value(2));
    CHECK_EVAL("SUM(SUM(-2;-2;-2);SUM(-2;-2;-2;-2);SUM(-2;-2;-2;-2;-2))", Value(-24));
}

void TestFormula::testInlineArrays()
{
#ifdef KSPREAD_INLINE_ARRAYS
    // inline arrays
    CHECK_TOKENIZE("{1;2|3;4}", "oioioioio");

    Value array(Value::Array);
    array.setElement(0, 0, Value((int)1));
    array.setElement(1, 0, Value((int)2));
    array.setElement(0, 1, Value((int)3));
    array.setElement(1, 1, Value((int)4));
    CHECK_EVAL("={1;2|3;4}", array);

    array.setElement(1, 0, Value(0.0));
    CHECK_EVAL("={1;SIN(0)|3;4}", array);   // "dynamic"
    CHECK_EVAL("=SUM({1;2|3;4})", Value(10));
#endif
}

QTEST_KDEMAIN(TestFormula, GUI)

#include "TestFormula.moc"
