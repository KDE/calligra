/* This file is part of the KDE project
  Copyright (C) 2002 - 2007, 2012 Dag Andersen <danders@get2net.dk>
  Copyright (C) 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>

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

#include "kptganttview.h"
#include "kptnodeitemmodel.h"
#include "kptappointment.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"
#include "kptdatetime.h"
#include "kptrelation.h"
#include "kptschedule.h"
#include "kptviewbase.h"
#include "kptitemviewsettup.h"
#include "kptduration.h"
#include "kptdatetime.h"
#include "kptresourceappointmentsmodel.h"
#include "kptdebug.h"

#include <kdganttproxymodel.h>
#include <kdganttconstraintmodel.h>
#include <kdganttconstraint.h>
#include <kdganttdatetimegrid.h>
#include <kdganttgraphicsview.h>
#include <kdgantttreeviewrowcontroller.h>

#include <KoDocument.h>
#include <KoPageLayoutWidget.h>

#include <kdebug.h>

#include <QSplitter>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDateTime>
#include <QMenu>
#include <QModelIndex>
#include <QPainter>
#include <QTabWidget>

#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include <kaction.h>
#include <KToggleAction>

#include "kdganttglobal.h"
#include "kdganttstyleoptionganttitem.h"


/// The main namespace
namespace KPlato
{

class GanttItemDelegate;

//-------------------------------------------------
GanttChartDisplayOptionsPanel::GanttChartDisplayOptionsPanel( GanttItemDelegate *delegate, QWidget *parent )
    : QWidget( parent ),
    m_delegate( delegate )
{
    setupUi( this );
    setValues( *delegate );

    connect( ui_showTaskName, SIGNAL(  stateChanged ( int ) ), SIGNAL( changed() ) );
    connect( ui_showResourceNames, SIGNAL(  stateChanged ( int ) ), SIGNAL( changed() ) );
    connect( ui_showDependencies, SIGNAL(  stateChanged ( int ) ), SIGNAL( changed() ) );
    connect( ui_showPositiveFloat, SIGNAL(  stateChanged ( int ) ), SIGNAL( changed() ) );
    connect( ui_showNegativeFloat, SIGNAL(  stateChanged ( int ) ), SIGNAL( changed() ) );
    connect( ui_showCriticalPath, SIGNAL(  stateChanged ( int ) ), SIGNAL( changed() ) );
    connect( ui_showCriticalTasks, SIGNAL(  stateChanged ( int ) ), SIGNAL( changed() ) );
    connect( ui_showCompletion, SIGNAL(  stateChanged ( int ) ), SIGNAL( changed() ) );
    connect( ui_showSchedulingError, SIGNAL(  stateChanged ( int ) ), SIGNAL( changed() ) );
    connect( ui_showTimeConstraint, SIGNAL(  stateChanged ( int ) ), SIGNAL( changed() ) );
}

void GanttChartDisplayOptionsPanel::slotOk()
{
    m_delegate->showTaskName = ui_showTaskName->checkState() == Qt::Checked;
    m_delegate->showResources = ui_showResourceNames->checkState() == Qt::Checked;
    m_delegate->showTaskLinks = ui_showDependencies->checkState() == Qt::Checked;
    m_delegate->showPositiveFloat = ui_showPositiveFloat->checkState() == Qt::Checked;
    m_delegate->showNegativeFloat = ui_showNegativeFloat->checkState() == Qt::Checked;
    m_delegate->showCriticalPath = ui_showCriticalPath->checkState() == Qt::Checked;
    m_delegate->showCriticalTasks = ui_showCriticalTasks->checkState() == Qt::Checked;
    m_delegate->showProgress = ui_showCompletion->checkState() == Qt::Checked;
    m_delegate->showSchedulingError = ui_showSchedulingError->checkState() == Qt::Checked;
    m_delegate->showTimeConstraint = ui_showTimeConstraint->checkState() == Qt::Checked;
}

void GanttChartDisplayOptionsPanel::setValues( const GanttItemDelegate &del )
{
    ui_showTaskName->setCheckState( del.showTaskName ? Qt::Checked : Qt::Unchecked );
    ui_showResourceNames->setCheckState( del.showResources ? Qt::Checked : Qt::Unchecked );
    ui_showDependencies->setCheckState( del.showTaskLinks ? Qt::Checked : Qt::Unchecked );
    ui_showPositiveFloat->setCheckState( del.showPositiveFloat ? Qt::Checked : Qt::Unchecked );
    ui_showNegativeFloat->setCheckState( del.showNegativeFloat ? Qt::Checked : Qt::Unchecked );
    ui_showCriticalPath->setCheckState( del.showCriticalPath ? Qt::Checked : Qt::Unchecked );
    ui_showCriticalTasks->setCheckState( del.showCriticalTasks ? Qt::Checked : Qt::Unchecked );
    ui_showCompletion->setCheckState( del.showProgress ? Qt::Checked : Qt::Unchecked );
    ui_showSchedulingError->setCheckState( del.showSchedulingError ? Qt::Checked : Qt::Unchecked );
    ui_showTimeConstraint->setCheckState( del.showTimeConstraint ? Qt::Checked : Qt::Unchecked );
}

void GanttChartDisplayOptionsPanel::setDefault()
{
    GanttItemDelegate del;
    setValues( del );
}

//----
GanttViewSettingsDialog::GanttViewSettingsDialog( GanttViewBase *gantt, GanttItemDelegate *delegate, ViewBase *view )
    : ItemViewSettupDialog( view, gantt->treeView(), true, view ),
    m_gantt( gantt )
{
    GanttChartDisplayOptionsPanel *panel = new GanttChartDisplayOptionsPanel( delegate );
    /*KPageWidgetItem *page = */insertWidget( 1, panel, i18n( "Chart" ), i18n( "Gantt Chart Settings" ) );
    QTabWidget *tab = new QTabWidget();
    QWidget *w = ViewBase::createPageLayoutWidget( view );
    tab->addTab( w, w->windowTitle() );
    m_pagelayout = w->findChild<KoPageLayoutWidget*>();
    Q_ASSERT( m_pagelayout );

    m_printingoptions = new GanttPrintingOptionsWidget( this );
    m_printingoptions->setOptions( gantt->printingOptions() );
    tab->addTab( m_printingoptions, m_printingoptions->windowTitle() );
    /*KPageWidgetItem *page = */insertWidget( 2, tab, i18n( "Printing" ), i18n( "Printing Options" ) );

    connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
    connect( this, SIGNAL( okClicked() ), panel, SLOT( slotOk() ) );
    connect( this, SIGNAL( defaultClicked() ), panel, SLOT( setDefault() ) );
}

void GanttViewSettingsDialog::slotOk()
{
    kDebug(planDbg());
    m_gantt->setPrintingOptions( m_printingoptions->options());
    ItemViewSettupDialog::slotOk();
}

//-------------------------
GanttPrintingOptions::GanttPrintingOptions()
    : printRowLabels( true ),
    singlePage( true )
{
}

bool GanttPrintingOptions::loadContext( const KoXmlElement &settings )
{
    KoXmlElement e = settings.namedItem( "print-options" ).toElement();
    if ( ! e.isNull() ) {
        printRowLabels = (bool)( e.attribute( "print-rowlabels", "0" ).toInt() );
        singlePage = (bool)( e.attribute( "print-singlepage", "0" ).toInt() );
    }
    kDebug()<<"..........."<<printRowLabels<<singlePage;
    return true;
}

void GanttPrintingOptions::saveContext( QDomElement &settings ) const
{
    QDomElement e = settings.ownerDocument().createElement( "print-options" );
    settings.appendChild( e );
    e.setAttribute( "print-rowlabels", printRowLabels );
    e.setAttribute( "print-singlepage", singlePage );
}

GanttPrintingOptionsWidget::GanttPrintingOptionsWidget( QWidget *parent )
    : QWidget( parent )
{
    setupUi( this );
    setWindowTitle( i18nc( "@title:tab", "Chart" ) );
}

GanttPrintingOptions GanttPrintingOptionsWidget::options() const
{
    GanttPrintingOptions opt;
    opt.printRowLabels = printRowLabels();
    opt.singlePage = singlePage();
    return opt;
}

void GanttPrintingOptionsWidget::setOptions( const GanttPrintingOptions &opt )
{
    setPrintRowLabels( opt.printRowLabels );
    setSinglePage(opt.singlePage );
}


//----------------
GanttPrintingDialog::GanttPrintingDialog( ViewBase *view, GanttViewBase *gantt )
    : PrintingDialog( view ),
    m_gantt( gantt ),
    m_options( 0 )
{
    m_headerHeight = gantt->graphicsView()->headerHeight();
    m_sceneRect = m_gantt->graphicsView()->printRect();
    m_horPages = 1;
    qreal c = m_sceneRect.width() - printer().pageRect().width();
    while ( c > 0 ) {
        ++m_horPages;
        c -= printer().pageRect().width();
    }
    m_vertPages = 1;
    c = m_sceneRect.height() - printer().pageRect().height() - m_headerHeight;
    while ( c > 0 ) {
        ++m_vertPages;
        c -= printer().pageRect().height();
    }
    kDebug(planDbg())<<m_sceneRect<<printer().pageRect()<<m_horPages<<m_vertPages;
    printer().setFromTo( documentFirstPage(), documentLastPage() );
}

// void GanttPrintingDialog::startPrinting(RemovePolicy removePolicy )
// {
//     QList<int> pages;
//     if ( printer().fromPage() > 0 ) {
//         pages << printer().fromPage();
//         if ( ! m_gantt->m_printOptions.singlePage ) {
//             int last = printer().toPage();
//             for ( int i = pages.first() + 1; i <= last; ++i ) {
//                 pages << i;
//             }
//         }
//     }
//     setPageRange( pages );
//
//     PrintingDialog::startPrinting( removePolicy );
// }

QList<QWidget*> GanttPrintingDialog::createOptionWidgets() const
{
    //kDebug(planDbg());
    GanttPrintingOptionsWidget *w = new GanttPrintingOptionsWidget();
    w->setPrintRowLabels( m_gantt->m_printOptions.printRowLabels );
    connect(w->ui_printRowLabels, SIGNAL(toggled(bool)), SLOT(slotPrintRowLabelsToogled(bool)));
    w->setSinglePage( m_gantt->m_printOptions.singlePage );
    connect(w->ui_singlePage, SIGNAL(toggled(bool)), SLOT(slotSinglePageToogled(bool)));
    const_cast<GanttPrintingDialog*>( this )->m_options = w;

    return QList<QWidget*>() << createPageLayoutWidget() << m_options;
}

void GanttPrintingDialog::slotPrintRowLabelsToogled( bool on )
{
    m_gantt->m_printOptions.printRowLabels = on;
}

void GanttPrintingDialog::slotSinglePageToogled( bool on )
{
    m_gantt->m_printOptions.singlePage = on;
    printer().setFromTo( documentFirstPage(), documentLastPage() );
}

int GanttPrintingDialog::documentLastPage() const
{
    //kDebug(planDbg())<<m_gantt->m_printOptions.singlePage<<m_horPages<<m_vertPages;
    return m_gantt->m_printOptions.singlePage ? documentFirstPage() : m_horPages * m_vertPages;
}


void GanttPrintingDialog::printPage( int page, QPainter &painter )
{
    kDebug(planDbg())<<"page:"<<page<<"first"<<documentFirstPage()<<"last:"<<documentLastPage();
    QRectF sourceRect = m_sceneRect;
    int p = page - documentFirstPage();
    QRectF pageRect = printer().pageRect();
    pageRect.moveTo( 0, 0 );
    bool singlePage = m_gantt->m_printOptions.singlePage;
    int vert = singlePage ? 0 : p / m_horPages;
    int hor = singlePage ? 0 : p % m_horPages;
    if ( ! singlePage && documentLastPage() > documentFirstPage() ) {
        // print on multiple pages, so calculate rects to print
        qreal hh = vert == 0 ? m_headerHeight : 0;
        qreal ho = vert > 0 ? m_headerHeight : 0;
        sourceRect = QRectF( sourceRect.x() + ( pageRect.width() * hor ), sourceRect.y() + ( ( pageRect.height() * vert ) - ho ), pageRect.width(), pageRect.height() - hh );
        kDebug(planDbg())<<p<<hor<<vert<<sourceRect;
    }
    painter.setClipRect( pageRect.adjusted( -1.0, -1.0, 1.0, 1.0 ) );
    m_gantt->print( &painter, pageRect, sourceRect, hor == 0 && m_gantt->m_printOptions.printRowLabels, vert == 0 );
}

//---------------------
class HeaderView : public QHeaderView
{
public:
    explicit HeaderView( QWidget* parent=0 ) : QHeaderView( Qt::Horizontal, parent ) {
    }

    QSize sizeHint() const { QSize s = QHeaderView::sizeHint(); s.rheight() *= 2; return s; }
};

GanttTreeView::GanttTreeView( QWidget* parent )
    : TreeViewBase( parent )
{
    disconnect( header() );
    setHeader( new HeaderView );

    setSelectionMode( QAbstractItemView::ExtendedSelection );

    header()->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( header(), SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
}


//-------------------------------------------
GanttViewBase::GanttViewBase( QWidget *parent )
    : KDGantt::View( parent )
{
    const KLocale *locale = KGlobal::locale();
    if ( locale ) {
        KDGantt::DateTimeGrid *g = static_cast<KDGantt::DateTimeGrid*>( grid() );
        g->setWeekStart( static_cast<Qt::DayOfWeek>( locale->weekStartDay() ) );
        int ws = locale->workingWeekStartDay();
        int we = locale->workingWeekEndDay();
        QSet<Qt::DayOfWeek> fd;
        for ( int i = Qt::Monday; i <= Qt::Sunday; ++i ) {
            if ( i < ws || i > we ) {
                fd << static_cast<Qt::DayOfWeek>( i );
            }
        }
        g->setFreeDays( fd );
    }
}

GanttTreeView *GanttViewBase::treeView() const
{
    QAbstractItemView *v = const_cast<QAbstractItemView*>( leftView() );
    return static_cast<GanttTreeView*>( v );
}

bool GanttViewBase::loadContext( const KoXmlElement &settings )
{
    KDGantt::DateTimeGrid *g = static_cast<KDGantt::DateTimeGrid*>( grid() );
    g->setScale( static_cast<KDGantt::DateTimeGrid::Scale>( settings.attribute( "chart-scale", "0" ).toInt() ) );
    g->setDayWidth( settings.attribute( "chart-daywidth", "30" ).toDouble() );
    return true;
}

void GanttViewBase::saveContext( QDomElement &settings ) const
{
    KDGantt::DateTimeGrid *g = static_cast<KDGantt::DateTimeGrid*>( grid() );
    settings.setAttribute( "chart-scale", g->scale() );
    settings.setAttribute( "chart-daywidth", g->dayWidth() );
}

//-------------------------------------------
NodeGanttViewBase::NodeGanttViewBase( QWidget *parent )
    : GanttViewBase( parent ),
    m_project( 0 ),
    m_ganttdelegate( new GanttItemDelegate( this ) )
{
    kDebug(planDbg())<<"------------------- create NodeGanttViewBase -----------------------";
    graphicsView()->setItemDelegate( m_ganttdelegate );
    GanttTreeView *tv = new GanttTreeView( this );
    tv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tv->setVerticalScrollMode( QAbstractItemView::ScrollPerPixel ); // needed since qt 4.2
    setLeftView( tv );
    m_rowController = new KDGantt::TreeViewRowController( tv, ganttProxyModel() );
    setRowController( m_rowController );
    tv->header()->setStretchLastSection( true );

    NodeSortFilterProxyModel *m = new NodeSortFilterProxyModel( &m_defaultModel, this );
    KDGantt::View::setModel( m );
}

NodeGanttViewBase::~NodeGanttViewBase()
{
    delete m_rowController;
}

NodeSortFilterProxyModel *NodeGanttViewBase::sfModel() const
{
    return static_cast<NodeSortFilterProxyModel*>( KDGantt::View::model() );
}

void NodeGanttViewBase::setItemModel( ItemModelBase *model )
{
    sfModel()->setSourceModel( model );
}

ItemModelBase *NodeGanttViewBase::model() const
{
    return sfModel()->itemModel();
}

void NodeGanttViewBase::setProject( Project *project )
{
    model()->setProject( project );
    m_project = project;
}

bool NodeGanttViewBase::loadContext( const KoXmlElement &settings )
{
    treeView()->loadContext( model()->columnMap(), settings );

    KoXmlElement e = settings.namedItem( "ganttchart" ).toElement();
    if ( ! e.isNull() ) {
        m_ganttdelegate->showTaskLinks = (bool)( e.attribute( "show-dependencies", "0" ).toInt() );
        m_ganttdelegate->showTaskName = (bool)( e.attribute( "show-taskname", "0" ).toInt() );
        m_ganttdelegate->showResources = (bool)( e.attribute( "show-resourcenames", "0" ).toInt() );
        m_ganttdelegate->showProgress = (bool)( e.attribute( "show-completion", "0" ).toInt() );
        m_ganttdelegate->showCriticalPath = (bool)( e.attribute( "show-criticalpath", "0" ).toInt() );
        m_ganttdelegate->showCriticalTasks = (bool)( e.attribute( "show-criticaltasks", "0" ).toInt() );
        m_ganttdelegate->showPositiveFloat = (bool)( e.attribute( "show-positivefloat", "0" ).toInt() );
        m_ganttdelegate->showSchedulingError = (bool)( e.attribute( "show-schedulingerror", "0" ).toInt() );
        m_ganttdelegate->showTimeConstraint = (bool)( e.attribute( "show-timeconstraint", "0" ).toInt() );
        m_ganttdelegate->showNegativeFloat = (bool)( e.attribute( "show-negativefloat", "0" ).toInt() );

        GanttViewBase::loadContext( e );

        m_printOptions.loadContext( e );
    }
    return true;
}

void NodeGanttViewBase::saveContext( QDomElement &settings ) const
{
    kDebug(planDbg());
    treeView()->saveContext( model()->columnMap(), settings );

    QDomElement e = settings.ownerDocument().createElement( "ganttchart" );
    settings.appendChild( e );
    e.setAttribute( "show-dependencies", m_ganttdelegate->showTaskLinks );
    e.setAttribute( "show-taskname", m_ganttdelegate->showTaskName );
    e.setAttribute( "show-resourcenames", m_ganttdelegate->showResources );
    e.setAttribute( "show-completion", m_ganttdelegate->showProgress );
    e.setAttribute( "show-criticalpath", m_ganttdelegate->showCriticalPath  );
    e.setAttribute( "show-criticaltasks", m_ganttdelegate->showCriticalTasks );
    e.setAttribute( "show-positivefloat", m_ganttdelegate->showPositiveFloat );
    e.setAttribute( "show-schedulingerror", m_ganttdelegate->showSchedulingError );
    e.setAttribute( "show-timeconstraint", m_ganttdelegate->showTimeConstraint );
    e.setAttribute( "show-negativefloat", m_ganttdelegate->showNegativeFloat );

    GanttViewBase::saveContext( e );

    m_printOptions.saveContext( e );
}

//-------------------------------------------
MyKDGanttView::MyKDGanttView( QWidget *parent )
    : NodeGanttViewBase( parent ),
    m_manager( 0 )
{
    kDebug(planDbg())<<"------------------- create MyKDGanttView -----------------------";
    GanttItemModel *gm = new GanttItemModel( this );
    setItemModel( gm );
    treeView()->createItemDelegates( gm );

    QList<int> show;
    show << NodeModel::NodeName
            << NodeModel::NodeCompleted
            << NodeModel::NodeStartTime
            << NodeModel::NodeEndTime;

    treeView()->setDefaultColumns( show );
    for ( int i = 0; i < model()->columnCount(); ++i ) {
        if ( ! show.contains( i ) ) {
            treeView()->hideColumn( i );
        }
    }

    setConstraintModel( new KDGantt::ConstraintModel( this ) );
    KDGantt::ProxyModel *m = static_cast<KDGantt::ProxyModel*>( ganttProxyModel() );

    m->setRole( KDGantt::ItemTypeRole, KDGantt::ItemTypeRole ); // To provide correct format
    m->setRole( KDGantt::StartTimeRole, Qt::EditRole ); // To provide correct format
    m->setRole( KDGantt::EndTimeRole, Qt::EditRole ); // To provide correct format

    m->setColumn( KDGantt::ItemTypeRole, NodeModel::NodeType );
    m->setColumn( KDGantt::StartTimeRole, NodeModel::NodeStartTime );
    m->setColumn( KDGantt::EndTimeRole, NodeModel::NodeEndTime );
    m->setColumn( KDGantt::TaskCompletionRole, NodeModel::NodeCompleted );

    KDGantt::DateTimeGrid *g = static_cast<KDGantt::DateTimeGrid*>( grid() );
    g->setDayWidth( 30 );
    // FIXME: improve/cover all options
    QMap<QString, QString> format;
    format.insert( "%H", "HH" );
    format.insert( "%k", "H" );
    format.insert( "%I", "HH A" );
    format.insert( "%l", "h a" );
    g->setHourFormat( format.value( KGlobal::locale()->timeFormat().left( 2 ) ) );

    connect( model(), SIGNAL( nodeInserted( Node* ) ), this, SLOT( slotNodeInserted( Node* ) ) );
}

GanttItemModel *MyKDGanttView::model() const
{
    return static_cast<GanttItemModel*>( NodeGanttViewBase::model() );
}

void MyKDGanttView::setProject( Project *proj )
{
    clearDependencies();
    if ( project() ) {
        disconnect( project(), SIGNAL( relationToBeModified(Relation*)), this, SLOT( removeDependency(Relation*)));
        disconnect( project(), SIGNAL( relationModified(Relation*)), this, SLOT( addDependency(Relation*)));
        disconnect( project(), SIGNAL( relationAdded( Relation* ) ), this, SLOT( addDependency( Relation* ) ) );
        disconnect( project(), SIGNAL( relationToBeRemoved( Relation* ) ), this, SLOT( removeDependency( Relation* ) ) );
        disconnect( project(), SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
    }
    NodeGanttViewBase::setProject( proj );
    if ( proj ) {
        connect( project(), SIGNAL( relationToBeModified(Relation*)), this, SLOT( removeDependency(Relation*)));
        connect( project(), SIGNAL( relationModified(Relation*)), this, SLOT( addDependency(Relation*)));
        connect( proj, SIGNAL( relationAdded( Relation* ) ), this, SLOT( addDependency( Relation* ) ) );
        connect( proj, SIGNAL( relationToBeRemoved( Relation* ) ), this, SLOT( removeDependency( Relation* ) ) );
        connect( proj, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
    }

    createDependencies();
}

void MyKDGanttView::slotProjectCalculated( ScheduleManager *sm )
{
    if ( m_manager == sm ) {
        setScheduleManager( sm );
    }
}

void MyKDGanttView::setScheduleManager( ScheduleManager *sm )
{
    clearDependencies();
    m_manager = sm;
    KDGantt::DateTimeGrid *g = static_cast<KDGantt::DateTimeGrid*>( grid() );
    if ( sm && project() ) {
        QDateTime start = project()->startTime( sm->scheduleId() ).addDays( -1 );
        if ( g->startDateTime() !=  start ) {
            g->setStartDateTime( start );
        }
    }
    if ( ! g->startDateTime().isValid() ) {
        g->setStartDateTime( QDateTime::currentDateTime() );
    }
    model()->setScheduleManager( sm );
    createDependencies();
}

void MyKDGanttView::slotNodeInserted( Node *node )
{
    foreach( Relation *r, node->dependChildNodes() ) {
        addDependency( r );
    }
    foreach( Relation *r, node->dependParentNodes() ) {
        addDependency( r );
    }
}

void MyKDGanttView::addDependency( Relation *rel )
{
    QModelIndex par = sfModel()->mapFromSource( model()->index( rel->parent() ) );
    QModelIndex ch = sfModel()->mapFromSource( model()->index( rel->child() ) );
//    kDebug(planDbg())<<"addDependency() "<<model()<<par.model();
    if ( par.isValid() && ch.isValid() ) {
        KDGantt::Constraint con( par, ch, KDGantt::Constraint::TypeSoft,
                                 static_cast<KDGantt::Constraint::RelationType>( rel->type() )/*NOTE!!*/
                               );
        if ( ! constraintModel()->hasConstraint( con ) ) {
            constraintModel()->addConstraint( con );
        }
    }
}

void MyKDGanttView::removeDependency( Relation *rel )
{
    QModelIndex par = sfModel()->mapFromSource( model()->index( rel->parent() ) );
    QModelIndex ch = sfModel()->mapFromSource( model()->index( rel->child() ) );
    KDGantt::Constraint con( par, ch, KDGantt::Constraint::TypeSoft,
                             static_cast<KDGantt::Constraint::RelationType>( rel->type() )/*NOTE!!*/
                           );
    constraintModel()->removeConstraint( con );
}

void MyKDGanttView::clearDependencies()
{
    constraintModel()->clear();
}

void MyKDGanttView::createDependencies()
{
    clearDependencies();
    if ( project() == 0 || m_manager == 0 ) {
        return;
    }
    foreach ( Node* n, project()->allNodes() ) {
        foreach ( Relation *r, n->dependChildNodes() ) {
            addDependency( r );
        }
    }
}

//------------------------------------------
GanttView::GanttView(KoPart *part, KoDocument *doc, QWidget *parent, bool readWrite)
    : ViewBase(part, doc, parent),
    m_readWrite( readWrite ),
    m_project( 0 )
{
    kDebug(planDbg()) <<" ---------------- KPlato: Creating GanttView ----------------";

    QVBoxLayout *l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_splitter = new QSplitter( this );
    l->addWidget( m_splitter );
    m_splitter->setOrientation( Qt::Vertical );

    m_gantt = new MyKDGanttView( m_splitter );

    setupGui();

    updateReadWrite( readWrite );
    //connect( m_gantt->constraintModel(), SIGNAL( constraintAdded( const Constraint& )), this, SLOT( update() ) );
    kDebug(planDbg()) <<m_gantt->constraintModel();

    connect( m_gantt->treeView(), SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), SLOT( slotContextMenuRequested( QModelIndex, const QPoint& ) ) );

    connect( m_gantt->treeView(), SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
}

KoPrintJob *GanttView::createPrintJob()
{
    return new GanttPrintingDialog( this, m_gantt );
}

void GanttView::setZoom( double )
{
    //kDebug(planDbg()) <<"setting gantt zoom:" << zoom;
    //m_gantt->setZoomFactor(zoom,true); NO!!! setZoomFactor() is something else
}

void GanttView::setupGui()
{
    // create context menu actions
    actionShowProject = new KToggleAction( i18n( "Show Project" ), this );
    connect(actionShowProject, SIGNAL(triggered(bool) ), m_gantt->model(), SLOT(setShowProject(bool)));
    addContextAction( actionShowProject );

    createOptionAction();
}

void GanttView::slotOptions()
{
    kDebug(planDbg());
    GanttViewSettingsDialog *dlg = new GanttViewSettingsDialog( m_gantt, m_gantt->delegate(), this );
    connect(dlg, SIGNAL(finished(int)), SLOT(slotOptionsFinished(int)));
    dlg->show();
    dlg->raise();
    dlg->activateWindow();
}

void GanttView::slotOptionsFinished( int result )
{
    GanttViewSettingsDialog *dlg = qobject_cast<GanttViewSettingsDialog*>( sender() );
    if ( dlg && result == QDialog::Accepted ) {
        m_gantt->graphicsView()->updateScene();
    }
    ViewBase::slotOptionsFinished( result );
}

void GanttView::clear()
{
//    m_gantt->clear();
}

void GanttView::setShowResources( bool on )
{
    m_gantt->delegate()->showResources = on;
}

void GanttView::setShowTaskName( bool on )
{
    m_gantt->delegate()->showTaskName = on;
}

void GanttView::setShowProgress( bool on )
{
    m_gantt->delegate()->showProgress = on;
}

void GanttView::setShowPositiveFloat( bool on )
{
    m_gantt->delegate()->showPositiveFloat = on;
}

void GanttView::setShowCriticalTasks( bool on )
{
    m_gantt->delegate()->showCriticalTasks = on;
}

void GanttView::setShowCriticalPath( bool on )
{
    m_gantt->delegate()->showCriticalPath = on;
}

void GanttView::setShowNoInformation( bool on )
{
    m_gantt->delegate()->showNoInformation = on;
}

void GanttView::setShowAppointments( bool on )
{
    m_gantt->delegate()->showAppointments = on;
}

void GanttView::setShowTaskLinks( bool on )
{
    m_gantt->delegate()->showTaskLinks = on;
}

void GanttView::setProject( Project *project )
{
    m_gantt->setProject( project );
}

void GanttView::setScheduleManager( ScheduleManager *sm )
{
    //kDebug(planDbg())<<id<<endl;
    m_gantt->setScheduleManager( sm );
}

void GanttView::draw( Project &project )
{
    setProject( &project );
}

void GanttView::drawChanges( Project &project )
{
    if ( m_project != &project ) {
        setProject( &project );
    }
}

Node *GanttView::currentNode() const
{
    QModelIndex idx = m_gantt->treeView()->selectionModel()->currentIndex();
    return m_gantt->model()->node( m_gantt->sfModel()->mapToSource( idx ) );
}

void GanttView::slotContextMenuRequested( QModelIndex idx, const QPoint &pos )
{
    kDebug(planDbg());
    QString name;
    Node *node = m_gantt->model()->node( m_gantt->sfModel()->mapToSource( idx ) );
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
    } else kDebug(planDbg())<<"No node";
    if ( name.isEmpty() ) {
        slotHeaderContextMenuRequested( pos );
        kDebug(planDbg())<<"No menu";
        return;
    }
    emit requestPopupMenu( name, pos );
}

bool GanttView::loadContext( const KoXmlElement &settings )
{
    kDebug(planDbg());
    ViewBase::loadContext( settings );
    bool show = (bool)(settings.attribute( "show-project", "0" ).toInt() );
    actionShowProject->setChecked( show );
    m_gantt->model()->setShowProject( show ); // why is this not called by the action?

    return m_gantt->loadContext( settings );
}

void GanttView::saveContext( QDomElement &settings ) const
{
    kDebug(planDbg());
    ViewBase::saveContext( settings );
    settings.setAttribute( "show-project", actionShowProject->isChecked() );

    m_gantt->saveContext( settings );

}

void GanttView::updateReadWrite( bool on )
{
    // TODO: KDGanttView needs read/write mode
    m_readWrite = on;
}

//----
MilestoneGanttViewSettingsDialog::MilestoneGanttViewSettingsDialog( GanttViewBase *gantt, ViewBase *view )
    : ItemViewSettupDialog( view, gantt->treeView(), true, view ),
    m_gantt( gantt )
{
    QTabWidget *tab = new QTabWidget();
    QWidget *w = ViewBase::createPageLayoutWidget( view );
    tab->addTab( w, w->windowTitle() );
    m_pagelayout = w->findChild<KoPageLayoutWidget*>();
    Q_ASSERT( m_pagelayout );

    m_printingoptions = new GanttPrintingOptionsWidget( this );
    m_printingoptions->setOptions( gantt->printingOptions() );
    tab->addTab( m_printingoptions, m_printingoptions->windowTitle() );
    /*KPageWidgetItem *page = */insertWidget( -1, tab, i18n( "Printing" ), i18n( "Printing Options" ) );

    connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
}

void MilestoneGanttViewSettingsDialog::slotOk()
{
    kDebug(planDbg());
    m_gantt->setPrintingOptions( m_printingoptions->options());
    ItemViewSettupDialog::slotOk();
}

//------------------------
MilestoneKDGanttView::MilestoneKDGanttView( QWidget *parent )
    : NodeGanttViewBase( parent ),
    m_manager( 0 )
{
    kDebug(planDbg())<<"------------------- create MilestoneKDGanttView -----------------------";
    MilestoneItemModel *mm = new MilestoneItemModel( this );
    setItemModel( mm );
    treeView()->createItemDelegates( mm );

    sfModel()->setFilterRole ( Qt::EditRole );
    sfModel()->setFilterFixedString( QString::number( Node::Type_Milestone ) );
    sfModel()->setFilterKeyColumn( NodeModel::NodeType );

    QList<int> show;
    show << NodeModel::NodeName
            << NodeModel::NodeStartTime;

    treeView()->setDefaultColumns( show );
    for ( int i = 0; i < model()->columnCount(); ++i ) {
        if ( ! show.contains( i ) ) {
            treeView()->hideColumn( i );
        }
    }
    treeView()->setRootIsDecorated ( false );

    KDGantt::ProxyModel *m = static_cast<KDGantt::ProxyModel*>( ganttProxyModel() );

    m->setRole( KDGantt::ItemTypeRole, KDGantt::ItemTypeRole ); // To provide correct format
    m->setRole( KDGantt::StartTimeRole, Qt::EditRole ); // To provide correct format
    m->setRole( KDGantt::EndTimeRole, Qt::EditRole ); // To provide correct format

    m->setColumn( KDGantt::ItemTypeRole, NodeModel::NodeType );
    m->setColumn( KDGantt::StartTimeRole, NodeModel::NodeStartTime );
    m->setColumn( KDGantt::EndTimeRole, NodeModel::NodeEndTime );
    m->setColumn( KDGantt::TaskCompletionRole, NodeModel::NodeCompleted );

    KDGantt::DateTimeGrid *g = static_cast<KDGantt::DateTimeGrid*>( grid() );
    g->setDayWidth( 30 );
    // FIXME: improve/cover all options
    QMap<QString, QString> format;
    format.insert( "%H", "HH" );
    format.insert( "%k", "H" );
    format.insert( "%I", "HH A" );
    format.insert( "%l", "h a" );
    g->setHourFormat( format.value( KGlobal::locale()->timeFormat().left( 2 ) ) );
}

MilestoneItemModel *MilestoneKDGanttView::model() const
{
    return static_cast<MilestoneItemModel*>( NodeGanttViewBase::model() );
}

void MilestoneKDGanttView::setProject( Project *proj )
{
    if ( project() ) {
        disconnect( project(), SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
    }
    NodeGanttViewBase::setProject( proj );
    if ( proj ) {
        connect( proj, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
    }
}

void MilestoneKDGanttView::slotProjectCalculated( ScheduleManager *sm )
{
    if ( m_manager == sm ) {
        setScheduleManager( sm );
    }
}

void MilestoneKDGanttView::setScheduleManager( ScheduleManager *sm )
{
    //kDebug(planDbg())<<id<<endl;
    model()->setScheduleManager( 0 );
    m_manager = sm;
    KDGantt::DateTimeGrid *g = static_cast<KDGantt::DateTimeGrid*>( grid() );
    if ( sm && m_project ) {
        QDateTime start;
        foreach ( const Node *n, model()->mileStones() ) {
            QDateTime nt = n->startTime( sm->scheduleId() );
            if ( ! nt.isValid() ) {
                continue;
            }
            if ( ! start.isValid() || start > nt ) {
                start = nt;
                kDebug(planDbg())<<n->name()<<start;
            }
        }
        if ( ! start.isValid() ) {
            start = project()->startTime( sm->scheduleId() );
        }
        start = start.addDays( -1 );
        if ( g->startDateTime() !=  start ) {
            g->setStartDateTime( start );
        }
    }
    if ( ! g->startDateTime().isValid() ) {
        g->setStartDateTime( QDateTime::currentDateTime() );
    }
    model()->setScheduleManager( sm );
}

//------------------------------------------

MilestoneGanttView::MilestoneGanttView(KoPart *part, KoDocument *doc, QWidget *parent, bool readWrite)
    : ViewBase(part, doc, parent),
        m_readWrite( readWrite ),
        m_project( 0 )
{
    kDebug(planDbg()) <<" ---------------- Plan: Creating Milesone GanttView ----------------";

    QVBoxLayout *l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_splitter = new QSplitter( this );
    l->addWidget( m_splitter );
    m_splitter->setOrientation( Qt::Vertical );

    setupGui();

    m_gantt = new MilestoneKDGanttView( m_splitter );

    m_showTaskName = false; // FIXME
    m_showProgress = false; //FIXME
    m_showPositiveFloat = false; //FIXME
    m_showCriticalTasks = false; //FIXME
    m_showNoInformation = false; //FIXME

    updateReadWrite( readWrite );

    connect( m_gantt->treeView(), SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), SLOT( slotContextMenuRequested( QModelIndex, const QPoint& ) ) );

    connect( m_gantt->treeView(), SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
}

void MilestoneGanttView::setZoom( double )
{
    //kDebug(planDbg()) <<"setting gantt zoom:" << zoom;
    //m_gantt->setZoomFactor(zoom,true); NO!!! setZoomFactor() is something else
}

void MilestoneGanttView::show()
{
}

void MilestoneGanttView::clear()
{
}

void MilestoneGanttView::setProject( Project *project )
{
    m_gantt->setProject( project );
}

void MilestoneGanttView::setScheduleManager( ScheduleManager *sm )
{
    //kDebug(planDbg())<<id<<endl;
    m_gantt->setScheduleManager( sm );
}

void MilestoneGanttView::draw( Project &project )
{
    setProject( &project );
}

void MilestoneGanttView::drawChanges( Project &project )
{
    if ( m_project != &project ) {
        setProject( &project );
    }
}

Node *MilestoneGanttView::currentNode() const
{
    QModelIndex idx = m_gantt->treeView()->selectionModel()->currentIndex();
    return m_gantt->model()->node( m_gantt->sfModel()->mapToSource( idx ) );
}

void MilestoneGanttView::setupGui()
{
    createOptionAction();
}

void MilestoneGanttView::slotContextMenuRequested( QModelIndex idx, const QPoint &pos )
{
    kDebug(planDbg());
    QString name;
    Node *node = m_gantt->model()->node( m_gantt->sfModel()->mapToSource( idx ) );
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
    } else kDebug(planDbg())<<"No node";
    if ( name.isEmpty() ) {
        kDebug(planDbg())<<"No menu";
        slotHeaderContextMenuRequested( pos );
        return;
    }
    emit requestPopupMenu( name, pos );
}

void MilestoneGanttView::slotOptions()
{
    kDebug(planDbg());
    MilestoneGanttViewSettingsDialog *dlg =  new MilestoneGanttViewSettingsDialog( m_gantt, this );
    connect(dlg, SIGNAL(finished(int)), SLOT(slotOptionsFinished(int)));
    dlg->show();
    dlg->raise();
    dlg->activateWindow();
}

bool MilestoneGanttView::loadContext( const KoXmlElement &settings )
{
    kDebug(planDbg());
    ViewBase::loadContext( settings );
    return m_gantt->loadContext( settings );
}

void MilestoneGanttView::saveContext( QDomElement &settings ) const
{
    kDebug(planDbg());
    ViewBase::saveContext( settings );
    return m_gantt->saveContext( settings );
}

void MilestoneGanttView::updateReadWrite( bool on )
{
    m_readWrite = on;
}

KoPrintJob *MilestoneGanttView::createPrintJob()
{
    return new GanttPrintingDialog( this, m_gantt );
}

//--------------------
ResourceAppointmentsGanttViewSettingsDialog::ResourceAppointmentsGanttViewSettingsDialog( ViewBase *view,  GanttTreeView *treeview )
: ItemViewSettupDialog( view, treeview, true, view )
{
    QTabWidget *tab = new QTabWidget();
    QWidget *w = ViewBase::createPageLayoutWidget( view );
    tab->addTab( w, w->windowTitle() );
    m_pagelayout = w->findChild<KoPageLayoutWidget*>();
    Q_ASSERT( m_pagelayout );
    /*KPageWidgetItem *page = */insertWidget( -1, tab, i18n( "Printing" ), i18n( "Printing Options" ) );
}

//------------------------------------------

ResourceAppointmentsGanttView::ResourceAppointmentsGanttView(KoPart *part, KoDocument *doc, QWidget *parent, bool readWrite)
    : ViewBase(part, doc, parent),
    m_project( 0 ),
    m_model( new ResourceAppointmentsGanttModel( this ) )
{
    kDebug(planDbg()) <<" ---------------- KPlato: Creating ResourceAppointmentsGanttView ----------------";

    m_gantt = new GanttViewBase( this );
    m_gantt->graphicsView()->setItemDelegate( new ResourceGanttItemDelegate( m_gantt ) );

    GanttTreeView *tv = new GanttTreeView( m_gantt );
    tv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tv->setVerticalScrollMode( QAbstractItemView::ScrollPerPixel ); // needed since qt 4.2
    m_gantt->setLeftView( tv );
    m_rowController = new KDGantt::TreeViewRowController( tv, m_gantt->ganttProxyModel() );
    m_gantt->setRowController( m_rowController );
    tv->header()->setStretchLastSection( true );


    KDGantt::ProxyModel *m = static_cast<KDGantt::ProxyModel*>( m_gantt->ganttProxyModel() );
    m->setRole( KDGantt::ItemTypeRole, KDGantt::ItemTypeRole );
    m->setRole( KDGantt::StartTimeRole, KDGantt::StartTimeRole );
    m->setRole( KDGantt::EndTimeRole, KDGantt::EndTimeRole );
    m->setRole( KDGantt::TaskCompletionRole, KDGantt::TaskCompletionRole );

    m_gantt->setModel( m_model );

    QVBoxLayout *l = new QVBoxLayout( this );
    l->setMargin( 0 );
    l->addWidget( m_gantt );

    setupGui();

    updateReadWrite( readWrite );

    connect( m_gantt->leftView(), SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), SLOT( slotContextMenuRequested( QModelIndex, const QPoint& ) ) );

    connect( m_gantt->leftView(), SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
}

ResourceAppointmentsGanttView::~ResourceAppointmentsGanttView()
{
    delete m_rowController;
}

void ResourceAppointmentsGanttView::setZoom( double )
{
    //kDebug(planDbg()) <<"setting gantt zoom:" << zoom;
    //m_gantt->setZoomFactor(zoom,true); NO!!! setZoomFactor() is something else
}

Project *ResourceAppointmentsGanttView::project() const
{
    return m_model->project();
}

void ResourceAppointmentsGanttView::setProject( Project *project )
{
    static_cast<KDGantt::DateTimeGrid*>( m_gantt->grid() )->setStartDateTime( project->startTime() );
    m_model->setProject( project );
}

void ResourceAppointmentsGanttView::setScheduleManager( ScheduleManager *sm )
{
    //kDebug(planDbg())<<id<<endl;
    m_model->setScheduleManager( sm );
}

void ResourceAppointmentsGanttView::setupGui()
{
    createOptionAction();
}

Node *ResourceAppointmentsGanttView::currentNode() const
{
    QModelIndex idx = treeView()->selectionModel()->currentIndex();
    return m_model->node( idx );
}

void ResourceAppointmentsGanttView::slotContextMenuRequested( QModelIndex idx, const QPoint &pos )
{
    kDebug(planDbg());
    QString name;
    if ( idx.isValid() ) {
        Node *n = m_model->node( idx );
        if ( n ) {
            name = "taskview_popup";
        }
    }
    if ( name.isEmpty() ) {
        slotHeaderContextMenuRequested( pos );
        return;
    }
    emit requestPopupMenu( name, pos );
}

void ResourceAppointmentsGanttView::slotOptions()
{
    kDebug(planDbg());
    QPointer<ItemViewSettupDialog> dlg = new ResourceAppointmentsGanttViewSettingsDialog( this, treeView() );
//     dlg->addPrintingOptions();
    dlg->exec();
    delete dlg;
}

bool ResourceAppointmentsGanttView::loadContext( const KoXmlElement &settings )
{
    kDebug(planDbg());
    ViewBase::loadContext( settings );
    m_gantt->loadContext( settings );
    return treeView()->loadContext( m_model->columnMap(), settings );
}

void ResourceAppointmentsGanttView::saveContext( QDomElement &settings ) const
{
    kDebug(planDbg());
    ViewBase::saveContext( settings );
    m_gantt->saveContext( settings );
    treeView()->saveContext( m_model->columnMap(), settings );
}

void ResourceAppointmentsGanttView::updateReadWrite( bool on )
{
    m_readWrite = on;
}

KoPrintJob *ResourceAppointmentsGanttView::createPrintJob()
{
    return new GanttPrintingDialog( this, m_gantt );
}

}  //KPlato namespace

#include "kptganttview.moc"
