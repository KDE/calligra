/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef kpttask_h
#define kpttask_h

#include "kptduration.h"
#include "kptnode.h"
#include "kptresource.h"
#include "defs.h"

#include <qptrlist.h>

class KPTDateTime;

/**
  * A task in the scheduling software is represented by this class. A task
  * can be anything from 'build house' to 'drill hole' It will always mean
  * an activity.
  */
class KPTTask : public KPTNode {
public:
    KPTTask(KPTNode *parent = 0);
    ~KPTTask();

    virtual int type() const;

    /**
     * The expected Duration is the expected time to complete a Task, Project,
     * etc. For an individual Task, this will calculate the expected duration
     * by querying the Distribution of the Task. If the Distribution is a
     * simple RiskNone, the value will equal the mode Duration, but for other
     * Distributions like RiskHigh, the value will have to be calculated. For
     * a Project or Subproject, the expected Duration is calculated by
     * PERT/CPM.
     */
    KPTDuration *getExpectedDuration();

    /**
     * Instead of using the expected duration, generate a random value using
     * the Distribution of each Task. This can be used for Monte-Carlo
     * estimation of Project duration.
     */
    KPTDuration *getRandomDuration();

    /**
     * Retrieve the calculated float of this node
     */
    KPTDuration *getFloat();

    /**
     * Return the resource request made to @group
     * (There should be only one)
     */
    KPTResourceGroupRequest *resourceGroupRequest(KPTResourceGroup *group) const;
    void clearResourceRequests();
    void addRequest(KPTResourceGroup *group, int numResources);
    void addRequest(KPTResourceGroupRequest *request);
    int units() const;
    int workUnits() const;
    void makeAppointments();

    void setConstraint(KPTNode::ConstraintType type);

    /**
     * TODO: Load and save
     */
    virtual bool load(QDomElement &element);
    virtual void save(QDomElement &element);

    /**
     * Returns the total planned cost for this task (or subtasks)
     */
    virtual double plannedCost();
    /**
     * Returns the planned cost for this task (or subtasks) upto date @dt
     */
    virtual double plannedCost(QDateTime &dt);
    /**
     * Returns the actaually reported cost for this task (or subtasks)
     */
    virtual double actualCost();

    int plannedWork();
    int plannedWork(QDateTime &dt);
    int actualWork();

    void initiateCalculationLists(QPtrList<KPTNode> &startnodes, QPtrList<KPTNode> &endnodes, QPtrList<KPTNode> &summarytasks);
    /**
     * Calculate @ref m_durationForward from @ref earliestStart and
     * return the resulting end time, 
     * which will be used as the succesors @ref earliestStart.
     *
     * If this task is a summarytask, all children are calculated first
     * and the summary tasks duration and earliestStart is calculated 
     * from the children.
     */
    KPTDateTime calculateForward(int use);
    /**
     * Calculate @ref m_durationBackward from @ref latestFinish and
     * return the resulting start time, 
     * which will be used as the predecessors @ref latestFinish.
     *
     * If this task is a summarytask, all children are calculated first
     * and  the summary tasks duration and latestFinish is calculated 
     * from the children.
     */
    KPTDateTime calculateBackward(int use);
    /**
     * Calculate @m_startTime, @ref m_endTime and @ref m_duration,
     * scheduling the task within the limits of earliestStart and latestFinish.
     * Return @ref m_endTime which can be used for scheduling the successor.
     * Assumes @ref calculateForward() and calculateBackward() has been run.
     */
    KPTDateTime &scheduleForward(KPTDateTime &earliest, int use);
    /**
     * Calculate @m_startTime, @ref m_endTime and @ref m_duration,
     * scheduling the task within the limits of earliestStart and latestFinish.
     * Return @ref m_startTime which can be used for scheduling the predecessor.
     * Assumes @ref calculateForward() and calculateBackward() has been run.
     */
    KPTDateTime &scheduleBackward(KPTDateTime &latest, int use);
    
    /**
     * Summarytasks (with milestones) need special treatment because 
     * milestones are always 'glued' to their predecessors.
     */
    void adjustSummarytask();
    
    /**
     * Returns the duration from latestFinish of the 'latest subtask' 
     * to @param time.
     * Used to calculate diration of summarytasks.
     */
    KPTDuration summarytaskDurationForward(const KPTDateTime &time);
     /// Returns the earliestStart of the 'earliest subtask'
    KPTDateTime summarytaskEarliestStart();
    /**
     * Returns the duration from earliestStart of the 'earliest subtask' 
     * to @param time.
     * Used to calculate diration of summarytasks.
     */
    KPTDuration summarytaskDurationBackward(const KPTDateTime &time);
     /// Returns the latestFinish of the 'latest subtask'
    KPTDateTime summarytaskLatestFinish();

    /**
     * Return the duration calculated on bases of the requested resources
     */
    KPTDuration calcDuration(const KPTDateTime &time, const KPTDuration &effort, bool backward);

private:
    QPtrList<KPTResourceGroup> m_resource;

    KPTResourceRequestCollection *m_requests;
 
    KPTDuration m_durationForward;
    KPTDuration m_durationBackward;

#ifndef NDEBUG
public:
    void printDebug(bool children, QCString indent);
#endif

};
#endif
