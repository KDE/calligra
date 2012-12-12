/* This file is part of the KDE project
   Copyright (C) 2006-2011 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexidatetimeformatter.h"

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdatepicker.h>
#include <kdatetable.h>
#include <klineedit.h>
#include <kmenu.h>
#include <kdatewidget.h>


class KexiDateFormatter::Private
{
public:
    Private() {}

    //! Input mask generated using the formatter settings. Can be used in QLineEdit::setInputMask().
    QString inputMask;

    //! Order of date sections
    Order order;

    //! 4 or 2 digits
    bool longYear;

    bool monthWithLeadingZero, dayWithLeadingZero;

    //! Date format used in toString()
    QString qtFormat;

    //! Used in fromString(const QString&) to convert string back to QDate
    int yearpos, monthpos, daypos;

    QString separator;
};

class KexiTimeFormatter::Private
{
public:
    Private()
        : hmsRegExp(new QRegExp(
            QLatin1String("(\\d*):(\\d*):(\\d*).*( am| pm){,1}"), Qt::CaseInsensitive))
        , hmRegExp(new QRegExp(
            QLatin1String("(\\d*):(\\d*).*( am| pm){,1}"), Qt::CaseInsensitive))
    {
    }

    ~Private()
    {
        delete hmsRegExp;
        delete hmRegExp;
    }

    //! Input mask generated using the formatter settings. Can be used in QLineEdit::setInputMask().
    QString inputMask;

//  //! Order of date sections
//  QDateEdit::Order order;

    //! 12 or 12h
    bool is24h;

    bool hoursWithLeadingZero;

    //! Time format used in toString(). Notation from KLocale::setTimeFormat() is used.
    QString outputFormat;

    //! Used in fromString(const QString&) to convert string back to QTime
    int hourpos, minpos, secpos, ampmpos;

    QRegExp *hmsRegExp, *hmRegExp;
};

KexiDateFormatter::KexiDateFormatter()
  : d(new Private)
{
    // use "short date" format system settings
//! @todo allow to override the format using column property and/or global app settings
    QString df(KGlobal::locale()->dateFormatShort());
    if (df.length() > 2)
        d->separator = df.mid(2, 1);
    else
        d->separator = "-";
    const int separatorLen = d->separator.length();
    QString yearMask("9999");
    QString yearDateFormat("yyyy");
    QString monthDateFormat("MM");
    QString dayDateFormat("dd"); //for setting up d->dateFormat
    bool ok = df.length() >= 8;
    int yearpos, monthpos, daypos; //result of df.find()
    if (ok) {//look at % variables
//! @todo more variables are possible here, see void KLocale::setDateFormatShort() docs
//!       http://developer.kde.org/documentation/library/3.5-api/kdelibs-apidocs/kdecore/html/classKLocale.html#a59
        yearpos = df.indexOf("%y", 0, Qt::CaseInsensitive); //&y or %y
        d->longYear = !(yearpos >= 0 && df.mid(yearpos + 1, 1) == "y");
        if (!d->longYear) {
            yearMask = "99";
            yearDateFormat = "yy";
        }
        monthpos = df.indexOf("%m", 0, Qt::CaseSensitive); //%m or %n
        d->monthWithLeadingZero = true;
        if (monthpos < 0) {
            monthpos = df.indexOf("%n", 0, Qt::CaseInsensitive);
            d->monthWithLeadingZero = false;
            monthDateFormat = "M";
        }
        daypos = df.indexOf("%d", 0, Qt::CaseSensitive);//%d or %e
        d->dayWithLeadingZero = true;
        if (daypos < 0) {
            daypos = df.indexOf("%e", 0, Qt::CaseInsensitive);
            d->dayWithLeadingZero = false;
            dayDateFormat = "d";
        }
        ok = (yearpos >= 0 && monthpos >= 0 && daypos >= 0);
    }
    d->order = YMD; //default
    if (ok) {
        if (yearpos < monthpos && monthpos < daypos) {
            //will be set in "default: YMD"
        } else if (yearpos < daypos && daypos < monthpos) {
            d->order = YDM;
//! @todo use QRegExp (to replace %Y by %1, etc.) instead of hardcoded "%1%299%399"
//!       because df may contain also other characters
            d->inputMask = yearMask + d->separator + QLatin1String("99") + d->separator + QLatin1String("99");
            d->qtFormat = yearDateFormat + d->separator + dayDateFormat + d->separator + monthDateFormat;
            d->yearpos = 0;
            d->daypos = yearMask.length() + separatorLen;
            d->monthpos = d->daypos + 2 + separatorLen;
        } else if (daypos < monthpos && monthpos < yearpos) {
            d->order = DMY;
            d->inputMask = QLatin1String("99") + d->separator + QLatin1String("99") + d->separator + yearMask;
            d->qtFormat = dayDateFormat + d->separator + monthDateFormat + d->separator + yearDateFormat;
            d->daypos = 0;
            d->monthpos = 2 + separatorLen;
            d->yearpos = d->monthpos + 2 + separatorLen;
        } else if (monthpos < daypos && daypos < yearpos) {
            d->order = MDY;
            d->inputMask = QLatin1String("99") + d->separator + QLatin1String("99") + d->separator + yearMask;
            d->qtFormat = monthDateFormat + d->separator + dayDateFormat + d->separator + yearDateFormat;
            d->monthpos = 0;
            d->daypos = 2 + separatorLen;
            d->yearpos = d->daypos + 2 + separatorLen;
        } else
            ok = false;
    }
    if (!ok || d->order == YMD) {//default: YMD
        d->inputMask = yearMask + d->separator + QLatin1String("99") + d->separator + QLatin1String("99");
        d->qtFormat = yearDateFormat + d->separator + monthDateFormat + d->separator + dayDateFormat;
        d->yearpos = 0;
        d->monthpos = yearMask.length() + separatorLen;
        d->daypos = d->monthpos + 2 + separatorLen;
    }
    d->inputMask += ";_";
}

KexiDateFormatter::~KexiDateFormatter()
{
    delete d;
}

QDate KexiDateFormatter::fromString(const QString& str) const
{
    bool ok = true;
    int year = str.mid(d->yearpos, d->longYear ? 4 : 2).toInt(&ok);
    if (!ok)
        return QDate();
    if (year < 30) {//2000..2029
        year = 2000 + year;
    } else if (year < 100) {//1930..1999
        year = 1900 + year;
    }

    int month = str.mid(d->monthpos, 2).toInt(&ok);
    if (!ok)
        return QDate();

    int day = str.mid(d->daypos, 2).toInt(&ok);
    if (!ok)
        return QDate();

    QDate date(year, month, day);
    if (!date.isValid())
        return QDate();
    return date;
}

QVariant KexiDateFormatter::stringToVariant(const QString& str) const
{
    if (isEmpty(str))
        return QVariant();
    const QDate date(fromString(str));
    if (date.isValid())
        return date;
    return QVariant();
}

bool KexiDateFormatter::isEmpty(const QString& str) const
{
    QString s(str);
    return s.remove(d->separator).trimmed().isEmpty();
}

QString KexiDateFormatter::inputMask() const
{
    return d->inputMask;
}

QString KexiDateFormatter::separator() const
{
    return d->separator;
}

QString KexiDateFormatter::toString(const QDate& date) const
{
    return date.toString(d->qtFormat);
}

//------------------------------------------------

KexiTimeFormatter::KexiTimeFormatter()
        : d(new Private)
{
    QString tf(KGlobal::locale()->timeFormat());
    //d->hourpos, d->minpos, d->secpos; are result of tf.indexOf()
    QString hourVariable, minVariable, secVariable;

    //detect position of HOUR section: find %H or %k or %I or %l
    d->is24h = true;
    d->hoursWithLeadingZero = true;
    d->hourpos = tf.indexOf("%H", 0, Qt::CaseSensitive);
    if (d->hourpos >= 0) {
        d->is24h = true;
        d->hoursWithLeadingZero = true;
    } else {
        d->hourpos = tf.indexOf("%k", 0, Qt::CaseSensitive);
        if (d->hourpos >= 0) {
            d->is24h = true;
            d->hoursWithLeadingZero = false;
        } else {
            d->hourpos = tf.indexOf("%I", 0, Qt::CaseSensitive);
            if (d->hourpos >= 0) {
                d->is24h = false;
                d->hoursWithLeadingZero = true;
            } else {
                d->hourpos = tf.indexOf("%l", 0, Qt::CaseSensitive);
                if (d->hourpos >= 0) {
                    d->is24h = false;
                    d->hoursWithLeadingZero = false;
                }
            }
        }
    }
    d->minpos = tf.indexOf("%M", 0, Qt::CaseSensitive);
    d->secpos = tf.indexOf("%S", 0, Qt::CaseSensitive); //can be -1
    d->ampmpos = tf.indexOf("%p", 0, Qt::CaseSensitive); //can be -1

    if (d->hourpos < 0 || d->minpos < 0) {
        //set default: hr and min are needed, sec are optional
        tf = "%H:%M:%S";
        d->is24h = true;
        d->hoursWithLeadingZero = false;
        d->hourpos = 0;
        d->minpos = 3;
        d->secpos = d->minpos + 3;
        d->ampmpos = -1;
    }
    hourVariable = tf.mid(d->hourpos, 2);

    d->inputMask = tf;
// d->inputMask.replace( hourVariable, "00" );
// d->inputMask.replace( "%M", "00" );
// d->inputMask.replace( "%S", "00" ); //optional
    d->inputMask.replace(hourVariable, "99");
    d->inputMask.replace("%M", "99");
    d->inputMask.replace("%S", "00");   //optional
    d->inputMask.replace("%p", "AA");   //am or pm
    d->inputMask += ";_";

    d->outputFormat = tf;
}

KexiTimeFormatter::~KexiTimeFormatter()
{
    delete d;
}

QTime KexiTimeFormatter::fromString(const QString& str) const
{
    int hour, min, sec;
    bool pm = false;

    bool tryWithoutSeconds = true;
    if (d->secpos >= 0) {
        if (-1 != d->hmsRegExp->indexIn(str)) {
            hour = d->hmsRegExp->cap(1).toInt();
            min = d->hmsRegExp->cap(2).toInt();
            sec = d->hmsRegExp->cap(3).toInt();
            if (d->ampmpos >= 0 && d->hmsRegExp->numCaptures() > 3)
                pm = d->hmsRegExp->cap(4).trimmed().toLower() == "pm";
            tryWithoutSeconds = false;
        }
    }
    if (tryWithoutSeconds) {
        if (-1 == d->hmRegExp->indexIn(str))
            return QTime(99, 0, 0);
        hour = d->hmRegExp->cap(1).toInt();
        min = d->hmRegExp->cap(2).toInt();
        sec = 0;
        if (d->ampmpos >= 0 && d->hmRegExp->numCaptures() > 2)
            pm = d->hmsRegExp->cap(4).toLower() == "pm";
    }

    if (pm && hour < 12)
        hour += 12; //PM
    return QTime(hour, min, sec);
}

QVariant KexiTimeFormatter::stringToVariant(const QString& str)
{
    if (isEmpty(str))
        return QVariant();
    const QTime time(fromString(str));
    if (time.isValid())
        return time;
    return QVariant();
}

bool KexiTimeFormatter::isEmpty(const QString& str) const
{
    QString s(str);
    return s.remove(':').trimmed().isEmpty();
}

QString KexiTimeFormatter::toString(const QTime& time) const
{
    if (!time.isValid())
        return QString();

    QString s(d->outputFormat);
    if (d->is24h) {
        if (d->hoursWithLeadingZero)
            s.replace("%H", QString::fromLatin1(time.hour() < 10 ? "0" : "") + QString::number(time.hour()));
        else
            s.replace("%k", QString::number(time.hour()));
    } else {
        int time12 = (time.hour() > 12) ? (time.hour() - 12) : time.hour();
        if (d->hoursWithLeadingZero)
            s.replace("%I", QString::fromLatin1(time12 < 10 ? "0" : "") + QString::number(time12));
        else
            s.replace("%l", QString::number(time12));
    }
    s.replace("%M", QString::fromLatin1(time.minute() < 10 ? "0" : "") + QString::number(time.minute()));
    if (d->secpos >= 0)
        s.replace("%S", QString::fromLatin1(time.second() < 10 ? "0" : "") + QString::number(time.second()));
    if (d->ampmpos >= 0)
        s.replace("%p", ki18n( time.hour() >= 12 ? "pm" : "am" ).toString( KGlobal::locale() ));
    return s;
}

QString KexiTimeFormatter::inputMask() const
{
    return d->inputMask;
}

//------------------------------------------------

QString KexiDateTimeFormatter::inputMask(const KexiDateFormatter& dateFormatter,
                                       const KexiTimeFormatter& timeFormatter)
{
    QString mask(dateFormatter.inputMask());
    mask.truncate(dateFormatter.inputMask().length() - 2);
    return mask + " " + timeFormatter.inputMask();
}

QDateTime KexiDateTimeFormatter::fromString(
    const KexiDateFormatter& dateFormatter,
    const KexiTimeFormatter& timeFormatter, const QString& str)
{
    QString s(str.trimmed());
    const int timepos = s.indexOf(" ");
    const bool emptyTime = timepos >= 0 && timeFormatter.isEmpty(s.mid(timepos + 1)); //.replace(':',"").trimmed().isEmpty();
    if (emptyTime)
        s = s.left(timepos);
    if (timepos > 0 && !emptyTime) {
        return QDateTime(
                   dateFormatter.fromString(s.left(timepos)),
                   timeFormatter.fromString(s.mid(timepos + 1))
               );
    } else {
        return QDateTime(
                   dateFormatter.fromString(s),
                   QTime(0, 0, 0)
               );
    }
}

QString KexiDateTimeFormatter::toString(const KexiDateFormatter &dateFormatter,
                                        const KexiTimeFormatter &timeFormatter,
                                        const QDateTime &value)
{
    if (value.isValid())
        return dateFormatter.toString(value.date()) + ' '
               + timeFormatter.toString(value.time());
    return QString();
}

bool KexiDateTimeFormatter::isEmpty(const KexiDateFormatter& dateFormatter,
                                    const KexiTimeFormatter& timeFormatter,
                                    const QString& str)
{
    int timepos = str.indexOf(" ");
    const bool emptyTime = timepos >= 0 && timeFormatter.isEmpty(str.mid(timepos + 1)); //s.mid(timepos+1).replace(':',"").trimmed().isEmpty();
    return (timepos >= 0 && dateFormatter.isEmpty(str.left(timepos)) //s.left(timepos).replace(d->dateFormatter.separator(), "").trimmed().isEmpty()
            && emptyTime);
}

bool KexiDateTimeFormatter::isValid(const KexiDateFormatter& dateFormatter,
                                    const KexiTimeFormatter& timeFormatter, const QString& str)
{
    int timepos = str.indexOf(" ");
    const bool emptyTime = timepos >= 0 && timeFormatter.isEmpty(str.mid(timepos + 1)); //s.mid(timepos+1).replace(':',"").trimmed().isEmpty();
    if (timepos >= 0 && dateFormatter.isEmpty(str.left(timepos)) // s.left(timepos).replace(d->dateFormatter.separator(), "").trimmed().isEmpty()
            && emptyTime)
        //empty date/time is valid
        return true;
    return timepos >= 0 && dateFormatter.fromString(str.left(timepos)).isValid()
           && (emptyTime /*date without time is also valid*/ || timeFormatter.fromString(str.mid(timepos + 1)).isValid());
}
