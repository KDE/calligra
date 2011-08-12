/* This file is part of the Calligra project
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
#include "kptnodeitemmodel.h"
#include "kptresourcemodel.h"
#include "kptaccountsmodel.h"
#include "kptcalendarmodel.h"

namespace KPlato {
    class Account;
    class Calendar;
    class Project;
    class Node;
    class ResourceGroup;
    class Resource;
    class ScheduleManager;
    class MacroCommand;
}

namespace Scripting {
    class Account;
    class Calendar;
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
            virtual ~Project();

            KPlato::Project *kplatoProject() const { return static_cast<KPlato::Project*>( m_node ); }
            
        public Q_SLOTS:
            /// Return data, default role and schedule
            QVariant data( QObject *object, const QString &property );
            /// Return data
            QVariant data( QObject *object, const QString &property, const QString &role, qlonglong scheduleId );
            /// Set the @p object @p property to @p data for @p role. Default @p role is Qt::EditRole
            bool setData( QObject *object, const QString &property, const QVariant &data, const QString &role = "Qt::EditRole" );
            /// Return header text
            QVariant headerData( int objectType, const QString &property );

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
            QObject *findNode( const QString &id );
            QObject *createTaskCopy( const QObject *copy, QObject *parent, QObject *after = 0 );
            QObject *createTask( QObject *parent, QObject *after = 0 );
            
            /// Returns resource header data for @p property
            QVariant resourceHeaderData( const QString &property );

            /// Number of resource groups
            int resourceGroupCount() const;
            /// Return the resource group at @p index
            QObject *resourceGroupAt( int index );
            /// Find resource group with identity @p id
            QObject *findResourceGroup( const QString &id );
            /// Create a copy of resource group @p group and insert it into the project
            QObject *createResourceGroup( QObject *group );

            /// Find resource with identity @p id
            QObject *findResource( const QString &id );
            /// Create a copy of @p resource and add to @p group
            QObject *createResource( QObject *group, QObject *resource );
            /// Add an external appointment to @p resource
            void addExternalAppointment( QObject *resource, const QVariant &id, const QString &name, const QVariantList &lst );
            /// Clear the resources @p resource external appointments to project with identity @p id
            void clearExternalAppointments( QObject *resource, const QString &id );
            /// Clear all the resources external appointments to project with identity @p id
            void clearExternalAppointments( const QString &id );
            /// Clear all the resources external appointments to all projects
            void clearExternalAppointments();
            /// Return a <id, name> map of external projects
            QVariantList externalProjects();

            /// Number of calendars
            int calendarCount() const;
            /// Return the calendar at @p index
            QObject *calendarAt( int index );
            /// Find calendar with identity @p id
            QObject *findCalendar( const QString &id );
            /// Create a copy of @p calendar and add to @p parent
            QObject *createCalendar( QObject *calendar, QObject *parent = 0 );

            /// Number of accounts
            int accountCount() const;
            /// Return the account at @p index
            QObject *accountAt( int index );
            /// Find account with identity @p id
            QObject *findAccount( const QString &id );

            /// Add all commands created since last addCommand() to the undo stack. The command is named @p name.
            void addCommand( const QString &name );
            /// Revert all commands that is not yet added with addCommand()
            void revertCommand();

        public:
            /// Return the Scripting::Node that interfaces the KPlato::Node @p node (create if necessary)
            Node *node( KPlato::Node *node );
            /// Return the data of @p node
            QVariant nodeData( const KPlato::Node *node, const QString &property, const QString &role, long schedule );
            /// Set node data
            bool setNodeData( KPlato::Node *node, const QString &property, const QVariant &data, const QString &role );
            
            /// Return ResourceGroup that interfaces the @p group (create if necessary)
            QObject *resourceGroup( KPlato::ResourceGroup *group );
            /// Return the data of resource group @p group
            QVariant resourceGroupData( const KPlato::ResourceGroup *group, const QString &property, const QString &role, long schedule = -1 );
            /// Set resource group data
            bool setResourceGroupData( KPlato::ResourceGroup *group, const QString &property, const QVariant &data, const QString &role );
            
            /// Return Resource that interfaces the @p resource (create if necessary)
            QObject *resource( KPlato::Resource *resource );
            /// Return the data of @p resource
            QVariant resourceData( const KPlato::Resource *resource, const QString &property, const QString &role, long schedule );
            /// Set resource data
            bool setResourceData( KPlato::Resource *resource, const QString &property, const QVariant &data, const QString &role );

            /// Return the Scripting::Calendar that interfaces the KPlato::Calendar @p cal
            Calendar *calendar( KPlato::Calendar *cal );
            /// Return the data of @p account
            QVariant calendarData( const KPlato::Calendar *calendar, const QString &property, const QString &role, long = -1 );
            /// Set calendar data
            bool setCalendarData( KPlato::Calendar *calendar, const QString &property, const QVariant &data, const QString &role );
            /// Return the header data of calendars
            QVariant calendarHeaderData( const QString &property );

            /// Return the Scripting::Schedule that interfaces the KPlato::ScheuleManager @p sch
            QObject *schedule( KPlato::ScheduleManager *sch );

            /// Return the Scripting::Account that interfaces the KPlato::Account @p acc
            QObject *account( KPlato::Account *acc );
            /// Return the header data of accounts
            QVariant accountHeaderData( const QString &property );
            /// Return the data of @p account
            QVariant accountData( const KPlato::Account *account, const QString &property, const QString &role, long = -1 );
            /// Set account data
            bool setAccountData( KPlato::Account *account, const QString &property, const QVariant &data, const QString &role );

        protected:
            inline KPlato::Project *project() { return m_nodeModel.project(); }
            inline const KPlato::Project *project() const { return m_nodeModel.project(); }
            
            int nodeColumnNumber( const QString &property ) const;
            
            int resourceColumnNumber( const QString &property ) const;
            
            int calendarColumnNumber( const QString &property ) const;

            int accountColumnNumber( const QString &property ) const;

        private slots:
            void slotAddCommand( KUndo2Command* );

        private:
            int stringToRole( const QString &role ) const;
            
        private:
            Module *m_module;
            
            KPlato::NodeItemModel m_nodeModel;
            QMap<KPlato::Node*, Node*> m_nodes;
            
            KPlato::ResourceItemModel m_resourceModel;
            QMap<KPlato::ResourceGroup*, ResourceGroup*> m_groups;
            QMap<KPlato::Resource*, Resource*> m_resources;

            KPlato::CalendarItemModel m_calendarModel;
            QMap<KPlato::Calendar*, Calendar*> m_calendars;

            QMap<KPlato::ScheduleManager*, Schedule*> m_schedules;
            
            KPlato::AccountModel m_accountModel;
            QMap<KPlato::Account*, Account*> m_accounts;

            KPlato::MacroCommand *m_command;
    };

}

#endif
