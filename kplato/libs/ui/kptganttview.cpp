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
#include "kpttaskappointmentsview.h"
#include "kptrelation.h"
#include "kptschedule.h"
#include "kptviewbase.h"
#include "kptitemviewsettup.h"

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

GanttItemDelegate::GanttItemDelegate( QObject *parent )
    : KDGantt::ItemDelegate( parent ),
    showResources( false ),
    showTaskName( false ),
    showTaskLinks( false ),
    showProgress( false ),
    showPositiveFloat( false ),
    showCriticalPath( false ),
    showCriticalTasks( false ),
    showAppointments( false )
{
}

QString GanttItemDelegate::itemText( const QModelIndex& idx, int type ) const
{
    QString txt;
    if ( showTaskName ) {
        QModelIndex i = idx.model()->index( idx.row(), NodeModel::NodeName, idx.parent() );
        txt = i.data( Qt::DisplayRole ).toString();
    }
    if ( type == KDGantt::TypeTask && showResources ) {
        QModelIndex i = idx.model()->index( idx.row(), NodeModel::NodeAllocation, idx.parent() );
        if ( ! txt.isEmpty() ) {
            txt += ' ';
        }
        txt += '(' + i.data( Qt::DisplayRole ).toString() + ')';
    }
    kDebug()<<txt;
    return txt;
}

KDGantt::Span GanttItemDelegate::itemBoundingSpan( const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx ) const
{
    kDebug()<<opt<<idx;
    if ( !idx.isValid() ) return KDGantt::Span();

    int typ = idx.model()->data( idx, KDGantt::ItemTypeRole ).toInt();
    QString txt = itemText( idx, typ );
    
    QRectF itemRect = opt.itemRect;
    if (  typ == KDGantt::TypeEvent ) {
        itemRect = QRectF( itemRect.left()-itemRect.height()/2., itemRect.top(), itemRect.height(), itemRect.height() );
    }

    int tw = opt.fontMetrics.width( txt );
    tw += static_cast<int>( itemRect.height()/2. );
    switch ( opt.displayPosition ) {
        case KDGantt::StyleOptionGanttItem::Left:
            return KDGantt::Span( itemRect.left()-tw, itemRect.width()+tw );
        case KDGantt::StyleOptionGanttItem::Right:
            return KDGantt::Span( itemRect.left(), itemRect.width()+tw );
        case KDGantt::StyleOptionGanttItem::Center:
            return KDGantt::Span( itemRect.left(), itemRect.width() );
    }
    return KDGantt::Span();
}

/*! Paints the gantt item \a idx using \a painter and \a opt
 */
void GanttItemDelegate::paintGanttItem( QPainter* painter, const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx )
{
    if ( !idx.isValid() ) return;
    
    const KDGantt::ItemType typ = static_cast<KDGantt::ItemType>( idx.model()->data( idx, KDGantt::ItemTypeRole ).toInt() );
    
    QString txt = itemText( idx, typ );
    QRectF itemRect = opt.itemRect;
    QRectF boundingRect = opt.boundingRect;
    boundingRect.setY( itemRect.y() );
    boundingRect.setHeight( itemRect.height() );

    painter->save();

    QPen pen = defaultPen( typ );
    if ( opt.state & QStyle::State_Selected ) pen.setWidth( 2*pen.width() );
    painter->setPen( pen );
    painter->setBrush( defaultBrush( typ ) );

    qreal pw = painter->pen().width()/2.;
    switch( typ ) {
    case KDGantt::TypeTask:
        if ( itemRect.isValid() ) {
            qreal pw = painter->pen().width()/2.;
            pw-=1;
            QRectF r = itemRect;
            r.translate( 0., r.height()/6. );
            r.setHeight( 2.*r.height()/3. );
            painter->setBrushOrigin( itemRect.topLeft() );
            painter->save();
            painter->translate( 0.5, 0.5 );
            painter->drawRect( r );
            if ( showProgress ) {
                bool ok;
                qreal completion = idx.model()->data( idx, KDGantt::TaskCompletionRole ).toDouble( &ok );
                if ( ok ) {
                    qreal h = r.height();
                    QRectF cr( r.x(), r.y()+h/4. + 1,
                            r.width()*completion/100., h/2. - 2 );
                    painter->fillRect( cr, painter->pen().brush() );
                }
            }
            painter->restore();
            Qt::Alignment ta;
            switch( opt.displayPosition ) {
            case KDGantt::StyleOptionGanttItem::Left: ta = Qt::AlignLeft; break;
            case KDGantt::StyleOptionGanttItem::Right: ta = Qt::AlignRight; break;
            case KDGantt::StyleOptionGanttItem::Center: ta = Qt::AlignCenter; break;
            }
            painter->drawText( boundingRect, ta, txt );
        }
        break;
    case KDGantt::TypeSummary:
        if ( opt.itemRect.isValid() ) {
            // TODO
            pw-=1;
            const QRectF r = QRectF( opt.itemRect ).adjusted( -pw, -pw, pw, pw );
            QPainterPath path;
            const qreal delta = r.height()/2.;
            path.moveTo( r.topLeft() );
            path.lineTo( r.topRight() );
            path.lineTo( QPointF( r.right(), r.top() + 2.*delta ) );
            //path.lineTo( QPointF( r.right()-3./2.*delta, r.top() + delta ) );
            path.quadTo( QPointF( r.right()-.5*delta, r.top() + delta ), QPointF( r.right()-2.*delta, r.top() + delta ) );
            //path.lineTo( QPointF( r.left()+3./2.*delta, r.top() + delta ) );
            path.lineTo( QPointF( r.left() + 2.*delta, r.top() + delta ) );
            path.quadTo( QPointF( r.left()+.5*delta, r.top() + delta ), QPointF( r.left(), r.top() + 2.*delta ) );
            path.closeSubpath();
            painter->setBrushOrigin( itemRect.topLeft() );
            painter->save();
            painter->translate( 0.5, 0.5 );
            painter->drawPath( path );
            painter->restore();
            Qt::Alignment ta;
            switch( opt.displayPosition ) {
            case KDGantt::StyleOptionGanttItem::Left: ta = Qt::AlignLeft; break;
            case KDGantt::StyleOptionGanttItem::Right: ta = Qt::AlignRight; break;
            case KDGantt::StyleOptionGanttItem::Center: ta = Qt::AlignCenter; break;
            }
            painter->drawText( boundingRect, ta | Qt::AlignVCenter, txt );
        }
        break;
    case KDGantt::TypeEvent:
        //qDebug() << opt.boundingRect << opt.itemRect;
        if ( opt.boundingRect.isValid() ) {
            const qreal pw = painter->pen().width() / 2. - 1;
            const QRectF r = QRectF( opt.rect ).adjusted( -pw, -pw, pw, pw );
            QPainterPath path;
            const qreal delta = static_cast< int >( r.height() / 2 );
            path.moveTo( delta, 0. );
            path.lineTo( 2.*delta, delta );
            path.lineTo( delta, 2.*delta );
            path.lineTo( 0., delta );
            path.closeSubpath();
            painter->save();
            painter->translate( r.topLeft() );
            painter->translate( 0.5, 0.5 );
            painter->drawPath( path );
            painter->restore();
            Qt::Alignment ta;
            switch( opt.displayPosition ) {
            case KDGantt::StyleOptionGanttItem::Left: ta = Qt::AlignLeft; break;
            case KDGantt::StyleOptionGanttItem::Right: ta = Qt::AlignRight; break;
            case KDGantt::StyleOptionGanttItem::Center: ta = Qt::AlignCenter; break;
            }
            painter->drawText( boundingRect, ta | Qt::AlignVCenter, txt );
        }
        break;
    default:
        break;
    }
    painter->restore();
}

//------------------------------------------------
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
    : ItemViewSettupDialog( view, parent )
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
    setWindowTitle( "Options" );
}

//----------------
GanttPrintingDialog::GanttPrintingDialog( ViewBase *view, KDGantt::View *gantt )
    : PrintingDialog( view ),
    m_gantt( gantt ),
    m_singlePage( true ),
    m_printRowLabels( true )
{
    m_pageRect = printer().pageRect();
    m_pageRect.setSize( m_pageRect.size() - QSize( -30, 30 ) ); // TEST
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
    m_singlePage = m_options->singlePage();
    //m_printRowLabels = m_options->printRowLabels();
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

MyKDGanttView::MyKDGanttView( QWidget *parent )
    : KDGantt::View( parent ),
    m_project( 0 ),
    m_manager( 0 ),
    m_ganttdelegate( new GanttItemDelegate( this ) )
{
    kDebug()<<"------------------- create MyKDGanttView -----------------------";
    GanttTreeView *tv = new GanttTreeView( this );
    tv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setLeftView( tv );
    setRowController( new KDGantt::TreeViewRowController( tv, ganttProxyModel() ) );
    m_model = new NodeItemModel( tv );
    setModel( m_model );
    
    tv->header()->setStretchLastSection( true );
    QList<int> show;
    show << NodeModel::NodeName
            << NodeModel::NodeCompleted
            << NodeModel::NodeStartTime
            << NodeModel::NodeEndTime;

    tv->setDefaultColumns( show );
    for ( int i = 0; i < model()->columnCount(); ++i ) {
        if ( ! show.contains( i ) ) {
            tv->hideColumn( i );
        }
    }

    graphicsView()->setItemDelegate( m_ganttdelegate );
    
    setConstraintModel( new KDGantt::ConstraintModel() );
    KDGantt::ProxyModel *m = static_cast<KDGantt::ProxyModel*>( ganttProxyModel() );

    m->setRole( KDGantt::ItemTypeRole, KDGantt::ItemTypeRole ); // To provide correct format
    m->setRole( KDGantt::StartTimeRole, KDGantt::StartTimeRole ); // To provide correct format
    m->setRole( KDGantt::EndTimeRole, KDGantt::EndTimeRole ); // To provide correct format
    
    m->setColumn( KDGantt::ItemTypeRole, NodeModel::NodeType );
    m->setColumn( KDGantt::StartTimeRole, NodeModel::NodeStartTime );
    m->setColumn( KDGantt::EndTimeRole, NodeModel::NodeEndTime );
    m->setColumn( KDGantt::TaskCompletionRole, NodeModel::NodeCompleted );
    
    static_cast<KDGantt::DateTimeGrid*>( grid() )->setDayWidth( 30 );
    static_cast<KDGantt::DateTimeGrid*>( grid() )->setRowSeparators( tv->alternatingRowColors() );
    
    connect( m_model, SIGNAL( nodeInserted( Node* ) ), this, SLOT( slotNodeInserted( Node* ) ) );
}

void MyKDGanttView::update()
{
    kDebug()<<endl;
    kDebug()<<"POULOU"<<endl;
}

GanttTreeView *MyKDGanttView::treeView() const
{
    QAbstractItemView *v = const_cast<QAbstractItemView*>( leftView() );
    return static_cast<GanttTreeView*>( v );
}

void MyKDGanttView::setProject( Project *project )
{
    clearDependencies();
    if ( m_project ) {
        disconnect( m_project, SIGNAL( relationAdded( Relation* ) ), this, SLOT( addDependency( Relation* ) ) );
        disconnect( m_project, SIGNAL( relationToBeRemoved( Relation* ) ), this, SLOT( removeDependency( Relation* ) ) );
        disconnect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
    }
    m_model->setProject( project );
    m_project = project;
    if ( m_project ) {
        connect( m_project, SIGNAL( relationAdded( Relation* ) ), this, SLOT( addDependency( Relation* ) ) );
        connect( m_project, SIGNAL( relationToBeRemoved( Relation* ) ), this, SLOT( removeDependency( Relation* ) ) );
        connect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
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
    m_model->setManager( sm );
    m_manager = sm;
    createDependencies();
    if ( sm && m_project ) {
        QDateTime start = m_project->startTime( sm->id() ).dateTime().addDays( -1 );
        KDGantt::DateTimeGrid *g = static_cast<KDGantt::DateTimeGrid*>( grid() );
        if ( g->startDateTime() !=  start ) {
            g->setStartDateTime( start );
        }
    }
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
    QModelIndex par = m_model->index( rel->parent() );
    QModelIndex ch = m_model->index( rel->child() );
    kDebug()<<"addDependency() "<<m_model<<par.model();
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
    QModelIndex par = m_model->index( rel->parent() );
    QModelIndex ch = m_model->index( rel->child() );
    qDebug()<<"removeDependency() "<<m_model<<par.model();
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
    if ( m_project == 0 || m_manager == 0 ) {
        return;
    }
    foreach ( Node* n, m_project->allNodes() ) {
        foreach ( Relation *r, n->dependChildNodes() ) {
            addDependency( r );
        }
    }
}

//------------------------------------------
GanttView::GanttView( KoDocument *part, QWidget *parent, bool readWrite )
        : ViewBase( part, parent ),
        m_readWrite( readWrite ),
        m_taskView( 0 ),
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
    
    m_taskView = new TaskAppointmentsView( m_splitter );
    m_taskView->hide();

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
    //m_taskView->setZoom( zoom );
}

void GanttView::setupGui()
{
    createOptionAction();
}

void GanttView::slotOptions()
{
    kDebug();
    GanttViewSettingsDialog dlg( m_gantt->treeView(), m_gantt->delegate() );
    if ( dlg.exec() == QDialog::Accepted ) {
        m_gantt->graphicsView()->updateScene();
    }
}

void GanttView::clear()
{
//    m_gantt->clear();
    m_taskView->clear();
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
    return m_gantt->model()->node( m_gantt->treeView()->selectionModel()->currentIndex() );
}

void GanttView::slotContextMenuRequested( QModelIndex idx, const QPoint &pos )
{
    kDebug();
    QString name;
    Node *node = m_gantt->model()->node( idx );
    if ( node ) {
        switch ( node->type() ) {
            case Node::Type_Task:
                name = "taskview_popup";
                break;
            case Node::Type_Milestone:
                name = "taskview_milestone_popup";
                break;
            case Node::Type_Summarytask:
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
    return m_gantt->treeView()->loadContext( m_gantt->model()->columnMap(), settings );
/*    QDomElement elm = context.firstChildElement( objectName() );
    if ( elm.isNull() ) {
        return false;
    }*/
    
//     Q3ValueList<int> list = m_splitter->sizes();
//     list[ 0 ] = context.ganttviewsize;
//     list[ 1 ] = context.taskviewsize;
//     m_splitter->setSizes( list );

    //TODO this does not work yet!
    //     currentItemChanged(findItem(project.findNode(context.currentNode)));

/*    m_showResources = context.showResources ;
    m_showTaskName = context.showTaskName;*/
//    m_showTaskLinks = (bool)settings.attribute( "show-dependencies" , "0" ).toInt();
/*    m_showProgress = context.showProgress;
    m_showPositiveFloat = context.showPositiveFloat;
    m_showCriticalTasks = context.showCriticalTasks;
    m_showCriticalPath = context.showCriticalPath;
    m_showNoInformation = context.showNoInformation;*/
    //TODO this does not work yet!
    //     getContextClosedNodes(context, m_gantt->firstChild());
    //     for (QStringList::ConstIterator it = context.closedNodes.begin(); it != context.closedNodes.end(); ++it) {
    //         KDGanttViewItem *item = findItem(m_project->findNode(*it));
    //         if (item) {
    //             item->setOpen(false);
    //         }
    //     }
    return true;
}

void GanttView::saveContext( QDomElement &settings ) const
{
    kDebug();
    m_gantt->treeView()->saveContext( m_gantt->model()->columnMap(), settings );
}

void GanttView::updateReadWrite( bool on )
{
    // TODO: KDGanttView needs read/write mode
    m_readWrite = on;
}

void GanttView::update()
{
    kDebug();
    kDebug()<<"POULOU";
}

//------------------------
MilestoneKDGanttView::MilestoneKDGanttView( QWidget *parent )
    : KDGantt::View( parent ),
    m_project( 0 ),
    m_manager( 0 ),
    m_model( new MilestoneItemModel( this ) )
{
    kDebug()<<"------------------- create MilestoneKDGanttView -----------------------"<<endl;
    GanttTreeView *tv = new GanttTreeView( this );
    tv->setRootIsDecorated( false );
    tv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setLeftView( tv );
    setRowController( new KDGantt::TreeViewRowController( tv, ganttProxyModel() ) );
    m_model = new MilestoneItemModel( tv );
    setModel( m_model );
    
    tv->header()->setStretchLastSection( true );
    QList<int> show;
    show << NodeModel::NodeName
            << NodeModel::NodeStartTime;

    tv->setDefaultColumns( show );
    for ( int i = 0; i < model()->columnCount(); ++i ) {
        if ( ! show.contains( i ) ) {
            tv->hideColumn( i );
        }
    }

    KDGantt::ProxyModel *m = static_cast<KDGantt::ProxyModel*>( ganttProxyModel() );
    
    m->setRole( KDGantt::ItemTypeRole, KDGantt::ItemTypeRole ); // To provide correct format
    m->setRole( KDGantt::StartTimeRole, KDGantt::StartTimeRole ); // To provide correct format
    m->setRole( KDGantt::EndTimeRole, KDGantt::EndTimeRole ); // To provide correct format
    
    m->setColumn( KDGantt::ItemTypeRole, NodeModel::NodeType );
    m->setColumn( KDGantt::StartTimeRole, NodeModel::NodeStartTime );
    m->setColumn( KDGantt::EndTimeRole, NodeModel::NodeEndTime );
    m->setColumn( KDGantt::TaskCompletionRole, NodeModel::NodeCompleted );

    static_cast<KDGantt::DateTimeGrid*>( grid() )->setDayWidth( 30 );
    static_cast<KDGantt::DateTimeGrid*>( grid() )->setRowSeparators( tv->alternatingRowColors() );
}

void MilestoneKDGanttView::update()
{
}

void MilestoneKDGanttView::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
    }
    m_model->setProject( project );
    m_project = project;
    if ( m_project ) {
        connect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
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
    m_model->setManager( sm );
    m_manager = sm;
    if ( sm && m_project ) {
        QDateTime start;
        foreach ( const Node *n, m_model->mileStones() ) {
            if ( ! start.isValid() || start > n->startTime().dateTime() ) {
                start = n->startTime( sm->id() ).dateTime();
            }
        }
        if ( ! start.isValid() ) {
            start = m_project->startTime( sm->id() ).dateTime();
        }
        KDGantt::DateTimeGrid *g = static_cast<KDGantt::DateTimeGrid*>( grid() );
        start = start.addDays( -1 );
        if ( g->startDateTime() !=  start ) {
            g->setStartDateTime( start );
        }
    }
}

GanttTreeView *MilestoneKDGanttView::treeView() const
{
    QAbstractItemView *v = const_cast<QAbstractItemView*>( leftView() );
    return static_cast<GanttTreeView*>( v );
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
    return m_gantt->model()->node( m_gantt->treeView()->selectionModel()->currentIndex() );
}

void MilestoneGanttView::setupGui()
{
    createOptionAction();
}

void MilestoneGanttView::slotContextMenuRequested( QModelIndex idx, const QPoint &pos )
{
    kDebug();
    QString name;
    Node *node = m_gantt->model()->node( idx );
    if ( node ) {
        switch ( node->type() ) {
            case Node::Type_Task:
                name = "taskview_popup";
                break;
            case Node::Type_Milestone:
                name = "taskview_milestone_popup";
                break;
            case Node::Type_Summarytask:
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
    ItemViewSettupDialog dlg( m_gantt->treeView(), true );
    dlg.exec();
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

void MilestoneGanttView::update()
{
}

KoPrintJob *MilestoneGanttView::createPrintJob()
{
    return new GanttPrintingDialog( this, m_gantt );
}


}  //KPlato namespace

#include "kptganttview.moc"
