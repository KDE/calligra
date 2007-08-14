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

#include "kpttaskstatusview.h"

#include "kptglobal.h"
#include "kptitemmodelbase.h"
#include "kptitemviewsettup.h"
#include "kptcommand.h"
#include "kptfactory.h"
#include "kptproject.h"
#include "kptview.h"

#include <QAbstractItemModel>
#include <QApplication>
#include <QComboBox>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QHeaderView>
#include <QItemDelegate>
#include <QItemSelectionModel>
#include <QLineEdit>
#include <QMap>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QVBoxLayout>
#include <QWidget>

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


TaskStatusItemModel::TaskStatusItemModel( Part *part, QObject *parent )
    : ItemModelBase( part, parent ),
    m_now( QDate::currentDate() ),
    m_period( 7 )
{
    m_topNames << i18n( "Not Started" );
    m_top.append(&m_notstarted );
    
    m_topNames << i18n( "Running" );
    m_top.append(&m_running );
    
    m_topNames << i18n( "Finished" );
    m_top.append(&m_finished );
    
    m_topNames << i18n( "Next Period" );
    m_top.append(&m_upcomming );
    
/*    connect( this, SIGNAL( modelAboutToBeReset() ), SLOT( slotAboutToBeReset() ) );
    connect( this, SIGNAL( modelReset() ), SLOT( slotReset() ) );*/
}

TaskStatusItemModel::~TaskStatusItemModel()
{
}
    
void TaskStatusItemModel::slotAboutToBeReset()
{
    kDebug()<<k_funcinfo;
    clear();
}

void TaskStatusItemModel::slotReset()
{
    kDebug()<<k_funcinfo;
    refresh();
}

void TaskStatusItemModel::slotNodeToBeInserted( Node *, int )
{
    //kDebug()<<k_funcinfo<<node->name();
    clear();
}

void TaskStatusItemModel::slotNodeInserted( Node */*node*/ )
{
    //kDebug()<<k_funcinfo<<node->getParent->name()<<"-->"<<node->name();
    refresh();
}

void TaskStatusItemModel::slotNodeToBeRemoved( Node */*node*/ )
{
    //kDebug()<<k_funcinfo<<node->name();
    clear();
}

void TaskStatusItemModel::slotNodeRemoved( Node */*node*/ )
{
    //kDebug()<<k_funcinfo<<node->name();
    refresh();
}

void TaskStatusItemModel::setProject( Project *project )
{
    clear();
    if ( m_project ) {
        disconnect( m_project, SIGNAL( currentViewScheduleIdChanged( long ) ), this, SLOT( slotReset() ) );
        
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeToBeAdded( Node* ) ), this, SLOT( slotNodeToBeInserted(  Node*, int ) ) );
        disconnect( m_project, SIGNAL( nodeToBeRemoved( Node* ) ), this, SLOT( slotNodeToBeRemoved( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeToBeMoved( Node* ) ), this, SLOT( slotLayoutToBeChanged() ) );
    
        disconnect( m_project, SIGNAL( nodeAdded( Node* ) ), this, SLOT( slotNodeInserted( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeRemoved( Node* ) ), this, SLOT( slotNodeRemoved( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeMoved( Node* ) ), this, SLOT( slotLayoutChanged() ) );
    }
    m_project = project;
    if ( project ) {
        connect( m_project, SIGNAL( currentViewScheduleIdChanged( long ) ), this, SLOT( slotReset() ) );
        
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
    QDate begin = m_now.addDays( m_period );
    QDate end = m_now.addDays( m_period );
    
    m_id = m_project->currentViewScheduleId();
    
    foreach( Node* n, m_project->allNodes() ) {
        if ( n->type() != Node::Type_Task ) {
            continue;
        }
        Task *t = static_cast<Task*>( n );
        const Completion &c = t->completion();
        if ( c.isFinished() ) {
            if ( c.finishTime().date() > begin && c.finishTime().date() <= m_now ) {
                m_finished.append( t );
            }
        } else if ( c.isStarted() ) {
            m_running.append( t );
        } else if ( t->startTime( m_id ).date() < m_now ) {
            // should have been started
            m_notstarted.append( t );
        } else if ( t->startTime( m_id ).date() <= end ) {
            // start next period
            m_upcomming.append( t );
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
    //kDebug()<<k_funcinfo<<index.internalPointer()<<":"<<index.row()<<","<<index.column();
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

bool TaskStatusItemModel::hasChildren( const QModelIndex &parent ) const
{
    return rowCount( parent ) > 0;
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

QVariant TaskStatusItemModel::name( const Node *node, int role ) const
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

QVariant TaskStatusItemModel::completed( const Task *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                return node->completion().percentFinished();
            }
            return QString();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant TaskStatusItemModel::status( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole: {
            int st = node->state( m_id );
            if ( st & Node::State_Finished ) {
                if ( st & Node::State_FinishedLate ) {
                    return i18n( "Finished late" );
                }
                if ( st & Node::State_FinishedEarly ) {
                    return i18n( "Finished early" );
                }
                return i18n( "Finished" );
            }
            if ( st & Node::State_Running ) {
                return i18n( "Running" );
            }
            if ( st & Node::State_Started ) {
                if ( st & Node::State_StartedLate ) {
                    return i18n( "Started late" );
                }
                if ( st & Node::State_StartedEarly ) {
                    return i18n( "Started early" );
                }
                return i18n( "Started" );
            }
            return i18n( "Not started" );
            break;
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant TaskStatusItemModel::startTime( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatDate( node->startTime( m_id ).date() );
        case Qt::EditRole:
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant TaskStatusItemModel::endTime( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatDate( node->endTime( m_id ).date() );
        case Qt::EditRole:
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant TaskStatusItemModel::plannedEffortTo( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatNumber( node->plannedEffortTo( m_now, m_id ).toDouble( Duration::Unit_h ), 1 );
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant TaskStatusItemModel::actualEffortTo( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatNumber( node->actualEffortTo( m_now, m_id ).toDouble( Duration::Unit_h ), 1 );
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant TaskStatusItemModel::plannedCostTo( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatMoney( node->plannedCostTo( m_now ) );
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant TaskStatusItemModel::actualCostTo( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return KGlobal::locale()->formatMoney( node->actualCostTo( m_now ) );
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant TaskStatusItemModel::note( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            if ( node->type() == Node::Type_Task ) {
                Node *n = const_cast<Node*>( node );
                return static_cast<Task*>( n )->completion().note();
            }
            break;
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
    if ( n->type() == Node::Type_Task ) {
        Task *t = static_cast<Task*>( n );
        switch ( index.column() ) {
            case 0: result = name( t, role ); break;
            case 1: result = status( t, role ); break;
            case 2: result = completed( t, role ); break;
            case 3: result = plannedEffortTo( t, role ); break;
            case 4: result = actualEffortTo( t, role ); break;
            case 5: result = plannedCostTo( t, role ); break;
            case 6: result = actualCostTo( t, role ); break;
            case 7: result = startTime( t, role ); break;
            case 8: result = endTime( t, role ); break;
            case 9: result = note( t, role ); break;
            default:
                kDebug()<<k_funcinfo<<"data: invalid display value column"<<index.column();;
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

bool TaskStatusItemModel::setData( const QModelIndex &, const QVariant &, int )
{
    return false;
}

QVariant TaskStatusItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            switch ( section ) {
                case 0: return i18n( "Name" );
                case 1: return i18n( "Status" );
                case 2: return i18n( "% Completed" );
                case 3: return i18n( "Planned Effort" );
                case 4: return i18n( "Actual Effort" );
                case 5: return i18n( "Planned Cost" );
                case 6: return i18n( "Actual Cost" );
                case 7: return i18n( "Start" );
                case 8: return i18n( "End" );
                case 9: return i18n( "Status Note" );
                default: return QVariant();
            }
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
    return 10;
}

int TaskStatusItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( ! parent.isValid() ) {
        //kDebug()<<k_funcinfo<<"top="<<m_top.count();
        return m_top.count();
    }
    NodeList *l = list( parent );
    if ( l ) {
        //kDebug()<<k_funcinfo<<"list"<<parent.row()<<":"<<l->count();
        return l->count();
    }
    //kDebug()<<k_funcinfo<<"node"<<parent.row();
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
            kDebug()<<k_funcinfo<<index.row();
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
    kDebug()<<k_funcinfo;
    refresh();
/*    if ( node == 0 || node->type() == Node::Type_Project ) {
        return;
    }
    int row = node->getParent()->findChildNode( node );
    emit dataChanged( createIndex( row, 0, node ), createIndex( row, columnCount(), node ) );*/
}

//--------------------
TaskStatusTreeView::TaskStatusTreeView( Part *part, QWidget *parent )
    : DoubleTreeViewBase( parent )
{
    setContextMenuPolicy( Qt::CustomContextMenu );
    setModel( new TaskStatusItemModel( part ) );
    //setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    
    connect( this, SIGNAL( activated ( const QModelIndex ) ), this, SLOT( slotActivated( const QModelIndex ) ) );

    QList<int> lst1; lst1 << 1 << -1;
    QList<int> lst2; lst2 << 0;
    hideColumns( lst1, lst2 );

}

void TaskStatusTreeView::slotActivated( const QModelIndex index )
{
    kDebug()<<k_funcinfo<<index.column();
}

void TaskStatusTreeView::dragMoveEvent(QDragMoveEvent *event)
{
/*    if (dragDropMode() == InternalMove
        && (event->source() != this || !(event->possibleActions() & Qt::MoveAction)))
        return;

    TreeViewBase::dragMoveEvent( event );
    if ( ! event->isAccepted() ) {
        return;
    }
    //QTreeView thinks it's ok to drop
    event->ignore();
    QModelIndex index = indexAt( event->pos() );
    if ( ! index.isValid() ) {
        event->accept();
        return; // always ok to drop on main project
    }
    Node *dn = itemModel()->node( index );
    if ( dn == 0 ) {
        kError()<<k_funcinfo<<"no node to drop on!"<<endl;
        return; // hmmm
    }
    switch ( dropIndicatorPosition() ) {
        case AboveItem:
        case BelowItem:
            //dn == sibling
            if ( itemModel()->dropAllowed( dn->parentNode(), event->mimeData() ) ) {
                event->accept();
            }
            break;
        case OnItem:
            //dn == new parent
            if ( itemModel()->dropAllowed( dn, event->mimeData() ) ) {
                event->accept();
            }
            break;
        default:
            break;
    }*/
}


//-----------------------------------
TaskStatusView::TaskStatusView( Part *part, QWidget *parent )
    : ViewBase( part, parent ),
    m_id( -1 )
{
    kDebug()<<"-------------------- creating TaskStatusView -------------------"<<endl;
    setupGui();

    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new TaskStatusTreeView( part, this );
    l->addWidget( m_view );

    m_view->itemModel()->setProject( &(part->getProject()) );
    
    connect( m_view, SIGNAL( contextMenuRequested( const QModelIndex&, const QPoint& ) ), SLOT( slotContextMenuRequested( const QModelIndex&, const QPoint& ) ) );
    
    connect( m_view, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
}

void TaskStatusView::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug()<<k_funcinfo;
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos,  lst.first() );
    }
}


Node *TaskStatusView::currentNode() const 
{
    Node * n = m_view->itemModel()->node( m_view->selectionModel()->currentIndex() );
    if ( n && n->type() != Node::Type_Task ) {
        return 0;
    }
    return n;
}

void TaskStatusView::draw( Project &project )
{
    m_project = &project;
    draw();
}

void TaskStatusView::draw()
{
    if ( m_project == 0 ) {
        return;
    }
}

void TaskStatusView::setGuiActive( bool activate )
{
    kDebug()<<k_funcinfo<<activate;
//    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
}

void TaskStatusView::slotContextMenuRequested( const QModelIndex &index, const QPoint& pos )
{
    kDebug()<<k_funcinfo<<index<<pos<<endl;
    if ( ! index.isValid() ) {
        return;
    }
    Node *node = m_view->itemModel()->node( index );
    if ( node == 0 ) {
        return;
    }
    slotContextMenuRequested( node, pos );
}

void TaskStatusView::slotContextMenuRequested( Node *node, const QPoint& pos )
{
    kDebug()<<k_funcinfo<<node->name()<<" :"<<pos;
    QString name;
    switch ( node->type() ) {
        case Node::Type_Task:
            name = "taskstatus_popup";
            break;
        case Node::Type_Milestone:
            break;
        case Node::Type_Summarytask:
            break;
        default:
            break;
    }
    kDebug()<<k_funcinfo<<name;
    if ( name.isEmpty() ) {
        return;
    }
    emit requestPopupMenu( name, pos );
}

void TaskStatusView::setupGui()
{
    // Add the context menu actions for the view options
    actionOptions = new KAction(KIcon("options"), i18n("Options"), this);
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );
}

void TaskStatusView::slotOptions()
{
    kDebug()<<k_funcinfo;
    ItemViewSettupDialog dlg( m_view->slaveView() );
    dlg.exec();
}


} // namespace KPlato

#include "kpttaskstatusview.moc"
