/* This file is part of the KDE project
  Copyright (C) 2009 Dag Andersen kplato@kde.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include "kptresourceallocationmodel.h"

#include "kptcommand.h"
#include "kptitemmodelbase.h"
#include "kptcalendar.h"
#include "kptduration.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"
#include "kptdatetime.h"

#include <QMimeData>
#include <QObject>
#include <QStringList>

#include <kaction.h>
#include <kicon.h>
#include <kglobal.h>
#include <klocale.h>
#include <kactioncollection.h>
#include <kxmlguifactory.h>
#include <kabc/addressee.h>
#include <kabc/vcardconverter.h>

#include <kdebug.h>

namespace KPlato
{

//--------------------------------------

ResourceAllocationModel::ResourceAllocationModel( QObject *parent )
    : QObject( parent ),
    m_project( 0 ),
    m_task( 0 )
{
}

ResourceAllocationModel::~ResourceAllocationModel()
{
}

const QMetaEnum ResourceAllocationModel::columnMap() const
{
    return metaObject()->enumerator( metaObject()->indexOfEnumerator("Properties") );
}

void ResourceAllocationModel::setProject( Project *project )
{
    m_project = project;
}

void ResourceAllocationModel::setTask( Task *task )
{
    m_task = task;
}

int ResourceAllocationModel::propertyCount() const
{
    return columnMap().keyCount();
}

QVariant ResourceAllocationModel::name( const Resource *res, int role ) const
{
    //kDebug()<<res->name()<<","<<role;
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return res->name();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceAllocationModel::name( const  ResourceGroup *res, int role ) const
{
    //kDebug()<<res->name()<<","<<role;
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return res->name();
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceAllocationModel::type( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return res->typeToString( true );
        case Role::EnumList: 
            return res->typeToStringList( true );
        case Role::EnumListValue: 
            return (int)res->type();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceAllocationModel::type( const ResourceGroup *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return res->typeToString( true );
        case Role::EnumList: 
            return res->typeToStringList( true );
        case Role::EnumListValue: 
            return (int)res->type();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}


QVariant ResourceAllocationModel::allocation( const ResourceGroup *group, const Resource *res, int role ) const
{
    if ( m_task == 0 ) {
        return QVariant();
    }
    const ResourceGroupRequest *rg = m_task->requests().find( group );
    const ResourceRequest *rr = 0;
    if ( rg ) {
        rr = rg->find( res );
    }
    switch ( role ) {
        case Qt::DisplayRole: {
            int units = rr ? rr->units() : 0;
            // xgettext: no-c-format
            return i18nc( "<value>%", "%1%", units );
        }
        case Qt::EditRole:
            return rr ? rr->units() : 0;;
        case Qt::ToolTipRole: {
            int units = rr ? rr->units() : 0;
            if ( units == 0 ) {
                return i18n( "Not allocated" );
            }
            // xgettext: no-c-format
            return i18n( "Allocated units: %1%", units );
        }
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
        case Role::Minimum:
            return 0;
        case Role::Maximum:
            return 100;
    }
    return QVariant();
}

QVariant ResourceAllocationModel::allocation( const ResourceGroup *res, int role ) const
{
    if ( m_task == 0 ) {
        return QVariant();
    }
    //qDebug()<<"ResourceAllocationModel::allocation:"<<res->name()<<role;
    const ResourceGroupRequest *req = m_task->requests().find( res );
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return req ? req->units() : 0;
        case Qt::ToolTipRole:
            return QVariant();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
        case Role::Minimum:
            return 0;
        case Role::Maximum:
            return res->numResources();
    }
    return QVariant();
}

QVariant ResourceAllocationModel::maximum( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            // xgettext: no-c-format
            return i18nc( "<value>%", "%1%", res->units() );
        case Qt::EditRole:
            return res->units();
        case Qt::ToolTipRole:
            // xgettext: no-c-format
            return i18n( "Maximum units available: %1%", res->units() );
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceAllocationModel::maximum( const ResourceGroup *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return res->numResources();
        case Qt::ToolTipRole:
            return i18np( "There is %1 resource available in this group", "There are %1 resources available in this group", res->numResources() );
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceAllocationModel::data( const ResourceGroup *group, const Resource *resource, int property, int role ) const
{
    QVariant result;
    if ( resource == 0 ) {
        return result;
    }
    switch ( property ) {
        case RequestName: result = name( resource, role ); break;
        case RequestType: result = type( resource, role ); break;
        case RequestAllocation: result = allocation( group, resource, role ); break;
        case RequestMaximum: result = maximum( resource, role ); break;
        default:
            kDebug()<<"data: invalid display value: property="<<property;
            break;
    }
    return result;
}

QVariant ResourceAllocationModel::data( const ResourceGroup *group, int property, int role ) const
{
    QVariant result;
    if ( group == 0 ) {
        return result;
    }
    switch ( property ) {
        case RequestName: result = name( group, role ); break;
        case RequestType: result = type( group, role ); break;
        case RequestAllocation: result = allocation( group, role ); break;
        case RequestMaximum: result = maximum( group, role ); break;
        default:
            if ( role == Qt::DisplayRole ) {
                if ( property < propertyCount() ) {
                    result = "";
                } else {
                    kDebug()<<"data: invalid display value column"<<property;;
                    return QVariant();
                }
            }
            break;
    }
    return result;
}


QVariant ResourceAllocationModel::headerData( int section, int role )
{
    if ( role == Qt::DisplayRole ) {
        switch ( section ) {
            case RequestName: return i18n( "Name" );
            case RequestType: return i18n( "Type" );
            case RequestAllocation: return i18n( "Allocation" );
            case RequestMaximum: return i18n( "Maximum" );
            default: return QVariant();
        }
    } else if ( role == Qt::TextAlignmentRole ) {
        switch (section) {
            case 0: return QVariant();
            default: return Qt::AlignCenter;
        }
    } else if ( role == Qt::ToolTipRole ) {
        switch ( section ) {
            case RequestName: return ToolTip::resourceName();
            case RequestType: return ToolTip::resourceType();
            case RequestAllocation: return i18n( "Resource allocation" );
            case RequestMaximum: return i18n( "Maximum amount of units or resources that can be allocated" );
            default: return QVariant();
        }
    }
    return QVariant();
}

//--------------------------------------

ResourceAllocationItemModel::ResourceAllocationItemModel( QObject *parent )
    : ItemModelBase( parent )
{
}

ResourceAllocationItemModel::~ResourceAllocationItemModel()
{
}

void ResourceAllocationItemModel::slotResourceToBeInserted( const ResourceGroup *group, int row )
{
    //kDebug()<<group->name()<<","<<row;
    beginInsertRows( index( group ), row, row );
}

void ResourceAllocationItemModel::slotResourceInserted( const Resource *resource )
{
    //kDebug()<<resource->name();
    endInsertRows();
    emit layoutChanged(); //HACK to make the right view react! Bug in qt?
}

void ResourceAllocationItemModel::slotResourceToBeRemoved( const Resource *resource )
{
    //kDebug()<<resource->name();
    int row = index( resource ).row();
    beginRemoveRows( index( resource->parentGroup() ), row, row );
}

void ResourceAllocationItemModel::slotResourceRemoved( const Resource *resource )
{
    //kDebug()<<resource->name();
    endRemoveRows();
}

void ResourceAllocationItemModel::slotResourceGroupToBeInserted( const ResourceGroup *group, int row )
{
    //kDebug()<<group->name();
    beginInsertRows( QModelIndex(), row, row );
}

void ResourceAllocationItemModel::slotResourceGroupInserted( const ResourceGroup *group )
{
    //kDebug()<<group->name();
    endInsertRows();
}

void ResourceAllocationItemModel::slotResourceGroupToBeRemoved( const ResourceGroup *group )
{
    //kDebug()<<group->name();
    int row = index( group ).row();
    beginRemoveRows( QModelIndex(), row, row );
}

void ResourceAllocationItemModel::slotResourceGroupRemoved( const ResourceGroup *group )
{
    //kDebug()<<group->name();
    endRemoveRows();
}

void ResourceAllocationItemModel::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( resourceChanged( Resource* ) ), this, SLOT( slotResourceChanged( Resource* ) ) );
        disconnect( m_project, SIGNAL( resourceGroupChanged( ResourceGroup* ) ), this, SLOT( slotResourceGroupChanged( ResourceGroup* ) ) );
        
        disconnect( m_project, SIGNAL( resourceGroupToBeAdded( const ResourceGroup*, int ) ), this, SLOT( slotResourceGroupToBeInserted( const ResourceGroup*, int ) ) );
        
        disconnect( m_project, SIGNAL( resourceGroupToBeRemoved( const ResourceGroup* ) ), this, SLOT( slotResourceGroupToBeRemoved( const ResourceGroup* ) ) );
        
        disconnect( m_project, SIGNAL( resourceToBeAdded( const ResourceGroup*, int ) ), this, SLOT( slotResourceToBeInserted( const ResourceGroup*, int ) ) );
        
        disconnect( m_project, SIGNAL( resourceToBeRemoved( const Resource* ) ), this, SLOT( slotResourceToBeRemoved( const Resource* ) ) );
        
        disconnect( m_project, SIGNAL( resourceGroupAdded( const ResourceGroup* ) ), this, SLOT( slotResourceGroupInserted( const ResourceGroup* ) ) );
        
        disconnect( m_project, SIGNAL( resourceGroupRemoved( const ResourceGroup* ) ), this, SLOT( slotResourceGroupRemoved( const ResourceGroup* ) ) );
        
        disconnect( m_project, SIGNAL( resourceAdded( const Resource* ) ), this, SLOT( slotResourceInserted( const Resource* ) ) );
        
        disconnect( m_project, SIGNAL( resourceRemoved( const Resource* ) ), this, SLOT( slotResourceRemoved( const Resource* ) ) );
        
    }
    m_project = project;
    if ( m_project ) {
        connect( m_project, SIGNAL( resourceChanged( Resource* ) ), this, SLOT( slotResourceChanged( Resource* ) ) );
        connect( m_project, SIGNAL( resourceGroupChanged( ResourceGroup* ) ), this, SLOT( slotResourceGroupChanged( ResourceGroup* ) ) );
        
        connect( m_project, SIGNAL( resourceGroupToBeAdded( const ResourceGroup*, int ) ), this, SLOT( slotResourceGroupToBeInserted( const ResourceGroup*, int ) ) );
        
        connect( m_project, SIGNAL( resourceGroupToBeRemoved( const ResourceGroup* ) ), this, SLOT( slotResourceGroupToBeRemoved( const ResourceGroup* ) ) );
        
        connect( m_project, SIGNAL( resourceToBeAdded( const ResourceGroup*, int ) ), this, SLOT( slotResourceToBeInserted( const ResourceGroup*, int ) ) );
        
        connect( m_project, SIGNAL( resourceToBeRemoved( const Resource* ) ), this, SLOT( slotResourceToBeRemoved( const Resource* ) ) );
        
        connect( m_project, SIGNAL( resourceGroupAdded( const ResourceGroup* ) ), this, SLOT( slotResourceGroupInserted( const ResourceGroup* ) ) );
        
        connect( m_project, SIGNAL( resourceGroupRemoved( const ResourceGroup* ) ), this, SLOT( slotResourceGroupRemoved( const ResourceGroup* ) ) );
        
        connect( m_project, SIGNAL( resourceAdded( const Resource* ) ), this, SLOT( slotResourceInserted( const Resource* ) ) );
        
        connect( m_project, SIGNAL( resourceRemoved( const Resource* ) ), this, SLOT( slotResourceRemoved( const Resource* ) ) );
    
    }
    m_model.setProject( m_project );
}

void ResourceAllocationItemModel::setTask( Task *task )
{
    if ( task == m_model.task() ) {
        return;
    }
    if ( m_model.task() == 0 ) {
        filldata( task );
        m_model.setTask( task );
        reset();
        return;
    }
    if ( task ) {
        emit layoutAboutToBeChanged();
        filldata( task );
        m_model.setTask( task );
        emit layoutChanged();
    }
}

void ResourceAllocationItemModel::filldata( Task *task )
{
    m_resourceCache.clear();
    m_groupCache.clear();
    if ( task ) {
        foreach ( const ResourceGroup *g, m_project->resourceGroups() ) {
            const ResourceGroupRequest *gr = task->requests().find( g );
            m_groupCache[ g->id() ] = gr ? gr->units() : 0;
        }
        foreach ( const Resource *r, m_project->resourceList() ) {
            const ResourceRequest *rr = task->requests().find( r );
            m_resourceCache[ r->id() ] = rr ? rr->units() : 0;
        }
    }
}

Qt::ItemFlags ResourceAllocationItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = ItemModelBase::flags( index );
    if ( !m_readWrite ) {
        //kDebug()<<"read only"<<flags;
        return flags &= ~Qt::ItemIsEditable;
    }
    if ( !index.isValid() ) {
        //kDebug()<<"invalid"<<flags;
        return flags;
    }
    switch ( index.column() ) {
        case ResourceAllocationModel::RequestAllocation:
            flags |= ( Qt::ItemIsEditable | Qt::ItemIsUserCheckable );
            break;
        default:
            flags &= ~Qt::ItemIsEditable;
            break;
    }
    return flags;
}


QModelIndex ResourceAllocationItemModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() || m_project == 0 ) {
        return QModelIndex();
    }
    //kDebug()<<index.internalPointer()<<":"<<index.row()<<","<<index.column();

    Resource *r = qobject_cast<Resource*>( object( index ) );
    if ( r && r->parentGroup() ) {
        // only resources have parent
        int row = m_project->indexOf(  r->parentGroup() );
        return createIndex( row, 0, r->parentGroup() );
    }
    
    return QModelIndex();
}

QModelIndex ResourceAllocationItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_project == 0 || column < 0 || column >= columnCount() || row < 0 ) {
        return QModelIndex();
    }
    if ( ! parent.isValid() ) {
        if ( row < m_project->numResourceGroups() ) {
            return createIndex( row, column, m_project->resourceGroupAt( row ) );
        }
        return QModelIndex();
    }
    QObject *p = object( parent );
    ResourceGroup *g = qobject_cast<ResourceGroup*>( p );
    if ( g ) {
        if ( row < g->numResources() ) {
            return createIndex( row, column, g->resourceAt( row ) );
        }
        return QModelIndex();
    }
    return QModelIndex();
}

QModelIndex ResourceAllocationItemModel::index( const Resource *resource ) const
{
    if ( m_project == 0 || resource == 0 ) {
        return QModelIndex();
    }
    Resource *r = const_cast<Resource*>(resource);
    int row = -1;
    ResourceGroup *par = r->parentGroup();
    if ( par ) {
        row = par->indexOf( r );
        return createIndex( row, 0, r );
    }
    return QModelIndex();
}

QModelIndex ResourceAllocationItemModel::index( const ResourceGroup *group ) const
{
    if ( m_project == 0 || group == 0 ) {
        return QModelIndex();
    }
    ResourceGroup *g = const_cast<ResourceGroup*>(group);
    int row = m_project->indexOf( g );
    return createIndex( row, 0, g );

}

int ResourceAllocationItemModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    return m_model.propertyCount();
}

int ResourceAllocationItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( m_project == 0 || m_model.task() == 0 ) {
        return 0;
    }
    if ( ! parent.isValid() ) {
        return m_project->numResourceGroups();
    }
    QObject *p = object( parent );
    ResourceGroup *g = qobject_cast<ResourceGroup*>( p );
    if ( g ) {
        return g->numResources();
    }
    return 0;
}

QVariant ResourceAllocationItemModel::allocation( const ResourceGroup *group, const Resource *res, int role ) const
{
    if ( m_model.task() == 0 ) {
        return QVariant();
    }
    if ( ! m_resourceCache.contains( res->id() ) ) {
        return m_model.allocation( group, res, role );
    }
    switch ( role ) {
        case Qt::DisplayRole: {
            // xgettext: no-c-format
            return i18nc( "<value>%", "%1%", m_resourceCache[ res->id() ] );
        }
        case Qt::EditRole:
            return m_resourceCache[ res->id() ];
        case Qt::ToolTipRole: {
            return i18np( "Not allocated", "Allocated units: %2", m_resourceCache[ res->id() ], allocation( group, res, Qt::DisplayRole ).toString() );
        }
        case Qt::CheckStateRole:
            return m_resourceCache[ res->id() ] == 0 ? Qt::Unchecked : Qt::Checked;
        default:
            return m_model.allocation( group, res, role );
    }
    return QVariant();
}

QVariant ResourceAllocationItemModel::allocation( const ResourceGroup *res, int role ) const
{
    if ( m_model.task() == 0 ) {
        return QVariant();
    }
    if ( ! m_groupCache.contains( res->id() ) ) {
        return m_model.allocation( res, role );
    }
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return qMax( m_groupCache[ res->id() ], allocation( res, Role::Minimum ).toInt() );
        case Qt::ToolTipRole:
            return i18np( "%1 resource allocated", "%1 resources allocated", allocation( res, Qt::DisplayRole ).toInt() );
        case Role::Minimum: {
            return 0;
        }
        default:
            return m_model.allocation( res, role );
    }
    return QVariant();
}


bool ResourceAllocationItemModel::setAllocation( Resource *res, const QVariant &value, int role )
{
    //qDebug()<<"ResourceAllocationItemModel::setAllocation:"<<res->name()<<value<<role;
    switch ( role ) {
        case Qt::EditRole: {
            m_resourceCache[ res->id() ] = value.toInt();
            return true;
        }
        case Qt::CheckStateRole:
            if ( m_resourceCache[ res->id() ] == 0 ) {
                m_resourceCache[ res->id() ] = res->units();
            } else {
                m_resourceCache[ res->id() ] = 0;
            }
            return true;
    }
    return false;
}

bool ResourceAllocationItemModel::setAllocation( ResourceGroup *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            m_groupCache[ res->id() ] = value.toInt();
            return true;
    }
    return false;
}


QVariant ResourceAllocationItemModel::notUsed( const ResourceGroup *, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return QString(" ");
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::EditRole:
        case Qt::ToolTipRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceAllocationItemModel::data( const QModelIndex &index, int role ) const
{
    QVariant result;
    QObject *obj = object( index );
    if ( obj == 0 ) {
        return QVariant();
    }
    if ( role == Qt::TextAlignmentRole ) {
        // use same alignment as in header (headers always horizontal)
        return headerData( index.column(), Qt::Horizontal, role );
    }
    Resource *r = qobject_cast<Resource*>( obj );
    if ( r ) {
        if ( index.column() == ResourceAllocationModel::RequestAllocation ) {
            return allocation( r->parentGroup(), r, role );
        }
        result = m_model.data( r->parentGroup(), r, index.column(), role );
    } else {
        ResourceGroup *g = qobject_cast<ResourceGroup*>( obj );
        if ( g ) {
            if ( index.column() == ResourceAllocationModel::RequestAllocation ) {
                return allocation( g, role );
            }
            result = m_model.data( g, index.column(), role );
        }
    }
    if ( role == Qt::DisplayRole && ! result.isValid() ) {
        result = " "; // HACK to show focus in empty cells
    }
    return result;
}

bool ResourceAllocationItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( ! index.isValid() ) {
        return ItemModelBase::setData( index, value, role );
    }
    if ( ( flags( index ) & Qt::ItemIsEditable ) == 0 ) {
        return false;
    }
    QObject *obj = object( index );
    Resource *r = qobject_cast<Resource*>( obj );
    if ( r ) {
        switch (index.column()) {
            case ResourceAllocationModel::RequestAllocation: 
                if ( setAllocation( r, value, role ) ) {
                    emit dataChanged( index, index );
                    QModelIndex idx = this->index( index.row(), ResourceAllocationModel::RequestAllocation, parent( parent( index ) ) );
                    emit dataChanged( idx, idx );
                    return true;
                }
                return false;
            default:
                //qWarning("data: invalid display value column %d", index.column());
                return false;
        }
    }
    ResourceGroup *g = qobject_cast<ResourceGroup*>( obj );
    if ( g ) {
        switch (index.column()) {
            case ResourceAllocationModel::RequestAllocation:
                if ( setAllocation( g, value, role ) ) {
                    emit dataChanged( index, index );
                    return true;
                }
                return false;
            default:
                //qWarning("data: invalid display value column %d", index.column());
                return false;
        }
    }
    return false;
}

QVariant ResourceAllocationItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            return m_model.headerData( section, role );
        }
        if ( role == Qt::TextAlignmentRole ) {
            switch (section) {
                case 0: return QVariant();
                default: return Qt::AlignCenter;
            }
            return Qt::AlignCenter;
        }
    }
    if ( role == Qt::ToolTipRole ) {
        return m_model.headerData( section, role );
    }
    return ItemModelBase::headerData(section, orientation, role);
}

QAbstractItemDelegate *ResourceAllocationItemModel::createDelegate( int col, QWidget *parent ) const
{
    switch ( col ) {
        case ResourceAllocationModel::RequestAllocation: return new SpinBoxDelegate( parent );
        default: break;
    }
    return 0;
}

QObject *ResourceAllocationItemModel::object( const QModelIndex &index ) const
{
    QObject *o = 0;
    if ( index.isValid() ) {
        o = static_cast<QObject*>( index.internalPointer() );
        Q_ASSERT( o );
    }
    return o;
}

void ResourceAllocationItemModel::slotResourceChanged( Resource *res )
{
    ResourceGroup *g = res->parentGroup();
    if ( g ) {
        int row = g->indexOf( res );
        emit dataChanged( createIndex( row, 0, res ), createIndex( row, columnCount() - 1, res ) );
        return;
    }
}

void ResourceAllocationItemModel::slotResourceGroupChanged( ResourceGroup *res )
{
    Project *p = res->project();
    if ( p ) {
        int row = p->resourceGroups().indexOf( res );
        emit dataChanged( createIndex( row, 0, res ), createIndex( row, columnCount() - 1, res ) );
    }
}

} // namespace KPlato

#include "kptresourceallocationmodel.moc"
