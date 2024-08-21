/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2004 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_VALUE_PARSER
#define CALLIGRA_SHEETS_VALUE_PARSER

#include <QDateTime>

#include "Number.h"
#include "sheets_engine_export.h"

namespace Calligra
{
namespace Sheets
{
class CalculationSettings;
class Value;

/**
 * \ingroup Value
 * Generates a Value by parsing a user input text.
 * Determines the most probable Value type, e.g. integer or date.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT ValueParser
{
public:
    /**
     * Constructor.
     */
    explicit ValueParser(const CalculationSettings *settings);

    /**
     * Returns the calculation settings this ValueFormatter uses.
     */
    const CalculationSettings *settings() const;

    /**
     * Parses the user input text \p str and tries to determine the correct
     * value type for it.
     */
    Value parse(const QString &str) const;

    /**
     * Tries for boolean type. If \p str can be interpreted as this
     * type, \p ok is set to \c true and the corresponding value will
     * be returned.
     */
    Value tryParseBool(const QString &str, bool *ok = nullptr) const;

    /**
     * Tries for floating point, integer, complex (and percentage) type.
     * If \p str can be interpreted as one of these types, \p ok is set to
     * \c true and the corresponding value will be returned.
     */
    Value tryParseNumber(const QString &str, bool *ok = nullptr) const;

    /**
     * Tries for datetime type. If \p str can be interpreted as this
     * type, \p ok is set to \c true and the corresponding value will
     * be returned.
     */
    Value tryParseDateTime(const QString &str, bool *ok = nullptr) const;

    /**
     * Tries for date type. If \p str can be interpreted as this
     * type, \p ok is set to \c true and the corresponding value will
     * be returned.
     */
    Value tryParseDate(const QString &str, bool *ok = nullptr) const;

    /**
     * Tries for time type. If \p str can be interpreted as this
     * type, \p ok is set to \c true and the corresponding value will
     * be returned.
     */
    Value tryParseTime(const QString &str, bool *ok = nullptr) const;

protected:
    /**
     * A helper function to read numbers and distinguish integers and FPs.
     */
    Value readNumber(const QString &_str, bool *ok) const;

    /**
     * A helper function to read the imaginary part of a complex number.
     */
    Number readImaginary(const QString &str, bool *ok) const;

    /**
     * A helper function to handle two-digit years.
     */
    int repairYear(int year, const QString &str) const;

    /**
     * A helper function to speed up date/time parsing.
     */
    bool containsDateTimeSeparator(const QString &str) const;

private:
    const CalculationSettings *m_settings;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_VALUE_PARSER
