/* This file is part of the KDE project
  Copyright (C) 2007 - 2010 Dag Andersen <danders@get2net.dk>

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
#include "kptcommonstrings.h"
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
#include <QItemSelection>
#include <QApplication>
#include <QResizeEvent>

#include <kicon.h>
#include <kaction.h>
#include <kglobal.h>
#include <klocale.h>
#include <kcalendarsystem.h>
#include <ktoggleaction.h>
#include <kactionmenu.h>
#include <kstandardaction.h>
#include <kstandardshortcut.h>
#include <kaccelgen.h>
#include <kactioncollection.h>

#include "KDChartChart"
#include "KDChartAbstractCoordinatePlane"
#include "KDChartBarDiagram"
#include "KDChartLineDiagram"
#include "KDChartCartesianAxis"
#include "KDChartCartesianCoordinatePlane"
#include "KDChartLegend"
#include "KDChartBackgroundAttributes"
#include "KDChartGridAttributes"

using namespace KDChart;

namespace KPlato
{


TaskStatusTreeView::TaskStatusTreeView( QWidget *parent )
    : DoubleTreeViewBase( parent )
{
    setContextMenuPolicy( Qt::CustomContextMenu );
    TaskStatusItemModel *m = new TaskStatusItemModel( this );
    setModel( m );
    //setSelectionBehavior( QAbstractItemView::SelectItems );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    setStretchLastSection( false );

    createItemDelegates( m );

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
            << NodeModel::NodeActualStart
            << NodeModel::NodeActualFinish
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

void TaskStatusTreeView::dragMoveEvent(QDragMoveEvent */*event*/)
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

void TaskStatusView::updateReadWrite( bool rw )
{
    m_view->setReadWrite( rw );
}

void TaskStatusView::setScheduleManager( ScheduleManager *sm )
{
    //kDebug();
    static_cast<TaskStatusItemModel*>( m_view->model() )->setScheduleManager( sm );
}

Node *TaskStatusView::currentNode() const
{
    return m_view->model()->node( m_view->selectionModel()->currentIndex() );
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
        slotHeaderContextMenuRequested( pos );
        return;
    }
    Node *node = m_view->model()->node( index );
    if ( node == 0 ) {
        slotHeaderContextMenuRequested( pos );
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
            name = "taskview_milestone_popup";
            break;
        case Node::Type_Summarytask:
            name = "taskview_summary_popup";
            break;
        default:
            break;
    }
    kDebug()<<name;
    if ( name.isEmpty() ) {
        slotHeaderContextMenuRequested( pos );
        return;
    }
    emit requestPopupMenu( name, pos );
}

void TaskStatusView::setupGui()
{
    // Add the context menu actions for the view options
    connect(m_view->actionSplitView(), SIGNAL(triggered(bool) ), SLOT(slotSplitView()));
    addContextAction( m_view->actionSplitView() );

    createOptionAction();
}

void TaskStatusView::slotSplitView()
{
    kDebug();
    m_view->setViewSplitMode( ! m_view->isViewSplit() );
    emit optionsModified();
}

void TaskStatusView::slotRefreshView()
{
    model()->refresh();
}

void TaskStatusView::slotOptions()
{
    kDebug();
    TaskStatusViewSettingsDialog *dlg = new TaskStatusViewSettingsDialog( m_view, this );
    connect(dlg, SIGNAL(finished(int)), SLOT(slotOptionsFinished(int)));
    dlg->show();
    dlg->raise();
    dlg->activateWindow();
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

KoPrintJob *TaskStatusView::createPrintJob()
{
    return m_view->createPrintJob( this );
}


//------------------------------------------------
TaskStatusViewSettingsPanel::TaskStatusViewSettingsPanel( TaskStatusTreeView *view, QWidget *parent )
    : QWidget( parent ),
    m_view( view )
{
    setupUi( this );

    QStringList lst;
    for ( int i = 1; i <= 7; ++i ) {
        lst << KGlobal::locale()->calendar()->weekDayName( i );
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
    m_project( 0 )
{
    kDebug()<<"-------------------- creating ProjectStatusView -------------------";
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new PerformanceStatusBase( this );
    l->addWidget( m_view );

    setupGui();

    connect( m_view, SIGNAL( customContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );


}

void ProjectStatusView::setScheduleManager( ScheduleManager *sm )
{
    //kDebug();
    m_view->setScheduleManager( sm );
    m_view->model()->clearNodes();
    if ( m_project ) {
        m_view->model()->addNode( m_project );
    }
}

void ProjectStatusView::setProject( Project *project )
{
    m_project = project;
    m_view->model()->clearNodes();
    m_view->setProject( project );
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
    createOptionAction();
}

void ProjectStatusView::slotOptions()
{
    kDebug();
    ProjectStatusViewSettingsDialog *dlg = new ProjectStatusViewSettingsDialog( m_view, this );
    connect(dlg, SIGNAL(finished(int)), SLOT(slotOptionsFinished(int)));
    dlg->show();
    dlg->raise();
    dlg->activateWindow();
}

bool ProjectStatusView::loadContext( const KoXmlElement &context )
{
    kDebug();
    return m_view->loadContext( context );
}

void ProjectStatusView::saveContext( QDomElement &context ) const
{
    m_view->saveContext( context );
}

//-----------------------------------
PerformanceStatusBase::PerformanceStatusBase( QWidget *parent )
    : QWidget( parent ),
    m_project( 0 ),
    m_manager( 0 )
{
    setupUi( this );

    labelBCWS->setToolTip( ToolTip::nodeBCWS() );
    labelBCWP->setToolTip( ToolTip::nodeBCWP() );
    labelACWP->setToolTip( ToolTip::nodeACWP() );
    labelPI->setToolTip(  i18nc( "@info:tooltip", "Performance indexes" ) );
    labelCPI->setToolTip( i18nc( "@info:tooltip", "Cost performance index (BCWP/ACWP)" ) );
    labelSPI->setToolTip( ToolTip::nodePerformanceIndex() );

    BackgroundAttributes backgroundAttrs( ui_chart->backgroundAttributes() );
    backgroundAttrs.setVisible( true );
    backgroundAttrs.setBrush( Qt::white );
    ui_chart->setBackgroundAttributes( backgroundAttrs );

    m_legend = new Legend( ui_chart );
    ui_chart->replaceLegend( m_legend );
    m_legend->setObjectName( "Chart legend" );

    backgroundAttrs = m_legend->backgroundAttributes();
    m_legend->setBackgroundAttributes( backgroundAttrs );
    backgroundAttrs.setVisible( true );
    backgroundAttrs.setBrush( Qt::white );

    m_legend->setPosition( Position::East );
    //m_legend->setAlignment( (Qt::Alignment)(Qt::AlignTop | Qt::AlignCenter) );
    m_legenddiagram.setModel( &m_chartmodel );
    m_legenddiagram.setObjectName( "Legend diagram" );
    m_legend->setDiagram( &m_legenddiagram );
    m_legend->setBrushesFromDiagram( &m_legenddiagram );
    kDebug()<<m_legend->brushes();

    // get rid of the default coordinate plane
    AbstractCoordinatePlane *p = ui_chart->coordinatePlane();
    ui_chart->takeCoordinatePlane( p );
    delete p;

    createBarChart();
    createLineChart();
    setupChart();

    connect(&m_chartmodel, SIGNAL(modelReset()), SLOT(slotUpdate()));
    setContextMenuPolicy ( Qt::DefaultContextMenu );
}

void PerformanceStatusBase::setChartInfo( const PerformanceChartInfo &info )
{
    if ( info != m_chartinfo ) {
        m_chartinfo = info;
        setupChart();
    }
}

void PerformanceStatusBase::refreshChart()
{
    // NOTE: Force grid/axis recalculation, couldn't find a better way :(
    QResizeEvent event( ui_chart->size(), QSize() );
    QApplication::sendEvent( ui_chart, &event );
}

void PerformanceStatusBase::createBarChart()
{
    m_barchart.effortplane = new CartesianCoordinatePlane( ui_chart );
    m_barchart.effortplane->setObjectName( "Bar chart, Effort" );
    m_barchart.costplane = new CartesianCoordinatePlane( ui_chart );
    m_barchart.costplane->setReferenceCoordinatePlane( m_barchart.effortplane );
    m_barchart.effortplane->setObjectName( "Bar chart, Cost" );

    BarDiagram *effortdiagram = new BarDiagram( ui_chart, m_barchart.effortplane );
    effortdiagram->setObjectName( "Effort diagram" );

    m_barchart.dateaxis = new CartesianAxis();
    m_barchart.dateaxis->setPosition( CartesianAxis::Bottom );

    m_barchart.effortaxis = new CartesianAxis( effortdiagram );
    m_barchart.effortaxis->setPosition( CartesianAxis::Right );
    effortdiagram->addAxis( m_barchart.effortaxis );
    m_barchart.effortplane->addDiagram( effortdiagram );

    // Hide cost in effort diagram
    effortdiagram->setHidden( 0, true );
    effortdiagram->setHidden( 1, true );
    effortdiagram->setHidden( 2, true );
    m_barchart.effortproxy.setZeroColumns( QList<int>() << 0 << 1 << 2 );

    m_barchart.effortproxy.setSourceModel( &m_chartmodel );
    effortdiagram->setModel( &(m_barchart.effortproxy) );

    BarDiagram *costdiagram = new BarDiagram( ui_chart, m_barchart.costplane );
    costdiagram->setObjectName( "Cost diagram" );

    m_barchart.costaxis = new CartesianAxis( costdiagram );
    m_barchart.costaxis->setPosition( CartesianAxis::Left );
    costdiagram->addAxis( m_barchart.costaxis );
    m_barchart.costplane->addDiagram( costdiagram );

    // Hide effort in cost diagram
    costdiagram->setHidden( 3, true );
    costdiagram->setHidden( 4, true );
    costdiagram->setHidden( 5, true );
    m_barchart.costproxy.setZeroColumns( QList<int>() << 3 << 4 << 5 );

    m_barchart.costproxy.setSourceModel( &m_chartmodel );
    costdiagram->setModel( &(m_barchart.costproxy) );
}

void PerformanceStatusBase::createLineChart()
{
    m_linechart.effortplane = new CartesianCoordinatePlane( ui_chart );
    m_linechart.effortplane->setObjectName( "Line chart, Effort" );
    m_linechart.effortplane->setRubberBandZoomingEnabled( true );
    m_linechart.costplane = new CartesianCoordinatePlane( ui_chart );
    m_linechart.costplane->setReferenceCoordinatePlane( m_linechart.effortplane );
    m_linechart.costplane->setObjectName( "Line chart, Cost" );
    m_linechart.costplane->setRubberBandZoomingEnabled( true );

    LineDiagram *effortdiagram = new LineDiagram( ui_chart, m_linechart.effortplane );
    effortdiagram->setObjectName( "Effort diagram" );

    m_linechart.dateaxis = new CartesianAxis();
    m_linechart.dateaxis->setPosition( CartesianAxis::Bottom );

    m_linechart.effortaxis = new CartesianAxis( effortdiagram );
    m_linechart.effortaxis->setPosition( CartesianAxis::Right );
    effortdiagram->addAxis( m_linechart.effortaxis );
    m_linechart.effortplane->addDiagram( effortdiagram );

    // Hide cost in effort diagram
    effortdiagram->setHidden( 0, true );
    effortdiagram->setHidden( 1, true );
    effortdiagram->setHidden( 2, true );
    m_linechart.effortproxy.setZeroColumns( QList<int>() << 0 << 1 << 2 );

    m_linechart.effortproxy.setSourceModel( &m_chartmodel );
    effortdiagram->setModel( &(m_linechart.effortproxy) );

    LineDiagram *costdiagram = new LineDiagram( ui_chart, m_linechart.costplane );
    costdiagram->setObjectName( "Cost diagram" );

    m_linechart.costaxis = new CartesianAxis( costdiagram );
    m_linechart.costaxis->setPosition( CartesianAxis::Left );
    costdiagram->addAxis( m_linechart.costaxis );
    m_linechart.costplane->addDiagram( costdiagram );
    // Hide effort in cost diagram
    costdiagram->setHidden( 3, true );
    costdiagram->setHidden( 4, true );
    costdiagram->setHidden( 5, true );

    m_linechart.costproxy.setZeroColumns( QList<int>() << 3 << 4 << 5 );
    m_linechart.costproxy.setSourceModel( &m_chartmodel );
    costdiagram->setModel( &(m_linechart.costproxy) );


}

void PerformanceStatusBase::setupChart()
{
    while ( ! ui_chart->coordinatePlanes().isEmpty() ) {
        ui_chart->takeCoordinatePlane( ui_chart->coordinatePlanes().last() );
    }
    if ( m_chartinfo.showBarChart ) {
        setupChart( m_barchart );
    } else if ( m_chartinfo.showLineChart ) {
        setupChart( m_linechart );
    } else {
        setupChart( m_linechart );
    }
    kDebug()<<"Planes:"<<ui_chart->coordinatePlanes();
    foreach ( AbstractCoordinatePlane *pl, ui_chart->coordinatePlanes() ) {
        CartesianCoordinatePlane *p = dynamic_cast<CartesianCoordinatePlane*>( pl );
        if ( p == 0 ) continue;
        GridAttributes ga = p->globalGridAttributes();
        ga.setGridVisible( p->referenceCoordinatePlane() == 0 );
        p->setGlobalGridAttributes( ga );
    }
    m_legend->setDatasetHidden( 0, ! ( m_chartinfo.showCost && m_chartinfo.showBCWSCost ) );
    m_legend->setDatasetHidden( 1, ! ( m_chartinfo.showCost && m_chartinfo.showBCWPCost ) );
    m_legend->setDatasetHidden( 2, ! ( m_chartinfo.showCost && m_chartinfo.showACWPCost ) );
    m_legend->setDatasetHidden( 3, ! ( m_chartinfo.showEffort && m_chartinfo.showBCWSEffort ) );
    m_legend->setDatasetHidden( 4, ! ( m_chartinfo.showEffort && m_chartinfo.showBCWPEffort ) );
    m_legend->setDatasetHidden( 5, ! ( m_chartinfo.showEffort && m_chartinfo.showACWPEffort ) );
    
    refreshChart();
}

void PerformanceStatusBase::setupChart( ChartContents &cc )
{
    const PerformanceChartInfo &info = m_chartinfo;
    kDebug()<<"cost="<<info.showCost<<"effort="<<info.showEffort;
    static_cast<AbstractCartesianDiagram*>( cc.effortplane->diagram() )->takeAxis( cc.dateaxis );
    static_cast<AbstractCartesianDiagram*>( cc.costplane->diagram() )->takeAxis( cc.dateaxis );
    if ( info.showEffort ) {
        // column 0, 1 and 2 is reserved for cost if cost is shown
        cc.effortplane->diagram()->setHidden( 0, false );
        cc.effortplane->diagram()->setHidden( 1, false );
        cc.effortplane->diagram()->setHidden( 2, false );
        // filter cost columns if cost is *not* shown
        cc.effortproxy.setRejectColumns( info.showCost ? QList<int>() : QList<int>() << 0 << 1 << 2 );
        // if cost is shown don't return a cost value or else it goes into the effort axis scale calculation
        cc.effortproxy.setZeroColumns( info.showCost ? QList<int>() << 0 << 1 << 2 : QList<int>()  );
        cc.effortaxis->setPosition( info.showCost ? CartesianAxis::Right : CartesianAxis::Left );
        ui_chart->addCoordinatePlane( cc.effortplane );

        static_cast<AbstractCartesianDiagram*>( cc.effortplane->diagram() )->addAxis( cc.dateaxis );
        cc.effortplane->setGridNeedsRecalculate();

        // If cost is shown, effort is column 3, 4, 5, else 0, 1, 2
        int col = info.showCost ? 3 : 0;
        cc.effortplane->diagram()->setHidden( col, ! info.showBCWSEffort );
        cc.effortplane->diagram()->setHidden( ++col, ! info.showBCWPEffort );
        cc.effortplane->diagram()->setHidden( ++col, ! info.showACWPEffort );
    }
    if ( info.showCost ) {
        // remove effort columns from cost if no effort is shown
        cc.costproxy.setRejectColumns( info.showEffort ? QList<int>() : QList<int>() << 3 << 4 << 5 );
        // Should never get any effort values in cost diagram
        cc.costproxy.setZeroColumns( QList<int>() << 3 << 4 << 5 );
        cc.costplane->setReferenceCoordinatePlane( info.showEffort ? cc.effortplane : 0 );
        ui_chart->addCoordinatePlane( cc.costplane );

        static_cast<AbstractCartesianDiagram*>( cc.costplane->diagram() )->addAxis( cc.dateaxis );
        cc.costplane->setGridNeedsRecalculate();

        cc.costplane->diagram()->setHidden( 0, ! info.showBCWSCost );
        cc.costplane->diagram()->setHidden( 1, ! info.showBCWPCost );
        cc.costplane->diagram()->setHidden( 2, ! info.showACWPCost );
        cc.costplane->diagram()->setHidden( 3, true );
        cc.costplane->diagram()->setHidden( 4, true );
        cc.costplane->diagram()->setHidden( 5, true );
    }
    if ( info.showEffort ) {
        cc.effortproxy.reset();
    }
    if ( info.showCost ) {
        cc.costaxis->setCachedSizeDirty();
        cc.costproxy.reset();
    }
#if 0
    kDebug()<<"Effort:"<<info.showEffort<<"reject="<<cc.effortproxy.rejectColumns()<<"zero="<<cc.effortproxy.zeroColumns()
    <<endl<<0<<(cc.effortplane->diagram()->isHidden(0)?"hide":"show")<<cc.effortproxy.headerData(0,Qt::Horizontal).toString()
    <<endl<<1<<(cc.effortplane->diagram()->isHidden(1)?"hide":"show")<<cc.effortproxy.headerData(1,Qt::Horizontal).toString()
    <<endl<<2<<(cc.effortplane->diagram()->isHidden(2)?"hide":"show")<<cc.effortproxy.headerData(2,Qt::Horizontal).toString()
    <<endl<<3<<(cc.effortplane->diagram()->isHidden(3)?"hide":"show")<<cc.effortproxy.headerData(3,Qt::Horizontal).toString()
    <<endl<<4<<(cc.effortplane->diagram()->isHidden(4)?"hide":"show")<<cc.effortproxy.headerData(4,Qt::Horizontal).toString()
    <<endl<<5<<(cc.effortplane->diagram()->isHidden(5)?"hide":"show")<<cc.effortproxy.headerData(5,Qt::Horizontal).toString();

    kDebug()<<"Cost:"<<info.showCost<<"reject="<<cc.costproxy.rejectColumns()<<"zero="<<cc.costproxy.zeroColumns()
    <<endl<<0<<(cc.costplane->diagram()->isHidden(0)?"hide":"show")<<cc.costproxy.headerData(0,Qt::Horizontal).toString()
    <<endl<<1<<(cc.costplane->diagram()->isHidden(1)?"hide":"show")<<cc.costproxy.headerData(1,Qt::Horizontal).toString()
    <<endl<<2<<(cc.costplane->diagram()->isHidden(2)?"hide":"show")<<cc.costproxy.headerData(2,Qt::Horizontal).toString()
    <<endl<<3<<(cc.costplane->diagram()->isHidden(3)?"hide":"show")<<cc.costproxy.headerData(3,Qt::Horizontal).toString()
    <<endl<<4<<(cc.costplane->diagram()->isHidden(4)?"hide":"show")<<cc.costproxy.headerData(4,Qt::Horizontal).toString()
    <<endl<<5<<(cc.costplane->diagram()->isHidden(5)?"hide":"show")<<cc.costproxy.headerData(5,Qt::Horizontal).toString();
#endif
}

void PerformanceStatusBase::contextMenuEvent( QContextMenuEvent *event )
{
    kDebug()<<event->globalPos();
    emit customContextMenuRequested( event->globalPos() );
}

void PerformanceStatusBase::slotUpdate()
{
    //kDebug();
    drawValues();
    refreshChart();
}

void PerformanceStatusBase::setScheduleManager( ScheduleManager *sm )
{
    //kDebug();
    m_manager = sm;
    m_chartmodel.setScheduleManager( sm );
}

void PerformanceStatusBase::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( localeChanged() ), this, SLOT( slotLocaleChanged() ) );
    }
    m_project = project;
    if ( m_project ) {
        connect( m_project, SIGNAL( localeChanged() ), this, SLOT( slotLocaleChanged() ) );
    }
    m_chartmodel.setProject( project );

    slotLocaleChanged();

    m_legend->setBrushesFromDiagram( &m_legenddiagram );
    kDebug()<<"legend brushes set:"<<m_legend->brushes();
}

void PerformanceStatusBase::slotLocaleChanged()
{
    kDebug();
    KLocale *locale = m_project ? m_project->locale() : KGlobal::locale();
    if ( locale ) {
        m_linechart.costaxis->setTitleText( i18nc( "Chart axis title 1=currency symbol", "Cost (%1)", m_project->locale()->currencySymbol() ) );
        m_linechart.effortaxis->setTitleText( i18nc( "Chart axis title", "Effort (hours)" ) );

        m_barchart.costaxis->setTitleText( i18nc( "Chart axis title 1=currency symbol", "Cost (%1)", m_project->locale()->currencySymbol() ) );
        m_barchart.effortaxis->setTitleText( i18nc( "Chart axis title", "Effort (hours)" ) );
    }
    if ( m_project == 0 ) {
        return;
    }
    drawValues();
}

void PerformanceStatusBase::drawValues()
{
    if ( m_project == 0 ) {
        return;
    }
    KLocale *locale = m_project->locale();
    const EffortCostMap &budget = m_chartmodel.bcwp();
    const EffortCostMap &actual = m_chartmodel.acwp();
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


bool PerformanceStatusBase::loadContext( const KoXmlElement &context )
{
    kDebug();
    m_chartinfo.showBarChart = context.attribute( "show-bar-chart", "0" ).toInt();
    m_chartinfo.showLineChart = context.attribute( "show-line-chart", "1" ).toInt();

    m_chartinfo.showCost = context.attribute( "show-cost", "1" ).toInt();
    m_chartinfo.showBCWSCost = context.attribute( "show-bcws-cost", "1" ).toInt();
    m_chartinfo.showBCWPCost = context.attribute( "show-bcwp-cost", "1" ).toInt();
    m_chartinfo.showACWPCost = context.attribute( "show-acwp-cost", "1" ).toInt();

    m_chartinfo.showEffort = context.attribute( "show-effort", "1" ).toInt();
    m_chartinfo.showBCWSEffort = context.attribute( "show-bcws-effort", "1" ).toInt();
    m_chartinfo.showBCWPEffort = context.attribute( "show-bcwp-effort", "1" ).toInt();
    m_chartinfo.showACWPEffort = context.attribute( "show-acwp-effort", "1" ).toInt();
    kDebug()<<"Cost:"<<m_chartinfo.showCost<<"bcws="<<m_chartinfo.showBCWSCost<<"bcwp="<<m_chartinfo.showBCWPCost<<"acwp="<<m_chartinfo.showACWPCost;
    kDebug()<<"Effort:"<<m_chartinfo.showCost<<"bcws="<<m_chartinfo.showBCWSCost<<"bcwp="<<m_chartinfo.showBCWPCost<<"acwp="<<m_chartinfo.showACWPCost;
    setupChart();
    return true;
}

void PerformanceStatusBase::saveContext( QDomElement &context ) const
{
    context.setAttribute( "show-cost", m_chartinfo.showCost );
    context.setAttribute( "show-bcws-cost", m_chartinfo.showBCWSCost );
    context.setAttribute( "show-bcwp-cost", m_chartinfo.showBCWPCost );
    context.setAttribute( "show-acwp-cost", m_chartinfo.showACWPCost );

    context.setAttribute( "show-effort",  m_chartinfo.showEffort );
    context.setAttribute( "show-bcws-effort", m_chartinfo.showBCWSEffort );
    context.setAttribute( "show-bcwp-effort", m_chartinfo.showBCWPEffort );
    context.setAttribute( "show-acwp-effort", m_chartinfo.showACWPEffort );
}

//-----------------------------------
PerformanceStatusTreeView::PerformanceStatusTreeView( QWidget *parent )
    : QSplitter( parent )
{
    m_tree = new TreeViewBase( this );
    NodeItemModel *m = new NodeItemModel( m_tree );
    m_tree->setModel( m );
    QList<int> lst1; lst1 << 1 << -1; // only display column 0 (NodeName) in tree view
    m_tree->setDefaultColumns( QList<int>() << 0 );
    m_tree->setColumnsHidden( lst1 );
    m_tree->setSelectionMode( QAbstractItemView::ExtendedSelection );
    addWidget( m_tree );

    m_chart = new PerformanceStatusBase( this );
    addWidget( m_chart );

    connect( m_tree->selectionModel(), SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection & ) ), SLOT( slotSelectionChanged( const QItemSelection &, const QItemSelection & ) ) );
}

void PerformanceStatusTreeView::slotSelectionChanged( const QItemSelection&, const QItemSelection& )
{
    //kDebug();
    QList<Node*> nodes;
    foreach ( const QModelIndex &i, m_tree->selectionModel()->selectedIndexes() ) {
        Node *n = nodeModel()->node( i );
        if ( ! nodes.contains( n ) ) {
            nodes.append( n );
        }
    }
    m_chart->model()->setNodes( nodes );
}

NodeItemModel *PerformanceStatusTreeView::nodeModel() const
{
    return static_cast<NodeItemModel*>( m_tree->model() );
}

void PerformanceStatusTreeView::setScheduleManager( ScheduleManager *sm )
{
    nodeModel()->setScheduleManager( sm );
    m_chart->setScheduleManager( sm );
}

Project *PerformanceStatusTreeView::project() const
{
    return nodeModel()->project();
}

void PerformanceStatusTreeView::setProject( Project *project )
{
    nodeModel()->setProject( project );
    m_chart->setProject( project );
}

bool PerformanceStatusTreeView::loadContext( const KoXmlElement &context )
{
    kDebug();

    bool res = false;
    res = m_chart->loadContext( context.namedItem( "chart" ).toElement() );
    res &= m_tree->loadContext( nodeModel()->columnMap(), context.namedItem( "tree" ).toElement() );
    return res;
}

void PerformanceStatusTreeView::saveContext( QDomElement &context ) const
{
    QDomElement c = context.ownerDocument().createElement( "chart" );
    context.appendChild( c );
    m_chart->saveContext( c );

    QDomElement t = context.ownerDocument().createElement( "tree" );
    context.appendChild( t );
    m_tree->saveContext( nodeModel()->columnMap(), t );
}

//-----------------------------------
PerformanceStatusView::PerformanceStatusView( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent )
{
    kDebug()<<"-------------------- creating PerformanceStatusView -------------------";
    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new PerformanceStatusTreeView( this );
    l->addWidget( m_view );

    setupGui();

    connect( m_view->treeView(), SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
    connect( m_view->chartView(), SIGNAL( customContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );

    connect( m_view->treeView(), SIGNAL( contextMenuRequested( const QModelIndex&, const QPoint& ) ), SLOT( slotContextMenuRequested( const QModelIndex&, const QPoint& ) ) );
}

void PerformanceStatusView::slotContextMenuRequested( const QModelIndex &index, const QPoint& pos )
{
    kDebug()<<index<<pos;
    if ( ! index.isValid() ) {
        slotHeaderContextMenuRequested( pos );
        return;
    }
    Node *node = m_view->nodeModel()->node( index );
    if ( node == 0 ) {
        slotHeaderContextMenuRequested( pos );
        return;
    }
    slotContextMenuRequested( node, pos );
}

Node *PerformanceStatusView::currentNode() const
{
    return m_view->nodeModel()->node( m_view->treeView()->selectionModel()->currentIndex() );
}

void PerformanceStatusView::slotContextMenuRequested( Node *node, const QPoint& pos )
{
    kDebug()<<node->name()<<" :"<<pos;
    QString name;
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
    //kDebug()<<name;
    if ( name.isEmpty() ) {
        slotHeaderContextMenuRequested( pos );
        return;
    }
    emit requestPopupMenu( name, pos );
}


void PerformanceStatusView::setScheduleManager( ScheduleManager *sm )
{
    //kDebug();
    m_view->setScheduleManager( sm );
}

void PerformanceStatusView::setProject( Project *project )
{
    m_view->setProject( project );
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
    createOptionAction();
}

void PerformanceStatusView::slotOptions()
{
    kDebug();
    PerformanceStatusViewSettingsDialog *dlg = new PerformanceStatusViewSettingsDialog( m_view, this );
    connect(dlg, SIGNAL(finished(int)), SLOT(slotOptionsFinished(int)));
    dlg->show();
    dlg->raise();
    dlg->activateWindow();
}

bool PerformanceStatusView::loadContext( const KoXmlElement &context )
{
    kDebug();
    return m_view->loadContext( context );
}

void PerformanceStatusView::saveContext( QDomElement &context ) const
{
    m_view->saveContext( context );
}

//------------------------------------------------
PerformanceStatusViewSettingsPanel::PerformanceStatusViewSettingsPanel( PerformanceStatusBase *view, QWidget *parent )
    : QWidget( parent ),
    m_view( view )
{
    setupUi( this );

    PerformanceChartInfo info = m_view->chartInfo();

    ui_linechart->setChecked( info.showLineChart );
    ui_barchart->setChecked( info.showBarChart );

    ui_bcwsCost->setCheckState( info.showBCWSCost ? Qt::Checked : Qt::Unchecked );
    ui_bcwpCost->setCheckState( info.showBCWPCost ? Qt::Checked : Qt::Unchecked );
    ui_acwpCost->setCheckState( info.showACWPCost ? Qt::Checked : Qt::Unchecked );
    ui_cost->setChecked( info.showCost );

    ui_bcwsEffort->setCheckState( info.showBCWSEffort ? Qt::Checked : Qt::Unchecked );
    ui_bcwpEffort->setCheckState( info.showBCWPEffort ? Qt::Checked : Qt::Unchecked );
    ui_acwpEffort->setCheckState( info.showACWPEffort ? Qt::Checked : Qt::Unchecked );
    ui_effort->setChecked( info.showEffort );
}

void PerformanceStatusViewSettingsPanel::slotOk()
{
    PerformanceChartInfo info;
    info.showLineChart = ui_linechart->isChecked();
    info.showBarChart = ui_barchart->isChecked();

    info.showBCWSCost = ui_bcwsCost->checkState() == Qt::Unchecked ? false : true;
    info.showBCWPCost = ui_bcwpCost->checkState() == Qt::Unchecked ? false : true;
    info.showACWPCost = ui_acwpCost->checkState() == Qt::Unchecked ? false : true;
    info.showCost = ui_cost->isChecked();

    info.showBCWSEffort = ui_bcwsEffort->checkState() == Qt::Unchecked ? false : true;
    info.showBCWPEffort = ui_bcwpEffort->checkState() == Qt::Unchecked ? false : true;
    info.showACWPEffort = ui_acwpEffort->checkState() == Qt::Unchecked ? false : true;
    info.showEffort = ui_effort->isChecked();
    m_view->setChartInfo( info );
}

void PerformanceStatusViewSettingsPanel::setDefault()
{
    ui_linechart->setChecked( true );

    ui_bcwsCost->setCheckState( Qt::Checked );
    ui_bcwpCost->setCheckState( Qt::Checked );
    ui_acwpCost->setCheckState( Qt::Checked );
    ui_cost->setChecked( true );

    ui_bcwsEffort->setCheckState( Qt::Checked );
    ui_bcwpEffort->setCheckState( Qt::Checked );
    ui_acwpEffort->setCheckState( Qt::Checked );
    ui_effort->setChecked( Qt::Unchecked );
}

//-----------------
PerformanceStatusViewSettingsDialog::PerformanceStatusViewSettingsDialog( PerformanceStatusTreeView *view, QWidget *parent )
    : ItemViewSettupDialog( view->treeView(), true, parent )
{
    PerformanceStatusViewSettingsPanel *panel = new PerformanceStatusViewSettingsPanel( view->chartView(), this );
    KPageWidgetItem *page = insertWidget( 0, panel, i18n( "Chart" ), i18n( "Chart Settings" ) );
    setCurrentPage( page );
    //connect( panel, SIGNAL( changed( bool ) ), this, SLOT( enableButtonOk( bool ) ) );

    connect( this, SIGNAL( okClicked() ), panel, SLOT( slotOk() ) );
    connect( this, SIGNAL( defaultClicked() ), panel, SLOT( setDefault() ) );
}

//-----------------
ProjectStatusViewSettingsDialog::ProjectStatusViewSettingsDialog( PerformanceStatusBase *view, QWidget *parent )
    : KPageDialog( parent )
{
    PerformanceStatusViewSettingsPanel *panel = new PerformanceStatusViewSettingsPanel( view, this );
    KPageWidgetItem *page = new KPageWidgetItem( panel, i18n( "Chart" ) );
    page->setHeader( i18n( "Chart Settings" ) );
    addPage( page );

    //connect( panel, SIGNAL( changed( bool ) ), this, SLOT( enableButtonOk( bool ) ) );

    connect( this, SIGNAL( okClicked() ), panel, SLOT( slotOk() ) );
    connect( this, SIGNAL( defaultClicked() ), panel, SLOT( setDefault() ) );
}


} // namespace KPlato

#include "kpttaskstatusview.moc"
