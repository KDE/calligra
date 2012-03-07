/* This file is part of the Calligra project
 * Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>
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
#include "kptcommand.h"

#include <QMetaEnum>

extern int planScriptingDebugArea();

Scripting::Project::Project( Scripting::Module* module, KPlato::Project *project )
    : Node( this, project, module ), m_module( module )
{
    kDebug(planScriptingDebugArea())<<this<<"KPlato::"<<project;
    m_nodeModel.setProject( project );
    m_nodeModel.setShowProject( true );
    m_nodeModel.setReadWrite( true );
    m_nodeModel.setReadOnly( NodeModel::NodeDescription, false );
    connect(&m_nodeModel, SIGNAL(executeCommand(KUndo2Command*)), SLOT(slotAddCommand(KUndo2Command*)));

    m_resourceModel.setProject( project );
    m_resourceModel.setReadWrite( true );
    connect(&m_resourceModel, SIGNAL(executeCommand(KUndo2Command*)), SLOT(slotAddCommand(KUndo2Command*)));

    m_accountModel.setProject( project );
    m_accountModel.setReadWrite( true );
    connect(&m_accountModel, SIGNAL(executeCommand(KUndo2Command*)), SLOT(slotAddCommand(KUndo2Command*)));

    m_calendarModel.setProject( project );
    m_calendarModel.setReadWrite( true );
    connect(&m_calendarModel, SIGNAL(executeCommand(KUndo2Command*)), SLOT(slotAddCommand(KUndo2Command*)));

    // Define the role to use where data fetched with Qt::DisplayRole cannot used in setData
    m_nodeprogramroles[ NodeModel::NodeEstimateType ] = Qt::EditRole;
    m_nodeprogramroles[ NodeModel::NodeEstimateCalendar ] = Qt::EditRole;
    m_nodeprogramroles[ NodeModel::NodeConstraint ] = Qt::EditRole;
    m_nodeprogramroles[ NodeModel::NodeConstraintStart ] = Qt::EditRole;
    m_nodeprogramroles[ NodeModel::NodeConstraintEnd ] = Qt::EditRole;
    m_nodeprogramroles[ NodeModel::NodeRunningAccount ] = Qt::EditRole;
    m_nodeprogramroles[ NodeModel::NodeStartupAccount ] = Qt::EditRole;
    m_nodeprogramroles[ NodeModel::NodeDescription ] = Qt::EditRole;
    m_nodeprogramroles[ NodeModel::NodeShutdownAccount ] = Qt::EditRole;
    m_nodeprogramroles[ NodeModel::NodeDescription ] = Qt::EditRole;
    m_nodeprogramroles[ NodeModel::NodeDescription ] = Qt::EditRole;

    m_resourceprogramroles[ ResourceModel::ResourceAvailableFrom ] = Qt::EditRole;
    m_resourceprogramroles[ ResourceModel::ResourceAvailableUntil ] = Qt::EditRole;
    m_resourceprogramroles[ ResourceModel::ResourceNormalRate ] = Qt::EditRole;
    m_resourceprogramroles[ ResourceModel::ResourceOvertimeRate ] = Qt::EditRole;

    // FIXME: faked for now
    int c = m_calendarModel.columnCount();
    m_calendarprogramroles[ c ] = Qt::EditRole; // Weekday
    m_calendarprogramroles[ c + 1 ] = Qt::EditRole; // Date
}

Scripting::Project::~Project()
{
    kDebug(planScriptingDebugArea())<<this;
    qDeleteAll( m_nodes );
    qDeleteAll( m_groups );
    qDeleteAll( m_resources );
    qDeleteAll( m_calendars );
    qDeleteAll( m_schedules );
    qDeleteAll( m_accounts );
}

QObject *Scripting::Project::defaultCalendar()
{
    return calendar( kplatoProject()->defaultCalendar() );
}

void Scripting::Project::setDefaultCalendar(Scripting::Calendar* calendar)
{
    if ( calendar ) {
        setCalendarData( calendar->kplatoCalendar(), "Name", Qt::Checked, "CheckStateRole" );
    }
}

QVariant Scripting::Project::data( QObject *object, const QString &property )
{
    return data( object, property, "DisplayRole", -1 );
}

QVariant Scripting::Project::data( QObject *object, const QString &property, const QString &role, qlonglong scheduleId )
{
    Node *n = qobject_cast<Node*>( object );
    if ( n ) {
        if ( n->project() != this ) {
            return QVariant();
        }
        return nodeData( n->kplatoNode(), property, role, scheduleId );
    }
    Resource *r = qobject_cast<Resource*>( object );
    if ( r ) {
        if ( r->project() != this ) {
            return QVariant();
        }
        return resourceData( r->kplatoResource(), property, role, scheduleId );
    }
    ResourceGroup *g = qobject_cast<ResourceGroup*>( object );
    if ( g ) {
        if ( g->project() != this ) {
            return QVariant();
        }
        return resourceGroupData( g->kplatoResourceGroup(), property, role );
    }
    Account *a = qobject_cast<Account*>( object );
    if ( a ) {
        if ( a->project() != this ) {
            return QVariant();
        }
        return accountData( a->kplatoAccount(), property, role );
    }
    Calendar *c = qobject_cast<Calendar*>( object );
    if ( c ) {
        if ( c->project() != this ) {
            return QVariant();
        }
        return calendarData( c->kplatoCalendar(), property, role );
    }
    // TODO Schedule (if needed)
    return QVariant();
}

QVariant Scripting::Project::setData( QObject* object, const QString& property, const QVariant& data, const QString& role )
{
    Node *n = qobject_cast<Node*>( object );
    if ( n ) {
        if ( n->project() != this ) {
            return "Invalid";
        }
        return setNodeData( n->kplatoNode(), property, data, role );
    }
    Resource *r = qobject_cast<Resource*>( object );
    if ( r ) {
        if ( r->project() != this ) {
            return "Invalid";
        }
        return setResourceData( r->kplatoResource(), property, data, role );
    }
    ResourceGroup *g = qobject_cast<ResourceGroup*>( object );
    if ( g ) {
        if ( g->project() != this ) {
            return "Invalid";
        }
        return setResourceGroupData( g->kplatoResourceGroup(), property, data, role );
    }
    Account *a = qobject_cast<Account*>( object );
    if ( a ) {
        if ( a->project() != this ) {
            return "Invalid";
        }
        return setAccountData( a->kplatoAccount(), property, data, role );
    }
    Calendar *c = qobject_cast<Calendar*>( object );
    if ( c ) {
        if ( c->project() != this ) {
            return "Invalid";
        }
        return setCalendarData( c->kplatoCalendar(), property, data, role );
    }
    return "Invalid";
}

QVariant Scripting::Project::headerData( int objectType, const QString &property, const QString &role )
{
    switch ( objectType ) {
        case 0: return taskHeaderData( property, role );
        case 1: return resourceHeaderData( property, role );
        case 2: return accountHeaderData( property, role );
        case 3: return calendarHeaderData( property, role );
        default: break;
    }
    return QVariant();
}

int Scripting::Project::scheduleCount() const
{
    return kplatoProject()->numScheduleManagers();
}

QObject *Scripting::Project::scheduleAt( int index )
{
    return schedule( kplatoProject()->scheduleManagers().value( index ) );
}

QObject *Scripting::Project::schedule( KPlato::ScheduleManager *sch )
{
    if ( sch == 0 ) {
        return 0;
    }
    if ( ! m_schedules.contains( sch ) ) {
        m_schedules[ sch ] = new Schedule( this, sch, parent() );
    }
    return m_schedules[ sch ];
}


QStringList Scripting::Project::taskPropertyList()
{
    QStringList lst;
    QMetaEnum e = m_nodeModel.columnMap();
    for ( int i = 0; i < e.keyCount(); ++i ) {
        QString s = QString( e.key( i ) );
        if ( s.left( 4 ) == "Node" ) {
            s.remove( 0, 4 );
        }
        lst << QString( e.key( i ) );
    }
    return lst;
}

QVariant Scripting::Project::taskHeaderData( const QString &property, const QString &role )
{
    int col = nodeColumnNumber( property );
    return m_nodeModel.headerData( col, Qt::Horizontal, stringToRole( role ) );
}

int Scripting::Project::nodeColumnNumber( const QString &property ) const
{
    int col = m_nodeModel.columnMap().keyToValue( property.toUtf8() );
    if ( col > 0 ) {
        return col;
    }
    QString prop = property;
    if ( prop.left( 4 ) != "Node" ) {
        prop.prepend( "Node" );
    }
    return m_nodeModel.columnMap().keyToValue( prop.toUtf8() );
}

int Scripting::Project::resourceColumnNumber( const QString &property ) const
{
    QString prop = property;
    if ( prop.left( 8 ) != "Resource" ) {
        prop.prepend( "Resource" );
    }
    return m_resourceModel.columnMap().keyToValue( prop.toUtf8() );
}

int Scripting::Project::programRole( const QMap<int, int> &map, int column ) const
{
    return map.contains( column ) ? map[ column ] : Qt::DisplayRole;
}

Scripting::Node *Scripting::Project::node( KPlato::Node *node )
{
    if ( node == 0 ) {
        return 0;
    }
    if ( ! m_nodes.contains( node ) ) {
        m_nodes[ node ] = new Node( this, node, parent() );
    }
    return m_nodes[ node ];
}

int Scripting::Project::taskCount() const
{
    return childCount();
}

QObject *Scripting::Project::taskAt( int index )
{
    return childAt( index );
}

QVariant Scripting::Project::nodeData( const KPlato::Node *node, const QString &property, const QString &role, long schedule )
{
    KPlato::ScheduleManager *sm = kplatoProject()->scheduleManager( schedule );
    if ( m_nodeModel.scheduleManager() != sm ) {
        m_nodeModel.setScheduleManager( kplatoProject()->scheduleManager( schedule ) );
    }
    int col = nodeColumnNumber( property );
    QModelIndex idx = m_nodeModel.index( node );
    idx = m_nodeModel.index( idx.row(), col, idx.parent() );
    if ( ! idx.isValid() ) {
        kDebug(planScriptingDebugArea())<<"Failed"<<node<<property<<idx;
        return QVariant();
    }
    int r = stringToRole( role, m_nodeprogramroles.value( col ) );
    if ( r < 0 ) {
        return QVariant(); // invalid role
    }
    if ( col == NodeModel::NodeDescription && r == Qt::DisplayRole ) {
        r = Qt::EditRole; // cannot use displayrole here
    }
    QVariant value = m_nodeModel.data( idx, r );
    if ( r == Qt::EditRole ) {
        switch ( col ) {
            case NodeModel::NodeType:
                value = QVariant( node->typeToString( KPlato::Node::NodeTypes( value.toInt() ), false ) );
                break;
            case NodeModel::NodeConstraint:
                // ASAP, ALAP, MustStartOn, MustFinishOn, StartNotEarlier, FinishNotLater, FixedInterval
                value = QVariant( node->constraintList( false ).value( value.toInt() ) );
                break;
            default:
                break;
        }
    }

    return value;
}

QVariant Scripting::Project::setNodeData( KPlato::Node *node, const QString &property, const QVariant &data, const QString &role )
{
    int col = nodeColumnNumber( property );
    QModelIndex idx = m_nodeModel.index( node, col );
    if ( ! idx.isValid() ) {
        return "Invalid";
    }
    if ( ( m_nodeModel.flags( idx ) & Qt::ItemIsEditable ) == 0 ) {
        return "ReadOnly";
    }
    int datarole = stringToRole( role, Qt::EditRole );
    if ( datarole < 0 ) {
        return "Invalid role: " + role;
    }
    if ( nodeData( node, property, datarole == Qt::EditRole ? "ProgramRole" : role ) == data ) {
        return "Success";
    }
    return m_nodeModel.setData( idx, data, datarole ) ? "Success" : "Error";
}


QObject *Scripting::Project::findTask( const QString &id )
{
    return node( kplatoProject()->findNode( id ) );
}

QObject *Scripting::Project::createTask(const QObject* copy, QObject* parent, QObject* after)
{
    const Node *cpy = static_cast<const Node*>( copy );
    KPlato::Node *t = 0;
    if ( cpy ) {
        t = kplatoProject()->createTask( static_cast<KPlato::Task&>( *( cpy->kplatoNode() ) ) );
    } else {
        t = kplatoProject()->createTask();
    }
    KPlato::NamedCommand *cmd;
    if ( parent ) {
        KPlato::Node *par = static_cast<Node*>( parent )->kplatoNode();
        cmd = new SubtaskAddCmd( kplatoProject(), t, par, i18nc( "(qtundo_format)", "Add task" ) );
    } else {
        KPlato::Node *aft = after ? static_cast<Node*>( after )->kplatoNode() : 0;
        cmd = new TaskAddCmd( kplatoProject(), t, aft, i18nc( "(qtundo_format)", "Add task" ) );
    }
    m_module->addCommand( cmd );
    return node( t );
}

QObject *Scripting::Project::createTask( QObject* parent, QObject* after )
{
    KPlato::Task *t = kplatoProject()->createTask();
    KPlato::NamedCommand *cmd;
    if ( parent ) {
        KPlato::Node *par = static_cast<Node*>( parent )->kplatoNode();
        cmd = new SubtaskAddCmd( kplatoProject(), t, par, i18nc( "(qtundo_format)", "Add task" ) );
    } else {
        KPlato::Node *aft = after ? static_cast<Node*>( after )->kplatoNode() : 0;
        cmd = new TaskAddCmd( kplatoProject(), t, aft, i18nc( "(qtundo_format)", "Add task" ) );
    }
    m_module->addCommand( cmd );
    return node( t );
}

int Scripting::Project::resourceGroupCount() const
{
    return kplatoProject()->resourceGroupCount();
}

QObject *Scripting::Project::resourceGroupAt( int index )
{
    return resourceGroup( kplatoProject()->resourceGroupAt( index ) );
}

QObject *Scripting::Project::findResourceGroup( const QString &id )
{
    KPlato::ResourceGroup *g = kplatoProject()->findResourceGroup( id );
    return g == 0 ? 0 : resourceGroup( g );
}

QObject *Scripting::Project::createResourceGroup( QObject *group )
{
    //kDebug(planScriptingDebugArea())<<this<<group;
    KPlato::ResourceGroup *g = 0;
    const ResourceGroup *gr = qobject_cast<ResourceGroup*>( group );
    if ( gr == 0 ) {
        return createResourceGroup();
    }
    KPlato::ResourceGroup *copyfrom = gr->kplatoResourceGroup();
    if ( copyfrom == 0 ) {
        kDebug(planScriptingDebugArea())<<"Nothing to copy from";
        return 0;
    }
    if ( kplatoProject()->findResourceGroup( copyfrom->id() ) ) {
        kDebug(planScriptingDebugArea())<<"Group with id already exists";
        return 0;
    }
    g = new KPlato::ResourceGroup( copyfrom );
    AddResourceGroupCmd *cmd = new AddResourceGroupCmd( kplatoProject(), g, i18nc( "(qtundo_format)", "Add resource group" ) );
    m_module->addCommand( cmd );
    return resourceGroup( g );
}

QObject *Scripting::Project::createResourceGroup()
{
    KPlato::ResourceGroup *g = new KPlato::ResourceGroup();
    AddResourceGroupCmd *cmd = new AddResourceGroupCmd( kplatoProject(), g, i18nc( "(qtundo_format)", "Add resource group" ) );
    m_module->addCommand( cmd );
    return resourceGroup( g );
}

QObject *Scripting::Project::resourceGroup( KPlato::ResourceGroup *group )
{
    if ( group == 0 ) {
        return 0;
    }
    if ( ! m_groups.contains( group ) ) {
        m_groups[ group ] = new ResourceGroup( this, group, parent() );
    }
    return m_groups[ group ];
}

QVariant Scripting::Project::setResourceGroupData( KPlato::ResourceGroup *resource, const QString &property, const QVariant &data, const QString &role )
{
    QModelIndex idx = m_resourceModel.index( resource, resourceColumnNumber( property ) );
    if ( ! idx.isValid() ) {
        return "Invalid";
    }
    if ( ( m_resourceModel.flags( idx ) & Qt::ItemIsEditable ) == 0 ) {
        return "ReadOnly";
    }
    int datarole = stringToRole( role, Qt::EditRole );
    if ( datarole < 0 ) {
        return "Invalid role: " + role;
    }
    if ( resourceGroupData( resource, property, datarole == Qt::EditRole ? "ProgramRole" : role ) == data ) {
        return "Success";
    }
    return m_resourceModel.setData( idx, data, datarole ) ? "Success" : "Error";
}

QVariant Scripting::Project::resourceGroupData( const KPlato::ResourceGroup *group, const QString &property, const QString &role, long /*schedule*/ )
{
    QModelIndex idx = m_resourceModel.index( group );
    idx = m_resourceModel.index( idx.row(), resourceColumnNumber( property ), idx.parent() );
    if ( ! idx.isValid() ) {
        return QVariant();
    }
    int r = m_resourceprogramroles.value( idx.column() );
    r = stringToRole( role, r );
    if ( r < 0 ) {
        return QVariant(); // invalid role
    }
    return m_resourceModel.data( idx, r );
}

QObject *Scripting::Project::createResource( QObject *group, QObject *copy )
{
    ResourceGroup *gr = qobject_cast<ResourceGroup*>( group );
    if ( gr == 0 ) {
        kDebug(planScriptingDebugArea())<<"No group specified";
        return 0;
    }
    KPlato::ResourceGroup *g = kplatoProject()->findResourceGroup( gr->kplatoResourceGroup()->id() );
    if ( g == 0 ) {
        kDebug(planScriptingDebugArea())<<"Could not find group";
        return 0;
    }
    KPlato::Resource *r = 0;
    const Resource *rs = qobject_cast<Resource*>( copy );
    if ( rs == 0 ) {
        return createResource( group );
    }
    r = kplatoProject()->findResource( rs->kplatoResource()->id() );
    if ( r ) {
        kDebug(planScriptingDebugArea())<<"Resource already exists";
        return 0;
    }
    r = new KPlato::Resource( rs->kplatoResource() );
    KPlato::Calendar *c = rs->kplatoResource()->calendar( true );
    if ( c ) {
        c = kplatoProject()->calendar( c->id() );
    }
    r->setCalendar( c );
    AddResourceCmd *cmd = new AddResourceCmd( g, r, i18nc( "(qtundo_format)", "Add resource" ) );
    m_module->addCommand( cmd );
    return resource( r );
}

QObject *Scripting::Project::createResource( QObject *group )
{
    ResourceGroup *gr = qobject_cast<ResourceGroup*>( group );
    if ( gr == 0 ) {
        kDebug(planScriptingDebugArea())<<"No group specified";
        return 0;
    }
    KPlato::ResourceGroup *g = kplatoProject()->findResourceGroup( gr->kplatoResourceGroup()->id() );
    if ( g == 0 ) {
        kDebug(planScriptingDebugArea())<<"Could not find group";
        return 0;
    }
    KPlato::Resource *r = new KPlato::Resource();
    AddResourceCmd *cmd = new AddResourceCmd( g, r, i18nc( "(qtundo_format)", "Add resource" ) );
    m_module->addCommand( cmd );
    return resource( r );
}


QObject *Scripting::Project::resource( KPlato::Resource *resource )
{
    if ( resource == 0 ) {
        return 0;
    }
    if ( ! m_resources.contains( resource ) ) {
        m_resources[ resource ] = new Resource( this, resource, parent() );
    }
    return m_resources[ resource ];
}

QVariant Scripting::Project::setResourceData( KPlato::Resource *resource, const QString &property, const QVariant &data, const QString &role )
{
    QModelIndex idx = m_resourceModel.index( resource, resourceColumnNumber( property ) );
    if ( ! idx.isValid() ) {
        return "Invalid";
    }
    if ( ( m_resourceModel.flags( idx ) & Qt::ItemIsEditable ) == 0 ) {
        return "ReadOnly";
    }
    int datarole = stringToRole( role, Qt::EditRole );
    if ( datarole < 0 ) {
        return "Invalid role: " + role;
    }
    if ( resourceData( resource, property, datarole == Qt::EditRole ? "ProgramRole" : role ) == data ) {
        return "Success";
    }
    return m_resourceModel.setData( idx, data, datarole ) ? "Success" : "Error";
}

QVariant Scripting::Project::resourceData( const KPlato::Resource *resource, const QString &property, const QString &role, long /*schedule*/ )
{
    QModelIndex idx = m_resourceModel.index( resource );
    idx = m_resourceModel.index( idx.row(), resourceColumnNumber( property ), idx.parent() );
    if ( ! idx.isValid() ) {
        kDebug(planScriptingDebugArea())<<"Invalid index"<<resource;
        return QVariant();
    }
    int r = stringToRole( role, m_resourceprogramroles.value( idx.column() ) );
    if ( r < 0 ) {
        return QVariant(); // invalid role
    }
    return m_resourceModel.data( idx, r );
}

QStringList Scripting::Project::resourcePropertyList()
{
    QStringList lst;
    QMetaEnum e = m_resourceModel.columnMap();
    for ( int i = 0; i < e.keyCount(); ++i ) {
        QString s = QString( e.key( i ) );
        if ( s.left( 8 ) == "Resource" ) {
            s.remove( 0, 8 );
        }
        lst << s;
    }
    return lst;
}

QVariant Scripting::Project::resourceHeaderData( const QString &property, const QString &role )
{
    int col = resourceColumnNumber( property );
    return m_resourceModel.headerData( col, Qt::Horizontal, stringToRole( role ) );
}

QObject *Scripting::Project::findResource( const QString &id )
{
    KPlato::Resource *r = kplatoProject()->findResource( id );
    return r == 0 ? 0 : resource( r );
}

QVariantList Scripting::Project::externalProjects()
{
    QVariantList lst;
    QMap<QString, QString> map = kplatoProject()->externalProjects();
    for ( QMapIterator<QString, QString> it( map ); it.hasNext(); ) {
        it.next();
        QVariantList m;
        m << it.key() << it.value();
        lst << QVariant(m);
    }
    return lst;
}

void Scripting::Project::addExternalAppointment( QObject *resource, const QVariant &id, const QString &name, const QVariantList &lst )
{
    Resource *r = qobject_cast<Resource*>( resource );
    if ( r == 0 ) {
        return;
    }
    //kDebug(planScriptingDebugArea())<<id<<name<<lst;
    KPlato::DateTime st = KPlato::DateTime::fromString( lst[0].toString() );
    KPlato::DateTime et = KPlato::DateTime::fromString( lst[1].toString() );
    double load = lst[2].toDouble();
    if ( ! st.isValid() || ! et.isValid() ) {
        return;
    }
    AddExternalAppointmentCmd *cmd = new AddExternalAppointmentCmd( r->kplatoResource(), id.toString(), name, st, et, load, i18nc( "(qtundofrmat)", "Add external appointment" ) );
    m_module->addCommand( cmd );
}

void Scripting::Project::clearExternalAppointments( QObject *resource, const QString &id )
{
    Resource *r = qobject_cast<Resource*>( resource );
    if ( r == 0 ) {
        return;
    }
    ClearExternalAppointmentCmd *cmd = new ClearExternalAppointmentCmd( r->kplatoResource(), id, i18nc( "(qtundofrmat)", "Clear external appointments" ) );
    m_module->addCommand( cmd );
}

void Scripting::Project::clearExternalAppointments( const QString &id )
{
    foreach ( KPlato::Resource *r, kplatoProject()->resourceList() ) {
        ClearExternalAppointmentCmd *cmd = new ClearExternalAppointmentCmd( r, id, i18nc( "(qtundo_format)", "Clear external appointments" ) );
        m_module->addCommand( cmd );
    }
}

void Scripting::Project::clearExternalAppointments()
{
    ClearAllExternalAppointmentsCmd *cmd = new ClearAllExternalAppointmentsCmd( kplatoProject(), i18nc( "(qtundo_format)", "Clear all external appointments" ) );
    m_module->addCommand( cmd );
}

int Scripting::Project::calendarCount() const
{
    return kplatoProject()->calendarCount();
}

QObject *Scripting::Project::calendarAt( int index )
{
    return calendar( kplatoProject()->calendarAt( index ) );
}

QObject *Scripting::Project::findCalendar( const QString &id )
{
    KPlato::Calendar *c = kplatoProject()->calendar( id );
    kDebug(planScriptingDebugArea())<<id<<c;
    return calendar( c );
}

QObject *Scripting::Project::createCalendar( QObject *copy, QObject *parent )
{
    kDebug(planScriptingDebugArea())<<this<<copy<<parent;
    const KPlato::Calendar *copyfrom = 0;
    KPlato::Calendar *c = 0;
    if ( copy == 0 ) {
        return createCalendar( parent );
    }
    const Calendar *cal = qobject_cast<Calendar*>( copy );
    copyfrom = cal->kplatoCalendar();
    if ( copyfrom == 0 ) {
        kDebug(planScriptingDebugArea())<<"Nothing to copy from";
        return 0;
    }
    c = kplatoProject()->calendar( copyfrom->id() );
    if ( c ) {
        kDebug(planScriptingDebugArea())<<"Calendar already exists";
        return 0;
    }
    Calendar *par = qobject_cast<Calendar*>( parent );
    KPlato::Calendar *p = 0;
    if ( par ) {
        p = kplatoProject()->calendar( par->id() );
    }
    c = new KPlato::Calendar();
    if ( copyfrom ) {
        *c = *copyfrom;
        c->setId( copyfrom->id() ); // NOTE: id is not copied
    }
    m_calendarModel.insertCalendar( c, -1, p );
    Calendar *call = this->calendar( c );
    kDebug(planScriptingDebugArea())<<call;
    return call;
}

QObject *Scripting::Project::createCalendar( QObject *parent )
{
    kDebug(planScriptingDebugArea())<<this<<parent;
    Calendar *par = qobject_cast<Calendar*>( parent );
    KPlato::Calendar *p = 0;
    if ( par ) {
        p = kplatoProject()->calendar( par->id() );
    }
    KPlato::Calendar *c = new KPlato::Calendar();
    m_calendarModel.insertCalendar( c, -1, p );
    return this->calendar( c );
}

Scripting::Calendar *Scripting::Project::calendar( KPlato::Calendar *calendar )
{
    if ( calendar == 0 ) {
        return 0;
    }
    if ( ! m_calendars.contains( calendar ) ) {
        m_calendars[ calendar ] = new Calendar( this, calendar, parent() );
    }
    return m_calendars[ calendar ];
}

QVariant Scripting::Project::calendarData(const KPlato::Calendar* calendar, const QString& property, const QString& role, long int )
{
    QModelIndex idx = m_calendarModel.index( calendar );
    idx = m_calendarModel.index( idx.row(), calendarColumnNumber( property ), idx.parent() );
    if ( ! idx.isValid() ) {
        return QVariant();
    }
    int r = stringToRole( role, m_calendarprogramroles.value( idx.column() ) );
    if ( r < 0 ) {
        return QVariant(); // invalid role
    }
    kDebug(planScriptingDebugArea())<<"data:"<<calendar<<property<<role<<":"<<idx<<m_calendarModel.data( idx, r );
    return m_calendarModel.data( idx, r );
}

QVariant Scripting::Project::setCalendarData( KPlato::Calendar *calendar, const QString &property, const QVariant &data, const QString &role )
{
    QModelIndex idx = m_calendarModel.index( calendar, calendarColumnNumber( property ) );
    if ( ! idx.isValid() ) {
        return "Invalid";
    }
    if ( ( m_calendarModel.flags( idx ) & Qt::ItemIsEditable ) == 0 ) {
        return "ReadOnly";
    }
    int datarole = stringToRole( role, Qt::EditRole );
    if ( datarole < 0 ) {
        return "Invalid role: " + role;
    }
    if ( calendarData( calendar, property, datarole == Qt::EditRole ? "ProgramRole" : role ) == data ) {
        return "Success";
    }
    return m_calendarModel.setData( idx, data, datarole ) ? "Success" : "Error";
}

int Scripting::Project::calendarColumnNumber(const QString& property) const
{
    int col = m_calendarModel.columnNumber( property );
    kDebug(planScriptingDebugArea())<<"calendarColumnNumber:"<<property<<"="<<col;
    return col;
}

QVariant Scripting::Project::calendarHeaderData( const QString &property, const QString &role )
{
    int col = calendarColumnNumber( property );
    return m_calendarModel.headerData( col, Qt::Horizontal, stringToRole( role ) );
}

QStringList Scripting::Project::calendarPropertyList()
{
    //FIXME: fake this for now
    QStringList lst;
    lst << "Name" << "TimeZone" << "Weekday" << "Date";
    return lst;
}


//-----------------------
int Scripting::Project::accountCount() const
{
    return kplatoProject()->accounts().accountCount();
}

QObject *Scripting::Project::accountAt( int index )
{
    return account( kplatoProject()->accounts().accountAt( index ) );
}

QObject *Scripting::Project::findAccount( const QString &id )
{
    KPlato::Account *a = kplatoProject()->accounts().findAccount( id );
    kDebug(planScriptingDebugArea())<<id<<a;
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

QObject *Scripting::Project::createAccount( QObject *parent )
{
    Account *par = qobject_cast<Account*>( parent );
    KPlato::Account *p = par ? par->kplatoAccount() : 0;
    KPlato::Account *a = new KPlato::Account();
    AddAccountCmd *cmd = new AddAccountCmd( *kplatoProject(), a, p );
    m_module->addCommand( cmd );
    return account( a );
}

QVariant Scripting::Project::accountHeaderData( const QString &property, const QString &role )
{
    int col = accountColumnNumber( property );
    return m_accountModel.headerData( col, Qt::Horizontal, stringToRole( role ) );
}

QStringList Scripting::Project::accountPropertyList()
{
    QStringList lst;
    QMetaEnum e = m_accountModel.columnMap();
    for ( int i = 0; i < e.keyCount(); ++i ) {
        lst << QString( e.key( i ) );
    }
    return lst;
}


int Scripting::Project::accountColumnNumber( const QString &property ) const
{
    return m_accountModel.columnMap().keyToValue( property.toUtf8() );
}

QVariant Scripting::Project::setAccountData( KPlato::Account *account, const QString &property, const QVariant &data, const QString &role )
{
    QModelIndex idx = m_accountModel.index( account, accountColumnNumber( property ) );
     if ( ! idx.isValid() ) {
        return "Invalid";
    }
    if ( ( m_accountModel.flags( idx ) & Qt::ItemIsEditable ) == 0 ) {
        return "ReadOnly";
    }
    int datarole = stringToRole( role, Qt::EditRole );
    if ( datarole < 0 ) {
        return "Invalid role: " + role;
    }
    if ( accountData( account, property, datarole == Qt::EditRole ? "ProgramRole" : role ) == data ) {
        return "Success";
    }
    return m_accountModel.setData( idx, data, datarole ) ? "Success" : "Error";
}

QVariant Scripting::Project::accountData( const KPlato::Account *account, const QString &property, const QString &role, long /*schedule*/ )
{
    QModelIndex idx = m_accountModel.index( account );
    idx = m_accountModel.index( idx.row(), accountColumnNumber( property ), idx.parent() );
    if ( ! idx.isValid() ) {
        return QVariant();
    }
    int r = stringToRole( role );
    if ( r < 0 ) {
        return QVariant();
    }
    return m_accountModel.data( idx, r );
}

int Scripting::Project::stringToRole( const QString &role, int programrole ) const
{
    if ( role == "ProgramRole" ) {
        return programrole;
    }
    const QMetaEnum e = metaObject()->enumerator( metaObject()->indexOfEnumerator("Roles") );
    return e.keyToValue( role.toUtf8() );
}

void Scripting::Project::addCommand( KUndo2Command *cmd )
{
    slotAddCommand( cmd );
}

void Scripting::Project::slotAddCommand( KUndo2Command *cmd )
{
    m_module->addCommand( cmd );
}


#include "Project.moc"
