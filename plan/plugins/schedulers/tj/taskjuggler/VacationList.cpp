/*
 * VacationList.cpp - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#include "VacationList.h"

namespace TJ
{

VacationList::~VacationList()
{
    while(!isEmpty()) {
        delete takeFirst();
    }
}

void
VacationList::inSort(VacationInterval* vi)
{
    //TODO
    append(vi);
}

void
VacationList::add(const QString& name, const Interval& i)
{
    inSort(new VacationInterval(name, i));
}

void
VacationList::add(VacationInterval* vi)
{
    inSort(vi);
}


bool
VacationList::isVacation(time_t date) const
{
    for (VacationList::Iterator vli(*this); vli.hasNext();)
        if (vli.next()->contains(date))
            return true;

    return false;
}

QString
VacationList::vacationName(time_t date) const
{
    for (VacationList::Iterator vli(*this);  vli.hasNext();) {
        VacationInterval *v = vli.next();
        if (v->contains(date))
            return v->getName();
    }
    return QString();
}

} // namespace TJ
