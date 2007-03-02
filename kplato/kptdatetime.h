/* This file is part of the KDE project
   Copyright (C) 2003-2007 Dag Andersen <danders@get2net.dk>

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

#include <kdatetime.h>

#include "kptduration.h"

namespace KPlato
{

class Duration;

/**
 * DateTime is a @ref KDateTime which knows about @ref Duration
 * Note that in KPlato all datetimes are really in the time zone specified
 * in the project.
 * Exception to this is the calendar related dates and times which has
 * their own time zone specification.
 */
class DateTime : public KDateTime {

public:
    DateTime();
    /// Create a datetime in the specified time zone
    DateTime(const QDateTime &dt, const KDateTime::Spec &spec);
    /// Create a copy of dt (same time, same time zone).
    DateTime(const KDateTime &dt);
    explicit DateTime(const QDate &date, const KDateTime::Spec &spec);
    DateTime(const QDate &date, const QTime &time, const KDateTime::Spec &spec=KDateTime::Spec(LocalZone));

    /**
     * Adds the duration @param duration to the datetime
     */
    DateTime operator+(const Duration &duration) const;
    /**
     * Subtracts the duration @param duration from the datetime
     */
    DateTime operator-(const Duration &duration) const ;
    /**
     * Returns the absolute duration between the two datetimes
     */
    Duration operator-(const DateTime &dt) const { return duration(dt); }
    Duration operator-(const DateTime &dt) { return duration(dt); }

    DateTime &operator+=(const Duration &duration);
    DateTime &operator-=(const Duration &duration);

    /**
     * Parse a datetime string and return a DateTime.
     */
    static DateTime fromString(const QString dts, const KDateTime::Spec &spec=LocalZone);
private:

    Duration duration(const DateTime &dt) const;
    void add(const Duration &duration);
    void subtract(const Duration &duration);

};

}  //KPlato namespace

#endif
