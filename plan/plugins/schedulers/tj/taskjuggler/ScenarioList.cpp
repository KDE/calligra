/*
 * ScenarioList.cpp - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#include "ScenarioList.h"
#include "Scenario.h"

namespace TJ
{

ScenarioList::ScenarioList() :
    CoreAttributesList()
{
    sorting[0] = CoreAttributesList::TreeMode;
    sorting[1] = CoreAttributesList::IdUp;
}

Scenario*
ScenarioList::operator[](int i)
{
    return static_cast<Scenario*>(at(i));
}

bool
ScenarioList::isSupportedSortingCriteria(int sc)
{
    switch (sc)
    {
    case TreeMode:
        return true;
    default:
        return CoreAttributesList::isSupportedSortingCriteria(sc);
    }
}

int
ScenarioList::compareItemsLevel(CoreAttributes* c1, CoreAttributes* c2,
                                int level)
{
    Scenario* r1 = static_cast<Scenario*>(c1);
    Scenario* r2 = static_cast<Scenario*>(c2);

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
    default:
        return CoreAttributesList::compareItemsLevel(r1, r2, level);
    }
}

Scenario*
ScenarioList::getScenario(const QString& id) const
{
    for (ScenarioListIterator rli(*this); *rli != 0; ++rli)
        if ((*rli)->getId() == id)
            return *rli;

    return 0;
}

Scenario* ScenarioListIterator::operator*()
{
    return static_cast<Scenario*>(CoreAttributesListIterator::operator*());
}

} // namespace TJ
