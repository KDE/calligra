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

#include <kdganttproxymodel.h>
#include <kdganttconstraintmodel.h>

#include <KoDocument.h>

#include <kdebug.h>

#include <QSplitter>
#include <QVBoxLayout>
#include <QHeaderView>

#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>

namespace KPlato
{

MyKDGanttView::MyKDGanttView( QWidget *parent )
    : KDGantt::View( parent ),
    m_project( 0 ),
    m_manager( 0 )
{
    kDebug()<<"------------------- create MyKDGanttView -----------------------"<<endl;
    setConstraintModel( new KDGantt::ConstraintModel() );
    KDGantt::ProxyModel *m = static_cast<KDGantt::ProxyModel*>( ganttProxyModel() );
    //m->setColumn( KDGantt::ItemTypeRole, 1 );
    m->setRole( KDGantt::ItemTypeRole, KDGantt::ItemTypeRole );
    m->setRole( KDGantt::StartTimeRole, KDGantt::StartTimeRole );
    m->setRole( KDGantt::EndTimeRole, KDGantt::EndTimeRole );
    m->setColumn( KDGantt::StartTimeRole, 18 );
    m->setColumn( KDGantt::EndTimeRole, 19 );
    m_model = new NodeItemModel( this );
    setModel( m_model );
    QTreeView *tv = dynamic_cast<QTreeView*>( leftView() ); //FIXME ?
    if ( tv ) {
        tv->header()->setStretchLastSection( true );
        // Only show name in treeview ;)
        for ( int i = 1; i < m_model->columnCount(); ++i ) {
            tv->hideColumn( i );
        }
    } else kDebug()<<"No treeview !!!"<<endl;
}

void MyKDGanttView::update()
{
    kDebug()<<endl;
    kDebug()<<"POULOU"<<endl;
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
}


void MyKDGanttView::addDependency( Relation *rel )
{
    QModelIndex par = m_model->index( rel->parent() );
    QModelIndex ch = m_model->index( rel->child() );
    qDebug()<<"addDependency() "<<m_model<<par.model();
    if ( par.isValid() && ch.isValid() ) {
        KDGantt::Constraint con( par, ch );
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
    KDGantt::Constraint con( par, ch );
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
}

void GanttView::setZoom( double )
{
    //kDebug() <<"setting gantt zoom:" << zoom;
    //m_gantt->setZoomFactor(zoom,true); NO!!! setZoomFactor() is something else
    //m_taskView->setZoom( zoom );
}

void GanttView::setupGui()
{
/*    actionViewGanttResources  = new KToggleAction(i18n("Resources"), this);
    actionCollection()->addAction("view_gantt_showResources", actionViewGanttResources );
    connect( actionViewGanttResources, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttResources() ) );
    actionViewGanttTaskName  = new KToggleAction(i18n("Task Name"), this);
    actionCollection()->addAction("view_gantt_showTaskName", actionViewGanttTaskName );
    connect( actionViewGanttTaskName, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttTaskName() ) );
    actionViewGanttTaskLinks  = new KToggleAction(i18n("Task Links"), this);
    actionCollection()->addAction("view_gantt_showTaskLinks", actionViewGanttTaskLinks );
    connect( actionViewGanttTaskLinks, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttTaskLinks() ) );
    actionViewGanttProgress  = new KToggleAction(i18n("Progress"), this);
    actionCollection()->addAction("view_gantt_showProgress", actionViewGanttProgress );
    connect( actionViewGanttProgress, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttProgress() ) );
    actionViewGanttFloat  = new KToggleAction(i18n("Float"), this);
    actionCollection()->addAction("view_gantt_showFloat", actionViewGanttFloat );
    connect( actionViewGanttFloat, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttFloat() ) );
    actionViewGanttCriticalTasks  = new KToggleAction(i18n("Critical Tasks"), this);
    actionCollection()->addAction("view_gantt_showCriticalTasks", actionViewGanttCriticalTasks );
    connect( actionViewGanttCriticalTasks, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttCriticalTasks() ) );
    actionViewGanttCriticalPath  = new KToggleAction(i18n("Critical Path"), this);
    actionCollection()->addAction("view_gantt_showCriticalPath", actionViewGanttCriticalPath );
    connect( actionViewGanttCriticalPath, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttCriticalPath() ) );

    actionViewGanttNotScheduled  = new KToggleAction(i18n("Not Scheduled"), this);
    actionCollection()->addAction("view_gantt_showNotScheduled", actionViewGanttNotScheduled );
    connect(actionViewGanttNotScheduled, SIGNAL(triggered(bool)), this, SLOT(slotViewGanttNotScheduled()));

    actionViewTaskAppointments  = new KToggleAction(i18n("Show allocations"), this);
    actionCollection()->addAction("view_task_appointments", actionViewTaskAppointments );
    connect( actionViewTaskAppointments, SIGNAL( triggered( bool ) ), SLOT( slotViewTaskAppointments() ) );
*/
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
//    return getNode( m_currentItem );
    return 0;
}

bool GanttView::loadContext( const KoXmlElement &settings )
{
    kDebug()<<endl;
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
    kDebug()<<endl;
/*    QDomElement elm = context.firstChildElement( objectName() );
    if ( elm.isNull() ) {
        return;
    }*/
/*    Context::Ganttview &context = c.ganttview;

    //kDebug();
    context.ganttviewsize = m_splitter->sizes() [ 0 ];
    context.taskviewsize = m_splitter->sizes() [ 1 ];
    //kDebug()<<"sizes="<<sizes()[0]<<","<<sizes()[1];
    if ( currentNode() ) {
        context.currentNode = currentNode() ->id();
    }
    context.showResources = m_showResources;
    context.showTaskName = m_showTaskName;*/
    settings.setAttribute( "show-dependencies", m_showTaskLinks );
/*    context.showProgress = m_showProgress;
    context.showPositiveFloat = m_showPositiveFloat;
    context.showCriticalTasks = m_showCriticalTasks;
    context.showCriticalPath = m_showCriticalPath;
    context.showNoInformation = m_showNoInformation;*/
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
    KDGantt::ProxyModel *m = static_cast<KDGantt::ProxyModel*>( ganttProxyModel() );
    //m->setColumn( KDGantt::ItemTypeRole, 1 );
    m->setRole( KDGantt::ItemTypeRole, KDGantt::ItemTypeRole );
    m->setRole( KDGantt::StartTimeRole, KDGantt::StartTimeRole );
    m->setRole( KDGantt::EndTimeRole, KDGantt::EndTimeRole );
    m->setColumn( KDGantt::StartTimeRole, 18 );
    m->setColumn( KDGantt::EndTimeRole, 19 );
    setModel( m_model );
    QTreeView *tv = dynamic_cast<QTreeView*>( leftView() ); //FIXME ?
    if ( tv ) {
        tv->header()->setStretchLastSection( true );
        // Only show name in treeview ;)
        for ( int i = 1; i < m_model->columnCount(); ++i ) {
            tv->hideColumn( i );
        }
    } else kDebug()<<"No treeview !!!"<<endl;
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

    m_gantt = new MilestoneKDGanttView( m_splitter );

    m_showTaskName = false; // FIXME
    m_showProgress = false; //FIXME
    m_showPositiveFloat = false; //FIXME
    m_showCriticalTasks = false; //FIXME
    m_showNoInformation = false; //FIXME

    updateReadWrite( readWrite );
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
//    return getNode( m_currentItem );
    return 0;
}

bool MilestoneGanttView::loadContext( const KoXmlElement &settings )
{
    kDebug()<<endl;
    return true;
}

void MilestoneGanttView::saveContext( QDomElement &settings ) const
{
    kDebug()<<endl;
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
