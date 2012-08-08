/* This file is part of the KDE project
 Copyright (C) 2005 - 2011 Dag Andersen <danders@get2net.dk>

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
 Boston, MA 02110-1301, USA.
*/

#ifndef KPTSCHEDULE_H
#define KPTSCHEDULE_H

#include "kplatokernel_export.h"

#include "kptglobal.h"
#include "kptcalendar.h"
#include "kpteffortcostmap.h"
#include "kptresource.h"

#include <QList>
#include <QMap>
#include <QString>

//#include "KoXmlReaderForward.h"
class QDomElement;
class QStringList;


/// The main namespace
namespace KPlato
{

class Appointment;
class DateTime;
class Duration;
class Node;
class Project;
class Task;
class ScheduleManager;
class XMLLoaderObject;
class SchedulerPlugin;
class KPlatoXmlLoaderBase;

/**
 * The Schedule class holds data calculated during project
 * calculation and scheduling, eg start- and end-times and
 * appointments.
 * There is one Schedule per node (tasks and project ) and one per resource.
 * Schedule is subclassed into:
 * MainSchedule     Used by the main project.
 * NodeSchedule     Used by all other nodes (tasks).
 * ResourceSchedule Used by resources.
 */
class KPLATOKERNEL_EXPORT Schedule
{
public:
    //NOTE: Must match Effort::Use atm.
    enum Type { Expected = 0   //Effort::Use_Expected
              };

    Schedule();
    Schedule( Schedule *parent );
    Schedule( const QString& name, Type type, long id );
    virtual ~Schedule();

    QString name() const { return m_name; }
    void setName( const QString& name ) { m_name = name; }
    Type type() const { return m_type; }
    void setType( Type type ) { m_type = type; }
    void setType( const QString& type );
    QString typeToString( bool translate = false ) const;
    long id() const { return m_id; }
    void setId( long id ) { m_id = id; }
    void setParent( Schedule *parent );
    Schedule *parent() const { return m_parent; }
    virtual bool isDeleted() const;
    virtual void setDeleted( bool on );
    virtual bool recalculate() const { return m_parent == 0 ? false : m_parent->recalculate(); }
    virtual DateTime recalculateFrom() const { return m_parent == 0 ? DateTime() : m_parent->recalculateFrom(); }

    virtual long parentScheduleId() const { return m_parent == 0 ? NOTSCHEDULED : m_parent->parentScheduleId(); }

    virtual Resource *resource() const { return 0; }
    virtual Node *node() const { return 0; }
    
    virtual bool isBaselined() const;
    virtual bool usePert() const;
    
    enum OBState { OBS_Parent, OBS_Allow, OBS_Deny };
    /// Sets whether overbooking resources is allowed locally for this schedule
    /// If @p state is OBS_Parent, the parent is checked when allowOverbooking() is called
    virtual void setAllowOverbookingState( OBState state );
    OBState allowOverbookingState() const;
    virtual bool allowOverbooking() const;
    virtual bool checkExternalAppointments() const;

    bool isCritical() const { return positiveFloat == Duration::zeroDuration; }

    virtual bool loadXML( const KoXmlElement &element, XMLLoaderObject &status );
    virtual void saveXML( QDomElement &element ) const;
    void saveCommonXML( QDomElement &element ) const;
    void saveAppointments( QDomElement &element ) const;

    /// Return the effort available in the @p interval
    virtual Duration effort( const DateTimeInterval &interval ) const;
    virtual DateTimeInterval available( const DateTimeInterval &interval ) const;

    enum CalculationMode { Scheduling, CalculateForward, CalculateBackward };
    /// Set calculation mode
    void setCalculationMode( int mode ) { m_calculationMode = mode; }
    /// Return calculation mode
    int calculationMode() const { return m_calculationMode; }
    /// Return the list of appointments
    QList<Appointment*> appointments() const { return m_appointments; }
    /// Return true if the @p which list is not empty
    bool hasAppointments( int which ) const;
    /// Return the list of appointments
    /// @param which specifies which list is returned
    QList<Appointment*> appointments(int which) const;
    /// Adds appointment to this schedule only
    virtual bool add( Appointment *appointment );
    /// Adds appointment to both this resource schedule and node schedule
    virtual void addAppointment( Schedule * /*other*/, const DateTime & /*start*/, const DateTime & /*end*/, double /*load*/ = 100 ) {}
    /// Removes appointment without deleting it.
    virtual void takeAppointment( Appointment *appointment, int type = Scheduling );
    Appointment *findAppointment( Schedule *resource, Schedule *node, int type = Scheduling );
    /// Attach the appointment to appropriate list (appointment->calculationMode() specifies list)
    bool attatch( Appointment *appointment );
    
    DateTime appointmentStartTime() const;
    DateTime appointmentEndTime() const;

    virtual Appointment appointmentIntervals( int which = Scheduling, const DateTimeInterval &interval = DateTimeInterval() ) const;
    void copyAppointments( CalculationMode from, CalculationMode to );

    virtual bool isOverbooked() const { return false; }
    virtual bool isOverbooked( const DateTime & /*start*/, const DateTime & /*end*/ ) const { return false; }
    virtual QStringList overbookedResources() const;
    /// Returns the first booked interval to @p node that intersects @p interval (limited to @p interval)
    virtual DateTimeInterval firstBookedInterval( const DateTimeInterval &interval, const Schedule *node ) const;

    /// Return the resources that has appointments to this schedule
    virtual QList<Resource*> resources() const;
    /// Return the resource names that has appointments to this schedule
    virtual QStringList resourceNameList() const;
    
    virtual EffortCostMap bcwsPrDay( EffortCostCalculationType type = ECCT_All ) const;
    virtual EffortCostMap plannedEffortCostPrDay( const QDate &start, const QDate &end, EffortCostCalculationType type = ECCT_All ) const;
    virtual EffortCostMap plannedEffortCostPrDay( const Resource *resource, const QDate &start, const QDate &end, EffortCostCalculationType type = ECCT_All ) const;
    
    /// Returns the total planned effort for @p resource this schedule
    virtual Duration plannedEffort( const Resource *resource, EffortCostCalculationType type = ECCT_All) const;
    /// Returns the total planned effort for this schedule
    virtual Duration plannedEffort( EffortCostCalculationType type = ECCT_All) const;
    /// Returns the total planned effort for this schedule on date
    virtual Duration plannedEffort( const QDate &date, EffortCostCalculationType type = ECCT_All ) const;
    /// Returns the planned effort for @p resource on the @p date date
    virtual Duration plannedEffort( const Resource *resource, const QDate &date, EffortCostCalculationType type = ECCT_All ) const;
    /// Returns the planned effort up to and including date
    virtual Duration plannedEffortTo( const QDate &date, EffortCostCalculationType type = ECCT_All ) const;
    /// Returns the planned effort for @p resource up to and including date
    virtual Duration plannedEffortTo( const Resource *resource, const QDate &date, EffortCostCalculationType type = ECCT_All ) const;

    /**
     * Planned cost is the sum total of all resources and other costs
     * planned for this node.
     */
    virtual EffortCost plannedCost( EffortCostCalculationType type = ECCT_All ) const;

    /// Planned cost on date
    virtual double plannedCost( const QDate &date, EffortCostCalculationType type = ECCT_All ) const;
    /**
     * Planned cost from start of activity up to and including date
     * is the sum of all resource costs and other costs planned for this schedule.
     */
    virtual double plannedCostTo( const QDate &date, EffortCostCalculationType type = ECCT_All ) const;
    
    virtual double normalRatePrHour() const { return 0.0; }

    void setEarliestStart( DateTime &dt ) { earlyStart = dt; }
    void setLatestFinish( DateTime &dt ) { lateFinish = dt; }

    virtual void initiateCalculation();
    virtual void calcResourceOverbooked();
    
    virtual void insertHardConstraint( Node * ) {}
    virtual void insertSoftConstraint( Node * ) {}
    virtual void insertForwardNode( Node *node );
    virtual void insertBackwardNode( Node *node );
    virtual void insertStartNode( Node * ) {}
    virtual void insertEndNode( Node * ) {}
    virtual void insertSummaryTask( Node * ) {}

    void setScheduled( bool on );
    bool isScheduled() const { return !notScheduled; }

    DateTime start() const { return startTime; }
    DateTime end() const { return endTime; }

    QStringList state() const;

    void setResourceError( bool on ) { resourceError = on; }
    void setResourceOverbooked( bool on ) { resourceOverbooked = on; }
    void setResourceNotAvailable( bool on ) { resourceNotAvailable = on; }
    void setConstraintError( bool on ) { constraintError = on; }
    void setNotScheduled( bool on ) { notScheduled = on; }
    void setSchedulingError( bool on ) { schedulingError = on; }

    void setPositiveFloat( const Duration &f ) { positiveFloat = f; }
    void setNegativeFloat( const Duration &f ) { negativeFloat = f; }
    void setFreeFloat( const Duration &f ) { freeFloat = f; }

    void setInCriticalPath( bool on = true ) { inCriticalPath = on; }

    virtual ScheduleManager *manager() const { return 0; }
    
    class KPLATOKERNEL_EXPORT Log {
        public:
            enum Type { Type_Debug = 0, Type_Info, Type_Warning, Type_Error };
            Log() 
                : node( 0 ), resource( 0 ), severity( 0 ), phase( -1 )
            {}
            Log( const Node *n, int sev, const QString &msg, int ph = -1 );
            Log( const Node *n, const Resource *r, int sev, const QString &msg, int ph = -1 );

            const Node *node;
            const Resource *resource;
            QString message;
            int severity;
            int phase;

            QString formatMsg() const;
    };
    virtual void addLog( Log &log );
    virtual void clearLogs() {};
    virtual void logError( const QString &, int = -1 ) {}
    virtual void logWarning( const QString &, int = -1 ) {}
    virtual void logInfo( const QString &, int = -1 ) {}
    virtual void logDebug( const QString &, int = -1 ) {}
    
    virtual void incProgress() { if ( m_parent ) m_parent->incProgress(); }

protected:
    virtual void changed( Schedule * /*sch*/ ) {}
    
protected:
    QString m_name;
    Type m_type;
    long m_id;
    bool m_deleted;
    Schedule *m_parent;
    OBState m_obstate;

    int m_calculationMode;
    QList<Appointment*> m_appointments;
    QList<Appointment*> m_forward;
    QList<Appointment*> m_backward;

    friend class Node;
    friend class Task;
    friend class Project;
    friend class Resource;
    friend class RecalculateProjectCmd;
    friend class ScheduleManager;
    friend class KPlatoXmlLoaderBase;
    /**
      * earlyStart is calculated by PERT/CPM.
      * A task may be scheduled to start later because of constraints
      * or resource availability etc.
      */
    DateTime earlyStart;
    /**
      * lateStart is calculated by PERT/CPM.
      * A task may not be scheduled to start later.
      */
    DateTime lateStart;
    /**
      * earlyFinish is calculated by PERT/CPM.
      * A task may not be scheduled to finish earlier.
      */
    DateTime earlyFinish;
    /**
      * lateFinish is calculated by PERT/CPM.
      * A task may be scheduled to finish earlier because of constraints
      * or resource availability etc.
      */
    DateTime lateFinish;
    /**  startTime is the scheduled start time.
      *  It depends on constraints (i.e. ASAP/ALAP) and resource availability.
      *  It will always be later or equal to earliestStart
      */
    DateTime startTime;
    /**
      *  m_endTime is the scheduled finish time.
      *  It depends on constraints (i.e. ASAP/ALAP) and resource availability.
      *  It will always be earlier or equal to latestFinish
      */
    DateTime endTime;
    /**
      *  duration is the scheduled duration which depends on
      *  e.g. estimated effort, allocated resources and risk
      */
    Duration duration;

    /// Set if EffortType == Effort, but no resource is requested
    bool resourceError;
    /// Set if the assigned resource is overbooked
    bool resourceOverbooked;
    /// Set if the requested resource is not available
    bool resourceNotAvailable;
    /// Set if the task cannot be scheduled to fullfil all the constraints
    bool constraintError;
    /// Set if the node has not been scheduled
    bool notScheduled;
    /// Set if the assigned resource cannot deliver the requered estimated effort
    bool effortNotMet;
    /// Set if some other scheduling error occurs
    bool schedulingError;

    DateTime workStartTime;
    DateTime workEndTime;
    bool inCriticalPath;
    
    Duration positiveFloat;
    Duration negativeFloat;
    Duration freeFloat;
};

/**
 * NodeSchedule holds scheduling information for a node (task).
 * 
 */
class KPLATOKERNEL_EXPORT NodeSchedule : public Schedule
{
public:
    NodeSchedule();
    NodeSchedule( Node *node, const QString& name, Schedule::Type type, long id );
    NodeSchedule( Schedule *parent, Node *node );
    virtual ~NodeSchedule();

    virtual bool isDeleted() const
    { return m_parent == 0 ? true : m_parent->isDeleted(); }
    void setDeleted( bool on );

    virtual bool loadXML( const KoXmlElement &element, XMLLoaderObject &status );
    virtual void saveXML( QDomElement &element ) const;

    // tasks------------>
    virtual void addAppointment( Schedule *resource, const DateTime &start, const DateTime &end, double load = 100 );
    virtual void takeAppointment( Appointment *appointment, int type = Schedule::Scheduling );

    virtual Node *node() const { return m_node; }
    virtual void setNode( Node *n ) { m_node = n; }

    /// Return the resources that has appointments to this schedule
    virtual QList<Resource*> resources() const;
    /// Return the resource names that has appointments to this schedule
    virtual QStringList resourceNameList() const;

    virtual void logError( const QString &msg, int phase = -1 );
    virtual void logWarning( const QString &msg, int phase = -1 );
    virtual void logInfo( const QString &msg, int phase = -1 );
    virtual void logDebug( const QString &, int = -1 );

protected:
    void init();

private:
    Node *m_node;
};

/**
 * ResourceSchedule holds scheduling information for a resource.
 * 
 */
class KPLATOKERNEL_EXPORT ResourceSchedule : public Schedule
{
public:
    ResourceSchedule();
    ResourceSchedule( Resource *Resource, const QString& name, Schedule::Type type, long id );
    ResourceSchedule( Schedule *parent, Resource *Resource );
    virtual ~ResourceSchedule();

    virtual bool isDeleted() const
    { return m_parent == 0 ? true : m_parent->isDeleted(); }
    virtual void addAppointment( Schedule *node, const DateTime &start, const DateTime &end, double load = 100 );
    virtual void takeAppointment( Appointment *appointment, int type = Scheduling );

    virtual bool isOverbooked() const;
    virtual bool isOverbooked( const DateTime &start, const DateTime &end ) const;

    virtual Resource *resource() const { return m_resource; }
    virtual double normalRatePrHour() const;

    /// Return the effort available in the @p interval
    virtual Duration effort( const DateTimeInterval &interval ) const;
    virtual DateTimeInterval available( const DateTimeInterval &interval ) const;
    
    virtual void logError( const QString &msg, int phase = -1 );
    virtual void logWarning( const QString &msg, int phase = -1 );
    virtual void logInfo( const QString &msg, int phase = -1 );
    virtual void logDebug( const QString &, int = -1 );

    void setNodeSchedule( const Schedule *sch ) { m_nodeSchedule = sch; }
    
private:
    Resource *m_resource;
    Schedule *m_parent;
    const Schedule *m_nodeSchedule; // used during scheduling
};

/**
 * MainSchedule holds scheduling information for the main project node.
 * 
 */
class KPLATOKERNEL_EXPORT MainSchedule : public NodeSchedule
{
public:
    MainSchedule();
    MainSchedule( Node *node, const QString& name, Schedule::Type type, long id );
    ~MainSchedule();
    virtual bool isDeleted() const { return m_deleted; }
    
    virtual bool isBaselined() const;
    virtual bool allowOverbooking() const;
    virtual bool checkExternalAppointments() const;
    virtual bool usePert() const;

    virtual bool loadXML( const KoXmlElement &element, XMLLoaderObject &status );
    virtual void saveXML( QDomElement &element ) const;

    void setManager( ScheduleManager *sm ) { m_manager = sm; }
    ScheduleManager *manager() const { return m_manager; }
    virtual bool recalculate() const;
    virtual DateTime recalculateFrom() const;
    virtual long parentScheduleId() const;
    
    DateTime calculateForward( int use );
    DateTime calculateBackward( int use );
    DateTime scheduleForward( const DateTime &earliest, int use );
    DateTime scheduleBackward( const DateTime &latest, int use );
    
    void clearNodes() { 
        m_hardconstraints.clear(); 
        m_softconstraints.clear(); 
        m_forwardnodes.clear(); 
        m_backwardnodes.clear();
        m_startNodes.clear();
        m_endNodes.clear();
        m_summarytasks.clear();
    }
    virtual void insertHardConstraint( Node *node ) { m_hardconstraints.append( node ); }
    QList<Node*> hardConstraints() const { return m_hardconstraints; }
    virtual void insertSoftConstraint( Node *node ) { m_softconstraints.append( node ); }
    QList<Node*> softConstraints() const { return m_softconstraints; }
    QList<Node*> forwardNodes() const { return m_forwardnodes; }
    virtual void insertForwardNode( Node *node ) { m_forwardnodes.append( node ); }
    QList<Node*> backwardNodes() const { return m_backwardnodes; }
    virtual void insertBackwardNode( Node *node ) { m_backwardnodes.append( node ); }
    virtual void insertStartNode( Node *node ) { m_startNodes.append( node ); }
    QList<Node*> startNodes() const { return m_startNodes; }
    virtual void insertEndNode( Node *node ) { m_endNodes.append( node ); }
    QList<Node*> endNodes() const { return m_endNodes; }
    virtual void insertSummaryTask( Node *node ) { m_summarytasks.append( node ); }
    QList<Node*> summaryTasks() const { return m_summarytasks; }
    
    void clearCriticalPathList();
    QList<Node*> *currentCriticalPath() const;
    void addCriticalPath( QList<Node*> *lst = 0 );
    const QList< QList<Node*> > *criticalPathList() const { return &(m_pathlists); }
    QList<Node*> criticalPath( int index = 0 ) {
        QList<Node*> lst;
        return m_pathlists.count() <= index ? lst : m_pathlists[ index ];
    }
    void addCriticalPathNode( Node *node );
    
    QList<Schedule::Log> logs() const;
    void setLog( const QList<Schedule::Log> &log ) { m_log = log; }
    virtual void addLog( Schedule::Log &log );
    virtual void clearLogs() { m_log.clear(); m_logPhase.clear(); }
    
    void setPhaseName( int phase, const QString &name ) { m_logPhase[ phase ] = name; }
    QString logPhase( int phase ) const { return m_logPhase.value( phase ); }
    static QString logSeverity( int severity );
    QMap<int, QString> phaseNames() const { return m_logPhase; }
    void setPhaseNames( const QMap<int, QString> &pn ) { m_logPhase = pn; }
    
    virtual void incProgress();

    QStringList logMessages() const;

    QList< QList<Node*> > m_pathlists;
    bool criticalPathListCached;

protected:
    virtual void changed( Schedule *sch );

private:
    friend class Project;
    
    ScheduleManager *m_manager;
    QList<Node*> m_hardconstraints;
    QList<Node*> m_softconstraints;
    QList<Node*> m_forwardnodes;
    QList<Node*> m_backwardnodes;
    QList<Node*> m_startNodes;
    QList<Node*> m_endNodes;
    QList<Node*> m_summarytasks;
    
    QList<Node*> *m_currentCriticalPath;
    
    
    QList<Schedule::Log> m_log;
    QMap<int, QString> m_logPhase;
};

/**
 * ScheduleManager is used by the Project class to manage the schedules.
 * The ScheduleManager is the bases for the user interface to scheduling.
 * A ScheduleManager can have child manager(s).
 */
class KPLATOKERNEL_EXPORT ScheduleManager : public QObject
{
    Q_OBJECT
public:
    enum CalculationResult { CalculationRunning = 0, CalculationDone, CalculationStopped, CalculationCanceled, CalculationError };

    explicit ScheduleManager( Project &project, const QString name = QString() );
    ~ScheduleManager();
    
    void setName( const QString& name );
    QString name() const { return m_name; }

    void setManagerId( const QString &id ) { m_id = id; }
    QString managerId() const { return m_id; }

    Project &project() const { return m_project; }
    
    void setParentManager( ScheduleManager *sm, int index = -1 );
    ScheduleManager *parentManager() const { return m_parent; }
    
    long scheduleId() const { return m_expected == 0 ? NOTSCHEDULED : m_expected->id(); }
    
    int removeChild( const ScheduleManager *sm );
    void insertChild( ScheduleManager *sm, int index = -1 );
    QList<ScheduleManager*> children() const { return m_children; }
    int childCount() const { return m_children.count(); }
    ScheduleManager *childAt( int index ) const { return m_children.value( index ); }
    /// Return list of all child managers (also childrens children)
    QList<ScheduleManager*> allChildren() const;
    int indexOf( const ScheduleManager* child ) const;
    bool isParentOf( const ScheduleManager *sm ) const;
    ScheduleManager *findManager( const QString &name ) const;
    
    /// This sub-schedule will be re-calculated based on the parents completion data
    bool recalculate() const { return m_recalculate; }
    /// Set re-calculate to @p on.
    void setRecalculate( bool on ) { m_recalculate = on; }
    /// The datetime this schedule will be calculated from
    DateTime recalculateFrom() const { return m_recalculateFrom; }
    /// Set the datetime this schedule will be calculated from to @p dt
    void setRecalculateFrom( const DateTime &dt ) { m_recalculateFrom = dt; }
    long parentScheduleId() const { return m_parent == 0 ? NOTSCHEDULED : m_parent->scheduleId(); }
    void createSchedules();
    
    void setDeleted( bool on );
    
    bool isScheduled() const { return m_expected == 0 ? false :  m_expected->isScheduled(); }

    void setExpected( MainSchedule *sch );
    MainSchedule *expected() const { return m_expected; }

    QStringList state() const;

    void setBaselined( bool on );
    bool isBaselined() const { return m_baselined; }
    bool isChildBaselined() const;
    void setAllowOverbooking( bool on );
    bool allowOverbooking() const;
    
    void setCheckExternalAppointments( bool on );
    bool checkExternalAppointments() const;

    void setUsePert( bool on );
    bool usePert() const { return m_usePert; }

    void setSchedulingDirection( bool on );
    bool schedulingDirection() const { return m_schedulingDirection; }

    void setScheduling( bool on );
    bool scheduling() const { return m_scheduling; }

    bool loadXML( KoXmlElement &element, XMLLoaderObject &status );
    void saveXML( QDomElement &element ) const;
    
    /// Save a workpackage document
    void saveWorkPackageXML( QDomElement &element, const Node &node ) const;
            
    void scheduleChanged( MainSchedule *sch );
    
    const QList<SchedulerPlugin*> schedulerPlugins() const;
    QString schedulerPluginId() const;
    void setSchedulerPluginId( const QString &id );
    SchedulerPlugin *schedulerPlugin() const;
    QStringList schedulerPluginNames() const;
    int schedulerPluginIndex() const;
    void setSchedulerPlugin( int index );

    /// Stop calculation. Use result if possible.
    void stopCalculation();
    /// Terminate calculation. Forget any results.
    void haltCalculation();
    void calculateSchedule();
    int calculationResult() const { return m_calculationresult; }
    void setCalculationResult( int r ) { m_calculationresult = r; }

    /// Increments progress in the project
    void incProgress();
    /// Returns current progress
    int progress() const { return m_progress; }
    /// Returns maximum progress value
    int maxProgress() const { return m_maxprogress; }

    /// Log added by MainSchedule
    /// Emits sigLogAdded() to enable synchronization between schedules
    void logAdded( Schedule::Log &log );

    /// Create and load a MainSchedule
    MainSchedule *loadMainSchedule( KoXmlElement &element, XMLLoaderObject &status );

    /// Load an existing MainSchedule
    bool loadMainSchedule( MainSchedule *schedule, KoXmlElement &element, XMLLoaderObject &status );

    QMap< int, QString > phaseNames() const;

public slots:
    /// Set maximum progress. Emits signal maxProgressChanged
    void setMaxProgress( int value );
    /// Set progress. Emits signal progressChanged
    void setProgress( int value );

    /// Add the lis of logs @p log to expected()
    void slotAddLog( const QList<KPlato::Schedule::Log> &log );

    void setPhaseNames( const QMap<int, QString> &phasenames );

signals:
    void maxProgressChanged( int );
    void progressChanged( int );

    /// Emitted by logAdded() when new log entries are added
    void logInserted( MainSchedule*, int firstrow, int lastrow );

    /// Emitted by logAdded()
    /// Used by scheduling thread
    void sigLogAdded( Schedule::Log log );

protected:
    Project &m_project;
    ScheduleManager *m_parent;
    QString m_name;
    QString m_id;
    bool m_baselined;
    bool m_allowOverbooking;
    bool m_checkExternalAppointments;
    bool m_usePert;
    bool m_recalculate;
    DateTime m_recalculateFrom;
    bool m_schedulingDirection;
    bool m_scheduling;
    int m_progress;
    int m_maxprogress;
    MainSchedule *m_expected;
    QList<ScheduleManager*> m_children;

    QString m_schedulerPluginId;
    
    int m_calculationresult;
};


} //namespace KPlato

KPLATOKERNEL_EXPORT QDebug operator<<( QDebug dbg, const KPlato::Schedule::Log &log );

#endif
