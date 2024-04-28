/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2004 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "ValueParser.h"

#include "CalculationSettings.h"
#include "Localization.h"
#include "Value.h"

#include <KLocalizedString>

using namespace Calligra::Sheets;

ValueParser::ValueParser(const CalculationSettings *settings)
    : m_settings(settings)
{
}

const CalculationSettings *ValueParser::settings() const
{
    return m_settings;
}

Value ValueParser::parse(const QString &str) const
{
    Value val;

    // If the text is empty, we don't have a value
    // If the user stated explicitly that they wanted text
    // (using the format or using a quote),
    // then we don't parse as a value, but as string.
    if (str.isEmpty() || str.at(0) == '\'') {
        val = Value(str);
        return val;
    }

    bool ok;

    QString strStripped = str.trimmed();
    // Try parsing as various datatypes, to find the type of the string

    // First as number
    val = tryParseNumber(strStripped, &ok);

    if (ok)
        return val;

    // Then as bool
    // Note - I swapped the order of these two to try parsing as a number
    // first because that will probably be the most common case
    val = tryParseBool(strStripped, &ok);
    if (ok)
        return val;

    val = tryParseDateTime(strStripped, &ok);
    if (ok)
        return val;

    val = tryParseDate(strStripped, &ok);
    if (ok)
        return val;

    val = tryParseTime(strStripped, &ok);
    if (ok)
        return val;

    // Nothing particular found, then this is simply a string
    val = Value(str);
    return val;
}

Value ValueParser::tryParseBool(const QString &str, bool *ok) const
{
    Value val;
    if (ok)
        *ok = false;

    const QString &lowerStr = str.toLower();

    if ((lowerStr == "true") || (lowerStr == m_settings->locale()->formatBool(true))) {
        val = Value(true);
        if (ok)
            *ok = true;
    } else if ((lowerStr == "false") || (lowerStr == m_settings->locale()->formatBool(false))) {
        val = Value(false);
        if (ok)
            *ok = true;
    }
    return val;
}

Value ValueParser::readNumber(const QString &_str, bool *ok) const
{
    bool isInt = false;
    QString str = _str.trimmed();
    bool neg = str.indexOf(m_settings->locale()->negativeSign()) == 0;
    if (neg)
        str.remove(0, m_settings->locale()->negativeSign().length());

    /* will hold the scientific notation portion of the number.
    Example, with 2.34E+23, exponentialPart == "E+23"
    */
    QString exponentialPart;
    int EPos = str.indexOf('E', 0, Qt::CaseInsensitive);

    if (EPos != -1) {
        exponentialPart = str.mid(EPos);
        str = str.left(EPos);
    }

    int pos;
    int fracPos;
    QString major;
    QString minor;
    if ((pos = str.indexOf(m_settings->locale()->decimalSymbol())) != -1) {
        major = str.left(pos);
        minor = str.mid(pos + m_settings->locale()->decimalSymbol().length());
        isInt = false;
    } else if (((pos = str.indexOf(' ')) != -1) && ((fracPos = str.indexOf('/')) != -1)) {
        // try to parse fractions of this form:
        // [0-9]+ [0-9]+/[1-9][0-9]?
        major = str.left(pos);
        QString numerator = str.mid(pos + 1, (fracPos - pos - 1));
        QString denominator = str.mid(fracPos + 1);
        double minorVal = numerator.toDouble() / denominator.toDouble();
        if (minorVal > 1) {
            // assume major is just a plain number
            double wholePart = floor(minorVal);
            minorVal -= wholePart;
            major = QString("%1").arg(major.toInt() + (int)wholePart);
        }
        minor = QString::number(minorVal, 'f').remove(0, 2); // chop off the "0." part
        // debugSheets <<"fraction:" << major <<"." << minor;
    } else {
        major = str;
        isInt = (EPos == -1); // only, if no exponential part was found
    }

    // Remove thousand separators
    int thlen = m_settings->locale()->thousandsSeparator().length();
    int lastpos = 0;
    while ((pos = major.indexOf(m_settings->locale()->thousandsSeparator())) > 0) {
        // e.g. 12,,345,,678,,922 Acceptable positions (from the end) are 5, 10, 15... i.e. (3+thlen)*N
        int fromEnd = major.length() - pos;
        if (fromEnd % (3 + thlen) != 0 // Needs to be a multiple, otherwise it's an error
            || pos - lastpos > 3 // More than 3 digits between two separators -> error
            || pos == 0 // Can't start with a separator
            || (lastpos > 0 && pos - lastpos != 3)) { // Must have exactly 3 digits between two separators
            if (ok)
                *ok = false;
            return Value();
        }

        lastpos = pos;
        major.remove(pos, thlen);
    }
    if (lastpos > 0 && major.length() - lastpos != 3) { // Must have exactly 3 digits after the last separator
        if (ok)
            *ok = false;
        return Value();
    }

    // log10(2^63) ~= 18
    if (isInt && major.length() > 19)
        isInt = false;

    QString tot;
    if (neg)
        tot = '-';
    tot += major;
    if (!isInt)
        tot += '.' + minor + exponentialPart;

    return isInt ? Value((int64_t)tot.toLongLong(ok)) : Value(tot.toDouble(ok));
}

Number ValueParser::readImaginary(const QString &str, bool *ok) const
{
    if (str.isEmpty()) {
        if (ok)
            *ok = false;
        return 0.0;
    }

    Number imag = 0.0;
    if (str[0] == 'i' || str[0] == 'j') {
        if (str.length() == 1) {
            if (ok)
                *ok = true;
            imag = 1.0;
        } else
            imag = readNumber(str.mid(1), ok).asFloat();
    } else if (str[str.length() - 1] == 'i' || str[str.length() - 1] == 'j') {
        const QString minus(m_settings->locale()->negativeSign());
        if (str.length() == 2 && str[0] == '+') {
            if (ok)
                *ok = true;
            imag = 1.0;
        } else if (str.length() == minus.length() + 1 && str.left(minus.length()) == minus) {
            if (ok)
                *ok = true;
            imag = -1.0;
        } else
            imag = readNumber(str.left(str.length() - 1), ok).asFloat();
    } else
        *ok = false;
    return imag;
}

Value ValueParser::tryParseNumber(const QString &str, bool *ok) const
{
    Value value;
    if (str.endsWith('%')) { // percentage
        const Number val = readNumber(str.left(str.length() - 1).trimmed(), ok).asFloat();
        if (*ok) {
            // debugSheets <<"ValueParser::tryParseNumber '" << str <<
            //     "' successfully parsed as percentage: " << val << '%' << Qt::endl;
            value = Value(val / 100.0);
            value.setFormat(Value::fmt_Percent);
        }
    } else if (str.count('i') == 1 || str.count('j') == 1) { // complex number
        Number real = 0.0;
        Number imag = 0.0;
        const QString minus(m_settings->locale()->negativeSign());
        // both parts, real and imaginary, present?
        int sepPos;
        if ((sepPos = str.indexOf('+', 1)) != -1) {
            // imaginary part
            imag = readImaginary(str.mid(sepPos + 1).trimmed(), ok);
            // real part
            if (*ok)
                real = readNumber(str.left(sepPos).trimmed(), ok).asFloat();
        } else if ((sepPos = str.indexOf(minus, minus.length())) != -1) {
            // imaginary part
            imag = -readImaginary(str.mid(sepPos + 1).trimmed(), ok);
            // real part
            if (*ok)
                real = readNumber(str.left(sepPos).trimmed(), ok).asFloat();
        } else {
            // imaginary part
            if (str.trimmed().length() > 1) // but don't parse a stand-alone 'i'
                imag = readImaginary(str.trimmed(), ok);
            // real part
            if (*ok)
                real = 0.0;
        }
        if (*ok)
            value = Value(complex<Number>(real, imag));
    } else // real number
        value = readNumber(str, ok);
    return value;
}

static QString removeYearFromFormat(const QString &format)
{
    // The tricky part is that we need to remove any separator around the year
    // For instance YY-mm-dd becomes mm-dd and dd/mm/YY becomes %d/%m
    // If the year is in the middle, say mm-YY/dd, we'll remove the sep.
    // before it (mm/dd).
    int yearPos = format.indexOf("Y", 0, Qt::CaseInsensitive);
    if (yearPos == -1)
        return QString();

    QString fmt = format;
    while ((fmt.length() > yearPos) && ((fmt[yearPos] == 'y') || (fmt[yearPos] == 'Y')))
        fmt.remove(yearPos, 1);
    if (yearPos == 0) {
        while (fmt.length() && fmt[0].isLetter())
            fmt.remove(0, 1);
    } else {
        while ((yearPos > 0) && (!fmt[yearPos - 1].isLetter())) {
            fmt.remove(yearPos - 1, 1);
            yearPos--;
        }
    }
    return fmt;
}

// If the year is in the 1-99 or 1900-1999 range, need to check if a two-digit or a four-digit value was entered.
int ValueParser::repairYear(int year, const QString &str) const
{
    if (year >= 2000)
        return year;

    int ref = m_settings->referenceYear();
    if ((year >= 1900) && (year <= 1999)) {
        QString strYear = QString::number(year);
        if (str.indexOf(strYear) >= 0)
            return year;
        // Entered a two-digit year
        if (year < ref)
            return year + 100;
        return year;
    }
    if ((year >= 1) && (year <= 99)) {
        // check if 00 + year was entered
        QString strYear = "00" + QString::number(year);
        if (str.indexOf(strYear) >= 0)
            return year;
        // We actually want 19XX or 20XX
        if (year + 1900 < ref)
            return year + 2000;
        return year + 1900;
    }

    return year;
}

bool ValueParser::containsDateTimeSeparator(const QString &str) const
{
    // Speedup: if the string doesn't contain any separator, it's not a date/time.
    // The separator cannot be on the first position.
    Localization *locale = m_settings->locale();
    QString sep = locale->dateSeparator(true);
    if (sep.length() && (str.indexOf(sep, 1) > 0))
        return true;
    sep = locale->dateSeparator(false);
    if (sep.length() && (str.indexOf(sep, 1) > 0))
        return true;
    sep = locale->timeSeparator();
    if (sep.length() && (str.indexOf(sep, 1) > 0))
        return true;
    if (str.indexOf('-', 1) > 0)
        return true;
    if (str.indexOf(':', 1) > 0)
        return true;
    return false;
}

Value ValueParser::tryParseDateTime(const QString &str, bool *ok) const
{
    *ok = false;
    if (!containsDateTimeSeparator(str)) {
        return Value(QDateTime(), m_settings);
    }

    Localization *locale = m_settings->locale();
    QDateTime datetime = locale->readDateTime(str, ok);
    // Try without the year
    if (!(*ok)) {
        QString fmt = locale->dateTimeFormat(true);
        fmt = removeYearFromFormat(fmt);
        datetime = locale->readDateTime(str, fmt, ok);
        if (ok) {
            int year = QDate::currentDate().year();
            datetime.setDate(QDate(year, datetime.date().month(), datetime.date().day()));
        }
    }
    if (!(*ok)) {
        QString fmt = locale->dateTimeFormat(false);
        fmt = removeYearFromFormat(fmt);
        datetime = locale->readDateTime(str, fmt, ok);
        if (ok) {
            int year = QDate::currentDate().year();
            datetime.setDate(QDate(year, datetime.date().month(), datetime.date().day()));
        }
    }
#if 0
    // TODO review this
    // This returns a valid datetime even if time component is missing
    // which means we get a fmt_DateTime Value instead of the expected fmt_Date
    if (!(*ok)) {
        // Still not valid - try to use the standard Qt date parsing, using ISO 8601 format
        datetime = QDateTime::fromString(str, Qt::ISODate);
        if (datetime.isValid())
            *ok = true;
    }
#endif

    if (*ok) {
        // repair two-digit years
        int myYear = datetime.date().year();
        int year = repairYear(myYear, str);
        if (year != myYear)
            datetime.setDate(QDate(year, datetime.date().month(), datetime.date().day()));
    }

    Value v(datetime, m_settings);
    return v;
}

Value ValueParser::tryParseDate(const QString &str, bool *ok) const
{
    *ok = false;
    if (!containsDateTimeSeparator(str)) {
        return Value(QDate(), m_settings);
    }

    Localization *locale = m_settings->locale();
    QDate date = locale->readDate(str, ok);
    // Try without the year
    if (!(*ok)) {
        QString fmt = locale->dateFormat(true);
        fmt = removeYearFromFormat(fmt);
        date = locale->readDate(str, fmt, ok);
        if (ok) {
            int year = QDate::currentDate().year();
            date.setDate(year, date.month(), date.day());
        }
    }
    if (!(*ok)) {
        QString fmt = locale->dateFormat(false);
        fmt = removeYearFromFormat(fmt);
        date = locale->readDate(str, fmt, ok);
        if (ok) {
            int year = QDate::currentDate().year();
            date.setDate(year, date.month(), date.day());
        }
    }

    if (!(*ok)) {
        // Still not valid - try to use the standard Qt date parsing, using ISO 8601 format
        date = QDate::fromString(str, Qt::ISODate);
        if (date.isValid())
            *ok = true;
    }

    if (*ok) {
        int year = repairYear(date.year(), str);
        if (year != date.year())
            date.setDate(year, date.month(), date.day());
    }
    Value v(date, m_settings);
    return v;
}

Value ValueParser::tryParseTime(const QString &str, bool *ok) const
{
    if (ok)
        *ok = false;
    if (!containsDateTimeSeparator(str)) {
        return Value(); // Should an invalid fmt_Time be returned?
    }

    const Localization *locale = m_settings->locale();
    Time time = locale->readTime(str, ok);
    Value v(time);
    return v;
}
