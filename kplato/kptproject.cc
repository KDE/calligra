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

#include "kptproject.h"
#include "kpttask.h"
#include "kptprojectdialog.h"
#include "kptduration.h"
#include "kptresource.h"
#include "kptdatetime.h"
#include "kptcalendar.h"

#include <qdom.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qbrush.h>
#include <qcanvas.h>
#include <qptrlist.h>

#include <kdebug.h>



KPTProject::KPTProject(KPTNode *parent)
    : KPTNode(parent)

{
    m_constraint = KPTNode::MustStartOn;
    m_startTime = KPTDateTime::currentDateTime();
    m_endTime = m_startTime;

    m_id = 0;

    m_maxNodeId = 0;
    m_maxGroupId = 0;
    m_maxResourceId = 0;
    
    m_standardWorktime = 0;
    
    m_calendars.setAutoDelete(true);

}


KPTProject::~KPTProject() {
    m_resourceGroups.setAutoDelete(true);
    m_resourceGroups.clear();
    delete m_standardWorktime;
}

int KPTProject::type() const { return KPTNode::Type_Project; }

void KPTProject::calculate(KPTEffort::Use use) {
    //kdDebug()<<k_funcinfo<<"Node="<<m_name<<" Start="<<m_startTime.toString()<<endl;
    if (isDeleted())
        return;
    // clear all resource appointments
    QPtrListIterator<KPTResourceGroup> git(m_resourceGroups);
    for ( ; git.current(); ++git ) {
        git.current()->clearAppointments();
    }
    if (type() == Type_Project) {
        initiateCalculation();
        if (m_constraint == KPTNode::MustStartOn) {
            // Calculate from start time
            propagateEarliestStart(m_startTime);
            m_endTime = calculateForward(use);
            propagateLatestFinish(m_endTime);
            calculateBackward(use);
            scheduleForward(m_startTime, use);
        } else {
            // Calculate from end time
            propagateLatestFinish(m_endTime);
            m_startTime = calculateBackward(use);
            propagateEarliestStart(m_startTime);
            calculateForward(use);
            scheduleBackward(m_endTime, use);
        }
        makeAppointments();
    } else if (type() == Type_Subproject) {
        kdWarning()<<k_funcinfo<<"Subprojects not implemented"<<endl;
    } else {
        kdError()<<k_funcinfo<<"Illegal project type: "<<type()<<endl;
    }
}

KPTDuration *KPTProject::getExpectedDuration() {
    //kdDebug()<<k_funcinfo<<endl;
    return new KPTDuration(getLatestFinish() - getEarliestStart());
}

KPTDuration *KPTProject::getRandomDuration() {
    return 0L;
}

KPTDuration *KPTProject::getFloat() {
    return 0L;
}

KPTDateTime KPTProject::calculateForward(int use) {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (isDeleted())
        return KPTDateTime();
    if (type() == KPTNode::Type_Project) {
        // Follow *parent* relations back and
        // calculate forwards following the child relations
        KPTDateTime finish;
        QPtrListIterator<KPTNode> endnodes = m_endNodes;
        for (; endnodes.current(); ++endnodes) {
            KPTDateTime time = endnodes.current()->calculateForward(use);
            if (time > finish || !finish.isValid())
                finish = time;
        }
        //kdDebug()<<k_funcinfo<<m_name<<" finish="<<finish.toString()<<endl;
        return finish;
    } else {
        //TODO: subproject
    }
    return KPTDateTime();
}

KPTDateTime KPTProject::calculateBackward(int use) {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (isDeleted())
        return KPTDateTime();
    if (type() == KPTNode::Type_Project) {
        // Follow *child* relations back and
        // calculate backwards following parent relation
        KPTDateTime start;
        QPtrListIterator<KPTNode> startnodes = m_startNodes;
        for (; startnodes.current(); ++startnodes) {
            KPTDateTime time = startnodes.current()->calculateBackward(use);
            if (time < start || !start.isValid())
                start = time;
        }
        //kdDebug()<<k_funcinfo<<m_name<<" start="<<start.toString()<<endl;
        return start;
    } else {
        //TODO: subproject
    }
    return KPTDateTime();
}

KPTDateTime &KPTProject::scheduleForward(KPTDateTime &earliest, int use) {
    if (isDeleted())
        return m_endTime;
    resetVisited();
    QPtrListIterator<KPTNode> it(m_endNodes);
    for (; it.current(); ++it) {
        it.current()->scheduleForward(earliest, use);
    }
    QPtrListIterator<KPTNode> ms(m_milestones);
    for (; ms.current(); ++ms) {
        static_cast<KPTTask*>(ms.current())->scheduleMilestone();
    }
    return m_endTime;
}

KPTDateTime &KPTProject::scheduleBackward(KPTDateTime &latest, int use) {
    if (isDeleted())
        return m_startTime;
    resetVisited();
    QPtrListIterator<KPTNode> it(m_startNodes);
    for (; it.current(); ++it) {
        it.current()->scheduleBackward(latest, use);
    }
    QPtrListIterator<KPTNode> ms(m_milestones);
    for (; ms.current(); ++ms) {
        static_cast<KPTTask*>(ms.current())->scheduleMilestone();
    }
    return m_startTime;
}

void KPTProject::initiateCalculation() {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    KPTNode::initiateCalculation();
    m_startNodes.clear();
    m_endNodes.clear();
    m_milestones.clear();
    initiateCalculationLists(m_startNodes, m_endNodes, m_milestones);
}

void KPTProject::initiateCalculationLists(QPtrList<KPTNode> &startnodes, QPtrList<KPTNode> &endnodes, QPtrList<KPTNode> &milestones) {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (type() == KPTNode::Type_Project) {
        QPtrListIterator<KPTNode> it = childNodeIterator();
        for (; it.current(); ++it) {
            it.current()->initiateCalculationLists(startnodes, endnodes, milestones);
        }
    } else {
        //TODO: subproject
    }
}

bool KPTProject::load(QDomElement &element) {
    // Maybe TODO: Delete old stuff here

    m_name = element.attribute("name");
    m_leader = element.attribute("leader");
    m_description = element.attribute("description");
    // Allow for both numeric and text
    bool ok = false;
    QString constraint = element.attribute("scheduling","0");
    m_constraint = (KPTNode::ConstraintType)constraint.toInt(&ok);
    if (!ok)
        KPTNode::setConstraint(constraint); // hmmm, why do I need KPTNode::?

    KPTDateTime dt( QDateTime::currentDateTime() );
    dt = dt.fromString( element.attribute("project-start", dt.toString()) );
    //kdDebug()<<k_funcinfo<<"Start="<<dt.toString()<<endl;
    setStartTime(dt);

    // Use project-start as default
    dt = dt.fromString( element.attribute("project-end", dt.toString()) );
    //kdDebug()<<k_funcinfo<<"End="<<dt.toString()<<endl;
    setEndTime(dt);

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
            // Load the task. Depends on resources already loaded
            KPTTask *child = new KPTTask(this);
            if (child->load(e))
                addChildNode(child);
            else
                // TODO: Complain about this
                delete child;
            } else if (e.tagName() == "relation") {
                // Load the relation
                KPTRelation *child = new KPTRelation();
                if (!child->load(e, *this)) {
                    // TODO: Complain about this
                    delete child;
                }
            } else if (e.tagName() == "resource-group") {
                // Load the resources
                KPTResourceGroup *child = new KPTResourceGroup(this);
                if (child->load(e)) {
                        addResourceGroup(child);
                } else {
                    // TODO: Complain about this
                    delete child;
                }
            } else if (e.tagName() == "appointment") {
                // Load the appointments. Resources and tasks must allready loaded
                KPTAppointment *child = new KPTAppointment();
                if (! child->load(e, *this)) {
                    // TODO: Complain about this
                    kdError()<<k_funcinfo<<"Failed to load appointment"<<endl;
                    delete child;
                }
            } else if (e.tagName() == "calendar") {
                // Load the calendar.
                KPTCalendar *child = new KPTCalendar();
                if (child->load(e)) {
                    addCalendar(child);
                } else {
                    // TODO: Complain about this
                    kdError()<<k_funcinfo<<"Failed to load calendar"<<endl;
                    delete child;
                }
            } else if (e.tagName() == "standard-worktime") {
                // Load standard worktime
                KPTStandardWorktime *child = new KPTStandardWorktime();
                if (child->load(e)) {
                    addStandardWorktime(child);
                } else {
                    kdError()<<k_funcinfo<<"Failed to load standard worktime"<<endl;
                    delete child;
                }
            }
        }
    }
    // fix calendar references
    QPtrListIterator<KPTCalendar> calit(m_calendars);
    for (; calit.current(); ++calit) {
        if (calit.current()->id() == calit.current()->parentId()) {
            kdError()<<k_funcinfo<<"Calendar want itself as parent"<<endl;
            continue;
        }
        QPtrListIterator<KPTCalendar> cals(m_calendars);
        for (; cals.current(); ++cals) {
            if (cals.current()->id() == calit.current()->parentId()) {
                if (cals.current()->hasParent(calit.current())) {
                    kdError()<<k_funcinfo<<"Avoid circular dependancy"<<endl;
                } else {
                    calit.current()->setParent(cals.current());
                }
                break;
            }
        }
    }
    return true;
}


void KPTProject::save(QDomElement &element)  {
    if (isDeleted())
        return;
    QDomElement me = element.ownerDocument().createElement("project");
    element.appendChild(me);

    me.setAttribute("name", m_name);
    me.setAttribute("leader", m_leader);
    me.setAttribute("description", m_description);

    me.setAttribute("project-start",startTime().toString());
    me.setAttribute("project-end",endTime().toString());
    me.setAttribute("scheduling",constraintToString());

    // save calendars
    QPtrListIterator<KPTCalendar> calit(m_calendars);
    for (int i=1; calit.current(); ++calit) {
        if (!calit.current()->isDeleted())
            calit.current()->setId(i++);
    }
    for (calit.toFirst(); calit.current(); ++calit) {
        calit.current()->save(me);
    }
    // save standard worktime
    if (m_standardWorktime)
        m_standardWorktime->save(me);
    
    // save project resources, must be after calendars
    m_maxGroupId = 0; m_maxResourceId = 0;  // we'll generate fresh ones
    QPtrListIterator<KPTResourceGroup> git(m_resourceGroups);
    for ( ; git.current(); ++git ) {
        git.current()->save(me);
    }

    // Only save parent relations
    QPtrListIterator<KPTRelation> it(m_dependParentNodes);
    for ( ; it.current(); ++it ) {
        it.current()->save(me);
    }

    for (int i=0; i<numChildren(); i++)
	// Save all children
	getChildNode(i)->save(me);

    // Now we can save relations assuming no tasks have relations outside the project
    QPtrListIterator<KPTNode> nodes(m_nodes);
    for ( ; nodes.current(); ++nodes ) {
	    nodes.current()->saveRelations(me);
    }

    // save appointments
    QPtrListIterator<KPTResourceGroup> rgit(m_resourceGroups);
    for ( ; rgit.current(); ++rgit ) {
        rgit.current()->saveAppointments(me);
    }

}

void KPTProject::addResourceGroup(KPTResourceGroup * group) {
    m_resourceGroups.append(group);
}


void KPTProject::removeResourceGroup(KPTResourceGroup * group){
    m_resourceGroups.remove(group);
}


void KPTProject::removeResourceGroup(int /* number */){
   // always auto remove
}


void KPTProject::insertResourceGroup( unsigned int /* index */,
			      KPTResourceGroup * /* resource */) {
}

QPtrList<KPTResourceGroup> &KPTProject::resourceGroups() {
     return m_resourceGroups;
}

void KPTProject::addTask( KPTNode* task, KPTNode* position )
{
	// we want to add a task at the given position. => the new node will
	// become next sibling right after position.
	if ( 0 == position ) {
      kdError()<<k_funcinfo<<"position=0, could not add task: "<<task->name()<<endl;
	  return;
	}
    //kdDebug()<<k_funcinfo<<"Add "<<task->name()<<" after "<<position->name()<<endl;
	// in case we want to add to the main project, we make it child element
	// of the root element.
	if ( KPTNode::Type_Project == position->type() ) {
        addSubTask(task, position);
		return;
	}
	// find the position
	// we have to tell the parent that we want to delete one of its children
	KPTNode* parentNode = position->getParent();
	if ( !parentNode ) {
		kdDebug()<<k_funcinfo<<"parent node not found???"<<endl;
		return;
	}
	int index = parentNode->findChildNode( position );
	if ( -1 == index ) {
		// ok, it does not exist
		kdDebug()<<k_funcinfo<<"Task not found???"<<endl;
		return;
	}
	parentNode->insertChildNode( index+1, task );
	task->setParent( parentNode ); // tell the node about it
}

void KPTProject::addSubTask( KPTNode* task, KPTNode* position )
{
	// we want to add a subtask to the node "position". It will become
	// position's last child.
	if ( 0 == position ) {
      kdError()<<k_funcinfo<<"No parent, can not add subtask: "<<task->name()<<endl;
	  return;
	}
	position->addChildNode(task);
	task->setParent( position ); // tell the node about it
    task->setId(mapNode(task));
}

void KPTProject::deleteTask( KPTNode* task )
{
	if ( 0 == task ) {
		// is always != 0. At least we would get the KPTProject, but you
		// never know who might change that, so better be careful
		return;
	}

	if ( KPTNode::Type_Project == task->type() ) {
		kdDebug()<<k_funcinfo<<"The root node cannot be deleted"<<endl;
		return;
	}

    task->setDeleted(true);
}

void KPTProject::indentTask( KPTNode* task )
{
	if ( 0 == task ) {
		// should always be != 0. At least we would get the KPTProject,
		// but you never know who might change that, so better be careful
		return;
	}

	if ( KPTNode::Type_Project == task->type() ) {
		kdDebug()<<k_funcinfo<<"The root node cannot be indented"<<endl;
		return;
	}

	// we have to find the parent of task to manipulate its list of children
	KPTNode* parentNode = task->getParent();
	if ( parentNode ) {
		int index = parentNode->findChildNode( task );
		if ( -1 == index ) {
			kdDebug()<<k_funcinfo<<"Tasknot found???"<<endl;
			return;
		}
		if ( 0 == index ) {
			kdDebug()<<k_funcinfo<<"Task already is first child and cannot be indented"<<endl;
			return;
		}
		KPTNode* newParent = parentNode->getChildNode( index -1);
		if ( 0 == newParent) {
			kdDebug()<<k_funcinfo<<"new parent node not found"<<endl;
			return;
		}
		parentNode->delChildNode( task, false ); // false: do not delete objekt
		newParent->addChildNode( task );
		task->setParent( newParent ); // tell the node about it
	}
}

void KPTProject::unindentTask( KPTNode* task )
{
	if ( 0 == task ) {
		// is always != 0. At least we would get the KPTProject, but you
		// never know who might change that, so better be careful
		return;
	}

	if ( KPTNode::Type_Project == task->type() ) {
		kdDebug()<<k_funcinfo<<"The root node cannot be unindented"<<endl;
		return;
	}

	// we have to find the parent of task to manipulate its list of children
	// and we need the parent's parent too
	KPTNode* parentNode = task->getParent();
	if ( !parentNode ) {
		return;
	}
	KPTNode* grandParentNode = parentNode->getParent();
	if ( !grandParentNode ) {
		kdDebug()<<k_funcinfo<<"This node already is at the top level"<<endl;
		return;
	}
	int index = parentNode->findChildNode( task );
	if ( -1 == index ) {
		kdDebug()<<k_funcinfo<<"Tasknot found???"<<endl;
		return;
	}
	parentNode->delChildNode( task, false ); // false: do not delete objekt
	grandParentNode->addChildNode( task, parentNode );
	task->setParent( grandParentNode ); // tell the node about it
}


void KPTProject::moveTaskUp( KPTNode* task )
{
	// we have to find the parent of task to manipulate its list of children
	KPTNode* parentNode = task->getParent();
	if ( parentNode ) {
		int index = parentNode->findChildNode( task );
		if ( -1 == index ) {
			kdDebug()<<k_funcinfo<<"Tasknot found???"<<endl;
			return;
		}
		if ( 0 == index ) {
			kdDebug()<<k_funcinfo<<"Task already is at top position"<<endl;
			return;
		}
		parentNode->delChildNode( task, false ); // false: do not delete objekt
		parentNode->insertChildNode( index-1, task );
	}
}


void KPTProject::moveTaskDown( KPTNode* task )
{
	// we have to find the parent of task to manipulate its list of children
	KPTNode* parentNode = task->getParent();
	if ( parentNode ) {
		int index = parentNode->findChildNode( task );
		if ( -1 == index ) {
			kdDebug()<<k_funcinfo<<"Tasknot found???"<<endl;
			return;
		}
		// let parent have 2 children: child 0 and child 1 (zero-based index)
		// then 0 is the last child that can be moved down.
		kdDebug()<<k_funcinfo<<"Current number of children: " << parentNode->numChildren() <<endl;
		if ( index + 1 >= parentNode->numChildren() ) {
			kdDebug()<<k_funcinfo<<"Task already is at bottom position"<<endl;
			return;
		}
		parentNode->delChildNode( task, false ); // false: do not delete objekt
		parentNode->insertChildNode( index+1, task );
	}
}

// TODO: find a more elegant/efficient solution to this id stuff
KPTNode *KPTProject::node(int id) {
    if (m_nodeMap.contains(id)) {
        return m_nodeMap[id];
    }
    return 0;
}

int KPTProject::mapNode(KPTNode *node) {
    m_nodeMap[++m_maxNodeId] = node;
    return m_maxNodeId;
}

int KPTProject::mapNode(int id, KPTNode *node) {
    if (id < 0) {
        return -1;
    }
    if (id > m_maxNodeId) {
        m_maxNodeId = id;
    }
    m_nodeMap[id] = node;
    return id;
}

KPTResourceGroup *KPTProject::group(int id) {
    QPtrListIterator<KPTResourceGroup> it(m_resourceGroups);
    for (; it.current(); ++it) {
        if (it.current()->id() == id)
            return it.current();
    }
    return 0;
}

KPTResource *KPTProject::resource(int id) {
    QPtrListIterator<KPTResourceGroup> it(m_resourceGroups);
    for (; it.current(); ++it) {
        QPtrListIterator<KPTResource> rit(it.current()->resources());
        for (; rit.current(); ++rit) {
            if (rit.current()->id() == id)
                return rit.current();
        }
    }
    return 0;
}

double KPTProject::plannedCost() {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    QPtrListIterator<KPTNode> it(childNodeIterator());
    for (; it.current(); ++it) {
        c += it.current()->plannedCost();
    }
    return c;
}

double KPTProject::plannedCost(QDateTime &dt) {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    QPtrListIterator<KPTNode> it(childNodeIterator());
    for (; it.current(); ++it) {
        c += it.current()->plannedCost(dt);
    }
    return c;
}

double KPTProject::actualCost() {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    QPtrListIterator<KPTNode> it(childNodeIterator());
    for (; it.current(); ++it) {
        c += it.current()->actualCost();
    }
    return c;
}

QPtrList<KPTAppointment> KPTProject::appointments(const KPTNode *node) {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrList<KPTAppointment> a;
    QPtrListIterator<KPTResourceGroup> rit(m_resourceGroups);
    for(; rit.current(); ++rit) {
        // hmmm, isn't it a better way?
        QPtrList<KPTAppointment> list = rit.current()->appointments(node);
        QPtrListIterator<KPTAppointment> it(list);
        for (; it.current(); ++it) {
            //kdDebug()<<k_funcinfo<<"Adding appointment"<<endl;
            a.append(it.current());
        }
    }
    return a;
}

void KPTProject::addCalendar(KPTCalendar *calendar) {
    //kdDebug()<<k_funcinfo<<calendar->name()<<endl;
    m_calendars.append(calendar);
}

KPTCalendar *KPTProject::calendar(int id) const {
    QPtrListIterator<KPTCalendar> it = m_calendars;
    for(; it.current(); ++it) {
        if (it.current()->id() == id)
            return it.current();
    }
    return 0;
}

void KPTProject::addStandardWorktime(KPTStandardWorktime * worktime) {
    if (m_standardWorktime != worktime) {
        delete m_standardWorktime; 
        m_standardWorktime = worktime; 
    }
}

#ifndef NDEBUG
void KPTProject::printDebug(bool children, QCString indent) {

    kdDebug()<<indent<<"+ Project node: "<<name()<<endl;
    indent += "!";
    QPtrListIterator<KPTResourceGroup> it(resourceGroups());
    for ( ; it.current(); ++it)
        it.current()->printDebug(indent);

    KPTNode::printDebug(children, indent);
}
void KPTProject::printCalendarDebug(QCString indent) {
    kdDebug()<<indent<<"-------- Calendars debug printout --------"<<endl;
    QPtrListIterator<KPTCalendar> it = m_calendars;
    for (; it.current(); ++it) {
        it.current()->printDebug(indent + "--");
        kdDebug()<<endl;
    }
    if (m_standardWorktime)
        m_standardWorktime->printDebug();
}
#endif
