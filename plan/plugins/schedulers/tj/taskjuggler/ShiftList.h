/*
 * ShiftList.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#ifndef _ShiftList_h_
#define _ShiftList_h_

#include "CoreAttributesList.h"

class QString;

namespace TJ
{

class Shift;
class Project;

/**
 * @short Stores a list of Shifts.
 * @author Chris Schlaeger <cs@kde.org>
 */
class ShiftList : public CoreAttributesList
{
public:
    ShiftList()
    {
        sorting[0] = TreeMode;
        sorting[1] = SequenceUp;
    }
    virtual ~ShiftList() { }

    Shift* getShift(const QString& id) const;

    virtual int compareItemsLevel(CoreAttributes* c1, CoreAttributes* c2,
                                  int level);
} ;

/**
 * @short Iterator class for ShiftList objects.
 * @see ShiftList
 * @author Chris Schlaeger <cs@kde.org>
 */
class ShiftListIterator : public virtual CoreAttributesListIterator
{
public:
    ShiftListIterator(const CoreAttributesList& l) :
        CoreAttributesListIterator(l) { }
    virtual ~ShiftListIterator() { }
    Shift* operator*();
} ;

} // namespace TJ

#endif
