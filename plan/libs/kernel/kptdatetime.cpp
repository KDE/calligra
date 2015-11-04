/* This file is part of the KDE project
   Copyright (C) 2003-2007 Dag Andersen <danders@get2net.dk>
   Copyright (C) 2011 Dag Andersen <danders@get2net.dk>

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

#include "kptdatetime.h"

namespace KPlato
{

DateTime::DateTime()
    : QDateTime()
{
}

DateTime::DateTime( const QDate &date )
    : QDateTime( date )
{
}

DateTime::DateTime( const QDate &date, const QTime &time)
    : QDateTime( date, time, Qt::LocalTime)
{
}

DateTime::DateTime( const QDateTime& other )
    : QDateTime( other.toLocalTime() )
{
}

DateTime::DateTime( const QDateTime &dt, const KDateTime::Spec &spec )
    : QDateTime( KDateTime( dt, spec ).toLocalZone().dateTime() )
{
}

DateTime::DateTime( const KDateTime &dt )
    : QDateTime( dt.toLocalZone().dateTime() )
{
}


void DateTime::add(const Duration &duration) {
    if (isValid()) {
        DateTime x = addMSecs(duration.milliseconds());
        setDate( x.date() );
        setTime( x.time() );
        //debugPlan<<toString();
    }
}

void DateTime::subtract(const Duration &duration) {
    if (isValid() && duration.m_ms) {
        *this = addMSecs(-duration.m_ms);
        //debugPlan<<toString();
    }
}

Duration DateTime::duration(const DateTime &dt) const {
    Duration dur;
    if (isValid() && dt.isValid()) {
        qint64 x = msecsTo( dt ); //NOTE: this does conversion to UTC (expensive)
        dur.m_ms = x < 0 ? -x : x;
    }
    //debugPlan<<dur.milliseconds();
    return dur;
}

DateTime DateTime::operator+(const Duration &duration) const {
    DateTime d(*this);
    d.add(duration);
    return d;
}

DateTime& DateTime::operator+=(const Duration &duration) {
    add(duration);
    return *this;
}

DateTime DateTime::operator-(const Duration &duration) const {
    DateTime d(*this);
    d.subtract(duration);
    return d;
}

DateTime& DateTime::operator-=(const Duration &duration) {
    subtract(duration);
    return *this;
}

DateTime DateTime::fromString( const QString dts, const KDateTime::Spec &spec )
{
    if (dts.isEmpty()) {
        return DateTime();
    }
    KDateTime dt = KDateTime::fromString(dts);
    if ( ! dt.isValid() ) {
        // try to parse in qt default format (used in early version)
        dt = KDateTime( QDateTime::fromString(dts), spec ).toLocalZone();
        return dt.dateTime();
    }
    if ( dt.isClockTime() ) {
        // timezone offset missing, set to spec
        return DateTime( dt.toLocalZone().dateTime() );
    }
    DateTime t = DateTime( dt.toTimeSpec( spec ).toLocalZone().dateTime() );
    return t;
}

}  //KPlato namespace
