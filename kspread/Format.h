/* This file is part of the KDE project
   Copyright 2005-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
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

#ifndef KSPREAD_FORMAT
#define KSPREAD_FORMAT

#include "kspread_export.h"

namespace KSpread
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
    Date1 = 200,        ///< e.g. 18-Feb-99
    Date2 = 201,        ///< e.g. 18-Feb-1999
    Date3 = 202,        ///< e.g. 18-Feb
    Date4 = 203,        ///< e.g. 18-05
    Date5 = 204,        ///< e.g. 18/05/00
    Date6 = 205,        ///< e.g. 18/05/1999
    Date7 = 206,        ///< e.g. Feb-99
    Date8 = 207,        ///< e.g. February-99
    Date9 = 208,        ///< e.g. February-1999
    Date10 = 209,       ///< e.g. F-99
    Date11 = 210,       ///< e.g. 18/Feb
    Date12 = 211,       ///< e.g. 18/02
    Date13 = 212,       ///< e.g. 18/Feb/1999
    Date14 = 213,       ///< e.g. 2000/Feb/18
    Date15 = 214,       ///< e.g. 2000-Feb-18
    Date16 = 215,       ///< e.g. 2000-02-18
    Date17 = 216,       ///< e.g. 2 February 2000
    Date18 = 217,       ///< e.g. 02/18/1999
    Date19 = 218,       ///< e.g. 02/18/99
    Date20 = 219,       ///< e.g. Feb/18/99
    Date21 = 220,       ///< e.g. Feb/18/1999
    Date22 = 221,       ///< e.g. Feb-1999
    Date23 = 222,       ///< e.g. 1999
    Date24 = 223,       ///< e.g. 99
    Date25 = 224,       ///< e.g. 2000/02/18
    Date26 = 225,       ///< e.g. 2000/Feb/18
    Date27 = 226,       ///< e.g. Feb/99, complements Date7
    Date28 = 227,       ///< e.g. Feb/1999, complements Date22
    Date29 = 228,       ///< e.g. February/99, complements Date8
    Date30 = 229,       ///< e.g. February/1999, complements Date9
    Date31 = 230,       ///< e.g. 18-02, complements Date12
    Date32 = 231,       ///< e.g. 02/99
    Date33 = 232,       ///< e.g. 02-99, complements Date32
    Date34 = 233,       ///< e.g. Mon, 2 Feb 99
    Date35 = 234,       ///< e.g. Mon, 2 February 1999
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
KSPREAD_EXPORT bool isDate(Type type);
KSPREAD_EXPORT bool isTime(Type type);
KSPREAD_EXPORT bool isFraction(Type type);

} // namespace Format
} // namespace KSpread

#endif // KSPREAD_FORMAT
