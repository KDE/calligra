/* This file is part of the KDE project
   Copyright (C) 2001 Thomas zander <zander@kde.org>

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


KPTTask::KPTTask(KPTNode *parent) : KPTNode(parent), m_resource() {
    m_resource.setAutoDelete(true);
    KPTDuration d(1, 0, 0);
    m_effort = new KPTEffort(d) ;
    m_requests = 0;

    if (m_parent)
        m_leader = m_parent->leader();
}


KPTTask::~KPTTask() {
    delete m_effort;
}

int KPTTask::type() const {
	if ( numChildren() > 0 && !allChildrenDeleted()) {
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
    if (m_deleted)
        return;
    if (type() == KPTNode::Type_Task) {
        if (m_requests) 
            m_requests->makeAppointments(this);
    } else if (type() == KPTNode::Type_Summarytask) {
        QPtrListIterator<KPTNode> nit(m_nodes);
        for ( ; nit.current(); ++nit ) {
            nit.current()->makeAppointments();
        }
    } else if (type() == KPTNode::Type_Milestone) {
        kdDebug()<<k_funcinfo<<"Milestone not implemented"<<endl;
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
    m_id = mapNode(QString(element.attribute("id","-1")).toInt(&ok), this);
    m_name = element.attribute("name");
    m_leader = element.attribute("leader");
    m_description = element.attribute("description");

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

    return true;
}


void KPTTask::save(QDomElement &element)  {
    if (isDeleted())
        return;
    QDomElement me = element.ownerDocument().createElement("task");
    element.appendChild(me);

    //TODO: Handle different types of tasks, milestone, summary...
    if (m_id < 0)
        m_id = m_parent->mapNode(this);
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

void KPTTask::initiateCalculationLists(QPtrList<KPTNode> &startnodes, QPtrList<KPTNode> &endnodes, QPtrList<KPTNode> &milestones) {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (type() == KPTNode::Type_Summarytask) {
        if (numDependChildNodes() == 0) {
            endnodes.insert(0, this); // Summarytasks go first in list
            //kdDebug()<<k_funcinfo<<"endnodes insert: "<<m_name<<endl;
        }
        if (numDependParentNodes() == 0) {
            startnodes.insert(0, this); // Summarytasks go first in list
            //kdDebug()<<k_funcinfo<<"startnodes insert: "<<m_name<<endl;
        }
        QPtrListIterator<KPTNode> it = childNodeIterator();
        for (; it.current(); ++it) {
            it.current()->initiateCalculationLists(startnodes, endnodes, milestones);
        }
    } else if (type() == KPTNode::Type_Milestone) {
        milestones.append(this);
        //kdDebug()<<k_funcinfo<<"milestones append: "<<m_name<<endl;
        if (numDependChildNodes() == 0) {
            endnodes.append(this);
            //kdDebug()<<k_funcinfo<<"endnodes append: "<<m_name<<endl;
        }
        if (numDependParentNodes() == 0) {
            startnodes.append(this);
            //kdDebug()<<k_funcinfo<<"startnodes append: "<<m_name<<endl;
        }
    } else {
        if (numDependChildNodes() == 0) {
            endnodes.append(this);
            //kdDebug()<<k_funcinfo<<"endnodes append: "<<m_name<<endl;
        }
        if (numDependParentNodes() == 0) {
            startnodes.append(this);
            //kdDebug()<<k_funcinfo<<"startnodes append: "<<m_name<<endl;
        }
    }
}
KPTDateTime KPTTask::calculateForward(int use) {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (m_visitedForward)
        return earliestStart + m_durationForward;
    // First, calculate all predecessors
    QPtrListIterator<KPTRelation> it = dependParentNodes();
    for (; it.current(); ++it) {
        KPTDateTime time = it.current()->parent()->calculateForward(use) + it.current()->lag();
        if (time > earliestStart)
            earliestStart = time;
    }
    if (type() == KPTNode::Type_Summarytask) {
        // my subtasks can't start earlier than I
        moveEarliestStart(earliestStart);
        // Must calculate all subtasks/earliestStart before I know my own
        QPtrListIterator<KPTNode> it = childNodeIterator();
        for (; it.current(); ++it) {
            it.current()->calculateForward(use);
        }
        earliestStart = summarytaskEarliestStart();
        m_durationForward = summarytaskDurationForward(earliestStart);
    } else if (type() == KPTNode::Type_Task) {
        if (m_visitedBackward && (latestFinish - m_durationBackward) == earliestStart) {
            m_durationForward = m_durationBackward;
        } else {
            m_durationForward = duration(earliestStart, use, false);
        }
    } else if (type() == KPTNode::Type_Milestone) {
        m_durationForward = KPTDuration::zeroDuration;
        //kdDebug()<<k_funcinfo<<m_name<<" "<<earliestStart.toString()<<endl
    } else { // ???
        m_durationForward = KPTDuration::zeroDuration;
    }
    //kdDebug()<<k_funcinfo<<m_name<<": "<<earliestStart.toString()<<" dur="<<m_durationForward.toString()<<endl;
    m_visitedForward = true;
    return earliestStart + m_durationForward;
}

KPTDateTime KPTTask::calculateBackward(int use) {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (m_visitedBackward)
        return latestFinish - m_durationBackward;
    // First, calculate all successors
    QPtrListIterator<KPTRelation> it = dependChildNodes();
    for (; it.current(); ++it) {
        KPTDateTime time = it.current()->child()->calculateBackward(use) - it.current()->lag();
        if (time < latestFinish)
            latestFinish = time;
    }
    if (type() == KPTNode::Type_Summarytask) {
        // my subtasks can't finish later than I
        moveLatestFinish(latestFinish);
        // Must calculate all subtasks before I know my own duration
        QPtrListIterator<KPTNode> it = childNodeIterator();
        for (; it.current(); ++it) {
            it.current()->calculateBackward(use);
        }
        latestFinish = summarytaskLatestFinish();
        m_durationBackward = summarytaskDurationBackward(latestFinish);
    } else if (type() == KPTNode::Type_Task) {
        if (m_visitedForward && (earliestStart + m_durationForward) == latestFinish) {
            m_durationBackward = m_durationForward;
        } else {
            m_durationBackward = duration(latestFinish, use, true);
        }
    } else if (type() == KPTNode::Type_Milestone) {
        m_durationBackward = KPTDuration::zeroDuration;
        //kdDebug()<<k_funcinfo<<m_name<<" "<<latestFinish.toString()<<" : "<<m_endTime.toString()<<endl;
    } else { // ???
        m_durationBackward = KPTDuration::zeroDuration;
    }
    //kdDebug()<<k_funcinfo<<m_name<<": "<<latestFinish.toString()<<" dur="<<m_durationBackward.toString()<<endl;
    m_visitedBackward = true;
    return latestFinish - m_durationBackward;
}


KPTDateTime &KPTTask::scheduleForward(KPTDateTime &earliest, int use) {
    //kdDebug()<<k_funcinfo<<m_name<<" earliest="<<earliest<<endl;
    if (m_visitedForward || m_deleted)
        return m_endTime;
    m_startTime = earliest > earliestStart ? earliest : earliestStart;
    // First, calculate all predecessors
    QPtrListIterator<KPTRelation> it = dependParentNodes();
    for (; it.current(); ++it) {
        // get the predecessors endtime (which we can use as our starttime)
        KPTDateTime earliest = it.current()->parent()->getEarliestStart();
        KPTDateTime time = it.current()->parent()->scheduleForward(earliest, use);
        if (time > m_startTime)
            m_startTime = time;
    }

    if(type() == KPTNode::Type_Task) {
        switch (m_constraint) {
        case KPTNode::ASAP:
            // m_startTime calculated above
            if (m_startTime == earliestStart)
                m_duration = m_durationForward;
            else if (m_startTime == latestFinish - m_duration)
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
            //TODO
            break;
        case KPTNode::FinishNotLater:
            //TODO
            break;
        case KPTNode::MustStartOn:
            break;
        default:
            break;
        }
        if (m_requests) {
            m_requests->reserve(m_startTime, m_duration);
        }
    } else if(type() == KPTNode::Type_Milestone) {
        m_endTime = m_startTime;
        m_duration = KPTDuration::zeroDuration;
    } else if (type() == KPTNode::Type_Summarytask) {
        m_endTime = m_startTime;
        KPTDateTime time = m_endTime;
        QPtrListIterator<KPTNode> it(m_nodes);
        for (; it.current(); ++it) {
            // find end time
            time = it.current()->scheduleForward(m_startTime, use);
            if (time > m_endTime)
                m_endTime = time;
            // now start time
            time = it.current()->startTime();
            if (time < m_startTime)
                m_startTime = time;
        }
        m_duration = m_endTime - m_startTime;
    }
    //kdDebug()<<k_funcinfo<<m_name<<": "<<m_startTime.toString()<<" : "<<m_endTime.toString()<<endl;
    m_visitedForward = true;
    return m_endTime;
}

KPTDateTime &KPTTask::scheduleBackward(KPTDateTime &latest, int use) {
    kdDebug()<<k_funcinfo<<m_name<<": latest="<<latest<<endl;
    if (m_visitedBackward || m_deleted)
        return m_startTime;
    m_endTime = latest < latestFinish ? latest : latestFinish;
    // First, calculate all successors
    QPtrListIterator<KPTRelation> it = dependChildNodes();
    for (; it.current(); ++it) {
        // get the successors starttime
        KPTDateTime latest = it.current()->child()->getLatestFinish();
        KPTDateTime time = it.current()->child()->scheduleBackward(latest, use);
        if (time < m_endTime)
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
            //TODO
            break;
        case KPTNode::FinishNotLater:
            //TODO
            break;
        case KPTNode::MustStartOn:
            break;
        default:
            break;
        }
        if (m_requests) {
            m_requests->reserve(m_startTime, m_duration);
        }
    } else if (type() == KPTNode::Type_Milestone) {
        m_startTime = m_endTime;
        m_duration = KPTDuration::zeroDuration;
    } else if (type() == KPTNode::Type_Summarytask) {
        //set limits for children
        m_startTime = m_endTime;
        KPTDateTime time = m_startTime;
        KPTDateTime end = earliestStart;        
        QPtrListIterator<KPTNode> it(m_nodes);
        for (; it.current(); ++it) {
            // find start time
            time = it.current()->scheduleBackward(m_endTime, use);
            if (time < m_startTime)
                m_startTime = time;
            // now end time
            time = it.current()->endTime();
            if (time > end)
                end = time;
        }
        m_endTime = end;
        m_duration = m_endTime - m_startTime;
    }
    kdDebug()<<k_funcinfo<<m_name<<": "<<m_startTime.toString()<<" : "<<m_endTime.toString()<<endl;
    m_visitedBackward = true;
    return m_startTime;
}

void KPTTask::scheduleMilestone() {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (m_deleted)
        return;
    m_startTime = earliestStart;
    QPtrListIterator<KPTRelation> it = dependParentNodes();
    for (; it.current(); ++it) {
        KPTDateTime time = it.current()->parent()->endTime();
        if (time > m_startTime)
            m_startTime = time;
    }
    m_endTime = m_startTime;
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

KPTDuration KPTTask::workbasedDuration(const KPTDateTime &time, const KPTDuration &effort, bool backward) {
    if (!m_requests) {
        m_resourceError = true;
        return effort;
    }
    return m_requests->duration(time, effort, backward);
}

#ifndef NDEBUG
void KPTTask::printDebug(bool children, QCString indent) {
    kdDebug()<<indent<<"+ Task node: "<<name()<<" type="<<type()<<endl;
    indent += "!  ";
    kdDebug()<<indent<<"Requested resources (total): "<<units()<<"%"<<endl;
    kdDebug()<<indent<<"Requested resources (work): "<<workUnits()<<"%"<<endl;
    kdDebug()<<indent<<"Resource overbooked="<<resourceOverbooked()<<endl;
    kdDebug()<<indent<<"resourceError="<<resourceError()<<endl;
    if (m_requests)
        m_requests->printDebug(indent);
    kdDebug()<<indent<<endl;
    KPTNode::printDebug(children, indent);

}
#endif
