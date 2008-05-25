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
#include <kdgantttreeviewrowcontroller.h>

#include <KoDocument.h>

#include <kdebug.h>

#include <QSplitter>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDateTime>
#include <QMenu>
#include <QModelIndex>

#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>
#include <kaction.h>

namespace KPlato
{

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
    m_manager( 0 )
{
    kDebug()<<"------------------- create MyKDGanttView -----------------------";
    GanttTreeView *tv = new GanttTreeView( this );
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
    
    m_showExpected = true;
    m_showOptimistic = false;
    m_showPessimistic = false;
    m_showResources = false; // FIXME
    m_showTaskName = false; // FIXME
    m_showTaskLinks = false; // FIXME
    m_showProgress = false; //FIXME
    m_showPositiveFloat = false; //FIXME
    m_showCriticalTasks = false; //FIXME
    m_showCriticalPath = false; //FIXME
    m_showNoInformation = false; //FIXME
    m_showAppointments = false;

    m_taskView = new TaskAppointmentsView( m_splitter );
    m_taskView->hide();

    updateReadWrite( readWrite );
    //connect( m_gantt->constraintModel(), SIGNAL( constraintAdded( const Constraint& )), this, SLOT( update() ) );
    kDebug() <<m_gantt->constraintModel();
    
    connect( m_gantt->treeView(), SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), SLOT( slotContextMenuRequested( QModelIndex, const QPoint& ) ) );

    connect( m_gantt->treeView(), SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
}

void GanttView::setZoom( double )
{
    //kDebug() <<"setting gantt zoom:" << zoom;
    //m_gantt->setZoomFactor(zoom,true); NO!!! setZoomFactor() is something else
    //m_taskView->setZoom( zoom );
}

void GanttView::setupGui()
{
    actionOptions = new KAction(KIcon("configure"), i18n("Configure..."), this);
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );
}

void GanttView::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug();
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos,  lst.first() );
    }
}

void GanttView::slotOptions()
{
    kDebug();
    ItemViewSettupDialog dlg( m_gantt->treeView(), true );
    dlg.exec();
}

void GanttView::clear()
{
//    m_gantt->clear();
    m_taskView->clear();
}

void GanttView::setShowTaskLinks( bool on )
{
    m_showTaskLinks = on;
//    m_gantt->setShowTaskLinks( on );
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
    m_showTaskLinks = (bool)settings.attribute( "show-dependencies" , "0" ).toInt();
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
    actionOptions = new KAction(KIcon("configure"), i18n("Configure..."), this);
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );
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
        return;
    }
    emit requestPopupMenu( name, pos );
}

void MilestoneGanttView::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug();
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos,  lst.first() );
    }
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

}  //KPlato namespace

#include "kptganttview.moc"
