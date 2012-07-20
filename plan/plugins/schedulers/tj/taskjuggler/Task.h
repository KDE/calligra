/*
 * task.h - TaskJuggler
 *
 * Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006
 * Chris Schlaeger <cs@kde.org>
 * Copyright (c) 2011 by Dag Andersen <danders@get2net.dk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#ifndef _Task_h_
#define _Task_h_

#include "kplatotj_export.h"

// #include <config.h>

#include <stdarg.h>

#include <QDomDocument>

#include "debug.h"
#include "TaskList.h"
#include "TaskScenario.h"
#include "ShiftSelectionList.h"
#include "LoopDetectorInfo.h"
#include "TaskDependency.h"
// #include "Journal.h"

class QDomElement;
class QDomDocument;

namespace TJ
{

class Project;
class Resource;
// class Account;
class Shift;
class TaskList;
class Allocation;
class Interval;
class UsageLimits;

/**
 * This class stores all task related information and provides methods to
 * manipulte them. It provides fundamental functions like the scheduler.
 *
 * @short The class that holds all task related information.
 * @see Resource
 * @see CoreAttributes
 * @author Chris Schlaeger <cs@kde.org>
 */
class KPLATOTJ_EXPORT Task : public CoreAttributes
{
    friend int TaskList::compareItemsLevel(CoreAttributes*, CoreAttributes*,
                                           int);

public:
    Task(Project* prj, const QString& id_, const QString& n, Task* p,
         const QString& f, int l);
    virtual ~Task();

    virtual CAType getType() const { return CA_Task; }

    Task* getParent() const { return static_cast<Task*>(parent); }

    TaskListIterator getSubListIterator() const
    {
        return TaskListIterator(*sub);
    }

    enum SchedulingInfo { ASAP, ALAP };

    void inheritValues();

    void setProjectId(const QString& i) { projectId = i; }
    const QString& getProjectId() const { return projectId; }

    void setNote(const QString& d) { note = d; }
    const QString& getNote() const { return note; }

//     void addJournalEntry(JournalEntry* entry);

//     bool hasJournal() const { return !journal.isEmpty(); }

//     Journal::Iterator getJournalIterator() const;

    void setReference(const QString& r, const QString& l)
    {
        ref = r;
        refLabel = l;
    }
    const QString& getReference() const { return ref; }
    const QString& getReferenceLabel() const { return refLabel; }

    void setScheduling(SchedulingInfo si) { scheduling = si; }
    SchedulingInfo getScheduling() const { return scheduling; }

    void setPriority(int p) { priority = p; }
    int getPriority() const { return priority; }

    void setResponsible(Resource* r) { responsible = r; }
    Resource* getResponsible() const { return responsible; }

    void setMilestone(bool ms = true) { milestone = ms; }
    bool isMilestone() const { return milestone; }

// // //     void setAccount(Account* a) { account = a; }
// // //     Account* getAccount() const { return account; }

    TaskDependency* addDepends(const QString& id);
    QList<TaskDependency*> getDepends() const
    {
        return depends;
    }
    QListIterator<TaskDependency*> getDependsIterator() const
    {
        return QListIterator<TaskDependency*>(depends);
    }
    TaskDependency* addPrecedes(const QString& id);
    QList<TaskDependency*> getPrecedes() const
    {
        return precedes;
    }
    QListIterator<TaskDependency*> getPrecedesIterator() const
    {
        return QListIterator<TaskDependency*>(precedes);
    }
    bool addShift(const Interval& i, Shift* s);

    void addAllocation(Allocation* a) { allocations.append(a); }
    void purgeAllocations() { allocations.clear(); }
    QListIterator<Allocation*> getAllocationIterator() const
    {
        return QListIterator<Allocation*>(allocations);
    }

    TaskListIterator getPreviousIterator() const
    {
        return TaskListIterator(previous);
    }
    bool hasPrevious() const { return !previous.isEmpty(); }

    TaskListIterator getFollowersIterator() const
    {
        return TaskListIterator(followers);
    }
    bool hasFollowers() const { return !followers.isEmpty(); }

    bool hasPrevious(const Task* t) { return previous.indexOf(const_cast<Task*>(t)) != -1; }
    bool hasFollower(const Task* t) { return followers.indexOf(const_cast<Task*>(t)) != -1; }

    // The following group of functions operates only on scenario variables.
    void setSpecifiedStart(int sc, time_t s)
    {
        scenarios[sc].specifiedStart = s;
        qDebug()<<"Task::setSpecifiedStart:"<<id<<":"<<sc<<s<<scenarios[sc].specifiedStart;
    }
    void setStart(int sc, time_t s) { scenarios[sc].start = s; }
    time_t getStart(int sc) const { return scenarios[sc].start; }

    void setSpecifiedEnd(int sc, time_t s)
    {
        scenarios[sc].specifiedEnd = s;
    }
    void setEnd(int sc, time_t s) { scenarios[sc].end = s; }
    time_t getEnd(int sc) const { return scenarios[sc].end; }

    void setSpecifiedPeriod(int sc, const Interval& iv)
    {
        scenarios[sc].specifiedStart = iv.getStart();
        scenarios[sc].specifiedEnd = iv.getEnd();
    }

    time_t getStartBufferEnd(int sc) const
    {
        return scenarios[sc].startBufferEnd;
    }
    time_t getEndBufferStart(int sc) const
    {
        return scenarios[sc].endBufferStart;
    }

    void setMinStart(int sc, time_t s) { scenarios[sc].minStart = s; }
    time_t getMinStart(int sc) const { return scenarios[sc].minStart; }

    void setMaxStart(int sc, time_t s) { scenarios[sc].maxStart = s; }
    time_t getMaxStart(int sc) const { return scenarios[sc].maxStart; }

    void setMinEnd(int sc, time_t e) { scenarios[sc].minEnd = e; }
    time_t getMinEnd(int sc) const { return scenarios[sc].minEnd; }

    void setMaxEnd(int sc, time_t e) { scenarios[sc].maxEnd = e; }
    time_t getMaxEnd(int sc) const { return scenarios[sc].maxEnd; }

    void setLength(int sc, double days) { scenarios[sc].length = days; }
    double getLength(int sc) const { return scenarios[sc].length; }

    void setEffort(int sc, double e) { scenarios[sc].effort = e; }
    double getEffort(int sc) const { return scenarios[sc].effort; }

    void setDuration(int sc, double d) { scenarios[sc].duration = d; }
    double getDuration(int sc) const { return scenarios[sc].duration; }

    bool isStartOk(int sc) const
    {
        return scenarios[sc].isStartOk();
    }
    bool isEndOk(int sc) const
    {
        return scenarios[sc].isEndOk();
    }

    bool isBuffer(int sc, const Interval& iv) const;

    void setComplete(int sc, double c) { scenarios[sc].reportedCompletion = c; }
    double getComplete(int sc) const
    {
        return scenarios[sc].reportedCompletion;
    }

    void setStatusNote(int sc, const QString& d)
    {
        scenarios[sc].statusNote = d;
    }
    const QString& getStatusNote(int sc) const
    {
        return scenarios[sc].statusNote;
    }

    void setStartBuffer(int sc, double p) { scenarios[sc].startBuffer = p; }
    double getStartBuffer(int sc) const { return scenarios[sc].startBuffer; }

    void setEndBuffer(int sc, double p) { scenarios[sc]. endBuffer = p; }
    double getEndBuffer(int sc) const { return scenarios[sc].endBuffer; }

    void setStartCredit(int sc, double c) { scenarios[sc].startCredit = c; }
    double getStartCredit(int sc) const { return scenarios[sc].startCredit; }

    void setEndCredit(int sc, double c) { scenarios[sc].endCredit = c; }
    double getEndCredit(int sc) const { return scenarios[sc].endCredit; }

    double getCalcEffort(int sc) const;
    double getCalcDuration(int sc) const;

//     double getCredits(int sc, const Interval& period, AccountType acctType,
//                       const Resource* resource = 0, bool recursive = true)
//         const;

    bool isActive(int sc, const Interval& period) const;
    TaskStatus getStatus(int sc) const { return scenarios[sc].status; }
    QString getStatusText(int sc) const;

    QString getSchedulingText() const;

    bool isCompleted(int sc, time_t date) const;
    void calcCompletionDegree(int sc);
    void calcContainerCompletionDegree(int sc, time_t now);
    double getCompletionDegree(int sc) const;
    double getCalcedCompletionDegree(int sc) const;
    TaskStatus getCompletionStatus(int sc) const
    {
        return scenarios[sc].status;
    }
    double getCompletedLoad(int sc) const;
    double getRemainingLoad(int sc) const;

    double getLoad(int sc, const Interval& period, const Resource* resource = 0)
        const;
    long getAllocatedTime(int sc, const Interval& period,
                          const Resource* resource = 0) const;
    double getAllocatedTimeLoad(int sc, const Interval& period,
                                const Resource* resource = 0) const;

    void addBookedResource(int sc, Resource* r)
    {
        if (scenarios[sc].bookedResources.indexOf((CoreAttributes*) r) == -1)
            scenarios[sc].bookedResources.inSort((CoreAttributes*) r);
    }
    bool isBookedResource(int sc, const Resource* r) const
    {
        return scenarios[sc].bookedResources.indexOf((CoreAttributes*) r) != -1;
    }
    ResourceListIterator getBookedResourcesIterator(int sc) const
    {
        return ResourceListIterator(scenarios[sc].bookedResources);
    }
    ResourceList getBookedResources(int sc) const
    {
        return scenarios[sc].bookedResources;
    }
    void setSpecifiedScheduled(int sc, bool ps)
    {
        scenarios[sc].specifiedScheduled = ps;
    }
    void setScheduled(int sc, bool ps) { scenarios[sc].scheduled = ps; }
    bool getScheduled(int sc) const { return scenarios[sc].scheduled; }

    bool isDutyOf(int sc, const Resource* r) const
    {
        return scenarios[sc].isDutyOf(r);
    }

    void overlayScenario(int base, int sc);
    void prepareScenario(int sc);
    void finishScenario(int sc);
    void computeCriticalness(int sc);
    double getCriticalness(int sc) const { return scenarios[sc].criticalness; }
    void checkAndMarkCriticalPath(int sc, double minSlack, time_t maxEnd);

    double computePathCriticalness(int sc);
    double getPathCriticalness(int sc) const
    {
        return scenarios[sc].pathCriticalness;
    }

    bool isOnCriticalPath(int sc, bool leavesOnly = true) const
    {
        if (!isLeaf() && leavesOnly)
            return false;

        return scenarios[sc].isOnCriticalPath;
    }

    bool hasCriticalLinkTo(int sc, const Task* t) const
    {
        return scenarios[sc].criticalLinks.contains(const_cast<Task*>(t));
    }

    /**
     * @retval true if itself or one of its subtasks (recursively) is on the critical path
     * @retval false otherwise.
     */
    bool isOrHasDescendantOnCriticalPath(int sc) const;

    bool isContainer() const { return !sub->isEmpty(); }

    bool xRef(QMap<QString, Task*>& hash);
    void implicitXRef();
    void sortAllocations();
    void saveSpecifiedBookedResources();
    QString resolveId(QString relId);

    bool preScheduleOk(int sc);
    bool scheduleOk(int sc) const;
    /**
     * Checks for loops in task interdependencies starting with the current
     * task.
     * @param ckedTaskList The list of already checked tasks. Will be appended
     * to.
     * @retval true if a loop was detected.
     * @retval false otherwise.
     */
    bool loopDetector(LDIList& chkedTaskList) const;
    bool checkDetermination(int sc) const;
    void computeBuffers();
    time_t nextSlot(time_t slotDuration) const;
    bool isReadyForScheduling() const;
    bool schedule(int sc, time_t& reqStart, time_t duration);
    void propagateInitialValues(int sc);
    void setRunaway();
    bool isRunaway() const;

    bool isSubTask(const Task* t) const;

    bool isSchedulingDone() const { return schedulingDone; }

    void errorMessage(const QString& msg) const;
    void warningMessage(const QString& msg) const;

    QDomElement xmlElement( QDomDocument& doc, bool absId = true );

    TaskList getSuccessors() const { return successors; }
    TaskList getPredecessors() const { return predecessors; }

    bool hasAlapPredecessor() const;

private:
    friend class Project;
    void propagateStart(int sc, time_t date);
    void propagateEnd(int sc, time_t date);
    /**
     * Checks for loops in task interdependencies starting with the current
     * task under ASAP or ALAP scheduling.
     * @param ckedTaskList The list of already checked tasks. Will be appended
     * to.
     * @retval true if a loop was detected.
     * @retval false otherwise.
     */
    bool loopDetection(LDIList& list, LDIList& chkedTaskList, bool atEnd,
                       bool fromOutside) const;
    bool checkPathForLoops(LDIList& list, bool atEnd) const;
    bool scheduleContainer(int sc);
    Task* subFirst() { return 0/*(Task*) sub->first()*/; }
    Task* subNext() { return 0/*(Task*) sub->next()*/; }
    /// Returns the availability of @p resource and its required resources (if any)
    int isAvailable(Allocation *allocation, Resource *resource, time_t slot) const;
    /// Book resource @p r if it (and its required resources) is available
    bool bookResource(Allocation *allocation, Resource* r, time_t day, time_t duration,
                      int& slotsToLimit, int& availability);
    void bookResources(int sc, time_t day, time_t duration);
    void addBookedResource(Resource* r)
    {
        if (bookedResources.indexOf((CoreAttributes*) r) == -1)
            bookedResources.inSort((CoreAttributes*) r);
    }
    QList<Resource*> createCandidateList(int sc, time_t date, Allocation* a);
    time_t earliestStart(int sc) const;
    time_t latestEnd(int sc) const;

    bool startCanBeDetermined(LDIList& list, int sc) const;
    bool endCanBeDetermined(LDIList& list, int sc) const;

    bool hasStartDependency(int sc) const;
    bool hasEndDependency(int sc) const;

    bool hasStartDependency() const;
    bool hasEndDependency() const;

    bool analyzePath(int sc, double minSlack, time_t pathStart, long busyTime,
                     long worstMinSlackTime, long& checked, long& found);
    void collectTransientFollowers(TaskList& list);

    bool countMilestones(int sc, time_t now, int& totalMilestones,
                         int& completedMilestones,
                         int& reportedCompletedMilestones);
    bool sumUpEffort(int sc, time_t now, double& totalEffort,
                     double& completedEffort,
                     double& reportedCompletedEffort);

    /// A longer description of the task.
    QString note;

    /// List of notes with a date attached.
//     Journal journal;

    /// A reference to an external document
    QString ref;

    /// A label used instead of the reference
    QString refLabel;

    /**
     * The dependencies of the task are stored twice. depends and precedes
     * store the information specified in the project file. For convenience we
     * also store the backward dependency together with the specified
     * dependencies in predecessors and successors.
     */
    QList<TaskDependency*> depends;

    /// @see depends
    QList<TaskDependency*> precedes;

    /**
     * A list of tasks that must precede this task.
     */
    TaskList predecessors;

    /**
     * A list of tasks that must follow this task.
     */
    TaskList successors;

    /**
     * A list of all tasks that preceed this task. It's the combination of
     * depends and predecessors. This is redundant information but stored for
     * conveniance. Interdependent tasks are linked in a doubly linked list.
     */
    TaskList previous;

    /**
     * A list of all tasks that follow this task. It's the combination of
     * precedes and successors. This is redundant information but stored for
     * conveniance. Interdependent tasks are linked in a doubly linked list.
     */
    TaskList followers;

    /**
     * The ID of the project this task belongs to. This is only
     * meaningful if multiple projects are joined to create a big
     * project. */
    QString projectId;

    /// True if the task is a milestone.
    bool milestone;

    /**
     * The priority is used during scheduling. The higher the priority
     * the more likely the task will get the requested resources. */
    int priority;

    /// The scheduling policy of the task.
    SchedulingInfo scheduling;

    /// ID of responsible resource.
    Resource* responsible;

    /// Tasks may only be worked on during the specified shifts.
    ShiftSelectionList shifts;

    /// List of resource allocations requested by the task
    QList<Allocation*> allocations;

    /// Account where the credits of the task are credited to.
//     Account* account;

    TaskScenario* scenarios;

    /* The following group of variables store values generated during a
     * scheduler run. They might be initialized by other values and/or
     * they might contain results of the scheduling run. But they should
     * never be initialized directly or read out directly. They should have
     * corresponding scenario variables. The get/set interface functions
     * should only access the scenario variables. */

    /// Day when the task should start
    time_t start;

    /// Day when the task should end
    time_t end;

    /// Length in working days
    double length;

    /// Effort (in man days) needed to complete the task
    double effort;

    /// Duration in calendar days
    double duration;

    /// The already completed effort in a scheduler run.
    double doneEffort;

    /// The already completed length in a scheduler run.
    double doneLength;

    /// The already completed duration in a scheduler run.
    double doneDuration;

    /**
     * Set to true when the first time slots have with resource usage
     * have been allocated. */
    bool workStarted;

    /**
     * Since the full time slot might not be available we need to
     * store the tentative start of a task in a separate
     * variable. Storing the information in 'start' would mark the
     * task as fully scheduled which might not yet be the case. */
    time_t tentativeStart;

    /**
     * Since the full time slot might not be available we need to
     * store the tentative end of a task in a separate
     * variable. Storing the information in 'end' would mark the task
     * as fully scheduled which might not yet be the case. */
    time_t tentativeEnd;

    /**
     * Depending on the scheduling policy the tasks need to be scheduled
     * from one end the other in a continuous way. No timeslot may be
     * scheduled twice. This variable stores information about the last
     * allocation, so we can make sure the next slot is exactly adjacent
     * the the previous one. */
    time_t lastSlot;

    /// This variable is set to true when the task has been scheduled.
    bool schedulingDone;

    /** This flag is set when the task does not fit into the project time
     * frame. */
    bool runAway;

    /// A list of all the resources booked for this task.
    ResourceList bookedResources;
} ;

} // namespace TJ

KPLATOTJ_EXPORT QDebug operator<<( QDebug dbg, const TJ::Task* t );
KPLATOTJ_EXPORT QDebug operator<<( QDebug dbg, const TJ::Task& t );

#endif

