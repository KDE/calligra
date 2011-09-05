/*
 * ResourceList.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */
#ifndef _SbBooking_h_
#define _SbBooking_h_

#include <qstring.h>

namespace TJ
{

class Task;

/**
 * @short Booking information for a time slot of the resource.
 * @author Chris Schlaeger <cs@kde.org>
 */
class SbBooking
{
public:
    SbBooking(Task* t)
        : task(t) { }
    SbBooking(const SbBooking* b) { task = b->task; }
    virtual ~SbBooking() { }

    Task* getTask() const { return task; }

private:
    /// A pointer to the task that caused the booking
    Task* task;

    /// For future use.
    // double amount;
};

} // namespace TJ

#endif
