/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KPTRESOURCE_H
#define KPTRESOURCE_H

#include "kptduration.h"
#include "kptdatetime.h"

#include <qdom.h>
#include <qstring.h>
#include <qptrlist.h>
#include "defs.h"

class KPTRisk;
class KPTEffort;
class KPTAppointment;
class KPTTask;
class KPTNode;
class KPTResource;
class KPTResourceRequest;
class KPTResourceGroupRequest;
class KPTProject;
class KPTCalendar;
class QTime;

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
          
          int id() { return m_id; }
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

         /**
         * Return a list of appointments made for the @param task
         */
          QPtrList<KPTAppointment> appointments(const KPTNode *node) const;
          void clearAppointments();
          void saveAppointments(QDomElement &element) const;


          void addNode(const KPTNode *node) { m_nodes.append(node); }
          void clearNodes() { m_nodes.clear(); }

          KPTCalendar *defaultCalendar() { return m_defaultCalendar; }

          int units();
        
#ifndef NDEBUG
        void printDebug(QString ident);
#endif

    private:
        KPTProject  *m_project;
        int m_id;   // unique id
        QString m_name;
        QPtrList<KPTResource> m_resources;
        QPtrList<KPTRisk> m_risks;
        QPtrList<KPTResourceGroup> m_requires;

        QPtrList<KPTNode> m_nodes; //The nodes that want resources from us

        KPTCalendar *m_defaultCalendar;
        Type m_type;
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
        ~KPTResource();

        int id() { return m_id; }

        enum Type { Type_Work, Type_Material };
        void setType(Type type) { m_type = type; }
        void setType(const QString &type);
        Type type() const { return m_type; }
        QString typeToString() const;

	    void setName(QString n) {m_name=n;}
	    const QString &name() const {return m_name;}

        void copy(KPTResource *resource);

        //TODO: calendar stuff
        void setAvailableFrom(QTime af) {m_availableFrom=af;}
        const QTime &availableFrom() const {return m_availableFrom;}
        void setAvailableUntil(QTime au) {m_availableUntil=au;}
        const QTime &availableUntil() const {return m_availableUntil;}

        void addWorkingHour(QTime from, QTime until);
        QPtrList<QTime> workingHours() { return m_workingHours; }

        KPTDateTime *getFirstAvailableTime(KPTDateTime after = KPTDateTime());
        KPTDateTime *getBestAvailableTime(KPTDuration duration);
        KPTDateTime *getBestAvailableTime(const KPTDateTime after, const KPTDuration duration);

        bool load(QDomElement &element);
        void save(QDomElement &element);

        QPtrList<KPTAppointment> &appointments() { return m_appointments; }
        /**
         * Return a list of appointments made for the @task
         */
        QPtrList<KPTAppointment> appointments(const KPTNode *node) const;

        bool isAvailable(KPTTask *task);
        void addAppointment(KPTAppointment *a);
        void clearAppointments();
        void makeAppointment(KPTDateTime &start, KPTDuration &duration, KPTTask *task);
        void saveAppointments(QDomElement &element) const;

        void setOverbooked(bool on) { m_overbooked = on; }
        bool isOverbooked() { return m_overbooked; }

        double normalRate() const { return cost.normalRate; }
        void setNormalRate(double rate) { cost.normalRate = rate; }
        double overtimeRate() const { return cost.overtimeRate; }
        void setOvertimeRate(double rate) { cost.overtimeRate = rate; }
        double fixedCost() const { return cost.fixed; }
        void setFixedCost(double value) { cost.fixed = value; }

        /**
         * Return availbale units in percent
         */
        int units() const { return m_units; }
        /**
         * Set available units in percent
         */
        void setUnits(int units) { m_units = units; }

        KPTProject *project() { return m_project; }

        KPTCalendar *calendar() const;
        KPTCalendar *calendar(int id) const;
        void setCalendar(KPTCalendar *calendar) { m_calendar = calendar; }
    
        /**
         * Used to clean up requests when the resource is deleted.
         */
        void registerRequest(const KPTResourceRequest *request)
            { m_requests.append(request); }
        void unregisterRequest(const KPTResourceRequest *request)
            { m_requests.removeRef(request); }
        
        KPTDuration effort(const KPTDateTime &start, const KPTDuration &duration) const;

    private:
        KPTProject *m_project;
        QPtrList<KPTAppointment> m_appointments; // TODO: Move appointments to KPTProject ????
        int m_id; // unique id
        QString m_name;
        QTime m_availableFrom;
        QTime m_availableUntil;
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

/**
  * A resource (@ref KPTResource) can be scheduled to be used at any time, this is represented
  * internally with KPTAppointments
  */
class KPTAppointment {
    public:
        KPTAppointment() {}
        KPTAppointment(KPTDateTime startTime, KPTDuration duration, KPTResource *resource, KPTTask *taskNode=0);
        ~KPTAppointment();

        // get/set member values.
        const KPTDateTime &startTime() const { return m_startTime; }
        void setStartTime(KPTDateTime st) { m_startTime=st; }

        const KPTDuration &duration() const { return m_duration; }
        void setDuration(KPTDuration d) { m_duration=d; }

        KPTTask *task() const { return m_task; }
        void setTask(KPTTask *t) { m_task = t; }

        KPTResource *resource() const { return m_resource; }
        void setResource(KPTResource *r) { m_resource = r; }

        const KPTDuration &repeatInterval() const {return m_repeatInterval;}
        void setRepeatInterval(KPTDuration ri) {m_repeatInterval=ri;}

        int repeatCount() const { return m_repeatCount; }
        void setRepeatCount(int rc) { m_repeatCount=rc; }

        void deleteAppointmentFromRepeatList(KPTDateTime time);
        void addAppointmentToRepeatList(KPTDateTime time);

        bool isBusy(const KPTDateTime &start, const KPTDateTime &end);

        bool load(QDomElement &element, KPTProject &project);
        void save(QDomElement &element);

        /**
         * Calculates the total planned cost for this appointment
         */
        double cost();
        /**
         * Calculates the planned cost up to date @param dt
         */
        double cost(const KPTDateTime &dt);

        int work();
        int work(const KPTDateTime &dt);

    private:
        KPTDateTime m_startTime;
        KPTDuration m_duration;
        KPTTask *m_task;
        KPTResource *m_resource;
        KPTDuration m_repeatInterval;
        int m_repeatCount;
        QPtrList<KPTDuration> m_extraRepeats;
        QPtrList<KPTDuration> m_skipRepeats;

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
 
        void makeAppointment(KPTDateTime &start, KPTDuration &duration, KPTTask *task) 
            { if (m_resource) m_resource->makeAppointment(start, duration, task); }
        
    private:
        KPTResource *m_resource;
        int m_units;
        KPTResourceGroupRequest *m_parent;

#ifndef NDEBUG
public:
        void printDebug(QString ident);
#endif
};

class KPTResourceGroupRequest {
    public:
        KPTResourceGroupRequest(KPTResourceGroup *group=0, int units=0);
        ~KPTResourceGroupRequest();

        KPTResourceGroup *group() { return m_group; }
        QPtrList<KPTResourceRequest> &resourceRequests() { return m_resourceRequests; }
        void addResourceRequest(KPTResourceRequest *request) {
            request->setParent(this);
            m_resourceRequests.append(request);
            request->registerRequest();
        }
        void removeResourceRequest(KPTResourceRequest *request) { m_resourceRequests.removeRef(request); }
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
    
        /**
         * Returns the duration needed to do the effort @param effort
         * starting at @param start.
         */
        KPTDuration duration(const KPTDateTime &start, const KPTDuration &effort);
        
        /**
         * Makes appointments for task @param task to the 
         * requested resources for the duration found in @ref duration().
         */
        void makeAppointments(KPTTask *task);
            
    private:
        KPTResourceGroup *m_group;
        int m_units;

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
    KPTResourceRequestCollection();
    ~KPTResourceRequestCollection();

    const QPtrList<KPTResourceGroupRequest> &requests() const { return m_requests; }
    void addRequest(KPTResourceGroupRequest *request) { m_requests.append(request); }
    void removeRequest(KPTResourceGroupRequest *request) { m_requests.removeRef(request); }
    KPTResourceGroupRequest *find(KPTResourceGroup *resource) const;
    KPTResourceRequest *find(KPTResource *resource) const;

    bool load(QDomElement &element, KPTProject *project);
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
    * starting at @param start.
    */
    KPTDuration duration(const KPTDateTime &start, const KPTDuration &effort);
        
    /**
    * Makes appointments for the task @param task to the requested resources.
    * Assumes that @ref duration() has been run.
    */
    void makeAppointments(KPTTask *task);
    
protected:
    struct Interval {
        KPTDateTime start;
        KPTDateTime end;
        KPTDuration effort;
    };
    

private:
    QPtrList<KPTResourceGroupRequest> m_requests;

#ifndef NDEBUG
public:
        void printDebug(QString ident);
#endif
};

#endif
