/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen <kplato@kde.org>

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

#include "kptpertcpmmodel.h"

#include "kptglobal.h"
#include "kptcommonstrings.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptnode.h"
#include "kptschedule.h"

#include <QStringList>
#include <QMimeData>

#include <kglobal.h>
#include <klocale.h>

namespace KPlato
{
  
class Project;
class Node;
class Task;

typedef QList<Node*> NodeList;

CriticalPathItemModel::CriticalPathItemModel( QObject *parent )
    : ItemModelBase( parent ),
    m_manager( 0 )
{
/*    connect( this, SIGNAL( modelAboutToBeReset() ), SLOT( slotAboutToBeReset() ) );
    connect( this, SIGNAL( modelReset() ), SLOT( slotReset() ) );*/
}

CriticalPathItemModel::~CriticalPathItemModel()
{
}
    
void CriticalPathItemModel::slotNodeToBeInserted( Node *, int )
{
    //kDebug()<<node->name();
}

void CriticalPathItemModel::slotNodeInserted( Node * /*node*/ )
{
    //kDebug()<<node->getParent->name()<<"-->"<<node->name();
}

void CriticalPathItemModel::slotNodeToBeRemoved( Node *node )
{
    //kDebug()<<node->name();
/*    if ( m_path.contains( node ) ) {
    }*/
}

void CriticalPathItemModel::slotNodeRemoved( Node *node )
{
    //kDebug()<<node->name();
}

void CriticalPathItemModel::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeToBeAdded( Node*, int ) ), this, SLOT( slotNodeToBeInserted(  Node*, int ) ) );
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
    reset();
}

void CriticalPathItemModel::setManager( ScheduleManager *sm )
{
    kDebug()<<this;
    m_manager = sm;
    m_nodemodel.setManager( sm );
    if ( m_project == 0 || m_manager == 0 ) {
        m_path.clear();
    } else {
        m_path = m_project->criticalPath( m_manager->id(), 0 );
    }
    kDebug()<<m_path;
    reset();
}

QModelIndex CriticalPathItemModel::parent( const QModelIndex & ) const
{
    return QModelIndex();
}

QModelIndex CriticalPathItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_project == 0 || column < 0 || column >= columnCount() || row < 0 ) {
        return QModelIndex();
    }
    if ( parent.isValid() ) {
        return QModelIndex();
    }
    Node *n = m_path.value( row );
    QModelIndex i = createIndex(row, column, n );
    return i;
}

Duration::Unit CriticalPathItemModel::presentationUnit( const Duration &dur ) const
{
    if ( dur.toDouble( Duration::Unit_d ) < 1.0 ) {
        return Duration::Unit_h;
    }
    return Duration::Unit_d;
}

QVariant CriticalPathItemModel::name( int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return i18n( "Path" );
        case Qt::ToolTipRole:
        case Qt::EditRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant CriticalPathItemModel::duration( int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
            Duration v = m_project->duration( m_manager->id() );
            return KGlobal::locale()->formatNumber( v.toDouble( presentationUnit( v ) ), 1 ) + Duration::unitToString( presentationUnit( v ) );
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant CriticalPathItemModel::variance( int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
            double v = 0.0;
            foreach ( Node *n, m_path ) {
                long id = m_manager->id();
                v += n->variance( id, presentationUnit( m_project->duration( id ) ) );
            }
            return KGlobal::locale()->formatNumber( v, 1 );
            break;
        }
        case Qt::EditRole: {
            double v = 0.0;
            foreach ( Node *n, m_path ) {
                v += n->variance( m_manager->id() );
            }
            return v;
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant CriticalPathItemModel::notUsed( int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return "";
        default:
            return QVariant();
    }
    return QVariant();
}

QVariant CriticalPathItemModel::data( const QModelIndex &index, int role ) const
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
            case NodeModel::NodeName: result = name( role ); break;
            case NodeModel::NodeDuration: result = duration( role ); break;
            case NodeModel::NodeVarianceDuration: result = variance( role ); break;
            default:
                result = notUsed( role ); break;
        }
    } else  {
        result = m_nodemodel.data( n, index.column(), role );
    }
    if ( result.isValid() ) {
        if ( role == Qt::DisplayRole && result.type() == QVariant::String && result.toString().isEmpty()) {
            // HACK to show focus in empty cells
            result = " ";
        }
        return result;
    }
    return result;
}

QVariant CriticalPathItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
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
    } else if ( role == Qt::WhatsThisRole ) {
        return m_nodemodel.headerData( section, role );
    }
    return ItemModelBase::headerData(section, orientation, role);
}

QVariant CriticalPathItemModel::alignment( int column ) const
{
    switch ( column ) {
        case 0: return QVariant(); // use default for column 0
        default: return Qt::AlignCenter;
    }
    return QVariant();
}

int CriticalPathItemModel::columnCount( const QModelIndex & ) const
{
    return m_nodemodel.propertyCount();
}

int CriticalPathItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( parent.isValid() ) {
        return 0;
    }
    if ( m_manager && m_manager->expected() && m_manager->expected()->criticalPathList() ) {
        return m_path.count() + 1;
    }
    return 0;
}

Node *CriticalPathItemModel::node( const QModelIndex &index ) const
{
    if ( ! index.isValid() ) {
        return 0;
    }
    return m_path.value( index.row() );
}

void CriticalPathItemModel::slotNodeChanged( Node *node )
{
    kDebug();
    if ( node == 0 || node->type() == Node::Type_Project || ! m_path.contains( node ) ) {
        return;
    }
    int row = m_path.indexOf( node );
    emit dataChanged( createIndex( row, 0, node ), createIndex( row, columnCount(), node ) );
}


//-----------------------------
PertResultItemModel::PertResultItemModel( QObject *parent )
    : ItemModelBase( parent ),
    m_manager( 0 )
{
/*    connect( this, SIGNAL( modelAboutToBeReset() ), SLOT( slotAboutToBeReset() ) );
    connect( this, SIGNAL( modelReset() ), SLOT( slotReset() ) );*/
}

PertResultItemModel::~PertResultItemModel()
{
}
    
void PertResultItemModel::slotAboutToBeReset()
{
    kDebug();
    clear();
}

void PertResultItemModel::slotReset()
{
    kDebug();
    refresh();
}

void PertResultItemModel::slotNodeToBeInserted( Node *, int )
{
    //kDebug()<<node->name();
    clear();
}

void PertResultItemModel::slotNodeInserted( Node * /*node*/ )
{
    //kDebug()<<node->getParent->name()<<"-->"<<node->name();
    refresh();
}

void PertResultItemModel::slotNodeToBeRemoved( Node * /*node*/ )
{
    //kDebug()<<node->name();
    clear();
}

void PertResultItemModel::slotNodeRemoved( Node * /*node*/ )
{
    //kDebug()<<node->name();
    refresh();
}

void PertResultItemModel::setProject( Project *project )
{
    clear();
    if ( m_project ) {
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeToBeAdded( Node*, int ) ), this, SLOT( slotNodeToBeInserted(  Node*, int ) ) );
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

void PertResultItemModel::setManager( ScheduleManager *sm )
{
    m_manager = sm;
    m_nodemodel.setManager( sm );
    refresh();
}

void PertResultItemModel::clear()
{
    kDebug()<<this;
    foreach ( NodeList *l, m_top ) {
        int c = l->count();
        if ( c > 0 ) {
            // FIXME: gives error msg:
            // Can't select indexes from different model or with different parents
            QModelIndex i = index( l );
            kDebug()<<i<<": "<<c;
//            beginRemoveRows( i, 0, c-1 );
//            endRemoveRows();
        }
    }
    m_critical.clear();
    m_noncritical.clear();
    if ( ! m_top.isEmpty() ) {
        beginRemoveRows( QModelIndex(), 0, m_top.count() - 1 );
        m_top.clear();
        m_topNames.clear();
        endRemoveRows();
    }
}

void PertResultItemModel::refresh()
{
    clear();
    if ( m_project == 0 ) {
        return;
    }
    long id = m_manager == 0 ? -1 : m_manager->id();
    kDebug()<<id;
    if ( id == -1 ) {
        return;
    }
    m_topNames << i18n( "Project" );
    m_top << &m_dummyList; // dummy
    const QList< NodeList > *lst = m_project->criticalPathList( id );
    if ( lst ) {
        for ( int i = 0; i < lst->count(); ++i ) {
            m_topNames << i18n( "Critical Path" );
            m_top.append( const_cast<NodeList*>( &( lst->at( i ) ) ) );
            kDebug()<<m_topNames.last()<<lst->at( i );
        }
        if ( lst->isEmpty() ) kDebug()<<"No critical path";
    }
    foreach( Node* n, m_project->allNodes() ) {
        if ( n->type() != Node::Type_Task && n->type() != Node::Type_Milestone ) {
            continue;
        }
        Task *t = static_cast<Task*>( n );
        if ( t->inCriticalPath( id ) ) {
            continue;
        } else if ( t->isCritical( id ) ) {
            m_critical.append( t );
        } else {
            m_noncritical.append( t );
        }
    }
    if ( ! m_critical.isEmpty() ) {
        m_topNames << i18n( "Critical" );
        m_top.append(&m_critical );
    }
    if ( ! m_noncritical.isEmpty() ) {
        m_topNames << i18n( "Non-critical" );
        m_top.append(&m_noncritical );
    }
    if ( ! m_top.isEmpty() ) {
        kDebug()<<m_top;
        beginInsertRows( QModelIndex(), 0, m_top.count() -1 );
        endInsertRows();
        foreach ( NodeList *l, m_top ) {
            int c = l->count();
            if ( c > 0 ) {
                beginInsertRows( index( l ), 0, c-1 );
                endInsertRows();
            }
        }
    }
}

Qt::ItemFlags PertResultItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags( index );
    flags &= ~( Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled );
    return flags;
}

    
QModelIndex PertResultItemModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() ) {
        return QModelIndex();
    }
    //kDebug()<<index.internalPointer()<<": "<<index.row()<<", "<<index.column();
    int row = index.internalId();
    if ( row < 0 ) {
        return QModelIndex(); // top level has no parent
    }
    if ( m_top.value( row ) == 0 ) {
        return QModelIndex();
    }
    return createIndex( row, 0, -1 );
}

QModelIndex PertResultItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_project == 0 || column < 0 || column >= columnCount() || row < 0 ) {
        return QModelIndex();
    }
    if ( ! parent.isValid() ) {
        if ( row == 0 ) {
            QModelIndex idx = createIndex(row, column, -2 ); // project
            return idx;
        }
        if ( row >= m_top.count() ) {
            return QModelIndex(); // shouldn't happend
        }
        QModelIndex idx = createIndex(row, column, -1 );
        //kDebug()<<parent<<", "<<idx;
        return idx;
    }
    if ( parent.row() == 0 ) {
        return QModelIndex();
    }
    NodeList *l = m_top.value( parent.row() );
    if ( l == 0 ) {
        return QModelIndex();
    }
    QModelIndex i = createIndex(row, column, parent.row() );
    return i;
}

// QModelIndex PertResultItemModel::index( const Node *node ) const
// {
//     if ( m_project == 0 || node == 0 ) {
//         return QModelIndex();
//     }
//     foreach( NodeList *l, m_top ) {
//         int row = l->indexOf( const_cast<Node*>( node ) );
//         if ( row != -1 ) {
//             return createIndex( row, 0, const_cast<Node*>( node ) );
//         }
//     }
//     return QModelIndex();
// }

QModelIndex PertResultItemModel::index( const NodeList *lst ) const
{
    if ( m_project == 0 || lst == 0 ) {
        return QModelIndex();
    }
    NodeList *l = const_cast<NodeList*>( lst );
    int row = m_top.indexOf( l );
    if ( row <= 0 ) {
        return QModelIndex();
    }
    return createIndex( row, 0, -1 );
}

QVariant PertResultItemModel::name( int row, int role ) const
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

QVariant PertResultItemModel::name( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return node->name();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant PertResultItemModel::earlyStart( const Task *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->earlyStart( m_manager->id() ).dateTime();
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatDate( node->earlyStart( m_manager->id() ).date() );
        case Qt::EditRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant PertResultItemModel::earlyFinish( const Task *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->earlyFinish( m_manager->id() ).dateTime();
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatDate( node->earlyFinish( m_manager->id() ).date() );
        case Qt::EditRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant PertResultItemModel::lateStart( const Task *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->lateStart( m_manager->id() ).dateTime();
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatDate( node->lateStart( m_manager->id() ).date() );
        case Qt::EditRole:
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant PertResultItemModel::lateFinish( const Task *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->lateFinish( m_manager->id() ).dateTime();
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatDate( node->lateFinish( m_manager->id() ).date() );
        case Qt::EditRole:
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant PertResultItemModel::positiveFloat( const Task *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->positiveFloat( m_manager->id() ).toString( Duration::Format_i18nHourFraction );
        case Qt::ToolTipRole:
            return node->positiveFloat( m_manager->id() ).toString( Duration::Format_i18nDayTime );
        case Qt::EditRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant PertResultItemModel::freeFloat( const Task *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->freeFloat( m_manager->id() ).toString( Duration::Format_i18nHourFraction );
        case Qt::ToolTipRole:
            return node->freeFloat( m_manager->id() ).toString( Duration::Format_i18nDayTime );
        case Qt::EditRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant PertResultItemModel::negativeFloat( const Task *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->negativeFloat( m_manager->id() ).toString( Duration::Format_i18nHourFraction );
        case Qt::ToolTipRole:
            return node->negativeFloat( m_manager->id() ).toString( Duration::Format_i18nDayTime );
        case Qt::EditRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant PertResultItemModel::startFloat( const Task *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->startFloat( m_manager->id() ).toString( Duration::Format_i18nHourFraction );
        case Qt::ToolTipRole:
            return node->startFloat( m_manager->id() ).toString( Duration::Format_i18nDayTime );
        case Qt::EditRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant PertResultItemModel::finishFloat( const Task *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return node->finishFloat( m_manager->id() ).toString( Duration::Format_i18nHourFraction );
        case Qt::ToolTipRole:
            return node->finishFloat( m_manager->id() ).toString( Duration::Format_i18nDayTime );
        case Qt::EditRole:
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant PertResultItemModel::data( const QModelIndex &index, int role ) const
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
    if ( n->type() == Node::Type_Task || n->type() == Node::Type_Milestone ) {
        result = m_nodemodel.data( n, index.column(), role );
    }
    if ( n->type() == Node::Type_Project ) {
        Project *p = static_cast<Project*>( n );
        switch ( index.column() ) {
            case NodeModel::NodeName: result = name( NodeModel::NodeName, role ); break;
            default:
                //kDebug()<<"data: invalid display value column "<<index.column();
                return QVariant();
        }
    }
    if ( result.isValid() ) {
        if ( role == Qt::DisplayRole && result.type() == QVariant::String && result.toString().isEmpty()) {
            // HACK to show focus in empty cells
            result = " ";
        }
        return result;
    }
    return QVariant();
}

QVariant PertResultItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
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
    } else if ( role == Qt::WhatsThisRole ) {
        return m_nodemodel.headerData( section, role );
    }
    return ItemModelBase::headerData(section, orientation, role);
}

QVariant PertResultItemModel::alignment( int column ) const
{
    switch ( column ) {
        case 0: return QVariant(); // use default for column 0
        default: return Qt::AlignCenter;
    }
    return QVariant();
}

QAbstractItemDelegate *PertResultItemModel::createDelegate( int column, QWidget * /*parent*/ ) const
{
    switch ( column ) {
        default: return 0;
    }
    return 0;
}

int PertResultItemModel::columnCount( const QModelIndex & ) const
{
    return m_nodemodel.propertyCount();
}

int PertResultItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( ! parent.isValid() ) {
        //kDebug()<<"top="<<m_top.count();
        return m_top.count();
    }
    NodeList *l = list( parent );
    if ( l ) {
        //kDebug()<<"list "<<parent.row()<<": "<<l->count();
        return l->count();
    }
    //kDebug()<<"node "<<parent.row();
    return 0; // nodes don't have children
}

Qt::DropActions PertResultItemModel::supportedDropActions() const
{
    return (Qt::DropActions)Qt::CopyAction | Qt::MoveAction;
}


QStringList PertResultItemModel::mimeTypes() const
{
    return QStringList();
}

QMimeData *PertResultItemModel::mimeData( const QModelIndexList & ) const
{
    QMimeData *m = new QMimeData();
    return m;
}

bool PertResultItemModel::dropAllowed( Node *, const QMimeData * )
{
    return false;
}

bool PertResultItemModel::dropMimeData( const QMimeData *, Qt::DropAction , int , int , const QModelIndex & )
{
    return false;
}

NodeList *PertResultItemModel::list( const QModelIndex &index ) const
{
    if ( index.isValid() && index.internalId() == -1 ) {
        //kDebug()<<index<<"is list: "<<m_top.value( index.row() );
        return m_top.value( index.row() );
    }
    //kDebug()<<index<<"is not list";
    return 0;
}

Node *PertResultItemModel::node( const QModelIndex &index ) const
{
    if ( ! index.isValid() ) {
        return 0;
    }
    if ( index.internalId() == -2 ) {
        return m_project;
    }
    if ( index.internalId() == 0 ) {
        return 0;
    }
    NodeList *l = m_top.value( index.internalId() );
    if ( l ) {
        return l->value( index.row() );
    }
    return 0;
}

void PertResultItemModel::slotNodeChanged( Node *)
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

#include "kptpertcpmmodel.moc"
