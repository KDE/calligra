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

#include "kptpart.h"
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
    m_gantt->setShowLegendButton(false);
    m_gantt->setShowHeaderPopupMenu();
    m_taskView = new KPTTaskAppointmentsView(this, "Task widget");
	draw(view->getPart()->getProject());


	connect(m_gantt, SIGNAL(lvContextMenuRequested ( KDGanttViewItem *, const QPoint &, int )),
	             this, SLOT (popupMenuRequested(KDGanttViewItem *, const QPoint &, int)));

	connect(m_gantt, SIGNAL(lvCurrentChanged(KDGanttViewItem*)), this, SLOT (currentItemChanged(KDGanttViewItem*)));

	connect(m_gantt, SIGNAL(itemDoubleClicked(KDGanttViewItem*)), this, SLOT (slotItemDoubleClicked(KDGanttViewItem*)));

    m_taskLinks.setAutoDelete(true);
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
    
    if (m_firstTime) {
        m_gantt->centerTimelineAfterShow(project.startTime().addDays(-1));
        m_firstTime = false;
    }
    m_gantt->setUpdateEnabled(true);
}

void KPTGanttView::drawChanges(KPTProject &project)
{
    //kdDebug()<<k_funcinfo<<endl;
    m_gantt->setUpdateEnabled(false);
    removeDeleted(m_gantt->firstChild());
    updateChildren(&project); // don't draw project
    
    m_taskLinks.clear();
    drawRelations();
    
    m_gantt->setUpdateEnabled(true);
}

KDGanttViewItem *KPTGanttView::findItem(KPTNode *node)
{
    return findItem(node, m_gantt->firstChild());
}

KDGanttViewItem *KPTGanttView::findItem(KPTNode *node, KDGanttViewItem *item)
{
    for (; item; item = item->nextSibling()) {
        if (node == getNode(item)) {
            return item;
        }
        KDGanttViewItem *i = findItem(node, item->firstChild());
        if (i)
            return i;
    }
    return 0;
}

KPTNode *KPTGanttView::getNode(KDGanttViewItem *item) {
    if (item) {
        if (item->type() == KDGanttViewItem::Event){
            return static_cast<KPTGanttViewEventItem *>(item)->getTask();
        } else if (item->type() == KDGanttViewItem::Task) {
            return static_cast<KPTGanttViewTaskItem *>(item)->getTask();
        } else if (item->type() == KDGanttViewItem::Summary) {
            return static_cast<KPTGanttViewSummaryItem *>(item)->getNode();
        }
    }
    return 0;
}

void KPTGanttView::removeDeleted(KDGanttViewItem *item)
{
    KDGanttViewItem *nextItem;
    for (; item; item = nextItem) {
        nextItem = item->nextSibling();
        KPTNode *n = getNode(item);
        if (n->isDeleted()) {
            deleteItem(item); // delete me and my children
        } else {
            removeDeleted(item->firstChild()); // check my children
        }
    }
}

void KPTGanttView::deleteItem(KDGanttViewItem *item)
{
    //kdDebug()<<k_funcinfo<<item->listViewText()<<endl;
    if (item->parent())
        item->parent()->takeItem(item);
    else
        item->listView()->takeItem(item);
    delete item;
}

KDGanttViewItem *KPTGanttView::correctType(KDGanttViewItem *item, KPTNode *node)
{
    //kdDebug()<<k_funcinfo<<item->listViewText()<<": "<<item->type()<<" node: "<<node->type()<<endl;
    switch (node->type()) {
        case KPTNode::Type_Project:
            return item;
            break;
        case KPTNode::Type_Summarytask:
        case KPTNode::Type_Subproject:
            if (item->type() == KDGanttViewItem::Summary)
                return item;
            break;
        case KPTNode::Type_Task:
            if (item->type() == KDGanttViewItem::Task)
                return item;
            break;
        case KPTNode::Type_Milestone:
            if (item->type() == KDGanttViewItem::Event)
                return item;
            break;
        default:
            return item;
            break;
    }
    KDGanttViewItem *newItem = addNode(item->parent(), node, item);
    newItem->setOpen(item->isOpen());
    deleteItem(item);
    return newItem;
}

void KPTGanttView::correctPosition(KDGanttViewItem *item, KPTNode *node)
{
    KDGanttViewItem *after = findItem(node->siblingBefore());
    if (after) {
        item->moveItem(after);
    }
}

KDGanttViewItem *KPTGanttView::correctParent(KDGanttViewItem *item, KPTNode *node)
{
    KDGanttViewItem *p = findItem(node->getParent());
    if (p == item->parent()) {
        return item;
    }
    KDGanttViewItem *newItem = addNode(p, node);
    newItem->setOpen(item->isOpen());
    deleteItem(item);
    return newItem;
}

void KPTGanttView::updateChildren(KPTNode *parentNode)
{
    //kdDebug()<<k_funcinfo<<endl;
    if (parentNode->isDeleted())
        return;
    QPtrListIterator<KPTNode> nit(parentNode->childNodeIterator());
    for (; nit.current(); ++nit )
    {
        updateNode(nit.current());
    }
}

void KPTGanttView::updateNode(KPTNode *node)
{
    //kdDebug()<<k_funcinfo<<node->name()<<endl;
    if (node->isDeleted())
        return;
    KDGanttViewItem *item = findItem(node);
    if (!item) {
        item = addNode(findItem(node->getParent()), node, findItem(node->siblingBefore()));
        if (item && node->type() == KPTNode::Type_Summarytask)
            updateChildren(node);
        return;
    }
    item = correctType(item, node);
    item = correctParent(item, node);
    correctPosition(item, node);
        
    modifyNode(node);
    
    if (node->type() == KPTNode::Type_Summarytask)
        updateChildren(node);
}

void KPTGanttView::modifyChildren(KPTNode *node)
{
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<KPTNode> nit(node->childNodeIterator());
    for ( nit.toLast(); nit.current(); --nit ) {
        modifyNode(nit.current());
        modifyChildren(nit.current());
    }
}

void KPTGanttView::modifyNode(KPTNode *node)
{
    //kdDebug()<<k_funcinfo<<endl;
    KDGanttViewItem *item = findItem(node);
    if (!item) {
        kdDebug()<<k_funcinfo<<" Item not found"<<endl;
        return;
    }
    if (node->type() == KPTNode::Type_Project) {
        return modifyProject(item, node);
    }
    if (node->type() == KPTNode::Type_Subproject) {
        return modifyProject(item, node);
    }
    if (node->type() == KPTNode::Type_Summarytask) {
        return modifySummaryTask(item, static_cast<KPTTask *>(node));
    }
    if (node->type() == KPTNode::Type_Task) {
        return modifyTask(item, static_cast<KPTTask *>(node));
    }
    if (node->type() == KPTNode::Type_Milestone) {
        return modifyMilestone(item, static_cast<KPTTask *>(node));
    }
    return;
}

void KPTGanttView::modifyProject(KDGanttViewItem *item, KPTNode *node)
{
    //kdDebug()<<k_funcinfo<<endl;
    if (node->isDeleted())
        return;
    KPTDateTime time = node->startTime();
    KPTDuration dur = node->duration();
    if (dur == KPTDuration::zeroDuration)
        dur.addSeconds(1); // avoid bug in KDGannt
    item->setStartTime(time);
    item->setEndTime(node->endTime());
    //item->setOpen(true);

}

void KPTGanttView::modifySummaryTask(KDGanttViewItem *item, KPTTask *task)
{
    //kdDebug()<<k_funcinfo<<endl;
    if (task->isDeleted())
        return;
    KPTDateTime time = task->startTime();
    KPTDuration dur = task->duration();
    if (dur == KPTDuration::zeroDuration)
        dur.addSeconds(1); // avoid bug in KDGannt
    item->setStartTime(time);
    item->setEndTime(task->endTime());
    //item->setOpen(true);
    if (m_showSlack) { // Test
        item->setListViewText(1, "  " +  task->getEarliestStart().toString(Qt::ISODate));
        item->setListViewText(2, "  " +  task->startTime().toString(Qt::ISODate));
        item->setListViewText(3,  "  " + task->endTime().toString(Qt::ISODate));
        item->setListViewText(4, "  " +  task->getLatestFinish().toString(Qt::ISODate));
    }
}

void KPTGanttView::modifyTask(KDGanttViewItem *item, KPTTask *task)
{
    //kdDebug()<<k_funcinfo<<endl;
    if (task->isDeleted())
        return;
    KPTDateTime time = task->startTime();
    KPTDuration dur = task->duration();
    if (dur == KPTDuration::zeroDuration)
        dur.addSeconds(1); // avoid bug in KDGannt
    item->setStartTime(time);
    item->setEndTime(task->endTime());
    if (task->resourceOverbooked() || task->resourceError()) {
        QColor c(yellow);
        item->setColors(c,c,c);
        //kdDebug()<<k_funcinfo<<"Task: "<<task->name()<<" resourceError="<<task->resourceError()<<endl;
    }
    //item->setOpen(true);
    if (m_showSlack) { // Test
        item->setListViewText(1, "  " +  task->getEarliestStart().toString(Qt::ISODate));
        item->setListViewText(2, "  " +  task->startTime().toString(Qt::ISODate));
        item->setListViewText(3,  "  " + task->endTime().toString(Qt::ISODate));
        item->setListViewText(4, "  " +  task->getLatestFinish().toString(Qt::ISODate));
    }
}

void KPTGanttView::modifyMilestone(KDGanttViewItem *item, KPTTask *task)
{
    //kdDebug()<<k_funcinfo<<endl;
    if (task->isDeleted()) {
        return;
    }
    item->setStartTime(task->startTime());
    //item->setOpen(true);
    if (m_showSlack) { // Test
        item->setListViewText(1, "  " +  task->getEarliestStart().toString(Qt::ISODate));
        item->setListViewText(2, "  " +  task->startTime().toString(Qt::ISODate));
        item->setListViewText(3,  "  " + task->endTime().toString(Qt::ISODate));
        item->setListViewText(4, "  " +  task->getLatestFinish().toString(Qt::ISODate));
    }
}

KDGanttViewItem *KPTGanttView::addNode( KDGanttViewItem *parentItem, KPTNode *node, KDGanttViewItem *after)
{
    //kdDebug()<<k_funcinfo<<endl;
    if (node->type() == KPTNode::Type_Project) {
        return addProject(parentItem, node, after);
    }
    if (node->type() == KPTNode::Type_Subproject) {
        return addSubProject(parentItem, node, after);
    }
    if (node->type() == KPTNode::Type_Summarytask) {
        return addSummaryTask(parentItem, static_cast<KPTTask *>(node), after);
    }
    if (node->type() == KPTNode::Type_Task) {
        return addTask(parentItem, static_cast<KPTTask *>(node), after);
    }
    if (node->type() == KPTNode::Type_Milestone) {
        return addMilestone(parentItem, static_cast<KPTTask *>(node), after);
    }
    return 0;
}

KDGanttViewItem *KPTGanttView::addProject(KDGanttViewItem *parentItem, KPTNode *node, KDGanttViewItem *after)
{
    //kdDebug()<<k_funcinfo<<endl;
    if (node->isDeleted())
        return 0;
    KPTGanttViewSummaryItem *item;
    if ( parentItem) {
        item = new KPTGanttViewSummaryItem(parentItem, node);
    } else {
        // we are on the top level
        item = new KPTGanttViewSummaryItem(m_gantt, node);
    }
    if (after)
        item->moveItem(after);
    modifyProject(item, node);
    return item;
}

KDGanttViewItem *KPTGanttView::addSubProject(KDGanttViewItem *parentItem, KPTNode *node, KDGanttViewItem *after)
{
    //kdDebug()<<k_funcinfo<<endl;    
    return addProject(parentItem, node, after);
}

KDGanttViewItem *KPTGanttView::addSummaryTask(KDGanttViewItem *parentItem, KPTTask *task, KDGanttViewItem *after)
{
    //kdDebug()<<k_funcinfo<<endl;
    if (task->isDeleted())
        return 0;
    // display summary item
    KPTGanttViewSummaryItem *item;
    if ( parentItem) {
        item = new KPTGanttViewSummaryItem(parentItem, task);
    } else {
        // we are on the top level
        item = new KPTGanttViewSummaryItem(m_gantt, task);
    }
    if (after)
        item->moveItem(after);
    modifySummaryTask(item, task);
    return item;
}

KDGanttViewItem *KPTGanttView::addTask(KDGanttViewItem *parentItem, KPTTask *task, KDGanttViewItem *after)
{
    //kdDebug()<<k_funcinfo<<endl;
    if (task->isDeleted())
        return 0;
    // display task item
    KPTGanttViewTaskItem *item;
    if ( parentItem ) {
        item = new KPTGanttViewTaskItem(parentItem, task);
    }
    else {
        // we are on the top level
        item = new KPTGanttViewTaskItem(m_gantt, task);
    }
    if (after)
        item->moveItem(after);
    modifyTask(item, task);
    return item;
}

KDGanttViewItem *KPTGanttView::addMilestone(KDGanttViewItem *parentItem, KPTTask *task, KDGanttViewItem *after)
{
    //kdDebug()<<k_funcinfo<<endl;
    if (task->isDeleted()) {
        return 0;
    }
    KPTGanttViewEventItem *item;
    if ( parentItem ) {
        item = new KPTGanttViewEventItem(parentItem, task);
    } else {
        // we are on the top level
        item = new KPTGanttViewEventItem(m_gantt, task);
    }
    if (after)
        item->moveItem(after);
    modifyMilestone(item, task);
    return item;
}

void KPTGanttView::drawChildren(KDGanttViewItem *parentItem, KPTNode &parentNode)
{
    //kdDebug()<<k_funcinfo<<endl;
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


void KPTGanttView::drawProject(KDGanttViewItem *parentItem, KPTNode *node)
{
    //kdDebug()<<k_funcinfo<<endl;
    KPTGanttViewSummaryItem *item = dynamic_cast<KPTGanttViewSummaryItem*>(addProject(parentItem, node));
    drawChildren(item, *node);
}

void KPTGanttView::drawSubProject(KDGanttViewItem *parentItem, KPTNode *node)
{
    //kdDebug()<<k_funcinfo<<endl;
    KPTGanttViewSummaryItem *item = dynamic_cast<KPTGanttViewSummaryItem*>(addSubProject(parentItem, node));
    drawChildren(item, *node);
}

void KPTGanttView::drawSummaryTask(KDGanttViewItem *parentItem, KPTTask *task)
{
    //kdDebug()<<k_funcinfo<<endl;
    KPTGanttViewSummaryItem *item = dynamic_cast<KPTGanttViewSummaryItem*>(addSummaryTask(parentItem, task));
    drawChildren(item, *task);
}

void KPTGanttView::drawTask(KDGanttViewItem *parentItem, KPTTask *task)
{
    //kdDebug()<<k_funcinfo<<endl;
    addTask(parentItem, task);
}

void KPTGanttView::drawMilestone(KDGanttViewItem *parentItem, KPTTask *task)
{
    //kdDebug()<<k_funcinfo<<endl;
    addMilestone(parentItem, task);
}

void KPTGanttView::addTaskLink(KDGanttViewTaskLink *link) {
    //kdDebug()<<k_funcinfo<<endl;
    m_taskLinks.append(link);
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
    //kdDebug()<<k_funcinfo<<endl;
    if (!item) return;

    KPTGanttViewSummaryItem *summaryItem = dynamic_cast<KPTGanttViewSummaryItem *>(item);
    if (summaryItem)
    {
        //kdDebug()<<k_funcinfo<<"Summary item: "<<summaryItem->listViewText()<<endl;
        summaryItem->insertRelations(this);
        return;
    }
    KPTGanttViewTaskItem *taskItem = dynamic_cast<KPTGanttViewTaskItem *>(item);
    if (taskItem)
    {
        //kdDebug()<<k_funcinfo<<"Task item: "<<taskItem->listViewText()<<endl;
        taskItem->insertRelations(this);
        return;
    }
    KPTGanttViewEventItem *milestoneItem = dynamic_cast<KPTGanttViewEventItem *>(item);
    if (milestoneItem)
    {
        //kdDebug()<<k_funcinfo<<"Milestone item: "<<milestoneItem->listViewText()<<endl;
        milestoneItem->insertRelations(this);
        return;
    }
    kdDebug()<<k_funcinfo<<"Unknown item type: "<<item->listViewText()<<endl;
}

void KPTGanttView::currentItemChanged(KDGanttViewItem* item)
{
    //kdDebug()<<k_funcinfo<<(item ? item->listViewText() : "null")<<endl;
    m_taskView->clear();
    m_gantt->setSelected(m_currentItem, false);
    m_currentItem = item;
    if (!item) {
        return;
    }
    m_gantt->setSelected(item, true);
    KPTGanttViewTaskItem *taskItem = dynamic_cast<KPTGanttViewTaskItem *>(item);
    if (taskItem) {
        m_taskView->draw(taskItem->getTask());
        return;
    }
    KPTGanttViewEventItem *msItem = dynamic_cast<KPTGanttViewEventItem *>(item);
    if (msItem)
        m_taskView->draw(msItem->getTask());
}

KPTNode *KPTGanttView::currentNode()
{
    return getNode(m_currentItem);
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
