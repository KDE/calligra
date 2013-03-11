/*
 * ShiftList.cpp - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#include "ShiftList.h"
#include "Shift.h"
#include "Project.h"

namespace TJ
{

Shift*
ShiftList::getShift(const QString& id) const
{
    for (ShiftListIterator sli(*this); *sli != 0; ++sli)
        if ((*sli)->getId() == id)
            return *sli;

    return 0;
}

int
ShiftList::compareItemsLevel(CoreAttributes* c1, CoreAttributes* c2,
                             int level)
{
    Shift* s1 = static_cast<Shift*>(c1);
    Shift* s2 = static_cast<Shift*>(c2);

    if (level < 0 || level >= maxSortingLevel)
        return -1;

    switch (sorting[level])
    {
    case TreeMode:
        if (level == 0)
            return compareTreeItemsT(this, s1, s2);
        else
            return s1->getSequenceNo() == s2->getSequenceNo() ? 0 :
                s1->getSequenceNo() < s2->getSequenceNo() ? -1 : 1;
    default:
        return CoreAttributesList::compareItemsLevel(s1, s2, level);
    }
}

Shift* ShiftListIterator::operator*()
{
    return static_cast<Shift*>(CoreAttributesListIterator::operator*());
}

} // namespace TJ
