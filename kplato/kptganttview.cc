/* This file is part of the KDE project
   Copyright (C) 2002 Dag Andersen <danders@get2net.dk>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kptganttview.h"

#include "kptview.h"
#include "kptcanvasitem.h"
#include "kptnode.h"
#include "kptpart.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"
#include "kptdatetime.h"
#include "kpttaskappointmentsview.h"
#include "KDGanttView.h"
#include "KDGanttViewItem.h"
#include "KDGanttViewTaskItem.h"
#include "KDGanttViewSummaryItem.h"
#include "KDGanttViewEventItem.h"

#include <kdebug.h>

#include <qsplitter.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qheader.h>
#include <qpopupmenu.h>
#include <qtabwidget.h>
#include <qptrlist.h>
#include <qlineedit.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qvaluelist.h>

#include <klocale.h>
#include <kglobal.h>
#include <kprinter.h>

KPTGanttView::KPTGanttView( KPTView *view, QWidget *parent, const char* name)
    : QSplitter(parent, name),
    m_mainview( view ),
	m_currentItem(0),
    m_taskView(0),
    m_showSlack(true),
    m_firstTime(true)
{

    setOrientation(QSplitter::Vertical);

    m_gantt = new KDGanttView(this, "Gantt view");
    // For test, we need "slack functinallity" in KDGantt...
    m_gantt->addColumn("Earliest start");
    m_gantt->addColumn("Start");
    m_gantt->addColumn("End");
    m_gantt->addColumn("Latest finish");
    
    m_gantt->setHeaderVisible(true);
    m_gantt->setScale(KDGanttView::Day);
    m_taskView = new KPTTaskAppointmentsView(this, "Task widget");
	draw(view->getPart()->getProject());


	connect(m_gantt, SIGNAL(lvContextMenuRequested ( KDGanttViewItem *, const QPoint &, int )),
	             this, SLOT (popupMenuRequested(KDGanttViewItem *, const QPoint &, int)));

	connect(m_gantt, SIGNAL(lvCurrentChanged(KDGanttViewItem*)), this, SLOT (currentItemChanged(KDGanttViewItem*)));

	connect(m_gantt, SIGNAL(itemDoubleClicked(KDGanttViewItem*)), this, SLOT (slotItemDoubleClicked(KDGanttViewItem*)));

}

void KPTGanttView::zoom(double /*zoom*/)
{
}

void KPTGanttView::clear()
{
	m_gantt->clear();
    m_taskView->clear();
}

void KPTGanttView::draw(KPTProject &project)
{
    //kdDebug()<<k_funcinfo<<endl;
    m_gantt->setUpdateEnabled(false);

    clear();
    drawChildren(NULL, project);
    drawRelations();
    
    currentItemChanged(m_gantt->firstChild());
    if (m_currentItem)
        m_gantt->setSelected(m_currentItem, true);
    
    if (m_firstTime) {
        m_gantt->centerTimelineAfterShow(project.startTime().addDays(-1));
        m_firstTime = false;
    }
    m_gantt->setUpdateEnabled(true);
}


void KPTGanttView::drawChildren(KDGanttViewSummaryItem *parentItem, KPTNode &parentNode)
{
    if (parentNode.isDeleted())
        return;
	QPtrListIterator<KPTNode> nit(parentNode.childNodeIterator());
	for ( nit.toLast(); nit.current(); --nit )
	{
		KPTNode *n = nit.current();
		if (n->type() == KPTNode::Type_Project)
	        drawProject(parentItem, n);
		else if (n->type() == KPTNode::Type_Subproject)
		    drawSubProject(parentItem, n);
		else if (n->type() == KPTNode::Type_Summarytask) {
            KPTTask *t = dynamic_cast<KPTTask *>(n);
		    drawSummaryTask(parentItem, t);
		} else if (n->type() == KPTNode::Type_Task) {
            KPTTask *t = dynamic_cast<KPTTask *>(n);
		    drawTask(parentItem, t);
        } else if (n->type() == KPTNode::Type_Milestone) {
            KPTTask *t = dynamic_cast<KPTTask *>(n);
			drawMilestone(parentItem, t);
        }
		else
		    kdDebug()<<k_funcinfo<<"Node type "<<n->type()<<" not implemented yet"<<endl;

	}
}

void KPTGanttView::drawProject(KDGanttViewSummaryItem *parentItem, KPTNode *node)
{
    if (node->isDeleted())
        return;
    KPTDateTime time = node->startTime();
    KPTDuration dur = node->duration();
    if (dur == KPTDuration::zeroDuration)
        dur.addSeconds(1); // avoid bug in KDGannt
    KPTGanttViewSummaryItem *item;
    if ( parentItem) {
      item = new KPTGanttViewSummaryItem(parentItem, node);
    } else {
        // we are on the top level
        item = new KPTGanttViewSummaryItem(m_gantt, node);
    }

    item->setStartTime(time);
    item->setEndTime(node->endTime());
    item->setOpen(true);

    drawChildren(item, *node);
}

void KPTGanttView::drawSubProject(KDGanttViewSummaryItem *parentItem, KPTNode *node)
{
    if (node->isDeleted())
        return;
    KPTDateTime time = node->startTime();
    KPTDuration dur = node->duration();
    if (dur == KPTDuration::zeroDuration)
        dur.addSeconds(1); // avoid bug in KDGannt
    // display summary item
    KPTGanttViewSummaryItem *item;
    if ( parentItem) {
        item = new KPTGanttViewSummaryItem(parentItem, node);
    } else {
        // we are on the top level
        item = new KPTGanttViewSummaryItem(m_gantt, node);
    }
    item->setStartTime(time);
    item->setEndTime(node->endTime());
    item->setOpen(true);

    drawChildren(item, *node);
}

void KPTGanttView::drawSummaryTask(KDGanttViewSummaryItem *parentItem, KPTTask *task)
{
    if (task->isDeleted())
        return;
    KPTDateTime time = task->startTime();
    KPTDuration dur = task->duration();
    if (dur == KPTDuration::zeroDuration)
        dur.addSeconds(1); // avoid bug in KDGannt
    // display summary item
    KPTGanttViewSummaryItem *item;
    if ( parentItem) {
        item = new KPTGanttViewSummaryItem(parentItem, task);
    } else {
        // we are on the top level
        item = new KPTGanttViewSummaryItem(m_gantt, task);
    }
    item->setStartTime(time);
    item->setEndTime(task->endTime());
    item->setOpen(true);
    if (m_showSlack) { // Test
        item->setListViewText(1, "  " +  item->getNode()->getEarliestStart().toString(Qt::ISODate));
        item->setListViewText(2, "  " +  item->getNode()->startTime().toString(Qt::ISODate));
        item->setListViewText(3,  "  " + item->getNode()->endTime().toString(Qt::ISODate));
        item->setListViewText(4, "  " +  item->getNode()->getLatestFinish().toString(Qt::ISODate));
    }

    drawChildren(item, *task);

}

void KPTGanttView::drawTask(KDGanttViewSummaryItem *parentItem, KPTTask *task)
{
    if (task->isDeleted())
        return;
    KPTDateTime time = task->startTime();
    KPTDuration dur = task->duration();
    if (dur == KPTDuration::zeroDuration)
        dur.addSeconds(1); // avoid bug in KDGannt
    // display task item
    KPTGanttViewTaskItem *item;
    if ( parentItem ) {
        item = new KPTGanttViewTaskItem(parentItem, task);
    }
    else {
        // we are on the top level
        item = new KPTGanttViewTaskItem(m_gantt, task);
    }
    item->setStartTime(time);
    item->setEndTime(task->endTime());
    item->setOpen(true);
    if (task->resourceOverbooked() || task->resourceError()) {
        QColor c(yellow);
        item->setColors(c,c,c);
        //kdDebug()<<k_funcinfo<<"Task: "<<task->name()<<" resourceError="<<task->resourceError()<<endl;
    }
    if (m_showSlack) { // Test
        item->setListViewText(1, "  " +  item->getTask()->getEarliestStart().toString(Qt::ISODate));
        item->setListViewText(2, "  " +  item->getTask()->startTime().toString(Qt::ISODate));
        item->setListViewText(3, "  " +  item->getTask()->endTime().toString(Qt::ISODate));
        item->setListViewText(4, "  " +  item->getTask()->getLatestFinish().toString(Qt::ISODate));
    }
}

void KPTGanttView::drawMilestone(KDGanttViewSummaryItem *parentItem, KPTTask *task)
{
    if (task->isDeleted())
        return;
    KPTDateTime time = task->startTime();
    KPTGanttViewEventItem *item;
    if ( parentItem ) {
        item = new KPTGanttViewEventItem(parentItem, task);
    } else {
        // we are on the top level
        item = new KPTGanttViewEventItem(m_gantt, task);
    }
    item->setStartTime(time);
    item->setOpen(true);
    if (m_showSlack) { // Test
        item->setListViewText(1, "  " +  item->getTask()->getEarliestStart().toString(Qt::ISODate));
        item->setListViewText(2, "  " +  item->getTask()->startTime().toString(Qt::ISODate));
        item->setListViewText(3, "  " +  item->getTask()->endTime().toString(Qt::ISODate));
        item->setListViewText(4, "  " +  item->getTask()->getLatestFinish().toString(Qt::ISODate));
    }
}

void KPTGanttView::drawRelations()
{
    KDGanttViewItem *item = m_gantt->firstChild();
    //kdDebug()<<k_funcinfo<<"First: "<<(item ? item->listViewText() : "nil")<<endl;
    for (; item; item = item->nextSibling())
    {
        drawRelations(item);
        drawChildRelations(item->firstChild());
    }
}

void KPTGanttView::drawChildRelations(KDGanttViewItem *item)
{
    //kdDebug()<<k_funcinfo<<"item: "<<(item ? item->listViewText() : "nil")<<endl;
    for (; item; item = item->nextSibling())
    {
        drawRelations(item);
        drawChildRelations(item->firstChild());
    }
}

void KPTGanttView::drawRelations(KDGanttViewItem *item)
{
    if (!item) return;

    KPTGanttViewSummaryItem *summaryItem = dynamic_cast<KPTGanttViewSummaryItem *>(item);
    if (summaryItem)
    {
        //kdDebug()<<k_funcinfo<<"Summary item: "<<summaryItem->listViewText()<<endl;
        summaryItem->insertRelations();
        return;
    }
    KPTGanttViewTaskItem *taskItem = dynamic_cast<KPTGanttViewTaskItem *>(item);
    if (taskItem)
    {
        //kdDebug()<<k_funcinfo<<"Task item: "<<taskItem->listViewText()<<endl;
        taskItem->insertRelations();
        return;
    }
    KPTGanttViewEventItem *milestoneItem = dynamic_cast<KPTGanttViewEventItem *>(item);
    if (milestoneItem)
    {
        //kdDebug()<<k_funcinfo<<"Milestone item: "<<milestoneItem->listViewText()<<endl;
        milestoneItem->insertRelations();
        return;
    }
    kdDebug()<<k_funcinfo<<"Unknown item type: "<<item->listViewText()<<endl;
}

void KPTGanttView::currentItemChanged(KDGanttViewItem* item)
{
    //kdDebug()<<k_funcinfo<<endl;
    m_taskView->clear();
    m_currentItem = item;
    KPTGanttViewTaskItem *taskItem = dynamic_cast<KPTGanttViewTaskItem *>(item);
    if (taskItem)
        m_taskView->draw(taskItem->getTask());

    KPTGanttViewEventItem *msItem = dynamic_cast<KPTGanttViewEventItem *>(item);
    if (msItem)
        m_taskView->draw(msItem->getTask());
}

KPTNode *KPTGanttView::currentNode()
{
    KDGanttViewItem *curr = m_currentItem;
	if (!curr)
    {
		// if we do not have a current item here we return 0.
		// The caller may then decide to use the KPTProject
		// root node, but that decision is up to the caller
		return 0;
	}
	if (curr->type() == KDGanttViewItem::Summary)
	{
	    KPTGanttViewSummaryItem *item = (KPTGanttViewSummaryItem *)curr;
		//kdDebug()<<k_funcinfo<<"Summary item="<<item<<endl;
		return item->getNode();
	}
	else if (curr->type() == KDGanttViewItem::Task)
	{
		KPTGanttViewTaskItem *item = (KPTGanttViewTaskItem *)curr;
		//kdDebug()<<k_funcinfo<<"Task item="<<item<<endl;
		return item->getTask();
	}
	else if (curr->type() == KDGanttViewItem::Event)
	{
		KPTGanttViewEventItem *item = (KPTGanttViewEventItem *)curr;
		//kdDebug()<<k_funcinfo<<"Event item="<<item<<endl;
		return item->getTask();
	}
	kdDebug()<<k_funcinfo<<"No item="<<endl;
	return 0;
}

void KPTGanttView::popupMenuRequested(KDGanttViewItem * /*item*/, const QPoint & pos, int)
{
	QPopupMenu *menu = m_mainview->popupMenu("node_popup");
	if (menu)
	{
		/*int id =*/ menu->exec(pos);
		//kdDebug()<<k_funcinfo<<"id="<<id<<endl;
	}
	else
		kdDebug()<<k_funcinfo<<"No menu!"<<endl;
}

void KPTGanttView::slotItemDoubleClicked(KDGanttViewItem* /*item*/)
{
}

void KPTGanttView::print(KPrinter &printer) {
    kdDebug()<<k_funcinfo<<endl;

}

#include "kptganttview.moc"
