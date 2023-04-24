/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2004 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_VALUE_FORMATTER
#define CALLIGRA_SHEETS_VALUE_FORMATTER

#include <QDateTime>

#include "sheets_core_export.h"
#include "Style.h"

#include "engine/Number.h"
#include "engine/Format.h"

namespace Calligra
{
namespace Sheets
{
class CalculationSettings;
class Value;
class ValueConverter;

/**
 * \ingroup Value
 * Generates a textual representation of a Value with a given formatting.
 */
class CALLIGRA_SHEETS_CORE_EXPORT ValueFormatter
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
     * \param value the value
     * \param formatType the value format, e.g. number, date
     * \param precision the number of decimals
     * \param floatFormat the number format, i.e. signed/unsigned information
     * \param prefix the preceding text
     * \param postfix the subsequent text
     * \param currencySymbol the currency symbol
     * \param formatString the Qt format string
     * \param thousandsSep whether to use thousands separator
     */
    Value formatText(const Value& value,
                     Format::Type formatType, int precision = -1,
                     Style::FloatFormat floatFormat = Style::OnlyNegSigned,
                     const QString& prefix = QString(),
                     const QString& postfix = QString(),
                     const QString& currencySymbol = QString(),
                     const QString& formatString = QString(),
                     bool thousandsSep = true);

//NOTE: All dates must be formatted using dateTimeFormat
    /**
     * Format @p date in accordance with @p formatString.
     * If @p formatString is empty, the date is formatted using @p formatType
     */
    QString dateFormat(const QDate& date, Format::Type formatType, const QString& formatString = QString() );

    /**
     * Format @p time in accordance with @p formatString.
     * If @p formatString is empty, the time is formatted using @p formatType
     */
    QString timeFormat(const QDateTime& time, Format::Type formatType, const QString& formatString = QString() );

    /**
     * Format the dattime @p time in accordance with @p formatString.
     * If @p formatString is empty, the time is formatted using @p formatType
     */
    QString dateTimeFormat(const QDateTime& time, Format::Type formatType, const QString& formatString = QString());

    /**
     * Determines the formatting type that should be used to format this value
     * in a cell with a given format type
     * \param value the value
     * \param formatType the value format, e.g. number, date
     */
    Format::Type determineFormatting(const Value& value, Format::Type formatType);

protected:

    /**
     * Creates a number format.
     * \param value the value
     * \param precision the number of decimals
     * \param formatType the value format, e.g. number, date
     * \param floatFormat the number format, i.e. signed/unsigned information
     * \param currencySymbol the currency symbol
     * \param formatString the Qt format string
     * \param thousandsSep whether to use thousands separator
     */
    QString createNumberFormat(Number value, int precision,
                               Format::Type formatType,
                               Style::FloatFormat floatFormat,
                               const QString& currencySymbol,
                               const QString& formatString,
                               bool thousandsSep);

    /**
     * Creates a fraction format.
     * \param value the value
     * \param formatType the value format, e.g. number, date
     */
    QString fractionFormat(Number value, Format::Type formatType);

    /**
     * Creates a complex number format.
     * \param value the initial value
     * \param precision the number of decimals
     * \param formatType the value format, e.g. number, date
     * \param floatFormat the number format, i.e. signed/unsigned information
     * \param currencySymbol the currency symbol
     * \param thousandsSep whether to use thousands separator
     */
    QString complexFormat(const Value& value, int precision,
                          Format::Type formatType,
                          Style::FloatFormat floatFormat,
                          const QString& currencySymbol,
                          bool thousandsSep);

    /**
     * Removes the trailing zeros and the decimal symbol \p decimalSymbol in
     * \p string , if necessary.
     * \return the truncated string
     */
    QString removeTrailingZeros(const QString& string, const QString &currencySymbol, const QString& decimalSymbol);

private:
    const ValueConverter* m_converter;
};

} // namespace Sheets
} // namespace Calligra

#endif  //CALLIGRA_SHEETS_VALUE_FORMATTER
