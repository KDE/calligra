/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2003,2004 Ariya Hidayat <ariya@kde.org>

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

#ifndef KSPREAD_VALUE_H
#define KSPREAD_VALUE_H

#include <complex>

#include <QDateTime>
#include <QSharedDataPointer>
#include <QString>
#include <QTextStream>
#include <QVariant>

#include <kdebug.h>

#include "kspread_export.h"

#include "Number.h"

using namespace std;

namespace KSpread
{
class CalculationSettings;
class ValueStorage;

/**
 * \ingroup Value
 * Provides a wrapper for cell value.
 *
 * Each cell in a worksheet must hold a value, either as enterred by user
 * or as a result of formula evaluation. Default cell holds empty value.
 *
 * Value uses implicit data sharing to reduce memory usage.
 */
class KSPREAD_EXPORT Value
{

public:

    enum Type {
        Empty,
        Boolean,
        Integer,
        Float,
        Complex,
        String,
        Array,
        CellRange, // not used yet
        Error
    };

    enum Format {
        fmt_None,
        fmt_Boolean,
        fmt_Number,
        fmt_Percent,
        fmt_Money,
        fmt_DateTime,
        fmt_Date,
        fmt_Time,
        fmt_String
    };
    /**
     * Creates an empty value, i.e holds nothing.
     */
    Value();

    /**
     * Creates a value of certain type.
     */
    explicit Value(Type _type);

    /**
     * Destroys the value.
     */
    virtual ~Value();

    /**
     * Creates a copy from another value.
     */
    Value(const Value& _value);

    /**
     * Assigns from another value.
     *
     * Because the data is implicitly shared, such assignment is very fast and
     * doesn't consume additional memory.
     */
    Value& operator= (const Value& _value);

    /**
     * Creates a boolean value.
     */
    explicit Value(bool b);

    /**
     * Creates an integer value.
     */
    explicit Value(qint64 i);

    /**
     * Creates an integer value.
     */
    explicit Value(int i);

    /**
     * Creates a floating-point value.
     */
    explicit Value(double f);

    /**
     * Creates a floating-point value.
     */
    explicit Value(long double f);

#ifdef KSPREAD_HIGH_PRECISION_SUPPORT
    /**
     * Creates a floating-point value.
     */
    explicit Value(Number f);
#endif // KSPREAD_HIGH_PRECISION_SUPPORT

    /**
     * Creates a complex number value.
     */
    explicit Value(const complex<Number>& c);

    /**
     * Creates a string value.
     */
    explicit Value(const QString& s);

    /**
     * Creates a string value.
     */
    explicit Value(const char *s);

    /**
     * Creates an array value using the data from \p array.
     */
    explicit Value(const ValueStorage& array, const QSize& size);

    /**
     * Creates a floating-point value from date/time.
     *
     * Internally date/time is represented as serial-number, i.e number of
     * elapsed day since reference date. Day 61 is defined as March 1, 1900.
     */
    Value(const QDateTime& dt, const CalculationSettings* settings);

    /**
     * Creates a floating-point value from time.
     * See also note above.
     */
    Value(const QTime& time, const CalculationSettings* settings);

    /**
     * Creates a floating-point value from date.
     * See also note above.
     */
    Value(const QDate& date, const CalculationSettings* settings);

    /**
     * Returns the type of the value.
     */
    Type type() const;

    /**
     * Returns the format of the value, i.e. how should it be interpreted.
     */
    Format format() const;

    /**
     * Sets format information for this value.
     */
    void setFormat(Format fmt);

    /**
     * Returns true if empty.
     */
    bool isEmpty() const {
        return type() == Empty;
    }

    /**
     * Returns true, if the type of this value is Boolean.
     */
    bool isBoolean() const {
        return type() == Boolean;
    }

    /**
     * Returns true, if the type of this value is integer.
     */
    bool isInteger() const {
        return type() == Integer;
    }

    /**
     * Returns true, if the type of this value is floating-point.
     */
    bool isFloat() const {
        return type() == Float;
    }

    /**
     * Returns true, if the type of this value is the complex number type.
     */
    bool isComplex() const {
        return type() == Complex;
    }

    /**
     * Returns true, if the type of this value is either
     * integer, floating-point or complex number.
     */
    bool isNumber() const {
        return (type() == Integer) || (type() == Float) || (type() == Complex);
    }

    /**
     * Returns true, if the type of this value is string.
     */
    bool isString() const {
        return type() == String;
    }

    /**
     * Returns true, if the type of this value is array.
     */
    bool isArray() const {
        return type() == Array;
    }

    /**
     * Returns true, if this value holds error information.
     */
    bool isError() const {
        return type() == Error;
    }

    /**
     * Sets this value to hold error message.
     */
    void setError(const QString& msg);

    /**
     * Returns the boolean value of this value.
     *
     * Call this function only if isBoolean() returns true.
     */
    bool asBoolean() const;

    /**
     * Returns the integer value of this value.
     *
     * Call this function only if isNumber() returns true.
     */
    qint64 asInteger() const;

    /**
     * Returns the floating-point value of this value.
     *
     * Call this function only if isNumber() returns true.
     */
    Number asFloat() const;

    /**
     * Returns the complex number value of this value.
     *
     * Call this function only if isNumber() returns true.
     */
    complex<Number> asComplex() const;

    /**
     * Returns the string value of this value.
     *
     * Call this function only if isString() returns true.
     */
    QString asString() const;

    /**
     * Returns the data as a QVariant
     */
    QVariant asVariant() const;

    /**
     * Returns the date/time representation of this value.
     */
    QDateTime asDateTime(const CalculationSettings* settings) const;

    /**
     * Returns the date representation of this value.
     */
    QDate asDate(const CalculationSettings* settings) const;

    /**
     * Returns the time representation of this value.
     */
    QTime asTime(const CalculationSettings* settings) const;

    /**
     * Returns an element in the array value.
     */
    Value element(unsigned column, unsigned row) const;

    /**
     * Returns an array element given by its index denoting its position in the
     * row-wise sorted list of non-empty values.
     * Usable to iterate over the array.
     * \see count()
     */
    Value element(unsigned index) const;

    /**
     * Sets an element in the array value. Do not use if isArray() is false.
     */
    void setElement(unsigned column, unsigned row, const Value& value);

    /**
     * If this value is an array, return the number of columns.
     * Returns 1, if isArray() returns false.
     */
    unsigned columns() const;

    /**
     * If this value is an array, return the number of rows.
     * Returns 1, if isArray() returns false.
     */
    unsigned rows() const;

    /**
     * If this value is an array, return the number of non-empty elements.
     * Returns 1 if isArray() returns false.
     * Usable to iterate over the array.
     * \see element(unsigned)
     */
    unsigned count() const;

    /**
     * Returns error message associated with this value.
     *
     * Call this function only if isError() returns true.
     */
    QString errorMessage() const;

    /**
     * Returns constant reference to empty value.
     */
    static const Value& empty();

    /**
     * Returns constant reference to '\#CIRCLE!' error.
     *
     * This is used to indicate circular cell references.
     */
    static const Value& errorCIRCLE();

    /**
     * Returns constant reference to '\#DEPEND!' error.
     *
     * This is used to indicate broken cell references.
     */
    static const Value& errorDEPEND();

    /**
     * Returns constant reference to '\#DIV/0!' error.
     *
     * This is used to indicate that a formula divides by 0 (zero).
     */
    static const Value& errorDIV0();

    /**
     * Returns constant reference to '\#N/A' error.
     *
     * This is to indicate that  a value is not available to a function.
     */
    static const Value& errorNA();

    /**
     * Returns constant reference to '\#NAME?' error.
     *
     * This is to indicate that certain text inside formula is not
     * recognized, possibly a misspelled name or name that
     * does not exist.
     */
    static const Value& errorNAME();

    /**
     * Returns constant reference to '\#NUM!' error.
     *
     * This is to indicate a problem with a number in a formula.
     */
    static const Value& errorNUM();

    /**
     * Returns constant reference to '\#NULL!' error.
     *
     * This is to indicate that two area do not intersect.
     */
    static const Value& errorNULL();

    /**
     * Returns constant reference to '\#PARSE!' error.
     *
     * This is used to indicate that a formula could not be parsed correctly.
     */
    static const Value& errorPARSE();

    /**
     * Returns constant reference to '\#REF!' error.
     *
     * This is used to indicate an invalid cell reference.
     */
    static const Value& errorREF();

    /**
     * Returns constant reference to '\#VALUE!' error.
     *
     * This is to indicate that wrong type of argument or operand
     * is used, usually within a function call, e.g SIN("some text").
     */
    static const Value& errorVALUE();

    /**
     * Returns true if it is OK to compare this value with v.
     * If this function returns false, then return value of compare is undefined.
     */
    bool allowComparison(const Value& v) const;

    /**
     * Returns -1, 0, 1, depends whether this value is less than, equal to, or
     * greater than v.
     */
    int compare(const Value& v) const;

    /**
     * Returns true if this value is equal to v.
     */
    bool equal(const Value& v) const;

    /**
     * Returns true if this value is less than v.
     */
    bool less(const Value& v) const;

    /**
     * Returns true if this value is greater than v.
     */
    bool greater(const Value& v) const;

    // comparison operator - returns true only if strictly identical, unlike equal()/compare()
    bool operator==(const Value& v) const;
    inline bool operator!=(const Value& other) const {
        return !operator==(other);
    }

    static int compare(Number v1, Number v2);

    bool isZero() const;

    static bool isZero(Number v);

private:
    class Private;
    QSharedDataPointer<Private> d;
};

/***************************************************************************
  QHash/QSet support
****************************************************************************/

uint qHash(const Value& value);

} // namespace KSpread

Q_DECLARE_METATYPE(KSpread::Value)
Q_DECLARE_TYPEINFO(KSpread::Value, Q_MOVABLE_TYPE);


/***************************************************************************
  QTextStream support
****************************************************************************/

KSPREAD_EXPORT QTextStream& operator<<(QTextStream& ts, KSpread::Value::Type type);
KSPREAD_EXPORT QTextStream& operator<<(QTextStream& ts, KSpread::Value value);

/***************************************************************************
  kDebug support
****************************************************************************/

KSPREAD_EXPORT kdbgstream operator<<(kdbgstream str, const KSpread::Value& v);
kdbgstream operator<<(kdbgstream stream, const KSpread::Value::Format& f);

#endif // KSPREAD_VALUE_H
