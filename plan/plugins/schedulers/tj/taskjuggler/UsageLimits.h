/*
 * UsageLimits.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004, 2005 by Chris Schlaeger <cs@kde.org>
 * Copyright (c) 2011 by Dag Andersen <danders@get2net.dk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#ifndef _UsageLimits_h_
#define _UsageLimits_h_

namespace TJ
{

/**
 * This class stores usage limits of resources or task allocations. The values
 * are stored as number of scoreboard slots.
 *
 * @short The class stores usage limits
 * @see Resource
 * @see Allocation
 * @author Chris Schlaeger <cs@kde.org>
 */
class UsageLimits
{
    public:
        UsageLimits() :
            dailyMax(0),
            weeklyMax(0),
            monthlyMax(0),
            dailyUnits(0)
        { }

        ~UsageLimits() { }

        /// Set the max number of work slots that can be allocated per day
        /// dailyUnits has precedence over dailyMax
        void setDailyMax(uint m) { dailyMax = m; }
        /// Get the max number of work slots that can be allocated per day
        uint getDailyMax() const { return dailyMax; }

        /// Set the max number of work slots that can be allocated per week
        void setWeeklyMax(uint m) { weeklyMax = m; }
        /// Get the max number of work slots that can be allocated per week
        uint getWeeklyMax() const { return weeklyMax; }

        /// Set the max number of work slots that can be allocated per month
        void setMonthlyMax(uint m) { monthlyMax = m; }
        /// Get the max number of work slots that can be allocated per month
        uint getMonthlyMax() const { return monthlyMax; }

        /// Set percentage of how many working slots can be allocated on a day of total number of working slots
        /// dailyUnits has precedence over dailyMax
        void setDailyUnits( uint units ) { dailyUnits = units; }
        /// Get percentage of how many working slots can be allocated on a day of total number of working slots
        uint getDailyUnits() const { return dailyUnits; }

    private:
        uint dailyMax;
        uint weeklyMax;
        uint monthlyMax;
        uint dailyUnits;
} ;

} // namespace TJ

#endif

