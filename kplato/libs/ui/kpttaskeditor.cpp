/* This file is part of the KDE project
  Copyright (C) 2006 - 2007 Dag Andersen <danders@get2net.dk>

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
#include "kptnodeitemmodel.h"
#include "kptcommand.h"
#include "kptproject.h"
#include "kptitemviewsettup.h"

#include <KoDocument.h>

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

//--------------------
NodeTreeView::NodeTreeView( QWidget *parent )
    : DoubleTreeViewBase( parent )
{
    setModel( new NodeItemModel() );
    //setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    setSelectionBehavior( QAbstractItemView::SelectRows );
    
    for ( int c = 0; c < model()->columnCount(); ++c ) {
        QItemDelegate *delegate = model()->createDelegate( c, this );
        if ( delegate ) {
            setItemDelegateForColumn( c, delegate );
        }
    }
}

void NodeTreeView::slotActivated( const QModelIndex index )
{
    kDebug()<<index.column();
}

//-----------------------------------
TaskEditor::TaskEditor( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent )
{
    kDebug()<<"----------------- Create TaskEditor ----------------------";
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new NodeTreeView( this );
    l->addWidget( m_view );
    kDebug()<<m_view->actionSplitView();
    setupGui();

    m_view->setEditTriggers( m_view->editTriggers() | QAbstractItemView::EditKeyPressed );
    QList<int> lst1; lst1 << 1 << -1;
    QList<int> lst2; lst2 << 0 << 18 << -1;
    m_view->hideColumns( lst1, lst2 );


    m_view->setDragDropMode( QAbstractItemView::InternalMove );
    m_view->setDropIndicatorShown( true );
    m_view->setDragEnabled ( true );
    m_view->setAcceptDrops( true );
    m_view->setAcceptDropsOnView( true );
    
    connect( model(), SIGNAL( executeCommand( QUndoCommand* ) ), part, SLOT( addCommand( QUndoCommand* ) ) );
    
    connect( m_view, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ), this, SLOT ( slotCurrentChanged( const QModelIndex &, const QModelIndex & ) ) );

    connect( m_view, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT ( slotSelectionChanged( const QModelIndexList ) ) );
    
    connect( m_view, SIGNAL( contextMenuRequested( const QModelIndex&, const QPoint& ) ), SLOT( slotContextMenuRequested( const QModelIndex&, const QPoint& ) ) );

    connect( m_view, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
    
}

void TaskEditor::updateReadWrite( bool rw )
{
    m_view->setReadWrite( rw );
}

void TaskEditor::draw( Project &project )
{
    m_view->setProject( &project );
}

void TaskEditor::draw()
{
}

void TaskEditor::setGuiActive( bool activate )
{
    kDebug()<<activate;
    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
    if ( activate && !m_view->selectionModel()->currentIndex().isValid() ) {
        m_view->selectionModel()->setCurrentIndex(m_view->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
    }
}

void TaskEditor::slotCurrentChanged(  const QModelIndex &curr, const QModelIndex & )
{
    kDebug()<<curr.row()<<","<<curr.column();
    slotEnableActions();
}

void TaskEditor::slotSelectionChanged( const QModelIndexList list)
{
    kDebug()<<list.count();
    slotEnableActions();
}

int TaskEditor::selectedNodeCount() const
{
    QItemSelectionModel* sm = m_view->selectionModel();
    return sm->selectedRows().count();
}

QList<Node*> TaskEditor::selectedNodes() const {
    QList<Node*> lst;
    QItemSelectionModel* sm = m_view->selectionModel();
    if ( sm == 0 ) {
        return lst;
    }
    foreach ( QModelIndex i, sm->selectedRows() ) {
        Node * n = m_view->model()->node( i );
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
    Node * n = m_view->model()->node( m_view->selectionModel()->currentIndex() );
    if ( n == 0 || n->type() == Node::Type_Project ) {
        return 0;
    }
    return n;
}

void TaskEditor::slotContextMenuRequested( const QModelIndex& index, const QPoint& pos )
{
    Node *node = m_view->model()->node( index );
    if ( node == 0 ) {
        return;
    }
    kDebug()<<node->name()<<" :"<<pos;
    QString name;
    switch ( node->type() ) {
        case Node::Type_Task:
        case Node::Type_Milestone:
            name = "task_popup";
            break;
        case Node::Type_Summarytask:
            name = "summarytask_popup";
            break;
        default:
            name = "node_popup";
    }
    kDebug()<<name;
    emit requestPopupMenu( name, pos );
}

void TaskEditor::slotCurrentScheduleManagerChanged( ScheduleManager *sm )
{
    //kDebug()<<endl;
    static_cast<NodeItemModel*>( m_view->model() )->setManager( sm );
}

void TaskEditor::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug();
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos,  lst.first() );
    }
}

void TaskEditor::slotEnableActions()
{
    updateActionsEnabled( true );
}

void TaskEditor::updateActionsEnabled( bool on )
{
    Project *p = m_view->project();
    
    bool o = ( on && p && selectedNodeCount() <= 1 );
    actionAddTask->setEnabled( o );
    actionAddMilestone->setEnabled( o );
    
    actionDeleteTask->setEnabled( on && p && selectedNodeCount() > 0 );
    
    o = ( on && p && selectedNodeCount() == 1 );
    Node *n = selectedNode();
    
    actionAddSubtask->setEnabled( o );
    actionMoveTaskUp->setEnabled( o && p->canMoveTaskUp( n ) );
    actionMoveTaskDown->setEnabled( o && p->canMoveTaskDown( n ) );
    actionIndentTask->setEnabled( o && p->canIndentTask( n ) );
    actionUnindentTask->setEnabled( o && p->canUnindentTask( n ) );
}

void TaskEditor::setupGui()
{
    QString name = "taskeditor_add_list";
    actionAddTask  = new KAction(KIcon( "add_task" ), i18n("Add Task..."), this);
    actionCollection()->addAction("add_task", actionAddTask );
    connect( actionAddTask, SIGNAL( triggered( bool ) ), SLOT( slotAddTask() ) );
    addAction( name, actionAddTask );
    
    actionAddSubtask  = new KAction(KIcon( "add_sub_task" ), i18n("Add Sub-Task..."), this);
    actionCollection()->addAction("add_sub_task", actionAddSubtask );
    connect( actionAddSubtask, SIGNAL( triggered( bool ) ), SLOT( slotAddSubtask() ) );
    addAction( name, actionAddSubtask );
    
    actionAddMilestone  = new KAction(KIcon( "add_milestone" ), i18n("Add Milestone..."), this);
    actionCollection()->addAction("add_milestone", actionAddMilestone );
    connect( actionAddMilestone, SIGNAL( triggered( bool ) ), SLOT( slotAddMilestone() ) );
    addAction( name, actionAddMilestone );
    
    actionDeleteTask  = new KAction(KIcon( "edit-delete" ), i18n("Delete Task"), this);
    actionCollection()->addAction("delete_task", actionDeleteTask );
    connect( actionDeleteTask, SIGNAL( triggered( bool ) ), SLOT( slotDeleteTask() ) );
    addAction( name, actionDeleteTask );

    
    name = "taskeditor_move_list";
    actionIndentTask  = new KAction(KIcon("indent_task"), i18n("Indent Task"), this);
    actionCollection()->addAction("indent_task", actionIndentTask );
    connect(actionIndentTask, SIGNAL(triggered(bool) ), SLOT(slotIndentTask()));
    addAction( name, actionIndentTask );
    
    actionUnindentTask  = new KAction(KIcon("unindent_task"), i18n("Unindent Task"), this);
    actionCollection()->addAction("unindent_task", actionUnindentTask );
    connect(actionUnindentTask, SIGNAL(triggered(bool) ), SLOT(slotUnindentTask()));
    addAction( name, actionUnindentTask );
    
    actionMoveTaskUp  = new KAction(KIcon("move_task_up"), i18n("Move Up"), this);
    actionCollection()->addAction("move_task_up", actionMoveTaskUp );
    connect(actionMoveTaskUp, SIGNAL(triggered(bool) ), SLOT(slotMoveTaskUp()));
    addAction( name, actionMoveTaskUp );
    
    actionMoveTaskDown  = new KAction(KIcon("move_task_down"), i18n("Move Down"), this);
    actionCollection()->addAction("move_task_down", actionMoveTaskDown );
    connect(actionMoveTaskDown, SIGNAL(triggered(bool) ), SLOT(slotMoveTaskDown()));
    addAction( name, actionMoveTaskDown );

    // Add the context menu actions for the view options
    connect(m_view->actionSplitView(), SIGNAL(triggered(bool) ), SLOT(slotSplitView()));
    addContextAction( m_view->actionSplitView() );

    actionOptions = new KAction(KIcon("configure"), i18n("Configure..."), this);
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );
}

void TaskEditor::slotSplitView()
{
    kDebug();
    m_view->setViewSplitMode( ! m_view->isViewSplit() );
}


void TaskEditor::slotOptions()
{
    kDebug();
    bool col0 = false;
    TreeViewBase *v = m_view->slaveView();
    if ( v->isHidden() ) {
        v = m_view->masterView();
        col0 = true;
    }
    ItemViewSettupDialog dlg( v, col0 );
    dlg.exec();
}

void TaskEditor::slotAddTask()
{
    kDebug();
    if ( selectedNodeCount() == 0 ) {
        // insert under main project
        Task *t = m_view->project()->createTask( /*TODO part()->config().taskDefaults(),*/ m_view->project() );
        edit( m_view->model()->insertSubtask( t, t->parentNode() ) );
	kDebug()<<"test"<<t->type();
        return;
    }
    Node *sib = selectedNode();
    if ( sib == 0 ) {
        return;
    }
    Task *t = m_view->project()->createTask( /*TODO part()->config().taskDefaults(),*/ sib->parentNode() );
    edit( m_view->model()->insertTask( t, sib ) );
}

void TaskEditor::slotAddMilestone()
{
    kDebug();
    if ( selectedNodeCount() == 0 ) {
        // insert under main project
        Task *t = m_view->project()->createTask( /*TODO part()->config().taskDefaults(),*/ m_view->project() );
        t->estimate()->clear();
        edit( m_view->model()->insertSubtask( t, t->parentNode() ) );
        return;
    }
    Node *sib = selectedNode(); // sibling
    if ( sib == 0 ) {
        return;
    }
    Task *t = m_view->project()->createTask( /*TODO part()->config().taskDefaults(),*/ sib->parentNode() );
    t->estimate()->clear();
    edit( m_view->model()->insertTask( t, sib ) );
}

void TaskEditor::slotAddSubtask()
{
    kDebug();
    Node *parent = selectedNode();
    if ( parent == 0 ) {
        return;
    }
    Task *t = m_view->project()->createTask( /*TODO part()->config().taskDefaults(),*/ parent );
    edit( m_view->model()->insertSubtask( t, parent ) );
}

void TaskEditor::edit( QModelIndex i )
{
    if ( i.isValid() ) {
        QModelIndex p = m_view->model()->parent( i );
//        m_view->setExpanded( p );
        m_view->selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
        m_view->edit( i );
    }
}

void TaskEditor::slotDeleteTask()
{
    //kDebug();
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
    //foreach ( Node* n, lst ) { kDebug()<<n->name(); }
    emit deleteTaskList( lst );
}

void TaskEditor::slotIndentTask()
{
    kDebug();
    emit indentTask();
}

void TaskEditor::slotUnindentTask()
{
    kDebug();
    emit unindentTask();
}

void TaskEditor::slotMoveTaskUp()
{
    kDebug();
    emit moveTaskUp();
}

void TaskEditor::slotMoveTaskDown()
{
    kDebug();
    emit moveTaskDown();
}

bool TaskEditor::loadContext( const KoXmlElement &context )
{
    kDebug()<<endl;
    return m_view->loadContext( context );
}

void TaskEditor::saveContext( QDomElement &context ) const
{
   m_view->saveContext( context );
}

//-----------------------------------
TaskView::TaskView( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent )
{
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new NodeTreeView( this );
    l->addWidget( m_view );
    updateReadWrite( false );
    setupGui();

    //m_view->setEditTriggers( m_view->editTriggers() | QAbstractItemView::EditKeyPressed );
    QList<int> lst1; lst1 << 1 << -1;
    QList<int> lst2; lst2 << 0 << 1 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12 << 13 << 14 << 15 << 16 << 18 << 19 << 20 << 21 << 22 << 23 << 24 << 25 << 26 << 27 << 28 << 29 << 30 << 31 << 32 << 34 << 35 << 36 << 37 << 40 << -1;
    m_view->hideColumns( lst1, lst2 );
    
    TreeViewBase *v = m_view->slaveView();
    if ( v == 0 ) {
        v = m_view->masterView();
    }
    v->mapToSection( 38, 1 );
    v->mapToSection( 39, 2 );
    v->mapToSection( 2, 3 );
    v->mapToSection( 33, 4 );
    v->mapToSection( 17, 5 );

    m_view->setDragDropMode( QAbstractItemView::InternalMove );
    m_view->setDropIndicatorShown( false );
    m_view->setDragEnabled ( true );
    m_view->setAcceptDrops( false );
    m_view->setAcceptDropsOnView( false );
    
    connect( m_view, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ), this, SLOT ( slotCurrentChanged( const QModelIndex &, const QModelIndex & ) ) );

    connect( m_view, SIGNAL( selectionChanged( const QModelIndexList ) ), this, SLOT ( slotSelectionChanged( const QModelIndexList ) ) );
    
    connect( m_view, SIGNAL( contextMenuRequested( const QModelIndex&, const QPoint& ) ), SLOT( slotContextMenuRequested( const QModelIndex&, const QPoint& ) ) );

    connect( m_view, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
}

void TaskView::updateReadWrite( bool /*rw*/ )
{
    // This is view only, don't allow editing
    m_view->setReadWrite( false );
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
    kDebug()<<activate;
    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
    if ( activate && !m_view->selectionModel()->currentIndex().isValid() ) {
        m_view->selectionModel()->setCurrentIndex(m_view->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
    }
}

void TaskView::slotCurrentChanged(  const QModelIndex &curr, const QModelIndex & )
{
    kDebug()<<curr.row()<<","<<curr.column();
    slotEnableActions();
}

void TaskView::slotSelectionChanged( const QModelIndexList list)
{
    kDebug()<<list.count();
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
    foreach ( QModelIndex i, sm->selectedRows() ) {
        Node * n = m_view->model()->node( i );
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
    Node * n = m_view->model()->node( m_view->selectionModel()->currentIndex() );
    if ( n == 0 || n->type() == Node::Type_Project ) {
        return 0;
    }
    return n;
}

void TaskView::slotContextMenuRequested( const QModelIndex& index, const QPoint& pos )
{
    QString name;
    Node *node = m_view->model()->node( index );
    if ( node ) {
        switch ( node->type() ) {
            case Node::Type_Task:
                name = "taskview_popup";
                break;
            case Node::Type_Milestone:
                break;
            case Node::Type_Summarytask:
                break;
            default:
                break;
        }
    } else kDebug()<<"No node: "<<index;
    if ( name.isEmpty() ) {
        kDebug()<<"No menu";
        return;
    }
    emit requestPopupMenu( name, pos );
}

void TaskView::slotCurrentScheduleManagerChanged( ScheduleManager *sm )
{
    //kDebug()<<endl;
    static_cast<NodeItemModel*>( m_view->model() )->setManager( sm );
}

void TaskView::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug();
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos,  lst.first() );
    }
}

void TaskView::slotEnableActions()
{
    updateActionsEnabled( true );
}

void TaskView::updateActionsEnabled( bool on )
{
    
}

void TaskView::setupGui()
{
    KActionCollection *coll = actionCollection();
    
    // Add the context menu actions for the view options
    connect(m_view->actionSplitView(), SIGNAL(triggered(bool) ), SLOT(slotSplitView()));
    addContextAction( m_view->actionSplitView() );
    
    actionOptions = new KAction(KIcon("configure"), i18n("Configure..."), this);
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );
}

void TaskView::slotSplitView()
{
    kDebug();
    m_view->setViewSplitMode( ! m_view->isViewSplit() );
}

void TaskView::slotOptions()
{
    kDebug();
    bool col0 = false;
    TreeViewBase *v = m_view->slaveView();
    if ( v->isHidden() ) {
        v = m_view->masterView();
        col0 = true;
    }
    ItemViewSettupDialog dlg( v, col0 );
    dlg.exec();
}

bool TaskView::loadContext( const KoXmlElement &context )
{
    kDebug()<<endl;
    return m_view->loadContext( context );
}

void TaskView::saveContext( QDomElement &context ) const
{
    m_view->saveContext( context );
}


} // namespace KPlato

#include "kpttaskeditor.moc"
