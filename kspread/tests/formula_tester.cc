/* This file is part of the KDE project
   Copyright 2004 Ariya Hidayat <ariya@kde.org>

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

#include <klocale.h>
#include <kdebug.h>

#include "tester.h"
#include "formula_tester.h"

#include <formula.h>
#include <kspread_util.h>
#include <kspread_value.h>

#define CHECK_PARSE(x,y)  checkParse(__FILE__,__LINE__,#x,x,y)
#define CHECK_EVAL(x,y)  checkEval(__FILE__,__LINE__,#x,x,y)
#define CHECK_OASIS(x,y)  checkOasis(__FILE__,__LINE__,#x,x,y)

using namespace KSpread;

FormulaParserTester::FormulaParserTester(): Tester()
{
}

QString FormulaParserTester::name()
{
  return QString("Formula (Parser)");
}

static char encodeTokenType( const Token& token )
{
  char result = '?';
  switch( token.type() )
  {
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

static QString describeTokenCodes( const QString& tokenCodes )
{
  QString result;

  if( tokenCodes.isEmpty() )
    result = "(invalid)";
  else
    for( unsigned i = 0; i < tokenCodes.length(); i++ )
    {
      switch( tokenCodes[i] )
      {
        case 'b': result.append( "Boolean" ); break;
        case 'i': result.append( "integer" ); break;
        case 'f': result.append( "float" ); break;
        case 'o': result.append( "operator" ); break;
        case 'c': result.append( "cell" ); break;
        case 'r': result.append( "range" ); break;
        case 'x': result.append( "identifier" ); break;
        default:  result.append( "unknown" ); break;
      }
      if( i < tokenCodes.length()-1 ) result.append( ", " );
    }

  return result.prepend("{").append("}");
}

void FormulaParserTester::checkParse( const char *file, int line, const char* msg,
  const QString& formula, const QString& tokenCodes )
{
  testCount++;

  Formula f;
  QString expr = formula;
  expr.prepend( '=' );
  f.setExpression( expr );
  Tokens tokens = f.tokens();

  QString resultCodes;
  if( tokens.valid() )
    for( unsigned i = 0; i < tokens.count(); i++ )
      resultCodes.append( encodeTokenType( tokens[i] ) );

  if( resultCodes != tokenCodes )
  {
    QString message = msg;
    message.append( " Result: ").append( describeTokenCodes( resultCodes ) );
    message.append( " Expected: ").append( describeTokenCodes( tokenCodes ) );
    fail( file, line, message );
  }
}

void FormulaParserTester::run()
{
  testCount = 0;
  errorList.clear();

  // simple, single-token formulas
  CHECK_PARSE( "True", "x" );
  CHECK_PARSE( "False", "x" );
  CHECK_PARSE( "36", "i" );
  CHECK_PARSE( "0", "i" );
  CHECK_PARSE( "3.14159", "f" );
  CHECK_PARSE( ".25", "f" );
  CHECK_PARSE( "1e-9", "f" );
  CHECK_PARSE( "2e3", "f" );
  CHECK_PARSE( ".3333e0", "f" );

  // cell/range/identifier
  CHECK_PARSE( "A1", "c" );
  CHECK_PARSE( "Sheet1!A1", "c" );
  CHECK_PARSE( "'Sheet1'!A1", "c" );
  CHECK_PARSE( "'Sheet One'!A1", "c" );
  CHECK_PARSE( "2006!A1", "c" );
  CHECK_PARSE( "2006bak!A1", "c" );
  CHECK_PARSE( "2006bak2!A1", "c" );
  CHECK_PARSE( "'2006bak2'!A1", "c" );
  CHECK_PARSE( "A1:B100", "r" );
  CHECK_PARSE( "Sheet1!A1:B100", "r" );
  CHECK_PARSE( "'Sheet One'!A1:B100", "r" );
  CHECK_PARSE( "SIN", "x" );
  // log2 and log10 are cell references and function identifiers
  CHECK_PARSE( "LOG2", "c" );
  CHECK_PARSE( "LOG10:11", "r" );
  CHECK_PARSE( "LOG2(2)", "xoio" );
  CHECK_PARSE( "LOG10(10)", "xoio" );

  // operators
  CHECK_PARSE( "+", "o" );
  CHECK_PARSE( "-", "o" );
  CHECK_PARSE( "*", "o" );
  CHECK_PARSE( "/", "o" );
  CHECK_PARSE( "+", "o" );
  CHECK_PARSE( "^", "o" );
  CHECK_PARSE( "(", "o" );
  CHECK_PARSE( ")", "o" );
  CHECK_PARSE( ",", "o" );
  CHECK_PARSE( ";", "o" );
  CHECK_PARSE( "=", "o" );
  CHECK_PARSE( "<", "o" );
  CHECK_PARSE( ">", "o" );
  CHECK_PARSE( "<=", "o" );
  CHECK_PARSE( ">=", "o" );
  CHECK_PARSE( "%", "o" );

  // commonly used formulas
  CHECK_PARSE( "A1+A2", "coc" );
  CHECK_PARSE( "2.5*B1", "foc" );
  CHECK_PARSE( "SUM(A1:Z10)", "xoro" );
  CHECK_PARSE( "MAX(Sheet1!Sales)", "xoro" );

  // should be correctly parsed though they are nonsense (can't be evaluated)
  CHECK_PARSE( "0E0.5", "ff" );
  CHECK_PARSE( "B3 D4:D5 Sheet1!K1", "crc" );
  CHECK_PARSE( "SIN A1", "xc" );
  CHECK_PARSE( "SIN A1:A20", "xr" );

  // invalid formulas, can't be parsed correctly
  CHECK_PARSE( "+1.23E", QString::null );
}

FormulaEvalTester::FormulaEvalTester(): Tester()
{
}

QString FormulaEvalTester::name()
{
  return QString("Formula (Eval)");
}

void FormulaEvalTester::checkEval( const char *file, int line, const char* msg,
  const QString& formula, const Value& expected )
{
  testCount++;

  Formula f;
  QString expr = formula;
  if ( expr[0] != '=' )
    expr.prepend( '=' );
  f.setExpression( expr );
  Value result = f.eval();

  if( !result.equal( expected ) )
  {
    QString message;
    QTextStream ts( &message, IO_WriteOnly );
    ts << msg;
    ts << " Result: " << result;
    ts << " Expected: " << expected;
    fail( file, line, message );
  }
}


void FormulaEvalTester::run()
{
  testCount = 0;
  errorList.clear();

  // simple constants
  CHECK_EVAL( "0", Value(0) );
  CHECK_EVAL( "1", Value(1) );
  CHECK_EVAL( "-1", Value(-1) );
  CHECK_EVAL( "3.14e7", Value(3.14e7) );
  CHECK_EVAL( "3.14e-7", Value(3.14e-7) );


  // simple binary operation
  CHECK_EVAL( "0+0", Value(0) );
  CHECK_EVAL( "1+1", Value(2) );

  // unary minus
  CHECK_EVAL( "-1", Value(-1) );
  CHECK_EVAL( "--1", Value(1) );
  CHECK_EVAL( "---1", Value(-1) );
  CHECK_EVAL( "----1", Value(1) );
  CHECK_EVAL( "-----1", Value(-1) );
  CHECK_EVAL( "5-1", Value(4) );
  CHECK_EVAL( "5--1", Value(6) );
  CHECK_EVAL( "5---1", Value(4) );
  CHECK_EVAL( "5----1", Value(6) );
  CHECK_EVAL( "5-----1", Value(4) );

  // no parentheses, checking operator precendences
  CHECK_EVAL( "14+3*77", Value(245) );
  CHECK_EVAL( "14-3*77", Value(-217) );
  CHECK_EVAL( "26*4+81", Value(185) );
  CHECK_EVAL( "26*4-81", Value(23) );
  CHECK_EVAL( "30-45/3", Value(15) );
  CHECK_EVAL( "45+45/3", Value(60) );
  CHECK_EVAL( "4+3*2-1", Value(9) );

  // power operator is right associative
  CHECK_EVAL( "2^3", Value(8) );
  CHECK_EVAL( "2^3^2", Value(512) );

  // lead to division by zero
  CHECK_EVAL( "0/0", Value::errorDIV0() );
  CHECK_EVAL( "1/0", Value::errorDIV0() );
  CHECK_EVAL( "-4/0", Value::errorDIV0() );
  CHECK_EVAL( "(2*3)/(6-2*3)", Value::errorDIV0() );
  CHECK_EVAL( "1e3+7/0", Value::errorDIV0() );
  CHECK_EVAL( "2^(99/0)", Value::errorDIV0() );

  // string expansion ...
  CHECK_EVAL( "\"2\"+5", Value(7) );
  CHECK_EVAL( "2+\"5\"", Value(7) );
  CHECK_EVAL( "\"2\"+\"5\"", Value(7) );

  //the built-in sine function
  CHECK_EVAL ("SIN(0)", Value(0));
  CHECK_EVAL ("2+sin(\"2\"-\"2\")", Value(2));
  CHECK_EVAL ("\"1\"+sin(\"0\")", Value(1));

  // tests from the OpenFormula testing suite:
  // note that these get auto-generated using generate-openformula-tests
  CHECK_EVAL("=(1/3)*3=1", Value(true));  // row 51
  CHECK_EVAL("=(\"4\" & \"5\")+2", Value(47));  // row 57
  CHECK_EVAL("=2+(\"4\" & \"5\")", Value(47));  // row 58
  CHECK_EVAL("=1+2", Value(3));  // row 63
  CHECK_EVAL("=3-1", Value(2));  // row 65
  CHECK_EVAL("=5--2", Value(7));  // row 67
  CHECK_EVAL("=3*4", Value(12));  // row 68
  CHECK_EVAL("=2+3*4", Value(14));  // row 70
  CHECK_EVAL("=6/3", Value(2));  // row 71
  CHECK_EVAL("=5/2", Value(2.5));  // row 72
  CHECK_EVAL("=ISERROR(1/0)", Value(true));  // row 73
  CHECK_EVAL("=2^3", Value(8));  // row 74
  CHECK_EVAL("=9^0.5", Value(3));  // row 75
  CHECK_EVAL("=(-5)^3", Value(-125));  // row 76
  CHECK_EVAL("=4^-1", Value(0.25));  // row 77
  CHECK_EVAL("=5^0", Value(1));  // row 78
  CHECK_EVAL("=0^5", Value(0));  // row 79
  CHECK_EVAL("=2+3*4^2", Value(50));  // row 80
  CHECK_EVAL("=-2^2", Value(4));  // row 81
  CHECK_EVAL("=1=1", Value(true));  // row 82
  CHECK_EVAL("=1=0", Value(false));  // row 84
  CHECK_EVAL("=3=3.0001", Value(false));  // row 85
// Not passed for line 86.
  CHECK_EVAL("=\"Hi\"=\"Bye\"", Value(false));  // row 87
  CHECK_EVAL("=FALSE()=FALSE()", Value(true));  // row 88
  CHECK_EVAL("=TRUE()=FALSE()", Value(false));  // row 89
  CHECK_EVAL("=\"5\"=5", Value(false));  // row 90
  CHECK_EVAL("=TRUE()=1", Value(false));  // row 91
// Not passed for line 92.
// Not passed for line 93.
  CHECK_EVAL("=1<>1", Value(false));  // row 94
  CHECK_EVAL("=1<>2", Value(true));  // row 95
  CHECK_EVAL("=1<>\"1\"", Value(true));  // row 96
// Not passed for line 97.
  CHECK_EVAL("=5<6", Value(true));  // row 98
  CHECK_EVAL("=5<=6", Value(true));  // row 99
  CHECK_EVAL("=5>6", Value(false));  // row 100
  CHECK_EVAL("=5>=6", Value(false));  // row 101
  CHECK_EVAL("=\"A\"<\"B\"", Value(true));  // row 102
// Not passed for line 103.
  CHECK_EVAL("=\"AA\">\"A\"", Value(true));  // row 104
  CHECK_EVAL("=\"Hi \" & \"there\"", Value("Hi there"));  // row 107
  CHECK_EVAL("=\"H\" & \"\"", Value("H"));  // row 108
// Not passed for line 109.
  CHECK_EVAL("=50%", Value(0.5));  // row 111
  CHECK_EVAL("=20+50%", Value(20.5));  // row 112
  CHECK_EVAL("=+5", Value(5));  // row 113
  CHECK_EVAL("=+\"Hello\"", Value("Hello"));  // row 114
  CHECK_EVAL("=-\"7\"", Value(-7));  // row 116
/*
 These are currently disabled, due to being locale specific.
 CHECK_EVAL("=DATE(2005;1;3)=DATEVALUE(\"2005-01-03\")", Value(true));  // row 118
  CHECK_EVAL("=DATE(2017.5; 1; 2)=DATEVALUE(\"2017-01-02\")", Value(true));  // row 119
  CHECK_EVAL("=DATE(2006; 2.5; 3)=DATEVALUE(\"2006-02-03\")", Value(true));  // row 120
  CHECK_EVAL("=DATE(2006; 1; 3.5)=DATEVALUE(\"2006-01-03\")", Value(true));  // row 121
  CHECK_EVAL("=DATE(2006; 13; 3)=DATEVALUE(\"2007-01-03\")", Value(true));  // row 122
  CHECK_EVAL("=DATE(2006; 1; 32)=DATEVALUE(\"2006-02-01\")", Value(true));  // row 123
  CHECK_EVAL("=DATE(2006; 25; 34)=DATEVALUE(\"2008-02-03\")", Value(true));  // row 124
  CHECK_EVAL("=DATE(2006;-1; 1)=DATEVALUE(\"2005-11-01\")", Value(true));  // row 125
// Not passed for line 126.
// Not passed for line 127.
  CHECK_EVAL("=DATE(2004;2;29)=DATEVALUE(\"2004-02-29\")", Value(true));  // row 128
  CHECK_EVAL("=DATE(2003;2;29)=DATEVALUE(\"2003-03-01\")", Value(true));  // row 129
  CHECK_EVAL("=DATE(1904; 1; 1)=DATEVALUE(\"1904-01-01\")", Value(true));  // row 130
  CHECK_EVAL("=DATEVALUE(\"2004-12-25\")=DATE(2004;12;25)", Value(true));  // row 131
  CHECK_EVAL("=DAY(\"2006-05-21\")", Value(21));  // row 132
  CHECK_EVAL("=DAY(\"5/21/2006\")", Value(21));  // row 133
  CHECK_EVAL("=DAY(\"05-21-2006\")", Value(21));  // row 134
  CHECK_EVAL("=DAY(\"5/21/06\")", Value(21));  // row 135
  CHECK_EVAL("=DAY(\"5-21-06\")", Value(21));  // row 136
*/

  // functions with optional arguments
  CHECK_EVAL("=ROUND(0.1)", Value(0));
  CHECK_EVAL("=ROUND(0.11;1)", Value(0.1));
  CHECK_EVAL("=ROUNDUP(0.1)", Value(1));
  CHECK_EVAL("=ROUNDUP(0.01;1)", Value(0.1));
  CHECK_EVAL("=ROUNDDOWN(0.9)", Value(0));
  CHECK_EVAL("=ROUNDDOWN(0.19;1)", Value(0.1));
}



FormulaOasisConversionTester::FormulaOasisConversionTester(): Tester()
{
}

QString FormulaOasisConversionTester::name()
{
  return QString("Formula (OpenDocument conversion)");
}

void FormulaOasisConversionTester::run()
{
  testCount = 0;
  errorList.clear();

  // cell references
  CHECK_OASIS( "A1", ".A1" );
  CHECK_OASIS( "A1:A4", ".A1:.A4" );
  CHECK_OASIS( "A$1:$A4", ".A$1:.$A4" );
  CHECK_OASIS( "Sheet2!A1", "Sheet2.A1" );
  CHECK_OASIS( "'Sheet 2'!A1", "'Sheet 2'.A1" );
  CHECK_OASIS( "=A1", "=[.A1]" );
  CHECK_OASIS( "=A1:A4", "=[.A1:A4]" );
  CHECK_OASIS( "=A$1:$A4", "=[.A$1:$A4]" );
  CHECK_OASIS( "=Sheet2!A1", "=[Sheet2.A1]" );
  CHECK_OASIS( "='Sheet 2'!A1", "=['Sheet 2'.A1]" );

  // equality
  CHECK_OASIS( "=A1==A2", "=[.A1]=[.A2]" );

  // strings
  CHECK_OASIS( "=\"2,2\"+2,1+\"2,0\"", "=\"2,2\"+2.1+\"2,0\"" );

  // decimal separator ','
  CHECK_OASIS( "=,12", "=.12" );
  CHECK_OASIS( "=12,12", "=12.12" );
  CHECK_OASIS( "=368*7*(0,1738+0,1784)*(0,1738+0,1784)", "=368*7*(0.1738+0.1784)*(0.1738+0.1784)"  );

  // function names
  CHECK_OASIS( "=sum(A1;A2;A3;A4;A5)", "=sum([.A1];[.A2];[.A3];[.A4];[.A5])" );
}

void FormulaOasisConversionTester::checkOasis( const char *file, int line, const char* /*msg*/,
                                      const QString& localeFormula, const QString& oasisFormula )
{
  testCount++;

  KLocale locale("en_US");
  locale.setDecimalSymbol(",");

  // KSpread -> OpenDocument
  QString formula = localeFormula;
#if 0
  Oasis::encodeFormula( formula, &locale );

  if( formula != oasisFormula )
  {
    QString message = "[Locale->Oasis] ";
    message.append( "\"" + localeFormula + "\"" );
    message.append( " Result: ").append( formula );
    message.append( " Expected: ").append( oasisFormula );
    fail( file, line, message );
  }

  testCount++;
#endif

  // OpenDocument -> KSpread
  formula = Oasis::decodeFormula( oasisFormula, &locale );

  if( formula != localeFormula )
  {
    QString message = "[Oasis->Locale] ";
    message.append( "\"" + oasisFormula + "\"" );
    message.append( " Result: ").append( formula );
    message.append( " Expected: ").append( localeFormula );
    fail( file, line, message );
  }
}
