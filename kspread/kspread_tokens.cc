/* This file is part of the KDE project
   Copyright (C) 2003 Ariya Hidayat <ariya@kde.org>

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

#include <qstring.h>
#include <qregexp.h>

#include <kspread_tokens.h>

// helper function: return operator of given token text
// e.g "+" yields Operator::Asterisk, and so on
static int matchOperator( const QString& text )
{
  int result = KSpreadToken::InvalidOp;
  QChar p = text[0];
  QChar q = text[1];

  switch( p.unicode() )
  {
    case '+': result = KSpreadToken::Plus; break;
    case '-': result = KSpreadToken::Minus; break;
    case '*': result = KSpreadToken::Asterisk; break;
    case '/': result = KSpreadToken::Slash; break;
    case ',': result = KSpreadToken::Comma; break;
    case ';': result = KSpreadToken::Semicolon; break;
    case '(': result = KSpreadToken::LeftPar; break;
    case ')': result = KSpreadToken::RightPar; break;
    case '&': result = KSpreadToken::Ampersand; break;
    case '=': result = KSpreadToken::Equal; break;
    case '<': result = (q != '=' ) ? KSpreadToken::Less :
      KSpreadToken::LessEqual; break;
    case '>': result = (q != '=' ) ? KSpreadToken::Greater :
      KSpreadToken::GreaterEqual; break;
    default: result = KSpreadToken::InvalidOp; break;
  }
  return result;
}

/**********************
    KSpreadToken
 **********************/

// creates a token
KSpreadToken::KSpreadToken( Type _type, const QString& _text )
{
  m_type = _type;
  m_text = _text;
}

KSpreadToken::KSpreadToken( const KSpreadToken& token )
{
  operator=( token );
}

KSpreadToken& KSpreadToken::operator=( const KSpreadToken& token )
{
  m_type = token.m_type;
  m_text = token.m_text;
}

int KSpreadToken::asInteger() const
{
  if( isInteger() ) return m_text.toInt();
  else return 0;
}

double KSpreadToken::asFloat() const
{
  if( isFloat() ) return m_text.toDouble();
  else return 0.0;
}

QString KSpreadToken::asString() const
{
  if( isString() ) return m_text.mid( 1, m_text.length()-2 );
  else return QString::null;
}

int KSpreadToken::asOperator() const
{
  if( isOperator() ) return matchOperator( m_text );
  else return -1;
}

QString KSpreadToken::sheetName() const
{
  if( !isCell() && !isRange() ) return QString::null;
  int i = m_text.find( '!' );
  if( i < 0 ) return QString();
  QString sheet = m_text.left( i );
  if( sheet[0] == QChar(39) )
    sheet = sheet.mid( 1, sheet.length()-2 );
  return sheet;
}

QString KSpreadToken::description() const
{
  QString desc;

  switch (m_type )
  {
  case  Integer:    desc = "Integer"; break;
  case  Float:      desc = "Float"; break;
  case  String:     desc = "String"; break;
  case  Identifier: desc = "Identifier"; break;
  case  Cell:       desc = "Cell"; break;
  case  Range:      desc = "Range"; break;
  case  Operator:   desc = "Operator"; break;
  default:          desc = "Unknown"; break;
  }

  while( desc.length() < 10 ) desc.prepend( ' ' );
  desc.append( " : " ).append( m_text );

  return desc;
}

// helper function: return true for valid identifier character
static bool isIdentifier( QChar ch )
{
  return ( ch.unicode() == '_' ) || (ch.unicode() == '$' ) || ( ch.isLetter() );
}


KSpreadTokens KSpreadToken::parse( const QString& expression )
{
  KSpreadTokens tokens;

  // parsing state
  enum { Start, Finish, Bad, InNumber, InDecimal, InExpIndicator, InExponent,
    InString, InIdentifier, InCell, InRange, InSheetName } state;

  // TODO use locale settings if specified
  QString thousand = "";
  QString decimal = ".";

  // force a terminator
  //expression.append( QChar() );

  // initialize variables
  unsigned int i = 0;
  QString tokenText;
  state = Start;

  // main loop
  while( (state != Bad) && (state != Finish) && (i < expression.length()) )
  {

    QChar ch = expression[i];

    switch( state )
    {

    case Start:

       // skip any whitespaces
       if( ch.isSpace() ) i++;

       // check for number
       else if( ch.isDigit() ) state = InNumber;

       // a string ?
       else if ( ch == '"' )
       {
         tokenText.append( expression[i++] );
         state = InString;
       }

       // beginning with alphanumeric ?
       // could be identifier, cell, range, or function...
       else if( isIdentifier( ch ) ) state = InIdentifier;

       // aposthrophe (') marks sheet name for 3-d cell, e.g 'Sales Q3'!A4
       else if ( ch.unicode() == 39 )
       {
         i++;
         state = InSheetName;
         tokenText.append( QChar( 39 ) );
       }

       // decimal dot ?
       else if ( ch == decimal )
       {
         tokenText.append( expression[i++] );
         state = InDecimal;
       }

       // terminator character
       else if ( ch == QChar::null )
          state = Finish;

       // look for operator match
       else
       {
         int op;
         QString s;

         // check for one-char operator, such as '+', ';', etc
         s.append( ch );
         op = matchOperator( s );

         if( op == KSpreadToken::InvalidOp )
         {
           // check for two-chars operator, such as '<=', '>=', etc
           s.append( expression[i+1] );
           op = matchOperator( s );
         }

         // any matched operator ?
         if( op != KSpreadToken::InvalidOp )
         {
           int len = s.length();
           i += len;
           tokens.append( KSpreadToken( KSpreadToken::Operator, s.left( len ) ) );
         }
         else state = Bad;
        }
       break;

    case InIdentifier:

       // consume as long as alpha, dollar sign, underscore, or digit
       if( isIdentifier( ch )  || ch.isDigit() ) tokenText.append( expression[i++] );

       // a '!' ? then this must be sheet name, e.g "Sheet4!"
       else if( ch == '!' )
       {
          tokenText.append( expression[i++] );
          state = InCell;
       }

       // we're done with identifier
       else
       {

         // check for cell reference,  e.g A1, VV123, ...
         QRegExp exp("(\\$?)([a-zA-Z]+)(\\$?)([0-9]+)$");
         int n = exp.search( tokenText );
         if( n >= 0 )
           state = InCell;
         else
         {
           tokens.append( KSpreadToken( KSpreadToken::Identifier, tokenText ) );
           tokenText = "";
           state = Start;
         }
       }
       break;

    case InCell:

       // consume as long as alpha, dollar sign, underscore, or digit
       if( isIdentifier( ch )  || ch.isDigit() ) tokenText.append( expression[i++] );

       // we're done with cell ref, possibly with sheet name (like "Sheet2!B2")
       // note that "Sheet2!TotalSales" is also possible, in which "TotalSales" is a named area
       else
       {

         // check if it's a cell ref like A32, not named area
         QString cell;
         for( int j = tokenText.length()-1; j>=0; j-- )
           if( tokenText[j] == '!' ) break; else cell.prepend( tokenText[j] );
         QRegExp exp("(\\$?)([a-zA-Z]+)(\\$?)([0-9]+)$");
         if( exp.search( cell ) != 0 )
         {

           // we're done with named area
           tokens.append( KSpreadToken( KSpreadToken::Range, tokenText ) );
           tokenText = "";
           state = Start;
         }

         else
         {

           // so up to now we've got something like A2 or Sheet2!F4
           // check for range reference
           if( ch == ':' )
           {
             tokenText.append( expression[i++] );
             state = InRange;
           }
           else
           {
             // we're done with cell reference
             tokens.append( KSpreadToken( KSpreadToken::Cell, tokenText ) );
             tokenText = "";
             state = Start;
           }
         }
       }
       break;

    case InRange:

       // consume as long as alpha, dollar sign, underscore, or digit
       if( isIdentifier( ch )  || ch.isDigit() ) tokenText.append( expression[i++] );

       // we're done with range reference
       else
       {
         tokens.append( KSpreadToken( KSpreadToken::Range, tokenText ) );
         tokenText = "";
         state = Start;
       }
       break;

    case InSheetName:

       // consume until '
       if( ch.unicode() != 39 ) tokenText.append( expression[i++] );

       else
       {
         // must be followed by '!'
         i++;
         if( expression[i] == '!' )
         {
           tokenText.append( expression[i++] );
           state = InCell;
         }
         else state = Bad;
       }
       break;

    case InNumber:

       // consume as long as it's digit
       if( ch.isDigit() ) tokenText.append( expression[i++] );

       // skip thousand separator
       else if( !thousand.isEmpty() && ( ch ==thousand[0] ) ) i++;

       // convert decimal separator to '.'
       else if( !decimal.isEmpty() && ( ch == decimal[0] ) )
       {
         tokenText.append( '.' );
         i++;
         state = InDecimal;
       }

       // exponent ?
       else if( ch.upper() == 'E' )
       {
         tokenText.append( 'E' );
         i++;
         state = InExpIndicator;
       }

       // we're done with integer number
       else
       {
         tokens.append( KSpreadToken( KSpreadToken::Integer, tokenText ) );
         tokenText = "";
         state = Start;
       };
       break;

    case InDecimal:

       // consume as long as it's digit
       if( ch.isDigit() ) tokenText.append( expression[i++] );

       // exponent ?
       else if( ch.upper() == 'E' )
       {
         tokenText.append( 'E' );
         i++;
         state = InExpIndicator;
       }

       // we're done with floating-point number
       else
       {
         tokens.append( KSpreadToken( KSpreadToken::Float, tokenText ) );
         tokenText = "";
         state = Start;
       };
       break;

    case InExpIndicator:

       // possible + or - right after E, e.g 1.23E+12 or 4.67E-8
       if( ( ch == '+' ) || ( ch == '-' ) ) tokenText.append( expression[i++] );

       // consume as long as it's digit
       else if( ch.isDigit() ) state = InExponent;

       // invalid thing here
       else state = Bad;

       break;

    case InExponent:

       // consume as long as it's digit
       if( ch.isDigit() ) tokenText.append( expression[i++] );

       // we're done with floating-point number
       else
       {
         tokens.append( KSpreadToken( KSpreadToken::Float, tokenText ) );
         tokenText = "";
         state = Start;
       };
       break;

    case InString:

       // consume until "'
       if( ch != '"' ) tokenText.append( expression[i++] );

       else
       {
         tokenText.append( ch ); i++;
         tokens.append( KSpreadToken( KSpreadToken::String, tokenText ) );
         state = Start;
       }
       break;

    case Bad: // bad bad bad
      break;

    default:
       break;
    };

  };

  return tokens;
}
