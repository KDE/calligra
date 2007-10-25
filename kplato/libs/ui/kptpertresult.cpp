/* This file is part of the KDE project
  Copyright (C) 2007 Florian Piquemal <flotueur@yahoo.fr>
  Copyright (C) 2007 Alexis Ménard <darktears31@gmail.com>
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
#include "kptpertresult.h"
#include "kptitemviewsettup.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptnode.h"
#include "kptschedule.h"

#include <KoDocument.h>

#include <QTreeView>
#include <QStringList>

#include <kicon.h>
#include <kaction.h>
#include <kglobal.h>
#include <klocale.h>
#include <ktoggleaction.h>
#include <kactionmenu.h>
#include <kstandardaction.h>
#include <kstandardshortcut.h>
#include <kaccelgen.h>
#include <kactioncollection.h>

namespace KPlato
{
  
class Project;
class Node;
class Task;

typedef QList<Node*> NodeList;

CriticalPathItemModel::CriticalPathItemModel( KoDocument *part, QObject *parent )
    : ItemModelBase( part, parent ),
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

void CriticalPathItemModel::slotNodeInserted( Node */*node*/ )
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

QVariant CriticalPathItemModel::name( const Node *node, int role ) const
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

QVariant CriticalPathItemModel::duration( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                Duration::Unit unit = presentationUnit();
                QList<double> scales; // TODO: week
                if ( node->estimate()->type() == Estimate::Type_Effort ) {
                    scales << m_project->standardWorktime()->day();
                    // rest is default
                }
                double v = Estimate::scale( node->duration( m_manager->id() ), unit, scales );
                //kDebug()<<node->name()<<": "<<v<<" "<<unit<<" : "<<scales;
                return KGlobal::locale()->formatNumber( v ) +  Duration::unitToString( unit, true );
            }
            break;
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
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatNumber( m_project->duration( m_manager->id() ).toDouble( presentationUnit() ), 1 );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant CriticalPathItemModel::variance( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                Duration::Unit unit = presentationUnit();
                double v = node->variance( m_manager->id(), unit );
                //kDebug()<<node->name()<<": "<<v<<" "<<unit<<" : "<<scales;
                return KGlobal::locale()->formatNumber( v ) +  Duration::unitToString( unit, true );
            }
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant CriticalPathItemModel::variance( const Estimate *est, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
                double v = est->variance( presentationUnit() );
                //kDebug()<<node->name()<<": "<<v<<" "<<unit<<" : "<<scales;
                return KGlobal::locale()->formatNumber( v );
            break;
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
                v += n->variance( m_manager->id(), presentationUnit() );
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

QVariant CriticalPathItemModel::optimistic( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
                Duration d = node->duration( m_manager->id() );
                d = ( d * ( 100 + node->estimate()->optimisticRatio() ) ) / 100;
                double v = d.toDouble( presentationUnit() );
                return KGlobal::locale()->formatNumber( v ) +  Duration::unitToString( presentationUnit(), true );
            break;
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant CriticalPathItemModel::optimistic( const Estimate *est, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
                Duration::Unit unit = presentationUnit();
                QList<double> scales; // TODO: week
                if ( est->type() == Estimate::Type_Effort ) {
                    scales << m_project->standardWorktime()->day();
                    // rest is default
                }
                double v = Estimate::scale( est->optimistic(), unit, scales );
                //kDebug()<<node->name()<<": "<<v<<" "<<unit<<" : "<<scales;
                return KGlobal::locale()->formatNumber( v ) +  Duration::unitToString( unit, true );
            break;
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}


QVariant CriticalPathItemModel::estimate( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                Duration::Unit unit = presentationUnit();
                QList<double> scales; // TODO: week
                if ( node->estimate()->type() == Estimate::Type_Effort ) {
                    scales << m_project->standardWorktime()->day();
                    // rest is default
                }
                double v = Estimate::scale( node->estimate()->expected(), unit, scales );
                //kDebug()<<node->name()<<": "<<v<<" "<<unit<<" : "<<scales;
                return KGlobal::locale()->formatNumber( v ) +  Duration::unitToString( unit, true );
            }
            break;
        case Role::DurationScales: {
            QVariantList lst; // TODO: week
            if ( node->estimate()->type() == Estimate::Type_Effort ) {
                lst.append( m_project->standardWorktime()->day() );
            } else {
                lst.append( 24.0 );
            }
            lst << 60.0 << 60.0 << 1000.0;
            return lst;
        }
        case Role::DurationUnit:
            return static_cast<int>( presentationUnit() );
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

QVariant CriticalPathItemModel::expected( const Estimate *est, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
            Duration::Unit unit = Duration::Unit_h;
            QList<double> scales; // TODO: week
            if ( est->type() == Estimate::Type_Effort ) {
                scales << m_project->standardWorktime()->day();
                // rest is default
            }
            double v = Estimate::scale( est->pertExpected(), unit, scales );
            return KGlobal::locale()->formatNumber( v ) +  Duration::unitToString( unit, true );
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant CriticalPathItemModel::pessimistic( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
                Duration d = node->duration( m_manager->id() );
                d = ( d * ( 100 + node->estimate()->pessimisticRatio() ) ) / 100;
                double v = d.toDouble( presentationUnit() );
                //kDebug()<<node->name()<<": "<<v<<" "<<unit<<" : "<<scales;
                return KGlobal::locale()->formatNumber( v ) +  Duration::unitToString( presentationUnit(), true );
            break;
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant CriticalPathItemModel::pessimistic( const Estimate *est, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole: {
                Duration::Unit unit = presentationUnit();
                QList<double> scales; // TODO: week
                if ( est->type() == Estimate::Type_Effort ) {
                    scales << m_project->standardWorktime()->day();
                    // rest is default
                }
                double v = Estimate::scale( est->pessimistic(), unit, scales );
                //kDebug()<<node->name()<<": "<<v<<" "<<unit<<" : "<<scales;
                return KGlobal::locale()->formatNumber( v ) +  Duration::unitToString( unit, true );
            break;
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
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
            case 0: result = name( role ); break;
            case 33: result = duration( role ); break;
            case 34: result = variance( role ); break;
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
        switch ( section ) {
            default: return QVariant();
        }
    }
    return ItemModelBase::headerData(section, orientation, role);
}

QVariant CriticalPathItemModel::alignment( int column ) const
{
    switch ( column ) {
        case 0: return QVariant(); // use default
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
PertResultItemModel::PertResultItemModel( KoDocument *part, QObject *parent )
    : ItemModelBase( part, parent ),
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

void PertResultItemModel::slotNodeInserted( Node */*node*/ )
{
    //kDebug()<<node->getParent->name()<<"-->"<<node->name();
    refresh();
}

void PertResultItemModel::slotNodeToBeRemoved( Node */*node*/ )
{
    //kDebug()<<node->name();
    clear();
}

void PertResultItemModel::slotNodeRemoved( Node */*node*/ )
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
/*        Task *t = static_cast<Task*>( n );
        switch ( index.column() ) {
            case 0: result = name( t, role ); break;
            case 1: result = earlyStart( t, role ); break;
            case 2: result = earlyFinish( t, role ); break;
            case 3: result = lateStart( t, role ); break;
            case 4: result = lateFinish( t, role ); break;
            case 5: result = positiveFloat( t, role ); break;
            case 6: result = freeFloat( t, role ); break;
            case 7: result = negativeFloat( t, role ); break;
            case 8: result = startFloat( t, role ); break;
            case 9: result = finishFloat( t, role ); break;
            default:
                kDebug()<<"data: invalid display value column "<<index.column();
                return QVariant();
        }*/
    }
    if ( n->type() == Node::Type_Project ) {
        Project *p = static_cast<Project*>( n );
        switch ( index.column() ) {
            case 0: result = name( 0, role ); break;
/*            case 1: result = earlyStart( p, role ); break;
            case 2: result = earlyFinish( p, role ); break;
            case 3: result = lateStart( p, role ); break;
            case 4: result = lateFinish( p, role ); break;
            case 5: result = positiveFloat( p, role ); break;
            case 6: result = freeFloat( p, role ); break;*/
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

bool PertResultItemModel::setData( const QModelIndex &, const QVariant &, int )
{
    return false;
}

QVariant PertResultItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            return m_nodemodel.headerData( section, role );
/*            switch ( section ) {
                case 0: return i18n( "Name" );
                case 1: return i18n( "Early Start" );
                case 2: return i18n( "Early Finish" );
                case 3: return i18n( "Late Start" );
                case 4: return i18n( "Late Finish" );
                case 5: return i18n( "Positive Float" );
                case 6: return i18n( "Free Float" );
                case 7: return i18n( "Negative Float" );
                case 8: return i18n( "Start Float" );
                case 9: return i18n( "Finish Float" );
                default: return QVariant();
            }*/
        } else if ( role == Qt::TextAlignmentRole ) {
            return alignment( section );
        }
    }
    if ( role == Qt::ToolTipRole ) {
        return m_nodemodel.headerData( section, role );
/*        switch ( section ) {
            case 0: return ToolTip::NodeName;
            case 1: return ToolTip::NodeEarlyStart;
            case 2: return ToolTip::NodeEarlyFinish;
            case 3: return ToolTip::NodeLateStart;
            case 4: return ToolTip::NodeLateFinish;
            case 5: return ToolTip::NodePositiveFloat;
            case 6: return ToolTip::NodeFreeFloat;
            case 7: return ToolTip::NodeNegativeFloat;
            case 8: return ToolTip::NodeStartFloat;
            case 9: return ToolTip::NodeFinishFloat;
            default: return QVariant();
        }*/
    }
    return ItemModelBase::headerData(section, orientation, role);
}

QVariant PertResultItemModel::alignment( int column ) const
{
    switch ( column ) {
        case 0: return QVariant(); // use default
        default: return Qt::AlignCenter;
    }
    return QVariant();
}

QItemDelegate *PertResultItemModel::createDelegate( int column, QWidget */*parent*/ ) const
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

//-----------------------------------
PertResult::PertResult( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent ),
    m_node( 0 ),
    m_project( 0 )
{
    kDebug() << " ---------------- KPlato: Creating PertResult ----------------" << endl;
    widget.setupUi(this);
    PertResultItemModel *m = new PertResultItemModel( part );
    widget.treeWidgetTaskResult->setModel( m );
    widget.treeWidgetTaskResult->setStretchLastSection( false );

//    QHeaderView *header=widget.treeWidgetTaskResult->header();
    setupGui();
    
    current_schedule=0;
    m_part = part;

        
    QList<int> lst1; lst1 << 1 << -1;
    QList<int> lst2; 
    for ( int i = 0; i < 24; ++i ) {
        lst2 << i;
    }
    lst2 << 33 << -1;
    widget.treeWidgetTaskResult->hideColumns( lst1, lst2 );
    
    connect( widget.treeWidgetTaskResult, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
}

void PertResult::draw( Project &project)
{
    setProject( &project );
    //draw();
}
  
void PertResult::draw()
{
    kDebug()<<m_project;
    widget.scheduleName->setText( i18n( "None" ) );
    widget.totalFloat->clear();
    if ( m_project && model()->manager() && model()->manager()->isScheduled() ) {
        long id = model()->manager()->id();
        if ( id == -1 ) {
            return;
        }
        widget.scheduleName->setText( model()->manager()->name() );
        KLocale *locale =  KGlobal::locale();
        Duration f;
        foreach ( Node *n, m_project->allNodes() ) {
            if ( n->type() == Node::Type_Task || n->type() == Node::Type_Milestone ) {
                f += static_cast<Task*>( n )->positiveFloat( id );
            }
        }
        widget.totalFloat->setText( locale->formatNumber( f.toDouble( Duration::Unit_h ) ) );
    }
/*    kDebug() << "UPDATE PE" << endl;
      widget.treeWidgetTaskResult->clear();
      if ( current_schedule == 0 || current_schedule->id() == -1 ) {
          return;
    }
    KLocale * locale = KGlobal::locale();
    QList<Node*> list;
    QString res;
    testComplexGraph();
    foreach(Node * currentNode, m_project->childNodeIterator()){
        if (currentNode->type()!=4){
 
            QTreeWidgetItem * item = new QTreeWidgetItem(widget.treeWidgetTaskResult );
            item->setText(0, currentNode->id());
            item->setText(1, currentNode->name());
            item->setText(2,locale->formatDateTime(getStartEarlyDate(currentNode)));
            item->setText(3,locale->formatDateTime(getFinishEarlyDate(currentNode)));
            item->setText(4,locale->formatDateTime(getStartLateDate(currentNode)));
            item->setText(5,locale->formatDateTime(getFinishLateDate(currentNode)));
            item->setText(6,res.number(getTaskFloat(currentNode).days()));
            item->setText(7,res.number(getFreeMargin(currentNode).days()));
        }
        widget.totalFloat->setText(res.number(getProjectFloat(m_project).days()));

    }
    list=criticalPath();
    QList<Node*>::iterator it=list.begin();
    while(it!=list.end()) 
    {
         res+=(*it)->id();
         it++;
         if(it!=list.end()) res+=" - ";
    }
    widget.labelResultCriticalPath->setText(res);*/
}

DateTime PertResult::getStartEarlyDate(Node * currentNode)
{
    DateTime duration;
    Task * t;
    t=static_cast<Task *>(currentNode);
    //if the task has no parent so the early date start is 0
    duration=t->earlyStart(current_schedule->id());
    duration.setDateOnly(true);
    return duration;
}

DateTime PertResult::getFinishEarlyDate(Node * currentNode)
{
    //it's the early start date + duration of the task
    Task * t;
    t=static_cast<Task *>(currentNode);
    DateTime duration;

    duration=t->earlyFinish(current_schedule->id());
    duration.setDateOnly(true);
    return (duration);
}
 
DateTime PertResult::getStartLateDate(Node * currentNode)
{
    Task * t;
    DateTime duration;
    t=static_cast<Task *>(currentNode);
    duration=t->lateStart(current_schedule->id());
    duration.setDateOnly(true);
    return (duration);

}


DateTime PertResult::getFinishLateDate(Node * currentNode)
{
    DateTime duration;
    Task * t;
    t=static_cast<Task *>(currentNode);
    duration=t->lateFinish(current_schedule->id());
    duration.setDateOnly(true);
    return duration;
}

Duration PertResult::getProjectFloat(Project *project)
{
    Duration duration;
    foreach(Node * currentNode, project->childNodeIterator() )
    {
	duration=duration+getTaskFloat(currentNode);
    }
    //duration.setDayOnly(true);
    return duration;
}

Duration PertResult::getFreeMargin(Node * currentNode)
{
    //search the small duration of the nextest task
    Task * t;
    DateTime duration;
    for (QList<Relation*>::iterator it=currentNode->dependChildNodes().begin();it!=currentNode->dependChildNodes().end();it++)
    {
        if(it==currentNode->dependChildNodes().begin())
        {
	    duration=getStartEarlyDate((*it)->child());
	}
	t=static_cast<Task *>((*it)->child ());
        if(getStartEarlyDate((*it)->child ())<duration)
	{
	    duration=getStartEarlyDate((*it)->child ());
	}
    }
    t=static_cast<Task *>(currentNode);
    duration.setDateOnly(true);
    return duration-(getStartEarlyDate(currentNode)+=(t->endTime()-t->startTime())); 
}

Duration PertResult::getTaskFloat(Node * currentNode)
{
    if(currentNode->dependChildNodes().size()==0  && complexGraph==true)
    {
         return getFinishLateDate(currentNode)-getStartEarlyDate(currentNode);
    }
    else
    {
        return getFinishLateDate(currentNode)-getFinishEarlyDate(currentNode);
    }
}

QList<Node*> PertResult::criticalPath()
{
    QList<Node*> list;
    foreach(Node * currentNode, m_node->childNodeIterator() )
    {
	if(currentNode->dependChildNodes().size()==0 && getFinishLateDate(currentNode)==getStartEarlyDate(currentNode))
    	{
          list.push_back(currentNode) ;
    	}
	else
	{
	   if(getFinishLateDate(currentNode)==getFinishEarlyDate(currentNode))
	   {
               list.push_back(currentNode) ;
	   }
	}
    }
    return list;
}
void PertResult::testComplexGraph()
{
    complexGraph=false;
    foreach(Node * currentNode, m_node->childNodeIterator() )
    {
	if(currentNode->dependParentNodes().size()>1)
	{
	    complexGraph=true;
	}
    } 
}

void PertResult::setupGui()
{
    // Add the context menu actions for the view options
    actionOptions = new KAction( KIcon("configure"), i18n("Configure..."), this );
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );
}

void PertResult::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug();
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos,  lst.first() );
    }
}

void PertResult::slotOptions()
{
    kDebug();
    ItemViewSettupDialog dlg( widget.treeWidgetTaskResult->slaveView() );
    dlg.exec();
}

void PertResult::slotUpdate(){

    draw();
}

void PertResult::slotScheduleSelectionChanged( ScheduleManager *sm )
{
    current_schedule = sm;
    model()->setManager( sm );
    draw();
}

void PertResult::slotProjectCalculated( ScheduleManager *sm )
{
    if ( sm && sm == model()->manager() ) {
        //draw();
        slotScheduleSelectionChanged( sm );
    }
}

void PertResult::slotScheduleManagerToBeRemoved( const ScheduleManager *sm )
{
    if ( sm == model()->manager() ) {
        current_schedule = 0;
        model()->setManager( 0 );
//        draw(); // clears view
    }
}

void PertResult::slotScheduleManagerChanged( ScheduleManager *sm )
{
    if ( current_schedule && current_schedule == sm ) {
        slotScheduleSelectionChanged( sm );
    }
}

void PertResult::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotUpdate() ) );
        disconnect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
        disconnect( m_project, SIGNAL( scheduleManagerToBeRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerToBeRemoved( const ScheduleManager* ) ) );
        disconnect( m_project, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), this, SLOT( slotScheduleManagerChanged( ScheduleManager* ) ) );
    }
    m_project = project;
    widget.treeWidgetTaskResult->model()->setProject( m_project );
    if ( m_project ) {
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotUpdate() ) );
        connect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
        connect( m_project, SIGNAL( scheduleManagerToBeRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerToBeRemoved( const ScheduleManager* ) ) );
        connect( m_project, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), this, SLOT( slotScheduleManagerChanged( ScheduleManager* ) ) );
    }
    draw();
}

bool PertResult::loadContext( const KoXmlElement &context )
{
    kDebug();
    return widget.treeWidgetTaskResult->loadContext( context );
}

void PertResult::saveContext( QDomElement &context ) const
{
    widget.treeWidgetTaskResult->saveContext( context );
}


//--------------------
PertCpmView::PertCpmView( KoDocument *part, QWidget *parent ) 
    : ViewBase( part, parent ),
    m_project( 0 ),
    current_schedule( 0 ),
    block( false )
{
    kDebug() << " ---------------- KPlato: Creating PertCpmView ----------------" << endl;
    widget.setupUi(this);
    widget.probabilityFrame->setVisible( false );

    widget.cpmTable->setStretchLastSection ( false );
    CriticalPathItemModel *m = new CriticalPathItemModel( part );
    widget.cpmTable->setModel( m );
    
    setupGui();
    
    QList<int> lst1; lst1 << 1 << -1;
    QList<int> show; show << 5 << 18 << 19 << 20 << 21 << 33 << 34 << 35 << 36;
    QList<int> lst2;
    for ( int i = 0; i < m->columnCount(); ++i ) {
        if ( ! show.contains( i ) ) {
            lst2 << i;
        }
    }
    widget.cpmTable->hideColumns( lst1, lst2 );
    widget.cpmTable->slaveView()->mapToSection( 33, 0 );
    widget.cpmTable->slaveView()->mapToSection( 34, 1 );
    widget.cpmTable->slaveView()->mapToSection( 35, 2 );
    widget.cpmTable->slaveView()->mapToSection( 36, 3 );
    widget.cpmTable->slaveView()->mapToSection( 5, 4 );
    widget.cpmTable->slaveView()->mapToSection( 18, 5 );
    widget.cpmTable->slaveView()->mapToSection( 19, 6 );
    widget.cpmTable->slaveView()->mapToSection( 20, 7 );
    widget.cpmTable->slaveView()->mapToSection( 21, 8 );
    
    connect( widget.cpmTable, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
    
    connect( widget.finishTime, SIGNAL( dateTimeChanged( const QDateTime& ) ), SLOT( slotFinishTimeChanged( const QDateTime& ) ) );
    
    connect( widget.probability, SIGNAL( valueChanged( int ) ), SLOT( slotProbabilityChanged( int ) ) );
}

void PertCpmView::setupGui()
{
    // Add the context menu actions for the view options
    actionOptions = new KAction( KIcon("configure"), i18n("Configure..."), this );
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );
}

void PertCpmView::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug();
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos,  lst.first() );
    }
}

void PertCpmView::slotOptions()
{
    kDebug();
    ItemViewSettupDialog dlg( widget.cpmTable->slaveView() );
    dlg.exec();
}

void PertCpmView::slotScheduleSelectionChanged( ScheduleManager *sm )
{
    bool enbl = sm && sm->isScheduled() && sm->usePert();
    widget.probabilityFrame->setVisible( enbl );
    current_schedule = sm;
    model()->setManager( sm );
    draw();
}

void PertCpmView::slotProjectCalculated( ScheduleManager *sm )
{
    if ( sm && sm == model()->manager() ) {
        slotScheduleSelectionChanged( sm );
    }
}

void PertCpmView::slotScheduleManagerChanged( ScheduleManager *sm )
{
    if ( current_schedule == sm ) {
        slotScheduleSelectionChanged( sm );
    }
}

void PertCpmView::slotScheduleManagerToBeRemoved( const ScheduleManager *sm )
{
    if ( sm == current_schedule ) {
        current_schedule = 0;
        model()->setManager( 0 );
        widget.probabilityFrame->setVisible( false );
    }
}

void PertCpmView::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotUpdate() ) );
        disconnect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
        disconnect( m_project, SIGNAL( scheduleManagerToBeRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerToBeRemoved( const ScheduleManager* ) ) );
        disconnect( m_project, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), this, SLOT( slotScheduleManagerChanged( ScheduleManager* ) ) );
    }
    m_project = project;
    widget.cpmTable->model()->setProject( m_project );
    if ( m_project ) {
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotUpdate() ) );
        connect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
        connect( m_project, SIGNAL( scheduleManagerToBeRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerToBeRemoved( const ScheduleManager* ) ) );
        connect( m_project, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), this, SLOT( slotScheduleManagerChanged( ScheduleManager* ) ) );
    }
    draw();
}

void PertCpmView::draw( Project &project )
{
    setProject( &project );
    // draw()
}

void PertCpmView::draw()
{
    widget.scheduleName->setText( i18n( "None" ) );
    if ( m_project && current_schedule && current_schedule->isScheduled() ) {
        long id = current_schedule->id();
        if ( id == -1 ) {
            return;
        }
        widget.scheduleName->setText( current_schedule->name() );
        widget.finishTime->setDateTime( m_project->endTime( id ).dateTime() );
    }
}

void PertCpmView::slotFinishTimeChanged( const QDateTime &dt )
{
    kDebug()<<dt;
    if ( block || m_project == 0 || current_schedule == 0 ) {
        return;
    }
    block = true;
    double var = model()->variance( Qt::EditRole ).toDouble();
    double dev = sqrt( var );
    DateTime et = m_project->endTime( current_schedule->id() );
    DateTime t = DateTime( KDateTime( dt ) );
    double d = ( et - t ).toDouble();
    d = t < et ? -d : d;
    double z = d / dev;
    double v = probability( z );
    widget.probability->setValue( (int)( v * 100 ) );
    kDebug()<<z<<", "<<v;
    block = false;
}

void PertCpmView::slotProbabilityChanged( int value )
{
    kDebug()<<value;
    if ( value == 0 || block || m_project == 0 || current_schedule == 0 ) {
        return;
    }
    block = true;
    double var = model()->variance( Qt::EditRole ).toDouble();
    double dev = sqrt( var );
    DateTime et = m_project->endTime( current_schedule->id() );
    double p = valueZ( value );
    DateTime t = et + Duration( qint64( p * dev ) );
    widget.finishTime->setDateTime( t.dateTime() );
    kDebug()<<p<<", "<<t.toString();
    block = false;
}

double PertCpmView::probability( double z ) const
{
    // TODO proper table
    double dist[][2] = { {0.0, 0.5}, {0.68, 0.7517}, {3.0, 0.9987}  };
    double p = 1.0;
    int i = 1;
    for ( ; i < 3; ++i ) {
        if ( QABS( z ) <= dist[i][0] ) {
            break;
        }
    }
    p = dist[i-1][1] + ( ( dist[i][1] - dist[i-1][1] ) * ( ( QABS(z) - dist[i-1][0] ) / (dist[i][0] - dist[i-1][0] ) ) );
    return z < 0 ? 1- p : p;
}

double PertCpmView::valueZ( double pr ) const
{
    // TODO proper table
    double prob[][2] = { {50.0, 0.0}, {75.0, 0.674}, {99.0, 2.326}  };
    double p = pr >= 50.0 ? pr : 100.0 - pr;
    double z = 3.0;
    int i = 1;
    for ( ; i < 3; ++i ) {
        if ( p < prob[i][0] ) {
            break;
        }
    }
    z = prob[i-1][1] + ( ( prob[i][1] - prob[i-1][1] ) * ( ( p - prob[i-1][0] ) / (prob[i][0] - prob[i-1][0] ) ) );
    return pr < 50.0 ? -z : z;
}

void PertCpmView::slotUpdate()
{
    draw();
}

bool PertCpmView::loadContext( const KoXmlElement &context )
{
    kDebug()<<objectName();
    return widget.cpmTable->loadContext( context );
}

void PertCpmView::saveContext( QDomElement &context ) const
{
    widget.cpmTable->saveContext( context );
}

} // namespace KPlato

#include "kptpertresult.moc"
