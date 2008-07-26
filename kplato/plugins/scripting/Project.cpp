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
#include "Account.h"
#include "Calendar.h"
#include "Resource.h"
#include "ResourceGroup.h"
#include "Schedule.h"

#include "kptglobal.h"
#include "kptaccount.h"
#include "kptcalendar.h"
#include "kptproject.h"
#include "kptschedule.h"
#include "kptresource.h"

#include <QMetaEnum>

Scripting::Project::Project( Scripting::Module* module, KPlato::Project *project )
    : Node( this, project, module ), m_module( module )
{
    kDebug()<<this<<"KPlato::"<<project;
    m_nodeModel.setProject( project );
    m_resourceModel.setProject( project );
}

Scripting::Project::~Project()
{
    kDebug()<<this;
    qDeleteAll( m_nodes.values() );
    qDeleteAll( m_groups.values() );
    qDeleteAll( m_resources.values() );
    qDeleteAll( m_calendars.values() );
    qDeleteAll( m_schedules.values() );
    qDeleteAll( m_accounts.values() );
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

QVariant Scripting::Project::nodeData( const KPlato::Node *node, const QString &property, const QString &role, long schedule )
{
    m_nodeModel.setManager( project()->scheduleManager( schedule ) );
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

QObject *Scripting::Project::findResourceGroup( const QString &id )
{
    KPlato::ResourceGroup *g = project()->findResourceGroup( id );
    return g == 0 ? 0 : resourceGroup( g );
}

QObject *Scripting::Project::createResourceGroup( QObject *group )
{
    //kDebug()<<this<<group;
    const ResourceGroup *gr = qobject_cast<ResourceGroup*>( group );
    if ( gr == 0 ) {
        kDebug()<<"No group specified";
        return 0;
    }
    KPlato::ResourceGroup *copyfrom = gr->kplatoResourceGroup();
    if ( copyfrom == 0 ) {
        kDebug()<<"Nothing to copy from";
        return 0;
    }
    KPlato::ResourceGroup *g = project()->findResourceGroup( copyfrom->id() );
    if ( g ) {
        kDebug()<<"Resource group already exists";
        return 0; // ???
    }
    g = new KPlato::ResourceGroup( copyfrom );
    project()->addResourceGroup( g );
    QObject *ng = resourceGroup( g );
    //kDebug()<<"New group created:"<<ng<<g;
    return ng;
}

QObject *Scripting::Project::resourceGroup( KPlato::ResourceGroup *group )
{
    if ( ! m_groups.contains( group ) ) {
        m_groups[ group ] = new ResourceGroup( this, group, parent() );
    }
    return m_groups[ group ];
}

QVariant Scripting::Project::resourceGroupData( const KPlato::ResourceGroup *group, const QString &property, const QString &role, long schedule )
{
//    m.setManager( project()->scheduleManager( schedule ) );
    return m_resourceModel.data( group, resourceColumnNumber( property ), stringToRole( role ) );
}

QObject *Scripting::Project::createResource( QObject *group, QObject *res )
{
    ResourceGroup *gr = qobject_cast<ResourceGroup*>( group );
    if ( gr == 0 ) {
        kDebug()<<"No group specified";
        return 0;
    }
    KPlato::ResourceGroup *g = project()->findResourceGroup( gr->kplatoResourceGroup()->id() );
    if ( g == 0 ) {
        kDebug()<<"Could not find group";
        return 0;
    }
    const Resource *rs = qobject_cast<Resource*>( res );
    if ( rs == 0 ) {
        kDebug()<<"No resource to copy from";
        return 0; // or create empty?
    }
    KPlato::Resource *r = project()->findResource( rs->kplatoResource()->id() );
    if ( r ) {
        kDebug()<<"Resource already exists";
        return 0;
    }
    r = new KPlato::Resource( rs->kplatoResource() );
    KPlato::Calendar *c = rs->kplatoResource()->calendar( true );
    if ( c ) {
        c = project()->calendar( c->id() );
    }
    r->setCalendar(c );
    project()->addResource( g, r );
    return resource( r );
}


QObject *Scripting::Project::resource( KPlato::Resource *resource )
{
    if ( ! m_resources.contains( resource ) ) {
        m_resources[ resource ] = new Resource( this, resource, parent() );
    }
    return m_resources[ resource ];
}

QVariant Scripting::Project::resourceData( const KPlato::Resource *resource, const QString &property, const QString &role, long schedule )
{
    //m_resourceModel.setManager( project()->scheduleManager( schedule.toLong() ) );
    return m_resourceModel.data( resource, resourceColumnNumber( property ), stringToRole( role ) ).toString();
}

QVariant Scripting::Project::resourceHeaderData( const QString &property )
{
    int col = resourceColumnNumber( property );
    return m_resourceModel.headerData( col );
}

QObject *Scripting::Project::findResource( const QString &id )
{
    KPlato::Resource *r = project()->findResource( id );
    return r == 0 ? 0 : resource( r );
}

void Scripting::Project::clearExternalAppointments( const QString &id )
{
    foreach ( KPlato::Resource *r, project()->resourceList() ) {
        r->clearExternalAppointments( id );
    }
}

void Scripting::Project::clearAllExternalAppointments()
{
    foreach ( KPlato::Resource *r, project()->resourceList() ) {
        r->clearExternalAppointments();
    }
}

int Scripting::Project::calendarCount() const
{
    return project()->calendarCount();
}

QObject *Scripting::Project::calendarAt( int index )
{
    return calendar( project()->calendarAt( index ) );
}

QObject *Scripting::Project::findCalendar( const QString &id )
{
    KPlato::Calendar *c = project()->calendar( id );
    kDebug()<<id<<c;
    return c == 0 ? 0 : calendar( c );
}

QObject *Scripting::Project::createCalendar( QObject *calendar, QObject *parent )
{
    kDebug()<<this<<calendar<<parent;
    const Calendar *cal = qobject_cast<Calendar*>( calendar );
    if ( cal == 0 ) {
        kDebug()<<"No calendar specified";
        return 0;
    }
    const KPlato::Calendar *copyfrom = cal->kplatoCalendar();
    if ( copyfrom == 0 ) {
        kDebug()<<"Nothing to copy from";
        return 0;
    }
    KPlato::Calendar *c = project()->calendar( copyfrom->id() );
    if ( c ) {
        kDebug()<<"Calendar already exists";
        return 0; // ???
    }
    Calendar *par = qobject_cast<Calendar*>( parent );
    KPlato::Calendar *p = 0;
    if ( par ) {
        p = project()->calendar( par->id() );
    }
    c = new KPlato::Calendar();
    *c = *copyfrom;
    c->setId( copyfrom->id() ); // NOTE: id is not copied
    project()->addCalendar( c, p );
    QObject *nc = this->calendar( c );
    return nc;
}

QObject *Scripting::Project::calendar( KPlato::Calendar *calendar )
{
    if ( calendar == 0 ) {
        return 0;
    }
    if ( ! m_calendars.contains( calendar ) ) {
        m_calendars[ calendar ] = new Calendar( this, calendar, parent() );
    }
    return m_calendars[ calendar ];
}

//-----------------------
int Scripting::Project::accountCount() const
{
    return project()->accounts().accountCount();
}

QObject *Scripting::Project::accountAt( int index )
{
    return account( project()->accounts().accountAt( index ) );
}

QObject *Scripting::Project::findAccount( const QString &id )
{
    KPlato::Account *a = project()->accounts().findAccount( id );
    kDebug()<<id<<a;
    return a == 0 ? 0 : account( a );
}

QObject *Scripting::Project::account( KPlato::Account *account )
{
    if ( account == 0 ) {
        return 0;
    }
    if ( ! m_accounts.contains( account ) ) {
        m_accounts[ account ] = new Account( this, account, parent() );
    }
    return m_accounts[ account ];
}

int Scripting::Project::accountProperty( const QString &property ) const
{
    if ( property == "AccountName" ) {
        return 0;
    }
    if ( property == "AccountDescription" ) {
        return 1;
    }
    return -1;
}

QVariant Scripting::Project::accountHeaderData( const QString &property )
{
    return m_accountModel.headerData( accountProperty( property ), Qt::Horizontal );
}

QVariant Scripting::Project::accountData( const KPlato::Account *account, const QString &property, const QString &, long )
{
    // Fake this atm
    switch ( accountProperty( property ) ) {
        case 0: return account->name();
        case 1: return account->description();
        default:
            break;
    }
    return QVariant();
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
