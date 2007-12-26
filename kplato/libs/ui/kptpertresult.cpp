/* This file is part of the KDE project
  Copyright (C) 2007 Florian Piquemal <flotueur@yahoo.fr>
  Copyright (C) 2007 Alexis Ménard <darktears31@gmail.com>
  Copyright (C) 2007 Dag Andersen <kplato@kde.org>

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
#include "kptpertresult.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptnode.h"
#include "kptschedule.h"
#include "kptitemviewsettup.h"

#include <KoDocument.h>

#include <QMenu>

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
  
class Project;
class Node;
class Task;


//-----------------------------------
PertResult::PertResult( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent ),
    m_node( 0 ),
    m_project( 0 )
{
    kDebug() << " ---------------- KPlato: Creating PertResult ----------------" << endl;
    widget.setupUi(this);
    PertResultItemModel *m = new PertResultItemModel();
    widget.treeWidgetTaskResult->setModel( m );
    widget.treeWidgetTaskResult->setStretchLastSection( false );

//    QHeaderView *header=widget.treeWidgetTaskResult->header();
    setupGui();
    
    current_schedule=0;

        
    QList<int> lst1; lst1 << 1 << -1;
    QList<int> lst2; 
    for ( int i = 0; i < 24; ++i ) {
        lst2 << i;
    }
    lst2 << 33 << -1;
    widget.treeWidgetTaskResult->hideColumns( lst1, lst2 );
    
    connect( widget.treeWidgetTaskResult, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
}

void PertResult::draw( Project &project)
{
    setProject( &project );
    //draw();
}
  
void PertResult::draw()
{
    kDebug()<<m_project;
    widget.scheduleName->setText( i18n( "None" ) );
    widget.totalFloat->clear();
    if ( m_project && model()->manager() && model()->manager()->isScheduled() ) {
        long id = model()->manager()->id();
        if ( id == -1 ) {
            return;
        }
        widget.scheduleName->setText( model()->manager()->name() );
        KLocale *locale =  KGlobal::locale();
        Duration f;
        foreach ( Node *n, m_project->allNodes() ) {
            if ( n->type() == Node::Type_Task || n->type() == Node::Type_Milestone ) {
                f += static_cast<Task*>( n )->positiveFloat( id );
            }
        }
        widget.totalFloat->setText( locale->formatNumber( f.toDouble( Duration::Unit_h ) ) );
    }
/*    kDebug() << "UPDATE PE" << endl;
      widget.treeWidgetTaskResult->clear();
      if ( current_schedule == 0 || current_schedule->id() == -1 ) {
          return;
    }
    KLocale * locale = KGlobal::locale();
    QList<Node*> list;
    QString res;
    testComplexGraph();
    foreach(Node * currentNode, m_project->childNodeIterator()){
        if (currentNode->type()!=4){
 
            QTreeWidgetItem * item = new QTreeWidgetItem(widget.treeWidgetTaskResult );
            item->setText(0, currentNode->id());
            item->setText(1, currentNode->name());
            item->setText(2,locale->formatDateTime(getStartEarlyDate(currentNode)));
            item->setText(3,locale->formatDateTime(getFinishEarlyDate(currentNode)));
            item->setText(4,locale->formatDateTime(getStartLateDate(currentNode)));
            item->setText(5,locale->formatDateTime(getFinishLateDate(currentNode)));
            item->setText(6,res.number(getTaskFloat(currentNode).days()));
            item->setText(7,res.number(getFreeMargin(currentNode).days()));
        }
        widget.totalFloat->setText(res.number(getProjectFloat(m_project).days()));

    }
    list=criticalPath();
    QList<Node*>::iterator it=list.begin();
    while(it!=list.end()) 
    {
         res+=(*it)->id();
         it++;
         if(it!=list.end()) res+=" - ";
    }
    widget.labelResultCriticalPath->setText(res);*/
}

DateTime PertResult::getStartEarlyDate(Node * currentNode)
{
    DateTime duration;
    Task * t;
    t=static_cast<Task *>(currentNode);
    //if the task has no parent so the early date start is 0
    duration=t->earlyStart(current_schedule->id());
    duration.setDateOnly(true);
    return duration;
}

DateTime PertResult::getFinishEarlyDate(Node * currentNode)
{
    //it's the early start date + duration of the task
    Task * t;
    t=static_cast<Task *>(currentNode);
    DateTime duration;

    duration=t->earlyFinish(current_schedule->id());
    duration.setDateOnly(true);
    return (duration);
}
 
DateTime PertResult::getStartLateDate(Node * currentNode)
{
    Task * t;
    DateTime duration;
    t=static_cast<Task *>(currentNode);
    duration=t->lateStart(current_schedule->id());
    duration.setDateOnly(true);
    return (duration);

}


DateTime PertResult::getFinishLateDate(Node * currentNode)
{
    DateTime duration;
    Task * t;
    t=static_cast<Task *>(currentNode);
    duration=t->lateFinish(current_schedule->id());
    duration.setDateOnly(true);
    return duration;
}

Duration PertResult::getProjectFloat(Project *project)
{
    Duration duration;
    foreach(Node * currentNode, project->childNodeIterator() )
    {
	duration=duration+getTaskFloat(currentNode);
    }
    //duration.setDayOnly(true);
    return duration;
}

Duration PertResult::getFreeMargin(Node * currentNode)
{
    //search the small duration of the nextest task
    Task * t;
    DateTime duration;
    for (QList<Relation*>::iterator it=currentNode->dependChildNodes().begin();it!=currentNode->dependChildNodes().end();it++)
    {
        if(it==currentNode->dependChildNodes().begin())
        {
	    duration=getStartEarlyDate((*it)->child());
	}
	t=static_cast<Task *>((*it)->child ());
        if(getStartEarlyDate((*it)->child ())<duration)
	{
	    duration=getStartEarlyDate((*it)->child ());
	}
    }
    t=static_cast<Task *>(currentNode);
    duration.setDateOnly(true);
    return duration-(getStartEarlyDate(currentNode)+=(t->endTime()-t->startTime())); 
}

Duration PertResult::getTaskFloat(Node * currentNode)
{
    if(currentNode->dependChildNodes().size()==0  && complexGraph==true)
    {
         return getFinishLateDate(currentNode)-getStartEarlyDate(currentNode);
    }
    else
    {
        return getFinishLateDate(currentNode)-getFinishEarlyDate(currentNode);
    }
}

QList<Node*> PertResult::criticalPath()
{
    QList<Node*> list;
    foreach(Node * currentNode, m_node->childNodeIterator() )
    {
	if(currentNode->dependChildNodes().size()==0 && getFinishLateDate(currentNode)==getStartEarlyDate(currentNode))
    	{
          list.push_back(currentNode) ;
    	}
	else
	{
	   if(getFinishLateDate(currentNode)==getFinishEarlyDate(currentNode))
	   {
               list.push_back(currentNode) ;
	   }
	}
    }
    return list;
}
void PertResult::testComplexGraph()
{
    complexGraph=false;
    foreach(Node * currentNode, m_node->childNodeIterator() )
    {
	if(currentNode->dependParentNodes().size()>1)
	{
	    complexGraph=true;
	}
    } 
}

void PertResult::setupGui()
{
    // Add the context menu actions for the view options
    connect(widget.treeWidgetTaskResult->actionSplitView(), SIGNAL(triggered(bool) ), SLOT(slotSplitView()));
    addContextAction( widget.treeWidgetTaskResult->actionSplitView() );
    
    actionOptions = new KAction( KIcon("configure"), i18n("Configure..."), this );
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );
}

void PertResult::slotSplitView()
{
    kDebug();
    widget.treeWidgetTaskResult->setViewSplitMode( ! widget.treeWidgetTaskResult->isViewSplit() );
}

void PertResult::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug();
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos,  lst.first() );
    }
}

void PertResult::slotOptions()
{
    kDebug();
    bool col0 = false;
    TreeViewBase *v = widget.treeWidgetTaskResult->slaveView();
    if ( v->isHidden() ) {
        v = widget.treeWidgetTaskResult->masterView();
        col0 = true;
    }
    ItemViewSettupDialog dlg( v, col0 );
    dlg.exec();
}

void PertResult::slotUpdate(){

    draw();
}

void PertResult::slotScheduleSelectionChanged( ScheduleManager *sm )
{
    current_schedule = sm;
    model()->setManager( sm );
    draw();
}

void PertResult::slotProjectCalculated( ScheduleManager *sm )
{
    if ( sm && sm == model()->manager() ) {
        //draw();
        slotScheduleSelectionChanged( sm );
    }
}

void PertResult::slotScheduleManagerToBeRemoved( const ScheduleManager *sm )
{
    if ( sm == model()->manager() ) {
        current_schedule = 0;
        model()->setManager( 0 );
//        draw(); // clears view
    }
}

void PertResult::slotScheduleManagerChanged( ScheduleManager *sm )
{
    if ( current_schedule && current_schedule == sm ) {
        slotScheduleSelectionChanged( sm );
    }
}

void PertResult::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotUpdate() ) );
        disconnect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
        disconnect( m_project, SIGNAL( scheduleManagerToBeRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerToBeRemoved( const ScheduleManager* ) ) );
        disconnect( m_project, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), this, SLOT( slotScheduleManagerChanged( ScheduleManager* ) ) );
    }
    m_project = project;
    widget.treeWidgetTaskResult->model()->setProject( m_project );
    if ( m_project ) {
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotUpdate() ) );
        connect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
        connect( m_project, SIGNAL( scheduleManagerToBeRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerToBeRemoved( const ScheduleManager* ) ) );
        connect( m_project, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), this, SLOT( slotScheduleManagerChanged( ScheduleManager* ) ) );
    }
    draw();
}

bool PertResult::loadContext( const KoXmlElement &context )
{
    kDebug();
    return widget.treeWidgetTaskResult->loadContext( context );
}

void PertResult::saveContext( QDomElement &context ) const
{
    widget.treeWidgetTaskResult->saveContext( context );
}


//--------------------
PertCpmView::PertCpmView( KoDocument *part, QWidget *parent ) 
    : ViewBase( part, parent ),
    m_project( 0 ),
    current_schedule( 0 ),
    block( false )
{
    kDebug() << " ---------------- KPlato: Creating PertCpmView ----------------" << endl;
    widget.setupUi(this);
    widget.probabilityFrame->setVisible( false );

    widget.cpmTable->setStretchLastSection ( false );
    CriticalPathItemModel *m = new CriticalPathItemModel();
    widget.cpmTable->setModel( m );
    
    setupGui();
    
    QList<int> lst1; lst1 << 1 << -1;
    QList<int> show; show << 5 << 18 << 19 << 20 << 21 << 33 << 34 << 35 << 36;
    QList<int> lst2;
    for ( int i = 0; i < m->columnCount(); ++i ) {
        if ( ! show.contains( i ) ) {
            lst2 << i;
        }
    }
    widget.cpmTable->hideColumns( lst1, lst2 );
    TreeViewBase *v = widget.cpmTable->slaveView();
    if ( v == 0 ) {
        v = widget.cpmTable->masterView();
    }
    v->mapToSection( 33, 0 );
    v->mapToSection( 34, 1 );
    v->mapToSection( 35, 2 );
    v->mapToSection( 36, 3 );
    v->mapToSection( 5, 4 );
    v->mapToSection( 18, 5 );
    v->mapToSection( 19, 6 );
    v->mapToSection( 20, 7 );
    v->mapToSection( 21, 8 );
    
    connect( widget.cpmTable, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
    
    connect( widget.finishTime, SIGNAL( dateTimeChanged( const QDateTime& ) ), SLOT( slotFinishTimeChanged( const QDateTime& ) ) );
    
    connect( widget.probability, SIGNAL( valueChanged( int ) ), SLOT( slotProbabilityChanged( int ) ) );
}

void PertCpmView::setupGui()
{
    // Add the context menu actions for the view options
    connect(widget.cpmTable->actionSplitView(), SIGNAL(triggered(bool) ), SLOT(slotSplitView()));
    addContextAction( widget.cpmTable->actionSplitView() );
    
    actionOptions = new KAction( KIcon("configure"), i18n("Configure..."), this );
    connect(actionOptions, SIGNAL(triggered(bool) ), SLOT(slotOptions()));
    addContextAction( actionOptions );
}

void PertCpmView::slotSplitView()
{
    kDebug();
    widget.cpmTable->setViewSplitMode( ! widget.cpmTable->isViewSplit() );
}

void PertCpmView::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug();
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos,  lst.first() );
    }
}

void PertCpmView::slotOptions()
{
    kDebug();
    bool col0 = false;
    TreeViewBase *v = widget.cpmTable->slaveView();
    if ( v->isHidden() ) {
        v = widget.cpmTable->masterView();
        col0 = true;
    }
    ItemViewSettupDialog dlg( v, col0 );
    dlg.exec();
}

void PertCpmView::slotScheduleSelectionChanged( ScheduleManager *sm )
{
    bool enbl = sm && sm->isScheduled() && sm->usePert();
    widget.probabilityFrame->setVisible( enbl );
    current_schedule = sm;
    model()->setManager( sm );
    draw();
}

void PertCpmView::slotProjectCalculated( ScheduleManager *sm )
{
    if ( sm && sm == model()->manager() ) {
        slotScheduleSelectionChanged( sm );
    }
}

void PertCpmView::slotScheduleManagerChanged( ScheduleManager *sm )
{
    if ( current_schedule == sm ) {
        slotScheduleSelectionChanged( sm );
    }
}

void PertCpmView::slotScheduleManagerToBeRemoved( const ScheduleManager *sm )
{
    if ( sm == current_schedule ) {
        current_schedule = 0;
        model()->setManager( 0 );
        widget.probabilityFrame->setVisible( false );
    }
}

void PertCpmView::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotUpdate() ) );
        disconnect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
        disconnect( m_project, SIGNAL( scheduleManagerToBeRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerToBeRemoved( const ScheduleManager* ) ) );
        disconnect( m_project, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), this, SLOT( slotScheduleManagerChanged( ScheduleManager* ) ) );
    }
    m_project = project;
    widget.cpmTable->model()->setProject( m_project );
    if ( m_project ) {
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotUpdate() ) );
        connect( m_project, SIGNAL( projectCalculated( ScheduleManager* ) ), this, SLOT( slotProjectCalculated( ScheduleManager* ) ) );
        connect( m_project, SIGNAL( scheduleManagerToBeRemoved( const ScheduleManager* ) ), this, SLOT( slotScheduleManagerToBeRemoved( const ScheduleManager* ) ) );
        connect( m_project, SIGNAL( scheduleManagerChanged( ScheduleManager* ) ), this, SLOT( slotScheduleManagerChanged( ScheduleManager* ) ) );
    }
    draw();
}

void PertCpmView::draw( Project &project )
{
    setProject( &project );
    // draw()
}

void PertCpmView::draw()
{
    widget.scheduleName->setText( i18n( "None" ) );
    if ( m_project && current_schedule && current_schedule->isScheduled() ) {
        long id = current_schedule->id();
        if ( id == -1 ) {
            return;
        }
        widget.scheduleName->setText( current_schedule->name() );
        widget.finishTime->setDateTime( m_project->endTime( id ).dateTime() );
    }
}

void PertCpmView::slotFinishTimeChanged( const QDateTime &dt )
{
    kDebug()<<dt;
    if ( block || m_project == 0 || current_schedule == 0 ) {
        return;
    }
    block = true;
    double var = model()->variance( Qt::EditRole ).toDouble();
    double dev = sqrt( var );
    DateTime et = m_project->endTime( current_schedule->id() );
    DateTime t = DateTime( KDateTime( dt ) );
    double d = ( et - t ).toDouble();
    d = t < et ? -d : d;
    double z = d / dev;
    double v = probability( z );
    widget.probability->setValue( (int)( v * 100 ) );
    kDebug()<<z<<", "<<v;
    block = false;
}

void PertCpmView::slotProbabilityChanged( int value )
{
    kDebug()<<value;
    if ( value == 0 || block || m_project == 0 || current_schedule == 0 ) {
        return;
    }
    block = true;
    double var = model()->variance( Qt::EditRole ).toDouble();
    double dev = sqrt( var );
    DateTime et = m_project->endTime( current_schedule->id() );
    double p = valueZ( value );
    DateTime t = et + Duration( qint64( p * dev ) );
    widget.finishTime->setDateTime( t.dateTime() );
    kDebug()<<p<<", "<<t.toString();
    block = false;
}

double PertCpmView::probability( double z ) const
{
    // TODO proper table
    double dist[][2] = { {0.0, 0.5}, {0.68, 0.7517}, {3.0, 0.9987}  };
    double p = 1.0;
    int i = 1;
    for ( ; i < 3; ++i ) {
        if ( QABS( z ) <= dist[i][0] ) {
            break;
        }
    }
    p = dist[i-1][1] + ( ( dist[i][1] - dist[i-1][1] ) * ( ( QABS(z) - dist[i-1][0] ) / (dist[i][0] - dist[i-1][0] ) ) );
    return z < 0 ? 1- p : p;
}

double PertCpmView::valueZ( double pr ) const
{
    // TODO proper table
    double prob[][2] = { {50.0, 0.0}, {75.0, 0.674}, {99.0, 2.326}  };
    double p = pr >= 50.0 ? pr : 100.0 - pr;
    double z = 3.0;
    int i = 1;
    for ( ; i < 3; ++i ) {
        if ( p < prob[i][0] ) {
            break;
        }
    }
    z = prob[i-1][1] + ( ( prob[i][1] - prob[i-1][1] ) * ( ( p - prob[i-1][0] ) / (prob[i][0] - prob[i-1][0] ) ) );
    return pr < 50.0 ? -z : z;
}

void PertCpmView::slotUpdate()
{
    draw();
}

bool PertCpmView::loadContext( const KoXmlElement &context )
{
    kDebug()<<objectName();
    return widget.cpmTable->loadContext( context );
}

void PertCpmView::saveContext( QDomElement &context ) const
{
    widget.cpmTable->saveContext( context );
}

} // namespace KPlato

#include "kptpertresult.moc"
