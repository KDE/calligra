/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org

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

#ifndef kptnode_h
#define kptnode_h

#include "defs.h"
#include "kptrelation.h"
#include "kptduration.h"

#include <qrect.h>
#include <qptrlist.h> 
#include <qstring.h> 
#include <qcanvas.h> 

#include <vector>

class KPTEffort;
class KPTProject;
class KPTTimeScale;
class KPTPertCanvas;
class KPTPertCanvasItem;
class QDomElement;


/**
 * This class represents any node in the project, a node can be a project to
 * a subproject and any task.
 * This class is basically an abstract interface to make the design more OO.
 */
class KPTNode {
    /**
     * Many KPTProject functions deal with lists of KPTNode objects. Although
     * KPTNode protected functions can be called for any KPTNode in a list
     * from a KPTNode function (data hiding is at class level) KPTProject can't
     * call the same functions unless we explicitly allow this. A friend
     * declaration is a simple solution.
     *
     * Note (Bo): When you need to use friend classes, you usually show a
     * basic design flaw. We should get rid of this.
     */
    friend class KPTProject;

public:
    enum ConstraintType { ASAP, ALAP, StartNotEarlier, FinishNotLater, MustStartOn };
    
    KPTNode(KPTNode *parent = 0);

    // Declare the class abstract
    virtual ~KPTNode() = 0;

    // The load and save methods
    virtual bool load(QDomElement &element) = 0;
    virtual void save(QDomElement &element) const = 0;
    virtual void completeLoad(KPTNode *node); // clean up relations etc.
    
    virtual bool openDialog() {return false;}
    
    // simple child node management
    // Child nodes are things like subtasks, basically a task can exists of
    // several sub-tasks. Creating a table has 4 subtasks, 1) measuring
    // 2) cutting 3) building 4) painting.
    KPTNode *getParent() const { return m_parent; }
    const QPtrList<KPTNode> &childNodeIterator() const { return m_nodes; }
    int numChildren() const { return m_nodes.count(); }
    virtual void addChildNode(KPTNode *node);
    virtual void insertChildNode(unsigned int index, KPTNode *node);
    void delChildNode(KPTNode *node, bool remove=true);
    void delChildNode(int number, bool remove=true);
    KPTNode &getChildNode(int number) { return *m_nodes.at(number); }
    const KPTNode &getChildNode(int number) const;

    // Time-dependent child-node-management.
    // list all nodes that are dependent upon this one.
    // Building a house requires the table to be finished, therefore the
    // house-building is time dependent on the table-building. So a child
    // of the table-building node is the house-building node.

    int numDependChildNodes() const { return m_dependChildNodes.count(); }
    virtual void addDependChildNode( KPTNode *node, TimingType t=START_ON_DATE,
				     TimingRelation p=FINISH_START);
    virtual void addDependChildNode( KPTNode *node, TimingType t,
				     TimingRelation p, KPTDuration lag);
    virtual void addDependChildNode( KPTRelation *relation);
    virtual void insertDependChildNode( unsigned int index, KPTNode *node,
					TimingType t=START_ON_DATE,
					TimingRelation p=FINISH_START);
    void delDependChildNode( KPTNode *node, bool remove=false);
    void delDependChildNode( KPTRelation *rel, bool remove=false);
    void delDependChildNode( int number, bool remove=false);
    KPTRelation *getDependChildNode( int number) {
	return m_dependChildNodes.at(number);
    } 

    int numDependParentNodes() const { return m_dependParentNodes.count(); }
    virtual void addDependParentNode(KPTNode *node, TimingType t=START_ON_DATE,
				     TimingRelation p=FINISH_START);
    virtual void addDependParentNode( KPTNode *node, TimingType t,
				      TimingRelation p, KPTDuration lag);
    virtual void addDependParentNode( KPTRelation *relation);
    virtual void insertDependParentNode( unsigned int index, KPTNode *node,
					 TimingType t=START_ON_DATE,
					 TimingRelation p=FINISH_START);
    void delDependParentNode( KPTNode *node, bool remove=false);
    void delDependParentNode( KPTRelation *rel, bool remove=false);
    void delDependParentNode( int number, bool remove=false);
    KPTRelation *getDependParentNode( int number) {
	return m_dependParentNodes.at(number);
    }
    
    KPTRelation *findRelation(KPTNode *node);

    // These are calculated, or set manually if node is MustStartOn
    void setStartTime(KPTDuration startTime) { m_startTime=startTime; }
    const KPTDuration &startTime() const { return m_startTime; }
    void setEndTime(KPTDuration endTime) { m_endTime=endTime; }
    const KPTDuration &endTime() const { return m_endTime; }

    // These are entered by the project man. during the project
    void setActualStartTime(KPTDuration startTime) { m_actualStartTime=startTime; }
    const KPTDuration &actualStartTime() const { return m_actualStartTime; }
    void setActualEndTime(KPTDuration endTime) { m_actualEndTime=endTime; }
    const KPTDuration &actualEndTime() const { return m_actualEndTime; }

    void setEffort(KPTEffort* e) { m_effort = e; }
    KPTEffort* effort() { return m_effort; }

     /**
     * Used for calculation of a project
     * Reimplement in project nodes.
     */
    virtual void calculate() {;}
    
     /**
     * The expected Duration is the expected time to complete a Task, Project,
     * etc. For an individual Task, this will calculate the expected duration
     * by querying the Distribution of the Task. If the Distribution is a
     * simple RiskNone, the value will equal the mode Duration, but for other
     * Distributions like RiskHigh, the value will have to be calculated. For
     * a Project or Subproject, the expected Duration is calculated by
     * PERT/CPM. 
     */
    virtual KPTDuration *getExpectedDuration() = 0;

    /**
     * Instead of using the expected duration, generate a random value using
     * the Distribution of each Task. This can be used for Monte-Carlo
     * estimation of Project duration.
     */
    virtual KPTDuration *getRandomDuration() = 0;

    /**
     * Calculate the start time, use actualStartTime() for the actually started time.
     */
    virtual KPTDuration *getStartTime() = 0;

    /**
     * Retrieve the calculated float of this node
     */
    virtual KPTDuration *getFloat() =0;

    /**
     * Calculate the delay of this node. Use the calculated startTime and the
     * setted startTime.
     */
    KPTDuration *getDelay();

    KPTDuration getEarliestStart() const { return earliestStart; }
    KPTDuration getLatestFinish() const { return latestFinish; }

    QString &name() { return m_name; }
    QString &leader() { return m_leader; }
    QString &description() { return m_description; }
    const QString &name() const { return m_name; }
    const QString &leader() const { return m_leader; }
    const QString &description() const { return m_description; }
    void setName(const QString &n) { m_name = n; }
    void setLeader(const QString &l) { m_leader = l; }
    void setDescription(const QString &d) { m_description = d; }

    void setConstraint(KPTNode::ConstraintType type) { m_constraint = type; }
    int constraint() { return m_constraint; }
    
    const KPTDuration& optimisticDuration(const KPTDuration &start);
    const KPTDuration& pessimisticDuration(const KPTDuration &start);
    const KPTDuration& expectedDuration(const KPTDuration &start);

    virtual void drawGanttBar(QCanvas*, KPTTimeScale*, int , int) {;}
    virtual void drawPert(KPTPertCanvas *view, QCanvas*, int) {;}
    virtual void drawPertRelations(QCanvas*) {;}
    
    virtual void setStartNotEarlier(KPTDuration time) { sneTime = time; }
    virtual KPTDuration &startNotEarlier() { return sneTime; }
    virtual void setFinishNotLater(KPTDuration time) { fnlTime = time; }
    virtual KPTDuration &finishNotLater() { return fnlTime; }
    virtual void setMustStartOn(KPTDuration time) { msoTime = time; }
    virtual KPTDuration &mustStartOn() { return msoTime; }
    
    virtual void showPopup();
    
	void setDrawn(bool drawn) { m_drawn = drawn; }  
	bool isDrawn() { return m_drawn; }  

	// For Gantt
    int y() { return m_y; }
    int height() { return m_h; }

	// For pert
	KPTPertCanvasItem *pertItem() { return m_pertItem; }
	void setPertItem(KPTPertCanvasItem *item) { m_pertItem = item; }
    int x();
    int width();
	
protected:
    /**
     * For pert/cpm it is useful to have a hidden start node for each
     * user-visible node. For KPTProject objects, this will be a
     * separate KPTNode object. A KPTTask is its own start node.
     * @return The start node.
     */
    virtual KPTNode* start_node(){ return this; }
    /**
     * For pert/cpm it is useful to have a hidden end node for each
     * user-visible node. For KPTProject objects, this will be a
     * separate KPTNode object. A KPTTask is its own end node.
     * @return The end node.
     */
    virtual KPTNode* end_node(){ return this; }
    /**
     * Occasionally we may want to take a start node or end node and
     * find which node it is the start or end node of.
     * @return The KPTNode object that has this as a start node
     * or end node.
     */
    KPTNode* owner_node() {
	return this == this->end_node() ? this : m_parent;
    }
    /**
     * Initialize the lists of nodes successors.list and
     * predecessors.list so that they match the time-dependencies of this
     * node. Although the lists contain nodes, refer to them as arcs
     * because we are interested in the relation between the nodes.
     */
    void initialize_arcs();
    /**
     * Set up the arcs so that pert/cpm will work.
     *
     * Precondition: initialize_arcs() has been called.
     */
    void set_up_arcs();
    /**
     * Set up values for unvisited arcs. This is a helper function for
     * pert/cpm. Inititially pert/cpm will not have looked at any
     * relations or arcsand so we have to set initial values.
     *
     * Precondition: set_up_arcs() has been called.
     */
    void set_unvisited_values();

    typedef KPTDuration KPTNode::*start_type;

    /**
     * Set values of earliest start or latest finish for start and
     * end node of this  node and all subnodes
     * KPTProject object.
     * @param time The time to set all values to.
     * @param start Either KPTNode::earliestStart or KPTNode::latestFinish.
     */
    void set_pert_values( const KPTDuration& time, start_type start );

    QPtrList<KPTNode> m_nodes;
    QPtrList<KPTRelation> m_dependChildNodes;
    QPtrList<KPTRelation> m_dependParentNodes;
    KPTNode *m_parent;
    
    // Used by load()    
    virtual void addPredesessorNode( KPTRelation *relation );
    void delPredesessorNode();
    QPtrList<KPTRelation> m_predesessorNodes;

    QString m_name;        // Name of this node
    QString m_leader;      // Person or group responsible for this node
    QString m_description; // Description of this node

    // Calculated start time, or set if MustStartOn
    KPTDuration m_startTime, m_endTime;
    
    // Both of these are entered during the project, not at the initial
    // calculation.
    KPTDuration m_actualStartTime, m_actualEndTime;

    KPTEffort* m_effort; 

    struct dependencies {
	/**
	 * An efficiently reconstructable list of successor/predecessor
	 * nodes. These are the implicit ones rather than the nodes
	 * explicitly created as KPTRelation objects.
	 */
	std::vector<KPTNode*> list;
	/**
	 * The total number of successors/predecessors. Sum of sizes of
	 * list and m_depend*Nodes.
	 */
	unsigned int number;
	/**
	 * The number of successors/predecessors not yet visited.
	 * Used internally by pert/cpm algorithm.
	 */
	unsigned int unvisited;
    } predecessors, successors;

    KPTDuration earliestStart;
    KPTDuration latestFinish;
    
    ConstraintType m_constraint;

    void calcDuration(const KPTDuration &start, const KPTDuration &effort);

    KPTDuration sneTime;
    KPTDuration fnlTime;
    KPTDuration msoTime;

	int m_y, m_h; // For Gantt
	// For Pert
	bool m_drawn;
	KPTPertCanvasItem *m_pertItem;
    
 private:
    KPTDuration m_duration;
    
#ifndef NDEBUG
public:
    virtual void printDebug(bool children, QCString indent);
#endif

};

////////////////////////////////////////////   KPTEffort   //////////////////////////////////////////////////
/** 
  * Any @ref KPTNode will store how much time it takes to complete the node
  * (typically a @ref KPTTask) in the traditional scheduling software the
  * effort which is needed to complete the node is not simply a timespan but
  * is stored as an optimistic, a pessimistic and an expected timespan.
  */
class KPTEffort {
public:
    KPTEffort ( KPTDuration e = KPTDuration(), KPTDuration p = KPTDuration(),
		KPTDuration o = KPTDuration() );
    
    KPTEffort ( double e, double p = 0, double o = 0);
    ~KPTEffort();

    const KPTDuration& optimistic() {return m_optimisticEffort;}
    const KPTDuration& pessimistic() {return m_pessimisticEffort;}
    const KPTDuration& expected() {return m_expectedEffort;}

    void set( KPTDuration e, KPTDuration p = 0, KPTDuration o = 0 );
        
    void set( int e, int p = -1, int o = -1 );
  
    bool load(QDomElement &element);
    void save(QDomElement &element) const;
    
    /**
     * No effort.
     */
    static const KPTEffort zeroEffort;
    
private:
    KPTDuration m_optimisticEffort;
    KPTDuration m_pessimisticEffort;
    KPTDuration m_expectedEffort;

#ifndef NDEBUG
public:
    void printDebug(QCString indent);
#endif

};

#endif
