/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2004 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 1998-2004 KSpread Team <calligra-devel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ValueFormatter.h"

#include "CalculationSettings.h"
#include "Localization.h"
#include "ValueConverter.h"

// #include <kcalendarsystem.h>
// #include <KLocalizedString>

#include <float.h>
// #include <math.h>

using namespace Calligra::Sheets;

ValueFormatter::ValueFormatter(const ValueConverter* converter)
        : m_converter(converter)
{
}

const CalculationSettings* ValueFormatter::settings() const
{
    return m_converter->settings();
}

Value ValueFormatter::formatText(const Value &value, Format::Type fmtType, int precision,
                                 Style::FloatFormat floatFormat, const QString &prefix,
                                 const QString &postfix, const QString &currencySymbol,
                                 const QString &formatString, bool thousandsSep)
{
    if (value.isError())
        return Value(value.errorMessage());

    //if we have an array, use its first element
    if (value.isArray())
        return formatText(value.element(0, 0), fmtType, precision,
                          floatFormat, prefix, postfix, currencySymbol, formatString, thousandsSep);

    Value result;

    //step 1: determine formatting that will be used
    fmtType = determineFormatting(value, fmtType);

    //step 2: format the value !
    bool ok = false;

    //text
    if (fmtType == Format::Text) {
        QString str = m_converter->asString(value).asString();
        if (!str.isEmpty() && str[0] == '\'')
            str = str.mid(1);
        result = Value(str);
        if (value.isBoolean()) {
            result.setFormat(Value::fmt_Boolean);
        }
        ok = true;
    }

    //datetime
    else if (fmtType == Format::DateTime || (Format::isDate(fmtType) && !formatString.isEmpty()) ) {
        Value dateValue = m_converter->asDateTime(value, &ok);
        if (ok) {
            result = Value(dateTimeFormat(dateValue.asDateTime(settings()), fmtType, formatString));
            result.setFormat(Value::fmt_DateTime);
        }
    }

    //
    else if (Format::isDate(fmtType)) {
        Value dateValue = m_converter->asDate(value, &ok);
        if (ok) {
            result = Value(dateFormat(dateValue.asDate(settings()), fmtType, formatString));
            result.setFormat(Value::fmt_Date);
        }
    }

    //time
    else if (Format::isTime(fmtType)) {
        Value timeValue = m_converter->asDateTime(value, &ok);
        if (ok) {
            result = Value(timeFormat(timeValue.asDateTime(settings()), fmtType, formatString));
            result.setFormat(Value::fmt_Time);
        }
    }

    //fraction
    else if (Format::isFraction(fmtType)) {
        Value fractionValue = m_converter->asFloat(value, &ok);
        if (ok) {
            result = Value(fractionFormat(fractionValue.asFloat(), fmtType));
            result.setFormat(Value::fmt_Number);
        }
    }

    //another
    else {
        // complex
        if (value.isComplex()) {
            Value complexValue = m_converter->asComplex(value, &ok);
            if (ok) {
                result = Value(complexFormat(complexValue, precision, fmtType, floatFormat, currencySymbol, thousandsSep));
                result.setFormat(Value::fmt_Number);
            }
        }

        // real number
        else {
            Number number = m_converter->asFloat(value, &ok).asFloat();
            if (ok) {
                result = Value(createNumberFormat(number, precision, fmtType, floatFormat, currencySymbol, formatString, thousandsSep));
                result.setFormat(Value::fmt_Number);
            }
        }
    }

    // Only string values can fail. If so, keep the string.
    if (!ok) {
        QString str = m_converter->asString(value).asString();
        if (!str.isEmpty() && str[0] == '\'')
            str = str.mid(1);
        result = Value(str);
    }

    if (!prefix.isEmpty())
        result = Value(prefix + ' ' + result.asString());

    if (!postfix.isEmpty())
        result = Value(result.asString() + ' ' + postfix);

    //debugSheets <<"ValueFormatter says:" << str;
    return result;
}

Format::Type ValueFormatter::determineFormatting(const Value &value,
        Format::Type fmtType)
{
    //now, everything depends on whether the formatting is Generic or not
    if (fmtType == Format::Generic) {
        //here we decide based on value's format...
        Value::Format fmt = value.format();
        switch (fmt) {
        case Value::fmt_None:
            fmtType = Format::Text;
            break;
        case Value::fmt_Boolean:
            fmtType = Format::Text;
            break;
        case Value::fmt_Number: {
            Number val = fabs(value.asFloat());
            if (((val > 10000e+10) || (val < 10000e-10)) && (val != 0.0))
                fmtType = Format::Scientific;
            else
                fmtType = Format::Number;
            }
            break;
        case Value::fmt_Percent:
            fmtType = Format::Percentage;
            break;
        case Value::fmt_Money:
            fmtType = Format::Money;
            break;
        case Value::fmt_DateTime:
            fmtType = Format::DateTime;
            break;
        case Value::fmt_Date:
            fmtType = Format::ShortDate;
            break;
        case Value::fmt_Time:
            fmtType = Format::Time8; // [h]:mm
            break;
        case Value::fmt_String:
            //this should never happen
            fmtType = Format::Text;
            break;
        };
        return fmtType;
    } else {
        //we'll mostly want to use the given formatting, the only exception
        //being Boolean values

        //TODO: is this correct? We may also want to convert bools to 1s and 0s
        //if we want to display a number...

        //TODO: what to do about Custom formatting? We don't support it as of now,
        //  but we'll have it ... one day, that is ...
        if (value.isBoolean())
            return Format::Text;
        else
            return fmtType;
    }
}


QString ValueFormatter::removeTrailingZeros(const QString& str, const QString &currencySymbol, const QString& decimalSymbol)
{
    if (!str.contains(decimalSymbol))
        //no decimal symbol -> nothing to do
        return str;

    int start = 0;
    int cslen = currencySymbol.length();
    if (str.indexOf('%') != -1)
        start = 2;
    else if (str.indexOf(currencySymbol) ==
             ((int)(str.length() - cslen)))
        start = cslen + 1;
    else if ((start = str.indexOf('E')) != -1)
        start = str.length() - start;
    else
        start = 0;

    QString result = str;
    int i = str.length() - start;
    bool bFinished = false;
    while (!bFinished && i > 0) {
        QChar ch = result[i - 1];
        if (ch == '0')
            result.remove(--i, 1);
        else {
            bFinished = true;
            if (result.mid(i - decimalSymbol.length(), decimalSymbol.length()) == decimalSymbol)
                result.remove(i - decimalSymbol.length(), decimalSymbol.length());
        }
    }
    return result;
}

QString ValueFormatter::createNumberFormat(Number value, int precision,
        Format::Type fmt, Style::FloatFormat floatFormat, const QString& currencySymbol,
        const QString& _formatString, bool thousandsSep)
{
    QString prefix, postfix;
    QString formatString(_formatString);

    int numExpDigits = 0; // QString::number() will be used
    // try to split formatstring into prefix, formatstring and postfix.
    if (!formatString.isEmpty() ) {
        QRegExp re( QLatin1String( "^([^0#.,E+]*)([0#.,E+]*)(.*)$" ) );
        if( re.exactMatch( formatString ) ) {
            prefix = re.cap( 1 );
            formatString = re.cap( 2 );
            postfix = re.cap( 3 );
        }
        if (formatString.isEmpty()) {
            return prefix + postfix;
        } else if (formatString.contains(QLatin1Char('.'))) {
            // if it contains an 'E', precision is zeros between '.' and 'E'
            int len = formatString.indexOf(QLatin1Char('E'));
            if (len == -1) {
                len = formatString.length();
            }
            precision = len - formatString.indexOf(QLatin1Char('.')) - 1;
        } else if (precision != -1) {
            precision = 0;
        }
        if (formatString.contains("E+")) {
            numExpDigits = formatString.length() - formatString.indexOf("E+") - 2;
        }
    }

    int p = precision;
    if (p == -1) {
        // If precision (obtained from the cell style) is -1 (arbitrary), use the document default decimal precision
        // and if that value is -1 too then use either automatic decimal place adjustment or a hardcoded default.
        p = settings()->defaultDecimalPrecision();
        if (p == -1) {
            if (fmt == Format::Number) {
                QString s = QString::number(double(numToDouble(value)));
                int _p = s.indexOf('.');
                p = _p >= 0 ? qMax(0, 10 - _p) : 0;
            } else {
                p = 2; // hardcoded default
            }
        }
    }

    QString localizedNumber;
    int pos = 0;

    // Always unsigned ?
    if ((floatFormat == Style::AlwaysUnsigned) && (value < 0.0))
        value *= -1.0;

    //multiply value by 100 for percentage format
    if (fmt == Format::Percentage)
        value *= 100;

    // round the number, based on desired precision if not scientific is chosen
    //(scientific has relative precision)
    if (fmt != Format::Scientific) {
        // this will avoid displaying negative zero, i.e "-0.0000"
        // TODO: is this really a good solution?
        if (fabs(value) < DBL_EPSILON) value = 0.0;

        double m[] = { 1, 10, 100, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10 };
        double mm = (p > 10) ? ::pow(10.0, p) : m[p];
        bool neg = value < 0;
        value = floor(numToDouble(fabs(value)) * mm + 0.5) / mm;
        if (neg) value = -value;
    }

    double val = numToDouble(value);
    switch (fmt) {
    case Format::Number:
        localizedNumber = m_converter->settings()->locale()->formatNumber(val, p);
        break;
    case Format::Percentage:
        localizedNumber = m_converter->settings()->locale()->formatNumber(val, p);
        if(!postfix.endsWith('%')) // percent formattings needs to end with a "%"-sign
            postfix += '%';
        break;
    case Format::Money:
        localizedNumber = m_converter->settings()->locale()->formatCurrency(val, currencySymbol, p);
        break;
    case Format::Scientific: {
        // TODO: port to QLocale or other (icu?) formatting
        const QString decimalSymbol = m_converter->settings()->locale()->decimalSymbol();
        localizedNumber = QString::number(val, 'E', p);
        if ((pos = localizedNumber.indexOf('.')) != -1) {
            localizedNumber.replace(pos, 1, decimalSymbol);
        }
        if (numExpDigits > 0) {
            // adjust number of exponent digits
            int firstDigit = localizedNumber.indexOf('E') + 2;
            int digits = localizedNumber.length() - firstDigit;
            if (digits > numExpDigits) {
                // remove leading 0 if present
                if (localizedNumber.at(firstDigit) == '0') {
                    localizedNumber.remove(firstDigit, 1);
                }
            } else {
                for (int i = digits; i < numExpDigits; ++i) {
                    localizedNumber.insert(firstDigit, '0');
                }
            }
        }
        break;
    }
    default :
        //other formatting?
        // This happens with Format::Custom...
        debugSheets << "Wrong usage of ValueFormatter::createNumberFormat fmt=" << fmt << "";
        break;
    }

    //prepend positive sign if needed
    if ((floatFormat == Style::AlwaysSigned) && value >= 0)
        if (m_converter->settings()->locale()->positiveSign().isEmpty())
            localizedNumber = '+' + localizedNumber;

    if (fmt != Format::Scientific) {
        // Remove trailing zeros and the decimal point if necessary
        // unless the number has no decimal point
        if (precision == -1) {
            QString decimalSymbol = m_converter->settings()->locale()->decimalSymbol();
            if (decimalSymbol.isNull())
                decimalSymbol = '.';
            QString currency = currencySymbol;
            if (!currency.length()) currency = m_converter->settings()->locale()->currencySymbol();
            localizedNumber = removeTrailingZeros(localizedNumber, currencySymbol, decimalSymbol);
        }

        // Remove thousands separators if necessary
        if (!thousandsSep) {
            const QString separator = m_converter->settings()->locale()->thousandsSeparator();
            if (!separator.isNull()) {
                localizedNumber.remove(separator);
            }
        }
    }

    // remove negative sign if prefix already ends with '-'
    if (!prefix.isEmpty() && prefix[prefix.length()-1] == '-' && !localizedNumber.isEmpty() && localizedNumber[0] == '-') {
        localizedNumber = localizedNumber.mid(1);
    }

    return prefix + localizedNumber + postfix;
}

QString ValueFormatter::fractionFormat(Number value, Format::Type fmtType)
{
    bool isNegative = value < 0;
    QString prefix = isNegative ? "-" : "";
    value = abs(value);
    Number result = value - floor(numToDouble(value));
    int index;
    int limit = 0;

    /* return w/o fraction part if not necessary */
    if (result == 0)
        return prefix + QString::number((double) numToDouble(value));

    switch (fmtType) {
    case Format::fraction_half:
        index = 2;
        break;
    case Format::fraction_quarter:
        index = 4;
        break;
    case Format::fraction_eighth:
        index = 8;
        break;
    case Format::fraction_sixteenth:
        index = 16;
        break;
    case Format::fraction_tenth:
        index = 10;
        break;
    case Format::fraction_hundredth:
        index = 100;
        break;
    case Format::fraction_one_digit:
        index = 3;
        limit = 9;
        break;
    case Format::fraction_two_digits:
        index = 4;
        limit = 99;
        break;
    case Format::fraction_three_digits:
        index = 5;
        limit = 999;
        break;
    default:
        debugSheets << "Error in Fraction format";
        return prefix + QString::number((double) numToDouble(value));
        break;
    } /* switch */


    /* handle halves, quarters, tenths, ... */
    if (fmtType != Format::fraction_three_digits
            && fmtType != Format::fraction_two_digits
            && fmtType != Format::fraction_one_digit) {
        Number calc = 0;
        int index1 = 0;
        Number diff = result;
        for (int i = 1; i <= index; i++) {
            calc = i * 1.0 / index;
            if (fabs(result - calc) < diff) {
                index1 = i;
                diff = fabs(result - calc);
            }
        }
        if (index1 == 0) return prefix + QString("%1").arg((double) floor(numToDouble(value)));
        if (index1 == index) return prefix + QString("%1").arg((double) floor(numToDouble(value)) + 1);
        if (floor(numToDouble(value)) == 0)
            return prefix + QString("%1/%2").arg(index1).arg(index);

        return prefix + QString("%1 %2/%3")
               .arg((double) floor(numToDouble(value)))
               .arg(index1)
               .arg(index);
    }


    /* handle Format::fraction_one_digit, Format::fraction_two_digit and Format::fraction_three_digit style */
    double target = numToDouble(result);
    double numerator = 1;
    double denominator = 1;
    double bestNumerator = 0;
    double bestDenominator = 1;
    double bestDist = target;

    // as soon as either numerator or denominator gets above the limit, we're done
    while (numerator <= limit && denominator <= limit) {
        double dist = abs((numerator / denominator) - target);
        if (dist < bestDist) {
            bestDist = dist;
            bestNumerator = numerator;
            bestDenominator = denominator;
        }
        if (numerator / denominator > target) {
            denominator++;
        } else {
            numerator++;
        }
    }

    if (bestNumerator == 0)
        return prefix + QString().setNum((double) floor(numToDouble(value)));
    else if (bestDenominator == bestNumerator)
        return prefix + QString().setNum((double) floor(numToDouble(value + 1)));
    else {
        if (floor(numToDouble(value)) == 0)
            return prefix + QString("%1/%2").arg(bestNumerator).arg(bestDenominator);
        else
            return prefix + QString("%1 %2/%3")
                   .arg((double)floor(numToDouble(value)))
                   .arg(bestNumerator)
                   .arg(bestDenominator);
    }
}

QString ValueFormatter::timeFormat(const QDateTime &_dt, Format::Type fmtType, const QString& formatString)
{
    if (!formatString.isEmpty()) {
        return _dt.toString( formatString );
    }

    const QDateTime dt(_dt.toUTC());
    QString result;
    if (fmtType == Format::Time)
        result = m_converter->settings()->locale()->formatTime(dt.time(), false);
    else if (fmtType == Format::SecondeTime)
        result = m_converter->settings()->locale()->formatTime(dt.time(), true);
    else {
        const int d = settings()->referenceDate().daysTo(dt.date());
        int h, m, s;
        if (fmtType != Format::Time6 && fmtType != Format::Time7 && fmtType != Format::Time8) { // time
            h = dt.time().hour();
            m = dt.time().minute();
            s = dt.time().second();
        } else if (d >= 0) { // positive duration
            h = dt.time().hour() + 24 * d;
            m = dt.time().minute();
            s = dt.time().second();
        } else { // negative duration
            s = (60 - dt.time().second()) % 60;
            m = (60 - dt.time().minute() - ((s == 0) ? 0 : 1)) % 60;
            h = -(dt.time().hour() + 24 * d) - ((m == 0 && s == 0) ? 0 : 1);
        }
        const bool pm = (h > 12);
        const QString sign = d < 0 ? QString('-') : QString("");

        if (fmtType == Format::Time1) {  // 9:01 AM
            result = QString("%1:%2 %3")
                     .arg(QString::number(pm ? h - 12 : h), 1)
                     .arg(QString::number(m), 2, '0')
                     .arg(pm ? i18n("PM") : i18n("AM"));
        } else if (fmtType == Format::Time2) { // 9:01:05 AM
            result = QString("%1:%2:%3 %4")
                     .arg(QString::number(pm ? h - 12 : h), 1)
                     .arg(QString::number(m), 2, '0')
                     .arg(QString::number(s), 2, '0')
                     .arg(pm ? i18n("PM") : i18n("AM"));
        } else if (fmtType == Format::Time3) { // 9 h 01 min 28 s
            result = QString("%1 %2 %3 %4 %5 %6")
                     .arg(QString::number(h), 2, '0')
                     .arg(i18n("h"))
                     .arg(QString::number(m), 2, '0')
                     .arg(i18n("min"))
                     .arg(QString::number(s), 2, '0')
                     .arg(i18n("s"));
        } else if (fmtType == Format::Time4) { // 9:01
            result = QString("%1:%2")
                     .arg(QString::number(h), 1)
                     .arg(QString::number(m), 2, '0');
        } else if (fmtType == Format::Time5) { // 9:01:12
            result = QString("%1:%2:%3")
                     .arg(QString::number(h), 1)
                     .arg(QString::number(m), 2, '0')
                     .arg(QString::number(s), 2, '0');
        } else if (fmtType == Format::Time6) { // [mm]:ss
            result = sign + QString("%1:%2")
                     .arg(QString::number(m + h * 60), 2, '0')
                     .arg(QString::number(s), 2, '0');
        } else if (fmtType == Format::Time7) { // [h]:mm:ss
            result = sign + QString("%1:%2:%3")
                     .arg(QString::number(h), 1)
                     .arg(QString::number(m), 2, '0')
                     .arg(QString::number(s), 2, '0');
        } else if (fmtType == Format::Time8) { // [h]:mm
            result = sign + QString("%1:%2")
                     .arg(QString::number(h), 1)
                     .arg(QString::number(m), 2, '0');
        }
    }
    return result;
}

QString ValueFormatter::dateTimeFormat(const QDateTime &_dt, Format::Type fmtType, const QString& formatString )
{
    if( !formatString.isEmpty() ) {
        if (formatString.contains('X')) {               // if we have the special extra-short month in the format string
            int monthPos = formatString.indexOf('X');
            QString before = formatString.left(monthPos);                               // get string before and after the extra-short month sign
            QString after = formatString.right(formatString.size() - monthPos - 1);
            QString monthShort = _dt.toString("MMM").left(1);                           // format the month as extra-short (only 1st letter)
            return _dt.toString( before ) + monthShort + _dt.toString( after );         // and construct the final date
        }

        return _dt.toString( formatString );
    }

    Q_UNUSED(fmtType);
    QString result;
    // pretty lame, just assuming something for the format
    // TODO: locale-aware formatting
    result += dateFormat(_dt.date(), Format::ShortDate) + ' ' + timeFormat(_dt, Format::Time1);
    return result;
}

QString ValueFormatter::dateFormat(const QDate &date, Format::Type fmtType, const QString& formatString )
{
    Localization *locale = m_converter->settings()->locale();
    if (!formatString.isEmpty())
        return locale->formatDate(date, formatString);

    if (fmtType == Format::ShortDate)
        return locale->formatDate(date, false);

    if (fmtType == Format::TextDate)
        return locale->formatDate(date, true);

    int ftype = 0;
    if (fmtType == Format::Date1) ftype = 1;
    if (fmtType == Format::Date2) ftype = 2;
    if (fmtType == Format::Date3) ftype = 3;
    if (fmtType == Format::Date4) ftype = 4;
    if (fmtType == Format::Date5) ftype = 5;
    if (fmtType == Format::Date6) ftype = 6;
    if (fmtType == Format::Date7) ftype = 7;
    if (fmtType == Format::Date8) ftype = 8;
    if (ftype) {
        QString format = locale->dateFormat(ftype);
        return locale->formatDate(date, ftype);
    }

    // fallback
    return locale->formatDate(date, false);
}

QString ValueFormatter::complexFormat(const Value& value, int precision,
                                      Format::Type formatType,
                                      Style::FloatFormat floatFormat,
                                      const QString& currencySymbol,
                                      bool thousandsSep)
{
    // FIXME Stefan: percentage, currency and scientific formats!
    QString str;
    const Number real = value.asComplex().real();
    const Number imag = value.asComplex().imag();
    str = createNumberFormat(real, precision, formatType, floatFormat, QString(), QString(), thousandsSep);
    str += createNumberFormat(imag, precision, formatType, Style::AlwaysSigned, currencySymbol, QString(), thousandsSep);
    str += 'i';
    return str;
}
