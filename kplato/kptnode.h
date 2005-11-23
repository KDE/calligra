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

#ifndef KPTNODE_H
#define KPTNODE_H

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

class Account;
class Project;
class Appointment;
class ResourceGroup;
class Resource;
class ResourceGroupRequest;
class Effort;
class WBSDefinition;
class EffortCostMap;

/**
 * This class represents any node in the project, a node can be a project or
 * a subproject or any task.
 * This class is basically an abstract interface to make the design more OO.
 */
class Node {
    /**
     * Many Project functions deal with lists of Node objects. Although
     * Node protected functions can be called for any Node in a list
     * from a Node function (data hiding is at class level) Project can't
     * call the same functions unless we explicitly allow this. A friend
     * declaration is a simple solution.
     *
     * Note (Bo): When you need to use friend classes, you usually show a
     * basic design flaw. We should get rid of this.
     */
    friend class Project;

public:
    enum ConstraintType { ASAP, ALAP, MustStartOn, MustFinishOn, StartNotEarlier, FinishNotLater, FixedInterval };

    Node(Node *parent = 0);
    Node(Node &node, Node *parent = 0);


    // Declare the class abstract
    virtual ~Node() = 0;

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
    virtual Node *projectNode();
    
    // The load and save methods
    virtual bool load(QDomElement &element) { return true; }
    virtual bool load(QDomElement &element, Project &project) { return true; }
    virtual void save(QDomElement &element)  = 0;
    /// Save my and my childrens relations.
    virtual void saveRelations(QDomElement &element);

    // simple child node management
    // Child nodes are things like subtasks, basically a task can exists of
    // several sub-tasks. Creating a table has 4 subtasks, 1) measuring
    // 2) cutting 3) building 4) painting.
    Node *getParent() const { return m_parent; }
	void setParent( Node* newParent ) { m_parent = newParent;}
    const QPtrList<Node> &childNodeIterator() const { return m_nodes; }
    int numChildren() const { return m_nodes.count(); }
    virtual void addChildNode(Node *node, Node *after=0);
    virtual void insertChildNode(unsigned int index, Node *node);
    void delChildNode(Node *node, bool remove=true);
    void delChildNode(int number, bool remove=true);
    Node* getChildNode(int number) { return m_nodes.at(number); }
    const Node* getChildNode(int number) const;
	int findChildNode( Node* node );

    // Time-dependent child-node-management.
    // list all nodes that are dependent upon this one.
    // Building a house requires the table to be finished, therefore the
    // house-building is time dependent on the table-building. So a child
    // of the table-building node is the house-building node.

    int numDependChildNodes() const { return m_dependChildNodes.count(); }
    /// Adds relation to both this node and @node
    virtual void addDependChildNode( Node *node, Relation::Type p=Relation::FinishStart);
    /// Adds relation to both this node and @node
    virtual void addDependChildNode( Node *node, Relation::Type p, Duration lag);
    /// Adds relation only to this node
    virtual bool addDependChildNode( Relation *relation);
    /// Inserts relation to this node at index @index and appends relation to @node
    virtual void insertDependChildNode( unsigned int index, Node *node, Relation::Type p=Relation::FinishStart);
    void delDependChildNode( Node *node, bool remove=false);
    void delDependChildNode( Relation *rel, bool remove=false);
    void delDependChildNode( int number, bool remove=false);
    Relation *getDependChildNode( int number) {
	return m_dependChildNodes.at(number);
    }
    QPtrList<Relation> &dependChildNodes() { return m_dependChildNodes; }

    /**
     * Takes the relation rel from this node only.
     * Never deletes even when autoDelete = true.
     */
    void takeDependChildNode(Relation *rel);
    
    int numDependParentNodes() const { return m_dependParentNodes.count(); }
    /// Adds relation to both this node and node
    virtual void addDependParentNode(Node *node, Relation::Type p=Relation::FinishStart);
    /// Adds relation to both this node and node
    virtual void addDependParentNode( Node *node, Relation::Type p, Duration lag);
    /// Adds relation only to this node
    virtual bool addDependParentNode( Relation *relation);
    /// Inserts relation to this node at index and appends relation to node
    virtual void insertDependParentNode( unsigned int index, Node *node, Relation::Type p=Relation::FinishStart);
    void delDependParentNode( Node *node, bool remove=false);
    void delDependParentNode( Relation *rel, bool remove=false);
    void delDependParentNode( int number, bool remove=false);
    Relation *getDependParentNode( int number) {
	return m_dependParentNodes.at(number);
    }
    QPtrList<Relation> &dependParentNodes() { return m_dependParentNodes; }
    
    /**
     * Takes the relation rel from this node only.
     * Never deletes even when autoDelete = true.
     */
    void takeDependParentNode(Relation *rel);

	bool isParentOf(Node *node);
    bool isDependChildOf(Node *node);

    Relation *findParentRelation(Node *node);
    Relation *findChildRelation(Node *node);
    Relation *findRelation(Node *node);

    void setStartTime(DateTime startTime);
    /// Return the scheduled start time
    const DateTime &startTime() const { return m_startTime; }
    const QDate &startDate() const { return m_dateOnlyStartDate; }
    void setEndTime(DateTime endTime);
    /// Return the scheduled end time
    const DateTime &endTime() const { return m_endTime; }
    const QDate &endDate() const { return m_dateOnlyEndDate; }

    void setEffort(Effort* e) { m_effort = e; }
    Effort* effort() const { return m_effort; }

    /**
     * Returns the (previously) calculated duration.
     */
    virtual Duration *getExpectedDuration() = 0;

    /**
     * Instead of using the expected duration, generate a random value using
     * the Distribution of each Task. This can be used for Monte-Carlo
     * estimation of Project duration.
     */
    virtual Duration *getRandomDuration() = 0;

    /**
     * Calculate the delay of this node. 
     * It is the difference between the actual startTime and scheduled startTime.
     */
    Duration *getDelay();

    /**
      * getEarliestStart() returns earliest time this node can start
      * given the constraints of the network.
      * @see earliestStart
      */
    DateTime getEarliestStart() const { return earliestStart; }
    /**
      * setEarliestStart() sets earliest time this node can start
      * @see earliestStart
      */
    void setEarliestStart(const DateTime &dt) { earliestStart = dt; }
    /**
      * getLatestFinish() returns latest time this node can finish
      * @see latestFinish
      */
    DateTime getLatestFinish() const { return latestFinish; }
    /**
      * setLatestFinish() sets latest time this node can finish
      * given the constraints of the network.
      * @see latestFinish
      */
    void setLatestFinish(const DateTime &dt) { latestFinish = dt; }

    QString &name() { return m_name; }
    QString &leader() { return m_leader; }
    QString &description() { return m_description; }
    const QString &name() const { return m_name; }
    const QString &leader() const { return m_leader; }
    const QString &description() const { return m_description; }
    void setName(const QString &n) { m_name = n; }
    void setLeader(const QString &l) { m_leader = l; }
    void setDescription(const QString &d) { m_description = d; }

    virtual void setConstraint(Node::ConstraintType type) { m_constraint = type; }
    void setConstraint(QString &type);
    int constraint() const { return m_constraint; }
    QString constraintToString() const;

    virtual void setConstraintStartTime(QDateTime time) { m_constraintStartTime = time; }
    virtual void setConstraintEndTime(QDateTime time) { m_constraintEndTime = time; }

    virtual DateTime &constraintStartTime() { return m_constraintStartTime; }
    virtual DateTime &constraintEndTime() { return m_constraintEndTime; }
    virtual DateTime &startNotEarlier() { return m_constraintStartTime; }
    virtual DateTime &finishNotLater() { return m_constraintEndTime; }
    virtual DateTime &mustStartOn() { return m_constraintStartTime; }
    virtual DateTime &mustFinishOn() { return m_constraintEndTime; }

    virtual ResourceGroupRequest *resourceRequest(ResourceGroup */*group*/) const { return 0; }
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
    
    virtual EffortCostMap plannedEffortCostPrDay(const QDate &start, const QDate &end) const=0;
        
    /// Returns the total planned effort for this task (or subtasks) 
    virtual Duration plannedEffort() { return Duration::zeroDuration; }
    /// Returns the total planned effort for this task (or subtasks) on date
    virtual Duration plannedEffort(const QDate &) { return Duration::zeroDuration; }
    /// Returns the planned effort up to and including date
    virtual Duration plannedEffortTo(const QDate &) { return Duration::zeroDuration; }
    
    /// Returns the total actual effort for this task (or subtasks) 
    virtual Duration actualEffort() { return Duration::zeroDuration; }
    /// Returns the total actual effort for this task (or subtasks) on date
    virtual Duration actualEffort(const QDate &/*date*/) { return Duration::zeroDuration; }
    /// Returns the total actual effort for this task (or subtasks) up to and including date
    virtual Duration actualEffortTo(const QDate &/*date*/) { return Duration::zeroDuration; }
    
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
    
    virtual void initiateCalculationLists(QPtrList<Node> &startnodes, QPtrList<Node> &endnodes, QPtrList<Node> &summarytasks) = 0;
    virtual DateTime calculateForward(int /*use*/) = 0;
    virtual DateTime calculateBackward(int /*use*/) = 0;
    virtual DateTime &scheduleForward(DateTime &, int /*use*/) = 0;
    virtual DateTime &scheduleBackward(DateTime &, int /*use*/) = 0;
    virtual void adjustSummarytask() = 0;

    virtual void initiateCalculation();
    virtual void resetVisited();
    void propagateEarliestStart(DateTime &time);
    void propagateLatestFinish(DateTime &time);
    void moveEarliestStart(DateTime &time);
    void moveLatestFinish(DateTime &time);
    // Reimplement this
    virtual Duration summarytaskDurationForward(const DateTime &/*time*/) 
        { return Duration::zeroDuration; }
    // Reimplement this
    virtual DateTime summarytaskEarliestStart() 
        { return DateTime(); }
    // Reimplement this
    virtual Duration summarytaskDurationBackward(const DateTime &/*time*/) 
        { return Duration::zeroDuration; }
    // Reimplement this
    virtual DateTime summarytaskLatestFinish() 
        { return DateTime(); }
    // Returns the (previously) calculated duration
    const Duration &duration() { return m_duration; }
    /**
     * Calculates and returns the duration of the node.
     * Uses the correct expected-, optimistic- or pessimistic effort
     * dependent on use.
     * @param time Where to start calculation.
     * @param use Calculate using expected-, optimistic- or pessimistic estimate.
     * @param Backward If true, time specifies when the task should end.
     */
    Duration duration(const DateTime &time, int use, bool backward);
    // Reimplement this
    virtual Duration calcDuration(const DateTime &/*time*/, const Duration &/*effort*/, bool /*backward*/) { return Duration::zeroDuration;}

    Node *siblingBefore();
    Node *childBefore(Node *node);
    Node *siblingAfter();
    Node *childAfter(Node *node);
    bool moveChildUp(Node *node);
    bool moveChildDown(Node *node);
    
    /// Check if this node can be linked to node
    bool legalToLink(Node *node);
    /// Check if node par can be linked to node child. (Reimplement)
    virtual bool legalToLink(Node *, Node *) { return false; }
    /// Check if this node has any dependent child nodes
    virtual bool isEndNode() const;
    /// Check if this node has any dependent parent nodes
    virtual bool isStartNode() const;
    virtual void clearProxyRelations() {}
    virtual void addParentProxyRelations(QPtrList<Relation> &) {}
    virtual void addChildProxyRelations(QPtrList<Relation> &) {}
    virtual void addParentProxyRelation(Node *, const Relation *) {}
    virtual void addChildProxyRelation(Node *, const Relation *) {}

    QPtrList<Appointment> &appointments() { return m_appointments; }

    /// Return appointments this node have with resource
    Appointment *findAppointment(Resource *resource);
    /// The total number of appointments
    int numAppointments() const { return m_appointments.count(); }
    /// Adds appointment to this node only (not to resource)
    virtual bool addAppointment(Appointment *appointment);
    /// Adds appointment to both this node and resource
    virtual void addAppointment(Resource *resource, DateTime &start, DateTime &end, double load=100);
    
    /// removes appoinrment and deletes it (independent of setAutoDelete)
    void removeAppointment(Appointment *appointment);
    /// removes appointment without deleting it (independent of setAutoDelete)
    void takeAppointment(Appointment *appointment);
    
    /// Find the node with my id
    virtual Node *findNode() const { return findNode(m_id); }
    /// Find the node with identity id
    virtual Node *findNode(const QString &id) const
        { return (m_parent ? m_parent->findNode(id) : 0); }
    /// Remove myself from the id register
    virtual bool removeId()  { return removeId(m_id); }
    /// Remove the registered identity id
    virtual bool removeId(const QString &id)
        { return (m_parent ? m_parent->removeId(id) : false); }
    /// Insert myself into the id register
    virtual void insertId(const QString &id) { insertId(id, this); }
    /// Insert node with identity id into the register
    virtual void insertId(const QString &id, const Node *node)
        { if (m_parent) m_parent->insertId(id, node); }
    
    DateTime &earliestStartForward() { return m_earliestStartForward; }
    DateTime &latestFinishBackward() { return m_latestFinishBackward; }
    
    /**
     * This is when work can start on this node in accordance with 
     * the calendar of allocated resources. Normally this is the same
     * as @ref startTime(), but may differ if timing constraints are set.
     */
    virtual const DateTime &workStartTime() const { return m_workStartTime; }
    void setWorkStartTime(const DateTime &dt) { m_workStartTime = dt; }
    
    /**
     * This is when work can finish on this node in accordance with 
     * the calendar of allocated resources. Normally this is the same
     * as @ref endTime(), but may differ if timing constraints are set.
     */
    virtual const DateTime &workEndTime() const { return m_workEndTime; }
    void setWorkEndTime(const DateTime &dt) { m_workEndTime = dt; }

    
    virtual bool isCritical() { return false; }
    virtual bool inCriticalPath() { return m_inCriticalPath; }
    virtual bool calcCriticalPath();
    
    /// Returns the level this node is in the hierarchy. Top node is level 0.
    virtual int level();
    /// Generate WBS
    virtual void generateWBS(int count, WBSDefinition &def, QString wbs=QString());
    QString wbs() const { return m_wbs; }
    
    double startupCost() const { return m_startupCost; }
    void setStartupCost(double cost) { m_startupCost = cost; }
    
    Account *startupAccount() const { return m_startupAccount; }
    void setStartupAccount(Account *acc) { m_startupAccount = acc; }
    
    double shutdownCost() const { return m_shutdownCost; }
    void  setShutdownCost(double cost) { m_shutdownCost = cost; }
    
    Account *shutdownAccount() const { return m_shutdownAccount; }
    void setShutdownAccount(Account *acc) { m_shutdownAccount = acc; }
    
    Account *runningAccount() const { return m_runningAccount; }
    void setRunningAccount(Account *acc) { m_runningAccount = acc; }

protected:
    QPtrList<Node> m_nodes;
    QPtrList<Relation> m_dependChildNodes;
    QPtrList<Relation> m_dependParentNodes;
    Node *m_parent;

    QString m_id; // unique id
    QString m_name;        // Name of this node
    QString m_leader;      // Person or group responsible for this node
    QString m_description; // Description of this node

    Effort* m_effort;
    
    /** earliestStart is calculated by PERT/CPM.
      * A task may be scheduled to start later because of constraints
      * or resource availability etc.
      */
    DateTime earliestStart;
    /** latestFinish is calculated by PERT/CPM.
      * A task may be scheduled to finish earlier because of constraints
      * or resource availability etc.
      */
    DateTime latestFinish;

    ConstraintType m_constraint;

    /**
      * @m_constraintTime is used if any of the constraints
      * FixedInterval, StartNotEarlier, MustStartOn or FixedInterval is selected
      */
    DateTime m_constraintStartTime;
    /**
      * @m_constraintEndTime is used if any of the constraints
      * FixedInterval, FinishNotLater, MustFinishOn or FixedInterval is selected
      */
    DateTime m_constraintEndTime;

    /**  m_startTime is the scheduled start time.
      *  It depends on constraints (i.e. ASAP/ALAP) and resource availability.
      *  It will always be later or equal to @ref earliestStart
      */
    DateTime m_startTime;

    /**  
      *  m_endTime is the scheduled finish time.
      *  It depends on constraints (i.e. ASAP/ALAP) and resource availability.
      *  It will always be earlier or equal to @ref latestFinish
      */
    DateTime m_endTime;
    
    /**  
      *  m_duration is the scheduled duration which depends on
      *  e.g. estimated effort, allocated resources and risk
      */
    Duration m_duration;

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
    Duration m_dateOnlyDuration;
 
    QPtrList<Appointment> m_appointments;
 
    DateTime m_earliestStartForward;
    Duration m_durationForward;
    DateTime m_latestFinishBackward;
    Duration m_durationBackward;
    DateTime m_workStartTime;
    DateTime m_workEndTime;

    bool m_inCriticalPath;
    QString m_wbs;
    
    double m_startupCost;
    Account *m_startupAccount;
    double m_shutdownCost;
    Account *m_shutdownAccount;
    Account *m_runningAccount;
    
private:
    void init();
        
#ifndef NDEBUG
public:
    virtual void printDebug(bool children, QCString indent);
#endif

};

////////////////////////////////   Effort   ////////////////////////////////
/**
  * Any @ref Node will store how much time it takes to complete the node
  * (typically a @ref Task) in the traditional scheduling software the
  * effort which is needed to complete the node is not simply a timespan but
  * is stored as an optimistic, a pessimistic and an expected timespan.
  */
class Effort {
public:
    Effort ( Duration e = Duration::zeroDuration, Duration p = Duration::zeroDuration,
		Duration o = Duration::zeroDuration );

    Effort ( double e, double p = 0, double o = 0);
    
    Effort (const Effort &effort);
    ~Effort();

    enum Type { Type_Effort = 0,        // Changing amount of resources changes the task duration
                          Type_FixedDuration = 1     // Changing amount of resources will not change the tasks duration
     };
    Type type() const { return m_type; }
    void setType(Type type) { m_type = type; }
    void setType(QString type);
    QString typeToString() const;

    enum Use { Use_Expected=0, Use_Optimistic=1, Use_Pessimistic=2 };
    const Duration& effort(int use) {
        if (use == Effort::Use_Expected)
            return m_expectedEffort;
        else if (use == Effort::Use_Optimistic)
            return m_optimisticEffort;
        else if (use == Effort::Use_Pessimistic)
            return m_pessimisticEffort;
        
        return m_expectedEffort; // default
    }
    const Duration& optimistic() const {return m_optimisticEffort;}
    const Duration& pessimistic() const {return m_pessimisticEffort;}
    const Duration& expected() const {return m_expectedEffort;}

    void set( Duration e, Duration p = Duration::zeroDuration, Duration o = Duration::zeroDuration );
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
    static const Effort zeroEffort;

private:
    Duration m_optimisticEffort;
    Duration m_pessimisticEffort;
    Duration m_expectedEffort;

    Type m_type;
    
#ifndef NDEBUG
public:
    void printDebug(QCString indent);
#endif

};

}  //KPlato namespace

#endif
