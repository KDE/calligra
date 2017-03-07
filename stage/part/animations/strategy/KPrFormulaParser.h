/* This file is part of the KDE project
 * Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>
 * Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>
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

#ifndef KPRFORMULAPARSER_H
#define KPRFORMULAPARSER_H

// Don't show this warning: it occurs because we inherit from QVector, which we
// strictly speaking shouldn't do, but which we apparently feel is fine
// anyway.
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wstrict-overflow"
#endif

#include <QVector>
#include <QString>
#include <QStack>

class KoShape;
class QTextBlockUserData;
class KPrAnimationCache;

class Token
{
public:
    /**
    * Token types
    */
    enum Type {
        Unknown = 0,         ///< unknown type
        Number,             ///< 14, 3, 1977, 3.14
        IdentifierName,     ///< "height", "x", "pi" ...
        Operator,           ///< +, *, /, -
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
        Comma           ///< ,
    };

    /**
    * Creates a Token.
    */
    explicit Token(Type type = Unknown, const QString &text = QString(), int pos = -1);

    static const Token null;

    Token(const Token&);
    Token &operator=(const Token&);

    /**
    * Returns type of the Token.
    */
    Type type() const {
        return m_type;
    }

    /**
    * Returns text associated with the Token.
    *
    * If you want to obtain meaningful value of this Token, instead of
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
    * Returns true if Token is either integer or floating-point Token.
    */
    bool isNumber() const {
        return (m_type == Number);
    }

    /**
    * Returns true if Token is a string Token.
    */
    bool isIdentifierName() const {
        return m_type == IdentifierName;
    }

    /**
    * Returns true if Token is an operator Token.
    */
    bool isOperator() const {
       return m_type == Operator;
   }

    /**
    * Returns string value for a string Token.
    * For any other type of Token, it returns QString().
    *
    * Note that Token text for a string Token still has leading and trailing
    * double-quotes, i.e for "Calligra", text() return "Calligra"
    * (with the quotes, 9 characters) while asString() only return Calligra
    * (without quotes, 7 characters).
    */
    QString asIdentifierName() const;

    /**
    * Returns operator value for an operator Token.
    * For any other type of Token, returns Token::InvalidOp.
    */
    Op asOperator() const;

    /**
    * Returns qreal value for a qreal Token.
    * For any other type of Token, returns 0.0.
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
    void push(const Token& Token);
    Token pop();
    const Token& top() const;
    const Token& top(unsigned index) const;
private:
    void ensureSpace();
    unsigned topIndex;
};

class Opcode
{
public:

    enum { Nop = 0, Load, Identifier, Add, Sub, Neg, Mul, Div,
           Pow, Function
         };

    unsigned type;
    unsigned index;

    Opcode(): type(Nop), index(0) {}
    Opcode(unsigned t): type(t), index(0) {}
    Opcode(unsigned t, unsigned i): type(t), index(i) {}
};

class KPrFormulaParser
{
public:
    enum ParseType {
        Values,
        Formula
    };

    KPrFormulaParser(const QString &formula, KoShape *shape, QTextBlockUserData *textBlockUserData, ParseType type);
    QString formula() const;
    qreal eval(KPrAnimationCache *cache, const qreal time = -1) const;
    bool valid() const;

protected:
    Tokens scan(const QString &formula) const;
    void compile(const Tokens &tokens);
    qreal identifierToValue(const QString &identifier, KPrAnimationCache *cache, const qreal time) const;
    qreal formulaToValue(const QString &identifier, qreal arg1, qreal arg2) const;
    qreal formulaToValue(const QString &identifier, qreal arg1) const;
private:
    KoShape *m_shape;
    QTextBlockUserData *m_textBlockData;
    QString m_formula;
    bool m_fcompiled;
    mutable bool m_fvalid;
    mutable QVector<Opcode> m_codes;
    QVector<qreal> m_constants;
    QVector<QString> m_identifier;
    QVector<QString> m_functions;
    ParseType m_type;
};

#endif // KPRFORMULAPARSER_H
