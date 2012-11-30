/*
 * Resource.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006
 * by Chris Schlaeger <cs@kde.org>
 * Copyright (c) 2011 by Dag Andersen <danders@get2net.dk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */
#ifndef _Resource_h_
#define _Resource_h_

#include "kplatotj_export.h"

#include "ResourceScenario.h"

class QDomDocument;
class QDomElement;

namespace TJ
{

class Project;
class Shift;
class Task;
class Booking;
class SbBooking;
class BookingList;
class Interval;
class UsageLimits;

/**
 * @short Stores all information about a resource.
 * @author Chris Schlaeger <cs@kde.org>
 */
class KPLATOTJ_EXPORT Resource : public CoreAttributes
{
    friend int ResourceList::compareItemsLevel(CoreAttributes* c1,
                                               CoreAttributes* c2, int level);
public:
    Resource(Project* p, const QString& i, const QString& n, Resource* pr,
             const QString& df = QString(), uint dl = 0);
    virtual ~Resource();

    static void deleteStaticData();

    virtual CAType getType() const { return CA_Resource; }

    Resource* getParent() const { return static_cast<Resource*>(parent); }

    ResourceListIterator getSubListIterator() const
    {
        return ResourceListIterator(*sub);
    }

    void inheritValues();

    bool isGroup() const { return !sub->isEmpty(); }

    void setMinEffort(double e) { minEffort = e; }
    double getMinEffort() const { return minEffort; }

    void setLimits(UsageLimits* l);

    const UsageLimits* getLimits() const { return limits; }

    void setEfficiency(double e) { efficiency = e; }
    double getEfficiency() const { return efficiency; }

    bool isWorker() const;

    void setRate(double r) { rate = r; }
    double getRate() const { return rate; }

    void addVacation(Interval* i);
    QListIterator<Interval*> getVacationListIterator() const
    {
        return QListIterator<Interval*>(vacations);
    }

    bool hasVacationDay(time_t day) const;

    bool isOnShift(const Interval& slot) const;

    void setWorkingHours(int day, const QList<Interval*>& l);
    const QList<Interval*>* const* getWorkingHours() const
    {
        return static_cast<const QList<Interval*>* const*>(workingHours);
    }

    bool addShift(const Interval& i, Shift* s);
    bool addShift(ShiftSelection* s);

    const ShiftSelectionList* getShiftList() const
    {
        return &shifts;
    }

    /**
    * \retval 0 { resource is available }
    * \retval 1 { resource is unavailable }
    * \retval 2 { resource is on vacation }
    * \retval 3 { undefined }
    * \retval 4 { resource is allocated to a task }
    */
    int isAvailable(time_t day);

    bool book(Booking* b);

    bool bookSlot(uint idx, SbBooking* nb);
//    bool bookInterval(Booking* b, int sc, int sloppy = 0, int overtime = 0);
//    bool addBooking(int sc, Booking* b, int sloppy = 0, int overtime = 0);
    /// Return a list of booked intervals for scenario @p sc and task @p task
    QList<Interval> getBookedIntervals(int sc, const Task* task) const;

    double getCurrentLoad(const Interval& i, const Task* task = 0) const;

    /// Get the number of work slots for the day @p date. It includes both free and booked.
    uint getWorkSlots(time_t date) const;

    uint getCurrentDaySlots(time_t date, const Task* t);
    uint getCurrentWeekSlots(time_t date, const Task* t);
    uint getCurrentMonthSlots(time_t date, const Task* t);

    /***
     * Return the load of the resource (and its children) weighted by their
     * efficiency.
     */
    double getEffectiveLoad(int sc, const Interval& i,
                            AccountType acctType = AllAccounts,
                             const Task* task = 0) const;
    double getAllocatedTimeLoad(int sc, const Interval& period,
                                AccountType acctType, const Task* task = 0)
        const;
    long getAllocatedTime(int sc, const Interval& period, AccountType acctType,
                          const Task* task = 0) const;

    /***
     * Return the unallocated load of the resource and its children wheighted
     * by their efficiency.
     */
    double getEffectiveFreeLoad(int sc, const Interval& period);
    double getAvailableTimeLoad(int sc, const Interval& period);
    long getAvailableTime(int sc, const Interval& period);

    double getCredits(int sc, const Interval& i, AccountType acctType,
                      const Task* task = 0) const;

    QString getProjectIDs(int sc, const Interval& i, const Task* task = 0)
        const;

    bool isAllocated(int sc, const Interval& i,
                     const QString& prjId = QString()) const;

    bool isAllocated(int sc, const Interval& i, const Task* t) const;

    BookingList getJobs(int sc) const;

    time_t getStartOfFirstSlot(int sc, const Task* task);
    time_t getEndOfLastSlot(int sc, const Task* task);

    QDomElement xmlIDElement( QDomDocument& doc ) const;

    void copyBookings(int sc, SbBooking*** srd, SbBooking*** dst);
    void saveSpecifiedBookings();
    void prepareScenario(int sc);
    void finishScenario(int sc);

    bool bookingsOk(int sc);

    void resetAllocationProbability(int sc) { allocationProbability[sc] = 0; }
    void addAllocationProbability(int sc, double ap)
    {
        allocationProbability[sc] += ap;
    }
    double getAllocationProbability(int sc) const
    {
        return allocationProbability[sc];
    }

    TaskListIterator getTaskListIterator(int sc) const
    {
        return TaskListIterator(scenarios[sc].allocatedTasks);
    }

//     void addJournalEntry(JournalEntry* entry);

//     bool hasJournal() const { return !journal.isEmpty(); }

//     Journal::Iterator getJournalIterator() const;

private:
    void getPIDs(int sc, const Interval& period, const Task* task,
                 QStringList& pids) const;

    void initScoreboard();

    long getCurrentLoadSub(uint startIdx, uint endIdx, const Task* task) const;

    long getAllocatedSlots(int sc, uint startIdx, uint endIdx,
                           AccountType acctType, const Task* task) const;

    long getAvailableSlots(int sc, uint startIdx, uint endIdx);

    bool isAllocatedSub(int sc, uint startIdx, uint endIdx, const QString&
                        prjId) const;
    bool isAllocatedSub(int sc, uint startIdx, uint endIdx, const Task* task)
        const;
    void updateSlotMarks(int sc);

    uint sbIndex(time_t date) const;

    time_t index2start(uint idx) const;
    time_t index2end(uint idx) const;

    /// The minimum effort (in man days) the resource should be used per day.
    double minEffort;

    /// List of notes with a date attached.
//     Journal journal;

    /// Usage limits of the resource.
    UsageLimits* limits;

    /**
     * The efficiency of the resource. A team of five should have an
     * efficiency of 5.0 */
    double efficiency;

    /// The daily costs of this resource.
    double rate;

    /// The list of standard working or opening hours for the resource.
    QList<Interval*>* workingHours[7];

    /**
     * In addition to the standard working hours a set of shifts can be
     * defined. This is useful when the working hours change over time.
     * A shift is only active in a defined interval. If no interval is
     * defined for a period of time the standard working hours of the
     * resource are used.
     */
    ShiftSelectionList shifts;

    /// List of all intervals the resource is not available.
    QList<Interval*> vacations;

    /**
     * For each time slot (of length scheduling granularity) we store:
     * A pointer to a booking, or
     * 0 if the resource is available,
     * 1 if slot is off-hours,
     * 2 if slot is during a vacation.
     */
    SbBooking** scoreboard;
    /// The number of time slots in the project.
    uint sbSize;

    SbBooking*** specifiedBookings;
    SbBooking*** scoreboards;

    ResourceScenario* scenarios;

    /**
     * The allocation probability is calculated prior to scheduling a
     * scenario. It is the expected average effort the resource has to deliver
     * based on the assignments to tasks, not taking parallel assignments into
     * account.
     */
    double* allocationProbability;
} ;

} // namespace TJ

#endif
