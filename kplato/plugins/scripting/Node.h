/* This file is part of the KOffice project
 * Copyright (c) 2008 Dag Andersen <kplato@kde.org>
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
        
        public Q_SLOTS:
            QDate startDate();
            QDate endDate();
            
            /// Return type of node
            QVariant type();
            /// Return number of child nodes
            int childCount() const;
            /// Return the child node at @p index
            QObject *childAt( int index );
            /// Return the data
            QVariant data( const QString &property, const QString &role, const QString &schedule );
            /// Return the data
            QVariant data( const QString &property );
            
            /// Return a map of palnned effort and cost pr day
            QVariant plannedEffortCostPrDay( const QVariant &start, const QVariant &end, const QString &schedule );

        private:
            Project *m_project;
            KPlato::Node *m_node;
    };

}

#endif
