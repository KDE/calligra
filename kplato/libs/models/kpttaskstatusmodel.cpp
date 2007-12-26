/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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

#include "kpttaskstatusmodel.h"

#include "kptglobal.h"
#include "kptitemmodelbase.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"

#include <QAbstractItemModel>
#include <QMimeData>
#include <QItemDelegate>
#include <QModelIndex>

#include <kglobal.h>
#include <klocale.h>

namespace KPlato
{


TaskStatusItemModel::TaskStatusItemModel( QObject *parent )
    : ItemModelBase( parent ),
    m_period( 7 )
{
    m_nodemodel.setNow( QDate::currentDate() );
    
    m_topNames << i18n( "Not Started" );
    m_top.append(&m_notstarted );
    
    m_topNames << i18n( "Running" );
    m_top.append(&m_running );
    
    m_topNames << i18n( "Finished" );
    m_top.append(&m_finished );
    
    m_topNames << i18n( "Next Period" );
    m_top.append(&m_upcoming );
    
/*    connect( this, SIGNAL( modelAboutToBeReset() ), SLOT( slotAboutToBeReset() ) );
    connect( this, SIGNAL( modelReset() ), SLOT( slotReset() ) );*/
}

TaskStatusItemModel::~TaskStatusItemModel()
{
}
    
void TaskStatusItemModel::slotAboutToBeReset()
{
    kDebug();
    clear();
}

void TaskStatusItemModel::slotReset()
{
    kDebug();
    refresh();
}

void TaskStatusItemModel::slotNodeToBeInserted( Node *, int )
{
    //kDebug()<<node->name();
    clear();
}

void TaskStatusItemModel::slotNodeInserted( Node */*node*/ )
{
    //kDebug()<<node->getParent->name()<<"-->"<<node->name();
    refresh();
}

void TaskStatusItemModel::slotNodeToBeRemoved( Node */*node*/ )
{
    //kDebug()<<node->name();
    clear();
}

void TaskStatusItemModel::slotNodeRemoved( Node */*node*/ )
{
    //kDebug()<<node->name();
    refresh();
}

void TaskStatusItemModel::setProject( Project *project )
{
    clear();
    if ( m_project ) {
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeToBeAdded( Node* ) ), this, SLOT( slotNodeToBeInserted(  Node*, int ) ) );
        disconnect( m_project, SIGNAL( nodeToBeRemoved( Node* ) ), this, SLOT( slotNodeToBeRemoved( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeToBeMoved( Node* ) ), this, SLOT( slotLayoutToBeChanged() ) );
    
        disconnect( m_project, SIGNAL( nodeAdded( Node* ) ), this, SLOT( slotNodeInserted( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeRemoved( Node* ) ), this, SLOT( slotNodeRemoved( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeMoved( Node* ) ), this, SLOT( slotLayoutChanged() ) );
    }
    m_project = project;
    m_nodemodel.setProject( project );
    if ( project ) {
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        connect( m_project, SIGNAL( nodeToBeAdded( Node*, int ) ), this, SLOT( slotNodeToBeInserted(  Node*, int ) ) );
        connect( m_project, SIGNAL( nodeToBeRemoved( Node* ) ), this, SLOT( slotNodeToBeRemoved( Node* ) ) );
        connect( m_project, SIGNAL( nodeToBeMoved( Node* ) ), this, SLOT( slotLayoutToBeChanged() ) );
    
        connect( m_project, SIGNAL( nodeAdded( Node* ) ), this, SLOT( slotNodeInserted( Node* ) ) );
        connect( m_project, SIGNAL( nodeRemoved( Node* ) ), this, SLOT( slotNodeRemoved( Node* ) ) );
        connect( m_project, SIGNAL( nodeMoved( Node* ) ), this, SLOT( slotLayoutChanged() ) );
        
    }
    refresh();
}

void TaskStatusItemModel::setManager( ScheduleManager *sm )
{
    clear();
    if ( m_nodemodel.manager() ) {
    }
    m_nodemodel.setManager( sm );
    if ( sm ) {
    }
    refresh();
}


void TaskStatusItemModel::clear()
{
    foreach ( NodeList *l, m_top ) {
        int c = l->count();
        if ( c > 0 ) {
            //FIXME: gives error msg:
            // Can't select indexes from different model or with different parents
            QModelIndex i = index( l );
            beginRemoveRows( index( l ), 0, c-1 );
            l->clear();
            endRemoveRows();
        }
    }
}

void TaskStatusItemModel::refresh()
{
    clear();
    if ( m_project == 0 ) {
        return;
    }
    m_id = m_nodemodel.id();
    if ( m_id == -1 ) {
        return;
    }
    QDate begin = m_nodemodel.now().addDays( -m_period );
    QDate end = m_nodemodel.now().addDays( m_period );
    
    foreach( Node* n, m_project->allNodes() ) {
        if ( n->type() != Node::Type_Task ) {
            continue;
        }
        Task *t = static_cast<Task*>( n );
        const Completion &c = t->completion();
        if ( c.isFinished() ) {
            if ( c.finishTime().date() > begin ) {
                m_finished.append( t );
            }
        } else if ( c.isStarted() ) {
            m_running.append( t );
        } else if ( t->startTime( m_id ).date() < m_nodemodel.now() ) {
            // should have been started
            m_notstarted.append( t );
        } else if ( t->startTime( m_id ).date() <= end ) {
            // start next period
            m_upcoming.append( t );
        }
    }
    foreach ( NodeList *l, m_top ) {
        int c = l->count();
        if ( c > 0 ) {
            beginInsertRows( index( l ), 0, c-1 );
            endInsertRows();
        }
    }
}

Qt::ItemFlags TaskStatusItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags( index );
    flags &= ~( Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled );
    return flags;
}

    
QModelIndex TaskStatusItemModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() ) {
        return QModelIndex();
    }
    //kDebug()<<index.internalPointer()<<":"<<index.row()<<","<<index.column();
    int row = m_top.indexOf( static_cast<NodeList*>( index.internalPointer() ) );
    if ( row != -1 ) {
        return QModelIndex(); // top level has no parent
    }
    Node *n = node( index );
    if ( n == 0  ) {
        return QModelIndex();
    }
    NodeList *lst = 0;
    foreach ( NodeList *l, m_top ) {
        if ( l->indexOf( n ) != -1 ) {
            lst = l;
            break;
        }
    }
    if ( lst == 0 ) {
        return QModelIndex();
    }
    return createIndex( m_top.indexOf( lst ), 0, lst );
}

QModelIndex TaskStatusItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_project == 0 || column < 0 || column >= columnCount() || row < 0 ) {
        return QModelIndex();
    }
    if ( ! parent.isValid() ) {
        if ( row >= m_top.count() ) {
            return QModelIndex();
        }
        return createIndex(row, column, m_top.value( row ) );
    }
    NodeList *l = m_top.value( parent.row() );
    if ( l == 0 ) {
        return QModelIndex();
    }
    QModelIndex i = createIndex(row, column, l->value( row ) );
    Q_ASSERT( i.internalPointer() != 0 );
    return i;
}

QModelIndex TaskStatusItemModel::index( const Node *node ) const
{
    if ( m_project == 0 || node == 0 ) {
        return QModelIndex();
    }
    foreach( NodeList *l, m_top ) {
        int row = l->indexOf( const_cast<Node*>( node ) );
        if ( row != -1 ) {
            return createIndex( row, 0, const_cast<Node*>( node ) );
        }
    }
    return QModelIndex();
}

QModelIndex TaskStatusItemModel::index( const NodeList *lst ) const
{
    if ( m_project == 0 || lst == 0 ) {
        return QModelIndex();
    }
    NodeList *l = const_cast<NodeList*>( lst );
    int row = m_top.indexOf( l );
    if ( row == -1 ) {
        return QModelIndex();
    }
    return createIndex( row, 0, l );
}

QVariant TaskStatusItemModel::name( int row, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return m_topNames.value( row );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant TaskStatusItemModel::data( const QModelIndex &index, int role ) const
{
    QVariant result;
    if ( ! index.isValid() ) {
        return result;
    }
    if ( role == Qt::TextAlignmentRole ) {
        return alignment( index.column() );
    }
    Node *n = node( index );
    if ( n == 0 ) {
        switch ( index.column() ) {
            case 0: return name( index.row(), role );
            default: break;
        }
        return QVariant();
    }
    result = m_nodemodel.data( n, index.column(), role );
    if ( result.isValid() ) {
        if ( role == Qt::DisplayRole && result.type() == QVariant::String && result.toString().isEmpty()) {
            // HACK to show focus in empty cells
            result = " ";
        }
        return result;
    }
    return QVariant();
}

bool TaskStatusItemModel::setData( const QModelIndex &, const QVariant &, int )
{
    return false;
}

QVariant TaskStatusItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            return m_nodemodel.headerData( section, role );
        } else if ( role == Qt::TextAlignmentRole ) {
            return alignment( section );
        }
    }
    if ( role == Qt::ToolTipRole ) {
        return m_nodemodel.headerData( section, role );
    }
    return ItemModelBase::headerData(section, orientation, role);
}

QVariant TaskStatusItemModel::alignment( int column ) const
{
    switch ( column ) {
        case 0: return QVariant(); // use default
        default: return Qt::AlignCenter;
    }
    return QVariant();
}

QItemDelegate *TaskStatusItemModel::createDelegate( int column, QWidget */*parent*/ ) const
{
    switch ( column ) {
        default: return 0;
    }
    return 0;
}

int TaskStatusItemModel::columnCount( const QModelIndex & ) const
{
    return m_nodemodel.propertyCount();
}

int TaskStatusItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( ! parent.isValid() ) {
        //kDebug()<<"top="<<m_top.count();
        return m_top.count();
    }
    NodeList *l = list( parent );
    if ( l ) {
        //kDebug()<<"list"<<parent.row()<<":"<<l->count();
        return l->count();
    }
    //kDebug()<<"node"<<parent.row();
    return 0; // nodes don't have children
}

Qt::DropActions TaskStatusItemModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}


QStringList TaskStatusItemModel::mimeTypes() const
{
    return QStringList();
}

QMimeData *TaskStatusItemModel::mimeData( const QModelIndexList & indexes ) const
{
    QMimeData *m = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    QList<int> rows;
    foreach (QModelIndex index, indexes) {
        if ( index.isValid() && !rows.contains( index.row() ) ) {
            kDebug()<<index.row();
            Node *n = node( index );
            if ( n ) {
                rows << index.row();
                stream << n->id();
            }
        }
    }
    m->setData("application/x-vnd.kde.kplato.nodeitemmodel.internal", encodedData);
    return m;
}

bool TaskStatusItemModel::dropAllowed( Node *, const QMimeData * )
{
    return false;
}

bool TaskStatusItemModel::dropMimeData( const QMimeData *, Qt::DropAction , int , int , const QModelIndex & )
{
    return false;
}

NodeList *TaskStatusItemModel::list( const QModelIndex &index ) const
{
    if ( index.isValid() ) {
        Q_ASSERT( index.internalPointer() );
        if ( m_top.indexOf( static_cast<NodeList*>( index.internalPointer() ) ) != -1 ) {
            return static_cast<NodeList*>( index.internalPointer() );
        }
    }
    return 0;
}

Node *TaskStatusItemModel::node( const QModelIndex &index ) const
{
    if ( index.isValid() ) {
        foreach ( NodeList *l, m_top ) {
            int row = l->indexOf( static_cast<Node*>( index.internalPointer() ) );
            if ( row != -1 ) {
                return static_cast<Node*>( index.internalPointer() );
            }
        }
    }
    return 0;
}

void TaskStatusItemModel::slotNodeChanged( Node *)
{
    kDebug();
    refresh();
/*    if ( node == 0 || node->type() == Node::Type_Project ) {
        return;
    }
    int row = node->getParent()->findChildNode( node );
    emit dataChanged( createIndex( row, 0, node ), createIndex( row, columnCount(), node ) );*/
}


} // namespace KPlato

#include "kpttaskstatusmodel.moc"
