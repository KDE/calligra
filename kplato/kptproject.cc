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
#include "kptmilestone.h"
#include "kptprojectdialog.h"
#include "kptduration.h"
#include "kptcanvasview.h"
#include "kptpertcanvas.h"
#include "kpttimescale.h"

#include <qdom.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qbrush.h>
#include <qcanvas.h>

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
}


KPTProject::~KPTProject() {
    m_resourceGroups.setAutoDelete(true);
    m_resourceGroups.clear();
}

int KPTProject::type() const { return TYPE; }
int KPTProject::TYPE = KPTNode::Type_Project;

void KPTProject::calculate() {
    kdDebug()<<k_funcinfo<<"Node="<<m_name<<" Start="<<m_startTime.dateTime().toString()<<endl;
    initialize_arcs();
    set_up_arcs();
    set_unvisited_values();
    
    pert_cpm();
    if (m_constraint != KPTNode::MustStartOn)
        setStartTime(startNode.getEarliestStart());
    setEndTime(endNode.getLatestFinish());
    
    QPtrListIterator<KPTNode> nit(m_nodes); 
    for ( ; nit.current(); ++nit ) {
        nit.current()->setStartTime(start_node()->getEarliestStart());
        nit.current()->setEndTime(end_node()->getLatestFinish());
    }
}

KPTDuration *KPTProject::getExpectedDuration() {
    kdDebug()<<k_funcinfo<<endl;
    KPTDuration *ed = new KPTDuration(end_node()->getLatestFinish());
    ed->subtract(start_node()->getEarliestStart());
    kdDebug()<<k_funcinfo<<"Project="<<name()<<" Duration="<<ed->dateTime().toString()<<endl;
    return ed;
}

KPTDuration *KPTProject::getRandomDuration() {
    return 0L;
}


KPTDuration *KPTProject::getStartTime() {
    if(m_startTime == KPTDuration()) {
        switch (m_constraint)
        {
        case KPTNode::ASAP:
            m_startTime.set(earliestStart);
            break;
        case KPTNode::ALAP:
            m_startTime.set(latestFinish);
            m_startTime.subtract(m_duration);
            break;
        case KPTNode::StartNotEarlier:
        case KPTNode::FinishNotLater:
        {
            if ( m_parent ) {
                // TODO Calculate
                return m_parent->getStartTime();
            } else {
                // hmmm, shouldn't happen to main project
                if ( m_startTime == KPTDuration::zeroDuration )
                    m_startTime = KPTDuration(QDateTime::currentDateTime());
            }
        break;
        }
        case KPTNode::MustStartOn:
            // m_startTime should have been set!
            if ( m_startTime == KPTDuration::zeroDuration )
                m_startTime = KPTDuration(QDateTime::currentDateTime());
        default:
            break;
        }
    }
    return new KPTDuration(m_startTime);
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
        //kdDebug()<<k_funcinfo<<"currentNode="<<currentNode.name()<<" earliest start="<<currentNode.earliestStart.dateTime().toString()<<endl;
        KPTDuration duration( currentNode.earliestStart );
        /* *** expected should be more general than this *** */
        /* *** we could use (say) a member function pointer *** */
        duration.add( currentNode.expectedDuration(duration) );
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
                //kdDebug()<<k_funcinfo<<"Node="<<(*i)->name()<<" Earliest start="<<duration.dateTime().toString()<<endl;
            }
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
                KPTDuration u( duration );
                u.add( i.current()->lag() );
                /* act if u is later than start of next node */
                //kdDebug()<<k_funcinfo<<"Calc time="<<u.dateTime().toString()<<" childs time="<<i.current()->child()->start_node()->getEarliestStart().dateTime().toString()<<endl;
                if( u > i.current()->child()->start_node()->earliestStart ) {
                    i.current()->child()->start_node()->earliestStart = u;
                    //kdDebug()<<k_funcinfo<<"Node="<<i.current()->child()->start_node()->name()<<" Move earliest start="<<u.dateTime().toString()<<endl;
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
#ifdef DEBUGPERT
        kdDebug() << "Node: " <<currentNode.name()<< " latestFinish="<< t.dateTime().toString().latin1() << endl;
#endif
        /* *** expected should be more general than this *** */
        /* *** we could use (say) a member function pointer *** */
        t.subtract( currentNode.expectedDuration(t) );
#ifdef DEBUGPERT
        kdDebug() << "  New calculated latest finish=" << t.dateTime().toString().latin1() << endl;
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
                //kdDebug() << "Node: " <<(*i)->name()<< " reduce latestFinish="<< t.dateTime().toString().latin1() << endl;
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
                KPTDuration u( t );
                u.subtract( i.current()->lag() );
                /* act if u is earlier than end of next node */
                if( u < i.current()->parent()->end_node()->latestFinish ) {
                    i.current()->parent()->end_node()->latestFinish = u;
                    //kdDebug() << "Node: " <<i.current()->parent()->end_node()->name()<< " reduce latestFinish="<< u.dateTime().toString().latin1() << endl;
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
    
    QDateTime dt( QDateTime::currentDateTime() );
    dt = dt.fromString( element.attribute("project-start", dt.toString()) );
    //kdDebug()<<k_funcinfo<<"Start="<<dt.toString()<<endl;
    setStartTime(KPTDuration(dt));
    
    // Use project-start as default    
    dt = dt.fromString( element.attribute("project-end", dt.toString()) );
    //kdDebug()<<k_funcinfo<<"End="<<dt.toString()<<endl;
    setEndTime(KPTDuration(dt));
    
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
		    // Load the milestone
		    KPTMilestone *child = new KPTMilestone();
		    if (child->load(e))
		        addChildNode(child);
		    else
		        // TODO: Complain about this
		        delete child;
	    } else if (e.tagName() == "startnode") {
            start_node()->load(e);
	    } else if (e.tagName() == "endnode") {
            end_node()->load(e);
	    } else if (e.tagName() == "predesessor") {
		    // Load the relation
		    KPTRelation *child = new KPTRelation(0, this);
    		if (!child->load(e)) {
		        // TODO: Complain about this
		        delete child;
            }
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
    
    me.setAttribute("project-start",startTime().dateTime().toString());
    me.setAttribute("project-end",endTime().dateTime().toString());

    QDomElement e = me.ownerDocument().createElement("startnode");
    me.appendChild(e);
    startNode.save(e);
    e = me.ownerDocument().createElement("endnode");
    me.appendChild(e);
    endNode.save(e);
    // Only save parent relations    
    QPtrListIterator<KPTRelation> it(m_dependParentNodes);
    for ( ; it.current(); ++it ) {
        it.current()->save(me);
    }

    for (int i=0; i<numChildren(); i++)
	// Save all children
	getChildNode(i).save(me);
}


bool KPTProject::openDialog() {
    kdDebug()<<k_funcinfo<<endl;
    KPTProjectDialog *dialog = new KPTProjectDialog(*this);
    bool ret = dialog->exec();
    delete dialog;
    return ret;
}

void KPTProject::pert_cpm() {
    std::list<KPTNode*> nodelist;
    /* Set initial time for nodes to project start */
    KPTDuration time( m_startTime );
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

void KPTProject::setStartTime(KPTDuration startTime) {
    m_startTime = startTime;
    if ( m_constraint == KPTNode::MustStartOn )
        earliestStart = startTime;
}

void KPTProject::drawGanttBar(QCanvas* canvas, KPTTimeScale *ts, int y, int h) {
    kdDebug()<<k_funcinfo<<endl;
    m_y = y;
    m_h = h;
    QBrush brush(Qt::black);
    KPTDuration *start = getStartTime();
    KPTDuration *dur = getExpectedDuration();    
    int x = ts->getX(start);
    int w = ts->getWidth(getExpectedDuration());
    QPointArray a(7);
    a.setPoint(0, x, y);
    a.setPoint(1, x, y+h);
    a.setPoint(2, x+2, y+h/2);
    a.setPoint(3, x+w-2, y+h/2);
    a.setPoint(4, x+w, y+h);
    a.setPoint(5, x+w, y);
    a.setPoint(6, x, y);
    QCanvasPolygon *r = new QCanvasPolygon( canvas );
    r->setPoints(a);
    r->setBrush( brush );
    r->setZ(50);
    r->show();
    delete start;
    delete dur;
}

void KPTProject::drawPert(KPTPertCanvas *view, QCanvas* canvas, KPTNode *parent) {
    kdDebug()<<k_funcinfo<<endl;
	
    QPtrListIterator<KPTNode> it(m_nodes); 
    for ( ; it.current(); ++it ) {
        it.current()->setDrawn(false, true);
	}
    QPtrListIterator<KPTNode> nit(m_nodes); 
    for ( ; nit.current(); ++nit ) {
	    if (nit.current()->numDependParentNodes() == 0) {
			if (!nit.current()->isDrawn()) {
    		    nit.current()->drawPert(view, canvas);
			}
		}
	}
	m_drawn = true;
}

void KPTProject::drawPertRelations(QCanvas* canvas) {
    kdDebug()<<k_funcinfo<<endl;
    
    QPtrListIterator<KPTNode> nit(m_nodes); 
    for ( ; nit.current(); ++nit ) {
        nit.current()->drawPertRelations(canvas);
	}
	m_drawn = true;
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
#endif
