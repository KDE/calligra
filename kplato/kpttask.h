/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTTASK_H
#define KPTTASK_H

#include "kptnode.h"
#include "kptduration.h"
#include "kptresource.h"

#include <qptrlist.h>

namespace KPlato
{

class DateTime;

/**
  * A task in the scheduling software is represented by this class. A task
  * can be anything from 'build house' to 'drill hole' It will always mean
  * an activity.
  */
class Task : public Node {
public:
    Task(Node *parent = 0);
    Task(Task &task, Node *parent = 0);
    ~Task();

    /// Return task type. Can be Type_Task, Type_Summarytask ot Type_Milestone.
    virtual int type() const;

    /**
     * Returns the (previously) calculated duration.
     * The caller must delete returned object.
     */
    Duration *getExpectedDuration();

    /**
     * Instead of using the expected duration, generate a random value using
     * the Distribution of each Task. This can be used for Monte-Carlo
     * estimation of Project duration.
     */
    Duration *getRandomDuration();

    /**
     * Return the resource request made to group
     * (There should be only one)
     */
    ResourceGroupRequest *resourceGroupRequest(ResourceGroup *group) const;
    void clearResourceRequests();
    void addRequest(ResourceGroup *group, int numResources);
    void addRequest(ResourceGroupRequest *request);
    void takeRequest(ResourceGroupRequest *request);
    int units() const;
    int workUnits() const;
    void makeAppointments();
    /**
     * Calculates if the assigned resource is overbooked 
     * within the duration of this task
     */
    void calcResourceOverbooked();
    
    void setConstraint(Node::ConstraintType type);

    /// Load from document
    virtual bool load(QDomElement &element, Project &project);
    /// Save to document
    virtual void save(QDomElement &element) const;
    /// Save appointments for schedule with id
    virtual void saveAppointments(QDomElement &element, long id) const;
    /**
     * Returns a list of planned effort and cost for this task
     * for the interval start, end inclusive
     */
    virtual EffortCostMap plannedEffortCostPrDay(const QDate &start, const QDate &end) const;
    
    /// Returns the total planned effort for this task (or subtasks) 
    virtual Duration plannedEffort();
    /// Returns the total planned effort for this task (or subtasks) on date
    virtual Duration plannedEffort(const QDate &date);
    /// Returns the planned effort up to and including date
    virtual Duration plannedEffortTo(const QDate &date);
    
    /// Returns the total actual effort for this task (or subtasks) 
    virtual Duration actualEffort();
    /// Returns the total actual effort for this task (or subtasks) on date
    virtual Duration actualEffort(const QDate &date);
    /// Returns the actual effort up to and including date
    virtual Duration actualEffortTo(const QDate &date);
    
    /**
     * Returns the total planned cost for this task (or subtasks)
     */
    virtual double plannedCost();
    /// Planned cost on date
    virtual double plannedCost(const QDate &/*date*/);
    /// Planned cost up to and including date
    virtual double plannedCostTo(const QDate &/*date*/);
    
    /**
     * Returns the actaually reported cost for this task (or subtasks)
     */
    virtual double actualCost();
    /// Actual cost on date
    virtual double actualCost(const QDate &/*date*/);
    /// Actual cost up to and including date
    virtual double actualCostTo(const QDate &/*date*/);

    /// Effort based performance index
    double effortPerformanceIndex(const QDate &date, bool *error=0);
    /// Cost performance index
    double costPerformanceIndex(const QDate &date, bool *error=0);
    
    void initiateCalculation(Schedule &sch);
    /**
     * Sets up the lists used for calculation.
     * This includes adding summarytasks relations to subtasks
     * and lists for start- and endnodes.
     */
    void initiateCalculationLists(QPtrList<Node> &startnodes, QPtrList<Node> &endnodes, QPtrList<Node> &summarytasks);
    /**
     * Calculates @ref m_durationForward from @ref earliestStart and
     * returns the resulting end time, 
     * which will be used as the succesors @ref earliestStart.
     *
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     */
    DateTime calculateForward(int use);
    /**
     * Calculates @ref m_durationBackward from @ref latestFinish and
     * returns the resulting start time, 
     * which will be used as the predecessors @ref latestFinish.
     *
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     */
    DateTime calculateBackward(int use);
    /**
     * Schedules the task within the limits of earliestStart and latestFinish.
     * Calculates @ref m_startTime, @ref m_endTime and @ref m_duration,
     * Assumes @ref calculateForward() and @ref calculateBackward() has been run.
     *
     * @param latest The task is not scheduled to start earlier than this
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     * @return The tasks endtime which can be used for scheduling the successor.
     */
    DateTime scheduleForward(const DateTime &earliest, int use);
    /**
     * Schedules the task within the limits of earliestStart and latestFinish.
     * Calculates @ref m_startTime, @ref m_endTime and @ref m_duration,
     * Assumes @ref calculateForward() and @ref calculateBackward() has been run.
     *
     * @param latest The task is not scheduled to end later than this
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     * @return The tasks starttime which can be used for scheduling the predeccessor.
     */
    DateTime scheduleBackward(const DateTime &latest, int use);
    
    /**
     * Summarytasks (with milestones) need special treatment because 
     * milestones are always 'glued' to their predecessors.
     */
    void adjustSummarytask();
    
    /**
     * Return the duration calculated on bases of the requested resources
     */
    Duration calcDuration(const DateTime &time, const Duration &effort, bool backward);

    // Proxy relations are relations to/from summarytasks. 
    // These relations are distrubuted to the relevant tasks before calculation.
    void clearProxyRelations();
    void addParentProxyRelations(QPtrList<Relation> &list);
    void addChildProxyRelations(QPtrList<Relation> &list);
    void addParentProxyRelation(Node *node, const Relation *rel);
    void addChildProxyRelation(Node *node, const Relation *rel);
    
    /// Check if this node has any dependent child nodes.
    bool isEndNode() const;
    /// Check if this node has any dependent parent nodes
    bool isStartNode() const;
    
    /**
     * Return the time when work can actually start on this task.
     * This will be the time assigned resources can start work in accordance
     * with their calendar, or if no resources have been assigned,
     * the scheduled starttime is used.
     */
    virtual DateTime workStartTime() const;
    
    /**
     * Return the time when work can actually finish on this task.
     * This will be the time assigned resources can end work in accordance
     * with their calendar, or if no resources have been assigned,
     * the scheduled endtime is used.
     */
    virtual DateTime workEndTime() const;
    
    /**
     * Return the duration that an activity's start can be delayed 
     * without affecting the project completion date. 
     * An activity with positive float is not on the critical path.
     */
    Duration positiveFloat();
    /**
     * Return the duration by which the duration of an activity or path 
     * has to be reduced in order to fullfill a timing constraint.
     */
    Duration negativeFloat() { return Duration(); }
    /**
     * Return the duration by which an activity can be delayed or extended 
     * without affecting the start of any succeeding activity.
     */
    Duration freeFloat() { return Duration(); }
    /**
     * Return the duration from Early Start to Late Start.
     */
    Duration startFloat() { return Duration(); }
    /**
     * Return the duration the task has at its finish  before a successor task starts.
     * This is the difference between the start time of the successor and
     * the finish time of this task.
     */
    Duration finishFloat() { return Duration(); }
    
    /// A task is critical if there is no positive float
    virtual bool isCritical();
    /// Calculate critical path
    virtual bool calcCriticalPath(bool fromEnd);
    
    /// Set current schedule to schedule with identity id, for me nd my children
    virtual void setCurrentSchedule(long id);
    
    struct Progress {
        Progress() { started = finished = false; percentFinished = 0; }
        bool operator==(struct Progress &p) {
            return started == p.started && finished == p.finished &&
                   startTime == p.startTime && finishTime == p.finishTime &&
                   percentFinished == p.percentFinished &&
                   remainingEffort == p.remainingEffort &&
                   totalPerformed == p.totalPerformed;
        }
        bool operator!=(struct Progress &p) { return !(*this == p); }
        struct Progress &operator=(struct Progress &p) {
            started = p.started; finished = p.finished;
            startTime = p.startTime; finishTime = p.finishTime;
            percentFinished = p.percentFinished;
            remainingEffort = p.remainingEffort;
            totalPerformed = p.totalPerformed;
            return *this;
        }
        bool started, finished;
        DateTime startTime, finishTime;
        int percentFinished;
        Duration remainingEffort;
        Duration totalPerformed;        
    };
    struct Progress &progress() { return m_progress; }
    
private:
    DateTime calculateSuccessors(const QPtrList<Relation> &list, int use);
    DateTime calculatePredeccessors(const QPtrList<Relation> &list, int use);
    DateTime scheduleSuccessors(const QPtrList<Relation> &list, int use);
    DateTime schedulePredeccessors(const QPtrList<Relation> &list, int use);
    
    DateTime workStartAfter(const DateTime &dt);
    DateTime workFinishBefore(const DateTime &dt);

private:
    QPtrList<ResourceGroup> m_resource;

    ResourceRequestCollection *m_requests;
 
    QPtrList<Relation> m_parentProxyRelations;
    QPtrList<Relation> m_childProxyRelations;
      
    struct Progress m_progress;
    
#ifndef NDEBUG
public:
    void printDebug(bool children, QCString indent);
#endif

};

}  //KPlato namespace

#endif
