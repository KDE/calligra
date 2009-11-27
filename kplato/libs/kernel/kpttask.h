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

#include "kplatokernel_export.h"

#include "kptnode.h"
#include "kptglobal.h"
#include "kptdatetime.h"
#include "kptduration.h"
#include "kptresource.h"

#include <QList>
#include <QMap>
#include <QPair>

/// The main namespace.
namespace KPlato
{

class Completion;

/**
 * The Completion class holds information about the tasks progress.
 */
class KPLATOKERNEL_EXPORT Completion
{
    
public:
    class KPLATOKERNEL_EXPORT UsedEffort
    {
        public:
            class KPLATOKERNEL_EXPORT ActualEffort : public QPair<Duration, Duration>
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
            /// Returns the total effort up to @p date
            Duration effortTo( const QDate &date ) const;
            /// Returns the total effort on @p date
            ActualEffort *effort( const QDate &date ) const { return m_actual.value( date ); }
            ActualEffort *takeEffort( const QDate &date ) { return m_actual.take( date ); }
            /// Returns the total effort for all registered dates
            Duration effort() const;
            const QMap<QDate, ActualEffort*> &actualEffortMap() const { return m_actual; }
            
            /// Load from document
            bool loadXML(KoXmlElement &element, XMLLoaderObject &status );
            /// Save to document
            void saveXML(QDomElement &element) const;
            bool contains( const QDate &date ) const { return m_actual.contains( date ); }

        private:
            QMap<QDate, ActualEffort*> m_actual;
    };
    typedef QMap<QDate, UsedEffort::ActualEffort*> DateUsedEffortMap;
    
    class KPLATOKERNEL_EXPORT Entry
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
            bool operator!=( const Entry &e ) const { return ! operator==( e ); }
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
    
    explicit Completion( Node *node = 0 );  // review * or &, or at all?
    Completion( const Completion &copy );
    virtual ~Completion();
    
    bool operator==(const Completion &p);
    bool operator!=(Completion &p) { return !(*this == p); }
    Completion &operator=(const Completion &p);
    
    /// Load from document
    bool loadXML(KoXmlElement &element, XMLLoaderObject &status );
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
    void setPercentFinished( const QDate &date, int value );
    void setRemainingEffort( const QDate &date, const Duration &value );
    void setActualEffort( const QDate &date, const Duration &value );
    
    /// Return a list of the resource that has done any work on this task
    QList<const Resource*> resources() { return m_usedEffort.keys(); }
    
    const EntryList &entries() const { return m_entries; }
    void addEntry( const QDate &date, Entry *entry );
    Entry *takeEntry( const QDate &date ) { return m_entries.take( date ); changed(); }
    Entry *entry( const QDate &date ) const { return m_entries[ date ]; }
    
    /// Returns the date of the latest entry
    QDate entryDate() const;
    /// Returns the percentFinished of the latest entry
    int percentFinished() const;
    /// Returns the percentFinished on @p date
    int percentFinished( const QDate &date ) const;
    /// Returns the estimated remaining effort
    Duration remainingEffort() const;
    /// Returns the estimated remaining effort on @p date
    Duration remainingEffort( const QDate &date ) const;
    /// Returns the total actual effort
    Duration actualEffort() const;
    /// Returns the total actual effort on @p date
    Duration actualEffort( const QDate &date ) const;
    /// Returns the total actual effort upto and including @p date
    Duration actualEffortTo( const QDate &date ) const;
    /// Returns the actual effort for @p resource on @p date
    Duration actualEffort( const Resource *resource, const QDate &date ) const;
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
    /// Returns the total actual effort and cost upto and including @p date
    EffortCost actualCostTo( const QDate &date ) const;
    
    /**
     * Returns a map of all actual effort and cost entered
     */
    virtual EffortCostMap actualEffortCost( long id ) const;

    void addUsedEffort( const Resource *resource, UsedEffort *value = 0 );
    UsedEffort *takeUsedEffort( const Resource *r ) { return m_usedEffort.take( const_cast<Resource*>( r ) ); changed(); }
    UsedEffort *usedEffort( const Resource *r ) const { return m_usedEffort.value( const_cast<Resource*>( r ) ); }
    const ResourceUsedEffortMap &usedEffortMap() const { return m_usedEffort; }
    
    void changed();
    Node *node() const { return m_node; }
    void setNode( Node *node ) { m_node = node; }
    
    enum Entrymode { FollowPlan, EnterCompleted, EnterEffortPerTask, EnterEffortPerResource };
    void setEntrymode( Entrymode mode ) { m_entrymode = mode; }
    Entrymode entrymode() const { return m_entrymode; }
    void setEntrymode( const QString &mode );
    QString entryModeToString() const;
    QStringList entrymodeList() const;
    
    EffortCostMap effortCostPrDay(const QDate &start, const QDate &end, long id = -1 ) const;
    
protected:
    void copy( const Completion &copy);
    double averageCostPrHour( const QDate &date, long id ) const;

private:
    Node *m_node;
    bool m_started, m_finished;
    DateTime m_startTime, m_finishTime;
    EntryList m_entries;
    ResourceUsedEffortMap m_usedEffort;
    Entrymode m_entrymode;
    
#ifndef NDEBUG
public:
    void printDebug( const QByteArray &ident ) const;
#endif
};

/**
 * The WorkPackage class controls work flow for a task
 */
class KPLATOKERNEL_EXPORT WorkPackage
{
public:

    /// @enum WPTransmitionStatus describes if this package was sent or received
    enum WPTransmitionStatus {
        TS_None,        /// Not sent nor received
        TS_Send,        /// Package was sent to resource
        TS_Receive      /// Package was received from resource
    };

    explicit WorkPackage( Task *task = 0 );
    explicit WorkPackage( const WorkPackage &wp );
    virtual ~WorkPackage();

    Task *parentTask() const { return m_task; }
    void setParentTask( Task *task ) { m_task = task; }

    /// Returns the transmission status of this package
    WPTransmitionStatus transmitionStatus() const { return m_transmitionStatus; }
    void setTransmitionStatus( WPTransmitionStatus sts ) { m_transmitionStatus = sts; }
    static QString transmitionStatusToString( WPTransmitionStatus sts, bool trans = false );
    static WPTransmitionStatus transmitionStatusFromString( const QString &sts );
    
    /// Load from document
    virtual bool loadXML(KoXmlElement &element, XMLLoaderObject &status );
    /// Save the full workpackage
    virtual void saveXML(QDomElement &element) const;

    /// Load from document
    virtual bool loadLoggedXML(KoXmlElement &element, XMLLoaderObject &status );
    /// Save the full workpackage
    virtual void saveLoggedXML(QDomElement &element) const;

    /// Set schedule manager
    void setScheduleManager( ScheduleManager *sm );
    /// Return schedule manager
    ScheduleManager *scheduleManager() const { return m_manager; }
    /// Return the schedule id, or NOTSCHEDULED if no schedule manager is set
    long id() const { return m_manager ? m_manager->id() : NOTSCHEDULED; }

    Completion &completion();
    const Completion &completion() const;

    void addLogEntry( DateTime &dt, const QString &str );
    const QMap<DateTime, QString> &log() const;
    QStringList log();

    /// Return a list of resources fetched from the appointements or requests
    /// merged with resources added to completion
    QList<Resource*> fetchResources();

    /// Return a list of resources fetched from the appointements or requests
    /// merged with resources added to completion
    QList<Resource*> fetchResources( long id );

    /// Returns id of the resource that owns this package. If empty, task leader owns it.
    QString ownerId() const { return m_ownerId; }
    /// Set the resource that owns this package to @p owner. If empty, task leader owns it.
    void setOwnerId( const QString &id ) { m_ownerId = id; }

    /// Returns the name of the resource that owns this package.
    QString ownerName() const { return m_ownerName; }
    /// Set the name of the resource that owns this package.
    void setOwnerName( const QString &name ) { m_ownerName = name; }

    DateTime transmitionTime() const { return m_transmitionTime; }
    void setTransmitionTime( const DateTime &dt ) { m_transmitionTime = dt; }

private:
    Task *m_task;
    ScheduleManager *m_manager;
    Completion m_completion;
    QString m_ownerName;
    QString m_ownerId;
    WPTransmitionStatus m_transmitionStatus;
    DateTime m_transmitionTime;

    QMap<DateTime, QString> m_log;
};

class KPLATOKERNEL_EXPORT WorkPackageSettings
{
public:
    WorkPackageSettings();
    bool loadXML( const KoXmlElement &element );
    void saveXML( QDomElement &element) const;
    bool operator==( const WorkPackageSettings &settings ) const;
    bool operator!=( const WorkPackageSettings &settings ) const;
    bool usedEffort;
    bool progress;
    bool remainingEffort;
    bool documents;
};

/**
  * A task in the scheduling software is represented by this class. A task
  * can be anything from 'build house' to 'drill hole' It will always mean
  * an activity.
  */
class KPLATOKERNEL_EXPORT Task : public Node {
    Q_OBJECT
public:
    Task(Node *parent = 0);
    Task(const Task &task, Node *parent = 0);
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
    ResourceGroupRequest *resourceGroupRequest(const ResourceGroup *group) const;
    void clearResourceRequests();
    void addRequest(ResourceGroup *group, int numResources);
    void addRequest(ResourceGroupRequest *request);
    void takeRequest(ResourceGroupRequest *request);
//    int units() const;
    int workUnits() const;
    void makeAppointments();
    virtual QStringList requestNameList() const;
    virtual QList<Resource*> requestedResources() const;
    virtual bool containsRequest( const QString &/*identity*/ ) const;
    virtual ResourceRequest *resourceRequest( const QString &/*name*/ ) const;
    
    /// Return the list of resources assigned to this task
    virtual QStringList assignedNameList( long id = CURRENTSCHEDULE ) const;

    /**
     * Calculates if the assigned resource is overbooked 
     * within the duration of this task
     */
    void calcResourceOverbooked();
    
    void setConstraint(Node::ConstraintType type);

    /// Load from document
    virtual bool load(KoXmlElement &element, XMLLoaderObject &status );
    /// Save to document
    virtual void save(QDomElement &element) const;
    /// Save appointments for schedule with id
    virtual void saveAppointments(QDomElement &element, long id) const;
    
    /// Save a workpackage document with schedule identity @p id
    void saveWorkPackageXML( QDomElement &element, long id ) const;

    /**
     * Returns a list of planned effort and cost for this task
     * for the interval start, end inclusive
     */
    virtual EffortCostMap plannedEffortCostPrDay(const QDate &start, const QDate &end,  long id = CURRENTSCHEDULE ) const;
    
    /// Returns the total planned effort for this task (or subtasks) 
    virtual Duration plannedEffort( long id = CURRENTSCHEDULE ) const;
    /// Returns the total planned effort for this task (or subtasks) on date
    virtual Duration plannedEffort(const QDate &date, long id = CURRENTSCHEDULE ) const;
    /// Returns the planned effort up to and including date
    virtual Duration plannedEffortTo(const QDate &date, long id = CURRENTSCHEDULE ) const;
    
    /// Returns the total actual effort for this task (or subtasks) 
    virtual Duration actualEffort() const;
    /// Returns the total actual effort for this task (or subtasks) on date
    virtual Duration actualEffort(const QDate &date ) const;
    /// Returns the actual effort up to and including date
    virtual Duration actualEffortTo(const QDate &date ) const;
    
    /**
     * Returns the total planned cost for this task (or subtasks)
     */
    virtual EffortCost plannedCost( long id = CURRENTSCHEDULE ) const;
    /// Planned cost on date
    virtual double plannedCost(const QDate &/*date*/, long id = CURRENTSCHEDULE ) const;
    /// Planned cost up to and including date
    virtual double plannedCostTo(const QDate &/*date*/, long id = CURRENTSCHEDULE ) const;
    
    /**
     * Returns the actaually reported cost for this task (or subtasks)
     */
    virtual double actualCost() const;
    /// Actual cost on @p date
    virtual double actualCost(const QDate &date) const;
    /// Returns actual effort and cost up to and including @p date
    virtual EffortCost actualCostTo(const QDate &date) const;

    /**
     * Returns a list of actual effort and cost for this task
     * for the interval start, end inclusive
     */
    virtual EffortCostMap actualEffortCostPrDay(const QDate &start, const QDate &end,  long id = CURRENTSCHEDULE ) const;
    
    /// Returns the effort planned to be used to reach the actual percent finished
    virtual Duration budgetedWorkPerformed( const QDate &date, long id = CURRENTSCHEDULE ) const;

    /// Returns the cost planned to be used to reach the actual percent finished
    virtual double budgetedCostPerformed( const QDate &date, long id = CURRENTSCHEDULE ) const;

    /// Return map of Budgeted Cost of Work Scheduled pr day
    virtual EffortCostMap bcwsPrDay( long id = CURRENTSCHEDULE ) const;
    
    /// Budgeted Cost of Work Scheduled
    virtual double bcws( const QDate &date, long id = CURRENTSCHEDULE ) const;

    /// Return map of Budgeted Cost of Work Performed pr day (also includes bcwsPrDay)
    virtual EffortCostMap bcwpPrDay( long id = CURRENTSCHEDULE ) const;
    /// Budgeted Cost of Work Performed
    virtual double bcwp( long id = CURRENTSCHEDULE ) const;
    /// Budgeted Cost of Work Performed ( up to @p date )
    virtual double bcwp( const QDate &date, long id = CURRENTSCHEDULE ) const;

    /// Map of Actual Cost of Work Performed
    virtual EffortCostMap acwp( long id = CURRENTSCHEDULE ) const;
    /// Actual Cost of Work Performed up to dat
    virtual EffortCost acwp( const QDate &date, long id = CURRENTSCHEDULE ) const;

    /// Effort based performance index
    virtual double effortPerformanceIndex( const QDate &date, long id = CURRENTSCHEDULE ) const;

    /// Schedule performance index
    virtual double schedulePerformanceIndex( const QDate &date, long id = CURRENTSCHEDULE ) const;
    /// Cost performance index
    virtual double costPerformanceIndex(const QDate &date, bool *error=0) const;
    
    /**
     * Return the duration that an activity's start can be delayed 
     * without affecting the project completion date. 
     * An activity with positive float is not on the critical path.
     * @param id Schedule identity. If id is CURRENTSCHEDULE, use current schedule.
     */
    Duration positiveFloat( long id = CURRENTSCHEDULE ) const;
    /**
     * Return the duration by which the duration of an activity or path 
     * has to be reduced in order to fullfill a timing- or dependency constraint.
     * @param id Schedule identity. If id is CURRENTSCHEDULE, use current schedule.
     */
    Duration negativeFloat( long id = CURRENTSCHEDULE ) const;
    /**
     * Return the duration by which an activity can be delayed or extended 
     * without affecting the start of any succeeding activity.
     * @param id Schedule identity. If id is CURRENTSCHEDULE, use current schedule.
     */
    Duration freeFloat( long id = CURRENTSCHEDULE ) const;
    /**
     * Return the duration from Early Start to Late Start.
     * @param id Schedule identity. If id is CURRENTSCHEDULE, use current schedule.
     */
    Duration startFloat( long id = CURRENTSCHEDULE ) const;
    /**
     * Return the duration from Early Finish to Late Finish.
     * @param id Schedule identity. If id is CURRENTSCHEDULE, use current schedule.
     */
    Duration finishFloat( long id = CURRENTSCHEDULE ) const;
    
    /**
     * A task is critical if positive float equals 0
     * @param id Schedule identity. If id is CURRENTSCHEDULE, use current schedule.
     */
    virtual bool isCritical( long id = CURRENTSCHEDULE ) const;
    
    /**
     * Set current schedule to schedule with identity id, for me and my children.
     * @param id Schedule identity
     */
    virtual void setCurrentSchedule(long id);
    
    /**
     * The assigned resources can not fullfill the estimated effort.
     * @param id Schedule identity. If id is CURRENTSCHEDULE, use current schedule.
     */
    virtual bool effortMetError( long id = CURRENTSCHEDULE ) const;
    
    Completion &completion() { return m_workPackage.completion(); }
    const Completion &completion() const { return m_workPackage.completion(); }
    
    WorkPackage &workPackage() { return m_workPackage; }
    const WorkPackage &workPackage() const { return m_workPackage; }

    int workPackageLogCount() const { return m_packageLog.count(); }
    QList<WorkPackage*> workPackageLog() const { return m_packageLog; }
    void addWorkPackage( WorkPackage *wp );
    void removeWorkPackage( WorkPackage *wp );
    WorkPackage *workPackageAt( int index ) const;

    QString wpOwnerName() const;
    WorkPackage::WPTransmitionStatus wpTransmitionStatus() const;
    DateTime wpTransmitionTime() const;

    /**
     * Returns the state of the task
     * @param id The identity of the schedule used when calculating the state
     */
    virtual uint state( long id = CURRENTSCHEDULE ) const;

    QList<Relation*> parentProxyRelations() const { return  m_parentProxyRelations; }
    QList<Relation*> childProxyRelations() const { return  m_childProxyRelations; }

signals:
    void workPackageToBeAdded( Node *node, int row );
    void workPackageAdded( Node *node );
    void workPackageToBeRemoved( Node *node, int row );
    void workPackageRemoved( Node *node );

protected:
    /// Copy info from parent schedule
    void copySchedule();
    /// Copy intervals from parent schedule
    void copyAppointments();
    /// Copy intervals from parent schedule in the range @p start, @p end
    void copyAppointments( const DateTime &start, const DateTime &end = DateTime() );
    
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
     * which will be used as the successors ref earliestStart.
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
    
    /// Calculate the critical path
    virtual bool calcCriticalPath(bool fromEnd);
    virtual void calcFreeFloat();
    
    // Proxy relations are relations to/from summarytasks. 
    // These relations are distributed to the child tasks before calculation.
    virtual void clearProxyRelations();
    virtual void addParentProxyRelations( const QList<Relation*> & );
    virtual void addChildProxyRelations( const QList<Relation*> & );
    virtual void addParentProxyRelation(Node *, const Relation *);
    virtual void addChildProxyRelation(Node *, const Relation *);

public:
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

protected:
    /**
     * Calculates and returns the duration of the node.
     * Uses the correct expected-, optimistic- or pessimistic effort
     * dependent on @p use.
     * @param time Where to start calculation.
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     * @param backward If true, time specifies when the task should end.
     */
    virtual Duration duration(const DateTime &time, int use, bool backward);
    /**
     * Return the duration calculated on bases of the requested resources
     */
    Duration calcDuration(const DateTime &time, const Duration &effort, bool backward);

    /**
     * Return the duration calculated on bases of the estimates calendar
     */
    Duration length(const DateTime &time, const Duration &duration, bool backward);

private:
    DateTime calculateSuccessors(const QList<Relation*> &list, int use);
    DateTime calculatePredeccessors(const QList<Relation*> &list, int use);
    DateTime scheduleSuccessors(const QList<Relation*> &list, int use);
    DateTime schedulePredeccessors(const QList<Relation*> &list, int use);
    
    /// Fixed duration: Returns @p dt
    /// Duration with calendar: Returns first available after @p dt
    /// Has working resource(s) allocated: Returns the earliest time a resource can start work after @p dt, and checks appointments if @p sch is not null.
    DateTime workTimeAfter(const DateTime &dt, Schedule *sch = 0) const;
    /// Fixed duration: Returns @p dt
    /// Duration with calendar: Returns first available before @p dt
    /// Has working resource(s) allocated: Returns the latest time a resource can finish work, and checks appointments if @p sch is not null.
    DateTime workTimeBefore(const DateTime &dt, Schedule *sch = 0) const;
    
private:
    QList<ResourceGroup*> m_resource;

    QList<Relation*> m_parentProxyRelations;
    QList<Relation*> m_childProxyRelations;
    
    // This list store pointers to linked task
    QList<Node*> m_requiredTasks;

    WorkPackage m_workPackage;
    QList<WorkPackage*> m_packageLog;

#ifndef NDEBUG
public:
    void printDebug(bool children, const QByteArray& indent);
#endif

};

}  //KPlato namespace

#endif
