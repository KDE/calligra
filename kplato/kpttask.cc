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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kpttask.h"
#include "kptproject.h"
#include "kpttaskdialog.h"
#include "kptduration.h"
#include "kptrelation.h"
#include "kptdatetime.h"

#include <qdom.h>
#include <qbrush.h>
#include <kdebug.h>
#include <koRect.h> //DEBUGRECT

namespace KPlato
{

KPTTask::KPTTask(KPTNode *parent) : KPTNode(parent), m_resource() {
    m_resource.setAutoDelete(true);
    KPTDuration d(1, 0, 0);
    m_effort = new KPTEffort(d) ;
    m_requests = 0;

    if (m_parent)
        m_leader = m_parent->leader();
        
    m_parentProxyRelations.setAutoDelete(true);
    m_childProxyRelations.setAutoDelete(true);
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

KPTDuration *KPTTask::getFloat() {
    return new KPTDuration;
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
        m_requests = new KPTResourceRequestCollection();
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
    m_startTime = KPTDateTime::fromString(element.attribute("start"));
    m_endTime = KPTDateTime::fromString(element.attribute("end"));
    m_duration = KPTDuration::fromString(element.attribute("duration"));

    // Allow for both numeric and text
    QString constraint = element.attribute("scheduling","0");
    m_constraint = (KPTNode::ConstraintType)constraint.toInt(&ok);
    if (!ok)
        KPTNode::setConstraint(constraint); // hmmm, why do I need KPTNode::?

    m_constraintStartTime = KPTDateTime::fromString(element.attribute("constraint-starttime"));
    m_constraintEndTime = KPTDateTime::fromString(element.attribute("constraint-endtime"));

    m_resourceError = element.attribute("resource-error", "0").toInt();
    m_resourceError = element.attribute("resource-overbooked", "0").toInt();
    m_resourceError = element.attribute("scheduling-conflict", "0").toInt();
        
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
    me.setAttribute("duration",m_duration.toString());
    me.setAttribute("scheduling",constraintToString());
    me.setAttribute("constraint-starttime",m_constraintStartTime.toString());
    me.setAttribute("constraint-endtime",m_constraintEndTime.toString());
    
    me.setAttribute("resource-error",m_resourceError);
    me.setAttribute("resource-overbooked",m_resourceOverbooked);
    me.setAttribute("scheduling-conflict",m_schedulingError);

    m_effort->save(me);


    if (m_requests) {
        m_requests->save(me);
    }

    for (int i=0; i<numChildren(); i++)
    	// First add the child
	    getChildNode(i)->save(me);
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
        QPtrList<KPTAppointment> list = appointments(this);
        QPtrListIterator<KPTAppointment> it(list);
        for (; it.current(); ++it) {
            c += it.current()->cost();
        }
    }
    return c;
}

double KPTTask::plannedCost(QDateTime &dt) {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == KPTNode::Type_Summarytask) {
        QPtrListIterator<KPTNode> it(childNodeIterator());
        for (; it.current(); ++it) {
            c += it.current()->plannedCost(dt);
        }
    } else {
        QPtrList<KPTAppointment> list = appointments(this);
        QPtrListIterator<KPTAppointment> it(list);
        for (; it.current(); ++it) {
            c += it.current()->cost(KPTDateTime(dt)); //FIXME
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
        c = 1; //TODO
    }
    return c;
}

int KPTTask::plannedWork()  {
    //kdDebug()<<k_funcinfo<<endl;
    int w = 0;
    if (type() == KPTNode::Type_Summarytask) {
        QPtrListIterator<KPTNode> it(childNodeIterator());
        for (; it.current(); ++it) {
            w += it.current()->plannedWork();
        }
    } else {
        QPtrList<KPTAppointment> a = appointments(this);
        QPtrListIterator<KPTAppointment> it(a);
        for (; it.current(); ++it) {
            if (it.current()->resource()->type() == KPTResource::Type_Work) { // hmmm. show only work?
                w += it.current()->work();
                //TODO:overtime and non-proportional work
            }
        }
    }
    return w;
}

int KPTTask::plannedWork(QDateTime &dt)  {
    //kdDebug()<<k_funcinfo<<endl;
    int w = 0;
    if (type() == KPTNode::Type_Summarytask) {
        QPtrListIterator<KPTNode> it(childNodeIterator());
        for (; it.current(); ++it) {
            w += it.current()->plannedWork(dt);
        }
    } else {
        QPtrList<KPTAppointment> a = appointments(this);
        QPtrListIterator<KPTAppointment> it(a);
        for (; it.current(); ++it) {
            w += it.current()->work(KPTDateTime(dt)); //FIXME
        }
    }
    return w;
}
int KPTTask::actualWork() {
    return 0;
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
        KPTDateTime t = it.current()->parent()->calculateForward(use);
        switch (it.current()->type()) {
            case KPTRelation::StartStart:
                t = it.current()->parent()->getEarliestStart() + it.current()->lag();
                break;
            case KPTRelation::FinishFinish:
                // I can't finisg later than my predeccessor, so
                // I can't start later than it's latestfinish - my duration
                t -= duration(t + it.current()->lag(), use, true);
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
        return earliestStart + m_durationForward;
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
    if (type() == KPTNode::Type_Summarytask) {
        kdWarning()<<k_funcinfo<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    } else if (type() == KPTNode::Type_Task) {
        switch (constraint()) {
            case KPTNode::MustFinishOn:
            case KPTNode::FinishNotLater:
                if (!m_visitedBackward) {
                    KPTDateTime t = m_constraintEndTime - duration(m_constraintEndTime, use, true);
                    if (t > earliestStart)
                        earliestStart = t;
                }
                break;
            case KPTNode::MustStartOn:
            case KPTNode::StartNotEarlier:
                if (!m_visitedBackward &&
                    earliestStart < m_constraintStartTime)
                    earliestStart = m_constraintStartTime;
                break;
            case KPTNode::FixedInterval:
                if (!m_visitedBackward &&
                    earliestStart < m_constraintStartTime) {
                    earliestStart = m_constraintStartTime;
                    m_durationForward = m_constraintEndTime - m_constraintStartTime;
                }
                break;
        }
        // Adjust duration to when resource(s) can start work
        if (m_requests) {
            earliestStart = m_requests->availableAfter(earliestStart);
        }
        if (m_visitedBackward && (latestFinish - m_durationBackward) == earliestStart) {
            m_durationForward = m_durationBackward;
        } else {
            m_durationForward = duration(earliestStart, use, false);
        }
        if (m_requests) {
            // Adjust duration to when resource(s) can do (end) work
            m_durationForward = m_requests->availableBefore(earliestStart + m_durationForward) - earliestStart;
        }
    } else if (type() == KPTNode::Type_Milestone) {
        m_durationForward = KPTDuration::zeroDuration;
        //kdDebug()<<k_funcinfo<<m_name<<" "<<earliestStart.toString()<<endl
    } else { // ???
        m_durationForward = KPTDuration::zeroDuration;
    }
    //kdDebug()<<earliestStart.toString()<<" + "<<m_durationBackward.toString()<<" "<<m_name<<" calculateForward()"<<endl;
    m_visitedForward = true;
    return earliestStart + m_durationForward;
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
                // I can't start before my successor, so
                // I can't finish later than it's starttime + my duration
                t += duration(t -  it.current()->lag(), use, false);
                break;
            case KPTRelation::FinishFinish:
                t = it.current()->parent()->getLatestFinish() -  it.current()->lag();
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
        return latestFinish - m_durationBackward;
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
    if (type() == KPTNode::Type_Summarytask) {
        kdWarning()<<k_funcinfo<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    } else if (type() == KPTNode::Type_Task) {
        switch (constraint()) {
            case KPTNode::MustStartOn:
            case KPTNode::StartNotEarlier:
                if (!m_visitedForward) {
                    KPTDateTime t = m_constraintStartTime + duration(m_constraintStartTime, use, true);
                    if (t < latestFinish)
                        latestFinish = t;
                }
                break;
            case KPTNode::MustFinishOn:
            case KPTNode::FinishNotLater:
                if (!m_visitedForward &&
                    latestFinish > m_constraintEndTime)
                    latestFinish = m_constraintEndTime;
                break;
            case KPTNode::FixedInterval:
                if (!m_visitedForward &&
                    latestFinish > m_constraintEndTime) {
                    latestFinish = m_constraintEndTime;
                    m_durationBackward = m_constraintEndTime - m_constraintStartTime;
                }
                break;
        }
        if (m_requests) {
            // Move to when resource(s) can do (end) work
            latestFinish = m_requests->availableBefore(latestFinish);
        }
        if (m_visitedForward && (earliestStart + m_durationForward) == latestFinish) {
            m_durationBackward = m_durationForward;
        } else {
            m_durationBackward = duration(latestFinish, use, true);            
        }
        if (m_requests) {
            // Adjust duration to when resource(s) can start work
            m_durationBackward = latestFinish - m_requests->availableAfter(latestFinish - m_durationBackward);
        }
    } else if (type() == KPTNode::Type_Milestone) {
        m_durationBackward = KPTDuration::zeroDuration;
        //kdDebug()<<k_funcinfo<<m_name<<" "<<latestFinish.toString()<<" : "<<m_endTime.toString()<<endl;
    } else { // ???
        m_durationBackward = KPTDuration::zeroDuration;
    }
    //kdDebug()<<latestFinish.toString()<<" - "<<m_durationBackward.toString()<<" "<<m_name<<" calculateBackward()"<<endl;
    m_visitedBackward = true;
    return latestFinish - m_durationBackward;
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
    //kdDebug()<<k_funcinfo<<m_name<<" earliest="<<earliest<<endl;
    if (m_visitedForward)
        return m_endTime;
    m_startTime = earliest > earliestStart ? earliest : earliestStart;
    // First, calculate all my own predecessors
    KPTDateTime time = schedulePredeccessors(dependParentNodes(), use);
    if (time.isValid() && time > m_startTime) {
        m_startTime = time;
        //kdDebug()<<k_funcinfo<<m_name<<" new startime="<<m_startTime<<endl;
    }
    // Then my parents
    time = schedulePredeccessors(m_parentProxyRelations, use);
    if (time.isValid() && time > m_startTime) {
        m_startTime = time;
        //kdDebug()<<k_funcinfo<<m_name<<" new startime="<<m_startTime<<endl;
    }

    if(type() == KPTNode::Type_Task) {
        switch (m_constraint) {
        case KPTNode::ASAP:
            // m_startTime calculated above
            if (m_startTime == earliestStart)
                m_duration = m_durationForward;
            else if (m_startTime == latestFinish - m_durationBackward)
                m_duration = m_durationBackward;
            else
                m_duration = duration(m_startTime, use, false);
            
            m_endTime = m_startTime + m_duration;
            break;
        case KPTNode::ALAP:
            m_startTime = latestFinish - m_durationBackward;
            m_endTime = latestFinish;
            m_duration = m_durationBackward;
            break;
        case KPTNode::StartNotEarlier:
            // m_startTime calculated above
            kdDebug()<<"StartNotEarlier="<<m_constraintStartTime.toString()<<" "<<m_startTime.toString()<<endl;
            if (m_startTime < m_constraintStartTime) {
                if (m_constraintStartTime < latestFinish - m_durationBackward) {
                    m_startTime = m_constraintStartTime;
                    m_duration = duration(m_startTime, use, false);
                } else {
                    m_duration = m_durationBackward;
                    m_startTime = latestFinish - m_duration;
                }
            } else if (m_startTime == earliestStart)
                m_duration = m_durationForward;
            else if (m_startTime == latestFinish - m_durationBackward)
                m_duration = m_durationBackward;
            else
                m_duration = duration(m_startTime, use, false);
            m_endTime = m_startTime + m_duration;            
            m_schedulingError = m_startTime < m_constraintStartTime;
            break;
        case KPTNode::FinishNotLater:
            // m_startTime calculated above
            kdDebug()<<"FinishNotLater="<<m_constraintEndTime.toString()<<" "<<m_startTime.toString()<<endl;
            if (m_startTime == earliestStart)
                m_duration = m_durationForward;
            else if (m_startTime == latestFinish - m_durationBackward)
                m_duration = m_durationBackward;
            else
                m_duration = duration(m_startTime, use, false);
            
            m_endTime = m_startTime + m_duration;
            m_schedulingError = m_endTime > m_constraintEndTime;
            break;
        case KPTNode::MustStartOn:
            // m_startTime calculated above
            kdDebug()<<"MustStartOn="<<m_constraintStartTime.toString()<<" "<<m_startTime.toString()<<endl;
            if (m_constraintStartTime < m_startTime ||
                m_constraintStartTime > latestFinish - m_durationBackward) {
                m_schedulingError = true;
            }
            m_startTime = m_constraintStartTime;            
            if (m_startTime == earliestStart)
                m_duration = m_durationForward;
            else if (m_startTime == latestFinish - m_durationBackward)
                m_duration = m_durationBackward;
            else
                m_duration = duration(m_startTime, use, false);            
            m_endTime = m_startTime + m_duration;            
            break;
        case KPTNode::MustFinishOn:
            // m_startTime calculated above
            kdDebug()<<"MustFinishOn="<<m_constraintEndTime.toString()<<" "<<m_startTime.toString()<<endl;
            if (m_constraintEndTime > latestFinish ||
                m_constraintEndTime < earliestStart + m_durationForward) {
                m_schedulingError = true;
            }                
            m_endTime = m_constraintEndTime;            
            if (m_endTime == latestFinish)
                m_duration = m_durationBackward;
            else if (m_endTime == earliestStart + m_durationForward)
                m_duration = m_durationForward;
            else
                m_duration = duration(m_endTime, use, true);            
            m_startTime = m_endTime - m_duration;
            break;
        case KPTNode::FixedInterval:
            // m_startTime calculated above
            kdDebug()<<"FixedInterval="<<m_constraintStartTime.toString()<<" "<<m_startTime.toString()<<endl;
            if (m_constraintEndTime > latestFinish ||
                m_constraintStartTime < m_startTime) {
                m_schedulingError = true;
            }                
            m_startTime = m_constraintStartTime;            
            m_endTime = m_constraintEndTime;            
            m_duration = m_endTime - m_startTime;
            break;
        default:
            break;
        }
        if (m_requests) {
            m_requests->reserve(m_startTime, m_duration);
        }
    } else if (type() == KPTNode::Type_Milestone) {
        switch (m_constraint) {
        case KPTNode::ASAP:
        case KPTNode::ALAP: {
            // milestones generally wants to stick to their dependant parent
            KPTDateTime time = schedulePredeccessors(dependParentNodes(), use);
            if (time.isValid() && time < m_endTime) {
                m_endTime = time;
            }
            // Then my parents
            time = schedulePredeccessors(m_parentProxyRelations, use);
            if (time.isValid() && time < m_endTime) {
                m_endTime = time;
            }
            break;
        }
        case KPTNode::MustStartOn:
        case KPTNode::FixedInterval:
            kdDebug()<<"Forw, MustStartOn: "<<m_constraintStartTime.toString()<<" "<<m_startTime.toString()<<endl;
            if (m_constraintStartTime < m_startTime ||
                m_constraintStartTime > latestFinish) {
                m_schedulingError = true;
            }
            m_startTime = m_endTime = m_constraintStartTime;
            break;
        case KPTNode::MustFinishOn:
            if (m_constraintEndTime < m_startTime ||
                m_constraintEndTime > latestFinish) {
                m_schedulingError = true;
            }
            m_startTime = m_endTime = m_constraintEndTime;
            break;
        case KPTNode::StartNotEarlier:
            if (m_constraintStartTime > m_startTime) {
                m_schedulingError = true;
            }
            m_endTime = m_startTime;
            break;
        case KPTNode::FinishNotLater:
            if (m_constraintEndTime > m_startTime) {
                m_schedulingError = true;
            }
            m_endTime = m_startTime;
            break;
        default:
            break;
        }
        m_duration = KPTDuration::zeroDuration;
    } else if (type() == KPTNode::Type_Summarytask) {
        //shouldn't come here
        m_endTime = m_startTime;
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
    m_endTime = latest < latestFinish ? latest : latestFinish;
    // First, calculate all my own successors
    KPTDateTime time = scheduleSuccessors(dependChildNodes(), use);
    if (time.isValid() && time < m_endTime) {
        m_endTime = time;
    }
    // Then my parents
    time = scheduleSuccessors(m_childProxyRelations, use);
    if (time.isValid() && time < m_endTime) {
        m_endTime = time;
    }

    if (type() == KPTNode::Type_Task) {
        switch (m_constraint) {
        case KPTNode::ASAP:
            m_startTime = earliestStart;
            m_endTime = earliestStart + m_durationForward;
            m_duration = m_durationForward;
            break;
        case KPTNode::ALAP:
            // m_endTime calculated above
            if (m_endTime == latestFinish)
                m_duration = m_durationBackward;
            else if (m_endTime == earliestStart + m_duration)
                m_duration = m_durationForward;
            else
                m_duration = duration(m_endTime, use, true);
            
            m_startTime = m_endTime - m_duration;
            break;
        case KPTNode::StartNotEarlier:
            // m_endTime calculated above
            kdDebug()<<"StartNotEarlier="<<m_constraintStartTime.toString()<<" "<<m_endTime.toString()<<endl;
            if (m_endTime == latestFinish)
                m_duration = m_durationBackward;
            else if (m_constraintStartTime > earliestStart &&
                     m_constraintStartTime < latestFinish - m_durationBackward) {
                m_duration = duration(m_constraintStartTime, use, false);
                if (m_endTime < m_constraintStartTime + m_duration)
                    m_endTime = m_constraintStartTime + m_duration;
            } else if (m_endTime == earliestStart + m_durationForward)
                m_duration = m_durationForward;
            else
                m_duration = duration(m_endTime, use, true);
                            
            m_startTime = m_endTime - m_duration;            
            m_schedulingError = m_startTime < m_constraintStartTime;                
            break;
        case KPTNode::FinishNotLater:
            // m_endTime calculated above
            kdDebug()<<"FinishNotLater="<<m_constraintEndTime.toString()<<" "<<m_endTime.toString()<<endl;
            
            if (m_endTime == earliestStart + m_durationForward)
                m_duration = m_durationForward;
            else if (m_constraintEndTime < latestFinish &&
                     m_constraintEndTime > earliestStart + m_durationForward &&
                     m_endTime > m_constraintEndTime) {
                m_duration = duration(m_constraintEndTime, use, true);
                m_endTime = m_constraintEndTime;
            } else if (m_endTime == latestFinish)
                m_duration = m_durationBackward;
            else
                m_duration = duration(m_endTime, use, true);
            
            m_startTime = m_endTime - m_duration;
            m_schedulingError = m_endTime > m_constraintEndTime;
            break;
        case KPTNode::MustStartOn:
            // m_endTime calculated above
            kdDebug()<<"MustStartOn="<<m_constraintStartTime.toString()<<" "<<m_startTime.toString()<<endl;
            if (m_constraintStartTime < earliestStart ||
                m_constraintStartTime > latestFinish - m_durationBackward) {
                m_schedulingError = true;
            }
            m_startTime = m_constraintStartTime;
            if (m_startTime == earliestStart)
                m_duration = m_durationForward;
            else if (m_startTime == latestFinish - m_durationBackward)
                m_duration = m_durationBackward;
            else
                m_duration = duration(m_startTime, use, false);            
            m_endTime = m_startTime + m_duration;            
            break;
        case KPTNode::MustFinishOn:
            // m_endTime calculated above
            kdDebug()<<"MustFinishOn="<<m_constraintEndTime.toString()<<" "<<m_startTime.toString()<<endl;
            if (m_constraintEndTime > latestFinish ||
                m_constraintEndTime < earliestStart + m_durationForward) {
                m_schedulingError = true;
            }                
            m_endTime = m_constraintEndTime;            
            if (m_endTime == latestFinish)
                m_duration = m_durationBackward;
            else if (m_endTime == earliestStart + m_durationForward)
                m_duration = m_durationForward;
            else
                m_duration = duration(m_endTime, use, true);            
            m_startTime = m_endTime - m_duration;
            break;
        case KPTNode::FixedInterval:
            // m_endTime calculated above
            kdDebug()<<"FixedInterval="<<m_constraintEndTime.toString()<<" "<<m_endTime.toString()<<endl;
            if (m_constraintEndTime > m_endTime ||
                m_constraintStartTime < earliestStart) {
                m_schedulingError = true;
            }                
            m_startTime = m_constraintStartTime;            
            m_endTime = m_constraintEndTime;            
            m_duration = m_endTime - m_startTime;
            break;
        default:
            break;
        }
        if (m_requests) {
            m_requests->reserve(m_startTime, m_duration);
        }
    } else if (type() == KPTNode::Type_Milestone) {
        switch (m_constraint) {
        case KPTNode::ASAP:
        case KPTNode::ALAP:
            // milestones generally wants to stick to their dependant parent
            // let's try using earliestStart
            m_startTime = m_endTime = earliestStart;
            break;
        case KPTNode::MustStartOn:
        case KPTNode::FixedInterval:
            if (m_constraintStartTime < earliestStart ||
                m_constraintStartTime > m_endTime) {
                m_schedulingError = true;
            }
            m_startTime = m_endTime = m_constraintStartTime;
            break;
        case KPTNode::MustFinishOn:
            if (m_constraintEndTime < earliestStart ||
                m_constraintEndTime > m_endTime) {
                m_schedulingError = true;
            }
            m_startTime = m_endTime = m_constraintEndTime;
            break;
        case KPTNode::StartNotEarlier:
            if (m_constraintStartTime > m_endTime) {
                m_schedulingError = true;
            }
            m_startTime = m_endTime;
            break;
        case KPTNode::FinishNotLater:
            if (m_constraintEndTime < m_endTime) {
                m_schedulingError = true;
            }
            m_startTime = m_endTime;
            break;
        default:
            break;
        }
        m_duration = KPTDuration::zeroDuration;
    } else if (type() == KPTNode::Type_Summarytask) {
        //shouldn't come here
        m_startTime = m_endTime;
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
        m_startTime = start;
        m_endTime = end;
        m_duration = end - start;
        //kdDebug()<<k_funcinfo<<m_name<<": "<<m_startTime.toString()<<" : "<<m_endTime.toString()<<endl;
    }
}

// Assumes all subtasks are calculated
KPTDuration KPTTask::summarytaskDurationForward(const KPTDateTime &time) {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    KPTDuration dur;
    if(type() == KPTNode::Type_Summarytask) {
        KPTDuration tmp;
        QPtrListIterator<KPTNode> it = childNodeIterator();
        for (; it.current(); ++it) {
            tmp = it.current()->summarytaskDurationForward(time);
            if (tmp > dur)
                dur = tmp;
        }
    } else {
        dur = earliestStart + m_durationForward - time;
    }
    //kdDebug()<<k_funcinfo<<m_name<<" dur="<<dur.toString()<<endl;
    return dur;
}

// Assumes all subtasks are calculated
KPTDateTime KPTTask::summarytaskEarliestStart() {
    KPTDateTime time;
    if(type() == KPTNode::Type_Summarytask) {
        KPTDateTime tmp;
        QPtrListIterator<KPTNode> it = childNodeIterator();
        for (; it.current(); ++it) {
            tmp = it.current()->summarytaskEarliestStart();
            if (tmp < time || !time.isValid())
                time = tmp;
        }
    } else {
        time = earliestStart;
    }
    return time;
}

// Assumes all subtasks are calculated
KPTDuration KPTTask::summarytaskDurationBackward(const KPTDateTime &time) {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    KPTDuration dur;
    if(type() == KPTNode::Type_Summarytask) {
        KPTDuration tmp;
        QPtrListIterator<KPTNode> it = childNodeIterator();
        for (; it.current(); ++it) {
            tmp = it.current()->summarytaskDurationBackward(time);
            if (tmp > dur)
                dur = tmp;
        }
    } else {
        dur = time - (latestFinish - m_durationBackward);
    }
    //kdDebug()<<k_funcinfo<<m_name<<" dur="<<dur.toString()<<endl;
    return dur;
}

// Assumes all subtasks are calculated
KPTDateTime KPTTask::summarytaskLatestFinish() {
    KPTDateTime time;
    if(type() == KPTNode::Type_Summarytask) {
        KPTDateTime tmp;
        QPtrListIterator<KPTNode> it = childNodeIterator();
        for (; it.current(); ++it) {
            tmp = it.current()->summarytaskLatestFinish();
            if (tmp > time || !time.isValid())
                time = tmp;
        }
    } else {
        time = latestFinish;
    }
    return time;
}

KPTDuration KPTTask::calcDuration(const KPTDateTime &time, const KPTDuration &effort, bool backward) {
    //kdDebug()<<"calcDuration "<<m_name<<endl;
    if (!m_requests) {
        m_resourceError = true;
        return effort;
    }
    KPTDuration dur = m_requests->duration(time, effort, backward);
    //kdDebug()<<"calcDuration "<<m_name<<": "<<time.toString()<<" to "<<(time+dur).toString()<<" = "<<dur.toString()<<endl;
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
        QPtrListIterator<KPTNode> nodes = m_nodes;
        for (; nodes.current(); ++nodes) {
            nodes.current()->addParentProxyRelations(list);
            nodes.current()->addParentProxyRelations(dependParentNodes());
        }        
    } else {
        // add 'this' as child relation to the relations parent
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
        QPtrListIterator<KPTNode> nodes = m_nodes;
        for (; nodes.current(); ++nodes) {
            nodes.current()->addChildProxyRelations(list);
            nodes.current()->addChildProxyRelations(dependChildNodes());
        }        
    } else {
        // add 'this' as parent relation to the relations child
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
        //kdDebug()<<"Add parent proxy from "<<node->name()<<" to (me) "<<m_name<<endl;
        m_parentProxyRelations.append(new KPTProxyRelation(node, this, rel->type(), rel->lag()));
    }
}

void KPTTask::addChildProxyRelation(KPTNode *node, const KPTRelation *rel) {
    if (node->type() != Type_Summarytask) {
        //kdDebug()<<"Add child proxy from (me) "<<m_name<<" to "<<node->name()<<endl;
        m_childProxyRelations.append(new KPTProxyRelation(this, node, rel->type(), rel->lag()));
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

#ifndef NDEBUG
void KPTTask::printDebug(bool children, QCString indent) {
    kdDebug()<<indent<<"+ Task node: "<<name()<<" type="<<type()<<endl;
    indent += "!  ";
    kdDebug()<<indent<<"Requested resources (total): "<<units()<<"%"<<endl;
    kdDebug()<<indent<<"Requested resources (work): "<<workUnits()<<"%"<<endl;
    kdDebug()<<indent<<"Resource overbooked="<<resourceOverbooked()<<endl;
    kdDebug()<<indent<<"resourceError="<<resourceError()<<endl;
    kdDebug()<<indent<<"schedulingError="<<schedulingError()<<endl;
    if (m_requests)
        m_requests->printDebug(indent);
    kdDebug()<<indent<<endl;
    KPTNode::printDebug(children, indent);

}

#endif

}  //KPlato namespace
