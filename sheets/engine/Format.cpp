/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006, 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Format.h"

using namespace Calligra::Sheets;

bool Format::isDateTime(Type fmt)
{
    return ((fmt >= Format::DateTimesBegin) && (fmt < DateTimesEnd));
}

bool Format::isDate(Type fmt)
{
    return ((fmt >= Format::DatesBegin) && (fmt < DatesEnd));
}

bool Format::isTime(Type fmt)
{
    return ((fmt >= TimesBegin) && (fmt < TimesEnd));
}

bool Format::isFraction(Type fmt)
{
    return ((fmt >= FractionsBegin) && (fmt <= FractionsEnd));
}

bool Format::isMoney(Type fmt)
{
    return (fmt == Money);
}

bool Format::isNumber(Type fmt)
{
    if (isFraction(fmt))
        return true;
    if ((fmt == Number) || (fmt == Percentage) || (fmt == Scientific))
        return true;
    return false;
}
