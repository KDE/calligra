/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org
   Copyright (C) 2004 - 2007 Dag Andersen <danders@get2net.dk>
   Copyright (C) 2007 Florian Piquemal <flotueur@yahoo.fr>
   Copyright (C) 2007 Alexis Ménard <darktears31@gmail.com>

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

#include <QDateTime>
#include <QList>
#include <QMap>

namespace KPlato
{

/**
 * The Completion class holds information about the tasks progress.
 */
class Completion
{
    
public:
    class Entry
    {
        public:
            Entry()
            : percentFinished( 0 ),
              remainingEffort( Duration::zeroDuration ),
              totalPerformed( Duration::zeroDuration )
            {}
            Entry( int percent,  Duration remaining, Duration performed )
            : percentFinished( percent ),
              remainingEffort( remaining ),
              totalPerformed( performed )
            {}
            
            int percentFinished;
            Duration remainingEffort;
            Duration totalPerformed;
    };
    typedef QMap<QDate, Entry*> EntryList;

    explicit Completion();
    ~Completion();
    
    bool operator==(const Completion &p);
    bool operator!=(Completion &p) { return !(*this == p); }
    Completion &operator=(const Completion &p);
    
    /// Load from document
    bool loadXML(QDomElement &element, XMLLoaderObject &status );
    /// Save to document
    void saveXML(QDomElement &element) const;
    
    bool started() const { return m_started; }
    void setStarted( bool on );
    bool finished() const { return m_finished; }
    void setFinished( bool on );
    DateTime startTime() const { return m_startTime; }
    void setStartTime( const QDateTime &dt );
    DateTime finishTime() const { return m_finishTime; }
    void setFinishTime( const QDateTime &dt );
    
    const EntryList &entries() const { return m_entries; }
    void addEntry( const QDate &date, Entry *entry );
    Entry *takeEntry( const QDate &date ) { return m_entries.take( date ); }
    Entry *entry( const QDate &date ) const { return m_entries[ date ]; }
    
    QDate entryDate() const;
    int percentFinished() const;
    Duration remainingEffort() const;
    Duration totalPerformed() const;
    
private:
    bool m_started, m_finished;
    DateTime m_startTime, m_finishTime;
    EntryList m_entries;

#ifndef NDEBUG
public:
    void printDebug( const QByteArray &ident ) const;
#endif
};


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
    virtual QStringList requestNameList() const;
    virtual bool containsRequest( const QString &/*identity*/ ) const;
    virtual ResourceRequest *resourceRequest( const QString &/*name*/ ) const;
    
    /**
     * Calculates if the assigned resource is overbooked 
     * within the duration of this task
     */
    void calcResourceOverbooked();
    
    void setConstraint(Node::ConstraintType type);

    /// Load from document
    virtual bool load(QDomElement &element, XMLLoaderObject &status );
    /// Save to document
    virtual void save(QDomElement &element) const;
    /// Save appointments for schedule with id
    virtual void saveAppointments(QDomElement &element, long id) const;
    /**
     * Returns a list of planned effort and cost for this task
     * for the interval start, end inclusive
     */
    virtual EffortCostMap plannedEffortCostPrDay(const QDate &start, const QDate &end,  long id = -1 ) const;
    
    /// Returns the total planned effort for this task (or subtasks) 
    virtual Duration plannedEffort( long id = -1 );
    /// Returns the total planned effort for this task (or subtasks) on date
    virtual Duration plannedEffort(const QDate &date, long id = -1 );
    /// Returns the planned effort up to and including date
    virtual Duration plannedEffortTo(const QDate &date, long id = -1 );
    
    /// Returns the total actual effort for this task (or subtasks) 
    virtual Duration actualEffort( long id = -1 );
    /// Returns the total actual effort for this task (or subtasks) on date
    virtual Duration actualEffort(const QDate &date, long id = -1 );
    /// Returns the actual effort up to and including date
    virtual Duration actualEffortTo(const QDate &date, long id = -1 );
    
    /**
     * Returns the total planned cost for this task (or subtasks)
     */
    virtual double plannedCost( long id = -1 );
    /// Planned cost on date
    virtual double plannedCost(const QDate &/*date*/, long id = -1 );
    /// Planned cost up to and including date
    virtual double plannedCostTo(const QDate &/*date*/, long id = -1 );
    
    /**
     * Returns the actaually reported cost for this task (or subtasks)
     */
    virtual double actualCost( long id = -1 );
    /// Actual cost on date
    virtual double actualCost(const QDate &/*date*/, long id = -1 );
    /// Actual cost up to and including date
    virtual double actualCostTo(const QDate &/*date*/, long id = -1 );

    /// Effort based performance index
    double effortPerformanceIndex(const QDate &date, bool *error=0);
    /// Cost performance index
    double costPerformanceIndex(const QDate &date, bool *error=0);
    
    /**
     * Return the duration that an activity's start can be delayed 
     * without affecting the project completion date. 
     * An activity with positive float is not on the critical path.
     * @param id Schedule identity. If id is -1, use current schedule.
     */
    Duration positiveFloat( long id = -1 ) const;
    /**
     * Return the duration by which the duration of an activity or path 
     * has to be reduced in order to fullfill a timing constraint.
     * @param id Schedule identity. If id is -1, use current schedule.
     */
    Duration negativeFloat( long id = -1 ) const;
    /**
     * Return the duration by which an activity can be delayed or extended 
     * without affecting the start of any succeeding activity.
     * @param id Schedule identity. If id is -1, use current schedule.
     */
    Duration freeFloat( long id = -1 ) const;
    /**
     * Return the duration from Early Start to Late Start.
     * @param id Schedule identity. If id is -1, use current schedule.
     */
    Duration startFloat( long id = -1 ) const;
    /**
     * Return the duration from Early Finish to Late Finish.
     * @param id Schedule identity. If id is -1, use current schedule.
     */
    Duration finishFloat( long id = -1 ) const;
    
    /**
     * A task is critical if positive float equals 0
     * @param id Schedule identity. If id is -1, use current schedule.
     */
    virtual bool isCritical( long id = -1 ) const;
    
    /**
     * Set current schedule to schedule with identity id, for me and my children.
     * @param id Schedule identity
     */
    virtual void setCurrentSchedule(long id);
    
    /**
     * The assigned resources can not fullfill the estimated effort.
     * @param id Schedule identity. If id is -1, use current schedule.
     */
    virtual bool effortMetError( long id = -1 ) const;
    
    Completion &completion() { return m_completion; }
    
    ResourceRequestCollection *requests() const { return m_requests; }
    
protected:
    /// Check if this node has any dependent child nodes
    virtual bool isEndNode() const;
    /// Check if this node has any dependent parent nodes
    virtual bool isStartNode() const;
    
    virtual void initiateCalculation(MainSchedule &sch);
    /**
     * Sets up the lists used for calculation.
     * This includes adding summarytasks relations to subtasks
     * and lists for start- and endnodes.
     */
    virtual void initiateCalculationLists(MainSchedule &sch);
    /**
     * Calculates early start and early finish, first for all predeccessors,
     * then for this task.
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     */
    virtual DateTime calculateForward(int use);
    /**
     * Calculates ref m_durationForward from ref earliestStart and
     * returns the resulting end time (early finish),
     * which will be used as the succesors ref earliestStart.
     *
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     */
    virtual DateTime calculateEarlyFinish(int use);
    /**
     * Calculates late start and late finish, first for all successors,
     * then for this task.
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     */
    virtual DateTime calculateBackward(int use);
    /**
     * Calculates ref m_durationBackward from ref latestFinish and
     * returns the resulting start time (late start),
     * which will be used as the predecessors ref latestFinish.
     *
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     */
    virtual DateTime calculateLateStart(int use);
    /**
     * Schedules the task within the limits of earliestStart and latestFinish.
     * Calculates ref m_startTime, ref m_endTime and ref m_duration,
     * Assumes ref calculateForward() and ref calculateBackward() has been run.
     *
     * @param earliest The task is not scheduled to start earlier than this
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     * @return The tasks endtime which can be used for scheduling the successor.
     */
    virtual DateTime scheduleForward(const DateTime &earliest, int use);
    /**
     * Schedules the task within the limits of start time and latestFinish,
     * Calculates end time and duration.
     * Assumes ref calculateForward() and ref calculateBackward() has been run.
     *
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     * @return The tasks endtime which can be used for scheduling the successor.
     */
    virtual DateTime scheduleFromStartTime(int use);
    /**
     * Schedules the task within the limits of earliestStart and latestFinish.
     * Calculates ref m_startTime, ref m_endTime and ref m_duration,
     * Assumes ref calculateForward() and ref calculateBackward() has been run.
     *
     * @param latest The task is not scheduled to end later than this
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     * @return The tasks starttime which can be used for scheduling the predeccessor.
     */
    virtual DateTime scheduleBackward(const DateTime &latest, int use);
    /**
     * Schedules the task within the limits of end time and latestFinish.
     * Calculates endTime and duration.
     * Assumes ref calculateForward() and ref calculateBackward() has been run.
     *
     * @param latest The task is not scheduled to end later than this
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     * @return The tasks starttime which can be used for scheduling the predeccessor.
     */
    virtual DateTime scheduleFromEndTime(int use);
    
    /**
     * Summarytasks (with milestones) need special treatment because 
     * milestones are always 'glued' to their predecessors.
     */
    virtual void adjustSummarytask();
    
    /**
     * Return the duration calculated on bases of the requested resources
     */
    virtual Duration calcDuration(const DateTime &time, const Duration &effort, bool backward);

    /// Calculate the critical path
    virtual bool calcCriticalPath(bool fromEnd);
    virtual void calcFreeFloat();
    
    // Proxy relations are relations to/from summarytasks. 
    // These relations are distributed to the child tasks before calculation.
    virtual void clearProxyRelations();
    virtual void addParentProxyRelations(QList<Relation*> &);
    virtual void addChildProxyRelations(QList<Relation*> &);
    virtual void addParentProxyRelation(Node *, const Relation *);
    virtual void addChildProxyRelation(Node *, const Relation *);

    // Those method are used by the pert editor to link tasks between each others
    virtual void addRequiredTask(Node * taskLinked);
    virtual void remRequiredTask(QString id);
    virtual QList<Node *> requiredTaskIterator() const;

    DateTime EarlyStartDate();
    void setEarlyStartDate(DateTime value);

    DateTime EarlyFinishDate();
    void setEarlyFinishDate(DateTime value);

    DateTime LateStartDate();
    void setLateStartDate(DateTime value);

    DateTime LateFinishDate();
    void setLateFinishDate(DateTime value);

    int ActivitySlack();
    void setActivitySlack(int value);

    int ActivityFreeMargin();
    void setActivityFreeMargin(int value);

private:
    DateTime calculateSuccessors(const QList<Relation*> &list, int use);
    DateTime calculatePredeccessors(const QList<Relation*> &list, int use);
    DateTime scheduleSuccessors(const QList<Relation*> &list, int use);
    DateTime schedulePredeccessors(const QList<Relation*> &list, int use);
    
    // Check appointments if specified in currentschedule
    DateTime workStartAfter(const DateTime &dt);
    DateTime workFinishBefore(const DateTime &dt);
    // Don't check for appointments
    DateTime workTimeAfter(const DateTime &dt) const;
    DateTime workTimeBefore(const DateTime &dt) const;
    
private:
    QList<ResourceGroup*> m_resource;

    ResourceRequestCollection *m_requests;
 
    QList<Relation*> m_parentProxyRelations;
    QList<Relation*> m_childProxyRelations;
    
    // This list store pointers to linked task
    QList<Node*> m_requiredTasks;

    // Attributes used for calculation
    DateTime m_earlyStartDate;
    DateTime m_earlyFinishDate;
    DateTime m_lateStartDate;
    DateTime m_lateFinishDate;
    int m_activitySlack;
    int m_activityFreeMargin;

    Completion m_completion;
    
#ifndef NDEBUG
public:
    void printDebug(bool children, const QByteArray& indent);
#endif

};

}  //KPlato namespace

#endif
