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

#ifndef SCRIPTING_RESOURCEGROUP_H
#define SCRIPTING_RESOURCEGROUP_H

#include <QObject>
#include <QVariant>


namespace KPlato {
    class ResourceGroup;
}

namespace Scripting {
    class Project;
    class ResourceGroup;
    
    /**
    * The ResourceGroup class represents a resource group in a project.
    */
    class ResourceGroup : public QObject
    {
            Q_OBJECT
        public:
            /// Create a group
            ResourceGroup( Project *project, KPlato::ResourceGroup *group, QObject *parent );
            /// Destructor
            virtual ~ResourceGroup() {}
            
            KPlato::ResourceGroup *kplatoResourceGroup() const { return m_group; }
        
        public Q_SLOTS:
            /// Return the project this resource group is part of
            QObject* project();
            /// Return the identity of resource group
            QString id();
            /// Return type of resource group
            QString type();
            /// Number of resources in this group
            int resourceCount() const;
            /// Return resource at @index
            QObject *resourceAt( int index ) const;
            
            /// Number of resources in this group
            int childCount() const;
            /// Return resource at @index
            QObject *childAt( int index ) const;

        private:
            Project *m_project;
            KPlato::ResourceGroup *m_group;
    };

}

#endif
