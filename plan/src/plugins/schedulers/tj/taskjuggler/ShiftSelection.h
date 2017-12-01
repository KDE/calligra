/*
 * ShiftSelection.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */
#ifndef _ShiftSelection_h_
#define _ShiftSelection_h_

#include "Shift.h"
#include "Interval.h"

namespace TJ
{

class ShiftSelectionList;

/**
 * @short Stores shift selection related information.
 * @author Chris Schlaeger <cs@kde.org>
 */
class ShiftSelection
{
    friend class ShiftSelectionList;

public:
    ShiftSelection(const Interval& p, Shift* s) :
        period(new Interval(p)),
        shift(s)
    { }

    ShiftSelection(Interval* p, Shift* s) :
        period(p),
        shift(s)
    { }

    ShiftSelection(const ShiftSelection& sl);

    ~ShiftSelection()
    {
        delete period;
    }

    const Interval& getPeriod() const { return *period; }
    Shift* getShift() const { return shift; }

    bool isVacationDay(time_t day) const;

private:
    Interval* period;
    Shift* shift;
};

} // namespace TJ

#endif
