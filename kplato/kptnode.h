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

#include <qdatetime.h> 
#include <qlist.h> 
#include <qstring.h> 

class KPTRisk;
class KPTRelation;

/**
 * This class represents any node in the project, a node can be a project to a subproject and any task.
 * This class is basically an abstract interface to make the design more OO.
 */
class KPTNode {
    public:

        KPTNode();
        ~KPTNode();

        // our timing calculations are based on the following point(s) in time
        enum TimingType {
          START_ON_DATE=0,
          FINISH_BY_DATE=1,
          WORK_BETWEEN_DATES=2,
          MILESTONE=3,
          PERCENT_OF_PROJECT=4 };

        enum TimingRelation {
          FINISH_START=0,   // child node cannot start until this node has finished 
          FINISH_FINISH=1,  // child node cannot finish until this node has finished
          START_START=2 };  // child node cannot start until this node has started


        // simple child node management
        // Child nodes are things like subtasks, basically a task can exists of several sub-tasks, 
        // creating a table has 4 subtasks, 1) measuring 2) cutting 3) building 4) painting.
        KPTNode *getParent() const {return m_parent; }
        const QList<KPTNode> &childNodeIterator() const { return m_nodes; }
        int numChildren() const { return m_nodes.count(); }
        virtual void addChildNode( KPTNode *node);
        void delChildNode( KPTNode *node, bool remove=true);
        void delChildNode( int number, bool remove=true);
        KPTNode *getChildNode( int number) { return m_nodes.at(number); }

        // resources management
        /** The resources are coupled with a user specified risk, we list the
         *  risks here, the risks contain a link to the resource itself.
         */
        const QList<KPTRisk> &riskIterator() const { return m_risks; }
        virtual void addRisk( KPTRisk *risk );
        void removeRisk( KPTRisk *risk );
        void removeRisk( int number );

        // Type of this node.
        enum NodeType {
          PROJECT=0,
          TASK=1,
          POINT_IN_TIME=2 };

        NodeType nodeType() { return m_nodeType; }

        // Time-dependent child-node-management.
        // list all nodes that are dependent upon this one.
        // Building a house requires the table to be finished, therefor the house-building
        // is time dependent on the table-building. So a child of the table-building node is the 
        // house-building node.

        const QList<KPTNode> &dependNodeIterator() const { return m_dependNodes; }
        int numDependNodes() const { return m_dependNodes.count(); }
        void addDependNode( KPTNode *node, TimingType t=START_ON_DATE, ParentRelation p=FINISH_START);
        void delDependNode( KPTNode *node, bool remove=false);
        void delDependNode( int number, bool remove=false);
        KPTNode *getDependNode( int number) { return m_dependNodes.at(number); }

        void setStartTime(QTime startTime) { m_startTime=startTime; }
        QTime startTime() { return m_startTime; }
        void setEndTime(QTime endTime) { m_endTime=endTime; }
        QTime endTime() { return endTime; }
        void setOptimisticDuration(QTime od) {m_optimisticDuration = od; }
        QTime optimisticDuration() { return m_optimisticDuration; }
        void setPessemisticDuration(QTime pd) {m_pessemisticDuration = pd; }
        QTime pessemisticDuration() { return m_pessemisticDuration; }
        void setExpectedDuration(QTime ed) { m_expectedDuration=ed; }
        QTime expectedDuration() { return m_expectedDuration; }

        /** The expected Duration is the expected time to complete a Task, Project, etc. For an 
         *  individual Task, this will calculate the expected duration by querying 
         *  the Distribution of the Task. If the Distribution is a simple RiskNone, the value 
         *  will equal the mode Duration, but for other Distributions like RiskHigh, the value 
         *  will have to be calculated. For a Project or Subproject, the expected Duration is 
         *  calculated by PERT/CPM. 
         */
        virtual QTime getExpectedDuration() = 0;

        /** Instead of using the expected duration, generate a random value using the Distribution of 
         *  each Task. This can be used for Monte-Carlo estimation of Project duration.
         */
        virtual QTime getRandomDuration() = 0;

        /** Calculate the start time, use startTime() for the actually started time.
         */
        virtual QTime getStartTime() = 0;

        /** Retrieve the calculated float of this node
         */
        virtual QTime getFloat() =0;

        /** Calculate the delay of this node. Use the calculated startTime and the setted startTime.
         */
        QTime getDelay();

        QString name() const { return m_name; }

    protected:
        QList<KPTNode> m_nodes;
        QList<KPTRelation> m_dependChildNodes;
        QList<KPTRelation> m_dependParentNodes;
        QList<KPTRisk> m_risk;
        KPTNode *m_parent;
        NodeType m_nodeType;
        QString m_name;

        QTime m_startTime, m_endTime; // both entered during the project, not at the initial calculation.
        // effort variables.
        QTime m_optimisticDuration, m_pessemisticDuration, m_expectedDuration;
};
#endif
