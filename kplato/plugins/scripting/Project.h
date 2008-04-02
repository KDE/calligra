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
#include <QMap>
#include <QVariant>
#include <QVariantList>

#include "Module.h"
#include "Node.h"

#include "kptproject.h"
#include <kptnodeitemmodel.h>
#include <kptresourcemodel.h>

namespace KPlato {
    class Project;
    class Node;
    class ResourceGroup;
    class Resource;
    class ScheduleManager;
}

namespace Scripting {
    class ResourceGroup;
    class Resource;
    class Schedule;

    /**
    * The Project class represents a KPlato project.
    */
    class Project : public Node
    {
            Q_OBJECT
        public:
            Project( Module* module, KPlato::Project *project );
            virtual ~Project() {}

            KPlato::Project *kplatoProject() const { return static_cast<KPlato::Project*>( m_node ); }
            
        public Q_SLOTS:
            /// Return number of schedule managers
            int scheduleCount() const;
            /// Return schedule manager at @p index
            QObject *scheduleAt( int index );
            
            /// Returns the names of all node properties
            QStringList nodePropertyList();
            
            /// Returns node header data for @p property
            QVariant nodeHeaderData( const QString &property );
        
            /// Number of nodes in the project (excluding the project itself)
            int nodeCount() const;
            /// Return the node at @p index
            QObject *nodeAt( int index );
            
            /// Returns resource header data for @p property
            QVariant resourceHeaderData( const QString &property );

            /// Number of resource groups
            int resourceGroupCount() const;
            /// Return the resource group at @p index
            QObject *resourceGroupAt( int index );
            /// Find resource with @p id
            QObject *findResource( const QString &id );
            /// Clear all resources external appointments to project with @p id
            void clearExternalAppointments( const QString &id );
            /// Clear all resources external appointments to any project
            void clearAllExternalAppointments();
                    
        public:
            
            /// Return the Scripting::Node that interfaces the KPlato::Node @p node (create if necessary)
            QObject *node( KPlato::Node *node );
            /// Return the data of @p node
            QVariant nodeData( const KPlato::Node *node, const QString &property, const QString &role, long schedule );
            
            /// Return ResourceGroup that interfaces the @p group (create if necessary)
            QObject *resourceGroup( KPlato::ResourceGroup *group );
            /// Return the data of resource group @p group
            QVariant resourceGroupData( const KPlato::ResourceGroup *group, const QString &property, const QString &role, long schedule = -1 );
            
            /// Return Resource that interfaces the @p resource (create if necessary)
            QObject *resource( KPlato::Resource *resource );
            /// Return the data of @p resource
            QVariant resourceData( const KPlato::Resource *resource, const QString &property, const QString &role, long schedule );

            /// Return the Scripting::Schedule that interfaces the KPlato::ScheuleManager @p sch
            QObject *schedule( KPlato::ScheduleManager *sch );

        protected:
            inline KPlato::Project *project() { return m_nodeModel.project(); }
            inline const KPlato::Project *project() const { return m_nodeModel.project(); }
            
            int nodeColumnNumber( const QString &property ) const;
            
            int resourceColumnNumber( const QString &property ) const;
            
        private:
            int stringToRole( const QString &role ) const;
            
        private:
            Module *m_module;
            
            KPlato::NodeModel m_nodeModel;
            QMap<KPlato::Node*, Node*> m_nodes;
            
            KPlato::ResourceModel m_resourceModel;
            QMap<KPlato::ResourceGroup*, ResourceGroup*> m_groups;
            QMap<KPlato::Resource*, Resource*> m_resources;
    
            QMap<KPlato::ScheduleManager*, Schedule*> m_schedules;
    };

}

#endif
