/*
 * VacationInterval.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */
#ifndef _VacationInterval_h_
#define _VacationInterval_h_

#include "Interval.h"

namespace TJ
{

/**
 * @short A time interval with a name.
 * @author Chris Schlaeger <cs@kde.org>
 */
class VacationInterval : public Interval
{
public:
    VacationInterval() :
        Interval(),
        name()
    { }

    VacationInterval(const QString& n, const Interval& i) :
        Interval(i),
        name(n)
    { }

    virtual ~VacationInterval() { }

    void setStart(time_t s) { start = s; }
    void setEnd(time_t e) { end = e; }
    void setName(const QString& n) { name = n; }
    const QString& getName() const { return name; }
    

private:
    QString name;
} ;

} // namespace TJ

#endif

