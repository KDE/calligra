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

private:
    QString m_currencySymbol;
    int m_decimalPlaces;
};

}  //KPlato namespace

#endif
