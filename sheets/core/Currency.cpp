/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000-2006 The KSpread Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Locale
#include "Currency.h"

#include <KLocalizedString>
#include <QLocale>

using namespace Calligra::Sheets;

QMap<QString, QString> Currency::m_symbols = QMap<QString, QString>();

Currency::Currency(QString const &code, Format format)
    : m_code(code)
{
    loadSymbols();

    if (format == Gnumeric) {
        // I use QChar(c,r) here so that this file can be opened in any encoding...
        if (code.indexOf(QChar(172, 32)) != -1) // Euro sign
            m_code = QChar(172, 32);
        else if (code.indexOf(QChar(163, 0)) != -1) // Pound sign
            m_code = QChar(163, 0);
        else if (code.indexOf(QChar(165, 0)) != -1) // Yen sign
            m_code = QChar(165, 0);
        else if (code[0] == '[' && code[1] == '$') {
            int n = code.indexOf(']');
            if (n != -1)
                m_code = code.mid(2, n - 2);
        } else if (code.indexOf('$') != -1)
            m_code = '$';
    } // end gnumeric

    m_code = symbolToCode(m_code); // ensure that we have the code (e.g. USD) and not the code (e.g. $)
}

Currency::~Currency()
{
}

void Currency::loadSymbols()
{
    if (!m_symbols.isEmpty())
        return; // already loaded - do nothing

    // Just grab the currencies and their symbols, no country/territory specifics, at least for now.
    QList<QLocale> allLocales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);
    for (auto l : allLocales) {
        QString code = l.currencySymbol(QLocale::CurrencyIsoCode);
        if (!code.size())
            continue;
        QString symbol = l.currencySymbol();
        if (!m_symbols.count(code))
            m_symbols[code] = symbol;
    }
}

QList<QString> Currency::symbols()
{
    loadSymbols();
    return m_symbols.keys();
}

QString Currency::symbolToCode(const QString &symbol)
{
    if (!symbol.size())
        return QString();
    if (symbol == "$")
        return "USD"; // simple and common case
    QMap<QString, QString>::iterator it;
    for (it = m_symbols.begin(); it != m_symbols.end(); ++it) {
        QString key = it.key();
        if (key == symbol)
            return key;
        if (it.key() == symbol)
            return key;
    }
    return symbol; // not found, let's just use the symbol as-is
}

bool Currency::operator==(Currency const &cur) const
{
    if (m_code != cur.m_code)
        return false;
    return true;
}

QString Currency::code(Format format) const
{
    if (format == Gnumeric) {
        if (m_code.length() == 1) // symbol
            return m_code;
        return QString("[$" + m_code + ']');
    }
    return m_code;
}

QString Currency::name() const
{
    return m_code;
}

QString Currency::symbol() const
{
    return m_symbols.count(m_code) ? m_symbols[m_code] : m_code;
}

size_t Calligra::Sheets::qHash(const Calligra::Sheets::Currency &cur, size_t seed)
{
    return ::qHash(cur.symbol(), seed);
}
