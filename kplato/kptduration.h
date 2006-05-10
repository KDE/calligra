/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org
   Copyright (C) 2004 - 2006 Dag Andersen <danders@get2net.dk>

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

#include <qglobal.h>
#include <QString>

namespace KPlato
{

/**
 * The duration class can be used to store a timespan in a convenient format.
 * The timespan can be in length in many many hours down to miliseconds.
 */
class Duration {
    public:
        /**
         * DayTime  = d hh:mm:ss.sss
         * Day      = d.ddd
         * Hour     = hh:mm
         * HourFraction = h.fraction of an hour
         */
        enum Format { Format_DayTime, Format_Day, Format_Hour, Format_HourFraction, Format_i18nDayTime, Format_i18nDay, Format_i18nHour, Format_i18nHourFraction };

        Duration();
        Duration(const Duration &d);
        Duration(unsigned d, unsigned h, unsigned m, unsigned s=0, unsigned ms=0);
        Duration(qint64 seconds);
        ~Duration();

        /**
         * Adds @param delta to *this. If @param delta > *this, *this is set to zeroDuration.
         */
        void addMilliseconds(qint64 delta)  { add(delta); }

        /**
         * Adds @param delta to *this. If @param delta > *this, *this is set to zeroDuration.
         */
        void addSeconds(qint64 delta) { addMilliseconds(delta * 1000); }

        /**
         * Adds @param delta to *this. If @param delta > *this, *this is set to zeroDuration.
         */
        void addMinutes(qint64 delta) { addSeconds(delta * 60); }

        /**
         * Adds @param delta to *this. If @param delta > *this, *this is set to zeroDuration.
         */
        void addHours(qint64 delta) { addMinutes(delta * 60); }

        /**
         * Adds @param delta to *this. If @param delta > *this, *this is set to zeroDuration.
         */
        void addDays(qint64 delta) { addHours(delta * 24); }

        //FIXME: overflow problem
        qint64 milliseconds() const { return m_ms; }
        qint64 seconds() const { return m_ms / 1000; }
        qint64 minutes() const { return seconds() / 60; }
        unsigned hours() const { return minutes() / 60; }
        unsigned days() const { return hours() / 24; }
        void get(unsigned *days, unsigned *hours, unsigned *minutes, unsigned *seconds=0, unsigned *milliseconds=0) const;

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
        Duration operator*(int unit) const; 
        Duration operator*(const double value) const;
        Duration operator/(int unit) const;
        double operator/(const Duration &d) const;
        
        Duration operator+(Duration &d) const
            {Duration dur(*this); dur.add(d); return dur; }
        Duration &operator+=(const Duration &d) {add(d); return *this; }
        
        Duration operator-(const Duration &d) const
            {Duration dur(*this); dur.subtract(d); return dur; }
        Duration &operator-=(const Duration &d) {subtract(d); return *this; }

        QString toString(Format format = Format_DayTime) const;
        static Duration fromString(const QString &s, Format format = Format_DayTime, bool *ok=0);
        
        //NOTE: These must match fieldnumbers in duration widget!
        enum Unit { Unit_d, Unit_h, Unit_m, Unit_s, Unit_ms };
        double toDouble(Unit u=Unit_ms) const { 
            if (u == Unit_ms) return (double)m_ms;
            else if (u == Unit_s) return (double)m_ms/1000.0;
            else if (u == Unit_m) return (double)m_ms/(1000.0*60.0);
            else if (u == Unit_h) return (double)m_ms/(1000.0*60.0*60.0);
            else if (u == Unit_d) return (double)m_ms/(1000.0*60.0*60.0*24.0);
            return (double)m_ms; 
        }

        /**
         * This is useful for occasions where we need a zero duration.
         */
        static const Duration zeroDuration;

    private:
	/**
	 * Duration in milliseconds. Signed to allow for simple calculations which
	 * might go negative for intermediate results.
	 */
        qint64 m_ms;

        void add(qint64 delta);
        void add(const Duration &delta);

        /**
         * Subtracts @param delta from *this. If @param delta > *this, *this is set to zeroDuration.
         */
        void subtract(const Duration &delta);
};

}  //KPlato namespace

#endif
