/*
 * TaskList.cpp - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */
#include "TaskList.h"
#include "Resource.h"

namespace TJ
{

Task*
TaskList::getTask(const QString& id) const
{
    for (TaskListIterator tli(*this); *tli != 0; ++tli)
        if ((*tli)->getId() == id)
            return *tli;

    return 0;
}

bool
TaskList::isSupportedSortingCriteria(int sc)
{
    switch (sc & 0xFFFF)
    {
    case TreeMode:
    case StartUp:
    case StartDown:
    case EndUp:
    case EndDown:
    case StatusUp:
    case StatusDown:
    case CompletedUp:
    case CompletedDown:
    case PrioUp:
    case PrioDown:
    case ResponsibleUp:
    case ResponsibleDown:
    case CriticalnessUp:
    case CriticalnessDown:
    case PathCriticalnessUp:
    case PathCriticalnessDown:
        return true;
    default:
        return CoreAttributesList::isSupportedSortingCriteria(sc);
    }
}

int
TaskList::compareItemsLevel(CoreAttributes* c1, CoreAttributes* c2, int level)
{
    Task* t1 = static_cast<Task*>(c1);
    Task* t2 = static_cast<Task*>(c2);

    if (level < 0 || level >= maxSortingLevel)
        return -1;

    switch (sorting[level])
    {
    case TreeMode:
        if (level == 0)
            return compareTreeItemsT(this, t1, t2);
        else
            return t1->getSequenceNo() == t2->getSequenceNo() ? 0 :
                t1->getSequenceNo() < t2->getSequenceNo() ? -1 : 1;
    case StartUp:
        return t1->scenarios[sortScenario].start ==
            t2->scenarios[sortScenario].start ? 0 :
            t1->scenarios[sortScenario].start <
            t2->scenarios[sortScenario].start ? -1 : 1;
    case StartDown:
        return t1->scenarios[sortScenario].start ==
            t2->scenarios[sortScenario].start ? 0 :
            t1->scenarios[sortScenario].start >
            t2->scenarios[sortScenario].start ? -1 : 1;
    case EndUp:
        return t1->scenarios[sortScenario].end ==
            t2->scenarios[sortScenario].end ? 0 :
            t1->scenarios[sortScenario].end < t2->scenarios[sortScenario].end
            ? -1 : 1;
    case EndDown:
        return t1->scenarios[sortScenario].end ==
            t2->scenarios[sortScenario].end ? 0 :
            t1->scenarios[sortScenario].end > t2->scenarios[sortScenario].end
            ? -1 : 1;
    case StatusUp:
        return t1->scenarios[sortScenario].status ==
            t2->scenarios[sortScenario].status ? 0 :
            t1->scenarios[sortScenario].status <
            t2->scenarios[sortScenario].status ? -1 : 1;
    case StatusDown:
        return t1->scenarios[sortScenario].status ==
            t2->scenarios[sortScenario].status ? 0 :
            t1->scenarios[sortScenario].status >
            t2->scenarios[sortScenario].status ? -1 : 1;
    case CompletedUp:
    {
        /* Unfortunately the floating point arithmetic on x86 processors is
         * slightly different from other processors. To get identical
         * results on all CPUs we ignore some precision that we don't need
         * anyhow. */
        int cd1 = static_cast<int> (t1->getCompletionDegree(sortScenario) *
                                    1000);
        int cd2 = static_cast<int> (t2->getCompletionDegree(sortScenario) *
                                    1000);
        return cd1 == cd2 ? 0 : cd1 < cd2 ? -1 : 1;
    }
    case CompletedDown:
    {
        int cd1 = static_cast<int> (t1->getCompletionDegree(sortScenario) *
                                    1000);
        int cd2 = static_cast<int> (t2->getCompletionDegree(sortScenario) *
                                    1000);
        return cd1 == cd2 ? 0 : cd1 > cd2 ? -1 : 1;
    }
    case PrioUp:
        if (t1->priority == t2->priority)
        {
            if (t1->scheduling == t2->scheduling)
                return 0;
            else if (t1->scheduling == Task::ASAP)
                return -1;
        }
        else
            return (t1->priority - t2->priority);
    case PrioDown:
        if (t1->priority == t2->priority)
        {
            if (t1->scheduling == t2->scheduling)
                return 0;
            else if (t1->scheduling == Task::ASAP)
                return 1;
        }
        else
            return (t2->priority - t1->priority);
    case ResponsibleUp:
    {
        QString fn1;
        t1->responsible->getFullName(fn1);
        QString fn2;
        t2->responsible->getFullName(fn2);
        return fn1.compare(fn2);
    }
    case ResponsibleDown:
    {
        QString fn1;
        t1->responsible->getFullName(fn1);
        QString fn2;
        t2->responsible->getFullName(fn2);
        return -fn1.compare(fn2);
    }
    case CriticalnessUp:
        return t1->scenarios[sortScenario].criticalness ==
            t2->scenarios[sortScenario].criticalness ? 0 :
            t1->scenarios[sortScenario].criticalness <
            t2->scenarios[sortScenario].criticalness ? -1 : 1;
    case CriticalnessDown:
        return t1->scenarios[sortScenario].criticalness ==
            t2->scenarios[sortScenario].criticalness ? 0 :
            t1->scenarios[sortScenario].criticalness >
            t2->scenarios[sortScenario].criticalness ? -1 : 1;
    case PathCriticalnessUp:
        return t1->scenarios[sortScenario].pathCriticalness ==
            t2->scenarios[sortScenario].pathCriticalness ? 0 :
            t1->scenarios[sortScenario].pathCriticalness <
            t2->scenarios[sortScenario].pathCriticalness ? -1 : 1;
    case PathCriticalnessDown:
        return t1->scenarios[sortScenario].pathCriticalness ==
            t2->scenarios[sortScenario].pathCriticalness ? 0 :
            t1->scenarios[sortScenario].pathCriticalness >
            t2->scenarios[sortScenario].pathCriticalness ? -1 : 1;
    default:
        return CoreAttributesList::compareItemsLevel(t1, t2, level);
    }
}

Task* TaskListIterator::operator*()
{
    return static_cast<Task*>(CoreAttributesListIterator::operator*());
}

} // namespace TJ
