/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Localization.h"

#include <QDateTime>

using namespace Calligra::Sheets;

Localization::Localization()
{
    locale = QLocale::system();
}

void Localization::defaultSystemConfig()
{
    locale = QLocale::system();
}

QString Localization::decimalSymbol() const
{
    return locale.decimalPoint();
}

QString Localization::negativeSign() const
{
    return locale.negativeSign();
}

QString Localization::thousandsSeparator() const
{
    return locale.groupSeparator();
}

QDateTime Localization::readDateTime(const QString &str, bool *ok)
{
    if (ok) *ok = false;
    QDateTime res = locale.toDateTime(str, QLocale::LongFormat);
    if (!res.isValid()) res = locale.toDateTime(str, QLocale::ShortFormat);
    if (res.isValid()) *ok = true;
    return res;
}

QDateTime Localization::readDateTime(const QString &str, const QString &format, bool *ok)
{
    if (ok) *ok = false;
    QDateTime res = locale.toDateTime(str, format);
    if (res.isValid()) *ok = true;
    return res;
}

QDate Localization::readDate(const QString &str, bool *ok)
{
    if (ok) *ok = false;
    QDate res = locale.toDate(str, QLocale::LongFormat);
    if (!res.isValid()) res = locale.toDate(str, QLocale::ShortFormat);
    if (res.isValid()) *ok = true;
    return res;
}

QDate Localization::readDate(const QString &str, const QString &format, bool *ok)
{
    if (ok) *ok = false;
    QDate res = locale.toDate(str, format);
    if (res.isValid()) *ok = true;
    return res;
}

QTime Localization::readTime(const QString &str, bool *ok)
{
    if (ok) *ok = false;
    QTime res = locale.toTime(str, QLocale::LongFormat);
    if (!res.isValid()) res = locale.toTime(str, QLocale::ShortFormat);
    if (res.isValid()) *ok = true;
    return res;
}

QTime Localization::readTime(const QString &str, const QString &format, bool *ok)
{
    if (ok) *ok = false;
    QTime res = locale.toTime(str, format);
    if (res.isValid()) *ok = true;
    return res;
}

QString Localization::dateTimeFormat(bool longFormat)
{
    return locale.dateTimeFormat(longFormat ? QLocale::LongFormat : QLocale::ShortFormat);
}

QString Localization::dateFormat(bool longFormat)
{
    return locale.dateTimeFormat(longFormat ? QLocale::LongFormat : QLocale::ShortFormat);
}

QString Localization::timeFormat(bool longFormat)
{
    return locale.dateTimeFormat(longFormat ? QLocale::LongFormat : QLocale::ShortFormat);
}


QString Localization::translateString(KLocalizedString str) const
{
    QStringList langs = locale.uiLanguages();
    return str.toString(langs);
}

QString Localization::formatBool(bool val) const
{
    KLocalizedString str = val ? ki18n("true") : ki18n("false");
    return translateString(str).toLower();
}

QString Localization::formatDoubleNoSep(double val) const
{
    QString res = QString::number(val, 'g', 10);
    int pos = res.indexOf('.');
    const QString decimalSymbol = locale.decimalPoint();
    if (!decimalSymbol.isNull() && (pos != -1))
        res.replace(pos, 1, decimalSymbol);
    return res;
}


QString Localization::formatDateTime(const QDateTime &datetime, bool longFormat)
{
    return locale.toString(datetime, longFormat ? QLocale::LongFormat : QLocale::ShortFormat);
}

QString Localization::formatDateTime(const QDateTime &datetime, const QString &format)
{
    return locale.toString(datetime, format);
}

QString Localization::formatDate(const QDate &date, bool longFormat)
{
    return locale.toString(date, longFormat ? QLocale::LongFormat : QLocale::ShortFormat);
}

QString Localization::formatDate(const QDate &date, const QString &format)
{
    return locale.toString(date, format);
}

QString Localization::formatTime(const QTime &time, bool longFormat)
{
    return locale.toString(time, longFormat ? QLocale::LongFormat : QLocale::ShortFormat);
}

QString Localization::formatTime(const QTime &time, const QString &format)
{
    return locale.toString(time, format);
}


QString Localization::toUpper(const QString &str) const
{
    return locale.toUpper(str);
}

QString Localization::toLower(const QString &str) const
{
    return locale.toLower(str);
}


