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
#include "kptdatetime.h"
#include "kptduration.h"
#include "kptresource.h"

#include <QList>
#include <QMap>
#include <QPair>

/// The main namespace.
namespace KPlato
{

/**
 * The Completion class holds information about the tasks progress.
 */
class Completion
{
    
public:
    class UsedEffort
    {
        public:
            class ActualEffort : public QPair<Duration, Duration>
            {
                public:
                    explicit ActualEffort( const Duration &ne = Duration::zeroDuration, const Duration oe = Duration::zeroDuration )
                        : QPair<Duration, Duration>( ne, oe )
                    {}
                    ActualEffort( const ActualEffort &e )
                        : QPair<Duration, Duration>( e.first, e.second )
                    {}
                    ~ActualEffort() {}
                    Duration normalEffort() const { return first; }
                    void setNormalEffort( const Duration &e ) { first = e; }
                    Duration overtimeEffort() const { return second; }
                    void setOvertimeEffort( const Duration &e ) { second = e; }
                    /// Returns the sum of normalEffort + overtimeEffort
                    Duration effort() const { return first + second; }
                    void setEffort( const Duration &ne, const Duration &oe = Duration::zeroDuration ) { first = ne; second = oe; }
            };
            UsedEffort();
            UsedEffort( const UsedEffort &e );
            ~UsedEffort();
            bool operator==( const UsedEffort &e ) const;
            bool operator!=( const UsedEffort &e ) const { return !operator==( e ); }
            void mergeEffort( const UsedEffort &value );
            void setEffort( const QDate &date, ActualEffort *value );
            /// Returns the total effort on @p date
            ActualEffort *effort( const QDate &date ) const { return m_actual.value( date ); }
            ActualEffort *takeEffort( const QDate &date ) { return m_actual.take( date ); }
            /// Returns the total effort for all registered dates
            Duration effort() const;
            const QMap<QDate, ActualEffort*> &actualEffortMap() const { return m_actual; }
            
            /// Load from document
            bool loadXML(QDomElement &element, XMLLoaderObject &status );
            /// Save to document
            void saveXML(QDomElement &element) const;
        
        private:
            QMap<QDate, ActualEffort*> m_actual;
    };
    typedef QMap<QDate, UsedEffort::ActualEffort*> DateUsedEffortMap;
    
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
            Entry( const Entry &e ) { copy( e ); }
            bool operator==( const Entry &e ) const {
                return percentFinished == e.percentFinished
                    && remainingEffort == e.remainingEffort
                    && totalPerformed == e.totalPerformed
                    && note == e.note;
            }
            Entry &operator=(const Entry &e ) { copy( e ); return *this; }
            
            int percentFinished;
            Duration remainingEffort;
            Duration totalPerformed;
            QString note;
        protected:
            void copy( const Entry &e ) {
                percentFinished = e.percentFinished;
                remainingEffort = e.remainingEffort;
                totalPerformed = e.totalPerformed;
                note = e.note;
            }
    };
    typedef QMap<QDate, Entry*> EntryList;

    typedef QMap<const Resource*, UsedEffort*> ResourceUsedEffortMap;
    
    explicit Completion( Node *node = 0 );
    Completion( const Completion &copy );
    ~Completion();
    
    bool operator==(const Completion &p);
    bool operator!=(Completion &p) { return !(*this == p); }
    Completion &operator=(const Completion &p);
    
    /// Load from document
    bool loadXML(QDomElement &element, XMLLoaderObject &status );
    /// Save to document
    void saveXML(QDomElement &element) const;
    
    bool isStarted() const { return m_started; }
    void setStarted( bool on );
    bool isFinished() const { return m_finished; }
    void setFinished( bool on );
    DateTime startTime() const { return m_startTime; }
    void setStartTime( const DateTime &dt );
    DateTime finishTime() const { return m_finishTime; }
    void setFinishTime( const DateTime &dt );
    
    const EntryList &entries() const { return m_entries; }
    void addEntry( const QDate &date, Entry *entry );
    Entry *takeEntry( const QDate &date ) { return m_entries.take( date ); changed(); }
    Entry *entry( const QDate &date ) const { return m_entries[ date ]; }
    
    /// Returns the date of the latest entry
    QDate entryDate() const;
    /// Returns the percentFinished of the latest entry
    int percentFinished() const;
    /// Returns the estimated remaining effort
    Duration remainingEffort() const;
    /// Returns the total actual effort
    Duration actualEffort() const;
    /// Returns the total actual effort on @ date
    Duration actualEffort( const QDate &date ) const;
    /// Returns the total actual effort upto and including @p date
    Duration actualEffortTo( const QDate &date ) const;
    /// TODO
    QString note() const;
    /// TODO
    void setNote( const QString &str );
    
    /// Returns the total actual cost
    double actualCost() const;
    /// Returns the actual cost on @p date
    double actualCost( const QDate &date ) const;
    /// Returns the total actual cost for @p resource
    double actualCost( const Resource *resource ) const;
    /// Returns the total actual cost upto and including @p date
    double actualCostTo( const QDate &date ) const;
    
    void addUsedEffort( const Resource *resource, UsedEffort *value = 0 );
    UsedEffort *takeUsedEffort( const Resource *r ) { return m_usedEffort.take( const_cast<Resource*>( r ) ); changed(); }
    UsedEffort *usedEffort( const Resource *r ) const { return m_usedEffort.value( const_cast<Resource*>( r ) ); }
    const ResourceUsedEffortMap &usedEffortMap() const { return m_usedEffort; }
    
    void changed();
    Node *node() const { return m_node; }
    
protected:
    void copy( const Completion &copy);
    
private:
    Node *m_node;
    bool m_started, m_finished;
    DateTime m_startTime, m_finishTime;
    EntryList m_entries;
    ResourceUsedEffortMap m_usedEffort;
    
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
class KPLATO_TEST_EXPORT Task : public Node {
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
    virtual Duration plannedEffort( long id = -1 ) const;
    /// Returns the total planned effort for this task (or subtasks) on date
    virtual Duration plannedEffort(const QDate &date, long id = -1 ) const;
    /// Returns the planned effort up to and including date
    virtual Duration plannedEffortTo(const QDate &date, long id = -1 ) const;
    
    /// Returns the total actual effort for this task (or subtasks) 
    virtual Duration actualEffort( long id = -1 ) const;
    /// Returns the total actual effort for this task (or subtasks) on date
    virtual Duration actualEffort(const QDate &date, long id = -1 ) const;
    /// Returns the actual effort up to and including date
    virtual Duration actualEffortTo(const QDate &date, long id = -1 ) const;
    
    /**
     * Returns the total planned cost for this task (or subtasks)
     */
    virtual double plannedCost( long id = -1 ) const;
    /// Planned cost on date
    virtual double plannedCost(const QDate &/*date*/, long id = -1 ) const;
    /// Planned cost up to and including date
    virtual double plannedCostTo(const QDate &/*date*/, long id = -1 ) const;
    
    /**
     * Returns the actaually reported cost for this task (or subtasks)
     */
    virtual double actualCost( long id = -1 ) const;
    /// Actual cost on date
    virtual double actualCost(const QDate &/*date*/, long id = -1 ) const;
    /// Actual cost up to and including date
    virtual double actualCostTo(const QDate &/*date*/, long id = -1 ) const;

    /// Effort based performance index
    double effortPerformanceIndex(const QDate &date, bool *error=0) const;
    /// Cost performance index
    double costPerformanceIndex(const QDate &date, bool *error=0) const;
    
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
    const Completion &completion() const { return m_completion; }
    
    ResourceRequestCollection *requests() const { return m_requests; }
    
    /**
     * Returns the state of the task
     * @param id The identity of the schedule used when calculating the state
     */
    virtual uint state( long id = -1 ) const;

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

public:
    // Those method are used by the pert editor to link tasks between each others

    virtual void addRequiredTask(Node * taskLinked);
    virtual void remRequiredTask(Node * taskLinked);
    QList<Node *> &requiredTaskIterator() {return m_requiredTasks;}

    DateTime earlyStartDate();
    void setEarlyStartDate(DateTime value);

    DateTime earlyFinishDate();
    void setEarlyFinishDate(DateTime value);

    DateTime lateStartDate();
    void setLateStartDate(DateTime value);

    DateTime lateFinishDate();
    void setLateFinishDate(DateTime value);

    int activitySlack();
    void setActivitySlack(int value);

    int activityFreeMargin();
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
