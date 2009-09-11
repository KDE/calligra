/* This file is part of the KDE project
  Copyright (C) 2006 - 2007 Frederic BECQUIER <frederic.becquier@gmail.com>

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

#include "kptresourceassignmentview.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"
#include "kptdatetime.h"
#include "kptrelation.h"

#include <KoDocument.h>

#include <kdebug.h>

#include <QWidget>
#include <QSplitter>
#include <QStringList>
#include <QCursor>

#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>

namespace KPlato
{

ResourcesList::ResourcesList( QWidget * parent )
        : QTreeWidget( parent )
{
    setContextMenuPolicy( Qt::CustomContextMenu );
    //connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), SLOT( slotContextMenuRequested( const QPoint& ) ) );
}

void ResourceAssignmentView::slotRequestPopupMenu( const QPoint &p )
{
    kDebug() << p;
    emit requestPopupMenu( "resourceassigment_popup", QCursor::pos() );
}

void ResourceAssignmentView::draw( Project &project )
{
    m_project = &project;
    m_resList->clear();

    foreach ( ResourceGroup * gr, project.resourceGroups() ) {
        QTreeWidgetItem * item = new QTreeWidgetItem( m_resList );
        item->setText( 0, gr->name() );
        drawResourcesName( item, gr );

        kDebug() <<"GROUP FOUND";
    }
}

/*This function is called for the left panel*/
void ResourceAssignmentView::drawResourcesName( QTreeWidgetItem *parent, ResourceGroup *group )
{
    /*for each resource*/
    foreach ( Resource * res, group->resources() ) {
        QTreeWidgetItem * item = new QTreeWidgetItem( parent );

        /*Determine the name and the type of the resource*/
        switch ( res->type() ) {
            case Resource::Type_Work:
                item->setText( 0, res->name() );
                item->setText( 1, i18n( "Work" ) );
                break;
            case Resource::Type_Material:
                item->setText( 0, res->name() );
                item->setText( 1, i18n( "Material" ) );
                break;
            default:
                break;
        }
    }
}


/*Constructor*/
ResourceAssignmentView::ResourceAssignmentView( KoDocument *part, QWidget *parent)
    : ViewBase( part, parent ),
    m_project( 0 )
{
    kDebug() <<" ---------------- KPlato: Creating ResourceAssignmentView ----------------";

    widget.setupUi(this);
    
    
   /* QVBoxLayout *l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_splitter = new QSplitter( this );
    l->addWidget( m_splitter );
    m_splitter->setOrientation( Qt::Horizontal );

    m_resList = new ResourcesList( m_splitter );

    QStringList sl;
    sl << i18n( "Name" ) << i18n( "Type" );
    m_resList->setHeaderLabels( sl );

    m_taskList = new ResourcesList( m_splitter );
    m_tasktreeroot = new QTreeWidgetItem ( m_taskList );

    QStringList sl2;
    sl2 << i18n( "Task" ); << i18n( "Completed" );
    m_taskList->setHeaderLabels( sl2 );*/
    
    
   /* m_resList = widget.assign( m_resList );
    m_taskList = widget.assign( m_taskList );
    m_tasktreeroot = widget.assign( m_tasktreeroot );*/
    

    m_selectedItem = 0;
    m_splitter = widget.m_splitter;
    m_resList = (ResourcesList *)widget.m_resList;
    m_taskList = (ResourcesList *)widget.m_taskList;
    m_part = part;
    m_tasktreeroot = new QTreeWidgetItem ( m_taskList );

    connect( m_resList, SIGNAL( itemSelectionChanged() ), SLOT( resSelectionChanged() ) );
    connect( m_taskList, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( slotRequestPopupMenu( const QPoint& ) ) );
}

/*Store the selected item*/
void ResourceAssignmentView::resSelectionChanged()
{
    QTreeWidgetItem * item = 0;
    QList<QTreeWidgetItem*> selList = m_resList->selectedItems();
    if ( !selList.isEmpty() )
        item = selList.first();
    resSelectionChanged( item );
}

/*Update tasks attributed to the selected item*/
void ResourceAssignmentView::resSelectionChanged( QTreeWidgetItem *item )
{
    QTreeWidgetItem * resItem =  item;
    if ( resItem ) {
        m_selectedItem = resItem;
	updateTasks();
        return ;
    }
    m_selectedItem = 0;
    // updateTasks();  that method uses m_selectedItem, so this will always crash... CID 3206
}

/**/
void ResourceAssignmentView::updateTasks()
{
    Q_ASSERT(m_selectedItem);
    /*Find Selected Item*/
    Resource* ItemRes = 0;
    ResourceGroup* ItemGrp = 0;

    QString name = m_selectedItem->text(0);
    QString type = m_selectedItem->text(1);
    if(!type.isEmpty()){
        kDebug() <<"Item Selected:" << name <<" / Type:" << type;
    }
    else{
        kDebug() <<"Group Selected:" << name;
    }
    m_taskList->clear();
    if ( m_project == 0 ) {
        return;
    }
    
    /*Find tasks attributed to the selected item*/

    /*The selected item is a resource*/
    if(!type.isEmpty())
    {
        foreach ( ResourceGroup * gr, m_project->resourceGroups() ) {
            foreach ( Resource * res, gr->resources() ) {
                if (name == res->name())
                {
                    ItemRes = res;
                    kDebug() <<"Selected Resource found";
                }
                else
                {
                    kDebug() <<"Not found";
                }
            }
        }
        drawTasksAttributedToAResource(ItemRes,m_tasktreeroot);
    }
    else
    /*The selected item is a group*/
    {
        foreach ( ResourceGroup * gr, m_project->resourceGroups() ) {
            if (name == gr->name())
            {
                ItemGrp = gr;
                kDebug() <<"[void KPlato::ResourceAssignmentView::updateTasks()] Selected Group founded";
            }
            else
            {
            kDebug() <<"[void KPlato::ResourceAssignmentView::updateTasks()] Group Not founded";
            }
        }
        drawTasksAttributedToAGroup(ItemGrp,m_tasktreeroot);
    }
}

void ResourceAssignmentView::drawTasksAttributedToAResource (Resource *res, QTreeWidgetItem *parent)
{
    QString taskName;
    Task *currentTask;

    /*Differents state regrouping tasks*/
    QTreeWidgetItem *notStarted;
    QTreeWidgetItem *started;
    QTreeWidgetItem *finished;
    QString advance ;

    /*Task node*/
    QTreeWidgetItem * item;

    /*Put the name of the resource on the node*/

    /*Case: the resource has no task attributed*/
    if((res->requests()).isEmpty())
    {
        QTreeWidgetItem * item = new QTreeWidgetItem( m_taskList );
        item->setText( 0, i18n( "No task attributed" ) );
    }
    else
    /*Case: the resource has tasks attributed*/
    {

    /*Creation of 3 categories of task*/
    notStarted = new QTreeWidgetItem( m_taskList );
    started = new QTreeWidgetItem( m_taskList );
    finished = new QTreeWidgetItem( m_taskList );

    /*Set names of categories*/
    notStarted->setText( 0, i18n( "Not Started" ) );
    started->setText( 0, i18n( "Started" ) );
    finished->setText( 0, i18n( "Finished" ) );

    /*For each task attibuted to the current resource*/
    foreach ( ResourceRequest * rr , res->requests() ){

        /*get name*/
        currentTask = (rr->parent())->task();
        taskName = currentTask->name();	

        /*get status*/
        /*State: started*/
        if ((((rr->parent())->task())->completion().isStarted()) && !(((rr->parent())->task())->completion().isFinished()))
        {
        kDebug() <<"[void KPlato::ResourceAssignmentView::drawTasksAttributedToAResource()] task started";
        /*adding to the tree*/
        item = new QTreeWidgetItem( started );
        item->setText( 0, taskName );

        /*Determine the task's advance*/
        int percent = ((rr->parent())->task())->completion().percentFinished();
        //kDebug() <<"[void KPlato::ResourceAssignmentView::drawTasksAttributedToAResource()]" << percent <<"";
        advance.setNum(percent);
	advance += '%';
        item->setText( 1, advance );
        }
        /*State: Finished*/
        else if (((rr->parent())->task())->completion().isFinished())
        {
        /*adding to the tree*/
        kDebug() <<"[void KPlato::ResourceAssignmentView::drawTasksAttributedToAResource()] task finished";
        item = new QTreeWidgetItem( finished );
        item->setText( 0, taskName );
        }
        /*State not started*/
        else
        {
        /*adding to the tree*/
        kDebug() <<"[void KPlato::ResourceAssignmentView::drawTasksAttributedToAResource()] task not started";
        item = new QTreeWidgetItem( notStarted );
        item->setText( 0, taskName );
        }
    }
    }
}

void ResourceAssignmentView::drawTasksAttributedToAGroup (ResourceGroup *group, QTreeWidgetItem *parent)
{
    QString taskName;
    Task *currentTask;

    bool alreadyStored;

    /*Task node*/
    QTreeWidgetItem * item;

    /*Differents state regrouping tasks*/
    QTreeWidgetItem *notStarted;
    QTreeWidgetItem *started;
    QTreeWidgetItem *finished;
    QString advance ;


    if((group->resources()).isEmpty())
    {
        QTreeWidgetItem * groupnode = new QTreeWidgetItem( parent );
        groupnode->setText( 0, i18n( "No resource attributed" ) );
    }
    else
    {

        /*Creation of 3 categories of task*/
        notStarted = new QTreeWidgetItem( m_taskList );
        started = new QTreeWidgetItem( m_taskList );
        finished = new QTreeWidgetItem( m_taskList );

        /*Set names of categories*/
        notStarted->setText( 0, i18n( "Not Started" ) );
        started->setText( 0, i18n( "Started" ) );
        finished->setText( 0, i18n( "Finished" ) );

	foreach ( Resource * res, group->resources() ) {
	    foreach ( ResourceRequest * rr , res->requests() ) {

                /*get name*/
                currentTask = (rr->parent())->task();
                taskName = currentTask->name();

                alreadyStored = false; 

                /*store tasks in the tree*/
                if ((((rr->parent())->task())->completion().isStarted()) && !(((rr->parent())->task())->completion().isFinished()))
                {
                    for (int i = 0; i < started->childCount();i++)
                    {
                        if (started->child(i)->text(0) == taskName)
                        { alreadyStored = true ;}
                    }

                    if ( !alreadyStored )
                    {
                    item = new QTreeWidgetItem( started );
                    item->setText( 0, taskName );

                    /*Determine the task's advance*/
                    int percent = ((rr->parent())->task())->completion().percentFinished();
                    advance.setNum(percent);
	            advance += '%';
                    item->setText( 1, advance );
                    }
	        }
	        else if (((rr->parent())->task())->completion().isFinished())
                {
                    for (int i = 0; i < finished->childCount();i++)
                    {
                        if (finished->child(i)->text(0) == taskName)
                        { alreadyStored = true ;}
                    }

                    if ( !alreadyStored )
                    {
                    item = new QTreeWidgetItem( finished );
                    item->setText( 0, taskName );
                    }
	        }
                else 
                {
                    for (int i = 0; i < notStarted->childCount();i++)
                    {
                        if (notStarted->child(i)->text(0) == taskName)
                        { alreadyStored = true ;}
                    }

                    if ( !alreadyStored )
                    {
                    item = new QTreeWidgetItem( notStarted );
                    item->setText( 0, taskName );
                    }
                }
	    }
        }
    }
}

void ResourceAssignmentView::setGuiActive( bool activate )
{
    kDebug()<<activate;
//    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
}

void ResourceAssignmentView::slotUpdate(){

 draw(*m_project);
}

}  //KPlato namespace

#include "kptresourceassignmentview.moc"

