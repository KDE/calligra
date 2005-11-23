/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org
   Copyright (C) 2004, 2005 Dag Andersen <danders@get2net.dk>

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

#include "kptduration.h"
#include "kptdatetime.h"

#include <qdom.h>
#include <qstring.h>
#include <qptrlist.h>

#include <kdebug.h>

class QTime;

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
class EffortCostMap;

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
class ResourceGroup {
    public:
	      ResourceGroup(Project *project);
	      ~ResourceGroup();

          enum Type { Type_Work, Type_Material };
          
          QString id() const { return m_id; }
          bool setId(QString id);
          void generateId();
          
          Project *project() { return m_project; }
          
	      void setName(QString n) {m_name=n;}
	      const QString &name() const {return m_name;}
          void setType(Type type) { m_type = type; }
          //void setType(const QString &type);
          Type type() const { return m_type; }

	      /** Manage the resources in this list
	        * <p>At some point we will have to look at not mixing types of resources
	        * (e.g. you can't add a person to a list of computers
	        *
	        * <p>Risks must always be associated with a resource, so there is no option
	        * to manipulate risks (@ref Risk) seperately
	        */
	      void addResource(Resource*, Risk*);
          void insertResource( unsigned int index, Resource *resource );
          void removeResource( Resource *resource );
          Resource *takeResource( Resource *resource );
	      void removeResource(int);

	      Resource* getResource(int);
	      Risk* getRisk(int);

	      /** Get the @num resources which is available in the time frame
            * defined by @start and @duration.
            */
          QPtrList<Resource> availableResources(const DateTime start, const Duration duration, int num);
	      /** Manage the dependent resources.  This is a list of the resource
	        * groups that must have available resources for this resource to
	        * perform the work
            * <p>see also @ref getRequiredResource, @ref getRequiredResource
	        */
	      void addRequiredResource(ResourceGroup*);
	      /** Manage the dependent resources.  This is a list of the resource
	        * groups that must have available resources for this resource to
	        * perform the work
            * <p>see also @ref addRequiredResource, @ref getRequiredResource
	        */
	      ResourceGroup* getRequiredResource(int);
	      /** Manage the dependent resources.  This is a list of the resource
	        * groups that must have available resources for this resource to
	        * perform the work
            * <p>see also @ref getRequiredResource, @ref addRequiredResource
	        */
	      void removeRequiredResource(int);
          int numResources() const { return m_resources.count(); }
          QPtrList<Resource> &resources() { return m_resources; }

          bool load(QDomElement &element);
          void save(QDomElement &element);

          void clearAppointments();
          void saveAppointments(QDomElement &element);


          void addNode(const Node *node) { m_nodes.append(node); }
          void clearNodes() { m_nodes.clear(); }

          Calendar *defaultCalendar() { return m_defaultCalendar; }

          int units();
        
          void registerRequest(ResourceGroupRequest *request)
            { m_requests.append(request); }
          void unregisterRequest(ResourceGroupRequest *request)
            { m_requests.removeRef(request); }
          const QPtrList<ResourceGroupRequest> &requests() const
            { return m_requests; }

          ResourceGroup *findId() const { return findId(m_id); }
          ResourceGroup *findId(const QString &id) const;
          bool removeId() { return removeId(m_id); }
          bool removeId(const QString &id);
          void insertId(const QString &id);

          Appointment appointmentIntervals() const;

#ifndef NDEBUG
        void printDebug(QString ident);
#endif

    private:
        Project  *m_project;
        QString m_id;   // unique id
        QString m_name;
        QPtrList<Resource> m_resources;
        QPtrList<Risk> m_risks;
        QPtrList<ResourceGroup> m_requires;

        QPtrList<Node> m_nodes; //The nodes that want resources from us

        Calendar *m_defaultCalendar;
        Type m_type;
        
        QPtrList<ResourceGroupRequest> m_requests;
        
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

class Resource {
public:

    Resource(Project *project);
    Resource(Resource *resource) { copy(resource); }
    virtual ~Resource();

    QString id() const { return m_id; }
    bool setId(QString id);
    void generateId();

    enum Type { Type_Work, Type_Material };
    void setType(Type type) { m_type = type; }
    void setType(const QString &type);
    Type type() const { return m_type; }
    QString typeToString() const;

    void setName(QString n) {m_name=n;}
    const QString &name() const {return m_name;}

    void setInitials(QString initials) {m_initials=initials;}
    const QString &initials() const {return m_initials;}

    void setEmail(QString email) {m_email=email;}
    const QString &email() const {return m_email;}

    void copy(Resource *resource);

    /// Set the time from when the resource is available to this project
    void setAvailableFrom(const QDateTime &af) {m_availableFrom=af;}
    /// Return the time when the resource is available to this project
    const DateTime &availableFrom() const {return m_availableFrom;}
    /// Set the time when the resource is no longer available to this project
    void setAvailableUntil(const QDateTime au) {m_availableUntil=au;}
    /// Return the time when the resource is no longer available to this project.
    const DateTime &availableUntil() const {return m_availableUntil;}

    void addWorkingHour(QTime from, QTime until);
    QPtrList<QTime> workingHours() { return m_workingHours; }

    DateTime *getFirstAvailableTime(DateTime after = DateTime());
    DateTime *getBestAvailableTime(Duration duration);
    DateTime *getBestAvailableTime(const DateTime after, const Duration duration);

    bool load(QDomElement &element);
    void save(QDomElement &element);

    /// removes appoinrment and deletes it (independent of setAutoDelete)
    void removeAppointment(Appointment *appointment);
    /// removes appointment without deleting it (independent of setAutoDelete)
    void takeAppointment(Appointment *appointment);
    
    ///Return the list of appointments
    QPtrList<Appointment> &appointments() { return m_appointments; }
    
    Appointment *findAppointment(Node *node);
    int numAppointments() const { return m_appointments.count(); }
    /// Adds appointment to this resource only (not to node)
    virtual bool addAppointment(Appointment *appointment);
    /// Adds appointment to both this resource and node
    virtual void addAppointment(Node *node, DateTime &start, DateTime &end, double load=100);
    
    void clearAppointments();
    bool isAvailable(Task *task);
    void makeAppointment(DateTime &start, Duration &duration, Task *task);
    void saveAppointments(QDomElement &element);

    bool isOverbooked() const;
    bool isOverbooked(const QDate &date) const;
    bool isOverbooked(const DateTime &start, const DateTime &end) const;

    double normalRate() const { return cost.normalRate; }
    void setNormalRate(double rate) { cost.normalRate = rate; }
    double overtimeRate() const { return cost.overtimeRate; }
    void setOvertimeRate(double rate) { cost.overtimeRate = rate; }
    double fixedCost() const { return cost.fixed; }
    void setFixedCost(double value) { cost.fixed = value; }

    /**
     * Return available units in percent
     */
    int units() const { return m_units; }
    /**
     * Set available units in percent
     */
    void setUnits(int units) { m_units = units; }

    Project *project() const { return m_project; }

    Calendar *calendar() const;
    Calendar *calendar(const QString id) const;
    void setCalendar(Calendar *calendar) { m_calendar = calendar; }

    /**
     * Used to clean up requests when the resource is deleted.
     */
    void registerRequest(const ResourceRequest *request)
        { m_requests.append(request); }
    void unregisterRequest(const ResourceRequest *request)
        { m_requests.removeRef(request); }
    const QPtrList<ResourceRequest> &requests() const
        { return m_requests; }
        
    Duration effort(const DateTime &start, const Duration &duration, bool *ok=0) const;

    DateTime availableAfter(const DateTime &time);
    DateTime availableBefore(const DateTime &time);

    Resource *findId() const { return findId(m_id); }
    Resource *findId(const QString &id) const;
    bool removeId() { return removeId(m_id); }
    bool removeId(const QString &id);
    void insertId(const QString &id);

    Calendar *findCalendar(const QString &id) const;

    Appointment appointmentIntervals() const;
    Duration plannedEffort(const QDate &date) const;
private:
    Project *m_project;
    QPtrList<Appointment> m_appointments;
    QString m_id; // unique id
    QString m_name;
    QString m_initials;
    QString m_email;
    DateTime m_availableFrom;
    DateTime m_availableUntil;
    QPtrList<QTime> m_workingHours;

    int m_units; // avalable units in percent
    bool m_overbooked;

    Type m_type;

    struct Cost {
        double normalRate;
        double overtimeRate;
        double fixed;
    } cost;

    Calendar *m_calendar;
    QPtrList<ResourceRequest> m_requests;
    
public:
#ifndef NDEBUG
        void printDebug(QString ident);
#endif
};


class AppointmentInterval {
public:
    AppointmentInterval();
    AppointmentInterval(const AppointmentInterval &AppointmentInterval);
    AppointmentInterval(const DateTime &start, const DateTime end, double load=100);
    ~AppointmentInterval();
    
    void set(DateTime &start, DateTime &end, double load=100);
    void set(DateTime &start, Duration &duration, double load=100);
    
    Duration effort() const { return (m_end - m_start) * m_load / 100; }
    Duration effort(const DateTime &start, const DateTime end) const;
    Duration effort(const DateTime &time, bool upto) const;
    
    bool loadXML(QDomElement &element);
    void saveXML(QDomElement &element);
    
    const DateTime &startTime() const { return m_start; }
    void setStartTime(const DateTime &time) { m_start = time; }
    const DateTime &endTime() const { return m_end; }
    void setEndTime(const DateTime &time) { m_end = time; }
    double load() const { return m_load; }
    void setLoad(double load) { m_load = load; }
    
    bool isValid() const;
    AppointmentInterval firstInterval(const AppointmentInterval &interval, const DateTime &from) const;

private:
    DateTime m_start;
    DateTime m_end;
    double m_load; //percent
};


/**
 * This list is sorted after 1) startdatetime, 2) enddatetime.
 * The intervals do not overlap, an interval does not start before the
 * previous interval ends.
 */
class AppointmentIntervalList : public QPtrList<AppointmentInterval> {
protected:
    int compareItems(QPtrCollection::Item item1, QPtrCollection::Item item2) {
        AppointmentInterval *i1 = static_cast<AppointmentInterval*>(item1);
        AppointmentInterval *i2 = static_cast<AppointmentInterval*>(item2);
        if (i1->startTime() < i2->startTime()) {
            return -1;
        }
        if (i1->startTime() > i2->startTime()) {
            return 1;
        }
        if (i1->endTime() < i2->endTime()) {
            return -1;
        }
        if (i1->endTime() > i2->endTime()) {
            return 1;
        }
        return 0;
    }
};
typedef QPtrListIterator<AppointmentInterval> AppointmentIntervalListIterator;

/**
 * A resource (@ref Resource) can be scheduled to be used at any time, 
 * this is represented internally with Appointments
 * There is one Appointment per resource-task pair.
 * An appointment can be devided into several intervals, represented with
 * a list of AppointmentInterval.
 * This list is sorted after 1) startdatetime, 2) enddatetime.
 * The intervals do not overlap, an interval does not start before the
 * previous interval ends.
 * An interval is a countinous time interval with the same load. It can span dates.
 */
class Appointment {
public:
    Appointment();
    Appointment(Resource *resource, Node *node, DateTime start, DateTime end, double load);
    Appointment(Resource *resource, Node *node, DateTime start, Duration duration, double load);
    ~Appointment();

    // get/set member values.
    Node *node() const { return m_node; }
    void setNode(Node *n) { m_node = n; }

    Resource *resource() const { return m_resource; }
    void setResource(Resource *r) { m_resource = r; }

    DateTime startTime() const;
    DateTime endTime() const;
    double maxLoad() const;
    
    const Duration &repeatInterval() const {return m_repeatInterval;}
    void setRepeatInterval(Duration ri) {m_repeatInterval=ri;}

    int repeatCount() const { return m_repeatCount; }
    void setRepeatCount(int rc) { m_repeatCount=rc; }

    void deleteAppointmentFromRepeatList(DateTime time);
    void addAppointmentToRepeatList(DateTime time);

    bool isBusy(const DateTime &start, const DateTime &end);

    /// attach appointment to resource and node
    bool attach();
    /// detach appointment from resource and node
    void detach();
    
    void addInterval(AppointmentInterval *a);
    void addInterval(AppointmentInterval &a) 
        { addInterval(new AppointmentInterval(a)); }
    void addInterval(const DateTime &start, const DateTime &end, double load=100);
    void addInterval(const DateTime &start, const Duration &duration, double load=100);
    
    const AppointmentIntervalList &intervals() const { return m_intervals; }

    bool loadXML(QDomElement &element, Project &project);
    void saveXML(QDomElement &element);

    /**
     * Returns the planned effort and cost for the interval start to end.
     * Only dates with any planned effort is returned.
     */
    EffortCostMap plannedPrDay(const QDate& start, const QDate& end) const;
    
    /// Returns the planned effort from start to end
    Duration effort(const DateTime &start, const DateTime &end) const;
    /// Returns the planned effort from start for the duration
    Duration effort(const DateTime &start, const Duration &duration) const;
    /// Returns the planned effort from time onwards
    Duration effortFrom(const DateTime &time) const;
    
    /// Returns the total planned effort for this appointment
    Duration plannedEffort() const;
    /// Returns the planned effort on the date
    Duration plannedEffort(const QDate &date) const;
    /// Returns the planned effort upto and including date
    Duration plannedEffortTo(const QDate &date) const;

    /// Returns the total actual effort for this appointment
    Duration actualEffort() const;
    /// Returns the actual effort on the date
    Duration actualEffort(const QDate &date) const;
    /// Returns the actual effort on the date
    Duration actualEffortTo(const QDate &date) const;

     /// Calculates the total planned cost for this appointment
    double plannedCost();
    /// Calculates the planned cost on date
    double plannedCost(const QDate &date);
    /// Calculates the planned cost upto and including date
    double plannedCostTo(const QDate &date);

     /// Calculates the total actual cost for this appointment
    double actualCost();
     /// Calculates the actual cost on date
    double actualCost(const QDate &date);
    /// Calculates the actual cost upto and including date
    double actualCostTo(const QDate &date);

    Appointment &operator=(const Appointment &app);
    Appointment &operator+=(const Appointment &app);
    Appointment operator+(const Appointment &app);
    
    void addActualEffort(QDate date, Duration effort, bool overtime=false);
    
private:
    Node *m_node;
    Resource *m_resource;
    Duration m_repeatInterval;
    int m_repeatCount;
    QPtrList<Duration> m_extraRepeats;
    QPtrList<Duration> m_skipRepeats;

    AppointmentIntervalList m_intervals;
    
    class UsedEffortItem {
    public:
        UsedEffortItem(QDate date, Duration effort, bool overtime=false);
        QDate date();
        Duration effort();
        bool isOvertime();
    private:
        QDate m_date;
        Duration m_effort;
        bool m_overtime;
    };
    class UsedEffort : QPtrList<UsedEffortItem> {
    public:
        UsedEffort();
        ~UsedEffort() {}
        void inSort(QDate date, Duration effort, bool overtime=false);
        Duration usedEffort(bool includeOvertime=true) const;
        Duration usedEffort(const QDate &date, bool includeOvertime=true) const;
        Duration usedEffortTo(const QDate &date, bool includeOvertime=true) const;
        Duration usedOvertime() const;
        Duration usedOvertime(const QDate &date) const;
        Duration usedOvertimeTo(const QDate &date) const;
        bool load(QDomElement &element);
        void save(QDomElement &element);
    
    protected:
        int compareItems(QPtrCollection::Item item1, QPtrCollection::Item item2);
    };
    
    UsedEffort m_actualEffort;

#ifndef NDEBUG
public:
        void printDebug(QString ident);
#endif
};


/**
 * Risk is associated with a resource/task pairing to indicate the planner's confidence in the
 * estimated effort. Risk can be one of none, low, or high. Some factors that may be taken into
 * account for risk are the experience of the person and the reliability of equipment.
 */
class Risk {
    public:

        enum RiskType {
            NONE=0,
            LOW=1,
            HIGH=2
        };

        Risk(Node *n, Resource *r, RiskType rt=NONE);
        ~Risk();

        RiskType riskType() { return m_riskType; }

        Node *node() { return m_node; }
        Resource *resource() { return m_resource; }

    private:
        Node *m_node;
        Resource *m_resource;
        RiskType m_riskType;
};

class ResourceRequest {
    public:
        ResourceRequest(Resource *resource=0, int units = 1);

        ~ResourceRequest();

        ResourceGroupRequest *parent() const { return m_parent; }
        void setParent(ResourceGroupRequest *parent) { m_parent = parent; }
        
        Resource *resource() const { return m_resource; }
        void setResource(Resource* resource) { m_resource = resource; }
        
        bool load(QDomElement &element, Project &project);
        void save(QDomElement &element);

        /**
        * Get amount of requested resource units in percent
        */
        int units() const;
        
        /**
        * Get amount of requested work units in percent
        */
        int workUnits() const;
                
        void registerRequest() { if (m_resource) m_resource->registerRequest(this); }
        void unregisterRequest() { if (m_resource) m_resource->unregisterRequest(this); }
 
        void makeAppointment(DateTime &start, Duration &duration, Task *task) { 
            if (m_resource) 
                m_resource->makeAppointment(start, duration, task);
        }
        
    private:
        Resource *m_resource;
        int m_units;
        ResourceGroupRequest *m_parent;

#ifndef NDEBUG
public:
        void printDebug(QString ident);
#endif
};

class ResourceGroupRequest {
    public:
        ResourceGroupRequest(ResourceGroup *group=0, int units=0);
        ~ResourceGroupRequest();

        void setParent(ResourceRequestCollection *parent) { m_parent = parent;}
        ResourceRequestCollection *parent() const { return m_parent; }
        
        ResourceGroup *group() const { return m_group; }
        QPtrList<ResourceRequest> &resourceRequests() { return m_resourceRequests; }
        void addResourceRequest(ResourceRequest *request);
        void removeResourceRequest(ResourceRequest *request) { m_resourceRequests.removeRef(request); }
        ResourceRequest *takeResourceRequest(ResourceRequest *request);
        ResourceRequest *find(Resource *resource) const;

        bool load(QDomElement &element, Project &project);
        void save(QDomElement &element);

        /**
        * Get total amount of resource units in percent
        */
        int units() const;
    
        /**
        * Get amount of work units in percent
        */
        int workUnits() const;
    
        Duration effort(const DateTime &time, const Duration &duration, bool *ok=0) const;
        
        /**
         * Returns the duration needed to do the effort @param effort
         * starting at @param start.
         */
        Duration duration(const DateTime &start, const Duration &effort, bool backward=false);
        
        DateTime availableAfter(const DateTime &time);
        DateTime availableBefore(const DateTime &time);
        
        /**
         * Makes appointments for task @param task to the 
         * requested resources for the duration found in @ref duration().
         */
        void makeAppointments(Task *task);
            
        /**
         * Reserves the requested resources for the specified interval
         */
        void reserve(const DateTime &start, const Duration &duration);

        bool isEmpty() const;
    
    private:
        ResourceGroup *m_group;
        int m_units;
        ResourceRequestCollection *m_parent;
        
        QPtrList<ResourceRequest> m_resourceRequests;
        DateTime m_start;
        Duration m_duration;

#ifndef NDEBUG
public:
        void printDebug(QString ident);
#endif
};

class ResourceRequestCollection {
public:
    ResourceRequestCollection(Task &task);
    ~ResourceRequestCollection();

    const QPtrList<ResourceGroupRequest> &requests() const { return m_requests; }
    void addRequest(ResourceGroupRequest *request) {                 
        m_requests.append(request);
        request->setParent(this);
    }
    void removeRequest(ResourceGroupRequest *request) { m_requests.removeRef(request); }
    void takeRequest(ResourceGroupRequest *request) { m_requests.take(m_requests.findRef(request)); }
    ResourceGroupRequest *find(ResourceGroup *resource) const;
    ResourceRequest *find(Resource *resource) const;
    bool isEmpty() const;
    
    //bool load(QDomElement &element, Project &project);
    void save(QDomElement &element);

    void clear() { m_requests.clear(); }
    
    /**
    * Returns the total amount of resource units in percent
    */
    int units() const;
    
    /**
    * Returns the amount of work units in percent
    */
    int workUnits() const;
    
    /**
    * Returns the duration needed to do the effort @param effort
    * starting at @param time.
    */
    Duration duration(const DateTime &time, const Duration &effort, bool backward=false);
    
    DateTime availableAfter(const DateTime &time);
    DateTime availableBefore(const DateTime &time);
    
    /**
    * Makes appointments for the task @param task to the requested resources.
    * Assumes that @ref duration() has been run.
    */
    void makeAppointments(Task *task);
    /**
     * Reserves the requested resources for the specified interval
     */
    void reserve(const DateTime &start, const Duration &duration);

    Task &task() const { return m_task; }
    
protected:
    struct Interval {
        DateTime start;
        DateTime end;
        Duration effort;
    };
    

private:
    Task &m_task;
    QPtrList<ResourceGroupRequest> m_requests;

#ifndef NDEBUG
public:
        void printDebug(QString ident);
#endif
};

}  //KPlato namespace

#endif
