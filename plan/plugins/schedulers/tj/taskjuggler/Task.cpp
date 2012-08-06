/*
 * Task.cpp - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006
 * Chris Schlaeger <cs@kde.org>
 * Copyright (c) 2011 Dag Andersen <danders@get2net.dk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#include "Task.h"

#include <KLocale>

#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "TjMessageHandler.h"
#include "tjlib-internal.h"
#include "Resource.h"
// #include "Account.h"
#include "Project.h"
#include "ResourceTreeIterator.h"
#include "Allocation.h"
#include "Booking.h"
// #include "ReportXML.h"
#include "Scenario.h"
#include "CustomAttributeDefinition.h"
#include "UsageLimits.h"
#include <QExplicitlySharedDataPointer>

namespace TJ
{

Task::Task(Project* proj, const QString& id_, const QString& n, Task* p,
           const QString& df, int dl) :
    CoreAttributes(proj, id_, n, p, df, dl),
    note(),
//     journal(),
    ref(),
    refLabel(),
    depends(),
    precedes(),
    predecessors(),
    successors(),
    previous(),
    followers(),
    projectId(),
    milestone(false),
    priority(0),
    scheduling(ASAP),
    responsible(0),
    shifts(),
    allocations(),
//     account(0),
    scenarios(new TaskScenario[proj->getMaxScenarios()]),
    start(0),
    end(0),
    length(0.0),
    effort(0.0),
    duration(0.0),
    doneEffort(0.0),
    doneLength(0.0),
    doneDuration(0.0),
    workStarted(false),
    tentativeStart(0),
    tentativeEnd(0),
    lastSlot(0),
    schedulingDone(false),
    runAway(false),
    bookedResources()
{
//     qDebug()<<"Task:"<<this;
    Q_ASSERT(proj->getMaxScenarios() > 0);
//     allocations.setAutoDelete(true);
//     shifts.setAutoDelete(true);
//     depends.setAutoDelete(true);
//     precedes.setAutoDelete(true);

    proj->addTask(this);

    for (int i = 0; i < proj->getMaxScenarios(); i++)
    {
        scenarios[i].task = this;
        scenarios[i].index = i;
    }

    scenarios[0].startBuffer = 0.0;
    scenarios[0].endBuffer = 0.0;
    scenarios[0].startCredit = 0.0;
    scenarios[0].endCredit = 0.0;

    for (int sc = 0; sc < project->getMaxScenarios(); ++sc)
    {
        scenarios[sc].minStart = scenarios[sc].minEnd = 0;
        scenarios[sc].maxStart = scenarios[sc].maxEnd = 0;
    }
    Q_ASSERT(duration == 0.0);
    Q_ASSERT(length == 0.0);
    Q_ASSERT(effort == 0.0);
}

Task::~Task()
{
//     qDebug()<<"~Task:"<<this;
    project->deleteTask(this);
    delete [] scenarios;
    qDeleteAll( depends );
    qDeleteAll( precedes );
    qDeleteAll( allocations );
}

void
Task::inheritValues()
{
    Task* p = static_cast<Task*>(parent);
    if (p)
    {
        // Inherit flags from parent task.
        for (QStringList::Iterator it = p->flags.begin();
             it != p->flags.end(); ++it)
            addFlag(*it);

        // Set attributes that are inherited from parent task.
        projectId = p->projectId;
        priority = p->priority;
        responsible = p->responsible;
//         account = p->account;
        scheduling = p->scheduling;

        for (int sc = 0; sc < project->getMaxScenarios(); ++sc)
        {
            scenarios[sc].minStart = p->scenarios[sc].minStart;
            scenarios[sc].maxStart = p->scenarios[sc].maxEnd;
            scenarios[sc].minEnd = p->scenarios[sc].minStart;
            scenarios[sc].maxEnd = p->scenarios[sc].maxEnd;
        }
        // Inherit depends from parent. Relative IDs need to get another '!'.
        for (QListIterator<TaskDependency*> tdi(p->depends); tdi.hasNext();)
        {
            TaskDependency *tdc = static_cast<TaskDependency*>(tdi.next());
            QString id = tdc->getTaskRefId();
            if (id[0] == '!')
                id = '!' + id;
            TaskDependency* td = new TaskDependency(id,
                                                    project->getMaxScenarios());
            for (int sc = 0; sc < project->getMaxScenarios(); ++sc)
            {
                td->setGapDuration(sc, tdc->getGapDurationNR(sc));
                td->setGapLength(sc, tdc->getGapLengthNR(sc));
            }
            depends.append(td);
        }

        // Inherit precedes from parent. Relative IDs need to get another '!'.
        for (QListIterator<TaskDependency*> tdi(p->precedes); tdi.hasNext();)
        {
            TaskDependency *tdc = static_cast<TaskDependency*>(tdi.next());
            QString id = tdc->getTaskRefId();
            if (id[0] == '!')
                id = '!' + id;
            TaskDependency* td = new TaskDependency(id,
                                                    project->getMaxScenarios());
            for (int sc = 0; sc < project->getMaxScenarios(); ++sc)
            {
                td->setGapDuration(sc, tdc->getGapDurationNR(sc));
                td->setGapLength(sc, tdc->getGapLengthNR(sc));
            }
            precedes.append(td);
        }

        // Inherit allocations from parent.
        for (QListIterator<Allocation*> ali(p->allocations); ali.hasNext();) {
            Allocation *a = static_cast<Allocation*>(ali.next());
            allocations.append(new Allocation(*(ali.next())));
        }

        // Inherit inheritable custom attributes
        inheritCustomAttributes(project->getTaskAttributeDict());
    }
    else
    {
        // Set attributes that are inherited from global attributes.
        projectId = project->getCurrentId();
        priority = project->getPriority();
        for (int sc = 0; sc < project->getMaxScenarios(); ++sc)
        {
            scenarios[sc].minStart = scenarios[sc].minEnd = 0;
            scenarios[sc].maxStart = scenarios[sc].maxEnd = 0;
        }
    }
}

// void
// Task::addJournalEntry(JournalEntry* entry)
// {
//     journal.inSort(entry);
// }
// 
// Journal::Iterator
// Task::getJournalIterator() const
// {
//     return Journal::Iterator(journal);
// }

TaskDependency*
Task::addDepends(const QString& rid)
{
    foreach ( TaskDependency *d, depends ) {
        if ( rid == d->getTaskRefId() ) {
            return d;
        }
    }
    TaskDependency* td = new TaskDependency(rid, project->getMaxScenarios());
    depends.append(td);
    return td;
}

TaskDependency*
Task::addPrecedes(const QString& rid)
{
    foreach ( TaskDependency *d, precedes ) {
        if ( rid == d->getTaskRefId() ) {
            return d;
        }
    }
    TaskDependency* td = new TaskDependency(rid, project->getMaxScenarios());
    precedes.append(td);
    return td;
}

bool
Task::addShift(const Interval& i, Shift* s)
{
    return shifts.insert(new ShiftSelection(i, s));
}

void
Task::errorMessage(const QString& msg) const
{
    TJMH.errorMessage(msg, this);
//     TJMH.errorMessage(msg, definitionFile, definitionLine);
}

void
Task::warningMessage(const QString& msg) const
{
    TJMH.warningMessage(msg, this);
//     TJMH.warningMessage(msg, definitionFile, definitionLine);
}

bool
Task::schedule(int sc, time_t& date, time_t slotDuration)
{
    // Has the task been scheduled already or is it a container?
    if (schedulingDone || !sub->isEmpty())
        return false;

    if (DEBUGTS(15))
        qDebug()<<QString("Trying to schedule %1 at %2").arg(name).arg(time2tjp(date));

    if (scheduling == Task::ASAP)
    {
        if (start == 0 ||
            (effort == 0.0 && length == 0.0 && duration == 0.0 && end == 0)) {
            if ( start == 0 ) {
                warningMessage(i18nc("@info/plain", "Cannot schedule: Valid start time is not set"));
            } else {
                warningMessage(i18nc("@info/plain", "Cannot schedule: Estimate is 0"));
            }
            return false;
        }

        if (lastSlot == 0)
        {
            lastSlot = start - 1;
            tentativeEnd = date + slotDuration - 1;
            if (DEBUGTS(5))
                qDebug()<<"Scheduling of ASAP task"<<name<<"starts at"<<time2tjp(start)<<"("<<time2tjp(date)<<")";
        }
        /* Do not schedule anything if the time slot is not directly
         * following the time slot that was previously scheduled.
         * The project should get back to us later */
        if (!((date - slotDuration <= lastSlot) && (lastSlot < date))) {
            if (DEBUGTS(20)) {
                qDebug()<<"Scheduling of ASAP task"<<name<<"not continuous slots:"<<time2tjp(date)<<"last:"<<time2tjp(lastSlot);
            }
            return false;
        }

        lastSlot = date + slotDuration - 1;
    }
    else
    {
        if (end == 0 ||
            (effort == 0.0 && length == 0.0 && duration == 0.0 && start == 0)) {
            if ( end == 0 ) {
                warningMessage(i18nc("@info/plain", "Cannot schedule: Valid end time is not set"));
            } else {
                warningMessage(i18nc("@info/plain", "Cannot schedule: Estimate is 0"));
            }
            return false;
        }
        if (lastSlot == 0)
        {
            lastSlot = end + 1;
            tentativeStart = date;
            if (DEBUGTS(5))
                qDebug()<<"Scheduling of ALAP task"<<name<<"starts at"<<time2tjp(lastSlot)<<"("<<time2tjp(date)<<")";
        }
        /* Do not schedule anything if the current time slot is not
         * directly preceding the previously scheduled time slot. */
        if (!((date + slotDuration <= lastSlot) &&
            (lastSlot < date + 2 * slotDuration)))
            return false;
        lastSlot = date;
    }

    if (DEBUGTS(10))
        qDebug()<<"Scheduling"<<name<<"at"<<time2tjp(date);

    if ((duration > 0.0) || (length > 0.0))
    {
//         TJMH.debugMessage(QString("Scheduling duration/length at: %1").arg(time2tjp(date)), this);
        /* Length specifies the number of working days (as daily load)
         * and duration specifies the number of calendar days. */
        if (!allocations.isEmpty())
            bookResources(sc, date, slotDuration);

        doneDuration += ((double) slotDuration) / ONEDAY;
        if (project->isWorkingTime(Interval(date, date + slotDuration - 1)))
            doneLength += project->convertToDailyLoad(slotDuration);

        if (DEBUGTS(10))
            qDebug("Length: %f/%f   Duration: %f/%f",
                   doneLength, length,
                   doneDuration, duration);
        // Check whether we are done with this task.
        /* The accumulated done* values contain rounding errors. This prevents
         * exact float comparisons. To avoid rounding problems we compare the
         * rounded values of the done* values multiplied by 2048. This should
         * result in worst case errors of smaller than a minute. The value
         * 2048 was chosen in the hope that a compiler is clever enough to
         * avoid a costly multiplication if possible. */
        if ((length > 0.0 &&
             qRound(doneLength * 2048) >= qRound(length * 2048)) ||
            (duration > 0.0 &&
             qRound(doneDuration * 2048) >= qRound(duration * 2048)))
        {
            if (scheduling == ASAP)
                propagateEnd(sc, date + slotDuration - 1);
            else
                propagateStart(sc, date);
            schedulingDone = true;
            if (DEBUGTS(4)) {
                qDebug()<<"Scheduling of task"<<name<<"completed";
                if (length > 0.0) qDebug()<<"Length estimate:"<<length<<"done:"<<doneLength;
                if (duration > 0.0) qDebug()<<"Duration estimate:"<<duration<<"done:"<<doneDuration;
            }
            if (length > 0.0) {
                TJMH.debugMessage(QString("Task scheduled: %1 - %2, estimated length: %3").arg(time2ISO(start)).arg(time2ISO(end)).arg(length), this);
            } else if (duration > 0.0) {
                TJMH.debugMessage(QString("Task scheduled: %1 - %2, estimated duration: %3").arg(time2ISO(start)).arg(time2ISO(end)).arg(duration), this);
            }
            return true;
        }
    }
    else if (effort > 0.0)
    {
//         TJMH.debugMessage(QString("Scheduling effort %2 at: %1").arg(time2tjp(date)).arg(effort), this);
        /* The effort of the task has been specified. We have to look
         * how much the resources can contribute over the following
         * workings days until we have reached the specified
         * effort. */
        bookResources(sc, date, slotDuration);
        // Check whether we are done with this task.
        if (qRound(doneEffort * 2048) >= qRound(effort * 2048))
        {
            if (scheduling == ASAP)
                propagateEnd(sc, tentativeEnd);
            else
                propagateStart(sc, tentativeStart);
            schedulingDone = true;
            if (DEBUGTS(4)) {
                qDebug()<<"Scheduling of task"<<this<<"completed:"<<time2ISO(start)<<"-"<<time2ISO(end);
                qDebug()<<"Effort estimate:"<<effort<<"done:"<<doneEffort;
            }
            TJMH.debugMessage(QString("Task scheduled: %3 - %4, estimated effort=%1d, booked=%2d").arg(effort).arg(doneEffort).arg(time2ISO(start)).arg(time2ISO(end)), this);
            return true;
        }
    }
    else if (milestone)
    {
        // Task is a milestone.
        if (scheduling == ASAP)
            propagateEnd(sc, start - 1);
        else
            propagateStart(sc, end + 1);

        if (DEBUGTS(4)) {
            qDebug()<<"Scheduling of task"<<this<<"completed:"<<time2ISO(start)<<"-"<<time2ISO(end);
        }
        TJMH.debugMessage(QString("Milestone scheduled: %1").arg(time2ISO(start)), this );
        return true;
    }
    else if (start != 0 && end != 0)
    {
        // Task with start and end date but no duration criteria.
        if (!allocations.isEmpty() && !project->isVacation(date))
            bookResources(sc, date, slotDuration);

        if ((scheduling == ASAP && (date + slotDuration) >= end) ||
            (scheduling == ALAP && date <= start))
        {
            schedulingDone = true;
            if (DEBUGTS(4))
                qDebug()<<"Scheduling of task"<<name<<"completed";
            TJMH.debugMessage(QString("Task scheduled: %1 - %2").arg(time2ISO(start)).arg(time2ISO(end)), this);
            return true;
        }
    }

    return false;
}

bool
Task::scheduleContainer(int sc)
{
    if (schedulingDone || !isContainer())
        return true;

    time_t nStart = 0;
    time_t nEnd = 0;

    for (TaskListIterator tli(*sub); tli.hasNext();)
    {
        Task *t = static_cast<Task*>(tli.next());
        /* Make sure that all sub tasks have been scheduled. If not we
         * can't yet schedule this task. */
        if (t->start == 0 || t->end == 0)
            return true;

        if (nStart == 0 || t->start < nStart)
            nStart = t->start;
        if (t->end > nEnd)
            nEnd = t->end;
    }

    if (start == 0 || start > nStart)
        propagateStart(sc, nStart);

    if (end == 0 || end < nEnd)
        propagateEnd(sc, nEnd);

    if (DEBUGTS(4))
        qDebug()<<QString("Scheduling of task %1 completed").arg(name);
    schedulingDone = true;

    return false;
}

bool
Task::hasAlapPredecessor() const
{
    foreach ( const CoreAttributes *t, predecessors ) {
        if ( static_cast<const Task*>( t )->getScheduling() == TJ::Task::ALAP || static_cast<const Task*>( t )->hasAlapPredecessor() ) {
            return true;
        }
    }
    return false;
}

void
Task::propagateStart(int sc, time_t date)
{
    start = date;

    if (DEBUGTS(11))
        qDebug()<<"PS1: Setting start of"<<this<<"to"<<time2tjp(start);

//     TJMH.debugMessage(QString("Set start: %2 ").arg(time2ISO(start)), this);

    /* If one end of a milestone is fixed, then the other end can be set as
     * well. */
    if (milestone && date > 0)
    {
        if (!schedulingDone) {
            schedulingDone = true;
            propagateEnd(sc, start - 1);
            if (DEBUGTS(4)) {
                qDebug()<<"Scheduling:"<<this<<"completed:"<<time2ISO(start)<<"-"<<time2ISO(end);
            }

        }
        // schedule successor ASAP milestones
        for (TaskListIterator tli(followers); tli.hasNext();) {
            Task *t = static_cast<Task*>(tli.next());
            if (t->milestone && !t->schedulingDone && t->scheduling == ASAP &&
                t->start == 0 && t->latestEnd(sc) != 0)
            {
                /* Recursively propagate the start date */
                t->propagateEnd(sc, t->latestEnd(sc));
            }
        }
    }
    /* Set start date to all previous that have no start date yet, but are
     * ALAP task or have no duration. */
    for (TaskListIterator tli(previous); *tli != 0; ++tli) {
        if ((*tli)->end == 0 && (*tli)->latestEnd(sc) != 0 &&
            !(*tli)->schedulingDone &&
            ((*tli)->scheduling == ALAP ||
             ((*tli)->effort == 0.0 && (*tli)->length == 0.0 &&
              (*tli)->duration == 0.0 && !(*tli)->milestone)))
        {
            /* Recursively propagate the end date */
            (*tli)->propagateEnd(sc, (*tli)->latestEnd(sc));
        }
    }
    /* Propagate start time to sub tasks which have only an implicit
     * dependancy on the parent task. Do not touch container tasks. */
    for (TaskListIterator tli(*sub); tli.hasNext();)
    {
        Task *t = static_cast<Task*>(tli.next());
        if (!t->hasStartDependency() && !t->schedulingDone)
        {
            /* Recursively propagate the start date */
            t->propagateStart(sc, start);
        }
    }

    if (parent)
    {
        if (DEBUGTS(11))
            qDebug()<<"Scheduling parent of"<<name;
        getParent()->scheduleContainer(sc);
    }
}

void
Task::propagateEnd(int sc, time_t date)
{
    end = date;

    if (DEBUGTS(11))
        qDebug()<<"PE1: Setting end of"<<name<<"to"<<time2tjp(end);

//     TJMH.debugMessage(QString("Set end: %2 ").arg(time2ISO(end)), this);
    /* If one end of a milestone is fixed, then the other end can be set as
     * well. */
    if (milestone && date > 0)
    {
        if (!schedulingDone) {
            schedulingDone = true;
            propagateStart(sc, end + 1);
            if (DEBUGTS(4)) {
                qDebug()<<"Scheduling:"<<this<<"completed:"<<time2ISO(start)<<"-"<<time2ISO(end);
            }
        }
        // schedule predecessor ALAP milestones
        for (TaskListIterator tli(previous); tli.hasNext();) {
            Task *t = static_cast<Task*>(tli.next());
            if (t->milestone && !t->schedulingDone && t->scheduling == ALAP &&
                t->start == 0 && t->earliestStart(sc) != 0)
            {
                /* Recursively propagate the start date */
                t->propagateEnd(sc, t->latestEnd(sc));
            }
        }
    }

    /* Set start date to all followers that have no start date yet, but are
     * ASAP task or have no duration. */
    for (TaskListIterator tli(followers); tli.hasNext();) {
        Task *t = static_cast<Task*>(tli.next());
        if (t->start == 0 && t->earliestStart(sc) != 0 &&
            !t->schedulingDone &&
            (t->scheduling == ASAP ||
             (t->effort == 0.0 && t->length == 0.0 &&
              t->duration == 0.0 && !t->milestone)))
        {
            /* Recursively propagate the start date */
            t->propagateStart(sc, t->earliestStart(sc));
        }
    }
    /* Propagate end time to sub tasks which have only an implicit
     * dependancy on the parent task. Do not touch container tasks. */
    for (TaskListIterator tli(*sub); tli.hasNext();) {
        Task *t = static_cast<Task*>(tli.next());
        if (!t->hasEndDependency() && !t->schedulingDone)
        {
            /* Recursively propagate the end date */
            t->propagateEnd(sc, end);
        }
    }
    if (parent)
    {
        if (DEBUGTS(11))
            qDebug()<<"Scheduling parent of"<<name;
        getParent()->scheduleContainer(sc);
    }
}

void
Task::propagateInitialValues(int sc)
{
    if (start != 0)
        propagateStart(sc, start);
    if (end != 0)
        propagateEnd(sc, end);

    // Check if the some data of sub tasks can already be propagated.
    if (!sub->isEmpty())
        scheduleContainer(sc);
}

void
Task::setRunaway()
{
    schedulingDone = true;
    runAway = true;
}

bool
Task::isRunaway() const
{
    /* If a container task has runaway sub tasts, it is very likely that they
     * are the culprits. So we don't report such a container task as runaway.
     */
    for (TaskListIterator tli(*sub); tli.hasNext();) {
        Task *t = static_cast<Task*>(tli.next());
        if (t->isRunaway())
            return false;
    }
    return runAway;
}

int
Task::isAvailable( Allocation *allocation, Resource *resource, time_t slot ) const
{
    int max = resource->isAvailable(slot);
    if (allocation->hasRequiredResources(resource)) {
        foreach (Resource *r, allocation->getRequiredResources(resource)) {
            int a = r->isAvailable(slot);
            if ( a > max ) {
//                 TJMH.debugMessage(QString("Required resource '%1' is not available at %2").arg(r->getName()).arg(time2ISO(slot)), this);
                max = a;
            }
        }
    }
    return max;
}

void
Task::bookResources(int sc, time_t date, time_t slotDuration)
{
    /* If the time slot overlaps with a specified shift interval, the
     * time slot must also be within the specified working hours of that
     * shift interval. */
    if (!shifts.isOnShift(Interval(date, date + slotDuration - 1)))
    {
        if (DEBUGRS(15))
            qDebug()<<"Task"<<name<<"is not active at"<<time2tjp(date);
        
//         TJMH.debugMessage(QString("Task is not active at %1").arg(time2tjp(date)), this);
        return;
    }

    /* In projection mode we do not allow bookings prior to the current date
     * for any task (in strict mode) or tasks which have user specified
     * bookings (sloppy mode). */
    if (project->getScenario(sc)->getProjectionMode() &&
        date < project->getNow() &&
        (project->getScenario(sc)->getStrictBookings() ||
         !scenarios[sc].specifiedBookedResources.isEmpty()))
    {
        if (DEBUGRS(15))
            qDebug()<<"No allocations prior to current date for task"<<id;

        TJMH.debugMessage(QString("Allocations prior to 'now' %1 is not allowed").arg(time2tjp(project->getNow())), this);
        return;
    }

    /* If any of the resources is marked as being mandatory, we have to check
     * if this resource is available. In case it's not available we do not
     * allocate any of the other resources for the time slot. */
    bool allMandatoriesAvailables = true;
    QList<Resource*> mandatoryResources;
    for (QListIterator<Allocation*> ali(allocations); ali.hasNext();) {
        Allocation *a = static_cast<Allocation*>(ali.next());
        if (a->isMandatory())
        {
            if (!a->isOnShift(Interval(date, date + slotDuration - 1)))
            {
//                 TJMH.debugMessage(QString("Mandatory allocation not on shift at: %1").arg(time2tjp(date)), this);
                allMandatoriesAvailables = false;
                break;
            }
            if (a->isPersistent() && a->getLockedResource())
            {
                int availability = isAvailable(a, a->getLockedResource(), date);
                if (availability > 0)
                {
                    allMandatoriesAvailables = false;
                    if (availability >= 4 && !a->getConflictStart())
                        a->setConflictStart(date);
                    break;
                }
            }
            else
            {
                /* For a mandatory allocation with alternatives at least one
                 * of the resources or resource groups must be available. */
                bool found = false;
                int maxAvailability = 0;
                QList<Resource*> candidates = a->getCandidates();
                foreach (Resource *r, candidates)
                {
                    /* If a resource group is marked mandatory, all members
                     * of the group must be available. */
                    int availability = 0;
                    bool allAvailable = true;
                    for (ResourceTreeIterator rti(r); *rti != 0; ++rti) {
                        if ((availability = isAvailable(a, (*rti), date)) > 0 ||
                            mandatoryResources.contains(*rti))
                        {
//                             TJMH.debugMessage(QString("Mandatory resource '%2' not available at: %1").arg(time2tjp(date)).arg((*rti)->getName()), this);
                            allAvailable = false;
                            if (availability >= maxAvailability)
                                maxAvailability = availability;
                        }
                        else {
                            mandatoryResources.append(*rti);
//                             TJMH.debugMessage(QString("Mandatory resource '%2' available at: %1").arg(time2tjp(date)).arg((*rti)->getName()));
                        }
                    }
                    if (allAvailable)
                        found = true;
                }
                if (!found)
                {
                    if (maxAvailability >= 4 && !a->getConflictStart())
                        a->setConflictStart(date);
                    allMandatoriesAvailables = false;
                    break;
                }
            }
        }
    }
    if ( ! allMandatoriesAvailables ) {
//         TJMH.debugMessage(QString("All mandatory resourcea are not available"), this);
    }
    for (QListIterator<Allocation*> ali(allocations);
         ali.hasNext() && allMandatoriesAvailables &&
         (effort == 0.0 || doneEffort < effort);)
    {
        /* If a shift has been defined for a resource for this task, there
         * must be a shift interval defined for this day and the time must
         * be within the working hours of that shift. */
        Allocation *a = static_cast<Allocation*>(ali.next());
        if (!a->isOnShift(Interval(date, date + slotDuration - 1)))
        {
            if (DEBUGRS(15))
                qDebug()<<"Allocation not on shift at"<<time2tjp(date);

//             TJMH.debugMessage(QString("Allocation not on shift at: %1").arg(time2tjp(date)), this);
            continue;
        }

        /* Now check the limits set for this allocation. */
        const UsageLimits* limits = a->getLimits();
        /* This variable holds the number of slots that are still available to
         * hit the nearest limit. -1 means unlimited slots. */
        int slotsToLimit = -1;
        if (limits)
        {
            QList<Resource*> resources = a->getCandidates();
            QString resStr = "";
            foreach (Resource *r, resources) {
                resStr += r->getId() + " ";
            }
            if (limits->getDailyUnits() > 0) {
                uint bookedSlots = 0;
                int workSlots = 0;
                foreach (Resource *r, resources) {
                    workSlots += r->getWorkSlots(date); // returns 0 if no bookings yet
                    bookedSlots += r->getCurrentDaySlots(date, this); // booked to this task
                }
                if ( workSlots > 0 ) {
                    workSlots = (workSlots * limits->getDailyUnits()) / 100;
                    if (workSlots == 0) {
                        workSlots = 1;
                    }
                }
                int freeSlots = bookedSlots > 0 ?  workSlots - bookedSlots : 1; // always allow one booking
                if (freeSlots <= 0) {
                    if (DEBUGRS(6)) {
                        qDebug()<<"  Resource(s)"<<resStr<<"overloaded";
                    }
                    continue;
                } else if (slotsToLimit < 0 || slotsToLimit > freeSlots) {
                    slotsToLimit = freeSlots;
                }
            }
            else if (limits->getDailyMax() > 0)
            {
                uint slotCount = 0;
                foreach (Resource *r, resources)
                    slotCount += r->getCurrentDaySlots(date, this);
                int freeSlots = limits->getDailyMax() - slotCount;
                if (freeSlots <= 0)
                {
                    if (DEBUGRS(6))
                        qDebug()<<"  Resource(s)"<<resStr<<"overloaded";
                    continue;
                }
                else if (slotsToLimit < 0 || slotsToLimit > freeSlots)
                    slotsToLimit = freeSlots;
            }
            if (limits->getWeeklyMax() > 0)
            {
                uint slotCount = 0;
                foreach (Resource *r, resources)
                    slotCount += r->getCurrentWeekSlots(date, this);
                int freeSlots = limits->getWeeklyMax() - slotCount;
                if (freeSlots <= 0)
                {
                    if (DEBUGRS(6))
                        qDebug()<<"  Resource(s)"<<resStr<<"overloaded";
                    continue;
                }
                else if (slotsToLimit < 0 || slotsToLimit > freeSlots)
                    slotsToLimit = freeSlots;
            }
            if (limits->getMonthlyMax() > 0)
            {
                uint slotCount = 0;
                foreach (Resource *r, resources)
                    slotCount += r->getCurrentMonthSlots(date, this);
                int freeSlots = limits->getMonthlyMax() - slotCount;
                if (freeSlots <= 0)
                {
                    if (DEBUGRS(6))
                        qDebug()<<"  Resource(s)"<<resStr<<"overloaded";
                    continue;
                }
                else if (slotsToLimit < 0 || slotsToLimit > freeSlots)
                    slotsToLimit = freeSlots;
            }
        }

        /* If the allocation has be marked persistent and a resource
         * has already been picked, try to book this resource again. If the
         * resource is not available there will be no booking for this
         * time slot. */
        int maxAvailability = 0;
        if (a->isPersistent() && a->getLockedResource())
        {
            if (!bookResource(a, a->getLockedResource(), date, slotDuration,
                              slotsToLimit, maxAvailability))
            {
                if (maxAvailability >= 4 && !a->getConflictStart())
                    a->setConflictStart(date);
            }
            else if (a->getConflictStart())
            {
                if (DEBUGRS(2))
                    qDebug()<<"Resource"<<a->getLockedResource()->getId()<<"is not available for task '"<<id<<"'"
                        <<"from"<<time2ISO(a->getConflictStart())<<"to"<<time2ISO(date);

//                 TJMH.debugMessage(QString("Resource %1 is not available from %2 to %3").arg(a->getLockedResource()->getName()).arg(time2ISO(a->getConflictStart())).arg(time2ISO(date)), this);
                a->setConflictStart(0);
            }
        }
        else
        {
            QList<Resource*> cl = createCandidateList(sc, date, a);

            bool found = false;
            foreach (Resource *r, cl)
                if (bookResource(a, r, date, slotDuration, slotsToLimit,
                                 maxAvailability))
                {
                    a->setLockedResource(r);;
                    found = true;
                    break;
                }
            if (!found && maxAvailability >= 4 && !a->getConflictStart())
                a->setConflictStart(date);
            else if (found && a->getConflictStart())
            {
                if (DEBUGRS(2))
                {
                    QString candidates;
                    bool first = true;
                    foreach (Resource *r, cl)
                    {
                        if (first)
                            first = false;
                        else
                            candidates += ", ";
                        candidates += r->getId();
                    }
                    qDebug()<<"No resource of the allocation ("<<candidates<<") is available for task '"<<name<<"' from"<<time2ISO(a->getConflictStart())<<"to"<<time2ISO(date);
                }
//                 TJMH.debugMessage(QString("No resource is available for task from %2 to %3").arg(time2ISO(a->getConflictStart())).arg(time2ISO(date)), this);

                a->setConflictStart(0);
            }
        }
    }
}

bool
Task::bookResource(Allocation *allocation, Resource* r, time_t date, time_t slotDuration,
                   int& slotsToLimit, int& maxAvailability)
{
    bool booked = false;
    double intervalLoad = project->convertToDailyLoad(slotDuration);

    for (ResourceTreeIterator rti(r); *rti != 0; ++rti)
    {
        int availability = isAvailable(allocation, (*rti), date);
        if (availability == 0)
        {
            if (!(*rti)->book(new Booking(Interval(date, date + slotDuration - 1), this))) {
                warningMessage(i18nc("@info/plain 1=resource name 2=datetime", "Failed to book resource: '%1' at %2", (*rti)->getName(), formatTime(date)));
                if (DEBUGTS(2)) {
                    qWarning()<<" Failed to book resource"<<(*rti)->getName()<<"at"<<time2ISO(date);
                }
                continue;
            }
            addBookedResource(*rti);
//             TJMH.debugMessage(QString("Booked resource: '%1' at %2").arg((*rti)->getName()).arg(time2ISO(date)), this);
            if (DEBUGTS(20)) {
                qDebug()<<" Booked resource"<<(*rti)->getName()<<"at"<<time2ISO(date);
            }
            if (allocation->hasRequiredResources(*rti)) {
                foreach(Resource *r, allocation->getRequiredResources(*rti)) {
                    if (r->book(new Booking(Interval(date, date + slotDuration - 1), this))) {
                        addBookedResource(r);
//                         TJMH.debugMessage(QString("Booked required resource: '%1' at %2").arg(r->getName()).arg(time2ISO(date)), this);
                        if (DEBUGTS(20)) {
                            qDebug()<<" Booked required resource"<<r->getName()<<"at"<<time2ISO(date);
                        }
                    } else {
                        warningMessage(i18nc("@info/plain 1=resource name 2=datetime", "Failed to book required resource: '%1' at %2", r->getName(), formatTime(date)));
                        if (DEBUGTS(2)) {
                            qWarning()<<" Failed to book required resource"<<r->getName()<<"at"<<time2ISO(date);
                        }
                        // NOTE: just go on as booking the main resource worked ok
                    }
                }
            }

            /* Move the start date to make sure that there is
             * some work going on at the start date. */
            if (!workStarted)
            {
                if (scheduling == ASAP)
                    start = date;
                else if (scheduling == ALAP)
                    end = date + slotDuration - 1;
                else
                    qFatal("Unknown scheduling mode");
                workStarted = true;
            }

            tentativeStart = date;
            tentativeEnd = date + slotDuration - 1;
            doneEffort += intervalLoad * (*rti)->getEfficiency();

            booked = true;

            if (slotsToLimit > 0 && --slotsToLimit <= 0)
                return true;
        }
        else if (availability > maxAvailability)
            maxAvailability = availability;
    }
    return booked;
}

QList<Resource*>
Task::createCandidateList(int sc, time_t date, Allocation* a)
{
    /* This function generates a list of resources that could be allocated to
     * the task. The order of the list is determined by the specified
     * selection function of the alternatives list. From this list, the
     * first available resource is picked later on. */
    QList<Resource*> candidates = a->getCandidates();
    QList<Resource*> cl;

    /* We try to minimize resource changes for consecutives time slots. So
     * the resource used for the previous time slot is put to the 1st position
     * of the list. */
    if (a->getLockedResource())
    {
        cl.append(a->getLockedResource());
        candidates.removeAll(a->getLockedResource());
        /* When an allocation is booked the resource is saved as locked
         * resource. */
        a->setLockedResource(0);
    }
    switch (a->getSelectionMode())
    {
        case Allocation::order:
            if (DEBUGTS(25))
                qDebug("order");
            while (!candidates.isEmpty())
            {
                cl.append(candidates.takeFirst());
            }
            break;
        case Allocation::minAllocationProbability:
        {
            if (DEBUGTS(25))
                qDebug("minAllocationProbability");
            /* This is another heuristic to optimize scheduling results. The
             * idea is to pick the resource that is most likely to be used
             * least during this project (because of the specified
             * allocations) and try to use it first. Unfortunately this
             * algorithm can make things worse in certain plan setups. */
            while (!candidates.isEmpty())
            {
                /* Find canidate with smallest allocationProbability and
                 * append it to the candidate list. */
                double minProbability = 0;
                Resource* minProbResource = 0;
                foreach (Resource *r, candidates)
                {
                    double probability = r->getAllocationProbability(sc);
                    if (minProbability == 0 || probability < minProbability)
                    {
                        minProbability = probability;
                        minProbResource = r;
                    }
                }
                cl.append(minProbResource);
                candidates.removeAt(candidates.indexOf(minProbResource));
            }
            break;
        }
        case Allocation::minLoaded:
        {
            if (DEBUGTS(25))
                qDebug("minLoad");
            while (!candidates.isEmpty())
            {
                double minLoad = 0;
                Resource* minLoaded = 0;
                foreach (Resource *r, candidates)
                {
                    /* We calculate the load as a relative value to the daily
                     * max load. This way part time people will reach their
                     * max as slowly as the full timers. */
                    double load =
                        r->getCurrentLoad(Interval(project->getStart(),
                                                        date), 0) /
                        ((r->getLimits() &&
                          r->getLimits()->getDailyMax() > 0) ?
                         project->convertToDailyLoad
                         (r->getLimits()->getDailyMax() *
                          project->getScheduleGranularity()) : 1.0);

                    if (minLoaded == 0 || load < minLoad)
                    {
                        minLoad = load;
                        minLoaded = r;
                    }
                }
                cl.append(minLoaded);
                candidates.removeAt(candidates.indexOf(minLoaded));
            }
            break;
        }
        case Allocation::maxLoaded:
        {
            if (DEBUGTS(25))
                qDebug("maxLoad");
            while (!candidates.isEmpty())
            {
                double maxLoad = 0;
                Resource* maxLoaded = 0;
                foreach (Resource *r, candidates)
                {
                    /* We calculate the load as a relative value to the daily
                     * max load. This way part time people will reach their
                     * max as fast as the full timers. */
                    double load =
                        r->getCurrentLoad(Interval(project->getStart(),
                                                        date), 0) /
                        ((r->getLimits() &&
                          r->getLimits()->getDailyMax() > 0) ?
                         project->convertToDailyLoad
                         (r->getLimits()->getDailyMax() *
                          project->getScheduleGranularity()) : 1.0);

                    if (maxLoaded == 0 || load > maxLoad)
                    {
                        maxLoad = load;
                        maxLoaded = r;
                    }
                }
                cl.append(maxLoaded);
                candidates.removeAt(candidates.indexOf(maxLoaded));
            }
            break;
        }
        case Allocation::random:
        {
            if (DEBUGTS(25))
                qDebug("random");
            while (!candidates.isEmpty())
            {
                cl.append(candidates.at(rand() % candidates.count()));
                candidates.takeFirst(); //### do not understand this
            }
            break;
        }
        default:
            qFatal("Illegal selection mode %d", a->getSelectionMode());
    }

    return cl;
}

QString
Task::getSchedulingText() const
{
    if (isLeaf())
    {
        return scheduling == ASAP ? "ASAP |-->|" : "ALAP |<--|";
    }
    else
    {
        QString text;

        for (TaskListIterator tli(*sub); tli.hasNext();)
        {
            Task *t = static_cast<Task*>(tli.next());
            if (text.isEmpty())
                text = t->getSchedulingText();
            else if (text != t->getSchedulingText())
            {
                text = "Mixed";
                break;
            }
        }
        return text;
    }
    return QString();
}

QString
Task::getStatusText(int sc) const
{
    QString text;
    switch (getStatus(sc))
    {
        case NotStarted:
            text = QString("Not yet started");
            break;
        case InProgressLate:
            text = QString("Behind schedule");
            break;
        case InProgress:
            text = QString("Work in progress");
            break;
        case OnTime:
            text = QString("On schedule");
            break;
        case InProgressEarly:
            text = QString("Ahead of schedule");
            break;
        case Finished:
            text = QString("Finished");
            break;
        case Late:
            text = QString("Late");
            break;
        default:
            text = QString("Unknown status");
            break;
    }
    return text;
}

bool
Task::isCompleted(int sc, time_t date) const
{
    if (scenarios[sc].reportedCompletion >= 0.0)
    {
        if (scenarios[sc].reportedCompletion >= 100.0)
            return true;

        // some completion degree has been specified.
        if (scenarios[sc].effort > 0.0)
        {
            return qRound((scenarios[sc].effort *
                           (scenarios[sc].reportedCompletion / 100.0)) * 1000)
                >= qRound(getLoad(sc, Interval(scenarios[sc].start, date), 0)
                         * 1000);
        }
        else
        {
            return (date <=
                    scenarios[sc].start +
                    static_cast<int>((scenarios[sc].reportedCompletion /
                                      100.0) * (scenarios[sc].end -
                                                scenarios[sc].start)));
        }
    }

    if (isContainer())
    {
        return (date <=
                scenarios[sc].start +
                static_cast<int>((scenarios[sc].containerCompletion /
                                  100.0) * (scenarios[sc].end -
                                            scenarios[sc].start)));
    }

    return (project->getNow() > date);
}

bool
Task::isBuffer(int sc, const Interval& iv) const
{
    return iv.overlaps(Interval(scenarios[sc].start,
                                scenarios[sc].startBufferEnd)) ||
        iv.overlaps(Interval(scenarios[sc].endBufferStart,
                             scenarios[sc].end));
}

time_t
Task::earliestStart(int sc) const
{
    time_t date = 0;
    // All tasks this task depends on must have an end date set.
    for (TaskListIterator tli(previous); tli.hasNext();) {
        Task *t = static_cast<Task*>(tli.next());
        if (t->end == 0)
        {
            if (t->scheduling == ASAP) {
                if (DEBUGTS(1)) {
                    qDebug()<<"Earliest start:"<<this<<":"<<t<<"end == 0";
                }
                return 0;
            }
        }
        else if (t->end + 1 > date)
            date = t->end + 1;
    }
    for (QListIterator<TaskDependency*> tdi(depends); tdi.hasNext();)
    {
        /* Add the gapDuration and/or gapLength to the end of the dependent
         * task. */
        TaskDependency *td = static_cast<TaskDependency*>(tdi.next());
        time_t potentialDate = td->getTaskRef()->end + 1;
        time_t dateAfterLengthGap;
        long gapLength = td->getGapLength(sc);
        for (dateAfterLengthGap = potentialDate;
             gapLength > 0 && dateAfterLengthGap < project->getEnd();
             dateAfterLengthGap += project->getScheduleGranularity())
            if (project->isWorkingTime(dateAfterLengthGap))
                gapLength -= project->getScheduleGranularity();
        if (dateAfterLengthGap > potentialDate + td->getGapDuration(sc))
            potentialDate = dateAfterLengthGap;
        else
            potentialDate += td->getGapDuration(sc);
        // Set 'date' to the latest end date plus gaps of all preceding tasks.
        if (potentialDate > date)
            date = potentialDate;
    }
    /* If any of the parent tasks has an explicit start date, the task must
     * start at or after this date. */
    for (Task* t = getParent(); t; t = t->getParent())
        if (t->start > date)
            return t->start;

    if (DEBUGTS(15)) {
        qDebug()<<"Earliest start:"<<this<<time2ISO(date);
    }

    return date;
}

time_t
Task::latestEnd(int sc) const
{
    time_t date = 0;
    // All tasks this task precedes must have a start date set.
    for (TaskListIterator tli(followers); tli.hasNext();) {
        Task *t = static_cast<Task*>(tli.next());
        if (t->start == 0)
        {
            if (t->scheduling == ALAP) {
                if (DEBUGTS(1)) {
                    qDebug()<<"Latest end:"<<this<<":"<<t<<"start == 0";
                }
                return 0;
            }
        }
        else if (date == 0 || t->start - 1 < date) {
            date = t->start - 1;
            if (DEBUGTS(15)) {
                qDebug()<<"Latest end:"<<this<<time2ISO(date)<<"from follower:"<<t<<time2ISO(t->start-1);
            }
        }
    }
    for (QListIterator<TaskDependency*> tdi(precedes); tdi.hasNext();)
    {
        /* Subtract the gapDuration and/or gapLength from the start of the
         * following task. */
        TaskDependency *td = static_cast<TaskDependency*>(tdi.next());
        time_t potentialDate = td->getTaskRef()->start - 1;
        time_t dateBeforeLengthGap;
        long gapLength = td->getGapLength(sc);
        for (dateBeforeLengthGap = potentialDate;
             gapLength > 0 && dateBeforeLengthGap >= project->getStart();
             dateBeforeLengthGap -= project->getScheduleGranularity())
            if (project->isWorkingTime(dateBeforeLengthGap))
                gapLength -= project->getScheduleGranularity();
        if (dateBeforeLengthGap < potentialDate - td->getGapDuration(sc))
            potentialDate = dateBeforeLengthGap;
        else
            potentialDate -= td->getGapDuration(sc);

        /* Set 'date' to the earliest end date minus gaps of all following
         * tasks. */
        if (date == 0 || potentialDate < date)
            date = potentialDate;
        if (DEBUGTS(15)) {
            qDebug()<<"Latest end:"<<this<<time2ISO(date)<<"from successor:"<<td->getTaskRef()<<time2ISO(td->getTaskRef()->start);
        }
    }
    /* If any of the parent tasks has an explicit end date, the task must
     * end at or before this date. */
    for (Task* t = getParent(); t; t = t->getParent())
        if (t->end != 0 && t->end < date)
            return t->end;

    if (DEBUGTS(15)) {
        qDebug()<<"Latest end:"<<this<<time2ISO(date);
    }
    return date;
}

double
Task::getCalcEffort(int sc) const
{
    if (milestone)
        return 0.0;

    return getLoad(sc, Interval(scenarios[sc].start, scenarios[sc].end));
}

double
Task::getCalcDuration(int sc) const
{
    if (milestone)
        return 0.0;

    return static_cast<double>(scenarios[sc].end + 1 - scenarios[sc].start) / ONEDAY;
}

double
Task::getLoad(int sc, const Interval& period, const Resource* resource) const
{
    if (milestone)
        return 0.0;

    double load = 0.0;

    if (isContainer())
    {
        for (TaskListIterator tli(*sub); tli.hasNext();) {
            Task *t = static_cast<Task*>(tli.next());
            load += t->getLoad(sc, period, resource);
        }
    }
    else
    {
        if (resource)
            load += resource->getEffectiveLoad(sc, period, AllAccounts, this);
        else
            for (ResourceListIterator rli(scenarios[sc].bookedResources);
                 rli.hasNext();)
                load += static_cast<Resource*>(rli.next())->getEffectiveLoad(sc, period, AllAccounts, this);
    }

    return load;
}

double
Task::getAllocatedTimeLoad(int sc, const Interval& period,
                           const Resource* resource) const
{
    return project->convertToDailyLoad
        (getAllocatedTime(sc, period, resource));
}

long
Task::getAllocatedTime(int sc, const Interval& period,
                       const Resource* resource) const
{
    if (milestone)
        return 0;

    long allocatedTime = 0;

    if (isContainer())
    {
        for (TaskListIterator tli(*sub); tli.hasNext();) {
            Task *t = static_cast<Task*>(tli.next());
            allocatedTime += t->getAllocatedTime(sc, period, resource);
        }
    }
    else
    {
        if (resource)
            allocatedTime += resource->getAllocatedTime(sc, period, AllAccounts,
                                                        this);
        else
            for (ResourceListIterator rli(scenarios[sc].bookedResources);
                 *rli;)
                allocatedTime += static_cast<Resource*>(rli.next())->getAllocatedTime(sc, period,
                                                          AllAccounts, this);
    }

    return allocatedTime;
}

// double
// Task::getCredits(int sc, const Interval& period, AccountType acctType,
//                  const Resource* resource, bool recursive) const
// {
//     double credits = 0.0;
// 
//     if (recursive && !sub->isEmpty())
//     {
//         for (TaskListIterator tli(*sub); tli.hasNext();) {
//             Task *t = static_cast<Task*>(tli.next());
//             credits += t->getCredits(sc, period, acctType, resource,
//                                           recursive);
//         }
//     }
// 
//     if (acctType != AllAccounts &&
//         (account == 0 || acctType != account->getAcctType()))
//         return credits;
// 
//     if (resource)
//         credits += resource->getCredits(sc, period, acctType, this);
//     else
//         for (ResourceListIterator rli(scenarios[sc].bookedResources);
//              *rli != 0; ++rli)
//             credits += r->getCredits(sc, period, acctType, this);
// 
//     if (period.contains(scenarios[sc].start))
//         credits += scenarios[sc].startCredit;
//     if (period.contains(scenarios[sc].end + (milestone ? 1 : 0)))
//         credits += scenarios[sc].endCredit;
// 
//     return credits;
// }


bool
Task::xRef(QMap<QString, Task*>& hash)
{
    if (DEBUGPF(5))
        qDebug()<<QString("Creating cross references for task %1 ...").arg(name);
    int errors = 0;

    QList<TaskDependency*> brokenDeps;
    for (QListIterator<TaskDependency*> tdi(depends); tdi.hasNext();)
    {
        TaskDependency *td = static_cast<TaskDependency*>(tdi.next());
        QString absId = resolveId(td->getTaskRefId());
        Task* t;
        if ((t = hash.value(absId)) == 0)
        {
            errorMessage(QString("Unknown dependency '%1'").arg(absId));
            brokenDeps.append(td);
            errors++;
        }
        else
        {
            for (QListIterator<TaskDependency*> tdi2(depends); tdi2.hasNext();) {
                TaskDependency *td2 = static_cast<TaskDependency*>(tdi2.next());
                if (td2->getTaskRef() == t)
                {
                    warningMessage(QString("No need to specify dependency %1 "
                                      "multiple times.").arg(absId));
                    break;
                }
            }
            if (errors == 0)
            {
                td->setTaskRef(t);
                if (t == this)
                {
                    errorMessage(QString("Task '%1' cannot depend on self.")
                                 .arg(name));
                    break;
                }
                if (t->isDescendantOf(this))
                {
                    errorMessage(QString("Task '%1' cannot depend on child.")
                                 .arg(name));
                    break;
                }
                if (isDescendantOf(t))
                {
                    errorMessage(QString/**/("Task '%1' cannot depend on parent.")
                                 .arg(t->id));
                    break;
                }
                if ( ! predecessors.contains( t ) ) {
                    // Unidirectional link
                    predecessors.append(t);
                }
                // Bidirectional link
                if ( ! previous.contains( t ) ) {
                    previous.append(t);
                }
                if ( ! t->followers.contains( this ) ) {
                    t->followers.append(this);
                }
                if (DEBUGPF(11))
                    qDebug()<<"Registering dependency:"<<this<<"precedes"<<t;
            }
        }
    }
    // Remove broken dependencies as they can cause trouble later on.
    for (QListIterator<TaskDependency*> tdi(brokenDeps); tdi.hasNext();) {
        depends.removeAt(depends.indexOf(tdi.next()));
    }
    brokenDeps.clear();

    for (QListIterator<TaskDependency*> tdi(precedes); tdi.hasNext();)
    {
        TaskDependency *td = static_cast<TaskDependency*>(tdi.next());
        QString absId = resolveId(td->getTaskRefId());
        Task* t;
        if ((t = hash.value(absId)) == 0)
        {
            errorMessage(QString("Unknown dependency '%1'").arg(absId));
            brokenDeps.append(td);
        }
        else
        {
            for (QListIterator<TaskDependency*> tdi2(precedes); tdi2.hasNext();) {
                TaskDependency *td = static_cast<TaskDependency*>(tdi2.next());
                if (td->getTaskRef() == t)
                {
                    warningMessage(QString("No need to specify dependency '%1'"
                                        "multiple times").arg(absId));
                    break;
                }
            }
            if (errors == 0)
            {
                td->setTaskRef(t);
                if (t == this)
                {
                    errorMessage(QString("Task '%1' cannot precede self.")
                                 .arg(name));
                    break;
                }
                if (t->isDescendantOf(this))
                {
                    errorMessage(QString("Task '%1' cannot precede a child.")
                                 .arg(name));
                    break;
                }
                if (isDescendantOf(t))
                {
                    errorMessage(QString("Task '%1' cannot precede parent.")
                                 .arg(t->id));
                    break;
                }
                if ( ! successors.contains( t ) ) {
                    // Unidirectional link
                    successors.append(t);
                }
                // Bidirectional link
                if ( ! followers.contains( t ) ) {
                    followers.append(t);
                }
                if ( ! t->previous.contains( this ) ) {
                    t->previous.append(this);
                }
                if (DEBUGPF(11))
                    qDebug()<<"Registering predecessor"<<this<<"with task"<<t;
            }
        }
    }
    // Remove broken dependencies as they can cause trouble later on.
    for (QListIterator<TaskDependency*> tdi(brokenDeps); tdi.hasNext();) {
        TaskDependency *td = static_cast<TaskDependency*>(tdi.next());
        precedes.removeAt(precedes.indexOf(td));
    }
    brokenDeps.clear();

    return errors > 0;
}

void
Task::implicitXRef()
{
    /* Every time the scheduling related information of a single task has been
     * changed, we have to reset the cache flags for the start and end
     * determinability. */
    for (int sc = 0; sc < project->getMaxScenarios(); ++sc)
    {
        scenarios[sc].startCanBeDetermined = false;
        scenarios[sc].endCanBeDetermined = false;
    }

    if (!sub->isEmpty())
        return;

    for (int sc = 0; sc < project->getMaxScenarios(); ++sc)
    {
        /* Propagate implicit dependencies. If a task has no specified start or
         * end date and no start or end dependencies, we check if a parent task
         * has an explicit start or end date which can be used. */

        if (milestone)
        {
            if (scenarios[sc].specifiedStart != 0 &&
                scenarios[sc].specifiedEnd == 0)
                scenarios[sc].specifiedEnd = scenarios[sc].specifiedStart - 1;
            if (scenarios[sc].specifiedEnd != 0 &&
                scenarios[sc].specifiedStart == 0)
                scenarios[sc].specifiedStart = scenarios[sc].specifiedEnd + 1;
        }
        bool hasDurationSpec = scenarios[sc].duration != 0 ||
            scenarios[sc].length != 0 ||
            scenarios[sc].effort != 0;

        if (scenarios[sc].specifiedStart == 0 && depends.isEmpty() &&
            !(hasDurationSpec && scheduling == ALAP))
            for (Task* tp = getParent(); tp; tp = tp->getParent())
            {
                if (tp->scenarios[sc].specifiedStart != 0)
                {
                    if (DEBUGPF(11))
                        qDebug()<<"Setting start of task '"<<id<<"' in scenario"<<project->getScenarioId(sc)<<"to"<<time2ISO(tp->scenarios[sc].specifiedStart);
                    
                    scenarios[sc].specifiedStart = tp->scenarios[sc].specifiedStart;
                    break;
                }
            }
        /* And the same for end values */
        if (scenarios[sc].specifiedEnd == 0 && precedes.isEmpty() &&
            !(hasDurationSpec && scheduling == ASAP))
            for (Task* tp = getParent(); tp; tp = tp->getParent())
            {
                if (tp->scenarios[sc].specifiedEnd != 0)
                {
                    if (DEBUGPF(11))
                        qDebug()<<"Setting end of task '"<<id<<"' in scenario"<<project->getScenarioId(sc)<<"to"<<time2ISO(tp->scenarios[sc].specifiedEnd),

                    scenarios[sc].specifiedEnd = tp->scenarios[sc].specifiedEnd;
                    break;
                }
            }
    }

    if (!isMilestone() && isLeaf())
    {
        /* Automatic milestone marker. As a convenience we convert tasks that
         * only have a start or end criteria as a milestone. This is handy
         * when in the early stage of a project draft, when you just want to
         * specify the project outline and fill in subtasks and details
         * later. */
        bool hasStartSpec = false;
        bool hasEndSpec = false;
        bool hasDurationSpec = false;
        for (int sc = 0; sc < project->getMaxScenarios(); ++sc)
        {
            if (scenarios[sc].specifiedStart != 0 || !depends.isEmpty())
                hasStartSpec = true;
            if (scenarios[sc].specifiedEnd != 0 || !precedes.isEmpty())
                hasEndSpec = true;
            if (scenarios[sc].duration != 0 || scenarios[sc].length != 0 ||
                scenarios[sc].effort != 0)
                hasDurationSpec = true;
//             qDebug()<<"Task::implicitXRef:"<<id<<"effort="<<getEffort( sc )<<"sc="<<sc<<":"<<hasDurationSpec<<(hasStartSpec ^ hasEndSpec);
        }
        if  (!hasDurationSpec && (hasStartSpec ^ hasEndSpec)) {
            milestone = true;
//             qDebug()<<"Task::implicitXRef:"<<"set"<<id<<"to milestone ("<<hasDurationSpec<<(hasStartSpec ^ hasEndSpec)<<")";
        }
    }
}

void
Task::sortAllocations()
{
    if (allocations.isEmpty())
        return;

//     allocations.setAutoDelete(false);
    for (QListIterator<Allocation*> ali(allocations); ali.hasNext(); )
    {
        Allocation *a = static_cast<Allocation*>(ali.next());
        if (!a->isWorker())
        {
            /* If the resource does not do any work we move it to the front of
             * the list. That way the 0 effective resources are always
             * allocated no matter if the effort limit has been reached or not.
             * At least in the same booking call. */
            allocations.removeAt(allocations.indexOf(a));
            allocations.prepend(a);
        }

    }
//     allocations.setAutoDelete(true);
}

void
Task::saveSpecifiedBookedResources()
{
    /* The project file readers use the same resource booking mechanism as the
     * scheduler. So we need to save the up to now booked resources as
     * specified resources. */
    for (int sc = 0; sc < project->getMaxScenarios(); ++sc)
        scenarios[sc].specifiedBookedResources =
            scenarios[sc].bookedResources;
}

bool
Task::loopDetector(LDIList& chkedTaskList) const
{
    /* Only check top-level tasks. All other tasks will be checked then as
     * well. */
    if (parent)
        return false;
    if (DEBUGPF(2))
        qDebug()<<"Running loop detector for task "<<id;
    // Check ASAP tasks
    LDIList list;
    if (loopDetection(list, chkedTaskList, false, true))
        return true;
    // Check ALAP tasks
    if (loopDetection(list, chkedTaskList, true, true))
        return true;
    return false;
}

bool
Task::loopDetection(LDIList& list, LDIList& chkedTaskList, bool atEnd,
                    bool fromOutside) const
{
    if (DEBUGPF(10))
        qDebug()<<QString().fill(' ', list.count() + 1)<<"loopDetection at"<<id<<"("<<(atEnd ? "End" : "Start")<<")";

    // First, check whether the task has already been checked for loops.
    {
        LoopDetectorInfo thisTask(this, atEnd);
        if (chkedTaskList.find(&thisTask))
        {
            // Already checked
            return false;
        }
    }

    if (checkPathForLoops(list, atEnd))
        return true;

    /* Now we have to traverse the graph in the direction of the specified
     * dependencies. 'precedes' and 'depends' specify dependencies in the
     * opposite direction of the flow of the tasks. So we have to make sure
     * that we do not follow the arcs in the direction that precedes and
     * depends points us. Parent/Child relationships also specify a
     * dependency. The scheduling mode of the child determines the direction
     * of the flow. With help of the 'caller' parameter we make sure that we
     * only visit children if we were referred to the task by a non-parent-child
     * relationship. */
    if (!atEnd)
    {
        if (fromOutside)
        {
            /*
                 |
                 v
               +--------
            -->| o--+
               +--- | --
                    |
                    V
            */
            /* If we were not called from a sub task we check all sub tasks.*/
            for (TaskListIterator tli(*sub); tli.hasNext();)
            {
                Task *t = static_cast<Task*>(tli.next());
                if (DEBUGPF(15))
                    qDebug()<<QString().fill(' ', list.count())<<"Checking sub task"<<t->getId()<<"of"<<id;
                if (t->loopDetection(list, chkedTaskList, false, true))
                    return true;
            }

            /*
                 |
                 v
               +--------
            -->| o---->
               +--------
            */
            if (DEBUGPF(15))
                qDebug()<<QString().fill(' ', list.count())<<"Checking end of task"<<id;
            if (loopDetection(list, chkedTaskList, true, false))
                return true;
        }
        else
        {
            /*
                 ^
                 |
               + | -----
               | o <--
               +--------
                 ^
                 |
            */
            if (parent)
            {
                if (DEBUGPF(15))
                    qDebug()<<QString().fill(' ', list.count())<<"Checking parent task of"<<id;
                if (getParent()->loopDetection(list, chkedTaskList, false,
                                               false))
                    return true;
            }

            /*
               +--------
            <--|- o <--
               +--------
                  ^
                  |
             */
            // Now check all previous tasks.
            for (TaskListIterator tli(previous); tli.hasNext();)
            {
                Task *t = static_cast<Task*>(tli.next());
                if (DEBUGPF(15))
                    qDebug()<<QString().fill(' ', list.count())<<"Checking previous"<<t->getId()<<"of task"<<id;
                if(t->loopDetection(list, chkedTaskList, true, true))
                    return true;
            }
        }
    }
    else
    {
        if (fromOutside)
        {
            /*
                  |
                  v
            --------+
               +--o |<--
            -- | ---+
               |
               v
            */
            /* If we were not called from a sub task we check all sub tasks.*/
            for (TaskListIterator tli(*sub); tli.hasNext();)
            {
                Task *t = static_cast<Task*>(tli.next());
                if (DEBUGPF(15))
                    qDebug()<<QString().fill(' ', list.count())<<"Checking sub task"<<t->getId()<<"of"<<id;
                if (t->loopDetection(list, chkedTaskList, true, true))
                    return true;
            }

            /*
                  |
                  v
            --------+
             <----o |<--
            --------+
            */
            if (DEBUGPF(15))
                qDebug()<<QString().fill(' ', list.count())<<"Checking start of task"<<id;
            if (loopDetection(list, chkedTaskList, false, false))
                return true;
        }
        else
        {
            /*
                  ^
                  |
            ----- | +
              --> o |
            --------+
                  ^
                  |
            */
            if (parent)
            {
                if (DEBUGPF(15))
                    qDebug()<<QString().fill(' ', list.count())<<"Checking parent task of"<<id;
                if (getParent()->loopDetection(list, chkedTaskList, true,
                                               false))
                    return true;
            }

            /*
            --------+
              --> o-|-->
            --------+
                  ^
                  |
            */
            // Now check all following tasks.
            for (TaskListIterator tli(followers); tli.hasNext();)
            {
                Task *t = static_cast<Task*>(tli.next());
                if (DEBUGPF(15))
                    qDebug()<<QString().fill(' ', list.count())<<"Checking follower"<<t->getId()<<"of task"<<id;
                if (t->loopDetection(list, chkedTaskList, false, true))
                    return true;
            }
        }
    }
    chkedTaskList.append(list.popLast());

    if (DEBUGPF(5))
        qDebug()<<QString().fill(' ', list.count())<<"No loops found in"<<id<<"("<<(atEnd ? "End" : "Start")<<")";
    return false;
}

bool
Task::checkPathForLoops(LDIList& list, bool atEnd) const
{
    /* If we find the current task (with same position) in the list, we have
     * detected a loop. In case there is no loop detected we add this tasks at
     * the end of the list. */
    LoopDetectorInfo* thisTask = new LoopDetectorInfo(this, atEnd);
    if (list.find(thisTask))
    {
        QString loopChain;
        LoopDetectorInfo* it;
        /* Find the first occurrence of this task in the list. This is the
         * start of the loop. */
        for (it = list.first(); *it != *thisTask; it = it->next())
            ;
        /* Then copy all loop elements to the loopChain string. */
        for ( ; it != 0; it = it->next())
        {
            loopChain += QString("%1 (%2) -> ")
                .arg(it->getTask()->getId())
                .arg(it->getAtEnd() ? "End" : "Start");
        }
        loopChain += QString("%1 (%2)").arg(name)
            .arg(atEnd ? "End" : "Start");
        delete thisTask;
        errorMessage(QString("Dependency loop detected: %1").arg(loopChain));
        return true;
    }
    list.append(thisTask);

    return false;
}

bool
Task::checkDetermination(int sc) const
{
    /* Check if the task and it's dependencies have enough information to
     * produce a fixed determined schedule. */
    if (DEBUGPF(10))
        qDebug()<<"Checking determination of task"<<id;
    LDIList list;

    if (!startCanBeDetermined(list, sc))
    {
        /* The error message must only be shown if the task has prececessors.
         * If not, is has been reported before already. */
        if (!previous.isEmpty())
            errorMessage
                (QString("The start of task '%1' is "
                      "underspecified. This is caused by underspecified "
                      "dependent tasks. You must use more fixed dates to "
                      "solve this problem.")
                 .arg(name));
        return false;
    }

    if (!endCanBeDetermined(list, sc))
    {
        /* The error message must only be shown if the task has followers.
         * If not, is has been reported before already. */
        if (!followers.isEmpty())
            errorMessage
                (QString("The end of task '%1' is underspecified. "
                      "This is caused by underspecified dependent tasks. You "
                      "must use more fixed dates to solve this problem.")
                 .arg(name));
        return false;
    }

    return true;
}

bool
Task::startCanBeDetermined(LDIList& list, int sc) const
{
    if (DEBUGPF(10))
        qDebug()<<"Checking if start of task"<<id<<"can be determined";

    if (scenarios[sc].startCanBeDetermined)
    {
        if (DEBUGPF(10))
            qDebug()<<"Start of task"<<id<<"can be determined (cached)";
        return true;
    }

    if (checkPathForLoops(list, false))
        return false;

    for (const Task* t = this; t; t = static_cast<const Task*>(t->parent))
        if (scenarios[sc].specifiedStart != 0) //FIXME ?? t->scenarios[sc] ??
        {
            if (DEBUGPF(10))
                qDebug()<<"Start of task"<<id<<"can be determined (fixed date)";
            goto isDetermined;
        }

    if (scheduling == ALAP &&
        (scenarios[sc].duration != 0.0 || scenarios[sc].length != 0.0 ||
         scenarios[sc].effort != 0.0 || milestone) &&
        endCanBeDetermined(list, sc))
    {
        if (DEBUGPF(10))
            qDebug()<<"Start of task"<<id<<"can be determined (end + fixed length)";
        goto isDetermined;
    }

    for (TaskListIterator tli(predecessors); tli.hasNext();) {
        Task *t = static_cast<Task*>(tli.next());
        if (t->endCanBeDetermined(list, sc))
        {
            if (DEBUGPF(10))
                qDebug()<<"Start of task"<<id<<"can be determined (dependency)";
            goto isDetermined;
        }
    }
    if (hasSubs())
    {
        for (TaskListIterator tli = getSubListIterator(); tli.hasNext();) {
            Task *t = static_cast<Task*>(tli.next());
            if (!t->startCanBeDetermined(list, sc))
                goto isNotDetermined;
        }
        if (DEBUGPF(10))
            qDebug()<<"Start of task"<<id<<"can be determined (children)";
        goto isDetermined;
    }

isNotDetermined:
    if (DEBUGPF(10))
        qDebug()<<"*** Start of task"<<id<<"cannot be determined";
    list.removeLast();
    return false;

isDetermined:
    list.removeLast();
    scenarios[sc].startCanBeDetermined = true;
    return true;
}

bool
Task::endCanBeDetermined(LDIList& list, int sc) const
{
    if (DEBUGPF(10))
        qDebug()<<"Checking if end of task"<<id<<"can be determined";

    if (scenarios[sc].endCanBeDetermined)
    {
        if (DEBUGPF(10))
            qDebug()<<"End of task"<<id<<"can be determined";
        return true;
    }

    if (checkPathForLoops(list, true))
        return false;

    for (const Task* t = this; t; t = static_cast<const Task*>(t->parent))
        if (scenarios[sc].specifiedEnd != 0) //FIXME ?? t->scenarios ??
        {
            if (DEBUGPF(10))
                qDebug()<<"End of task"<<id<<"can be determined (fixed date)";
            goto isDetermined;
        }

    if (scheduling == ASAP &&
        (scenarios[sc].duration != 0.0 || scenarios[sc].length != 0.0 ||
         scenarios[sc].effort != 0.0 || milestone) &&
        startCanBeDetermined(list, sc))
    {
        if (DEBUGPF(10))
            qDebug()<<"End of task"<<id<<"can be determined (end + fixed length)";
        goto isDetermined;
    }

    for (TaskListIterator tli(successors); tli.hasNext();) {
        Task *t = static_cast<Task*>(tli.next());
        if (t->startCanBeDetermined(list, sc))
        {
            if (DEBUGPF(10))
                qDebug()<<"End of task"<<id<<"can be determined (dependency)";
            goto isDetermined;
        }
    }
    if (hasSubs())
    {
        for (TaskListIterator tli = getSubListIterator(); tli.hasNext();) {
            Task *t = static_cast<Task*>(tli.next());
            if (!t->endCanBeDetermined(list, sc))
            {
                if (DEBUGPF(10))
                    qDebug()<<"End of task"<<id<<"cannot be determined (child"<<t->id<<")";
                goto isNotDetermined;
            }
        }

        if (DEBUGPF(10))
            qDebug()<<"End of task"<<id<<"can be determined (children)";
        goto isDetermined;
    }

isNotDetermined:
    if (DEBUGPF(10))
        qDebug()<<"*** End of task"<<id<<"cannot be determined";
    list.removeLast();
    return false;

isDetermined:
    list.removeLast();
    scenarios[sc].endCanBeDetermined = true;
    return true;
}

QString
Task::resolveId(QString relId)
{
    /* Converts a relative ID to an absolute ID. Relative IDs start
     * with a number of bangs. A set of bangs means 'Name of the n-th
     * parent task' with n being the number of bangs. */
    if (relId[0] != '!')
        return relId;

    Task* t = this;
    unsigned int i;
    for (i = 0; i < relId.length() && relId.mid(i, 1) == "!"; ++i)
    {
        if (t == 0)
        {
            errorMessage(QString("Illegal relative ID '%1'").arg(relId));
            return relId;
        }
        t = t->getParent();
    }
    if (t)
        return t->id + "." + relId.right(relId.length() - i);
    else
        return relId.right(relId.length() - i);
}

bool
Task::hasStartDependency(int sc) const
{
    /* Checks whether the task has a start specification for the
     * scenario. This can be a fixed start time or a dependency on another
     * task's end or an implicit dependency on the fixed start time of a
     * parent task. */
    if (scenarios[sc].specifiedStart != 0 || !depends.isEmpty()) {
//         qDebug()<<"Task::hasStartDependency:"<<id<<sc<<(scenarios[sc].specifiedStart != 0)<<(!depends.isEmpty());
        return true;
    }
    for (Task* p = getParent(); p; p = p->getParent()) {
        if (p->scenarios[sc].specifiedStart != 0) {
//             qDebug()<<"Task::hasStartDependency:"<<id<<sc<<"parent";
            return true;
        }
    }
//     qDebug()<<"Task::hasStartDependency:"<<id<<sc<<"no dependency:"<<(scenarios[sc].specifiedStart != 0)<<(!depends.isEmpty());
    return false;
}

bool
Task::hasEndDependency(int sc) const
{
    /* Checks whether the task has an end specification for the
     * scenario. This can be a fixed end time or a dependency on another
     * task's start or an implicit dependency on the fixed end time of a
     * parent task. */
    if (scenarios[sc].specifiedEnd != 0 || !precedes.isEmpty())
        return true;
    for (Task* p = getParent(); p; p = p->getParent())
        if (p->scenarios[sc].specifiedEnd != 0)
            return true;
    return false;
}

bool
Task::hasStartDependency() const
{
    /* Check whether the task or any of it's sub tasks has a start
     * dependency. */
    if (start != 0 || !previous.isEmpty() || scheduling == ALAP)
        return true;

    for (TaskListIterator tli(*sub); tli.hasNext();) {
        Task *t = static_cast<Task*>(tli.next());
        if (t->hasStartDependency())
            return true;
    }

    return false;
}

bool
Task::hasEndDependency() const
{
    /* Check whether the task or any of it's sub tasks has an end
     * dependency. */
    if (end != 0 || !followers.isEmpty() || scheduling == ASAP)
        return true;

    for (TaskListIterator tli(*sub); tli.hasNext();) {
        Task *t = static_cast<Task*>(tli.next());
        if (t->hasEndDependency())
            return true;
    }

    return false;
}

bool
Task::preScheduleOk(int sc)
{
//     if (account && !account->isLeaf())
//     {
//         errorMessage(QString
//                      ("Task '%1' must not have an account group ('%2') "
//                       "assigned to it.")
//                      .arg(name).arg(account->getId()));
//         return false;
//     }

    if (hasSubs() && !scenarios[sc].bookedResources.isEmpty())
    {
        errorMessage(QString
                     ("Task '%1' is a container task and must not have "
                      "bookings assigned to it.").arg(name));
        return false;
    }

    if (milestone && !scenarios[sc].bookedResources.isEmpty())
    {
        errorMessage(QString
                     ("Task '%1' is a milestone task and must not have "
                      "bookings assigned to it.").arg(name));
        return false;
    }

    if (scenarios[sc].specifiedScheduled && !sub->isEmpty() &&
        (scenarios[sc].specifiedStart == 0 ||
         scenarios[sc].specifiedEnd == 0))
    {
        errorMessage(QString
                     ("Task '%1' is marked as scheduled but does not have "
                      "a fixed start and end date.").arg(name));
        return false;
    }

    if (scenarios[sc].effort > 0.0 && allocations.count() == 0 &&
        !scenarios[sc].specifiedScheduled)
    {
        errorMessage(QString
                     ("No allocations specified for effort based task '%1'")
                     .arg(name));
        return false;
    }

    if (scenarios[sc].startBuffer + scenarios[sc].endBuffer >= 100.0)
    {
        errorMessage(QString
                     ("Start and end buffers may not overlap. So their sum must be smaller then 100%."));
        return false;
    }

    int durationSpec = 0;
    if (scenarios[sc].effort > 0.0)
        durationSpec++;
    if (scenarios[sc].length > 0.0)
        durationSpec++;
    if (scenarios[sc].duration > 0.0)
        durationSpec++;
    if (durationSpec > 1)
    {
        errorMessage(QString("Task '%1' may only have one duration "
                          "criteria in '%2' scenario.").arg(name)
                     .arg(project->getScenarioId(sc)));
        return false;
    }

    /*
    |: fixed start or end date
    -: no fixed start or end date
    M: Milestone
    D: start or end dependency
    x->: ASAP task with duration criteria
    <-x: ALAP task with duration criteria
    -->: ASAP task without duration criteria
    <--: ALAP task without duration criteria
     */
    bool hasStartDep = hasStartDependency(sc);
    bool hasEndDep = hasEndDependency(sc);
    if (!sub->isEmpty())
    {
        if (durationSpec != 0)
        {
            errorMessage(QString
                         ("Container task '%1' may not have a duration "
                          "criteria in '%2' scenario").arg(name)
                         .arg(project->getScenarioId(sc)));
            return false;
        }
        if (milestone)
        {
            errorMessage(QString
                         ("The container task '%1' may not be a "
                          "milestone.").arg(name));
            return false;
        }
    }
    else if (milestone)
    {
        if (durationSpec != 0)
        {
            errorMessage(QString
                         ("Milestone '%1' may not have a duration "
                          "criteria in '%2' scenario").arg(name)
                         .arg(project->getScenarioId(sc)));
            return false;
        }
        /*
        |  M -   ok     |D M -   ok     - M -   err1   -D M -   ok
        |  M |   err2   |D M |   err2   - M |   ok     -D M |   ok
        |  M -D  ok     |D M -D  ok     - M -D  ok     -D M -D  ok
        |  M |D  err2   |D M |D  err2   - M |D  ok     -D M |D  ok
         */
        /* err1: no start and end
        - M -
         */
        if (!hasStartDep && !hasEndDep)
        {
            errorMessage(QString("Milestone '%1' must have a start or end "
                              "specification in '%2' scenario.")
                         .arg(name).arg(project->getScenarioId(sc)));
            return false;
        }
        /* err2: different start and end
        |  M |
        |  M |D
        |D M |
        |D M |D
         */
        if (scenarios[sc].specifiedStart != 0 &&
            scenarios[sc].specifiedEnd != 0 &&
            scenarios[sc].specifiedStart != scenarios[sc].specifiedEnd + 1)
        {
            errorMessage(QString
                         ("Milestone '%1' may not have both a start "
                          "and an end specification that do not "
                          "match in the '%2' scenario.").arg(name)
                         .arg(project->getScenarioId(sc)));
            return false;
        }
    }
    else
    {
        /*
        Error table for non-container, non-milestone tasks:

        | x-> -   ok     |D x-> -   ok     - x-> -   err3   -D x-> -   ok
        | x-> |   err1   |D x-> |   err1   - x-> |   err3   -D x-> |   err1
        | x-> -D  ok     |D x-> -D  ok     - x-> -D  err3   -D x-> -D  ok
        | x-> |D  err1   |D x-> |D  err1   - x-> |D  err3   -D x-> |D  err1
        | --> -   err2   |D --> -   err2   - --> -   err3   -D --> -   err2
        | --> |   ok     |D --> |   ok     - --> |   err3   -D --> |   ok
        | --> -D  ok     |D --> -D  ok     - --> -D  err3   -D --> -D  ok
        | --> |D  ok     |D --> |D  ok     - --> |D  err3   -D --> |D  ok
        | <-x -   err4   |D <-x -   err4   - <-x -   err4   -D <-x -   err4
        | <-x |   err1   |D <-x |   err1   - <-x |   ok     -D <-x |   ok
        | <-x -D  err1   |D <-x -D  err1   - <-x -D  ok     -D <-x -D  ok
        | <-x |D  err1   |D <-x |D  err1   - <-x |D  ok     -D <-x |D  ok
        | <-- -   err4   |D <-- -   err4   - <-- -   err4   -D <-- -   err4
        | <-- |   ok     |D <-- |   ok     - <-- |   err2   -D <-- |   ok
        | <-- -D  ok     |D <-- -D  ok     - <-- -D  err2   -D <-- -D  ok
        | <-- |D  ok     |D <-- |D  ok     - <-- |D  err2   -D <-- |D  ok
         */
        /*
        err1: Overspecified (12 cases)
        |  x-> |
        |  <-x |
        |  x-> |D
        |  <-x |D
        |D x-> |
        |D <-x |
        |D <-x |D
        |D x-> |D
        -D x-> |
        -D x-> |D
        |D <-x -D
        |  <-x -D
         */
        if (((scenarios[sc].specifiedStart != 0 &&
              scenarios[sc].specifiedEnd != 0) ||
             (hasStartDep && scenarios[sc].specifiedStart == 0 &&
              scenarios[sc].specifiedEnd != 0 && scheduling == ASAP) ||
             (scenarios[sc].specifiedStart != 0 && scheduling == ALAP &&
              hasEndDep && scenarios[sc].specifiedEnd == 0)) &&
            durationSpec != 0 && !scenarios[sc].specifiedScheduled)
        {
            errorMessage(QString("Task '%1' has a start, an end and a "
                              "duration specification for '%2' scenario.")
                         .arg(name).arg(project->getScenarioId(sc)));
            return false;
        }
        /*
        err2: Underspecified (6 cases)
        |  --> -
        |D --> -
        -D --> -
        -  <-- |
        -  <-- |D
        -  <-- -D
         */
        if ((hasStartDep ^ hasEndDep) && durationSpec == 0)
        {
            errorMessage(QString
                         ("Task '%1' has only a start or end specification "
                          "but no duration for the '%2' scenario.")
                         .arg(name).arg(project->getScenarioId(sc)));
            return false;
        }
        /*
        err3: ASAP + Duration must have fixed start (8 cases)
        -  x-> -
        -  x-> |
        -  x-> -D
        -  x-> |D
        -  --> -
        -  --> |
        -  --> -D
        -  --> |D
         */
        if (!hasStartDep && scheduling == ASAP)
        {
            errorMessage(QString
                         ("Task '%1' needs a start specification to be "
                          "scheduled in ASAP mode in the '%2' scenario.")
                         .arg(name).arg(project->getScenarioId(sc)));
            return false;
        }
        /*
        err4: ALAP + Duration must have fixed end (8 cases)
        -  <-x -
        |  <-x -
        |D <-x -
        -D <-x -
        -  <-- -
        |  <-- -
        -D <-- -
        |D <-- -
         */
        if (!hasEndDep && scheduling == ALAP)
        {
            errorMessage(QString
                         ("Task '%1' needs an end specification to be "
                          "scheduled in ALAP mode in the '%2' scenario.")
                         .arg(name).arg(project->getScenarioId(sc)));
            return false;
        }
    }

//     if (!account &&
//         (scenarios[sc].startCredit > 0.0 || scenarios[sc].endCredit > 0.0))
//     {
//         errorMessage(QString
//                      ("Task '%1' has a specified start- or endcredit "
//                       "but no account assigned in scenario '%2'.")
//                      .arg(name).arg(project->getScenarioId(sc)));
//         return false;
//     }

    if (!scenarios[sc].bookedResources.isEmpty() && scheduling == ALAP &&
        !scenarios[sc].specifiedScheduled)
    {
        errorMessage
            (QString("Error in task '%1'. "
                  "An ALAP task can only have bookings if it has been "
                  "completely scheduled. The 'scheduled' attribute must be "
                  "present. Keep in mind that certain attributes such as "
                  "'precedes' or 'end' implicitly set the scheduling mode "
                  "to ALAP. Put 'scheduling asap' at the end of the task "
                  "definition to avoid the problem.")
             .arg(name));
        return false;
    }

    return true;
}

bool
Task::scheduleOk(int sc) const
{
    const QString scenario = project->getScenarioId(sc);

    /* It is of little use to report errors of container tasks, if any of
     * their sub tasks has errors. */
    int oldErrors = TJMH.getErrors();
    for (TaskListIterator tli(*sub); tli.hasNext();) {
        Task *t = static_cast<Task*>(tli.next());
        t->scheduleOk(sc);
    }
    if (oldErrors != TJMH.getErrors())
    {
        if (DEBUGPS(2))
            tjDebug(QString("Scheduling errors in sub tasks of '%1'.")
                   .arg(name));
        return false;
    }

    /* Runaway errors have already been reported. Since the data of this task
     * is very likely completely bogus, we just return false. */
    if (runAway)
        return false;

    if (DEBUGPS(3))
        qDebug()<<"Checking task"<<name;

    /* If any of the dependant tasks is a runAway, we can safely surpress all
     * other error messages. */
    for (QListIterator<TaskDependency*> tdi(depends); tdi.hasNext();) {
        TaskDependency *td = static_cast<TaskDependency*>(tdi.next());
        if (td->getTaskRef()->runAway)
            return false;
    }
    for (QListIterator<TaskDependency*> tdi(precedes); tdi.hasNext();) {
        TaskDependency *td = static_cast<TaskDependency*>(tdi.next());
        if (td->getTaskRef()->runAway)
            return false;
    }
    if (start == 0)
    {
        errorMessage(i18nc("@info/plain", "Start time is not calculated"));
        return false;
    }
    if (start < project->getStart() || start > project->getEnd())
    {
        errorMessage(i18nc("@info/plain", "Start time %1 is outside of the project target times (%2 - %3)",
                     formatTime(start),
                     formatTime(project->getStart()),
                     formatTime(project->getEnd())));
        return false;
    }
#if 0
NOT USED ATM
    if (scenarios[sc].minStart != 0 && start < scenarios[sc].minStart)
    {
        warningMessage(QString("'%1' start time of task '%2' is too early\n"
                            "Date is:  %3\n"
                            "Limit is: %4")
                       .arg(scenario).arg(name).arg(time2tjp(start))
                       .arg(time2tjp(scenarios[sc].minStart)));
        return false;
    }
    if (scenarios[sc].maxStart != 0 && start > scenarios[sc].maxStart)
    {
        warningMessage(QString("'%1' start time of task '%2' is too late\n"
                            "Date is:  %3\n"
                            "Limit is: %4")
                       .arg(scenario).arg(name)
                       .arg(time2tjp(start))
                       .arg(time2tjp(scenarios[sc].maxStart)));
        return false;
    }
#endif
    if (end == 0)
    {
        errorMessage(i18nc("info/plain", "End time is not calculated"));
        return false;
    }
    if ((end + 1) < project->getStart() || (end > project->getEnd()))
    {
        errorMessage(i18nc("info/plain", "End time %1 is outside of the project target times (%2 - %3)",
                     formatTime(end + 1),
                     formatTime(project->getStart()),
                     formatTime(project->getEnd() + 1)));
        return false;
    }
#if 0
NOT USED ATM
    if (scenarios[sc].minEnd != 0 && end < scenarios[sc].minEnd)
    {
        warningMessage(QString("'%1' end time of task '%2' is too early\n"
                            "Date is:  %3\n"
                            "Limit is: %4")
                       .arg(scenario).arg(name)
                       .arg(time2tjp(end + 1))
                       .arg(time2tjp(scenarios[sc].minEnd + 1)));
        return false;
    }
    if (scenarios[sc].maxEnd != 0 && end > scenarios[sc].maxEnd)
    {
        warningMessage(QString("'%1' end time of task '%2' is too late\n"
                            "Date is:  %2\n"
                            "Limit is: %3")
                       .arg(scenario).arg(name)
                       .arg(time2tjp(end + 1))
                       .arg(time2tjp(scenarios[sc].maxEnd + 1)));
        return false;
    }
    if (!sub->isEmpty())
    {
        // All sub task must fit into their parent task.
        for (TaskListIterator tli(*sub); tli.hasNext();)
        {
            Task *t = static_cast<Task*>(tli.next());
            if (start > t->start)
            {
                if (!t->runAway)
                {
                    errorMessage(QString("Task '%1' has earlier '%2' start than "
                                      "parent\n"
                                      "%3 start date: %4\n"
                                      "%5 start date: %6")
                                 .arg(t->getId()).arg(scenario)
                                 .arg(name)
                                 .arg(time2ISO(start))
                                 .arg(t->getId())
                                 .arg(time2ISO(t->start)));
                }
                return false;
            }
            if (end < t->end)
            {
                if (!t->runAway)
                {
                    errorMessage(QString("Task '%1' has later '%2' end than "
                                      "parent")
                                 .arg(name).arg(scenario));
                }
                return false;
            }
        }
    }
#endif
    // Check if all previous tasks end before start of this task.
    for (TaskListIterator tli(predecessors); tli.hasNext();) {
        Task *t = static_cast<Task*>(tli.next());
        if (t->end > start && !t->runAway)
        {
            errorMessage(i18nc("@info/plain", "Impossible dependency:<nl/>"
                              "Task '%1' ends at %2 but must precede<nl/>"
                              "task '%3' which starts at %4",
                         t->getName(), formatTime(t->end + 1),
                         name, formatTime(start)));
            return false;
        }
    }
    // Check if all following task start after this tasks end.
    for (TaskListIterator tli(successors); tli.hasNext();) {
        Task *t = static_cast<Task*>(tli.next());
        if (end > t->start && !t->runAway)
        {
            errorMessage(i18nc("@info/plain", "Impossible dependency:<nl/>"
                              "Task '%1' starts at %2 but must follow<nl/>"
                              "task %3 which ends at %4",
                         t->getName(), formatTime(t->start),
                         name, formatTime(end + 1)));
            return false;
        }
    }
    if (!schedulingDone)
    {
        warningMessage(i18nc("info/plain", "Task has not been marked completed.\n"
                          "It is scheduled to last from %1 to %2.\n"
                          "This might be a bug in the scheduler.",
                     formatTime(start), formatTime(end + 1)));
        return false;
    }

    return true;
}

time_t
Task::nextSlot(time_t slotDuration) const
{
    if (scheduling == ASAP)
    {
        if (lastSlot == 0)
            return start;
        return lastSlot + 1;
    }
    else
    {
        if (lastSlot == 0)
            return end - slotDuration + 1;

        return lastSlot - slotDuration;
    }

    return 0;
}

bool
Task::isReadyForScheduling() const
{
    /* This function returns true if the tasks has all the necessary
     * information to be scheduled and has not been completely scheduled yet.
     */
    if (schedulingDone)
        return false;

    if (scheduling == ASAP)
    {
        if (start != 0)
        {
            if (effort == 0.0 && length == 0.0 && duration == 0.0 &&
                !milestone && end == 0)
                return false;

            return true;
        }
    }
    else
    {
        if (end != 0)
        {
            if (effort == 0.0 && length == 0.0 && duration == 0.0 &&
                !milestone && start == 0) {
                return false;
            }

            return true;
        }
    }
//     TJMH.debugMessage(QString("Not ready for scheduling: %1 start=%2, end=%3").arg(scheduling==ASAP?"ASAP":"ALAP").arg(start).arg(end), this);
    return false;
}

bool
Task::isActive(int sc, const Interval& period) const
{
    return period.overlaps(Interval(scenarios[sc].start,
                                    milestone ? scenarios[sc].start :
                                    scenarios[sc].end));
}

bool
Task::isSubTask(const Task* tsk) const
{
    for (TaskListIterator tli(*sub); tli.hasNext();) {
        Task *t = static_cast<Task*>(tli.next());
        if (*tli == tsk || t->isSubTask(tsk))
            return true;
    }
    return false;
}

void
Task::overlayScenario(int base, int sc)
{
    /* Copy all values that the scenario sc does not provide, but that are
     * provided by the base scenario to the scenario sc. */
    if (scenarios[sc].specifiedStart == 0)
        scenarios[sc].specifiedStart = scenarios[base].specifiedStart;
    if (scenarios[sc].specifiedEnd == 0)
        scenarios[sc].specifiedEnd = scenarios[base].specifiedEnd;
    if (scenarios[sc].minStart == 0)
        scenarios[sc].minStart = scenarios[base].minStart;
    if (scenarios[sc].maxStart == 0)
        scenarios[sc].maxStart = scenarios[base].maxStart;
    if (scenarios[sc].minEnd == 0)
        scenarios[sc].minEnd = scenarios[base].minEnd;
    if (scenarios[sc].maxEnd == 0)
        scenarios[sc].maxEnd = scenarios[base].maxEnd;
    if (scenarios[sc].duration == 0.0)
        scenarios[sc].duration =  scenarios[base].duration;
    if (scenarios[sc].length == 0.0)
        scenarios[sc].length = scenarios[base].length;
    if (scenarios[sc].effort == 0.0)
        scenarios[sc].effort = scenarios[base].effort;
    if (scenarios[sc].startBuffer < 0.0)
        scenarios[sc].startBuffer = scenarios[base].startBuffer;
    if (scenarios[sc].endBuffer < 0.0)
        scenarios[sc].endBuffer = scenarios[base].endBuffer;
    if (scenarios[sc].startCredit < 0.0)
        scenarios[sc].startCredit = scenarios[base].startCredit;
    if (scenarios[sc].endCredit < 0.0)
        scenarios[sc].endCredit = scenarios[base].endCredit;
    if (scenarios[sc].reportedCompletion < 0.0)
        scenarios[sc].reportedCompletion = scenarios[base].reportedCompletion;
}

void
Task::prepareScenario(int sc)
{
    start = scenarios[sc].start = scenarios[sc].specifiedStart;
    end = scenarios[sc].end = scenarios[sc].specifiedEnd;
    schedulingDone = scenarios[sc].scheduled = scenarios[sc].specifiedScheduled;
    scenarios[sc].isOnCriticalPath = false;
    scenarios[sc].pathCriticalness = -1.0;

    duration = scenarios[sc].duration;
    length = scenarios[sc].length;
    effort = scenarios[sc].effort;
    lastSlot = 0;
    doneEffort = 0.0;
    doneDuration = 0.0;
    doneLength = 0.0;
    tentativeStart = tentativeEnd = 0;
    workStarted = false;
    runAway = false;
    bookedResources.clear();
    bookedResources = scenarios[sc].specifiedBookedResources;

    /* The user could have made manual bookings already. The effort of these
     * bookings needs to be calculated so that the scheduler only schedules
     * the still missing effort. Scheduling will begin after the last booking.
     * This will only work for ASAP tasks. ALAP tasks cannot be partly booked.
     */
    time_t firstSlot = 0;
    for (ResourceListIterator rli(bookedResources); rli.hasNext();)
    {
        Resource *r = static_cast<Resource*>(rli.next());
        double effort = r->getEffectiveLoad
            (sc, Interval(project->getStart(), project->getEnd()),
             AllAccounts, this);
        if (effort > 0.0)
        {
            doneEffort += effort;
            if (firstSlot == 0 ||
                firstSlot > r->getStartOfFirstSlot(sc, this))
            {
                firstSlot = r->getStartOfFirstSlot(sc, this);
            }
            time_t ls = r->getEndOfLastSlot(sc, this);
            if (ls > lastSlot)
                lastSlot = ls;
        }
    }

    if (lastSlot > 0)
    {
        if (schedulingDone)
        {
            /* If the done flag is set, the user declares that the task is
             * done. If no end date has been specified, set the start and end
             * date to the begin of the first slot and end of the last slot.
             */
            if (scenarios[sc].start == 0)
                start = scenarios[sc].start = firstSlot;
            if (scenarios[sc].end == 0)
                end = scenarios[sc].end = lastSlot;
        }
        else
        {
            /* Some bookings have been specified for the task, but it is not
             * marked completed yet. */
            workStarted = true;
            // Trim start to first booked time slot.
            start = firstSlot;

            /* In projection mode, we assume that the completed work has been
             * reported with booking attributes. Now we compute the completion
             * degree according to the overall effort. Then the end date of
             * the task is calculated. */
            if (project->getScenario(sc)->getProjectionMode() && effort > 0.0)
            {
                scenarios[sc].reportedCompletion = doneEffort / effort * 100.0;
                if (scenarios[sc].reportedCompletion > 100.0)
                    scenarios[sc].reportedCompletion = 100.0;

                if (doneEffort >= effort)
                {
                    /* In case the required effort is reached or exceeded by
                     * the specified bookings for this task, we set the task
                     * end to the last booking and mark the task as completely
                     * scheduled. */
                    end = scenarios[sc].end = lastSlot;
                    schedulingDone = true;

                    /* We allow up to one time slot fuzziness before we
                     * generate a warning. */
                    if (project->getScenario(sc)->getStrictBookings() &&
                        doneEffort > effort +
                        project->convertToDailyLoad
                        (project->getScheduleGranularity() - 1))
                    {
                        /* In case the bookings exceed the specified effort
                         * in strict mode, show a warning. */
                        warningMessage(i18nc("info/plain", "Planned effort %1 exceeds estimated effort %2",
                                       doneEffort,
                                       effort));
                    }
                }
                else
                    lastSlot = project->getNow() - 1;
            }
        }
    }

    /*
     * To determine the criticalness of an effort based task, we need to
     * determine the allocation probability of all of the resources. The more
     * the resources that are allocated to a task are allocated the smaller is
     * the likelyhood that the task will get it's allocation, the more
     * critical it is.
     *
     * The allocation probability of a resource for this task is basically
     * effort divided by number of allocated resources. Since the efficiency
     * of resources can vary we need to determine the overall efficiency
     * first.
     *
     * TODO: We need to respect limits and shifts here!
     */
    double allocationEfficiency = 0;
    for (QListIterator<Allocation*> ali(allocations); ali.hasNext();)
    {
        Allocation *a = static_cast<Allocation*>(ali.next());
        a->init();
        if (a->isPersistent() && !bookedResources.isEmpty())
        {
            /* If the allocation is persistent and we have already bookings,
             * we need to find the resource with the last booking for this
             * task and save it as looked resource. */
            time_t lastSlot = 0;
            Resource* lastResource = 0;
            for (QListIterator<Resource*> rli = a->getCandidatesIterator(); rli.hasNext();) {
                Resource *r = static_cast<Resource*>(rli.next());
                for (ResourceTreeIterator rti(r); *rti; ++rti) {
                    if (bookedResources.indexOf(*rti) != -1 &&
                        (lastResource == 0 ||
                         lastSlot < (*rti)->getEndOfLastSlot(sc, this)))
                    {
                        lastSlot = (*rti)->getEndOfLastSlot(sc, this);
                        lastResource = r;
                    }
                }
            }
            a->setLockedResource(lastResource);
        }
        if (scenarios[sc].effort > 0.0)
        {
            double maxEfficiency = 0;
            for (QListIterator<Resource*> rli = a->getCandidatesIterator(); rli.hasNext();)
            {
                Resource *r = static_cast<Resource*>(rli.next());
                for (ResourceTreeIterator rti(r); *rti; ++rti) {
                    if ((*rti)->getEfficiency() > maxEfficiency)
                        maxEfficiency = (*rti)->getEfficiency();
                }
            }
            allocationEfficiency += maxEfficiency;
        }
    }
    if (scenarios[sc].effort > 0.0)
    {
        /* Now we can add the allocation probability for this task to all the
         * individual resources. */
        double effortPerResource = effort / allocationEfficiency;
        for (QListIterator<Allocation*> ali(allocations); ali.hasNext();) {
            Allocation *a = static_cast<Allocation*>(ali.next());
            for (QListIterator<Resource*> rli = a->getCandidatesIterator(); rli.hasNext();) {
                Resource *r = static_cast<Resource*>(rli.next());
                for (ResourceTreeIterator rti(r); *rti; ++rti) {
                    (*rti)->addAllocationProbability
                        (sc, effortPerResource * (*rti)->getEfficiency());
                }
            }
        }
    }
}

void
Task::computeCriticalness(int sc)
{
    if (scenarios[sc].effort > 0.0)
    {
        double overallAllocationProbability = 0;
        for (QListIterator<Allocation*> ali(allocations); ali.hasNext();)
        {
            /* We assume that out of the candidates for an allocation the
             * one with the smallest overall allocation probability will
             * be assigned to the task. */
            Allocation *a = static_cast<Allocation*>(ali.next());
            double smallestAllocationProbablity = 0;
            for (QListIterator<Resource*> rli =
                 a->getCandidatesIterator(); rli.hasNext();)
            {
                /* If the candidate is a resource group we use the average
                 * allocation probablility of all the resources of the group.
                 */
                int resources = 0;
                double averageProbability = 0.0;
                for (ResourceTreeIterator rti(rli.next()); *rti; ++rti, ++resources) {
                    averageProbability +=
                        (*rti)->getAllocationProbability(sc);
                }
                if (resources > 0)
                    averageProbability /= resources;

                if (smallestAllocationProbablity == 0 ||
                    averageProbability < smallestAllocationProbablity)
                    smallestAllocationProbablity = averageProbability;
            }
            overallAllocationProbability += smallestAllocationProbablity;
        }
        /* New we normalize the allocationProbability to the duration of the
         * project (working days). For a resource that is statistically
         * allocated no more and no less than the number of working days in
         * the expected project time the probability will be one. This
         * certainly neglects many things like vacations, shifts, parallel
         * assignements and other factors. But we don't know enough about
         * these factors yet, to take them into account. So we have to live
         * with what we got. */
        overallAllocationProbability /=
            allocations.count() *
            ((project->getEnd() - project->getStart()) / (60.0 * 60 * 24)) *
            (project->getYearlyWorkingDays() / 365.0);
        /* Weight the average allocation probability with the effort of the
         * task. The higher the effort and the higher the probability that the
         * resources are allocated, the more critical the task rating gets. To
         * ensure that the criticalness is at least as high as a comparable
         * 'length' tasks use the effort value as a baseline and add the
         * weighted effort ontop. */
        scenarios[sc].criticalness = (1 + overallAllocationProbability) *
            scenarios[sc].effort;
    }
    else if (scenarios[sc].duration > 0.0)
        scenarios[sc].criticalness = duration;
    else if (scenarios[sc].length > 0.0)
        scenarios[sc].criticalness = length *
            (365 / project->getYearlyWorkingDays());
    else if (isMilestone())
    {
        /* People think of milestones usually as something important. So let's
         * assume a milestone has the importance of a full working day. This
         * is only done to raise the criticalness of pathes that contain
         * milestones. */
        scenarios[sc].criticalness = 1.0;
    }
    else
        scenarios[sc].criticalness = 0;

}

double
Task::computePathCriticalness(int sc)
{
    /*
     * The path criticalness is a measure for the overall criticalness of the
     * task taking the dependencies into account. The fact that a task is part
     * of a chain of effort-based task raises all the task in the chain to a
     * higher criticalness level than the individual tasks. In fact, the path
     * criticalness of this chain is equal to the sum of the individual
     * criticalnesses of the tasks that are trailing this task. It does not
     * take the user-defined priorities into account.
     */
    // If the value has been computed already, just return it.
    if (scenarios[sc].pathCriticalness >= 0.0)
        return scenarios[sc].pathCriticalness;

    double maxCriticalness = 0.0;
    if (hasSubs())
    {
        double criticalness;
        for (TaskListIterator tli(getSubListIterator()); tli.hasNext();)
        {
            Task *t = static_cast<Task*>(tli.next());
            criticalness = t->computePathCriticalness(sc);

            if (criticalness > maxCriticalness)
                maxCriticalness = criticalness;
        }
    }
    else
    {
        /* We only care about leaf tasks because that's were the resources are
         * actually used. Container tasks are respected during path tracking,
         * though.
         * Therefore, we generate a list of all successors to this task. These
         * are directly specified successors or successors of any of the
         * parent tasks of this task. */
        TaskList followerList;
        Task* t = this;
        while (t)
        {
            for (TaskListIterator tli(t->followers); tli.hasNext();) {
                Task *t = static_cast<Task*>(tli.next());
                if (followerList.indexOf(t) == -1)
                    followerList.append(t);
            }
            t = static_cast<Task*>(t->parent);
        }


        double criticalness;
        for (TaskListIterator tli(followerList); tli.hasNext();)
        {
            Task *t = static_cast<Task*>(tli.next());
            criticalness = t->computePathCriticalness(sc);

            if (criticalness > maxCriticalness)
                maxCriticalness = criticalness;
        }
    }

    scenarios[sc].pathCriticalness = scenarios[sc].criticalness +
        maxCriticalness;

    return scenarios[sc].pathCriticalness;
}

void
Task::checkAndMarkCriticalPath(int sc, double minSlack, time_t maxEnd)
{
    // The algorithm has to start at a leaf task that has no predecessors.
    if (hasSubs() || !previous.isEmpty())
        return;

    if (DEBUGPA(3))
        qDebug()<<"Starting critical path search at"<<id;

    long worstMinSlackTime = static_cast<long>((maxEnd - getStart(sc)) *
                                               minSlack);
    long checks = 0;
    long found = 0;
    analyzePath(sc, minSlack, getStart(sc), 0, worstMinSlackTime, checks,
                found);
}

bool
Task::analyzePath(int sc, double minSlack, time_t pathStart, long busyTime,
                  long worstMinSlackTime, long& checks, long& found)
{
    /* Saveguard to limit the runtime for this NP hard algorithm. */
    long maxPaths = project->getScenario(sc)->getMaxPaths();
    if (maxPaths > 0 && checks >= maxPaths)
        return false;

    if (DEBUGPA(14))
        qDebug()<<"  * Checking task"<<id;

    bool critical = false;

    if (hasSubs())
    {
        if (DEBUGPA(15))
            qDebug()<<"  > Sub check started for"<<id;

        for (TaskListIterator tli(*sub); tli.hasNext();) {
            Task *t = static_cast<Task*>(tli.next());
            if (t->analyzePath(sc, minSlack, pathStart, busyTime,
                                    worstMinSlackTime, checks, found))
                critical = true;
        }
        if (DEBUGPA(15))
            qDebug()<<"  < Sub check finished for"<<id;
    }
    else
    {
        busyTime += (getEnd(sc) + 1 - getStart(sc));

        /* If we have enough slack already that the path cannot be critical,
         * we stop looking at the rest of the path. */
        long currentSlack = (getEnd(sc) + 1 - pathStart) - busyTime;
        if (currentSlack > worstMinSlackTime)
        {
            checks++;
            if (DEBUGPA(6))
                qDebug()<<"Path cannot be critical. Stopping at task"<<id;
            return false;
        }

        /* Find out if any of the followers is a sibling of the parent of this
         * task. */
        bool hasBrotherFollower = false;
        for (TaskListIterator tli(followers); tli.hasNext() && !hasBrotherFollower;)
            for (Task* t = static_cast<Task*>(tli.next()); t; t = t->getParent())
                if (t == getParent())
                {
                    hasBrotherFollower = true;
                    break;
                }

        /* We first have to gather a list of all followers of this task. This
         * list must also include the followers registered for all parent
         * tasks of this task as they are followers as well. */
        TaskList allFollowers;
        for (Task* task = this; task; task = task->getParent())
        {
            for (TaskListIterator tli(task->followers); tli.hasNext();) {
                Task *t = static_cast<Task*>(tli.next());
                if (allFollowers.indexOf(t) < 0)
                    allFollowers.append(t);
            }
            /* If the task has a follower that is a sibling of the same parent
             * we ignore the parent followers. */
            if (hasBrotherFollower)
                break;
        }

        /* Get a list of all transient followers that follow the direct and
         * indirect followers of this task. If the allFollowers list contain
         * any of the transient followers, we can ignore it later. */
        TaskList transientFollowers;
        for (TaskListIterator tli(allFollowers); tli.hasNext();) {
            Task *t = static_cast<Task*>(tli.next());
            t->collectTransientFollowers(transientFollowers);
        }
        /* For inherited dependencies we only follow the bottommost task that
         * is a follower. All parents in the allFollowers list are ignored. */
        TaskList ignoreList;
        for (TaskListIterator tli(allFollowers); tli.hasNext();) {
            Task *t = static_cast<Task*>(tli.next());
            for (Task* p = t->getParent(); p; p = p->getParent()) {
                Task *t = static_cast<Task*>(tli.next());
                if (allFollowers.indexOf(p) >= 0 && ignoreList.indexOf(p) < 0)
                    ignoreList.append(p);
            }
        }
        /* Now we can check the paths through the remaining followers. */
        for (TaskListIterator tli(allFollowers); tli.hasNext();)
        {
            Task *t = static_cast<Task*>(tli.next());
            if (ignoreList.indexOf(t) >= 0 ||
                transientFollowers.indexOf(t) >= 0)
                continue;

            if (DEBUGPA(16))
                qDebug()<<QString("  > Follower check started for %1").arg(t->id);

            if (t->analyzePath(sc, minSlack, pathStart, busyTime,
                                    worstMinSlackTime, checks, found))
            {
                if (scenarios[sc].criticalLinks.indexOf(t) < 0)
                {
                    if (DEBUGPA(5))
                        qDebug()<<QString("  +++ Critical link %1 -> %2").arg(name).arg(t->id);
                    scenarios[sc].criticalLinks.append(t);
                }

                critical = true;
            }

            if (DEBUGPA(16))
                qDebug()<<"  < Follower check finished for"<<id;
        }

        if (allFollowers.isEmpty())
        {
            // We've reached the end of a path. Now let's see if it's critical.
            long overallDuration = getEnd(sc) + 1 - pathStart;
            /* A path is considered critical if the ratio of busy time and
             * overall path time is above the minSlack threshold and the path
             * contains more than one task. */
            critical = overallDuration > 0 &&
                ((double) busyTime / overallDuration) > (1.0 - minSlack);
            if (critical)
            {
                found++;
                if (DEBUGPA(5))
                    qDebug()<<"Critical path with"<<(100.0 - ((double) busyTime / overallDuration) * 100.0)<<"% slack ending at"<<id<<"found";
            }
            else
            {
                if (DEBUGPA(11))
                    qDebug()<<"Path ending at"<<id<<"is not critical";
            }
            if (++checks == maxPaths)
            {
                warningMessage(QString("Maximum number of paths reached during "
                                    "critical path analysis. Set 'maxPaths' "
                                    "to 0 if you want an exhaustive search. "
                                    "Aborting critical paths detection."));
                return false;
            }
            if (checks % 100000 == 0 && DEBUGPA(1))
                qDebug()<<"Already check"<<checks<<"paths."<<found<<"critical found.";
        }
    }

    if (critical)
       scenarios[sc].isOnCriticalPath = true;

    if (DEBUGPA(14))
        qDebug()<<"  - Check of task"<<id<<"completed ("<<checks<<")";
    return critical;
}

void
Task::collectTransientFollowers(TaskList& list)
{
    if (hasSubs())
    {
        for (TaskListIterator tli(followers); tli.hasNext();) {
            Task *t = static_cast<Task*>(tli.next());
            if (list.indexOf(t) < 0)
            {
                list.append(t);
                t->collectTransientFollowers(list);
            }
        }
    }
    else
    {
        for (Task* task = getParent(); task; task = task->getParent()) {
            for (TaskListIterator tli(task->followers); tli.hasNext();) {
                Task *t = static_cast<Task*>(tli.next());
                if (list.indexOf(t) < 0)
                {
                    list.append(t);
                    t->collectTransientFollowers(list);
                }
            }
        }
    }
}

void
Task::finishScenario(int sc)
{
    scenarios[sc].start = start;
    scenarios[sc].end = end;
    scenarios[sc].bookedResources = bookedResources;
    scenarios[sc].scheduled = schedulingDone;
//     qDebug()<<"finishScenario: '"<< name << "' finish: " << time2ISO(start) << " - " << time2ISO(end);
}

void
Task::computeBuffers()
{
    int sg = project->getScheduleGranularity();
    for (int sc = 0; sc < project->getMaxScenarios(); sc++)
    {
        scenarios[sc].startBufferEnd = scenarios[sc].start - 1;
        scenarios[sc].endBufferStart = scenarios[sc].end + 1;

        if (scenarios[sc].start == 0 || scenarios[sc].end == 0)
        {
            scenarios[sc].startBufferEnd = scenarios[sc].endBufferStart = 0;
            continue;
        }

        if (duration > 0.0)
        {
            if (scenarios[sc].startBuffer > 0.0)
                scenarios[sc].startBufferEnd = scenarios[sc].start +
                    static_cast<time_t>((scenarios[sc].end -
                                         scenarios[sc].start) *
                              scenarios[sc].startBuffer / 100.0);
            if (scenarios[sc].endBuffer > 0.0)
                scenarios[sc].endBufferStart = scenarios[sc].end -
                    static_cast<time_t>((scenarios[sc].end -
                                         scenarios[sc].start) *
                              scenarios[sc].endBuffer / 100.0);
        }
        else if (length > 0.0)
        {
            double l;
            if (scenarios[sc].startBuffer > 0.0)
            {
                for (l = 0.0; scenarios[sc].startBufferEnd < scenarios[sc].end;
                     scenarios[sc].startBufferEnd += sg)
                {
                    if (project->isWorkingDay(scenarios[sc].startBufferEnd))
                    l += (double) sg / ONEDAY;
                    if (l >= scenarios[sc].length *
                        scenarios[sc].startBuffer / 100.0)
                        break;
                }
            }
            if (scenarios[sc].endBuffer > 0.0)
            {
                for (l = 0.0; scenarios[sc].endBufferStart >
                     scenarios[sc].start; scenarios[sc].endBufferStart -= sg)
                {
                    if (project->isWorkingDay(scenarios[sc].endBufferStart))
                        l += (double) sg / ONEDAY;
                    if (l >= scenarios[sc].length *
                        scenarios[sc].endBuffer / 100.0)
                        break;
                }
            }
        }
        else if (effort > 0.0)
        {
            double e;
            if (scenarios[sc].startBuffer > 0.0)
            {
                for (e = 0.0; scenarios[sc].startBufferEnd < scenarios[sc].end;
                     scenarios[sc].startBufferEnd += sg)
                {
                    e += getLoad(sc,
                                 Interval(scenarios[sc].startBufferEnd,
                                          scenarios[sc].startBufferEnd + sg));
                    if (e >= scenarios[sc].effort *
                        scenarios[sc].startBuffer / 100.0)
                        break;
                }
            }
            if (scenarios[sc].endBuffer > 0.0)
            {
                for (e = 0.0; scenarios[sc].endBufferStart >
                     scenarios[sc].start; scenarios[sc].endBufferStart -= sg)
                {
                    e += getLoad(sc,
                                 Interval(scenarios[sc].endBufferStart - sg,
                                          scenarios[sc].endBufferStart));
                    if (e >= scenarios[sc].effort *
                        scenarios[sc].endBuffer / 100.0)
                        break;
                }
            }
        }
    }
}

void
Task::calcCompletionDegree(int sc)
{
    time_t now = project->getNow();

    /* In-progress container task are pretty complex to deal with. The mixture
     * of effort, length and duration tasks makes it impossible to use one
     * coherent criteria to determine the progress of a task. */
    if (isContainer() && scenarios[sc].start < now && now <= scenarios[sc].end)
        calcContainerCompletionDegree(sc, now);
    else
        /* Calc completion for simple tasks and determine the task state. */
        scenarios[sc].calcCompletionDegree(now);
}

double
Task::getCompletionDegree(int sc) const
{
    if(scenarios[sc].reportedCompletion >= 0.0)
        return(scenarios[sc].reportedCompletion);

    return isContainer() && scenarios[sc].containerCompletion >= 0.0 ?
        scenarios[sc].containerCompletion : scenarios[sc].completionDegree;
}

double
Task::getCalcedCompletionDegree(int sc) const
{
    return scenarios[sc].completionDegree;
}

void
Task::calcContainerCompletionDegree(int sc, time_t now)
{
    assert(isContainer());
    assert(scenarios[sc].start < now && now <= scenarios[sc].end);

    scenarios[sc].status = InProgress;

    int totalMilestones = 0;
    int completedMilestones = 0;
    int reportedCompletedMilestones = 0;
    if (countMilestones(sc, now, totalMilestones, completedMilestones,
                        reportedCompletedMilestones))
    {
        scenarios[sc].completionDegree = completedMilestones * 100.0 /
            totalMilestones;
        scenarios[sc].containerCompletion = reportedCompletedMilestones
            * 100.0 / totalMilestones;
        return;
    }

    double totalEffort = 0.0;
    double completedEffort = 0.0;
    double reportedCompletedEffort = 0.0;
    if (sumUpEffort(sc, now, totalEffort, completedEffort,
                    reportedCompletedEffort))
    {
        scenarios[sc].completionDegree = completedEffort * 100.0 /
            totalEffort;
        scenarios[sc].containerCompletion = reportedCompletedEffort * 100.0 /
            totalEffort;
    }
    else
    {
        /* We can't determine the completion degree for mixed work/non-work
         * tasks. So we use -1.0 as "in progress" value. */
        double comp = -1.0;
        if (scenarios[sc].start > now)
            comp = 0.0; // not yet started
        else if (scenarios[sc].end < now)
            comp = 100.0; // completed
        scenarios[sc].completionDegree =
            scenarios[sc].containerCompletion = comp;
    }
}

double
Task::getCompletedLoad(int sc) const
{
    return getLoad(sc, Interval(project->getStart(), project->getEnd())) *
        getCompletionDegree(sc) / 100.0;
}

double
Task::getRemainingLoad(int sc) const
{
    return getLoad(sc, Interval(project->getStart(), project->getEnd())) *
        (1.0 - getCompletionDegree(sc) / 100.0);
}

bool
Task::countMilestones(int sc, time_t now, int& totalMilestones,
                      int& completedMilestones,
                      int& reportedCompletedMilestones)
{
    if (isContainer())
    {
        for (TaskListIterator tli(*sub); tli.hasNext();) {
            Task *t = static_cast<Task*>(tli.next());
            if (!t->countMilestones(sc, now, totalMilestones,
                                         completedMilestones,
                                         reportedCompletedMilestones))
                return false;
        }
        /* A reported completion for a container always overrides the computed
         * completion. */
        if (scenarios[sc].reportedCompletion >= 0.0)
            reportedCompletedMilestones = static_cast<int>(totalMilestones *
                scenarios[sc].reportedCompletion / 100.0);

        return true;
    }
    else if (isMilestone())
    {
        totalMilestones++;
        if (scenarios[sc].start <= now)
            completedMilestones++;

        if (scenarios[sc].reportedCompletion >= 100.0)
            reportedCompletedMilestones++;
        else
            if (scenarios[sc].start <= now)
                reportedCompletedMilestones++;

        return true;
    }

    return false;
}

bool
Task::sumUpEffort(int sc, time_t now, double& totalEffort,
                  double& completedEffort, double& reportedCompletedEffort)
{
    if (isContainer())
    {
        for (TaskListIterator tli(*sub); tli.hasNext();) {
            Task *t = static_cast<Task*>(tli.next());
            if (!t->sumUpEffort(sc, now, totalEffort, completedEffort,
                                     reportedCompletedEffort))
                return false;
        }
        /* A reported completion for a container always overrides the computed
         * completion. */
        if (scenarios[sc].reportedCompletion >= 0.0)
            reportedCompletedEffort = totalEffort *
                scenarios[sc].reportedCompletion / 100.0;

        return true;
    }
    if (scenarios[sc].effort > 0.0)
    {
        /* Pure effort based tasks are simple to handle. The total effort is
         * specified and the effort up to 'now' can be computed. */
        totalEffort += scenarios[sc].effort;
        double load = getLoad(sc, Interval(scenarios[sc].start, now));
        if (scenarios[sc].start < now)
            completedEffort += load;

        /* If the user reported a completion we use this instead of the
         * calculated completion. */
        if (scenarios[sc].reportedCompletion >= 0.0)
            reportedCompletedEffort +=
                getLoad(sc, Interval(scenarios[sc].start, scenarios[sc].end)) *
                scenarios[sc].reportedCompletion / 100.0;
        else
            reportedCompletedEffort += load;

        return true;
    }
    if (!allocations.isEmpty())
    {
        /* This is for length and duration tasks that have allocations. We
         * handle them similar to effort tasks. Since there is no specified
         * total effort, we calculate the total allocated effort. */
        double totalLoad = getLoad(sc, Interval(scenarios[sc].start,
                                                scenarios[sc].end));
        totalEffort += totalLoad;
        double load = getLoad(sc, Interval(scenarios[sc].start, now));
        if (scenarios[sc].start < now)
            completedEffort += load;

        /* If the user reported a completion we use this instead of the
         * calculated completion. */
        if (scenarios[sc].reportedCompletion >= 0.0)
            reportedCompletedEffort += totalLoad *
                scenarios[sc].reportedCompletion / 100.0;
        else
            reportedCompletedEffort += load;

        return true;
    }
    if (isMilestone())
    {
        /* We assume that milestones are only dependent on sub tasks of this
         * tasks. So we can ignore them for the completion degree. In case
         * there is a non completed task that the milestone depends on, the
         * milestone is accounted for as well. This approximation should work
         * fine for most real world projects. */
        return true;
    }

    return false;
}

QDomElement Task::xmlElement( QDomDocument& doc, bool /* absId */ )
{
   QDomElement taskElem = doc.createElement( "Task" );
/*   QDomElement tempElem;

   QString idStr = getId();
   if( !absId )
      idStr = idStr.section( '.', -1 ); 

   taskElem.setAttribute( "Id", idStr );

   QDomText t;
   taskElem.appendChild( ReportXML::createXMLElem( doc, "Index", QString::number(getIndex()) ));
   taskElem.appendChild( ReportXML::createXMLElem( doc, "Name", getName() ));
   taskElem.appendChild( ReportXML::createXMLElem( doc, "ProjectID", projectId ));
   taskElem.appendChild( ReportXML::createXMLElem( doc, "Priority", QString::number(getPriority())));

   double cmplt = getCompletionDegree(0);
   taskElem.appendChild( ReportXML::createXMLElem( doc, "complete", QString::number(cmplt, 'f', 1) ));

   QString tType = "Milestone";
   if( !isMilestone() )
   {
      if( isContainer() )
     tType = "Container";
      else
     tType = "Task";

   }
   taskElem.appendChild( ReportXML::createXMLElem( doc, "Type", tType  ));

   CoreAttributes *parent = getParent();
   if( parent )
      taskElem.appendChild( ReportXML::ReportXML::createXMLElem( doc, "ParentTask", parent->getId()));

   if( !note.isEmpty())
      taskElem.appendChild( ReportXML::createXMLElem( doc, "Note", getNote()));
   if(!ref.isEmpty())
       taskElem.appendChild(ReportXML::createXMLElem(doc, "Reference",
                                                     ref));
   if(!refLabel.isEmpty())
       taskElem.appendChild(ReportXML::createXMLElem(doc, "ReferenceLabel",
                                                     refLabel));

    if (scenarios[0].minStart != 0)
    {
        tempElem = ReportXML::createXMLElem
            ( doc, "minStart", QString::number(scenarios[0].minStart));
        tempElem.setAttribute( "humanReadable",
                               time2ISO(scenarios[0].minStart));
        taskElem.appendChild( tempElem );
    }

    if (scenarios[0].maxStart != 0)
    {
        tempElem = ReportXML::createXMLElem
            (doc, "maxStart", QString::number(scenarios[0].maxStart));
        tempElem.setAttribute( "humanReadable",
                               time2ISO(scenarios[0].maxStart));
        taskElem.appendChild( tempElem );
    }

    if (scenarios[0].minEnd != 0)
    {
        tempElem = ReportXML::createXMLElem
            (doc, "minEnd", QString::number(scenarios[0].minEnd));
        tempElem.setAttribute( "humanReadable",
                               time2ISO(scenarios[0].minEnd));
        taskElem.appendChild( tempElem );
    }

    if (scenarios[0].maxEnd != 0)
    {
        tempElem = ReportXML::createXMLElem
            (doc, "maxEnd", QString::number(scenarios[0].maxEnd));
        tempElem.setAttribute( "humanReadable",
                               time2ISO(scenarios[0].maxEnd));
        taskElem.appendChild( tempElem );
    }
    if (project->getMaxScenarios() > 1)
    {
        tempElem = ReportXML::createXMLElem( doc, "actualStart",
                                             QString::number(scenarios[1].start));
        tempElem.setAttribute( "humanReadable",
                               time2ISO(scenarios[1].start));
        taskElem.appendChild( tempElem );

        tempElem = ReportXML::createXMLElem( doc, "actualEnd",
                                             QString::number(scenarios[1].end + 1));
        tempElem.setAttribute( "humanReadable",
                               time2ISO(scenarios[1].end + 1));
        taskElem.appendChild( tempElem );
    }

   tempElem = ReportXML::createXMLElem( doc, "planStart", QString::number( scenarios[0].start ));
   tempElem.setAttribute( "humanReadable", time2ISO( scenarios[0].start ));
   taskElem.appendChild( tempElem );

   tempElem = ReportXML::createXMLElem( doc, "planEnd",
                                        QString::number(scenarios[0].end + 1));
   tempElem.setAttribute( "humanReadable", time2ISO( scenarios[0].end + 1));
   taskElem.appendChild( tempElem );

   // Start- and Endbuffer 
   if(getStartBuffer(0) > 0.01)
   {
       // startbuffer exists 
       tempElem = ReportXML::createXMLElem
           (doc, "startBufferSize",
            QString::number(getStartBuffer(0)));
       taskElem.appendChild( tempElem );

       tempElem = ReportXML::createXMLElem
           (doc, "PlanStartBufferEnd",
            QString::number(getStartBufferEnd(0)));
       tempElem.setAttribute("humanReadable",
                             time2ISO(getStartBufferEnd(0)));
       taskElem.appendChild(tempElem);

       tempElem = ReportXML::createXMLElem
           (doc, "PlanStartBufferEnd",
            QString::number(getStartBufferEnd(0)));
       tempElem.setAttribute("humanReadable",
                             time2ISO(getStartBufferEnd(0)));
       taskElem.appendChild(tempElem);
   }

   if(getEndBuffer(0) > 0.01)
   {
       // startbuffer exists 
       tempElem = ReportXML::createXMLElem
           (doc, "EndBufferSize", QString::number(getEndBuffer(0)));
       taskElem.appendChild(tempElem);

       tempElem = ReportXML::createXMLElem
           (doc, "PlanEndBufferStart",
            QString::number(getEndBufferStart(0)));
       tempElem.setAttribute("humanReadable",
                             time2ISO(getEndBufferStart(0)));
       taskElem.appendChild(tempElem);

       tempElem = ReportXML::createXMLElem
           (doc, "PlanEndBufferStart",
            QString::number(getEndBufferStart(0)));
       tempElem.setAttribute("humanReadable",
                             time2ISO(getStartBufferEnd(0)));
       taskElem.appendChild(tempElem);
   }

   // Responsible persons 
   if( getResponsible() )
      taskElem.appendChild( getResponsible()->xmlIDElement( doc ));

   // Now start the subtasks 
   int cnt = 0;
   QDomElement subTaskElem = doc.createElement( "SubTasks" );
   for (Task* t = subFirst(); t != 0; t = subNext())
   {
      if( t != this )
      {
     QDomElement sTask = t->xmlElement( doc, false );
     subTaskElem.appendChild( sTask );
     cnt++;
      }
   }
   if( cnt > 0 )
      taskElem.appendChild( subTaskElem);

   // list of tasks by id which are previous 
   if( previous.count() > 0 )
   {
      for (TaskListIterator tli(previous); tli.hasNext();)
      {
          Task *t = static_cast<Task*>(tli.next());
            if( *tli != this )
            {
                taskElem.appendChild( ReportXML::createXMLElem( doc, "Previous",
                                                                t->getId()));
            }
      }
   }

   // list of tasks by id which follow 
   if( followers.count() > 0 )
   {
      for (TaskListIterator tli(followers); tli.hasNext();)
      {
          Task *t = static_cast<Task*>(tli.next());
            if( t != this )
            {
                taskElem.appendChild( ReportXML::createXMLElem( doc, "Follower",
                                                                (t)->getId()));
            }
      }
   }

   ** Allocations and Booked Resources
    *  With the following code, the task in XML contains simply a list of all Allocations
    *  wiht the ResourceID for which resource the allocation is. After that, there comes
    *  a list of all Resources, again having the Resource Id as key. That could be put
    *  in a hierarchy like
    *  <Resource Id="dev2" >Larry Bono
    *       <Income>1000</Income>
    *       <Allocation>
    *          <Load>100</Load>
    *          <Persistent>Yes</Persistent>
    *       </Allocation>
    *  </Resource>
    *
    *  But we do not ;-) to have full flexibility.
    *
    *
   // Allocations 
   if( allocations.count() > 0 )
   {
      QPtrList<Allocation> al(allocations);
      for (QListIterator<Allocation*> ali(al); ali.hasNext();)
      {
          Allocation *a = static_cast<Allocation*>(ali.next());
          taskElem.appendChild( a->xmlElement( doc ));
      }
   }

   // booked Resources 
   if( bookedResources.count() > 0 )
   {
       for (ResourceListIterator rli(bookedResources); *rli != 0; ++rli)
      {
     taskElem.appendChild( r->xmlIDElement( doc ));
      }
   }
*/
   return( taskElem );
}

bool
Task::isOrHasDescendantOnCriticalPath(int sc) const
{
    if (isOnCriticalPath(sc, false))
        return true;
    if (isContainer())
    {
        for (TaskListIterator tli = getSubListIterator(); tli.hasNext();) {
            Task *t = static_cast<Task*>(tli.next());
            if (t->isOrHasDescendantOnCriticalPath(sc))
                return true;
        }
    }
    return false;
}

} // namespace TJ

QDebug operator<<( QDebug dbg, const TJ::Task* t )
{
    if ( t == 0 ) {
        return dbg << (void*)t;
    }
    return operator<<( dbg, *t );
}

QDebug operator<<( QDebug dbg, const TJ::Task& t )
{
    dbg << (t.isMilestone() ? "Milestone[" : "Task[");
    dbg << t.getName() << (t.getScheduling() == TJ::Task::ASAP ? "(ASAP)" : "(ALAP)");
    if ( t.isSchedulingDone() ) {
        dbg << "Scheduled";
    } else if ( t.isReadyForScheduling() ) {
        dbg << "ReadyForScheduling";
    } else if ( t.isRunaway() ) {
        dbg << "Runaway";
    }
    dbg << "]";
    return dbg;
}
