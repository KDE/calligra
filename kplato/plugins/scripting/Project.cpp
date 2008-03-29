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

#include "Project.h"
#include "Resource.h"
#include "ResourceGroup.h"
#include "Schedule.h"

#include "kptglobal.h"
#include "kptproject.h"
#include "kptschedule.h"
#include "kptresource.h"

#include <QMetaEnum>

Scripting::Project::Project( Scripting::Module* module, KPlato::Project *project )
    : Node( this, project, module ), m_module( module )
{
    m_nodeModel.setProject( project );
    m_resourceModel.setProject( project );
}

int Scripting::Project::scheduleCount() const
{
    return project()->numScheduleManagers();
}

QObject *Scripting::Project::scheduleAt( int index )
{
    return schedule( project()->scheduleManagers().value( index ) );
}

QObject *Scripting::Project::schedule( KPlato::ScheduleManager *sch )
{
    if ( ! m_schedules.contains( sch ) ) {
        m_schedules[ sch ] = new Schedule( this, sch, parent() );
    }
    return m_schedules[ sch ];
}


QStringList Scripting::Project::nodePropertyList()
{
    QStringList lst;
    QMetaEnum e = m_nodeModel.columnMap();
    for ( int i = 0; i < e.keyCount(); ++i ) {
        lst << QString( e.key( i ) );
    }
    return lst;
}

QVariant Scripting::Project::nodeHeaderData( const QString &property )
{
    int col = nodeColumnNumber( property );
    return m_nodeModel.headerData( col );
}

int Scripting::Project::nodeColumnNumber( const QString &property ) const
{
    return m_nodeModel.columnMap().keyToValue( property.toUtf8() );
}

int Scripting::Project::resourceColumnNumber( const QString &property ) const
{
    return m_resourceModel.columnMap().keyToValue( property.toUtf8() );
}

QObject *Scripting::Project::node( KPlato::Node *node )
{
    if ( ! m_nodes.contains( node ) ) {
        m_nodes[ node ] = new Node( this, node, parent() );
    }
    return m_nodes[ node ];
}

int Scripting::Project::nodeCount() const
{
    return project()->nodeCount();
}

QObject *Scripting::Project::nodeAt( int index )
{
    return node( project()->allNodes().value( index ) );
}

QVariant Scripting::Project::nodeData( const KPlato::Node *node, const QString &property, const QString &role, const QString &schedule )
{
    m_nodeModel.setManager( project()->scheduleManager( schedule.toLong() ) );
    return m_nodeModel.data( node, nodeColumnNumber( property ), stringToRole( role ) ).toString();
}

int Scripting::Project::resourceGroupCount() const
{
    return project()->resourceGroupCount();
}

QObject *Scripting::Project::resourceGroupAt( int index )
{
    return resourceGroup( project()->resourceGroupAt( index ) );
}

QObject *Scripting::Project::resourceGroup( KPlato::ResourceGroup *group )
{
    if ( ! m_groups.contains( group ) ) {
        m_groups[ group ] = new ResourceGroup( this, group, parent() );
    }
    return m_groups[ group ];
}

QVariant Scripting::Project::resourceGroupData( const KPlato::ResourceGroup *group, const QString &property, const QString &role )
{
//    m.setManager( project()->scheduleManager( schedule ) );
    return m_resourceModel.data( group, resourceColumnNumber( property ), stringToRole( role ) );
}

QObject *Scripting::Project::resource( KPlato::Resource *resource )
{
    if ( ! m_resources.contains( resource ) ) {
        m_resources[ resource ] = new Resource( this, resource, parent() );
    }
    return m_resources[ resource ];
}

QVariant Scripting::Project::resourceData( const KPlato::Resource *resource, const QString &property, const QString &role, const QString &schedule )
{
    //m_resourceModel.setManager( project()->scheduleManager( schedule.toLong() ) );
    return m_resourceModel.data( resource, resourceColumnNumber( property ), stringToRole( role ) ).toString();
}

QVariant Scripting::Project::resourceHeaderData( const QString &property )
{
    int col = resourceColumnNumber( property );
    return m_resourceModel.headerData( col );
}

int Scripting::Project::stringToRole( const QString &role ) const
{
    // TODO: use metaobject if possible
    QString r = role;
    if ( r.contains( "::" ) ) {
        r = r.right( role.length() - role.lastIndexOf( "::" ) - 2 );
    }
    if ( r == "DisplayRole" ) {
        return Qt::DisplayRole;
    }
    if ( r == "EditRole" ) {
        return Qt::EditRole;
    }
    kDebug()<<"Role is not handled:"<<role;
    return -1;
}

#include "Project.moc"
