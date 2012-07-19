/*
 * Project.cpp - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006
 * by Chris Schlaeger <cs@kde.org>
 * Copyright (c) 2011 Dag Andersen <danders@get2net.dk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#define tjDebug qDebug

#include "Project.h"

#include <stdlib.h>
#include <QList>
#include <QString>
#include <QStringList>

#include <KLocale>
#include <KDebug>

#include "TjMessageHandler.h"
#include "Scenario.h"
#include "Shift.h"
// #include "Account.h"
#include "Resource.h"
/*#include "HTMLTaskReport.h"
#include "HTMLResourceReport.h"
#include "HTMLAccountReport.h"
#include "HTMLWeeklyCalendar.h"
#include "HTMLStatusReport.h"
#include "CSVTaskReport.h"
#include "CSVResourceReport.h"
#include "CSVAccountReport.h"
#include "ExportReport.h"
#include "ReportXML.h"*/
#include "UsageLimits.h"
#include "CustomAttributeDefinition.h"

DebugController DebugCtrl;

namespace TJ
{

Project::Project() :
    QObject(),
    start(0),
    end(0),
    now(0),
    allowRedefinitions(false),
    weekStartsMonday(true),
    name(),
    version(),
    copyright(),
    customer(),
    timeZone(),
    timeFormat("%Y-%m-%d %H:%M"),
    shortTimeFormat("%H:%M"),
//     currency(),
//     currencyDigits(3),
//     numberFormat("-", "", ",", ".", 1),
//     currencyFormat("(", ")", ",", ".", 0),
    priority(500),
    minEffort(0.0),
    resourceLimits(0),
    rate(0.0),
    dailyWorkingHours(8.0),
    yearlyWorkingDays(260.714),
    workingHours(),
    scheduleGranularity(ONEHOUR),
    allowedFlags(),
    projectIDs(),
    currentId(),
    maxErrors(0),
//     journal(),
    vacationList(),
    scenarioList(),
    taskList(),
    resourceList(),
//     accountList(),
    shiftList(),
    originalTaskList(),
    originalResourceList(),
//     originalAccountList(),
    taskAttributes(),
    resourceAttributes(),
//     accountAttributes(),
//     xmlreport(0),
//     reports(),
//     interactiveReports(),
    sourceFiles(),
    breakFlag(false)
{
    qDebug()<<"Project:"<<this;
    /* Pick some reasonable initial number since we don't know the
     * project time frame yet. */
    initUtility(20000);

//     vacationList.setAutoDelete(true);
//     accountAttributes.setAutoDelete(true);
//     taskAttributes.setAutoDelete(true);
//     resourceAttributes.setAutoDelete(true);
//     reports.setAutoDelete(true);

    new Scenario(this, "plan", "Plan", 0);
    scenarioList.createIndex(true);
    scenarioList.createIndex(false);
    foreach(CoreAttributes *s, scenarioList) {
        qDebug()<<"Project:"<<static_cast<CoreAttributes*>(s)<<static_cast<CoreAttributes*>(s)->getName()<<static_cast<CoreAttributes*>(s)->getSequenceNo();
    }
    setNow(time(0));

    /* Initialize working hours with default values that match the Monday -
     * Friday 9 - 6 (with 1 hour lunch break) pattern used by many western
     * countries. */
    // Sunday
    workingHours[0] = new QList<Interval*>();
//     workingHours[0]->setAutoDelete(true);

    for (int i = 1; i < 6; ++i)
    {
        workingHours[i] = new QList<Interval*>();
//         workingHours[i]->setAutoDelete(true);
        workingHours[i]->append(new Interval(9 * ONEHOUR, 12 * ONEHOUR - 1));
        workingHours[i]->append(new Interval(13 * ONEHOUR, 18 * ONEHOUR - 1));
    }

    // Saturday
    workingHours[6] = new QList<Interval*>();
//     workingHours[6]->setAutoDelete(true);
}

Project::~Project()
{
    qDebug()<<"~Project:"<<this;
    taskList.deleteContents();
    resourceList.deleteContents();
    Resource::deleteStaticData();

//     accountList.deleteContents();
    shiftList.deleteContents();
    scenarioList.deleteContents();

    delete resourceLimits;

    // Remove support for 1.0 XML reports for next major release. */
//     delete xmlreport;

    for (int i = 0; i < 7; ++i) {
        while ( ! workingHours[i]->isEmpty() ) {
            delete workingHours[i]->takeFirst();
        }
        delete workingHours[i];
    }
    exitUtility();

    qDebug()<<"~Project:"<<this;
}

// void
// Project::addSourceFile(const QString& f)
// {
//     if (sourceFiles.find(f) == sourceFiles.end())
//         sourceFiles.append(f);
// }
// 
// QStringList
// Project::getSourceFiles() const
// {
//     return sourceFiles;
// }

void
Project::setProgressInfo(const QString& i)
{
    emit updateProgressInfo(i);
}

void
Project::setProgressBar(int i, int of)
{
    emit updateProgressBar(i, of);
}

bool
Project::setTimeZone(const QString& tz)
{
    if (!setTimezone(tz.toLocal8Bit()))
        return false;

    timeZone = tz;
    return true;
}

Scenario*
Project::getScenario(int sc) const
{
    return static_cast<Scenario*>(scenarioList.value(sc));
}

QString
Project::getScenarioName(int sc) const
{
    Scenario *s = getScenario(sc);
    return s ? s->getName() : QString();
}

QString
Project::getScenarioId(int sc) const
{
    Scenario *s = getScenario(sc);
    return s ? s->getId() : QString();
}

int
Project::getScenarioIndex(const QString& id) const
{
    return scenarioList.getIndex(id);
}

void
Project::setNow(time_t n)
{
    /* Align 'now' time to timing resolution. If the resolution is
     * changed later, this has to be done again. */
    now = (n / scheduleGranularity) * scheduleGranularity;
}

void
Project::setWorkingHours(int day, const QList< Interval* >& l)
{
    if (day < 0 || day > 6)
        qFatal("day out of range");
    delete workingHours[day];

    // Create a deep copy of the interval list.
    workingHours[day] = new QList<Interval*>;
//     workingHours[day]->setAutoDelete(true);
    foreach( Interval *i, l) {
        workingHours[day]->append(new Interval(*i));

    }
}

bool
Project::addId(const QString& id, bool changeCurrentId)
{
    if (projectIDs.indexOf(id) != -1)
        return false;
    else
        projectIDs.append(id);

    if (changeCurrentId)
        currentId = id;

    return true;
}

QString
Project::getIdIndex(const QString& i) const
{
    int idx;
    if ((idx = projectIDs.indexOf(i)) == -1)
        return QString("?");
    QString idxStr;
    do
    {
        idxStr = QChar('A' + idx % ('Z' - 'A')) + idxStr;
        idx /= 'Z' - 'A';
    } while (idx > 'Z' - 'A');

    return idxStr;
}

void
Project::addScenario(Scenario* s)
{
    scenarioList.append(s);

    /* This is not too efficient, but since there are usually only a few
     * scenarios in a project, this doesn't hurt too much. */
    scenarioList.createIndex(true);
    scenarioList.createIndex(false);
}

void
Project::deleteScenario(Scenario* s)
{
    if (scenarioList.contains(s)) {
        scenarioList.removeAt(scenarioList.indexOf(s));
    }
}

void
Project::setResourceLimits(UsageLimits* l)
{
    if (resourceLimits)
        delete resourceLimits;
    resourceLimits = l;
}

void
Project::addTask(Task* t)
{
    taskList.append(t);
}

void
Project::deleteTask(Task* t)
{
    if (taskList.contains(t)) {
        taskList.removeAt(taskList.indexOf(t));
    }
}

bool
Project::addTaskAttribute(const QString& id, CustomAttributeDefinition* cad)
{
    if (taskAttributes.contains(id))
        return false;

    taskAttributes.insert(id, cad);
    return true;
}

const CustomAttributeDefinition*
Project::getTaskAttribute(const QString& id) const
{
    return taskAttributes[id];
}

void
Project::addShift(Shift* s)
{
    shiftList.append(s);
}

void
Project::deleteShift(Shift* s)
{
    if (shiftList.contains(s)) {
        shiftList.removeAt(shiftList.indexOf(s));
    }
}

void
Project::addResource(Resource* r)
{
    qDebug()<<"Project::addResource:"<<r<<resourceList;
    resourceList.append(r);
}

void
Project::deleteResource(Resource* r)
{
    if (resourceList.contains(r)) {
        resourceList.removeAt(resourceList.indexOf(r));
    }
}

bool
Project::addResourceAttribute(const QString& id,
                              CustomAttributeDefinition* cad)
{
    if (resourceAttributes.contains(id))
        return false;

    resourceAttributes.insert(id, cad);
    return true;
}

const CustomAttributeDefinition*
Project::getResourceAttribute(const QString& id) const
{
    return resourceAttributes[id];
}

// void
// Project::addAccount(Account* a)
// {
//     accountList.append(a);
// }
// 
// void
// Project::deleteAccount(Account* a)
// {
//     if (accountList.contains(a)) {
//         accountList.removeAt(accountList.indexOf(a);
// }
// 
// bool
// Project::addAccountAttribute(const QString& id,
//                               CustomAttributeDefinition* cad)
// {
//     if (accountAttributes.find(id))
//         return false;
// 
//     accountAttributes.insert(id, cad);
//     return true;
// }

// const CustomAttributeDefinition*
// Project::getAccountAttribute(const QString& id) const
// {
//     return accountAttributes[id];
// }

bool
Project::isWorkingDay(time_t wd) const
{
    return !(workingHours[dayOfWeek(wd, false)]->isEmpty() ||
             isVacation(wd));
}

bool
Project::isWorkingTime(time_t wd) const
{
    if (isVacation(wd))
        return false;

    int dow = dayOfWeek(wd, false);
    foreach (Interval *i, *getWorkingHours(dow)) {
        if (i->contains(secondsOfDay(wd)))
            return true;
    }
    return false;
}

bool
Project::isWorkingTime(const Interval& iv) const
{
    if (isVacation(iv.getStart()))
        return false;

    int dow = dayOfWeek(iv.getStart(), false);
    foreach (Interval *i, *(workingHours[dow]))
    {
        if (i->contains(Interval(secondsOfDay(iv.getStart()),
                                  secondsOfDay(iv.getEnd()))))
            return true;
    }
    return false;
}

int
Project::calcWorkingDays(const Interval& iv) const
{
    int workingDays = 0;

    for (time_t s = midnight(iv.getStart()); s <= iv.getEnd();
         s = sameTimeNextDay(s))
        if (isWorkingDay(s))
            workingDays++;

    return workingDays;
}

double
Project::convertToDailyLoad(long secs) const
{
    return ((double) secs / (dailyWorkingHours * ONEHOUR));
}

// void
// Project::addJournalEntry(JournalEntry* entry)
// {
//     journal.inSort(entry);
// }

// Journal::Iterator
// Project::getJournalIterator() const
// {
//     return Journal::Iterator(journal);
// }

bool
Project::pass2(bool noDepCheck)
{
    int oldErrors = TJMH.getErrors();

    if (taskList.isEmpty())
    {
        TJMH.errorMessage(i18nc("@info/plain", "The project does not contain any tasks."));
        return false;
    }
    qDebug()<<"pass2 task info:";
    foreach ( CoreAttributes *a, taskList ) {
        Task *t = static_cast<Task*>( a );
        qDebug()<<t->getName()<<t->getDuration( 0 )<<t->getPrecedes()<<t->getDepends();
    }
    QMap<QString, Task*> idHash;

    /* The optimum size for the localtime hash is twice the number of time
     * slots times 2 (because of timeslot and timeslot - 1s). */
    initUtility(4 * ((end - start) / scheduleGranularity));

    // Generate sequence numbers for all lists.
    taskList.createIndex(true);
    resourceList.createIndex(true);
//     accountList.createIndex(true);
    shiftList.createIndex(true);

    // Initialize random generator.
    srand((int) start);

    // Create hash to map task IDs to pointers.
    foreach (CoreAttributes *t, taskList)
    {
        idHash.insert(static_cast<Task*>(t)->getId(), static_cast<Task*>(t));
    }
    // Create cross links from dependency lists.
    foreach (CoreAttributes *t, taskList)
        static_cast<Task*>(t)->xRef(idHash);

    foreach (CoreAttributes *t, taskList)
    {
        // Set dates according to implicit dependencies
        static_cast<Task*>(t)->implicitXRef();

        // Sort allocations properly
        static_cast<Task*>(t)->sortAllocations();

        // Save so far booked resources as specified resources
        static_cast<Task*>(t)->saveSpecifiedBookedResources();
    }

    // Save a copy of all manually booked resources.
    foreach (CoreAttributes *r, resourceList)
        static_cast<Resource*>(r)->saveSpecifiedBookings();

    /* Now we can copy the missing values from the plan scenario to the other
     * scenarios. */
    if (scenarioList.count() > 1)
    {
        kWarning()<<"TODO";
/*        for (ScenarioListIterator sli(scenarioList[0]->getSubListIterator());
             *sli; ++sli)
            overlayScenario(0, (*sli)->getSequenceNo() - 1);*/
    }

    // Now check that all tasks have sufficient data to be scheduled.
    setProgressInfo(QString("Checking scheduling data..."));
    bool error = false;
    foreach (CoreAttributes *s, scenarioList) {
        foreach (CoreAttributes *t, taskList) {
            if (!static_cast<Task*>(t)->preScheduleOk(static_cast<Scenario*>(s)->getSequenceNo() - 1))
            {
                error = true;
            }
        }
    }
    if (error)
        return false;

    if (!noDepCheck)
    {
        setProgressInfo(QString("Searching for dependency loops ..."));
        if (DEBUGPS(1))
            tjDebug("Searching for dependency loops ...");
        // Check all tasks for dependency loops.
        LDIList chkedTaskList;
        foreach (CoreAttributes *t, taskList) {
            if (static_cast<Task*>(t)->loopDetector(chkedTaskList))
                return false;
        }

        setProgressInfo(QString("Searching for underspecified tasks ..."));
        if (DEBUGPS(1))
            tjDebug("Searching for underspecified tasks ...");
        foreach (CoreAttributes *s, scenarioList) {
            foreach (CoreAttributes *t, taskList) {
                if (!static_cast<Task*>(t)->checkDetermination(static_cast<Scenario*>(s)->getSequenceNo() - 1))
                    error = true;
            }
        }

        if (error)
            return false;
    }
    TJ::TaskList starts;
    TJ::TaskList ends;
    QStringList tl;
    foreach ( TJ::CoreAttributes *t, taskList ) {
        tl << t->getName();
        if ( ! static_cast<TJ::Task*>(t)->hasPrevious() ) {
            starts << static_cast<TJ::Task*>(t);
            tl << "(s)";
        }
        if ( ! static_cast<TJ::Task*>(t)->hasFollowers() ) {
            ends << static_cast<TJ::Task*>(t);
            tl << "(e)";
        }
    }
    tl.clear();
    foreach ( TJ::CoreAttributes *t, taskList ) {
        tl << t->getName();
        if ( ! static_cast<TJ::Task*>(t)->hasPrevious() ) {
            starts << static_cast<TJ::Task*>(t);
            tl << "(s)";
        }
        if ( ! static_cast<TJ::Task*>(t)->hasFollowers() ) {
            ends << static_cast<TJ::Task*>(t);
            tl << "(e)";
        }
    }
    if (DEBUGPS(2)) {
        qDebug()<<"Tasks:"<<tl;
        qDebug()<<"Depends/precedes: -------------------";
        tl.clear();
        foreach ( TJ::CoreAttributes *t, taskList ) {
            tl << t->getName() + ( static_cast<TJ::Task*>(t)->getScheduling() == TJ::Task::ASAP ? " (ASAP)" : " (ALAP)" ) + " depends: ";
            for ( QListIterator<TJ::TaskDependency*> it = static_cast<TJ::Task*>(t)->getDependsIterator(); it.hasNext(); ) {
                const TJ::Task *a = it.next()->getTaskRef();
                QString s = a->getName() + ( a->getScheduling() == TJ::Task::ASAP ? " (ASAP)" : " (ALAP)" );
                tl << s;
            }
            qDebug()<<tl; tl.clear();
            tl << t->getName() + ( static_cast<TJ::Task*>(t)->getScheduling() == TJ::Task::ASAP ? " (ASAP)" : " (ALAP)" ) + " precedes: ";
            for ( QListIterator<TJ::TaskDependency*> it = static_cast<TJ::Task*>(t)->getPrecedesIterator(); it.hasNext(); ) {
                const TJ::Task *a = it.next()->getTaskRef();
                QString s = a->getName() + ( a->getScheduling() == TJ::Task::ASAP ? " (ASAP)" : " (ALAP)" );
                tl << s;
            }
            qDebug()<<tl; tl.clear();
        }
        qDebug()<<"Followers/previous: -------------------";
        tl.clear();
        foreach ( TJ::CoreAttributes *t, taskList ) {
            tl << t->getName() + ( static_cast<TJ::Task*>(t)->getScheduling() == TJ::Task::ASAP ? " (ASAP)" : " (ALAP)" ) + " followers: ";
            for ( TJ::TaskListIterator it = static_cast<TJ::Task*>(t)->getFollowersIterator(); it.hasNext(); ) {
                const TJ::Task *a = static_cast<TJ::Task*>( it.next() );
                QString s = a->getName() + ( a->getScheduling() == TJ::Task::ASAP ? " (ASAP)" : " (ALAP)" );
                tl << s;
            }
            qDebug()<<tl; tl.clear();
            tl << t->getName() + ( static_cast<TJ::Task*>(t)->getScheduling() == TJ::Task::ASAP ? " (ASAP)" : " (ALAP)" ) + " previous: ";
            for ( TJ::TaskListIterator it = static_cast<TJ::Task*>(t)->getPreviousIterator(); it.hasNext(); ) {
                const TJ::Task *a = static_cast<TJ::Task*>( it.next() );
                QString s = a->getName() + ( a->getScheduling() == TJ::Task::ASAP ? " (ASAP)" : " (ALAP)" );
                tl << s;
            }
            qDebug()<<tl; tl.clear();
        }
        qDebug()<<"Successors/predecessors: -------------------";
        tl.clear();
        foreach ( TJ::CoreAttributes *c, taskList ) {
            tl << c->getName() + ( static_cast<TJ::Task*>(c)->getScheduling() == TJ::Task::ASAP ? " (ASAP)" : " (ALAP)" ) + " successors: ";
            foreach ( TJ::CoreAttributes *t, static_cast<TJ::Task*>(c)->getSuccessors() ) {
                TJ::Task *a = static_cast<TJ::Task*>(t);
                QString s = a->getName() + ( a->getScheduling() == TJ::Task::ASAP ? " (ASAP)" : " (ALAP)" );
                tl << s;
            }
            qDebug()<<tl; tl.clear();
            tl << c->getName() + ( static_cast<TJ::Task*>(c)->getScheduling() == TJ::Task::ASAP ? " (ASAP)" : " (ALAP)" ) + " predecessors: ";
            foreach ( TJ::CoreAttributes *t, static_cast<TJ::Task*>(c)->getPredecessors() ) {
                TJ::Task *a = static_cast<TJ::Task*>(t);
                QString s = a->getName() + ( a->getScheduling() == TJ::Task::ASAP ? " (ASAP)" : " (ALAP)" );
                tl << s;
            }
            qDebug()<<tl; tl.clear();
        }
    }
    return TJMH.getErrors() == oldErrors;
}

bool
Project::scheduleScenario(Scenario* sc)
{
    int oldErrors = TJMH.getErrors();

//     setProgressInfo(QString("Scheduling scenario %1...").arg(sc->getName()));

    int scIdx = sc->getSequenceNo() - 1;
    prepareScenario(scIdx);

    if (!schedule(scIdx))
    {
        if (DEBUGPS(2))
            tjDebug()<<"Scheduling errors in scenario: "<<(sc->getId());
        if (breakFlag)
            return false;
    }
    finishScenario(scIdx);

    foreach (CoreAttributes *r, resourceList)
    {
        if (!static_cast<Resource*>(r)->bookingsOk(scIdx))
            break;
    }

    return TJMH.getErrors() == oldErrors;
}

void
Project::completeBuffersAndIndices()
{
    foreach (CoreAttributes *t, taskList) {
        static_cast<Task*>(t)->computeBuffers();
    }
    /* Create indices for all lists according to their default sorting
     * criteria. */
    taskList.createIndex();
    resourceList.createIndex();
//     accountList.createIndex();
    shiftList.createIndex();
}

bool
Project::scheduleAllScenarios()
{
    bool schedulingOk = true;
    foreach (CoreAttributes *s, scenarioList) {
        if (static_cast<Scenario*>(s)->getEnabled())
        {
            if (DEBUGPS(1))
                tjDebug()<<"Scheduling scenario:"<<static_cast<Scenario*>(s)->getId();

            if (!scheduleScenario(static_cast<Scenario*>(s)))
                schedulingOk = false;
            if (breakFlag)
                return false;
        }
    }

    completeBuffersAndIndices();

    return schedulingOk;
}

void
Project::overlayScenario(int base, int sc)
{
    foreach (CoreAttributes *t, taskList) {
        static_cast<Task*>(t)->overlayScenario(base, sc);
    }
    foreach (CoreAttributes *s, scenarioList[sc]->getSubList()) {
        overlayScenario(sc, static_cast<Scenario*>(s)->getSequenceNo() - 1);
    }
}

void
Project::prepareScenario(int sc)
{
    foreach (CoreAttributes *r, resourceList) {
        static_cast<Resource*>(r)->prepareScenario(sc);
    }
    foreach (CoreAttributes *t, taskList) {
        static_cast<Task*>(t)->prepareScenario(sc);
    }

    /* First we compute the criticalness of the individual task without their
     * dependency context. */
    foreach (CoreAttributes *t, taskList) {
        static_cast<Task*>(t)->computeCriticalness(sc);
    }
    /* Then we compute the path criticalness that represents the criticalness
     * of a task taking their dependency context into account. */
    foreach (CoreAttributes *t, taskList) {
        static_cast<Task*>(t)->computePathCriticalness(sc);
    }

    foreach (CoreAttributes *t, taskList) {
        static_cast<Task*>(t)->propagateInitialValues(sc);
    }

    if (DEBUGTS(4))
    {
        tjDebug("Allocation probabilities for the resources:");
        foreach (CoreAttributes *r, resourceList) {
            qDebug()<<QString("Resource %1: %2%")
                   .arg(static_cast<Resource*>(r)->getName())
                   .arg(static_cast<Resource*>(r)->getAllocationProbability(sc));
        }
        tjDebug("Criticalnesses of the tasks with respect to resource "
               "availability:");
        foreach (CoreAttributes *t, taskList) {
            qDebug()<<QString("Task %1: %2 %3").arg(static_cast<Task*>(t)->getName())
                   .arg(static_cast<Task*>(t)->getCriticalness(sc))
                   .arg(static_cast<Task*>(t)->getPathCriticalness(sc));
        }
    }
}

void
Project::finishScenario(int sc)
{
    foreach (CoreAttributes *r, resourceList) {
        static_cast<Resource*>(r)->finishScenario(sc);
    }
    foreach (CoreAttributes *t, taskList) {
        static_cast<Task*>(t)->finishScenario(sc);
    }
#if 0
    /* We need to have finished the scenario for all tasks before we can
     * calculate the completion degree. */
    foreach (CoreAttributes *t, taskList) {
        static_cast<Task*>(t)->calcCompletionDegree(sc);
    }
#endif
    /* If the user has not set the minSlackRate to 0 we look for critical
     * pathes. */
    if (getScenario(sc)->getMinSlackRate() > 0.0)
    {
        setProgressInfo(QString("Computing critical pathes..."));
        /* The critical path detector needs to know the end of the last task.
         * So we have to find this out first. */
        time_t maxEnd = 0;
        foreach (CoreAttributes *t, taskList) {
            if (maxEnd < static_cast<Task*>(t)->getEnd(sc))
                maxEnd = static_cast<Task*>(t)->getEnd(sc);
        }
        foreach (CoreAttributes *t, taskList) {
            static_cast<Task*>(t)->checkAndMarkCriticalPath
                (sc, getScenario(sc)->getMinSlackRate(), maxEnd);
        }
    }
}

TaskList Project::tasksReadyToBeScheduled(int sc, const TaskList& allLeafTasks)
{
    TaskList workItems;
    foreach (CoreAttributes *t, allLeafTasks) {
        if (static_cast<Task*>(t)->isReadyForScheduling())
            workItems.append(static_cast<Task*>(t));
    }
    if ( workItems.isEmpty() ) {
        foreach (CoreAttributes *t, allLeafTasks) {
            if (!static_cast<Task*>(t)->isSchedulingDone() && !static_cast<Task*>(t)->isReadyForScheduling()) {
                TJMH.debugMessage("Not ready to be scheduled", t);
            }
        }
        foreach (CoreAttributes *c, allLeafTasks) {
            Task *t = static_cast<Task*>(c);
            if (!t->isSchedulingDone() /*&& !t->isRunaway()*/) {
                if (t->getScheduling() == Task::ASAP) {
                    time_t es = t->earliestStart(sc);
                    //qDebug()<<"schedule rest: earliest start"<<time2ISO(es)<<time2ISO(time_t(1));
                    if (es > 1) { // NOTE: es is 1 if predecessor is not scheduled!
                        t->propagateStart(sc, es);
                    } else if (t->hasAlapPredecessor()) {
                        time_t le = t->latestEnd(sc);
                        if (le > time_t(0) ) {
                            t->setScheduling(Task::ALAP);
                            t->propagateEnd(sc, le );
                        }// else qDebug()<<"schedule rest: no end time"<<t;
                    } //else qDebug()<<"schedule rest: no start time"<<t;
                } else {
                    time_t le = t->latestEnd(sc);
                    if (le > time_t(0) ) {
                        t->propagateEnd(sc, le );
                    } else qDebug()<<"ALAP schedule rest: no end time"<<t;
                }
                if (t->isReadyForScheduling()) {
                    workItems.append(t);
                } else qDebug()<<"Schedule rest: not ready"<<t;
            }
            if (workItems.isEmpty()) {
                TaskList lst;
                foreach (CoreAttributes *c, allLeafTasks) {
                    Task *t = static_cast<Task*>(c);
                    if (!t->isSchedulingDone()) {
                        lst << t;
                    }
                }
                if (lst.isEmpty()) {
                    return workItems; // finished
                }
                if (DEBUGPS(5)) {
                    if (!lst.isEmpty()) {
                        qDebug()<<"These tasks are still not ready to be scheduled:"<<allLeafTasks;
                    }
                }
            }
        }
    }
/*    if ( workItems.isEmpty() && getTask("TJ::StartJob")->getScheduling() == Task::ASAP) {
        foreach (CoreAttributes *c, allLeafTasks) {
        }
    }                                                                                                                                                                                                                                                                                                                                                                                               */
    if ( workItems.isEmpty() && getTask("TJ::StartJob")->getScheduling() == Task::ALAP) {
        qDebug()<<"tasksReadyToSchedule:"<<"backward, try really hard";
        foreach (CoreAttributes *c, allLeafTasks) {
            Task *task = static_cast<Task*>(c);
            if (!task->isSchedulingDone()) {
                continue;
            }
            qDebug()<<"tasksReadyToSchedule:"<<"scheduled task:"<<task<<time2ISO(task->start)<<time2ISO(task->end);
            Task *predecessor = 0;
            long gapLength = 0;
            long gapDuration = 0;
            foreach (CoreAttributes *c, task->previous) {
                Task *t = static_cast<Task*>(c);
                if (t->isSchedulingDone()) {
                    continue;
                }
                // get the dependency/longest gap
                foreach (TaskDependency *d, t->precedes) {
                    if (d->getTaskRef() == task) {
                        predecessor = t;
                        gapLength = qMax(gapLength, d->getGapLength(sc));
                        gapDuration = qMax(gapDuration, d->getGapDuration(sc));
                    }
                }
            }
            if ( predecessor == 0 ) {
                continue;
            }
            time_t potentialDate = task->start - 1;
            time_t dateBeforeLengthGap;
            for (dateBeforeLengthGap = potentialDate; gapLength > 0 && dateBeforeLengthGap >= start; dateBeforeLengthGap -= getScheduleGranularity()) {
                if (isWorkingTime(dateBeforeLengthGap)) {
                    gapLength -= getScheduleGranularity();
                }
                if (dateBeforeLengthGap < potentialDate - gapDuration) {
                    potentialDate = dateBeforeLengthGap;
                } else {
                    potentialDate -= gapDuration;
                }
            }
            qDebug()<<"tasksReadyToSchedule:"<<"schedule predecessor:"<<predecessor<<time2ISO(potentialDate);
            predecessor->propagateEnd(sc, potentialDate);
            workItems << predecessor;
            break;
        }
    }
    return workItems;
}

bool
Project::schedule(int sc)
{
    int oldErrors = TJMH.getErrors();
    int maxProgress = 0;

    // The scheduling function only cares about leaf tasks. Container tasks
    // are scheduled automatically when all their childern are scheduled. So
    // we create a task list that only contains leaf tasks.
    TaskList allLeafTasks;
    foreach (CoreAttributes *t, taskList) {
        if (!static_cast<Task*>(t)->hasSubs()) {
            allLeafTasks.append(static_cast<Task*>(t));
//             TJMH.debugMessage("Leaf task", t);
        }
    }

    allLeafTasks.setSorting(CoreAttributesList::PrioDown, 0);
    allLeafTasks.setSorting(CoreAttributesList::PathCriticalnessDown, 1);
    allLeafTasks.setSorting(CoreAttributesList::SequenceUp, 2);
    allLeafTasks.sort();
    maxProgress = allLeafTasks.count();
    int sortedTasks = 0;
    foreach (CoreAttributes *t, allLeafTasks) {
        if (static_cast<Task*>(t)->isSchedulingDone())
            sortedTasks++;
    }
    /* The workItems list contains all tasks that are ready to be scheduled at
     * any given iteration. When a tasks has been scheduled completely, this
     * list needs to be updated again as some tasks may now have become ready
     * to be scheduled. */
    TaskList workItems = tasksReadyToBeScheduled(sc, allLeafTasks);

    bool done;
    /* While the scheduling process progresses, the list contains more and
     * more scheduled tasks. We use the cleanupTimer to remove those in
     * certain intervals. As we potentially have already completed tasks in
     * the list when we start, we initialize the timer with a very large
     * number so the first round of cleanup is done right after the first
     * scheduling pass. */
    breakFlag = false;
    bool runAwayFound = false;
    do
    {
        done = true;
        time_t slot = 0;
        int priority = 0;
        double pathCriticalness = 0.0;
        Task::SchedulingInfo schedulingInfo = Task::ASAP;

        /* The task list is sorted by priority. The priority decreases towards
         * the end of the list. We iterate through the list and look for a
         * task that can be scheduled. It the determines the time slot that
         * will be scheduled during this run for all subsequent tasks as well.
         */
        foreach (CoreAttributes *t, workItems) {
//             TJMH.debugMessage(QString("'%1' schedule for slot: %2, (%3 -%4)").arg(static_cast<Task*>(t)->getName()).arg(time2ISO(slot)).arg(time2ISO(start)).arg(time2ISO(end)));
            if (slot == 0)
            {
                /* No time slot has been set yet. Check if this task can be
                 * scheduled and provides a suggestion. */
                slot = static_cast<Task*>(t)->nextSlot(scheduleGranularity);
//                 TJMH.debugMessage(QString("'%1' first slot: %2, (%3 -%4)").arg(static_cast<Task*>(t)->getName()).arg(time2ISO(slot)).arg(time2ISO(start)).arg(time2ISO(end)), t);
                /* If not, try the next task. */
                if (slot == 0)
                    continue;
                priority = static_cast<Task*>(t)->getPriority();
                pathCriticalness = static_cast<Task*>(t)->getPathCriticalness(sc);
                schedulingInfo = static_cast<Task*>(t)->getScheduling();

                if (DEBUGPS(4))
                    qDebug()<<QString("Task '%1' (Prio %2, Direction: %3) requests slot %4")
                           .arg(static_cast<Task*>(t)->getName()).arg(static_cast<Task*>(t)->getPriority())
                           .arg(static_cast<Task*>(t)->getScheduling())
                           .arg(time2ISO(slot));
                /* If the task wants a time slot outside of the project time
                 * frame, we flag this task as a runaway and go to the next
                 * task. */
                if (slot < start ||
                    slot > (end - (time_t) scheduleGranularity + 1))
                {
                    static_cast<Task*>(t)->setRunaway();
                    runAwayFound = true;
                    slot = 0;
                    continue;
                }
            }
            done = false;
            /* Each task has a scheduling direction (forward or backward)
             * depending on it's constrains. The task with the highest
             * priority/pathCriticalness determines the time slot and hence the
             * scheduling direction. Since tasks that have the other direction
             * cannot the scheduled then, we have to stop this run as soon as
             * we hit a task that runs in the other direction. If we would not
             * do this, tasks with lower priority/pathCriticalness  would grab
             * resources form tasks with higher priority. */
            if (static_cast<Task*>(t)->getScheduling() != schedulingInfo &&
                !static_cast<Task*>(t)->isMilestone())
            {
                if (DEBUGPS(4))
                    qDebug()<<QString("Changing scheduling direction to %1 due to task '%2'")
                            .arg(static_cast<Task*>(t)->getScheduling())
                            .arg(static_cast<Task*>(t)->getName());
                break;
            }
            /* We must avoid that lower priority tasks get resources even
             * though there are higher priority tasks that are ready to be
             * scheduled but have a non-adjacent last slot. If two tasks have
             * the same priority the pathCriticalness is being used. */
            if (static_cast<Task*>(t)->getPriority() < priority ||
                (static_cast<Task*>(t)->getPriority() == priority &&
                 static_cast<Task*>(t)->getPathCriticalness(sc) < pathCriticalness))
                break;

            // Schedule this task for the current time slot.
            if (static_cast<Task*>(t)->schedule(sc, slot, scheduleGranularity))
            {
                workItems = tasksReadyToBeScheduled(sc, allLeafTasks);
                int oldSortedTasks = sortedTasks;
                sortedTasks = 0;
                foreach (CoreAttributes *t, allLeafTasks) {
                    if (static_cast<Task*>(t)->isSchedulingDone())
                        sortedTasks++;
                }
                // Update the progress bar after every 10th completed tasks.
                if (oldSortedTasks / 10 != sortedTasks / 10)
                {
                    setProgressBar(100 * ( (double)sortedTasks / maxProgress ), sortedTasks);
                    setProgressInfo(QString("Scheduling scenario %1 at %2")
                         .arg(getScenarioId(sc)).arg(time2tjp(slot)));
                }
            }
        }
    } while (!done && !breakFlag);

    if (breakFlag)
    {
        setProgressInfo("");
        setProgressBar(0, 0);
        TJMH.infoMessage(i18nc("@info/plain", "Scheduling aborted on user request"));
        return false;
    }
    if (runAwayFound) {
        foreach (CoreAttributes *t, taskList) {
            if (static_cast<Task*>(t)->isRunaway()) {
                if (static_cast<Task*>(t)->getScheduling() == Task::ASAP) {
                    TJMH.errorMessage(i18nc("@info/plain", "Task '%1' cannot meet the projects target finish time. Try using a later project end date.", t->getName()));
                } else {
                    TJMH.errorMessage(i18nc("@info/plain", "Task '%1' cannot meet the projetcs target start time. Try using an earlier project start date.", t->getName()));
                }
            }
        }
    }
    if (TJMH.getErrors() == oldErrors)
        setProgressBar(100, 100);

    /* Check that the resulting schedule meets all the requirements that the
     * user has specified. */
    setProgressInfo(QString("Checking schedule of scenario %1")
                    .arg(getScenarioId(sc)));
    checkSchedule(sc);

    return TJMH.getErrors() == oldErrors;
}

void
Project::breakScheduling()
{
    breakFlag = true;
}

bool
Project::checkSchedule(int sc) const
{
    int oldErrors = TJMH.getErrors();
    foreach (CoreAttributes *t, taskList) {
        /* Only check top-level tasks, since they recursively check their sub
         * tasks. */
        if (static_cast<Task*>(t)->getParent() == 0)
            static_cast<Task*>(t)->scheduleOk(sc);
        if (maxErrors > 0 && TJMH.getErrors() >= maxErrors)
        {
            TJMH.errorMessage(i18nc("@info/plain", "Too many errors. Giving up."));
            return false;
        }
    }

    return TJMH.getErrors() == oldErrors;
}

// Report*
// Project::getReport(uint idx) const
// {
//     QPtrListIterator<Report> it(reports);
//     for (uint i = 0; *it && i < idx; ++it, ++i)
//         ;
//     return *it;
// }
// 
// /*QPtr*/ListIterator<Report>
// Project::getReportListIterator() const
// {
//     return QPtrListIterator<Report>(reports);
// }
// 
// bool
// Project::generateReports() const
// {
//     // Generate reports
//     int errors = 0;
//     for (QPtrListIterator<Report> ri(reports); *ri != 0; ++ri)
//     {
//         // We generate all but Qt*Reports. Those are for the GUI version.
//         if (strncmp((*ri)->getType(), "Qt", 2) != 0)
//         {
//             if (DEBUGPS(1))
//                 tjDebug(QString("Generating report '%1' ...")
//                        .arg((*ri)->getFileName()));
// 
//             if (!(*ri)->generate())
//                 errors++;
//         }
//     }
// 
//     generateXMLReport();
// 
//     return errors == 0;
// }
// 
// bool Project::generateXMLReport() const
// {
//     if ( xmlreport )
//         return xmlreport->generate();
//     else
//         return false;
// }

} // namespace TJ

#include "Project.moc"
