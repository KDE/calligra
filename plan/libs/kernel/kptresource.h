/* This file is part of the KDE project
 Copyright (C) 2001 Thomas Zander zander@kde.org
 Copyright (C) 2004-2007 Dag Andersen <danders@get2net.dk>
 Copyright (C) 2011 Dag Andersen <danders@get2net.dk>

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
#include "kptcalendar.h"

#include <QDomDocument>
#include <QHash>
#include <QString>
#include <QList>

#include <kdebug.h>

#include <KoXmlReader.h>

class QTime;


/// The main namespace.
namespace KPlato
{

class Account;
class Risk;
class Effort;
class Appointment;
class Task;
class Node;
class Project;
class Resource;
class ResourceRequest;
class ResourceGroupRequest;
class ResourceRequestCollection;
class Schedule;
class NodeSchedule;
class ResourceSchedule;
class Schedule;
class XMLLoaderObject;
class DateTimeInterval;

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
    
    /// Return true if any resource in this group is baselined
    bool isBaselined( long id = BASELINESCHEDULE ) const;

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
    
    /// Save workpackage document. Include only resources listed in @p lst
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

    enum Type { Type_Work, Type_Material, Type_Team };
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

    /// Returns true if this resource will be allocated by default to new tasks
    bool autoAllocate() const;
    /// Set if this resource will be allocated by default to new tasks
    void setAutoAllocate( bool on );

    void copy( Resource *resource );
    
    void setParentGroup( ResourceGroup *parent ) { m_parent = parent; }
    ResourceGroup *parentGroup() const { return m_parent; }

    /// Set the time from when the resource is available to this project
    void setAvailableFrom( const QDateTime &af ) { m_availableFrom = af; changed();}
    /// Set the time from when the resource is available to this project
    void setAvailableFrom( const DateTime &af ) { m_availableFrom = af; changed(); }
    /// Return the time when the resource is available to this project
    const DateTime &availableFrom() const { return m_availableFrom;}
    /// Set the time when the resource is no longer available to this project
    void setAvailableUntil( const QDateTime &au ) { m_availableUntil = au; changed(); }
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
    
    /// Adds appointment to current schedule
    virtual bool addAppointment( Appointment *appointment );
    /// Adds appointment to schedule sch
    virtual bool addAppointment( Appointment *appointment, Schedule &main );
    /// Adds appointment to both this resource and node
    virtual void addAppointment( Schedule *node, const DateTime &start, const DateTime &end, double load = 100 );

    void initiateCalculation( Schedule &sch );
    bool isAvailable( Task *task );
    void makeAppointment( Schedule *schedule, int load, const QList<Resource*> &required = QList<Resource*>() );

    bool isOverbooked() const;
    /// check if overbooked on date.
    bool isOverbooked( const QDate &date ) const;
    /// check if overbooked within the interval start, end.
    bool isOverbooked( const DateTime &start, const DateTime &end ) const;

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
    void setUnits( int units );

    Project *project() const { return m_project; }
    /// Return the resources timespec. Defaults to local.
    KDateTime::Spec timeSpec() const;
    
    /**
     * Get the calendar for this resource.
     * Working resources may have a default calendar if the a calendar is marked as default,
     * this is checked if local=false.
     * If no calendar can be found for a working resource, the resource is not available.
     *
     * Material resources must have calendar explicitly set.
     * If there is no calendar set for a material resource, the resource is always available.
     */
    Calendar *calendar( bool local = false ) const;
    //Calendar *calendar( const QString& id ) const;
    void setCalendar( Calendar *calendar );

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

    /// Returns a list of work intervals in the interval @p from, @p until.
    /// Appointments are subtracted if @p schedule is not 0 and overbooking is not allowed.
    AppointmentIntervalList workIntervals( const DateTime &from, const DateTime &until, Schedule *schedule ) const;

    /// Returns a list of work intervals in the interval @p from, @p until.
    AppointmentIntervalList workIntervals( const DateTime &from, const DateTime &until ) const;

    /// Updates work interval cache a list of work intervals extracted from the resource calendar
    /// with @p load in the interval @p from, @p until.
    /// The load of the intervals is set to m_units
    /// Note: The list may contain intervals outside @p from, @p until
    void calendarIntervals( const DateTime &from, const DateTime &until ) const;
    /// Load cache from @p element
    bool loadCalendarIntervalsCache( const KoXmlElement& element, KPlato::XMLLoaderObject& status );
    /// Save cache to @p element
    void saveCalendarIntervalsCache( QDomElement &element ) const;

    /// Returns the effort that can be done starting at @p start within @p duration.
    /// The current schedule is used to check for appointments.
    /// If @p  backward is true, checks backward in time.
    Duration effort( const DateTime &start, const Duration &duration, int units = 100, bool backward = false, const QList<Resource*> &required = QList<Resource*>() ) const;

    /// Returns the effort that can be done starting at @p start within @p duration.
    /// The schedule @p sch is used to check for appointments.
    /// If @p  backward is true, checks backward in time.
    /// Status is returned in @p ok
    Duration effort( KPlato::Schedule* sch, const DateTime &start, const Duration& duration, int units = 100, bool backward = false, const QList< Resource* >& required = QList<Resource*>() ) const;


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
    
    EffortCostMap plannedEffortCostPrDay( const QDate &start, const QDate &end, long id, EffortCostCalculationType = ECCT_All );
    Duration plannedEffort( const QDate &date, EffortCostCalculationType = ECCT_All ) const;

    void setCurrentSchedulePtr( Schedule *schedule ) { m_currentSchedule = schedule; }
    void setCurrentSchedule( long id ) { m_currentSchedule = findSchedule( id ); }
    Schedule *currentSchedule() const { return m_currentSchedule; }

    bool isScheduled() const;
    QHash<long, Schedule*> schedules() const { return m_schedules; }
    /**
     * Return schedule with @id
     * If @p id == CURRENTSCHEDULE, return m_currentSchedule
     * Return 0 if schedule with @p id doesn't exist.
     */
    Schedule *schedule( long id = CURRENTSCHEDULE ) const;
    /// Returns true if schedule with @p id is baselined.
    /// if Team resource, if any of the team members is baselined
    /// By default returns true if any schedule is baselined
    bool isBaselined( long id = BASELINESCHEDULE ) const;
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

    void addExternalAppointment( const QString &id, Appointment *a );

    void addExternalAppointment( const QString &id, const QString &name, const DateTime &from, const DateTime &end, double load = 100 );
    void subtractExternalAppointment( const QString &id, const DateTime &from, const DateTime &end, double load );

    void clearExternalAppointments();
    void clearExternalAppointments( const QString id );
    /// Take the external appointments with identity @p id from the list of external appointments
    Appointment *takeExternalAppointment( const QString &id );
    /// Return external appointments with identity @p id
    AppointmentIntervalList externalAppointments( const QString &id );
    AppointmentIntervalList externalAppointments( const DateTimeInterval &interval = DateTimeInterval() ) const;

    int numExternalAppointments() const { return m_externalAppointments.count(); }
    QList<Appointment*> externalAppointmentList() const { return m_externalAppointments.values(); }
    /// return a map of project id, project name
    QMap<QString, QString> externalProjects() const;

    /// Return a measure of how suitable the resource is for allocation
    long allocationSuitability( const DateTime &time, const Duration &duration, bool backward );

    DateTime startTime( long id ) const;
    DateTime endTime( long id ) const;

    /// Returns the list of requiered resources.
    /// Note: This list is used as default for allocation dialog, not for scheduling.
    QList<Resource*> requiredResources() const;
    /// Set the list of the required resources's ids so they can be resolved when used
    /// A required resource may not exist in the project yet
    void setRequiredIds( const QStringList &lst );
    /// Add a resource id to the required ids list
    void addRequiredId( const QString &id );
    /// Returns the list of requiered resource ids.
    QStringList requiredIds() const { return m_requiredIds; }

    /// Return the list of team members.
    QList<Resource*> teamMembers() const;
    /// Return the list of team members.
    QStringList teamMemberIds() const;
    /// Clear the list of team members.
    void clearTeamMembers() { m_teamMembers.clear(); }
    /// Add resource @p id to the list of team members.
    void addTeamMemberId( const QString &id );
    /// Remove resource @p id from the list of team members.
    void removeTeamMemberId( const QString &id );

    /// Return the account
    Account *account() const { return cost.account; }
    /// Set the @p account
    void setAccount( Account *account );

    // for xml loading code
    
    class WorkInfoCache
    {
    public:
        WorkInfoCache() { clear(); }
        void clear() { start = end = DateTime(); effort = Duration::zeroDuration; intervals.clear(); version = -1; }
        bool isValid() const { return start.isValid() && end.isValid(); }
        DateTime firstAvailableAfter( const DateTime &time, const DateTime &limit, Calendar *cal, Schedule *sch ) const;
        DateTime firstAvailableBefore( const DateTime &time, const DateTime &limit, Calendar *cal, Schedule *sch ) const;

        DateTime start;
        DateTime end;
        Duration effort;
        AppointmentIntervalList intervals;
        int version;

        bool load( const KoXmlElement& element, KPlato::XMLLoaderObject& status );
        void save( QDomElement &element ) const;
    };
    const WorkInfoCache &workInfoCache() const { return m_workinfocache; }

signals:
    void externalAppointmentToBeAdded( Resource *r, int row );
    void externalAppointmentAdded( Resource*, Appointment* );
    void externalAppointmentToBeRemoved( Resource *r, int row );
    void externalAppointmentRemoved();
    void externalAppointmentChanged( Resource *r, Appointment *a );

protected:
    DateTimeInterval requiredAvailable(Schedule *node, const DateTime &start, const DateTime &end ) const;
    void makeAppointment( Schedule *node, const DateTime &from, const DateTime &end, int load, const QList<Resource*> &required = QList<Resource*>() );
    virtual void changed();

private:
    Project *m_project;
    ResourceGroup *m_parent;
    QHash<long, Schedule*> m_schedules;
    QString m_id; // unique id
    QString m_name;
    QString m_initials;
    QString m_email;
    bool m_autoAllocate;
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
        Account *account;
    }
    cost;
    
    Calendar *m_calendar;
    QList<ResourceRequest*> m_requests;
    QStringList m_requiredIds;
    
    QStringList m_teamMembers;

    Schedule *m_currentSchedule;

    mutable WorkInfoCache m_workinfocache;

    // return this if resource has no calendar and is a material resource
    Calendar m_materialCalendar;

#ifndef NDEBUG
public:
    void printDebug( const QString& ident );
#endif
};

KPLATOKERNEL_EXPORT QDebug operator<<( QDebug dbg, const KPlato::Resource::WorkInfoCache &c );

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
    explicit ResourceRequest( const ResourceRequest &r );

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

    void makeAppointment( Schedule *schedule, int amount );
    void makeAppointment( Schedule *schedule );
    Task *task() const;

    /// Return the datetime from when the resource is available.
    /// If it is not valid, the project constraint start time is used.
    /// For teams the earliest time for any team member is used.
    DateTime availableFrom();
    /// Return the datetime until when the resource is available.
    /// If it is not valid, the project constraint end time is used.
    /// For teams the latest time for any team member is used.
    DateTime availableUntil();

    Schedule *resourceSchedule( Schedule *ns, Resource *resource = 0 );
    DateTime availableAfter(const DateTime &time, Schedule *ns);
    DateTime availableBefore(const DateTime &time, Schedule *ns);
    Duration effort( const DateTime &time, const Duration &duration, Schedule *ns, bool backward );
    DateTime workTimeAfter(const DateTime &dt, Schedule *ns = 0);
    DateTime workTimeBefore(const DateTime &dt, Schedule *ns = 0);

    /// Resource is allocated dynamically by the group request
    bool isDynamicallyAllocated() const { return m_dynamic; }
    /// Set resource is allocated dynamically
    void setAllocatedDynaically( bool dyn ) { m_dynamic = dyn; }

    /// Return a measure of how suitable the resource is for allocation
    long allocationSuitability( const DateTime &time, const Duration &duration, Schedule *ns, bool backward );

    /// Returns a list of all the required resources that will be used in scheduling.
    /// Note: This list overrides the resources own list which is just used as default for allocation dialog.
    QList<Resource*> requiredResources() const { return m_required; }
    /// Set the list of required resources that will be used in scheduling.
    void setRequiredResources( const QList<Resource*> &lst ) { m_required = lst; }

private:
    friend class ResourceGroupRequest;
    QList<ResourceRequest*> teamMembers() const;

protected:
    void changed();

    void setCurrentSchedulePtr( Schedule *ns );
    void setCurrentSchedulePtr( Resource *resource, Schedule *ns );

private:
    Resource *m_resource;
    int m_units;
    ResourceGroupRequest *m_parent;
    bool m_dynamic;
    QList<Resource*> m_required;
    mutable QList<ResourceRequest*> m_teamMembers;

#ifndef NDEBUG
public:
    void printDebug( const QString& ident );
#endif
};
QDebug &operator<<( QDebug &dbg, const KPlato::ResourceRequest *r );
QDebug &operator<<( QDebug &dbg, const KPlato::ResourceRequest &r );

class KPLATOKERNEL_EXPORT ResourceGroupRequest
{
public:
    explicit ResourceGroupRequest( ResourceGroup *group = 0, int units = 0 );
    explicit ResourceGroupRequest( const ResourceGroupRequest &group );
    ~ResourceGroupRequest();

    void setParent( ResourceRequestCollection *parent ) { m_parent = parent;}
    ResourceRequestCollection *parent() const { return m_parent; }

    ResourceGroup *group() const { return m_group; }
    void setGroup( ResourceGroup *group ) { m_group = group; }
    void unregister( const ResourceGroup *group ) { if ( group == m_group ) m_group = 0; }
    /// Return a list of resource requests.
    /// If @p resolveTeam is true, include the team members,
    /// if @p resolveTeam is false, include the team resource itself.
    QList<ResourceRequest*> resourceRequests( bool resolveTeam=true ) const;
    void addResourceRequest( ResourceRequest *request );
    void deleteResourceRequest( ResourceRequest *request );
    int count() const { return m_resourceRequests.count(); }
    ResourceRequest *requestAt( int idx ) const { return m_resourceRequests.value( idx ); }

    ResourceRequest *takeResourceRequest( ResourceRequest *request );
    ResourceRequest *find( const Resource *resource ) const;
    ResourceRequest *resourceRequest( const QString &name );
    /// Return a list of allocated resources, allocation to group is not included by default.
    QStringList requestNameList( bool includeGroup = false ) const;
    /// Return a list of allocated resources.
    /// Allocations to groups are not included.
    /// Team resources are included but *not* the team members.
    /// Any dynamically allocated resource is not included.
    QList<Resource*> requestedResources() const;
    bool load( KoXmlElement &element, XMLLoaderObject &status );
    void save( QDomElement &element ) const;

    /// The number of requested resources
    int units() const;
    void setUnits( int value ) { m_units = value; changed(); }

    /**
     * Returns the duration needed to do the @p effort starting at @p start.
     */
    Duration duration( const DateTime &start, const Duration &effort, Schedule *ns, bool backward = false );

    DateTime availableAfter( const DateTime &time, Schedule *ns );
    DateTime availableBefore( const DateTime &time, Schedule *ns );
    DateTime workTimeAfter(const DateTime &dt, Schedule *ns = 0);
    DateTime workTimeBefore(const DateTime &dt, Schedule *ns = 0);

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

    QList<ResourceGroupRequest*> requests() const { return m_requests; }
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
    /// Return a list of names of allocated resources.
    /// Allocations to groups are not included by default.
    /// Team resources are included but *not* the team members.
    /// Any dynamically allocated resource is not included.
    QStringList requestNameList( bool includeGroup = false ) const;
    /// Return a list of allocated resources.
    /// Allocations to groups are not included.
    /// Team resources are included but *not* the team members.
    /// Any dynamically allocated resource is not included.
    QList<Resource*> requestedResources() const;

    /// Return a list of all resource requests.
    /// If @p resolveTeam is true, include the team members,
    /// if @p resolveTeam is false, include the team resource itself.
    QList<ResourceRequest*> resourceRequests( bool resolveTeam=true ) const;
    
    //bool load(KoXmlElement &element, Project &project);
    void save( QDomElement &element ) const;

    /**
    * Returns the duration needed to do the @p effort starting at @p time.
    */
    Duration duration( const DateTime &time, const Duration &effort, Schedule *sch, bool backward = false );

    DateTime availableAfter( const DateTime &time, Schedule *ns );
    DateTime availableBefore( const DateTime &time, Schedule *ns );
    DateTime workTimeAfter(const DateTime &dt, Schedule *ns = 0) const;
    DateTime workTimeBefore(const DateTime &dt, Schedule *ns = 0) const;
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
    
    Duration effort( const QList<ResourceRequest*> &lst, const DateTime &time, const Duration &duration, Schedule *ns, bool backward ) const;
    int numDays(const QList<ResourceRequest*> &lst, const DateTime &time, bool backward) const;
    Duration duration(const QList<ResourceRequest*> &lst, const DateTime &time, const Duration &_effort, Schedule *ns, bool backward);

private:
    Task *m_task;
    QList<ResourceGroupRequest*> m_requests;
};

}  //KPlato namespace

#endif
