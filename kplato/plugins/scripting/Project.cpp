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

#include <Project.h>

#include <kptglobal.h>
#include <kptproject.h>
#include <kptschedule.h>

#include <QMetaEnum>

Scripting::Project::Project( Scripting::Module* module, KPlato::Project *project )
    : QObject( module ), m_project( project )
{
    m_nodeModel.setProject( project );
}

QString Scripting::Project::projectIdentity()
{
    return m_project->id();
}

QStringList Scripting::Project::childNodesIdentityList()
{
    return childNodesIdentityList( projectIdentity() );
}

QStringList Scripting::Project::childNodesIdentityList( const QString &nodeId )
{
    QStringList lst;
    KPlato::Node *node = m_project->findNode( nodeId );
    if ( node ) {
        foreach ( KPlato::Node *n, node->childNodeIterator() ) {
            lst << n->id();
        }
    }
    kDebug()<<lst;
    return lst;
}

QString Scripting::Project::scheduleData( const QString &id, const QString &/*property*/ )
{
    //TODO: needs a schedule model
    KPlato::ScheduleManager *m = m_project->scheduleManager( id.toLong() );
    return m == 0 ? QString() : m->name();
}

QStringList Scripting::Project::schedulesIdentityList()
{
    QStringList lst;
    foreach ( KPlato::ScheduleManager *m, m_project->allScheduleManagers() ) {
        lst << QString("%1").arg( m->id() );
    }
    return lst;
}

QStringList Scripting::Project::nodesIdentityList()
{
    QStringList lst;
    foreach ( KPlato::Node *n, m_project->allNodes() ) {
        lst << n->id();
    }
    return lst;
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

QString Scripting::Project::nodeData( const QString &nodeId, const QString &property )
{
    return nodeData( nodeId, property, "DisplayRole" );
}

QString Scripting::Project::nodeData( const QString &nodeId, const QString &property, const QString &role )
{
    return nodeData( nodeId, property, role, "-1" );
}

QString Scripting::Project::nodeData( const QString &nodeId, const QString &property, const QString &role, const QString &schedule )
{
    KPlato::Node *n = node( nodeId );
    if ( n == 0 ) {
        kDebug()<<"Can't find node with id ="<<nodeId;
        return QString();
    }
    m_nodeModel.setManager( m_project->scheduleManager( schedule.toLong() ) );
    return m_nodeModel.data( n, columnNumber( property ), stringToRole( role ) ).toString();
}

QString Scripting::Project::nodeHeaderData( const QString &property )
{
    return m_nodeModel.headerData( columnNumber( property ) ).toString();
}

int Scripting::Project::columnNumber( const QString &property ) const
{
    return m_nodeModel.columnMap().keyToValue( property.toUtf8() );
}


// QString Scripting::Project::resourceData( const QString &resourceId, const QString &property )
// {
//     return resourceData( resourceId, property, -1 );
// }
// 
// QString Scripting::Project::resourceData( const QString &resourceId, const QString &property, long schedule  )
// {
//     KPlato::ResourceItemModel m;
//     m.setProject( m_project );
// //    m.setManager( m_project->scheduleManager( schedule ) );
//     return m.data( m_project->findResource( resourceId ), m.columnNames().columnNumber( property ) ).toString();
// }


KPlato::Node *Scripting::Project::node( const QString &nodeId ) const
{
    return m_project->findNode( nodeId );
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

