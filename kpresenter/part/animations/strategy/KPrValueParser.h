/* This file is part of the KDE project
 * Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

/* heavily based on Ariya's work (see kspread/formula) -- all errors are my own! */

#ifndef KPRVALUEPARSER_H
#define KPRVALUEPARSER_H

#include <QVector>
#include <QString>

class KoShape;
class KPrAnimationCache;

class Token
{
public:
    /**
    * token types
    */
    enum Type {
        Unknown = 0, ///< unknown type
        Number,     ///< 14, 3, 1977, 3.14
        IdentifierName,      ///< "height", "x", "pi" ...
        Operator,    ///< +, *, /, -
    };

    /**
    * operator types
    */
    enum Op {
        InvalidOp = 0,  ///< invalid operator
        Plus,           ///<  + (addition)
        Minus,          ///<  - (subtraction, negation)
        Asterisk,       ///<  * (multiplication)
        Slash,          ///<  / (division)
        Caret,          ///<  ^ (power)
        LeftPar,        ///<  (
        RightPar,       ///<  )
    };

    /**
    * Creates a token.
    */
    explicit Token(Type type = Unknown, const QString& text = QString(), int pos = -1);

    static const Token null;

    Token(const Token&);
    Token& operator=(const Token&);

    /**
    * Returns type of the token.
    */
    Type type() const {
        return m_type;
    }

    /**
    * Returns text associated with the token.
    *
    * If you want to obtain meaningful value of this token, instead of
    * text(), you might use asInteger(), asFloat(), asString(), sheetName(),
    * etc.
    */
    QString text() const {
            return m_text;
    }

    int pos() const {
        return m_pos;
    }

    /**
    * Returns true if token is either integer or floating-point token.
    */
    bool isNumber() const {
        return (m_type == Number);
    }

    /**
    * Returns true if token is a string token.
    */
    bool isIdentifierName() const {
        return m_type == IdentifierName;
    }

    /**
    * Returns true if token is an operator token.
    */
    bool isOperator() const {
       return m_type == Operator;
   }

    /**
    * Returns string value for a string token.
    * For any other type of token, it returns QString().
    *
    * Note that token text for a string token still has leading and trailing
    * double-quotes, i.e for "KOffice", text() return "KOffice"
    * (with the quotes, 9 characters) while asString() only return KOffice
    * (without quotes, 7 characters).
    */
    QString asIdentifierName() const;

    /**
    * Returns operator value for an operator token.
    * For any other type of token, returns Token::InvalidOp.
    */
    Op asOperator() const;

    /**
    * Returns qreal value for a qreal token.
    * For any other type of token, returns 0.0.
    */
    qreal asNumber() const;

protected:
    Type m_type;
    QString m_text;
    int m_pos;

};

class Tokens: public QVector<Token>
{
public:
    Tokens(): QVector<Token>(), m_valid(true) {}
    bool valid() const {
        return m_valid;
    }
    void setValid(bool v) {
        m_valid = v;
    }
protected:
    bool m_valid;
};

class TokenStack : public QVector<Token>
{
public:
    TokenStack();
    bool isEmpty() const;
    unsigned itemCount() const;
    void push(const Token& token);
    Token pop();
    const Token& top();
    const Token& top(unsigned index);
private:
    void ensureSpace();
    unsigned topIndex;
};

class Opcode
{
public:

    enum { Nop = 0, Load, Identifier, Add, Sub, Neg, Mul, Div,
           Pow
         };

    unsigned type;
    unsigned index;

    Opcode(): type(Nop), index(0) {}
    Opcode(unsigned t): type(t), index(0) {}
    Opcode(unsigned t, unsigned i): type(t), index(i) {}
};

class KPrValueParser
{
public:
    KPrValueParser(QString formula, KoShape *shape);
    QString formula() const;
    qreal eval(KPrAnimationCache * cache) const;
    bool valid() const;
protected:
    Tokens scan(QString formula);
    void compile(const Tokens& tokens) const;
    qreal identifierToValue(QString identifier, KPrAnimationCache * cache) const;
private:
    KoShape * m_shape;
    QString m_formula;
    mutable bool m_compiled;
    mutable bool m_valid;
    mutable QVector<Opcode> m_codes;
    mutable QVector<qreal> m_constants;
    mutable QVector<QString> m_identifier;
};

#endif // KPRVALUEPARSER_H
