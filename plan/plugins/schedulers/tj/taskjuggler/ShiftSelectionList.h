/*
 * ShiftSelectionList.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */
#ifndef _ShiftSelectionList_h_
#define _ShiftSelectionList_h_

#include "ShiftSelection.h"

namespace TJ
{

class Interval;

/**
 * @short Holds a list of shift selections.
 * @author Chris Schlaeger <cs@kde.org>
 */
class ShiftSelectionList : public QList<ShiftSelection*>
{
public:
    ShiftSelectionList() { }
    virtual ~ShiftSelectionList() { }

    typedef QListIterator<ShiftSelection*> Iterator;

    bool insert(ShiftSelection* s);

    bool isOnShift(const Interval& iv) const;

    bool isVacationDay(time_t day) const;

private:
    friend class ShiftSelection;
//     virtual int compareItems(QCollection::Item i1, QCollection::Item i2);
};

} // namespace TJ

#endif
