/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Localization.h"
#include "CS_Time.h"

#include <QDateTime>
#include <QRegularExpression>

#ifdef _MSC_VER
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
    QMap<Format::Type, QString> dateTimeFormats, dateFormats, timeFormats;
    bool includesAMPM;
    QString trueString, falseString;
};
class Q_DECL_HIDDEN Localization::Private
{
public:
    Private(Localization::PrivateData *pd = nullptr)
        : data(pd)
    {
    }
    QSharedDataPointer<Localization::PrivateData> data;
};
}
}

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
    QMap<Format::Type, QString> dateTimeFormats, dateFormats, timeFormats;
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
    if (ok)
        *ok = false;
    QDateTime res;

    // Try all the formats.
    for (const QString &format : std::as_const(d->dateTimeFormats)) {
        res = readDateTime(str, format, ok);
        if (res.isValid())
            break;
    }
    if (ok)
        *ok = res.isValid();
    return res;
}

QDateTime Localization::readDateTime(const QString &str, const QString &format, bool *ok) const
{
    if (ok)
        *ok = false;
    QDateTime res = d->locale.toDateTime(str, format);
    res.setTimeSpec(Qt::UTC);
    if (ok)
        *ok = res.isValid();
    return res;
}

QDate Localization::readDate(const QString &str, bool *ok) const
{
    QDate res;
    if (ok)
        *ok = false;

    // Try all the formats.
    for (const auto &format : std::as_const(d->dateFormats)) {
        res = readDate(str, format, ok);
        if (res.isValid())
            break;
    }
    if (ok)
        *ok = res.isValid();
    return res;
}

QDate Localization::readDate(const QString &str, const QString &format, bool *ok) const
{
    if (ok)
        *ok = false;
    QDate res = d->locale.toDate(str, format);
    if (!res.isValid()) {
        res = QDate::fromString(str);
    }
    if (ok)
        *ok = res.isValid();
    return res;
}

Time Localization::readTime(const QString &str, bool *ok) const
{
    Time res;
    if (ok)
        *ok = false;

    // Try all the formats.
    for (const auto &format : std::as_const(d->timeFormats)) {
        res = readTime(str, format, ok);
        if (res.isValid()) {
            break;
        }
    }
    return res;
}

QString Localization::timeToRegExp(const QString &format, bool neg) const
{
    auto f = format;
    if (neg) {
        f.prepend("-?");
    }
    if (f.contains("[h]")) {
        f.replace("[h]", "\\d+");
    } else if (f.contains("[mm]")) {
        f.replace("[mm]", "[0-5]\\d");
    } else if (f.contains("hh")) {
        f.replace("hh", "\\d+");
    } else if (f.contains("h")) {
        f.replace("h", "\\d+");
    }
    if (f.contains("mm")) {
        f.replace("mm", "([0-5]\\d|\\d)");
    } else if (f.contains("m")) {
        f.replace("m", "([0-5]\\d|\\d)");
    }
    if (f.contains("ss")) {
        f.replace("ss", "([0-5]\\d)");
    } else if (f.contains("s")) {
        f.replace("s", "([0-5]\\d|\\d{1})");
    }
    if (f.contains(".zzz")) {
        f.replace(".zzz", "\\d{3}");
    } else if (f.contains("z")) {
        f.replace("z", "\\d+");
    }
    if (f.contains("ap")) {
        f.replace("ap",
                  QString("(%1|%2|%3|%4)")
                      .arg(d->locale.amText().toUpper(), d->locale.amText().toLower(), d->locale.pmText().toUpper(), d->locale.pmText().toLower()));
    }
    return f;
}

Time Localization::readTime(const QString &str, const QString &format, bool *ok) const
{
    // TODO parsing too simple
    // TODO handle negative values
    if (!(format.contains('[') && format.contains(']'))) {
        auto f = format;
        QRegularExpression regap(QString("%1|%2").arg(d->locale.amText(), d->locale.pmText()), QRegularExpression::CaseInsensitiveOption);
        if (!format.contains("ap") && str.contains(regap)) {
            f.append(" ap"); // TODO: does this work always?
        }
        const auto exp = timeToRegExp(f);
        const QRegularExpression reg(exp);
        const auto match = reg.match(str);
        if (!match.hasMatch() || match.capturedLength() != str.length()) {
            Time time;
            time.setValid(false);
            if (ok)
                *ok = false;
            return time;
        }
        // just use QTime
        auto qt = d->locale.toTime(str, f);
        Time time(qt);
        if (ok)
            *ok = time.isValid();
        return time;
    }
    const auto exp = timeToRegExp(format, true);
    const QRegularExpression reg(exp);
    const auto match = reg.match(str);
    if (!match.hasMatch() || match.capturedLength() != str.length()) {
        Time time;
        time.setValid(false);
        if (ok)
            *ok = false;
        return time;
    }

    QMap<int, int> values;
    if (format.contains("[h]"))
        values.insert(format.indexOf("[h]"), 1);
    if (format.contains("[mm]"))
        values.insert(format.indexOf("[mm]"), 2);
    else if (format.contains("mm"))
        values.insert(format.indexOf("mm"), 2);
    if (format.contains("ss"))
        values.insert(format.indexOf("ss"), 3);

    int64_t h = 0;
    int64_t m = 0;
    double s = 0.0;

    QString number;
    bool succ = false;
    for (int i = 0; i <= str.length(); ++i) {
        if (i < str.length() && (str.at(i).isDigit() || str.at(i) == d->locale.decimalPoint() || (str.at(i) == d->locale.negativeSign() && number.isEmpty()))) {
            number += str.at(i);
            continue;
        } else if (!number.isEmpty()) {
            auto type = values.constBegin().value();
            switch (type) {
            case 1:
                h = number.toLong(&succ);
                if (!succ) {
                    Time time;
                    time.setValid(false);
                    if (ok)
                        *ok = false;
                    return time;
                }
                values.remove(values.key(type));
                break;
            case 2:
                m = number.toLong(&succ);
                if (!succ) {
                    Time time;
                    time.setValid(false);
                    if (ok)
                        *ok = false;
                    return time;
                }
                values.remove(values.key(type));
                break;
            case 3:
                s = number.toDouble(&succ);
                if (!succ) {
                    Time time;
                    time.setValid(false);
                    if (ok)
                        *ok = false;
                    return time;
                }
                values.remove(values.key(type));
                break;
            default:
                break;
            }
            number.clear();
        }
    }
    if (succ && str.contains('-')) {
        const Number dur = (double)h + m / 60.0 + s / 3600;
        return Time(dur);
    }
    if (ok)
        *ok = succ;
    return Time(h, m, s);
}

QString Localization::dateTimeFormat(bool longFormat) const
{
    QString res = d->locale.dateTimeFormat(longFormat ? QLocale::LongFormat : QLocale::ShortFormat);
    const auto pos = res.indexOf(" t"); // timespec should never be used
    if (pos != -1) {
        res.remove(pos, 2);
    }
    // The format must match what is used in KoOdfNumberstyles::loadOdfNumberStyle(),
    // which use lowercase atm
    res.replace('H', 'h').replace("AP", "ap");
    return res;
}

QString Localization::dateFormat(bool longFormat) const
{
    QString res = d->locale.dateFormat(longFormat ? QLocale::LongFormat : QLocale::ShortFormat);
    return res;
}

QString Localization::timeFormat(bool longFormat) const
{
    auto fmt = d->locale.timeFormat(longFormat ? QLocale::LongFormat : QLocale::ShortFormat);
    // The format must match what is used in KoOdfNumberstyles::loadOdfNumberStyle(),
    // which use lowercase atm
    fmt = fmt.toLower();
    const auto pos = fmt.indexOf(" t"); // timespec should never be used
    if (pos != -1) {
        fmt.remove(pos, 2);
    }
    // QLocale returns seconds (ss) also for short format, remove
    if (!longFormat) {
        fmt.replace(":ss", "");
    }
    return fmt;
}

QString Localization::dateTimeFormat(Format::Type type) const
{
    return d->dateTimeFormats.value(type);
}

QString Localization::dateFormat(Format::Type type) const
{
    return d->dateFormats.value(type);
}

QString Localization::timeFormat(Format::Type type) const
{
    return d->timeFormats.value(type);
}

QString Localization::currencySymbol() const
{
    return d->locale.currencySymbol();
}

int Localization::firstDayOfWeek() const
{
    return (int)d->locale.firstDayOfWeek();
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
    if ((num > 1e9) || (num < -1e9) || ((num > 0) && (num < 1e-8)) || ((num < 0) && (num > -1e-8)))
        fmt = 'e';
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

QString Localization::formatTime(const Time &time, bool longFormat) const
{
    QString fmt = timeFormat(longFormat);
    return d->locale.toString(time.toQTime(), fmt);
}

QString Localization::formatTime(const Time &time, const QString &format) const
{
    // TODO parsing is too simple
    QString result;
    if (!format.contains("[h]") && !format.contains("[mm]")) {
        result = d->locale.toString(time.toQTime(), format); // clock time
    } else {
        // duration
        // TODO: leading zeros
        auto fmt = format;
        if (format.contains("[h]")) {
            fmt.replace("[h]", "%1");
            const auto h = d->locale.toString(time.hours());
            if (fmt.contains("mm")) {
                fmt.replace("mm", "%2");
                const auto m = d->locale.toString(time.minute());
                if (fmt.contains("ss.z")) {
                    fmt.replace("ss.z", "%3");
                    const auto s = d->locale.toString(time.seconds());
                    result = fmt.arg(h, m, s);
                } else if (fmt.contains("ss")) {
                    fmt.replace("ss", "%3");
                    const auto s = d->locale.toString(time.second());
                    result = fmt.arg(h, m, s);
                } else {
                    result = fmt.arg(h, m);
                }
            } else {
                result = fmt.arg(h);
            }
        } else if (fmt.contains("[mm]")) {
            fmt.replace("[mm]", "%1");
            const auto m = d->locale.toString(time.minutes());
            if (fmt.contains("ss.z")) {
                fmt.replace("ss.z", "%2");
                const auto s = d->locale.toString(time.seconds());
                result = fmt.arg(m, s);
            } else if (fmt.contains("ss")) {
                fmt.replace("ss", "%2");
                const auto s = d->locale.toString(time.second());
                result = fmt.arg(m, s);
            } else {
                result = fmt.arg(m);
            }
        } else {
            // error
        }
    }
    return result;
}

QString Localization::languageName(bool full) const
{
    if (full)
        return d->locale.nativeLanguageName();
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

static QString getSeparator(const QString &str)
{
    QString sep;
    int stage = 0;
    for (int i = 0; i < str.length(); ++i) {
        if (stage == 0) {
            if (str[i].isLetter())
                stage++;
            continue;
        }
        if (stage == 1) {
            if (str[i].isLetter())
                continue;
            stage++; // no continue here, we want to apply stage2 logic
        }
        if (stage == 2) {
            if (str[i].isLetter())
                break; // we're done
            sep += str[i];
        }
    }

    return sep;
}

void Localization::updateDateTimeFormats()
{
    d->dateFormats.clear();

    // Determine the date/time separator
    const auto longFormat = dateTimeFormat(true);
    const auto sepLong = getSeparator(longFormat);
    const auto shortFormat = dateTimeFormat(false);
    const auto sepShort = getSeparator(shortFormat);

    // datetime formats
    d->dateTimeFormats.insert(Format::DateTimeShort, shortFormat);
    d->dateTimeFormats.insert(Format::DateTimeLong, longFormat);

    // do not localize
    d->dateTimeFormats.insert(Format::DateTimeIsoShort, "yyyy-MM-ddThh:mm:ss");
    d->dateTimeFormats.insert(Format::DateTimeIsoLong, "yyyy-MM-ddThh:mm:ss.zzz");
}

void Localization::updateDateFormats()
{
    QString fmt;
    d->dateFormats.clear();

    // Determine the date/time separator
    const auto longFormat = dateFormat(true);
    d->dateSepLong = getSeparator(longFormat);
    QString shortFormat = dateFormat(false);
    d->dateSepShort = getSeparator(shortFormat);
    d->dateFormats.insert(Format::ShortDate, shortFormat);
    d->dateFormats.insert(Format::TextDate, longFormat);

    // short
    fmt = shortFormat;
    if (fmt.contains("yyyy")) {
        d->dateFormats.insert(Format::Date1, fmt.replace("yyyy", "yy"));
    } else {
        d->dateFormats.insert(Format::Date1, fmt.replace("yy", "yyyy"));
    }
    if (fmt.contains("MM")) {
        fmt.replace("MM", "M");
        d->dateFormats.insert(Format::Date2, fmt.replace("dd", "d")); // short month, long day
    } else {
        fmt.replace("M", "MM");
        if (!fmt.contains("dd")) {
            d->dateFormats.insert(Format::Date2, fmt.replace("d", "dd")); // long month, long day
        }
    }
    if (fmt.contains("yyyy")) {
        d->dateFormats.insert(Format::Date3, fmt.replace("yyyy", "yy"));
    } else {
        d->dateFormats.insert(Format::Date3, fmt.replace("yy", "yyyy"));
    }
    // long
    fmt = longFormat;
    d->dateFormats.insert(Format::Date4, fmt.replace("MMMM", "MMM")); // long day, abrev moth
    fmt = longFormat;
    d->dateFormats.insert(Format::Date5, fmt.replace("dddd", "ddd"));
    d->dateFormats.insert(Format::Date6, fmt.replace("MMMM", "MMM"));
    d->dateFormats.insert(Format::Date7, fmt.replace("yyyy", "yy"));
    // d->dateFormats.insert(fmt.replace("MMM", "MMMMM")); // special one letter month format

    // do not localize
    d->dateFormats.insert(Format::DateIso1, "yyyy-MM-dd");
    d->dateFormats.insert(Format::DateIso2, "yy-MM-dd");
}

void Localization::updateTimeFormats()
{
    QString fmt;
    d->timeFormats.clear();

    const auto shortFormat = timeFormat(false);
    d->timeSep = getSeparator(shortFormat);
    d->includesAMPM = shortFormat.contains("ap");

    const auto longFormat = timeFormat(true);

    // time formats
    d->timeFormats.clear();

    fmt = shortFormat;
    if (d->includesAMPM) {
        fmt = fmt.replace("ap", "").trimmed();
    }
    d->timeFormats.insert(Format::Time, fmt);
    fmt = longFormat;
    if (d->includesAMPM) {
        fmt = fmt.replace("ap", "").trimmed();
    }
    d->timeFormats.insert(Format::SecondeTime, fmt);

    fmt = shortFormat;
    if (!d->includesAMPM) {
        fmt = fmt.append(" ap");
    }
    d->timeFormats.insert(Format::ShortTimeAP, fmt);

    fmt = longFormat;
    if (!d->includesAMPM) {
        fmt = fmt.append(" ap");
    }
    d->timeFormats.insert(Format::LongTimeAP, fmt);

    // Formats without am/pm
    fmt = shortFormat;
    fmt = fmt.replace("ap", "").trimmed();
    fmt.replace("hh", "h"); // 9:03
    d->timeFormats.insert(Format::Time1, fmt);
    fmt.replace("mm", "m"); // 9:3
    d->timeFormats.insert(Format::Time2, fmt);
    fmt.replace("m", ""); // 9.
    d->timeFormats.insert(Format::Time3, fmt);

    fmt = longFormat;
    fmt = fmt.replace("ap", "").trimmed();
    fmt.replace("hh", "h"); // 9:03:05
    d->timeFormats.insert(Format::Time4, fmt);
    fmt.replace("mm", "m"); // 9:3:05
    d->timeFormats.insert(Format::Time5, fmt);
    fmt.replace("ss", "s"); // 9:3:5
    d->timeFormats.insert(Format::Time6, fmt);

    fmt.replace("s", "s" + decimalSymbol() + 'z'); // milliseconds
    d->timeFormats.insert(Format::TimeMS, fmt);

    // Formats with am/pm
    fmt = shortFormat;
    if (!d->includesAMPM) {
        fmt.append(" ap");
    }
    fmt.replace("hh", "h"); // 9:03
    d->timeFormats.insert(Format::Time1_AP, fmt);
    fmt.replace("mm", "m"); // 9:3
    d->timeFormats.insert(Format::Time2_AP, fmt);
    fmt.replace("m", ""); // 9.
    d->timeFormats.insert(Format::Time3_AP, fmt);

    fmt = longFormat;
    if (!d->includesAMPM) {
        fmt.append(" ap");
    }
    fmt.replace("hh", "h"); // 9:03:05
    d->timeFormats.insert(Format::Time4_AP, fmt);
    fmt.replace("mm", "m"); // 9:3:05
    d->timeFormats.insert(Format::Time5_AP, fmt);
    fmt.replace("ss", "s"); // 9:3:5
    d->timeFormats.insert(Format::Time6_AP, fmt);

    fmt.replace("s", "s" + decimalSymbol() + 'z'); // milliseconds
    d->timeFormats.insert(Format::TimeMS_AP, fmt);

    // Durations
    d->timeFormats.insert(Format::DurationHourLong, QString("[h]%1mm%1ss").arg(d->timeSep));
    d->timeFormats.insert(Format::DurationHourShort, QString("[h]%1mm").arg(d->timeSep));
    d->timeFormats.insert(Format::DurationMinute, QString("[mm]%1ss").arg(d->timeSep));

    d->timeFormats.insert(Format::TimeIso, "hh:mm:ss.z");
}

void Localization::setLocale(const QLocale &l)
{
    d->locale = l;

    updateDateTimeFormats();
    updateDateFormats();
    updateTimeFormats();

    // cache true/false
    d->trueString = translateString(ki18n("true")).toLower();
    d->falseString = translateString(ki18n("false")).toLower();
}

QDebug operator<<(QDebug dbg, const Calligra::Sheets::Localization *l)
{
    if (l) {
        return operator<<(dbg, *l);
    }
    dbg.noquote().nospace() << "Calligra::Sheets::Localization(" << (void *)l << ')';
    return dbg.quote().space();
}

QDebug operator<<(QDebug dbg, const Calligra::Sheets::Localization &l)
{
    dbg.noquote().nospace() << "Calligra::Sheets::Localization(" << l.languageName(false) << " t='" << l.thousandsSeparator() << "' d='" << l.decimalSymbol()
                            << "')";
    return dbg.quote().space();
}
