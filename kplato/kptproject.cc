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

#include <qdom.h>
#include <kdebug.h>


KPTProject::KPTProject() : KPTNode(), startNode( this ), endNode( this ) {
    // Set start and end nodes to have zero duration
    startNode.setEffort( const_cast<KPTEffort*>( &KPTEffort::zeroEffort ) );
    endNode.setEffort( const_cast<KPTEffort*>( &KPTEffort::zeroEffort ) );
}


KPTProject::~KPTProject() {
}


KPTDuration *KPTProject::getExpectedDuration() {
    KPTDuration *ed= new KPTDuration();
    QPtrListIterator<KPTNode> it(m_nodes); // iterator for employee list
    for ( ; it.current(); ++it ) {
        KPTNode *node = it.current();
        KPTDuration *childDuration = node->getExpectedDuration();
        ed->add(*childDuration);
        delete childDuration;
    }
    return ed;
}


KPTDuration *KPTProject::getRandomDuration() {
    return 0L;
}


KPTDuration *KPTProject::getStartTime() {
    return 0L;
}


KPTDuration *KPTProject::getFloat() {
    return 0L;
}


void KPTProject::forward_pass( std::list<KPTNode*> nodelist ) {
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
        KPTDuration duration( currentNode.earliestStart );
        /* *** expected should be more general than this *** */
        /* *** we could use (say) a member function pointer *** */
        duration.add( currentNode.effort()->expected() );
        /* Go through arcs from currentNode, propagating values */
        for( std::vector<KPTNode*>::iterator i = currentNode.successors.list.begin(); i != currentNode.successors.list.end(); ++i ) {
            /* add new nodes if necessary */
            if( (*i)->predecessors.unvisited == (*i)->predecessors.number )
            nodelist.push_back( *i );
            /* reduce unvisited to indicate that an arc/relation has been followed */
            (*i)->predecessors.unvisited--;
            /* act if duration is later than start of arc node */
            if( duration > (*i)->earliestStart ){
                (*i)->earliestStart = duration;
            }
        }
        /* Only act if node is an end node here - KPTRelations
         * should not be followed for a start node */
        if( (*curNode)->owner_node()->end_node() == *curNode )
            /* Go through relations from currentNode, propagating values */
            for( QPtrListIterator<KPTRelation> i( currentNode.owner_node()->m_dependChildNodes ); i.current(); ++i ) {
                /* add new nodes if necessary */
                if( i.current()->child()->start_node()->predecessors.unvisited == i.current()->child()->start_node()->predecessors.number )
                    nodelist.push_back( i.current()->child()->start_node() );
                /* reduce unvisited to indicate that a relation has been followed */
                i.current()->child()->start_node()->predecessors.unvisited--;
                /* calculate u = duration (plus) lag of relation */
                KPTDuration u( duration );
                u.add( i.current()->lag() );
                /* act if u is later than start of next node */
                if( u > i.current()->child()->start_node()->earliestStart ) {
                    i.current()->child()->start_node()->earliestStart = u;
                }
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
            kdDebug() << (*k)->name().latin1() << " (" << (*k)->successors.unvisited << ")" << endl;;
        }
        kdDebug() << endl;
#endif

        KPTNode &currentNode = **curNode;
        KPTDuration t( currentNode.latestFinish );
        /* *** expected should be more general than this *** */
        /* *** we could use (say) a member function pointer *** */
        t.subtract( currentNode.effort()->expected() );
#ifdef DEBUGPERT
        kdDebug() << "*******" << t.toString().latin1() << endl;
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
            }
        }
        /* Only act if node is an start node here - KPTRelations
        * should not be followed for a end node */
        if( (*curNode)->owner_node()->start_node() == *curNode )
            /* Go through relations from currentNode, propagating values */
            for( QPtrListIterator<KPTRelation> i( currentNode.owner_node()->m_dependParentNodes ); i.current(); ++i ) {
                /* add new nodes if necessary */
                if( i.current()->parent()->end_node()->successors.unvisited == i.current()->parent()->end_node()->successors.number )
                    nodelist.push_back( i.current()->parent()->end_node() );
                /* reduce unvisited to indicate that a relation has been followed */
                i.current()->parent()->end_node()->successors.unvisited--;
                /* calculate u = t (minus) lag of relation */
                KPTDuration u( t );
                u.subtract( i.current()->lag() );
                /* act if u is earlier than end of next node */
                if( u < i.current()->parent()->end_node()->latestFinish ) {
                    i.current()->parent()->end_node()->latestFinish = u;
                }
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

    // Load the project children
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
	if (list.item(i).isElement()) {
	    QDomElement e = list.item(i).toElement();

	    if (e.tagName() == "project") {
		// Load the subproject
		KPTProject *child = new KPTProject();
		if (child->load(e))
		    addChildNode(child);
		else
		    // TODO: Complain about this
		    delete child;
	    } else if (e.tagName() == "task") {
		// Load the task
		KPTTask *child = new KPTTask();
		if (child->load(e))
		    addChildNode(child);
		else
		    // TODO: Complain about this
		    delete child;
	    } else if (e.tagName() == "milestone") {
		// TODO: Load the milestone
	    } else if (e.tagName() == "terminalnode") {
		// TODO: Load the terminalnode
	    } else if (e.tagName() == "relation") {
		// TODO: Load the relation
	    } else if (e.tagName() == "resource") {
		// TODO: Load the resource
	    }
	}
    }

    return true;
}


void KPTProject::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("project");
    element.appendChild(me);

    // TODO: Save some more info
    me.setAttribute("name", m_name);
    me.setAttribute("leader", m_leader);
    me.setAttribute("description", m_description);

    for (int i=0; i<numChildren(); i++)
	// Save all children
	getChildNode(i).save(me);
}


void KPTProject::pert_cpm() {
    std::list<KPTNode*> nodelist;
    /* Set initial time for nodes to zero */
    KPTDuration time( KPTDuration::zeroDuration );
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
