/*
 * VacationList.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#ifndef _VacationList_h_
#define _VacationList_h_

#include <QList>

#include "VacationInterval.h"

namespace TJ
{

class Interval;

/**
 * @short A list of vacations.
 * @author Chris Schlaeger <cs@kde.org>
 */
class VacationList : public QList<VacationInterval*>
{
public:
    VacationList() {}
    virtual ~VacationList();

    typedef QListIterator<VacationInterval*> Iterator;

    void inSort(VacationInterval* vi); //TODO

    void add(const QString& name, const Interval& i);
    void add(VacationInterval* vi);
    bool isVacation(time_t date) const;
    QString vacationName(time_t date) const;

protected:
//     virtual int compareItems(QCollection::Item i1, QCollection::Item i2);
} ;

} // namespace TJ

#endif
