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

KPTProject::KPTProject() : KPTNode(), startNode( this ), endNode( this ) {
        m_nodeType=PROJECT;
    // Set start and end nodes to have zero duration
    startNode.setEffort( const_cast<KPTEffort*>
                 ( &KPTEffort::zeroEffort ) );
    endNode.setEffort( const_cast<KPTEffort*>( &KPTEffort::zeroEffort ) );
}

KPTProject::~KPTProject() {
}

KPTDuration *KPTProject::getExpectedDuration() {
    KPTDuration *ed= new KPTDuration();
    QListIterator<KPTNode> it(m_nodes); // iterator for employee list
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

void KPTProject::forward_pass( std::list<KPTNode*> nodelist ){
  /* Propagate (start) value of first node in list to all nodes in project */
  /* First find the first node with no predecessors values */
  for( list<KPTNode*>::iterator i_node
     = find_if( nodelist.begin(), nodelist.end(),
            no_unvisited( &KPTNode::predecessors ) );
       i_node != nodelist.end();
       i_node = find_if( nodelist.begin(), nodelist.end(),
             no_unvisited( &KPTNode::predecessors ) ) ){
    /* at this point i_node will contain the first node from
     * which we can search: refer to node as n and earliest
     * finish (or latest start) time for n as t */
    KPTNode &n = **i_node;
    KPTDuration t( n.earliestStart );
    /* *** expected should be more general than this *** */
    /* *** we could use (say) a member function pointer *** */
    t.add( n.effort()->expected() );
    /* Go through arcs from n, propagating values */
    for( vector<KPTNode*>::iterator i = n.successors.list.begin();
     i != n.successors.list.end(); ++i ){
      /* add new nodes if necessary */
      if( (*i)->predecessors.unvisited == (*i)->predecessors.number )
    nodelist.push_back( *i );
      /* reduce unvisited to indicate that an arc/relation has been followed */
      (*i)->predecessors.unvisited--;
      /* act if t is later than start of arc node */
      if( t > (*i)->earliestStart ){
    (*i)->earliestStart = t;
      }
    }
    /* Only act if node is an end node here - KPTRelations
     * should not be followed for a start node */
    if( (*i_node)->owner_node()->end_node() == *i_node )
      /* Go through relations from n, propagating values */
      for( QListIterator<KPTRelation> i( n.owner_node()->m_dependChildNodes );
       i.current(); ++i ){
    /* add new nodes if necessary */
    if( i.current()->child()->start_node()->predecessors.unvisited
        == i.current()->child()->start_node()->predecessors.number )
      nodelist.push_back( i.current()->child()->start_node() );
    /* reduce unvisited to indicate that a relation has been followed */
    i.current()->child()->start_node()->predecessors.unvisited--;
    /* calculate u = t (plus) lag of relation */
    KPTDuration u( t );
    u.add( i.current()->lag() );
    /* act if u is later than start of next node */
    if( u > i.current()->child()->start_node()->earliestStart ){
      i.current()->child()->start_node()->earliestStart = u;
    }
      }
    /* Remove n from list so that we don't use it again */
    nodelist.erase( i_node );
  }
}

void KPTProject::backward_pass( std::list<KPTNode*> nodelist ){
  /* Propagate (start) value of first node in list to all nodes in project */
  /* First find the first node with no successor values */
  for( list<KPTNode*>::iterator i_node
     = find_if( nodelist.begin(), nodelist.end(),
            no_unvisited( &KPTNode::successors ) );
       i_node != nodelist.end();
       i_node = find_if( nodelist.begin(), nodelist.end(),
             no_unvisited( &KPTNode::successors ) ) ){
    /* at this point i_node will contain the first node from
     * which we can search: refer to node as n and earliest
     * finish (or latest start) time for n as t */

    for( list<KPTNode*>::const_iterator k = nodelist.begin();
     k != nodelist.end(); ++k )
      {
    std::cerr << (*k)->name().latin1() << " ("
          << (*k)->successors.unvisited
          << ") :";
      }
    std::cerr << endl;

    KPTNode &n = **i_node;
    KPTDuration t( n.latestFinish );
    /* *** expected should be more general than this *** */
    /* *** we could use (say) a member function pointer *** */
    t.subtract( n.effort()->expected() );
    std::cerr << "*******" << t.toString().latin1() << endl;
    /* Go through arcs from n, propagating values */
    for( vector<KPTNode*>::iterator i = n.predecessors.list.begin();
     i != n.predecessors.list.end(); ++i ){
      /* add new nodes if necessary */
      if( (*i)->successors.unvisited == (*i)->successors.number )
    nodelist.push_back( *i );
      /* reduce unvisited to indicate that an arc/relation has been followed */
      (*i)->successors.unvisited--;
      /* act if t is earlier than finish of arc node */
      if( t < (*i)->latestFinish ){
    (*i)->latestFinish = t;
      }
    }
    /* Only act if node is an start node here - KPTRelations
     * should not be followed for a end node */
    if( (*i_node)->owner_node()->start_node() == *i_node )
      /* Go through relations from n, propagating values */
      for( QListIterator<KPTRelation> i( n.owner_node()->m_dependParentNodes );
       i.current(); ++i ){
    /* add new nodes if necessary */
    if( i.current()->parent()->end_node()->successors.unvisited
        == i.current()->parent()->end_node()->successors.number )
      nodelist.push_back( i.current()->parent()->end_node() );
    /* reduce unvisited to indicate that a relation has been followed */
    i.current()->parent()->end_node()->successors.unvisited--;
    /* calculate u = t (minus) lag of relation */
    KPTDuration u( t );
    u.subtract( i.current()->lag() );
    /* act if u is earlier than end of next node */
    if( u < i.current()->parent()->end_node()->latestFinish ){
      i.current()->parent()->end_node()->latestFinish = u;
    }
      }
    /* Remove n from list so that we don't use it again */
    nodelist.erase( i_node );
  }
}

void KPTProject::pert_cpm() {
  list<KPTNode*> nodelist;
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
