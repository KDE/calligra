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
#include "kptnode.h"
#include "kptcanvasitem.h"

#include <qptrlist.h>
#include <qdatetime.h>
#include <qdom.h>

#include <kdebug.h>

KPTNode::KPTNode(KPTNode *parent) : m_nodes(), m_dependChildNodes(), m_dependParentNodes() {
    m_parent = parent;
    m_name="";
    m_startTime.set(KPTDuration());
    m_endTime.set(KPTDuration());
    earliestStart.set(KPTDuration());
    latestFinish.set(KPTDuration());
    m_constraint = KPTNode::ASAP;
    m_pertItem = 0;
	m_drawn = false;
    m_effort = 0;
}

KPTNode::~KPTNode() {
}

void KPTNode::delChildNode( KPTNode *node, bool remove) {
    if ( m_nodes.findRef(node) != -1 ) {
        if(remove)
            m_nodes.remove();
        else
            m_nodes.take();
    }
}

void KPTNode::delChildNode( int number, bool remove) {
    if(remove)
        m_nodes.remove(number);
    else
        m_nodes.take(number);
}

void KPTNode::insertChildNode( unsigned int index, KPTNode *node) {
    m_nodes.insert(index,node);
}

void KPTNode::addChildNode( KPTNode *node) {
    m_nodes.append(node);
}


const KPTNode &KPTNode::getChildNode(int number) const {
    // Work around missing const at() method in QPtrList
    const QPtrList<KPTNode> &nodes = m_nodes;
    return *(const_cast<QPtrList<KPTNode> &>(nodes)).at(number);
}

KPTDuration *KPTNode::getDelay() {
    /* TODO
       Calculate the delay of this node. Use the calculated startTime and the setted startTime.
    */
    return 0L;
}

void KPTNode::addDependChildNode( KPTNode *node, TimingType t, TimingRelation p) {
    addDependChildNode(node,t,p,KPTDuration());
}

void KPTNode::addDependChildNode( KPTNode *node, TimingType t, TimingRelation p, KPTDuration lag) {
    KPTRelation *relation = new KPTRelation(this, node, t, p, lag);
    m_dependChildNodes.append(relation);
    node->addDependParentNode(relation);
}

void KPTNode::insertDependChildNode( unsigned int index, KPTNode *node, TimingType t, TimingRelation p) {
    KPTRelation *relation = new KPTRelation(this, node, t, p, KPTDuration());
    m_dependChildNodes.insert(index, relation);
    node->addDependParentNode(relation);

}

void KPTNode::addDependChildNode( KPTRelation *relation) {
    if(m_dependChildNodes.findRef(relation) != -1)
        return;
    m_dependChildNodes.append(relation);
}

void KPTNode::delDependChildNode( KPTNode *node, bool remove) {
    if ( m_nodes.findRef(node) != -1 ) {
        if(remove)
            m_dependChildNodes.remove();
        else
            m_dependChildNodes.take();
    }
}

void KPTNode::delDependChildNode( KPTRelation *rel, bool remove) {
    if ( m_dependChildNodes.findRef(rel) != -1 ) {
        if(remove)
            m_dependChildNodes.remove();
        else
            m_dependChildNodes.take();
    }
}

void KPTNode::delDependChildNode( int number, bool remove) {
    if(remove)
        m_dependChildNodes.remove(number);
    else
        m_dependChildNodes.take(number);
}


void KPTNode::addDependParentNode( KPTNode *node, TimingType t, TimingRelation p) {
    addDependParentNode(node,t,p,KPTDuration());
}

void KPTNode::addDependParentNode( KPTNode *node, TimingType t, TimingRelation p, KPTDuration lag) {
    KPTRelation *relation = new KPTRelation(node, this, t, p, lag);
    m_dependParentNodes.append(relation);
    node->addDependChildNode(relation);
}

void KPTNode::insertDependParentNode( unsigned int index, KPTNode *node, TimingType t, TimingRelation p) {
    KPTRelation *relation = new KPTRelation(this, node, t, p, KPTDuration());
    m_dependParentNodes.insert(index,relation);
    node->addDependChildNode(relation);
}

void KPTNode::addDependParentNode( KPTRelation *relation) {
    if(m_dependParentNodes.findRef(relation) != -1)
        return;
    m_dependParentNodes.append(relation);
}

void KPTNode::delDependParentNode( KPTNode *node, bool remove) {
    if ( m_nodes.findRef(node) != -1 ) {
        if(remove)
            m_dependParentNodes.remove();
        else
            m_dependParentNodes.take();
    }
}

void KPTNode::delDependParentNode( KPTRelation *rel, bool remove) {
    if ( m_dependParentNodes.findRef(rel) != -1 ) {
        if(remove)
            m_dependParentNodes.remove();
        else
            m_dependParentNodes.take();
    }
}

void KPTNode::delDependParentNode( int number, bool remove) {
    if(remove)
        m_dependParentNodes.remove(number);
    else
        m_dependParentNodes.take(number);
}

void KPTNode::addPredesessorNode( KPTRelation *relation) {
    kdDebug()<<k_funcinfo<<"rel="<<relation<<endl;
    m_predesessorNodes.append(relation);
}

void KPTNode::delPredesessorNode() {
    KPTRelation *relation = m_predesessorNodes.take();
    kdDebug()<<k_funcinfo<<"rel="<<relation<<endl;
    delete relation;
}

KPTRelation *KPTNode::findRelation(KPTNode *node) {
    for (int i=0; i<numDependParentNodes(); i++) {
        KPTRelation *rel = getDependParentNode(i);
        if (rel->parent() == node)
            return rel;
    }
    for (int i=0; i<numDependChildNodes(); i++) {
        KPTRelation *rel = getDependChildNode(i);
        if (rel->child() == node)
            return rel;
    }
    return (KPTRelation *)0;
}

bool KPTNode::isChildOf(KPTNode *node) {
    //kdDebug()<<k_funcinfo<<" '"<<m_name<<"' checking against '"<<node->name()<<"'"<<endl;
    for (int i=0; i<numDependParentNodes(); i++) {
        KPTRelation *rel = getDependParentNode(i);
        if (rel->parent() == node)
            return true;
		if (rel->parent()->isChildOf(node))
		    return true;
    }
	return false;
}

int KPTNode::parentColumn() {
    //kdDebug()<<k_funcinfo<<endl;
	int col = -1;
    for (int i=0; i<numDependParentNodes(); i++) {
        KPTRelation *rel = getDependParentNode(i);
        KPTPertCanvasItem *item = rel->parent()->pertItem();
        if (item)
		    col = QMAX(col,item->column());
	}
	return col;
}

void KPTNode::initialize_arcs() {
  // Clear all lists of arcs and set unvisited to zero
  start_node()->successors.list.clear();
  start_node()->successors.unvisited = 0;
  start_node()->successors.number = 0;
  start_node()->predecessors.list.clear();
  start_node()->predecessors.unvisited = 0;
  start_node()->predecessors.number = 0;
  if( end_node() != start_node() ) {
    end_node()->successors.list.clear();
    end_node()->successors.unvisited = 0;
    end_node()->successors.number = 0;
    end_node()->predecessors.list.clear();
    end_node()->predecessors.unvisited = 0;
    end_node()->predecessors.number = 0;
  }
  // Now do the same for each subnode
  for( QPtrListIterator<KPTNode> i( childNodeIterator() ); i.current(); ++i )
      i.current()->initialize_arcs();
}

void KPTNode::set_up_arcs() {
  // Call this function for all nodes by recursive descent
  // and set up implicit arcs.
  for( QPtrListIterator<KPTNode> i( childNodeIterator() ); i.current(); ++i )
    {
      i.current()->set_up_arcs();
      // Now add implicit arcs:
      // First, i.current() cannot start until this has started.
      start_node()->successors.list.push_back( i.current()->start_node() );
      // Now add the corresponding predecessor.
      i.current()->start_node()->predecessors.list.push_back( start_node() );
      // Second, this cannot finish until i.current() has finished.
      i.current()->end_node()->successors.list.push_back( end_node() );
      // Now add the corresponding predecessor.
      end_node()->predecessors.list.push_back( i.current()->end_node() );
    }
  // Now add extra arcs from start_node to end_node if these are different.
  // This is necessary because if a subnode can act as a container then
  // it can contain nothing and then the arcs guarantee that
  // start_node has a successor and end_node a predecessor.
  if( start_node() != end_node() ) {
    start_node()->successors.list.push_back( end_node() );
    end_node()->predecessors.list.push_back( start_node() );
  }
}

void KPTNode::set_unvisited_values() {
  // Call this function for all nodes by recursive descent
  for( QPtrListIterator<KPTNode> i( childNodeIterator() ); i.current(); ++i )
    {
      i.current()->set_unvisited_values();
    }
  // set the actual values.
  start_node()->successors.unvisited
    = start_node()->successors.number
    = start_node()->successors.list.size();
  end_node()->predecessors.unvisited
    = end_node()->predecessors.number
    = end_node()->predecessors.list.size();
  start_node()->predecessors.unvisited
    = start_node()->predecessors.number
    = start_node()->predecessors.list.size() + numDependParentNodes();
  end_node()->successors.unvisited
    = end_node()->successors.number
    = end_node()->successors.list.size() + numDependChildNodes();
}

void KPTNode::set_pert_values( const KPTDuration& time,
                   start_type start ) {
  start_node()->*start = time;
  if( start_node() != end_node() )
    end_node()->*start = time;
  for( QPtrListIterator<KPTNode> i( childNodeIterator() ); i.current(); ++i )
    i.current()->set_pert_values( time, start );
}

 const KPTDuration& KPTNode::optimisticDuration(const KPTDuration &start)
 {
    m_duration.set(KPTDuration::zeroDuration);
    if (m_effort)
        calcDuration(start, m_effort->optimistic());
    return m_duration;
 }

const KPTDuration& KPTNode::pessimisticDuration(const KPTDuration &start)
 {
    m_duration.set(KPTDuration::zeroDuration);
    if (m_effort)
        calcDuration(start, m_effort->pessimistic());
    return m_duration;
 }
 const KPTDuration& KPTNode::expectedDuration(const KPTDuration &start)
 {
    m_duration.set(KPTDuration::zeroDuration);
    if (m_effort)
        calcDuration(start, m_effort->expected());
    return m_duration;
 }

void KPTNode::calcDuration( const KPTDuration &start, const KPTDuration &effort )
{
    //kdDebug()<<k_funcinfo<<endl;
    m_duration.add(effort);
}

void KPTNode::completeLoad(KPTNode *node) {
    kdDebug()<<k_funcinfo<<endl;
    // Complete relations for this node
    QPtrListIterator<KPTRelation> it(m_predesessorNodes);
    for ( ; it.current(); ++it ) {
        if (it.current()->completeLoad(node)) {
            addDependParentNode(it.current()->parent(), it.current()->timingType(), it.current()->timingRelation(), it.current()->lag());
            delPredesessorNode();
        }
    }
    // Now my children
    QPtrListIterator<KPTNode> nit(m_nodes);
    for ( ; nit.current(); ++nit ) {
        nit.current()->completeLoad(node);
    }
    
}

void KPTNode::showPopup() {
    kdDebug()<<k_funcinfo<<endl;
}

 int KPTNode::x() { 
    return m_pertItem->rect().left(); 
}

int KPTNode::width() {
    return m_pertItem->rect().width(); 
}

////////////////////////////////////   KPTEffort   ////////////////////////////////////////////

KPTEffort::KPTEffort( KPTDuration e, KPTDuration p, KPTDuration o) {
  m_expectedEffort = e;
  m_pessimisticEffort = p;
  m_optimisticEffort = o;
}

KPTEffort::~KPTEffort() {
}

const KPTEffort KPTEffort::zeroEffort( KPTDuration::zeroDuration,
                       KPTDuration::zeroDuration,
                       KPTDuration::zeroDuration );

void KPTEffort::set( KPTDuration e, KPTDuration p, KPTDuration o )
{
    m_expectedEffort.set(e);
    p == KPTDuration::zeroDuration ? m_pessimisticEffort.set(e) :  m_pessimisticEffort.set(p);
    o == KPTDuration::zeroDuration ? m_optimisticEffort.set(e) : m_optimisticEffort.set(o);
}

void KPTEffort::set( int e, int p, int o )
{
    m_expectedEffort.set(KPTDuration(e));
    p < 0 ? m_pessimisticEffort.set(KPTDuration(e)) :  m_pessimisticEffort.set(KPTDuration(p));
    o < 0 ? m_optimisticEffort.set(KPTDuration(e)) : m_optimisticEffort.set(KPTDuration(o));
    //kdDebug()<<k_funcinfo<<"   Expected: "<<m_expectedEffort.dateTime().toString()<<endl;
    //kdDebug()<<k_funcinfo<<"   Optimistic: "<<m_optimisticEffort.dateTime().toString()<<endl;
    //kdDebug()<<k_funcinfo<<"   Pessimistic: "<<m_pessimisticEffort.dateTime().toString()<<endl;
    
    kdDebug()<<k_funcinfo<<"   Expected: "<<m_expectedEffort.duration()<<" manseconds"<<endl;
}

bool KPTEffort::load(QDomElement &element) {
    m_expectedEffort = KPTDuration(QDateTime::fromString(element.attribute("expected")));
    m_optimisticEffort = KPTDuration(QDateTime::fromString(element.attribute("optimistic")));
    m_pessimisticEffort = KPTDuration(QDateTime::fromString(element.attribute("pessimistic")));
    return true;
}

void KPTEffort::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("effort");
    element.appendChild(me);
    me.setAttribute("expected", m_expectedEffort.dateTime().toString());
    me.setAttribute("optimistic", m_optimisticEffort.dateTime().toString());
    me.setAttribute("pessimistic", m_pessimisticEffort.dateTime().toString());
}

// Debugging
#ifndef NDEBUG
void KPTNode::printDebug(bool children, QCString indent) {
    QCString c[] = {"ASAP","ALAP","StartNotEarlier","FinishNotLater","MustStartOn"};
    if (m_effort) m_effort->printDebug(indent);
    QString s = "  Constraint: " + c[m_constraint];
    switch (m_constraint)
    {
        case KPTNode::StartNotEarlier:
            s += "   " + sneTime.dateTime().toString();
            break;
        case KPTNode::FinishNotLater:
            s += "   " + fnlTime.dateTime().toString();
            break;
        case KPTNode::MustStartOn:
            s += "   " + msoTime.dateTime().toString();
            break;
    }
    kdDebug()<<indent<<s<<endl;
    //kdDebug()<<indent<<"  Duration: "<<m_duration.dateTime().toString()<<endl;
    kdDebug()<<indent<<"  Duration: "<<m_duration.duration()<<QCString(" mansecs")<<endl;
    kdDebug()<<indent<<"  Start time: "<<m_startTime.dateTime().toString()<<endl;
    kdDebug()<<indent<<"  End time: " <<m_endTime.dateTime().toString()<<endl;
    kdDebug()<<indent<<"  Earliest start: "<<earliestStart.dateTime().toString()<<endl;
    kdDebug()<<indent<<"  Latest finish: " <<latestFinish.dateTime().toString()<<endl;
    //kdDebug()<<indent<<"  Parent: "<<(m_parent ? m_parent->name() : QString("None"))<<endl;
    kdDebug()<<indent<<"  Predecessors="<<start_node()->predecessors.number<<" unvisited="<<start_node()->predecessors.unvisited<<endl;
    kdDebug()<<indent<<"  Successors="<<start_node()->successors.number<<" unvisited="<<start_node()->successors.unvisited<<endl;

                
    QPtrListIterator<KPTRelation> pit(m_dependParentNodes);
    kdDebug()<<indent<<"  Dependant parents="<<pit.count()<<endl;
    if (pit.count() > 0) {
        for ( ; pit.current(); ++pit ) {
            pit.current()->printDebug(indent);
        }
    }
    
    QPtrListIterator<KPTRelation> cit(m_dependChildNodes);
    kdDebug()<<indent<<"  Dependant children="<<cit.count()<<endl;
    if (cit.count() > 0) {
        for ( ; cit.current(); ++cit ) {
            cit.current()->printDebug(indent);
        }
    }
    
    kdDebug()<<indent<<endl;
    indent += "  ";
    if (children) {
        QPtrListIterator<KPTNode> it(m_nodes); 
        for ( ; it.current(); ++it ) {
            it.current()->printDebug(true,indent);
        }
    }
        
}
#endif


#ifndef NDEBUG
void KPTEffort::printDebug(QCString indent) {
    kdDebug()<<indent<<"  Effort: "<<m_expectedEffort.dateTime().toString()<<endl;
    indent += "  ";
    kdDebug()<<indent<<"  Expected: "<<m_expectedEffort.dateTime().toString()<<endl;
    kdDebug()<<indent<<"  Optimistic: "<<m_optimisticEffort.dateTime().toString()<<endl;
    kdDebug()<<indent<<"  Pessimistic: "<<m_pessimisticEffort.dateTime().toString()<<endl;
}
#endif
