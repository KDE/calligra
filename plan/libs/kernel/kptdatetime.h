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

#include "kplatokernel_export.h"
#include "kptduration.h"

/// The main namespace.
namespace KPlato
{

class Duration;

/**
 * DateTime is a QDateTime which knows about Duration
 * Note that in Plan all datetimes are often in the time zone specified
 * in the project.
 * Exception to this is the calendar related dates and times which has
 * their own time zone specification.
 */
class KPLATOKERNEL_EXPORT DateTime : public QDateTime {

public:
    /// Create a DateTime.
    DateTime();
    /// Constructs a datetime with the given date, a valid time(00:00:00.000), and sets the timeSpec() to Qt::LocalTime.
    explicit DateTime( const QDate & );
    ///Constructs a datetime with the given date and time, using the time specification defined by @p spec.
    /// If date is valid and time is not, the time will be set to midnight.
    DateTime( const QDate &, const QTime &, Qt::TimeSpec spec = Qt::LocalTime );
    /// Constructs a copy of the @p other datetime.
    DateTime( const QDateTime &other );

    /// Constructs a datetime from @p dt with timespec @p spec
    DateTime( const QDateTime &dt, const KDateTime::Spec &spec );
    /// Constructs a copy of the @p dt KDateTime.
    DateTime( const KDateTime &dt );
    /**
     * Adds the duration @p duration to the datetime
     */
    DateTime operator+(const Duration &duration) const;
    /**
     * Subtracts the duration @p duration from the datetime
     */
    DateTime operator-(const Duration &duration) const ;
    /**
     * Returns the absolute duration between the two datetimes
     */
    Duration operator-(const DateTime &dt) const { return duration(dt); }
    /**
     * Returns the absolute duration between the two datetimes
     */
    Duration operator-(const DateTime &dt) { return duration(dt); }
    /// Add @p duration to this datetime.
    DateTime &operator+=(const Duration &duration);
    /// Subtract the @p duration from this datetime.
    DateTime &operator-=(const Duration &duration);

    /**
     * Parse a datetime string and return a DateTime.
     */
    static DateTime fromString(const QString dts, const KDateTime::Spec &spec=KDateTime::LocalZone);
private:

    Duration duration(const DateTime &dt) const;
    void add(const Duration &duration);
    void subtract(const Duration &duration);

};

}  //KPlato namespace

#endif
