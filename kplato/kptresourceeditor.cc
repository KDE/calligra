/* This file is part of the KDE project
  Copyright (C) 2006 Dag Andersen kplato@kde.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation;
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#include "kptresourceeditor.h"

#include "kptcommand.h"
#include "kptitemmodelbase.h"
#include "kptcalendar.h"
#include "kptduration.h"
#include "kptfactory.h"
#include "kptresourceappointmentsview.h"
#include "kptview.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"
#include "kptdatetime.h"
#include "kptcontext.h"

#include <QMenu>
#include <QPainter>
#include <QStyle>
#include <QList>
#include <QHeaderView>
#include <QObject>
#include <QTreeWidget>
#include <QStringList>
#include <QVBoxLayout>


#include <kicon.h>
#include <kglobal.h>
#include <klocale.h>
#include <kprinter.h>
#include <kxmlguifactory.h>

#include <kdebug.h>

namespace KPlato
{

ResourceItemModel::ResourceItemModel( Part *part, QObject *parent )
    : ItemModelBase( part, parent )
{
}

ResourceItemModel::~ResourceItemModel()
{
}

void ResourceItemModel::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( resourceChanged( Resource* ) ), this, SLOT( slotResourceChanged( Resource* ) ) );
        disconnect( m_project, SIGNAL( resourceGroupChanged( ResourceGroup* ) ), this, SLOT( slotResourceGroupChanged( Resource* ) ) );
        
        disconnect( m_project, SIGNAL( resourceGroupToBeAdded( const ResourceGroup* ) ), this, SLOT( slotLayoutToBeChanged() ) );
        
        disconnect( m_project, SIGNAL( resourceGroupToBeRemoved( const ResourceGroup* ) ), this, SLOT( slotLayoutToBeChanged() ) );
        
        disconnect( m_project, SIGNAL( resourceToBeAdded( const ResourceGroup*, const Resource* ) ), this, SLOT( slotLayoutToBeChanged() ) );
        
        disconnect( m_project, SIGNAL( resourceToBeRemoved( const ResourceGroup*, const Resource* ) ), this, SLOT( slotLayoutToBeChanged() ) );
        
        disconnect( m_project, SIGNAL( resourceGroupAdded( const ResourceGroup* ) ), this, SLOT( slotLayoutChanged() ) );
        
        disconnect( m_project, SIGNAL( resourceGroupRemoved( const ResourceGroup* ) ), this, SLOT( slotLayoutChanged() ) );
        
        disconnect( m_project, SIGNAL( resourceAdded( const ResourceGroup*, const Resource* ) ), this, SLOT( slotLayoutChanged() ) );
        
        disconnect( m_project, SIGNAL( resourceRemoved( const ResourceGroup*, const Resource* ) ), this, SLOT( slotLayoutChanged() ) );
        
    }
    m_project = project;
    connect( m_project, SIGNAL( resourceChanged( Resource* ) ), this, SLOT( slotResourceChanged( Resource* ) ) );
    connect( m_project, SIGNAL( resourceGroupChanged( ResourceGroup* ) ), this, SLOT( slotResourceGroupChanged( Resource* ) ) );

    connect( m_project, SIGNAL( resourceGroupToBeAdded( const ResourceGroup* ) ), this, SLOT( slotLayoutToBeChanged() ) );
        
    connect( m_project, SIGNAL( resourceGroupToBeRemoved( const ResourceGroup* ) ), this, SLOT( slotLayoutToBeChanged() ) );
        
    connect( m_project, SIGNAL( resourceToBeAdded( const ResourceGroup*, const Resource* ) ), this, SLOT( slotLayoutToBeChanged() ) );
        
    connect( m_project, SIGNAL( resourceToBeRemoved( const ResourceGroup*, const Resource* ) ), this, SLOT( slotLayoutToBeChanged() ) );
        
    connect( m_project, SIGNAL( resourceGroupAdded( const ResourceGroup* ) ), this, SLOT( slotLayoutChanged() ) );
        
    connect( m_project, SIGNAL( resourceGroupRemoved( const ResourceGroup* ) ), this, SLOT( slotLayoutChanged() ) );
        
    connect( m_project, SIGNAL( resourceAdded( const ResourceGroup*, const Resource* ) ), this, SLOT( slotLayoutChanged() ) );
        
    connect( m_project, SIGNAL( resourceRemoved( const ResourceGroup*, const Resource* ) ), this, SLOT( slotLayoutChanged() ) );
}

Qt::ItemFlags ResourceItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags( index );
    if ( !index.isValid() )
        return flags;
    if ( !m_readWrite ) {
        return flags &= ~Qt::ItemIsEditable;
    }
    if ( qobject_cast<Resource*>( object ( index ) ) ) {
        switch ( index.column() ) {
            default: flags |= Qt::ItemIsEditable;
        }
    } else if ( qobject_cast<ResourceGroup*>( object( index ) ) ) {
        switch ( index.column() ) {
            case 0: flags |= Qt::ItemIsEditable; break;
            case 1: flags |= Qt::ItemIsEditable; break;
            default: flags &= ~Qt::ItemIsEditable;
        }
    }
    return flags;
}


QModelIndex ResourceItemModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() || m_project == 0 ) {
        return QModelIndex();
    }
    //kDebug()<<k_funcinfo<<index.internalPointer()<<": "<<index.row()<<", "<<index.column()<<endl;
    QObject *p = object( index )->parent();
    if ( p == 0 ) {
        return QModelIndex();
    }
    ResourceGroup *g = qobject_cast<ResourceGroup*>( p );
    if ( g ) {
        int row = m_project->indexOf(  g );
        return createIndex( row, 0, g );
    }
    
    return QModelIndex();
}

bool ResourceItemModel::hasChildren( const QModelIndex &parent ) const
{
    //kDebug()<<k_funcinfo<<parent.internalPointer()<<": "<<parent.row()<<", "<<parent.column()<<endl;
    if ( m_project == 0 ) {
        return false;
    }
    QObject *p = object( parent );
    if ( p == 0 ) {
        return m_project->numResourceGroups() > 0;
    }
    ResourceGroup *g = qobject_cast<ResourceGroup*>( p );
    if ( g ) {
        return g->numResources() > 0;
    }
    return false;
}

QModelIndex ResourceItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_project == 0 || column < 0 || column >= columnCount() || row < 0 ) {
        return QModelIndex();
    }
    QObject *p = object( parent );
    if ( p == 0 ) {
        if ( row < m_project->numResourceGroups() ) {
            return createIndex( row, column, m_project->resourceGroupAt( row ) );
        }
        return QModelIndex();
    }
    ResourceGroup *g = qobject_cast<ResourceGroup*>( p );
    if ( g ) {
        if ( row < g->numResources() ) {
            return createIndex( row, column, g->resourceAt( row ) );
        }
        return QModelIndex();
    }
    return QModelIndex();
}

int ResourceItemModel::columnCount( const QModelIndex &parent ) const
{
    return 9;
}

int ResourceItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( m_project == 0 ) {
        return 0;
    }
    QObject *p = object( parent );
    if ( p == 0 ) {
        return m_project->numResourceGroups();
    }
    ResourceGroup *g = qobject_cast<ResourceGroup*>( p );
    if ( g ) {
        return g->numResources();
    }
    return 0;
}

bool ResourceItemModel::insertRows( int row, int count, const QModelIndex &parent )
{
//TODO
    return false;
}

bool ResourceItemModel::removeRows( int row, int count, const QModelIndex &parent )
{
//TODO
    return false;
}

QVariant ResourceItemModel::name( const Resource *res, int role ) const
{
    //kDebug()<<k_funcinfo<<res->name()<<", "<<role<<endl;
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

QVariant ResourceItemModel::name( const  ResourceGroup *res, int role ) const
{
    //kDebug()<<k_funcinfo<<res->name()<<", "<<role<<endl;
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

bool ResourceItemModel::setName( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand( new ModifyResourceNameCmd( m_part, res, value.toString(), "Modify resource name" ) );
            return true;
    }
    return false;
}

bool ResourceItemModel::setName( ResourceGroup *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand( new ModifyResourceGroupNameCmd( m_part, res, value.toString(), "Modify resourcegroup name" ) );
            return true;
    }
    return false;
}

QVariant ResourceItemModel::type( const Resource *res, int role ) const
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

QVariant ResourceItemModel::type( const ResourceGroup *res, int role ) const
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

bool ResourceItemModel::setType( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            Resource::Type v = static_cast<Resource::Type>( value.toInt() );
            m_part->addCommand( new ModifyResourceTypeCmd( m_part, res, v, "Modify resource type" ) );
            return true;
    }
    return false;
}

bool ResourceItemModel::setType( ResourceGroup *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            ResourceGroup::Type v = static_cast<ResourceGroup::Type>( value.toInt() );
            m_part->addCommand( new ModifyResourceGroupTypeCmd( m_part, res, v, "Modify resourcegroup type" ) );
            return true;
    }
    return false;
}

QVariant ResourceItemModel::initials( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return res->initials();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ResourceItemModel::setInitials( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand( new ModifyResourceInitialsCmd( m_part, res, value.toString(), "Modify resource initials" ) );
            return true;
    }
    return false;
}

QVariant ResourceItemModel::email( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return res->email();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ResourceItemModel::setEmail( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand( new ModifyResourceEmailCmd( m_part, res, value.toString(), "Modify resource email" ) );
            return true;
    }
    return false;
}

QVariant ResourceItemModel::units( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return res->units();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::ToolTipRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ResourceItemModel::setUnits( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand( new ModifyResourceUnitsCmd( m_part, res, value.toInt(), "Modify resource available units" ) );
            return true;
    }
    return false;
}

QVariant ResourceItemModel::availableFrom( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return KGlobal::locale()->formatDateTime( res->availableFrom() );
        case Qt::EditRole:
            return res->availableFrom();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatDateTime( res->availableFrom() );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ResourceItemModel::setAvailableFrom( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand( new ModifyResourceAvailableFromCmd( m_part, res, value.toDateTime(), "Modify resource available from" ) );
            return true;
    }
    return false;
}
    
QVariant ResourceItemModel::availableUntil( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return KGlobal::locale()->formatDateTime( res->availableUntil() );
        case Qt::EditRole:
            return res->availableUntil();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatDateTime( res->availableUntil() );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ResourceItemModel::setAvailableUntil( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand( new ModifyResourceAvailableUntilCmd( m_part, res, value.toDateTime(), "Modify resource available until" ) );
            return true;
    }
    return false;
}

QVariant ResourceItemModel::normalRate( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return res->normalRate();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::ToolTipRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ResourceItemModel::setNormalRate( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand( new ModifyResourceNormalRateCmd( m_part, res, value.toDouble(), "Modify resource normal rate" ) );
            return true;
    }
    return false;
}


QVariant ResourceItemModel::overtimeRate( const Resource *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return res->overtimeRate();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::ToolTipRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool ResourceItemModel::setOvertimeRate( Resource *res, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            m_part->addCommand( new ModifyResourceOvertimeRateCmd( m_part, res, value.toDouble(), "Modify resource overtime rate" ) );
            return true;
    }
    return false;
}


QVariant ResourceItemModel::notUsed( const ResourceGroup *res, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return QString(" ");
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::ToolTipRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant ResourceItemModel::data( const QModelIndex &index, int role ) const
{
    QVariant result;
    QObject *obj = object( index );
    if ( obj == 0 ) {
        return QVariant();
    }
    Resource *r = qobject_cast<Resource*>( obj );
    if ( r ) {
        switch ( index.column() ) {
            case 0: result = name( r, role ); break;
            case 1: result = type( r, role ); break;
            case 2: result = initials( r, role ); break;
            case 3: result = email( r, role ); break;
            case 4: result = units( r, role ); break;
            case 5: result = availableFrom( r, role ); break;
            case 6: result = availableUntil( r, role ); break;
            case 7: result = normalRate( r, role ); break;
            case 8: result = overtimeRate( r, role ); break;
            default:
                kDebug()<<k_funcinfo<<"data: invalid display value column "<<index.column()<<endl;;
                return QVariant();
        }
        if ( result.isValid() ) {
            if ( result.type() == QVariant::String && result.toString().isEmpty()) {
                // HACK to show focus in empty cells
                result = " ";
            }
            return result;
        }
    } else { 
        ResourceGroup *g = qobject_cast<ResourceGroup*>( obj );
        if ( g ) {
            switch ( index.column() ) {
                case 0: result = name( g, role ); break;
                case 1: result = type( g, role ); break;
                default:
                    if ( index.column() < columnCount() ) {
                        result = notUsed( g, role );
                    } else {
                        kDebug()<<k_funcinfo<<"data: invalid display value column "<<index.column()<<endl;;
                        return QVariant();
                    }
            }
            if ( result.isValid() ) {
                if ( result.type() == QVariant::String && result.toString().isEmpty()) {
                // HACK to show focus in empty cells
                    result = " ";
                }
                return result;
            }
        }
    }
    // define default action

    return QVariant();
}

bool ResourceItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( ( flags( index ) &Qt::ItemIsEditable ) == 0 || role != Qt::EditRole ) {
        return false;
    }
    QObject *obj = object( index );
    Resource *r = qobject_cast<Resource*>( obj );
    if ( r ) {
        switch (index.column()) {
            case 0: return setName( r, value, role );
            case 1: return setType( r, value, role );
            case 2: return setInitials( r, value, role );
            case 3: return setEmail( r, value, role );
            case 4: return setUnits( r, value, role );
            case 5: return setAvailableFrom( r, value, role );
            case 6: return setAvailableUntil( r, value, role );
            case 7: return setNormalRate( r, value, role );
            case 8: return setOvertimeRate( r, value, role );
            default:
                qWarning("data: invalid display value column %d", index.column());
                return false;
        }
    } else { 
        ResourceGroup *g = qobject_cast<ResourceGroup*>( obj );
        if ( g ) {
            switch (index.column()) {
                case 0: return setName( g, value, role );
                case 1: return setType( g, value, role );
                default:
                    qWarning("data: invalid display value column %d", index.column());
                    return false;
            }
        }
    }
    return false;
}

QVariant ResourceItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            switch ( section ) {
                case 0: return i18n( "Name" );
                case 1: return i18n( "Type" );
                case 2: return i18n( "Initials" );
                case 3: return i18n( "Email" );
                case 4: return i18n( "Limit (%)" );
                case 5: return i18n( "Available From" );
                case 6: return i18n( "Available Until" );
                case 7: return i18n( "Normal Rate" );
                case 8: return i18n( "Overtime Rate" );
                case 9: return i18n( "Fixed Cost" );
                default: return QVariant();
            }
        } else if ( role == Qt::TextAlignmentRole ) {
            switch (section) {
                case 1: return Qt::AlignCenter;
                case 2: return Qt::AlignCenter;
                case 3: return Qt::AlignCenter;
                case 4: return Qt::AlignCenter;
                case 5: return Qt::AlignCenter;
                case 6: return Qt::AlignCenter;
                case 7: return Qt::AlignCenter;
                case 8: return Qt::AlignCenter;
                case 9: return Qt::AlignCenter;
                default: return QVariant();
            }
        }
    }
    if ( role == Qt::ToolTipRole ) {
        switch ( section ) {
            case 0: return ToolTip::ResourceName;
            case 1: return ToolTip::ResourceType;
            case 2: return ToolTip::ResourceInitials;
            case 3: return ToolTip::ResourceEMail;
            case 4: return ToolTip::ResourceUnits;
            case 5: return ToolTip::ResourceAvailableFrom;
            case 6: return ToolTip::ResourceAvailableUntil;
            case 7: return ToolTip::ResourceNormalRate;
            case 8: return ToolTip::ResourceOvertimeRate;
            default: return QVariant();
        }
    }
    return ItemModelBase::headerData(section, orientation, role);
}

void ResourceItemModel::sort( int column, Qt::SortOrder order )
{
}

QMimeData * ResourceItemModel::mimeData( const QModelIndexList &indexes ) const
{
    return 0;
}

QStringList ResourceItemModel::mimeTypes () const
{
    return QStringList();
}

QObject *ResourceItemModel::object( const QModelIndex &index ) const
{
    QObject *o = 0;
    if ( index.isValid() ) {
        o = static_cast<QObject*>( index.internalPointer() );
        Q_ASSERT( o );
    }
    return o;
}

void ResourceItemModel::slotResourceChanged( Resource *res )
{
    QObject *par = res->parent();
    if ( par ) {
        ResourceGroup *g = qobject_cast<ResourceGroup*>( par );
        if ( g ) {
            int row = g->indexOf( res );
            emit dataChanged( createIndex( row, 0, res ), createIndex( row, columnCount() - 1, res ) );
            return;
        }
    }
}

void ResourceItemModel::slotResourceGroupChanged( ResourceGroup *res )
{
    Project *p = res->project();
    if ( p ) {
        int row = p->resourceGroups().indexOf( res );
        emit dataChanged( createIndex( row, 0, res ), createIndex( row, columnCount() - 1, res ) );
    }
}

//--------------------
ResourceTreeView::ResourceTreeView( Part *part, QWidget *parent )
    : QTreeView( parent ),
    m_arrowKeyNavigation( true )
{
    header()->setContextMenuPolicy( Qt::CustomContextMenu );
    setModel( new ResourceItemModel( part ) );
    setSelectionModel( new QItemSelectionModel( model() ) );
    setSelectionMode( QAbstractItemView::ExtendedSelection );

    setItemDelegateForColumn( 1, new EnumDelegate( this ) );

    connect( header(), SIGNAL( customContextMenuRequested ( const QPoint& ) ), this, SLOT( headerContextMenuRequested( const QPoint& ) ) );
    connect( this, SIGNAL( activated ( const QModelIndex ) ), this, SLOT( slotActivated( const QModelIndex ) ) );


}

void ResourceTreeView::slotActivated( const QModelIndex index )
{
    kDebug()<<k_funcinfo<<index.column()<<endl;
}

void ResourceTreeView::headerContextMenuRequested( const QPoint &pos )
{
    kDebug()<<k_funcinfo<<header()->logicalIndexAt(pos)<<" at "<<pos<<endl;
}

void ResourceTreeView::contextMenuEvent ( QContextMenuEvent *event )
{
    kDebug()<<k_funcinfo<<endl;
    emit contextMenuRequested( indexAt(event->pos()), event->globalPos() );
}

void ResourceTreeView::selectionChanged( const QItemSelection &sel, const QItemSelection &desel )
{
    kDebug()<<k_funcinfo<<sel.indexes().count()<<endl;
    foreach( QModelIndex i, selectionModel()->selectedIndexes() ) {
        kDebug()<<k_funcinfo<<i.row()<<", "<<i.column()<<endl;
    }
    QTreeView::selectionChanged( sel, desel );
    emit selectionChanged( selectionModel()->selectedIndexes() );
}

void ResourceTreeView::currentChanged( const QModelIndex & current, const QModelIndex & previous )
{
    kDebug()<<k_funcinfo<<endl;
    QTreeView::currentChanged( current, previous );
    emit currentChanged( current );
}

QObject *ResourceTreeView::currentObject() const
{
    return itemModel()->object( currentIndex() );
}

/*!
    \reimp
 */
void ResourceTreeView::keyPressEvent(QKeyEvent *event)
{
    kDebug()<<k_funcinfo<<event->key()<<", "<<m_arrowKeyNavigation<<endl;
    if ( !m_arrowKeyNavigation ) {
        QTreeView::keyPressEvent( event );
        return;
    }
    QModelIndex current = currentIndex();
    if ( current.isValid() ) {
        switch (event->key()) {
            case Qt::Key_Right: {
                if ( current.column() < model()->columnCount() - 1 ) {
                    QModelIndex i = model()->index( current.row(), current.column() + 1, current.parent() );
                    selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
                }
                event->accept();
                return;
                break;
            }
            case Qt::Key_Left: {
                if ( current.column() > 0 ) {
                    QModelIndex i = model()->index( current.row(), current.column() - 1, current.parent() );
                    selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
                }
                event->accept();
                return;
                break;
            }
            case Qt::Key_Down: {
                QModelIndex i = indexBelow( current );
                if ( i.isValid() ) {
                    i = model()->index( i.row(), current.column(), i.parent() );
                    selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
                }
                event->accept();
                return;
                break;
            }
            case Qt::Key_Up: {
                QModelIndex i = indexAbove( current );
                if ( i.isValid() ) {
                    i = model()->index( i.row(), current.column(), i.parent() );
                    selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
                }
                event->accept();
                return;
                break;
            }
            case Qt::Key_Plus:
                if ( itemsExpandable()) {
                    if ( model()->hasChildren( current ) ) {
                        QTreeView::keyPressEvent( event );
                    //HACK: Bug in qt??
                        selectionModel()->setCurrentIndex(current, QItemSelectionModel::NoUpdate);
                    }
                    event->accept();
                    return;
                }
                break;
            case Qt::Key_Minus:
                if ( itemsExpandable() ) {
                    if ( model()->hasChildren( current ) ) {
                        QTreeView::keyPressEvent( event );
                    //HACK: Bug in qt??
                        selectionModel()->setCurrentIndex(current, QItemSelectionModel::NoUpdate);
                    }
                    event->accept();
                    return;
                }
                break;
        }
    }
    QTreeView::keyPressEvent(event);
}

QItemSelectionModel::SelectionFlags ResourceTreeView::selectionCommand(const QModelIndex &index, const QEvent *event) const
{
    /*    if ( event && event->type() == QEvent::KeyPress && selectionMode() == QAbstractItemView::ExtendedSelection ) {
        if ( static_cast<const QKeyEvent*>(event)->key() == Qt::Key_Space ) {
        Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
        QItemSelectionModel::SelectionFlags bflags = QItemSelectionModel::Rows;
        // 
        if ( modifiers && Qt::ShiftModifier ) {
        return QItemSelectionModel::SelectCurrent|bflags;
    }
    // Toggle on Ctrl-Qt::Key_Space
        if ( modifiers & Qt::ControlModifier ) {
        return QItemSelectionModel::Toggle|bflags;
    }
    // Select on Space alone
        return QItemSelectionModel::ClearAndSelect|bflags;
    }
    }*/
    return QTreeView::selectionCommand( index, event );
}

//-----------------------------------
ResourceEditor::ResourceEditor( Part *part, QWidget *parent )
    : ViewBase( part, parent )
{
    setupGui();
    
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_editor = new ResourceTreeView( part, this );
    l->addWidget( m_editor );
    m_editor->setEditTriggers( m_editor->editTriggers() | QAbstractItemView::EditKeyPressed );

    connect( m_editor, SIGNAL( currentChanged( QModelIndex ) ), this, SLOT( slotCurrentChanged( QModelIndex ) ) );

    connect( m_editor, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT( slotSelectionChanged( const QModelIndexList ) ) );
    
    connect( m_editor, SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), this, SLOT( slotContextMenuRequested( QModelIndex, const QPoint& ) ) );
}

void ResourceEditor::draw( Project &project )
{
    m_editor->setProject( &project );
}

void ResourceEditor::draw()
{
}

void ResourceEditor::setGuiActive( bool activate )
{
    kDebug()<<k_funcinfo<<activate<<endl;
    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
    if ( activate && !m_editor->currentIndex().isValid() ) {
        m_editor->selectionModel()->setCurrentIndex(m_editor->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
    }
}

void ResourceEditor::slotContextMenuRequested( QModelIndex index, const QPoint& pos )
{
    kDebug()<<k_funcinfo<<index.row()<<", "<<index.column()<<": "<<pos<<endl;
    QString name;
    if ( index.isValid() ) {
        QObject *obj = m_editor->itemModel()->object( index );
        ResourceGroup *g = qobject_cast<ResourceGroup*>( obj );
        if ( g ) {
            name = "resourceeditor_group_popup";
        } else {
            Resource *r = qobject_cast<Resource*>( obj );
            if ( r ) {
                name = "resourceeditor_resource_popup";
            }
        }
    }
    kDebug()<<k_funcinfo<<name<<endl;
    emit requestPopupMenu( name, pos );
}

Resource *ResourceEditor::currentResource() const
{
    return qobject_cast<Resource*>( m_editor->currentObject() );
}

ResourceGroup *ResourceEditor::currentResourceGroup() const
{
    return qobject_cast<ResourceGroup*>( m_editor->currentObject() );
}

void ResourceEditor::slotCurrentChanged(  const QModelIndex &curr )
{
    kDebug()<<k_funcinfo<<curr.row()<<", "<<curr.column()<<endl;
    slotEnableActions();
}

void ResourceEditor::slotSelectionChanged( const QModelIndexList list)
{
    kDebug()<<k_funcinfo<<list.count()<<endl;
    bool res = false, grp = false;
    foreach ( QModelIndex i, list ) {
        Resource *r = qobject_cast<Resource*>( m_editor->itemModel()->object( i ) );
        if ( r ) {
            res = true;
            continue;
        }
        ResourceGroup *g = qobject_cast<ResourceGroup*>( m_editor->itemModel()->object( i ) );
        if ( r ) {
            grp = true;
        }
    }
    slotEnableActions( res, grp );
}

void ResourceEditor::slotEnableActions( bool resource, bool group )
{
    updateActionsEnabled( true );
}

void ResourceEditor::updateActionsEnabled(  bool resource, bool group )
{
    Project *p = m_editor->project();

    actionAddResource->setEnabled( resource );
    actionAddGroup->setEnabled( resource );
    actionDeleteSelection->setEnabled( resource || group );

}

void ResourceEditor::setupGui()
{
    KActionCollection *coll = actionCollection();
    
    QString name = "resourceeditor_edit_list";
    actionAddResource = new KAction( KIcon( "filenew" ), i18n( "Add Resource..." ), coll, "add_resource" );
    connect( actionAddResource, SIGNAL( triggered( bool ) ), SLOT( slotAddResource() ) );
    addAction( name, actionAddResource );
    
    actionAddGroup = new KAction( KIcon( "filenew" ), i18n( "Add Resource Group..." ), coll, "add_group" );
    connect( actionAddGroup, SIGNAL( triggered( bool ) ), SLOT( slotAddGroup() ) );
    addAction( name, actionAddGroup );
    
    actionDeleteSelection = new KAction( KIcon( "editdelete" ), i18n( "Delete Selected Items" ), coll, "delete_selection" );
    connect( actionDeleteSelection, SIGNAL( triggered( bool ) ), SLOT( slotDeleteSelection() ) );
    addAction( name, actionDeleteSelection );
    
}

void ResourceEditor::slotAddResource()
{
    kDebug()<<k_funcinfo<<endl;
    /*    emit addResource();*/
}

void ResourceEditor::slotAddGroup()
{
    kDebug()<<k_funcinfo<<endl;
    /*    emit addResourceGroup();*/
}

void ResourceEditor::slotDeleteSelection()
{
    kDebug()<<k_funcinfo<<endl;
    /*    emit deleteTaskList( selectedResources() );*/
}


} // namespace KPlato

#include "kptresourceeditor.moc"
