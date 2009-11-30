/* This file is part of the KDE project
  Copyright (C) 2002 - 2007 Dag Andersen <danders@get2net.dk>
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

#include <kdganttproxymodel.h>
#include <kdganttconstraintmodel.h>
#include <kdganttconstraint.h>
#include <kdganttdatetimegrid.h>
#include <kdganttgraphicsview.h>
#include <kdgantttreeviewrowcontroller.h>

#include <KoDocument.h>

#include <kdebug.h>

#include <QSplitter>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDateTime>
#include <QMenu>
#include <QModelIndex>
#include <QPainter>

#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include <kaction.h>

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
    connect( ui_showCriticalPath, SIGNAL(  stateChanged ( int ) ), SIGNAL( changed() ) );
    connect( ui_showCriticalTasks, SIGNAL(  stateChanged ( int ) ), SIGNAL( changed() ) );
    connect( ui_showCompletion, SIGNAL(  stateChanged ( int ) ), SIGNAL( changed() ) );
}

void GanttChartDisplayOptionsPanel::slotOk()
{
    m_delegate->showTaskName = ui_showTaskName->checkState() == Qt::Checked;
    m_delegate->showResources = ui_showResourceNames->checkState() == Qt::Checked;
    m_delegate->showTaskLinks = ui_showDependencies->checkState() == Qt::Checked;
    m_delegate->showPositiveFloat = ui_showPositiveFloat->checkState() == Qt::Checked;
    m_delegate->showCriticalPath = ui_showCriticalPath->checkState() == Qt::Checked;
    m_delegate->showCriticalTasks = ui_showCriticalTasks->checkState() == Qt::Checked;
    m_delegate->showProgress = ui_showCompletion->checkState() == Qt::Checked;
}

void GanttChartDisplayOptionsPanel::setValues( const GanttItemDelegate &del )
{
    ui_showTaskName->setCheckState( del.showTaskName ? Qt::Checked : Qt::Unchecked );
    ui_showResourceNames->setCheckState( del.showResources ? Qt::Checked : Qt::Unchecked );
    ui_showDependencies->setCheckState( del.showTaskLinks ? Qt::Checked : Qt::Unchecked );
    ui_showPositiveFloat->setCheckState( del.showPositiveFloat ? Qt::Checked : Qt::Unchecked );
    ui_showCriticalPath->setCheckState( del.showCriticalPath ? Qt::Checked : Qt::Unchecked );
    ui_showCriticalTasks->setCheckState( del.showCriticalTasks ? Qt::Checked : Qt::Unchecked );
    ui_showCompletion->setCheckState( del.showProgress ? Qt::Checked : Qt::Unchecked );
}

void GanttChartDisplayOptionsPanel::setDefault()
{
    GanttItemDelegate del;
    setValues( del );
}

//----
GanttViewSettingsDialog::GanttViewSettingsDialog( TreeViewBase *view, GanttItemDelegate *delegate, QWidget *parent )
    : ItemViewSettupDialog( view, true, parent )
{
    GanttChartDisplayOptionsPanel *panel = new GanttChartDisplayOptionsPanel( delegate );
    KPageWidgetItem *page = insertWidget( 1, panel, i18n( "Chart" ), i18n( "Gantt Chart Settings" ) );
    
    connect( this, SIGNAL( okClicked() ), panel, SLOT( slotOk() ) );
    connect( this, SIGNAL( defaultClicked() ), panel, SLOT( setDefault() ) );
}

//-------------------------
GanttPrintingOptions::GanttPrintingOptions( QWidget *parent )
    : QWidget( parent )
{
    setupUi( this );
    ui_printRowLabels->hide(); // TODO or remove?
    setWindowTitle( i18n("Options") );
}

//----------------
GanttPrintingDialog::GanttPrintingDialog( ViewBase *view, KDGantt::View *gantt )
    : PrintingDialog( view ),
    m_gantt( gantt ),
    m_options( 0 ),
    m_singlePage( true ),
    m_printRowLabels( true )
{
    m_pageRect = printer().pageRect();
    m_headerHeight = gantt->graphicsView()->headerHeight();
    m_sceneRect = m_gantt->graphicsView()->printRect();
    m_horPages = 1;
    qreal c = m_sceneRect.width() - m_pageRect.width();
    while ( c > 0 ) {
        ++m_horPages;
        c -= m_pageRect.width();
    }
    m_vertPages = 1;
    c = m_sceneRect.height() - m_pageRect.height() - m_headerHeight;
    while ( c > 0 ) {
        ++m_vertPages;
        c -= m_pageRect.height();
    }
    kDebug()<<m_sceneRect<<m_pageRect<<m_horPages<<m_vertPages;
    printer().setFromTo( documentFirstPage(), documentLastPage() );
}

void GanttPrintingDialog::startPrinting(RemovePolicy removePolicy )
{
    if ( m_options ) {
        m_singlePage = m_options->singlePage();
        //m_printRowLabels = m_options->printRowLabels();
    }
    KoPrintingDialog::startPrinting( removePolicy );
}

QList<QWidget*> GanttPrintingDialog::createOptionWidgets() const
{
    //kDebug();
    GanttPrintingOptions *w = new GanttPrintingOptions();
    //w->setPrintRowLabels( m_printRowLabels );
    w->setSinglePage( m_singlePage );
    
    const_cast<GanttPrintingDialog*>( this )->m_options = w;
    
    return QList<QWidget*>() << m_options;
}

int GanttPrintingDialog::documentLastPage() const
{
    kDebug()<<m_horPages<<m_vertPages;
    return m_singlePage ? documentFirstPage() : m_horPages * m_vertPages;
}


void GanttPrintingDialog::printPage( int page, QPainter &painter )
{
    QRectF sourceRect = m_sceneRect;
    int p = page - documentFirstPage();
    QRectF pageRect = m_pageRect;
    int vert = m_singlePage ? 0 : p / m_horPages;
    int hor = m_singlePage ? 0 : p % m_horPages;
    if ( ! m_singlePage && documentLastPage() > documentFirstPage() ) {
        // print on multiple pages, so calculate rects to print
        qreal hh = vert == 0 ? m_headerHeight : 0;
        qreal ho = vert > 0 ? m_headerHeight : 0;
        sourceRect = QRectF( sourceRect.x() + ( pageRect.width() * hor ), sourceRect.y() + ( ( pageRect.height() * vert ) - ho ), pageRect.width(), pageRect.height() - hh );
        kDebug()<<p<<hor<<vert<<sourceRect;
    }
    m_gantt->print( &painter, pageRect, sourceRect, m_printRowLabels, vert == 0 );
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
    : KDGantt::View( parent ),
    m_project( 0 ),
    m_ganttdelegate( new GanttItemDelegate( this ) )
{
    kDebug()<<"------------------- create GanttViewBase -----------------------";
    graphicsView()->setItemDelegate( m_ganttdelegate );
    GanttTreeView *tv = new GanttTreeView( this );
    tv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tv->setVerticalScrollMode( QAbstractItemView::ScrollPerPixel ); // needed since qt 4.2
    setLeftView( tv );
    setRowController( new KDGantt::TreeViewRowController( tv, ganttProxyModel() ) );
    tv->header()->setStretchLastSection( true );
    
    NodeSortFilterProxyModel *m = new NodeSortFilterProxyModel( &m_defaultModel, this );
    KDGantt::View::setModel( m );
}

NodeSortFilterProxyModel *GanttViewBase::sfModel() const
{
    return static_cast<NodeSortFilterProxyModel*>( KDGantt::View::model() );
}

void GanttViewBase::setItemModel( ItemModelBase *model )
{
    sfModel()->setSourceModel( model );
}

ItemModelBase *GanttViewBase::model() const
{
    return sfModel()->itemModel();
}

GanttTreeView *GanttViewBase::treeView() const
{
    QAbstractItemView *v = const_cast<QAbstractItemView*>( leftView() );
    return static_cast<GanttTreeView*>( v );
}

void GanttViewBase::setProject( Project *project )
{
    model()->setProject( project );
    m_project = project;
}

bool GanttViewBase::loadContext( const KoXmlElement &settings )
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
    }
    return true;
}

void GanttViewBase::saveContext( QDomElement &settings ) const
{
    kDebug();
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
}

//-------------------------------------------
MyKDGanttView::MyKDGanttView( QWidget *parent )
    : GanttViewBase( parent ),
    m_manager( 0 )
{
    kDebug()<<"------------------- create MyKDGanttView -----------------------";
    setItemModel( new GanttItemModel( this ) );

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
    
    static_cast<KDGantt::DateTimeGrid*>( grid() )->setDayWidth( 30 );
    //static_cast<KDGantt::DateTimeGrid*>( grid() )->setRowSeparators( treeView()->alternatingRowColors() );
    
    connect( model(), SIGNAL( nodeInserted( Node* ) ), this, SLOT( slotNodeInserted( Node* ) ) );
}

GanttItemModel *MyKDGanttView::model() const
{
    return static_cast<GanttItemModel*>( GanttViewBase::model() );
}

void MyKDGanttView::setProject( Project *proj )
{
    clearDependencies();
    if ( project() ) {
        disconnect( project(), SIGNAL( relationAdded( Relation* ) ), this, SLOT( addDependency( Relation* ) ) );
        disconnect( project(), SIGNAL( relationToBeRemoved( Relation* ) ), this, SLOT( removeDependency( Relation* ) ) );
        disconnect( project(), SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
    }
    GanttViewBase::setProject( proj );
    if ( proj ) {
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
    //kDebug()<<id<<endl;
    clearDependencies();
    model()->setManager( sm );
    m_manager = sm;
    if ( sm && project() ) {
        QDateTime start = project()->startTime( sm->id() ).dateTime().addDays( -1 );
        KDGantt::DateTimeGrid *g = static_cast<KDGantt::DateTimeGrid*>( grid() );
        if ( g->startDateTime() !=  start ) {
            g->setStartDateTime( start );
        }
    }
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
    kDebug()<<"addDependency() "<<model()<<par.model();
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
    qDebug()<<"removeDependency() "<<model()<<par.model();
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
GanttView::GanttView( KoDocument *part, QWidget *parent, bool readWrite )
    : ViewBase( part, parent ),
    m_readWrite( readWrite ),
    m_project( 0 )
{
    kDebug() <<" ---------------- KPlato: Creating GanttView ----------------";

    QVBoxLayout *l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_splitter = new QSplitter( this );
    l->addWidget( m_splitter );
    m_splitter->setOrientation( Qt::Vertical );

    m_gantt = new MyKDGanttView( m_splitter );

    setupGui();
    
    updateReadWrite( readWrite );
    //connect( m_gantt->constraintModel(), SIGNAL( constraintAdded( const Constraint& )), this, SLOT( update() ) );
    kDebug() <<m_gantt->constraintModel();
    
    connect( m_gantt->treeView(), SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), SLOT( slotContextMenuRequested( QModelIndex, const QPoint& ) ) );

    connect( m_gantt->treeView(), SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
}

KoPrintJob *GanttView::createPrintJob()
{
    return new GanttPrintingDialog( this, m_gantt );
}

void GanttView::setZoom( double )
{
    //kDebug() <<"setting gantt zoom:" << zoom;
    //m_gantt->setZoomFactor(zoom,true); NO!!! setZoomFactor() is something else
}

void GanttView::setupGui()
{
    createOptionAction();
}

void GanttView::slotOptions()
{
    kDebug();
    GanttViewSettingsDialog *dlg = new GanttViewSettingsDialog( m_gantt->treeView(), m_gantt->delegate(), this );
    if ( dlg->exec() == QDialog::Accepted ) {
        m_gantt->graphicsView()->updateScene();
    }
    delete dlg;
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
    //kDebug()<<id<<endl;
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
    kDebug();
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
    } else kDebug()<<"No node";
    if ( name.isEmpty() ) {
        slotHeaderContextMenuRequested( pos );
        kDebug()<<"No menu";
        return;
    }
    emit requestPopupMenu( name, pos );
}

bool GanttView::loadContext( const KoXmlElement &settings )
{
    kDebug();
    return m_gantt->loadContext( settings );
}

void GanttView::saveContext( QDomElement &settings ) const
{
    kDebug();
    m_gantt->saveContext( settings );
}

void GanttView::updateReadWrite( bool on )
{
    // TODO: KDGanttView needs read/write mode
    m_readWrite = on;
}

//------------------------
MilestoneKDGanttView::MilestoneKDGanttView( QWidget *parent )
    : GanttViewBase( parent ),
    m_manager( 0 )
{
    kDebug()<<"------------------- create MilestoneKDGanttView -----------------------";
    
    setItemModel( new MilestoneItemModel( this ) );
    
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

    static_cast<KDGantt::DateTimeGrid*>( grid() )->setDayWidth( 30 );
    //static_cast<KDGantt::DateTimeGrid*>( grid() )->setRowSeparators( treeView()->alternatingRowColors() );
}

MilestoneItemModel *MilestoneKDGanttView::model() const
{
    return static_cast<MilestoneItemModel*>( GanttViewBase::model() );
}

void MilestoneKDGanttView::setProject( Project *proj )
{
    if ( project() ) {
        disconnect( project(), SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
    }
    GanttViewBase::setProject( proj );
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
    //kDebug()<<id<<endl;
    model()->setManager( sm );
    m_manager = sm;
    if ( sm && m_project ) {
        QDateTime start;
        foreach ( const Node *n, model()->mileStones() ) {
            QDateTime nt = n->startTime( sm->id() ).dateTime();
            if ( ! nt.isValid() ) {
                continue;
            }
            if ( ! start.isValid() || start > nt ) {
                start = nt;
                kDebug()<<n->name()<<start;
            }
        }
        if ( ! start.isValid() ) {
            start = project()->startTime( sm->id() ).dateTime();
        }
        KDGantt::DateTimeGrid *g = static_cast<KDGantt::DateTimeGrid*>( grid() );
        start = start.addDays( -1 );
        kDebug()<<project()->startTime().toString()<<g->startDateTime()<<start;
        if ( g->startDateTime() !=  start ) {
            g->setStartDateTime( start );
        }
    }
}

//------------------------------------------

MilestoneGanttView::MilestoneGanttView( KoDocument *part, QWidget *parent, bool readWrite )
    : ViewBase( part, parent ),
        m_readWrite( readWrite ),
        m_project( 0 )
{
    kDebug() <<" ---------------- KPlato: Creating Milesone GanttView ----------------";

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
    //kDebug() <<"setting gantt zoom:" << zoom;
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
    //kDebug()<<id<<endl;
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
    kDebug();
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
    } else kDebug()<<"No node";
    if ( name.isEmpty() ) {
        kDebug()<<"No menu";
        slotHeaderContextMenuRequested( pos );
        return;
    }
    emit requestPopupMenu( name, pos );
}

void MilestoneGanttView::slotOptions()
{
    kDebug();
    ItemViewSettupDialog *dlg = new ItemViewSettupDialog( m_gantt->treeView(), true, this );
    dlg->exec();
    delete dlg;
}

bool MilestoneGanttView::loadContext( const KoXmlElement &settings )
{
    kDebug();
    return m_gantt->treeView()->loadContext( m_gantt->model()->columnMap(), settings );
}

void MilestoneGanttView::saveContext( QDomElement &settings ) const
{
    kDebug();
    return m_gantt->treeView()->saveContext( m_gantt->model()->columnMap(), settings );
}

void MilestoneGanttView::updateReadWrite( bool on )
{
    m_readWrite = on;
}

KoPrintJob *MilestoneGanttView::createPrintJob()
{
    return new GanttPrintingDialog( this, m_gantt );
}

//------------------------------------------

ResourceAppointmentsGanttView::ResourceAppointmentsGanttView( KoDocument *part, QWidget *parent, bool readWrite )
    : ViewBase( part, parent ),
    m_project( 0 ),
    m_model( new ResourceAppointmentsGanttModel( this ) )
{
    kDebug() <<" ---------------- KPlato: Creating ResourceAppointmentsGanttView ----------------";

    m_gantt = new KDGantt::View( this );
    m_gantt->graphicsView()->setItemDelegate( new ResourceGanttItemDelegate( m_gantt ) );

    GanttTreeView *tv = new GanttTreeView( m_gantt );
    tv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tv->setVerticalScrollMode( QAbstractItemView::ScrollPerPixel ); // needed since qt 4.2
    m_gantt->setLeftView( tv );
    m_gantt->setRowController( new KDGantt::TreeViewRowController( tv, m_gantt->ganttProxyModel() ) );
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

void ResourceAppointmentsGanttView::setZoom( double )
{
    //kDebug() <<"setting gantt zoom:" << zoom;
    //m_gantt->setZoomFactor(zoom,true); NO!!! setZoomFactor() is something else
}

Project *ResourceAppointmentsGanttView::project() const
{
    return m_model->project();
}

void ResourceAppointmentsGanttView::setProject( Project *project )
{
    static_cast<KDGantt::DateTimeGrid*>( m_gantt->grid() )->setStartDateTime( project->startTime().dateTime() );
    m_model->setProject( project );
}

void ResourceAppointmentsGanttView::setScheduleManager( ScheduleManager *sm )
{
    //kDebug()<<id<<endl;
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
    kDebug();
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
    kDebug();
    ItemViewSettupDialog *dlg = new ItemViewSettupDialog( treeView(), true, this );
    dlg->exec();
    delete dlg;
}

bool ResourceAppointmentsGanttView::loadContext( const KoXmlElement &settings )
{
    kDebug();
    return treeView()->loadContext( m_model->columnMap(), settings );;
}

void ResourceAppointmentsGanttView::saveContext( QDomElement &settings ) const
{
    kDebug();
    treeView()->saveContext( m_model->columnMap(), settings );
}

void ResourceAppointmentsGanttView::updateReadWrite( bool on )
{
    m_readWrite = on;
}

KoPrintJob *ResourceAppointmentsGanttView::createPrintJob()
{
    return 0;
}

}  //KPlato namespace

#include "kptganttview.moc"
