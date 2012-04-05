/*
 * Shift.cpp - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 * Copyright (c) 2011 by Dag Andersen <danders@get2net.dk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#include "Shift.h"
#include "Project.h"

namespace TJ
{

Shift::Shift(Project* prj, const QString& i, const QString& n, Shift* p,
             const QString& df, uint dl) :
    CoreAttributes(prj, i, n, p, df, dl),
    workingHours()
{
    prj->addShift(this);

    for (int i = 0; i < 7; i++)
    {
        workingHours[i] = new QList<Interval*>();
//         workingHours[i]->setAutoDelete(true);
    }
}

Shift::~Shift()
{
    for (int i = 0; i < 7; i++)
        delete workingHours[i];
    project->deleteShift(this);
}

void
Shift::inheritValues()
{
    Shift* p = (Shift*) parent;

    if (p)
    {
        // Inherit start values from parent resource.
        for (int i = 0; i < 7; i++)
        {
            while (!workingHours[i]->isEmpty()) delete workingHours[i]->takeFirst();
            delete workingHours[i];
            workingHours[i] = new QList<Interval*>();
//             workingHours[i]->setAutoDelete(true);
            for (QListIterator<Interval*> ivi(*(p->workingHours[i])); ivi.hasNext();)
                workingHours[i]->append(new Interval(*ivi.next()));
        }
    }
    else
    {
        // Inherit start values from project defaults.
        for (int i = 0; i < 7; i++)
        {
            while (!workingHours[i]->isEmpty()) delete workingHours[i]->takeFirst();
            delete workingHours[i];
            workingHours[i] = new QList<Interval*>();
//             workingHours[i]->setAutoDelete(true);
            for (QListIterator<Interval*>
                 ivi(project->getWorkingHoursIterator(i)); ivi.hasNext();)
                workingHours[i]->append(new Interval(*ivi.next()));
        }
    }
}

void
Shift::setWorkingHours(int day, const QList<Interval*>& l)
{
    while (!workingHours[day]->isEmpty()) delete workingHours[day]->takeFirst();
    delete workingHours[day];

    // Create a deep copy of the interval list.
    workingHours[day] = new QList<Interval*>;
//     workingHours[day]->setAutoDelete(true);
    for (QListIterator<Interval*> pli(l); pli.hasNext();)
        workingHours[day]->append(new Interval(*(pli.next())));
}

ShiftListIterator
Shift::getSubListIterator() const
{
    return ShiftListIterator(*sub);
}

bool
Shift::isOnShift(const Interval& iv) const
{
    int dow = dayOfWeek(iv.getStart(), false);
    int ivStart = secondsOfDay(iv.getStart());
    int ivEnd = secondsOfDay(iv.getEnd());
    Interval dayIv(ivStart, ivEnd);
    for (QListIterator<Interval*> ili(*(workingHours[dow])); ili.hasNext();)
        if (ili.next()->contains(dayIv))
            return true;

    return false;
}

bool
Shift::isVacationDay(time_t day) const
{
    return workingHours[dayOfWeek(day, false)]->isEmpty();
}

} // namespace TJ
