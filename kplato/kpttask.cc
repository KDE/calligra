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
    // I have no idea if this is correct...
    KPTDuration *ed= new KPTDuration();
    if(numChildren() == 0) {
        ed->set(m_effort->expected());
        // TODO: handle resources/risc
    } else {
        // Summary task
        QPtrListIterator<KPTNode> it(m_nodes);
        for ( ; it.current(); ++it ) {
            KPTNode *node = it.current();
            KPTDuration *time = node->getStartTime();
            KPTDuration *dur = node->getExpectedDuration();
            kdDebug()<<k_funcinfo<<" Child node="<<node->name()<<":"<<endl;
            kdDebug()<<k_funcinfo<<"   Nodes start: "<<time->dateTime().toString()<<":"<<endl;
            kdDebug()<<k_funcinfo<<"   Nodes duration: "<<dur->dateTime().toString()<<":"<<endl;
            kdDebug()<<k_funcinfo<<"   Current finish: "<<ed->dateTime().toString()<<":"<<endl;
            time->add(*dur);
            if ( *ed < *time ) {
                ed->set(*time);
                kdDebug()<<k_funcinfo<<"   New finish: "<<ed->dateTime().toString()<<":"<<endl;
            }
            delete time;
            delete dur;
        }
        KPTDuration *start = getStartTime();
        ed->subtract(*start);
        kdDebug()<<k_funcinfo<<"   My start: "<<start->dateTime().toString()<<":"<<endl;
        kdDebug()<<k_funcinfo<<"   Duration: "<<ed->dateTime().toString()<<":"<<endl;
        delete start;
    }
    return ed;
}

KPTDuration *KPTTask::getRandomDuration() {
    return 0L;
}

KPTDuration *KPTTask::getStartTime() {
    KPTDuration *time = new KPTDuration();
    if(numChildren() == 0) {
        switch (m_constraint)
        {
        case KPTNode::ASAP:
            time->set(earliestStart);
            break;
        case KPTNode::ALAP:
        {
            time->set(latestFinish);
            KPTDuration *d = getExpectedDuration();
            time->subtract(*d);
            delete d;
            break;
        }
        case KPTNode::StartNotEarlier:
            sneTime > earliestStart ? time->set(sneTime) : time->set(earliestStart);
            break;
        case KPTNode::FinishNotLater:
            time->set(fnlTime); // FIXME
            break;
        case KPTNode::MustStartOn:
            time->set(msoTime);
            break;
        default:
            break;
        }
    } else {
        // summary task
        KPTDuration *start;
        QPtrListIterator<KPTNode> it(m_nodes);
        for ( ; it.current(); ++it ) {
            start = it.current()->getStartTime();
            if (*start < *time || *time == KPTDuration::zeroDuration)
                time->set(*start);
            delete start;
        }
    }
    return time;
}

KPTDuration *KPTTask::getFloat() {
    return new KPTDuration;
}


void KPTTask::addResource(KPTResourceGroup * /* resource */) {
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


bool KPTTask::load(QDomElement &element) {
    // Maybe TODO: Delete old stuff here

    // Load attributes (TODO: Finish with the rest of them)
    m_name = element.attribute("name");
    m_leader = element.attribute("leader");
    m_description = element.attribute("description");

    earliestStart = KPTDuration(QDateTime::fromString(element.attribute("earlieststart")));
    latestFinish = KPTDuration(QDateTime::fromString(element.attribute("latestfinish")));

    bool ok;
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
//	    } else if (e.tagName() == "milestone") {
//		    // Load the milestone
//		    KPTMilestone *child = new KPTMilestone(this);
//		    if (child->load(e))
//		        addChildNode(child);
//		    else
//		        // TODO: Complain about this
//		        delete child;
	    } else if (e.tagName() == "terminalnode") {
		// TODO: Load the terminalnode
	    } else if (e.tagName() == "predesessor") {
		    // Load the relation
		    KPTRelation *rel = new KPTRelation(0, this);
    		if (rel->load(e)) {
                addPredesessorNode(rel);
            } else {
                kdDebug()<<k_funcinfo<<"Failed to load relation"<<endl;
		        // TODO: Complain about this
		        delete rel;
            }
	    } else if (e.tagName() == "resource") {
		// TODO: Load the resource
	    } else if (e.tagName() == "effort") {
    		//  Load the effort
            m_effort->load(e);
	    }
	}
    }

    return true;
}


void KPTTask::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("task");
    element.appendChild(me);

    // TODO: Save the rest of the information
    me.setAttribute("name", m_name);
    me.setAttribute("leader", m_leader);
    me.setAttribute("description", m_description);

    me.setAttribute("earlieststart",earliestStart.dateTime().toString());
    me.setAttribute("latestfinish",latestFinish.dateTime().toString());

    me.setAttribute("scheduling",m_constraint);

    m_effort->save(me);

    // Only save parent relations
    QPtrListIterator<KPTRelation> it(m_dependParentNodes);
    for ( ; it.current(); ++it ) {
        it.current()->save(me);
    }

    for (int i=0; i<numChildren(); i++)
    	// First add the child
	    getChildNode(i)->save(me);
}

bool KPTTask::openDialog() {
    kdDebug()<<k_funcinfo<<endl;
    KPTTaskDialog *dialog = new KPTTaskDialog(*this);
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
    kdDebug()<<indent<<"+ Task node: "<<name()<<endl;
    indent += "!  ";
    KPTDuration *time = getStartTime();
    kdDebug()<<indent<<"Calculated start: "<<time->dateTime().toString()<<endl;
    delete time;
    time = getExpectedDuration();
    kdDebug()<<indent<<"Calculated duration: "<<time->dateTime().toString()<<endl;
    kdDebug()<<indent<<endl;
    KPTNode::printDebug(children, indent);

}
#endif
