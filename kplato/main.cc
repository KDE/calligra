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

#include "defs.h"
#include "kptproject.h"
#include "kpttask.h"

#include <kdebug.h>
#include <kptduration.h>

int main( int /*argc*/, char /***argv */) {
    // new project
    KPTProject *p = new KPTProject();
    p->setName("Project p");

    // new task with name Task A
    // this task does not have any duration assigned to it, that be calculated from
    // its subtasks.
    KPTTask *ta = new KPTTask();
    ta->setName("Task A");
    p->addChildNode(ta);
    kdDebug() << "Creating Task A\n";
    kdDebug() << "  Project\n";
    kdDebug() << "  +-- > Task A\n";

    // new task with name Task B
    // Effort initialized in one line
    KPTTask *tb = new KPTTask();
    tb->setName("Task B");
    p->addChildNode(tb);
    tb->setEffort( new KPTEffort(KPTDuration(2,0)));
    kdDebug() << "\n";
    kdDebug() << "Added task B with effort of 2 hours \n";
    kdDebug() << "  Project\n";
    kdDebug() << "  +-- > Task A\n";
    kdDebug() << "  +-- > Task B\n";
    kdDebug() << "          -> "<< tb->getExpectedDuration()->toString() << endl;

    // new subtask, we let it be a subtask of taskA, and name it Subtask A1
    KPTTask *ta1 = new KPTTask();
    ta1->setEffort(new KPTEffort(KPTDuration(1,0)));
    ta1->setName("Subtask A1");
    ta->addChildNode(ta1);
    kdDebug() << "\n";
    kdDebug() << "Added subtask A1 with effort 1 hour\n";
    kdDebug() << "  Project\n";
    kdDebug() << "  +-- > Task A\n";
    kdDebug() << "  |       -> "<< ta->getExpectedDuration()->toString() << " (implied)" << endl;
    kdDebug() << "  |       +-- > Task A1\n";
    kdDebug() << "  |               -> "<< ta1->getExpectedDuration()->toString() << endl;
    kdDebug() << "  +-- > Task B\n";
    kdDebug() << "          -> "<< tb->getExpectedDuration()->toString() << endl;

    // Make task B dependent on the finish of subtask A1, and make it start 1 hour 45 minutes after 
    // Subtask A1 has finished.
    tb->addDependChildNode(ta1, START_ON_DATE, FINISH_START, KPTDuration(1,45));
    kdDebug() << "\n";
    kdDebug() << "Setting Task B to start 1h45m after A finished" << endl;
    kdDebug() << "  Task A1 -- Task B" << endl;

    // How long is the project suppost to be running?
    // Task A1 takes 1 hour
    // Task A therefor also takes 1 hour (only one subtask)
    // After task A1 finishes we wait for 1 3/4 hour and then start task B
    // Task B takes 2 hours
    // Total: 4 hour 45 minutes.

    kdDebug() << "\n";
    kdDebug() << "Total running time: " << p->getExpectedDuration()->toString() << endl;
 
    KPTProject::printTree(p,QString("-> "));    
    KPTProject::pert_test();
    return 0;
}

void KPTProject::pert_test(){
    kdDebug() << "\nPERT/CPM TEST PROJECT\n";
    // Some more test stuff
    // Not a plausible project, but tests structure 
    KPTProject P, Q, R, S;
    KPTTask a, b, c, d, e, f, g;
    // Put them all in a list
    vector<KPTNode*> list;
    list.push_back( &P );
    list.push_back( &Q );
    list.push_back( &R );
    list.push_back( &S );
    list.push_back( &a );
    list.push_back( &b );
    list.push_back( &c );
    list.push_back( &d );
    list.push_back( &e );
    list.push_back( &f );
    list.push_back( &g );
    // set up
    P.setName( "P" );
    Q.setName( "Q" );
    R.setName( "R" );
    S.setName( "S" );
    P.start_node()->setName( "P start" );
    P.end_node()->setName( "P end" );
    Q.start_node()->setName( "Q start" );
    Q.end_node()->setName( "Q end" );
    R.start_node()->setName( "R start" );
    R.end_node()->setName( "R end" );
    S.start_node()->setName( "S start" );
    S.end_node()->setName( "S end" );
    a.setName( "a" );
    b.setName( "b" );
    c.setName( "c" );
    d.setName( "d" );
    e.setName( "e" );
    f.setName( "f" );
    g.setName( "g" );
    a.setEffort( new KPTEffort( KPTDuration( 6, 0 ) ) );
    b.setEffort( new KPTEffort( KPTDuration( 4, 0 ) ) );
    c.setEffort( new KPTEffort( KPTDuration( 1, 0 ) ) );
    d.setEffort( new KPTEffort( KPTDuration( 2, 0 ) ) );
    e.setEffort( new KPTEffort( KPTDuration( 3, 0 ) ) );
    f.setEffort( new KPTEffort( KPTDuration( 4, 0 ) ) );
    g.setEffort( new KPTEffort( KPTDuration( 2, 0 ) ) );
    // Put together "weird" project
    P.addChildNode( &Q );
    P.addChildNode( &a );
    P.addChildNode( &R );
    Q.addChildNode( &b );
    Q.addChildNode( &S );
    Q.addChildNode( &c );
    R.addChildNode( &d );
    R.addChildNode( &e );
    R.addChildNode( &f );
    R.addChildNode( &g );
    // Now set up KPTRelations
    S.addDependChildNode( &e, START_ON_DATE, FINISH_START, KPTDuration( 0, 30 ) );
    b.addDependChildNode( &S, START_ON_DATE, FINISH_START, KPTDuration( 0, 30 ) );
    S.addDependChildNode( &c, START_ON_DATE);
    d.addDependChildNode( &e, START_ON_DATE);
    d.addDependChildNode( &f, START_ON_DATE);
    e.addDependChildNode( &g, START_ON_DATE);
    f.addDependChildNode( &g, START_ON_DATE);
    // Now try to initialise arcs
    P.initialise_arcs();
    // Try to set up arcs
    P.set_up_arcs();
    // Try to set up values of unvisited_values
    P.set_unvisited_values();
    // Print values
    for( vector<KPTNode*>::iterator i = list.begin(); i != list.end(); ++i ){
      // print name of Node
      kdDebug() << (*i)->name() << ": expected duration: "
		<< (*i)->getExpectedDuration()->toString() << "\n";
      if( (*i)->nodeType() == 0 ){
	// list start predecessors
	kdDebug() << "  "
		  << (*i)->start_node()->predecessors.unvisited
		  << " start node predecessors:\n";
	for( vector<KPTNode*>::iterator j
	       = (*i)->start_node()->predecessors.list.begin();
	     j != (*i)->start_node()->predecessors.list.end(); ++j ){
	  kdDebug() << "   " << (*j)->name() 
		    << ": " << KPTDuration::zeroDuration.toString() << "\n"; 
	}
	for( int j = 0; j < (*i)->numDependParentNodes(); j++ ){
	  kdDebug() << "   "
		    << (*i)->getDependParentNode( j )->parent()->name() 
		    << ": "
		    << (*i)->getDependParentNode( j )->lag().toString()
		    << "\n"; 
	}
	// list start successors
	kdDebug() << "  "
		  << (*i)->start_node()->successors.unvisited
		  << " start node successors:\n";
	for( vector<KPTNode*>::iterator j
	       = (*i)->start_node()->successors.list.begin();
	     j != (*i)->start_node()->successors.list.end(); ++j ){
	  kdDebug() << "   " << (*j)->name() 
		    << ": " << KPTDuration::zeroDuration.toString() << "\n"; 
	}
	// list end predecessors
	kdDebug() << "  "
		  << (*i)->end_node()->predecessors.unvisited
		  << " end node predecessors:\n";
	for( vector<KPTNode*>::iterator j
	       = (*i)->end_node()->predecessors.list.begin();
	     j != (*i)->end_node()->predecessors.list.end(); ++j ){
	  kdDebug() << "   " << (*j)->name() 
		    << ": " << KPTDuration::zeroDuration.toString() << "\n"; 
	}
	// list end successors
	kdDebug() << "  "
		  << (*i)->end_node()->successors.unvisited
		  << " end node successors:\n";
	for( vector<KPTNode*>::iterator j
	       = (*i)->end_node()->successors.list.begin();
	     j != (*i)->end_node()->successors.list.end(); ++j ){
	  kdDebug() << "   " << (*j)->name() 
		    << ": " << KPTDuration::zeroDuration.toString() << "\n"; 
	}
	for( int j = 0; j < (*i)->numDependChildNodes(); j++ ){
	  kdDebug() << "   "
		    << (*i)->getDependChildNode( j )->child()->name() 
		    << ": "
		    << (*i)->getDependChildNode( j )->lag().toString()
		    << "\n"; 
	}
      }
      else {
		// list predecessors
	kdDebug() << "  "
		  << (*i)->start_node()->predecessors.unvisited
		  << " predecessors:\n";
	for( vector<KPTNode*>::iterator j
	       = (*i)->start_node()->predecessors.list.begin();
	     j != (*i)->start_node()->predecessors.list.end(); ++j ){
	  kdDebug() << "   " << (*j)->name() 
		    << ": " << KPTDuration::zeroDuration.toString() << "\n"; 
	}
	for( int j = 0; j < (*i)->numDependParentNodes(); j++ ){
	  kdDebug() << "   "
		    << (*i)->getDependParentNode( j )->parent()
	    ->end_node()->name() 
		    << ": "
		    << (*i)->getDependParentNode( j )->lag().toString()
		    << "\n"; 
	}
	// list successors
	kdDebug() << "  "
		  << (*i)->end_node()->successors.unvisited
		  << " successors:\n";
	for( vector<KPTNode*>::iterator j
	       = (*i)->end_node()->successors.list.begin();
	     j != (*i)->end_node()->successors.list.end(); ++j ){
	  kdDebug() << "   " << (*j)->name() 
		    << ": " << KPTDuration::zeroDuration.toString() << "\n"; 
	}
	for( int j = 0; j < (*i)->numDependChildNodes(); j++ ){
	  kdDebug() << "   "
		    << (*i)->getDependChildNode( j )->child()
	    ->start_node()->name() 
		    << ": "
		    << (*i)->getDependChildNode( j )->lag().toString()
		    << "\n"; 
	}
      }
    }
    kdDebug() << "SETUP FINISHED" << endl;
    /* TESTS */
    std::list<KPTNode*> nodelist;
    /* Set initial time for nodes to zero */
    /* initialise list of nodes - start with start node of this */
    nodelist.push_back( P.start_node() );
    /* Now find earliest starts */
    /* call pert/cpm */
    P.pert_cpm();
    // Print values
    for( vector<KPTNode*>::iterator i = list.begin(); i != list.end(); ++i ){
      // print name of Node
      kdDebug() << (*i)->name() << endl;
      kdDebug() << "  Earliest start: "
		<< (*i)->start_node()->earliestStart.toString().latin1()
		<< endl;
      kdDebug() << "  Latest finish: "
		<< (*i)->end_node()->latestFinish.toString().latin1() << endl;
    }
    /* Check output */
    kdDebug() << "Now check critical path makes some sense" << endl;
    for( vector<KPTNode*>::iterator i = list.begin(); i != list.end(); ++i ){
      // print name of Node
      KPTDuration duration( (*i)->end_node()->latestFinish );
      duration.subtract( (*i)->start_node()->earliestStart );
      if( (*i)->start_node() == *i ) {
	kdDebug() << (*i)->name() << ": expected duration = "
		  << (*i)->getExpectedDuration()->toString()
		  << ( duration == *(*i)->getExpectedDuration() ?
		       " (critical)" : " (non-critical)" )
		  << endl;
      }
      else {
	kdDebug() << (*i)->name() << endl;
      }
    }

    KPTProject::printTree(&P,QString("-> "));
}

void KPTProject::printTree(KPTNode *top, QString prepend) {
    kdDebug() << prepend << top->name() << endl;
    kdDebug() << prepend << "+- Duration: " << top->getExpectedDuration()->toString() << endl;

    for(unsigned int i=top->numChildren();i>0;i--) { 
        KPTNode *node = top->getChildNode(i-1);
        if(node->numChildren() != 0) {
            printTree(node, prepend+"  ");
        } else {
            QString dep=QString("");
            kdDebug() << prepend << "  " << node->name() << endl;
            for(unsigned int parentNodes=node->numDependParentNodes(); parentNodes>0;parentNodes--) {
                dep += node->getDependParentNode(parentNodes-1)->parent()->name();
                if(parentNodes > 1)  dep += ", ";
            }
            if(node->numDependParentNodes() > 0)
                kdDebug() << prepend << "  +- depends on: " << dep << endl;
            if(node->effort() && node->effort()->expected() > KPTDuration::zeroDuration)
                kdDebug() << prepend << "  +- effort: " << node->effort()->expected().toString() << endl;
            kdDebug() << prepend << "  +- duration: " << node->getExpectedDuration()->toString() << endl;
        }
    }
}

