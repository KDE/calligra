/*
 * ShiftSelectionList.cpp - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#include "ShiftSelectionList.h"

// int
// ShiftSelectionList::compareItems(QCollection::Item i1, QCollection::Item i2)
// {
//     ShiftSelection* s1 = static_cast<ShiftSelection*>(i1);
//     ShiftSelection* s2 = static_cast<ShiftSelection*>(i2);
// 
//     return s1->period->compare(*s2->period);
// }

namespace TJ
{

bool
ShiftSelectionList::isOnShift(const Interval& iv) const
{
    /* Check whether any of the defined shift intervals contains the interval
     * 'iv'. If not return true. If it does, check whether the interval 'iv'
     * lies within the specified working hours. */
    for (ShiftSelectionList::Iterator ssli(*this); ssli.hasNext();) {
        ShiftSelection *s = ssli.next();
        if (s->getPeriod().contains(iv))
           return s->getShift()->isOnShift(iv);
    }
    return true;
}

bool
ShiftSelectionList::isVacationDay(time_t day) const
{
    for (ShiftSelectionList::Iterator ssli(*this);
         ssli.hasNext() && day <= ssli.peekNext()->getPeriod().getEnd();)
        if (ssli.next()->isVacationDay(day))
            return true;
    return false;
}

bool
ShiftSelectionList::insert(ShiftSelection* s)
{
    for (ShiftSelectionList::Iterator ssli(*this); ssli.hasNext();)
        if (ssli.next()->getPeriod().overlaps(s->getPeriod()))
            return false;
    append(s);
    return true;
}

} // namespace TJ
