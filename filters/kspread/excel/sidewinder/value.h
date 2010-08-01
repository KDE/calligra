/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003 Ariya Hidayat <ariya@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA
*/

#ifndef SWINDER_VALUE_H
#define SWINDER_VALUE_H

#include <iostream>
#include <map>
#include "format.h"

namespace Swinder
{

class ValueData;



/**
 * Provides a wrapper for cell value.
 *
 * Each cell in a worksheet must hold a value, either as enterred by user
 * or as a result of formula evaluation. Default cell holds empty value.
 *
 * Value uses implicit data sharing to reduce memory usage.
 */

class Value
{

public:

    typedef enum {
        Empty,
        Boolean,
        Integer,
        Float,
        String,
        RichText,
        CellRange, // not used yet
        Array,     // not used yet
        Error
    } Type;

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
    
    bool operator==(const Value &other) const;
    bool operator!=(const Value &other) const;

    /**
     * Assigns from another value. Same as above.
     */
    Value& assign(const Value& _value);

    /**
     * Creates a boolean value.
     */
    explicit Value(bool b);

    /**
     * Creates an integer value.
     */
    explicit Value(int i);

    /**
     * Create a floating-point value.
     */
    explicit Value(double f);

    /**
     * Create a string value.
     */
    explicit Value(const QString& s);

    /**
     * Create a richtext value
     */
    Value(const QString& s, const std::map<unsigned, FormatFont>& formatRuns);

    /**
     * Returns the type of the value.
     */
    Type type() const;

    /**
     * Returns true if empty.
     */
    bool isEmpty() const {
        return type() == Empty;
    }

    /**
     * Returns true if the type of this value is Boolean.
     */
    bool isBoolean() const {
        return type() == Boolean;
    }

    /**
     * Returns true if the type of this value is integer.
     */
    bool isInteger() const {
        return type() == Integer;
    }

    /**
     * Returns true if the type of this value is floating-point.
     */
    bool isFloat() const {
        return type() == Float;
    }

    /**
     * Returns true if the type of this value is either
     * integer or floating-point.
     */
    bool isNumber() const {
        return (type() == Integer) || (type() == Float);
    }

    /**
     * Returns true if the type of this value is string.
     */
    bool isString() const {
        return type() == String;
    }

    /**
     * Returns true if the type of this value is richtext.
     */
    bool isRichText() const {
        return type() == RichText;
    }

    /**
     * Returns true if the type of this value is string or richtext.
     */
    bool isText() const {
        return type() == String || type() == RichText;
    }

    /**
     * Returns true if this value holds error information.
     */
    bool isError() const {
        return type() == Error;
    }

    void setValue(const Value& v);

    /**
     * Sets this value to boolean value.
     */
    void setValue(bool b);

    /**
     * Sets this value to integer value.
     */
    void setValue(int i);

    /**
     * Sets this value to floating-point value.
     */
    void setValue(double f);

    /**
     * Sets this value to string value.
     */
    void setValue(const QString& s);

    /**
     * Sets this value to richtext value.
     */
    void setValue(const QString& s, const std::map<unsigned, FormatFont>& formatRuns);

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
    int asInteger() const;

    /**
     * Returns the floating-point value of this value.
     *
     * Call this function only if isNumber() returns true.
     */
    double asFloat() const;

    /**
     * Returns the string value of this value.
     *
     * Call this function only if isText() returns true.
     */
    QString asString() const;

    /**
     * Returns the format runs of this value.
     *
     * Call this function only if isRichText() returns true.
     */
    std::map<unsigned, FormatFont> formatRuns() const;

    /**
     * Returns error message associated with this value.
     *
     * Call this function only if isError() returns true.
     */
    QString errorMessage() const;

    /**
     * Detaches itself from shared value data, i.e make a private, deep copy
     * of the data. Usually this function is called automatically so you
     * don't have to care about it.
     */
    void detach();

    /**
     * Returns constant reference to empty value.
     */
    static const Value& empty();

    /**
     * Returns constant reference to \#DIV/0! error.
     *
     * This is used to indicate that a formula divides by 0 (zero).
     */
    static const Value& errorDIV0();

    /**
     * Returns constant reference to \#N/A error.
     *
     * This is to indicate that  a value is not available to a function.
     */
    static const Value& errorNA();

    /**
     * Returns constant reference to \#NAME? error.
     *
     * This is to indicate that certain text inside formula is not
     * recognized, possibly a misspelled name or name that
     * does not exist.
     */
    static const Value& errorNAME();

    /**
     * Returns constant reference to \#NUM! error.
     *
     * This is to indicate a problem with a number in a formula.
     */
    static const Value& errorNUM();

    /**
     * Returns constant reference to \#NULL! error.
     *
     * This is to indicate that two area do not intersect.
     */
    static const Value& errorNULL();

    /**
     * Returns constant reference to \#REF! error.
     *
     * This is used to indicate an invalid cell reference.
     */
    static const Value& errorREF();

    /**
     * Returns constant reference to \#VALUE! error.
     *
     * This is to indicate that wrong type of argument or operand
     * is used, usually within a function call, e.g SIN("some text").
     */
    static const Value& errorVALUE();

protected:

    ValueData* d; // can't never be 0
};

/**
 * Dumps value to output stream, useful for debugging.
 */
std::ostream& operator<<(std::ostream& s, Value value);

} // namespace Swinder

#endif // SWINDER_VALUE_H
