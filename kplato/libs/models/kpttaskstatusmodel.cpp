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
#include "kpttaskcompletedelegate.h"
#include "kptcommand.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptnodeitemmodel.h"

#include <QAbstractItemModel>
#include <QMimeData>
#include <QModelIndex>

#include <kglobal.h>
#include <klocale.h>

namespace KPlato
{


TaskStatusItemModel::TaskStatusItemModel( QObject *parent )
    : ItemModelBase( parent ),
    m_period( 7 ),
    m_periodType( UseCurrentDate ),
    m_weekday( Qt::Friday )

{
    m_topNames << i18n( "Not Started" );
    m_topTips << i18n( "Tasks that should have been started" );
    m_top.append(&m_notstarted );
    
    m_topNames << i18n( "Running" );
    m_topTips << i18n( "Tasks that are running" );
    m_top.append(&m_running );
    
    m_topNames << i18n( "Finished" );
    m_topTips << i18n( "Tasks that have finished during this period" );
    m_top.append(&m_finished );
    
    m_topNames << i18n( "Next Period" );
    m_topTips << i18n( "Tasks that are scheduled to start next period" );
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

void TaskStatusItemModel::slotNodeInserted( Node * /*node*/ )
{
    //kDebug()<<node->getParent->name()<<"-->"<<node->name();
    refresh();
}

void TaskStatusItemModel::slotNodeToBeRemoved( Node * /*node*/ )
{
    //kDebug()<<node->name();
    clear();
}

void TaskStatusItemModel::slotNodeRemoved( Node * /*node*/ )
{
    //kDebug()<<node->name();
    refresh();
}

void TaskStatusItemModel::setProject( Project *project )
{
    clear();
    if ( m_project ) {
        disconnect( m_project, SIGNAL( localeChanged() ), this, SLOT( slotLayoutChanged() ) );
        disconnect( m_project, SIGNAL( wbsDefinitionChanged() ), this, SLOT( slotWbsDefinitionChanged() ) );
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
        connect( m_project, SIGNAL( localeChanged() ), this, SLOT( slotLayoutChanged() ) );
        connect( m_project, SIGNAL( wbsDefinitionChanged() ), this, SLOT( slotWbsDefinitionChanged() ) );
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

void TaskStatusItemModel::setManager( ScheduleManager *sm )
{
    clear();
    if ( m_nodemodel.manager() ) {
    }
    m_nodemodel.setManager( sm );
    if ( sm ) {
    }
    reset();
    refresh();
}

void TaskStatusItemModel::clear()
{
    foreach ( NodeMap *l, m_top ) {
        int c = l->count();
        if ( c > 0 ) {
            //FIXME: gives error msg:
            // Can't select indexes from different model or with different parents
            QModelIndex i = index( l );
            kDebug()<<i<<0<<c-1;
            beginRemoveRows( i, 0, c-1 );
            l->clear();
            endRemoveRows();
        }
    }
}

void TaskStatusItemModel::setNow()
{
    switch ( m_periodType ) {
        case UseWeekday: {
            QDate date = QDate::currentDate();
            int wd = date.dayOfWeek();
            date = date.addDays( m_weekday - wd );
            if ( wd < m_weekday ) {
                date = date.addDays( -7 );
            }
            m_nodemodel.setNow( date );
            break; }
        case UseCurrentDate: m_nodemodel.setNow( QDate::currentDate() ); break;
        default: 
            m_nodemodel.setNow( QDate::currentDate() );
            break;
    }
}

void TaskStatusItemModel::refresh()
{
    //kDebug();
    clear();
    if ( m_project == 0 ) {
        return;
    }
    m_id = m_nodemodel.id();
    if ( m_id == -1 ) {
        return;
    }
    setNow();
    QDate begin = m_nodemodel.now().addDays( -m_period );
    QDate end = m_nodemodel.now().addDays( m_period );
    
    foreach( Node* n, m_project->allNodes() ) {
        if ( n->type() != Node::Type_Task && n->type() != Node::Type_Milestone ) {
            continue;
        }
        Task *t = static_cast<Task*>( n );
        const Completion &c = t->completion();
        if ( c.isFinished() ) {
            if ( c.finishTime().date() > begin ) {
                m_finished.insert( t->wbsCode(), t );
            }
        } else if ( c.isStarted() ) {
            m_running.insert( t->wbsCode(), t );
        } else if ( t->startTime( m_id ).date() < m_nodemodel.now() ) {
            // should have been started
            m_notstarted.insert( t->wbsCode(), t );
        } else if ( t->startTime( m_id ).date() <= end ) {
            // start next period
            m_upcoming.insert( t->wbsCode(), t );
        }
    }
    foreach ( NodeMap *l, m_top ) {
        int c = l->count();
        if ( c > 0 ) {
            kDebug()<<index(l)<<0<<c-1;
            beginInsertRows( index( l ), 0, c-1 );
            endInsertRows();
        }
    }
    emit layoutChanged(); //HACK to get views updated
}

Qt::ItemFlags TaskStatusItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags( index );
    flags &= ~( Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled );
    Node *n = node( index );
    if ( ! m_readWrite || n == 0 || m_id == -1 || ! n->isScheduled( m_id ) ) {
        return flags;
    }
    if ( n->type() != Node::Type_Task && n->type() != Node::Type_Milestone ) {
        return flags;
    }
    Task *t = static_cast<Task*>( n );
    if ( ! t->completion().isStarted() ) {
        switch ( index.column() ) {
            case NodeModel::NodeActualStart:
                flags |= Qt::ItemIsEditable;
                break;
            case NodeModel::NodeCompleted:
                if ( t->state() & Node::State_ReadyToStart ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            default: break;
        }
    } else if ( ! t->completion().isFinished() ) {
        // task is running
        switch ( index.column() ) {
            case NodeModel::NodeActualFinish:
            case NodeModel::NodeCompleted:
            case NodeModel::NodeRemainingEffort:
                flags |= Qt::ItemIsEditable;
                break;
            case NodeModel::NodeActualEffort:
                if ( t->completion().entrymode() == Completion::EnterEffortPerTask || t->completion().entrymode() == Completion::EnterEffortPerResource ) {
                    flags |= Qt::ItemIsEditable;
                }
                break;
            default: break;
        }
    }
    return flags;
}

    
QModelIndex TaskStatusItemModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() ) {
        return QModelIndex();
    }
    //kDebug()<<index.internalPointer()<<":"<<index.row()<<","<<index.column();
    int row = m_top.indexOf( static_cast<NodeMap*>( index.internalPointer() ) );
    if ( row != -1 ) {
        return QModelIndex(); // top level has no parent
    }
    Node *n = node( index );
    if ( n == 0  ) {
        return QModelIndex();
    }
    NodeMap *lst = 0;
    foreach ( NodeMap *l, m_top ) {
        if ( l->values().indexOf( n ) != -1 ) {
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
    //kDebug()<<row<<column<<parent;
    if ( m_project == 0 || column < 0 || column >= columnCount() || row < 0 ) {
        return QModelIndex();
    }
    if ( ! parent.isValid() ) {
        if ( row >= m_top.count() ) {
            return QModelIndex();
        }
        return createIndex(row, column, m_top.value( row ) );
    }
    NodeMap *l = list( parent );
    if ( l == 0 ) {
        return QModelIndex();
    }
    if ( row >= rowCount( parent ) ) {
        kWarning()<<"Row >= rowCount, Qt4.4 asks, so we need to handle it"<<parent<<row<<column;
        return QModelIndex();
    }
    QModelIndex i = createIndex(row, column, l->values().value( row ) );
    Q_ASSERT( i.internalPointer() != 0 );
    return i;
}

QModelIndex TaskStatusItemModel::index( const Node *node ) const
{
    if ( m_project == 0 || node == 0 ) {
        return QModelIndex();
    }
    foreach( NodeMap *l, m_top ) {
        int row = l->values().indexOf( const_cast<Node*>( node ) );
        if ( row != -1 ) {
            return createIndex( row, 0, const_cast<Node*>( node ) );
        }
    }
    return QModelIndex();
}

QModelIndex TaskStatusItemModel::index( const NodeMap *lst ) const
{
    if ( m_project == 0 || lst == 0 ) {
        return QModelIndex();
    }
    NodeMap *l = const_cast<NodeMap*>( lst );
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
            return m_topNames.value( row );
        case Qt::ToolTipRole:
            return m_topTips.value( row );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool TaskStatusItemModel::setCompletion( Node *node, const QVariant &value, int role )
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

bool TaskStatusItemModel::setRemainingEffort( Node *node, const QVariant &value, int role )
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

bool TaskStatusItemModel::setActualEffort( Node *node, const QVariant &value, int role )
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

bool TaskStatusItemModel::setStartedTime( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole: {
            Task *t = qobject_cast<Task*>( node );
            if ( t == 0 ) {
                return false;
            }
            MacroCommand *m = new MacroCommand( i18n( "Modify actual start time" ) );
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

bool TaskStatusItemModel::setFinishedTime( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole: {
            Task *t = qobject_cast<Task*>( node );
            if ( t == 0 ) {
                return false;
            }
            MacroCommand *m = new MacroCommand( i18n( "Modify actual finish time" ) );
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
            case NodeModel::NodeName: return name( index.row(), role );
            default: break;
        }
        return QVariant();
    }
    result = m_nodemodel.data( n, index.column(), role );
    if ( role == Qt::DisplayRole ) {
        switch ( index.column() ) {
            case NodeModel::NodeActualStart:
                if ( ! result.isValid() ) {
                    return m_nodemodel.data( n, NodeModel::NodeStatus, role );
                }
            break;
        }
    } else if ( role == Qt::EditRole ) {
        switch ( index.column() ) {
            case NodeModel::NodeActualStart:
            case NodeModel::NodeActualFinish:
                if ( ! result.isValid() ) {
                    return QDateTime::currentDateTime();
                }
            break;
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


bool TaskStatusItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( ! index.isValid() ) {
        return ItemModelBase::setData( index, value, role );
    }
    switch ( index.column() ) {
        case NodeModel::NodeCompleted:
            return setCompletion( node( index ), value, role );
        case NodeModel::NodeRemainingEffort:
            return setRemainingEffort( node( index ), value, role );
        case NodeModel::NodeActualEffort:
            return setActualEffort( node( index ), value, role );
        case NodeModel::NodeActualStart:
            return setStartedTime( node( index ), value, role );
        case NodeModel::NodeActualFinish:
            return setFinishedTime( node( index ), value, role );
        default:
            break;
    }
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

QAbstractItemDelegate *TaskStatusItemModel::createDelegate( int column, QWidget *parent ) const
{
    switch ( column ) {
        case NodeModel::NodeCompleted: return new TaskCompleteDelegate( parent );
        case NodeModel::NodeRemainingEffort: return new DurationSpinBoxDelegate( parent );
        case NodeModel::NodeActualEffort: return new DurationSpinBoxDelegate( parent );
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
        //kDebug()<<"top="<<m_top.count()<<m_top;
        return m_top.count();
    }
    NodeMap *l = list( parent );
    if ( l ) {
        //kDebug()<<"list"<<parent.row()<<":"<<l->count()<<l<<m_topNames.value( parent.row() );
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
    foreach (const QModelIndex &index, indexes) {
        if ( index.isValid() && !rows.contains( index.row() ) ) {
            //kDebug()<<index.row();
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

NodeMap *TaskStatusItemModel::list( const QModelIndex &index ) const
{
    if ( index.isValid() ) {
        Q_ASSERT( index.internalPointer() );
        if ( m_top.contains( static_cast<NodeMap*>( index.internalPointer() ) ) ) {
            return static_cast<NodeMap*>( index.internalPointer() );
        }
    }
    return 0;
}

Node *TaskStatusItemModel::node( const QModelIndex &index ) const
{
    if ( index.isValid() ) {
        foreach ( NodeMap *l, m_top ) {
            int row = l->values().indexOf( static_cast<Node*>( index.internalPointer() ) );
            if ( row != -1 ) {
                return static_cast<Node*>( index.internalPointer() );
            }
        }
    }
    return 0;
}

void TaskStatusItemModel::slotNodeChanged( Node *node )
{
    kDebug();
    if ( node == 0 || node->type() == Node::Type_Project ) {
        return;
    }
    QString wbs = node->wbsCode();
    int row = -1;
    if ( m_notstarted.value( wbs ) == node ) {
        row = m_notstarted.keys().indexOf( wbs );
    } else if ( m_running.value( wbs ) == node ) {
        row = m_running.keys().indexOf( wbs );
    } else if ( m_finished.value( wbs ) == node ) {
        row = m_finished.keys().indexOf( wbs );
    } else if ( m_upcoming.value( wbs ) == node ) {
        row = m_upcoming.keys().indexOf( wbs );
    }
    if ( row >= 0 ) {
        emit dataChanged( createIndex( row, 0, node ), createIndex( row, columnCount(), node ) );
    }
}

void TaskStatusItemModel::slotWbsDefinitionChanged()
{
    kDebug();
    foreach ( NodeMap *l, m_top ) {
        for ( int row = 0; row < l->count(); ++row ) {
            emit dataChanged( createIndex( row, NodeModel::NodeWBSCode, l->values().value( row ) ), createIndex( row, NodeModel::NodeWBSCode, l->values().value( row ) ) );
        }
    }
}

void TaskStatusItemModel::slotLayoutChanged()
{
    //kDebug()<<node->name();
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

} // namespace KPlato

#include "kpttaskstatusmodel.moc"
