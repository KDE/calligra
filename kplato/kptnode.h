/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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
#include "kptdatetime.h"
#include "kptresource.h"

#include <qrect.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qcanvas.h>

#include <vector>

class KPTProject;
class KPTAppointment;
class KPTResourceGroup;
class KPTResource;
class KPTResourceGroupRequest;

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
    enum ConstraintType { ASAP, ALAP, StartNotEarlier, FinishNotLater, MustStartOn, MustFinishOn };

    KPTNode(KPTNode *parent = 0);


    // Declare the class abstract
    virtual ~KPTNode() = 0;

    int id() { return m_id; } // unique identity

    enum NodeTypes {
	  Type_Node = 0,
	  Type_Project = 1,
	  Type_Subproject = 2,
	  Type_Task = 3,
	  Type_Milestone = 4,
	  Type_Periodic = 5,
      Type_Summarytask = 6
    };

    virtual int type() const = 0;

    virtual KPTNode *projectNode();
    
    // The load and save methods
    virtual bool load(QDomElement &element) = 0;
    virtual void save(QDomElement &element)  = 0;
    virtual void saveRelations(QDomElement &element);

    // simple child node management
    // Child nodes are things like subtasks, basically a task can exists of
    // several sub-tasks. Creating a table has 4 subtasks, 1) measuring
    // 2) cutting 3) building 4) painting.
    KPTNode *getParent() const { return m_parent; }
	void setParent( KPTNode* newParent ) { m_parent = newParent;}
    const QPtrList<KPTNode> &childNodeIterator() const { return m_nodes; }
    int numChildren() const { return m_nodes.count(); }
    virtual void addChildNode(KPTNode *node, KPTNode *after=0);
    virtual void insertChildNode(unsigned int index, KPTNode *node);
    void delChildNode(KPTNode *node, bool remove=true);
    void delChildNode(int number, bool remove=true);
    KPTNode* getChildNode(int number) { return m_nodes.at(number); }
    const KPTNode* getChildNode(int number) const;
	int findChildNode( KPTNode* node );

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
    virtual bool addDependChildNode( KPTRelation *relation);
    virtual void insertDependChildNode( unsigned int index, KPTNode *node,
					TimingType t=START_ON_DATE,
					TimingRelation p=FINISH_START);
    void delDependChildNode( KPTNode *node, bool remove=false);
    void delDependChildNode( KPTRelation *rel, bool remove=false);
    void delDependChildNode( int number, bool remove=false);
    KPTRelation *getDependChildNode( int number) {
	return m_dependChildNodes.at(number);
    }
    QPtrList<KPTRelation> &dependChildNodes() { return m_dependChildNodes; }

    /**
     * Takes the relation @rel.
     * Never deletes even when autoDelete = true.
     */
    void takeDependChildNode(KPTRelation *rel);
    
    int numDependParentNodes() const { return m_dependParentNodes.count(); }
    virtual void addDependParentNode(KPTNode *node, TimingType t=START_ON_DATE,
				     TimingRelation p=FINISH_START);
    virtual void addDependParentNode( KPTNode *node, TimingType t,
				      TimingRelation p, KPTDuration lag);
    virtual bool addDependParentNode( KPTRelation *relation);
    virtual void insertDependParentNode( unsigned int index, KPTNode *node,
					 TimingType t=START_ON_DATE,
					 TimingRelation p=FINISH_START);
    void delDependParentNode( KPTNode *node, bool remove=false);
    void delDependParentNode( KPTRelation *rel, bool remove=false);
    void delDependParentNode( int number, bool remove=false);
    KPTRelation *getDependParentNode( int number) {
	return m_dependParentNodes.at(number);
    }
    QPtrList<KPTRelation> &dependParentNodes() { return m_dependParentNodes; }
    
    /**
     * Takes the relation @rel.
     * Never deletes even when autoDelete = true.
     */
    void takeDependParentNode(KPTRelation *rel);

	bool isParentOf(KPTNode *node);
    bool isDependChildOf(KPTNode *node);

    KPTRelation *findParentRelation(KPTNode *node);
    KPTRelation *findChildRelation(KPTNode *node);
    KPTRelation *findRelation(KPTNode *node);

    void setStartTime(KPTDateTime startTime) { m_startTime=startTime; }
    /// Return the (previously) calculated start time
    const KPTDateTime &startTime() const { return m_startTime; }
    void setEndTime(KPTDateTime endTime) { m_endTime=endTime; }
    /// Return the (previously) calculated end time
    const KPTDateTime &endTime() const { return m_endTime; }

    // These are entered by the project man. during the project
    void setActualStartTime(KPTDateTime startTime) { m_actualStartTime=startTime; }
    const KPTDateTime &actualStartTime() const { return m_actualStartTime; }
    void setActualEndTime(KPTDateTime endTime) { m_actualEndTime=endTime; }
    const KPTDateTime &actualEndTime() const { return m_actualEndTime; }

    void setEffort(KPTEffort* e) { m_effort = e; }
    KPTEffort* effort() const { return m_effort; }

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
     * Retrieve the calculated float of this node
     */
    virtual KPTDuration *getFloat() =0;

    /**
     * Calculate the delay of this node. Use the calculated startTime and the
     * setted startTime.
     */
    KPTDuration *getDelay();

    /**
      * getEarliestStart() returns earliest time this node can start
      * @see earliestStart
      */
    KPTDateTime getEarliestStart() const { return earliestStart; }
    /**
      * getLatestFinish() returns latest time this node can finish
      * @see latestFinish
      */
    KPTDateTime getLatestFinish() const { return latestFinish; }

    QString &name() { return m_name; }
    QString &leader() { return m_leader; }
    QString &description() { return m_description; }
    const QString &name() const { return m_name; }
    const QString &leader() const { return m_leader; }
    const QString &description() const { return m_description; }
    void setName(const QString &n) { m_name = n; }
    void setLeader(const QString &l) { m_leader = l; }
    void setDescription(const QString &d) { m_description = d; }

    virtual void setConstraint(KPTNode::ConstraintType type) { m_constraint = type; }
    void setConstraint(QString &type);
    int constraint() const { return m_constraint; }
    QString constraintToString() const;

    virtual void setConstraintTime(QDateTime time) { m_constraintTime = time; }

    virtual KPTDateTime &constraintTime() { return m_constraintTime; }
    virtual KPTDateTime &startNotEarlier() { return m_constraintTime; }
    virtual KPTDateTime &finishNotLater() { return m_constraintTime; }
    virtual KPTDateTime &mustStartOn() { return m_constraintTime; }
    virtual KPTDateTime &mustFinishOn() { return m_constraintTime; }

    virtual KPTResourceGroupRequest *resourceRequest(KPTResourceGroup *group) const { return 0; }
    virtual void makeAppointments();

    bool resourceError() const { return m_resourceError; }

    virtual void setResourceOverbooked(bool on) { m_resourceOverbooked = on; }
    virtual bool resourceOverbooked() { return m_resourceOverbooked; }

    void setId(int id) { m_id = id; }
    virtual int mapNode(KPTNode *node);
    virtual int mapNode(int id, KPTNode *node);

    /**
     * Planned cost is the sum total of all resources and other costs
     * planned for this node.
     */
    virtual double plannedCost() { return 0; }
    /**
     * Planned cost to date is the sum of all resources and other costs
     * planned for this node up to the date @dt
     */
    virtual double plannedCost(QDateTime &/*dt*/) { return 0; }
    /**
     * Actual cost is the sum total of the reported costs actually used
     * for this node.
     */
    virtual double actualCost() { return 0; }
    virtual int plannedWork() { return 0; }
    virtual int plannedWork(QDateTime &/*dt*/) { return 0; }
    virtual int actualWork() { return 0; }

    virtual QPtrList<KPTAppointment> appointments(const KPTNode *node);

    virtual bool isDeleted() const 
        { return (!m_deleted && m_parent) ? m_parent->isDeleted() : m_deleted; }
    
    virtual void setDeleted(bool on) { m_deleted = on; }
    bool allChildrenDeleted() const;
    
    virtual void initiateCalculationLists(QPtrList<KPTNode> &startnodes, QPtrList<KPTNode> &endnodes, QPtrList<KPTNode> &milestones) = 0;
    virtual KPTDateTime calculateForward(int /*use*/) = 0;
    virtual KPTDateTime calculateBackward(int /*use*/) = 0;
    virtual KPTDateTime &scheduleForward(KPTDateTime &, int /*use*/) = 0;
    virtual KPTDateTime &scheduleBackward(KPTDateTime &, int /*use*/) = 0;

    virtual void initiateCalculation();
    virtual void resetVisited();
    void propagateEarliestStart(KPTDateTime &time);
    void propagateLatestFinish(KPTDateTime &time);
    void moveEarliestStart(KPTDateTime &time);
    void moveLatestFinish(KPTDateTime &time);
    // Reimplement this
    virtual KPTDuration summarytaskDurationForward(const KPTDateTime &time) 
        { return KPTDuration::zeroDuration; }
    // Reimplement this
    virtual KPTDateTime summarytaskEarliestStart() 
        { return KPTDateTime(); }
    // Reimplement this
    virtual KPTDuration summarytaskDurationBackward(const KPTDateTime &time) 
        { return KPTDuration::zeroDuration; }
    // Reimplement this
    virtual KPTDateTime summarytaskLatestFinish() 
        { return KPTDateTime(); }
    // Reimplement this
    virtual KPTDuration workbasedDuration(const KPTDateTime &/*time*/, const KPTDuration &/*effort*/, bool /*backward*/) { return KPTDuration::zeroDuration;}
    /**
     * Calculates and returns the duration of the node.
     * Uses the correct expected-, optimistic- or pessimistic effort
     * dependent on @param use. If the effort type is Type_Workbased,
     * the duration is calculated, else the effort is returned.
     */
    KPTDuration duration(const KPTDateTime &time, int use, bool backward);
    // Returns the (previously) calculated duration
    const KPTDuration &duration() { return m_duration; }

protected:
    QPtrList<KPTNode> m_nodes;
    QPtrList<KPTRelation> m_dependChildNodes;
    QPtrList<KPTRelation> m_dependParentNodes;
    KPTNode *m_parent;

    QString m_name;        // Name of this node
    QString m_leader;      // Person or group responsible for this node
    QString m_description; // Description of this node

    // Both of these are entered during the project, not at the initial
    // calculation.
    KPTDateTime m_actualStartTime, m_actualEndTime;

    KPTEffort* m_effort;
    
    /** earliestStart is calculated by PERT/CPM.
      * A task may be scheduled to start later because other tasks
      * scehduled in parallell takes more time to complete
      */
    KPTDateTime earliestStart;
    /** latestFinish is calculated by PERT/CPM.
      * A task may be scheduled to finish earlier because other tasks
      * scehduled in parallell takes more time to complete
      */
    KPTDateTime latestFinish;

    ConstraintType m_constraint;

    /**
      * @m_constraintTime is used if any of the constraints
      * StartNotEarlier, FinishNotLater or MustStartOn is selected
      */
    KPTDateTime m_constraintTime;

    /**  m_startTime is the calculated start time.
      *  It depends on constraints (i.e. ASAP/ALAP)
      *  It will always be later or equal to @ref earliestStart
      */
    KPTDateTime m_startTime;

    /**  m_endTime is the calculated finish time.
      *  It depends on constraints (i.e. ASAP/ALAP)
      *  It will always be earlier or equal to @ref latestFinish
      */
    KPTDateTime m_endTime;
    /**  m_duration is the calculated duration which depends on
      *  e.g. estimated effort, allocated resources and risk
      */
    KPTDuration m_duration;

    //TODO: better error indications
    bool m_resourceError;
    bool m_resourceOverbooked;

    int m_id; // unique id
    
    bool m_deleted;

    bool m_visitedForward;
    bool m_visitedBackward;
    
 private:
    void init();

#ifndef NDEBUG
public:
    virtual void printDebug(bool children, QCString indent);
#endif

};

////////////////////////////////   KPTEffort   ////////////////////////////////
/**
  * Any @ref KPTNode will store how much time it takes to complete the node
  * (typically a @ref KPTTask) in the traditional scheduling software the
  * effort which is needed to complete the node is not simply a timespan but
  * is stored as an optimistic, a pessimistic and an expected timespan.
  */
class KPTEffort {
public:
    KPTEffort ( KPTDuration e = KPTDuration::zeroDuration, KPTDuration p = KPTDuration::zeroDuration,
		KPTDuration o = KPTDuration::zeroDuration );

    KPTEffort ( double e, double p = 0, double o = 0);
    ~KPTEffort();

    enum Type { Type_WorkBased = 0,        // Changing amount of resources changes the task duration
                          Type_FixedDuration = 1     // Changing amount of resources will not change the tasks duration
     };
    Type type() const { return m_type; }
    void setType(Type type) { m_type = type; }
    void setType(QString type);
    QString typeToString() const;

    enum Use { Use_Expected=0, Use_Optimistic=1, Use_Pessimistic=2 };
    const KPTDuration& effort(int use) {
        if (use == KPTEffort::Use_Expected)
            return m_expectedEffort;
        else if (use == KPTEffort::Use_Optimistic)
            return m_optimisticEffort;
        else if (use == KPTEffort::Use_Pessimistic)
            return m_pessimisticEffort;
        
        return m_expectedEffort; // default
    }
    const KPTDuration& optimistic() const {return m_optimisticEffort;}
    const KPTDuration& pessimistic() const {return m_pessimisticEffort;}
    const KPTDuration& expected() const {return m_expectedEffort;}

    void set( KPTDuration e, KPTDuration p = KPTDuration::zeroDuration, KPTDuration o = KPTDuration::zeroDuration );
    void set( int e, int p = -1, int o = -1 );
    void set(unsigned days, unsigned hours, unsigned minutes);
    void expectedEffort(unsigned *days, unsigned *hours, unsigned *minutes);

    bool load(QDomElement &element);
    void save(QDomElement &element) const;

    /**
     * Set the optimistic duration
     * @percent should be a negativ value.
     */
    void setOptimistic(int percent) {}
    /**
     * Return the @optimistic duaration as deviation from @expected in percent.
     * This should be a negativ value.
     */
    int optimisticRatio() const { return -10; } // FIXME
    /**
     * Set the pessimistic duration
     * @percent should be a positive value.
     */
    void setPessimistic() {}
    /**
     * Return the @pessimistic duaration as the deviation from @expected in percent.
     * This should be a positive value.
     */
    int pessimisticRatio() const { return 20; }  // FIXME

    /**
     * No effort.
     */
    static const KPTEffort zeroEffort;

private:
    KPTDuration m_optimisticEffort;
    KPTDuration m_pessimisticEffort;
    KPTDuration m_expectedEffort;

    Type m_type;

#ifndef NDEBUG
public:
    void printDebug(QCString indent);
#endif

};

#endif
