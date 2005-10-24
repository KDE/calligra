/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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

class KPTAccount;
class KPTRisk;
class KPTEffort;
class KPTAppointment;
class KPTTask;
class KPTNode;
class KPTProject;
class KPTResource;
class KPTResourceRequest;
class KPTResourceGroupRequest;
class KPTCalendar;
class KPTResourceRequestCollection;

/**
  * This class represents a group of similar resources to be assigned to a task
  * e.g. The list of employees, computer resources, etc
  */

/* IDEA; lets create a resourceGroup that has the intelligence to import PIM schedules
 *  from the kroupware project and use the schedules to use the factory pattern to build
 *  KPTResources (probably a derived class) which returns values on getFirstAvailableTime
 *  and friends based on the schedules we got from the PIM projects.
 *  (Thomas Zander mrt-2003 by suggestion of Shaheed)
 */
class KPTResourceGroup {
    public:
	      KPTResourceGroup(KPTProject *project);
	      ~KPTResourceGroup();

          enum Type { Type_Work, Type_Material };
          
          QString id() const { return m_id; }
          bool setId(QString id);
          void generateId();
          
          KPTProject *project() { return m_project; }
          
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
	        * to manipulate risks (@ref KPTRisk) seperately
	        */
	      void addResource(KPTResource*, KPTRisk*);
          void insertResource( unsigned int index, KPTResource *resource );
          void removeResource( KPTResource *resource );
          KPTResource *takeResource( KPTResource *resource );
	      void removeResource(int);

	      KPTResource* getResource(int);
	      KPTRisk* getRisk(int);

	      /** Get the @num resources which is available in the time frame
            * defined by @start and @duration.
            */
          QPtrList<KPTResource> availableResources(const KPTDateTime start, const KPTDuration duration, int num);
	      /** Manage the dependent resources.  This is a list of the resource
	        * groups that must have available resources for this resource to
	        * perform the work
            * <p>see also @ref getRequiredResource, @ref getRequiredResource
	        */
	      void addRequiredResource(KPTResourceGroup*);
	      /** Manage the dependent resources.  This is a list of the resource
	        * groups that must have available resources for this resource to
	        * perform the work
            * <p>see also @ref addRequiredResource, @ref getRequiredResource
	        */
	      KPTResourceGroup* getRequiredResource(int);
	      /** Manage the dependent resources.  This is a list of the resource
	        * groups that must have available resources for this resource to
	        * perform the work
            * <p>see also @ref getRequiredResource, @ref addRequiredResource
	        */
	      void removeRequiredResource(int);
          int numResources() const { return m_resources.count(); }
          QPtrList<KPTResource> &resources() { return m_resources; }

          bool load(QDomElement &element);
          void save(QDomElement &element);

          void clearAppointments();
          void saveAppointments(QDomElement &element);


          void addNode(const KPTNode *node) { m_nodes.append(node); }
          void clearNodes() { m_nodes.clear(); }

          KPTCalendar *defaultCalendar() { return m_defaultCalendar; }

          int units();
        
          void registerRequest(KPTResourceGroupRequest *request)
            { m_requests.append(request); }
          void unregisterRequest(KPTResourceGroupRequest *request)
            { m_requests.removeRef(request); }
          const QPtrList<KPTResourceGroupRequest> &requests() const
            { return m_requests; }

          KPTResourceGroup *findId() const { return findId(m_id); }
          KPTResourceGroup *findId(const QString &id) const;
          bool removeId() { return removeId(m_id); }
          bool removeId(const QString &id);
          void insertId(const QString &id);

          KPTAppointment appointmentIntervals() const;

#ifndef NDEBUG
        void printDebug(QString ident);
#endif

    private:
        KPTProject  *m_project;
        QString m_id;   // unique id
        QString m_name;
        QPtrList<KPTResource> m_resources;
        QPtrList<KPTRisk> m_risks;
        QPtrList<KPTResourceGroup> m_requires;

        QPtrList<KPTNode> m_nodes; //The nodes that want resources from us

        KPTCalendar *m_defaultCalendar;
        Type m_type;
        
        QPtrList<KPTResourceGroupRequest> m_requests;
        
};

/**
  * Any resource that is used by a task. A resource can be a worker, or maybe wood.
  * If the resources is a worker or a piece of equiment which can be reused but
  * can only be used by one node in time, then we can use the scheduling methods of the
  * resource to schedule the resource available time for the project.
  * The Idea is that all nodes which need this resource point to it and the scheduling
  * code (partly implemented here) schedules the actual usage.
  * See also @ref KPTResourceGroup
  */

class KPTResource {
public:

    KPTResource(KPTProject *project);
    KPTResource(KPTResource *resource) { copy(resource); }
    virtual ~KPTResource();

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

    void copy(KPTResource *resource);

    /// Set the time from when the resource is available to this project
    void setAvailableFrom(const QDateTime &af) {m_availableFrom=af;}
    /// Return the time when the resource is available to this project
    const KPTDateTime &availableFrom() const {return m_availableFrom;}
    /// Set the time when the resource is no longer available to this project
    void setAvailableUntil(const QDateTime au) {m_availableUntil=au;}
    /// Return the time when the resource is no longer available to this project.
    const KPTDateTime &availableUntil() const {return m_availableUntil;}

    void addWorkingHour(QTime from, QTime until);
    QPtrList<QTime> workingHours() { return m_workingHours; }

    KPTDateTime *getFirstAvailableTime(KPTDateTime after = KPTDateTime());
    KPTDateTime *getBestAvailableTime(KPTDuration duration);
    KPTDateTime *getBestAvailableTime(const KPTDateTime after, const KPTDuration duration);

    bool load(QDomElement &element);
    void save(QDomElement &element);

    /// removes appoinrment and deletes it (independent of setAutoDelete)
    void removeAppointment(KPTAppointment *appointment);
    /// removes appointment without deleting it (independent of setAutoDelete)
    void takeAppointment(KPTAppointment *appointment);
    
    ///Return the list of appointments
    QPtrList<KPTAppointment> &appointments() { return m_appointments; }
    
    KPTAppointment *findAppointment(KPTNode *node);
    int numAppointments() const { return m_appointments.count(); }
    /// Adds appointment to this resource only (not to node)
    virtual bool addAppointment(KPTAppointment *appointment);
    /// Adds appointment to both this resource and node
    virtual void addAppointment(KPTNode *node, KPTDateTime &start, KPTDateTime &end, double load=100);
    
    void clearAppointments();
    bool isAvailable(KPTTask *task);
    void makeAppointment(KPTDateTime &start, KPTDuration &duration, KPTTask *task);
    void saveAppointments(QDomElement &element);

    bool isOverbooked() const;
    bool isOverbooked(const QDate &date) const;
    bool isOverbooked(const KPTDateTime &start, const KPTDateTime &end) const;

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

    KPTProject *project() const { return m_project; }

    KPTCalendar *calendar() const;
    KPTCalendar *calendar(const QString id) const;
    void setCalendar(KPTCalendar *calendar) { m_calendar = calendar; }

    /**
     * Used to clean up requests when the resource is deleted.
     */
    void registerRequest(const KPTResourceRequest *request)
        { m_requests.append(request); }
    void unregisterRequest(const KPTResourceRequest *request)
        { m_requests.removeRef(request); }
    const QPtrList<KPTResourceRequest> &requests() const
        { return m_requests; }
        
    KPTDuration effort(const KPTDateTime &start, const KPTDuration &duration, bool *ok=0) const;

    KPTDateTime availableAfter(const KPTDateTime &time);
    KPTDateTime availableBefore(const KPTDateTime &time);

    KPTResource *findId() const { return findId(m_id); }
    KPTResource *findId(const QString &id) const;
    bool removeId() { return removeId(m_id); }
    bool removeId(const QString &id);
    void insertId(const QString &id);

    KPTCalendar *findCalendar(const QString &id) const;

    KPTAppointment appointmentIntervals() const;
    KPTDuration plannedEffort(const QDate &date) const;
private:
    KPTProject *m_project;
    QPtrList<KPTAppointment> m_appointments;
    QString m_id; // unique id
    QString m_name;
    QString m_initials;
    QString m_email;
    KPTDateTime m_availableFrom;
    KPTDateTime m_availableUntil;
    QPtrList<QTime> m_workingHours;

    int m_units; // avalable units in percent
    bool m_overbooked;

    Type m_type;

    struct Cost {
        double normalRate;
        double overtimeRate;
        double fixed;
    } cost;

    KPTCalendar *m_calendar;
    QPtrList<KPTResourceRequest> m_requests;
    
public:
#ifndef NDEBUG
        void printDebug(QString ident);
#endif
};


class KPTAppointmentInterval {
public:
    KPTAppointmentInterval();
    KPTAppointmentInterval(const KPTAppointmentInterval &KPTAppointmentInterval);
    KPTAppointmentInterval(const KPTDateTime &start, const KPTDateTime end, double load=100);
    ~KPTAppointmentInterval();
    
    void set(KPTDateTime &start, KPTDateTime &end, double load=100);
    void set(KPTDateTime &start, KPTDuration &duration, double load=100);
    
    KPTDuration effort() const { return (m_end - m_start) * m_load / 100; }
    KPTDuration effort(const KPTDateTime &start, const KPTDateTime end) const;
    KPTDuration effort(const KPTDateTime &time, bool upto) const;
    
    bool loadXML(QDomElement &element);
    void saveXML(QDomElement &element);
    
    const KPTDateTime &startTime() const { return m_start; }
    void setStartTime(const KPTDateTime &time) { m_start = time; }
    const KPTDateTime &endTime() const { return m_end; }
    void setEndTime(const KPTDateTime &time) { m_end = time; }
    double load() const { return m_load; }
    void setLoad(double load) { m_load = load; }
    
    bool isValid() const;
    KPTAppointmentInterval firstInterval(const KPTAppointmentInterval &interval, const KPTDateTime &from) const;

private:
    KPTDateTime m_start;
    KPTDateTime m_end;
    double m_load; //percent
};

class KPTAppointmentIntervalList : public QPtrList<KPTAppointmentInterval> {
protected:
    int compareItems(QPtrCollection::Item item1, QPtrCollection::Item item2) {
        KPTAppointmentInterval *i1 = static_cast<KPTAppointmentInterval*>(item1);
        KPTAppointmentInterval *i2 = static_cast<KPTAppointmentInterval*>(item2);
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

/**
  * A resource (@ref KPTResource) can be scheduled to be used at any time, 
  * this is represented internally with KPTAppointments
  * There is one KPTAppointment per resource-task pair.
  * An appointment can be devided into several intervals, represented with
  * a list of KPTAppointmentInterval
  */

class KPTAppointment {
public:
    KPTAppointment();
    KPTAppointment(KPTResource *resource, KPTNode *node, KPTDateTime start, KPTDateTime end, double load);
    KPTAppointment(KPTResource *resource, KPTNode *node, KPTDateTime start, KPTDuration duration, double load);
    ~KPTAppointment();

    // get/set member values.
    KPTNode *node() const { return m_node; }
    void setNode(KPTNode *n) { m_node = n; }

    KPTResource *resource() const { return m_resource; }
    void setResource(KPTResource *r) { m_resource = r; }

    KPTDateTime startTime() const;
    KPTDateTime endTime() const;
    double maxLoad() const;
    
    const KPTDuration &repeatInterval() const {return m_repeatInterval;}
    void setRepeatInterval(KPTDuration ri) {m_repeatInterval=ri;}

    int repeatCount() const { return m_repeatCount; }
    void setRepeatCount(int rc) { m_repeatCount=rc; }

    void deleteAppointmentFromRepeatList(KPTDateTime time);
    void addAppointmentToRepeatList(KPTDateTime time);

    bool isBusy(const KPTDateTime &start, const KPTDateTime &end);

    /// attach appointment to resource and node
    bool attach();
    /// detach appointment from resource and node
    void detach();
    
    void addInterval(KPTAppointmentInterval *a);
    void addInterval(KPTAppointmentInterval &a) 
        { addInterval(new KPTAppointmentInterval(a)); }
    void addInterval(const KPTDateTime &start, const KPTDateTime &end, double load=100);
    void addInterval(const KPTDateTime &start, const KPTDuration &duration, double load=100);
    
    const KPTAppointmentIntervalList &intervals() const { return m_intervals; }

    bool loadXML(QDomElement &element, KPTProject &project);
    void saveXML(QDomElement &element);

    /// Returns the planned effort from start to end
    KPTDuration effort(const KPTDateTime &start, const KPTDateTime &end) const;
    /// Returns the planned effort from start for the duration
    KPTDuration effort(const KPTDateTime &start, const KPTDuration &duration) const;
    /// Returns the planned effort from time onwards
    KPTDuration effortFrom(const KPTDateTime &time) const;
    
    /// Returns the total planned effort for this appointment
    KPTDuration plannedEffort() const;
    /// Returns the planned effort on the date
    KPTDuration plannedEffort(const QDate &date) const;
    /// Returns the planned effort upto and including date
    KPTDuration plannedEffortTo(const QDate &date) const;

    /// Returns the total actual effort for this appointment
    KPTDuration actualEffort() const;
    /// Returns the actual effort on the date
    KPTDuration actualEffort(const QDate &date) const;
    /// Returns the actual effort on the date
    KPTDuration actualEffortTo(const QDate &date) const;

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

    KPTAppointment &operator=(const KPTAppointment &app);
    KPTAppointment &operator+=(const KPTAppointment &app);
    KPTAppointment operator+(const KPTAppointment &app);
    
    void addActualEffort(QDate date, KPTDuration effort, bool overtime=false);
    
private:
    KPTNode *m_node;
    KPTResource *m_resource;
    KPTDuration m_repeatInterval;
    int m_repeatCount;
    QPtrList<KPTDuration> m_extraRepeats;
    QPtrList<KPTDuration> m_skipRepeats;

    KPTAppointmentIntervalList m_intervals;
    
    class UsedEffortItem {
    public:
        UsedEffortItem(QDate date, KPTDuration effort, bool overtime=false);
        QDate date();
        KPTDuration effort();
        bool isOvertime();
    private:
        QDate m_date;
        KPTDuration m_effort;
        bool m_overtime;
    };
    class UsedEffort : QPtrList<UsedEffortItem> {
    public:
        UsedEffort();
        ~UsedEffort() {}
        void inSort(QDate date, KPTDuration effort, bool overtime=false);
        KPTDuration usedEffort(bool includeOvertime=true) const;
        KPTDuration usedEffort(const QDate &date, bool includeOvertime=true) const;
        KPTDuration usedEffortTo(const QDate &date, bool includeOvertime=true) const;
        KPTDuration usedOvertime() const;
        KPTDuration usedOvertime(const QDate &date) const;
        KPTDuration usedOvertimeTo(const QDate &date) const;
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
class KPTRisk {
    public:

        enum RiskType {
            NONE=0,
            LOW=1,
            HIGH=2
        };

        KPTRisk(KPTNode *n, KPTResource *r, RiskType rt=NONE);
        ~KPTRisk();

        RiskType riskType() { return m_riskType; }

        KPTNode *node() { return m_node; }
        KPTResource *resource() { return m_resource; }

    private:
        KPTNode *m_node;
        KPTResource *m_resource;
        RiskType m_riskType;
};

class KPTResourceRequest {
    public:
        KPTResourceRequest(KPTResource *resource=0, int units = 1);

        ~KPTResourceRequest();

        KPTResourceGroupRequest *parent() const { return m_parent; }
        void setParent(KPTResourceGroupRequest *parent) { m_parent = parent; }
        
        KPTResource *resource() const { return m_resource; }
        void setResource(KPTResource* resource) { m_resource = resource; }
        
        bool load(QDomElement &element, KPTProject *project);
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
 
        void makeAppointment(KPTDateTime &start, KPTDuration &duration, KPTTask *task) 
            { if (m_resource) m_resource->makeAppointment(start, duration, task); }
        
        KPTAccount *account() const { return m_account; }
        void setAccount(KPTAccount *account) { m_account = account; }
        
    private:
        KPTResource *m_resource;
        int m_units;
        KPTResourceGroupRequest *m_parent;
        KPTAccount *m_account;

#ifndef NDEBUG
public:
        void printDebug(QString ident);
#endif
};

class KPTResourceGroupRequest {
    public:
        KPTResourceGroupRequest(KPTResourceGroup *group=0, int units=0);
        ~KPTResourceGroupRequest();

        void setParent(KPTResourceRequestCollection *parent) { m_parent = parent;}
        KPTResourceRequestCollection *parent() const { return m_parent; }
        
        KPTResourceGroup *group() const { return m_group; }
        QPtrList<KPTResourceRequest> &resourceRequests() { return m_resourceRequests; }
        void addResourceRequest(KPTResourceRequest *request);
        void removeResourceRequest(KPTResourceRequest *request) { m_resourceRequests.removeRef(request); }
        KPTResourceRequest *takeResourceRequest(KPTResourceRequest *request);
        KPTResourceRequest *find(KPTResource *resource) const;

        bool load(QDomElement &element, KPTProject *project);
        void save(QDomElement &element);

        /**
        * Get total amount of resource units in percent
        */
        int units() const;
    
        /**
        * Get amount of work units in percent
        */
        int workUnits() const;
    
        KPTDuration effort(const KPTDateTime &time, const KPTDuration &duration, bool *ok=0) const;
        
        /**
         * Returns the duration needed to do the effort @param effort
         * starting at @param start.
         */
        KPTDuration duration(const KPTDateTime &start, const KPTDuration &effort, bool backward=false);
        
        KPTDateTime availableAfter(const KPTDateTime &time);
        KPTDateTime availableBefore(const KPTDateTime &time);
        
        /**
         * Makes appointments for task @param task to the 
         * requested resources for the duration found in @ref duration().
         */
        void makeAppointments(KPTTask *task);
            
        /**
         * Reserves the requested resources for the specified interval
         */
        void reserve(const KPTDateTime &start, const KPTDuration &duration);

        bool isEmpty() const;
    
    private:
        KPTResourceGroup *m_group;
        int m_units;
        KPTResourceRequestCollection *m_parent;
        
        QPtrList<KPTResourceRequest> m_resourceRequests;
        KPTDateTime m_start;
        KPTDuration m_duration;

#ifndef NDEBUG
public:
        void printDebug(QString ident);
#endif
};

class KPTResourceRequestCollection {
public:
    KPTResourceRequestCollection(KPTTask &task);
    ~KPTResourceRequestCollection();

    const QPtrList<KPTResourceGroupRequest> &requests() const { return m_requests; }
    void addRequest(KPTResourceGroupRequest *request) {                 
        m_requests.append(request);
        request->setParent(this);
    }
    void removeRequest(KPTResourceGroupRequest *request) { m_requests.removeRef(request); }
    void takeRequest(KPTResourceGroupRequest *request) { m_requests.take(m_requests.findRef(request)); }
    KPTResourceGroupRequest *find(KPTResourceGroup *resource) const;
    KPTResourceRequest *find(KPTResource *resource) const;
    bool isEmpty() const;
    
    //bool load(QDomElement &element, KPTProject *project);
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
    KPTDuration duration(const KPTDateTime &time, const KPTDuration &effort, bool backward=false);
    
    KPTDateTime availableAfter(const KPTDateTime &time);
    KPTDateTime availableBefore(const KPTDateTime &time);
    
    /**
    * Makes appointments for the task @param task to the requested resources.
    * Assumes that @ref duration() has been run.
    */
    void makeAppointments(KPTTask *task);
    /**
     * Reserves the requested resources for the specified interval
     */
    void reserve(const KPTDateTime &start, const KPTDuration &duration);

    KPTTask &task() const { return m_task; }
    
protected:
    struct Interval {
        KPTDateTime start;
        KPTDateTime end;
        KPTDuration effort;
    };
    

private:
    KPTTask &m_task;
    QPtrList<KPTResourceGroupRequest> m_requests;

#ifndef NDEBUG
public:
        void printDebug(QString ident);
#endif
};

}  //KPlato namespace

#endif
