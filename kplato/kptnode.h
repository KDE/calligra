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
#include "defs.h"
#include "kptrelation.h"

class KPTEffort;

/**
 * This class represents any node in the project, a node can be a project to a subproject and any task.
 * This class is basically an abstract interface to make the design more OO.
 */
class KPTNode {
    public:

        KPTNode();
        virtual ~KPTNode();

        // simple child node management
        // Child nodes are things like subtasks, basically a task can exists of several sub-tasks, 
        // creating a table has 4 subtasks, 1) measuring 2) cutting 3) building 4) painting.
        KPTNode *getParent() const {return m_parent; }
        const QList<KPTNode> &childNodeIterator() const { return m_nodes; }
        int numChildren() const { return m_nodes.count(); }
        virtual void addChildNode( KPTNode *node);
        virtual void insertChildNode( unsigned int index, KPTNode *node);
        void delChildNode( KPTNode *node, bool remove=true);
        void delChildNode( int number, bool remove=true);
        KPTNode *getChildNode( int number) { return m_nodes.at(number); }

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

        int numDependChildNodes() const { return m_dependChildNodes.count(); }
        virtual void addDependChildNode( KPTNode *node, TimingType t=START_ON_DATE, TimingRelation p=FINISH_START);
        virtual void addDependChildNode( KPTNode *node, TimingType t, TimingRelation p, QDateTime lag);
        virtual void insertDependChildNode( unsigned int index, KPTNode *node, TimingType t=START_ON_DATE, TimingRelation p=FINISH_START);
        void delDependChildNode( KPTNode *node, bool remove=false);
        void delDependChildNode( int number, bool remove=false);
        KPTRelation *getDependChildNode( int number) { return m_dependChildNodes.at(number); } 

        int numDependParentNodes() const { return m_dependParentNodes.count(); }
        virtual void addDependParentNode( KPTNode *node, TimingType t=START_ON_DATE, TimingRelation p=FINISH_START);
        virtual void addDependParentNode( KPTNode *node, TimingType t, TimingRelation p, QDateTime lag);
        virtual void insertDependParentNode( unsigned int index, KPTNode *node, TimingType t=START_ON_DATE, TimingRelation p=FINISH_START);
        void delDependParentNode( KPTNode *node, bool remove=false);
        void delDependParentNode( int number, bool remove=false);
        KPTRelation *getDependParentNode( int number) { return m_dependParentNodes.at(number); } 

        void setStartTime(QDateTime startTime) { m_startTime=startTime; }
        const QDateTime &startTime() const { return m_startTime; }
        void setEndTime(QDateTime endTime) { m_endTime=endTime; }
        const QDateTime &endTime() const { return m_endTime; }


        void setEffort(KPTEffort* e) { m_effort = e; }
        KPTEffort* effort() { return m_effort; }

        /** The expected Duration is the expected time to complete a Task, Project, etc. For an 
         *  individual Task, this will calculate the expected duration by querying 
         *  the Distribution of the Task. If the Distribution is a simple RiskNone, the value 
         *  will equal the mode Duration, but for other Distributions like RiskHigh, the value 
         *  will have to be calculated. For a Project or Subproject, the expected Duration is 
         *  calculated by PERT/CPM. 
         */
        virtual QDateTime *getExpectedDuration() = 0;

        /** Instead of using the expected duration, generate a random value using the Distribution of 
         *  each Task. This can be used for Monte-Carlo estimation of Project duration.
         */
        virtual QDateTime *getRandomDuration() = 0;

        /** Calculate the start time, use startTime() for the actually started time.
         */
        virtual QDateTime *getStartTime() = 0;

        /** Retrieve the calculated float of this node
         */
        virtual QDateTime *getFloat() =0;

        /** Calculate the delay of this node. Use the calculated startTime and the setted startTime.
         */
        QDateTime *getDelay();

        void setName(QString name) { m_name=name; }
        QString name() const { return m_name; }

    protected:
        QList<KPTNode> m_nodes;
        QList<KPTRelation> m_dependChildNodes;
        QList<KPTRelation> m_dependParentNodes;
        KPTNode *m_parent;
        NodeType m_nodeType;
        QString m_name;

        QDateTime m_startTime, m_endTime; // both entered during the project, not at the initial calculation.
        // effort variables.
        KPTEffort* m_effort;
};

class KPTEffort {

    public:
        KPTEffort ( QDateTime e = QDateTime(QDate(0,1,1)), QDateTime p = QDateTime(QDate(0,1,1)), QDateTime o = QDateTime(QDate(0,1,1)) );
        ~KPTEffort();

        const QDateTime& optimistic() {return m_optimisticDuration;}
        const QDateTime& pessimistic() {return m_pessimisticDuration;}
        const QDateTime& expected() {return m_expectedDuration;}

    private:
       QDateTime m_optimisticDuration;
       QDateTime m_pessimisticDuration;
       QDateTime m_expectedDuration;
};

#endif
