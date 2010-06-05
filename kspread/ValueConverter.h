/* This file is part of the KDE project
   Copyright 2004 Tomas Mecir <mecirt@gmail.com>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_VALUE_CONVERTER
#define KSPREAD_VALUE_CONVERTER

#include "Value.h"

namespace KSpread
{
class CalculationSettings;
class ValueParser;

/**
 * \ingroup Value
 * Converts between the different Value types.
 */
class KSPREAD_EXPORT ValueConverter
{
public:
    /**
     * Constructor.
     */
    explicit ValueConverter(const ValueParser* parser);

    /**
     * Returns the calculation settings this ValueFormatter uses.
     */
    const CalculationSettings* settings() const;

    /**
     * Converts \p value to a Value of boolean type.
     */
    Value asBoolean(const Value& value, bool *ok = 0) const;

    /**
     * Converts \p value to a Value of integer type.
     */
    Value asInteger(const Value& value, bool *ok = 0) const;

    /**
     * Converts \p value to a Value of floating point type.
     */
    Value asFloat(const Value& value, bool* ok = 0) const;

    /**
     * Converts \p value to a Value of complex number type.
     */
    Value asComplex(const Value& value, bool *ok = 0) const;

    /**
     * Converts \p value to a Value of number type, i.e. Values of integer and
     * complex number type stay as they are; all others are converted to the
     * floating point type.
     */
    Value asNumeric(const Value& value, bool *ok = 0) const;

    /**
     * Converts \p value to a Value of string type.
     */
    Value asString(const Value& value) const;

    /**
     * Converts \p value to a Value of date/time type.
     */
    Value asDateTime(const Value& value, bool *ok = 0) const;

    /**
     * Converts \p value to a Value of date type.
     */
    Value asDate(const Value& value, bool *ok = 0) const;

    /**
     * Converts \p value to a Value of time type.
     */
    Value asTime(const Value& value, bool *ok = 0) const;


    bool toBoolean(const Value& value) const;
    int toInteger(const Value& value) const;
    Number toFloat(const Value& value) const;
    complex<Number> toComplex(const Value& value) const;
    QString toString(const Value& value) const;
    QDateTime toDateTime(const Value& value) const;
    QDate toDate(const Value& value) const;
    QTime toTime(const Value& value) const;

private:
    const ValueParser* m_parser;
};

}  //KSpread namespace

#endif  //KSPREAD_VALUE_CONVERTER
