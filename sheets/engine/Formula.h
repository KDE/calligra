// This file is part of the KDE project
// SPDX-FileCopyrightText: 2003, 2004 Ariya Hidayat <ariya@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_FORMULA
#define CALLIGRA_SHEETS_FORMULA

#include <QHash>
#include <QString>
#include <QTextStream>
#include <QVector>

#include "CellBase.h"
#include "sheets_engine_export.h"

#define CALLIGRA_SHEETS_INLINE_ARRAYS

namespace Calligra
{
namespace Sheets
{
class Localization;
class SheetBase;
typedef QHash<CellBase, CellBase> CellIndirection;

/**
 * \ingroup Value
 * A formula token.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT Token
{
public:
    /**
     * token types
     */
    enum Type {
        Unknown = 0, ///< unknown type
        Boolean, ///< True, False (also i18n-ized)
        Integer, ///< 14, 3, 1977
        Float, ///< 3.141592, 1e10, 5.9e-7
        String, ///< "Calligra", "The quick brown fox..."
        Operator, ///< +, *, /, -
        Cell, ///< $A$1, F4, Sheet2!B5, 'Sales Forecast'!Sum
        Range, ///< C1:C100
        Identifier, ///< function name or named area
        Error ///< error, like \#REF!, \#VALUE!, ...
    };

    /**
     * operator types
     */
    enum Op {
        InvalidOp = 0, ///< invalid operator
        Plus, ///<  + (addition)
        Minus, ///<  - (subtraction, negation)
        Asterisk, ///<  * (multiplication)
        Slash, ///<  / (division)
        Caret, ///<  ^ (power)
        Intersect, ///< " " (a space means intersection)
        LeftPar, ///<  (
        RightPar, ///<  )
        Comma, ///<  ,
        Semicolon, ///<  ; (argument separator)
        Ampersand, ///<  & (string concat)
        Equal, ///<  =
        NotEqual, ///<  <>
        Less, ///<  <
        Greater, ///<  >
        LessEqual, ///<  <=
        GreaterEqual, ///<  >=
        Percent, ///<  %
        CurlyBra, ///<  { (array start)
        CurlyKet, ///<  } (array end)
        Pipe, ///<  | (array row separator)
        Union ///<  ~ (union of ranges)
    };

    /**
     * Creates a token.
     */
    explicit Token(Type type = Unknown, const QString &text = QString(), int pos = -1);

    static const Token null;

    Token(const Token &);
    Token &operator=(const Token &);

    /**
     * Returns type of the token.
     */
    Type type() const
    {
        return m_type;
    }

    /**
     * Returns text associated with the token.
     *
     * If you want to obtain meaningful value of this token, instead of
     * text(), you might use asInteger(), asFloat(), asString(), sheetName(),
     * etc.
     */
    const QString &text() const
    {
        return m_text;
    }

    int pos() const
    {
        return m_pos;
    }

    /**
     * Returns true if token is a boolean token.
     */
    bool isBoolean() const
    {
        return m_type == Boolean;
    }

    /**
     * Returns true if token is a integer token.
     */
    bool isInteger() const
    {
        return m_type == Integer;
    }

    /**
     * Returns true if token is a floating-point token.
     */
    bool isFloat() const
    {
        return m_type == Float;
    }

    /**
     * Returns true if token is either integer or floating-point token.
     */
    bool isNumber() const
    {
        return (m_type == Integer) || (m_type == Float);
    }

    /**
     * Returns true if token is a string token.
     */
    bool isString() const
    {
        return m_type == String;
    }

    /**
     * Returns true if token is an operator token.
     */
    bool isOperator() const
    {
        return m_type == Operator;
    }

    /**
     * Returns true if token is a cell reference token.
     */
    bool isCell() const
    {
        return m_type == Cell;
    }

    /**
     * Returns true if token is a range reference token.
     */
    bool isRange() const
    {
        return m_type == Range;
    }

    /**
     * Returns true if token is an identifier.
     */
    bool isIdentifier() const
    {
        return m_type == Identifier;
    }

    /**
     * Returns true if token is a error token.
     */
    bool isError() const
    {
        return m_type == Error;
    }

    /**
     * Returns boolean value for an boolean token.
     * For any other type of token, return value is undefined.
     */
    bool asBoolean() const;

    /**
     * Returns integer value for an integer token.
     * For any other type of token, returns 0.
     */
    int64_t asInteger() const;

    /**
     * Returns floating-point value for a floating-point token.
     * For any other type of token, returns 0.0.
     */
    double asFloat() const;

    /**
     * Returns string value for a string token.
     * For any other type of token, it returns QString().
     *
     * Note that token text for a string token still has leading and trailing
     * double-quotes, i.e for "Calligra", text() return "Calligra"
     * (with the quotes, 9 characters) while asString() only return Calligra
     * (without quotes, 7 characters).
     */
    QString asString() const;

    /**
     * Returns operator value for an operator token.
     * For any other type of token, returns Token::InvalidOp.
     */
    Op asOperator() const;

    /**
     * Returns string value for a error token.
     * For any other type of token, it returns QString().
     */
    QString asError() const;

    /**
     * Returns sheet name in a cell reference token.
     * For any other type of token, it returns QString().
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
    int m_pos;
};

/**
 * \ingroup Value
 * An array of formula tokens.
 *
 */
class Tokens : public QVector<Token>
{
public:
    Tokens()
        : QVector<Token>()
        , m_valid(true)
    {
    }
    bool valid() const
    {
        return m_valid;
    }
    void setValid(bool v)
    {
        m_valid = v;
    }

protected:
    bool m_valid;
};

/**
 * \ingroup Value
 * A formula for a cell.
 *
 * A Formula is a equations which perform calculations on values in the cells
 * and sheets. Every formula must start with an equal sign (=).
 *
 *
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT Formula
{
public:
    /**
     * Creates a formula. It must be owned by a sheet.
     */
    Formula(SheetBase *sheet, const CellBase &cell);

    /**
     * Creates a formula. It must be owned by a sheet.
     */
    explicit Formula(SheetBase *sheet);

    /**
     * Creates a formula that is not owned by any sheet.
     * This might be useful in some cases.
     */
    Formula();

    /**
     * Returns a null formula object, this is quicker than creating a new one.
     */
    static Formula empty();

    /**
     * Copy constructor.
     */
    Formula(const Formula &);

    /**
     * Destroys the formula.
     */
    ~Formula();

    /**
     * Returns the cell which owns this formula.
     */
    SheetBase *sheet() const;
    /**
     * Returns the cell which owns this formula.
     */
    const CellBase &cell() const;

    /**
     * Is the formula empty?
     */
    bool isEmpty() const;

    /**
     * Sets the expression for this formula.
     */
    void setExpression(const QString &expr);

    /**
     * Gets the expression of this formula.
     */
    QString expression() const;

    /**
     * Clears everything, makes as like a newly constructed formula.
     */
    void clear();

    /**
     * Returns true if the specified expression is valid, i.e. it contains
     * no parsing error.
     * Empty formula (i.e. without expression) is always invalid.
     */
    bool isValid() const;

    /**
     * Returns list of tokens associated with this formula. This has nothing to
     * with the formula evaluation but might be useful, e.g. for syntax
     * highlight or similar features.
     * If the formula contains error, the returned tokens is invalid.
     */
    Tokens tokens() const;

    /**
     * Evaluates the formula and returns the result.
     * The optional cellIndirections parameter can be used to replace all
     * occurrences of a references to certain cells with references to
     * different cells. If this mapping is non-empty this does mean
     * that intermediate results can't be cached.
     */
    Value eval(CellIndirection cellIndirections = CellIndirection()) const;

    /**
     * Given an expression, this function separates it into tokens.
     * If the expression contains error (e.g. unknown operator, string no terminated)
     * this function returns tokens which is not valid.
     */
    Tokens scan(const QString &expr, const Localization *locale = nullptr) const;

    // Locale belonging to the current cell/sheet, if any.
    Localization *locale() const;

    /**
     * Assignment operator.
     */
    Formula &operator=(const Formula &);

    bool operator==(const Formula &) const;
    inline bool operator!=(const Formula &o) const
    {
        return !operator==(o);
    }

    QString dump() const;

    /**
     * helper function: return true for valid identifier character
     */
    static bool isIdentifier(QChar ch);

    /**
     * helper function: return operator of given token text
     * e.g. "*" yields Operator::Asterisk, and so on
     */
    static Token::Op matchOperator(const QString &text);

    /**
     * helper function to parse operator
     *
     * If a operator is found the data and out pointer are advanced by the number
     * of chars the operators consist of.
     * @param data pointer into the input string
     * @param out pointer into the out string, The out string needs to be big enough
     *
     * @returns true if a operator was found, false otherwise.
     */
    static bool parseOperator(const QChar *&data, QChar *&out);

protected:
    void compile(const Tokens &tokens) const;

    /**
     * helper function: return true for valid named area
     */
    bool isNamedArea(const QString &expr) const;

    /**
     * helper function for recursive evaluations; makes sure one cell
     * is not evaluated more than once resulting in infinite loops
     */
    Value evalRecursive(CellIndirection cellIndirections, QHash<CellBase, Value> &values) const;

private:
    class Private;
    QSharedDataPointer<Private> d;
};

/**
 * Dumps the formula, should be used only to assist debugging.
 */
QTextStream &operator<<(QTextStream &ts, Formula formula);

/***************************************************************************
  QHash/QSet support
****************************************************************************/

inline size_t qHash(const Formula &formula, size_t seed = 0)
{
    return qHash(formula.expression(), seed);
}

} // namespace Sheets
} // namespace Calligra

Q_DECLARE_METATYPE(Calligra::Sheets::Formula)
Q_DECLARE_TYPEINFO(Calligra::Sheets::Formula, Q_MOVABLE_TYPE);

#endif // CALLIGRA_SHEETS_FORMULA
