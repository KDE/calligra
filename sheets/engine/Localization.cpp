/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Localization.h"
#include "SheetsDebug.h"

#include <QDateTime>

namespace Calligra { namespace Sheets {
// temporary solution avoid memleaks and reuse instances
class Locales {
public:
    Locales() {}
    ~Locales() {
        QList<Localization*> list = locales.values();
        while (!list.isEmpty()) {
            auto l = list.takeFirst();
            list.removeAll(l);
            delete l;
        }
    }
    const Localization *locale(const QString &language) {
        if (language.isEmpty()) {
            return &defaultLocale;
        }
        if (!locales.contains(language)) {
            auto l = new Localization;
            l->setLanguage(language);
            locales.insert(language, l);
            return l;
        }
        return locales.value(language);
    }
private:
    QMap<QString, Localization*> locales;
    Localization defaultLocale;
};
}}

#ifdef Q_OS_WIN
// HACK to get this to compile on msvc
#include <QSharedDataPointer>

namespace Calligra
{
namespace Sheets
{

class Q_DECL_HIDDEN Localization::PrivateData : public QSharedData
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
    Private(Localization::PrivateData *pd = nullptr) : data(pd) {}
    QSharedDataPointer<Localization::PrivateData> data;
};
}}

using namespace Calligra::Sheets;

Localization::Localization()
    : pd(new Private(new PrivateData))
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

QString Localization::dateFormat(Format::Type type) const
{
    Q_ASSERT(type >= Format::DatesBegin && type <= Format::DatesEnd);
    QString format;
    switch (type) {
        case Format::ShortDate:
            format = d->locale.dateFormat(QLocale::ShortFormat);
            break;
        case Format::Date1:
            format = "d" + d->dateSepShort + "M" + d->dateSepShort + "yyyy";
            break;
        case Format::Date2:
            format = "d" + d->dateSepShort + "M" + d->dateSepShort + "yy";
            break;
        case Format::Date3:
            format = "d" + d->dateSepShort + "MMM" + d->dateSepShort + "yy";
            break;
        case Format::Date4:
            format = "d" + d->dateSepShort + "MMM" + d->dateSepShort + "yyyy";
            break;
        case Format::Date5:
            format = "d" + d->dateSepLong + "MMMM" + d->dateSepLong + "yy";
            break;
        case Format::TextDate:
            format = d->locale.dateFormat(QLocale::LongFormat);
            break;
        case Format::Date6:
            format = "d" + d->dateSepLong + "MMMM" + d->dateSepLong + "yyyy";
            break;
        case Format::Date7:
            format = QStringLiteral("MMMM%1d%2yy").arg(d->dateSepLong, d->dateSepLong);
            break;
        case Format::Date8:
            format = "MMMM" + d->dateSepLong + "d" + d->dateSepLong + "yyyy";
            break;
        default:
            errorSheets<<"Invalid date format type:"<<type;
            break;
    }
    return format;
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
    QString fmt = dateFormat(longFormat ? Format::TextDate : Format::ShortDate);
    return d->locale.toString(date, fmt);
}

QString Localization::formatDate(const QDate &date, const QString &format) const
{
    auto s = d->locale.toString(date, format);
    return s;
}

QString Localization::timeFormat(Format::Type type) const
{
    Q_ASSERT(type >= Format::TimesBegin && type <= Format::TimesEnd);
    QString format;
    switch (type) {
        case Format::ShortTime:
            format = d->locale.timeFormat(QLocale::ShortFormat);
            break;
        case Format::LongTime:
            format = d->locale.timeFormat(QLocale::LongFormat);
            break;
        case Format::Time1: // e.g. 09:01 AM
            format = QString().append("hh").append(timeSeparator()).append("mm").append('A');
            break;
        case Format::Time2: // e.g. 09:01:05 PM
            format = QString().append("hh").append(timeSeparator()).append("mm").append("ss").append('A');
            break;
        case Format::Time3: // e.g. 01:28,24
            format = QString().append("hh").append("mm").append('z');
            break;
        case Format::Time4: // e.g. 9:01
            format = QString().append('h').append(timeSeparator()).append("mm");
            break;
        case Format::Time5: // e.g. 9:01:12
            format = QString().append('h').append(timeSeparator()).append("mm").append("ss");
            break;
        case Format::Time6: // e.g. 4023:28
            format = QString().append("[mm]").append(timeSeparator()).append("ss");
            break;
        case Format::Time7: // 5013:28:12
            format = QString().append("[h]").append(timeSeparator()).append("mm").append("ss");
            break;
        case Format::Time8: // 5013:28
            format = QString().append("[h]").append(timeSeparator()).append("mm");
            break;
        default:
            format = d->locale.timeFormat(QLocale::ShortFormat);
            break;

    }
    return format;
}

QString Localization::formatTime(const QTime &time, bool longFormat) const
{
    QString fmt = timeFormat(longFormat ? Format::LongTime : Format::ShortTime);
     // Timezone is irrelevant, remove it
    fmt = fmt.replace(QLatin1Char('t'), QLatin1String()).trimmed();
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
            // handle quotes, they should not be part of the separator
            if (str[i] == QLatin1Char('\'')) break;
            if (str[i] == QLatin1Char('\"')) break;
            sep += str[i];
        }
    }
    return sep;
}

void Localization::setLocale(const QLocale &l) {
    d->locale = l;

    // Determine the date/time separator
    QString longFormat = dateFormat(Format::TextDate);
    d->dateSepLong = getSeparator(longFormat);
    QString shortFormat = dateFormat(Format::ShortDate);
    d->dateSepShort = getSeparator(shortFormat);

    // Date formats
    // Note: These are only used for converting a string to Qdate, see readDate()
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

    // Date/time formats.
    // Note: These are only used for converting a string to QDateTime, see readDateTime()
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
    shortFormat = d->locale.timeFormat(QLocale::ShortFormat);
    d->timeSep = getSeparator(shortFormat);

    // Time formats.
    // Note: These are only used for converting a string to QTime, see readTime()
    d->timeFormats.clear();

    // hours
    QString format = QStringLiteral("h%1").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("h%1 A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    format = QStringLiteral("hh%1").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("hh%1 A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    // minutes
    format = QStringLiteral("h%1m").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("h%1m A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    format = QStringLiteral("hh%1m").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("hh%1m A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    format = QStringLiteral("h%1mm").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("h%1mm A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    format = QStringLiteral("hh%1mm").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("hh%1mm A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    // seconds single
    format = QStringLiteral("h%1m%1s").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("h%1m%1s A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    format = QStringLiteral("hh%1m%1s").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("hh%1m%1s A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    format = QStringLiteral("h%1mm%1s").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("h%1mm%1s A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    format = QStringLiteral("hh%1mm%1s").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("hh%1mm%1s A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    // seconds double
    format = QStringLiteral("h%1m%1ss").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("h%1m%1ss A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    format = QStringLiteral("hh%1m%1ss").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("hh%1m%1ss A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    format = QStringLiteral("h%1mm%1ss").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("h%1mm%1ss A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    format = QStringLiteral("hh%1mm%1ss").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("hh%1mm%1ss A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    // milliseconds
    format = QStringLiteral("h%1m%1s.z").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("h%1m%1s.z A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    format = QStringLiteral("hh%1m%1s.z").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("hh%1m%1s.z A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    format = QStringLiteral("h%1mm%1s.z").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("h%1mm%1s.z A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    format = QStringLiteral("hh%1mm%1s.z").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("hh%1mm%1s.z A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    format = QStringLiteral("h%1m%1ss.z").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("h%1m%1ss.z A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    format = QStringLiteral("hh%1m%1ss.z").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("hh%1m%1ss.z A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    format = QStringLiteral("h%1mm%1ss.z").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("h%1mm%1ss.z A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    format = QStringLiteral("hh%1mm%1ss.z").arg(d->timeSep);
    d->timeFormats.prepend(format);
    format = QStringLiteral("hh%1mm%1ss.z A").arg(d->timeSep);
    d->timeFormats.prepend(format);

    // cache true/false
    d->trueString = translateString(ki18n("true")).toLower();
    d->falseString = translateString(ki18n("false")).toLower();
}

bool Localization::operator==(const Localization &other) const
{
    return d->locale == other.d->locale;
}

bool Localization::operator!=(const Localization &other) const
{
    return d->locale != other.d->locale;
}

QLocale Localization::qLocale() const
{
    return d->locale;
}

const Localization *Localization::getLocale(const QString &language, const QString &country, const QString &script)
{
    static Locales locales ;
    auto lang = language;
    if (!script.isEmpty()) lang.append('-').append(script);
    if (!country.isEmpty()) lang.append('-').append(country);
    return locales.locale(lang);
}

QDebug operator<<(QDebug dbg, const Calligra::Sheets::Localization *l)
{
    if (l) {
        return operator<<(dbg, *l);
    }
    dbg.noquote().nospace()<<"Calligra::Sheets::Localization("<<(void*)l<<')';
    return dbg.quote().space();
}

QDebug operator<<(QDebug dbg, const Calligra::Sheets::Localization &l)
{
    dbg.noquote().nospace()<<"Calligra::Sheets::Localization("<<l.qLocale()<<" t='"<<l.thousandsSeparator()<<"' d='"<<l.decimalSymbol()<<"')";
    return dbg.quote().space();
}
