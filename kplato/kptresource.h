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
#include <qintdict.h>
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
class Schedule;
class ResourceSchedule;
class Schedule;

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
          void save(QDomElement &element) const;

          void initiateCalculation(Schedule &sch);

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
    Resource(Resource *resource);
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

    DateTime getFirstAvailableTime(DateTime after = DateTime());
    DateTime getBestAvailableTime(Duration duration);
    DateTime getBestAvailableTime(const DateTime after, const Duration duration);

    bool load(QDomElement &element);
    void save(QDomElement &element) const;

    ///Return the list of appointments for current schedule.
    QPtrList<Appointment> appointments();
    
    Appointment *findAppointment(Node *node);
    /// Adds appointment to current schedule
    virtual bool addAppointment(Appointment *appointment);
    /// Adds appointment to schedule sch
    virtual bool addAppointment(Appointment *appointment, Schedule &main);
    /// Adds appointment to both this resource and node
    virtual void addAppointment(Schedule *node, DateTime &start, DateTime &end, double load=100);
    
    void initiateCalculation(Schedule &sch);
    bool isAvailable(Task *task);
    void makeAppointment(Schedule *schedule);

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

    /**
     * Get the calendar for this resource. 
     * If local=false, check if there is a default calendar.
     */
    Calendar *calendar(bool local=false) const;
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
        
    Duration effort(const DateTime &start, const Duration &duration, bool backward, bool *ok=0) const;

    /**
     * Find the first available time after time, within limit.
     * Returns invalid DateTime if not available.
     */
    DateTime availableAfter(const DateTime &time, const DateTime limit=DateTime(), bool checkAppointments=false) const;
    /**
     * Find the first available time before time, within limit.
     * Returns invalid DateTime if not available.
     */
    DateTime availableBefore(const DateTime &time, const DateTime limit=DateTime(), bool checkAppointments=false) const;

    Resource *findId() const { return findId(m_id); }
    Resource *findId(const QString &id) const;
    bool removeId() { return removeId(m_id); }
    bool removeId(const QString &id);
    void insertId(const QString &id);

    Calendar *findCalendar(const QString &id) const;

    Appointment appointmentIntervals() const;
    Duration plannedEffort(const QDate &date) const;

    void setCurrentSchedule(Schedule *schedule) { m_currentSchedule = schedule; }
    void setCurrentSchedule(long id) { m_currentSchedule = findSchedule(id); }
    Schedule *currentSchedule() const { return m_currentSchedule; }
    
    QIntDict<Schedule> &schedules() { return m_schedules; }
    Schedule *findSchedule(long id) { return m_schedules[id]; }
    /// Take, and delete.
    void removeSchedule(Schedule *schedule);
    /// Take, don't delete.
    void takeSchedule(const Schedule *schedule);
    void addSchedule(Schedule *schedule);
    ResourceSchedule *createSchedule(QString name, int type, int id);
    ResourceSchedule *createSchedule(Schedule *parent);
    
protected:
    void makeAppointment(Schedule *node, const DateTime &from, const DateTime &end);
    
private:
    Project *m_project;
    QIntDict<Schedule> m_schedules;
    QString m_id; // unique id
    QString m_name;
    QString m_initials;
    QString m_email;
    DateTime m_availableFrom;
    DateTime m_availableUntil;
    QPtrList<QTime> m_workingHours;

    int m_units; // avalable units in percent

    Type m_type;

    struct Cost {
        double normalRate;
        double overtimeRate;
        double fixed;
    } cost;

    Calendar *m_calendar;
    QPtrList<ResourceRequest> m_requests;
    
    Schedule *m_currentSchedule;
    
public:
#ifndef NDEBUG
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
        void save(QDomElement &element) const;

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
 
        void makeAppointment(Schedule *schedule) { 
            if (m_resource) 
                m_resource->makeAppointment(schedule);
        }
        Task *task() const;
    
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
        void save(QDomElement &element) const;

        /**
        * Get total amount of resource units in percent
        */
        int units() const;
    
        /**
        * Get amount of work units in percent
        */
        int workUnits() const;
    
        Duration effort(const DateTime &time, const Duration &duration, bool backward, bool *ok=0) const;
        
        int numDays(const DateTime &time, bool backward) const;
        
        /**
         * Returns the duration needed to do the effort  effort
         * starting at start.
         */
        Duration duration(const DateTime &start, const Duration &effort, bool backward=false);
        
        DateTime availableAfter(const DateTime &time);
        DateTime availableBefore(const DateTime &time);
        
        /**
         * Makes appointments for task @param task to the 
         * requested resources for the duration found in @ref duration().
         */
        void makeAppointments(Schedule *schedule);
            
        /**
         * Reserves the requested resources for the specified interval
         */
        void reserve(const DateTime &start, const Duration &duration);

        bool isEmpty() const;
        
        Task *task() const;
        
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
    void save(QDomElement &element) const;

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
    void makeAppointments(Schedule *schedule);
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
