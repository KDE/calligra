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

#ifndef SCRIPTING_RESOURCE_H
#define SCRIPTING_RESOURCE_H

#include <QObject>
#include <QVariant>


namespace KPlato {
    class Resource;
}

namespace Scripting {
    class Project;
    class Resource;
    
    /**
    * The Resource class represents a resource in a project.
    */
    class Resource : public QObject
    {
            Q_OBJECT
        public:
            /// Create a resource
            Resource( Project *project, KPlato::Resource *resource, QObject *parent );
            /// Destructor
            virtual ~Resource() {}
        
        public Q_SLOTS:
            /// Return type of resource
            QVariant type();
            /// Return the data
            QVariant data( const QString &property, const QString &role, const QString &schedule );
            /// Return the data
            QVariant data( const QString &property );
            
            /// Add external appointments
            void addExternalAppointment( const KPlato::Resource *resource, const QString &start, const QString &end, int load ) {}

        private:
            Project *m_project;
            KPlato::Resource *m_resource;
    };

}

#endif
