/* This file is part of the KDE project
  Copyright (C) 2002 - 2005 Dag Andersen <danders@get2net.dk>
  Copyright (C) 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation;
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
#include "kdganttproxymodel.h"

#include "kptnodeitemmodel.h"
#include "kptappointment.h"
#include "kptpart.h"
#include "kptview.h"
//#include "kptcanvasitem.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"
#include "kptdatetime.h"
#include "kpttaskappointmentsview.h"
#include "kptrelation.h"
#include "kptcontext.h"
#include "kptschedule.h"

#include "kdganttview.h"

#include <kdebug.h>

#include <QHeaderView>
#include <QLayout>
#include <QSplitter>

#include <klocale.h>
#include <kglobal.h>
#include <kprinter.h>
#include <kmessagebox.h>

namespace KPlato
{

class MyKDGanttView : public KDGantt::View
{
public:
    MyKDGanttView( Part *part, QWidget *parent )
        : KDGantt::View( parent )
    {
        KDGantt::ProxyModel *m = static_cast<KDGantt::ProxyModel*>( ganttProxyModel() );
        //m->setColumn( KDGantt::ItemTypeRole, 1 );
        m->setRole( KDGantt::ItemTypeRole, KDGantt::ItemTypeRole );
        m->setColumn( KDGantt::StartTimeRole, 18 );
        m->setColumn( KDGantt::EndTimeRole, 19 );
        m_model = new NodeItemModel( part, this );
        setModel( m_model );
        QTreeView *tv = dynamic_cast<QTreeView*>( leftView() ); //FIXME ?
        if ( tv ) {
            tv->header()->setStretchLastSection( true );
            // Only show name in treeview ;)
            tv->hideColumn( 1 );
            tv->hideColumn( 2 );
            tv->hideColumn( 3 );
            tv->hideColumn( 4 );
            tv->hideColumn( 5 );
            tv->hideColumn( 6 );
            tv->hideColumn( 7 );
            tv->hideColumn( 8 );
            tv->hideColumn( 9 );
            tv->hideColumn( 10 );
            tv->hideColumn( 11 );
            tv->hideColumn( 12 );
            tv->hideColumn( 13 );
            tv->hideColumn( 14 );
            tv->hideColumn( 15 );
            tv->hideColumn( 16 );
            tv->hideColumn( 17 );
            tv->hideColumn( 18 );
            tv->hideColumn( 19 );
        } else kDebug()<<k_funcinfo<<"No treeview !!!"<<endl;
    }
    NodeItemModel *itemModel() const { return m_model; }
    void setProject( Project *project ) { m_model->setProject( project ); }

protected:
    NodeItemModel *m_model;
};

GanttView::GanttView( Part *part, QWidget *parent, bool readWrite )
        : ViewBase( part, parent ),
        m_readWrite( readWrite ),
        m_taskView( 0 ),
        m_project( 0 ),
        m_id( -1 )
{
    kDebug() << " ---------------- KPlato: Creating GanttView ----------------" << endl;

    QVBoxLayout *l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_splitter = new QSplitter( this );
    l->addWidget( m_splitter );
    m_splitter->setOrientation( Qt::Vertical );

    m_gantt = new MyKDGanttView( part, m_splitter );

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

    setReadWriteMode( readWrite );

}

void GanttView::setZoom( double )
{
    //kDebug() << "setting gantt zoom: " << zoom << endl;
    //m_gantt->setZoomFactor(zoom,true); NO!!! setZoomFactor() is something else
    //m_taskView->setZoom( zoom );
}

void GanttView::show()
{
    //m_gantt->show();
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

void GanttView::slotScheduleIdChanged( long id )
{
    //kDebug()<<k_funcinfo<<id<<endl;
    m_id = id;
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

bool GanttView::setContext( const Context &c )
{
    //kDebug()<<k_funcinfo<<endl;
    const Context::Ganttview &context = c.ganttview;
    
    Q3ValueList<int> list = m_splitter->sizes();
    list[ 0 ] = context.ganttviewsize;
    list[ 1 ] = context.taskviewsize;
    m_splitter->setSizes( list );

    //TODO this does not work yet!
    //     currentItemChanged(findItem(project.findNode(context.currentNode)));

    m_showResources = context.showResources ;
    m_showTaskName = context.showTaskName;
    m_showTaskLinks = context.showTaskLinks;
    m_showProgress = context.showProgress;
    m_showPositiveFloat = context.showPositiveFloat;
    m_showCriticalTasks = context.showCriticalTasks;
    m_showCriticalPath = context.showCriticalPath;
    m_showNoInformation = context.showNoInformation;

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

void GanttView::getContext( Context &c ) const
{
    Context::Ganttview &context = c.ganttview;

    //kDebug()<<k_funcinfo<<endl;
    context.ganttviewsize = m_splitter->sizes() [ 0 ];
    context.taskviewsize = m_splitter->sizes() [ 1 ];
    //kDebug()<<k_funcinfo<<"sizes="<<sizes()[0]<<","<<sizes()[1]<<endl;
    if ( currentNode() ) {
        context.currentNode = currentNode() ->id();
    }
    context.showResources = m_showResources;
    context.showTaskName = m_showTaskName;
    context.showTaskLinks = m_showTaskLinks;
    context.showProgress = m_showProgress;
    context.showPositiveFloat = m_showPositiveFloat;
    context.showCriticalTasks = m_showCriticalTasks;
    context.showCriticalPath = m_showCriticalPath;
    context.showNoInformation = m_showNoInformation;
}

void GanttView::setReadWriteMode( bool on )
{
    m_readWrite = on;
}

}  //KPlato namespace

#include "kptganttview.moc"
