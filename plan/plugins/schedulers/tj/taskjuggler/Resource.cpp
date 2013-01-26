/*
 * Resource.cpp - TaskJuggler
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

#include "Resource.h"

#include <KLocale>

#include <assert.h>

#include "ResourceTreeIterator.h"

#include "Project.h"
#include "ShiftSelection.h"
#include "BookingList.h"
// #include "Account.h"
#include "UsageLimits.h"
#include "TjMessageHandler.h"
#include "tjlib-internal.h"
// #include "ReportXML.h"
#include "CustomAttributeDefinition.h"

namespace TJ
{

/*
 * Calls to sbIndex are fairly expensive due to the floating point
 * division. We therefor use buffers that stores the index of the
 * first/last slot of a day/week/month for each slot.
 */
static uint* DayStartIndex = 0;
static uint* WeekStartIndex = 0;
static uint* MonthStartIndex = 0;
static uint* DayEndIndex = 0;
static uint* WeekEndIndex = 0;
static uint* MonthEndIndex = 0;


Resource::Resource(Project* p, const QString& i, const QString& n,
                   Resource* pr, const QString& df, uint dl) :
    CoreAttributes(p, i, n, pr, df, dl),
    minEffort(0.0),
//     journal(),
    limits(0),
    efficiency(0.0),
    rate(0.0),
    workingHours(),
    shifts(),
    vacations(),
    scoreboard(0),
    sbSize((p->getEnd() + 1 - p->getStart()) / p->getScheduleGranularity() + 1),
    specifiedBookings(new SbBooking**[p->getMaxScenarios()]),
    scoreboards(new SbBooking**[p->getMaxScenarios()]),
    scenarios(new ResourceScenario[p->getMaxScenarios()]),
    allocationProbability(new double[p->getMaxScenarios()])
{
//     vacations.setAutoDelete(true);
//     shifts.setAutoDelete(true);

    p->addResource(this);

    for (int sc = 0; sc < p->getMaxScenarios(); sc++)
    {
        scoreboards[sc] = 0;
        specifiedBookings[sc] = 0;
    }

    for (int i = 0; i < p->getMaxScenarios(); ++i)
        allocationProbability[i] = 0;

    if (!DayStartIndex)
    {
        DayStartIndex = new uint[sbSize];
        WeekStartIndex = new uint[sbSize];
        MonthStartIndex = new uint[sbSize];
        long i = 0;
        uint dayStart = 0;
        uint weekStart = 0;
        uint monthStart = 0;
        bool weekStartsMonday = project->getWeekStartsMonday();
        for (time_t ts = p->getStart(); i < (long) sbSize; ts +=
             p->getScheduleGranularity(), ++i)
        {
            if (ts == midnight(ts))
                dayStart = i;
            DayStartIndex[i] = dayStart;

            if (ts == beginOfWeek(ts, weekStartsMonday))
                weekStart = i;
            WeekStartIndex[i] = weekStart;

            if (ts == beginOfMonth(ts))
                monthStart = i;
            MonthStartIndex[i] = monthStart;
        }

        DayEndIndex = new uint[sbSize];
        WeekEndIndex = new uint[sbSize];
        MonthEndIndex = new uint[sbSize];
        i = sbSize - 1;
        uint dayEnd = i;
        uint weekEnd = i;
        uint monthEnd = i;
        // WTF does p->getEnd not return the 1st sec after the time frame!!!
        for (time_t ts = p->getEnd() + 1; i >= 0;
             ts -= p->getScheduleGranularity(), --i)
        {
            DayEndIndex[i] = dayEnd;
            if (ts - midnight(ts) < (int) p->getScheduleGranularity())
                dayEnd = i > 0 ? i - 1 : 0;

            WeekEndIndex[i] = weekEnd;
            if (ts - beginOfWeek(ts, weekStartsMonday) <
                (int) p->getScheduleGranularity())
                weekEnd = i > 0 ? i - 1 : 0;

            MonthEndIndex[i] = monthEnd;
            if (ts - beginOfMonth(ts) < (int) p->getScheduleGranularity())
                monthEnd = i > 0 ? i - 1 : 0;
        }
    }

    for (int i = 0; i < 7; i++)
    {
        workingHours[i] = new QList<Interval*>();
//         workingHours[i]->setAutoDelete(true);
    }
}

Resource::~Resource()
{
    int i;
    for (i = 0; i < 7; i++) {
        while (!workingHours[i]->isEmpty()) delete workingHours[i]->takeFirst();
        delete workingHours[i];
    }
    for (int sc = 0; sc < project->getMaxScenarios(); sc++)
    {
        if (scoreboards[sc])
        {
            for (uint i = 0; i < sbSize; i++)
                if (scoreboards[sc][i] >= (SbBooking*) 4)
                {
                    uint j;
                    for (j = i + 1; j < sbSize &&
                         scoreboards[sc][i] == scoreboards[sc][j]; j++)
                        ;
                    delete scoreboards[sc][i];
                    i = j - 1;
                }
            delete [] scoreboards[sc];
            scoreboards[sc] = 0;
        }
        if (specifiedBookings[sc])
        {
            for (uint i = 0; i < sbSize; i++)
                if (specifiedBookings[sc][i] >= (SbBooking*) 4)
                {
                    uint j;
                    for (j = i + 1; j < sbSize &&
                         specifiedBookings[sc][i] == specifiedBookings[sc][j];
                         j++)
                        ;
                    delete specifiedBookings[sc][i];
                    i = j - 1;
                }
            delete [] specifiedBookings[sc];
            specifiedBookings[sc] = 0;
        }
    }
    delete [] allocationProbability;
    delete [] specifiedBookings;
    delete [] scoreboards;
    delete [] scenarios;

    delete limits;

    project->deleteResource(this);
}

void
Resource::deleteStaticData()
{
    delete [] DayStartIndex;
    delete [] WeekStartIndex;
    delete [] MonthStartIndex;
    delete [] DayEndIndex;
    delete [] WeekEndIndex;
    delete [] MonthEndIndex;
    DayStartIndex = 0;
    WeekStartIndex = 0;
    MonthStartIndex = 0;
    DayEndIndex = 0;
    WeekEndIndex = 0;
    MonthEndIndex = 0;
}

void
Resource::inheritValues()
{
    Resource* pr = (Resource*) parent;

    if (pr)
    {
        // Inherit flags from parent resource.
        for (QStringList::Iterator it = pr->flags.begin();
             it != pr->flags.end(); ++it)
            addFlag(*it);

        // Inherit default working hours from parent resource.
        for (int i = 0; i < 7; i++)
        {
            while (!workingHours[i]->isEmpty()) delete workingHours[i]->takeFirst();
            delete workingHours[i];
            workingHours[i] = new QList<Interval*>();
//             workingHours[i]->setAutoDelete(true);
            for (QListIterator<Interval*> ivi(*pr->workingHours[i]); ivi.hasNext();)
                workingHours[i]->append(new Interval(*(ivi.next())));
        }

        // Inherit vacation intervals from parent resource.
        for (QListIterator<Interval*> vli(pr->vacations); vli.hasNext();)
            vacations.append(new Interval(*(vli.next())));

        minEffort = pr->minEffort;

        if (pr->limits)
            limits = new UsageLimits(*pr->limits);
        else
            limits = 0;

        rate = pr->rate;
        efficiency = pr->efficiency;

        // Inherit inheritable custom attributes
        inheritCustomAttributes(project->getResourceAttributeDict());
    }
    else
    {
        // Inherit from default working hours project defaults.
        for (int i = 0; i < 7; i++)
        {
            while (!workingHours[i]->isEmpty()) delete workingHours[i]->takeFirst();
            delete workingHours[i];
            workingHours[i] = new QList<Interval*>();
//             workingHours[i]->setAutoDelete(true);
            for (QListIterator<Interval*>ivi(project->getWorkingHoursIterator(i)); ivi.hasNext();) {
                workingHours[i]->append(new Interval(*(ivi.next())));
            }
        }

        minEffort = project->getMinEffort();

        if (project->getResourceLimits())
            limits = new UsageLimits(*project->getResourceLimits());
        else
            limits = 0;

//         rate = project->getRate();
        efficiency = 1.0;
    }
}

void
Resource::setLimits(UsageLimits* l)
{
    if (limits)
        delete limits;
    limits = l;
}

void
Resource::initScoreboard()
{
    scoreboard = new SbBooking*[sbSize];

    // First mark all scoreboard slots as unavailable (1).
    for (uint i = 0; i < sbSize; i++)
        scoreboard[i] = (SbBooking*) 1;

    // Then change all worktime slots to 0 (available) again.
    for (time_t t = project->getStart(); t < project->getEnd() + 1;
         t += project->getScheduleGranularity())
    {
        if (isOnShift(Interval(t, t + project->getScheduleGranularity() - 1))) {
            scoreboard[sbIndex(t)] = (SbBooking*) 0;
        }
    }
    // Then mark all resource specific vacation slots as such (2).
    for (QListIterator<Interval*> ivi(vacations); ivi.hasNext();) {
        Interval *i = ivi.next();
        for (time_t date = i->getStart() > project->getStart() ?
             i->getStart() : project->getStart();
             date < i->getEnd() && date < project->getEnd() + 1;
             date += project->getScheduleGranularity()) {
            scoreboard[sbIndex(date)] = (SbBooking*) 2;
        }
    }
    // Mark all global vacation slots as such (2)
    for (VacationList::Iterator ivi(project->getVacationListIterator()); ivi.hasNext();)
    {
        Interval *i = ivi.next();
        if (i->getStart() > project->getEnd() ||
            i->getEnd() < project->getStart())
            continue;
        uint startIdx = sbIndex(i->getStart() >= project->getStart() ?
                                i->getStart() : project->getStart());
        uint endIdx = sbIndex(i->getEnd() >= project->getStart() ?
                              i->getEnd() : project->getEnd());
        for (uint idx = startIdx; idx <= endIdx; ++idx)
            scoreboard[idx] = (SbBooking*) 2;
    }
}

uint
Resource::sbIndex(time_t date) const
{
    if (date < project->getStart()) qDebug()<<"Resource::sbIndex:"<<time2ISO(date)<<time2ISO(project->getStart());
    assert(date >= project->getStart());
    if (date > project->getEnd()) qDebug()<<"Resource::sbIndex:"<<time2ISO(date)<<time2ISO(project->getEnd());
    assert(date <= project->getEnd());
    // Convert date to corresponding scoreboard index.
    uint sbIdx = (date - project->getStart()) /
        project->getScheduleGranularity();
    assert(sbIdx < sbSize);
    return sbIdx;
}

time_t
Resource::index2start(uint idx) const
{
    return project->getStart() + idx *
        project->getScheduleGranularity();
}

time_t
Resource::index2end(uint idx) const
{
    return project->getStart() + (idx + 1) *
        project->getScheduleGranularity() - 1;
}

/**
 * \retval 0 { resource is available }
 * \retval 1 { resource is unavailable }
 * \retval 2 { resource is on vacation }
 * \retval 3 { undefined }
 * \retval 4 { resource is allocated to a task }
 */
int
Resource::isAvailable(time_t date)
{
    /* The scoreboard of a resource is only generated on demand, so that large
     * resource lists that are only scarcely used for the project do not slow
     * TJ down too much. */
    if (!scoreboard)
        initScoreboard();
    // Check if the interval is booked or blocked already.
    uint sbIdx = sbIndex(date);
    if (scoreboard[sbIdx])
    {
        QString reason;
        if (scoreboard[sbIdx] == ((SbBooking*) 1)) {
            reason = "off-hour";
        } else if (scoreboard[sbIdx] == ((SbBooking*) 2)) {
            reason = "vacation";
        } else if (scoreboard[sbIdx] == ((SbBooking*) 3)) {
            reason = "UNDEFINED";
        } else {
            reason = "allocated to " + scoreboard[sbIdx]->getTask()->getName();
        }
        if (DEBUGRS(6))  {
            qDebug()<<QString("  Resource %1 is busy (%2) at: %3").arg(name).arg(reason).arg(time2ISO(date));
        }
        return scoreboard[sbIdx] < ((SbBooking*) 4) ? 1 : 4;;
    }

    if (!limits) {
//         TJMH.debugMessage(QString("Resource is available today (%1) ").arg(time2ISO(date)), this);
        return 0;
    }
    if (limits && limits->getDailyUnits() > 0) {
        int bookedSlots = 1;
        int workSlots = 0;
        for (uint i = DayStartIndex[sbIdx]; i <= DayEndIndex[sbIdx]; i++) {
            SbBooking* b = scoreboard[i];
            if (b == (SbBooking*) 0) {
                ++workSlots;
            } else if (b >= (SbBooking*) 4) {
                ++workSlots;
                ++bookedSlots;
            }
        }
        if ( workSlots > 0 ) {
            workSlots = (workSlots * limits->getDailyUnits()) / 100;
            if (workSlots == 0) {
                workSlots = 1;
            }
        }
        if (bookedSlots > workSlots) {
            if (DEBUGRS(2)) {
                qDebug()<<"Resource is overloaded:"<<name<<"units="<<limits->getDailyUnits()<<"work="<<workSlots<<"booked="<<bookedSlots;
            }
//             TJMH.debugMessage(QString("Resource is overloaded today: %1 (%2 slots)").arg(time2ISO(date)).arg(bookedSlots), this);
            return 2; //TODO review
        }
    }
    else if ((limits && limits->getDailyMax() > 0))
    {
        // Now check that the resource is not overloaded on this day.
        uint bookedSlots = 1;

        for (uint i = DayStartIndex[sbIdx]; i <= DayEndIndex[sbIdx]; i++)
        {
            SbBooking* b = scoreboard[i];
            if (b < (SbBooking*) 4)
                continue;

            bookedSlots++;
        }

        if (limits && limits->getDailyMax() > 0 &&
            bookedSlots > limits->getDailyMax())
        {
            if (DEBUGRS(6))
                qDebug()<<QString("  Resource %1 overloaded today (%2)").arg(name).arg(bookedSlots);

//             TJMH.debugMessage(QString("Resource is overloaded today: %1 (%2 slots)").arg(time2ISO(date)).arg(bookedSlots), this);
            return 2;
        }
    }
    if ((limits && limits->getWeeklyMax() > 0))
    {
        // Now check that the resource is not overloaded on this week.
        uint bookedSlots = 1;

        for (uint i = WeekStartIndex[sbIdx]; i <= WeekEndIndex[sbIdx]; i++)
        {
            SbBooking* b = scoreboard[i];
            if (b < (SbBooking*) 4)
                continue;

            bookedSlots++;
        }

        if (limits && limits->getWeeklyMax() > 0 &&
            bookedSlots > limits->getWeeklyMax())
        {
            if (DEBUGRS(6))
                qDebug()<<QString("  Resource %1 overloaded this week (%2)").arg(name).arg(bookedSlots);
            return 2;
        }
    }
    if ((limits && limits->getMonthlyMax() > 0))
    {
        // Now check that the resource is not overloaded on this month.
        uint bookedSlots = 1;

        for (uint i = MonthStartIndex[sbIdx]; i <= MonthEndIndex[sbIdx]; i++)
        {
            SbBooking* b = scoreboard[i];
            if (b < (SbBooking*) 4)
                continue;

            bookedSlots++;
        }

        if (limits && limits->getMonthlyMax() > 0 &&
            bookedSlots > limits->getMonthlyMax())
        {
            if (DEBUGRS(6))
                qDebug()<<QString("  Resource %1 overloaded this month (%2)").arg(name).arg(bookedSlots);
            return 2;
        }
    }
    return 0;
}

bool
Resource::book(Booking* nb)
{
    uint idx = sbIndex(nb->getStart());

    return bookSlot(idx, nb);
}

bool
Resource::bookSlot(uint idx, SbBooking* nb)
{
    // Make sure that the time slot is still available.
    if (scoreboard[idx] > (SbBooking*) 0)
    {
        delete nb;
        return false;
    }

    SbBooking* b;
    // Try to merge the booking with the booking in the previous slot.
    if (idx > 0 && (b = scoreboard[idx - 1]) >= (SbBooking*) 4 &&
        b->getTask() == nb->getTask())
    {
        scoreboard[idx] = b;
        delete nb;
        return true;
    }
    // Try to merge the booking with the booking in the following slot.
    if (idx < sbSize - 1 && (b = scoreboard[idx + 1]) >= (SbBooking*) 4 &&
        b->getTask() == nb->getTask())
    {
        scoreboard[idx] = b;
        delete nb;
        return true;
    }
    scoreboard[idx] = nb;
    return true;
}

//bool
//Resource::bookInterval(Booking* nb, int sc, int sloppy, int overtime)
//{
//    uint sIdx = sbIndex(nb->getStart());
//    uint eIdx = sbIndex(nb->getEnd());

//    bool conflict = false;

//    for (uint i = sIdx; i <= eIdx; i++)
//        if (scoreboard[i] > (SbBooking*) overtime)
//        {
//            uint j;
//            for (j = i + 1 ; j <= eIdx &&
//                 scoreboard[i] == scoreboard[j]; j++)
//                ;
//            if (scoreboard[i] == (SbBooking*) 1)
//            {
//                if (sloppy > 0)
//                {
//                    i = j;
//                    continue;
//                }
//                TJMH.errorMessage(i18nc("@info/plain 1=datetime 2=task name", "Resource is unavailable at %1. It cannot be assigned to task %2.", formatTime(index2start(i)), nb->getTask()->getName()), this);
//            }
//            else if (scoreboard[i] == (SbBooking*) 2)
//            {
//                if (sloppy > 1)
//                {
//                    i = j;
//                    continue;
//                }
//                TJMH.errorMessage(i18nc("@info/plain 1=datetime 2=task name", "Resource is on vacation at %1. It cannot be assigned to task %2.", formatTime(index2start(i)), nb->getTask()->getName()), this);
//            }
//            else
//            {
//                if (sloppy > 2)
//                {
//                    i = j;
//                    continue;
//                }
//                TJMH.errorMessage(i18nc("@info/plain 1=datetime 2=task name 3=task name", "Allocation conflict at %1. Conflicting tasks are %2 and %3.", formatTime(index2start(i)), scoreboard[i]->getTask()->getName(), nb->getTask()->getName()), this);
//            }

//            conflict = true;
//            i = j;
//        }

//    if (conflict)
//        return false;

//    for (uint i = sIdx; i <= eIdx; i++)
//        if (scoreboard[i] <= static_cast<SbBooking*>(1))
//            bookSlot(i, new SbBooking(*nb), overtime);

//    return true;
//}

//bool
//Resource::addBooking(int sc, Booking* nb, int sloppy, int overtime)
//{
//    SbBooking** tmp = scoreboard;

//    if (scoreboards[sc])
//        scoreboard = scoreboards[sc];
//    else
//        initScoreboard();
//    bool retVal = bookInterval(nb, sc, sloppy, overtime);
//    // Cross register booking with task.
//    if (retVal && nb->getTask())
//        nb->getTask()->addBookedResource(sc, this);
//    delete nb;
//    scoreboards[sc] = scoreboard;
//    scoreboard = tmp;

//    return retVal;
//}

bool
Resource::addShift(const Interval& i, Shift* s)
{
    return shifts.insert(new ShiftSelection(i, s));
}

bool
Resource::addShift(ShiftSelection* s)
{
    return shifts.insert(s);
}

void
Resource::addVacation(Interval* i)
{
    vacations.append(i);
}

bool
Resource::isWorker() const
{
    for (ConstResourceTreeIterator rti(this); *rti; ++rti)
        if ((*rti)->efficiency == 0.0)
            return false;

    return true;
}

double
Resource::getCurrentLoad(const Interval& period, const Task* task) const
{
    Interval iv(period);
    if (!iv.overlap(Interval(project->getStart(), project->getEnd())))
        return 0.0;

    return efficiency * project->convertToDailyLoad
        (getCurrentLoadSub(sbIndex(iv.getStart()), sbIndex(iv.getEnd()), task) *
         project->getScheduleGranularity());
}

long
Resource::getCurrentLoadSub(uint startIdx, uint endIdx, const Task* task) const
{
    long bookings = 0;

    for (ResourceListIterator rli(*sub); rli.hasNext();) {
        Resource *r = static_cast<Resource*>(rli.next());
        bookings += r->getCurrentLoadSub(startIdx, endIdx, task);
    }
    if (!scoreboard)
        return bookings;

    for (uint i = startIdx; i <= endIdx && i < sbSize; i++)
    {
        SbBooking* b = scoreboard[i];
        if (b < (SbBooking*) 4)
            continue;
        if (!task || task == b->getTask() || b->getTask()->isDescendantOf(task))
            bookings++;
    }

    return bookings;
}

uint
Resource::getWorkSlots(time_t date) const
{
    if (!scoreboard) {
        return 0;
    }
    uint workSlots = 0;
    uint sbIdx = sbIndex(date);
    for (uint i = DayStartIndex[sbIdx]; i <= DayEndIndex[sbIdx]; i++) {
        SbBooking* b = scoreboard[i];
        if (b == (SbBooking*) 0 || b >= (SbBooking*) 4) {
            ++workSlots;
        }
    }
    return workSlots;
}

uint
Resource::getCurrentDaySlots(time_t date, const Task* t)
{
    /* Return the number of slots this resource is allocated to in the current
     * scenario. If a task is given, only the slots allocated to this task
     * will be counted. */

    if (hasSubs())
    {
        uint timeSlots = 0;
        for (ResourceListIterator rli(getSubListIterator()); rli.hasNext();) {
            Resource *r = static_cast<Resource*>(rli.next());
            timeSlots += r->getCurrentDaySlots(date, t);
        }
        return timeSlots;
    }

    if (!scoreboard)
        return 0;

    uint sbIdx = sbIndex(date);

    uint bookedSlots = 0;

    for (uint i = DayStartIndex[sbIdx]; i <= DayEndIndex[sbIdx]; i++)
    {
        SbBooking* b = scoreboard[i];
        if (b < (SbBooking*) 4)
            continue;

        if (!t || b->getTask() == t || b->getTask()->isDescendantOf(t))
            bookedSlots++;
    }

    return bookedSlots;
}

uint
Resource::getCurrentWeekSlots(time_t date, const Task* t)
{
    /* Return the number of slots this resource is allocated to in the current
     * scenario. If a task is given, only the slots allocated to this task
     * will be counted. */

    if (hasSubs())
    {
        uint timeSlots = 0;
        for (ResourceListIterator rli(getSubListIterator()); rli.hasNext();) {
            Resource *r = static_cast<Resource*>(rli.next());
            timeSlots += (*rli)->getCurrentWeekSlots(date, t);
        }
        return timeSlots;
    }

    if (!scoreboard)
        return 0;

    uint sbIdx = sbIndex(date);

    uint bookedSlots = 0;

    for (uint i = WeekStartIndex[sbIdx]; i <= WeekEndIndex[sbIdx]; i++)
    {
        SbBooking* b = scoreboard[i];
        if (b < (SbBooking*) 4)
            continue;

        if (!t || b->getTask() == t || b->getTask()->isDescendantOf(t))
            bookedSlots++;
    }

    return bookedSlots;
}

uint
Resource::getCurrentMonthSlots(time_t date, const Task* t)
{
    /* Return the number of slots this resource is allocated to in the current
     * scenario. If a task is given, only the slots allocated to this task
     * will be counted. */

    if (hasSubs())
    {
        uint timeSlots = 0;
        for (ResourceListIterator rli(getSubListIterator()); rli.hasNext();) {
            Resource *r = static_cast<Resource*>(rli.next());
            timeSlots += (*rli)->getCurrentMonthSlots(date, t);
        }
        return timeSlots;
    }

    if (!scoreboard)
        return 0;

    uint sbIdx = sbIndex(date);

    uint bookedSlots = 0;

    for (uint i = MonthStartIndex[sbIdx]; i <= MonthEndIndex[sbIdx]; i++)
    {
        SbBooking* b = scoreboard[i];
        if (b < (SbBooking*) 4)
            continue;

        if (!t || b->getTask() == t || b->getTask()->isDescendantOf(t))
            bookedSlots++;
    }

    return bookedSlots;
}

double
Resource::getEffectiveLoad(int sc, const Interval& period, AccountType acctType,
                           const Task* task) const
{
    double load = 0.0;
    Interval iv(period);
    if (!iv.overlap(Interval(project->getStart(), project->getEnd())))
        return 0.0;

    if (hasSubs())
    {
        for (ResourceListIterator rli(*sub); rli.hasNext();) {
            Resource *r = static_cast<Resource*>(rli.next());
            load += r->getEffectiveLoad(sc, iv, acctType, task);
        }
    }
    else
    {
        uint startIdx = sbIndex(iv.getStart());
        uint endIdx = sbIndex(iv.getEnd());
        load = project->convertToDailyLoad
            (getAllocatedSlots(sc, startIdx, endIdx, acctType, task) *
             project->getScheduleGranularity()) * efficiency;
    }

    return load;
}

double
Resource::getAllocatedTimeLoad(int sc, const Interval& period,
                               AccountType acctType, const Task* task) const
{
    return project->convertToDailyLoad
        (getAllocatedTime(sc, period, acctType, task));
}

long
Resource::getAllocatedTime(int sc, const Interval& period, AccountType acctType,
                          const Task* task) const
{
    Interval iv(period);
    if (!iv.overlap(Interval(project->getStart(), project->getEnd())))
        return 0;
    uint startIdx = sbIndex(iv.getStart());
    uint endIdx = sbIndex(iv.getEnd());
    if (scenarios[sc].firstSlot > 0 && scenarios[sc].lastSlot > 0)
    {
        if (startIdx < (uint) scenarios[sc].firstSlot)
            startIdx = scenarios[sc].firstSlot;
        if (endIdx > (uint) scenarios[sc].lastSlot)
            endIdx = scenarios[sc].lastSlot;
    }

    return getAllocatedSlots(sc, startIdx, endIdx, acctType, task) *
        project->getScheduleGranularity();
}

long
Resource::getAllocatedSlots(int sc, uint startIdx, uint endIdx,
                            AccountType acctType, const Task* task) const
{
    long bookings = 0;

    if (isGroup())
    {
        for (ResourceListIterator rli(*sub); rli.hasNext();) {
            Resource *r = static_cast<Resource*>(rli.next());
            bookings += r->getAllocatedSlots(sc, startIdx, endIdx,
                                                  acctType, task);
        }
        return bookings;
    }

    // If the scoreboard has not been initialized there is no load.
    if (!scoreboards[sc])
        return bookings;

    if (scenarios[sc].firstSlot > 0 && scenarios[sc].lastSlot > 0)
    {
        if (task)
        {
            /* If the load is to be calculated for a certain task, we check
             * whether this task is in the resource allocation list. Only then
             * we calculate the real number of allocated slots. */
            bool isAllocated = false;
            for (TaskListIterator tli(scenarios[sc].allocatedTasks); tli.hasNext();) {
                Task *t = static_cast<Task*>(tli.next());
                if (task == t || t->isDescendantOf(task))
                {
                    isAllocated = true;
                    break;
                }
            }
            if (!isAllocated)
                return bookings;
        }

        if (startIdx < (uint) scenarios[sc].firstSlot)
            startIdx = scenarios[sc].firstSlot;
        if (endIdx > (uint) scenarios[sc].lastSlot)
            endIdx = scenarios[sc].lastSlot;
    }
    for (uint i = startIdx; i <= endIdx && i < sbSize; i++)
    {
        SbBooking* b = scoreboards[sc][i];
        if (b < (SbBooking*) 4)
            continue;
        if ((task == 0 ||
            (task != 0 && (task == b->getTask() ||
             b->getTask()->isDescendantOf(task))))/* &&
            (acctType == AllAccounts ||
            (b->getTask()->getAccount() && b->getTask()->getAccount()->getAcctType() == acctType))*/)
            bookings++;
    }

    return bookings;
}

double
Resource::getEffectiveFreeLoad(int sc, const Interval& period)
{
    double load = 0.0;
    Interval iv(period);
    if (!iv.overlap(Interval(project->getStart(), project->getEnd())))
        return 0.0;

    if (hasSubs())
    {
        for (ResourceListIterator rli(*sub); *rli != 0; ++rli)
            load += (*rli)->getEffectiveFreeLoad(sc, iv);
    }
    else
    {
        uint startIdx = sbIndex(iv.getStart());
        uint endIdx = sbIndex(iv.getEnd());
        load = project->convertToDailyLoad
            (getAvailableSlots(sc, startIdx, endIdx) *
             project->getScheduleGranularity()) * efficiency;
    }

    return load;
}

double
Resource::getAvailableTimeLoad(int sc, const Interval& period)
{
    return project->convertToDailyLoad(getAvailableTime(sc, period));
}

long
Resource::getAvailableTime(int sc, const Interval& period)
{
    Interval iv(period);
    if (!iv.overlap(Interval(project->getStart(), project->getEnd())))
        return 0;

    return getAvailableSlots(sc, sbIndex(iv.getStart()),
                             sbIndex(iv.getEnd())) *
        project->getScheduleGranularity();
}

long
Resource::getAvailableSlots(int sc, uint startIdx, uint endIdx)
{
    long availSlots = 0;

    if (!sub->isEmpty())
    {
        for (ResourceListIterator rli(*sub); *rli != 0; ++rli)
            availSlots += (*rli)->getAvailableSlots(sc, startIdx, endIdx);
    }
    else
    {
        if (!scoreboards[sc])
        {
            scoreboard = scoreboards[sc];
            initScoreboard();
            scoreboards[sc] = scoreboard;
        }

        for (uint i = startIdx; i <= endIdx; i++)
            if (scoreboards[sc][i] == 0)
                availSlots++;
    }

    return availSlots;
}

double
Resource::getCredits(int sc, const Interval& period, AccountType acctType,
                     const Task* task) const
{
    return project->convertToDailyLoad
        (getAllocatedTime(sc, period, acctType, task)) * rate;
}

bool
Resource::isAllocated(int sc, const Interval& period, const QString& prjId)
    const
{
    Interval iv(period);
    if (!iv.overlap(Interval(project->getStart(), project->getEnd())))
        return false;

    uint startIdx = sbIndex(iv.getStart());
    uint endIdx = sbIndex(iv.getEnd());
    if (scenarios[sc].firstSlot > 0 && scenarios[sc].lastSlot > 0)
    {
        if (startIdx < (uint) scenarios[sc].firstSlot)
            startIdx = scenarios[sc].firstSlot;
        if (endIdx > (uint) scenarios[sc].lastSlot)
            endIdx = scenarios[sc].lastSlot;
    }

    if (endIdx < startIdx)
        return false;

    return isAllocatedSub(sc, startIdx, endIdx, prjId);
}

bool
Resource::isAllocatedSub(int sc, uint startIdx, uint endIdx, const QString&
                         prjId) const
{
    /* If resource is a group, check members first. */
    for (ResourceListIterator rli(*sub); *rli != 0; ++rli)
        if ((*rli)->isAllocatedSub(sc, startIdx, endIdx, prjId))
            return true;

    if (!scoreboards[sc])
        return false;
    for (uint i = startIdx; i <= endIdx; i++)
    {
        SbBooking* b = scoreboards[sc][i];
        if (b < (SbBooking*) 4)
            continue;
        if (prjId.isNull() || b->getTask()->getProjectId() == prjId)
            return true;
    }
    return false;
}

bool
Resource::isAllocated(int sc, const Interval& period, const Task* task) const
{
    Interval iv(period);
    if (!iv.overlap(Interval(project->getStart(), project->getEnd())))
        return false;

    uint startIdx = sbIndex(iv.getStart());
    uint endIdx = sbIndex(iv.getEnd());
    if (scenarios[sc].firstSlot > 0 && scenarios[sc].lastSlot > 0)
    {
        if (startIdx < (uint) scenarios[sc].firstSlot)
            startIdx = scenarios[sc].firstSlot;
        if (endIdx > (uint) scenarios[sc].lastSlot)
            endIdx = scenarios[sc].lastSlot;
    }
    if (endIdx < startIdx)
        return false;

    return isAllocatedSub(sc, startIdx, endIdx, task);
}

bool
Resource::isAllocatedSub(int sc, uint startIdx, uint endIdx, const Task* task)
    const
{
    /* If resource is a group, check members first. */
    for (ResourceListIterator rli(*sub); *rli != 0; ++rli)
        if ((*rli)->isAllocatedSub(sc, startIdx, endIdx, task))
            return true;

    if (!scoreboards[sc])
        return false;
    for (uint i = startIdx; i <= endIdx; i++)
    {
        SbBooking* b = scoreboards[sc][i];
        if (b < (SbBooking*) 4)
            continue;
        if (!task || b->getTask() == task || b->getTask()->isDescendantOf(task))
            return true;
    }
    return false;
}

void
Resource::getPIDs(int sc, const Interval& period, const Task* task,
                  QStringList& pids) const
{
    Interval iv(period);
    if (!iv.overlap(Interval(project->getStart(), project->getEnd())))
        return;

    for (ResourceListIterator rli(*sub); *rli != 0; ++rli)
        (*rli)->getPIDs(sc, iv, task, pids);

    if (!scoreboards[sc])
        return;
    for (uint i = sbIndex(iv.getStart());
         i <= sbIndex(iv.getEnd()) && i < sbSize; i++)
    {
        SbBooking* b = scoreboards[sc][i];
        if (b < (SbBooking*) 4)
            continue;
        if ((!task || task == b->getTask() ||
             b->getTask()->isDescendantOf(task)) &&
            pids.indexOf(b->getTask()->getProjectId()) == -1)
        {
            pids.append(b->getTask()->getProjectId());
        }
    }
}

QString
Resource::getProjectIDs(int sc, const Interval& period, const Task* task) const
{
    QStringList pids;
    getPIDs(sc, period, task, pids);
    QString pidStr;
    for (QStringList::Iterator it = pids.begin(); it != pids.end(); ++it)
        pidStr += QString(it != pids.begin() ? ", " : "") + *it;

    return pidStr;
}

bool
Resource::hasVacationDay(time_t day) const
{
    Interval fullDay(midnight(day),
                     sameTimeNextDay(midnight(day)) - 1);
    for (QListIterator<Interval*> vli(vacations); vli.hasNext();) {
        if (vli.next()->overlaps(fullDay))
            return true;
    }

    if (shifts.isVacationDay(day))
        return true;

    if (workingHours[dayOfWeek(day, false)]->isEmpty())
        return true;

    return false;
}

bool
Resource::isOnShift(const Interval& slot) const
{
    for (ShiftSelectionList::Iterator ssli(shifts); ssli.hasNext();) {
        ShiftSelection *s = ssli.next();
        if (s->getPeriod().contains(slot))
            return s->getShift()->isOnShift(slot);
    }
    int dow = dayOfWeek(slot.getStart(), false);
    for (QListIterator<Interval*> ivi(*workingHours[dow]); ivi.hasNext();) {
        if (ivi.next()->contains(Interval(secondsOfDay(slot.getStart()), secondsOfDay(slot.getEnd())))) {
            return true;
        }
    }
    return false;
}

void
Resource::setWorkingHours(int day, const QList<Interval*>& l)
{
    while (!workingHours[day]->isEmpty()) delete workingHours[day]->takeFirst();
    delete workingHours[day];

    // Create a deep copy of the interval list.
    workingHours[day] = new QList<Interval*>;
//     workingHours[day]->setAutoDelete(true);
    for (QListIterator<Interval*> pli(l); pli.hasNext();)
        workingHours[day]->append(new Interval(*(pli.next())));
}

BookingList
Resource::getJobs(int sc) const
{
    BookingList bl;
    if (scoreboards[sc])
    {
        SbBooking* b = 0;
        uint startIdx = 0;
        for (uint i = 0; i < sbSize; i++)
            if (scoreboards[sc][i] != b)
            {
                if (b)
                    bl.append(new Booking(Interval(index2start(startIdx),
                                                   index2end(i - 1)),
                                          scoreboards[sc][startIdx]));
                if (scoreboards[sc][i] >= (SbBooking*) 4)
                {
                    b = scoreboards[sc][i];
                    startIdx = i;
                }
                else
                    b = 0;
            }
    }
    return bl;
}

QList<Interval> Resource::getBookedIntervals(int sc, const TJ::Task* task) const
{
    QList<Interval> lst;
    if (scoreboards[sc] == 0)
        return lst;
    for (uint i = 0; i < sbSize; ++i)
    {
        if (scoreboards[sc][i] > ((SbBooking*) 3) && scoreboards[sc][i]->getTask() == task) {
            time_t s = index2start(i);
            time_t e = index2end(i);
            Interval ti(s, e);
            if (!lst.isEmpty() && lst.last().append(ti)) {
                continue;
            }
            lst << ti;
        }
    }
    return lst;
}

time_t
Resource::getStartOfFirstSlot(int sc, const Task* task)
{
    if (scoreboards[sc] == 0)
        return 0;
    for (uint i = 0; i < sbSize; ++i)
    {
        if (scoreboards[sc][i] > ((SbBooking*) 3) &&
            scoreboards[sc][i]->getTask() == task)
            return index2start(i);
    }

    return 0;
}

time_t
Resource::getEndOfLastSlot(int sc, const Task* task)
{
    if (scoreboards[sc] == 0)
        return 0;
    int i = sbSize;
    for ( ; ; )
    {
        --i;
        if (scoreboards[sc][i] > ((SbBooking*) 3) &&
            scoreboards[sc][i]->getTask() == task)
            return index2end(i);
        if (i == 0)
            break;
    }

    return 0;
}

void
Resource::copyBookings(int sc, SbBooking*** src, SbBooking*** dst)
{
    /* This function copies a set of bookings the specified scenario. If the
     * destination set already contains bookings it is cleared first.
     */
    if (dst[sc])
        for (uint i = 0; i < sbSize; i++)
            if (dst[sc][i] >= (SbBooking*) 4)
            {
                /* Small pointers are fake bookings. We can safely ignore
                 * them. Identical pointers in successiv slots must only be
                 * deleted once. */
                uint j;
                for (j = i + 1; j < sbSize &&
                     dst[sc][i] == dst[sc][j]; j++)
                    ;
                delete dst[sc][i];
                i = j - 1;
            }

    // Now copy the source set to the destination.
    if (src[sc])
    {
        if (!dst[sc])
            dst[sc] = new SbBooking*[sbSize];
        for (uint i = 0; i < sbSize; i++)
            if (src[sc][i] >= (SbBooking*) 4)
            {
                /* Small pointers can just be copied. Identical successiv
                 * pointers need to be allocated once and can then be assigned
                 * to all destination slots. */
                dst[sc][i] = new SbBooking(src[sc][i]);
                uint j;
                for (j = i + 1; j < sbSize &&
                     src[sc][i] == src[sc][j]; j++)
                    dst[sc][j] = dst[sc][i];
                i = j - 1;
            }
            else
                dst[sc][i] = src[sc][i];
    }
    else
    {
        delete [] dst[sc];
        dst[sc] = 0;
    }
}

void
Resource::saveSpecifiedBookings()
{
    for (int sc = 0; sc < project->getMaxScenarios(); sc++)
        copyBookings(sc, scoreboards, specifiedBookings);
}

void
Resource::prepareScenario(int sc)
{
    copyBookings(sc, specifiedBookings, scoreboards);
    scoreboard = scoreboards[sc];

    updateSlotMarks(sc);
}

void
Resource::finishScenario(int sc)
{
    scoreboards[sc] = scoreboard;
    updateSlotMarks(sc);
}

bool
Resource::bookingsOk(int sc)
{
    if (scoreboards[sc] == 0)
        return true;

    if (hasSubs())
    {
       TJMH.debugMessage(QString("Group resource may not have bookings"), this);
       return false;
    }

    for (uint i = 0; i < sbSize; ++i)
        if (scoreboards[sc][i] >= ((SbBooking*) 4))
        {
            time_t start = index2start(i);
            time_t end = index2end(i);
            time_t tStart = scoreboards[sc][i]->getTask()->getStart(sc);
            time_t tEnd = scoreboards[sc][i]->getTask()->getEnd(sc);
            if (start < tStart || start > tEnd ||
                end < tStart || end > tEnd)
            {
                TJMH.errorMessage(i18nc("@info/plain 1=task name, 2, 3, 4=datetime", "Booking on task '%1' at %2 is outside of task interval (%3 - %4)", scoreboards[sc][i]->getTask()->getName(),formatTime(start), formatTime(tStart), formatTime(tEnd)), this);
                return false;
            }
        }

    return true;
}

// void
// Resource::addJournalEntry(JournalEntry* entry)
// {
//     journal.append(entry);
// }
// 
// Journal::Iterator
// Resource::getJournalIterator() const
// {
//     return Journal::Iterator(journal);
// }

void
Resource::updateSlotMarks(int sc)
{
    scenarios[sc].allocatedTasks.clear();
    scenarios[sc].firstSlot = -1;
    scenarios[sc].lastSlot = -1;

    if (scoreboard)
    {
        for (uint i = 0; i < sbSize; i++)
            if (scoreboard[i] > (SbBooking*) 4)
            {
                if (scenarios[sc].firstSlot == -1)
                    scenarios[sc].firstSlot = i;
                scenarios[sc].lastSlot = i;
                scenarios[sc].addTask(scoreboard[i]->getTask());
            }
    }
}

QDomElement Resource::xmlIDElement( QDomDocument& doc ) const
{
   QDomElement elem;/* = ReportXML::createXMLElem( doc, "Resource", getName());
   elem.setAttribute( "Id", getId() );*/

   return( elem );
}

} // namespace TJ


