/* This file is part of the KDE project
   Copyright (C) 2003 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#ifndef KPTDATETIME_H
#define KPTDATETIME_H

#include <qdatetime.h>
#include "kptduration.h"

namespace KPlato
{

class KPTDuration;

/**
 * KPTDateTime is a @ref QDateTime which knows about @ref KPTDuration
 */
class KPTDateTime : public QDateTime {

public:
    KPTDateTime();
    KPTDateTime(const QDateTime &dt);
    KPTDateTime(const QDate &date, const QTime &time);

    /**
     * Adds the duration @param duration to the datetime
     */
    KPTDateTime operator+(const KPTDuration &duration) const;
    /**
     * Subtracts the duration @param duration from the datetime
     */
    KPTDateTime operator-(const KPTDuration &duration) const ;
    /**
     * Returns the absolute duration between the two datetimes
     */
    KPTDuration operator-(const KPTDateTime &dt) const { return duration(dt); }
    KPTDuration operator-(const KPTDateTime &dt) { return duration(dt); }

    KPTDateTime &operator+=(const KPTDuration &duration);
    KPTDateTime &operator-=(const KPTDuration &duration);

private:

    KPTDuration duration(const KPTDateTime &dt) const;
    void add(const KPTDuration &duration);
    void subtract(const KPTDuration &duration);

};

}  //KPlato namespace

#endif
