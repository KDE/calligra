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

namespace KSpread
{

/**
 * Collection of string formatting enumeration and functions.
 */
namespace Format
{

enum Type
{
    Generic=0,
    Number=1, Text=5, Money=10, Percentage=25,
    Scientific=30,
    ShortDate=35, TextDate=36, Time=50,
    SecondeTime=51,
    Time1=52, Time2=53, Time3=54, Time4=55,
    Time5=56, Time6=57, Time7=58, Time8=59,
    fraction_half=70,fraction_quarter=71,fraction_eighth=72,fraction_sixteenth=73,
    fraction_tenth=74,fraction_hundredth=75,fraction_one_digit=76,
    fraction_two_digits=77,fraction_three_digits=78,
    Date1=200,Date2=201,Date3=202,Date4=203,
    Date5=204,Date6=205,Date7=206,Date8=207,
    Date9=208,Date10=209,Date11=210,Date12=211,
    Date13=212,Date14=213,Date15=214,Date16=215,
    Date17=216,Date18=217,Date19=218,Date20=219,
    Date21=220,Date22=221,Date23=222,Date24=223,
    Date25=224,Date26=225,
    Custom = 300, None = 400
};

//helper functions for the formatting
bool isDate( Type type );
bool isTime( Type type );
bool isFraction( Type type );

} // namespace Format
} // namespace KSpread

#endif // KSPREAD_FORMAT
