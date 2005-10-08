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

#include <qdom.h>
#include <qbrush.h>
#include <kdebug.h>
#include <koRect.h> //DEBUGRECT

namespace KPlato
{

KPTTask::KPTTask(KPTNode *parent) : KPTNode(parent), m_resource() {
    m_resource.setAutoDelete(true);
    KPTDuration d(1, 0, 0);
    m_effort = new KPTEffort(d);
    m_effort->setOptimisticRatio(-10);
    m_effort->setPessimisticRatio(20);
    m_requests = 0;

    if (m_parent)
        m_leader = m_parent->leader();
            
    m_parentProxyRelations.setAutoDelete(true);
    m_childProxyRelations.setAutoDelete(true);
}

KPTTask::KPTTask(KPTTask &task, KPTNode *parent) 
    : KPTNode(task, parent), 
      m_resource() {
    m_resource.setAutoDelete(true);
    m_parentProxyRelations.setAutoDelete(true);
    m_childProxyRelations.setAutoDelete(true);
    m_requests = 0;
    
    m_effort = task.effort() ? new KPTEffort(*(task.effort())) 
                             : new KPTEffort(); // Avoid crash, (shouldn't be zero)
}


KPTTask::~KPTTask() {
    delete m_effort;
}

int KPTTask::type() const {
	if ( numChildren() > 0) {
	  return KPTNode::Type_Summarytask;
	}
	else if ( 0 == effort()->expected().seconds() ) {
		return KPTNode::Type_Milestone;
	}
	else {
		return KPTNode::Type_Task;
	}
}



KPTDuration *KPTTask::getExpectedDuration() {
    //kdDebug()<<k_funcinfo<<endl;
    // Duration should already be calculated
    return new KPTDuration(m_duration);
}

KPTDuration *KPTTask::getRandomDuration() {
    return 0L;
}

KPTResourceGroupRequest *KPTTask::resourceGroupRequest(KPTResourceGroup *group) const {
    if (m_requests)
        return m_requests->find(group);
    return 0;
}

void KPTTask::clearResourceRequests() {
    if (m_requests)
        m_requests->clear();
}

void KPTTask::addRequest(KPTResourceGroup *group, int numResources) {
    addRequest(new KPTResourceGroupRequest(group, numResources));
}

void KPTTask::addRequest(KPTResourceGroupRequest *request) {
    if (!m_requests)
        m_requests = new KPTResourceRequestCollection(*this);
    m_requests->addRequest(request);
}

void KPTTask::takeRequest(KPTResourceGroupRequest *request) {
    if (m_requests) {
        m_requests->takeRequest(request);
        if (m_requests->isEmpty()) {
            delete m_requests;
            m_requests = 0;
        }
    }
}

int KPTTask::units() const {
    if (!m_requests)
        return 0;
    return m_requests->units();
}

int KPTTask::workUnits() const {
    if (!m_requests)
        return 0;
    return m_requests->workUnits();
}

void KPTTask::makeAppointments() {
    if (type() == KPTNode::Type_Task) {
        if (m_requests) 
            m_requests->makeAppointments(this);
    } else if (type() == KPTNode::Type_Summarytask) {
        QPtrListIterator<KPTNode> nit(m_nodes);
        for ( ; nit.current(); ++nit ) {
            nit.current()->makeAppointments();
        }
    } else if (type() == KPTNode::Type_Milestone) {
        //kdDebug()<<k_funcinfo<<"Milestone not implemented"<<endl;
        // Well, shouldn't have resources anyway...
    }
}

void KPTTask::calcResourceOverbooked() {
    m_resourceOverbooked = false;
    if (type() == KPTNode::Type_Task) {
        QPtrListIterator<KPTAppointment> it = m_appointments;
        for (; it.current(); ++it) {
            if (it.current()->resource()->isOverbooked(m_startTime, m_endTime)) {
                m_resourceOverbooked = true;
            }
        }
    } else if (type() == KPTNode::Type_Summarytask) {
        QPtrListIterator<KPTNode> nit(m_nodes);
        for ( ; nit.current(); ++nit ) {
            nit.current()->calcResourceOverbooked();
        }
    }
}

// A new constraint means start/end times and duration must be recalculated
void KPTTask::setConstraint(KPTNode::ConstraintType type) {
    if (m_constraint == type)
        return;
    m_constraint = type;
    // atm, the user must recalculate the project
//    calculateStartEndTime();
}


bool KPTTask::load(QDomElement &element) {
    // Load attributes (TODO: Handle different types of tasks, milestone, summary...)
    bool ok = false;
    m_id = element.attribute("id");
    
    m_name = element.attribute("name");
    m_leader = element.attribute("leader");
    m_description = element.attribute("description");
    //kdDebug()<<k_funcinfo<<m_name<<": id="<<m_id<<endl;

    earliestStart = KPTDateTime::fromString(element.attribute("earlieststart"));
    latestFinish = KPTDateTime::fromString(element.attribute("latestfinish"));
    setStartTime(KPTDateTime::fromString(element.attribute("start")));
    setEndTime(KPTDateTime::fromString(element.attribute("end")));
    m_workStartTime = KPTDateTime::fromString(element.attribute("start-work"));
    m_workEndTime = KPTDateTime::fromString(element.attribute("end-work"));
    m_duration = KPTDuration::fromString(element.attribute("duration"));

    // Allow for both numeric and text
    QString constraint = element.attribute("scheduling","0");
    m_constraint = (KPTNode::ConstraintType)constraint.toInt(&ok);
    if (!ok)
        KPTNode::setConstraint(constraint); // hmmm, why do I need KPTNode::?

    m_constraintStartTime = KPTDateTime::fromString(element.attribute("constraint-starttime"));
    m_constraintEndTime = KPTDateTime::fromString(element.attribute("constraint-endtime"));

    m_inCriticalPath = element.attribute("in-critical-path", "0").toInt();
    
    m_resourceError = element.attribute("resource-error", "0").toInt();
    m_resourceOverbooked = element.attribute("resource-overbooked", "0").toInt();
    m_schedulingError = element.attribute("scheduling-conflict", "0").toInt();
    m_notScheduled = element.attribute("not-scheduled", "0").toInt();
    
    m_wbs = element.attribute("wbs", "");
    
    // Load the project children
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
	if (list.item(i).isElement()) {
	    QDomElement e = list.item(i).toElement();

	    if (e.tagName() == "project") {
		// Load the subproject
		KPTProject *child = new KPTProject(this);
		if (child->load(e))
		    addChildNode(child);
		else
		    // TODO: Complain about this
		    delete child;
	    } else if (e.tagName() == "task") {
		// Load the task
		KPTTask *child = new KPTTask(this);
		if (child->load(e))
		    addChildNode(child);
		else
		    // TODO: Complain about this
		    delete child;
	    } else if (e.tagName() == "resource") {
		// TODO: Load the resource
	    } else if (e.tagName() == "effort") {
    		//  Load the effort
            m_effort->load(e);
	    } else if (e.tagName() == "resourcegroup-request") {
		    // Load the resource request
            KPTProject *p = dynamic_cast<KPTProject *>(projectNode());
            if (p == 0) {
                kdDebug()<<k_funcinfo<<"Project does not exist"<<endl;
            } else {
                KPTResourceGroupRequest *r = new KPTResourceGroupRequest();
                if (r->load(e, p))
                    addRequest(r);
                else {
                    kdError()<<k_funcinfo<<"Failed to load resource request"<<endl;
                    delete r;
                }
            }
        } else if (e.tagName() == "progress") {
            m_progress.started = (bool)e.attribute("started", "0").toInt();
            m_progress.finished = (bool)e.attribute("finished", "0").toInt();
            m_progress.startTime = KPTDateTime::fromString(e.attribute("startTime"));
            m_progress.finishTime = KPTDateTime::fromString(e.attribute("finishTime"));
            m_progress.percentFinished = e.attribute("percent-finished", "0").toInt();
            m_progress.remainingEffort = KPTDuration::fromString(e.attribute("remaining-effort"));
            m_progress.totalPerformed = KPTDuration::fromString(e.attribute("performed-effort"));
        }
    }
    }
    //kdDebug()<<k_funcinfo<<m_name<<" loaded"<<endl;
    return true;
}


void KPTTask::save(QDomElement &element)  {
    QDomElement me = element.ownerDocument().createElement("task");
    element.appendChild(me);

    //TODO: Handle different types of tasks, milestone, summary...
    me.setAttribute("id", m_id);
    me.setAttribute("name", m_name);
    me.setAttribute("leader", m_leader);
    me.setAttribute("description", m_description);

    me.setAttribute("earlieststart",earliestStart.toString());
    me.setAttribute("latestfinish",latestFinish.toString());

    me.setAttribute("start",m_startTime.toString());
    me.setAttribute("end",m_endTime.toString());
    me.setAttribute("start-work", m_workStartTime.toString());
    me.setAttribute("end-work", m_workEndTime.toString());
    me.setAttribute("duration",m_duration.toString());
    me.setAttribute("scheduling",constraintToString());
    me.setAttribute("constraint-starttime",m_constraintStartTime.toString());
    me.setAttribute("constraint-endtime",m_constraintEndTime.toString());
    
    me.setAttribute("in-critical-path",m_inCriticalPath);
    
    me.setAttribute("resource-error",m_resourceError);
    me.setAttribute("resource-overbooked",m_resourceOverbooked);
    me.setAttribute("scheduling-conflict",m_schedulingError);
    me.setAttribute("not-scheduled",m_notScheduled);

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
    
    if (m_requests) {
        m_requests->save(me);
    }

    for (int i=0; i<numChildren(); i++)
    	// First add the child
	    getChildNode(i)->save(me);
}

// Returns the total planned effort for this task (or subtasks) 
KPTDuration KPTTask::plannedEffort() {
   //kdDebug()<<k_funcinfo<<endl;
    KPTDuration eff;
    if (type() == KPTNode::Type_Summarytask) {
        QPtrListIterator<KPTNode> it(childNodeIterator());
        for (; it.current(); ++it) {
            eff += it.current()->plannedEffort();
        }
    } else {
        QPtrListIterator<KPTAppointment> it(m_appointments);
        for (; it.current(); ++it) {
            eff += it.current()->plannedEffort();
        }
    }
    return eff;
}

// Returns the total planned effort for this task (or subtasks) on date
KPTDuration KPTTask::plannedEffort(const QDate &date) {
   //kdDebug()<<k_funcinfo<<endl;
    KPTDuration eff;
    if (type() == KPTNode::Type_Summarytask) {
        QPtrListIterator<KPTNode> it(childNodeIterator());
        for (; it.current(); ++it) {
            eff += it.current()->plannedEffort(date);
        }
    } else {
        QPtrListIterator<KPTAppointment> it(m_appointments);
        for (; it.current(); ++it) {
            eff += it.current()->plannedEffort(date);
        }
    }
    return eff;
}

// Returns the total planned effort for this task (or subtasks) upto and including date
KPTDuration KPTTask::plannedEffortTo(const QDate &date) {
    //kdDebug()<<k_funcinfo<<endl;
    KPTDuration eff;
    if (type() == KPTNode::Type_Summarytask) {
        QPtrListIterator<KPTNode> it(childNodeIterator());
        for (; it.current(); ++it) {
            eff += it.current()->plannedEffortTo(date);
        }
    } else {
        QPtrListIterator<KPTAppointment> it(m_appointments);
        for (; it.current(); ++it) {
            eff += it.current()->plannedEffortTo(date);
        }
    }
    return eff;
}

// Returns the total planned effort for this task (or subtasks) 
KPTDuration KPTTask::actualEffort() {
   //kdDebug()<<k_funcinfo<<endl;
    KPTDuration eff;
    if (type() == KPTNode::Type_Summarytask) {
        QPtrListIterator<KPTNode> it(childNodeIterator());
        for (; it.current(); ++it) {
            eff += it.current()->actualEffort();
        }
    } else {
        QPtrListIterator<KPTAppointment> it(m_appointments);
        for (; it.current(); ++it) {
            eff += it.current()->actualEffort();
        }
    }
    return eff;
}

// Returns the total planned effort for this task (or subtasks) on date
KPTDuration KPTTask::actualEffort(const QDate &date) {
   //kdDebug()<<k_funcinfo<<endl;
    KPTDuration eff;
    if (type() == KPTNode::Type_Summarytask) {
        QPtrListIterator<KPTNode> it(childNodeIterator());
        for (; it.current(); ++it) {
            eff += it.current()->actualEffort(date);
        }
    } else {
        QPtrListIterator<KPTAppointment> it(m_appointments);
        for (; it.current(); ++it) {
            eff += it.current()->actualEffort(date);
        }
    }
    return eff;
}

// Returns the total planned effort for this task (or subtasks) on date
KPTDuration KPTTask::actualEffortTo(const QDate &date) {
   //kdDebug()<<k_funcinfo<<endl;
    KPTDuration eff;
    if (type() == KPTNode::Type_Summarytask) {
        QPtrListIterator<KPTNode> it(childNodeIterator());
        for (; it.current(); ++it) {
            eff += it.current()->actualEffortTo(date);
        }
    } else {
        QPtrListIterator<KPTAppointment> it(m_appointments);
        for (; it.current(); ++it) {
            eff += it.current()->actualEffortTo(date);
        }
    }
    return eff;
}

double KPTTask::plannedCost() {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == KPTNode::Type_Summarytask) {
        QPtrListIterator<KPTNode> it(childNodeIterator());
        for (; it.current(); ++it) {
            c += it.current()->plannedCost();
        }
    } else {
        QPtrListIterator<KPTAppointment> it(m_appointments);
        for (; it.current(); ++it) {
            c += it.current()->plannedCost();
        }
    }
    return c;
}

double KPTTask::plannedCost(const QDate &date) {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == KPTNode::Type_Summarytask) {
        QPtrListIterator<KPTNode> it(childNodeIterator());
        for (; it.current(); ++it) {
            c += it.current()->plannedCost(date);
        }
    } else {
        QPtrListIterator<KPTAppointment> it(m_appointments);
        for (; it.current(); ++it) {
            c += it.current()->plannedCost(date);
        }
    }
    return c;
}

double KPTTask::plannedCostTo(const QDate &date) {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == KPTNode::Type_Summarytask) {
        QPtrListIterator<KPTNode> it(childNodeIterator());
        for (; it.current(); ++it) {
            c += it.current()->plannedCostTo(date);
        }
    } else {
        QPtrListIterator<KPTAppointment> it(m_appointments);
        for (; it.current(); ++it) {
            c += it.current()->plannedCostTo(date);
        }
    }
    return c;
}

double KPTTask::actualCost() {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == KPTNode::Type_Summarytask) {
        QPtrListIterator<KPTNode> it(childNodeIterator());
        for (; it.current(); ++it) {
            c += it.current()->actualCost();
        }
    } else {
        QPtrListIterator<KPTAppointment> it(m_appointments);
        for (; it.current(); ++it) {
            c += it.current()->actualCost();
        }
    }
    return c;
}

double KPTTask::actualCost(const QDate &date) {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == KPTNode::Type_Summarytask) {
        QPtrListIterator<KPTNode> it(childNodeIterator());
        for (; it.current(); ++it) {
            c += it.current()->actualCost(date);
        }
    } else {
        QPtrListIterator<KPTAppointment> it(m_appointments);
        for (; it.current(); ++it) {
            c += it.current()->actualCost(date);
        }
    }
    return c;
}

double KPTTask::actualCostTo(const QDate &date) {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == KPTNode::Type_Summarytask) {
        QPtrListIterator<KPTNode> it(childNodeIterator());
        for (; it.current(); ++it) {
            c += it.current()->actualCostTo(date);
        }
    } else {
        QPtrListIterator<KPTAppointment> it(m_appointments);
        for (; it.current(); ++it) {
            c += it.current()->actualCostTo(date);
        }
    }
    return c;
}

//FIXME Handle summarytasks
double KPTTask::effortPerformanceIndex(const QDate &date, bool *error) {
    double res = 0.0;
    KPTDuration ae = actualEffortTo(date);
    
    bool e = (ae == KPTDuration::zeroDuration || m_progress.percentFinished == 0);
    if (error) {
        *error = e;
    }
    if (!e) {
        res = (plannedEffortTo(date).toDouble() * ((double)m_progress.percentFinished/100.0) / ae.toDouble());
    }
    return res;
}

//FIXME Handle summarytasks
double KPTTask::costPerformanceIndex(const QDate &date, bool *error) {
    double res = 0.0;
    KPTDuration ac = actualCostTo(date);
    
    bool e = (ac == KPTDuration::zeroDuration || m_progress.percentFinished == 0);
    if (error) {
        *error = e;
    }
    if (!e) {
        res = (plannedCostTo(date) * m_progress.percentFinished)/(100 * actualCostTo(date));
    }
    return res;
}


void KPTTask::initiateCalculationLists(QPtrList<KPTNode> &startnodes, QPtrList<KPTNode> &endnodes, QPtrList<KPTNode> &summarytasks/*, QPtrList<KPTNode> &milestones*/) {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (type() == KPTNode::Type_Summarytask) {
        summarytasks.append(this);
        // propagate my relations to my children and dependent nodes
        
        QPtrListIterator<KPTNode> nodes = m_nodes;
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

KPTDateTime KPTTask::calculatePredeccessors(const QPtrList<KPTRelation> &list, int use) {
    KPTDateTime time;
    QPtrListIterator<KPTRelation> it = list;
    for (; it.current(); ++it) {
        if (it.current()->parent()->type() == Type_Summarytask) {
            //kdDebug()<<k_funcinfo<<"Skip summarytask: "<<it.current()->parent()->name()<<endl;
            continue; // skip summarytasks
        }
        KPTDateTime t = it.current()->parent()->calculateForward(use); // early finish
        switch (it.current()->type()) {
            case KPTRelation::StartStart:
                // I can't start earlier than my predesseccor
                t = it.current()->parent()->earliestStartForward() + it.current()->lag();
                break;
            case KPTRelation::FinishFinish:
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
KPTDateTime KPTTask::calculateForward(int use) {
    //kdDebug()<<k_funcinfo<<m_name<<<<endl;
    if (m_visitedForward) {
    //kdDebug()<<earliestStart.toString()<<" + "<<m_durationBackward.toString()<<" "<<m_name<<" calculateForward() (visited)"<<endl;
        return m_earliestStartForward + m_durationForward;
    }
    // First, calculate all predecessors
    if (!dependParentNodes().isEmpty()) {
        KPTDateTime time = calculatePredeccessors(dependParentNodes(), use);
        if (time.isValid() && time > earliestStart) {
            earliestStart = time;
        }
    }
    if (!m_parentProxyRelations.isEmpty()) {
        KPTDateTime time = calculatePredeccessors(m_parentProxyRelations, use);
        if (time.isValid() && time > earliestStart) {
            earliestStart = time;
        }
    }    
    m_earliestStartForward = earliestStart;
    if (type() == KPTNode::Type_Task) {
        switch (constraint()) {
            case KPTNode::ASAP:
            case KPTNode::ALAP:
                m_durationForward = duration(m_earliestStartForward, use, false);
                break;
            case KPTNode::MustFinishOn:
                m_durationForward = duration(m_constraintEndTime, use, true);
                m_earliestStartForward = m_constraintEndTime - m_durationForward;
                break;
            case KPTNode::FinishNotLater:
                m_durationForward = duration(m_earliestStartForward, use, false);
                if (m_earliestStartForward + m_durationForward > m_constraintEndTime) {
                    m_durationForward = duration(m_constraintEndTime, use, true);
                    m_earliestStartForward = m_constraintEndTime - m_durationForward;
                }
                break;
            case KPTNode::MustStartOn:
                m_earliestStartForward = m_constraintStartTime;
                m_durationForward = duration(m_earliestStartForward, use, false);
                break;
            case KPTNode::StartNotEarlier:
                if (m_earliestStartForward < m_constraintStartTime) {
                    m_earliestStartForward = m_constraintStartTime;
                }
                m_durationForward = duration(m_earliestStartForward, use, false);
                break;
            case KPTNode::FixedInterval: {
                KPTDateTime st = m_constraintStartTime;
                KPTDateTime end = m_constraintEndTime;
                m_earliestStartForward = st;
                m_durationForward = end - st;
                break;
            }
        }
    } else if (type() == KPTNode::Type_Milestone) {
        m_durationForward = KPTDuration::zeroDuration;
        switch (constraint()) {
            case KPTNode::MustFinishOn:
                m_earliestStartForward = m_constraintEndTime;
                break;
            case KPTNode::FinishNotLater:
                if (m_earliestStartForward > m_constraintEndTime) {
                    m_earliestStartForward = m_constraintEndTime;
                }
                break;
            case KPTNode::MustStartOn:
                m_earliestStartForward = m_constraintStartTime;
                break;
            case KPTNode::StartNotEarlier:
                if (m_earliestStartForward < m_constraintStartTime) {
                    m_earliestStartForward = m_constraintStartTime;
                }
                break;
            case KPTNode::FixedInterval:
                m_earliestStartForward = m_constraintStartTime;
                break;
            default:
                break;
        }
        //kdDebug()<<k_funcinfo<<m_name<<" "<<earliestStart.toString()<<endl
    } else if (type() == KPTNode::Type_Summarytask) {
        kdWarning()<<k_funcinfo<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    } else { // ???
        m_durationForward = KPTDuration::zeroDuration;
    }
    
    //kdDebug()<<"Earlyfinish: "<<m_earliestStartForward.toString()<<"+"<<m_durationForward.toString()<<"="<<(m_earliestStartForward+m_durationForward).toString()<<" "<<m_name<<" calculateForward()"<<endl;
    m_visitedForward = true;
    return m_earliestStartForward + m_durationForward;
}

KPTDateTime KPTTask::calculateSuccessors(const QPtrList<KPTRelation> &list, int use) {
    KPTDateTime time;
    QPtrListIterator<KPTRelation> it = list;
    for (; it.current(); ++it) {
        if (it.current()->child()->type() == Type_Summarytask) {
            //kdDebug()<<k_funcinfo<<"Skip summarytask: "<<it.current()->parent()->name()<<endl;
            continue; // skip summarytasks
        }
        KPTDateTime t = it.current()->child()->calculateBackward(use);
        switch (it.current()->type()) {
            case KPTRelation::StartStart:
                // I must start before my successor, so
                // I can't finish later than it's (starttime-lag) + my duration
                t -= it.current()->lag();
                t += duration(t, use, false);
                break;
            case KPTRelation::FinishFinish:
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
KPTDateTime KPTTask::calculateBackward(int use) {
    //kdDebug()<<k_funcinfo<<m_name<<<<endl;
    if (m_visitedBackward) {
    //kdDebug()<<latestFinish.toString()<<" - "<<m_durationBackward.toString()<<" "<<m_name<<" calculateBackward() (visited)"<<endl;
        return m_latestFinishBackward - m_durationBackward;
    }
    // First, calculate all successors
    if (!dependChildNodes().isEmpty()) {
        KPTDateTime time = calculateSuccessors(dependChildNodes(), use);
        if (time.isValid() && time < latestFinish) {
            latestFinish = time;
        }
    }
    if (!m_childProxyRelations.isEmpty()) {
        KPTDateTime time = calculateSuccessors(m_childProxyRelations, use);
        if (time.isValid() && time < latestFinish) {
            latestFinish = time;
        }
    }
    m_latestFinishBackward = latestFinish;
    if (type() == KPTNode::Type_Task) {
        switch (constraint()) {
            case KPTNode::ASAP:
            case KPTNode::ALAP:
                m_durationBackward = duration(m_latestFinishBackward, use, true);
                break;
            case KPTNode::MustStartOn:
                m_durationBackward = duration(m_constraintStartTime, use, false);
                m_latestFinishBackward = m_constraintStartTime + m_durationBackward;
                break;
            case KPTNode::StartNotEarlier:
                m_durationBackward = duration(m_latestFinishBackward, use, true);
                if (m_latestFinishBackward - m_durationBackward < m_constraintStartTime) {
                    m_durationBackward = duration(m_constraintStartTime, use, false);
                    m_latestFinishBackward = m_constraintStartTime + m_durationBackward;
                }
                break;
            case KPTNode::MustFinishOn:
                m_latestFinishBackward = m_constraintEndTime;
                m_durationBackward = duration(m_latestFinishBackward, use, true);
                break;
            case KPTNode::FinishNotLater:
                if (m_latestFinishBackward > m_constraintEndTime)
                    m_latestFinishBackward = m_constraintEndTime;
                m_durationBackward = duration(m_latestFinishBackward, use, true);
                break;
            case KPTNode::FixedInterval: {
                KPTDateTime st = m_constraintStartTime;
                KPTDateTime end = m_constraintEndTime;
                m_latestFinishBackward = end;
                m_durationBackward = end - st;
                break;
            }
        }
    } else if (type() == KPTNode::Type_Milestone) {
        m_durationBackward = KPTDuration::zeroDuration;
        switch (constraint()) {
            case KPTNode::MustFinishOn:
                m_latestFinishBackward = m_constraintEndTime;
                break;
            case KPTNode::FinishNotLater:
                if (m_latestFinishBackward > m_constraintEndTime) {
                    m_latestFinishBackward = m_constraintEndTime;
                }
                break;
            case KPTNode::MustStartOn:
                m_latestFinishBackward = m_constraintStartTime;
                break;
            case KPTNode::StartNotEarlier:
                if (m_latestFinishBackward < m_constraintStartTime) {
                    m_latestFinishBackward = m_constraintStartTime;
                }
                break;
            case KPTNode::FixedInterval:
                m_latestFinishBackward = m_constraintEndTime;
                break;
            default:
                break;
        }
        //kdDebug()<<k_funcinfo<<m_name<<" "<<latestFinish.toString()<<" : "<<m_endTime.toString()<<endl;
    } else if (type() == KPTNode::Type_Summarytask) {
        kdWarning()<<k_funcinfo<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    } else { // ???
        m_durationBackward = KPTDuration::zeroDuration;
    }
    //kdDebug()<<"Latestart: "<<m_latestFinishBackward.toString()<<"-"<<m_durationBackward.toString()<<"="<<(m_latestFinishBackward-m_durationBackward).toString()<<" "<<m_name<<" calculateBackward()"<<endl;
    m_visitedBackward = true;
    return m_latestFinishBackward - m_durationBackward;
}

KPTDateTime KPTTask::schedulePredeccessors(const QPtrList<KPTRelation> &list, int use) {
    KPTDateTime time;
    QPtrListIterator<KPTRelation> it = list;
    for (; it.current(); ++it) {
        if (it.current()->parent()->type() == Type_Summarytask) {
            //kdDebug()<<k_funcinfo<<"Skip summarytask: "<<it.current()->parent()->name()<<endl;
            continue; // skip summarytasks
        }
        // schedule the predecessors
        KPTDateTime earliest = it.current()->parent()->getEarliestStart();
        KPTDateTime t = it.current()->parent()->scheduleForward(earliest, use);
        switch (it.current()->type()) {
            case KPTRelation::StartStart:
                // I can't start before my predesseccor
                t = it.current()->parent()->startTime() + it.current()->lag();
                break;
            case KPTRelation::FinishFinish:
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

KPTDateTime &KPTTask::scheduleForward(KPTDateTime &earliest, int use) {
    //kdDebug()<<k_funcinfo<<m_name<<" earliest="<<earliest.toString()<<endl;
    if (m_visitedForward)
        return m_endTime;
    m_notScheduled = false;
    setStartTime(earliest > earliestStart ? earliest : earliestStart);
    // First, calculate all my own predecessors
    KPTDateTime time = schedulePredeccessors(dependParentNodes(), use);
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
    if(type() == KPTNode::Type_Task) {
        switch (m_constraint) {
        case KPTNode::ASAP:
            // m_startTime calculated above
            m_workStartTime = workStartAfter(m_startTime);
            setStartTime(m_workStartTime);
            m_duration = duration(m_startTime, use, false);
            setEndTime(m_startTime + m_duration);
            m_workEndTime = workFinishBefore(m_endTime);
            break;
        case KPTNode::ALAP:
            // m_startTime calculated above
            m_workEndTime = workFinishBefore(latestFinish);
            setEndTime(m_workEndTime);
            m_duration = duration(m_endTime, use, true);
            setStartTime(m_endTime - m_duration);
            m_workStartTime = workStartAfter(m_startTime);
            break;
        case KPTNode::StartNotEarlier:
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
        case KPTNode::FinishNotLater:
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
        case KPTNode::MustStartOn:
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
        case KPTNode::MustFinishOn:
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
        case KPTNode::FixedInterval: {
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
    } else if (type() == KPTNode::Type_Milestone) {
        switch (m_constraint) {
        case KPTNode::ASAP: {
            setEndTime(m_startTime);
            break;
        }
        case KPTNode::ALAP: {
            setStartTime(latestFinish);
            setEndTime(latestFinish);
            break;
        }
        case KPTNode::MustStartOn:
        case KPTNode::FixedInterval:
            //kdDebug()<<"Forw, MustStartOn: "<<m_constraintStartTime.toString()<<" "<<m_startTime.toString()<<endl;
            if (m_constraintStartTime < m_startTime ||
                m_constraintStartTime > latestFinish) {
                m_schedulingError = true;
            }
            setStartTime(m_constraintStartTime);
            setEndTime(m_constraintStartTime);
            break;
        case KPTNode::MustFinishOn:
            if (m_constraintEndTime < m_startTime ||
                m_constraintEndTime > latestFinish) {
                m_schedulingError = true;
            }
            setStartTime(m_constraintEndTime);
            setEndTime(m_constraintEndTime);
            break;
        case KPTNode::StartNotEarlier:
            if (m_startTime < m_constraintStartTime) {
                m_schedulingError = true;
            }
            setEndTime(m_startTime);
            break;
        case KPTNode::FinishNotLater:
            if (m_startTime > m_constraintEndTime) {
                m_schedulingError = true;
            }
            setEndTime(m_startTime);
            break;
        default:
            break;
        }
        m_duration = KPTDuration::zeroDuration;
    } else if (type() == KPTNode::Type_Summarytask) {
        //shouldn't come here
        setEndTime(m_startTime);
        m_duration = m_endTime - m_startTime;
        kdWarning()<<k_funcinfo<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    }
    //kdDebug()<<m_startTime.toString()<<" : "<<m_endTime.toString()<<" "<<m_name<<" scheduleForward()"<<endl;
    m_visitedForward = true;
    return m_endTime;
}

KPTDateTime KPTTask::scheduleSuccessors(const QPtrList<KPTRelation> &list, int use) {
    KPTDateTime time;
    QPtrListIterator<KPTRelation> it = list;
    for (; it.current(); ++it) {
        if (it.current()->child()->type() == Type_Summarytask) {
            //kdDebug()<<k_funcinfo<<"Skip summarytask: "<<it.current()->child()->name()<<endl;
            continue;
        }
        // get the successors starttime
        KPTDateTime latest = it.current()->child()->getLatestFinish();
        KPTDateTime t = it.current()->child()->scheduleBackward(latest, use);
        switch (it.current()->type()) {
            case KPTRelation::StartStart:
                // I can't start before my successor, so
                // I can't finish later than it's starttime + my duration
                t += duration(t - it.current()->lag(), use, false);
                break;
            case KPTRelation::FinishFinish:
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
KPTDateTime &KPTTask::scheduleBackward(KPTDateTime &latest, int use) {
    //kdDebug()<<k_funcinfo<<m_name<<": latest="<<latest<<endl;
    if (m_visitedBackward)
        return m_startTime;
    m_notScheduled = false;
    setEndTime(latest < latestFinish ? latest : latestFinish);
    // First, calculate all my own successors
    KPTDateTime time = scheduleSuccessors(dependChildNodes(), use);
    if (time.isValid() && time < m_endTime) {
        setEndTime(time);
    }
    // Then my parents
    time = scheduleSuccessors(m_childProxyRelations, use);
    if (time.isValid() && time < m_endTime) {
        setEndTime(time);
    }

    if (type() == KPTNode::Type_Task) {
        switch (m_constraint) {
        case KPTNode::ASAP:
            // m_endTime calculated above
            m_workEndTime = workFinishBefore(m_endTime);
            setEndTime(m_workEndTime);
            m_duration = duration(m_endTime, use, true);
            setStartTime(m_endTime - m_duration);
            m_workStartTime = workStartAfter(m_startTime);
            break;
        case KPTNode::ALAP:
            // m_endTime calculated above
            m_workEndTime = workFinishBefore(m_endTime);
            setEndTime(m_workEndTime);
            m_duration = duration(m_endTime, use, true);
            setStartTime(m_endTime - m_duration);
            m_workStartTime = workStartAfter(m_startTime);
            break;
        case KPTNode::StartNotEarlier:
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
        case KPTNode::FinishNotLater:
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
        case KPTNode::MustStartOn:
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
        case KPTNode::MustFinishOn:
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
        case KPTNode::FixedInterval: {
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
    } else if (type() == KPTNode::Type_Milestone) {
        switch (m_constraint) {
        case KPTNode::ASAP:
            setStartTime(earliestStart);
            setEndTime(earliestStart);
            break;
        case KPTNode::ALAP:
            setStartTime(latestFinish);
            setEndTime(latestFinish);
            break;
        case KPTNode::MustStartOn:
        case KPTNode::FixedInterval:
            if (m_constraintStartTime < earliestStart ||
                m_constraintStartTime > m_endTime) {
                m_schedulingError = true;
            }
            setStartTime(earliestStart);
            setEndTime(earliestStart);
            break;
        case KPTNode::MustFinishOn:
            if (m_constraintEndTime < earliestStart ||
                m_constraintEndTime > m_endTime) {
                m_schedulingError = true;
            }
            setStartTime(earliestStart);
            setEndTime(earliestStart);
            break;
        case KPTNode::StartNotEarlier:
            if (m_constraintStartTime > m_endTime) {
                m_schedulingError = true;
            }
            setStartTime(m_endTime);
            break;
        case KPTNode::FinishNotLater:
            if (m_constraintEndTime < m_endTime) {
                m_schedulingError = true;
            }
            setStartTime(m_endTime);
            break;
        default:
            break;
        }
        m_duration = KPTDuration::zeroDuration;
    } else if (type() == KPTNode::Type_Summarytask) {
        //shouldn't come here
        setStartTime(m_endTime);
        m_duration = m_endTime - m_startTime;
        kdWarning()<<k_funcinfo<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    }
    //kdDebug()<<k_funcinfo<<m_name<<": "<<m_startTime.toString()<<" : "<<m_endTime.toString()<<endl;
    m_visitedBackward = true;
    return m_startTime;
}

void KPTTask::adjustSummarytask() {
    if (type() == Type_Summarytask) {
        KPTDateTime start = latestFinish;
        KPTDateTime end = earliestStart;
        QPtrListIterator<KPTNode> it(m_nodes);
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

KPTDuration KPTTask::calcDuration(const KPTDateTime &time, const KPTDuration &effort, bool backward) {
    //kdDebug()<<"calcDuration "<<m_name<<" effort="<<effort.toString(KPTDuration::Format_Day)<<endl;
    if (!m_requests || m_requests->isEmpty()) {
        m_resourceError = true;
        return effort;
    }
    KPTDuration dur = m_requests->duration(time, effort, backward);
    //kdDebug()<<"calcDuration "<<m_name<<": "<<time.toString()<<" to "<<(time+dur).toString()<<" = "<<dur.toString(KPTDuration::Format_Day)<<endl;
    return dur;
}

void KPTTask::clearProxyRelations() {
    m_parentProxyRelations.clear();
    m_childProxyRelations.clear();
}

void KPTTask::addParentProxyRelations(QPtrList<KPTRelation> &list) {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (type() == Type_Summarytask) {
        // propagate to my children
        //kdDebug()<<k_funcinfo<<m_name<<" is summary task"<<endl;
        QPtrListIterator<KPTNode> nodes = m_nodes;
        for (; nodes.current(); ++nodes) {
            nodes.current()->addParentProxyRelations(list);
            nodes.current()->addParentProxyRelations(dependParentNodes());
        }        
    } else {
        // add 'this' as child relation to the relations parent
        //kdDebug()<<k_funcinfo<<m_name<<" is not summary task"<<endl;
        QPtrListIterator<KPTRelation> it = list;
        for (; it.current(); ++it) {
            it.current()->parent()->addChildProxyRelation(this, it.current());
            // add a parent relation to myself
            addParentProxyRelation(it.current()->parent(), it.current());
        }
    }
}

void KPTTask::addChildProxyRelations(QPtrList<KPTRelation> &list) {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (type() == Type_Summarytask) {
        // propagate to my children
        //kdDebug()<<k_funcinfo<<m_name<<" is summary task"<<endl;
        QPtrListIterator<KPTNode> nodes = m_nodes;
        for (; nodes.current(); ++nodes) {
            nodes.current()->addChildProxyRelations(list);
            nodes.current()->addChildProxyRelations(dependChildNodes());
        }        
    } else {
        // add 'this' as parent relation to the relations child
        //kdDebug()<<k_funcinfo<<m_name<<" is not summary task"<<endl;
        QPtrListIterator<KPTRelation> it = list;
        for (; it.current(); ++it) {
            it.current()->child()->addParentProxyRelation(this, it.current());
            // add a child relation to myself
            addChildProxyRelation(it.current()->child(), it.current());
        }
    }
}

void KPTTask::addParentProxyRelation(KPTNode *node, const KPTRelation *rel) {
    if (node->type() != Type_Summarytask) {
        if (type() == Type_Summarytask) {
            //kdDebug()<<"Add parent proxy from my children "<<m_name<<" to "<<node->name()<<endl;
            QPtrListIterator<KPTNode> nodes = m_nodes;
            for (; nodes.current(); ++nodes) {
                nodes.current()->addParentProxyRelation(node, rel);
            }
        } else {
            //kdDebug()<<"Add parent proxy from "<<node->name()<<" to (me) "<<m_name<<endl;
            m_parentProxyRelations.append(new KPTProxyRelation(node, this, rel->type(), rel->lag()));
        }
    }
}

void KPTTask::addChildProxyRelation(KPTNode *node, const KPTRelation *rel) {
    if (node->type() != Type_Summarytask) {
        if (type() == Type_Summarytask) {
            //kdDebug()<<"Add child proxy from my children "<<m_name<<" to "<<node->name()<<endl;
            QPtrListIterator<KPTNode> nodes = m_nodes;
            for (; nodes.current(); ++nodes) {
                nodes.current()->addChildProxyRelation(node, rel);
            }
        } else {
            //kdDebug()<<"Add child proxy from (me) "<<m_name<<" to "<<node->name()<<endl;
            m_childProxyRelations.append(new KPTProxyRelation(this, node, rel->type(), rel->lag()));
        }
    }
}

bool KPTTask::isEndNode() const {
    QPtrListIterator<KPTRelation> it = m_dependChildNodes;
    for (; it.current(); ++it) {
        if (it.current()->type() == KPTRelation::FinishStart)
            return false;
    }
    QPtrListIterator<KPTRelation> pit = m_childProxyRelations;
    for (; pit.current(); ++pit) {
        if (pit.current()->type() == KPTRelation::FinishStart)
            return false;
    }
    return true;
}
bool KPTTask::isStartNode() const {
    QPtrListIterator<KPTRelation> it = m_dependParentNodes;
    for (; it.current(); ++it) {
        if (it.current()->type() == KPTRelation::FinishStart ||
            it.current()->type() == KPTRelation::StartStart)
            return false;
    }
    QPtrListIterator<KPTRelation> pit = m_parentProxyRelations;
    for (; pit.current(); ++pit) {
        if (pit.current()->type() == KPTRelation::FinishStart ||
            pit.current()->type() == KPTRelation::StartStart)
            return false;
    }
    return true;
}

const KPTDateTime &KPTTask::workStartTime() const {
    if (m_requests)
        return m_workStartTime;
    return startTime();
}

const KPTDateTime &KPTTask::workEndTime() const {
    if (m_requests)
        return m_workEndTime;
    return endTime();
}

KPTDateTime KPTTask::workStartAfter(const KPTDateTime &dt) {
    if (m_requests) {
        KPTDateTime t = m_requests->availableAfter(dt);
        return t.isValid() ? t : dt;
    }
    KPTProject *p = static_cast<KPTProject*>(projectNode());
    if (p && p->standardWorktime()) {
        return p->standardWorktime()->workStartAfter(dt);
    }
    return dt;
}

KPTDateTime KPTTask::workFinishBefore(const KPTDateTime &dt) {
    if (m_requests) {
        return m_requests->availableBefore(dt);
    }
    KPTProject *p = static_cast<KPTProject*>(projectNode());
    if (p && p->standardWorktime()) {
        return p->standardWorktime()->workFinishBefore(dt);
    }
    return dt;
}

KPTDuration KPTTask::positiveFloat() {
    KPTDateTime t = workFinishBefore(latestFinish);
    if (t <= (type() == KPTNode::Type_Milestone ? m_startTime : m_endTime))
        return KPTDuration::zeroDuration;
    return t - m_endTime;
}

bool KPTTask::isCritical() {
    return positiveFloat() == KPTDuration::zeroDuration;
}

bool KPTTask::calcCriticalPath() {
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
    QPtrListIterator<KPTRelation> it(m_parentProxyRelations);
    for (; it.current(); ++it) {
        if (it.current()->parent()->calcCriticalPath()) {
            m_inCriticalPath = true;
        }
    }
    QPtrListIterator<KPTRelation> pit(m_dependParentNodes);
    for (; pit.current(); ++pit) {
        if (pit.current()->parent()->calcCriticalPath()) {
            m_inCriticalPath = true;
        }
    }
    return m_inCriticalPath;
}

#ifndef NDEBUG
void KPTTask::printDebug(bool children, QCString indent) {
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
    KPTNode::printDebug(children, indent);
    kdDebug()<<indent<<"workStartTime="<<m_workStartTime.toString()<<endl;
    kdDebug()<<indent<<"workEndTime="<<m_workEndTime.toString()<<endl;
    kdDebug()<<indent<<"earliestStartForward="<<m_earliestStartForward.toString()<<endl;
    kdDebug()<<indent<<"latestFinishBackward="<<m_latestFinishBackward.toString()<<endl;
    kdDebug()<<indent<<"durationForward="<<m_durationForward.toString()<<endl;
    kdDebug()<<indent<<"durationBackward="<<m_durationBackward.toString()<<endl;

}

#endif

}  //KPlato namespace
