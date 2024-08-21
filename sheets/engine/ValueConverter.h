/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_VALUE_CONVERTER
#define CALLIGRA_SHEETS_VALUE_CONVERTER

#include "CS_Time.h"
#include "Value.h"

#include "sheets_engine_export.h"

namespace Calligra
{
namespace Sheets
{
class CalculationSettings;
class ValueParser;

/**
 * \ingroup Value
 * Converts between the different Value types.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT ValueConverter
{
public:
    /**
     * Constructor.
     */
    explicit ValueConverter(const ValueParser *parser);

    /**
     * Returns the calculation settings this ValueFormatter uses.
     */
    const CalculationSettings *settings() const;

    /**
     * Converts \p value to a Value of boolean type.
     */
    Value asBoolean(const Value &value, bool *ok = nullptr) const;

    /**
     * Converts \p value to a Value of integer type.
     */
    Value asInteger(const Value &value, bool *ok = nullptr) const;

    /**
     * Converts \p value to a Value of floating point type.
     */
    Value asFloat(const Value &value, bool *ok = nullptr) const;

    /**
     * Converts \p value to a Value of complex number type.
     */
    Value asComplex(const Value &value, bool *ok = nullptr) const;

    /**
     * Converts \p value to a Value of number type, i.e. Values of integer and
     * complex number type stay as they are; all others are converted to the
     * floating point type.
     */
    Value asNumeric(const Value &value, bool *ok = nullptr) const;

    /**
     * Converts \p value to a Value of string type.
     */
    Value asString(const Value &value) const;

    /**
     * Converts \p value to a Value of date/time type.
     */
    Value asDateTime(const Value &value, bool *ok = nullptr) const;

    /**
     * Converts \p value to a Value of date type.
     */
    Value asDate(const Value &value, bool *ok = nullptr) const;

    /**
     * Converts \p value to a Value of time type.
     */
    Value asTime(const Value &value, bool *ok = nullptr) const;

    bool toBoolean(const Value &value) const;
    int toInteger(const Value &value) const;
    Number toFloat(const Value &value) const;
    complex<Number> toComplex(const Value &value) const;
    QString toString(const Value &value) const;
    QDateTime toDateTime(const Value &value) const;
    QDate toDate(const Value &value) const;
    Time toTime(const Value &value) const;

private:
    const ValueParser *m_parser;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_VALUE_CONVERTER
