/* This file is part of the KDE project
   Copyright (C) 2002 - 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

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
#include "kptrelation.h"
#include "kptcontext.h"

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
#include <qpainter.h> 
#include <qpaintdevicemetrics.h> 

#include <klocale.h>
#include <kglobal.h>
#include <kprinter.h>
#include <kmessagebox.h>

namespace KPlato
{

KPTGanttView::KPTGanttView(KPTView *view, QWidget *parent, bool readWrite, const char* name)
    : QSplitter(parent, name),
    m_mainview(view),
    m_readWrite(readWrite),
    m_currentItem(0),
    m_taskView(0),
    m_firstTime(true)
{

    setOrientation(QSplitter::Vertical);
    
    m_gantt = new KDGanttView(this, "Gantt view");
    
    m_showResources = false; // FIXME
    m_showTaskName = false; // FIXME
    m_showTaskLinks = false; // FIXME
    m_showProgress = false; //FIXME
    m_showPositiveFloat = false; //FIXME
    m_showCriticalTasks = false; //FIXME
    m_showCriticalPath = false; //FIXME
    m_gantt->setHeaderVisible(true);
    m_gantt->addColumn(i18n("Work Breakdown Structure", "WBS"));
    m_gantt->setScale(KDGanttView::Day);
    m_gantt->setShowLegendButton(false);
    m_gantt->setShowHeaderPopupMenu();
    m_taskView = new KPTTaskAppointmentsView(this, "Task widget");
    // hide KPTTaskAppointmentsView
    QValueList<int> list = sizes();
    list[0] += list[1];
    list[1] = 0;
    setSizes(list);
    draw(view->getPart()->getProject());
    setReadWriteMode(readWrite);
    
	connect(m_gantt, SIGNAL(lvContextMenuRequested ( KDGanttViewItem *, const QPoint &, int )),
	             this, SLOT (popupMenuRequested(KDGanttViewItem *, const QPoint &, int)));

	connect(m_gantt, SIGNAL(lvCurrentChanged(KDGanttViewItem*)), this, SLOT (currentItemChanged(KDGanttViewItem*)));

	connect(m_gantt, SIGNAL(itemDoubleClicked(KDGanttViewItem*)), this, SLOT (slotItemDoubleClicked(KDGanttViewItem*)));

    m_taskLinks.setAutoDelete(true);
    
    if (m_gantt->firstChild()) {
        m_gantt->firstChild()->listView()->setCurrentItem(m_gantt->firstChild());
        m_gantt->firstChild()->listView()->header()->moveSection(1, 0);
        m_gantt->firstChild()->listView()->setFocus();
    }
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
    resetDrawn(m_gantt->firstChild());
    updateChildren(&project); // don't draw project
    removeNotDrawn(m_gantt->firstChild());
    
    m_taskLinks.clear();
    drawRelations();
    
    m_gantt->setUpdateEnabled(true);
    if (m_currentItem == 0 && m_gantt->firstChild()) {
        m_gantt->firstChild()->listView()->setCurrentItem(m_gantt->firstChild());
        currentItemChanged(m_gantt->firstChild()); //hmmm
    }
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

KPTNode *KPTGanttView::getNode(KDGanttViewItem *item) const {
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

bool KPTGanttView::isDrawn(KDGanttViewItem *item) {
    if (item) {
        if (item->type() == KDGanttViewItem::Event){
            return static_cast<KPTGanttViewEventItem *>(item)->isDrawn();
        } else if (item->type() == KDGanttViewItem::Task) {
            return static_cast<KPTGanttViewTaskItem *>(item)->isDrawn();
        } else if (item->type() == KDGanttViewItem::Summary) {
            return static_cast<KPTGanttViewSummaryItem *>(item)->isDrawn();
        } else {
            kdWarning()<<k_funcinfo<<"Unknown item type: "<<item->type()<<endl;
        }
    }
    return false;
}

void KPTGanttView::setDrawn(KDGanttViewItem *item, bool state) {
    if (item) {
        if (item->type() == KDGanttViewItem::Event){
            static_cast<KPTGanttViewEventItem *>(item)->setDrawn(state);
        } else if (item->type() == KDGanttViewItem::Task) {
            static_cast<KPTGanttViewTaskItem *>(item)->setDrawn(state);
        } else if (item->type() == KDGanttViewItem::Summary) {
            static_cast<KPTGanttViewSummaryItem *>(item)->setDrawn(state);
        } else {
            kdWarning()<<k_funcinfo<<"Unknown item type: "<<item->type()<<endl;
        }
    }
    return;
}

void KPTGanttView::resetDrawn(KDGanttViewItem *_item)
{
    KDGanttViewItem *nextItem, *item=_item;
    for (; item; item = nextItem) {
        nextItem = item->nextSibling();
        setDrawn(item, false);
        resetDrawn(item->firstChild()); // then my children
    }
}

void KPTGanttView::removeNotDrawn(KDGanttViewItem *_item)
{
    KDGanttViewItem *nextItem, *item=_item;
    for (; item; item = nextItem) {
        nextItem = item->nextSibling();
        if (!isDrawn(item)) {
            deleteItem(item);
        } else {
            removeNotDrawn(item->firstChild()); // then my children
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
    QPtrListIterator<KPTNode> nit(parentNode->childNodeIterator());
    for (; nit.current(); ++nit )
    {
        updateNode(nit.current());
    }
}

void KPTGanttView::updateNode(KPTNode *node)
{
    //kdDebug()<<k_funcinfo<<node->name()<<endl;
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
    item->setListViewText(node->name());
    item->setListViewText(1, node->wbs());
    item->setStartTime(node->startTime());
    item->setEndTime(node->endTime());
    //item->setOpen(true);
    setDrawn(item, true);

}

void KPTGanttView::modifySummaryTask(KDGanttViewItem *item, KPTTask *task)
{
    //kdDebug()<<k_funcinfo<<endl;
    item->setListViewText(task->name());
    item->setListViewText(1, task->wbs());
    KPTDuration dur = task->duration();
    item->setStartTime(task->startTime());
    item->setEndTime(task->endTime());
    //item->setOpen(true);
    if (m_showTaskName) {
        item->setText(task->name());
    } else {
        item->setText(QString());
    }    
    setDrawn(item, true);
}

void KPTGanttView::modifyTask(KDGanttViewItem *item, KPTTask *task)
{
    //kdDebug()<<k_funcinfo<<endl;
    item->setListViewText(task->name());
    item->setListViewText(1, task->wbs());
    item->setStartTime(task->startTime());
    item->setEndTime(task->endTime());
    //item->setOpen(true);
    QString text;
    if (m_showTaskName) {
        text = task->name();
    }
    if (m_showResources) {
        QPtrListIterator<KPTAppointment> it = task->appointments();
        for (; it.current(); ++it) {
            if (!text.isEmpty())
                text += ", ";
            text += it.current()->resource()->name();
        }
    }
    item->setText(text);
    if (m_showProgress) {
        item->setProgress(task->progress().percentFinished);
    } else {
        item->setProgress(0);
    }
    if (m_showPositiveFloat) {
        QDateTime t = task->endTime() + task->positiveFloat();
        if (t.isValid() && t > task->endTime()) {
            item->setFloatEndTime(t);
        } else {
            item->setFloatEndTime(QDateTime());
        }
    } else {
        item->setFloatStartTime(QDateTime());
        item->setFloatEndTime(QDateTime());
    }
    //TODO i18n
    QString w="Name: " + task->name();
    
    w += "\n"; w += "Start: "  + task->startTime().toString();
    w += "\n"; w += "End  : " + task->endTime().toString();
    if (m_showProgress) {
        w += "\n"; w += "Progress (%): " + QString().setNum(task->progress().percentFinished);
    }
    w += "\n"; w += "Float: " + task->positiveFloat().toString(KPTDuration::Format_Hour);
    
    if (task->inCriticalPath()) {
        w += "\n"; w += "Critical path";
    } else if (task->isCritical()) {
        w += "\n"; w += "Critical";
    }

    QString sts;
    bool ok = true;
    if (task->notScheduled()) {
        sts += "\n"; sts += "Not scheduled";
        ok = false;
    }
    if (task->resourceError()) {
        sts += "\n"; sts += "No resource assigned";
        ok = false;
    }
    if (task->resourceOverbooked()) {
        sts += "\n"; sts += "Resource overbooked";
        ok = false;
    }
    if (task->resourceNotAvailable()) {
        sts += "\n"; sts += "Resource not available";
        ok = false;
    }
    if (task->schedulingError()) {
        sts += "\n"; sts += "Scheduling conflict";
        ok = false;
    }
    if (ok) {
        QColor c(green);
        item->setColors(c,c,c);
    } else {
        w += sts;
        QColor c(yellow);
        item->setColors(c,c,c);
    }
    item->setHighlight(false);
    if (m_showCriticalTasks) {
        item->setHighlight(task->isCritical());
    } else if (m_showCriticalPath) {
        item->setHighlight(task->inCriticalPath());
    }
    
    item->setTooltipText(w);
    setDrawn(item, true);
}

void KPTGanttView::modifyMilestone(KDGanttViewItem *item, KPTTask *task)
{
    //kdDebug()<<k_funcinfo<<endl;
    item->setListViewText(task->name());
    item->setListViewText(1, task->wbs());
    item->setStartTime(task->startTime());
    //item->setOpen(true);
    if (m_showTaskName) {
        item->setText(task->name());
    } else {
        item->setText(QString());
    }    
    if (m_showPositiveFloat) {
        KPTDateTime t = task->startTime() + task->positiveFloat();
        kdDebug()<<k_funcinfo<<task->name()<<" float: "<<t.toString()<<endl;
        if (t.isValid() && t > task->startTime()) {
            item->setFloatEndTime(t);
        } else {
            item->setFloatEndTime(QDateTime());
        }
    } else {
        item->setFloatStartTime(QDateTime());
        item->setFloatEndTime(QDateTime());
    }
    //TODO i18n
    QString w="Name: " + task->name();
    w += "\n"; w += "Time: ";
    w += task->startTime().toString();
    
    w += "\n"; w += "Float: " + task->positiveFloat().toString(KPTDuration::Format_Hour);

    if (task->inCriticalPath()) {
        w += "\n"; w += "Critical path";
    } else if (task->isCritical()) {
        w += "\n"; w += "Critical";
    }

    bool ok = true;
    if (task->notScheduled()) {
        w += "\n"; w += "Not scheduled";
        ok = false;
    }
    if (task->schedulingError()) {
        w += "\n"; w += "Scheduling conflict";
        ok = false;
    }
    if (ok) {
        QColor c(blue);
        item->setColors(c,c,c);
    } else {
        QColor c(yellow);
        item->setColors(c,c,c);
    }
    item->setHighlight(false);
    if (m_showCriticalTasks) {
        item->setHighlight(task->isCritical());
    } else if (m_showCriticalPath) {
        item->setHighlight(task->inCriticalPath());
    }
    
    item->setTooltipText(w);
    setDrawn(item, true);
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
    if (!m_showTaskLinks)
        return;
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

KPTNode *KPTGanttView::currentNode() const
{
    return getNode(m_currentItem);
}

void KPTGanttView::popupMenuRequested(KDGanttViewItem * item, const QPoint & pos, int)
{
    //kdDebug()<<k_funcinfo<<(item?item->listViewText(0):"0")<<endl;
    if (item == 0) {
        kdDebug()<<"No item selected"<<endl;
        return;
    }
    KPTNode *n = getNode(item);
    if (n == 0) {
        kdDebug()<<"No node selected"<<endl;
        return;
    }
    KPTTask *t =  dynamic_cast<KPTTask*>(n);
    if (t && (t->type() == KPTNode::Type_Task || t->type() == KPTNode::Type_Milestone)) {
        QPopupMenu *menu = m_mainview->popupMenu("task_popup");
        if (menu)
        {
            /*int id =*/ menu->exec(pos);
            //kdDebug()<<k_funcinfo<<"id="<<id<<endl;
        }
        return;
    }
    if (t && t->type() == KPTNode::Type_Summarytask) {
        QPopupMenu *menu = m_mainview->popupMenu("node_popup");
        if (menu)
        {
            /*int id =*/ menu->exec(pos);
            //kdDebug()<<k_funcinfo<<"id="<<id<<endl;
        }
        return;
    }
    //TODO: Other nodetypes
}

void KPTGanttView::slotItemDoubleClicked(KDGanttViewItem* /*item*/)
{
}

//TODO: 1) make it koffice compliant, 
//      2) allow printing on multiple pages
void KPTGanttView::print(KPrinter &prt) {
    //kdDebug()<<k_funcinfo<<endl;

    KDGanttViewItem *selItem = m_gantt->selectedItem();
    if (selItem)
        selItem->setSelected(false);
    
    //Comment from KWord
    //   We don't get valid metrics from the printer - and we want a better resolution
    //   anyway (it's the PS driver that takes care of the printer resolution).
    //But KSpread uses fixed 300 dpis, so we can use it.

    QPaintDeviceMetrics metrics( &prt );

    // get the size of the desired output for scaling.
    // here we want to print: ListView and TimeLine (default)
    // for this purpose, we call drawContents() with a 0 pointer as painter
    QSize size = m_gantt->drawContents(0);
        
    QPainter p;
    p.begin( &prt );
    
    // Make a simple header
    p.drawRect(0,0,metrics.width(),metrics.height());
    QString text = "Project: " + m_mainview->getPart()->getProject().name();
    QRect r = p.boundingRect(1,0,0,0, Qt::AlignLeft, text );
    p.drawText( r, Qt::AlignLeft, text );
    int hei = r.height();
    //kdDebug()<<"Project r="<<r.left()<<","<<r.top()<<" "<<r.width()<<"x"<<r.height()<<endl;
    text = QDateTime::currentDateTime().toString();
    r = p.boundingRect(metrics.width()-1,0,0,0, Qt::AlignRight, text );
    p.drawText( r, Qt::AlignRight, text );
    hei = QMAX(hei, r.height());
    //kdDebug()<<"Date r="<<r.left()<<","<<r.top()<<" "<<r.width()<<"x"<<r.height()<<endl;
    hei++;
    p.drawLine(0,hei,metrics.width(),hei);
    hei += 3;
    // compute the scale
    float dx = (float) (metrics.width()-2)  / (float)size.width();
    float dy  = (float)(metrics.height()-hei) / (float)size.height();
    float scale;
    // scale to fit the width or height of the paper
    if ( dx < dy )
        scale = dx;
    else
        scale = dy;
    // set the scale
    p.translate(1,hei);
    p.scale( scale, scale );
    m_gantt->drawContents(&p);
    // the drawContents() has the side effect, that the painter translation is
    // after drawContents() set to the bottom of the painted stuff
    // for instance a
    // p.drawText(0, 0, "printend");
    // would be painted directly below the paintout of drawContents()
    
    p.end();
    if (selItem)
        selItem->setSelected(true);
}

void KPTGanttView::slotItemRenamed(KDGanttViewItem* item, int col, const QString& str) {
    //kdDebug()<<k_funcinfo<<(item ? item->listViewText(col) : "null")<<": "<<str<<endl;
    if (col == 0) {
        m_mainview->renameNode(getNode(item), QString(str));
    }
}

 void KPTGanttView::slotGvItemClicked(KDGanttViewItem *) {
}

// testing
bool KPTGanttView::exportGantt(QIODevice* device) {
    kdDebug()<<k_funcinfo<<endl;
    return m_gantt->saveProject(device);
}

void KPTGanttView::slotLinkItems(KDGanttViewItem* from, KDGanttViewItem* to, int linkType) {
    kdDebug()<<k_funcinfo<<(from?from->listViewText():"null")<<" to "<<(to?to->listViewText():"null")<<" linkType="<<linkType<<endl;
    KPTNode *par = getNode(from);
    KPTNode *child = getNode(to);
    if (!par || !child || !(par->legalToLink(child))) {
        KMessageBox::sorry(this, i18n("Cannot link these nodes"));
        return;
    }
    KPTRelation *rel = child->findRelation(par);
    if (rel)
        emit modifyRelation(rel, linkTypeToRelation(linkType));
    else
        emit addRelation(par, child, linkTypeToRelation(linkType));
    
    return;
}

int KPTGanttView::linkTypeToRelation(int linkType) {
    switch (linkType) {
        case KDGanttViewTaskLink::FinishStart: 
            return KPTRelation::FinishStart;
            break;
        case KDGanttViewTaskLink::StartStart: 
            return KPTRelation::StartStart;
            break;
        case KDGanttViewTaskLink::FinishFinish: 
            return KPTRelation::FinishFinish;
            break;
        case KDGanttViewTaskLink::StartFinish:
        default: 
            return -1;
            break;
    }
}

void KPTGanttView::slotModifyLink(KDGanttViewTaskLink* link) {
    //kdDebug()<<k_funcinfo<<link<<endl;
    // we support only one from/to item in each link
    KPTNode *par = getNode(link->from().first());
    KPTRelation *rel = par->findRelation(getNode(link->to().first()));
    if (rel)
        emit modifyRelation(rel);
}

bool KPTGanttView::setContext(KPTContext &context) {
    kdDebug()<<k_funcinfo<<endl;
    
    QValueList<int> list = sizes();
    list[0] = context.ganttviewsize;
    list[1] = context.taskviewsize;
    setSizes(list);    

    KPTProject &p = m_mainview->getProject();
    currentItemChanged(findItem(p.findNode(context.currentNode)));
    
    m_showResources = context.showResources ;
    m_showTaskName = context.showTaskName;
    m_showTaskLinks = context.showTaskLinks;
    m_showProgress = context.showProgress;
    m_showPositiveFloat = context.showPositiveFloat;
    m_showCriticalTasks = context.showCriticalTasks;
    m_showCriticalPath = context.showCriticalPath;
    
    getContextClosedNodes(context, m_gantt->firstChild());
    for (QStringList::ConstIterator it = context.closedNodes.begin(); it != context.closedNodes.end(); ++it) {
        KDGanttViewItem *item = findItem(p.findNode(*it));
        if (item) {
            item->setOpen(false);
        }
    }
    return true;
}

void KPTGanttView::getContext(KPTContext &context) const {
    //kdDebug()<<k_funcinfo<<endl;
    context.ganttviewsize = sizes()[0];
    context.taskviewsize = sizes()[1];
    kdDebug()<<k_funcinfo<<"sizes="<<sizes()[0]<<","<<sizes()[1]<<endl;
    if (currentNode()) {
        context.currentNode = currentNode()->id();
    }
    context.showResources = m_showResources;
    context.showTaskName = m_showTaskName;
    context.showTaskLinks = m_showTaskLinks;
    context.showProgress = m_showProgress;
    context.showPositiveFloat = m_showPositiveFloat;
    context.showCriticalTasks = m_showCriticalTasks;
    context.showCriticalPath = m_showCriticalPath;
    getContextClosedNodes(context, m_gantt->firstChild());
}

void KPTGanttView::getContextClosedNodes(KPTContext &context, KDGanttViewItem *item) const {
    if (item == 0)
        return;
    for (KDGanttViewItem *i = item; i; i = i->nextSibling()) {
        if (!i->isOpen()) {
            context.closedNodes.append(getNode(i)->id());
            //kdDebug()<<k_funcinfo<<"add closed "<<i->listViewText()<<endl;
        }
        getContextClosedNodes(context, i->firstChild());
    }
}

void KPTGanttView::setReadWriteMode(bool on) {
    m_readWrite = on;
    disconnect(m_gantt, SIGNAL(linkItems(KDGanttViewItem*, KDGanttViewItem*, int)), this, SLOT(slotLinkItems(KDGanttViewItem*, KDGanttViewItem*, int)));
    disconnect(m_gantt, SIGNAL(taskLinkDoubleClicked(KDGanttViewTaskLink*)), this, SLOT(slotModifyLink(KDGanttViewTaskLink*)));
    m_gantt->setLinkItemsEnabled(on);

    if (on) {
        connect(m_gantt, SIGNAL(linkItems(KDGanttViewItem*, KDGanttViewItem*, int)), SLOT(slotLinkItems(KDGanttViewItem*, KDGanttViewItem*, int)));
        
        connect(m_gantt, SIGNAL(taskLinkDoubleClicked(KDGanttViewTaskLink*)), SLOT(slotModifyLink(KDGanttViewTaskLink*)));
    }
    setRenameEnabled(m_gantt->firstChild(), on);
}

void KPTGanttView::setRenameEnabled(QListViewItem *item, bool on) {
    if (item == 0)
        return;
    for (QListViewItem *i = item; i; i = i->nextSibling()) {
        i->setRenameEnabled(0, on);
        setRenameEnabled(i->firstChild(), on);
    }
}

}  //KPlato namespace

#include "kptganttview.moc"
