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
//#include "kptmilestone.h"
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
    : KPTNode(parent), startNode( this ), endNode( this )

{
    // Set start and end nodes to have zero duration
    startNode.setEffort( const_cast<KPTEffort*>( &KPTEffort::zeroEffort ) );
    endNode.setEffort( const_cast<KPTEffort*>( &KPTEffort::zeroEffort ) );

#ifndef NDEBUG
    startNode.setName(QString("startNode"));
    endNode.setName(QString("endNode"));
#endif

    m_constraint = KPTNode::MustStartOn;
    m_startTime = KPTDateTime::currentDateTime();
    m_endTime = m_startTime;

    m_id = 0;

    m_maxNodeId = 0;
    m_maxGroupId = 0;
    m_maxResourceId = 0;

}


KPTProject::~KPTProject() {
    m_resourceGroups.setAutoDelete(true);
    m_resourceGroups.clear();
}

int KPTProject::type() const { return KPTNode::Type_Project; }

void KPTProject::calculate() {
    //kdDebug()<<k_funcinfo<<"Node="<<m_name<<" Start="<<m_startTime.toString()<<endl;
    // clear all resource appointments
    QPtrListIterator<KPTResourceGroup> git(m_resourceGroups);
    for ( ; git.current(); ++git ) {
        git.current()->clearAppointments();
    }

    initialize_arcs();
    set_up_arcs();
    set_unvisited_values();

    pert_cpm();
    if (m_constraint != KPTNode::MustStartOn) //hmmm. projects should maybe only have MustStartOn or MustFinishOn
        setStartTime(startNode.getEarliestStart());
    setEndTime(endNode.getLatestFinish());

    QPtrListIterator<KPTNode> nit(m_nodes);
    for ( ; nit.current(); ++nit ) {
        nit.current()->setStartEndTime();
    }

    QPtrListIterator<KPTNode> it(m_nodes);
    for ( ; it.current(); ++it ) {
        it.current()->requestResources();
    }

    QPtrListIterator<KPTResourceGroup> rit(m_resourceGroups);
    for ( ; rit.current(); ++rit ) {
        rit.current()->makeAppointments();
    }
}

KPTDuration *KPTProject::getExpectedDuration() {
    //kdDebug()<<k_funcinfo<<endl;
    return new KPTDuration(end_node()->getLatestFinish() - start_node()->getEarliestStart());
}

KPTDuration *KPTProject::getRandomDuration() {
    return 0L;
}


KPTDateTime *KPTProject::getStartTime() {
    if(!m_startTime.isValid()) {
        switch (m_constraint)
        {
        case KPTNode::ASAP:
            m_startTime = earliestStart;
            break;
        case KPTNode::ALAP:
            m_startTime = latestFinish - m_duration;
            break;
        case KPTNode::StartNotEarlier:
        case KPTNode::FinishNotLater:
        {
            if ( m_parent ) {
                // TODO Calculate
                return m_parent->getStartTime();
            } else {
                // hmmm, shouldn't happen to main project
                if (!m_startTime.isValid())
                    m_startTime = QDateTime::currentDateTime();
            }
        break;
        }
        case KPTNode::MustStartOn:
            // m_startTime should have been set!
            if (!m_startTime.isValid())
                m_startTime = QDateTime::currentDateTime();
        default:
            break;
        }
    }
    return new KPTDateTime(m_startTime);
}


KPTDuration *KPTProject::getFloat() {
    return 0L;
}


void KPTProject::forward_pass( std::list<KPTNode*> nodelist ) {
    //kdDebug()<<k_funcinfo<<endl;
    /* Propagate (start) value of first node in list to all nodes in project */
    /* First find the first node with no predecessors values */
    std::list<KPTNode*>::iterator curNode;
    curNode = find_if( nodelist.begin(), nodelist.end(),
		       no_unvisited( &KPTNode::predecessors ) );

    while(curNode != nodelist.end()) {
        /* At this point curNode will contain the first node from
         * which we can search: refer to node as currentNode and earliest
         * finish (or latest start) time for currentNode as duration */
        KPTNode &currentNode = **curNode;
        //kdDebug()<<k_funcinfo<<"currentNode="<<currentNode.name()<<" earliest start="<<currentNode.earliestStart.toString()<<endl;
        KPTDateTime startTime = currentNode.earliestStart;
        /* *** expected should be more general than this *** */
        /* *** we could use (say) a member function pointer *** */
        startTime += currentNode.expectedDuration(startTime);
        /* Go through arcs from currentNode, propagating values */
        for( std::vector<KPTNode*>::iterator i = currentNode.successors.list.begin(); i != currentNode.successors.list.end(); ++i ) {
            /* add new nodes if necessary */
            if( (*i)->predecessors.unvisited == (*i)->predecessors.number )
            nodelist.push_back( *i );
            /* reduce unvisited to indicate that an arc/relation has been followed */
            (*i)->predecessors.unvisited--;
            /* act if duration is later than start of arc node */
            if( startTime > (*i)->earliestStart ){
                (*i)->earliestStart = startTime;
            }
            //kdDebug()<<"Node="<<currentNode.name()<<" Successor Node="<<(*i)->name()<<" Earliest start="<<(*i)->earliestStart.toString()<<endl;
        }
        /* Only act if node is an end node here - KPTRelations
         * should not be followed for a start node */
        if( (*curNode)->owner_node()->end_node() == *curNode )
            /* Go through relations from currentNode, propagating values */
            for( QPtrListIterator<KPTRelation> i( currentNode.owner_node()->m_dependChildNodes ); i.current(); ++i ) {
                //kdDebug()<<k_funcinfo<<"child relations"<<endl;
                /* add new nodes if necessary */
                if( i.current()->child()->start_node()->predecessors.unvisited == i.current()->child()->start_node()->predecessors.number )
                    nodelist.push_back( i.current()->child()->start_node() );
                /* reduce unvisited to indicate that a relation has been followed */
                i.current()->child()->start_node()->predecessors.unvisited--;
                /* calculate u = duration (plus) lag of relation */
                KPTDateTime u = startTime;
                u += i.current()->lag();
                /* act if u is later than start of next node */
                //kdDebug()<<k_funcinfo<<"Calc time="<<u.toString()<<" childs time="<<i.current()->child()->start_node()->getEarliestStart().toString()<<endl;
                if( u > i.current()->child()->start_node()->earliestStart ) {
                    i.current()->child()->start_node()->earliestStart = u;
                }
                //kdDebug()<<"Node="<<currentNode.owner_node()->name()<<" Child node="<<i.current()->child()->start_node()->name()<<" Move earliest start="<<u.toString()<<endl;
            }
        /* Remove currentNode from list so that we don't use it again */
        nodelist.erase( curNode );
        curNode = find_if( nodelist.begin(), nodelist.end(), no_unvisited(&KPTNode::predecessors) );
    }
}


void KPTProject::backward_pass( std::list<KPTNode*> nodelist ){
    /* Propagate (start) value of first node in list to all nodes in project */
    /* First find the first node with no successor values */
    std::list<KPTNode*>::iterator curNode;
    curNode = find_if( nodelist.begin(), nodelist.end(), no_unvisited( &KPTNode::successors ) );
    while(curNode != nodelist.end()) {
        /* at this point curNode will contain the first node from
        * which we can search: refer to node as currentNode and earliest
        * finish (or latest start) time for currentNode as t */

#ifdef DEBUGPERT
        for( std::list<KPTNode*>::const_iterator k = nodelist.begin(); k != nodelist.end(); ++k ) {
            kdDebug() << (*k)->name().latin1() << " (" << (*k)->successors.unvisited << ")" << endl;
        }
        kdDebug() << endl;
#endif

        KPTNode &currentNode = **curNode;
        KPTDateTime t = currentNode.latestFinish;
#ifdef DEBUGPERT
        kdDebug() << "Node: " <<currentNode.name()<< " latestFinish="<< t.toString() << endl;
#endif
        /* *** expected should be more general than this *** */
        /* *** we could use (say) a member function pointer *** */
        t -= currentNode.expectedDuration(t);
#ifdef DEBUGPERT
        kdDebug() << "  New calculated latest finish=" << t.toString() << endl;
#endif
        /* Go through arcs from currentNode, propagating values */
        for( std::vector<KPTNode*>::iterator i = currentNode.predecessors.list.begin(); i != currentNode.predecessors.list.end(); ++i ) {
            /* add new nodes if necessary */
            if( (*i)->successors.unvisited == (*i)->successors.number )
                nodelist.push_back( *i );
            /* reduce unvisited to indicate that an arc/relation has been followed */
            (*i)->successors.unvisited--;
            /* act if t is earlier than finish of arc node */
            if( t < (*i)->latestFinish ) {
                (*i)->latestFinish = t;
                //kdDebug() << "Node: " <<(*i)->name()<< " reduce latestFinish="<< t.toString() << endl;
            }
        }
        /* Only act if node is an start node here - KPTRelations
        * should not be followed for a end node */
        if( (*curNode)->owner_node()->start_node() == *curNode )
            /* Go through relations from currentNode, propagating values */
            for( QPtrListIterator<KPTRelation> i( currentNode.owner_node()->m_dependParentNodes ); i.current(); ++i ) {
                //kdDebug()<<k_funcinfo<<"parent relations"<<endl;
                /* add new nodes if necessary */
                if( i.current()->parent()->end_node()->successors.unvisited == i.current()->parent()->end_node()->successors.number )
                    nodelist.push_back( i.current()->parent()->end_node() );
                /* reduce unvisited to indicate that a relation has been followed */
                i.current()->parent()->end_node()->successors.unvisited--;
                /* calculate u = t (minus) lag of relation */
                KPTDateTime u = t;
                u -= i.current()->lag();
                /* act if u is earlier than end of next node */
                if( u < i.current()->parent()->end_node()->latestFinish ) {
                    i.current()->parent()->end_node()->latestFinish = u;
                }
                //kdDebug() << "Node: " <<i.current()->parent()->end_node()->name()<< " reduce latestFinish="<< u.toString() << endl;
        }
        /* Remove currentNode from list so that we don't use it again */
        nodelist.erase( curNode );
        curNode = find_if( nodelist.begin(), nodelist.end(), no_unvisited( &KPTNode::successors) );
    }
}


bool KPTProject::load(QDomElement &element) {
    // Maybe TODO: Delete old stuff here

    // Load attributes (TODO: Finish with the rest of them)
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
//	    } else if (e.tagName() == "milestone") {
//		    // Load the milestone
//		    KPTMilestone *child = new KPTMilestone(this);
//		    if (child->load(e))
//		        addChildNode(child);
//		    else
//		        // TODO: Complain about this
//		        delete child;
	    } else if (e.tagName() == "startnode") {
            start_node()->load(e);
	    } else if (e.tagName() == "endnode") {
            end_node()->load(e);
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
    QDomElement me = element.ownerDocument().createElement("project");
    element.appendChild(me);

    // TODO: Save some more info
    me.setAttribute("name", m_name);
    me.setAttribute("leader", m_leader);
    me.setAttribute("description", m_description);

    me.setAttribute("project-start",startTime().toString());
    me.setAttribute("project-end",endTime().toString());
    me.setAttribute("scheduling",constraintToString());

    QDomElement e = me.ownerDocument().createElement("startnode");
    me.appendChild(e);
    startNode.save(e);
    e = me.ownerDocument().createElement("endnode");
    me.appendChild(e);
    endNode.save(e);

    // save calendars
    QPtrListIterator<KPTCalendar> calit(m_calendars);
    for (int i=1; calit.current(); ++calit) {
        if (!calit.current()->isDeleted())
            calit.current()->setId(i++);
    }
    for (calit.toFirst(); calit.current(); ++calit) {
        calit.current()->save(me);
    }
    
    // save project resources
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


void KPTProject::pert_cpm() {
    std::list<KPTNode*> nodelist;
    /* Set initial time for nodes to project start */
    KPTDateTime time( m_startTime );
    set_pert_values( time, &KPTNode::earliestStart );
    /* initialise list of nodes - start with start node of this */
    nodelist.push_back( start_node() );
    /* Now find earliest starts */
    forward_pass( nodelist );
    /* **Note that nodelist is now empty again** */
    nodelist.clear();
    /* Now set final project time to earlies start time of end node */
    time = end_node()->earliestStart;
    set_pert_values( time, &KPTNode::latestFinish );
    /* reinitialise list of nodes - start with end node of this */
    nodelist.push_back( end_node() );
    /* Finally, find latest finishes */
    backward_pass( nodelist );
}

void KPTProject::setStartTime(KPTDateTime startTime) {
    m_startTime = startTime;
    if ( m_constraint == KPTNode::MustStartOn )
        earliestStart = startTime;
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
	  return;
	}
	// in case we want to add to the main project, we make it child element
	// of the root element.
	if ( KPTNode::Type_Project == position->type() ) {
		this->addChildNode(task);
		task->setParent( this ); // tell the node about it
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
		kdDebug()<<k_funcinfo<<"Tasknot found???"<<endl;
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

	// we have to tell the parent that we want to delete one of its children
	KPTNode* parentNode = task->getParent();
	if ( parentNode ) {
		parentNode->delChildNode( task, true );
	}
	else {
		// the only nodes that do not have a parent are those that are
		// directly owned by the project
		delChildNode( task, true );
	}
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
	grandParentNode->addChildNode( task );
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
        m_nodeMap[id] = node;
        return id;
    }
    if (m_nodeMap.contains(id)) {
        m_nodeMap[id] = node;
        return id;
    }
    // problem
    return -1; // not mapped
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
    m_calendars.append(calendar);
}

#ifndef NDEBUG
void KPTProject::printDebug(bool children, QCString indent) {

    kdDebug()<<indent<<"+ Project node: "<<name()<<endl;
    indent += "!";
    kdDebug()<<indent<<" Start node: "<<endl;
    startNode.printDebug(false, indent);
    kdDebug()<<indent<<" End node: "<<endl;
    endNode.printDebug(false, indent);
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
}
#endif
