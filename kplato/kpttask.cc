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
    m_requests = 0;
    KPTDuration d(1, 0, 0);
    m_effort = new KPTEffort(d) ;

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
    KPTDuration *ed= new KPTDuration(m_duration);
    return ed;
}

KPTDuration *KPTTask::getRandomDuration() {
    return 0L;
}

void KPTTask::setStartTime() {
    //kdDebug()<<k_funcinfo<<endl;
    if(type() == KPTNode::Type_Task ||
       type() == KPTNode::Type_Milestone) {
        switch (m_constraint)
        {
        case KPTNode::ASAP:
            //TODO: it must be room for parents/children also
            m_startTime = earliestStart;
            break;
        case KPTNode::ALAP:
        {
            //TODO: it must be room for parents/children also
            m_startTime = latestFinish - m_duration;
            break;
        }
        case KPTNode::StartNotEarlier:
            constraintTime() > earliestStart ? m_startTime = constraintTime() : m_startTime = earliestStart;
            break;
        case KPTNode::FinishNotLater:
            m_startTime = constraintTime(); // FIXME
            break;
        case KPTNode::MustStartOn:
        {
            KPTDateTime t(constraintTime() + m_duration);
            if (constraintTime() >= earliestStart && t <= latestFinish)
                m_startTime = constraintTime();
            else {
                // TODO: conflict
                m_startTime = earliestStart;
            }
            break;
        }
        default:
            break;
        }
    } else if (type() == KPTNode::Type_Summarytask) {
        m_startTime = QDateTime();
        KPTDateTime *time = 0;
        QPtrListIterator<KPTNode> it(m_nodes);
        for ( ; it.current(); ++it ) {
            it.current()->setStartTime();
            time = it.current()->getStartTime();
            if (!m_startTime.isValid() || m_startTime < *time)
                m_startTime = *time;
        }
        delete time;
    }
}

void KPTTask::setEndTime() {
    //kdDebug()<<k_funcinfo<<endl;
    if(numChildren() == 0) {
        m_endTime = m_startTime + m_duration;
    } else {
        // summary task
        m_endTime = QDateTime();
        KPTDateTime *time = 0;
        QPtrListIterator<KPTNode> it(m_nodes);
        for ( ; it.current(); ++it ) {
            it.current()->setEndTime();
            time = it.current()->getEndTime();
            if (!m_endTime.isValid() || *time > m_endTime)
                m_endTime = *time;

        }
        delete time;
    }
}

KPTDateTime *KPTTask::getStartTime() {
    //kdDebug()<<k_funcinfo<<endl;
    KPTDateTime *time = new KPTDateTime();
    if(numChildren() == 0) {
        *time = m_startTime;
    } else {
        // summary task
        KPTDateTime *start = 0;
        QPtrListIterator<KPTNode> it(m_nodes);
        for ( ; it.current(); ++it ) {
            start = it.current()->getStartTime();
            if (!time->isValid() || *start < *time) {
                *time = *start;
            }
            delete start;
        }
    }
    return time;
}

KPTDateTime *KPTTask::getEndTime() {
    //kdDebug()<<k_funcinfo<<endl;
    KPTDateTime *time = new KPTDateTime();
    if(numChildren() == 0) {
        *time = m_endTime;
    } else {
        // summary task
        KPTDateTime *end;
        QPtrListIterator<KPTNode> it(m_nodes);
        for ( ; it.current(); ++it ) {
            end = it.current()->getEndTime();
            if (!time->isValid() || *end > *time) {
                *time = *end;
            }
            delete end;
        }
    }
    return time;
}

KPTDuration *KPTTask::getFloat() {
    return new KPTDuration;
}

const KPTDuration& KPTTask::expectedDuration(const KPTDateTime &start) {
    //kdDebug()<<k_funcinfo<<endl;
    calculateDuration(start);
    return m_duration;
 }

void KPTTask::calculateDuration(const KPTDateTime &start) {
    //kdDebug()<<k_funcinfo<<m_name<<"  --------->>>>"<<endl;
    m_resourceError = false;
    m_duration = KPTDuration::zeroDuration;
    if (m_effort->type() == KPTEffort::Type_WorkBased) {
        if (m_requests){
            m_duration = m_requests->duration(start, m_effort->expected());
        } else {
            m_duration = m_effort->expected();
            m_resourceError = true;        
        }
    } else if (m_effort->type() == KPTEffort::Type_FixedDuration) {
        // The amount of resource doesn't matter
        m_duration = m_effort->expected(); //hmmm, non-working days?
    } else {
        // error
        kdError()<<k_funcinfo<<"Unsupported effort type"<<endl;
    }

    // TODO: handle risc

    //kdDebug()<<k_funcinfo<<m_name<<"="<<m_duration.toString(KPTDuration::Format_Day)<<"  <<<----"<<endl;
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
    } else {
        kdDebug()<<k_funcinfo<<"Not yet implemented"<<endl;
    }
}

/*
void KPTTask::addResource(KPTResourceGroup * resource) {
}


void KPTTask::removeResource(KPTResourceGroup * resource){
   // always auto remove
}


void KPTTask::removeResource(int number){
   // always auto remove
}


void KPTTask::insertResource( unsigned int index,
			      KPTResourceGroup *resource) {
}
*/
// A new constraint means start/end times must be recalculated
void KPTTask::setConstraint(KPTNode::ConstraintType type) {
    if (m_constraint == type)
        return;
    m_constraint = type;
    calculateStartEndTime();
}

void KPTTask::calculateStartEndTime() {
    //kdDebug()<<k_funcinfo<<endl;
    setStartEndTime();
    QPtrListIterator<KPTRelation> it(m_dependChildNodes);
    for (; it.current(); ++it) {
        it.current()->child()->calculateStartEndTime(m_endTime); // adjust for all dependant children
    }
}

void KPTTask::calculateStartEndTime(const KPTDateTime &start) {
    //kdDebug()<<k_funcinfo<<endl;
    if (start > m_startTime) { //TODO: handle different constraints
        m_startTime = start;
        setEndTime();
    }

    QPtrListIterator<KPTRelation> it(m_dependChildNodes);
    for (; it.current(); ++it) {
        it.current()->child()->calculateStartEndTime(m_endTime); // adjust for all dependent children
    }
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
