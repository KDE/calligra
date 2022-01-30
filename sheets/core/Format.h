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

#ifndef CALLIGRA_SHEETS_FORMAT
#define CALLIGRA_SHEETS_FORMAT

#include "sheets_core_export.h"

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

    ShortDate = 35,     ///< Short (numeric) date format, e.g. 08-04-2007 (locale specific)
    TextDate = 36,      ///< Long (text) date format, e.g. Sunday 08 April 2007 (locale specific)
    DatesBegin = 200,   ///< \internal
    Date1 = 200,
    Date2 = 201,
    Date3 = 202,
    Date4 = 203,
    Date5 = 204,
    Date6 = 205,
    Date7 = 206,
    Date8 = 207,
    DatesEnd = 234,     ///< \internal

    DateTime = 40,      ///< e.g. 08-04-2007 11:02 (locale specific)

    TimesBegin = 50,    ///< \internal
    Time = 50,          ///< Short time format without seconds, e.g. 11:02 (locale specific)
    SecondeTime = 51,   ///< Long time format with seconds, e.g. 11:02:45 (locale specific)
    Time1 = 52,         ///< e.g. 9:01 AM
    Time2 = 53,         ///< e.g. 9:01:05 AM
    Time3 = 54,         ///< e.g. 9 h 01 min 28 s
    Time4 = 55,         ///< e.g. 9:01
    Time5 = 56,         ///< e.g. 9:01:12
    Time6 = 57,         ///< [mm]:ss
    Time7 = 58,         ///< [h]:mm:ss
    Time8 = 59,         ///< [h]:mm
    TimesEnd = 59,      ///< \internal

    FractionsBegin = 70,  ///< \internal
    fraction_half = 70,
    fraction_quarter = 71,
    fraction_eighth = 72,
    fraction_sixteenth = 73,
    fraction_tenth = 74,
    fraction_hundredth = 75,
    fraction_one_digit = 76,
    fraction_two_digits = 77,
    fraction_three_digits = 78,
    FractionsEnd = 78,  ///< \internal

    Custom = 300,
    None = 400
};

//helper functions for the formatting
CALLIGRA_SHEETS_CORE_EXPORT bool isDate(Type type);
CALLIGRA_SHEETS_CORE_EXPORT bool isTime(Type type);
CALLIGRA_SHEETS_CORE_EXPORT bool isFraction(Type type);
CALLIGRA_SHEETS_CORE_EXPORT bool isMoney(Type type);
CALLIGRA_SHEETS_CORE_EXPORT bool isNumber(Type type);

} // namespace Format
} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_FORMAT
