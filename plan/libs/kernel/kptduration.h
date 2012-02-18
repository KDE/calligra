/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org
   Copyright (C) 2004 - 2011 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTDURATION_H
#define KPTDURATION_H

#include "kplatokernel_export.h"

#include <QtGlobal>
#include <QString>

class KLocale;

/// The main namespace.
namespace KPlato
{

/**
 * The Duration class can be used to store a timespan in a convenient format.
 * The timespan can be in length in many many hours down to milliseconds.
 */
class KPLATOKERNEL_EXPORT Duration
{
public:
    /**
     * DayTime  = d hh:mm:ss.sss
     * Day      = d.ddd
     * Hour     = hh:mm
     * HourFraction = h.fraction of an hour
     */
    enum Format { Format_DayTime, Format_Year, Format_Month, Format_Week, Format_Day, Format_Hour, Format_HourFraction, Format_i18nDayTime, Format_i18nYear, Format_i18nMonth, Format_i18nWeek, Format_i18nDay, Format_i18nHour, Format_i18nHourFraction };

    //NOTE: These must match units in DurationSpinBox!
    enum Unit { Unit_Y, Unit_M, Unit_w, Unit_d, Unit_h, Unit_m, Unit_s, Unit_ms };

    /// Create a zero duration
    Duration();
    /// Create a duration of @p value, the value is in @p unit (defaut unit is milliseconds)
    explicit Duration(const qint64 value, Unit unit = Unit_ms);
    /// Create a duration of @p value, the value is in @p unit (default is hours)
    explicit Duration(double value, Unit unit = Unit_h);
    /// Create a duration of @p d days, @p h hours, @p m minutes, @p s seconds and @p ms milliseconds
    Duration(unsigned d, unsigned h, unsigned m, unsigned s=0, unsigned ms=0);

    /// Return duration in milliseconds
    qint64 milliseconds() const { return m_ms; }
    /// Return duration in whole seconds
    qint64 seconds() const { return m_ms / 1000; }
    /// Return duration in whole minutes
    qint64 minutes() const { return seconds() / 60; }
    /// Return duration in whole hours
    unsigned hours() const { return minutes() / 60; }
    /// Return duration in whole days
    unsigned days() const { return hours() / 24; }

    /**
     * Adds @p delta to *this. If @p delta > *this, *this is set to zeroDuration.
     */
    void addMilliseconds(qint64 delta)  { add(delta); }

    /**
     * Adds @p delta to *this. If @p delta > *this, *this is set to zeroDuration.
     */
    void addSeconds(qint64 delta) { addMilliseconds(delta * 1000); }

    /**
     * Adds @p delta to *this. If @p delta > *this, *this is set to zeroDuration.
     */
    void addMinutes(qint64 delta) { addSeconds(delta * 60); }

    /**
     * Adds @p delta to *this. If @p delta > *this, *this is set to zeroDuration.
     */
    void addHours(qint64 delta) { addMinutes(delta * 60); }

    /**
     * Adds @p delta to *this. If @p delta > *this, *this is set to zeroDuration.
     */
    void addDays(qint64 delta) { addHours(delta * 24); }

    bool   operator==( const Duration &d ) const { return m_ms == d.m_ms; }
    bool   operator==( qint64 d ) const { return m_ms == d; }
    bool   operator!=( const Duration &d ) const { return m_ms != d.m_ms; }
    bool   operator!=( qint64 d ) const { return m_ms != d; }
    bool   operator<( const Duration &d ) const { return m_ms < d.m_ms; }
    bool   operator<( qint64 d ) const { return m_ms < d; }
    bool   operator<=( const Duration &d ) const { return m_ms <= d.m_ms; }
    bool   operator<=( qint64 d ) const { return m_ms <= d; }
    bool   operator>( const Duration &d ) const { return m_ms > d.m_ms; }
    bool   operator>( qint64 d ) const { return m_ms > d; }
    bool   operator>=( const Duration &d ) const { return m_ms >= d.m_ms; }
    bool   operator>=( qint64 d ) const { return m_ms >= d; }
    Duration &operator=(const Duration &d ) { m_ms = d.m_ms; return *this;}
    Duration operator*(int value) const; 
    Duration operator*(const double value) const;
    Duration operator*(const Duration value) const;
    /// Divide duration with the integer @p value
    Duration operator/(int value) const;
    /// Divide duration with the duration @p d
    double operator/(const Duration &d) const;
    /// Add duration with duration @p d
    Duration operator+(const Duration &d) const
        {Duration dur(*this); dur.add(d); return dur; }
    /// Add duration with duration @p d
    Duration &operator+=(const Duration &d) {add(d); return *this; }
    /// Subtract duration with duration @p d
    Duration operator-(const Duration &d) const
        {Duration dur(*this); dur.subtract(d); return dur; }
    /// Subtract duration with duration @p d
    Duration &operator-=(const Duration &d) {subtract(d); return *this; }

    /// Format duration into a string with @p unit and @p presition using @p locale. If @p locale == 0, uses KGLobal::locale.
    QString format( Unit unit = Unit_h, int presition = 1, const KLocale *locale = 0 ) const;
    /// Convert duration to a string with @p format
    QString toString(Format format = Format_DayTime) const;
    /// Create a duration from string @p s with @p format
    static Duration fromString(const QString &s, Format format = Format_DayTime, bool *ok=0);

    /// Return the duration scaled to hours
    double toHours() const;
    /**
     * Converts Duration into a double and scales it to unit @p u (default unit is hours)
     */
    double toDouble( Unit u = Unit_h ) const;

    /// Return the list of units. Translated if @p trans is true.
    static QStringList unitList( bool trans = false );
    /// Return @p unit in human readable form. Translated if @p trans is true.
    static QString unitToString( Duration::Unit unit, bool trans = false );
    /// Convert @p unit name into Unit
    static Unit unitFromString( const QString &unit );
    /// Returns value and unit from a <value><unit> coded string in @p rv and @p unit.
    static bool valueFromString( const QString &value, double &rv, Unit &unit );
    /**
     * This is useful for occasions where we need a zero duration.
     */
    static const Duration zeroDuration;

private:
    friend class DateTime;
    /**
     * Duration in milliseconds. Signed to allow for simple calculations which
     * might go negative for intermediate results.
     */
    qint64 m_ms;
    
private:
    void add(qint64 delta);
    void add(const Duration &delta);

    /**
    * Subtracts @param delta from *this. If @param delta > *this, *this is set to zeroDuration.
    */
    void subtract(const Duration &delta);
};

}  //KPlato namespace

#endif
