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
#include "kpttaskstatusmodel.h"

#include "kptglobal.h"
#include "kptcommand.h"
#include "kptproject.h"
#include "kptschedule.h"

#include <KoDocument.h>

#include <QDragMoveEvent>
#include <QMenu>
#include <QModelIndex>
#include <QVBoxLayout>
#include <QWidget>
#include <QTextBrowser>
#include <QTextCursor>
#include <QTextTableFormat>
#include <QTextLength>
#include <QTextTable>
#include <QTextFrame>
#include <QTextFrameFormat>
#include <QTextCharFormat>
#include <QTextTableCell>

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


TaskStatusTreeView::TaskStatusTreeView( QWidget *parent )
    : DoubleTreeViewBase( parent )
{
    setContextMenuPolicy( Qt::CustomContextMenu );
    TaskStatusItemModel *m = new TaskStatusItemModel();
    setModel( m );
    //setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    setStretchLastSection( false );
    
    createItemDelegates();
    
    connect( this, SIGNAL( activated ( const QModelIndex ) ), this, SLOT( slotActivated( const QModelIndex ) ) );

    QList<int> lst1; lst1 << 1 << -1; // only display column 0 (NodeName) in left view
    masterView()->setDefaultColumns( QList<int>() << 0 );
    QList<int> show;
    show << NodeModel::NodeCompleted
            << NodeModel::NodeActualEffort
            << NodeModel::NodeRemainingEffort
            << NodeModel::NodePlannedEffort
            << NodeModel::NodePlannedCost
            << NodeModel::NodeActualCost
            << NodeModel::NodeStatus
            << NodeModel::NodeStarted
            << NodeModel::NodeFinished
            << NodeModel::NodeStatusNote;

    QList<int> lst2; 
    for ( int i = 0; i < m->columnCount(); ++i ) {
        if ( ! show.contains( i ) ) {
            lst2 << i;
        }
    }
    hideColumns( lst1, lst2 );
    slaveView()->setDefaultColumns( show );
}

int TaskStatusTreeView::weekday() const
{
    return model()->weekday();
}

void TaskStatusTreeView::setWeekday( int day )
{
    model()->setWeekday( day );
    refresh();
}

int TaskStatusTreeView::defaultPeriodType() const
{
    return TaskStatusItemModel::UseCurrentDate;
}

int TaskStatusTreeView::periodType() const
{
    return model()->periodType();
}

void TaskStatusTreeView::setPeriodType( int type )
{
    model()->setPeriodType( type );
    refresh();
}

int TaskStatusTreeView::period() const
{
    return model()->period();
}

void TaskStatusTreeView::setPeriod( int days )
{
    model()->setPeriod( days );
    refresh();
}

TaskStatusItemModel *TaskStatusTreeView::model() const
{
    return static_cast<TaskStatusItemModel*>( DoubleTreeViewBase::model() );
}

Project *TaskStatusTreeView::project() const
{
    return model()->project();
}

void TaskStatusTreeView::setProject( Project *project )
{
    model()->setProject( project );
}

void TaskStatusTreeView::slotActivated( const QModelIndex index )
{
    kDebug()<<index.column();
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
    Node *dn = model()->node( index );
    if ( dn == 0 ) {
        kError()<<"no node to drop on!"
        return; // hmmm
    }
    switch ( dropIndicatorPosition() ) {
        case AboveItem:
        case BelowItem:
            //dn == sibling
            if ( model()->dropAllowed( dn->parentNode(), event->mimeData() ) ) {
                event->accept();
            }
            break;
        case OnItem:
            //dn == new parent
            if ( model()->dropAllowed( dn, event->mimeData() ) ) {
                event->accept();
            }
            break;
        default:
            break;
    }*/
}


//-----------------------------------
TaskStatusView::TaskStatusView( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent ),
    m_id( -1 )
{
    kDebug()<<"-------------------- creating TaskStatusView -------------------";
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new TaskStatusTreeView( this );
    l->addWidget( m_view );
    setupGui();

    connect( model(), SIGNAL( executeCommand( QUndoCommand* ) ), part, SLOT( addCommand( QUndoCommand* ) ) );

    connect( m_view, SIGNAL( contextMenuRequested( const QModelIndex&, const QPoint& ) ), SLOT( slotContextMenuRequested( const QModelIndex&, const QPoint& ) ) );
    
    connect( m_view, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
}

void TaskStatusView::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug();
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos,  lst.first() );
    }
}

void TaskStatusView::updateReadWrite( bool rw )
{
    m_view->setReadWrite( rw );
}

void TaskStatusView::setScheduleManager( ScheduleManager *sm )
{
    //kDebug();
    static_cast<TaskStatusItemModel*>( m_view->model() )->setManager( sm );
}

Node *TaskStatusView::currentNode() const 
{
    Node * n = m_view->model()->node( m_view->selectionModel()->currentIndex() );
    if ( n && n->type() != Node::Type_Task ) {
        return 0;
    }
    return n;
}

void TaskStatusView::setProject( Project *project )
{
    m_project = project;
    m_view->model()->setProject( m_project );
}

void TaskStatusView::draw( Project &project )
{
    setProject( &project );
}

void TaskStatusView::setGuiActive( bool activate )
{
    kDebug()<<activate;
//    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
}

void TaskStatusView::slotContextMenuRequested( const QModelIndex &index, const QPoint& pos )
{
    kDebug()<<index<<pos;
    if ( ! index.isValid() ) {
        return;
    }
    Node *node = m_view->model()->node( index );
    if ( node == 0 ) {
        return;
    }
    slotContextMenuRequested( node, pos );
}

void TaskStatusView::slotContextMenuRequested( Node *node, const QPoint& pos )
{
    kDebug()<<node->name()<<" :"<<pos;
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
    kDebug()<<name;
    if ( name.isEmpty() ) {
        return;
    }
    emit requestPopupMenu( name, pos );
}

void TaskStatusView::setupGui()
{
    // Add the context menu actions for the view options
    connect(m_view->actionSplitView(), SIGNAL(triggered(bool) ), SLOT(slotSplitView()));
    addContextAction( m_view->actionSplitView() );
    
    actionOptions = new KAction(KIcon("configure"), i18n("Configure..."), this);
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );
}

void TaskStatusView::slotSplitView()
{
    kDebug();
    m_view->setViewSplitMode( ! m_view->isViewSplit() );
}


void TaskStatusView::slotOptions()
{
    kDebug();
    TaskStatusViewSettingsDialog dlg( m_view );
    dlg.exec();
}

bool TaskStatusView::loadContext( const KoXmlElement &context )
{
    kDebug();
    m_view->setPeriod( context.attribute( "period", QString("%1").arg( m_view->defaultPeriod() ) ).toInt() );
    
    m_view->setPeriodType( context.attribute( "periodtype", QString("%1").arg( m_view->defaultPeriodType() ) ).toInt() );
    
    m_view->setWeekday( context.attribute( "weekday", QString("%1").arg( m_view->defaultWeekday() ) ).toInt() );
    return m_view->loadContext( model()->columnMap(), context );
}

void TaskStatusView::saveContext( QDomElement &context ) const
{
    context.setAttribute( "period", m_view->period() );
    context.setAttribute( "periodtype", m_view->periodType() );
    context.setAttribute( "weekday", m_view->weekday() );
    m_view->saveContext( model()->columnMap(), context );
}

//------------------------------------------------
TaskStatusViewSettingsPanel::TaskStatusViewSettingsPanel( TaskStatusTreeView *view, QWidget *parent )
    : QWidget( parent ),
    m_view( view )
{
    setupUi( this );
    
    QStringList lst;
    for ( int i = 1; i <= 7; ++i ) {
        lst << QDate::longDayName( i );
    }
    weekdays->addItems( lst );
    period->setValue( view->period() );
    switch ( view->periodType() ) {
        case TaskStatusItemModel::UseCurrentDate: useCurrentDate->setChecked( true ); break;
        case TaskStatusItemModel::UseWeekday: useWeekday->setChecked( true ); break;
        default: break;
    }
    weekdays->setCurrentIndex( m_view->weekday() - 1 );
    
    connect( period, SIGNAL( valueChanged( int ) ), SIGNAL( changed() ) );
    connect( useWeekday, SIGNAL( toggled( bool ) ), SIGNAL( changed() ) );
    connect( useCurrentDate, SIGNAL( toggled( bool ) ), SIGNAL( changed() ) );
    connect( weekdays, SIGNAL( currentIndexChanged( int ) ), SIGNAL( changed() ) );
}

void TaskStatusViewSettingsPanel::slotOk()
{
    if ( period->value() != m_view->period() ) {
        m_view->setPeriod( period->value() );
    }
    if ( weekdays->currentIndex() != m_view->weekday() - 1 ) {
        m_view->setWeekday( weekdays->currentIndex() + 1 );
    }
    if ( useCurrentDate->isChecked() && m_view->periodType() != TaskStatusItemModel::UseCurrentDate ) {
        m_view->setPeriodType( TaskStatusItemModel::UseCurrentDate );
    } else if ( useWeekday->isChecked() && m_view->periodType() != TaskStatusItemModel::UseWeekday ) {
        m_view->setPeriodType( TaskStatusItemModel::UseWeekday );
    }
}

void TaskStatusViewSettingsPanel::setDefault()
{
    period->setValue( m_view->defaultPeriod() );
    switch ( m_view->defaultPeriodType() ) {
        case TaskStatusItemModel::UseCurrentDate: useCurrentDate->setChecked( true ); break;
        case TaskStatusItemModel::UseWeekday: useWeekday->setChecked( true ); break;
        default: break;
    }
    weekdays->setCurrentIndex( m_view->defaultWeekday() - 1 );
}

TaskStatusViewSettingsDialog::TaskStatusViewSettingsDialog( TaskStatusTreeView *view, QWidget *parent )
    : SplitItemViewSettupDialog( view, parent )
{
    TaskStatusViewSettingsPanel *panel = new TaskStatusViewSettingsPanel( view );
    KPageWidgetItem *page = insertWidget( 0, panel, i18n( "General" ), i18n( "General Settings" ) );
    setCurrentPage( page );
    //connect( panel, SIGNAL( changed( bool ) ), this, SLOT( enableButtonOk( bool ) ) );
    
    connect( this, SIGNAL( okClicked() ), panel, SLOT( slotOk() ) );
    connect( this, SIGNAL( defaultClicked() ), panel, SLOT( setDefault() ) );
}

//-----------------------------------
ProjectStatusView::ProjectStatusView( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent ),
    m_project( 0 ),
    m_manager( 0 )
{
    kDebug()<<"-------------------- creating ProjectStatusView -------------------";
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new QTextBrowser( this );
    l->addWidget( m_view );
    setupGui();

}

void ProjectStatusView::setScheduleManager( ScheduleManager *sm )
{
    //kDebug();
    m_manager = sm;
    draw();
}

void ProjectStatusView::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotUpdate( ScheduleManager* ) ) );
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotUpdate() ) );
    }
    m_project = project;
    if ( m_project ) {
        connect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotUpdate( ScheduleManager* ) ) );
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotUpdate() ) );
    }
}

void ProjectStatusView::slotUpdate()
{
    draw();
}

void ProjectStatusView::slotUpdate( ScheduleManager *sm )
{
    if ( sm == m_manager ) {
        slotUpdate();
    }
}

void ProjectStatusView::draw()
{
    m_view->clear();
    if ( m_project == 0 ) {
        return;
    }
    
    QTextCursor cursor = m_view->textCursor();
    cursor.movePosition(QTextCursor::Start);
    QTextFrame *topFrame = cursor.currentFrame();
    
    QTextTableFormat tableFormat;
    tableFormat.setAlignment(Qt::AlignHCenter);
//    tableFormat.setBackground(QColor("#e0e0e0"));
    tableFormat.setCellPadding(2);
    tableFormat.setCellSpacing(4);
    
    QVector<QTextLength> constraints;
    constraints << QTextLength(QTextLength::PercentageLength, 20)
            << QTextLength(QTextLength::PercentageLength, 80);
    tableFormat.setColumnWidthConstraints(constraints);
    
    QTextTable *table = cursor.insertTable(2, 2, tableFormat);
    QTextFrame *frame = cursor.currentFrame();
    QTextFrameFormat frameFormat = frame->frameFormat();
    frameFormat.setBorder(1);
    frame->setFrameFormat(frameFormat);
    QTextCharFormat format = cursor.charFormat();
    format.setFontPointSize(11);

    QTextCharFormat boldFormat = format;
    boldFormat.setFontWeight(QFont::Bold);

    QTextCharFormat highlightedFormat = boldFormat;
    highlightedFormat.setBackground(Qt::yellow);
    
    QTextTableCell cell = table->cellAt(0, 0);
    QTextCursor cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(i18n("Project:"), format);
    
    cell = table->cellAt(0, 1);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(m_project->name(), boldFormat);

    cell = table->cellAt(1, 0);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(i18n("Schedule:"), format);

    QString s = m_manager == 0 ? i18n("None") : m_manager->name();
    cell = table->cellAt(1, 1);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(s, boldFormat);
    
    if ( m_manager == 0 ) {
        return;
    }

    NodeModel m;
    m.setProject( m_project );
    m.setManager( m_manager );
    
    cursor.setPosition(topFrame->lastPosition());
    cursor.insertBlock();
    cursor.insertBlock();
    
    constraints.clear();
    constraints << QTextLength(QTextLength::PercentageLength, 25)
            << QTextLength(QTextLength::PercentageLength, 25)
            << QTextLength(QTextLength::PercentageLength, 25)
            << QTextLength(QTextLength::PercentageLength, 25);
    tableFormat.setColumnWidthConstraints(constraints);
    table = cursor.insertTable(2, 4, tableFormat);
    //frame = cursor.currentFrame();
    //frameFormat = frame->frameFormat();
    //frameFormat.setBorder(1);
    //frame->setFrameFormat(frameFormat);
    //format = cursor.charFormat();
    //format.setFontPointSize(11);

    //boldFormat = format;
    //boldFormat.setFontWeight(QFont::Bold);

    //highlightedFormat = boldFormat;
    //highlightedFormat.setBackground(Qt::yellow);
    
    cell = table->cellAt(0, 0);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(i18n("BCWS"), boldFormat);
    
    cell = table->cellAt(1, 0);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(m.data( m_project, NodeModel::NodeBCWS ).toString(), format);

    cell = table->cellAt(0, 1);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(i18n("BCWP"), boldFormat);

    cell = table->cellAt(1, 1);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(m.data( m_project, NodeModel::NodeBCWP ).toString(), format);
    
    cell = table->cellAt(0, 2);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(i18n("ACWP"), boldFormat);

    cell = table->cellAt(1, 2);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(m.data( m_project, NodeModel::NodeACWP ).toString(), format);

    cell = table->cellAt(0, 3);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(i18n("SPI"), boldFormat);

    cell = table->cellAt(1, 3);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(m.data( m_project, NodeModel::NodePerformanceIndex ).toString(), format);
}

void ProjectStatusView::setGuiActive( bool activate )
{
    kDebug()<<activate;
//    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
}

void ProjectStatusView::setupGui()
{
    // Add the context menu actions for the view options
/*    actionOptions = new KAction(KIcon("configure"), i18n("Configure..."), this);
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );*/
}

void ProjectStatusView::slotSplitView()
{
    kDebug();
}


void ProjectStatusView::slotOptions()
{
    kDebug();
}

bool ProjectStatusView::loadContext( const KoXmlElement &context )
{
    kDebug();
    return true;
}

void ProjectStatusView::saveContext( QDomElement &context ) const
{
}



} // namespace KPlato

#include "kpttaskstatusview.moc"
