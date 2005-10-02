/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KOSCRIPT_LOCALE_H
#define KOSCRIPT_LOCALE_H

#include <klocale.h>

/**
 * This locale is used when executing external KoScript scripts.
 * This locale says, that it assumes numbers to be formatted like
 * in C,C++: 12345.6789
 * Dates are formatted like in the US: 10/31/99
 * Time is formatted like this: 12:43:56
 */
class KSLocale : public KLocale
{
public:
    KSLocale();
};

#endif
