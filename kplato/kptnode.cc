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
#include <qptrlist.h>

KPTNode::KPTNode() : m_nodes(), m_dependChildNodes(), m_dependParentNodes() {
    m_name="";
    m_startTime = KPTDuration();
    m_endTime = KPTDuration();
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

void KPTNode::delDependParentNode( int number, bool remove) {
    if(remove)
        m_dependParentNodes.remove(number);
    else
        m_dependParentNodes.take(number);
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

KPTEffort::KPTEffort( KPTDuration e, KPTDuration p, KPTDuration o) {
  m_expectedDuration = e;
  m_pessimisticDuration = p;
  m_optimisticDuration = o;
}

KPTEffort::~KPTEffort() {
}

const KPTEffort KPTEffort::zeroEffort( KPTDuration::zeroDuration,
                       KPTDuration::zeroDuration,
                       KPTDuration::zeroDuration );
