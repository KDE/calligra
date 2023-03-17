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

#ifndef CALLIGRA_SHEETS_FORMATENUM
#define CALLIGRA_SHEETS_FORMATENUM

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Style
 * Collection of string formatting enumeration.
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

    // Note: Keep these and Localization::dateFormat() in sync.
    // Group theese in short- and text dates, the sequence is also used in the ui.
    DatesBegin = 200,   ///< \internal, used for iterating
    ShortDate = DatesBegin, ///< Short (numeric) date format, e.g. 08-04-2007
    Date1,
    Date2,
    Date3,
    Date4,
    Date5,            ///< Week number
    TextDate,         ///< Long (text) date format, e.g. Sunday 08 April 2007
    Date6,
    Date7,
    Date8,
    DatesEnd = Date8, ///< \internal, used for iterating

    DateTime = 40,      ///< e.g. 08-04-2007 11:02 (locale specific)

    // Note: Keep these and Localization::timeFormat() in sync.
    TimesBegin = 50,    ///< \internal, used for iteration
    ShortTime = TimesBegin,  ///< Short time format without seconds, e.g. 11:02
    LongTime,        ///< Long time format with seconds, e.g. 11:02:45
    Time1 = 52,         ///< e.g. 9:01 AM
    Time2 = 53,         ///< e.g. 9:01:05 AM
    Time3 = 54,         ///< e.g. 9 h 01 min 28 s
    Time4 = 55,         ///< e.g. 9:01
    Time5 = 56,         ///< e.g. 9:01:12
    Time6 = 57,         ///< [mm]:ss
    Time7 = 58,         ///< [h]:mm:ss
    Time8 = 59,         ///< [h]:mm
    TimesEnd = Time8,   ///< \internal, used for iteration

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

} // namespace Format
} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_FORMATENUM
