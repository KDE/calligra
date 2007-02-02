/* This file is part of the KDE project
  Copyright (C) 2003 - 2006 Frederic BECQUIER <frederic.becquier@gmail.com>

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
#include "kptpart.h"
#include "kptview.h"
#include "kptcanvasitem.h"
#include "kptnode.h"
#include "kptpart.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"
#include "kptdatetime.h"
#include "kptrelation.h"
#include "kptcontext.h"

#include <kdebug.h>

#include <q3listview.h>
#include <q3popupmenu.h>

#include <QLineEdit>
#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QSpinBox>
#include <QSplitter>
#include <QStringList>
#include <QTabWidget>
#include <QPainter>

#include <klocale.h>
#include <kglobal.h>
#include <kprinter.h>
#include <kmessagebox.h>

namespace KPlato
{

ResourcesList::ResourcesList( QWidget * parent )
        : QTreeWidget( parent )
{
    setContextMenuPolicy( Qt::CustomContextMenu );
}

void ResourceAssignmentView::draw( Project &project )
{
    m_resList->clear();

    foreach ( ResourceGroup * gr, project.resourceGroups() ) {
        QTreeWidgetItem * item = new QTreeWidgetItem( m_resList );
        item->setText( 0, gr->name() );
        drawResourcesName( item, gr );

	kDebug() << "[void KPlato::ResourceAssignmentView::draw( Project &project )] GROUP FOUNDED" << endl;

    }
    

}


void ResourceAssignmentView::drawResourcesName( QTreeWidgetItem *parent, ResourceGroup *group )
{
    foreach ( Resource * res, group->resources() ) {
        QTreeWidgetItem * item = new QTreeWidgetItem( parent );

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



ResourceAssignmentView::ResourceAssignmentView( Part *part, QWidget *parent): ViewBase( part, parent )
{
kDebug() << " ---------------- KPlato: Creating ResourceAssignmentView ----------------" << endl;

    QVBoxLayout *l = new QVBoxLayout( this );
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
    sl2 << i18n( "Task" );
    m_taskList->setHeaderLabels( sl2 );

    m_selectedItem = 0;

    m_part = part;

    draw(m_part->getProject());

    connect( m_resList, SIGNAL( itemSelectionChanged() ), SLOT( resSelectionChanged() ) );

}


void ResourceAssignmentView::resSelectionChanged()
{
    QTreeWidgetItem * item = 0;
    QList<QTreeWidgetItem*> selList = m_resList->selectedItems();
    if ( !selList.isEmpty() )
        item = selList.first();
    resSelectionChanged( item );
}


void ResourceAssignmentView::resSelectionChanged( QTreeWidgetItem *item )
{
    QTreeWidgetItem * resItem =  item;
    if ( resItem ) {
        m_selectedItem = resItem;
	updateTasks();
        return ;
    }
    m_selectedItem = 0;
    updateTasks();
}

void ResourceAssignmentView::updateTasks()
{
    /*Find Selected Item*/
    Resource* ItemRes;
    ResourceGroup* ItemGrp;

    QString name = m_selectedItem->text(0);
    QString type = m_selectedItem->text(1);
    if(type != ""){
        kDebug() << "[void KPlato::ResourceAssignmentView::updateTasks()] Item Selected: " << name << " / Type: " << type << endl;
    }
    else{
        kDebug() << "[void KPlato::ResourceAssignmentView::updateTasks()] Group Selected: " << name << endl;
    }


    m_taskList->clear();
    m_tasktreeroot = new QTreeWidgetItem ( m_taskList );

    /*Find tasks attributed to the selected item*/

    /*The selected item is a resource*/
    if(type != "")
    {
        foreach ( ResourceGroup * gr, ((m_part->getProject()).resourceGroups()) ) {
            foreach ( Resource * res, gr->resources() ) {
                if (name == res->name())
                {
                    ItemRes = res;
                    kDebug() << "[void KPlato::ResourceAssignmentView::updateTasks()] Selected Resource founded";
                }
                else
                {
                kDebug() << "[void KPlato::ResourceAssignmentView::updateTasks()] Not founded";
                }
            }
        }
        drawTasksAttributedToAResource(ItemRes,m_tasktreeroot);
    }
    else
    /*The selected item is a group*/
    {
        foreach ( ResourceGroup * gr, ((m_part->getProject()).resourceGroups()) ) {
            if (name == gr->name())
            {
                ItemGrp = gr;
                kDebug() << "[void KPlato::ResourceAssignmentView::updateTasks()] Selected Group founded";
            }
            else
            {
            kDebug() << "[void KPlato::ResourceAssignmentView::updateTasks()] Group Not founded";
            }
        }
        drawTasksAttributedToAGroup(ItemGrp,m_tasktreeroot);
    }
}

void ResourceAssignmentView::drawTasksAttributedToAResource (Resource *res, QTreeWidgetItem *parent)
{
    QString taskName;

    parent->setText( 0, res->name() );

    if((res->requests()).isEmpty())
    {
        QTreeWidgetItem * item = new QTreeWidgetItem( parent );
        item->setText( 0, i18n( "No task attributed" ) );
    }
    else
    {
    foreach ( ResourceRequest * rr , res->requests() ){

        taskName = ((rr->parent())->task())->name();

        QTreeWidgetItem * item = new QTreeWidgetItem( parent );
        item->setText( 0, taskName );
    }
    }
}

void ResourceAssignmentView::drawTasksAttributedToAGroup (ResourceGroup *group, QTreeWidgetItem *parent)
{
    parent->setText( 0, group->name() );

    if((group->resources()).isEmpty())
    {
        QTreeWidgetItem * groupnode = new QTreeWidgetItem( parent );
        groupnode->setText( 0, i18n( "No resource attributed" ) );
    }
    else
    {
        foreach ( Resource * res, group->resources() ) 
        {
            QTreeWidgetItem * groupnode = new QTreeWidgetItem( parent );
            drawTasksAttributedToAResource(res,groupnode);
        }
    }
	
}

}  //KPlato namespace

#include "kptresourceassignmentview.moc"

