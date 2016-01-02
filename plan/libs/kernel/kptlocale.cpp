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

#include "kptlocale.h"

#include <klocale.h>

#include <QLocale>


namespace KPlato
{

Locale::Locale()
{
    QLocale locale;
    klocale = new KLocale( QLocale::languageToString(locale.language()), QLocale::countryToString(locale.country()) );
}

Locale::~Locale()
{
    delete klocale;
}

void Locale::setPositiveMonetarySignPosition(SignPosition signpos)
{
    klocale->setPositiveMonetarySignPosition((KLocale::SignPosition)signpos);
}

void Locale::setNegativeMonetarySignPosition(SignPosition signpos)
{
    klocale->setNegativeMonetarySignPosition((KLocale::SignPosition)signpos);
}

Locale::SignPosition Locale::positiveMonetarySignPosition() const
{
    return (SignPosition)klocale->positiveMonetarySignPosition();
}

Locale::SignPosition Locale::negativeMonetarySignPosition() const
{
    return (SignPosition)klocale->negativeMonetarySignPosition();
}

void Locale::setPositivePrefixCurrencySymbol(bool prefix)
{
    klocale->setPositivePrefixCurrencySymbol(prefix);
}

void Locale::setNegativePrefixCurrencySymbol(bool prefix)
{
    klocale->setNegativePrefixCurrencySymbol(prefix);
}

bool Locale::positivePrefixCurrencySymbol() const
{
    return klocale->positivePrefixCurrencySymbol();
}

bool Locale::negativePrefixCurrencySymbol() const
{
    return klocale->negativePrefixCurrencySymbol();
}

void Locale::setCurrencySymbol(const QString &symbol)
{
    klocale->setCurrencySymbol(symbol);
}

QString Locale::currencySymbol() const
{
    return klocale->currencySymbol();
}

void Locale::setMonetaryDecimalPlaces(int digits)
{
    klocale->setMonetaryDecimalPlaces(digits);
}

int Locale::monetaryDecimalPlaces() const
{
    return klocale->monetaryDecimalPlaces();
}

QString Locale::monetaryThousandsSeparator() const
{
    return klocale->monetaryThousandsSeparator();
}

QString Locale::monetaryDecimalSymbol() const
{
    return klocale->monetaryDecimalSymbol();
}

QString Locale::formatMoney(double num, const QString &currency, int precision) const
{
    return klocale->formatMoney(num, currency, precision);
}

double Locale::readMoney(const QString &numStr, bool *ok) const
{
    return klocale->readMoney(numStr, ok);
}

QString Locale::country() const
{
    return klocale->country();
}

}  //KPlato namespace
