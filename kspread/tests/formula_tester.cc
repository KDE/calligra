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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "tester.h"
#include "formula_tester.h"

#include <formula.h>
#include <kspread_value.h>

#define CHECK_PARSE(x,y)  checkParse(__FILE__,__LINE__,#x,x,y)
#define CHECK_EVAL(x,y)  checkEval(__FILE__,__LINE__,#x,x,y)

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
  CHECK_PARSE( "True", "b" );
  CHECK_PARSE( "False", "b" );
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
  CHECK_PARSE( "'Sheet One'!A1", "c" );
  CHECK_PARSE( "A1:B100", "r" );
  CHECK_PARSE( "Sheet1!A1:B100", "r" );
  CHECK_PARSE( "'Sheet One'!A1:B100", "r" );
  CHECK_PARSE( "SIN", "x" );
  
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
  const QString& formula, const KSpreadValue& expected )
{
  testCount++;
  
  Formula f;
  QString expr = formula;
  expr.prepend( '=' );
  f.setExpression( expr );
  KSpreadValue result = f.eval();

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
  CHECK_EVAL( "0", KSpreadValue(0) );
  CHECK_EVAL( "1", KSpreadValue(1) );
  CHECK_EVAL( "-1", KSpreadValue(-1) );
  CHECK_EVAL( "3.14e7", KSpreadValue(3.14e7) );
  CHECK_EVAL( "3.14-e7", KSpreadValue(3.14e-7) );
  
  
  // simple binary operation  
  CHECK_EVAL( "0+0", KSpreadValue(0) );
  CHECK_EVAL( "1+1", KSpreadValue(2) );
}
