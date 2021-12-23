/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Localization.h"

using namespace Calligra::Sheets;

Localization::Localization()
        : KLocale()
{
}

void Localization::defaultSystemConfig()
{
    KLocale locale("calligrasheets");
    setWeekStartDay(locale.weekStartDay());
    setDecimalSymbol(locale.decimalSymbol());
    setThousandsSeparator(locale.thousandsSeparator());
    setCurrencySymbol(locale.currencySymbol());
    setMonetaryDecimalSymbol(locale.monetaryDecimalSymbol());
    setMonetaryThousandsSeparator(locale.monetaryThousandsSeparator());
    setPositiveSign(locale.positiveSign());
    setNegativeSign(locale.negativeSign());
    setMonetaryDecimalPlaces(locale.monetaryDecimalPlaces());
    setDecimalPlaces(locale.decimalPlaces());
    setPositivePrefixCurrencySymbol(locale.positivePrefixCurrencySymbol());
    setNegativePrefixCurrencySymbol(locale.negativePrefixCurrencySymbol());
    setPositiveMonetarySignPosition(locale.positiveMonetarySignPosition());
    setNegativeMonetarySignPosition(locale.negativeMonetarySignPosition());
    setTimeFormat(locale.timeFormat());
    setDateFormat(locale.dateFormat());
    setDateFormatShort(locale.dateFormatShort());

}

