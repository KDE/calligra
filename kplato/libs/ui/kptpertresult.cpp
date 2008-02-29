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

#include <QAbstractItemView>
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


static double dist[][2] = {
    {0.00, 0.5000}, {0.02, 0.5080}, {0.04, 0.5160}, {0.06, 0.5239}, {0.08, 0.5319},
    {0.10, 0.5398}, {0.12, 0.5478}, {0.14, 0.5557}, {0.16, 0.5636}, {0.18, 0.5714},
    {0.20, 0.5793}, {0.22, 0.5871}, {0.24, 0.5948}, {0.26, 0.6026}, {0.28, 0.6103},
    {0.30, 0.6179}, {0.32, 0.6255}, {0.34, 0.6331}, {0.36, 0.6406}, {0.38, 0.6480},
    {0.40, 0.6554}, {0.42, 0.6628}, {0.44, 0.6700}, {0.46, 0.6772}, {0.48, 0.6844},
    {0.50, 0.6915}, {0.52, 0.6985}, {0.54, 0.7054}, {0.56, 0.7123}, {0.58, 0.7190},
    {0.60, 0.7257}, {0.62, 0.7324}, {0.64, 0.7389}, {0.66, 0.7454}, {0.68, 0.7517},
    {0.70, 0.7580}, {0.72, 0.7642}, {0.74, 0.7704}, {0.76, 0.7764}, {0.78, 0.7823},
    {0.80, 0.7881}, {0.82, 0.7939}, {0.84, 0.7995}, {0.86, 0.8051}, {0.88, 0.8106},
    {0.90, 0.8159}, {0.92, 0.8212}, {0.94, 0.8264}, {0.96, 0.8315}, {0.98, 0.8365},
    {1.00, 0.8413}, {1.02, 0.8461}, {1.04, 0.8508}, {1.06, 0.8554}, {1.08, 0.8599},
    {1.10, 0.8643}, {1.12, 0.8686}, {1.14, 0.8729}, {1.16, 0.8770}, {1.18, 0.8810},
    {1.20, 0.8849}, {1.22, 0.8888}, {1.24, 0.8925}, {1.26, 0.8962}, {1.28, 0.8997},
    {1.30, 0.9032}, {1.32, 0.9066}, {1.34, 0.9099}, {1.36, 0.9131}, {1.38, 0.9162},
    {1.40, 0.9192}, {1.42, 0.9222}, {1.44, 0.9251}, {1.46, 0.9279}, {1.48, 0.9306},
    {1.50, 0.9332}, {1.52, 0.9357}, {1.54, 0.9382}, {1.56, 0.9406}, {1.58, 0.9429},
    {1.60, 0.9452}, {1.62, 0.9474}, {1.64, 0.9495}, {1.66, 0.9515}, {1.68, 0.9535},
    {1.70, 0.9554}, {1.72, 0.9573}, {1.74, 0.9591}, {1.76, 0.9608}, {1.78, 0.9625},
    {1.80, 0.9641}, {1.82, 0.9656}, {1.84, 0.9671}, {1.86, 0.9686}, {1.88, 0.9699},
    {1.90, 0.9713}, {1.92, 0.9726}, {1.94, 0.9738}, {1.96, 0.9750}, {1.98, 0.9761},
    {2.00, 0.9772}, {2.02, 0.9783}, {2.34, 0.9793}, {2.36, 0.9803}, {2.38, 0.9812},
    {2.10, 0.9821}, {2.12, 0.9830}, {2.34, 0.9838}, {2.36, 0.9846}, {2.38, 0.9854},
    {2.20, 0.9861}, {2.22, 0.9868}, {2.34, 0.9875}, {2.36, 0.9881}, {2.38, 0.9887},
    {2.30, 0.9893}, {2.32, 0.9898}, {2.34, 0.9904}, {2.36, 0.9909}, {2.38, 0.9913},
    {2.40, 0.9918}, {2.42, 0.9922}, {2.44, 0.9927}, {2.46, 0.9931}, {2.48, 0.9934},
    {2.50, 0.9938}, {2.52, 0.9941}, {2.54, 0.9945}, {2.56, 0.9948}, {2.58, 0.9951},
    {2.60, 0.9953}, {2.62, 0.9956}, {2.64, 0.9959}, {2.66, 0.9961}, {2.68, 0.9963},
    {2.70, 0.9965}, {2.72, 0.9967}, {2.74, 0.9969}, {2.76, 0.9971}, {2.78, 0.9973},
    {2.80, 0.9974}, {2.82, 0.9976}, {2.84, 0.9977}, {2.86, 0.9979}, {2.88, 0.9980},
    {2.90, 0.9981}, {2.92, 0.9982}, {2.94, 0.9984}, {2.96, 0.9985}, {2.98, 0.9986},
    {3.00, 0.9987}
};

//-----------------------------------
PertResult::PertResult( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent ),
    m_node( 0 ),
    m_project( 0 )
{
    kDebug() << " ---------------- KPlato: Creating PertResult ----------------";
    widget.setupUi(this);
    PertResultItemModel *m = new PertResultItemModel();
    widget.treeWidgetTaskResult->setModel( m );
    widget.treeWidgetTaskResult->setStretchLastSection( false );
    widget.treeWidgetTaskResult->setSelectionMode( QAbstractItemView::ExtendedSelection );

//    QHeaderView *header=widget.treeWidgetTaskResult->header();
    setupGui();
    
    current_schedule=0;

        
    QList<int> lst1; lst1 << 1 << -1; // only display column 0 (NodeName) in left view
    QList<int> show;
    show << NodeEarlyStart
            << NodeEarlyFinish
            << NodeLateStart
            << NodeLateFinish
            << NodePositiveFloat
            << NodeFreeFloat
            << NodeNegativeFloat
            << NodeStartFloat
            << NodeFinishFloat;

    QList<int> lst2; 
    for ( int i = 0; i < m->columnCount(); ++i ) {
        if ( ! show.contains( i ) ) {
            lst2 << i;
        }
    }
    widget.treeWidgetTaskResult->hideColumns( lst1, lst2 );
    widget.treeWidgetTaskResult->masterView()->setDefaultColumns( QList<int>() << 0 );
    widget.treeWidgetTaskResult->slaveView()->setDefaultColumns( show );
    
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
    SplitItemViewSettupDialog dlg( widget.treeWidgetTaskResult );
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
    return widget.treeWidgetTaskResult->loadContext( context, model()->columnNames() );
}

void PertResult::saveContext( QDomElement &context ) const
{
    widget.treeWidgetTaskResult->saveContext( context, model()->columnNames() );
}


//--------------------
PertCpmView::PertCpmView( KoDocument *part, QWidget *parent ) 
    : ViewBase( part, parent ),
    m_project( 0 ),
    current_schedule( 0 ),
    block( false )
{
    kDebug() << " ---------------- KPlato: Creating PertCpmView ----------------";
    widget.setupUi(this);
    widget.cpmTable->setSelectionMode( QAbstractItemView::ExtendedSelection );
    widget.probabilityFrame->setVisible( false );

    widget.cpmTable->setStretchLastSection ( false );
    CriticalPathItemModel *m = new CriticalPathItemModel();
    widget.cpmTable->setModel( m );
    
    setupGui();
    
    QList<int> lst1; lst1 << 1 << -1; // only display first column (NodeName) in left view
    widget.cpmTable->masterView()->setDefaultColumns( QList<int>() << 0 );
    
    QList<int> show;
    show << NodeDuration
            << NodeVarianceDuration
            << NodeOptimisticDuration
            << NodePessimisticDuration
            << NodeEstimate
            << NodeExpected
            << NodeVarianceEstimate
            << NodeOptimistic
            << NodePessimistic;

    QList<int> lst2;
    for ( int i = 0; i < m->columnCount(); ++i ) {
        if ( ! show.contains( i ) ) {
            lst2 << i;
        }
    }
    widget.cpmTable->hideColumns( lst1, lst2 );
    
    for ( int s = 0; s < show.count(); ++s ) {
        widget.cpmTable->slaveView()->mapToSection( show[s], s );
    }
    widget.cpmTable->slaveView()->setDefaultColumns( show );
    
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
    SplitItemViewSettupDialog dlg( widget.cpmTable );
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
    widget.probabilityFrame->setVisible( false );
    if ( m_project && current_schedule && current_schedule->isScheduled() ) {
        long id = current_schedule->id();
        if ( id == -1 ) {
            return;
        }
        widget.probabilityFrame->setVisible( true );
        widget.scheduleName->setText( current_schedule->name() );
        widget.finishTime->setDateTime( m_project->endTime( id ).dateTime() );
        bool ro = model()->variance( Qt::EditRole ).toDouble() == 0.0;
        if ( ro ) {
            widget.probability->setValue( 50 );
        }
        widget.finishTime->setReadOnly( ro );
        widget.probability->setEnabled( ! ro );
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
    //kDebug()<<z<<", "<<v;
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
    //kDebug()<<p<<", "<<t.toString();
    block = false;
}

double PertCpmView::probability( double z ) const
{
    double p = 1.0;
    int i = 1;
    for ( ; i < 155; ++i ) {
        if ( QABS( z ) <= dist[i][0] ) {
            break;
        }
    }
    p = dist[i-1][1] + ( ( dist[i][1] - dist[i-1][1] ) * ( ( QABS(z) - dist[i-1][0] ) / (dist[i][0] - dist[i-1][0] ) ) );
    //kDebug()<<i<<":"<<z<<dist[i][0]<<dist[i][1]<<"="<<p;
    return z < 0 ? 1 - p : p;
}

double PertCpmView::valueZ( double pr ) const
{
    double p = ( pr >= 50.0 ? pr : 100.0 - pr ) / 100.0;
    double z = 3.0;
    int i = 1;
    for ( ; i < 151; ++i ) {
        if ( p < dist[i][1] ) {
            break;
        }
    }
    z = dist[i-1][0] + ( ( dist[i][0] - dist[i-1][0] ) * ( ( p - dist[i-1][1] ) / (dist[i][1] - dist[i-1][1] ) ) );
    //kDebug()<<i<<":"<<pr<<p<<dist[i][0]<<dist[i][1]<<"="<<z;
    return pr < 50.0 ? -z : z;
}

void PertCpmView::slotUpdate()
{
    draw();
}

bool PertCpmView::loadContext( const KoXmlElement &context )
{
    kDebug()<<objectName();
    return widget.cpmTable->loadContext( context, model()->columnNames() );
}

void PertCpmView::saveContext( QDomElement &context ) const
{
    widget.cpmTable->saveContext( context, model()->columnNames() );
}

} // namespace KPlato

#include "kptpertresult.moc"
