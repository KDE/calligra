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

#include <qdom.h>
#include <qstring.h>
#include "kptduration.h"
#include <qptrlist.h>
#include "defs.h"

class KPTRisk;
class KPTEffort;
class KPTAppointment;
class KPTTask;
class KPTNode;
class KPTResource;
class KPTProject;

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

          int id() { return m_id; }
	      void setName(QString n) {m_name=n;}
	      const QString &name() const {return m_name;}
	
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
          QPtrList<KPTResource> availableResources(const KPTDuration start, const KPTDuration duration, int num);
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

          QPtrList<KPTResource> &resources() { return m_resources; }

          bool load(QDomElement &element);
          void save(QDomElement &element);

          void clearAppointments();
          void makeAppointments();
          void saveAppointments(QDomElement &element) const;


          void addNode(const KPTNode *node) { m_nodes.append(node); }
          void clearNodes() { m_nodes.clear(); }
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
        ~KPTResource();

        int id() { return m_id; }

	   void setName(QString n) {m_name=n;}
	   const QString &name() const {return m_name;}

        void setAvailableFrom(KPTDuration af) {m_availableFrom=af;}
        const KPTDuration &availableFrom() const {return m_availableFrom;}
        void setAvailableUntil(KPTDuration au) {m_availableUntil=au;}
        const KPTDuration &availableUntil() const {return m_availableUntil;}

        void addWorkingHour(KPTDuration from, KPTDuration until);

        KPTDuration *getFirstAvailableTime(KPTDuration after = KPTDuration());
        KPTDuration *getBestAvailableTime(KPTDuration duration);
        KPTDuration *getBestAvailableTime(const KPTDuration after, const KPTDuration duration);

        bool load(QDomElement &element);
        void save(QDomElement &element);

        QPtrList<KPTAppointment> &appointments() { return m_appointments; }

        bool isAvailable(KPTTask *task);
        void addAppointment(KPTAppointment *a);
        void clearAppointments();
        void makeAppointment(KPTTask *task);
        void saveAppointments(QDomElement &element) const;

        void setOverbooked(bool on) { m_overbooked = on; }
        bool isOverbooked() { return m_overbooked; }

#ifndef NDEBUG
        void printDebug(QString ident);
#endif
    private:
        KPTProject *m_project;
        QPtrList<KPTAppointment> m_appointments;
        int m_id; // unique id
        QString m_name;
        KPTDuration m_availableFrom;
        KPTDuration m_availableUntil;
        QPtrList<KPTDuration> m_workingHours;

        bool m_overbooked;
};

/**
  * A resource (@ref KPTResource) can be scheduled to be used at any time, this is represented
  * internally with KPTAppointments
  */
class KPTAppointment {
    public:
        KPTAppointment() {}
        KPTAppointment(KPTDuration startTime, KPTDuration duration, KPTResource *resource, KPTTask *taskNode=0);
        ~KPTAppointment();

        // get/set member values.
        const KPTDuration &startTime() const { return m_startTime; }
        void setStartTime(KPTDuration st) { m_startTime=st; }

        const KPTDuration &duration() const { return m_duration; }
        void setDuration(KPTDuration d) { m_duration=d; }

        KPTTask *task() { return m_task; }
        void setTask(KPTTask *t) { m_task = t; }

        const KPTDuration &repeatInterval() const {return m_repeatInterval;}
        void setRepeatInterval(KPTDuration ri) {m_repeatInterval=ri;}

        int repeatCount() { return m_repeatCount; }
        void setRepeatCount(int rc) { m_repeatCount=rc; }

        void deleteAppointmentFromRepeatList(KPTDuration time);
        void addAppointmentToRepeatList(KPTDuration time);

        bool isBusy(const KPTDuration &start, const KPTDuration &end);

        bool load(QDomElement &element, KPTProject &project);
        void save(QDomElement &element);

    private:
        KPTDuration m_startTime;
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
        KPTResourceRequest(KPTResourceGroup *group=0, int numResources=0)
        : m_group(group),
          m_numResources(numResources) {}

        KPTResourceGroup *group() { return m_group; }
        int numResources() { return m_numResources; }

        virtual bool load(QDomElement &element, KPTProject *project);
        virtual void save(QDomElement &element);

    private:
        KPTResourceGroup *m_group;
        int m_numResources;
};

#endif
