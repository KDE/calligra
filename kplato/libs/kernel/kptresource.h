/* This file is part of the KDE project
 Copyright (C) 2001 Thomas Zander zander@kde.org
 Copyright (C) 2004-2007 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTRESOURCE_H
#define KPTRESOURCE_H

#include "kplatokernel_export.h"

#include "kptglobal.h"
#include "kptduration.h"
#include "kptdatetime.h"
#include "kptappointment.h"

#include <qdom.h>
#include <QHash>
#include <QString>
#include <QList>

#include <kdebug.h>

#include <KoXmlReader.h>

class QTime;


/// The main namespace.
namespace KPlato
{

class Risk;
class Effort;
class Appointment;
class Task;
class Node;
class Project;
class Resource;
class ResourceRequest;
class ResourceGroupRequest;
class Calendar;
class ResourceRequestCollection;
class Schedule;
class NodeSchedule;
class ResourceSchedule;
class Schedule;
class XMLLoaderObject;

/**
  * This class represents a group of similar resources to be assigned to a task
  * e.g. The list of employees, computer resources, etc
  */

/* IDEA; lets create a resourceGroup that has the intelligence to import PIM schedules
 *  from the kroupware project and use the schedules to use the factory pattern to build
 *  Resources (probably a derived class) which returns values on getFirstAvailableTime
 *  and friends based on the schedules we got from the PIM projects.
 *  (Thomas Zander mrt-2003 by suggestion of Shaheed)
 */
 
class KPLATOKERNEL_EXPORT ResourceGroup : public QObject
{
    Q_OBJECT
public:
    /// Default constructor
    explicit ResourceGroup();
    ResourceGroup( const ResourceGroup *group );
    ~ResourceGroup();

    enum Type { Type_Work, Type_Material };

    QString id() const { return m_id; }
    void setId( const QString& id );

    Project *project() { return m_project; }

    void setName( const QString& n );
    const QString &name() const { return m_name;}
    void setType( Type type );
    void setType(const QString &type);
    Type type() const { return m_type; }
    QString typeToString( bool trans = false ) const;
    static QStringList typeToStringList( bool trans = false );

    bool isScheduled() const;
    
    /** Manage the resources in this list
     * <p>At some point we will have to look at not mixing types of resources
     * (e.g. you can't add a person to a list of computers
     *
     * <p>Risks must always be associated with a resource, so there is no option
     * to manipulate risks (@ref Risk) separately
         */
    void addResource( int index, Resource*, Risk* );
    Resource *takeResource( Resource *resource );
    QList<Resource*> resources() const { return m_resources; }
    int indexOf( const Resource *resource ) const;
    Resource *resourceAt( int pos ) const { return m_resources.value( pos ); }
    int numResources() const { return m_resources.count(); }
    
    Risk* getRisk( int );

    /** Get the "num" resources which is available in the time frame
     * defined by "start" and "duration".
     * @param start todo 
     * @param duration todo
     * @param num todo
     */
    QList<Resource> availableResources( const DateTime start, const Duration duration, int num );
    /** Manage the dependent resources.  This is a list of the resource
     * groups that must have available resources for this resource to
     * perform the work
     * <p>see also @ref getRequiredResource, @ref getRequiredResource
         */
    void addRequiredResource( ResourceGroup* );
    /** Manage the dependent resources.  This is a list of the resource
     * groups that must have available resources for this resource to
     * perform the work
     * <p>see also @ref addRequiredResource, @ref getRequiredResource
         */
    ResourceGroup* getRequiredResource( int );
    /** Manage the dependent resources.  This is a list of the resource
     * groups that must have available resources for this resource to
     * perform the work
     * <p>see also @ref getRequiredResource, @ref addRequiredResource
         */
    void deleteRequiredResource( int );

    bool load( KoXmlElement &element, XMLLoaderObject &status );
    void save( QDomElement &element ) const;
    
    /// Save workpackage document. Include only resources listed in @lst
    void saveWorkPackageXML( QDomElement &element, const QList<Resource*> lst ) const;

    void initiateCalculation( Schedule &sch );

    void addNode( Node *node ) { m_nodes.append( node ); }
    void clearNodes() { m_nodes.clear(); }

    Calendar *defaultCalendar() { return m_defaultCalendar; }

    int units() const;

    void registerRequest( ResourceGroupRequest *request )
    { m_requests.append( request ); }
    void unregisterRequest( ResourceGroupRequest *request )
    {
        int i = m_requests.indexOf( request );
        if ( i != -1 )
            m_requests.removeAt( i );
    }
    const QList<ResourceGroupRequest*> &requests() const
    { return m_requests; }

    ResourceGroup *findId() const { return findId( m_id ); }
    ResourceGroup *findId( const QString &id ) const;
    bool removeId() { return removeId( m_id ); }
    bool removeId( const QString &id );
    void insertId( const QString &id );

    Appointment appointmentIntervals() const;

    // m_project is set when the resourcegroup is added to the project,
    // and reset when the resourcegroup is removed from the project
    void setProject( Project *project );

    void copy( const ResourceGroup *group );
    
    DateTime startTime( long id ) const;
    DateTime endTime( long id ) const;

#ifndef NDEBUG

    void printDebug( const QString& ident );
#endif

protected:
    virtual void changed();

private:
    Project *m_project;
    QString m_id;   // unique id
    QString m_name;
    QList<Resource*> m_resources;
    QList<Risk*> m_risks;
    QList<ResourceGroup*> m_requires;

    QList<Node*> m_nodes; //The nodes that want resources from us

    Calendar *m_defaultCalendar;
    Type m_type;

    QList<ResourceGroupRequest*> m_requests;

};

/**
  * Any resource that is used by a task. A resource can be a worker, or maybe wood.
  * If the resources is a worker or a piece of equiment which can be reused but
  * can only be used by one node in time, then we can use the scheduling methods of the
  * resource to schedule the resource available time for the project.
  * The Idea is that all nodes which need this resource point to it and the scheduling
  * code (partly implemented here) schedules the actual usage.
  * See also @ref ResourceGroup
  */

class KPLATOKERNEL_EXPORT Resource : public QObject
{
    Q_OBJECT
public:

    Resource();
    Resource( Resource *resource );
    virtual ~Resource();

    QString id() const { return m_id; }
    void setId( const QString& id );

    enum Type { Type_Work, Type_Material };
    void setType( Type type );
    void setType( const QString &type );
    Type type() const { return m_type; }
    QString typeToString( bool trans = false ) const;
    static QStringList typeToStringList( bool trans = false );

    void setName( const QString n );
    const QString &name() const { return m_name;}

    void setInitials( const QString initials );
    const QString &initials() const { return m_initials;}

    void setEmail( const QString email );
    const QString &email() const { return m_email;}

    void copy( Resource *resource );
    
    void setParentGroup( ResourceGroup *parent ) { m_parent = parent; }
    ResourceGroup *parentGroup() const { return m_parent; }
    
    /// Set the time from when the resource is available to this project
    void setAvailableFrom( const QDateTime &af ) { m_availableFrom.setDateTime( af ); changed();}
    /// Set the time from when the resource is available to this project
    void setAvailableFrom( const DateTime &af ) { m_availableFrom = af; changed(); }
    /// Return the time when the resource is available to this project
    const DateTime &availableFrom() const { return m_availableFrom;}
    /// Set the time when the resource is no longer available to this project
    void setAvailableUntil( const QDateTime &au ) { m_availableUntil.setDateTime( au ); changed(); }
    /// Set the time when the resource is no longer available to this project
    void setAvailableUntil( const DateTime &au ) { m_availableUntil = au; changed(); }
    /// Return the time when the resource is no longer available to this project.
    const DateTime &availableUntil() const { return m_availableUntil;}

    DateTime firstAvailableAfter( const DateTime &time, const DateTime &limit ) const;
    
    DateTime getBestAvailableTime( Duration duration );
    DateTime getBestAvailableTime( const DateTime after, const Duration duration );

    bool load( KoXmlElement &element, XMLLoaderObject &status );
    void save( QDomElement &element ) const;

    /// Return the list of appointments for schedule @p id.
    QList<Appointment*> appointments( long id = -1 ) const;
    /// Return the number of appointments (nodes)
    int numAppointments( long id = -1 ) const { return appointments( id ).count(); }
    /// Return the appointment at @p index for schedule @p id
    Appointment *appointmentAt( int index, long id = -1 ) const { return appointments( id ).value( index ); }
    int indexOf( Appointment *a, long id = -1 ) const { return appointments( id ).indexOf( a ); }
    
    Appointment *findAppointment( Node *node );
    /// Adds appointment to current schedule
    virtual bool addAppointment( Appointment *appointment );
    /// Adds appointment to schedule sch
    virtual bool addAppointment( Appointment *appointment, Schedule &main );
    /// Adds appointment to both this resource and node
    virtual void addAppointment( Schedule *node, DateTime &start, DateTime &end, double load = 100 );

    void initiateCalculation( Schedule &sch );
    bool isAvailable( Task *task );
    void makeAppointment( Schedule *schedule, int load );

    bool isOverbooked() const;
    /// check if overbooked on date.
    bool isOverbooked( const QDate &date ) const;
    /// check if overbooked within the interval start, end.
    bool isOverbooked( const KDateTime &start, const KDateTime &end ) const;

    double normalRate() const { return cost.normalRate; }
    void setNormalRate( double rate ) { cost.normalRate = rate; changed(); }
    double overtimeRate() const { return cost.overtimeRate; }
    void setOvertimeRate( double rate ) { cost.overtimeRate = rate; changed(); }

    /**
     * Return available units in percent
     */
    int units() const { return m_units; }
    /**
     * Set available units in percent
     */
    void setUnits( int units ) { m_units = units; changed(); }

    Project *project() const { return m_project; }
    /// Return the resources timespec. Defaults to local.
    KDateTime::Spec timeSpec() const;
    
    /**
     * Get the calendar for this resource. 
     * If local=false, check if there is a default calendar.
     */
    Calendar *calendar( bool local = false ) const;
    //Calendar *calendar( const QString& id ) const;
    void setCalendar( Calendar *calendar ) { m_calendar = calendar; changed(); }

    /// Delete all requests for me
    void removeRequests();
    /**
     * Used to clean up requests when the resource is deleted.
     */
    void registerRequest( ResourceRequest *request )
    { m_requests.append( request ); }
    void unregisterRequest( ResourceRequest *request )
    {
        int i = m_requests.indexOf( request );
        if ( i != -1 )
            m_requests.removeAt( i );
    }
    const QList<ResourceRequest*> &requests() const
    { return m_requests; }

    /// Returns the effort that can be done starting at @p start within @p duration.
    /// The current schedule is used to check for appointments.
    /// If @p  backward is true, checks backward in time.
    /// Status is returned in @p ok
    Duration effort( const DateTime &start, const Duration &duration, bool backward = false, bool *ok = 0 ) const;

    /// Returns the effort that can be done starting at @p start within @p duration.
    /// The schedule @p sch is used to check for appointments.
    /// If @p  backward is true, checks backward in time.
    /// Status is returned in @p ok
    Duration effort( Schedule *sch, const DateTime &start, const Duration &duration, bool backward = false, bool *ok = 0 ) const;


    /**
     * Find the first available time after @p time, within @p limit.
     * Returns invalid DateTime if not available.
     * Uses the current schedule to check for appointments.
     */
    DateTime availableAfter( const DateTime &time, const DateTime limit = DateTime() ) const;
    /**
     * Find the first available time before @p time, within @p limit.
     * Returns invalid DateTime if not available.
     * Uses the current schedule to check for appointments.
     */
    DateTime availableBefore( const DateTime &time, const DateTime limit = DateTime()) const;

    /**
     * Find the first available time after @p time, within @p limit.
     * Returns invalid DateTime if not available.
     * If @p sch == 0, Appointments are not checked.
     */
    DateTime availableAfter( const DateTime &time, const DateTime limit, Schedule *sch ) const;
    /**
     * Find the first available time before @p time, within @p limit.
     * Returns invalid DateTime if not available.
     * If @p sch == 0, Appointments are not checked.
     */
    DateTime availableBefore( const DateTime &time, const DateTime limit, Schedule *sch ) const;

    Resource *findId() const { return findId( m_id ); }
    Resource *findId( const QString &id ) const;
    bool removeId() { return removeId( m_id ); }
    bool removeId( const QString &id );
    void insertId( const QString &id );

    Calendar *findCalendar( const QString &id ) const;

    Appointment appointmentIntervals( long id ) const;
    Appointment appointmentIntervals() const;
    Duration plannedEffort( const QDate &date ) const;

    void setCurrentSchedulePtr( Schedule *schedule ) { m_currentSchedule = schedule; }
    void setCurrentSchedule( long id ) { m_currentSchedule = findSchedule( id ); }
    Schedule *currentSchedule() const { return m_currentSchedule; }

    bool isScheduled() const;
    QHash<long, Schedule*> &schedules() { return m_schedules; }
    /**
     * Return schedule with @id
     * If @id == -1, return m_currentSchedule
     * Return 0 if schedule with @id doesn't exist.
     */
    Schedule *schedule( long id = -1 ) const;
    /**
     * Return schedule with @id
     * Return 0 if schedule with @id doesn't exist.
     */
    Schedule *findSchedule( long id ) const;
    /// Take, and delete.
    void deleteSchedule( Schedule *schedule );
    /// Take, don't delete.
    void takeSchedule( const Schedule *schedule );
    void addSchedule( Schedule *schedule );
    ResourceSchedule *createSchedule( const QString& name, int type, long id );
    ResourceSchedule *createSchedule( Schedule *parent );

    // m_project is set when the resource (or the parent) is added to the project,
    // and reset when the resource is removed from the project
    void setProject( Project *project );


    void addExternalAppointment( const QString &id, const QString &name, const DateTime &from, const DateTime &end, double load = 100 );
    void clearExternalAppointments();
    void clearExternalAppointments( const QString id );
    AppointmentIntervalList externalAppointments( const QString &id );
    AppointmentIntervalList externalAppointments() const;

    int numExternalAppointments() const { return m_externalAppointments.count(); }
    QList<Appointment*> externalAppointmentList() const { return m_externalAppointments.values(); }
    
    /// Return a measure of how suitable the resource is for allocation
    long allocationSuitability( const DateTime &time, const Duration &duration, bool backward );

    DateTime startTime( long id ) const;
    DateTime endTime( long id ) const;

signals:
    void externalAppointmentToBeAdded( Resource *r, int row );
    void externalAppointmentAdded( Resource*, Appointment* );
    void externalAppointmentToBeRemoved( Resource *r, int row );
    void externalAppointmentRemoved();
    void externalAppointmentChanged( Resource *r, Appointment *a );

protected:
    void makeAppointment( Schedule *node, const DateTime &from, const DateTime &end, int load );
    virtual void changed();

private:
    Project *m_project;
    ResourceGroup *m_parent;
    QHash<long, Schedule*> m_schedules;
    QString m_id; // unique id
    QString m_name;
    QString m_initials;
    QString m_email;
    DateTime m_availableFrom;
    DateTime m_availableUntil;
    QMap<QString, Appointment*> m_externalAppointments;

    int m_units; // avalable units in percent

    Type m_type;

    struct Cost
    {
        double normalRate;
        double overtimeRate;
        double fixed ;
    }
    cost;
    
    Calendar *m_calendar;
    QList<ResourceRequest*>
    m_requests;
    
    Schedule *m_currentSchedule;

#ifndef NDEBUG
public:
    void printDebug( const QString& ident );
#endif
};


/**
 * Risk is associated with a resource/task pairing to indicate the planner's confidence in the
 * estimated effort. Risk can be one of none, low, or high. Some factors that may be taken into
 * account for risk are the experience of the person and the reliability of equipment.
 */
class Risk
{
public:

    enum RiskType {
        NONE = 0,
        LOW = 1,
        HIGH = 2
    };

    Risk( Node *n, Resource *r, RiskType rt = NONE );
    ~Risk();

    RiskType riskType() { return m_riskType; }

    Node *node() { return m_node; }
    Resource *resource() { return m_resource; }

private:
    Node *m_node;
    Resource *m_resource;
    RiskType m_riskType;
};

class KPLATOKERNEL_EXPORT ResourceRequest
{
public:
    explicit ResourceRequest( Resource *resource = 0, int units = 1 );

    ~ResourceRequest();

    ResourceGroupRequest *parent() const { return m_parent; }
    void setParent( ResourceGroupRequest *parent ) { m_parent = parent; }

    Resource *resource() const { return m_resource; }
    void setResource( Resource* resource ) { m_resource = resource; }

    bool load( KoXmlElement &element, Project &project );
    void save( QDomElement &element ) const;

    /**
    * Get amount of requested resource units in percent
    */
    int units() const;
    void setUnits( int value );

    /**
    * Get amount of requested work units in percent
    */
    int workUnits() const;

    void registerRequest()
    {
        if ( m_resource )
            m_resource->registerRequest( this );
    }
    void unregisterRequest()
    {
        if ( m_resource )
            m_resource->unregisterRequest( this );
    }

    void makeAppointment( Schedule *schedule );
    Task *task() const;

    Schedule *resourceSchedule( Schedule *ns );
    DateTime availableAfter(const DateTime &time, Schedule *ns);
    DateTime availableBefore(const DateTime &time, Schedule *ns);
    Duration effort( const DateTime &time, const Duration &duration, Schedule *ns, bool backward, bool *ok = 0 );
    DateTime workTimeAfter(const DateTime &dt);
    DateTime workTimeBefore(const DateTime &dt);

    /// Resource is allocated dynamically by the group request
    bool isDynamicallyAllocated() const { return m_dynamic; }
    /// Set resource is allocated dynamically
    void setAllocatedDynaically( bool dyn ) { m_dynamic = dyn; }

    /// Return a measure of how suitable the resource is for allocation
    long allocationSuitability( const DateTime &time, const Duration &duration, Schedule *ns, bool backward );

protected:
    void changed();

private:
    Resource *m_resource;
    int m_units;
    ResourceGroupRequest *m_parent;
    bool m_dynamic;

#ifndef NDEBUG
public:
    void printDebug( const QString& ident );
#endif
};

class KPLATOKERNEL_EXPORT ResourceGroupRequest
{
public:
    explicit ResourceGroupRequest( ResourceGroup *group = 0, int units = 0 );
    ~ResourceGroupRequest();

    void setParent( ResourceRequestCollection *parent ) { m_parent = parent;}
    ResourceRequestCollection *parent() const { return m_parent; }

    ResourceGroup *group() const { return m_group; }
    void setGroup( ResourceGroup *group ) { m_group = group; }
    void unregister( const ResourceGroup *group ) { if ( group == m_group ) m_group = 0; }
    QList<ResourceRequest*> &resourceRequests() { return m_resourceRequests; }
    void addResourceRequest( ResourceRequest *request );
    void deleteResourceRequest( ResourceRequest *request );
    int count() const { return m_resourceRequests.count(); }
    ResourceRequest *requestAt( int idx ) const { return m_resourceRequests.value( idx ); }

    ResourceRequest *takeResourceRequest( ResourceRequest *request );
    ResourceRequest *find( const Resource *resource ) const;
    ResourceRequest *resourceRequest( const QString &name );
    /// Return a list of allocated resources, allocation to group is not included by default.
    QStringList requestNameList( bool includeGroup = false ) const;
    
    bool load( KoXmlElement &element, Project &project );
    void save( QDomElement &element ) const;

    /// The number of requested resources
    int units() const;
    void setUnits( int value ) { m_units = value; changed(); }

    /**
    * Get amount of allocated work units in percent
    */
    int workUnits() const;

    /**
    * Get the number of resources allocated
    */
    int workAllocation() const;

    /**
     * Returns the duration needed to do the effort  effort
     * starting at start.
     */
    Duration duration( const DateTime &start, const Duration &effort, Schedule *ns, bool backward = false );

    DateTime availableAfter( const DateTime &time, Schedule *ns );
    DateTime availableBefore( const DateTime &time, Schedule *ns );
    DateTime workTimeAfter(const DateTime &dt);
    DateTime workTimeBefore(const DateTime &dt);

    /**
     * Makes appointments for task @param task to the 
     * requested resources for the duration found in @ref duration().
     */
    void makeAppointments( Schedule *schedule );

    /**
     * Reserves the requested resources for the specified interval
     */
    void reserve( const DateTime &start, const Duration &duration );

    bool isEmpty() const;

    Task *task() const;

    void changed();
    
    /// Reset dynamic resource allocations
    void resetDynamicAllocations();
    /// Allocate dynamic requests. Do nothing if already allocated.
    void allocateDynamicRequests( const DateTime &time, const Duration &effort, Schedule *ns, bool backward );

private:
    ResourceGroup *m_group;
    int m_units;
    ResourceRequestCollection *m_parent;

    QList<ResourceRequest*> m_resourceRequests;
    DateTime m_start;
    Duration m_duration;

#ifndef NDEBUG
public:
    void printDebug( const QString& ident );
#endif
};

class KPLATOKERNEL_EXPORT ResourceRequestCollection
{
public:
    explicit ResourceRequestCollection( Task *task = 0 );
    ~ResourceRequestCollection();

    const QList<ResourceGroupRequest*> &requests() const { return m_requests; }
    void addRequest( ResourceGroupRequest *request );
    void deleteRequest( ResourceGroupRequest *request )
    {
        int i = m_requests.indexOf( request );
        if ( i != -1 )
            m_requests.removeAt( i );
        delete request;
        changed();
    }

    int takeRequest( ResourceGroupRequest *request )
    {
        int i = m_requests.indexOf( request );
        if ( i != -1 ) {
            m_requests.removeAt( i );
            changed();
        }
        return i;
    }

    ResourceGroupRequest *find( const ResourceGroup *resource ) const;
    ResourceRequest *find( const Resource *resource ) const;
    ResourceRequest *resourceRequest( const QString &name ) const;
    /// The ResourceRequestCollection has no requests
    bool isEmpty() const;
    /// Empty the ResourceRequestCollection of all requets
    void clear() { m_requests.clear(); }
    /// Reset dynamic resource allocations
    void resetDynamicAllocations();

    bool contains( const QString &identity ) const;
    ResourceGroupRequest *findGroupRequestById( const QString &id ) const;
    /// Return a list of allocated resources, allocations to groups are not included by default.
    QStringList requestNameList( bool includeGroup = false ) const;
    QList<Resource*> requestedResources() const;
    
    //bool load(KoXmlElement &element, Project &project);
    void save( QDomElement &element ) const;

    /**
    * Returns the total amount of resource units in percent
    */
//    int units() const;

    /**
    * Returns the amount of allocated work units in percent
    */
    int workUnits() const;

    /**
    * Returns the number of allocated working resources
    */
    int workAllocation() const;

    /**
    * Returns the duration needed to do the effort @param effort
    * starting at @param time.
    */
    Duration duration( const DateTime &time, const Duration &effort, Schedule *sch, bool backward = false );

    DateTime availableAfter( const DateTime &time, Schedule *ns );
    DateTime availableBefore( const DateTime &time, Schedule *ns );
    DateTime workTimeAfter(const DateTime &dt) const;
    DateTime workTimeBefore(const DateTime &dt) const;
    DateTime workStartAfter(const DateTime &time, Schedule *ns);
    DateTime workFinishBefore(const DateTime &time, Schedule *ns);

    /**
    * Makes appointments for the task @param task to the requested resources.
    * Assumes that @ref duration() has been run.
    */
    void makeAppointments( Schedule *schedule );
    /**
     * Reserves the requested resources for the specified interval
     */
    void reserve( const DateTime &start, const Duration &duration );

    Task *task() const { return m_task; }
    void setTask( Task *t ) { m_task = t; }

    void changed();
    
    Duration effort(const QList<ResourceRequest*> &lst, const DateTime &time, const Duration &duration, Schedule *ns, bool backward, bool *ok) const;
    int numDays(const QList<ResourceRequest*> &lst, const DateTime &time, bool backward) const;
    Duration duration(const QList<ResourceRequest*> &lst, const DateTime &time, const Duration &_effort, Schedule *ns, bool backward);

protected:
    struct Interval
    {
        DateTime start;
        DateTime end;
        Duration effort;
    };


private:
    Task *m_task;
    QList<ResourceGroupRequest*> m_requests;

#ifndef NDEBUG
public:
    void printDebug( const QString& ident );
#endif
};

}  //KPlato namespace

#endif
