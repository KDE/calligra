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
#include <qlist.h>

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
    m_dependChildNodes.append(new KPTRelation(this, node, t, p, lag));
}

void KPTNode::insertDependChildNode( unsigned int index, KPTNode *node, TimingType t, TimingRelation p) {
    m_dependChildNodes.insert(index, new KPTRelation(this, node, t, p, KPTDuration()));

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
    m_dependChildNodes.append(new KPTRelation(node, this, t, p, lag));
}

void KPTNode::insertDependParentNode( unsigned int index, KPTNode *node, TimingType t, TimingRelation p) {
    m_dependParentNodes.insert(index,new KPTRelation(this, node, t, p, KPTDuration()));
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

KPTEffort::KPTEffort( KPTDuration e, KPTDuration p, KPTDuration o) {
  m_expectedDuration = e;
  m_pessimisticDuration = p;
  m_optimisticDuration = o;
}

KPTEffort::~KPTEffort() {
}
