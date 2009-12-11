/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2004 Tomas Mecir <mecirt@gmail.com>
   Copyright 1998,1999 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Local
#include "ValueParser.h"

#include "CalculationSettings.h"
#include "Localization.h"
#include "Style.h"
#include "Value.h"

using namespace KSpread;

ValueParser::ValueParser(const CalculationSettings* settings)
        : m_settings(settings)
{
}

const CalculationSettings* ValueParser::settings() const
{
    return m_settings;
}

Value ValueParser::parse(const QString& str) const
{
    Value val;

    // If the text is empty, we don't have a value
    // If the user stated explicitly that he wanted text
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

    // Test for money number
    Number money = m_settings->locale()->readMoney(strStripped, &ok);
    if (ok) {
        val = Value(money);
        val.setFormat(Value::fmt_Money);
        return val;
    }

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

Value ValueParser::tryParseBool(const QString& str, bool *ok) const
{
    Value val;
    if (ok) *ok = false;

    const QString& lowerStr = str.toLower();

    if ((lowerStr == "true") ||
            (lowerStr == ki18n("true").toString(m_settings->locale()).toLower())) {
        val = Value(true);
        if (ok) *ok = true;
    } else if ((lowerStr == "false") ||
               (lowerStr == ki18n("false").toString(m_settings->locale()).toLower())) {
        val = Value(false);
        if (ok) *ok = true;
    }
    return val;
}

Value ValueParser::readNumber(const QString& _str, bool *ok) const
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
    int EPos  = str.indexOf('E', 0, Qt::CaseInsensitive);

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
    } else if (((pos = str.indexOf(' ')) != -1) &&
               ((fracPos = str.indexOf('/')) != -1)) {
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
        minor = QString::number(minorVal, 'f').mid(2);     // chop off the "0." part
        // kDebug() <<"fraction:" << major <<"." << minor;
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
                || pos == 0          // Can't start with a separator
                || (lastpos > 0 && pos - lastpos != 3)) { // Must have exactly 3 digits between two separators
            if (ok) *ok = false;
            return Value();
        }

        lastpos = pos;
        major.remove(pos, thlen);
    }
    if (lastpos > 0 && major.length() - lastpos != 3) { // Must have exactly 3 digits after the last separator
        if (ok) *ok = false;
        return Value();
    }

    // log10(2^63) ~= 18
    if (isInt && major.length() > 19) isInt = false;

    QString tot;
    if (neg) tot = '-';
    tot += major;
    if (!isInt) tot += '.' + minor + exponentialPart;

    return isInt ? Value(tot.toLongLong(ok)) : Value(tot.toDouble(ok));
}

Number ValueParser::readImaginary(const QString& str, bool* ok) const
{
    if (str.isEmpty()) {
        if (ok) *ok = false;
        return 0.0;
    }

    Number imag = 0.0;
    if (str[0] == 'i' || str[0] == 'j') {
        if (str.length() == 1) {
            if (ok) *ok = true;
            imag = 1.0;
        } else
            imag = readNumber(str.mid(1), ok).asFloat();
    } else if (str[str.length()-1] == 'i' || str[str.length()-1] == 'j') {
        const QString minus(m_settings->locale()->negativeSign());
        if (str.length() == 2 && str[0] == '+') {
            if (ok) *ok = true;
            imag = 1.0;
        } else if (str.length() == minus.length() + 1 && str.left(minus.length()) == minus) {
            if (ok) *ok = true;
            imag = -1.0;
        } else
            imag = readNumber(str.left(str.length() - 1), ok).asFloat();
    } else
        *ok = false;
    return imag;
}

Value ValueParser::tryParseNumber(const QString& str, bool *ok) const
{
    Value value;
    if (str.endsWith('%')) {   // percentage
        const Number val = readNumber(str.left(str.length() - 1).trimmed(), ok).asFloat();
        if (*ok) {
            //kDebug(36001) <<"ValueParser::tryParseNumber '" << str <<
            //    "' successfully parsed as percentage: " << val << '%' << endl;
            value = Value(val / 100.0);
            value.setFormat(Value::fmt_Percent);
        }
    } else if (str.count('i') == 1 || str.count('j') == 1) {    // complex number
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

Value ValueParser::tryParseDate(const QString& str, bool *ok) const
{
    bool valid = false;
    QDate tmpDate = m_settings->locale()->readDate(str, &valid);
    if (!valid) {
        // Try without the year
        // The tricky part is that we need to remove any separator around the year
        // For instance %Y-%m-%d becomes %m-%d and %d/%m/%Y becomes %d/%m
        // If the year is in the middle, say %m-%Y/%d, we'll remove the sep.
        // before it (%m/%d).
        QString fmt = m_settings->locale()->dateFormatShort();
        int yearPos = fmt.indexOf("%Y", 0, Qt::CaseInsensitive);
        if (yearPos > -1) {
            if (yearPos == 0) {
                fmt.remove(0, 2);
                while (fmt[0] != '%')
                    fmt.remove(0, 1);
            } else {
                fmt.remove(yearPos, 2);
                for (; yearPos > 0 && fmt[yearPos-1] != '%'; --yearPos)
                    fmt.remove(yearPos, 1);
            }
            //kDebug(36001) <<"Cell::tryParseDate short format w/o date:" << fmt;
            tmpDate = m_settings->locale()->readDate(str, fmt, &valid);
        }
    }
    if (valid) {
        // Note: if shortdate format only specifies 2 digits year, then 3/4/1955
        // will be treated as in year 3055, while 3/4/55 as year 2055
        // (because 55 < 69, see KLocale) and thus there's no way to enter for
        // year 1995

        // The following fixes the problem, 3/4/1955 will always be 1955

        QString fmt = m_settings->locale()->dateFormatShort();
        if ((fmt.contains("%y") == 1) && (tmpDate.year() > 2999))
            tmpDate = tmpDate.addYears(-1900);

        // this is another HACK !
        // with two digit years, 0-69 is treated as year 2000-2069 (see KLocale)
        // however, in Excel only 0-29 is year 2000-2029, 30 or later is 1930
        // onwards

        // the following provides workaround for KLocale so we're compatible
        // with Excel
        // (e.g 3/4/45 is Mar 4, 1945 not Mar 4, 2045)
        if ((tmpDate.year() >= 2030) && (tmpDate.year() <= 2069)) {
            QString yearFourDigits = QString::number(tmpDate.year());
            QString yearTwoDigits = QString::number(tmpDate.year() % 100);

            // if year is 2045, check to see if "2045" isn't there --> actual
            // input is "45"
            if ((str.count(yearTwoDigits) >= 1) &&
                    (str.count(yearFourDigits) == 0))
                tmpDate = tmpDate.addYears(-100);
        }
    }
    if (!valid) {
        //try to use the standard Qt date parsing, using ISO 8601 format
        tmpDate = QDate::fromString(str, Qt::ISODate);
        if (tmpDate.isValid()) {
            valid = true;
        }
    }

    if (ok)
        *ok = valid;

    return Value(tmpDate, m_settings);
}

Value ValueParser::tryParseTime(const QString& str, bool *ok) const
{
    bool valid = false;

    QDateTime tmpTime = readTime(str, true, &valid);
    if (!valid)
        tmpTime = readTime(str, false, &valid);

    if (!valid) {
        const QString stringPm = ki18n("pm").toString(m_settings->locale());
        const QString stringAm = ki18n("am").toString(m_settings->locale());
        int pos = 0;
        if ((pos = str.indexOf(stringPm, 0, Qt::CaseInsensitive)) != -1) {
            // cut off 'PM'
            QString tmp = str.mid(0, str.length() - stringPm.length());
            tmp = tmp.simplified();
            // try again
            tmpTime = readTime(tmp, true, &valid);
            if (!valid)
                tmpTime = readTime(tmp, false, &valid);
            if (valid && tmpTime.time().hour() > 11)
                valid = false;
            else if (valid)
                tmpTime = tmpTime.addSecs(43200); // add 12 hours
        } else if ((pos = str.indexOf(stringAm, 0, Qt::CaseInsensitive)) != -1) {
            // cut off 'AM'
            QString tmp = str.mid(0, str.length() - stringAm.length());
            tmp = tmp.simplified();
            // try again
            tmpTime = readTime(tmp, true, &valid);
            if (!valid)
                tmpTime = readTime(tmp, false, &valid);
            if (valid && tmpTime.time().hour() > 11)
                valid = false;
        }
    }

    if (ok)
        *ok = valid;
    Value value;
    if (valid) {
        value = Value(tmpTime, m_settings);
        value.setFormat(Value::fmt_Time);
    }
    return value;
}

QDateTime ValueParser::readTime(const QString& intstr, bool withSeconds, bool* ok) const
{
    QString str = intstr.simplified().toLower();
    QString format = m_settings->locale()->timeFormat().simplified();
    if (!withSeconds) {
        int n = format.indexOf("%S");
        format = format.left(n - 1);
    }

    QDateTime result;
    int hour = 0;
    int minute = 0;
    int second = 0;
    int msecs = 0;
    bool g_12h = false;
    bool pm = false;
    bool negative = false;
    uint strpos = 0;
    uint formatpos = 0;

    const uint l  = format.length();
    const uint sl = str.length();

    while (l > formatpos || sl > strpos) {
        if (!(l > formatpos && sl > strpos))
            goto error;

        QChar c(format.at(formatpos++));

        if (c != '%') {
            if (c.isSpace())
                ++strpos;
            else if (c != str.at(strpos++))
                goto error;
            continue;
        }

        // remove space at the beginning
        if (sl > strpos && str.at(strpos).isSpace())
            ++strpos;

        c = format.at(formatpos++);
        switch (c.toLatin1()) {
        case 'p': {
            QString s(ki18n("pm").toString(m_settings->locale()).toLower());
            int len = s.length();
            if (str.mid(strpos, len) == s) {
                pm = true;
                strpos += len;
            } else {
                s = ki18n("am").toString(m_settings->locale()).toLower();
                len = s.length();
                if (str.mid(strpos, len) == s) {
                    pm = false;
                    strpos += len;
                } else
                    goto error;
            }
        }
        break;

        case 'k':
        case 'H':
            g_12h = false;
            if (str.at(strpos) == '-') {
                negative = true;
                if (sl <= ++strpos)
                    goto error;
            }
            hour = readInt(str, strpos);
            if (hour < 0)
                goto error;

            break;

        case 'l':
        case 'I':
            g_12h = true;
            if (str.at(strpos) == '-') {
                negative = true;
                if (sl <= ++strpos)
                    goto error;
            }
            hour = readInt(str, strpos);
            if (hour < 1 || hour > 12)
                goto error;

            break;

        case 'M':
            minute = readInt(str, strpos);
            if (minute < 0 || minute > 59)
                goto error;

            break;

        case 'S':
            if (!withSeconds)
                break;
            second = readInt(str, strpos);
            if (second < 0 || second > 59)
                goto error;
            if (strpos < sl && str.indexOf(m_settings->locale()->decimalSymbol()) == (int)strpos) {
                strpos += m_settings->locale()->decimalSymbol().length();
                msecs = readInt(str, strpos);
                if (msecs < 0 || msecs > 999)
                    goto error;
            }

            break;
        }
    }

    if (g_12h) {
        hour %= 12;
        if (pm) hour += 12;
    }

    if (ok)
        *ok = true;
    result = QDateTime(m_settings->referenceDate(), QTime(0, 0), Qt::UTC);
    msecs += (((hour * 60 + minute) * 60 + second) * 1000);
    result = result.addMSecs(negative ? -msecs : msecs);
    return result;

error:
    if (ok)
        *ok = false;
    // return invalid date if it didn't work
    return QDateTime(m_settings->referenceDate(), QTime(-1, -1, -1), Qt::UTC);
}

/**
 * helper function to read integers, used in readTime
 * @param str
 * @param pos the position to start at. It will be updated when we parse it.
 * @return the integer read in the string, or -1 if no string
 */
int ValueParser::readInt(const QString& str, uint& pos) const
{
    if (!str.at(pos).isDigit())
        return -1;
    int result = 0;
    for (; (uint) str.length() > pos && str.at(pos).isDigit(); pos++) {
        result *= 10;
        result += str.at(pos).digitValue();
    }

    return result;
}

