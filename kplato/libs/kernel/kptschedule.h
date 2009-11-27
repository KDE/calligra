/* This file is part of the KDE project
 Copyright (C) 2005 - 2007 Dag Andersen <danders@get2net.dk>

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

/**
 * The Schedule class holds data calculated during project
 * calculation and scheduling, eg start- and end-times and
 * appointments.
 * There is one schedule per node and one per resource.
 * Schedules can be of type Expected, Optimistic or Pessimistic
 * referring to which estimate is used for the calculation.
 * Schedule is subclassed into:
 * MainSchedule     Used by the main project.
 * NodeSchedule     Used by all other nodes (tasks).
 * ResourceSchedule Used by resources.
 */
class KPLATOKERNEL_EXPORT Schedule
{
public:
    //NOTE: Must match Effort::Use atm.
    enum Type { Expected = 0,   //Effort::Use_Expected
                Optimistic = 1,   //Effort::Use_Optimistic
                Pessimistic = 2 //Effort::Use_Pessimistic
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
    virtual void setAllowOverbooking( bool state );
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
    QList<Appointment*> &appointments() { return m_appointments; }
    /// Return the list of appointments
    /// @param which specifies which list is returned
    QList<Appointment*> &appointments( int which);
    /// Adds appointment to this schedule only
    virtual bool add( Appointment *appointment );
    /// Adds appointment to both this resource schedule and node schedule
    virtual void addAppointment( Schedule * /*other*/, DateTime & /*start*/, DateTime & /*end*/, double /*load*/ = 100 ) {}
    /// Removes appointment without deleting it.
    virtual void takeAppointment( Appointment *appointment, int type = Scheduling );
    Appointment *findAppointment( Schedule *resource, Schedule *node, int type = Scheduling );
    /// Attach the appointment to appropriate list (appointment->calculationMode() specifies list)
    bool attatch( Appointment *appointment );
    
    virtual Appointment appointmentIntervals( int which = Scheduling ) const;

    virtual bool isOverbooked() const { return false; }
    virtual bool isOverbooked( const KDateTime & /*start*/, const KDateTime & /*end*/ ) const { return false; }
    virtual QStringList overbookedResources() const;
    /// Returns the first booked interval to @p node that intersects @p interval (limited to @p interval)
    virtual DateTimeInterval firstBookedInterval( const DateTimeInterval &interval, const Schedule *node ) const;

    /// Return the resources that has appointments to this schedule
    virtual QList<Resource*> resources() const;
    /// Return the resource names that has appointments to this schedule
    virtual QStringList resourceNameList() const;
    
    virtual EffortCostMap bcwsPrDay() const;
    virtual EffortCostMap plannedEffortCostPrDay( const QDate &start, const QDate &end ) const;

    /// Returns the total planned effort for this schedule
    virtual Duration plannedEffort() const;
    /// Returns the total planned effort for this schedule on date
    virtual Duration plannedEffort( const QDate &date ) const;
    /// Returns the planned effort up to and including date
    virtual Duration plannedEffortTo( const QDate &date ) const;

    /**
     * Planned cost is the sum total of all resources and other costs
     * planned for this node.
     */
    virtual EffortCost plannedCost() const;

    /// Planned cost on date
    virtual double plannedCost( const QDate &date ) const;
    /**
     * Planned cost from start of activity up to and including date
     * is the sum of all resource costs and other costs planned for this schedule.
     */
    virtual double plannedCostTo( const QDate &date ) const;
    
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
    
    virtual ScheduleManager *manager() const { return 0; }
    
    class Log {
        public:
            enum Type { Type_Debug = 0, Type_Info, Type_Warning, Type_Error };
            Log() 
                : node( 0 ), resource( 0 ), severity( 0 ), phase( -1 )
            {}
            Log( const Node *n, int sev, const QString &msg, int ph = -1 )
                : node( n ), resource( 0 ), message( msg ), severity( sev ), phase( ph )
            {}
            Log( const Node *n, const Resource *r, int sev, const QString &msg, int ph = -1 )
                : node( n ), resource( r ), message( msg ), severity( sev ), phase( ph )
            {}
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
    /// Set if the task cannot be scheduled to fullfill all the constraints
    bool schedulingError;
    /// Set if the node has not been scheduled
    bool notScheduled;

    DateTime workStartTime;
    DateTime workEndTime;
    bool inCriticalPath;
    
    Duration positiveFloat;
    Duration negativeFloat;
    Duration freeFloat;

#ifndef NDEBUG
public:
    virtual void printDebug( const QString& ident );
#endif
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
    virtual void addAppointment( Schedule *resource, DateTime &start, DateTime &end, double load = 100 );
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

#ifndef NDEBUG
public:
    virtual void printDebug( const QString& ident );
#endif
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
    virtual void addAppointment( Schedule *node, DateTime &start, DateTime &end, double load = 100 );
    virtual void takeAppointment( Appointment *appointment, int type = Scheduling );

    virtual bool isOverbooked() const;
    virtual bool isOverbooked( const KDateTime &start, const KDateTime &end ) const;

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
    
#ifndef NDEBUG
public:
    virtual void printDebug( const QString& ident );
#endif
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
    virtual void setAllowOverbooking( bool state );
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
    virtual void insertForwardNode( Node *node ) { m_forwardnodes.append( node ); }
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
    
    QList<Schedule::Log> logs();
    const QList<Schedule::Log> &logs() const;
    virtual void addLog( Schedule::Log &log );
    virtual void clearLogs() { m_log.clear(); m_logPhase.clear(); }
    
    void setPhaseName( int phase, const QString &name ) { m_logPhase[ phase ] = name; }
    QString logPhase( int phase ) const { return m_logPhase.value( phase ); }
    static QString logSeverity( int severity );
    
    virtual void incProgress();

    QStringList logMessages() const;

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
    
    QList< QList<Node*> > m_pathlists;
    QList<Node*> *m_currentCriticalPath;
    bool criticalPathListCached;
    
    
    QList<Schedule::Log> m_log;
    QMap<int, QString> m_logPhase;
    
#ifndef NDEBUG
public:
    virtual void printDebug( const QString& ident );
#endif
};

/**
 * ScheduleManager is used by the Project class to manage the schedules.
 * Each ScheduleManager manages a schedule group that can consist of 
 * Expected-, Optimistic- and Pessimistic schedules.
 * A ScheduleManager can also have child manager(s).
 */
class KPLATOKERNEL_EXPORT ScheduleManager
{
public:
    explicit ScheduleManager( Project &project, const QString name = QString() );
    ~ScheduleManager();
    
    void setName( const QString& name );
    QString name() const { return m_name; }

    Project &project() const { return m_project; }
    
    void setParentManager( ScheduleManager *sm );
    ScheduleManager *parentManager() const { return m_parent; }
    
    long id() const { return m_expected == 0 ? NOTSCHEDULED : m_expected->id(); }
    
    int removeChild( const ScheduleManager *sm );
    void insertChild( ScheduleManager *sm, int index = -1 );
    const QList<ScheduleManager*> &children() const { return m_children; }
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
    long parentScheduleId() const { return m_parent == 0 ? NOTSCHEDULED : m_parent->id(); }
    void createSchedules();
    
    void setDeleted( bool on );
    
    bool isScheduled() const { return m_expected == 0 ? false :  m_expected->isScheduled(); }

    void setExpected( MainSchedule *sch );
    MainSchedule *expected() const { return m_expected; }

    void setOptimistic( MainSchedule *sch );
    MainSchedule *optimistic() const { return m_optimistic; }

    void setPessimistic( MainSchedule *sch );
    MainSchedule *pessimistic() const { return m_pessimistic; }

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

    void setCalculateAll( bool on );
    bool calculateAll() const { return m_calculateAll; }

    void setSchedulingDirection( bool on );
    bool schedulingDirection() const { return m_schedulingDirection; }

    QList<MainSchedule*> schedules() const;
    int numSchedules() const;
    int indexOf( const MainSchedule *sch ) const;

    bool loadXML( KoXmlElement &element, XMLLoaderObject &status );
    void saveXML( QDomElement &element ) const;
    
    /// Save a workpackage document
    void saveWorkPackageXML( QDomElement &element, const Node &node ) const;
            
    void scheduleChanged( MainSchedule *sch );
    
    void incProgress();

    const QList<SchedulerPlugin*> schedulerPlugins() const;
    QString schedulerPluginId() const;
    void setSchedulerPluginId( const QString &id );
    SchedulerPlugin *schedulerPlugin() const;
    QStringList schedulerPluginNames() const;
    int schedulerPluginIndex() const;
    void setSchedulerPlugin( int index );

    void calculateSchedule();

protected:
    MainSchedule *loadMainSchedule( KoXmlElement &element, XMLLoaderObject &status );
    
protected:
    Project &m_project;
    ScheduleManager *m_parent;
    QString m_name;
    bool m_baselined;
    bool m_allowOverbooking;
    bool m_checkExternalAppointments;
    bool m_calculateAll;
    bool m_usePert;
    bool m_recalculate;
    DateTime m_recalculateFrom;
    bool m_schedulingDirection;
    MainSchedule *m_expected;
    MainSchedule *m_optimistic;
    MainSchedule *m_pessimistic;
    QList<MainSchedule*> m_schedules;
    QList<ScheduleManager*> m_children;

    QString m_schedulerPluginId;
};


} //namespace KPlato

#endif
