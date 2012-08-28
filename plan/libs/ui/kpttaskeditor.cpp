/* This file is part of the KDE project
  Copyright (C) 2006 - 2010, 2012 Dag Andersen <danders@get2net.dk>

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

#include "kpttaskeditor.h"

#include "kptglobal.h"
#include "kptcommonstrings.h"
#include "kptnodeitemmodel.h"
#include "kptcommand.h"
#include "kptproject.h"
#include "kptitemviewsettup.h"
#include "kptworkpackagesenddialog.h"
#include "kptworkpackagesendpanel.h"
#include "kptdatetime.h"
#include "kptdebug.h"
#include "kptresourcemodel.h"
#include "kptresourceallocationmodel.h"

#include <KoDocument.h>
#include <KoIcon.h>

#include <QItemSelectionModel>
#include <QModelIndex>
#include <QVBoxLayout>
#include <QWidget>
#include <QMenu>
#include <QDragMoveEvent>
#include <QDockWidget>

#include <kaction.h>
#include <kactionmenu.h>
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

//--------------------
TaskEditorItemModel::TaskEditorItemModel( QObject *parent )
: NodeItemModel( parent )
{
}

Qt::ItemFlags TaskEditorItemModel::flags( const QModelIndex &index ) const
{
    if ( index.column() == NodeModel::NodeType ) {
        if ( ! m_readWrite || isColumnReadOnly( index.column() ) ) {
            return QAbstractItemModel::flags( index );
        }
        Node *n = node( index );
        bool baselined = n ? n->isBaselined() : false;
        if ( n && ! baselined && ( n->type() == Node::Type_Task || n->type() == Node::Type_Milestone ) ) {
            return QAbstractItemModel::flags( index ) | Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
        }
        return QAbstractItemModel::flags( index ) | Qt::ItemIsDropEnabled;
    }
    return NodeItemModel::flags( index );
}

QVariant TaskEditorItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal && section == NodeModel::NodeType ) {
        if ( role == Qt::ToolTipRole ) {
            return i18nc( "@info:tooltip", "The type of task or the estimate type of the task" );
        } else if ( role == Qt::WhatsThisRole ) {
            return i18nc( "@info:whatsthis",
                          "<p>Indicates the type of task or the estimate type of the task.</p>"
                          "The type can be set to <emphasis>Milestone</emphasis>, <emphasis>Effort</emphasis> or <emphasis>Duration</emphasis>.<nl/>"
                          "<note>If the type is <emphasis>Summary</emphasis> or <emphasis>Project</emphasis> the type is not editable.</note>");
        }
    }
    return NodeItemModel::headerData(section, orientation, role);
}

QVariant TaskEditorItemModel::data( const QModelIndex &index, int role ) const
{
    if ( role == Qt::TextAlignmentRole ) {
        return NodeItemModel::data( index, role );
    }
    Node *n = node( index );
    if ( n != 0 && index.column() == NodeModel::NodeType ) {
        return type( n, role );
    }
    return NodeItemModel::data( index, role );
}

bool TaskEditorItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    Node *n = node( index );
    if ( n != 0 && role == Qt::EditRole && index.column() == NodeModel::NodeType ) {
        return setType( n, value, role );
    }
    return NodeItemModel::setData( index, value, role );
}

QVariant TaskEditorItemModel::type( const Node *node, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole: {
            if ( node->type() == Node::Type_Task ) {
                return node->estimate()->typeToString( true );
            }
            return node->typeToString( true );
        }
        case Qt::EditRole:
            return node->type();
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::ToolTipRole: {
            if ( node->type() == Node::Type_Task ) {
                return i18nc( "@info:tooltip", "Task with estimate type: %1", node->estimate()->typeToString( true ) );
            }
            return i18nc( "@info:tooltip", "Task type: %1", node->typeToString( true ) );
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
        case Role::EnumListValue: {
            if ( node->type() == Node::Type_Milestone ) {
                return 0;
            }
            if ( node->type() == Node::Type_Task ) {
                return node->estimate()->type() + 1;
            }
            return -1;
        }
        case Role::EnumList: {
            QStringList lst;
            lst << Node::typeToString( Node::Type_Milestone, true );
            lst += Estimate::typeToStringList( true );
            return lst;
        }
    }
    return QVariant();
}

bool TaskEditorItemModel::setType( Node *node, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole: {
            if ( node->type() == Node::Type_Summarytask ) {
                return false;
            }
            int v = value.toInt();
            switch ( v ) {
                case 0: { // Milestone
                    NamedCommand *cmd = 0;
                    if ( node->constraint() == Node::FixedInterval ) {
                        cmd = new NodeModifyConstraintEndTimeCmd( *node, node->constraintStartTime(), i18nc( "(qtundo-format)", "Set type to Milestone" ) );
                    } else {
                        cmd =  new ModifyEstimateCmd( *node, node->estimate()->expectedEstimate(), 0.0, i18nc( "(qtundo-format)", "Set type to Milestone" ) );
                    }
                    emit executeCommand( cmd );
                    return true;
                }
                default: { // Estimate
                    --v;
                    MacroCommand *m = new MacroCommand( i18nc( "(qtundo-format)", "Set type to %1", Estimate::typeToString( (Estimate::Type)v, true ) ) );
                    m->addCommand( new ModifyEstimateTypeCmd( *node, node->estimate()->type(), v ) );
                    if ( node->type() == Node::Type_Milestone ) {
                        if ( node->constraint() == Node::FixedInterval ) {
                            m->addCommand( new NodeModifyConstraintEndTimeCmd( *node, node->constraintStartTime().addDays( 1 ) ) );
                        } else {
                            m->addCommand( new ModifyEstimateUnitCmd( *node, node->estimate()->unit(), Duration::Unit_d ) );
                            m->addCommand( new ModifyEstimateCmd( *node, node->estimate()->expectedEstimate(), 1.0 ) );
                        }
                    }
                    emit executeCommand( m );
                    return true;
                }
            }
            break;
        }
        default: break;
    }
    return false;
}

//--------------------
TaskEditorTreeView::TaskEditorTreeView( QWidget *parent )
    : DoubleTreeViewBase( parent )
{
    TaskEditorItemModel *m = new TaskEditorItemModel( this );
    setModel( m );
    //setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    setSelectionBehavior( QAbstractItemView::SelectRows );

    createItemDelegates( m );
    setItemDelegateForColumn( NodeModel::NodeType, new EnumDelegate( this ) );

    connect( this, SIGNAL( dropAllowed( const QModelIndex&, int, QDragMoveEvent* ) ), SLOT(slotDropAllowed( const QModelIndex&, int, QDragMoveEvent* ) ) );
}

NodeItemModel *TaskEditorTreeView::baseModel() const
{
    NodeSortFilterProxyModel *pr = proxyModel();
    if ( pr ) {
        return static_cast<NodeItemModel*>( pr->sourceModel() );
    }
    return static_cast<NodeItemModel*>( model() );
}

void TaskEditorTreeView::slotDropAllowed( const QModelIndex &index, int dropIndicatorPosition, QDragMoveEvent *event )
{
    QModelIndex idx = index;
    NodeSortFilterProxyModel *pr = proxyModel();
    if ( pr ) {
        idx = pr->mapToSource( index );
    }
    event->ignore();
    if ( baseModel()->dropAllowed( idx, dropIndicatorPosition, event->mimeData() ) ) {
        event->accept();
    }
}

//--------------------
NodeTreeView::NodeTreeView( QWidget *parent )
    : DoubleTreeViewBase( parent )
{
    NodeItemModel *m = new NodeItemModel( this );
    setModel( m );
    //setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    setSelectionBehavior( QAbstractItemView::SelectRows );

    createItemDelegates( m );

    connect( this, SIGNAL( dropAllowed( const QModelIndex&, int, QDragMoveEvent* ) ), SLOT(slotDropAllowed( const QModelIndex&, int, QDragMoveEvent* ) ) );
}

NodeItemModel *NodeTreeView::baseModel() const
{
    NodeSortFilterProxyModel *pr = proxyModel();
    if ( pr ) {
        return static_cast<NodeItemModel*>( pr->sourceModel() );
    }
    return static_cast<NodeItemModel*>( model() );
}

void NodeTreeView::slotDropAllowed( const QModelIndex &index, int dropIndicatorPosition, QDragMoveEvent *event )
{
    QModelIndex idx = index;
    NodeSortFilterProxyModel *pr = proxyModel();
    if ( pr ) {
        idx = pr->mapToSource( index );
    }
    event->ignore();
    if ( baseModel()->dropAllowed( idx, dropIndicatorPosition, event->mimeData() ) ) {
        event->accept();
    }
}


//-----------------------------------
TaskEditor::TaskEditor(KoPart *part, KoDocument *doc, QWidget *parent)
    : ViewBase(part, doc, parent )
{
    kDebug(planDbg())<<"----------------- Create TaskEditor ----------------------";
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new TaskEditorTreeView( this );
    l->addWidget( m_view );
    kDebug(planDbg())<<m_view->actionSplitView();
    setupGui();

    m_view->setEditTriggers( m_view->editTriggers() | QAbstractItemView::EditKeyPressed );

    m_view->setDragDropMode( QAbstractItemView::DragDrop );
    m_view->setDropIndicatorShown( true );
    m_view->setDragEnabled ( true );
    m_view->setAcceptDrops( true );
    m_view->setAcceptDropsOnView( true );

    QList<int> lst1; lst1 << 1 << -1; // only display column 0 (NodeName) in left view
    QList<int> show;
    show << NodeModel::NodeResponsible
            << NodeModel::NodeAllocation
            << NodeModel::NodeType
            << NodeModel::NodeEstimateCalendar
            << NodeModel::NodeEstimate
            << NodeModel::NodeOptimisticRatio
            << NodeModel::NodePessimisticRatio
            << NodeModel::NodeRisk
            << NodeModel::NodeConstraint
            << NodeModel::NodeConstraintStart
            << NodeModel::NodeConstraintEnd
            << NodeModel::NodeRunningAccount
            << NodeModel::NodeStartupAccount
            << NodeModel::NodeStartupCost
            << NodeModel::NodeShutdownAccount
            << NodeModel::NodeShutdownCost
            << NodeModel::NodeDescription;

    QList<int> lst2;
    for ( int i = 0; i < model()->columnCount(); ++i ) {
        if ( ! show.contains( i ) ) {
            lst2 << i;
        }
    }
    for ( int i = 0; i < show.count(); ++i ) {
        int sec = m_view->slaveView()->header()->visualIndex( show[ i ] );
        //kDebug(planDbg())<<"move section:"<<i<<show[i]<<sec;
        if ( i != sec ) {
            m_view->slaveView()->header()->moveSection( sec, i );
        }
    }
    m_view->hideColumns( lst1, lst2 );
    m_view->masterView()->setDefaultColumns( QList<int>() << NodeModel::NodeName );
    m_view->slaveView()->setDefaultColumns( show );

    connect( model(), SIGNAL( executeCommand( KUndo2Command* ) ), doc, SLOT( addCommand( KUndo2Command* ) ) );

    connect( m_view, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ), this, SLOT ( slotCurrentChanged( const QModelIndex &, const QModelIndex & ) ) );

    connect( m_view, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT ( slotSelectionChanged( const QModelIndexList ) ) );

    connect( m_view, SIGNAL( contextMenuRequested( const QModelIndex&, const QPoint& ) ), SLOT( slotContextMenuRequested( const QModelIndex&, const QPoint& ) ) );

    connect( m_view, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );

    connect(baseModel(), SIGNAL(projectShownChanged(bool)), SLOT(slotProjectShown(bool)));
}

void TaskEditor::slotProjectShown( bool on )
{
    kDebug(planDbg())<<proxyModel();
    QModelIndex idx;
    if ( proxyModel() ) {
        if ( proxyModel()->rowCount() > 0 ) {
            idx = proxyModel()->index( 0, 0 );
            m_view->selectionModel()->setCurrentIndex( idx, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows );
        }
    } else if ( baseModel() && baseModel()->rowCount() > 0 ) {
        idx = baseModel()->index( 0, 0 );
        m_view->selectionModel()->setCurrentIndex( idx, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows );
    }
    if ( on && idx.isValid() ) {
        m_view->masterView()->expand( idx );
    }
    slotEnableActions();
}

void TaskEditor::updateReadWrite( bool rw )
{
    m_view->setReadWrite( rw );
    ViewBase::updateReadWrite( rw );
}

void TaskEditor::setProject( Project *project )
{
    kDebug(planDbg())<<project;
    m_view->setProject( project );
    ViewBase::setProject( project );
}

void TaskEditor::createDockers()
{
    // Add dockers
    DockWidget *ds = new DockWidget( this, "Allocations", i18nc( "@title resource allocations", "Allocations" ) );
    QTreeView *x = new QTreeView( ds );
    AllocatedResourceItemModel *m1 = new AllocatedResourceItemModel( x );
    x->setModel( m1 );
    m1->setProject( project() );
//     x->setHeaderHidden( true );
    x->setSelectionBehavior( QAbstractItemView::SelectRows );
    x->setSelectionMode( QAbstractItemView::ExtendedSelection );
    x->expandAll();
    x->resizeColumnToContents( 0 );
    x->setDragDropMode( QAbstractItemView::DragOnly );
    x->setDragEnabled ( true );
    ds->setWidget( x );
    connect(this, SIGNAL(projectChanged(Project*)), m1, SLOT(setProject(Project*)));
    connect(this, SIGNAL(taskSelected(Task*)), m1, SLOT(setTask(Task*)));
    connect(m1, SIGNAL(expandAll()), x, SLOT(expandAll()));
    connect(m1, SIGNAL(resizeColumnToContents(int)), x, SLOT(resizeColumnToContents(int)));
    addDocker( ds );

    ds = new DockWidget( this, "Resources", i18nc( "@title", "Resources" ) );
    ds->setToolTip( i18nc( "@info:tooltip",
                          "Drag resources into the Task Editor"
                          " and drop into the allocations- or responsible column" ) );
    QTreeView *e = new QTreeView( ds );
    ResourceItemModel *m = new ResourceItemModel( e );
    e->setModel( m );
    m->setProject( project() );
    m->setReadWrite( isReadWrite() );
    QList<int> show; show << ResourceModel::ResourceName;
    for ( int i = m->columnCount() - 1; i >= 0; --i ) {
        e->setColumnHidden( i, ! show.contains( i ) );
    }
    e->setHeaderHidden( true );
    e->setSelectionBehavior( QAbstractItemView::SelectRows );
    e->setSelectionMode( QAbstractItemView::ExtendedSelection );
    e->expandAll();
    e->resizeColumnToContents( ResourceModel::ResourceName );
    e->setDragDropMode( QAbstractItemView::DragOnly );
    e->setDragEnabled ( true );
    ds->setWidget( e );
    connect(this, SIGNAL(projectChanged(Project*)), m, SLOT(setProject(Project*)));
    connect(this, SIGNAL(readWriteChanged(bool)), m, SLOT(setReadWrite(bool)));
    connect(m, SIGNAL(executeCommand(KUndo2Command*)), part(), SLOT(addCommand(KUndo2Command*)));
    addDocker( ds );

    {
        ds = new DockWidget( this, "Taskmodules", i18nc( "@title", "Task Modules" ) );
        ds->setToolTip( i18nc( "@info:tooltip", "Drag a task module into the <emphasis>Task Editor</emphasis> to add it to the project" ) );
        ds->setLocation( Qt::LeftDockWidgetArea );
        e = new QTreeView( ds );
        TaskModuleModel *m = new TaskModuleModel( e );
        e->setModel( m );
        e->setHeaderHidden( true );
        e->setRootIsDecorated( false );
        e->setSelectionBehavior( QAbstractItemView::SelectRows );
        e->setSelectionMode( QAbstractItemView::SingleSelection );
//         e->resizeColumnToContents( 0 );
        e->setDragDropMode( QAbstractItemView::DragDrop );
        e->setAcceptDrops( true );
        e->setDragEnabled ( true );
        ds->setWidget( e );
        connect(this, SIGNAL(loadTaskModules(const QStringList&)), m, SLOT(loadTaskModules(const QStringList&)));
        connect(m, SIGNAL(saveTaskModule(const KUrl&, Project*)), this, SIGNAL(saveTaskModule(const KUrl&, Project*)));
        connect(m, SIGNAL(removeTaskModule(const KUrl&)), this, SIGNAL(removeTaskModule(const KUrl&)));
        addDocker( ds );
    }
}

void TaskEditor::setTaskModules(const QStringList& files)
{
    emit loadTaskModules( files );
}

void TaskEditor::setGuiActive( bool activate )
{
    kDebug(planDbg())<<activate;
    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
    if ( activate && !m_view->selectionModel()->currentIndex().isValid() && m_view->model()->rowCount() > 0 ) {
        m_view->selectionModel()->setCurrentIndex(m_view->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
    }
}

void TaskEditor::slotCurrentChanged( const QModelIndex &curr, const QModelIndex & )
{
    kDebug(planDbg())<<curr.row()<<","<<curr.column();
    slotEnableActions();
}

void TaskEditor::slotSelectionChanged( const QModelIndexList list)
{
    kDebug(planDbg())<<list.count();
    slotEnableActions();
    emit taskSelected( dynamic_cast<Task*>( selectedNode() ) );
}

QModelIndexList TaskEditor::selectedRows() const
{
#if 0
// Qt bug?
    return m_view->selectionModel()->selectedRows();
#else
    QModelIndexList lst;
    foreach ( QModelIndex i, m_view->selectionModel()->selectedIndexes() ) {
        if ( i.column() == 0 ) {
            lst << i;
        }
    }
    return lst;
#endif
}

int TaskEditor::selectedRowCount() const
{
    return selectedRows().count();
}

QList<Node*> TaskEditor::selectedNodes() const {
    QList<Node*> lst;
    foreach ( const QModelIndex &i, selectedRows() ) {
        Node * n = m_view->baseModel()->node( i );
        if ( n != 0 && n->type() != Node::Type_Project ) {
            lst.append( n );
        }
    }
    return lst;
}

Node *TaskEditor::selectedNode() const
{
    QList<Node*> lst = selectedNodes();
    if ( lst.count() != 1 ) {
        return 0;
    }
    return lst.first();
}

Node *TaskEditor::currentNode() const {
    Node * n = m_view->baseModel()->node( m_view->selectionModel()->currentIndex() );
    if ( n == 0 || n->type() == Node::Type_Project ) {
        return 0;
    }
    return n;
}

void TaskEditor::slotContextMenuRequested( const QModelIndex& index, const QPoint& pos )
{
    Node *node = m_view->baseModel()->node( index );
    if ( node == 0 ) {
        return;
    }
    kDebug(planDbg())<<node->name()<<" :"<<pos;
    QString name;
    switch ( node->type() ) {
        case Node::Type_Project:
            name = "task_edit_popup";
            break;
        case Node::Type_Task:
            name = node->isScheduled( baseModel()->id() ) ? "task_popup" : "task_edit_popup";
            break;
        case Node::Type_Milestone:
            name = node->isScheduled( baseModel()->id() ) ? "taskeditor_milestone_popup" : "task_edit_popup";
            break;
        case Node::Type_Summarytask:
            name = "summarytask_popup";
            break;
        default:
            name = "node_popup";
            break;
    }
    if ( name.isEmpty() ) {
        slotHeaderContextMenuRequested( pos );
        return;
    }
    kDebug(planDbg())<<name;
    emit requestPopupMenu( name, pos );
}

void TaskEditor::setScheduleManager( ScheduleManager *sm )
{
    //kDebug(planDbg())<<endl;
    m_view->baseModel()->setScheduleManager( sm );
}

void TaskEditor::slotEnableActions()
{
    updateActionsEnabled( isReadWrite() );
}

void TaskEditor::updateActionsEnabled( bool on )
{
//     kDebug(planDbg())<<selectedRowCount()<<selectedNode()<<currentNode();
    if ( ! on ) {
        menuAddTask->setEnabled( false );
        actionAddTask->setEnabled( false );
        actionAddMilestone->setEnabled( false );
        menuAddSubTask->setEnabled( false );
        actionAddSubtask->setEnabled( false );
        actionAddSubMilestone->setEnabled( false );
        actionDeleteTask->setEnabled( false );
        actionMoveTaskUp->setEnabled( false );
        actionMoveTaskDown->setEnabled( false );
        actionIndentTask->setEnabled( false );
        actionUnindentTask->setEnabled( false );
        return;
    }
        
    int selCount = selectedRowCount();
    if ( selCount == 0 ) {
        if ( currentNode() ) {
            // there are tasks but none is selected
            menuAddTask->setEnabled( false );
            actionAddTask->setEnabled( false );
            actionAddMilestone->setEnabled( false );
            menuAddSubTask->setEnabled( false );
            actionAddSubtask->setEnabled( false );
            actionAddSubMilestone->setEnabled( false );
            actionDeleteTask->setEnabled( false );
            actionMoveTaskUp->setEnabled( false );
            actionMoveTaskDown->setEnabled( false );
            actionIndentTask->setEnabled( false );
            actionUnindentTask->setEnabled( false );
        } else {
            // we need to be able to add the first task
            menuAddTask->setEnabled( true );
            actionAddTask->setEnabled( true );
            actionAddMilestone->setEnabled( true );
            menuAddSubTask->setEnabled( false );
            actionAddSubtask->setEnabled( false );
            actionAddSubMilestone->setEnabled( false );
            actionDeleteTask->setEnabled( false );
            actionMoveTaskUp->setEnabled( false );
            actionMoveTaskDown->setEnabled( false );
            actionIndentTask->setEnabled( false );
            actionUnindentTask->setEnabled( false );
        }
        return;
    }
    Node *n = selectedNode(); // 0 if not a single task, summarytask or milestone
    if ( selCount == 1 && n == 0 ) {
        // only project selected
        menuAddTask->setEnabled( true );
        actionAddTask->setEnabled( true );
        actionAddMilestone->setEnabled( true );
        menuAddSubTask->setEnabled( true );
        actionAddSubtask->setEnabled( true );
        actionAddSubMilestone->setEnabled( true );
        actionDeleteTask->setEnabled( false );
        actionMoveTaskUp->setEnabled( false );
        actionMoveTaskDown->setEnabled( false );
        actionIndentTask->setEnabled( false );
        actionUnindentTask->setEnabled( false );
        return;
    }
    if ( selCount == 1 && n != currentNode() ) {
        // multi selection in progress
        menuAddTask->setEnabled( false );
        actionAddTask->setEnabled( false );
        actionAddMilestone->setEnabled( false );
        menuAddSubTask->setEnabled( false );
        actionAddSubtask->setEnabled( false );
        actionAddSubMilestone->setEnabled( false );
        actionDeleteTask->setEnabled( false );
        actionMoveTaskUp->setEnabled( false );
        actionMoveTaskDown->setEnabled( false );
        actionIndentTask->setEnabled( false );
        actionUnindentTask->setEnabled( false );
        return;
    }

    bool baselined = false;
    Project *p = m_view->project();
    if ( p && p->isBaselined() ) {
        foreach ( Node *n, selectedNodes() ) {
            if ( n->isBaselined() ) {
                baselined = true;
                break;
            }
        }
    }
    if ( selCount == 1 ) {
        menuAddTask->setEnabled( true );
        actionAddTask->setEnabled( true );
        actionAddMilestone->setEnabled( true );
        menuAddSubTask->setEnabled( ! baselined || n->type() == Node::Type_Summarytask );
        actionAddSubtask->setEnabled( ! baselined || n->type() == Node::Type_Summarytask );
        actionAddSubMilestone->setEnabled( ! baselined || n->type() == Node::Type_Summarytask );
        actionDeleteTask->setEnabled( ! baselined );
        Node *s = n->siblingBefore();
        actionMoveTaskUp->setEnabled( s );
        actionMoveTaskDown->setEnabled( n->siblingAfter() );
        s = n->siblingBefore();
        actionIndentTask->setEnabled( ! baselined && s && ! s->isBaselined() );
        actionUnindentTask->setEnabled( ! baselined && n->level() > 1 );
        return;
    }
    // selCount > 1
    menuAddTask->setEnabled( false );
    actionAddTask->setEnabled( false );
    actionAddMilestone->setEnabled( false );
    menuAddSubTask->setEnabled( false );
    actionAddSubtask->setEnabled( false );
    actionAddSubMilestone->setEnabled( false );
    actionDeleteTask->setEnabled( ! baselined );
    actionMoveTaskUp->setEnabled( false );
    actionMoveTaskDown->setEnabled( false );
    actionIndentTask->setEnabled( false );
    actionUnindentTask->setEnabled( false );
}

void TaskEditor::setupGui()
{
    QString name = "taskeditor_add_list";

    menuAddTask = new KActionMenu(koIcon("view-task-add"), i18n("Add Task"), this);
    actionCollection()->addAction("add_task", menuAddTask );
    connect( menuAddTask, SIGNAL( triggered( bool ) ), SLOT( slotAddTask() ) );
    addAction( name, menuAddTask );

    actionAddTask  = new KAction( i18n( "Add Task" ), this);
    actionAddTask->setShortcut( KShortcut( Qt::CTRL + Qt::Key_I ) );
    connect( actionAddTask, SIGNAL( triggered( bool ) ), SLOT( slotAddTask() ) );
    menuAddTask->addAction( actionAddTask );

    actionAddMilestone  = new KAction( i18n( "Add Milestone" ), this );
    actionAddMilestone->setShortcut( KShortcut( Qt::CTRL + Qt::ALT + Qt::Key_I ) );
    connect( actionAddMilestone, SIGNAL( triggered( bool ) ), SLOT( slotAddMilestone() ) );
    menuAddTask->addAction( actionAddMilestone );


    menuAddSubTask = new KActionMenu(koIcon("view-task-child-add"), i18n("Add Sub-Task"), this);
    actionCollection()->addAction("add_subtask", menuAddTask );
    connect( menuAddSubTask, SIGNAL( triggered( bool ) ), SLOT( slotAddSubtask() ) );
    addAction( name, menuAddSubTask );

    actionAddSubtask  = new KAction( i18n( "Add Sub-Task" ), this );
    actionAddSubtask->setShortcut( KShortcut( Qt::CTRL + Qt::SHIFT + Qt::Key_I ) );
    connect( actionAddSubtask, SIGNAL( triggered( bool ) ), SLOT( slotAddSubtask() ) );
    menuAddSubTask->addAction( actionAddSubtask );

    actionAddSubMilestone = new KAction( i18n( "Add Sub-Milestone" ), this );
    actionAddSubMilestone->setShortcut( KShortcut( Qt::CTRL + Qt::SHIFT + Qt::ALT + Qt::Key_I ) );
    connect( actionAddSubMilestone, SIGNAL( triggered( bool ) ), SLOT( slotAddSubMilestone() ) );
    menuAddSubTask->addAction( actionAddSubMilestone );

    actionDeleteTask  = new KAction(koIcon("edit-delete"), i18nc("@action", "Delete"), this);
    actionDeleteTask->setShortcut( KShortcut( Qt::Key_Delete ) );
    actionCollection()->addAction("delete_task", actionDeleteTask );
    connect( actionDeleteTask, SIGNAL( triggered( bool ) ), SLOT( slotDeleteTask() ) );
    addAction( name, actionDeleteTask );


    name = "taskeditor_move_list";
    actionIndentTask  = new KAction(koIcon("format-indent-more"), i18n("Indent Task"), this);
    actionCollection()->addAction("indent_task", actionIndentTask );
    connect(actionIndentTask, SIGNAL(triggered(bool) ), SLOT(slotIndentTask()));
    addAction( name, actionIndentTask );

    actionUnindentTask  = new KAction(koIcon("format-indent-less"), i18n("Unindent Task"), this);
    actionCollection()->addAction("unindent_task", actionUnindentTask );
    connect(actionUnindentTask, SIGNAL(triggered(bool) ), SLOT(slotUnindentTask()));
    addAction( name, actionUnindentTask );

    actionMoveTaskUp  = new KAction(koIcon("arrow-up"), i18n("Move Up"), this);
    actionCollection()->addAction("move_task_up", actionMoveTaskUp );
    connect(actionMoveTaskUp, SIGNAL(triggered(bool) ), SLOT(slotMoveTaskUp()));
    addAction( name, actionMoveTaskUp );

    actionMoveTaskDown  = new KAction(koIcon("arrow-down"), i18n("Move Down"), this);
    actionCollection()->addAction("move_task_down", actionMoveTaskDown );
    connect(actionMoveTaskDown, SIGNAL(triggered(bool) ), SLOT(slotMoveTaskDown()));
    addAction( name, actionMoveTaskDown );

    // Add the context menu actions for the view options
    actionShowProject = new KToggleAction( i18n( "Show Project" ), this );
    connect(actionShowProject, SIGNAL(triggered(bool) ), baseModel(), SLOT(setShowProject(bool)));
    addContextAction( actionShowProject );

    connect(m_view->actionSplitView(), SIGNAL(triggered(bool) ), SLOT(slotSplitView()));
    addContextAction( m_view->actionSplitView() );

    createOptionAction();

    createDockers();
}

void TaskEditor::slotSplitView()
{
    kDebug(planDbg());
    m_view->setViewSplitMode( ! m_view->isViewSplit() );
    emit optionsModified();
}


void TaskEditor::slotOptions()
{
    kDebug(planDbg());
    SplitItemViewSettupDialog *dlg = new SplitItemViewSettupDialog( this, m_view, this );
    dlg->addPrintingOptions();
    connect(dlg, SIGNAL(finished(int)), SLOT(slotOptionsFinished(int)));
    dlg->show();
    dlg->raise();
    dlg->activateWindow();
}

void TaskEditor::slotAddTask()
{
    kDebug(planDbg());
    if ( selectedRowCount() == 0 || ( selectedRowCount() == 1 && selectedNode() == 0 ) ) {
        m_view->closePersistentEditor( m_view->selectionModel()->currentIndex() );
        Task *t = m_view->project()->createTask( m_view->project()->taskDefaults() );
        QModelIndex idx = m_view->baseModel()->insertSubtask( t, m_view->project() );
        Q_ASSERT( idx.isValid() );
        edit( idx );
        return;
    }
    Node *sib = selectedNode();
    if ( sib == 0 ) {
        return;
    }
    m_view->closePersistentEditor( m_view->selectionModel()->currentIndex() );
    Task *t = m_view->project()->createTask( m_view->project()->taskDefaults() );
    QModelIndex idx = m_view->baseModel()->insertTask( t, sib );
    Q_ASSERT( idx.isValid() );
    edit( idx );
}

void TaskEditor::slotAddMilestone()
{
    kDebug(planDbg());
    if ( selectedRowCount() == 0  || ( selectedRowCount() == 1 && selectedNode() == 0 ) ) {
        // None selected or only project selected: insert under main project
        m_view->closePersistentEditor( m_view->selectionModel()->currentIndex() );
        Task *t = m_view->project()->createTask();
        t->estimate()->clear();
        QModelIndex idx = m_view->baseModel()->insertSubtask( t, m_view->project() );
        Q_ASSERT( idx.isValid() );
        edit( idx );
        return;
    }
    Node *sib = selectedNode(); // sibling
    if ( sib == 0 ) {
        return;
    }
    m_view->closePersistentEditor( m_view->selectionModel()->currentIndex() );
    Task *t = m_view->project()->createTask();
    t->estimate()->clear();
    QModelIndex idx = m_view->baseModel()->insertTask( t, sib );
    Q_ASSERT( idx.isValid() );
    edit( idx );
}

void TaskEditor::slotAddSubMilestone()
{
    kDebug(planDbg());
    Node *parent = selectedNode();
    if ( parent == 0 && selectedRowCount() == 1 ) {
        // project selected
        parent = m_view->project();
    }
    if ( parent == 0 ) {
        return;
    }
    m_view->closePersistentEditor( m_view->selectionModel()->currentIndex() );
    Task *t = m_view->project()->createTask( m_view->project()->taskDefaults() );
    t->estimate()->clear();
    QModelIndex idx = m_view->baseModel()->insertSubtask( t, parent );
    Q_ASSERT( idx.isValid() );
    edit( idx );
}

void TaskEditor::slotAddSubtask()
{
    kDebug(planDbg());
    Node *parent = selectedNode();
    if ( parent == 0 && selectedRowCount() == 1 ) {
        // project selected
        parent = m_view->project();
    }
    if ( parent == 0 ) {
        return;
    }
    m_view->closePersistentEditor( m_view->selectionModel()->currentIndex() );
    Task *t = m_view->project()->createTask( m_view->project()->taskDefaults() );
    QModelIndex idx = m_view->baseModel()->insertSubtask( t, parent );
    Q_ASSERT( idx.isValid() );
    edit( idx );
}

void TaskEditor::edit( QModelIndex i )
{
    if ( i.isValid() ) {
        m_view->selectionModel()->setCurrentIndex( i, QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect );
        m_view->setParentsExpanded( i, true ); // in case treeview does not have focus
        m_view->edit( i );
    }
}

void TaskEditor::slotDeleteTask()
{
    //kDebug(planDbg());
    QList<Node*> lst = selectedNodes();
    while ( true ) {
        // remove children of selected tasks, as parents delete their children
        Node *ch = 0;
        foreach ( Node *n1, lst ) {
            foreach ( Node *n2, lst ) {
                if ( n2->isChildOf( n1 ) ) {
                    ch = n2;
                    break;
                }
            }
            if ( ch != 0 ) {
                break;
            }
        }
        if ( ch == 0 ) {
            break;
        }
        lst.removeAt( lst.indexOf( ch ) );
    }
    //foreach ( Node* n, lst ) { kDebug(planDbg())<<n->name(); }
    emit deleteTaskList( lst );
    QModelIndex i = m_view->selectionModel()->currentIndex();
    if ( i.isValid() ) {
        m_view->selectionModel()->select( i, QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect );
        m_view->selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
    }
}

void TaskEditor::slotIndentTask()
{
    kDebug(planDbg());
    Node *n = selectedNode();
    if ( n ) {
        emit indentTask();
        QModelIndex i = baseModel()->index( n );
        m_view->selectionModel()->select( i, QItemSelectionModel::Rows | QItemSelectionModel::Current | QItemSelectionModel::ClearAndSelect );
        m_view->selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
        m_view->setParentsExpanded( i, true );
    }
}

void TaskEditor::slotUnindentTask()
{
    kDebug(planDbg());
    Node *n = selectedNode();
    if ( n ) {
        emit unindentTask();
        QModelIndex i = baseModel()->index( n );
        m_view->selectionModel()->select( i, QItemSelectionModel::Rows | QItemSelectionModel::Current | QItemSelectionModel::ClearAndSelect );
        m_view->selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
    }
}

void TaskEditor::slotMoveTaskUp()
{
    kDebug(planDbg());
    Node *n = selectedNode();
    if ( n ) {
        emit moveTaskUp();
        QModelIndex i = baseModel()->index( n );
        m_view->selectionModel()->select( i, QItemSelectionModel::Rows | QItemSelectionModel::Current | QItemSelectionModel::ClearAndSelect );
        m_view->selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
    }
}

void TaskEditor::slotMoveTaskDown()
{
    kDebug(planDbg());
    Node *n = selectedNode();
    if ( n ) {
        emit moveTaskDown();
        QModelIndex i = baseModel()->index( n );
        m_view->selectionModel()->select( i, QItemSelectionModel::Rows | QItemSelectionModel::Current | QItemSelectionModel::ClearAndSelect );
         m_view->selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
   }
}

bool TaskEditor::loadContext( const KoXmlElement &context )
{
    kDebug(planDbg());
    ViewBase::loadContext( context );
    bool show = (bool)(context.attribute( "show-project", "0" ).toInt() );
    actionShowProject->setChecked( show );
    baseModel()->setShowProject( show ); // why is this not called by the action?
    return m_view->loadContext( baseModel()->columnMap(), context );
}

void TaskEditor::saveContext( QDomElement &context ) const
{
    ViewBase::saveContext( context );
    context.setAttribute( "show-project", baseModel()->projectShown() );
    m_view->saveContext( baseModel()->columnMap(), context );
}

KoPrintJob *TaskEditor::createPrintJob()
{
    return m_view->createPrintJob( this );
}


//-----------------------------------
TaskView::TaskView(KoPart *part, KoDocument *doc, QWidget *parent)
    : ViewBase(part, doc, parent)
{
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new NodeTreeView( this );
    NodeSortFilterProxyModel *p = new NodeSortFilterProxyModel( m_view->baseModel(), m_view );
    m_view->setModel( p );
    l->addWidget( m_view );
    setupGui();

    //m_view->setEditTriggers( m_view->editTriggers() | QAbstractItemView::EditKeyPressed );
    m_view->setDragDropMode( QAbstractItemView::InternalMove );
    m_view->setDropIndicatorShown( false );
    m_view->setDragEnabled ( true );
    m_view->setAcceptDrops( false );
    m_view->setAcceptDropsOnView( false );

    QList<int> readonly;
    readonly << NodeModel::NodeName
            << NodeModel::NodeResponsible
            << NodeModel::NodeAllocation
            << NodeModel::NodeEstimateType
            << NodeModel::NodeEstimateCalendar
            << NodeModel::NodeEstimate
            << NodeModel::NodeOptimisticRatio
            << NodeModel::NodePessimisticRatio
            << NodeModel::NodeRisk
            << NodeModel::NodeConstraint
            << NodeModel::NodeConstraintStart
            << NodeModel::NodeConstraintEnd
            << NodeModel::NodeRunningAccount
            << NodeModel::NodeStartupAccount
            << NodeModel::NodeStartupCost
            << NodeModel::NodeShutdownAccount
            << NodeModel::NodeShutdownCost
            << NodeModel::NodeDescription;
    foreach ( int c, readonly ) {
        m_view->baseModel()->setReadOnly( c, true );
    }

    QList<int> lst1; lst1 << 1 << -1;
    QList<int> show;
    show << NodeModel::NodeStatus
            << NodeModel::NodeCompleted
            << NodeModel::NodeResponsible
            << NodeModel::NodeAssignments
            << NodeModel::NodePerformanceIndex
            << NodeModel::NodeBCWS
            << NodeModel::NodeBCWP
            << NodeModel::NodeACWP
            << NodeModel::NodeDescription;

    for ( int s = 0; s < show.count(); ++s ) {
        m_view->slaveView()->mapToSection( show[s], s );
    }
    QList<int> lst2;
    for ( int i = 0; i < m_view->model()->columnCount(); ++i ) {
        if ( ! show.contains( i ) ) {
            lst2 << i;
        }
    }
    m_view->hideColumns( lst1, lst2 );
    m_view->masterView()->setDefaultColumns( QList<int>() << 0 );
    m_view->slaveView()->setDefaultColumns( show );

    connect( m_view->baseModel(), SIGNAL( executeCommand( KUndo2Command* ) ), doc, SLOT( addCommand( KUndo2Command* ) ) );

    connect( m_view, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ), this, SLOT ( slotCurrentChanged( const QModelIndex &, const QModelIndex & ) ) );

    connect( m_view, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT ( slotSelectionChanged( const QModelIndexList ) ) );

    connect( m_view, SIGNAL( contextMenuRequested( const QModelIndex&, const QPoint& ) ), SLOT( slotContextMenuRequested( const QModelIndex&, const QPoint& ) ) );

    connect( m_view, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
}

void TaskView::updateReadWrite( bool rw )
{
    m_view->setReadWrite( rw );
    ViewBase::updateReadWrite( rw );
}

void TaskView::draw( Project &project )
{
    m_view->setProject( &project );
}

void TaskView::draw()
{
}

void TaskView::setGuiActive( bool activate )
{
    kDebug(planDbg())<<activate;
    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
    if ( activate && !m_view->selectionModel()->currentIndex().isValid() && m_view->model()->rowCount() > 0 ) {
        m_view->selectionModel()->setCurrentIndex(m_view->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
    }
}

void TaskView::slotCurrentChanged(  const QModelIndex &curr, const QModelIndex & )
{
    kDebug(planDbg())<<curr.row()<<","<<curr.column();
    slotEnableActions();
}

void TaskView::slotSelectionChanged( const QModelIndexList list)
{
    kDebug(planDbg())<<list.count();
    slotEnableActions();
}

int TaskView::selectedNodeCount() const
{
    QItemSelectionModel* sm = m_view->selectionModel();
    return sm->selectedRows().count();
}

QList<Node*> TaskView::selectedNodes() const {
    QList<Node*> lst;
    QItemSelectionModel* sm = m_view->selectionModel();
    if ( sm == 0 ) {
        return lst;
    }
    foreach ( const QModelIndex &i, sm->selectedRows() ) {
        Node * n = m_view->baseModel()->node( proxyModel()->mapToSource( i ) );
        if ( n != 0 && n->type() != Node::Type_Project ) {
            lst.append( n );
        }
    }
    return lst;
}

Node *TaskView::selectedNode() const
{
    QList<Node*> lst = selectedNodes();
    if ( lst.count() != 1 ) {
        return 0;
    }
    return lst.first();
}

Node *TaskView::currentNode() const {
    Node * n = m_view->baseModel()->node( proxyModel()->mapToSource( m_view->selectionModel()->currentIndex() ) );
    if ( n == 0 || n->type() == Node::Type_Project ) {
        return 0;
    }
    return n;
}

void TaskView::slotContextMenuRequested( const QModelIndex& index, const QPoint& pos )
{
    QString name;
    Node *node = m_view->baseModel()->node( proxyModel()->mapToSource( index ) );
    if ( node ) {
        switch ( node->type() ) {
            case Node::Type_Task:
                name = "taskview_popup";
                break;
            case Node::Type_Milestone:
                name = "taskview_milestone_popup";
                break;
            case Node::Type_Summarytask:
                name = "taskview_summary_popup";
                break;
            default:
                break;
        }
    } else kDebug(planDbg())<<"No node: "<<index;
    if ( name.isEmpty() ) {
        kDebug(planDbg())<<"No menu";
        slotHeaderContextMenuRequested( pos );
        return;
    }
    emit requestPopupMenu( name, pos );
}

void TaskView::setScheduleManager( ScheduleManager *sm )
{
    //kDebug(planDbg())<<endl;
    m_view->baseModel()->setScheduleManager( sm );
}

void TaskView::slotEnableActions()
{
    updateActionsEnabled( true );
}

void TaskView::updateActionsEnabled( bool /*on*/ )
{

}

void TaskView::setupGui()
{
//    KActionCollection *coll = actionCollection();

    // Add the context menu actions for the view options
    actionShowProject = new KToggleAction( i18n( "Show Project" ), this );
    connect(actionShowProject, SIGNAL(triggered(bool) ), baseModel(), SLOT(setShowProject(bool)));
    addContextAction( actionShowProject );

    connect(m_view->actionSplitView(), SIGNAL(triggered(bool) ), SLOT(slotSplitView()));
    addContextAction( m_view->actionSplitView() );

    createOptionAction();
}

void TaskView::slotSplitView()
{
    kDebug(planDbg());
    m_view->setViewSplitMode( ! m_view->isViewSplit() );
    emit optionsModified();
}

void TaskView::slotOptions()
{
    kDebug(planDbg());
    SplitItemViewSettupDialog *dlg = new SplitItemViewSettupDialog( this, m_view, this );
    dlg->addPrintingOptions();
    connect(dlg, SIGNAL(finished(int)), SLOT(slotOptionsFinished(int)));
    dlg->show();
    dlg->raise();
    dlg->activateWindow();
}

bool TaskView::loadContext( const KoXmlElement &context )
{
    ViewBase::loadContext( context );
    bool show = (bool)(context.attribute( "show-project", "0" ).toInt() );
    actionShowProject->setChecked( show );
    baseModel()->setShowProject( show ); // why is this not called by the action?
    return m_view->loadContext( m_view->baseModel()->columnMap(), context );
}

void TaskView::saveContext( QDomElement &context ) const
{
    ViewBase::saveContext( context );
    context.setAttribute( "show-project", baseModel()->projectShown() );
    m_view->saveContext( m_view->baseModel()->columnMap(), context );
}

KoPrintJob *TaskView::createPrintJob()
{
    return m_view->createPrintJob( this );
}

//---------------------------------
GeneralNodeTreeView::GeneralNodeTreeView( QWidget *parent )
    : DoubleTreeViewBase( parent )
{
    GeneralNodeItemModel *m = new GeneralNodeItemModel( this );
    setModel( m );
    //setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    setSelectionBehavior( QAbstractItemView::SelectRows );

    createItemDelegates( m );

    connect( this, SIGNAL( dropAllowed( const QModelIndex&, int, QDragMoveEvent* ) ), SLOT(slotDropAllowed( const QModelIndex&, int, QDragMoveEvent* ) ) );
}

void GeneralNodeTreeView::setModus( int mode )
{
    baseModel()->setModus( mode );
}

GeneralNodeItemModel *GeneralNodeTreeView::baseModel() const
{
    NodeSortFilterProxyModel *pr = proxyModel();
    if ( pr ) {
        return static_cast<GeneralNodeItemModel*>( pr->sourceModel() );
    }
    return static_cast<GeneralNodeItemModel*>( model() );
}

void GeneralNodeTreeView::slotDropAllowed( const QModelIndex &index, int dropIndicatorPosition, QDragMoveEvent *event )
{
    QModelIndex idx = index;
    NodeSortFilterProxyModel *pr = proxyModel();
    if ( pr ) {
        idx = pr->mapToSource( index );
    }
    event->ignore();
    if ( baseModel()->dropAllowed( idx, dropIndicatorPosition, event->mimeData() ) ) {
        event->accept();
    }
}

//--------------------------------
TaskWorkPackageView::TaskWorkPackageView(KoPart *part, KoDocument *doc, QWidget *parent)
    : ViewBase(part, doc, parent ),
    m_cmd( 0 )
{
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new GeneralNodeTreeView( this );
    m_view->setModus( GeneralNodeItemModel::Flat | GeneralNodeItemModel::WorkPackage );
    NodeSortFilterProxyModel *p = new NodeSortFilterProxyModel( m_view->baseModel(), m_view, false );
    m_view->setModel( p );

    m_view->setSortingEnabled( true );
    m_view->sortByColumn( NodeModel::NodeWBSCode, Qt::AscendingOrder );

    // match empty string or Type_Task
    p->setFilterRegExp( QRegExp( QString( "^$|%1").arg( Node::Type_Task ) ) );
    p->setFilterRole( Qt::EditRole );
    p->setFilterKeyColumn( NodeModel::NodeType );

    l->addWidget( m_view );
    setupGui();

    //m_view->setEditTriggers( m_view->editTriggers() | QAbstractItemView::EditKeyPressed );
    m_view->setDragDropMode( QAbstractItemView::InternalMove );
    m_view->setDropIndicatorShown( false );
    m_view->setDragEnabled ( true );
    m_view->setAcceptDrops( false );
    m_view->setAcceptDropsOnView( false );

    QList<int> readonly;
    readonly << NodeModel::NodeName
            << NodeModel::NodeResponsible
            << NodeModel::NodeAllocation
            << NodeModel::NodeEstimateType
            << NodeModel::NodeEstimateCalendar
            << NodeModel::NodeEstimate
            << NodeModel::NodeOptimisticRatio
            << NodeModel::NodePessimisticRatio
            << NodeModel::NodeRisk
            << NodeModel::NodeConstraint
            << NodeModel::NodeConstraintStart
            << NodeModel::NodeConstraintEnd
            << NodeModel::NodeRunningAccount
            << NodeModel::NodeStartupAccount
            << NodeModel::NodeStartupCost
            << NodeModel::NodeShutdownAccount
            << NodeModel::NodeShutdownCost
            << NodeModel::NodeDescription;
    foreach ( int c, readonly ) {
        m_view->baseModel()->setReadOnly( c, true );
    }

    QList<int> lst1; lst1 << 1 << -1;
    QList<int> show;
    show << NodeModel::NodeStatus
            << NodeModel::NodeCompleted
            << NodeModel::NodeResponsible
            << NodeModel::NodeAssignments
            << NodeModel::NodeDescription;

    for ( int s = 0; s < show.count(); ++s ) {
        m_view->slaveView()->mapToSection( show[s], s );
    }
    QList<int> lst2;
    for ( int i = 0; i < m_view->model()->columnCount(); ++i ) {
        if ( ! show.contains( i ) ) {
            lst2 << i;
        }
    }
    m_view->hideColumns( lst1, lst2 );
    m_view->masterView()->setDefaultColumns( QList<int>() << 0 );
    m_view->slaveView()->setDefaultColumns( show );

    connect( m_view->baseModel(), SIGNAL( executeCommand( KUndo2Command* ) ), doc, SLOT( addCommand( KUndo2Command* ) ) );

    connect( m_view, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ), this, SLOT ( slotCurrentChanged( const QModelIndex &, const QModelIndex & ) ) );

    connect( m_view, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT ( slotSelectionChanged( const QModelIndexList ) ) );

    connect( m_view, SIGNAL( contextMenuRequested( const QModelIndex&, const QPoint& ) ), SLOT( slotContextMenuRequested( const QModelIndex&, const QPoint& ) ) );

    connect( m_view, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
}

Project *TaskWorkPackageView::project() const
{
    return m_view->project();
}

void TaskWorkPackageView::setProject( Project *project )
{
    m_view->setProject( project );
}

NodeSortFilterProxyModel *TaskWorkPackageView::proxyModel() const
{
    return m_view->proxyModel();
}

void TaskWorkPackageView::updateReadWrite( bool rw )
{
    m_view->setReadWrite( rw );
    ViewBase::updateReadWrite( rw );
}

void TaskWorkPackageView::setGuiActive( bool activate )
{
    kDebug(planDbg())<<activate;
    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
    if ( activate && !m_view->selectionModel()->currentIndex().isValid() && m_view->model()->rowCount() > 0 ) {
        m_view->selectionModel()->setCurrentIndex(m_view->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
    }
}

void TaskWorkPackageView::slotRefreshView()
{
    emit checkForWorkPackages();
}

void TaskWorkPackageView::slotCurrentChanged(  const QModelIndex &curr, const QModelIndex & )
{
    kDebug(planDbg())<<curr.row()<<","<<curr.column();
    slotEnableActions();
}

void TaskWorkPackageView::slotSelectionChanged( const QModelIndexList list)
{
    kDebug(planDbg())<<list.count();
    slotEnableActions();
}

int TaskWorkPackageView::selectedNodeCount() const
{
    QItemSelectionModel* sm = m_view->selectionModel();
    return sm->selectedRows().count();
}

QList<Node*> TaskWorkPackageView::selectedNodes() const {
    QList<Node*> lst;
    QItemSelectionModel* sm = m_view->selectionModel();
    if ( sm == 0 ) {
        return lst;
    }
    foreach ( const QModelIndex &i, sm->selectedRows() ) {
        Node * n = m_view->baseModel()->node( proxyModel()->mapToSource( i ) );
        if ( n != 0 && n->type() != Node::Type_Project ) {
            lst.append( n );
        }
    }
    return lst;
}

Node *TaskWorkPackageView::selectedNode() const
{
    QList<Node*> lst = selectedNodes();
    if ( lst.count() != 1 ) {
        return 0;
    }
    return lst.first();
}

Node *TaskWorkPackageView::currentNode() const {
    Node * n = m_view->baseModel()->node( proxyModel()->mapToSource( m_view->selectionModel()->currentIndex() ) );
    if ( n == 0 || n->type() == Node::Type_Project ) {
        return 0;
    }
    return n;
}

void TaskWorkPackageView::slotContextMenuRequested( const QModelIndex& index, const QPoint& pos )
{
    QString name;
    Node *node = m_view->baseModel()->node( proxyModel()->mapToSource( index ) );
    if ( node ) {
        switch ( node->type() ) {
            case Node::Type_Task:
                name = "workpackage_popup";
                break;
            case Node::Type_Milestone:
                name = "taskview_milestone_popup";
                break;
            case Node::Type_Summarytask:
                name = "taskview_summary_popup";
                break;
            default:
                break;
        }
    } else kDebug(planDbg())<<"No node: "<<index;
    if ( name.isEmpty() ) {
        kDebug(planDbg())<<"No menu";
        slotHeaderContextMenuRequested( pos );
        return;
    }
    emit requestPopupMenu( name, pos );
}

void TaskWorkPackageView::setScheduleManager( ScheduleManager *sm )
{
    //kDebug(planDbg())<<endl;
    m_view->baseModel()->setScheduleManager( sm );
}

void TaskWorkPackageView::slotEnableActions()
{
    updateActionsEnabled( true );
}

void TaskWorkPackageView::updateActionsEnabled( bool on )
{
    bool o = ! selectedNodes().isEmpty();
    actionMailWorkpackage->setEnabled( o && on );
}

void TaskWorkPackageView::setupGui()
{
//    KActionCollection *coll = actionCollection();

    QString name = "workpackage_list";
    actionMailWorkpackage  = new KAction(koIcon("mail-send"), i18n("Send..."), this);
    actionMailWorkpackage->setShortcut( KShortcut( Qt::CTRL + Qt::Key_M ) );
    actionCollection()->addAction("send_workpackage", actionMailWorkpackage );
    connect( actionMailWorkpackage, SIGNAL( triggered( bool ) ), SLOT( slotMailWorkpackage() ) );
    addAction( name, actionMailWorkpackage );

    // Add the context menu actions for the view options
    connect(m_view->actionSplitView(), SIGNAL(triggered(bool) ), SLOT(slotSplitView()));
    addContextAction( m_view->actionSplitView() );

    createOptionAction();
}

void TaskWorkPackageView::slotMailWorkpackage()
{
    QList<Node*> lst = selectedNodes();
    if ( ! lst.isEmpty() ) {
        // TODO find a better way to log to avoid undo/redo
        m_cmd = new MacroCommand( i18nc( "(qtundo-format)", "Log Send Workpackage" ) );
        QPointer<WorkPackageSendDialog> dlg = new WorkPackageSendDialog( lst, scheduleManager(), this );
        connect ( dlg->panel(), SIGNAL( sendWorkpackages( QList<Node*>&, Resource* ) ), this, SIGNAL( mailWorkpackages( QList<Node*>&, Resource* ) ) );

        connect ( dlg->panel(), SIGNAL( sendWorkpackages( QList<Node*>&, Resource* ) ), this, SLOT( slotWorkPackageSent( QList<Node*>&, Resource* ) ) );
        dlg->exec();
        delete dlg;
        if ( ! m_cmd->isEmpty() ) {
            part()->addCommand( m_cmd );
            m_cmd = 0;
        }
        delete m_cmd;
        m_cmd = 0;
    }
}

void TaskWorkPackageView::slotWorkPackageSent( QList<Node*> &nodes, Resource *resource )
{
    foreach ( Node *n, nodes ) {
        WorkPackage *wp = new WorkPackage( static_cast<Task*>( n )->workPackage() );
        wp->setOwnerName( resource->name() );
        wp->setOwnerId( resource->id() );
        wp->setTransmitionTime( DateTime::currentDateTime() );
        wp->setTransmitionStatus( WorkPackage::TS_Send );
        m_cmd->addCommand( new WorkPackageAddCmd( static_cast<Project*>( n->projectNode() ), n, wp ) );
    }
}

void TaskWorkPackageView::slotSplitView()
{
    kDebug(planDbg());
    m_view->setViewSplitMode( ! m_view->isViewSplit() );
    emit optionsModified();
}

void TaskWorkPackageView::slotOptions()
{
    kDebug(planDbg());
    SplitItemViewSettupDialog *dlg = new SplitItemViewSettupDialog( this, m_view, this );
    dlg->addPrintingOptions();
    connect(dlg, SIGNAL(finished(int)), SLOT(slotOptionsFinished(int)));
    dlg->show();
    dlg->raise();
    dlg->activateWindow();
}

bool TaskWorkPackageView::loadContext( const KoXmlElement &context )
{
    kDebug(planDbg());
    ViewBase::loadContext( context );
    return m_view->loadContext( m_view->baseModel()->columnMap(), context );
}

void TaskWorkPackageView::saveContext( QDomElement &context ) const
{
    ViewBase::saveContext( context );
    m_view->saveContext( m_view->baseModel()->columnMap(), context );
}

KoPrintJob *TaskWorkPackageView::createPrintJob()
{
    return m_view->createPrintJob( this );
}

} // namespace KPlato

#include "kpttaskeditor.moc"
