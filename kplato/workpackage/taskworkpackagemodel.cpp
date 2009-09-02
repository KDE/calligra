/* This file is part of the KDE project
  Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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

#include "taskworkpackagemodel.h"

#include "part.h"
#include "workpackage.h"

#include "kptglobal.h"
#include "kptresource.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptcommand.h"
#include "kptitemmodelbase.h"
#include "kpttaskcompletedelegate.h"

#include <QModelIndex>
#include <QMetaEnum>
#include <QObject>
#include <QAbstractItemDelegate>

using namespace KPlato;

namespace KPlatoWork
{

TaskWorkPackageModel::TaskWorkPackageModel( Part *part, QObject *parent )
    : ItemModelBase( parent ),
    m_part( part )
{
    connect( part, SIGNAL( workPackageAdded( WorkPackage*, int ) ), this, SLOT( addWorkPackage( WorkPackage*, int ) ) );
    connect( part, SIGNAL( workPackageRemoved( WorkPackage*, int ) ), this, SLOT( removeWorkPackage( WorkPackage*, int ) ) );
}

Qt::ItemFlags TaskWorkPackageModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags( index );
    flags &= ~( Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled );
    Node *n = nodeForIndex( index );
    if ( n == 0 ) {
        return flags;
    }
    if ( n->type() != Node::Type_Task && n->type() != Node::Type_Milestone ) {
        return flags;
    }
    Task *t = static_cast<Task*>( n );
    if ( ! t->completion().isStarted() ) {
        switch ( index.column() ) {
            case NodeActualStart:
                flags |= Qt::ItemIsEditable;
                break;
            case NodeCompleted:
                flags |= Qt::ItemIsEditable;
                break;
            default: break;
        }
    } else if ( ! t->completion().isFinished() ) {
        // task is running
        switch ( index.column() ) {
            case NodeActualFinish:
            case NodeCompleted:
            case NodeRemainingEffort:
                flags |= Qt::ItemIsEditable;
                break;
            case NodeActualEffort:
                if ( t->completion().entrymode() == Completion::EnterEffortPerTask || t->completion().entrymode() == Completion::EnterEffortPerResource ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            default: break;
        }
    }
    return flags;
}

void TaskWorkPackageModel::slotNodeToBeInserted( Node *parent, int row )
{
    //kDebug()<<parent->name()<<"; "<<row;
    beginInsertRows( indexForNode( parent ), row, row );
}

void TaskWorkPackageModel::slotNodeInserted( Node *node )
{
    //kDebug()<<node->parentNode()->name()<<"-->"<<node->name();
    endInsertRows();
}

void TaskWorkPackageModel::slotNodeToBeRemoved( Node *node )
{
    //kDebug()<<node->name();
    int row = indexForNode( node ).row();
    beginRemoveRows( indexForNode( node->parentNode() ), row, row );
}

void TaskWorkPackageModel::slotNodeRemoved( Node *node )
{
    //kDebug()<<node->name();
    endRemoveRows();
}

void TaskWorkPackageModel::slotNodeChanged( Node *node )
{
    if ( node == 0 || node->type() == Node::Type_Project ) {
        return;
    }
    int row = node->parentNode()->indexOf( node );
    kDebug()<<node->name()<<row;
    emit dataChanged( createIndex( row, 0, node->parentNode() ), createIndex( row, columnCount()-1, node->parentNode() ) );
}

void TaskWorkPackageModel::addWorkPackage( WorkPackage *package, int row )
{
    beginInsertRows( QModelIndex(), row, row );
    endInsertRows();
    Project *project = package->project();
    kDebug()<<package->project();
    if ( project ) {
        connect( project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        connect( project, SIGNAL( nodeToBeAdded( Node*, int ) ), this, SLOT( slotNodeToBeInserted(  Node*, int ) ) );
        connect( project, SIGNAL( nodeToBeRemoved( Node* ) ), this, SLOT( slotNodeToBeRemoved( Node* ) ) );

        connect( project, SIGNAL( nodeAdded( Node* ) ), this, SLOT( slotNodeInserted( Node* ) ) );
        connect( project, SIGNAL( nodeRemoved( Node* ) ), this, SLOT( slotNodeRemoved( Node* ) ) );
    }
}

void TaskWorkPackageModel::removeWorkPackage( WorkPackage *package, int row )
{
    beginRemoveRows( QModelIndex(), row, row );
    Project *project = package->project();
    kDebug()<<package->project();
    if ( project ) {
        disconnect( project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        disconnect( project, SIGNAL( nodeToBeAdded( Node*, int ) ), this, SLOT( slotNodeToBeInserted(  Node*, int ) ) );
        disconnect( project, SIGNAL( nodeToBeRemoved( Node* ) ), this, SLOT( slotNodeToBeRemoved( Node* ) ) );

        disconnect( project, SIGNAL( nodeAdded( Node* ) ), this, SLOT( slotNodeInserted( Node* ) ) );
        disconnect( project, SIGNAL( nodeRemoved( Node* ) ), this, SLOT( slotNodeRemoved( Node* ) ) );
    }
    endRemoveRows();
}

QVariant TaskWorkPackageModel::name( const Resource *r, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return r->name();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant TaskWorkPackageModel::email( const Resource *r, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return r->email();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant TaskWorkPackageModel::projectName( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole: {
            const Node *proj = node->projectNode();
            return proj == 0 ? QVariant() : proj->name();
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant TaskWorkPackageModel::projectManager( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole: {
            const Node *proj = node->projectNode();
            return proj == 0 ? QVariant() : proj->leader();
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

int TaskWorkPackageModel::rowCount( const QModelIndex &parent ) const
{
    if ( ! parent.isValid() ) {
        //qDebug()<<"TaskWorkPackageModel::rowCount:"<<parent<<"nodes:"<<m_part->workPackageCount();
        return m_part->workPackageCount(); // == no of nodes (1 node pr wp)
    }
    Node *n = nodeForIndex( parent );
    if ( n ) {
        //qDebug()<<"TaskWorkPackageModel::rowCount:"<<parent<<"docs:"<<n->documents().count();
        return n->documents().count();
    }
    //qDebug()<<"TaskWorkPackageModel::rowCount:"<<parent<<"rows:"<<0;
    return 0; // documents have no children
}

int TaskWorkPackageModel::columnCount( const QModelIndex & ) const
{
    return columnMap().keyCount();
}

QVariant TaskWorkPackageModel::data( const QModelIndex &index, int role ) const
{
    if ( ! index.isValid() ) {
        return QVariant();
    }
    Node *n = nodeForIndex( index );
    if ( n ) {
        return nodeData( n, index.column(), role );
    }
    Document *doc = documentForIndex( index );
    if ( doc ) {
        return documentData( doc, index.column(), role );
    }
    return QVariant();
}

QVariant TaskWorkPackageModel::nodeData( Node *n, int column, int role ) const
{
    switch ( column ) {
        case NodeName: return m_nodemodel.data( n, NodeModel::NodeName, role );
        case NodeType: return m_nodemodel.data( n, NodeModel::NodeType, role );
        case NodeResponsible: return m_nodemodel.data( n, NodeModel::NodeResponsible, role );
        case NodeDescription: return m_nodemodel.data( n, NodeModel::NodeDescription, role );

        // After scheduling
        case NodeStartTime: return m_nodemodel.data( n, NodeModel::NodeStartTime, role );
        case NodeEndTime: return m_nodemodel.data( n, NodeModel::NodeEndTime, role );
        case NodeAssignments: return m_nodemodel.data( n, NodeModel::NodeAssignments, role );

        // Completion
        case NodeCompleted: return m_nodemodel.data( n, NodeModel::NodeCompleted, role );
        case NodePlannedEffort: return m_nodemodel.data( n, NodeModel::NodePlannedEffort, role );
        case NodeActualEffort: return m_nodemodel.data( n, NodeModel::NodeActualEffort, role );
        case NodeRemainingEffort: return m_nodemodel.data( n, NodeModel::NodeRemainingEffort, role );
        case NodeActualStart: return m_nodemodel.data( n, NodeModel::NodeActualStart, role );
        case NodeStarted: return m_nodemodel.data( n, NodeModel::NodeStarted, role );
        case NodeActualFinish: return m_nodemodel.data( n, NodeModel::NodeActualFinish, role );
        case NodeFinished: return m_nodemodel.data( n, NodeModel::NodeFinished, role );
        case NodeStatusNote: return m_nodemodel.data( n, NodeModel::NodeStatusNote, role );

        case ProjectName: return projectName( n, role );
        case ProjectManager: return projectManager( n, role );

        default:
            //kDebug()<<"Invalid column number: "<<index.column()<<endl;;
            break;
    }
    return "";
}

QVariant TaskWorkPackageModel::documentData( Document *doc, int column, int role ) const
{
    //qDebug()<<"TaskWorkPackageModel::documentData:"<<doc->url().fileName()<<column<<role;
    if ( role != Qt::DisplayRole ) {
        return QVariant();
    }
    switch ( column ) {
        case 0: return doc->url().fileName();
        default:
            return "";
    }
    return QVariant();
}

bool TaskWorkPackageModel::setCompletion( Node *node, const QVariant &value, int role )
{
    if ( role != Qt::EditRole ) {
        return false;
    }
    if ( node->type() == Node::Type_Task ) {
        Completion &c = static_cast<Task*>( node )->completion();
        QDateTime dt = QDateTime::currentDateTime();
        QDate date = dt.date();
        // xgettext: no-c-format
        MacroCommand *m = new MacroCommand( i18n( "Modify % Completed" ) );
        if ( ! c.isStarted() ) {
            m->addCommand( new ModifyCompletionStartedCmd( c, true ) );
            m->addCommand( new ModifyCompletionStartTimeCmd( c, dt ) );
        }
        m->addCommand( new ModifyCompletionPercentFinishedCmd( c, date, value.toInt() ) );
        if ( value.toInt() == 100 ) {
            m->addCommand( new ModifyCompletionFinishedCmd( c, true ) );
            m->addCommand( new ModifyCompletionFinishTimeCmd( c, dt ) );
        }
        emit executeCommand( m ); // also adds a new entry if necessary
        if ( c.entrymode() == Completion::EnterCompleted ) {
            Duration planned = static_cast<Task*>( node )->plannedEffort( m_nodemodel.id() );
            Duration actual = ( planned * value.toInt() ) / 100;
            kDebug()<<planned.toString()<<value.toInt()<<actual.toString();
            NamedCommand *cmd = new ModifyCompletionActualEffortCmd( c, date, actual );
            cmd->execute();
            m->addCommand( cmd );
            cmd = new ModifyCompletionRemainingEffortCmd( c, date, planned - actual  );
            cmd->execute();
            m->addCommand( cmd );
        }
        return true;
    }
    if ( node->type() == Node::Type_Milestone ) {
        Completion &c = static_cast<Task*>( node )->completion();
        if ( value.toInt() > 0 ) {
            QDateTime dt = QDateTime::currentDateTime();
            QDate date = dt.date();
            MacroCommand *m = new MacroCommand( i18n( "Set finished" ) );
            m->addCommand( new ModifyCompletionStartedCmd( c, true ) );
            m->addCommand( new ModifyCompletionStartTimeCmd( c, dt ) );
            m->addCommand( new ModifyCompletionFinishedCmd( c, true ) );
            m->addCommand( new ModifyCompletionFinishTimeCmd( c, dt ) );
            m->addCommand( new ModifyCompletionPercentFinishedCmd( c, date, 100 ) );
            emit executeCommand( m ); // also adds a new entry if necessary
            return true;
        }
        return false;
    }
    return false;
}

bool TaskWorkPackageModel::setRemainingEffort( Node *node, const QVariant &value, int role )
{
    if ( role == Qt::EditRole && node->type() == Node::Type_Task ) {
        Task *t = static_cast<Task*>( node );
        double d( value.toList()[0].toDouble() );
        Duration::Unit unit = static_cast<Duration::Unit>( value.toList()[1].toInt() );
        Duration dur( d, unit );
        emit executeCommand( new ModifyCompletionRemainingEffortCmd( t->completion(), QDate::currentDate(), dur, i18n( "Modify Remaining Effort" ) ) );
        return true;
    }
    return false;
}

bool TaskWorkPackageModel::setActualEffort( Node *node, const QVariant &value, int role )
{
    if ( role == Qt::EditRole && node->type() == Node::Type_Task ) {
        Task *t = static_cast<Task*>( node );
        double d( value.toList()[0].toDouble() );
        Duration::Unit unit = static_cast<Duration::Unit>( value.toList()[1].toInt() );
        Duration dur( d, unit );
        emit executeCommand( new ModifyCompletionActualEffortCmd( t->completion(), QDate::currentDate(), dur, i18n( "Modify Actual Effort" ) ) );
        return true;
    }
    return false;
}

bool TaskWorkPackageModel::setStartedTime( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole: {
            Task *t = qobject_cast<Task*>( node );
            if ( t == 0 ) {
                return false;
            }
            MacroCommand *m = new MacroCommand( headerData( NodeModel::NodeActualStart, Qt::Horizontal, Qt::DisplayRole ).toString() ); //FIXME: proper description when string freeze is lifted
            if ( ! t->completion().isStarted() ) {
                m->addCommand( new ModifyCompletionStartedCmd( t->completion(), true ) );
            }
            m->addCommand( new ModifyCompletionStartTimeCmd( t->completion(), value.toDateTime() ) );
            if ( t->type() == Node::Type_Milestone ) {
                m->addCommand( new ModifyCompletionFinishedCmd( t->completion(), true ) );
                m->addCommand( new ModifyCompletionFinishTimeCmd( t->completion(), value.toDateTime() ) );
                if ( t->completion().percentFinished() < 100 ) {
                    Completion::Entry *e = new Completion::Entry( 100, Duration::zeroDuration, Duration::zeroDuration );
                    m->addCommand( new AddCompletionEntryCmd( t->completion(), value.toDate(), e ) );
                }
            }
            emit executeCommand( m );
            return true;
        }
    }
    return false;
}

bool TaskWorkPackageModel::setFinishedTime( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole: {
            Task *t = qobject_cast<Task*>( node );
            if ( t == 0 ) {
                return false;
            }
            MacroCommand *m = new MacroCommand( headerData( NodeModel::NodeActualFinish, Qt::Horizontal, Qt::DisplayRole ).toString() ); //FIXME: proper description when string freeze is lifted
            if ( ! t->completion().isFinished() ) {
                m->addCommand( new ModifyCompletionFinishedCmd( t->completion(), true ) );
                if ( t->completion().percentFinished() < 100 ) {
                    Completion::Entry *e = new Completion::Entry( 100, Duration::zeroDuration, Duration::zeroDuration );
                    m->addCommand( new AddCompletionEntryCmd( t->completion(), value.toDate(), e ) );
                }
            }
            m->addCommand( new ModifyCompletionFinishTimeCmd( t->completion(), value.toDateTime() ) );
            if ( t->type() == Node::Type_Milestone ) {
                m->addCommand( new ModifyCompletionStartedCmd( t->completion(), true ) );
                m->addCommand( new ModifyCompletionStartTimeCmd( t->completion(), value.toDateTime() ) );
            }
            emit executeCommand( m );
            return true;
        }
    }
    return false;
}

bool TaskWorkPackageModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( ! index.isValid() ) {
        return ItemModelBase::setData( index, value, role );
    }
    switch ( index.column() ) {
        case NodeCompleted:
            return setCompletion( nodeForIndex( index ), value, role );
        case NodeRemainingEffort:
            return setRemainingEffort( nodeForIndex( index ), value, role );
        case NodeActualEffort:
            return setActualEffort( nodeForIndex( index ), value, role );
        case NodeActualStart:
            return setStartedTime( nodeForIndex( index ), value, role );
        case NodeActualFinish:
            return setFinishedTime( nodeForIndex( index ), value, role );
        default:
            break;
    }
    return false;
}

QVariant TaskWorkPackageModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Vertical ) {
        return section;
    }
    if ( role == Qt::DisplayRole ) {
        switch ( section ) {
        case NodeName: return i18n( "Name" );
        case NodeType: return i18n( "Type" );
        case NodeResponsible: return i18n( "Responsible" );
        case NodeDescription: return i18n( "Description" );

        // After scheduling
        case NodeStartTime: return i18n( "Planned Start" );
        case NodeEndTime: return i18n( "Planned Finish" );
        case NodeAssignments: return i18n( "Resource Assignments" );

        // Completion
        case NodeCompleted: return i18n( "Completion" );
        case NodePlannedEffort: return i18n( "Planned Effort" );
        case NodeActualEffort: return i18n( "Actual Effort" );
        case NodeRemainingEffort: return i18n( "Remaining Effort" );
        case NodeActualStart: return i18n( "Actual Start" );
        case NodeStarted: return i18n( "Started" );
        case NodeActualFinish: return i18n( "Actual Finish" );
        case NodeFinished: return i18n( "Finished" );
        case NodeStatusNote: return i18n( "Note" );

        case ProjectName: return i18n( "Project Name" );
        case ProjectManager: return i18n( "Project Manager" );

        default:
            //kDebug()<<"Invalid column number: "<<index.column()<<endl;;
            break;
    }
    }
    return QVariant();
}

QModelIndex TaskWorkPackageModel::parent( const QModelIndex &idx ) const
{
    if ( ! idx.isValid() ) {
        return QModelIndex();
    }
    if ( isDocument( idx ) ) {
        // a document index has a node as parent
        return indexForNode( ptrToNode( idx ) );
    }
    // a node index has no parent
    return QModelIndex();
}

QModelIndex TaskWorkPackageModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( ! parent.isValid() ) {
        // create a node index
        return createIndex( row, column, workPackage( row ) );
    }
    if ( isNode( parent ) ) {
        // create a document index
        return createIndex( row, column, nodeForIndex( parent ) );
    }
    // documents don't have children, so shouldn't get here
    return QModelIndex();
}

Node *TaskWorkPackageModel::nodeForIndex( const QModelIndex &index ) const
{
    WorkPackage *wp = ptrToWorkPackage( index );
    if ( wp ) {
        //qDebug()<<"TaskWorkPackageModel::nodeForIndex:"<<index<<parent->node()->name();
        return wp->node();
    }
    return 0;
}

Document *TaskWorkPackageModel::documentForIndex( const QModelIndex &index ) const
{
    if ( index.isValid() ) {
        Node *parent = ptrToNode( index );
        if ( parent && index.row() < parent->documents().count() ) {
            //qDebug()<<"TaskWorkPackageModel::documentForIndex:"<<index<<parent->name();
            return parent->documents().value( index.row() );
        }
    }
    return 0;
}

QModelIndex TaskWorkPackageModel::indexForNode( Node *node ) const
{
    WorkPackage *p = m_part->workPackage( node );
    if ( p == 0 ) {
        return QModelIndex();
    }
    return createIndex( m_part->indexOf( p ), 0, p );
}

WorkPackage *TaskWorkPackageModel::workPackage( int index ) const
{
    return m_part->workPackage( index );
}

QAbstractItemDelegate *TaskWorkPackageModel::createDelegate( int column, QWidget *parent ) const
{
    switch ( column ) {
        case NodeCompleted: return new TaskCompleteDelegate( parent );
        case NodeRemainingEffort: return new DurationSpinBoxDelegate( parent );
        case NodeActualEffort: return new DurationSpinBoxDelegate( parent );

        default: break;
    }
    return 0;
}

WorkPackage *TaskWorkPackageModel::ptrToWorkPackage( const QModelIndex &idx ) const
{
    return qobject_cast<WorkPackage*>( static_cast<QObject*>( idx.internalPointer() ) );
}

Node *TaskWorkPackageModel::ptrToNode( const QModelIndex &idx ) const
{
    return qobject_cast<Node*>( static_cast<QObject*>( idx.internalPointer() ) );
}

bool TaskWorkPackageModel::isNode( const QModelIndex &idx ) const
{
    // a node index: ptr is WorkPackage*
    return qobject_cast<WorkPackage*>( static_cast<QObject*>( idx.internalPointer() ) ) != 0;
}

bool TaskWorkPackageModel::isDocument( const QModelIndex &idx ) const
{
    // a document index: ptr is Node*
    return qobject_cast<Node*>( static_cast<QObject*>( idx.internalPointer() ) ) != 0;
}


} //namespace KPlato

#include "taskworkpackagemodel.moc"
