/* This file is part of the KDE project
  Copyright (C) 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version..

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#ifndef KPTLOCALE_H
#define KPTLOCALE_H

#include "kplatokernel_export.h"

#include <QString>

class KLocale;

namespace KPlato
{

/**
 * Temporary wrapper for KLocale's currency methods.
 */
class KPLATOKERNEL_EXPORT Locale
{
public:
    Locale();
    ~Locale();

public:
    /**
     * Various positions for where to place the positive or negative
     * sign when they are related to a monetary value.
     */
    enum SignPosition {
        /**
         * Put parantheses around the quantity, e.g. "$ (217)"
         */
        ParensAround = 0,
        /**
         * Prefix the quantity with the sign, e.g. "$ -217"
         */
        BeforeQuantityMoney = 1,
        /**
         * Suffix the quanitity with the sign, e.g. "$ 217-"
         */
        AfterQuantityMoney = 2,
        /**
         * Prefix the currency symbol with the sign, e.g. "-$ 217"
         */
        BeforeMoney = 3,
        /**
         * Suffix the currency symbol with the sign, e.g. "$- 217"
         */
        AfterMoney = 4
    };

    /**
     * Changes the sign position used for positive monetary values.
     *
     * @param signpos The new sign position
     */
    void setPositiveMonetarySignPosition(SignPosition signpos);

    /**
     * Changes the sign position used for negative monetary values.
     *
     * @param signpos The new sign position
     */
    void setNegativeMonetarySignPosition(SignPosition signpos);

    /**
     * Returns the position of a positive sign in relation to a
     * monetary value.
     *
     * @return Where/how to print the positive sign.
     * @see SignPosition
     */
    SignPosition positiveMonetarySignPosition() const;

    /**
     * Denotes where to place a negative sign in relation to a
     * monetary value.
     *
     * @return Where/how to print the negative sign.
     * @see SignPosition
     */
    SignPosition negativeMonetarySignPosition() const;

    /**
     * Changes the position where the currency symbol should be printed for
     * positive monetary values.
     *
     * @param prefix True if the currency symbol should be prefixed instead of
     * postfixed
     */
    void setPositivePrefixCurrencySymbol(bool prefix);

    /**
     * Changes the position where the currency symbol should be printed for
     * negative monetary values.
     *
     * @param prefix True if the currency symbol should be prefixed instead of
     * postfixed
     */
    void setNegativePrefixCurrencySymbol(bool prefix);

    /**
     * If and only if the currency symbol precedes a positive value,
     * this will be true.
     *
     * @return Where to print the currency symbol for positive numbers.
     */
    bool positivePrefixCurrencySymbol() const;

    /**
     * If and only if the currency symbol precedes a negative value,
     * this will be true.
     *
     * @return True if the currency symbol precedes negative numbers.
     */
    bool negativePrefixCurrencySymbol() const;

    /**
     * Changes the current currency symbol.
     *
     * This symbol should be consistant with the selected Currency Code
     *
     * @param symbol The new currency symbol
     * @see currencyCode, KCurrency::currencySymbols
     */
    void setCurrencySymbol(const QString &symbol);

    /**
     * Returns what the symbol denoting currency in the current locale
     * as as defined by user settings should look like.
     *
     * @return The default currency symbol used by locale.
     */
    QString currencySymbol() const;

     /**
     * @since 4.4
     *
     * Changes the number of decimal places used when formating money.
     *
     * @param digits The default number of digits to use.
     */
    void setMonetaryDecimalPlaces(int digits);

    /**
     * @since 4.4
     *
     * The number of decimal places to include in monetary values (usually 2).
     *
     * @return Default number of monetary decimal places used by locale.
     */
    int monetaryDecimalPlaces() const;

    /**
     * Returns what a thousands separator for monetary values should
     * look like ("," or " " etc.) according to the current locale or
     * user settings.
     *
     * @return The monetary thousands separator used by locale.
     */
    QString monetaryThousandsSeparator() const;

    /**
     * Returns what a decimal point should look like ("." or "," etc.)
     * for monetary values, according to the current locale or user
     * settings.
     *
     * @return The monetary decimal symbol used by locale.
     */
    QString monetaryDecimalSymbol() const;

    /**
     * Given a double, converts that to a numeric string containing
     * the localized monetary equivalent.
     *
     * e.g. given 123456, return "$ 123,456.00".
     *
     * If precision isn't specified or is < 0, then the default monetaryDecimalPlaces() is used.
     *
     * @param num The number we want to format
     * @param currency The currency symbol you want.
     * @param precision Number of decimal places displayed
     *
     * @return The number of money as a localized string
     * @see monetaryDecimalPlaces()
     */
    QString formatMoney(double num, const QString &currency = QString(), int precision = -1) const;

    /**
     * Converts a localized monetary string to a double.
     *
     * @param numStr the string we want to convert.
     * @param ok the boolean that is set to false if it's not a number.
     *           If @p ok is 0, it will be ignored
     *
     * @return The string converted to a double
     */
    double readMoney(const QString &numStr, bool *ok = 0) const;

    /**
     * Returns the country code of the country where the user lives.
     *
     * The returned code complies with the ISO 3166-1 alpha-2 standard,
     * except by KDE convention it is returned in lowercase whereas the
     * official standard is uppercase.
     * See http://en.wikipedia.org/wiki/ISO_3166-1_alpha-2 for details.
     *
     * defaultCountry() is returned by default, if no other available,
     * this will always be uppercase 'C'.
     *
     * Use countryCodeToName(country) to get human readable, localized
     * country names.
     *
     * @return the country code for the user
     *
     * @see countryCodeToName
     */
    QString country() const;

private:
    KLocale * klocale;
};

}  //KPlato namespace

#endif
