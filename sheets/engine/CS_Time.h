/** This file is part of the KDE project
 *  SPDX-FileCopyrightText: 2023 dag Andersen <dag.andersen@kdemail.net>
 *  SPDX-License-Identifier: LGPL-2.0-only
 */

#ifndef CALLIGRA_SHEETS_TIME_H
#define CALLIGRA_SHEETS_TIME_H

#include "sheets_engine_export.h"

#include "Number.h"

#include <QDebug>
#include <QTime>

class QLocale;

namespace Calligra
{
namespace Sheets
{

/**
 * Provides a time value as duration
 *
 * Similar to QTime but has no limit on number of hours,
 * and seconds are store as a double to cater for fractions of seconds.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT Time
{
public:
    /// Creates a duration of @p hours
    Time(const Number &hours = 0.0);
    /// Creates a duration of @p h hours, @p m minutes and @p s seconds.
    /// If any of the values are negative an invalid zero duration is created.
    explicit Time(int64_t h, int m, double s = 0.0);
    /// Creates a duration of @p h hours, @p m minutes, @p s seconds and @p ms milliseconds.
    /// If any of the values are negative an invalid zero duration is created.
    explicit Time(int64_t h, int m, int s, int ms = 0);
    /// Creates Time from @p time QTime object
    Time(const QTime &time);
    /// Copy c'tor
    Time(const Time &time);

    /// Set this Time to @p valid
    void setValid(bool valid)
    {
        m_valid = valid;
    }
    /// @return true if this Time is valid
    bool isValid() const
    {
        return m_valid;
    }
    /// @return the duration
    Number duration() const;
    /// Set @p duration
    /// Duration can be both positive and negative
    void setDuration(const Number &duration);

    /// @return the duration in number of hours
    int64_t hours() const;
    /// @return the duration in number of minutes.
    /// E.g a duration of 1 hour 3 minutes returns 63 minutes.
    int64_t minutes() const;
    /// @return the seconds part of the duration included fraction of seconds.
    /// E.g 1.5 seconds corresponds to 1 second 500 milliseconds.
    double seconds() const;

    /// QTime behaviour.
    /// returns the duration in hours in mod 24.
    /// E.g. a duration of 35 hours retuns 1.
    int hour() const;
    /// QTime behaviour.
    /// @return the minutes part of the duration.
    /// E.g a duration of 1 hour 3 minutes returns 3 minutes.
    int minute() const;
    /// QTime behaviour.
    /// @return the seconds port of duration
    /// If @p round is true, rounded to nearest integer value,
    /// e.g duration of 1.5 seconds retuns 2.
    int second() const;
    /// QTime behaviour.
    /// @return the fraction of seconds as milliseconds rounded to nearest integer value.
    /// E.g duration of 1.5006 seconds returns 501.
    int msec() const;

    /// @return the duration converted to QTime .
    QTime toQTime() const;

    /// Add @p other duration to this duration and return the new Time.
    Time operator+(const Time &other) const;
    /// Add @p other duration to this duration.
    Time &operator+=(const Time &other);
    /// Set this duration to @p other duration.
    Time &operator=(const Time &other);
    /// @return true if this duration equals @p other duration.
    bool operator==(const Time &other) const;
    /// @return true if this duration does not equal @p other duration.
    bool operator!=(const Time &other) const;

    /// @return the current time
    static Time currentTime()
    {
        return Time(QTime::currentTime());
    }

private:
    void fixCastingOffsets();
    Number m_duration = 0.0;
    bool m_valid = true;
    int64_t m_hours = 0;
    int64_t m_minutes = 0;
    double m_seconds = 0;
    int m_hour = 0;
    int m_minute = 0;
    int m_second = 0;
    int m_msecs = 0;
};
}
}

CALLIGRA_SHEETS_ENGINE_EXPORT QDebug operator<<(QDebug dbg, const Calligra::Sheets::Time *v);
CALLIGRA_SHEETS_ENGINE_EXPORT QDebug operator<<(QDebug dbg, const Calligra::Sheets::Time &v);

#endif // CALLIGRA_SHEETS_TIME_H
