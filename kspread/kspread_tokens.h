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

#ifndef KSPREAD_TOKENS_H
#define KSPREAD_TOKENS_H

#include <qstring.h>
#include <qvaluevector.h>

#include "kspread_value.h"

class KLocale;
class KSpreadTable;

class KSpreadToken;
typedef QValueVector<KSpreadToken> KSpreadTokens;

/**
 * Encapsulates a token for use in the formula/expression parser.
 *
 * A token can be of several type, such as integer, floating-point,
 * cell reference, range reference, and so on.
 * Each token always have a text which is a string associated
 * with the token, e.g "1.34" for floating-point token.
 *
 * Example:
 * Parsing this formula "=1e4*A1+($A$2/SUM(B1:B5))+Sheet3!B5" yields
 * the following tokens:
 *
 *  operator       "="
 *  floating-point "1e4"
 *  operator       "*"
 *  cell reference "A1"
 *  operator       "+"
 *  operator       "("
 *  cell reference "$A$2"
 *  operator       "/"
 *  identifier     "SUM"
 *  operator       "("
 *  range          "B1:B5"
 *  operator       ")"
 *  operator       ")"
 *  operator       "+"
 *  cell reference "Sheet3!B5"
 *
 */

class KSpreadToken
{
  public:

    typedef enum
    {
      Unknown,
      Integer,     // 14, 3, 1977
      Float,       // 3.141592, 1e10, 5.9e-7
      String,      // "KOffice", "The quick brown fox..."
      Operator,    // +, *, /, -
      Cell,        // $A$1, F4, Sheet2!B5, 'Sales Forecast'!Sum
      Range,       // C1:C100
      Identifier   // function name or named area
    } Type;

    enum
    {
      InvalidOp = 0,
      Plus,           // +
      Minus,          // -
      Asterisk,       // *
      Slash,          // /
      LeftPar,        // (
      RightPar,       // )
      Comma,          // ,
      Semicolon,      // ; (argument separator)
      Ampersand,      // & (string concat)
      Equal,          // =
      NotEqual,
      Less,           // <
      Greater,        // >
      LessEqual,      // <=
      GreaterEqual    // >=
    };

    /**
     * Creates a token.
     */
    KSpreadToken( Type _type = Unknown, const QString& _text = QString::null);

    /**
     * Returns type of the token.
     */
    Type type() const { return m_type; }

    KSpreadToken( const KSpreadToken& );
    KSpreadToken& operator= ( const KSpreadToken& );

    /**
     * Returns text associated with the token.
     *
     * If you want to obtain meaningful value of this token, instead of
     * text(), you might use asInteger(), asFloat(), asString(), sheetName(),
     * etc.
     */
    QString text() const { return m_text; }

    /**
     * Returns true if token is a integer token.
     */
    bool isInteger() const { return m_type == Integer; }

    /**
     * Returns true if token is a floating-point token.
     */
    bool isFloat() const { return m_type == Float; }

    /**
     * Returns true if token is either integer or floating-point token.
     */
    bool isNumber() const { return (m_type == Integer) || (m_type == Float); }

    /**
     * Returns true if token is a string token.
     */
    bool isString() const { return m_type == String; }

    /**
     * Returns true if token is an operator token.
     */
    bool isOperator() const { return m_type == Operator; }

    /**
     * Returns true if token is a cell reference token.
     */
    bool isCell() const { return m_type == Cell; }

    /**
     * Returns true if token is a range reference token.
     */
    bool isRange() const { return m_type == Range; }

    /**
     * Returns integer value for an integer token.
     * For any other type of token, returns 0.
     */
    int asInteger() const;

    /**
     * Returns floating-point value for a floating-point token.
     * For any other type of token, returns 0.0.
     */
    double asFloat() const;

    /**
     * Returns string value for a string token.
     * For any other type of token, it returns QString::null.
     *
     * Note that token text for a string token still has leading and trailing
     * double-quotes, i.e for "KOffice", text() return "KOffice"
     * (with the quotes, 9 characters) while asString() only return KOffice
     * (without quotes, 7 characters).
     */
    QString asString() const;

    /**
     * Returns operator value for an operator token.
     * For any other type of token, returns -1.
     */
    int asOperator() const;

    static KSpreadTokens parse( const QString& expression );


    /**
     * Returns sheet name in a cell reference token.
     * For any other type of token, it returns QString::null.
     *
     * If the cell reference doesn't specify sheet name, an empty string
     * is returned. As example, for "Sheet1!B3" , sheetName() returns
     * "Sheet1" while for "A2" sheetName() returns "".
     *
     * When sheet name contains quotes (as if the name has spaces) like
     * in "'Sales Forecast'!F4", sheetName() returns the name
     * without the quotes, i.e "Sales Forecast" in this case.
     */
    QString sheetName() const;

    /**
     * Returns a short description of the token.
     * Should be used only to assist debugging.
     */
    QString description() const;

  protected:

    Type m_type;
    QString m_text;

};

#endif // KSPREAD_TOKENS_H
