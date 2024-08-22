/** This file is part of the KDE project
 *  SPDX-FileCopyrightText: 2023 dag Andersen <dag.andersen@kdemail.net>
 *  SPDX-License-Identifier: LGPL-2.0-only
 */

#include "CS_Time.h"
#include <QTime>
#include <SheetsDebug.h>

using namespace Calligra::Sheets;

Time::Time(const Number &hours)
{
    setDuration(hours);
}

Time::Time(int64_t hours, int minutes, double seconds)
{
    m_valid = (hours >= 0 && minutes >= 0 && minutes < 60 && seconds >= 0.0 && seconds < 60.0);
    if (m_valid) {
        m_hours = hours;
        m_hour = hours % 24;
        m_minute = minutes;
        m_minutes = (m_hours * 60) + m_minute;
        m_seconds = seconds;
        m_second = m_seconds;
        m_msecs = qRound((m_seconds - (double)(m_second)) * 1000.0);
        m_duration = (double)m_minutes / 60 + (seconds / 3600.0);
        fixCastingOffsets();
    }
}

Time::Time(int64_t hours, int minutes, int seconds, int msecs)
{
    m_valid = (hours >= 0 && minutes >= 0 && minutes < 60 && seconds >= 0 && seconds < 60 && msecs >= 0 && msecs < 1000);

    if (m_valid) {
        m_hours = hours;
        m_minute = minutes;
        m_second = seconds;
        m_msecs = msecs;

        m_minutes = (m_hours * 60.) + m_minute;
        m_seconds = (double)seconds + ((double)msecs / 1000.);

        m_hour = hours % 24;
        m_minute = minutes;
        m_second = seconds;
        m_msecs = msecs;

        m_duration = (double)m_minutes / 60. + (seconds / 3600.0) + (double)msecs / 3600000.;
        fixCastingOffsets();
    }
}

Time::Time(const QTime &time)
{
    m_valid = time.isValid();
    if (m_valid) {
        m_duration = (double)(time.msecsSinceStartOfDay()) / 3600000.;
        m_hour = time.hour();
        m_hours = m_hour;
        m_minute = time.minute();
        m_minutes = (m_hours * 60.) + m_minute;
        m_second = time.second();
        m_msecs = time.msec();
        m_seconds = (double)m_second + ((double)m_msecs / 1000.);
    }
}

Time::Time(const Time &other)
{
    operator=(other);
}

Number Time::duration() const
{
    return m_duration;
}

void Time::setDuration(const Number &duration)
{
    m_valid = true;
    m_duration = numToDouble(duration);
    m_hours = m_duration;
    m_hour = m_hours % 24;
    m_minutes = m_duration * 60;
    m_minute = m_minutes % 60;
    m_seconds = (((m_duration - m_hours) * 60) - m_minute) * 60;
    m_second = m_seconds;
    m_msecs = qRound((m_seconds - (double)(m_second)) * 1000.0);
    fixCastingOffsets();
}

int64_t Time::hours() const
{
    return m_hours;
}

int64_t Time::minutes() const
{
    return m_minutes;
}

double Time::seconds() const
{
    return m_seconds;
}

int Time::hour() const
{
    return m_hour;
}

int Time::minute() const
{
    return m_minute;
}

int Time::second() const
{
    return m_second;
}

int Time::msec() const
{
    return m_msecs;
}

Time Time::operator+(const Time &other) const
{
    Time t;
    t.setDuration(m_duration + other.m_duration);
    return t;
}

Time &Time::operator+=(const Time &other)
{
    setDuration(m_duration + other.m_duration);
    return *this;
}

Time &Time::operator=(const Time &other) = default;

bool Time::operator==(const Time &other) const
{
    return m_valid == other.m_valid && m_duration == other.m_duration;
}

bool Time::operator!=(const Time &other) const
{
    return m_valid != other.m_valid || m_duration != other.m_duration;
}

QTime Time::toQTime() const
{
    QTime t(m_hour, m_minute, m_second, m_msecs);
    return t;
}

void Time::fixCastingOffsets()
{
    if (m_msecs > 999) {
        // debugSheets<<Q_FUNC_INFO<<"msecs: adjusted"<<m_duration<<'s'<<m_second<<'m'<<m_msecs;
        m_second += m_msecs / 1000;
        m_msecs = m_msecs % 1000;
    }
    if (m_second > 59) {
        // debugSheets<<Q_FUNC_INFO<<"secs: adjusted"<<m_duration<<'s'<<m_second<<'m'<<m_minute;
        m_minute += m_second / 60;
        m_second = m_second % 60;
    }
    if (m_minute > 60) {
        // debugSheets<<Q_FUNC_INFO<<"mins: adjusted"<<m_duration<<'h'<<m_hour<<'m'<<m_minute;
        m_hour += m_minute / 60;
        m_minute = m_minute % 60;
    }
}

QDebug operator<<(QDebug dbg, const Calligra::Sheets::Time *v)
{
    if (v)
        return dbg << *v;
    dbg.noquote().nospace() << "Calligra::Sheets::Time(";
    dbg << "0x0";
    dbg << ')';
    return dbg.quote().space();
}

QDebug operator<<(QDebug dbg, const Calligra::Sheets::Time &v)
{
    dbg.noquote().nospace() << "Calligra::Sheets::Time(";
    if (v.isValid()) {
        dbg << "d=" << v.duration() << " h=" << v.hours() << '(' << v.hour() << ')';
        dbg << " m=" << (int)v.minutes() << '(' << v.minute() << ')';
        dbg << " s=" << v.seconds() << '(' << v.second() << '.' << v.msec() << ')';
    } else {
        dbg << "Invalid";
    }
    dbg << ')';
    return dbg.quote().space();
}
