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
    setDefaultLocale();
}

void Localization::setDefaultLocale()
{
    setLocale(QLocale::system());
}

void Localization::setLanguage(const QString &name)
{
    QLocale locale(name);
    setLocale(locale);
}

void Localization::setLanguage(QLocale::Language language, QLocale::Script script, QLocale::Country country)
{
    QLocale locale(language, script, country);
    setLocale(locale);
}

QString Localization::name() const
{
    return locale.name();
}


QString Localization::decimalSymbol() const
{
    return locale.decimalPoint();
}

QString Localization::negativeSign() const
{
    return locale.negativeSign();
}

QString Localization::positiveSign() const
{
    return locale.positiveSign();
}

QString Localization::thousandsSeparator() const
{
    return locale.groupSeparator();
}


double Localization::readNumber(const QString &str, bool *ok) const
{
    return locale.toDouble(str, ok);
}

QDateTime Localization::readDateTime(const QString &str, bool *ok) const
{
    if (ok) *ok = false;
    QDateTime res = locale.toDateTime(str, QLocale::LongFormat);
    if (!res.isValid()) res = locale.toDateTime(str, QLocale::ShortFormat);

    // let's try without AM/PM or its equivalent
    if (!res.isValid()) {
        QString format = dateTimeFormat(true).replace("AP", "").replace("t", "").trimmed();
        res = locale.toDateTime(str, format);
    }
    if (!res.isValid()) {
        QString format = dateTimeFormat(false).replace("AP", "").replace("t", "").trimmed();
        res = locale.toDateTime(str, format);
    }

    // long/short year?
    if (!res.isValid()) {
        QString format = dateTimeFormat(true).replace("yy", "yyyy").replace("AP", "").replace("t", "").trimmed();
        res = locale.toDateTime(str, format);
    }
    if (!res.isValid()) {
        QString format = dateTimeFormat(false).replace("yy", "yyyy").replace("AP", "").replace("t", "").trimmed();
        res = locale.toDateTime(str, format);
    }
    if (!res.isValid()) {
        QString format = dateTimeFormat(true).replace("yyyy", "yy").replace("AP", "").replace("t", "").trimmed();
        res = locale.toDateTime(str, format);
    }
    if (!res.isValid()) {
        QString format = dateTimeFormat(false).replace("yyyy", "yy").replace("AP", "").replace("t", "").trimmed();
        res = locale.toDateTime(str, format);
    }


    if (res.isValid()) *ok = true;
    return res;
}

QDateTime Localization::readDateTime(const QString &str, const QString &format, bool *ok) const
{
    if (ok) *ok = false;
    QDateTime res = locale.toDateTime(str, format);
    if (res.isValid()) *ok = true;
    return res;
}

QDate Localization::readDate(const QString &str, bool *ok) const
{
    if (ok) *ok = false;
    QDate res = locale.toDate(str, QLocale::LongFormat);
    if (!res.isValid()) res = locale.toDate(str, QLocale::ShortFormat);

    // long/short year?
    if (!res.isValid()) {
        QString format = dateFormat(true).replace("yy", "yyyy").trimmed();
        res = locale.toDate(str, format);
    }
    if (!res.isValid()) {
        QString format = dateFormat(false).replace("yy", "yyyy").trimmed();
        res = locale.toDate(str, format);
    }
    if (!res.isValid()) {
        QString format = dateFormat(true).replace("yyyy", "yy").trimmed();
        res = locale.toDate(str, format);
    }
    if (!res.isValid()) {
        QString format = dateFormat(false).replace("yyyy", "yy").trimmed();
        res = locale.toDate(str, format);
    }

    if (res.isValid()) *ok = true;
    return res;
}

QDate Localization::readDate(const QString &str, const QString &format, bool *ok) const
{
    if (ok) *ok = false;
    QDate res = locale.toDate(str, format);
    if (res.isValid()) *ok = true;
    return res;
}

QTime Localization::readTime(const QString &str, bool *ok) const
{
    if (ok) *ok = false;
    QTime res = locale.toTime(str, QLocale::LongFormat);
    if (!res.isValid()) res = locale.toTime(str, QLocale::ShortFormat);
    
    // let's try without AM/PM or its equivalent, also without timezones
    if (!res.isValid()) {
        QString format = timeFormat(true).replace("AP", "").replace("t", "").trimmed();
        res = locale.toTime(str, format);
    }
    if (!res.isValid()) {
        QString format = timeFormat(false).replace("AP", "").replace("t", "").trimmed();
        res = locale.toTime(str, format);
    }
    if (res.isValid()) *ok = true;
    return res;
}

QTime Localization::readTime(const QString &str, const QString &format, bool *ok) const
{
    if (ok) *ok = false;
    QTime res = locale.toTime(str, format);
    if (res.isValid()) *ok = true;
    return res;
}

QString Localization::dateTimeFormat(bool longFormat) const
{
    return locale.dateTimeFormat(longFormat ? QLocale::LongFormat : QLocale::ShortFormat);
}

QString Localization::dateFormat(bool longFormat) const
{
    return locale.dateFormat(longFormat ? QLocale::LongFormat : QLocale::ShortFormat);
}

QString Localization::timeFormat(bool longFormat) const
{
    return locale.timeFormat(longFormat ? QLocale::LongFormat : QLocale::ShortFormat);
}

QString Localization::dateFormat(int type) const
{
    if (type == 1) return "d" + dateSepShort + "M" + dateSepShort + "yyyy";
    if (type == 2) return "d" + dateSepShort + "M" + dateSepShort + "yy";
    if (type == 3) return "d" + dateSepShort + "MMM" + dateSepShort + "yy";
    if (type == 4) return "d" + dateSepShort + "MMM" + dateSepShort + "yyyy";
    if (type == 5) return "d" + dateSepLong + "MMMM" + dateSepLong + "yy";
    if (type == 6) return "d" + dateSepLong + "MMMM" + dateSepLong + "yyyy";
    if (type == 7) return "MMMM" + dateSepLong + "d" + dateSepLong + "yy";
    if (type == 8) return "MMMM" + dateSepLong + "d" + dateSepLong + "yyyy";

    return QString();
}

QString Localization::currencySymbol() const
{
    return locale.currencySymbol();
}

int Localization::firstDayOfWeek() const
{
    return (int) locale.firstDayOfWeek();
}

QString Localization::dayName(int day) const
{
    return locale.dayName(day);
}

QString Localization::monthName(int month) const
{
    return locale.monthName(month);
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

// TODO - we should let the caller specify whether to use scientific format or not
QString Localization::formatNumber(double num, int precision) const
{
    return locale.toString(num, 'g', precision);
}

QString Localization::formatCurrency(double num, const QString &currencySymbol, int precision) const
{
    return locale.toCurrencyString(num, currencySymbol, precision);
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


QString Localization::formatDateTime(const QDateTime &datetime, bool longFormat) const
{
    return locale.toString(datetime, longFormat ? QLocale::LongFormat : QLocale::ShortFormat);
}

QString Localization::formatDateTime(const QDateTime &datetime, const QString &format) const
{
    return locale.toString(datetime, format);
}

QString Localization::formatDate(const QDate &date, bool longFormat) const
{
    return locale.toString(date, longFormat ? QLocale::LongFormat : QLocale::ShortFormat);
}

QString Localization::formatDate(const QDate &date, const QString &format) const
{
    return locale.toString(date, format);
}

QString Localization::formatTime(const QTime &time, bool longFormat) const
{
    return locale.toString(time, longFormat ? QLocale::LongFormat : QLocale::ShortFormat);
}

QString Localization::formatTime(const QTime &time, const QString &format) const
{
    return locale.toString(time, format);
}

QString Localization::languageName(bool full) const
{
    if (full) return locale.nativeLanguageName();
    return locale.name();
}


QString Localization::toUpper(const QString &str) const
{
    return locale.toUpper(str);
}

QString Localization::toLower(const QString &str) const
{
    return locale.toLower(str);
}

static QString getSeparator(const QString &str) {
    QString sep;
    int stage = 0;
    for (int i = 0; i < str.length(); ++i)
    {
        if (stage == 0) {
            if (str[i].isLetter())
                stage++;
            continue;
        }
        if (stage == 1) {
            if (str[i].isLetter()) continue;
            stage++;  // no continue here, we want to apply stage2 logic
        }
        if (stage == 2) {
            if (str[i].isLetter()) break;  // we're done
            sep += str[i];
        }
    }

    return sep;
}

void Localization::setLocale(const QLocale &l) {
    locale = l;

    // Determine the date/time separator
    QString format = timeFormat(false);
    timeSep = getSeparator(format);
    format = timeFormat(true);
    if (format.indexOf("A") >= 0) includesAMPM = true;
    else includesAMPM = false;

    format = dateFormat(true);
    dateSepLong = getSeparator(format);
    format = dateFormat(false);
    dateSepShort = getSeparator(format);
}


