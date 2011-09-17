/* This file is part of the Calligra project
 * Copyright (c) 2008 Dag Andersen <danders@get2net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SCRIPTING_NODE_H
#define SCRIPTING_NODE_H

#include <QObject>
#include <QVariant>
#include <QDate>

namespace KPlato {
    class Node;
}

namespace Scripting {
    class Project;
    class Node;

    /**
    * The Node class represents a node in a project.
    */
    class Node : public QObject
    {
            Q_OBJECT
        public:
            /// Create a node
            Node( Project *project, KPlato::Node *node, QObject *parent );
            /// Destructor
            virtual ~Node() {}
        
            KPlato::Node *kplatoNode() const { return m_node; }
            
        public Q_SLOTS:
            /// Return the project this task is part of
            QObject* project();
            /// Return the tasks name
            QString name();

            QDate startDate();
            QDate endDate();
            
            /// Return the nodes id
            QString id();
            /// Return type of node
            QVariant type();
            /// Return number of child nodes
            int childCount() const;
            /// Return the child node at @p index
            QObject *childAt( int index );
            /// Return the data
            QObject *parentNode();

            /// Return a map of planed effort and cost pr day
            QVariant plannedEffortCostPrDay( const QVariant &start, const QVariant &end, const QVariant &schedule );

            /// Return a map of Budgeted Cost of Work Scheduled pr day
            QVariant bcwsPrDay( const QVariant &schedule ) const;
            /// Return a map of Budgeted Cost of Work Performed pr day
            QVariant bcwpPrDay( const QVariant &schedule ) const;
            /// Return a map of Actual Cost of Work Performed pr day
            QVariant acwpPrDay( const QVariant &schedule ) const;

        protected:
            Project *m_project;
            KPlato::Node *m_node;
    };

}

#endif
