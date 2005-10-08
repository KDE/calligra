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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef kptnode_h
#define kptnode_h

#include "kptrelation.h"
#include "kptduration.h"
#include "kptdatetime.h"

#include <qrect.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qcanvas.h>

#include <vector>

class QDomElement;

namespace KPlato
{

class KPTProject;
class KPTAppointment;
class KPTResourceGroup;
class KPTResource;
class KPTResourceGroupRequest;
class KPTEffort;
class KPTWBSDefinition;

/**
 * This class represents any node in the project, a node can be a project or
 * a subproject or any task.
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
    enum ConstraintType { ASAP, ALAP, MustStartOn, MustFinishOn, StartNotEarlier, FinishNotLater, FixedInterval };

    KPTNode(KPTNode *parent = 0);
    KPTNode(KPTNode &node, KPTNode *parent = 0);


    // Declare the class abstract
    virtual ~KPTNode() = 0;

    bool setId(QString id);
    QString id() const { return m_id; } // unique identity
    
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

    /**
     * Returns a pointer to the project node (main- or sub-project)
     * Returns 0 if no project exists.
     */
    virtual KPTNode *projectNode();
    
    // The load and save methods
    virtual bool load(QDomElement &element) = 0;
    virtual void save(QDomElement &element)  = 0;
    /// Save my and my childrens relations.
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
    /// Adds relation to both this node and @node
    virtual void addDependChildNode( KPTNode *node, KPTRelation::Type p=KPTRelation::FinishStart);
    /// Adds relation to both this node and @node
    virtual void addDependChildNode( KPTNode *node, KPTRelation::Type p, KPTDuration lag);
    /// Adds relation only to this node
    virtual bool addDependChildNode( KPTRelation *relation);
    /// Inserts relation to this node at index @index and appends relation to @node
    virtual void insertDependChildNode( unsigned int index, KPTNode *node, KPTRelation::Type p=KPTRelation::FinishStart);
    void delDependChildNode( KPTNode *node, bool remove=false);
    void delDependChildNode( KPTRelation *rel, bool remove=false);
    void delDependChildNode( int number, bool remove=false);
    KPTRelation *getDependChildNode( int number) {
	return m_dependChildNodes.at(number);
    }
    QPtrList<KPTRelation> &dependChildNodes() { return m_dependChildNodes; }

    /**
     * Takes the relation rel from this node only.
     * Never deletes even when autoDelete = true.
     */
    void takeDependChildNode(KPTRelation *rel);
    
    int numDependParentNodes() const { return m_dependParentNodes.count(); }
    /// Adds relation to both this node and node
    virtual void addDependParentNode(KPTNode *node, KPTRelation::Type p=KPTRelation::FinishStart);
    /// Adds relation to both this node and node
    virtual void addDependParentNode( KPTNode *node, KPTRelation::Type p, KPTDuration lag);
    /// Adds relation only to this node
    virtual bool addDependParentNode( KPTRelation *relation);
    /// Inserts relation to this node at index and appends relation to node
    virtual void insertDependParentNode( unsigned int index, KPTNode *node, KPTRelation::Type p=KPTRelation::FinishStart);
    void delDependParentNode( KPTNode *node, bool remove=false);
    void delDependParentNode( KPTRelation *rel, bool remove=false);
    void delDependParentNode( int number, bool remove=false);
    KPTRelation *getDependParentNode( int number) {
	return m_dependParentNodes.at(number);
    }
    QPtrList<KPTRelation> &dependParentNodes() { return m_dependParentNodes; }
    
    /**
     * Takes the relation rel from this node only.
     * Never deletes even when autoDelete = true.
     */
    void takeDependParentNode(KPTRelation *rel);

	bool isParentOf(KPTNode *node);
    bool isDependChildOf(KPTNode *node);

    KPTRelation *findParentRelation(KPTNode *node);
    KPTRelation *findChildRelation(KPTNode *node);
    KPTRelation *findRelation(KPTNode *node);

    void setStartTime(KPTDateTime startTime);
    /// Return the scheduled start time
    const KPTDateTime &startTime() const { return m_startTime; }
    const QDate &startDate() const { return m_dateOnlyStartDate; }
    void setEndTime(KPTDateTime endTime);
    /// Return the scheduled end time
    const KPTDateTime &endTime() const { return m_endTime; }
    const QDate &endDate() const { return m_dateOnlyEndDate; }

    void setEffort(KPTEffort* e) { m_effort = e; }
    KPTEffort* effort() const { return m_effort; }

    /**
     * Returns the (previously) calculated duration.
     */
    virtual KPTDuration *getExpectedDuration() = 0;

    /**
     * Instead of using the expected duration, generate a random value using
     * the Distribution of each Task. This can be used for Monte-Carlo
     * estimation of Project duration.
     */
    virtual KPTDuration *getRandomDuration() = 0;

    /**
     * Calculate the delay of this node. 
     * It is the difference between the actual startTime and scheduled startTime.
     */
    KPTDuration *getDelay();

    /**
      * getEarliestStart() returns earliest time this node can start
      * given the constraints of the network.
      * @see earliestStart
      */
    KPTDateTime getEarliestStart() const { return earliestStart; }
    /**
      * setEarliestStart() sets earliest time this node can start
      * @see earliestStart
      */
    void setEarliestStart(const KPTDateTime &dt) { earliestStart = dt; }
    /**
      * getLatestFinish() returns latest time this node can finish
      * @see latestFinish
      */
    KPTDateTime getLatestFinish() const { return latestFinish; }
    /**
      * setLatestFinish() sets latest time this node can finish
      * given the constraints of the network.
      * @see latestFinish
      */
    void setLatestFinish(const KPTDateTime &dt) { latestFinish = dt; }

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

    virtual void setConstraintStartTime(QDateTime time) { m_constraintStartTime = time; }
    virtual void setConstraintEndTime(QDateTime time) { m_constraintEndTime = time; }

    virtual KPTDateTime &constraintStartTime() { return m_constraintStartTime; }
    virtual KPTDateTime &constraintEndTime() { return m_constraintEndTime; }
    virtual KPTDateTime &startNotEarlier() { return m_constraintStartTime; }
    virtual KPTDateTime &finishNotLater() { return m_constraintEndTime; }
    virtual KPTDateTime &mustStartOn() { return m_constraintStartTime; }
    virtual KPTDateTime &mustFinishOn() { return m_constraintEndTime; }

    virtual KPTResourceGroupRequest *resourceRequest(KPTResourceGroup */*group*/) const { return 0; }
    virtual void makeAppointments();

    /// EffortType == Effort, but no resource is requested
    bool resourceError() const { return m_resourceError; }
    /// The assigned resource is overbooked
    virtual bool resourceOverbooked() const { return m_resourceOverbooked; }
    /// Calculates if the assigned resource is overbooked 
    /// within the duration of this node
    virtual void calcResourceOverbooked();
    /// The requested resource is not available
    bool resourceNotAvailable() const { return m_resourceNotAvailable; }
    /// The task cannot be scheduled to fullfill all the constraints
    virtual bool schedulingError() const { return m_schedulingError; }
    /// The node has not been scheduled
    bool notScheduled() const { return m_notScheduled; }
    
    /// Returns the total planned effort for this task (or subtasks) 
    virtual KPTDuration plannedEffort() { return KPTDuration::zeroDuration; }
    /// Returns the total planned effort for this task (or subtasks) on date
    virtual KPTDuration plannedEffort(const QDate &) { return KPTDuration::zeroDuration; }
    /// Returns the planned effort up to and including date
    virtual KPTDuration plannedEffortTo(const QDate &) { return KPTDuration::zeroDuration; }
    
    /// Returns the total actual effort for this task (or subtasks) 
    virtual KPTDuration actualEffort() { return KPTDuration::zeroDuration; }
    /// Returns the total actual effort for this task (or subtasks) on date
    virtual KPTDuration actualEffort(const QDate &/*date*/) { return KPTDuration::zeroDuration; }
    /// Returns the total actual effort for this task (or subtasks) up to and including date
    virtual KPTDuration actualEffortTo(const QDate &/*date*/) { return KPTDuration::zeroDuration; }
    
    /**
     * Planned cost is the sum total of all resources and other costs
     * planned for this node.
     */
    virtual double plannedCost() { return 0; }
    
    /// Planned cost on date
    virtual double plannedCost(const QDate &/*date*/) { return 0; }
    /**
     * Planned cost from start of activity up to and including date
     * is the sum of all resource costs and other costs planned for this node.
     */
    virtual double plannedCostTo(const QDate &/*date*/) { return 0; }
    /**
     * Actual cost is the sum total of the reported costs actually used
     * for this node.
     */
    virtual double actualCost() { return 0; }
    /// Actual cost on date
    virtual double actualCost(const QDate &/*date*/) { return 0; }
    /// Actual cost up to and including date
    virtual double actualCostTo(const QDate &/*date*/) { return 0; }
    
    /// Effort based performance index
    double effortPerformanceIndex(const QDate &/*date*/, bool */*error=0*/) { return 0.0; }
    /// Cost performance index
    double costPerformanceIndex(const QDate &/*date*/, bool */*error=0*/) { return 0.0; }
    
    virtual void initiateCalculationLists(QPtrList<KPTNode> &startnodes, QPtrList<KPTNode> &endnodes, QPtrList<KPTNode> &summarytasks) = 0;
    virtual KPTDateTime calculateForward(int /*use*/) = 0;
    virtual KPTDateTime calculateBackward(int /*use*/) = 0;
    virtual KPTDateTime &scheduleForward(KPTDateTime &, int /*use*/) = 0;
    virtual KPTDateTime &scheduleBackward(KPTDateTime &, int /*use*/) = 0;
    virtual void adjustSummarytask() = 0;

    virtual void initiateCalculation();
    virtual void resetVisited();
    void propagateEarliestStart(KPTDateTime &time);
    void propagateLatestFinish(KPTDateTime &time);
    void moveEarliestStart(KPTDateTime &time);
    void moveLatestFinish(KPTDateTime &time);
    // Reimplement this
    virtual KPTDuration summarytaskDurationForward(const KPTDateTime &/*time*/) 
        { return KPTDuration::zeroDuration; }
    // Reimplement this
    virtual KPTDateTime summarytaskEarliestStart() 
        { return KPTDateTime(); }
    // Reimplement this
    virtual KPTDuration summarytaskDurationBackward(const KPTDateTime &/*time*/) 
        { return KPTDuration::zeroDuration; }
    // Reimplement this
    virtual KPTDateTime summarytaskLatestFinish() 
        { return KPTDateTime(); }
    // Returns the (previously) calculated duration
    const KPTDuration &duration() { return m_duration; }
    /**
     * Calculates and returns the duration of the node.
     * Uses the correct expected-, optimistic- or pessimistic effort
     * dependent on use.
     * @param time Where to start calculation.
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     * @param Backward If true, time specifies when the task should end.
     */
    KPTDuration duration(const KPTDateTime &time, int use, bool backward);
    // Reimplement this
    virtual KPTDuration calcDuration(const KPTDateTime &/*time*/, const KPTDuration &/*effort*/, bool /*backward*/) { return KPTDuration::zeroDuration;}

    KPTNode *siblingBefore();
    KPTNode *childBefore(KPTNode *node);
    KPTNode *siblingAfter();
    KPTNode *childAfter(KPTNode *node);
    bool moveChildUp(KPTNode *node);
    bool moveChildDown(KPTNode *node);
    
    /// Check if this node can be linked to node
    bool legalToLink(KPTNode *node);
    /// Check if node par can be linked to node child. (Reimplement)
    virtual bool legalToLink(KPTNode *, KPTNode *) { return false; }
    /// Check if this node has any dependent child nodes
    virtual bool isEndNode() const;
    /// Check if this node has any dependent parent nodes
    virtual bool isStartNode() const;
    virtual void clearProxyRelations() {}
    virtual void addParentProxyRelations(QPtrList<KPTRelation> &) {}
    virtual void addChildProxyRelations(QPtrList<KPTRelation> &) {}
    virtual void addParentProxyRelation(KPTNode *, const KPTRelation *) {}
    virtual void addChildProxyRelation(KPTNode *, const KPTRelation *) {}

    QPtrList<KPTAppointment> &appointments() { return m_appointments; }

    /// Return appointments this node have with resource
    KPTAppointment *findAppointment(KPTResource *resource);
    /// The total number of appointments
    int numAppointments() const { return m_appointments.count(); }
    /// Adds appointment to this node only (not to resource)
    virtual bool addAppointment(KPTAppointment *appointment);
    /// Adds appointment to both this node and resource
    virtual void addAppointment(KPTResource *resource, KPTDateTime &start, KPTDateTime &end, double load=100);
    
    /// removes appoinrment and deletes it (independent of setAutoDelete)
    void removeAppointment(KPTAppointment *appointment);
    /// removes appointment without deleting it (independent of setAutoDelete)
    void takeAppointment(KPTAppointment *appointment);
    
    /// Find the node with my id
    virtual KPTNode *findNode() const { return findNode(m_id); }
    /// Find the node with identity id
    virtual KPTNode *findNode(const QString &id) const
        { return (m_parent ? m_parent->findNode(id) : 0); }
    /// Remove myself from the id register
    virtual bool removeId()  { return removeId(m_id); }
    /// Remove the registered identity id
    virtual bool removeId(const QString &id)
        { return (m_parent ? m_parent->removeId(id) : false); }
    /// Insert myself into the id register
    virtual void insertId(const QString &id) { insertId(id, this); }
    /// Insert node with identity id into the register
    virtual void insertId(const QString &id, const KPTNode *node)
        { if (m_parent) m_parent->insertId(id, node); }
    
    KPTDateTime &earliestStartForward() { return m_earliestStartForward; }
    KPTDateTime &latestFinishBackward() { return m_latestFinishBackward; }
    
    /**
     * This is when work can start on this node in accordance with 
     * the calendar of allocated resources. Normally this is the same
     * as @ref startTime(), but may differ if timing constraints are set.
     */
    virtual const KPTDateTime &workStartTime() const { return m_workStartTime; }
    void setWorkStartTime(const KPTDateTime &dt) { m_workStartTime = dt; }
    
    /**
     * This is when work can finish on this node in accordance with 
     * the calendar of allocated resources. Normally this is the same
     * as @ref endTime(), but may differ if timing constraints are set.
     */
    virtual const KPTDateTime &workEndTime() const { return m_workEndTime; }
    void setWorkEndTime(const KPTDateTime &dt) { m_workEndTime = dt; }

    
    virtual bool isCritical() { return false; }
    virtual bool inCriticalPath() { return m_inCriticalPath; }
    virtual bool calcCriticalPath();
    
    /// Returns the level this node is in the hierarchy. Top node is level 0.
    virtual int level();
    /// Generate WBS
    virtual void generateWBS(int count, KPTWBSDefinition &def, QString wbs=QString());
    QString wbs() const { return m_wbs; }
    
protected:
    QPtrList<KPTNode> m_nodes;
    QPtrList<KPTRelation> m_dependChildNodes;
    QPtrList<KPTRelation> m_dependParentNodes;
    KPTNode *m_parent;

    QString m_id; // unique id
    QString m_name;        // Name of this node
    QString m_leader;      // Person or group responsible for this node
    QString m_description; // Description of this node

    KPTEffort* m_effort;
    
    /** earliestStart is calculated by PERT/CPM.
      * A task may be scheduled to start later because of constraints
      * or resource availability etc.
      */
    KPTDateTime earliestStart;
    /** latestFinish is calculated by PERT/CPM.
      * A task may be scheduled to finish earlier because of constraints
      * or resource availability etc.
      */
    KPTDateTime latestFinish;

    ConstraintType m_constraint;

    /**
      * @m_constraintTime is used if any of the constraints
      * FixedInterval, StartNotEarlier, MustStartOn or FixedInterval is selected
      */
    KPTDateTime m_constraintStartTime;
    /**
      * @m_constraintEndTime is used if any of the constraints
      * FixedInterval, FinishNotLater, MustFinishOn or FixedInterval is selected
      */
    KPTDateTime m_constraintEndTime;

    /**  m_startTime is the scheduled start time.
      *  It depends on constraints (i.e. ASAP/ALAP) and resource availability.
      *  It will always be later or equal to @ref earliestStart
      */
    KPTDateTime m_startTime;

    /**  
      *  m_endTime is the scheduled finish time.
      *  It depends on constraints (i.e. ASAP/ALAP) and resource availability.
      *  It will always be earlier or equal to @ref latestFinish
      */
    KPTDateTime m_endTime;
    
    /**  
      *  m_duration is the scheduled duration which depends on
      *  e.g. estimated effort, allocated resources and risk
      */
    KPTDuration m_duration;

    /// Set if EffortType == Effort, but no resource is requested
    bool m_resourceError;
    /// Set if the assigned resource is overbooked
    bool m_resourceOverbooked;
    /// Set if the requested resource is not available
    bool m_resourceNotAvailable;
    /// Set if the task cannot be scheduled to fullfill all the constraints
    bool m_schedulingError;
    /// Set if the node has not been scheduled
    bool m_notScheduled;
    
    bool m_visitedForward;
    bool m_visitedBackward;
    
    QDate m_dateOnlyStartDate;
    QDate m_dateOnlyEndDate;
    KPTDuration m_dateOnlyDuration;
 
    QPtrList<KPTAppointment> m_appointments;
 
    KPTDateTime m_earliestStartForward;
    KPTDuration m_durationForward;
    KPTDateTime m_latestFinishBackward;
    KPTDuration m_durationBackward;
    KPTDateTime m_workStartTime;
    KPTDateTime m_workEndTime;

    bool m_inCriticalPath;
    QString m_wbs;
    
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
    
    KPTEffort (const KPTEffort &effort);
    ~KPTEffort();

    enum Type { Type_Effort = 0,        // Changing amount of resources changes the task duration
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
    void setOptimisticRatio(int percent);
    /**
     * Return the @optimistic duaration as deviation from @expected in percent.
     * This should be a negativ value.
     */
    int optimisticRatio() const;
    /**
     * Set the pessimistic duration
     * @percent should be a positive value.
     */
    void setPessimisticRatio(int percent);
    /**
     * Return the @pessimistic duaration as the deviation from @expected in percent.
     * This should be a positive value.
     */
    int pessimisticRatio() const;

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

}  //KPlato namespace

#endif
