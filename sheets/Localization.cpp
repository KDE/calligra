/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Localization.h"

#include <QDomDocument>

using namespace Calligra::Sheets;

Localization::Localization()
        : KLocale()
{
}

void Localization::load(const KoXmlElement& element)
{
    if (element.hasAttribute("weekStartsMonday")) {
        QString c = element.attribute("weekStartsMonday");
        if (c != "False") {
            setWeekStartDay(1 /*Monday*/);
        }
    }
    if (element.hasAttribute("decimalSymbol"))
        setDecimalSymbol(element.attribute("decimalSymbol"));
    if (element.hasAttribute("thousandsSeparator"))
        setThousandsSeparator(element.attribute("thousandsSeparator"));
    if (element.hasAttribute("currencySymbol"))
        setCurrencySymbol(element.attribute("currencySymbol"));
    if (element.hasAttribute("monetaryDecimalSymbol"))
        setMonetaryDecimalSymbol(element.attribute("monetaryDecimalSymbol"));
    if (element.hasAttribute("monetaryThousandsSeparator"))
        setMonetaryThousandsSeparator(element.attribute("monetaryThousandsSeparator"));
    if (element.hasAttribute("positiveSign"))
        setPositiveSign(element.attribute("positiveSign"));
    if (element.hasAttribute("negativeSign"))
        setNegativeSign(element.attribute("negativeSign"));
    if (element.hasAttribute("fracDigits"))
        setMonetaryDecimalPlaces(element.attribute("fracDigits").toInt());
    if (element.hasAttribute("positivePrefixCurrencySymbol")) {
        QString c = element.attribute("positivePrefixCurrencySymbol");
        setPositivePrefixCurrencySymbol(c == "True");
    }
    if (element.hasAttribute("negativePrefixCurrencySymbol")) {
        QString c = element.attribute("negativePrefixCurrencySymbol");
        setNegativePrefixCurrencySymbol(c == "True");
    }
    if (element.hasAttribute("positiveMonetarySignPosition"))
        setPositiveMonetarySignPosition((SignPosition)element.attribute("positiveMonetarySignPosition").toInt());
    if (element.hasAttribute("negativeMonetarySignPosition"))
        setNegativeMonetarySignPosition((SignPosition)element.attribute("negativeMonetarySignPosition").toInt());
    if (element.hasAttribute("timeFormat"))
        setTimeFormat(element.attribute("timeFormat"));
    if (element.hasAttribute("dateFormat"))
        setDateFormat(element.attribute("dateFormat"));
    if (element.hasAttribute("dateFormatShort"))
        setDateFormatShort(element.attribute("dateFormatShort"));
}

QDomElement Localization::save(QDomDocument& doc) const
{
    QDomElement element = doc.createElement("locale");

    element.setAttribute("weekStartsMonday", (weekStartDay() == 1) ? "True" : "False");
    element.setAttribute("decimalSymbol", decimalSymbol());
    element.setAttribute("thousandsSeparator", thousandsSeparator());
    element.setAttribute("currencySymbol", currencySymbol());
    element.setAttribute("monetaryDecimalSymbol", monetaryDecimalSymbol());
    element.setAttribute("monetaryThousandsSeparator", monetaryThousandsSeparator());
    element.setAttribute("positiveSign", positiveSign());
    element.setAttribute("negativeSign", negativeSign());
    element.setAttribute("fracDigits", QString::number(monetaryDecimalPlaces()));
    element.setAttribute("positivePrefixCurrencySymbol", positivePrefixCurrencySymbol() ? "True" : "False");
    element.setAttribute("negativePrefixCurrencySymbol", negativePrefixCurrencySymbol() ? "True" : "False");
    element.setAttribute("positiveMonetarySignPosition", QString::number((int)positiveMonetarySignPosition()));
    element.setAttribute("negativeMonetarySignPosition", QString::number((int)negativeMonetarySignPosition()));
    element.setAttribute("timeFormat", timeFormat());
    element.setAttribute("dateFormat", dateFormat());
    element.setAttribute("dateFormatShort", dateFormatShort());

    return element;
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

