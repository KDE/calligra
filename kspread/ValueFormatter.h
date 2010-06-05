/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
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

#ifndef KSPREAD_VALUE_FORMATTER
#define KSPREAD_VALUE_FORMATTER

#include <QDateTime>

#include "Global.h"
#include "Number.h"
#include "Style.h"

namespace KSpread
{
class CalculationSettings;
class Value;
class ValueConverter;

/**
 * \ingroup Value
 * Generates a textual representation of a Value with a given formatting.
 */
class KSPREAD_TEST_EXPORT ValueFormatter
{
public:
    /**
     * Constructor.
     */
    explicit ValueFormatter(const ValueConverter* converter);

    /**
     * Returns the calculation settings this ValueFormatter uses.
     */
    const CalculationSettings* settings() const;

    /**
     * Creates a textual representation of \p value with the explicit given
     * formattings.
     * \param formatType the value format, e.g. number, date
     * \param precision the number of decimals
     * \param floatFormat the number format, i.e. signed/unsigned information
     * \param prefix the preceding text
     * \param postfix the subsequent text
     * \param currencySymbol the currency symbol
     */
    Value formatText(const Value& value,
                     Format::Type formatType, int precision = -1,
                     Style::FloatFormat floatFormat = Style::OnlyNegSigned,
                     const QString& prefix = QString(),
                     const QString& postfix = QString(),
                     const QString& currencySymbol = QString(), const QString& formatString = QString());

    /**
     * Creates a date format.
     * \param formatType the value format, e.g. number, date
     */
    QString dateFormat(const QDate& date, Format::Type formatType, const QString& formatString = QString() );

    /**
     * Creates a time format.
     * \param formatType the value format, e.g. number, date
     */
    QString timeFormat(const QDateTime& time, Format::Type formatType, const QString& formatString = QString() );

    /**
     * Creates a date and time format.
     * \param formatType the value format, e.g. number, date
     */
    QString dateTimeFormat(const QDateTime& time, Format::Type formatType, const QString& formatString = QString() );

protected:
    /**
     * Determines the formatting type that should be used to format this value
     * in a cell with a given format type
     * \param formatType the value format, e.g. number, date
     */
    Format::Type determineFormatting(const Value& value, Format::Type formatType);

    /**
     * Creates a number format.
     * \param precision the number of decimals
     * \param formatType the value format, e.g. number, date
     * \param floatFormat the number format, i.e. signed/unsigned information
     * \param currencySymbol the currency symbol
     */
    QString createNumberFormat(Number value, int precision,
                               Format::Type formatType,
                               Style::FloatFormat floatFormat,
                               const QString& currencySymbol,
                               const QString& formatString);

    /**
     * Creates a fraction format.
     * \param formatType the value format, e.g. number, date
     */
    QString fractionFormat(Number value, Format::Type formatType);

    /**
     * Creates a complex number format.
     * \param precision the number of decimals
     * \param formatType the value format, e.g. number, date
     * \param floatFormat the number format, i.e. signed/unsigned information
     * \param currencySymbol the currency symbol
     */
    QString complexFormat(const Value& value, int precision,
                          Format::Type formatType,
                          Style::FloatFormat floatFormat,
                          const QString& currencySymbol);

    /**
     * Removes the trailing zeros and the decimal symbol \p decimalSymbol in
     * \p string , if necessary.
     * \return the truncated string
     */
    QString removeTrailingZeros(const QString& string, const QString& decimalSymbol);

private:
    const ValueConverter* m_converter;
};

}  //namespace KSpread

#endif  //KSPREAD_VALUE_FORMATTER
