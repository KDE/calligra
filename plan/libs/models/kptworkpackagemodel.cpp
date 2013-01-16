/* This file is part of the KDE project
  Copyright (C) 2007, 2012 Dag Andersen <danders@get2net.dk>

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
* Boston, MA 02110-1301, USA.
*/

#include "kptworkpackagemodel.h"

#include "kptglobal.h"
#include "kptcommonstrings.h"
#include "kptflatproxymodel.h"
#include "kptnodeitemmodel.h"
#include "kptnode.h"
#include "kpttask.h"
#include "kptproject.h"
#include "kptschedule.h"
#include "kptdebug.h"

#include <QModelIndex>
#include <QVariant>
#include <QRegExp>

namespace KPlato
{

QVariant WorkPackageModel::nodeName( const WorkPackage *wp, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return wp->parentTask() ? wp->parentTask()->name() : "";
        case Qt::EditRole:
            return wp->parentTask() ? wp->parentTask()->name() : "";
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}


QVariant WorkPackageModel::ownerName( const WorkPackage *wp, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return wp->ownerName();
        case Qt::EditRole:
            return wp->ownerName();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant WorkPackageModel::transmitionStatus( const WorkPackage *wp, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return wp->transmitionStatusToString( wp->transmitionStatus(), true );
        case Qt::EditRole:
            return wp->transmitionStatus();
        case Qt::ToolTipRole: {
            int sts = wp->transmitionStatus();
            if ( sts == WorkPackage::TS_Send ) {
                return i18n( "Sent to %1 at %2", wp->ownerName(), transmitionTime( wp, Qt::DisplayRole ).toString() );
            }
            if ( sts == WorkPackage::TS_Receive ) {
                return i18n( "Received from %1 at %2", wp->ownerName(), transmitionTime( wp, Qt::DisplayRole ).toString() );
            }
            return i18n( "Not available" );
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant WorkPackageModel::transmitionTime( const WorkPackage *wp, int role ) const
{
    if ( ! wp ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::DisplayRole:
            return KGlobal::locale()->formatDateTime( wp->transmitionTime() );
        case Qt::EditRole:
            return wp->transmitionTime();
        case Qt::ToolTipRole: {
            int sts = wp->transmitionStatus();
            QString t = KGlobal::locale()->formatDateTime( wp->transmitionTime(), KLocale::LongDate, KLocale::TimeZone );
            if ( sts == WorkPackage::TS_Send ) {
                return i18n( "Work package sent at: %1", t );
            }
            if ( sts == WorkPackage::TS_Receive ) {
                return i18n( "Work package transmission received at: %1", t );
            }
            return i18n( "Not available" );
        }

        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant WorkPackageModel::completion( const WorkPackage *wp, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            if ( wp->transmitionStatus() == WorkPackage::TS_Receive ) {
                return wp->completion().percentFinished();
            }
            break;
        case Qt::EditRole:
            if ( wp->transmitionStatus() == WorkPackage::TS_Receive ) {
                return wp->completion().percentFinished();
            }
            break;
        case Qt::ToolTipRole:
            if ( wp->transmitionStatus() == WorkPackage::TS_Receive ) {
                return i18n( "Task reported %1% completed", wp->completion().percentFinished() );
            }
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant WorkPackageModel::data( const WorkPackage *wp, int column, int role ) const
{
    switch ( column ) {
        case NodeModel::WPOwnerName:
        case NodeModel::NodeName: return ownerName( wp, role );
        case NodeModel::WPTransmitionStatus:
        case NodeModel::NodeStatus: return transmitionStatus( wp, role );
        case NodeModel::NodeCompleted: return completion( wp, role );
        case NodeModel::WPTransmitionTime:
        case NodeModel::NodeActualStart: return transmitionTime( wp, role );

        default: break;
    }
    return QVariant();
}

//-----------------------------
bool WPSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if ( sourceModel()->index( source_row, NodeModel::NodeType, source_parent ).data( Qt::EditRole ).toInt() != Node::Type_Task ) {
        return false;
    }
    if ( sourceModel()->index( source_row, NodeModel::NodeStatus, source_parent ).data( Qt::EditRole ).toInt() & Node::State_NotScheduled ) {
        return false;
    }
    return true;
}

WorkPackageProxyModel::WorkPackageProxyModel( QObject *parent )
    : QAbstractProxyModel( parent )
{
    m_proxies << new WPSortFilterProxyModel( this );
    m_proxies << new FlatProxyModel( this );
    m_nodemodel = new NodeItemModel( this );
    QAbstractProxyModel *p = this;
    foreach ( QAbstractProxyModel *m, m_proxies ) {
        p->setSourceModel( m );
        p = m;
    }
    p->setSourceModel( m_nodemodel );

}

Qt::ItemFlags WorkPackageProxyModel::flags(const QModelIndex &index) const
{
    if ( isWorkPackageIndex( index ) ) {
        return Qt::ItemIsEnabled;
    }
    return QAbstractProxyModel::flags( index );
}

void WorkPackageProxyModel::setSourceModel( QAbstractItemModel *model )
{
    if ( sourceModel() ) {
        disconnect(sourceModel(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
                this, SLOT(sourceDataChanged(const QModelIndex&, const QModelIndex&)));
/*        disconnect(sourceModel(), SIGNAL(headerDataChanged(Qt::Orientation, int, int )),
                this, SLOT(sourceHeaderDataChanged(Qt::Orientation, int, int )));*/
        disconnect(sourceModel(), SIGNAL(layoutChanged()),
                this, SIGNAL(layoutChanged()));
        disconnect(sourceModel(), SIGNAL(layoutAboutToBeChanged()),
                this, SIGNAL(layoutAboutToBeChanged()));
        disconnect(sourceModel(), SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int )),
                this, SLOT(sourceRowsAboutToBeInserted(const QModelIndex&, int, int )));
        disconnect(sourceModel(), SIGNAL(rowsInserted(const QModelIndex&, int, int)),
                this, SLOT(sourceRowsInserted(const QModelIndex&, int, int)));
        disconnect(sourceModel(), SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)),
                this, SLOT(sourceRowsAboutToBeRemoved(const QModelIndex&, int, int)));
        disconnect(sourceModel(), SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
                this, SLOT(sourceRowsAboutToBeRemoved(const QModelIndex&, int, int)));
/*
        disconnect(sourceModel(), SIGNAL(columnsAboutToBeInserted(const QModelIndex&, int, int)),
                this, SLOT(sourceColumnsAboutToBeInserted(const QModelIndex&, int, int)));
        disconnect(sourceModel(), SIGNAL(columnsInserted(const QModelIndex&, int, int)),
                this, SLOT(sourceColumnsInserted(const QModelIndex&, int, int)));

        disconnect(sourceModel(), SIGNAL(columnsAboutToBeRemoved(const QModelIndex&, int, int)),
                this, SLOT(sourceColumnsAboutToBeRemoved(const QModelIndex&, int, int)));
        disconnect(sourceModel(), SIGNAL(columnsRemoved(const QModelIndex&, int, int)),
                this, SLOT(sourceColumnsRemoved(const QModelIndex&, int, int)));
        */
        disconnect(sourceModel(), SIGNAL(modelAboutToBeReset()), this, SLOT(sourceModelAboutToBeReset()));
        disconnect(sourceModel(), SIGNAL(modelReset()), this, SLOT(sourceModelReset()));

        disconnect(sourceModel(), SIGNAL(rowsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int )),
                this, SLOT(sourceRowsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int )));
        disconnect(sourceModel(), SIGNAL(rowsMoved(const QModelIndex&, int, int, const QModelIndex&, int )),
                this, SLOT(sourceRowsMoved(const QModelIndex&, int, int, const QModelIndex&, int )));
/*
        disconnect(sourceModel(), SIGNAL(columnsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int )),
                this, SLOT(sourceColumnsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int )));
        disconnect(sourceModel(), SIGNAL(columnsMoved(const QModelIndex&parent, int, int, const QModelIndex&, int )),
                this, SLOT(sourceColumnsMoved(const QModelIndex&parent, int, int, const QModelIndex&, int )));*/
    }
    QAbstractProxyModel::setSourceModel( model );
    if ( model ) {
        connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
                this, SLOT(sourceDataChanged(const QModelIndex&, const QModelIndex&)));
/*        connect(model, SIGNAL(headerDataChanged(Qt::Orientation, int, int )),
                this, SLOT(sourceHeaderDataChanged(Qt::Orientation, int, int )));*/
        connect(model, SIGNAL(layoutChanged()),
                this, SIGNAL(layoutChanged()));
        connect(model, SIGNAL(layoutAboutToBeChanged()),
                this, SIGNAL(layoutAboutToBeChanged()));
        connect(model, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int )),
                this, SLOT(sourceRowsAboutToBeInserted(const QModelIndex&, int, int )));
        connect(model, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
                this, SLOT(sourceRowsInserted(const QModelIndex&, int, int)));
        connect(model, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)),
                this, SLOT(sourceRowsAboutToBeRemoved(const QModelIndex&, int, int)));
        connect(model, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
                this, SLOT(sourceRowsRemoved(const QModelIndex&, int, int)));
/*
        connect(model, SIGNAL(columnsAboutToBeInserted(const QModelIndex&, int, int)),
                this, SLOT(sourceColumnsAboutToBeInserted(const QModelIndex&, int, int)));
        connect(model, SIGNAL(columnsInserted(const QModelIndex&, int, int)),
                this, SLOT(sourceColumnsInserted(const QModelIndex&, int, int)));

        connect(model, SIGNAL(columnsAboutToBeRemoved(const QModelIndex&, int, int)),
                this, SLOT(sourceColumnsAboutToBeRemoved(const QModelIndex&, int, int)));
        connect(model, SIGNAL(columnsRemoved(const QModelIndex&, int, int)),
                this, SLOT(sourceColumnsRemoved(const QModelIndex&, int, int)));
        */
        connect(model, SIGNAL(modelAboutToBeReset()), this, SLOT(sourceModelAboutToBeReset()));
        connect(model, SIGNAL(modelReset()), this, SLOT(sourceModelReset()));

        connect(model, SIGNAL(rowsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int )),
                this, SLOT(sourceRowsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int )));
        connect(model, SIGNAL(rowsMoved(const QModelIndex&, int, int, const QModelIndex&, int )),
                this, SLOT(sourceRowsMoved(const QModelIndex&, int, int, const QModelIndex&, int )));
/*
        connect(model, SIGNAL(columnsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int )),
                this, SLOT(sourceColumnsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int )));
        connect(model, SIGNAL(columnsMoved(const QModelIndex&parent, int, int, const QModelIndex&, int )),
                this, SLOT(sourceColumnsMoved(const QModelIndex&parent, int, int, const QModelIndex&, int )));*/
    }
}

void WorkPackageProxyModel::sourceDataChanged(const QModelIndex &start, const QModelIndex &end)
{
    emit dataChanged( mapFromSource( start ), mapFromSource( end ) );
}

void WorkPackageProxyModel::sourceModelAboutToBeReset()
{
//    kDebug(planDbg());
    beginResetModel();
    detachTasks();
}

void WorkPackageProxyModel::sourceModelReset()
{
//    kDebug(planDbg());
    attachTasks();
    for ( int r = 0; r < rowCount(); ++r ) {
        kDebug(planDbg())<<index( r, 0 ).data();
    }
    endResetModel();
}

void WorkPackageProxyModel::sourceRowsAboutToBeInserted(const QModelIndex &parent, int start, int end )
{
    kDebug(planDbg())<<parent<<start<<end;
    Q_ASSERT( ! parent.isValid() );
    beginInsertRows( QModelIndex(), start, end );
}

void WorkPackageProxyModel::sourceRowsInserted(const QModelIndex &parent, int start, int end)
{
    kDebug(planDbg())<<parent<<start<<end<<":"<<rowCount();
    Q_ASSERT( ! parent.isValid() );
    for ( int r = start; r <= end; ++r ) {
        QModelIndex i = index( r, 0 );
        Task *task = taskFromIndex( i );
        if ( task ) {
            attachTasks( task );
        }
    }
    endInsertRows();
}

void WorkPackageProxyModel::sourceRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end )
{
    kDebug(planDbg())<<parent<<start<<end;
    Q_ASSERT( ! parent.isValid() );
    beginInsertRows( QModelIndex(), start, end );
}

void WorkPackageProxyModel::sourceRowsRemoved(const QModelIndex &parent, int start, int end)
{
    kDebug(planDbg())<<parent<<start<<end;
    Q_ASSERT( ! parent.isValid() );
    for ( int r = start; r <= end; ++r ) {
        QModelIndex i = index( r, 0 );
        Task *task = taskFromIndex( i );
        if ( task ) {
            detachTasks( task );
        }
    }
    endInsertRows();
}

void WorkPackageProxyModel::sourceRowsAboutToBeMoved(const QModelIndex&, int sourceStart, int sourceEnd, const QModelIndex&, int destStart )
{
    beginMoveRows( QModelIndex(), sourceStart, sourceEnd, QModelIndex(), destStart );
}

void WorkPackageProxyModel::sourceRowsMoved(const QModelIndex &, int , int , const QModelIndex &, int )
{
    endMoveRows();
}

bool WorkPackageProxyModel::hasChildren(const QModelIndex &parent) const
{
    return rowCount(parent) > 0;
}

int WorkPackageProxyModel::rowCount( const QModelIndex &parent ) const
{
    int rows = 0;
    if ( ! parent.isValid() ) {
        rows = sourceModel()->rowCount();
    } else if ( isTaskIndex( parent ) ) {
        Task *task = taskFromIndex( parent );
        rows = task ? task->workPackageLogCount() : 0;
    }
//    kDebug(planDbg())<<rows;
    for ( int r = 0; r < rows; ++r ) {
//        kDebug(planDbg())<<r<<index( r, 0 ).data();
    }
    return rows;
}

int WorkPackageProxyModel::columnCount(const QModelIndex &/*parent*/) const
{
    return sourceModel()->columnCount();
}

QModelIndex WorkPackageProxyModel::mapToSource( const QModelIndex &proxyIndex ) const
{
    if ( ! proxyIndex.isValid() ) {
        return QModelIndex();
    }
    if ( isWorkPackageIndex( proxyIndex ) ) {
        // workpackage, not mapped to source model
        return QModelIndex();
    }
    return sourceModel()->index( proxyIndex.row(), proxyIndex.column() );
}

QModelIndex WorkPackageProxyModel::mapFromSource( const QModelIndex &sourceIndex ) const
{
    // index from source model is always a node
    return createIndex( sourceIndex.row(), sourceIndex.column() );
}

QModelIndex WorkPackageProxyModel::parent( const QModelIndex &child ) const
{
    QModelIndex idx;
    if ( isWorkPackageIndex( child ) ) {
        // only work packages have parent
        idx = m_nodemodel->index( static_cast<Node*>( child.internalPointer() ) );
        idx = mapFromBaseModel( idx );
    }
//    kDebug(planDbg())<<child<<idx;
    return idx;
}

QModelIndex WorkPackageProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    QModelIndex idx;
    if ( ! parent.isValid() ) {
        // this should be a node
        idx = createIndex( row, column );
    } else if ( isTaskIndex( parent ) ) {
        // Should be a work package, parent should be a task
        Task *task = taskFromIndex( parent );
        if ( task ) {
            idx = createIndex( row, column, task );
        }
    }
/*    if ( ! idx.isValid() ) {
        kDebug(planDbg())<<"not valid:"<<parent<<row<<column<<idx;
    } else {
        kDebug(planDbg())<<parent<<row<<column<<idx;
    }*/
    return idx;
}

QVariant WorkPackageProxyModel::data( const QModelIndex &idx, int role ) const
{
    QVariant value;
    if ( isTaskIndex( idx ) ) {
        value = mapToSource( idx ).data( role );
    } else if ( isWorkPackageIndex( idx ) ) {
        Task *task = taskFromIndex( idx );
        if ( task ) {
            value = m_model.data( task->workPackageAt( idx.row() ), idx.column(), role );
        }
    }
//    kDebug(planDbg())<<idx<<value;
    return value;
}

Task *WorkPackageProxyModel::taskFromIndex( const QModelIndex &idx ) const
{
    Task *task = 0;
    if ( idx.internalPointer() ) {
        task = static_cast<Task*>( idx.internalPointer() );
    } else if ( idx.isValid() ) {
        QVariant obj = data( idx, Role::Object );
        task = qobject_cast<Task*>( obj.value<QObject*>() );
    }
//    kDebug(planDbg())<<idx<<task;
    return task;
}

QModelIndex WorkPackageProxyModel::indexFromTask(const Node *node) const
{
    return mapFromBaseModel( m_nodemodel->index( node ) );
}

QModelIndex WorkPackageProxyModel::mapFromBaseModel( const QModelIndex &idx ) const
{
    if ( ! idx.isValid() ) {
        return QModelIndex();
    }
    QModelIndex in = idx;
    for ( int i = m_proxies.count() -1; i >= 0; --i ) {
        in = m_proxies.at( i )->mapFromSource( in );
    }
    return mapFromSource( in );
}
void WorkPackageProxyModel::setProject( Project *project )
{
    kDebug(planDbg())<<project;
    m_nodemodel->setProject( project );
}

void WorkPackageProxyModel::setScheduleManager(ScheduleManager *sm)
{
    kDebug(planDbg())<<sm;
    m_nodemodel->setScheduleManager( sm );
}

NodeItemModel *WorkPackageProxyModel::baseModel() const
{
    return m_nodemodel;
}

void WorkPackageProxyModel::detachTasks( Task *task )
{
    if ( task ) {
        disconnect(task, SIGNAL(workPackageToBeAdded(Node*,int)), this, SLOT(workPackageToBeAdded(Node*, int)));
        disconnect(task, SIGNAL(workPackageAdded(Node*)), this, SLOT(workPackageAdded(Node*)));
        disconnect(task, SIGNAL(workPackageToBeRemoved(Node*,int)), this, SLOT(workPackageToBeRemoved(Node*, int)));
        disconnect(task, SIGNAL(workPackageRemoved(Node*)), this, SLOT(workPackageRemoved(Node*)));
//        kDebug(planDbg())<<task;
    } else {
        for ( int r = 0; r < rowCount(); ++r ) {
            Task *t = taskFromIndex( index( r, 0 ) );
            if ( t ) {
                detachTasks( t );
            }
        }
    }
}

void WorkPackageProxyModel::attachTasks( Task *task )
{
    if ( task ) {
        connect(task, SIGNAL(workPackageToBeAdded(Node*,int)), this, SLOT(workPackageToBeAdded(Node*, int)));
        connect(task, SIGNAL(workPackageAdded(Node*)), this, SLOT(workPackageAdded(Node*)));
        connect(task, SIGNAL(workPackageToBeRemoved(Node*,int)), this, SLOT(workPackageToBeRemoved(Node*, int)));
        connect(task, SIGNAL(workPackageRemoved(Node*)), this, SLOT(workPackageRemoved(Node*)));
//        kDebug(planDbg())<<task;
    } else {
        for ( int r = 0; r < rowCount(); ++r ) {
            Task *t = taskFromIndex( index( r, 0 ) );
            if ( t ) {
                attachTasks( t );
            }
        }
    }
}

void WorkPackageProxyModel::workPackageToBeAdded(Node *node, int row )
{
    QModelIndex idx = indexFromTask( node );
    kDebug(planDbg())<<node<<row<<idx;
    beginInsertRows( idx, row, row );
}

void WorkPackageProxyModel::workPackageAdded(Node *)
{
    endInsertRows();
}

void WorkPackageProxyModel::workPackageToBeRemoved(Node *node, int row)
{
    QModelIndex idx = indexFromTask( node );
    beginRemoveRows( idx, row, row );
}

void WorkPackageProxyModel::workPackageRemoved(Node *)
{
    endRemoveRows();
}

} //namespace KPlato

#include "kptworkpackagemodel.moc"
