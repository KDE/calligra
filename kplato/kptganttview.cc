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

KPTTaskAppointmentsView::KPTTaskAppointmentsView(QWidget *parent, const char* name)
    : QWidget(parent, name),
    appList(0),
    m_taskName(0),
    m_responsible(0),
    m_costToDate(0),
    m_totalCost(0),
    m_workToDate(0),
    m_totalWork(0),
    m_completion(0),
    m_deviationCost(0),
    m_deviationWork(0)
{
    QGridLayout *layout = new QGridLayout(this, 4, 11, 8, 2);
    layout->setRowStretch(0, 0);
    layout->setRowStretch(1, 0);
    layout->setRowStretch(2, 0);
    layout->setRowStretch(3, 0);
    layout->setColSpacing(2,10);
    layout->setColSpacing(5,10);

    layout->addWidget(new QLabel(i18n("Task name: "), this), 0, 0);
    m_taskName = new QLineEdit(this);
    layout->addMultiCellWidget(m_taskName, 0, 0, 1, 4);
    layout->addWidget(new QLabel(i18n("Responsible: "), this), 0, 6);
    m_responsible = new QLineEdit(this);
    layout->addMultiCellWidget(m_responsible, 0, 0, 7, 10);

    layout->addWidget(new QLabel(i18n("Cost to date: "), this), 1, 0);
    m_costToDate = new QLineEdit(this);
    layout->addWidget(m_costToDate, 1, 1);
    layout->addWidget(new QLabel(i18n("Deviation: "), this), 1, 3);
    m_deviationCost = new QLineEdit(this);
    layout->addWidget(m_deviationCost, 1, 4);
    layout->addWidget(new QLabel(i18n("Total planned cost: "), this), 1, 6);
    m_totalCost = new QLineEdit(this);
    layout->addWidget(m_totalCost, 1, 7);

    layout->addWidget(new QLabel(i18n("Work to date: "), this), 2, 0);
    m_workToDate = new QLineEdit(this);
    layout->addWidget(m_workToDate, 2, 1);
    layout->addWidget(new QLabel(i18n("Deviation: "), this), 2, 3);
    m_deviationWork = new QLineEdit(this);
    layout->addWidget(m_deviationWork, 2, 4);
    layout->addWidget(new QLabel(i18n("Total planned work: "), this), 2, 6);
    m_totalWork = new QLineEdit(this);
    layout->addWidget(m_totalWork, 2, 7);

    layout->addWidget(new QLabel(i18n("Completion: "), this), 2, 9);
    QSpinBox *m_completion = new QSpinBox(this);
    m_completion->setSuffix(i18n("%"));
    m_completion->setDisabled(true);
    layout->addWidget(m_completion, 2, 10);

    QTabWidget *resTab = new QTabWidget(this);
    layout->addMultiCellWidget(resTab, 3, 4, 0, 10);
    appList = new QListView(resTab, "Appointments view");
    appList->addColumn(i18n("Resource"));
    appList->addColumn(i18n("Type"));
    appList->setColumnAlignment(1, AlignHCenter);
    appList->addColumn(i18n("Start date"));
    appList->addColumn(i18n("Duration"));
    appList->setColumnAlignment(3, AlignRight);
    appList->addColumn(i18n("Normal rate"));
    appList->setColumnAlignment(4, AlignRight);
    appList->addColumn(i18n("Overtime rate"));
    appList->setColumnAlignment(5, AlignRight);
    appList->addColumn(i18n("Fixed cost"));
    appList->setColumnAlignment(6, AlignRight);

    resTab->addTab(appList, i18n("Appointments"));

}

void KPTTaskAppointmentsView::clear()
{
    if (appList) appList->clear();
    if (m_taskName) m_taskName->clear();
    if (m_responsible) m_responsible->clear();
    if (m_costToDate) m_costToDate->clear();
    if (m_totalCost) m_totalCost->clear();
    if (m_workToDate) m_workToDate->clear();
    if (m_totalWork) m_totalWork->clear();
}

void KPTTaskAppointmentsView::draw(KPTTask *task)
{
    //kdDebug()<<k_funcinfo<<endl;
    clear();
    if (!task)
        return;
    m_taskName->setText(task->name());
    m_responsible->setText(task->leader());
    QDateTime dt = QDateTime::currentDateTime();
    m_costToDate->setText(KGlobal::locale()->formatMoney(task->plannedCost(dt)));
    m_totalCost->setText(KGlobal::locale()->formatMoney(task->plannedCost()));
    m_workToDate->setText(QString("%1").arg(task->plannedWork(dt)));
    m_totalWork->setText(QString("%1").arg(task->plannedWork()));

    KPTProject *p = dynamic_cast<KPTProject *>(task->projectNode());
    if (!p) {
        kdError()<<k_funcinfo<<"Task: '"<<task->name()<<"' has no project"<<endl;
        return;
    }
    QPtrListIterator<KPTResourceGroup> it(p->resourceGroups());
    for (; it.current(); ++it) {
        QPtrListIterator<KPTResource> rit(it.current()->resources());
        for (; rit.current(); ++rit) {
            KPTResource *r = rit.current();
            QPtrListIterator<KPTAppointment> ait(rit.current()->appointments());
            for (; ait.current(); ++ait) {
                if (ait.current()->task() == task) {
                    QListViewItem *item = new QListViewItem(appList, r->name());
                    item->setText(1, r->typeToString());
                    item->setText(2, ait.current()->startTime().date().toString());
                    item->setText(3, ait.current()->duration().toString(KPTDuration::Format_Hour));
                    item->setText(4, KGlobal::locale()->formatMoney(r->normalRate()));
                    item->setText(5, KGlobal::locale()->formatMoney(r->overtimeRate()));
                    item->setText(6, KGlobal::locale()->formatMoney(r->fixedCost()));
                }
            }
        }
    }
}

KPTGanttView::KPTGanttView( KPTView *view, QWidget *parent, const char* name)
    : QSplitter(parent, name),
    m_mainview( view ),
	m_currentItem(0),
    m_taskView(0)
{

    setOrientation(QSplitter::Vertical);

    m_gantt = new KDGanttView(this, "Gantt view");
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

	m_gantt->setUpdateEnabled(true);
}


void KPTGanttView::drawChildren(KDGanttViewSummaryItem *parentItem, KPTNode &parentNode)
{
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
	KPTDateTime *time = node->getStartTime();
	KPTDuration *dur = node->getExpectedDuration();
    if (*dur == KPTDuration::zeroDuration)
        dur->addSecs(1); // avoid bug in KDGannt
	KPTGanttViewSummaryItem *item;
	if ( parentItem) {
	  item = new KPTGanttViewSummaryItem(parentItem, node);
	}
	else {
		// we are on the top level
		item = new KPTGanttViewSummaryItem(m_gantt, node);
	}

	item->setStartTime(*time);
	item->setEndTime(*time + *dur);
	item->setOpen(true);

    delete time;
	delete dur;

	drawChildren(item, *node);
}

void KPTGanttView::drawSubProject(KDGanttViewSummaryItem *parentItem, KPTNode *node)
{
	KPTDateTime *time = node->getStartTime();
	KPTDuration *dur = node->getExpectedDuration();
    if (*dur == KPTDuration::zeroDuration)
        dur->addSecs(1); // avoid bug in KDGannt
	// display summary item
	KPTGanttViewSummaryItem *item;
	if ( parentItem) {
		item = new KPTGanttViewSummaryItem(parentItem, node);
	}
	else {
		// we are on the top level
		item = new KPTGanttViewSummaryItem(m_gantt, node);
	}
	item->setStartTime(*time);
	item->setEndTime(*time + *dur);
	item->setOpen(true);

	drawChildren(item, *node);

	delete time;
	delete dur;
}

void KPTGanttView::drawSummaryTask(KDGanttViewSummaryItem *parentItem, KPTTask *task)
{
	KPTDateTime *time = task->getStartTime();
	KPTDuration *dur = task->getExpectedDuration();
    if (*dur == KPTDuration::zeroDuration)
        dur->addSecs(1); // avoid bug in KDGannt
	// display summary item
	KPTGanttViewSummaryItem *item;
	if ( parentItem) {
		item = new KPTGanttViewSummaryItem(parentItem, task);
	}
	else {
		// we are on the top level
		item = new KPTGanttViewSummaryItem(m_gantt, task);
	}
	item->setStartTime(*time);
	item->setEndTime(*time + *dur);
	item->setOpen(true);

	drawChildren(item, *task);

	delete time;
	delete dur;
}

void KPTGanttView::drawTask(KDGanttViewSummaryItem *parentItem, KPTTask *task)
{
	KPTDateTime *time = task->getStartTime();
	KPTDuration *dur = task->getExpectedDuration();
    if (*dur == KPTDuration::zeroDuration)
        dur->addSecs(1); // avoid bug in KDGannt
	// display task item
	KPTGanttViewTaskItem *item;
	if ( parentItem ) {
		item = new KPTGanttViewTaskItem(parentItem, task);
	}
	else {
		// we are on the top level
		item = new KPTGanttViewTaskItem(m_gantt, task);
	}
	item->setStartTime(*time);
	item->setEndTime(*time + *dur);
	item->setOpen(true);
    if (task->resourceOverbooked() || task->resourceError()) {
        QColor c(yellow);
        item->setColors(c,c,c);
        //kdDebug()<<k_funcinfo<<"Task: "<<task->name()<<" resourceError="<<task->resourceError()<<endl;
    }

	delete time;
	delete dur;
}

void KPTGanttView::drawMilestone(KDGanttViewSummaryItem *parentItem, KPTTask *task)
{
	KPTDateTime *time = task->getStartTime();
	KPTGanttViewEventItem *item;
	if ( parentItem ) {
		item = new KPTGanttViewEventItem(parentItem, task);
	}
	else {
		// we are on the top level
		item = new KPTGanttViewEventItem(m_gantt, task);
	}
	item->setStartTime(*time);
	item->setOpen(true);

    delete time;
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
