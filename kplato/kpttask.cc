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
#include "kptpertcanvas.h"
#include "kptrelation.h"
#include "kptcanvasitem.h"

#include <qdom.h>
#include <qbrush.h>
#include <kdebug.h>
#include <koRect.h> //DEBUGRECT


KPTTask::KPTTask(KPTNode *parent) : KPTNode(parent), m_resource() {
    m_resource.setAutoDelete(true);

    KPTDuration d(24, 0);
    m_effort = new KPTEffort(d) ;
}


KPTTask::~KPTTask() {
    delete m_effort;
}

int KPTTask::type() {
	if ( numChildren() > 0) {
	  return KPTNode::Type_Subproject;
	}
	else if ( 0 == effort()->expected().duration() ) {
		return KPTNode::Type_Milestone;
	}
	else {
		return KPTNode::Type_Task;
	}
}



KPTDuration *KPTTask::getExpectedDuration() {
    //kdDebug()<<k_funcinfo<<endl;
    // Duration should already be calculated
    KPTDuration *ed= new KPTDuration(m_duration.dateTime());
    return ed;
}

KPTDuration *KPTTask::getRandomDuration() {
    return 0L;
}

void KPTTask::setStartTime() {
    kdDebug()<<k_funcinfo<<endl;
    if(numChildren() == 0) {
        switch (m_constraint)
        {
        case KPTNode::ASAP:
            m_startTime.set(earliestStart);
            break;
        case KPTNode::ALAP:
        {
            m_startTime.set(latestFinish);
            m_startTime.subtract(m_duration);
            break;
        }
        case KPTNode::StartNotEarlier:
            sneTime > earliestStart ? m_startTime.set(sneTime) : m_startTime.set(earliestStart);
            break;
        case KPTNode::FinishNotLater:
            m_startTime.set(fnlTime); // FIXME
            break;
        case KPTNode::MustStartOn:
        {
            KPTDuration t(msoTime.dateTime());
            t.add(m_duration);
            if (msoTime >= earliestStart && t <= latestFinish)
                m_startTime.set(msoTime);
            else {
                // TODO: conflict
                m_startTime.set(earliestStart);
            }
            break;
        }
        default:
            break;
        }
    } else {
        // summary task
        m_startTime.set(KPTDuration::zeroDuration);
        KPTDuration *time = 0;
        QPtrListIterator<KPTNode> it(m_nodes);
        for ( ; it.current(); ++it ) {
            it.current()->setStartTime();
            time = it.current()->getStartTime();
            if (m_startTime < *time || *time == KPTDuration::zeroDuration)
                m_startTime.set(*time);
        }
        delete time;
    }
}

void KPTTask::setEndTime() {
    kdDebug()<<k_funcinfo<<endl;
    if(numChildren() == 0) {
        m_endTime.set(m_startTime);
        m_endTime.add(m_duration);
    } else {
        // summary task
        m_endTime.set(KPTDuration::zeroDuration);
        KPTDuration *time = 0;
        QPtrListIterator<KPTNode> it(m_nodes);
        for ( ; it.current(); ++it ) {
            it.current()->setEndTime();
            time = it.current()->getEndTime();
            if (*time > m_endTime)
                m_endTime.set(*time);

        }
        delete time;
    }
}

KPTDuration *KPTTask::getStartTime() {
    kdDebug()<<k_funcinfo<<endl;
    KPTDuration *time = new KPTDuration();
    if(numChildren() == 0) {
        time->set(m_startTime.dateTime());
    } else {
        // summary task
        KPTDuration *start = 0;
        QPtrListIterator<KPTNode> it(m_nodes);
        for ( ; it.current(); ++it ) {
            start = it.current()->getStartTime();
            if (*start < *time || *time == KPTDuration::zeroDuration) {
                time->set(*start);
            }
            delete start;
        }
    }
    return time;
}

KPTDuration *KPTTask::getEndTime() {
    kdDebug()<<k_funcinfo<<endl;
    KPTDuration *time = new KPTDuration();
    if(numChildren() == 0) {
        time->set(m_startTime.dateTime());
    } else {
        // summary task
        KPTDuration *end;
        QPtrListIterator<KPTNode> it(m_nodes);
        for ( ; it.current(); ++it ) {
            end = it.current()->getEndTime();
            if (*end < *time || *time == KPTDuration::zeroDuration)
                time->set(*end);
            delete end;
        }
    }
    return time;
}

KPTDuration *KPTTask::getFloat() {
    return new KPTDuration;
}

const KPTDuration& KPTTask::expectedDuration(const KPTDuration &start) {
    kdDebug()<<k_funcinfo<<endl;
    calculateDuration(start);
    return m_duration;
 }

void KPTTask::calculateDuration(const KPTDuration &start) {
    kdDebug()<<k_funcinfo<<endl;

    // Here we assume that all requested resources are present
    // which means that the plan will be an ideal one.
    // The ideal plan must later be matched against available resources (elswhere)
    // and conflicts reported to user which must take action to solve them.
    int num = numWorkResources(); // We assume duration is only dependent on resources that do work
    if (num == 0) {
        // This must be reported as a conflict to user, but we assign a resource to get a decent plan
        m_resourceConflict = "No resources requested";
        num = 1;
    } else {
        m_resourceConflict="";
    }

    int sec = m_effort->expected().duration()/num; //TODO: handle workdays/holidays
    KPTDuration d = KPTDuration(sec);
    m_duration.set(d);

    // TODO: handle risc

}

KPTResourceRequest *KPTTask::resourceRequest(KPTResourceGroup *group) const {
    QPtrListIterator<KPTResourceRequest> it(m_requests);
    for (; it.current(); ++it) {
        if (it.current()->group() == group)
            return it.current(); // we assume only one request to the same group
    }
    return 0;
}

void KPTTask::clearResourceRequests() {
    m_requests.clear();
}

void KPTTask::addResourceRequest(KPTResourceGroup *group, int numResources) {
    m_requests.append(new KPTResourceRequest(group, numResources));
}

void KPTTask::addResourceRequest(KPTResourceRequest *request) {
    m_requests.append(request);
}

int KPTTask::numResources() {
    QPtrListIterator<KPTResourceRequest> it(m_requests);
    int num = 0;
    for (; it.current(); ++it) {
        //if (it.current()->isWork())
            num += it.current()->numResources();
    }
    kdDebug()<<k_funcinfo<<" num="<<num<<endl;
    return num;
}

int KPTTask::numWorkResources() {
    QPtrListIterator<KPTResourceRequest> it(m_requests);
    int num = 0;
    for (; it.current(); ++it) {
        //if (it.current()->isWork())
            num += it.current()->numResources();
    }
    kdDebug()<<k_funcinfo<<" num="<<num<<endl;
    return num;
}

void KPTTask::requestResources() const {
    kdDebug()<<k_funcinfo<<endl;
    if (m_nodes.count() == 0) {
        QPtrListIterator<KPTResourceRequest> it(m_requests);
        for (; it.current(); ++it) {
            // Tell the resource group I want resource(s)
            it.current()->group()->addNode(this);
        }
    } else {
        // summary task, so take care of my children
        QPtrListIterator<KPTNode> nit(m_nodes);
        for ( ; nit.current(); ++nit ) {
            nit.current()->requestResources();
        }
    }
}


void KPTTask::addResource(KPTResourceGroup * resource) {
}


void KPTTask::removeResource(KPTResourceGroup * /* resource */){
   // always auto remove
}


void KPTTask::removeResource(int /* number */){
   // always auto remove
}


void KPTTask::insertResource( unsigned int /* index */,
			      KPTResourceGroup * /* resource */) {
}

// A new constraint means start/end times must be recalculated
void KPTTask::setConstraint(KPTNode::ConstraintType type) {
    if (m_constraint == type)
        return;
    m_constraint = type;
    calculateStartEndTime();
}

void KPTTask::calculateStartEndTime() {
    kdDebug()<<k_funcinfo<<endl;
    setStartEndTime();
    QPtrListIterator<KPTRelation> it(m_dependChildNodes);
    for (; it.current(); ++it) {
        it.current()->child()->calculateStartEndTime(m_endTime); // adjust for all dependant children
    }
}

void KPTTask::calculateStartEndTime(const KPTDuration &start) {
    kdDebug()<<k_funcinfo<<endl;
    if (start > m_startTime) { //TODO: handle different constraints
        m_startTime.set(start);
        setEndTime();
    }

    QPtrListIterator<KPTRelation> it(m_dependChildNodes);
    for (; it.current(); ++it) {
        it.current()->child()->calculateStartEndTime(m_endTime); // adjust for all dependant children
    }
}

bool KPTTask::load(QDomElement &element) {
    // Load attributes (TODO: Handle different types of tasks, milestone, summary...)
    bool ok = false;
    m_id = mapNode(QString(element.attribute("id","-1")).toInt(&ok), this);
    m_name = element.attribute("name");
    m_leader = element.attribute("leader");
    m_description = element.attribute("description");

    earliestStart = KPTDuration(QDateTime::fromString(element.attribute("earlieststart")));
    latestFinish = KPTDuration(QDateTime::fromString(element.attribute("latestfinish")));
    m_startTime = KPTDuration(QDateTime::fromString(element.attribute("start")));
    m_endTime = KPTDuration(QDateTime::fromString(element.attribute("end")));
    m_duration = KPTDuration(QDateTime::fromString(element.attribute("duration")));

    m_constraint = (KPTNode::ConstraintType)QString(element.attribute("scheduling","0")).toInt(&ok);

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
	    } else if (e.tagName() == "resource-request") {
		    // Load the resource request
            KPTProject *p = dynamic_cast<KPTProject *>(projectNode());
            if (p == 0) {
                kdDebug()<<k_funcinfo<<"Project does not exist"<<endl;
            } else {
                KPTResourceRequest *r = new KPTResourceRequest();
                if (r->load(e, p))
                    addResourceRequest(r);
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
    me.setAttribute("scheduling",m_constraint);

    m_effort->save(me);


    QPtrListIterator<KPTResourceRequest> it(m_requests);
    for ( ; it.current(); ++it ) {
        it.current()->save(me);
    }

    for (int i=0; i<numChildren(); i++)
    	// First add the child
	    getChildNode(i)->save(me);
}

bool KPTTask::openDialog(QPtrList<KPTResourceGroup> &resourceGroups) {
    kdDebug()<<k_funcinfo<<endl;
    KPTTaskDialog *dialog = new KPTTaskDialog(*this, resourceGroups);
    bool ret = dialog->exec();
    delete dialog;
    return ret;
}


/*void KPTTask::drawPert(KPTPertCanvas *view, KPTNode *parent) {
	if (!m_drawn) {
		if ( numChildren() > 0  &&
	         numDependChildNodes() == 0 &&
		     numDependParentNodes() == 0)
		{
			int col = view->summaryColumn();
			m_pertItem = new KPTPertTaskItem(view, *this, 0, col);
			m_pertItem->show();
			m_drawn = true;
			kdDebug()<<k_funcinfo<<" drawn summary task("<<0<<","<<col<<"): "<<m_name<<endl;
			return;
		}
		if (!allParentsDrawn()) {
			return;
		}
		int col = getColumn(parent);
		int row = view->row(getRow(parent), col);
		m_pertItem = new KPTPertTaskItem(view, *this, row, col);
		m_pertItem->show();
		m_drawn = true;
		//kdDebug()<<k_funcinfo<<" draw ("<<row<<","<<col<<"): "<<m_name<<endl;
	}
	QPtrListIterator<KPTRelation> cit(m_dependChildNodes);
	for ( ; cit.current(); ++cit ) {
		cit.current()->child()->drawPert(view);
	}
}
*/
#ifndef NDEBUG
void KPTTask::printDebug(bool children, QCString indent) {
    kdDebug()<<indent<<"+ Task node: "<<name()<<" type="<<type()<<endl;
    indent += "!  ";
    kdDebug()<<indent<<"Requested work resources: "<<numWorkResources()<<endl;
    for (int i=0; i < m_requests.count(); ++i) {
        kdDebug()<<indent<<"   Name: "<< m_requests.at(i)->group()->name()<<endl;
    }
    kdDebug()<<indent<<"Resource overbooked="<<resourceOverbooked()<<endl;
    kdDebug()<<indent<<m_resourceConflict<<endl;
    kdDebug()<<indent<<endl;
    KPTNode::printDebug(children, indent);

}
#endif
