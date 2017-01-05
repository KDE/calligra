/* This file is part of the KDE project
  Copyright (C) 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
  Copyright (C) 2016 Dag Andersen <danders@get2net.dk>
  
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

#include "kptdebug.h"

#include <QLocale>


namespace KPlato
{

Locale::Locale()
{
    QLocale locale;
    m_language = locale.language();
    m_country = locale.country();
    m_decimalPlaces = 2;
}

Locale::~Locale()
{
}

void Locale::setCurrencyLocale(QLocale::Language language, QLocale::Country country)
{
    m_language = language;
    m_country = country;
}

void Locale::setCurrencySymbol(const QString &symbol)
{
    m_currencySymbol = symbol;
}

QString Locale::currencySymbol() const
{
    QString s = m_currencySymbol;
    if (s.isEmpty()) {
        QLocale locale(m_language, m_country);
        s = locale.currencySymbol(QLocale::CurrencySymbol);
    }
    return s;
}

void Locale::setMonetaryDecimalPlaces(int digits)
{
    m_decimalPlaces = digits;
}

int Locale::monetaryDecimalPlaces() const
{
    return m_decimalPlaces;
}

QString Locale::formatMoney(double num, const QString &currency, int precision) const
{
    QString c = currency;
    if (c.isEmpty()) {
        c = currencySymbol();
    }
    int p = precision;
    if (p < 0) {
        p = m_decimalPlaces;
    }
    QLocale l;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    QString s = l.toCurrencyString(num, c, p);
#else
    QString s = l.toCurrencyString(num, c);
#endif
    return s;
}

double Locale::readMoney(const QString &numStr, bool *ok) const
{
    QLocale l;
    QString s = numStr;
    bool okk = false;
    s.remove(m_currencySymbol);
    double v = l.toDouble(s, &okk);
    if (ok) {
        *ok = okk;
    }
    if (!okk) {
        errorPlan<<"Failed to read money:"<<numStr;
    }
    return v;
}

QString Locale::currencySymbolExplicit() const
{
    return m_currencySymbol;
}

QLocale::Language Locale::currencyLanguage() const
{
    return m_language;
}

QLocale::Country Locale::currencyCountry() const
{
    return m_country;
}

}  //KPlato namespace
