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

#ifndef kpttask_h
#define kpttask_h

#include "kptnode.h"
#include "kptduration.h"
#include "kptresource.h"

#include <qptrlist.h>

namespace KPlato
{

class KPTDateTime;

/**
  * A task in the scheduling software is represented by this class. A task
  * can be anything from 'build house' to 'drill hole' It will always mean
  * an activity.
  */
class KPTTask : public KPTNode {
public:
    KPTTask(KPTNode *parent = 0);
    KPTTask(KPTTask &task, KPTNode *parent = 0);
    ~KPTTask();

    /// Return task type. Can be Type_Task, Type_Summarytask ot Type_Milestone.
    virtual int type() const;

    /**
     * Returns the (previously) calculated duration.
     * The caller must delete returned object.
     */
    KPTDuration *getExpectedDuration();

    /**
     * Instead of using the expected duration, generate a random value using
     * the Distribution of each Task. This can be used for Monte-Carlo
     * estimation of Project duration.
     */
    KPTDuration *getRandomDuration();

    /**
     * Return the resource request made to group
     * (There should be only one)
     */
    KPTResourceGroupRequest *resourceGroupRequest(KPTResourceGroup *group) const;
    void clearResourceRequests();
    void addRequest(KPTResourceGroup *group, int numResources);
    void addRequest(KPTResourceGroupRequest *request);
    void takeRequest(KPTResourceGroupRequest *request);
    int units() const;
    int workUnits() const;
    void makeAppointments();
    /**
     * Calculates if the assigned resource is overbooked 
     * within the duration of this task
     */
    void calcResourceOverbooked();
    
    void setConstraint(KPTNode::ConstraintType type);

    /// Load from document
    virtual bool load(QDomElement &element);
    /// Save to document
    virtual void save(QDomElement &element);

    /// Returns the total planned effort for this task (or subtasks) 
    virtual KPTDuration plannedEffort();
    /// Returns the total planned effort for this task (or subtasks) on date
    virtual KPTDuration plannedEffort(const QDate &date);
    /// Returns the planned effort up to and including date
    virtual KPTDuration plannedEffortTo(const QDate &date);
    
    /// Returns the total actual effort for this task (or subtasks) 
    virtual KPTDuration actualEffort();
    /// Returns the total actual effort for this task (or subtasks) on date
    virtual KPTDuration actualEffort(const QDate &date);
    /// Returns the actual effort up to and including date
    virtual KPTDuration actualEffortTo(const QDate &date);
    
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
    
    /**
     * Sets up the lists used for calculation.
     * This includes adding summarytasks relations to subtasks
     * and lists for start- and endnodes.
     */
    void initiateCalculationLists(QPtrList<KPTNode> &startnodes, QPtrList<KPTNode> &endnodes, QPtrList<KPTNode> &summarytasks);
    /**
     * Calculates @ref m_durationForward from @ref earliestStart and
     * returns the resulting end time, 
     * which will be used as the succesors @ref earliestStart.
     *
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     */
    KPTDateTime calculateForward(int use);
    /**
     * Calculates @ref m_durationBackward from @ref latestFinish and
     * returns the resulting start time, 
     * which will be used as the predecessors @ref latestFinish.
     *
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     */
    KPTDateTime calculateBackward(int use);
    /**
     * Schedules the task within the limits of earliestStart and latestFinish.
     * Calculates @ref m_startTime, @ref m_endTime and @ref m_duration,
     * Assumes @ref calculateForward() and @ref calculateBackward() has been run.
     *
     * @param latest The task is not scheduled to start earlier than this
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     * @return The tasks endtime which can be used for scheduling the successor.
     */
    KPTDateTime &scheduleForward(KPTDateTime &earliest, int use);
    /**
     * Schedules the task within the limits of earliestStart and latestFinish.
     * Calculates @ref m_startTime, @ref m_endTime and @ref m_duration,
     * Assumes @ref calculateForward() and @ref calculateBackward() has been run.
     *
     * @param latest The task is not scheduled to end later than this
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     * @return The tasks starttime which can be used for scheduling the predeccessor.
     */
    KPTDateTime &scheduleBackward(KPTDateTime &latest, int use);
    
    /**
     * Summarytasks (with milestones) need special treatment because 
     * milestones are always 'glued' to their predecessors.
     */
    void adjustSummarytask();
    
    /**
     * Return the duration calculated on bases of the requested resources
     */
    KPTDuration calcDuration(const KPTDateTime &time, const KPTDuration &effort, bool backward);

    // Proxy relations are relations to/from summarytasks. 
    // These relations are distrubuted to the relevant tasks before calculation.
    void clearProxyRelations();
    void addParentProxyRelations(QPtrList<KPTRelation> &list);
    void addChildProxyRelations(QPtrList<KPTRelation> &list);
    void addParentProxyRelation(KPTNode *node, const KPTRelation *rel);
    void addChildProxyRelation(KPTNode *node, const KPTRelation *rel);
    
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
    virtual const KPTDateTime &workStartTime() const;
    
    /**
     * Return the time when work can actually finish on this task.
     * This will be the time assigned resources can end work in accordance
     * with their calendar, or if no resources have been assigned,
     * the scheduled endtime is used.
     */
    virtual const KPTDateTime &workEndTime() const;
    
    /**
     * Return the duration that an activity's start can be delayed 
     * without affecting the project completion date. 
     * An activity with positive float is not on the critical path.
     */
    KPTDuration positiveFloat();
    /**
     * Return the duration by which the duration of an activity or path 
     * has to be reduced in order to fullfill a timing constraint.
     */
    KPTDuration negativeFloat() { return KPTDuration(); }
    /**
     * Return the duration by which an activity can be delayed or extended 
     * without affecting the start of any succeeding activity.
     */
    KPTDuration freeFloat() { return KPTDuration(); }
    /**
     * Return the duration from Early Start to Late Start.
     */
    KPTDuration startFloat() { return KPTDuration(); }
    /**
     * Return the duration the task has at its finish  before a successor task starts.
     * This is the difference between the start time of the successor and
     * the finish time of this task.
     */
    KPTDuration finishFloat() { return KPTDuration(); }
    
    /// A task is critical if there is no positive float
    virtual bool isCritical();
    /// Calculate critical path
    virtual bool calcCriticalPath();
    
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
        KPTDateTime startTime, finishTime;
        int percentFinished;
        KPTDuration remainingEffort;
        KPTDuration totalPerformed;        
    };
    struct Progress &progress() { return m_progress; }
    
private:
    KPTDateTime calculateSuccessors(const QPtrList<KPTRelation> &list, int use);
    KPTDateTime calculatePredeccessors(const QPtrList<KPTRelation> &list, int use);
    KPTDateTime scheduleSuccessors(const QPtrList<KPTRelation> &list, int use);
    KPTDateTime schedulePredeccessors(const QPtrList<KPTRelation> &list, int use);
    
    KPTDateTime workStartAfter(const KPTDateTime &dt);
    KPTDateTime workFinishBefore(const KPTDateTime &dt);

private:
    QPtrList<KPTResourceGroup> m_resource;

    KPTResourceRequestCollection *m_requests;
 
    QPtrList<KPTRelation> m_parentProxyRelations;
    QPtrList<KPTRelation> m_childProxyRelations;
      
    struct Progress m_progress;
    
#ifndef NDEBUG
public:
    void printDebug(bool children, QCString indent);
#endif

};

}  //KPlato namespace

#endif
