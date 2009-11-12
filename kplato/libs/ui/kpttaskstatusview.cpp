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
#include <QItemSelection>

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

#include "plotting/kplotwidget.h"
#include "plotting/kplotobject.h"
#include "plotting/kplotaxis.h"

#include <kplotpoint.h>

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
}

void TaskStatusView::slotRefreshView()
{
    model()->refresh();
}

void TaskStatusView::slotOptions()
{
    kDebug();
    TaskStatusViewSettingsDialog *dlg = new TaskStatusViewSettingsDialog( m_view, this );
    dlg->exec();
    delete dlg;
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

void ProjectStatusView::draw()
{
    m_view->draw();
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
    dlg->exec();
    delete dlg;
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

    plotwidget->setAntialiasing(false);
    plotwidget->setShowGrid(false);
    connect( &m_model, SIGNAL( reset() ), SLOT( slotReset() ) );
    
    setContextMenuPolicy ( Qt::DefaultContextMenu );

}

void PerformanceStatusBase::contextMenuEvent( QContextMenuEvent *event )
{
    kDebug()<<event->globalPos();
    emit customContextMenuRequested( event->globalPos() );
}

void PerformanceStatusBase::slotReset()
{
    //kDebug();
    draw();
}

void PerformanceStatusBase::slotUpdate()
{
    //kDebug();
    drawValues();
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
        disconnect( m_project, SIGNAL( localeChanged() ), this, SLOT( slotUpdate() ) );
    }
    m_project = project;
    m_model.setProject( project );
    if ( m_project ) {
        connect( m_project, SIGNAL( localeChanged() ), this, SLOT( slotUpdate() ) );
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
    if ( m_project == 0 ) {
        return;
    }
    KLocale *locale = m_project->locale();
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
    //kDebug();
    plotwidget->resetPlot();
    int axisCount = m_model.axisCount();
    for ( int i = 0; i < axisCount; ++i ) {
        kDebug()<<i<<"of"<<axisCount;
        ChartAxisIndex ai = m_model.axisIndex( i );
        if ( ! ai.isValid() ) {
            continue;
        }
        QList<QPointF> range = drawAxis( ai );
        kDebug()<<i<<range;
        if ( i == 0 ) {
            plotwidget->setLimits( range[0].x(), range[0].y(), range[1].x(), range[1].y()  );
            //kDebug()<<"Primary:"<<plotwidget->dataRect();
        }
        if ( i == 1 ) {
            plotwidget->setSecondaryLimits( range[0].x(), range[0].y(), range[1].x(), range[1].y() );
            //kDebug()<<"Secondary:"<<plotwidget->secondaryDataRect();
        }
        drawData( ai );
    }
   
}

QList<QPointF> PerformanceStatusBase::drawAxis( const ChartAxisIndex &idx ) 
{
    //kDebug();
    int axisCount = m_model.axisCount( idx );
    QList<QPointF> range;
    for ( int i = 0; i < axisCount; ++i ) {
        ChartAxisIndex ai = m_model.axisIndex( i, idx );
        if ( ! ai.isValid() ) {
            continue;
        }
        int type = m_model.axisData( ai, AbstractChartModel::AxisTypeRole ).toInt();
        if ( type == AbstractChartModel::Axis_X ) {
            //kDebug()<<"add x-axis";
            range.prepend ( QPointF ( m_model.axisData( ai, AbstractChartModel::AxisMinRole ).toDouble(), m_model.axisData( ai, AbstractChartModel::AxisMaxRole ).toDouble() ) );
            if ( idx.number() == 0 ) {
                plotwidget->axis( KPlotWidget::BottomAxis )->setLabel( m_model.axisData( ai, Qt::DisplayRole ).toString() );
                
                plotwidget->axis( KPlotWidget::BottomAxis )->setStartDate( m_model.axisData( ai, AbstractChartModel::AxisStartDateRole ).toDate() );
                plotwidget->axis( KPlotWidget::BottomAxis )->setTickLabelFormat( 'd' );
            } else {
                plotwidget->axis( KPlotWidget::TopAxis )->setLabel( m_model.axisData( ai, Qt::DisplayRole ).toString() );
                
                plotwidget->axis( KPlotWidget::TopAxis )->setStartDate( m_model.axisData( ai, AbstractChartModel::AxisStartDateRole ).toDate() );
                plotwidget->axis( KPlotWidget::TopAxis )->setTickLabelFormat( 'w' );
                
                plotwidget->axis( KPlotWidget::TopAxis )->setTickLabelsShown( true );
            }
        } else if ( type == AbstractChartModel::Axis_Y ) {
            //kDebug()<<"add y-axis"<<idx<<ai;
            range.append ( QPointF ( m_model.axisData( ai, AbstractChartModel::AxisMinRole ).toDouble(), m_model.axisData( ai, AbstractChartModel::AxisMaxRole ).toDouble() ) );
            if ( idx.number() == 0 ) {
                QString s = m_model.axisData( ai, Qt::DisplayRole ).toString();
                //kDebug()<<"LeftAxis:"<<s;
                plotwidget->axis( KPlotWidget::LeftAxis )->setLabel( s );
                
                plotwidget->axis( KPlotWidget::LeftAxis )->setStartDate( m_model.axisData( ai, AbstractChartModel::AxisStartDateRole ).toDate() );
            } else {
                QString s = m_model.axisData( ai, Qt::DisplayRole ).toString();
                //kDebug()<<"RightAxis:"<<s;
                plotwidget->axis( KPlotWidget::RightAxis )->setLabel( s );
            
                plotwidget->axis( KPlotWidget::RightAxis )->setStartDate( m_model.axisData( ai, AbstractChartModel::AxisStartDateRole ).toDate() );
            
                plotwidget->axis( KPlotWidget::RightAxis )->setTickLabelsShown( true );
            }
        }
        //kDebug()<<range;
    }
    return range;
}

void PerformanceStatusBase::drawData( const ChartAxisIndex &axisSet ) 
{
    //kDebug()<<axisSet;
    int dataCount = m_model.dataSetCount( axisSet );
    for ( int i = 0; i < dataCount; ++i ) {
        ChartDataIndex di = m_model.index( i, axisSet );
        if ( ! di.isValid() ) {
            //kDebug()<<"Invalid index"<<di;
            continue;
        }
        if ( m_model.hasChildren( di ) ) {
            //kDebug()<<"sections";
            int c = m_model.childCount( di );
            for ( int ii = 0; ii < c; ++ii ) {
                ChartDataIndex cidx = m_model.index( ii, di );
                drawData( cidx, axisSet );
            }
        } else {
            //kDebug()<<"no sections, go direct to data"<<di;
            drawData( di, axisSet );
        }
    }
}

void PerformanceStatusBase::drawData( const ChartDataIndex &index, const ChartAxisIndex &axisSet )
{
    //kDebug()<<index;
    QVariantList data;
    int axisCount = m_model.axisCount( axisSet );
    for ( int i = 0; i < axisCount; ++i ) {
        ChartAxisIndex axis = m_model.axisIndex( i, axisSet );
        //kDebug()<<"data axis"<<axis<<" set="<<axisSet;
        data << m_model.data( index, axis );
    }
    //kDebug()<<data;
    QVariantList xlst = data[0].toList();
    QVariantList ylst = data[1].toList();
    QVariant color = m_model.data( index, axisSet, Qt::ForegroundRole );
    QVariant label = m_model.data( index, axisSet, AbstractChartModel::DataLabelRole );
    KPlotObject *kpo = new KPlotObject( color.value<QColor>(), KPlotObject::Lines, 4 );
    double factor = plotwidget->dataRect().height() / plotwidget->secondaryDataRect().height();
    for (int i = 0; i < ylst.count(); ++i ) {
        //kDebug()<<"Add point:"<<x[i].toInt() << y[i].toDouble();
        double y = ylst[i].toDouble();
        if ( axisSet.number() == 1 ) {
            y *= factor;
        }
        KPlotPoint *p = new KPlotPoint( xlst[i].toInt(), y );
        if ( label.isValid() && i == ylst.count() / 2 ) {
            p->setLabel( label.toString() );
        }
        kpo->addPoint( p );
    }
    plotwidget->addPlotObject( kpo );
}

bool PerformanceStatusBase::loadContext( const KoXmlElement &context )
{
    kDebug();
    NodeChartModel::DataShown show;
    show.showCost = context.attribute( "show-cost", "1" ).toInt();
    show.showBCWSCost = context.attribute( "show-bcws-cost", "1" ).toInt();
    show.showBCWPCost = context.attribute( "show-bcwp-cost", "1" ).toInt();
    show.showACWPCost = context.attribute( "show-acwp-cost", "1" ).toInt();
    
    show.showEffort = context.attribute( "show-effort", "1" ).toInt();
    show.showBCWSEffort = context.attribute( "show-bcws-effort", "1" ).toInt();
    show.showBCWPEffort = context.attribute( "show-bcwp-effort", "1" ).toInt();
    show.showACWPEffort = context.attribute( "show-acwp-effort", "1" ).toInt();
    model()->setDataShown( show );
    return true;
}

void PerformanceStatusBase::saveContext( QDomElement &context ) const
{
    NodeChartModel::DataShown show = model()->dataShown();
    context.setAttribute( "show-cost", show.showCost );
    context.setAttribute( "show-bcws-cost", show.showBCWSCost );
    context.setAttribute( "show-bcwp-cost", show.showBCWPCost );
    context.setAttribute( "show-acwp-cost", show.showACWPCost );
    
    context.setAttribute( "show-effort",  show.showEffort );
    context.setAttribute( "show-bcws-effort", show.showBCWSEffort );
    context.setAttribute( "show-bcwp-effort", show.showBCWPEffort );
    context.setAttribute( "show-acwp-effort", show.showACWPEffort );
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

void PerformanceStatusTreeView::draw()
{
    m_chart->draw();
}

NodeItemModel *PerformanceStatusTreeView::nodeModel() const
{
    return static_cast<NodeItemModel*>( m_tree->model() );
}

void PerformanceStatusTreeView::setScheduleManager( ScheduleManager *sm )
{
    nodeModel()->setManager( sm );
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
    createOptionAction();
}

void PerformanceStatusView::slotOptions()
{
    kDebug();
    PerformanceStatusViewSettingsDialog *dlg = new PerformanceStatusViewSettingsDialog( m_view, this );
    dlg->exec();
    delete dlg;
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
    
    NodeChartModel::DataShown show = m_view->dataShown();

    ui_bcwsCost->setCheckState( show.showBCWSCost ? Qt::Checked : Qt::Unchecked );
    ui_bcwpCost->setCheckState( show.showBCWPCost ? Qt::Checked : Qt::Unchecked );
    ui_acwpCost->setCheckState( show.showACWPCost ? Qt::Checked : Qt::Unchecked );
    ui_cost->setChecked( show.showCost );

    ui_bcwsEffort->setCheckState( show.showBCWSEffort ? Qt::Checked : Qt::Unchecked );
    ui_bcwpEffort->setCheckState( show.showBCWPEffort ? Qt::Checked : Qt::Unchecked );
    ui_acwpEffort->setCheckState( show.showACWPEffort ? Qt::Checked : Qt::Unchecked );
    ui_effort->setChecked( show.showEffort );
}

void PerformanceStatusViewSettingsPanel::slotOk()
{
    NodeChartModel::DataShown show;
    show.showBCWSCost = ui_bcwsCost->checkState() == Qt::Unchecked ? false : true;
    show.showBCWPCost = ui_bcwpCost->checkState() == Qt::Unchecked ? false : true;
    show.showACWPCost = ui_acwpCost->checkState() == Qt::Unchecked ? false : true;
    show.showCost = ui_cost->isChecked();

    show.showBCWSEffort = ui_bcwsEffort->checkState() == Qt::Unchecked ? false : true;
    show.showBCWPEffort = ui_bcwpEffort->checkState() == Qt::Unchecked ? false : true;
    show.showACWPEffort = ui_acwpEffort->checkState() == Qt::Unchecked ? false : true;
    show.showEffort = ui_effort->isChecked();
    m_view->setDataShown( show );
}

void PerformanceStatusViewSettingsPanel::setDefault()
{
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
