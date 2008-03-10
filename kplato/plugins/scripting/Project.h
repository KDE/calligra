/* This file is part of the KOffice project
 * Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>
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

#ifndef SCRIPTING_PROJECT_H
#define SCRIPTING_PROJECT_H

#include <QObject>

#include "Module.h"

#include "kptnode.h"
#include <kptnodeitemmodel.h>

namespace KPlato {
    class Project;
}

namespace Scripting {

    /**
    * The Project class represents a KPlato project.
    */
    class Project : public QObject
    {
            Q_OBJECT
        public:
            Project( Module* module, KPlato::Project *project );
            virtual ~Project() {}

        public Q_SLOTS:
            /// Returns the identity of the project node
            QString projectIdentity();
            /// Returns the list of identities of all schedules
            QStringList schedulesIdentityList();
            /// Returns the data of @p property from schedule with id @p scheduleId
            QString scheduleData( const QString &scheduleId, const QString &property );
            /// Returns the names of all node properties
            QStringList nodePropertyList();
            /// Returns data for @p property of node with id @p nodeId
            QString nodeData( const QString &nodeId, const QString &property );
            /// Returns data for @p property of node with id @p nodeId and dataRole @p role
            QString nodeData( const QString &nodeId, const QString &property, const QString &role );
            /// Returns data for @p property of node with id @p nodeId and dataRole @p role, using @p schedule
            QString nodeData( const QString &nodeId, const QString &property, const QString &role, const QString &schedule );
            /// Returns header data for @p property
            QString nodeHeaderData( const QString &property );

            /// Returns a list of identities of all nodes in the project
            QStringList nodesIdentityList();
            /// Returns a list of identities of all top level nodes (children of project)
            QStringList childNodesIdentityList();
            /// Returns a list of identities of child nodes to the node with id @p nodeId
            QStringList childNodesIdentityList( const QString &nodeId );
        
        protected:
            int columnNumber( const QString &property ) const;
            KPlato::Node *node( const QString &id ) const;
            
            int stringToRole( const QString &role ) const;
            
        private:
            KPlato::Project *m_project;
            KPlato::NodeModel m_nodeModel;

    };

}

#endif
