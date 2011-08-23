/* This file is part of the Calligra project
 * Copyright (c) 2008, 2011 Dag Andersen <danders@get2net>
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

            KPlato::Resource *kplatoResource() const { return m_resource; }

        public Q_SLOTS:
            /// Return the project this resource is part of
            QObject* project();
            /// Return type of resource
            QVariant type();
            /// Return type of resource
            QString id() const;

            /// Add external appointments
            void addExternalAppointment( const KPlato::Resource */*resource*/, const QString &/*start*/, const QString &/*end*/, int /*load */) {}

            /**
             * Return all internal appointments the resource has
             */
            QVariantList appointmentIntervals( qlonglong schedule ) const;
            /**
             * Return all external appointments the resource has
             */
            QVariantList externalAppointments() const;
            /// Add an external appointment
            void addExternalAppointment( const QVariant &id, const QString &name, const QVariantList &lst );
            /// Clear appointments with identity @p id
            void clearExternalAppointments( const QString &id );

            /// Number of child resources. Only team resources has children (team members)
            int childCount() const;
            /// Return resource at @index.  Only team resources has children (team members)
            QObject *childAt( int index ) const;
            /// Set list of children. The list of existing children is cleared
            void setChildren( const QList<QObject*> &members );

        private:
            Project *m_project;
            KPlato::Resource *m_resource;
    };

}

#endif
