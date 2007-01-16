/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KOENHANCEDPATHFORMULA_H
#define KOENHANCEDPATHFORMULA_H

#include <QString>
#include <QVariant>

class KoEnhancedPathShape;

class FormulaToken
{
public:
    /// token types
    enum Type
    {
      TypeUnknown = 0, ///< unknown type
      TypeNumber,      ///< 14, 3, 1977, 3.141592, 1e10, 5.9e-7
      TypeOperator,    ///< +, *, /, -
      TypeIdentifier   ///< function name, function reference, modifier reference or named variable
    };

    /// operators
    enum Operator {
        OperatorInvalid,   ///< invalid operator
        OperatorAdd,       ///< + addition
        OperatorSub,       ///< - substraction
        OperatorMul,       ///< * multiplication
        OperatorDiv,       ///< / division
        OperatorLeftPar,   ///< ( left parentheses
        OperatorRightPar,  ///< ) right parentheses
        OperatorComma      ///< , comma
    };

    /// Constructs token with givne tyoe, textm and position
    FormulaToken( Type type = TypeUnknown, const QString & text = QString(), int position = -1 );

    /// copy constructor
    FormulaToken( const FormulaToken &token );

    /// assignment operator
    FormulaToken& operator=( const FormulaToken & token );

    /// Returns the type of the token
    Type type() const { return m_type; }
    /// Returns the text representation of the token
    QString text() const { return m_text; }
    /// Returns the position of the token
    int position() const { return m_position; }

    /// Returns if the token is a number
    bool isNumber() const { return m_type == TypeNumber; }
    /// Returns if the token is a operator, OperatorInvalid if token is no operator
    bool isOperator() const { return m_type == TypeOperator; }
    /// Returns if token is an identifier
    bool isIdentifier() const { return m_type == TypeIdentifier; }

    /// Returns the token converted to double
    double asNumber() const;
    /// Returns the token as operator
    Operator asOperator() const;
private:
    Type m_type;    ///< the token type
    QString m_text; ///< the token text representation
    int m_position; ///< the tokens position
};

typedef QList<FormulaToken> TokenList;

class Opcode;

class KoEnhancedPathFormula
{
public:
    /// functions
    enum Function {
        FunctionUnknown,
        // unary functions
        FunctionAbs,
        FunctionSqrt,
        FunctionSin,
        FunctionCos,
        FunctionTan,
        FunctionAtan,
        FunctionAtan2,
        // binary functions
        FunctionMin,
        FunctionMax,
        // ternary functions
        FunctionIf
    };

    enum Error {
        ErrorNone,    ///< no error
        ErrorValue,   ///< error when converting value
        ErrorParse,   ///< parsing error
        ErrorCompile, ///< compiling error
        ErrorName     ///< function name value
    };

    KoEnhancedPathFormula( const QString &text );
    ~KoEnhancedPathFormula();
    double evaluate( KoEnhancedPathShape * path );
    Error error() { return m_error; }
private:
    /// Separates the given formula text into tokens.
    TokenList scan( const QString &formula ) const;
    bool compile( const TokenList & tokens );
    double evaluateFunction( Function function, const QList<double> &arguments ) const;
    void debugTokens( const TokenList &tokens );
    void debugOpcodes();
    bool m_valid;
    bool m_compiled;
    Error m_error;
    QString m_text; ///< the formula text representation
    QList<QVariant> m_constants; ///< constant values
    QList<Opcode> m_codes; ///< byte code
};

#endif // KOENHANCEDPATHFORMULA_H
