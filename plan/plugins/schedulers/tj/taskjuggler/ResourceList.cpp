/*
 * ResourceList.cpp - TaskJuggler
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

#include "ResourceList.h"

#include "TjMessageHandler.h"
#include "Resource.h"
#include "Project.h"
#include "UsageLimits.h"

namespace TJ
{

ResourceList::ResourceList() :
    CoreAttributesList()
{
    sorting[0] = CoreAttributesList::TreeMode;
    sorting[1] = CoreAttributesList::IdUp;
}

bool
ResourceList::isSupportedSortingCriteria(int sc)
{
    switch (sc)
    {
    case TreeMode:
    case MinEffortUp:
    case MinEffortDown:
    case MaxEffortUp:
    case MaxEffortDown:
    case RateUp:
    case RateDown:
        return true;
    default:
        return CoreAttributesList::isSupportedSortingCriteria(sc);
    }
}

int
ResourceList::compareItemsLevel(CoreAttributes* c1, CoreAttributes* c2,
                                int level)
{
    Resource* r1 = static_cast<Resource*>(c1);
    Resource* r2 = static_cast<Resource*>(c2);

    if (level < 0 || level >= maxSortingLevel)
        return -1;

    switch (sorting[level])
    {
    case TreeMode:
        if (level == 0)
            return compareTreeItemsT(this, r1, r2);
        else
            return r1->getSequenceNo() == r2->getSequenceNo() ? 0 :
                r1->getSequenceNo() < r2->getSequenceNo() ? -1 : 1;
    case MinEffortUp:
        return r1->minEffort == r2->minEffort ? 0 :
            r1->minEffort < r2->minEffort ? -1 : 1;
    case MinEffortDown:
        return r1->minEffort == r2->minEffort ? 0 :
            r1->minEffort < r2->minEffort ? 1 : -1;
    case MaxEffortUp:
        return r1->limits->getDailyMax() == r2->limits->getDailyMax() ? 0 :
            r1->limits->getDailyMax() < r2->limits->getDailyMax() ? -1 : 1;
    case MaxEffortDown:
        return r1->limits->getDailyMax() == r2->limits->getDailyMax() ? 0 :
            r1->limits->getDailyMax() < r2->limits->getDailyMax() ? 1 : -1;
    case RateUp:
        return r1->rate == r2->rate ? 0 : r1->rate < r2->rate ? -1 : 1;
    case RateDown:
        return r1->rate == r2->rate ? 0 : r1->rate < r2->rate ? 1 : -1;
    default:
        return CoreAttributesList::compareItemsLevel(r1, r2, level);
    }
}

Resource*
ResourceList::getResource(const QString& id) const
{
    for (ResourceListIterator rli(*this); *rli != 0; ++rli)
        if ((*rli)->getId() == id)
            return *rli;

    return 0;
}

Resource* ResourceListIterator::operator*()
{
    return static_cast<Resource*>(CoreAttributesListIterator::operator*());
}

} // namespace TJ
