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

#include "kptdebug.h"

#include <klocale.h>

#include <QLocale>


namespace KPlato
{

Locale::Locale()
{
    QLocale locale;
    m_currencySymbol = locale.currencySymbol(QLocale::CurrencySymbol);
    m_decimalPlaces = 2;
}

Locale::~Locale()
{
}

void Locale::setCurrencySymbol(const QString &symbol)
{
    m_currencySymbol = symbol;
}

QString Locale::currencySymbol() const
{
    return m_currencySymbol;
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
        c = m_currencySymbol;
    }
    int p = precision;
    if (p < 0) {
        p = m_decimalPlaces;
    }
    QLocale l;
    QString s = l.toCurrencyString(num, c, p);
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

}  //KPlato namespace
