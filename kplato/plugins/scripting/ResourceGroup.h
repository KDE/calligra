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
            /// Return type of resource group
            QVariant type();
            /// Number of resources in this group
            int resourceCount() const;
            /// Return resource at @index
            QObject *resourceAt( int index ) const;
            
        private:
            Project *m_project;
            KPlato::ResourceGroup *m_group;
    };

}

#endif
