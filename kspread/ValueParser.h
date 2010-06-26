/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2004 Tomas Mecir <mecirt@gmail.com>
   Copyright 1998,1999 Torben Weis <weis@kde.org>

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

#ifndef KSPREAD_VALUE_PARSER
#define KSPREAD_VALUE_PARSER

#include <QDateTime>

#include "Format.h"
#include "Number.h"

#include "kspread_export.h"

namespace KSpread
{
class CalculationSettings;
class Value;

/**
 * \ingroup Value
 * Generates a Value by parsing an user input text.
 * Determines the most probable Value type, e.g. integer or date.
 */
class KSPREAD_EXPORT ValueParser
{
public:
    /**
     * Constructor.
     */
    explicit ValueParser(const CalculationSettings* settings);

    /**
     * Returns the calculation settings this ValueFormatter uses.
     */
    const CalculationSettings* settings() const;

    /**
     * Parses the user input text \p str and tries to determine the correct
     * value type for it.
     */
    Value parse(const QString& str) const;

    /**
     * Tries for boolean type. If \p str can be interpreted as this
     * type, \p ok is set to \c true and the corresponding value will
     * be returned.
     */
    Value tryParseBool(const QString& str, bool *ok = 0) const;

    /**
     * Tries for floating point, integer, complex (and percentage) type.
     * If \p str can be interpreted as one of these types, \p ok is set to
     * \c true and the corresponding value will be returned.
     */
    Value tryParseNumber(const QString& str, bool *ok = 0) const;

    /**
     * Tries for date type. If \p str can be interpreted as this
     * type, \p ok is set to \c true and the corresponding value will
     * be returned.
     */
    Value tryParseDate(const QString& str, bool *ok = 0) const;

    /**
     * Tries for time type. If \p str can be interpreted as this
     * type, \p ok is set to \c true and the corresponding value will
     * be returned.
     */
    Value tryParseTime(const QString& str, bool *ok = 0) const;

protected:
    /**
     * Converts \p str to a date/time value.
     */
    QDateTime readTime(const QString& str, bool withSeconds, bool *ok) const;

    /**
     * A helper function to read numbers and distinguish integers and FPs.
     */
    Value readNumber(const QString &_str, bool* ok) const;

    /**
     * A helper function to read the imaginary part of a complex number.
     */
    Number readImaginary(const QString& str, bool* ok) const;

    /**
     * A helper function to read integers.
     * Used in the parsing process for date and time values.
     */
    int readInt(const QString& str, uint& pos) const;

private:
    const CalculationSettings* m_settings;
};

}  //namespace KSpread

#endif  //KSPREAD_VALUE_PARSER
