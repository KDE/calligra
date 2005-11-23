/* This file is part of the KDE project
   Copyright (C) 2001 Thomas zander <zander@kde.org>
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

#include "kpttask.h"
#include "kptproject.h"
#include "kpttaskdialog.h"
#include "kptduration.h"
#include "kptrelation.h"
#include "kptdatetime.h"
#include "kptcalendar.h"
#include "kpteffortcostmap.h"

#include <qdom.h>
#include <qbrush.h>
#include <kdebug.h>
#include <koRect.h> //DEBUGRECT

namespace KPlato
{

Task::Task(Node *parent) : Node(parent), m_resource() {
    m_resource.setAutoDelete(true);
    Duration d(1, 0, 0);
    m_effort = new Effort(d);
    m_effort->setOptimisticRatio(-10);
    m_effort->setPessimisticRatio(20);
    m_requests = 0;

    if (m_parent)
        m_leader = m_parent->leader();
            
    m_parentProxyRelations.setAutoDelete(true);
    m_childProxyRelations.setAutoDelete(true);
}

Task::Task(Task &task, Node *parent) 
    : Node(task, parent), 
      m_resource() {
    m_resource.setAutoDelete(true);
    m_parentProxyRelations.setAutoDelete(true);
    m_childProxyRelations.setAutoDelete(true);
    m_requests = 0;
    
    m_effort = task.effort() ? new Effort(*(task.effort())) 
                             : new Effort(); // Avoid crash, (shouldn't be zero)
}


Task::~Task() {
    delete m_effort;
}

int Task::type() const {
	if ( numChildren() > 0) {
	  return Node::Type_Summarytask;
	}
	else if ( 0 == effort()->expected().seconds() ) {
		return Node::Type_Milestone;
	}
	else {
		return Node::Type_Task;
	}
}



Duration *Task::getExpectedDuration() {
    //kdDebug()<<k_funcinfo<<endl;
    // Duration should already be calculated
    return new Duration(m_duration);
}

Duration *Task::getRandomDuration() {
    return 0L;
}

ResourceGroupRequest *Task::resourceGroupRequest(ResourceGroup *group) const {
    if (m_requests)
        return m_requests->find(group);
    return 0;
}

void Task::clearResourceRequests() {
    if (m_requests)
        m_requests->clear();
}

void Task::addRequest(ResourceGroup *group, int numResources) {
    addRequest(new ResourceGroupRequest(group, numResources));
}

void Task::addRequest(ResourceGroupRequest *request) {
    if (!m_requests)
        m_requests = new ResourceRequestCollection(*this);
    m_requests->addRequest(request);
}

void Task::takeRequest(ResourceGroupRequest *request) {
    if (m_requests) {
        m_requests->takeRequest(request);
        if (m_requests->isEmpty()) {
            delete m_requests;
            m_requests = 0;
        }
    }
}

int Task::units() const {
    if (!m_requests)
        return 0;
    return m_requests->units();
}

int Task::workUnits() const {
    if (!m_requests)
        return 0;
    return m_requests->workUnits();
}

void Task::makeAppointments() {
    if (type() == Node::Type_Task) {
        if (m_requests) 
            m_requests->makeAppointments(this);
    } else if (type() == Node::Type_Summarytask) {
        QPtrListIterator<Node> nit(m_nodes);
        for ( ; nit.current(); ++nit ) {
            nit.current()->makeAppointments();
        }
    } else if (type() == Node::Type_Milestone) {
        //kdDebug()<<k_funcinfo<<"Milestone not implemented"<<endl;
        // Well, shouldn't have resources anyway...
    }
}

void Task::calcResourceOverbooked() {
    m_resourceOverbooked = false;
    if (type() == Node::Type_Task) {
        QPtrListIterator<Appointment> it = m_appointments;
        for (; it.current(); ++it) {
            if (it.current()->resource()->isOverbooked(m_startTime, m_endTime)) {
                m_resourceOverbooked = true;
            }
        }
    } else if (type() == Node::Type_Summarytask) {
        QPtrListIterator<Node> nit(m_nodes);
        for ( ; nit.current(); ++nit ) {
            nit.current()->calcResourceOverbooked();
        }
    }
}

// A new constraint means start/end times and duration must be recalculated
void Task::setConstraint(Node::ConstraintType type) {
    if (m_constraint == type)
        return;
    m_constraint = type;
    // atm, the user must recalculate the project
//    calculateStartEndTime();
}


bool Task::load(QDomElement &element, Project &project) {
    // Load attributes (TODO: Handle different types of tasks, milestone, summary...)
    bool ok = false;
    m_id = element.attribute("id");
    
    m_name = element.attribute("name");
    m_leader = element.attribute("leader");
    m_description = element.attribute("description");
    //kdDebug()<<k_funcinfo<<m_name<<": id="<<m_id<<endl;

    // Allow for both numeric and text
    QString constraint = element.attribute("scheduling","0");
    m_constraint = (Node::ConstraintType)constraint.toInt(&ok);
    if (!ok)
        Node::setConstraint(constraint); // hmmm, why do I need Node::?

    m_constraintStartTime = DateTime::fromString(element.attribute("constraint-starttime"));
    m_constraintEndTime = DateTime::fromString(element.attribute("constraint-endtime"));
    
    m_startupCost = element.attribute("startup-cost", "0.0").toDouble();
    m_shutdownCost = element.attribute("shutdown-cost", "0.0").toDouble();
    
    m_wbs = element.attribute("wbs", "");
    
    // Load the project children
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();

            if (e.tagName() == "project") {
                // Load the subproject
                Project *child = new Project(this);
                if (child->load(e)) {
                    addChildNode(child);
                } else {
                    // TODO: Complain about this
                    delete child;
                }
            } else if (e.tagName() == "task") {
                // Load the task
                Task *child = new Task(this);
                if (child->load(e, project)) {
                    addChildNode(child);
                } else {
                    // TODO: Complain about this
                    delete child;
                }
            } else if (e.tagName() == "resource") {
                // TODO: Load the resource (projects don't have resources yet)
            } else if (e.tagName() == "effort") {
                //  Load the effort
                m_effort->load(e);
            } else if (e.tagName() == "resourcegroup-request") {
                // Load the resource request
                ResourceGroupRequest *r = new ResourceGroupRequest();
                if (r->load(e, project)) {
                    addRequest(r);
                } else {
                    kdError()<<k_funcinfo<<"Failed to load resource request"<<endl;
                    delete r;
                }
            } else if (e.tagName() == "progress") {
                m_progress.started = (bool)e.attribute("started", "0").toInt();
                m_progress.finished = (bool)e.attribute("finished", "0").toInt();
                m_progress.startTime = DateTime::fromString(e.attribute("startTime"));
                m_progress.finishTime = DateTime::fromString(e.attribute("finishTime"));
                m_progress.percentFinished = e.attribute("percent-finished", "0").toInt();
                m_progress.remainingEffort = Duration::fromString(e.attribute("remaining-effort"));
                m_progress.totalPerformed = Duration::fromString(e.attribute("performed-effort"));
            } else if (e.tagName() == "schedules") {
                // Prepare for multiple schedules
                QDomNodeList lst = e.childNodes();
                for (unsigned int i=0; i<lst.count(); ++i) {
                    if (lst.item(i).isElement()) {
                        QDomElement sch = lst.item(i).toElement();
                        if (sch.tagName() == "schedule") {
                            earliestStart = DateTime::fromString(sch.attribute("earlieststart"));
                            latestFinish = DateTime::fromString(sch.attribute("latestfinish"));
                            setStartTime(DateTime::fromString(sch.attribute("start")));
                            setEndTime(DateTime::fromString(sch.attribute("end")));
                            m_workStartTime = DateTime::fromString(sch.attribute("start-work"));
                            m_workEndTime = DateTime::fromString(sch.attribute("end-work"));
                            m_duration = Duration::fromString(sch.attribute("duration"));
                            m_inCriticalPath = sch.attribute("in-critical-path", "0").toInt();
                            
                            m_resourceError = sch.attribute("resource-error", "0").toInt();
                            m_resourceOverbooked = sch.attribute("resource-overbooked", "0").toInt();
                            m_schedulingError = sch.attribute("scheduling-conflict", "0").toInt();
                            m_notScheduled = sch.attribute("not-scheduled", "1").toInt();
                        }
                    }
                }
            }
        }
    }
    //kdDebug()<<k_funcinfo<<m_name<<" loaded"<<endl;
    return true;
}


void Task::save(QDomElement &element)  {
    QDomElement me = element.ownerDocument().createElement("task");
    element.appendChild(me);

    //TODO: Handle different types of tasks, milestone, summary...
    me.setAttribute("id", m_id);
    me.setAttribute("name", m_name);
    me.setAttribute("leader", m_leader);
    me.setAttribute("description", m_description);

    me.setAttribute("scheduling",constraintToString());
    me.setAttribute("constraint-starttime",m_constraintStartTime.toString());
    me.setAttribute("constraint-endtime",m_constraintEndTime.toString());    

    me.setAttribute("startup-cost", m_startupCost);
    me.setAttribute("shutdown-cost", m_shutdownCost);
    
    me.setAttribute("wbs", m_wbs);
    
    m_effort->save(me);

    QDomElement el = me.ownerDocument().createElement("progress");
    me.appendChild(el);
    el.setAttribute("started", m_progress.started);
    el.setAttribute("finished", m_progress.finished);
    el.setAttribute("startTime", m_progress.startTime.toString());
    el.setAttribute("finishTime", m_progress.finishTime.toString());
    el.setAttribute("percent-finished", m_progress.percentFinished);
    el.setAttribute("remaining-effort", m_progress.remainingEffort.toString());
    el.setAttribute("performed-effort", m_progress.totalPerformed.toString());
    
    // Prepare for multiple schedules (expected, optimistic, pessmistic...)
    // This goes into separate class later.
    QDomElement schs = me.ownerDocument().createElement("schedules");
    me.appendChild(schs);
    QDomElement sch = schs.ownerDocument().createElement("schedule");
        schs.appendChild(sch);
        kdDebug()<<k_funcinfo<<m_name<<" save schedule"<<endl;
        sch.setAttribute("name", "Standard");
        sch.setAttribute("type", "Expected");
        
        sch.setAttribute("earlieststart",earliestStart.toString());
        sch.setAttribute("latestfinish",latestFinish.toString());
        sch.setAttribute("start",m_startTime.toString());
        sch.setAttribute("end",m_endTime.toString());
        sch.setAttribute("start-work", m_workStartTime.toString());
        sch.setAttribute("end-work", m_workEndTime.toString());
        sch.setAttribute("duration",m_duration.toString());
    
        sch.setAttribute("in-critical-path",m_inCriticalPath);
        
        sch.setAttribute("resource-error",m_resourceError);
        sch.setAttribute("resource-overbooked",m_resourceOverbooked);
        sch.setAttribute("scheduling-conflict",m_schedulingError);
        sch.setAttribute("not-scheduled",m_notScheduled);
    
    
    if (m_requests) {
        m_requests->save(me);
    }

    for (int i=0; i<numChildren(); i++)
    	// First add the child
	    getChildNode(i)->save(me);
}

EffortCostMap Task::plannedEffortCostPrDay(const QDate &start, const QDate &end) const {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    EffortCostMap ec;
    QPtrListIterator<Appointment> it(m_appointments);
    for (; it.current(); ++it) {
        //kdDebug()<<k_funcinfo<<m_name<<endl;
        ec += it.current()->plannedPrDay(start, end);
    }
    return ec;
}

// Returns the total planned effort for this task (or subtasks) 
Duration Task::plannedEffort() {
   //kdDebug()<<k_funcinfo<<endl;
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        QPtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            eff += it.current()->plannedEffort();
        }
    } else {
        QPtrListIterator<Appointment> it(m_appointments);
        for (; it.current(); ++it) {
            eff += it.current()->plannedEffort();
        }
    }
    return eff;
}

// Returns the total planned effort for this task (or subtasks) on date
Duration Task::plannedEffort(const QDate &date) {
   //kdDebug()<<k_funcinfo<<endl;
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        QPtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            eff += it.current()->plannedEffort(date);
        }
    } else {
        QPtrListIterator<Appointment> it(m_appointments);
        for (; it.current(); ++it) {
            eff += it.current()->plannedEffort(date);
        }
    }
    return eff;
}

// Returns the total planned effort for this task (or subtasks) upto and including date
Duration Task::plannedEffortTo(const QDate &date) {
    //kdDebug()<<k_funcinfo<<endl;
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        QPtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            eff += it.current()->plannedEffortTo(date);
        }
    } else {
        QPtrListIterator<Appointment> it(m_appointments);
        for (; it.current(); ++it) {
            eff += it.current()->plannedEffortTo(date);
        }
    }
    return eff;
}

// Returns the total planned effort for this task (or subtasks) 
Duration Task::actualEffort() {
   //kdDebug()<<k_funcinfo<<endl;
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        QPtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            eff += it.current()->actualEffort();
        }
    } else {
        QPtrListIterator<Appointment> it(m_appointments);
        for (; it.current(); ++it) {
            eff += it.current()->actualEffort();
        }
    }
    return eff;
}

// Returns the total planned effort for this task (or subtasks) on date
Duration Task::actualEffort(const QDate &date) {
   //kdDebug()<<k_funcinfo<<endl;
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        QPtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            eff += it.current()->actualEffort(date);
        }
    } else {
        QPtrListIterator<Appointment> it(m_appointments);
        for (; it.current(); ++it) {
            eff += it.current()->actualEffort(date);
        }
    }
    return eff;
}

// Returns the total planned effort for this task (or subtasks) on date
Duration Task::actualEffortTo(const QDate &date) {
   //kdDebug()<<k_funcinfo<<endl;
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        QPtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            eff += it.current()->actualEffortTo(date);
        }
    } else {
        QPtrListIterator<Appointment> it(m_appointments);
        for (; it.current(); ++it) {
            eff += it.current()->actualEffortTo(date);
        }
    }
    return eff;
}

double Task::plannedCost() {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        QPtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            c += it.current()->plannedCost();
        }
    } else {
        QPtrListIterator<Appointment> it(m_appointments);
        for (; it.current(); ++it) {
            c += it.current()->plannedCost();
        }
    }
    return c;
}

double Task::plannedCost(const QDate &date) {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        QPtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            c += it.current()->plannedCost(date);
        }
    } else {
        QPtrListIterator<Appointment> it(m_appointments);
        for (; it.current(); ++it) {
            c += it.current()->plannedCost(date);
        }
    }
    return c;
}

double Task::plannedCostTo(const QDate &date) {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        QPtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            c += it.current()->plannedCostTo(date);
        }
    } else {
        QPtrListIterator<Appointment> it(m_appointments);
        for (; it.current(); ++it) {
            c += it.current()->plannedCostTo(date);
        }
    }
    return c;
}

double Task::actualCost() {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        QPtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            c += it.current()->actualCost();
        }
    } else {
        QPtrListIterator<Appointment> it(m_appointments);
        for (; it.current(); ++it) {
            c += it.current()->actualCost();
        }
    }
    return c;
}

double Task::actualCost(const QDate &date) {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        QPtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            c += it.current()->actualCost(date);
        }
    } else {
        QPtrListIterator<Appointment> it(m_appointments);
        for (; it.current(); ++it) {
            c += it.current()->actualCost(date);
        }
    }
    return c;
}

double Task::actualCostTo(const QDate &date) {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        QPtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            c += it.current()->actualCostTo(date);
        }
    } else {
        QPtrListIterator<Appointment> it(m_appointments);
        for (; it.current(); ++it) {
            c += it.current()->actualCostTo(date);
        }
    }
    return c;
}

//FIXME Handle summarytasks
double Task::effortPerformanceIndex(const QDate &date, bool *error) {
    double res = 0.0;
    Duration ae = actualEffortTo(date);
    
    bool e = (ae == Duration::zeroDuration || m_progress.percentFinished == 0);
    if (error) {
        *error = e;
    }
    if (!e) {
        res = (plannedEffortTo(date).toDouble() * ((double)m_progress.percentFinished/100.0) / ae.toDouble());
    }
    return res;
}

//FIXME Handle summarytasks
double Task::costPerformanceIndex(const QDate &date, bool *error) {
    double res = 0.0;
    Duration ac = actualCostTo(date);
    
    bool e = (ac == Duration::zeroDuration || m_progress.percentFinished == 0);
    if (error) {
        *error = e;
    }
    if (!e) {
        res = (plannedCostTo(date) * m_progress.percentFinished)/(100 * actualCostTo(date));
    }
    return res;
}


void Task::initiateCalculationLists(QPtrList<Node> &startnodes, QPtrList<Node> &endnodes, QPtrList<Node> &summarytasks/*, QPtrList<Node> &milestones*/) {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (type() == Node::Type_Summarytask) {
        summarytasks.append(this);
        // propagate my relations to my children and dependent nodes
        
        QPtrListIterator<Node> nodes = m_nodes;
        for (; nodes.current(); ++nodes) {
            if (!dependParentNodes().isEmpty()) 
                nodes.current()->addParentProxyRelations(dependParentNodes());
            if (!dependChildNodes().isEmpty()) 
                nodes.current()->addChildProxyRelations(dependChildNodes());
            nodes.current()->initiateCalculationLists(startnodes, endnodes, summarytasks);
        }        
    } else {
        if (isEndNode()) {
            endnodes.append(this);
            //kdDebug()<<k_funcinfo<<"endnodes append: "<<m_name<<endl;
        }
        if (isStartNode()) {
            startnodes.append(this);
            //kdDebug()<<k_funcinfo<<"startnodes append: "<<m_name<<endl;
        }
    }
}

DateTime Task::calculatePredeccessors(const QPtrList<Relation> &list, int use) {
    DateTime time;
    QPtrListIterator<Relation> it = list;
    for (; it.current(); ++it) {
        if (it.current()->parent()->type() == Type_Summarytask) {
            //kdDebug()<<k_funcinfo<<"Skip summarytask: "<<it.current()->parent()->name()<<endl;
            continue; // skip summarytasks
        }
        DateTime t = it.current()->parent()->calculateForward(use); // early finish
        switch (it.current()->type()) {
            case Relation::StartStart:
                // I can't start earlier than my predesseccor
                t = it.current()->parent()->earliestStartForward() + it.current()->lag();
                break;
            case Relation::FinishFinish:
                // I can't finish earlier than my predeccessor, so
                // I can't start earlier than it's (earlyfinish+lag)- my duration
                t += it.current()->lag();
                t -= duration(t, use, true);
                break;
            default:
                t += it.current()->lag();
                break;
        }
        if (!time.isValid() || t > time)
            time = t;
    }
    //kdDebug()<<time.toString()<<"                  "<<m_name<<" calculatePredeccessors() ("<<list.count()<<")"<<endl;
    return time;
}
DateTime Task::calculateForward(int use) {
    //kdDebug()<<k_funcinfo<<m_name<<<<endl;
    if (m_visitedForward) {
    //kdDebug()<<earliestStart.toString()<<" + "<<m_durationBackward.toString()<<" "<<m_name<<" calculateForward() (visited)"<<endl;
        return m_earliestStartForward + m_durationForward;
    }
    // First, calculate all predecessors
    if (!dependParentNodes().isEmpty()) {
        DateTime time = calculatePredeccessors(dependParentNodes(), use);
        if (time.isValid() && time > earliestStart) {
            earliestStart = time;
        }
    }
    if (!m_parentProxyRelations.isEmpty()) {
        DateTime time = calculatePredeccessors(m_parentProxyRelations, use);
        if (time.isValid() && time > earliestStart) {
            earliestStart = time;
        }
    }    
    m_earliestStartForward = earliestStart;
    if (type() == Node::Type_Task) {
        switch (constraint()) {
            case Node::ASAP:
            case Node::ALAP:
                m_durationForward = duration(m_earliestStartForward, use, false);
                break;
            case Node::MustFinishOn:
                m_durationForward = duration(m_constraintEndTime, use, true);
                m_earliestStartForward = m_constraintEndTime - m_durationForward;
                break;
            case Node::FinishNotLater:
                m_durationForward = duration(m_earliestStartForward, use, false);
                if (m_earliestStartForward + m_durationForward > m_constraintEndTime) {
                    m_durationForward = duration(m_constraintEndTime, use, true);
                    m_earliestStartForward = m_constraintEndTime - m_durationForward;
                }
                break;
            case Node::MustStartOn:
                m_earliestStartForward = m_constraintStartTime;
                m_durationForward = duration(m_earliestStartForward, use, false);
                break;
            case Node::StartNotEarlier:
                if (m_earliestStartForward < m_constraintStartTime) {
                    m_earliestStartForward = m_constraintStartTime;
                }
                m_durationForward = duration(m_earliestStartForward, use, false);
                break;
            case Node::FixedInterval: {
                DateTime st = m_constraintStartTime;
                DateTime end = m_constraintEndTime;
                m_earliestStartForward = st;
                m_durationForward = end - st;
                break;
            }
        }
    } else if (type() == Node::Type_Milestone) {
        m_durationForward = Duration::zeroDuration;
        switch (constraint()) {
            case Node::MustFinishOn:
                m_earliestStartForward = m_constraintEndTime;
                break;
            case Node::FinishNotLater:
                if (m_earliestStartForward > m_constraintEndTime) {
                    m_earliestStartForward = m_constraintEndTime;
                }
                break;
            case Node::MustStartOn:
                m_earliestStartForward = m_constraintStartTime;
                break;
            case Node::StartNotEarlier:
                if (m_earliestStartForward < m_constraintStartTime) {
                    m_earliestStartForward = m_constraintStartTime;
                }
                break;
            case Node::FixedInterval:
                m_earliestStartForward = m_constraintStartTime;
                break;
            default:
                break;
        }
        //kdDebug()<<k_funcinfo<<m_name<<" "<<earliestStart.toString()<<endl
    } else if (type() == Node::Type_Summarytask) {
        kdWarning()<<k_funcinfo<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    } else { // ???
        m_durationForward = Duration::zeroDuration;
    }
    
    //kdDebug()<<"Earlyfinish: "<<m_earliestStartForward.toString()<<"+"<<m_durationForward.toString()<<"="<<(m_earliestStartForward+m_durationForward).toString()<<" "<<m_name<<" calculateForward()"<<endl;
    m_visitedForward = true;
    return m_earliestStartForward + m_durationForward;
}

DateTime Task::calculateSuccessors(const QPtrList<Relation> &list, int use) {
    DateTime time;
    QPtrListIterator<Relation> it = list;
    for (; it.current(); ++it) {
        if (it.current()->child()->type() == Type_Summarytask) {
            //kdDebug()<<k_funcinfo<<"Skip summarytask: "<<it.current()->parent()->name()<<endl;
            continue; // skip summarytasks
        }
        DateTime t = it.current()->child()->calculateBackward(use);
        switch (it.current()->type()) {
            case Relation::StartStart:
                // I must start before my successor, so
                // I can't finish later than it's (starttime-lag) + my duration
                t -= it.current()->lag();
                t += duration(t, use, false);
                break;
            case Relation::FinishFinish:
                // My successor cannot finish before me, so
                // I can't finish later than it's latest finish - lag
                t = it.current()->child()->latestFinishBackward() -  it.current()->lag();
                break;
            default:
                t -= it.current()->lag();
                break;
        }
        if (!time.isValid() || t < time)
            time = t;
    }
    //kdDebug()<<time.toString()<<"                  "<<m_name<<" calculateSuccessors() ("<<list.count()<<")"<<endl;
    return time;
}
DateTime Task::calculateBackward(int use) {
    //kdDebug()<<k_funcinfo<<m_name<<<<endl;
    if (m_visitedBackward) {
    //kdDebug()<<latestFinish.toString()<<" - "<<m_durationBackward.toString()<<" "<<m_name<<" calculateBackward() (visited)"<<endl;
        return m_latestFinishBackward - m_durationBackward;
    }
    // First, calculate all successors
    if (!dependChildNodes().isEmpty()) {
        DateTime time = calculateSuccessors(dependChildNodes(), use);
        if (time.isValid() && time < latestFinish) {
            latestFinish = time;
        }
    }
    if (!m_childProxyRelations.isEmpty()) {
        DateTime time = calculateSuccessors(m_childProxyRelations, use);
        if (time.isValid() && time < latestFinish) {
            latestFinish = time;
        }
    }
    m_latestFinishBackward = latestFinish;
    if (type() == Node::Type_Task) {
        switch (constraint()) {
            case Node::ASAP:
            case Node::ALAP:
                m_durationBackward = duration(m_latestFinishBackward, use, true);
                break;
            case Node::MustStartOn:
                m_durationBackward = duration(m_constraintStartTime, use, false);
                m_latestFinishBackward = m_constraintStartTime + m_durationBackward;
                break;
            case Node::StartNotEarlier:
                m_durationBackward = duration(m_latestFinishBackward, use, true);
                if (m_latestFinishBackward - m_durationBackward < m_constraintStartTime) {
                    m_durationBackward = duration(m_constraintStartTime, use, false);
                    m_latestFinishBackward = m_constraintStartTime + m_durationBackward;
                }
                break;
            case Node::MustFinishOn:
                m_latestFinishBackward = m_constraintEndTime;
                m_durationBackward = duration(m_latestFinishBackward, use, true);
                break;
            case Node::FinishNotLater:
                if (m_latestFinishBackward > m_constraintEndTime)
                    m_latestFinishBackward = m_constraintEndTime;
                m_durationBackward = duration(m_latestFinishBackward, use, true);
                break;
            case Node::FixedInterval: {
                DateTime st = m_constraintStartTime;
                DateTime end = m_constraintEndTime;
                m_latestFinishBackward = end;
                m_durationBackward = end - st;
                break;
            }
        }
    } else if (type() == Node::Type_Milestone) {
        m_durationBackward = Duration::zeroDuration;
        switch (constraint()) {
            case Node::MustFinishOn:
                m_latestFinishBackward = m_constraintEndTime;
                break;
            case Node::FinishNotLater:
                if (m_latestFinishBackward > m_constraintEndTime) {
                    m_latestFinishBackward = m_constraintEndTime;
                }
                break;
            case Node::MustStartOn:
                m_latestFinishBackward = m_constraintStartTime;
                break;
            case Node::StartNotEarlier:
                if (m_latestFinishBackward < m_constraintStartTime) {
                    m_latestFinishBackward = m_constraintStartTime;
                }
                break;
            case Node::FixedInterval:
                m_latestFinishBackward = m_constraintEndTime;
                break;
            default:
                break;
        }
        //kdDebug()<<k_funcinfo<<m_name<<" "<<latestFinish.toString()<<" : "<<m_endTime.toString()<<endl;
    } else if (type() == Node::Type_Summarytask) {
        kdWarning()<<k_funcinfo<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    } else { // ???
        m_durationBackward = Duration::zeroDuration;
    }
    //kdDebug()<<"Latestart: "<<m_latestFinishBackward.toString()<<"-"<<m_durationBackward.toString()<<"="<<(m_latestFinishBackward-m_durationBackward).toString()<<" "<<m_name<<" calculateBackward()"<<endl;
    m_visitedBackward = true;
    return m_latestFinishBackward - m_durationBackward;
}

DateTime Task::schedulePredeccessors(const QPtrList<Relation> &list, int use) {
    DateTime time;
    QPtrListIterator<Relation> it = list;
    for (; it.current(); ++it) {
        if (it.current()->parent()->type() == Type_Summarytask) {
            //kdDebug()<<k_funcinfo<<"Skip summarytask: "<<it.current()->parent()->name()<<endl;
            continue; // skip summarytasks
        }
        // schedule the predecessors
        DateTime earliest = it.current()->parent()->getEarliestStart();
        DateTime t = it.current()->parent()->scheduleForward(earliest, use);
        switch (it.current()->type()) {
            case Relation::StartStart:
                // I can't start before my predesseccor
                t = it.current()->parent()->startTime() + it.current()->lag();
                break;
            case Relation::FinishFinish:
                // I can't end before my predecessor, so
                // I can't start before it's endtime - my duration
                t -= duration(t + it.current()->lag(), use, true);
                break;
            default:
                time += it.current()->lag();
                break;
        }
        if (!time.isValid() || t > time)
            time = t;
    }
    //kdDebug()<<time.toString()<<" "<<m_name<<" schedulePredeccessors()"<<endl;
    return time;
}

DateTime &Task::scheduleForward(DateTime &earliest, int use) {
    //kdDebug()<<k_funcinfo<<m_name<<" earliest="<<earliest.toString()<<endl;
    if (m_visitedForward)
        return m_endTime;
    m_notScheduled = false;
    setStartTime(earliest > earliestStart ? earliest : earliestStart);
    // First, calculate all my own predecessors
    DateTime time = schedulePredeccessors(dependParentNodes(), use);
    if (time.isValid() && time > m_startTime) {
        setStartTime(time);
        //kdDebug()<<k_funcinfo<<m_name<<" new startime="<<m_startTime<<endl;
    }
    // Then my parents
    time = schedulePredeccessors(m_parentProxyRelations, use);
    if (time.isValid() && time > m_startTime) {
        setStartTime(time);
        //kdDebug()<<k_funcinfo<<m_name<<" new startime="<<m_startTime<<endl;
    }
    //kdDebug()<<k_funcinfo<<m_name<<" m_startTime="<<m_startTime.toString()<<endl;
    if(type() == Node::Type_Task) {
        switch (m_constraint) {
        case Node::ASAP:
            // m_startTime calculated above
            m_workStartTime = workStartAfter(m_startTime);
            setStartTime(m_workStartTime);
            m_duration = duration(m_startTime, use, false);
            setEndTime(m_startTime + m_duration);
            m_workEndTime = workFinishBefore(m_endTime);
            break;
        case Node::ALAP:
            // m_startTime calculated above
            m_workEndTime = workFinishBefore(latestFinish);
            setEndTime(m_workEndTime);
            m_duration = duration(m_endTime, use, true);
            setStartTime(m_endTime - m_duration);
            m_workStartTime = workStartAfter(m_startTime);
            break;
        case Node::StartNotEarlier:
            // m_startTime calculated above
            //kdDebug()<<"StartNotEarlier="<<m_constraintStartTime.toString()<<" "<<m_startTime.toString()<<endl;            
            m_workStartTime = workStartAfter(m_startTime);
            setStartTime(m_workStartTime);
            if (m_startTime < m_constraintStartTime) {
                setStartTime(m_constraintStartTime);
            }
            m_duration = duration(m_startTime, use, false);
            setEndTime(m_startTime + m_duration);
            if (m_endTime > latestFinish) {
                m_schedulingError = true;
            }
            m_workStartTime = workStartAfter(m_startTime);
            m_workEndTime = workFinishBefore(m_endTime);
            break;
        case Node::FinishNotLater:
            // m_startTime calculated above
            //kdDebug()<<"FinishNotLater="<<m_constraintEndTime.toString()<<" "<<m_startTime.toString()<<endl;
            m_workStartTime = workStartAfter(m_startTime);
            setStartTime(m_startTime);
            m_duration = duration(m_startTime, use, false);            
            setEndTime(m_startTime + m_duration);
            if ( m_endTime > m_constraintEndTime) {
                m_schedulingError = true;
                setEndTime(m_constraintEndTime);
                m_duration = duration(m_endTime, use, true);
                setStartTime(m_endTime - m_duration);
            }
            m_workEndTime = workFinishBefore(m_endTime);
            break;
        case Node::MustStartOn:
            // m_startTime calculated above
            //kdDebug()<<"MustStartOn="<<m_constraintStartTime.toString()<<" "<<m_startTime.toString()<<endl;
            if (m_constraintStartTime < m_startTime ||
                m_constraintStartTime > latestFinish - m_durationBackward) {
                m_schedulingError = true;
            }
            setStartTime(m_constraintStartTime);
            m_duration = duration(m_startTime, use, false);            
            setEndTime(m_startTime + m_duration);
            m_workStartTime = workStartAfter(m_startTime);
            m_workEndTime = workFinishBefore(m_endTime);
            break;
        case Node::MustFinishOn:
            // m_startTime calculated above
            //kdDebug()<<"MustFinishOn="<<m_constraintEndTime.toString()<<" "<<m_startTime.toString()<<endl;
            if (m_constraintEndTime > latestFinish ||
                m_constraintEndTime < earliestStart + m_durationForward) {
                m_schedulingError = true;
            }                
            setEndTime(m_constraintEndTime);
            m_duration = duration(m_endTime, use, true);            
            setStartTime(m_endTime - m_duration);
            m_workStartTime = workStartAfter(m_startTime);
            m_workEndTime = workFinishBefore(m_endTime);
            break;
        case Node::FixedInterval: {
            // m_startTime calculated above
            //kdDebug()<<"FixedInterval="<<m_constraintStartTime.toString()<<" "<<m_startTime.toString()<<endl;
            if (m_endTime > latestFinish || m_startTime < earliestStart) {
                m_schedulingError = true;
            }
            setStartTime(m_constraintStartTime);
            setEndTime(m_constraintEndTime);
            m_duration = m_endTime - m_startTime;
            m_workStartTime = workStartAfter(m_startTime);
            m_workEndTime = workFinishBefore(m_endTime);
            break;
        }
        default:
            break;
        }
        if (m_requests) {
            m_requests->reserve(m_startTime, m_duration);
        }
    } else if (type() == Node::Type_Milestone) {
        switch (m_constraint) {
        case Node::ASAP: {
            setEndTime(m_startTime);
            break;
        }
        case Node::ALAP: {
            setStartTime(latestFinish);
            setEndTime(latestFinish);
            break;
        }
        case Node::MustStartOn:
        case Node::FixedInterval:
            //kdDebug()<<"Forw, MustStartOn: "<<m_constraintStartTime.toString()<<" "<<m_startTime.toString()<<endl;
            if (m_constraintStartTime < m_startTime ||
                m_constraintStartTime > latestFinish) {
                m_schedulingError = true;
            }
            setStartTime(m_constraintStartTime);
            setEndTime(m_constraintStartTime);
            break;
        case Node::MustFinishOn:
            if (m_constraintEndTime < m_startTime ||
                m_constraintEndTime > latestFinish) {
                m_schedulingError = true;
            }
            setStartTime(m_constraintEndTime);
            setEndTime(m_constraintEndTime);
            break;
        case Node::StartNotEarlier:
            if (m_startTime < m_constraintStartTime) {
                m_schedulingError = true;
            }
            setEndTime(m_startTime);
            break;
        case Node::FinishNotLater:
            if (m_startTime > m_constraintEndTime) {
                m_schedulingError = true;
            }
            setEndTime(m_startTime);
            break;
        default:
            break;
        }
        m_duration = Duration::zeroDuration;
    } else if (type() == Node::Type_Summarytask) {
        //shouldn't come here
        setEndTime(m_startTime);
        m_duration = m_endTime - m_startTime;
        kdWarning()<<k_funcinfo<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    }
    //kdDebug()<<m_startTime.toString()<<" : "<<m_endTime.toString()<<" "<<m_name<<" scheduleForward()"<<endl;
    m_visitedForward = true;
    return m_endTime;
}

DateTime Task::scheduleSuccessors(const QPtrList<Relation> &list, int use) {
    DateTime time;
    QPtrListIterator<Relation> it = list;
    for (; it.current(); ++it) {
        if (it.current()->child()->type() == Type_Summarytask) {
            //kdDebug()<<k_funcinfo<<"Skip summarytask: "<<it.current()->child()->name()<<endl;
            continue;
        }
        // get the successors starttime
        DateTime latest = it.current()->child()->getLatestFinish();
        DateTime t = it.current()->child()->scheduleBackward(latest, use);
        switch (it.current()->type()) {
            case Relation::StartStart:
                // I can't start before my successor, so
                // I can't finish later than it's starttime + my duration
                t += duration(t - it.current()->lag(), use, false);
                break;
            case Relation::FinishFinish:
                t = it.current()->child()->endTime() - it.current()->lag();
                break;
            default:
                t -= it.current()->lag();
                break;
        }
        if (!time.isValid() || t < time)
            time = t;
   }
   return time;
}
DateTime &Task::scheduleBackward(DateTime &latest, int use) {
    //kdDebug()<<k_funcinfo<<m_name<<": latest="<<latest<<endl;
    if (m_visitedBackward)
        return m_startTime;
    m_notScheduled = false;
    setEndTime(latest < latestFinish ? latest : latestFinish);
    // First, calculate all my own successors
    DateTime time = scheduleSuccessors(dependChildNodes(), use);
    if (time.isValid() && time < m_endTime) {
        setEndTime(time);
    }
    // Then my parents
    time = scheduleSuccessors(m_childProxyRelations, use);
    if (time.isValid() && time < m_endTime) {
        setEndTime(time);
    }

    if (type() == Node::Type_Task) {
        switch (m_constraint) {
        case Node::ASAP:
            // m_endTime calculated above
            m_workEndTime = workFinishBefore(m_endTime);
            setEndTime(m_workEndTime);
            m_duration = duration(m_endTime, use, true);
            setStartTime(m_endTime - m_duration);
            m_workStartTime = workStartAfter(m_startTime);
            break;
        case Node::ALAP:
            // m_endTime calculated above
            m_workEndTime = workFinishBefore(m_endTime);
            setEndTime(m_workEndTime);
            m_duration = duration(m_endTime, use, true);
            setStartTime(m_endTime - m_duration);
            m_workStartTime = workStartAfter(m_startTime);
            break;
        case Node::StartNotEarlier:
            // m_endTime calculated above
            //kdDebug()<<"StartNotEarlier="<<m_constraintStartTime.toString()<<" "<<m_endTime.toString()<<endl;
            m_workEndTime = workFinishBefore(m_endTime);
            setEndTime(m_workEndTime);
            m_duration = duration(m_endTime, use, true);
            setStartTime(m_endTime - m_duration);
            if (m_startTime < m_constraintStartTime) {
                m_schedulingError = true;
                setStartTime(m_constraintStartTime);
                m_duration = duration(m_startTime, use, false);
                setEndTime(m_startTime + m_duration);
                m_workEndTime = workFinishBefore(m_endTime);
            }
            m_workStartTime = workStartAfter(m_startTime);
            break;
        case Node::FinishNotLater:
            // m_endTime calculated above
            //kdDebug()<<"FinishNotLater="<<m_constraintEndTime.toString()<<" "<<m_endTime.toString()<<endl;            
            m_workEndTime = workFinishBefore(m_endTime);
            setEndTime(m_workEndTime);
            if (m_endTime > m_constraintEndTime) {
                m_schedulingError = true;
                setEndTime(m_constraintEndTime);
            }
            m_duration = duration(m_endTime, use, true);
            setStartTime(m_endTime - m_duration);
            m_workStartTime = workStartAfter(m_startTime);
            break;
        case Node::MustStartOn:
            // m_endTime calculated above
            //kdDebug()<<"MustStartOn="<<m_constraintStartTime.toString()<<" "<<m_startTime.toString()<<endl;
            if (m_constraintStartTime < earliestStart ||
                m_constraintStartTime > latestFinish - m_durationBackward) {
                m_schedulingError = true;
            }
            setStartTime(m_constraintStartTime);
            m_duration = duration(m_startTime, use, false);            
            setEndTime(m_startTime + m_duration);
            m_workStartTime = workStartAfter(m_startTime);
            m_workEndTime = workFinishBefore(m_endTime);
            break;
        case Node::MustFinishOn:
            // m_endTime calculated above
            //kdDebug()<<"MustFinishOn="<<m_constraintEndTime.toString()<<" "<<m_startTime.toString()<<endl;
            if (m_constraintEndTime > latestFinish ||
                m_constraintEndTime < earliestStart + m_durationForward) {
                m_schedulingError = true;
            }                
            setEndTime(m_constraintEndTime);
            m_duration = duration(m_endTime, use, true);            
            setStartTime(m_endTime - m_duration);
            m_workStartTime = workStartAfter(m_startTime);
            m_workEndTime = workFinishBefore(m_endTime);
            break;
        case Node::FixedInterval: {
            // m_endTime calculated above
            //kdDebug()<<"FixedInterval="<<m_constraintEndTime.toString()<<" "<<m_endTime.toString()<<endl;
            if (m_constraintEndTime > m_endTime ||
                m_startTime < earliestStart) {
                m_schedulingError = true;
            }
            setStartTime(m_constraintStartTime);
            setEndTime(m_constraintEndTime);            
            m_duration = m_endTime - m_startTime;
            m_workStartTime = workStartAfter(m_startTime);
            m_workEndTime = workFinishBefore(m_endTime);
            break;
        }
        default:
            break;
        }
        if (m_requests) {
            m_requests->reserve(m_startTime, m_duration);
        }
    } else if (type() == Node::Type_Milestone) {
        switch (m_constraint) {
        case Node::ASAP:
            setStartTime(earliestStart);
            setEndTime(earliestStart);
            break;
        case Node::ALAP:
            setStartTime(latestFinish);
            setEndTime(latestFinish);
            break;
        case Node::MustStartOn:
        case Node::FixedInterval:
            if (m_constraintStartTime < earliestStart ||
                m_constraintStartTime > m_endTime) {
                m_schedulingError = true;
            }
            setStartTime(earliestStart);
            setEndTime(earliestStart);
            break;
        case Node::MustFinishOn:
            if (m_constraintEndTime < earliestStart ||
                m_constraintEndTime > m_endTime) {
                m_schedulingError = true;
            }
            setStartTime(earliestStart);
            setEndTime(earliestStart);
            break;
        case Node::StartNotEarlier:
            if (m_constraintStartTime > m_endTime) {
                m_schedulingError = true;
            }
            setStartTime(m_endTime);
            break;
        case Node::FinishNotLater:
            if (m_constraintEndTime < m_endTime) {
                m_schedulingError = true;
            }
            setStartTime(m_endTime);
            break;
        default:
            break;
        }
        m_duration = Duration::zeroDuration;
    } else if (type() == Node::Type_Summarytask) {
        //shouldn't come here
        setStartTime(m_endTime);
        m_duration = m_endTime - m_startTime;
        kdWarning()<<k_funcinfo<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    }
    //kdDebug()<<k_funcinfo<<m_name<<": "<<m_startTime.toString()<<" : "<<m_endTime.toString()<<endl;
    m_visitedBackward = true;
    return m_startTime;
}

void Task::adjustSummarytask() {
    if (type() == Type_Summarytask) {
        DateTime start = latestFinish;
        DateTime end = earliestStart;
        QPtrListIterator<Node> it(m_nodes);
        for (; it.current(); ++it) {
            it.current()->adjustSummarytask();
            if (it.current()->startTime() < start)
                start = it.current()->startTime();
            if (it.current()->endTime() > end)
                end = it.current()->endTime();
        }
        setStartTime(start);
        setEndTime(end);
        m_duration = end - start;
        //kdDebug()<<k_funcinfo<<m_name<<": "<<m_startTime.toString()<<" : "<<m_endTime.toString()<<endl;
    }
}

Duration Task::calcDuration(const DateTime &time, const Duration &effort, bool backward) {
    //kdDebug()<<"calcDuration "<<m_name<<" effort="<<effort.toString(Duration::Format_Day)<<endl;
    if (!m_requests || m_requests->isEmpty()) {
        m_resourceError = true;
        return effort;
    }
    Duration dur = m_requests->duration(time, effort, backward);
    //kdDebug()<<"calcDuration "<<m_name<<": "<<time.toString()<<" to "<<(time+dur).toString()<<" = "<<dur.toString(Duration::Format_Day)<<endl;
    return dur;
}

void Task::clearProxyRelations() {
    m_parentProxyRelations.clear();
    m_childProxyRelations.clear();
}

void Task::addParentProxyRelations(QPtrList<Relation> &list) {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (type() == Type_Summarytask) {
        // propagate to my children
        //kdDebug()<<k_funcinfo<<m_name<<" is summary task"<<endl;
        QPtrListIterator<Node> nodes = m_nodes;
        for (; nodes.current(); ++nodes) {
            nodes.current()->addParentProxyRelations(list);
            nodes.current()->addParentProxyRelations(dependParentNodes());
        }        
    } else {
        // add 'this' as child relation to the relations parent
        //kdDebug()<<k_funcinfo<<m_name<<" is not summary task"<<endl;
        QPtrListIterator<Relation> it = list;
        for (; it.current(); ++it) {
            it.current()->parent()->addChildProxyRelation(this, it.current());
            // add a parent relation to myself
            addParentProxyRelation(it.current()->parent(), it.current());
        }
    }
}

void Task::addChildProxyRelations(QPtrList<Relation> &list) {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (type() == Type_Summarytask) {
        // propagate to my children
        //kdDebug()<<k_funcinfo<<m_name<<" is summary task"<<endl;
        QPtrListIterator<Node> nodes = m_nodes;
        for (; nodes.current(); ++nodes) {
            nodes.current()->addChildProxyRelations(list);
            nodes.current()->addChildProxyRelations(dependChildNodes());
        }        
    } else {
        // add 'this' as parent relation to the relations child
        //kdDebug()<<k_funcinfo<<m_name<<" is not summary task"<<endl;
        QPtrListIterator<Relation> it = list;
        for (; it.current(); ++it) {
            it.current()->child()->addParentProxyRelation(this, it.current());
            // add a child relation to myself
            addChildProxyRelation(it.current()->child(), it.current());
        }
    }
}

void Task::addParentProxyRelation(Node *node, const Relation *rel) {
    if (node->type() != Type_Summarytask) {
        if (type() == Type_Summarytask) {
            //kdDebug()<<"Add parent proxy from my children "<<m_name<<" to "<<node->name()<<endl;
            QPtrListIterator<Node> nodes = m_nodes;
            for (; nodes.current(); ++nodes) {
                nodes.current()->addParentProxyRelation(node, rel);
            }
        } else {
            //kdDebug()<<"Add parent proxy from "<<node->name()<<" to (me) "<<m_name<<endl;
            m_parentProxyRelations.append(new ProxyRelation(node, this, rel->type(), rel->lag()));
        }
    }
}

void Task::addChildProxyRelation(Node *node, const Relation *rel) {
    if (node->type() != Type_Summarytask) {
        if (type() == Type_Summarytask) {
            //kdDebug()<<"Add child proxy from my children "<<m_name<<" to "<<node->name()<<endl;
            QPtrListIterator<Node> nodes = m_nodes;
            for (; nodes.current(); ++nodes) {
                nodes.current()->addChildProxyRelation(node, rel);
            }
        } else {
            //kdDebug()<<"Add child proxy from (me) "<<m_name<<" to "<<node->name()<<endl;
            m_childProxyRelations.append(new ProxyRelation(this, node, rel->type(), rel->lag()));
        }
    }
}

bool Task::isEndNode() const {
    QPtrListIterator<Relation> it = m_dependChildNodes;
    for (; it.current(); ++it) {
        if (it.current()->type() == Relation::FinishStart)
            return false;
    }
    QPtrListIterator<Relation> pit = m_childProxyRelations;
    for (; pit.current(); ++pit) {
        if (pit.current()->type() == Relation::FinishStart)
            return false;
    }
    return true;
}
bool Task::isStartNode() const {
    QPtrListIterator<Relation> it = m_dependParentNodes;
    for (; it.current(); ++it) {
        if (it.current()->type() == Relation::FinishStart ||
            it.current()->type() == Relation::StartStart)
            return false;
    }
    QPtrListIterator<Relation> pit = m_parentProxyRelations;
    for (; pit.current(); ++pit) {
        if (pit.current()->type() == Relation::FinishStart ||
            pit.current()->type() == Relation::StartStart)
            return false;
    }
    return true;
}

const DateTime &Task::workStartTime() const {
    if (m_requests)
        return m_workStartTime;
    return startTime();
}

const DateTime &Task::workEndTime() const {
    if (m_requests)
        return m_workEndTime;
    return endTime();
}

DateTime Task::workStartAfter(const DateTime &dt) {
    if (m_requests) {
        DateTime t = m_requests->availableAfter(dt);
        return t.isValid() ? t : dt;
    }
    Project *p = static_cast<Project*>(projectNode());
    if (p && p->standardWorktime()) {
        return p->standardWorktime()->workStartAfter(dt);
    }
    return dt;
}

DateTime Task::workFinishBefore(const DateTime &dt) {
    if (m_requests) {
        return m_requests->availableBefore(dt);
    }
    Project *p = static_cast<Project*>(projectNode());
    if (p && p->standardWorktime()) {
        return p->standardWorktime()->workFinishBefore(dt);
    }
    return dt;
}

Duration Task::positiveFloat() {
    DateTime t = workFinishBefore(latestFinish);
    if (t <= (type() == Node::Type_Milestone ? m_startTime : m_endTime))
        return Duration::zeroDuration;
    return t - m_endTime;
}

bool Task::isCritical() {
    return positiveFloat() == Duration::zeroDuration;
}

bool Task::calcCriticalPath() {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (m_inCriticalPath) {
        return true; // path allready calculated
    }
    if (!isCritical()) {
        return false;
    }
    if (isStartNode()) {
        m_inCriticalPath = true;
        return true;
    }
    QPtrListIterator<Relation> it(m_parentProxyRelations);
    for (; it.current(); ++it) {
        if (it.current()->parent()->calcCriticalPath()) {
            m_inCriticalPath = true;
        }
    }
    QPtrListIterator<Relation> pit(m_dependParentNodes);
    for (; pit.current(); ++pit) {
        if (pit.current()->parent()->calcCriticalPath()) {
            m_inCriticalPath = true;
        }
    }
    return m_inCriticalPath;
}

#ifndef NDEBUG
void Task::printDebug(bool children, QCString indent) {
    kdDebug()<<indent<<"+ Task node: "<<name()<<" type="<<type()<<endl;
    indent += "!  ";
    kdDebug()<<indent<<"Requested resources (total): "<<units()<<"%"<<endl;
    kdDebug()<<indent<<"Requested resources (work): "<<workUnits()<<"%"<<endl;
    kdDebug()<<indent<<"Resource overbooked="<<m_resourceOverbooked<<endl;
    kdDebug()<<indent<<"resourceError="<<resourceError()<<endl;
    kdDebug()<<indent<<"schedulingError="<<schedulingError()<<endl;
    if (m_requests)
        m_requests->printDebug(indent);
    kdDebug()<<indent<<endl;
    Node::printDebug(children, indent);
    kdDebug()<<indent<<"workStartTime="<<m_workStartTime.toString()<<endl;
    kdDebug()<<indent<<"workEndTime="<<m_workEndTime.toString()<<endl;
    kdDebug()<<indent<<"earliestStartForward="<<m_earliestStartForward.toString()<<endl;
    kdDebug()<<indent<<"latestFinishBackward="<<m_latestFinishBackward.toString()<<endl;
    kdDebug()<<indent<<"durationForward="<<m_durationForward.toString()<<endl;
    kdDebug()<<indent<<"durationBackward="<<m_durationBackward.toString()<<endl;

}

#endif

}  //KPlato namespace
