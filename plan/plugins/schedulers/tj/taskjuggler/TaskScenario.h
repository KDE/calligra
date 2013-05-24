/*
 * TaskScenario.h - TaskJuggler
 *
 * Copyright (c) 2002 by Chris Schlaeger <cs@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * $Id$
 */

#ifndef _TaskScenario_h_
#define _TaskScenario_h_

#include <time.h>

#include "ResourceList.h"

namespace TJ
{

class Task;
class Resource;

class TaskScenario
{
    friend class Task;
    friend class TaskList;
public:
    TaskScenario();
    ~TaskScenario() { }

    void setStart(time_t s) { start = s; }
    void setEnd(time_t e) { end = e; }
    void setMaxEnd(time_t e) { maxEnd = e; }
    void setMinEnd(time_t e) { minEnd = e; }
    void setMaxStart(time_t s) { maxStart = s; }
    void setMinStart(time_t s) { minStart = s; }

    bool isStartOk() const
    {
        return !((minStart > 0 && minStart > start) ||
                 (maxStart > 0 && start > maxStart));
    }
    bool isEndOk() const
    {
        return !((minEnd > 0 && (end < minEnd)) ||
                 (maxEnd > 0 && (end > maxEnd)));
    }

    void calcCompletionDegree(time_t now);

    bool isDutyOf(const Resource* r) const;

    ResourceListIterator getBookedResourcesIterator() const
    {
	return ResourceListIterator(bookedResources);
    }

private:
    /// Pointer to the corresponding task.
    Task* task;

    /// Index of the scenario
    int index;

    /// Time the user has specified as a start time.
    time_t specifiedStart;

    /// Time the user has specified as an end time.
    time_t specifiedEnd;

    /// Time when the task starts
    time_t start;

    /// Time when the task ends
    time_t end;

    /// Ealiest day when the task should start
    time_t minStart;

    /// Latest day when the task should start
    time_t maxStart;

    /// Ealiest day when the task should end
    time_t minEnd;

    /// Latest day when the task should end
    time_t maxEnd;

    /* Specifies how many percent the task start can be delayed but still
     * finish in time if all goes well. This value is for documentation
     * purposes only. It is not used for task scheduling. */
    double startBuffer;

    /* Specifies how many percent the task can be finished earlier if
     * all goes well. This value is for documentation purposes only. It is
     * not used for task scheduling. */
    double endBuffer;

    /// Time when the start buffer ends.
    time_t startBufferEnd;

    /// Time when the end buffer starts.
    time_t endBufferStart;

    /// The duration of the task (in calendar days).
    double duration;

    /// The length of the task (in working days).
    double length;

    /// The effort of the task (in resource-days).
    double effort;

    /// Amount that is credited to the account at the start date.
    double startCredit;

    /// Amount that is credited to the account at the end date.
    double endCredit;

    /** Measure for the likelyhood that the tasks gets the allocated
     * resources. */
    double criticalness;

    /** Measure for the criticalness of the task chain. This value is computed
     * prior to scheduling. It's in fact used to improve the scheduling
     * result. But it should not be confused with the critical path of the
     * final result. */
    double pathCriticalness;

    /** Contrary to the previous criticalness related values this is the
     * result of the post-scheduling analysis. */
    bool isOnCriticalPath;

    /// User specified percentage of completion of the task
    double reportedCompletion;

    /* Container tasks can have an indirect reported completion. This is based
     * on reported completions and calculated completions of their subtasks.
     * This value is only valid for container tasks. */
    double containerCompletion;

    /// Calculated completion degree
    double completionDegree;

    /// Status that the task is in (according to 'now' date)
    TaskStatus status;

    /// A longer description of the task status.
    QString statusNote;

    /**
     * true if the user has specified the task for the scenario as already
     * fully scheduled.
     */
    bool specifiedScheduled;

    /// true if the task has been completely scheduled.
    bool scheduled;

    /**
     * This following variables are used to cache the result whether or not
     * the start/end of this task can be determined. They are determined once
     * and possible the value is used later on as the determination of once
     * task can depend on another task.
     */
    bool startCanBeDetermined;
    bool endCanBeDetermined;

    /// List of specified booked resources.
    ResourceList specifiedBookedResources;

    /// List of booked resources.
    ResourceList bookedResources;

    /**
     * This list stores pointers to the task that have been found to be
     * critical followers.
     */
    QList<Task*> criticalLinks;
} ;

} // namespace TJ

#endif
