/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef kptduration_h
#define kptduration_h

#include <math.h>

#include <qdatetime.h>

class KPTDuration;

/**
 * The duration class can be used to store a timespan in a convenient format.
 * The timespan can be in length in many many hours down to miliseconds.
 */
class KPTDuration {
    public:
        enum Format { Format_DateTime, Format_Hour, Format_Day, Format_Week };

        KPTDuration();
        KPTDuration(const KPTDuration &d);
        KPTDuration(int h, int m, int s=0, int ms=0);
        KPTDuration(const QTime time);
        KPTDuration(const QDateTime time);
        KPTDuration(int seconds);
        ~KPTDuration();

        void add(KPTDuration time);
        void add(KPTDuration *time);
        void addSecs(int secs) { m_theTime = m_theTime.addSecs(secs); }
        void addDays(int days) { m_theTime = m_theTime.addDays(days); }
        void addMonths(int months) { m_theTime = m_theTime.addMonths(months); }
        void addYears(int years) { m_theTime = m_theTime.addYears(years); }
        /**
         * Subtracts @param duration from *this.
         * If @param duration > *this, *this is set to zeroDuration.
         */
        void subtract(const KPTDuration &duration);
        void subtract(const KPTDuration *duration);
        void const set(const KPTDuration newTime);
        void const set(const QDateTime newTime);

        int duration() const { return zero.secsTo(m_theTime); } // Note: this defies the use of QDateTime. We could just have used an int!

        bool   operator==( const KPTDuration &d ) const { return m_theTime == d.m_theTime; }
        bool   operator!=( const KPTDuration &d ) const { return m_theTime != d.m_theTime; }
        bool   operator<( const KPTDuration &d ) const { return m_theTime < d.m_theTime; }
        bool   operator<=( const KPTDuration &d ) const { return m_theTime <= d.m_theTime; }
        bool   operator>( const KPTDuration &d ) const { return m_theTime > d.m_theTime; }
        bool   operator>=( const KPTDuration &d ) const { return m_theTime >= d.m_theTime; }
        KPTDuration &operator=(const KPTDuration &d ) { set(d); return *this;}
        KPTDuration operator*(int unit) const; 
        KPTDuration operator/(int unit) const;
        
        KPTDuration operator+(KPTDuration &d) const
            {KPTDuration dur; dur.add(d); return dur; }
        KPTDuration &operator+=(const KPTDuration &d) {add(d); return *this; }
        
        KPTDuration operator-(const KPTDuration &d) const
            {KPTDuration dur; dur.subtract(d); return dur; }
        KPTDuration &operator-=(const KPTDuration &d) {subtract(d); return *this; }

        QString toString(Format format = Format_DateTime) const;
        static KPTDuration fromString(const QString &s) { return KPTDuration(QDateTime::fromString(s)); }

        QDateTime dateTime() const { return m_theTime; }
        QDate date() const { return m_theTime.date(); }
        QTime time() const { return m_theTime.time(); }

        QTime timeDuration() const { return QTime(m_theTime.time().hour()-zero.time().hour(),
                                                                 m_theTime.time().minute()-zero.time().minute(),
                                                                 m_theTime.time().second()-zero.time().second());
        }

        int days() const { return zero.daysTo(m_theTime); }
        int hours() const { return zero.secsTo(m_theTime) / 3600; }
        int hoursTo(const QDateTime &dt) const { return m_theTime.secsTo(dt) / 3600; }

        bool isCloseTo(const KPTDuration &d) const;

    /**
     * This is useful for occasions where we need a zero duration.
     */
    static const KPTDuration zeroDuration;

    private:
        QDateTime zero, m_theTime;
};

#endif
