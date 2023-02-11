/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Localization.h"

#include <QDateTime>

#ifdef Q_OS_WIN
// HACK to get this to compile on msvc
#include <QSharedDataPointer>

namespace Calligra
{
namespace Sheets
{

class Q_DECL_HIDDEN LocalizationPrivateData : public QSharedData
 {
 public:
    QLocale locale;
    QString timeSep, dateSepShort, dateSepLong;
    QStringList dateTimeFormats, dateFormats, timeFormats;
    bool includesAMPM;
    QString trueString, falseString;
 };
class Q_DECL_HIDDEN Localization::Private
{
public:
    Private(LocalizationPrivateData *pd = nullptr) : data(pd) {}
    QSharedDataPointer<LocalizationPrivateData> data;
};
}}

using namespace Calligra::Sheets;

Localization::Localization()
    : pd(new Private(new LocalizationPrivateData))
{
    d = pd->data;
    setDefaultLocale();
}
Localization::Localization(const Localization &other)
{
    delete pd->data;
    pd->data = other.pd->data;
    d = pd->data;
}

// This must be defined or we get errors.
Localization::~Localization()
{
    delete d;
}
#else
using namespace Calligra::Sheets;

class Q_DECL_HIDDEN Localization::Private : public QSharedData
{
public:
    QLocale locale;
    QString timeSep, dateSepShort, dateSepLong;
    QStringList dateTimeFormats, dateFormats, timeFormats;
    bool includesAMPM;
    QString trueString, falseString;
};

Localization::Localization()
    : d(new Private)
{
    setDefaultLocale();
}

// This must be defined or we get errors.
Localization::~Localization()
{
}
#endif

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
    return d->locale.name();
}


QString Localization::decimalSymbol() const
{
    return d->locale.decimalPoint();
}

QString Localization::negativeSign() const
{
    return d->locale.negativeSign();
}

QString Localization::positiveSign() const
{
    return d->locale.positiveSign();
}

QString Localization::thousandsSeparator() const
{
    return d->locale.groupSeparator();
}

QString Localization::timeSeparator() const
{
    return d->timeSep;
}

bool Localization::timeWithAMPM() const
{
    return d->includesAMPM;
}

QString Localization::dateSeparator(bool longDate) const
{
    return longDate ? d->dateSepLong : d->dateSepShort;
}


double Localization::readNumber(const QString &str, bool *ok) const
{
    return d->locale.toDouble(str, ok);
}

QDateTime Localization::readDateTime(const QString &str, bool *ok) const
{
    if (ok) *ok = false;
    QDateTime res;

    // Try all the formats.
    for (QString format : d->dateTimeFormats) {
        res = readDateTime(str, format, ok);
        if (*ok) return res;
    }

    *ok = false;
    return res;
}

QDateTime Localization::readDateTime(const QString &str, const QString &format, bool *ok) const
{
    if (ok) *ok = false;
    QDateTime res = d->locale.toDateTime(str, format);
    if (res.isValid()) *ok = true;
    return res;
}

QDate Localization::readDate(const QString &str, bool *ok) const
{
    QDate res;
    if (ok) *ok = false;

    // Try all the formats.
    for (QString format : d->dateFormats) {
        res = readDate(str, format, ok);
        if (*ok) return res;
    }

    *ok = false;
    return res;
}

QDate Localization::readDate(const QString &str, const QString &format, bool *ok) const
{
    if (ok) *ok = false;
    QDate res = d->locale.toDate(str, format);
    if (res.isValid()) *ok = true;
    return res;
}

QTime Localization::readTime(const QString &str, bool *ok) const
{
    QTime res;
    if (ok) *ok = false;

    // Try all the formats.
    for (QString format : d->timeFormats) {
        res = readTime(str, format, ok);
        if (*ok) return res;
    }

    *ok = false;
    return res;
}

QTime Localization::readTime(const QString &str, const QString &format, bool *ok) const
{
    if (ok) *ok = false;
    QTime res = d->locale.toTime(str, format);
    if (res.isValid()) *ok = true;
    return res;
}

QString Localization::dateTimeFormat(bool longFormat) const
{
    QString res = d->locale.dateTimeFormat(longFormat ? QLocale::LongFormat : QLocale::ShortFormat);

    // But let's use long years even if the locale has short ones.
    if (res.contains("yy") && (!res.contains("yyyy")))
        res = res.replace("yy", "yyyy");

    return res;
}

QString Localization::dateFormat(bool longFormat) const
{
    QString res = d->locale.dateFormat(longFormat ? QLocale::LongFormat : QLocale::ShortFormat);

    // But let's use long years even if the locale has short ones.
    if (res.contains("yy") && (!res.contains("yyyy")))
        res = res.replace("yy", "yyyy");

    return res;
}

QString Localization::timeFormat(bool longFormat) const
{
    return d->locale.timeFormat(longFormat ? QLocale::LongFormat : QLocale::ShortFormat);
}

QString Localization::dateFormat(int type) const
{
    if (type == 1) return "d" + d->dateSepShort + "M" + d->dateSepShort + "yyyy";
    if (type == 2) return "d" + d->dateSepShort + "M" + d->dateSepShort + "yy";
    if (type == 3) return "d" + d->dateSepShort + "MMM" + d->dateSepShort + "yy";
    if (type == 4) return "d" + d->dateSepShort + "MMM" + d->dateSepShort + "yyyy";
    if (type == 5) return "d" + d->dateSepLong + "MMMM" + d->dateSepLong + "yy";
    if (type == 6) return "d" + d->dateSepLong + "MMMM" + d->dateSepLong + "yyyy";
    if (type == 7) return "MMMM" + d->dateSepLong + "d" + d->dateSepLong + "yy";
    if (type == 8) return "MMMM" + d->dateSepLong + "d" + d->dateSepLong + "yyyy";

    return QString();
}

QString Localization::currencySymbol() const
{
    return d->locale.currencySymbol();
}

int Localization::firstDayOfWeek() const
{
    return (int) d->locale.firstDayOfWeek();
}

QString Localization::dayName(int day, bool longFormat) const
{
    return d->locale.standaloneDayName(day, longFormat ? QLocale::LongFormat : QLocale::ShortFormat);
}

QString Localization::monthName(int month, bool longFormat) const
{
    return d->locale.standaloneMonthName(month, longFormat ? QLocale::LongFormat : QLocale::ShortFormat);
}


QString Localization::translateString(KLocalizedString str) const
{
    QStringList langs = d->locale.uiLanguages();
    return str.toString(langs);
}

QString Localization::formatBool(bool val) const
{
    return val ? d->trueString : d->falseString;
}

// TODO - we should let the caller specify whether to use scientific format or not
QString Localization::formatNumber(double num, int precision) const
{
    char fmt = 'f';
    // use scientific format?
    if ((num > 1e9) || (num < -1e9) || ((num > 0) && (num < 1e-8)) || ((num < 0) && (num > -1e-8))) fmt = 'e';
    return d->locale.toString(num, fmt, precision);
}

QString Localization::formatCurrency(double num, const QString &currencySymbol, int precision) const
{
    return d->locale.toCurrencyString(num, currencySymbol, precision);
}

QString Localization::formatDoubleNoSep(double val) const
{
    QString res = QString::number(val, 'g', 10);
    int pos = res.indexOf('.');
    const QString decimalSymbol = d->locale.decimalPoint();
    if (!decimalSymbol.isNull() && (pos != -1))
        res.replace(pos, 1, decimalSymbol);
    return res;
}


QString Localization::formatDateTime(const QDateTime &datetime, bool longFormat) const
{
    QString fmt = dateTimeFormat(longFormat);
    return d->locale.toString(datetime, fmt);
}

QString Localization::formatDateTime(const QDateTime &datetime, const QString &format) const
{
    return d->locale.toString(datetime, format);
}

QString Localization::formatDate(const QDate &date, bool longFormat) const
{
    QString fmt = dateFormat(longFormat);
    return d->locale.toString(date, fmt);
}

QString Localization::formatDate(const QDate &date, const QString &format) const
{
    return d->locale.toString(date, format);
}

QString Localization::formatTime(const QTime &time, bool longFormat) const
{
    QString fmt = timeFormat(longFormat);
    return d->locale.toString(time, fmt);
}

QString Localization::formatTime(const QTime &time, const QString &format) const
{
    return d->locale.toString(time, format);
}

QString Localization::languageName(bool full) const
{
    if (full) return d->locale.nativeLanguageName();
    return d->locale.name();
}


QString Localization::toUpper(const QString &str) const
{
    return d->locale.toUpper(str);
}

QString Localization::toLower(const QString &str) const
{
    return d->locale.toLower(str);
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
    d->locale = l;

    // Determine the date/time separator
    QString longFormat = dateFormat(true);
    d->dateSepLong = getSeparator(longFormat);
    QString shortFormat = dateFormat(false);
    d->dateSepShort = getSeparator(shortFormat);

    // date formats
    d->dateFormats.clear();
    d->dateFormats.append (longFormat);
    d->dateFormats.append (shortFormat);
    d->dateFormats.append (longFormat.replace("yy", "yyyy"));
    d->dateFormats.append (longFormat.replace("yyyy", "yy"));
    //long-year
    if (shortFormat.contains("yyyy")) {
        d->dateFormats.append (shortFormat.replace("MM", "M"));
        d->dateFormats.append (shortFormat.replace("MM", "M").replace("dd", "d"));
        d->dateFormats.append (shortFormat.replace("dd", "d"));
        shortFormat = shortFormat.replace("yyyy", "yy");
        d->dateFormats.append (shortFormat);
    } else {
        d->dateFormats.append (shortFormat.replace("yy", "yyyy"));
    }
    // short-year
    d->dateFormats.append (shortFormat.replace("MM", "M"));
    d->dateFormats.append (shortFormat.replace("MM", "M").replace("dd", "d"));
    d->dateFormats.append (shortFormat.replace("dd", "d"));

    // date/time formats
    longFormat = dateTimeFormat(true);
    shortFormat = dateTimeFormat(false);
    d->dateTimeFormats.clear();
    d->dateTimeFormats.append (longFormat);

    d->dateTimeFormats.append (shortFormat);
    longFormat = longFormat.replace("AP", "").replace("t", "").trimmed();
    shortFormat = shortFormat.replace("AP", "").replace("t", "").trimmed();
    d->dateTimeFormats.append (longFormat);
    d->dateTimeFormats.append (shortFormat);
    d->dateTimeFormats.append (longFormat.replace("yy", "yyyy"));
    d->dateTimeFormats.append (longFormat.replace("yyyy", "yy"));
    //long-year
    if (shortFormat.contains("yyyy")) {
        d->dateTimeFormats.append (shortFormat.replace("MM", "M"));
        d->dateTimeFormats.append (shortFormat.replace("MM", "M").replace("dd", "d"));
        d->dateTimeFormats.append (shortFormat.replace("dd", "d"));
        shortFormat = shortFormat.replace("yyyy", "yy");
        d->dateTimeFormats.append (shortFormat);
    } else {
        d->dateTimeFormats.append (shortFormat.replace("yy", "yyyy"));
    }
    // short-year
    d->dateTimeFormats.append (shortFormat.replace("MM", "M"));
    d->dateTimeFormats.append (shortFormat.replace("MM", "M").replace("dd", "d"));
    d->dateTimeFormats.append (shortFormat.replace("dd", "d"));


    // time separator
    shortFormat = timeFormat(false);
    d->timeSep = getSeparator(shortFormat);
    longFormat = timeFormat(true);
    if (longFormat.indexOf("A") >= 0) d->includesAMPM = true;
    else d->includesAMPM = false;

    // time formats
    d->timeFormats.clear();
    d->timeFormats.append (longFormat);
    d->timeFormats.append (shortFormat);
    longFormat = longFormat.replace("AP", "").replace("t", "").trimmed();
    shortFormat = shortFormat.replace("AP", "").replace("t", "").trimmed();
    d->timeFormats.append (longFormat);
    d->timeFormats.append (shortFormat);


    // cache true/false
    d->trueString = translateString(ki18n("true")).toLower();
    d->falseString = translateString(ki18n("false")).toLower();
}


