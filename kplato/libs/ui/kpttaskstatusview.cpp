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
#include "kpteffortcostmap.h"

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
#include <QLineEdit>

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
#include <kplotobject.h>

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
    QTime t; t.start();
    kDebug();
    m_view->clear();
    if ( m_project == 0 ) {
        return;
    }
    KLocale *l = KGlobal::locale();
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

    QTextCharFormat highlightedFormat = format;
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
    constraints << QTextLength(QTextLength::PercentageLength, 20)
            << QTextLength(QTextLength::PercentageLength, 20)
            << QTextLength(QTextLength::PercentageLength, 20)
            << QTextLength(QTextLength::PercentageLength, 20)
            << QTextLength(QTextLength::PercentageLength, 20);
    tableFormat.setColumnWidthConstraints(constraints);
    table = cursor.insertTable(2, 5, tableFormat);
    //frame = cursor.currentFrame();
    //frameFormat = frame->frameFormat();
    //frameFormat.setBorder(1);
    //frame->setFrameFormat(frameFormat);
    //format = cursor.charFormat();
    //format.setFontPointSize(11);

    //boldFormat = format;
    //boldFormat.setFontWeight(QFont::Bold);

    
    cell = table->cellAt(0, 0);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(i18nc("Budgeted Cost of Work Scheduled", "BCWS (hours/cost)"), boldFormat);
    
    cell = table->cellAt(1, 0);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(m.data( m_project, NodeModel::NodeBCWS ).toString(), highlightedFormat);

    cell = table->cellAt(0, 1);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(i18nc("Budgeted Cost of Work Performed", "BCWP (hours/cost)"), boldFormat);

    cell = table->cellAt(1, 1);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(m.data( m_project, NodeModel::NodeBCWP ).toString(), highlightedFormat);
    
    cell = table->cellAt(0, 2);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(i18nc("Actual Cost of Work Performed", "ACWP (hours/cost)"), boldFormat);

    cell = table->cellAt(1, 2);
    cellCursor = cell.firstCursorPosition();
    QString ss = m.data( m_project, NodeModel::NodeACWP ).toString();
    cellCursor.insertText(ss, highlightedFormat);
    
    cell = table->cellAt(0, 3);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(i18nc("Schedule Performance Index", "SPI (hours/cost)"), boldFormat);

    cell = table->cellAt(1, 3);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(m.data( m_project, NodeModel::NodePerformanceIndex ).toString(), highlightedFormat);

    cell = table->cellAt(0, 4);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(i18n("Date"), boldFormat);

    cell = table->cellAt(1, 4);
    cellCursor = cell.firstCursorPosition();
    cellCursor.insertText( m.now().toString( Qt::ISODate ) + " (" + i18n("Today") + ")", highlightedFormat);

    EffortCostMap c = m_project->acwp( m.id() );
    int elapsed = t.elapsed();
    kDebug()<<"acwp:"<<elapsed;
    
    EffortCostMap b = m_project->bcwpPrDay( m.id() );
    kDebug()<<"bcwp:"<<t.elapsed() - elapsed;
    
    QMap<QDate, int> days;
    foreach ( const QDate &d, b.days().keys() ) {
        days.insert( d, 0 );
    }
    foreach ( const QDate &d, c.days().keys() ) {
        if ( ! days.contains( d ) ) {
            days.insert( d, 0 );
        }
    }
    table->resize( 2 + days.count(), table->columns() );
    kDebug()<<b.days().count()<<c.days().count()<<days.count()<<table->rows();
    double eff = 0.0;
    double cost = 0.0;
    double aeff = 0.0;
    double acost = 0.0;
    int i=2;
    foreach ( const QDate &d, days.keys() ) {
        eff += b.days().value( d ).effort().toDouble( Duration::Unit_h );
        cost += b.days().value( d ).cost();
        cell = table->cellAt(i, 0);
        cellCursor = cell.firstCursorPosition();
        cellCursor.insertText( QString("%1 / %2").arg(eff, 0, 'f', 2 ).arg(l->formatMoney(cost, QString(), 0)), format );
        
        double beff = b.days().value( d ).bcwpEffort();
        double bcost = b.days().value( d ).bcwpCost();
        cell = table->cellAt(i, 1);
        cellCursor = cell.firstCursorPosition();
        cellCursor.insertText( QString("%1 / %2").arg(beff, 0, 'f', 2).arg(l->formatMoney(bcost, QString(), 0)), format );
    
        EffortCost acwp = c.days().value( d );
        aeff += acwp.effort().toDouble( Duration::Unit_h );
        acost += acwp.cost();
        cell = table->cellAt(i, 2);
        cellCursor = cell.firstCursorPosition();
        cellCursor.insertText( QString("%1 / %2").arg(aeff, 0, 'f', 2).arg(l->formatMoney(acost, QString(), 0)), format );
        
        cell = table->cellAt(i, 3);
        cellCursor = cell.firstCursorPosition();
        cellCursor.insertText( QString("%1 / %2").arg((beff/eff), 0, 'f', 2).arg(bcost/cost, 0, 'f', 2), format );
        
        cell = table->cellAt(i, 4);
        cellCursor = cell.firstCursorPosition();
        cellCursor.insertText( d.toString( Qt::ISODate ), format );
        
        ++i;
    }
    kDebug()<<"finished"<<t.elapsed();
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


//-----------------------------------
PerformanceStatusBase::PerformanceStatusBase( QWidget *parent )
    : QWidget( parent ),
    m_project( 0 ),
    m_manager( 0 )
{
    setupUi( this );
    plotwidget->setAntialiasing(false);
}

void PerformanceStatusBase::setScheduleManager( ScheduleManager *sm )
{
    //kDebug();
    m_manager = sm;
    m_model.setScheduleManager( sm );
}

void PerformanceStatusBase::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotUpdate( ScheduleManager* ) ) );
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotUpdate() ) );
    }
    m_project = project;
    m_model.setProject( project );
    if ( m_project ) {
        connect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotUpdate( ScheduleManager* ) ) );
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotUpdate() ) );
    }
}

void PerformanceStatusBase::draw()
{
    if ( m_project == 0 || m_manager == 0 ) {
        return;
    }
    drawValues();
    drawPlot( *m_project, *m_manager );
}

void PerformanceStatusBase::drawValues()
{
    KLocale *locale = KGlobal::locale();
    const EffortCostMap &budget = m_model.bcwp();
    const EffortCostMap &actual = m_model.acwp();
    bcwsCost->setText( locale->formatMoney( budget.totalCost() ) );
    bcwpCost->setText( locale->formatMoney( budget.bcwpTotalCost() ) );
    acwpCost->setText( locale->formatMoney( actual.totalCost() ) );
    
    double cpi_ = 0.0;
    if ( actual.totalCost() > 0.0 ) {
        cpi_ = budget.bcwpTotalCost() / actual.totalCost();
    }
    cpi->setText( locale->formatNumber( cpi_ ) );
    
    double bh = budget.totalEffort().toDouble( Duration::Unit_h);
    bcwsEffort->setText( locale->formatNumber( bh ) );
    bcwpEffort->setText( locale->formatNumber( budget.bcwpTotalEffort() ) );
    acwpEffort->setText( locale->formatNumber( actual.totalEffort().toDouble( Duration::Unit_h) ) );

    double spi_ = 0.0;
    if ( bh > 0.0 ) {
        spi_ = budget.bcwpTotalEffort() / bh;
    }
    spi->setText( locale->formatNumber( spi_ ) );
}

void PerformanceStatusBase::drawPlot( Project &p, ScheduleManager &sm ) 
{
    kDebug();
    plotwidget->resetPlot();
    int axisCount = m_model.axisCount();
    for ( int i = 0; i < axisCount; ++i ) {
        ChartAxisIndex ai = m_model.axisIndex( i );
        if ( ! ai.isValid() ) {
            continue;
        }
        drawAxis( ai );
        drawData( ai );
    }
   
}

void PerformanceStatusBase::drawAxis( const ChartAxisIndex &idx ) 
{
    kDebug();
    int axisCount = m_model.axisCount( idx );
    QList<double> range;
    for ( int i = 0; i < axisCount; ++i ) {
        ChartAxisIndex ai = m_model.axisIndex( i, idx );
        if ( ! ai.isValid() ) {
            continue;
        }
        if ( m_model.hasAxisChildren( ai ) ) {
            kDebug()<<"multiple axis";
        } else {
            range << m_model.axisData( ai, AbstractChartModel::AxisMinRole ).toDouble();
            range << m_model.axisData( ai, AbstractChartModel::AxisMaxRole ).toDouble();
        }
    }
    plotwidget->setLimits( range[0], range[1], range[2], range[3] );
}

void PerformanceStatusBase::drawData( const ChartAxisIndex &axisSet ) 
{
    kDebug();
    int dataCount = m_model.dataSetCount( axisSet );
    for ( int i = 0; i < dataCount; ++i ) {
        kDebug()<<"create data index";
        ChartDataIndex di = m_model.index( i, axisSet );
        kDebug()<<"created data index:"<<di.number()<<di.userData;
        if ( ! di.isValid() ) {
            kDebug()<<"Invalid index";
            continue;
        }
        if ( m_model.hasChildren( di ) ) {
            kDebug()<<"sections";
            int c = m_model.childCount( di );
            for ( int ii = 0; ii < c; ++ii ) {
                ChartDataIndex cidx = m_model.index( ii, di );
                drawData( cidx, axisSet );
            }
        } else {
            kDebug()<<"no sections, go direct to data";
            drawData( di, axisSet );
        }
    }
}

void PerformanceStatusBase::drawData( const ChartDataIndex &index, const ChartAxisIndex &axisSet ) 
{
    kDebug()<<index.number()<<index.userData;
    QVariantList data;
    int axisCount = m_model.axisCount( axisSet );
    for ( int j = 0; j < axisCount; ++j ) {
        ChartAxisIndex axis = m_model.axisIndex( j, axisSet );
        if ( m_model.hasAxisChildren( axis ) ) {
            kDebug()<<"multiple axis";
        } else {
            data << m_model.data( index, axis );
        }
    }
    kDebug()<<data;
    Q_ASSERT( data.count() == 2 );
    QVariantList x = data[0].toList();
    QVariantList y = data[1].toList();
    QVariant color = m_model.data( index, Qt::ForegroundRole );
    KPlotObject *kpo = new KPlotObject( color.value<QColor>(), KPlotObject::Lines, 4 );
    for (int i = 0; i < y.count(); ++i ) {
        kDebug()<<"Add point:"<<x[i].toInt() << y[i].toDouble();
        kpo->addPoint( x[i].toInt(), y[i].toDouble() );
    }
    plotwidget->addPlotObject( kpo );
}


//-----------------------------------
PerformanceStatusView::PerformanceStatusView( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent ),
    m_project( 0 ),
    m_manager( 0 )
{
    kDebug()<<"-------------------- creating PerformanceStatusView -------------------";
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new PerformanceStatusBase( this );
    l->addWidget( m_view );
    setupGui();

}

void PerformanceStatusView::setScheduleManager( ScheduleManager *sm )
{
    //kDebug();
    m_manager = sm;
    m_view->setScheduleManager( sm );
    draw();
}

void PerformanceStatusView::setProject( Project *project )
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
    m_view->setProject( project );
}

void PerformanceStatusView::slotUpdate()
{
    draw();
}

void PerformanceStatusView::slotUpdate( ScheduleManager *sm )
{
    if ( sm == m_manager ) {
        slotUpdate();
    }
}

void PerformanceStatusView::draw()
{
    m_view->draw();
}

void PerformanceStatusView::setGuiActive( bool activate )
{
    kDebug()<<activate;
//    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
}

void PerformanceStatusView::setupGui()
{
    // Add the context menu actions for the view options
/*    actionOptions = new KAction(KIcon("configure"), i18n("Configure..."), this);
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );*/
}

void PerformanceStatusView::slotSplitView()
{
    kDebug();
}


void PerformanceStatusView::slotOptions()
{
    kDebug();
}

bool PerformanceStatusView::loadContext( const KoXmlElement &context )
{
    kDebug();
    return true;
}

void PerformanceStatusView::saveContext( QDomElement &context ) const
{
}


} // namespace KPlato

#include "kpttaskstatusview.moc"
