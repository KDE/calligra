/*
 * ShiftSelection.cpp - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#include "ShiftSelection.h"


namespace TJ
{

ShiftSelection::ShiftSelection(const ShiftSelection& sl) :
    period(new Interval(*sl.period)),
    shift(sl.shift)
{
}

bool
ShiftSelection::isVacationDay(time_t day) const
{
    return period->contains(day) && shift->isVacationDay(day);
}

} // namespace TJ

