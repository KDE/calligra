/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             2006 Fredrik Edemar <f_edemar@linux.se>
             2005-2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
             2004 Tomas Mecir <mecirt@gmail.com>
             2003 Norbert Andres <nandres@web.de>
             2002 Philipp Mueller <philipp.mueller@gmx.de>
             2000 David Faure <faure@kde.org>
             2000 Werner Trobin <trobin@kde.org>
             2000-2006 Laurent Montel <montel@kde.org>
             1999, 2000 Torben Weis <weis@kde.org>
             1999 Stephan Kulow <coolo@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_FORMAT_H
#define CALLIGRA_SHEETS_FORMAT_H

#include "sheets_engine_export.h"

#include <QObject>

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Style
 * Collection of string formatting enumeration and functions.
 */
namespace Format
{

enum Type {
    Generic = 0,
    Number = 1,
    Text = 5,
    Money = 10,
    Percentage = 25,
    Scientific = 30,

    FractionsBegin = 70, ///< \internal
    fraction_half = 70,
    fraction_quarter = 71,
    fraction_eighth = 72,
    fraction_sixteenth = 73,
    fraction_tenth = 74,
    fraction_hundredth = 75,
    fraction_one_digit = 76,
    fraction_two_digits = 77,
    fraction_three_digits = 78,
    FractionsEnd = 78, ///< \internal

    TimesBegin = 100, ///< \internal
    Time = TimesBegin, ///< Short time format w/o seconds and w/o am/pm, e.g. 01:02
    SecondeTime, ///< Long time format with seconds and w/o am/pm, e.g. 01:02:45
    ShortTimeAP, ///< Always with am/pm e.g. 09:01 AM
    LongTimeAP, ///< Always with am/pm e.g. 09:01:30 AM
    Time1, ///< e.g. 9:01
    Time2, ///< e.g. 9:1
    Time3, ///< e.g. 9:
    Time4, ///< e.g. 9:01:05
    Time5, ///< e.g. 9:1:05
    Time6, ///< e.g. 9:1:5
    TimeMS, ///< 9:1:5.3 (with milliseconds)
    Time1_AP, ///< e.g. 9:01 (am/pm)
    Time2_AP, ///< e.g. 9:1 (am/pm)
    Time3_AP, ///< e.g. 9: (am/pm)
    Time4_AP, ///< e.g. 9:01:05 (am/pm)
    Time5_AP, ///< e.g. 9:1:05 (am/pm)
    Time6_AP, ///< e.g. 9:1:5 (am/pm)
    TimeMS_AP, ///< 9:1:5.3 (with milliseconds, am/pm)
    DurationHourShort, ///< [h]:mm:ss
    DurationHourLong, ///< [h]:mm:ss.z
    DurationMinute, ///< [mm]:ss
    TimeIso, ///< hh:mm:ss.z
    TimesEnd, ///< \internal

    DatesBegin = 200, ///< \internal
    ShortDate = DatesBegin, ///< Short (numeric) date format, e.g. 08-04-2007 (locale specific)
    TextDate, ///< Long (text) date format, e.g. Sunday 08 April 2007 (locale specific)
    Date1,
    Date2,
    Date3,
    Date4,
    Date5,
    Date6,
    Date7,
    DateIso1, ///< long year
    DateIso2, ///< Short year
    DatesEnd, ///< \internal

    DateTimesBegin = DatesEnd, ///< \internal
    DateTimeShort = DateTimesBegin, ///< Short datetime
    DateTimeLong, ///< Long datetime
    DateTimeIsoShort, ///< Iso without ms
    DateTimeIsoLong, ///< Iso with ms
    DateTimesEnd,

    Custom = 300,
    None = 400
};

// helper functions for the formatting
CALLIGRA_SHEETS_ENGINE_EXPORT bool isDateTime(Type type);
CALLIGRA_SHEETS_ENGINE_EXPORT bool isDate(Type type);
CALLIGRA_SHEETS_ENGINE_EXPORT bool isTime(Type type);
CALLIGRA_SHEETS_ENGINE_EXPORT bool isFraction(Type type);
CALLIGRA_SHEETS_ENGINE_EXPORT bool isMoney(Type type);
CALLIGRA_SHEETS_ENGINE_EXPORT bool isNumber(Type type);

} // namespace Format
} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_FORMAT
