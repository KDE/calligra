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

class Duration;

/**
 * DateTime is a @ref QDateTime which knows about @ref Duration
 */
class DateTime : public QDateTime {

public:
    DateTime();
    DateTime(const QDateTime &dt);
    DateTime(const QDate &date);
    DateTime(const QDate &date, const QTime &time);

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

    static DateTime fromString(const QString dts) {
        QDateTime dt;
        if (dts.isEmpty())
            return DateTime();
        dt = QDateTime::fromString(dts, Qt::ISODate);
        if (dt.isValid())
            return DateTime(dt);
        return DateTime(QDateTime::fromString(dts));
    }
private:

    Duration duration(const DateTime &dt) const;
    void add(const Duration &duration);
    void subtract(const Duration &duration);

};

}  //KPlato namespace

#endif
