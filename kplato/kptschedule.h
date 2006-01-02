/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

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

#include "kpteffortcostmap.h"
#include "kptresource.h"

#include <qintdict.h>
#include <qptrlist.h>
#include <qstring.h>

class QDomElement;

namespace KPlato
{

class Appointment;
class DateTime;
class Duration;
class Node;
class Task;

class Schedule
{
public:
    //NOTE: Must match Effort::Use atm.
    enum Type { Expected=0, //Effort::Use_Expected
                Optimistic=1, //Effort::Use_Optimistic
                Pessimistic=2 //Effort::Use_Pessimistic
    };
    
    Schedule();
    Schedule(QString name, Type type, long id);
    ~Schedule();
    
    QString name() const { return m_name; }
    void setName(QString name) { m_name = name; }
    Type type() const { return m_type; }
    void setType(Type type) { m_type = type; }
    void setType(QString type);
    QString typeToString(bool translate=false) const;
    long id() const { return m_id; }
    void setId(long id) { m_id = id; }
    bool isDeleted() const { return m_deleted; }
    void setDeleted(bool on) { m_deleted = on; }
    
    virtual Resource *resource() const { return 0; }
    virtual Node *node() const { return 0; }
    
    virtual bool loadXML(const QDomElement &element);
    virtual void saveXML(QDomElement &element) const;
    void saveCommonXML(QDomElement &element) const;
    void saveAppointments(QDomElement &element) const;
    
    /// Return the list of appointments
    QPtrList<Appointment> &appointments() { return m_appointments; }
    /// Adds appointment to this schedule only
    virtual bool add(Appointment *appointment);
    /// Adds appointment to both this resource schedule and node schedule
    virtual void addAppointment(Schedule *other, DateTime &start, DateTime &end, double load=100) {}
    /// removes appointment and deletes it (independent of setAutoDelete)
    void removeAppointment(Appointment *appointment);
    /// removes appointment without deleting it (independent of setAutoDelete)
    void takeAppointment(Appointment *appointment);
    Appointment *findAppointment(Schedule *resource, Schedule *node);
    
    Appointment appointmentIntervals() const;
    
    virtual bool isOverbooked() const { return false; }
    virtual bool isOverbooked(const DateTime &start, const DateTime &end) const { return false; }

    virtual EffortCostMap plannedEffortCostPrDay(const QDate &start, const QDate &end) const;
        
    /// Returns the total planned effort for this task (or subtasks) 
    virtual Duration plannedEffort() const;
    /// Returns the total planned effort for this task (or subtasks) on date
    virtual Duration plannedEffort(const QDate &date) const;
    /// Returns the planned effort up to and including date
    virtual Duration plannedEffortTo(const QDate &date) const;
    
    /// Returns the total actual effort for this task (or subtasks) 
    virtual Duration actualEffort() const;
    /// Returns the total actual effort for this task (or subtasks) on date
    virtual Duration actualEffort(const QDate &date) const;
    /// Returns the total actual effort for this task (or subtasks) up to and including date
    virtual Duration actualEffortTo(const QDate &date) const;
    
    /**
     * Planned cost is the sum total of all resources and other costs
     * planned for this node.
     */
    virtual double plannedCost() const; 
    
    /// Planned cost on date
    virtual double plannedCost(const QDate &date) const;
    /**
     * Planned cost from start of activity up to and including date
     * is the sum of all resource costs and other costs planned for this node.
     */
    virtual double plannedCostTo(const QDate &date) const;
    /**
     * Actual cost is the sum total of the reported costs actually used
     * for this node.
     */
    virtual double actualCost() const;
    /// Actual cost on date
    virtual double actualCost(const QDate &date) const;
    /// Actual cost up to and including date
    virtual double actualCostTo(const QDate &date) const;
    
    /// Effort based performance index
    double effortPerformanceIndex(const QDate &/*date*/, bool */*error=0*/) { return 0.0; }
    /// Cost performance index
    double costPerformanceIndex(const QDate &/*date*/, bool */*error=0*/) { return 0.0; }

    virtual double normalRatePrHour() const { return 0.0; }
protected:
    QString m_name;
    Type m_type;
    long m_id;
    bool m_deleted;

    QPtrList<Appointment> m_appointments;

#ifndef NDEBUG
public:
    void printDebug(QString ident);
#endif
};

class NodeSchedule : public Schedule
{
public:
    NodeSchedule();
    NodeSchedule(Node *node, QString name, Schedule::Type type, long id);
    ~NodeSchedule();
    
    static NodeSchedule *find(const QIntDict<NodeSchedule> &sch, Schedule::Type type);
    static NodeSchedule *find(const QIntDict<NodeSchedule> &sch, QString name, Schedule::Type type);
    
    virtual bool loadXML(const QDomElement &element);
    virtual void saveXML(QDomElement &element) const;
    virtual bool loadProjectXML(const QDomElement &element, Project &project);
    virtual void saveProjectXML(QDomElement &element) const;

// tasks------------>
    void setEarliestStart(DateTime &dt) { earliestStart = dt; }
    void setLatestFinish(DateTime &dt) { latestFinish = dt; }

    virtual void addAppointment(Schedule *resource, DateTime &start, DateTime &end, double load=100);
    
    void initiateCalculation();
    void calcResourceOverbooked();

    virtual Node *node() const { return m_node; }
    virtual void setNode(Node *n) { m_node = n; }
    
private:
    void init();
    
private:
    friend class Node;
    friend class Task;
    friend class Project;
    friend class Resource;
    Node *m_node;
    /** 
      * earliestStart is calculated by PERT/CPM.
      * A task may be scheduled to start later because of constraints
      * or resource availability etc.
      */
    DateTime earliestStart;
    /** 
      * latestFinish is calculated by PERT/CPM.
      * A task may be scheduled to finish earlier because of constraints
      * or resource availability etc.
      */
    DateTime latestFinish;
    /**  startTime is the scheduled start time.
      *  It depends on constraints (i.e. ASAP/ALAP) and resource availability.
      *  It will always be later or equal to @ref earliestStart
      */
    DateTime startTime;
    /**  
      *  m_endTime is the scheduled finish time.
      *  It depends on constraints (i.e. ASAP/ALAP) and resource availability.
      *  It will always be earlier or equal to @ref latestFinish
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

#ifndef NDEBUG
public:
    void printDebug(QString ident);
#endif
};

class ResourceSchedule : public Schedule
{
public:
    ResourceSchedule();
    ResourceSchedule(Resource *Resource, QString name, Schedule::Type type, long id);
    ~ResourceSchedule();
    
    virtual void addAppointment(Schedule *node, DateTime &start, DateTime &end, double load=100);
    
    virtual bool isOverbooked() const;
    virtual bool isOverbooked(const DateTime &start, const DateTime &end) const;
    Appointment appointmentIntervals() const;
    
    void initiateCalculation();
    void calcResourceOverbooked();

    virtual Resource *resource() const { return m_resource; }
    virtual double normalRatePrHour() const;
private:
    Resource *m_resource;

#ifndef NDEBUG
public:
    void printDebug(QString ident);
#endif
};

} //namespace KPlato

#endif
