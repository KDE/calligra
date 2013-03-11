/*
 * Shift.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006
 * by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */
#ifndef _Shift_h_
#define _Shift_h_

#include <time.h>

#include <QList>

#include "CoreAttributes.h"

namespace TJ
{

class Interval;
class ShiftListIterator;

/**
 * @short Stores all shift related information.
 * @author Chris Schlaeger <cs@kde.org>
 */
class Shift : public CoreAttributes
{
public:
    Shift(Project* prj, const QString& i, const QString& n, Shift* p,
          const QString& df = QString(), uint dl = 0);
    virtual ~Shift();

    virtual CAType getType() const { return CA_Shift; }

    Shift* getParent() const { return static_cast<Shift*>(parent); }

    ShiftListIterator getSubListIterator() const;

    void inheritValues();

    void setWorkingHours(int day, const QList<Interval*>& l);

    QList<Interval*>* getWorkingHours(int day) const
    {
        return workingHours[day];
    }
    const QList<Interval*>* const * getWorkingHours() const
    {
        return static_cast<const QList<Interval*>* const*>(workingHours);
    }

    bool isOnShift(const Interval& iv) const;

    bool isVacationDay(time_t day) const;

private:
    QList<Interval*>* workingHours[7];
};

} // namespace TJ

#endif
