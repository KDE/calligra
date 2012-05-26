/* This file is part of the KDE project
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

 /* heavily based on Benjamin Port'work and in Ariya's work (see kspread/formula)*/

#ifndef KPRFORMULAPARSER_H
#define KPRFORMULAPARSER_H

#include <QVector>
#include <QString>
#include <QStack>

class KoShape;
class KoTextBlockData;
class KPrAnimationCache;

class FToken
{
public:
    /**
    * FToken types
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
    * Creates a FToken.
    */
    explicit FToken(Type type = Unknown, const QString &text = QString(), int pos = -1);

    static const FToken null;

    FToken(const FToken&);
    FToken &operator=(const FToken&);

    /**
    * Returns type of the FToken.
    */
    Type type() const {
        return m_type;
    }

    /**
    * Returns text associated with the FToken.
    *
    * If you want to obtain meaningful value of this FToken, instead of
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
    * Returns true if FToken is either integer or floating-point FToken.
    */
    bool isNumber() const {
        return (m_type == Number);
    }

    /**
    * Returns true if FToken is a string FToken.
    */
    bool isIdentifierName() const {
        return m_type == IdentifierName;
    }

    /**
    * Returns true if FToken is an operator FToken.
    */
    bool isOperator() const {
       return m_type == Operator;
   }

    /**
    * Returns string value for a string FToken.
    * For any other type of FToken, it returns QString().
    *
    * Note that FToken text for a string FToken still has leading and trailing
    * double-quotes, i.e for "Calligra", text() return "Calligra"
    * (with the quotes, 9 characters) while asString() only return Calligra
    * (without quotes, 7 characters).
    */
    QString asIdentifierName() const;

    /**
    * Returns operator value for an operator FToken.
    * For any other type of FToken, returns FToken::InvalidOp.
    */
    Op asOperator() const;

    /**
    * Returns qreal value for a qreal FToken.
    * For any other type of FToken, returns 0.0.
    */
    qreal asNumber() const;

protected:
    Type m_type;
    QString m_text;
    int m_pos;

};

class FTokens: public QVector<FToken>
{
public:
    FTokens(): QVector<FToken>(), m_valid(true) {}
    bool valid() const {
        return m_valid;
    }
    void setValid(bool v) {
        m_valid = v;
    }
protected:
    bool m_valid;
};

class FTokenStack : public QVector<FToken>
{
public:
    FTokenStack();
    bool isEmpty() const;
    unsigned itemCount() const;
    void push(const FToken& FToken);
    FToken pop();
    const FToken& top();
    const FToken& top(unsigned index);
private:
    void ensureSpace();
    unsigned topIndex;
};

class FOpcode
{
public:

    enum { Nop = 0, Load, Identifier, Add, Sub, Neg, Mul, Div,
           Pow, Function
         };

    unsigned type;
    unsigned index;

    FOpcode(): type(Nop), index(0) {}
    FOpcode(unsigned t): type(t), index(0) {}
    FOpcode(unsigned t, unsigned i): type(t), index(i) {}
};

class KPrFormulaParser
{
public:
    enum ParseType {
        Values,
        Formula
    };

    KPrFormulaParser(QString formula, KoShape *shape, KoTextBlockData *textBlockData, ParseType type);
    QString formula() const;
    qreal eval(KPrAnimationCache *cache, const qreal time = -1) const;
    bool valid() const;

protected:
    FTokens scan(QString formula) const;
    void compile(const FTokens &FTokens);
    qreal identifierToValue(QString identifier, KPrAnimationCache *cache, const qreal time) const;
    qreal formulaToValue(QString identifier, qreal arg1, qreal arg2) const;
private:
    KoShape *m_shape;
    KoTextBlockData *m_textBlockData;
    QString m_formula;
    bool m_fcompiled;
    mutable bool m_fvalid;
    mutable QVector<FOpcode> m_codes;
    QVector<qreal> m_constants;
    QVector<QString> m_identifier;
    QVector<QString> m_functions;
    ParseType m_type;
};

#endif // KPRFORMULAPARSER_H
