/* This file is part of the KDE project
   Copyright (C) 2002 - 2005 Dag Andersen <danders@get2net.dk>
   Copyright (C) 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>

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

#include "kptappointment.h"
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
#include "kptschedule.h"

#include "KDGanttView.h"
#include "KDGanttViewItem.h"
#include "KDGanttViewTaskItem.h"
#include "KDGanttViewSummaryItem.h"
#include "KDGanttViewEventItem.h"

#include <kdebug.h>

#include <qsplitter.h>
#include <q3vbox.h>
#include <QLayout>
#include <q3listview.h>
#include <q3header.h>
#include <q3popupmenu.h>
#include <qtabwidget.h>
#include <q3ptrlist.h>
#include <QLineEdit>
#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <q3valuelist.h>
#include <qpainter.h>
#include <q3paintdevicemetrics.h>

#include <klocale.h>
#include <kglobal.h>
#include <kprinter.h>
#include <kmessagebox.h>

namespace KPlato
{

class MyKDGanttView : public KDGanttView {
public:
    MyKDGanttView(QWidget *parent, const char *name)
    : KDGanttView(parent, name) {
    }
    virtual QSize sizeHint() const {
        return minimumSizeHint(); //HACK: koshell splitter minimumSize problem
    }
};

GanttView::GanttView(QWidget *parent, bool readWrite, const char* name)
    : QSplitter(parent, name),
    m_readWrite(readWrite),
    m_currentItem(0),
    m_taskView(0),
    m_firstTime(true),
    m_project(0)
{
    kDebug() << " ---------------- KPlato: Creating GanttView ----------------" << endl;
    setOrientation(Qt::Vertical);

    m_gantt = new MyKDGanttView(this, "Gantt view");

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

    m_gantt->setHeaderVisible(true);
    m_gantt->addColumn(i18nc("Work Breakdown Structure", "WBS"));
    // HACK: need changes to kdgantt
    KDGanttViewTaskItem *item = new KDGanttViewTaskItem(m_gantt);
    Q3ListView *lv = item->listView();
    lv->header()->moveSection(1, 0);

    m_gantt->setScale(KDGanttView::Day);
    m_gantt->setShowLegendButton(false);
    m_gantt->setShowHeaderPopupMenu();
    m_taskView = new TaskAppointmentsView(this);
    // hide TaskAppointmentsView
    Q3ValueList<int> list = sizes();
    list[0] += list[1];
    list[1] = 0;
    setSizes(list);
    m_taskView->hide();

    setReadWriteMode(readWrite);

	connect(m_gantt, SIGNAL(lvContextMenuRequested ( KDGanttViewItem *, const QPoint &, int )),
	             this, SLOT (popupMenuRequested(KDGanttViewItem *, const QPoint &, int)));

	connect(m_gantt, SIGNAL(lvCurrentChanged(KDGanttViewItem*)), this, SLOT (currentItemChanged(KDGanttViewItem*)));

    // HACK: kdgantt emits 2 signals for each *double* click, so we go direct to listview
	connect(lv, SIGNAL(doubleClicked(Q3ListViewItem*, const QPoint&, int)), this, SLOT (slotItemDoubleClicked(Q3ListViewItem*)));

    m_taskLinks.setAutoDelete(true);

    if (m_gantt->firstChild()) {
        m_gantt->firstChild()->listView()->setCurrentItem(m_gantt->firstChild());
        m_gantt->firstChild()->listView()->setFocus();
    }
}

void GanttView::setZoom(double zoom)
{
  kDebug() << "setting gantt zoom: " << zoom << endl;
  m_gantt->setZoomFactor(zoom,true);
  m_taskView->zoom( zoom );
}

void GanttView::clear()
{
    m_gantt->clear();
    m_taskView->clear();
}

void GanttView::draw(Project &project)
{
    m_project = &project;
    //kDebug()<<k_funcinfo<<endl;
    Schedule::Type type = Schedule::Expected;
    if (m_showOptimistic) {
        type = Schedule::Optimistic;
    } else if (m_showPessimistic) {
        type = Schedule::Pessimistic;
    }
    Schedule *sch = project.findSchedule(type);
    if (sch) {
        project.setCurrentSchedule(sch->id());
    }
    //kDebug()<<k_funcinfo<<"Schedule: "<<(sch?sch->typeToString():"None")<<endl;
    m_gantt->setUpdateEnabled(false);

    clear();
    drawChildren(NULL, project);
    drawRelations();

    if (m_firstTime) {
        m_gantt->centerTimelineAfterShow(project.startTime().addDays(-1));
        m_firstTime = false;
    }
    m_gantt->setUpdateEnabled(true);
    currentItemChanged(m_currentItem);
}

void GanttView::drawChanges(Project &project)
{
    m_project = &project; //FIXME Only draw changes on same project
    //kDebug()<<k_funcinfo<<endl;
    Schedule::Type type = Schedule::Expected;
    if (m_showOptimistic) {
        type = Schedule::Optimistic;
    } else if (m_showPessimistic) {
        type = Schedule::Pessimistic;
    }
    Schedule *sch = project.findSchedule(type);
    if (sch) {
        project.setCurrentSchedule(sch->id());
    }
    //kDebug()<<k_funcinfo<<"Schedule: "<<(sch?sch->typeToString():"None")<<endl;
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
    currentItemChanged(m_currentItem);
}

void GanttView::drawOnPainter(QPainter* painter, const QRect rect)
{
    // Assume clipping is allready set

    // Fill out the rect by adding ticks to right side of the timeline
    QSize s = m_gantt->drawContents(0, false, true);
    while (s.width() < rect.width()) {
        m_gantt->addTicksRight();
        m_gantt->setTimelineToEnd();
        s = m_gantt->drawContents(0, false, true);
    }
    kDebug()<<k_funcinfo<<rect<<" : "<<s<<endl;
    painter->save();

//    QValueList<int> sizes = m_taskView->sizes();
//    if (sizes.count() >= 2)
//    {
//        int first = sizes[0];
//        int second = sizes[1];
//        sizes.pop_front();
//        sizes.pop_front();
//        sizes.prepend(first+second);
//        sizes.prepend(0);
//        m_taskView->setSizes(sizes);
//    }
//    else
//        kWarning() << "Apparently the task view splitter contains less than 2 parts!" << endl;

//    bool showlistview = m_gantt->showListView();
//    int listviewwidth = m_gantt->listViewWidth();
//    m_gantt->setShowListView(false);
//    m_gantt->setListViewWidth(0);

//    m_gantt->setGanttMaximumWidth(rect.x());
    m_gantt->drawContents(painter,false,true);
//    m_gantt->setShowListView(showlistview);
//    m_gantt->setListViewWidth(listviewwidth);

//   m_taskView->drawContents(painter); //TODO doesn't seem to do very much
    painter->restore();
}

KDGanttViewItem *GanttView::findItem(Node *node)
{
    return findItem(node, m_gantt->firstChild());
}

KDGanttViewItem *GanttView::findItem(Node *node, KDGanttViewItem *item)
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

Node *GanttView::getNode(KDGanttViewItem *item) const {
    if (item) {
        if (item->type() == KDGanttViewItem::Event){
            return static_cast<GanttViewEventItem *>(item)->getTask();
        } else if (item->type() == KDGanttViewItem::Task) {
            return static_cast<GanttViewTaskItem *>(item)->getTask();
        } else if (item->type() == KDGanttViewItem::Summary) {
            return static_cast<GanttViewSummaryItem *>(item)->getNode();
        }
    }
    return 0;
}

bool GanttView::isDrawn(KDGanttViewItem *item) {
    if (item) {
        if (item->type() == KDGanttViewItem::Event){
            return static_cast<GanttViewEventItem *>(item)->isDrawn();
        } else if (item->type() == KDGanttViewItem::Task) {
            return static_cast<GanttViewTaskItem *>(item)->isDrawn();
        } else if (item->type() == KDGanttViewItem::Summary) {
            return static_cast<GanttViewSummaryItem *>(item)->isDrawn();
        } else {
            kWarning()<<k_funcinfo<<"Unknown item type: "<<item->type()<<endl;
        }
    }
    return false;
}

void GanttView::setDrawn(KDGanttViewItem *item, bool state) {
    if (item) {
        if (item->type() == KDGanttViewItem::Event){
            static_cast<GanttViewEventItem *>(item)->setDrawn(state);
        } else if (item->type() == KDGanttViewItem::Task) {
            static_cast<GanttViewTaskItem *>(item)->setDrawn(state);
        } else if (item->type() == KDGanttViewItem::Summary) {
            static_cast<GanttViewSummaryItem *>(item)->setDrawn(state);
        } else {
            kWarning()<<k_funcinfo<<"Unknown item type: "<<item->type()<<endl;
        }
    }
    return;
}

void GanttView::resetDrawn(KDGanttViewItem *_item)
{
    KDGanttViewItem *nextItem, *item=_item;
    for (; item; item = nextItem) {
        nextItem = item->nextSibling();
        setDrawn(item, false);
        resetDrawn(item->firstChild()); // then my children
    }
}

void GanttView::removeNotDrawn(KDGanttViewItem *_item)
{
    KDGanttViewItem *nextItem, *item=_item;
    for (; item; item = nextItem) {
        nextItem = item->nextSibling();
        if (!isDrawn(item)) {
            if (item == m_currentItem)
                m_currentItem = 0;
            deleteItem(item);
        } else {
            removeNotDrawn(item->firstChild()); // then my children
        }
    }
}

void GanttView::deleteItem(KDGanttViewItem *item)
{
    //kDebug()<<k_funcinfo<<item->listViewText()<<endl;
    if (item->parent())
        item->parent()->takeItem(item);
    else
        item->listView()->takeItem(item);
    delete item;
}

KDGanttViewItem *GanttView::correctType(KDGanttViewItem *item, Node *node)
{
    //kDebug()<<k_funcinfo<<item->listViewText()<<": "<<item->type()<<" node: "<<node->type()<<endl;
    switch (node->type()) {
        case Node::Type_Project:
            return item;
            break;
        case Node::Type_Summarytask:
        case Node::Type_Subproject:
            if (item->type() == KDGanttViewItem::Summary)
                return item;
            break;
        case Node::Type_Task:
            if (item->type() == KDGanttViewItem::Task)
                return item;
            break;
        case Node::Type_Milestone:
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

void GanttView::correctPosition(KDGanttViewItem *item, Node *node)
{
    KDGanttViewItem *after = findItem(node->siblingBefore());
    if (after) {
        item->moveItem(after);
    }
}

KDGanttViewItem *GanttView::correctParent(KDGanttViewItem *item, Node *node)
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

void GanttView::updateChildren(Node *parentNode)
{
    //kDebug()<<k_funcinfo<<endl;
    Q3PtrListIterator<Node> nit(parentNode->childNodeIterator());
    for (; nit.current(); ++nit )
    {
        updateNode(nit.current());
    }
}

void GanttView::updateNode(Node *node)
{
    //kDebug()<<k_funcinfo<<node->name()<<endl;
    KDGanttViewItem *item = findItem(node);
    if (!item) {
        item = addNode(findItem(node->getParent()), node, findItem(node->siblingBefore()));
        if (item && node->type() == Node::Type_Summarytask)
            updateChildren(node);
        return;
    }
    item = correctType(item, node);
    item = correctParent(item, node);
    correctPosition(item, node);

    modifyNode(node);

    if (node->type() == Node::Type_Summarytask)
        updateChildren(node);
}

void GanttView::modifyChildren(Node *node)
{
    //kDebug()<<k_funcinfo<<endl;
    Q3PtrListIterator<Node> nit(node->childNodeIterator());
    for ( nit.toLast(); nit.current(); --nit ) {
        modifyNode(nit.current());
        modifyChildren(nit.current());
    }
}

void GanttView::modifyNode(Node *node)
{
    //kDebug()<<k_funcinfo<<endl;
    KDGanttViewItem *item = findItem(node);
    if (!item) {
        kDebug()<<k_funcinfo<<" Item not found"<<endl;
        return;
    }
    if (node->type() == Node::Type_Project) {
        return modifyProject(item, node);
    }
    if (node->type() == Node::Type_Subproject) {
        return modifyProject(item, node);
    }
    if (node->type() == Node::Type_Summarytask) {
        return modifySummaryTask(item, static_cast<Task *>(node));
    }
    if (node->type() == Node::Type_Task) {
        return modifyTask(item, static_cast<Task *>(node));
    }
    if (node->type() == Node::Type_Milestone) {
        return modifyMilestone(item, static_cast<Task *>(node));
    }
    return;
}

void GanttView::modifyProject(KDGanttViewItem *item, Node *node)
{
    //kDebug()<<k_funcinfo<<endl;
    item->setListViewText(node->name());
    item->setListViewText(1, node->wbs());
    item->setStartTime(node->startTime());
    item->setEndTime(node->endTime());
    //item->setOpen(true);
    setDrawn(item, true);

}

void GanttView::modifySummaryTask(KDGanttViewItem *item, Task *task)
{
    //kDebug()<<k_funcinfo<<endl;
    KLocale *locale = KGlobal::locale();
    //kDebug()<<k_funcinfo<<task->name()<<": "<<task->currentSchedule()<<", "<<task->notScheduled()<<", "<<(m_project ? m_project->notScheduled() : false)<<endl;
    if (task->currentSchedule() == 0) {
        item->setShowNoInformation(m_showNoInformation);
        item->setStartTime(task->projectNode()->startTime());
        item->setEndTime(item->startTime().addDays(1));
    } else {
        bool noinf = m_showNoInformation && (task->notScheduled() || (m_project ? m_project->notScheduled() : false /*hmmm, no project?*/));
        item->setShowNoInformation(noinf);
        item->setStartTime(task->startTime());
        item->setEndTime(task->endTime());
    }
    item->setListViewText(task->name());
    item->setListViewText(1, task->wbs());
    //item->setOpen(true);
    if (m_showTaskName) {
        item->setText(task->name());
    } else {
        item->setText(QString());
    }
    QString w = i18n("Name: %1", task->name());
    if (!task->notScheduled()) {
        w += "\n" + i18n("Start: %1", locale->formatDateTime(task->startTime()));
        w += "\n" + i18n("End: %1", locale->formatDateTime(task->endTime()));
    }
    bool ok = true;
    if (task->notScheduled()) {
        w += "\n" + i18n("Not scheduled");
        ok = false;
    } else {
        if (!m_showNoInformation && m_project && m_project->notScheduled()) {
            ok = false;
        }
    }
    if (ok) {
        QColor c(Qt::cyan);
        item->setColors(c,c,c);
    } else {
        QColor c(Qt::yellow);
        item->setColors(c,c,c);
    }
    item->setTooltipText(w);
    setDrawn(item, true);
}

void GanttView::modifyTask(KDGanttViewItem *item, Task *task)
{
    //kDebug()<<k_funcinfo<<endl;
    KLocale *locale = KGlobal::locale();
    //kDebug()<<k_funcinfo<<task->name()<<": "<<task->currentSchedule()<<", "<<task->notScheduled()<<", "<<(m_project ? m_project->notScheduled() : false)<<endl;
    item->setListViewText(task->name());
    item->setListViewText(1, task->wbs());
    if (task->currentSchedule() == 0) {
        item->setShowNoInformation(m_showNoInformation);
        item->setStartTime(task->projectNode()->startTime());
        item->setEndTime(item->startTime().addDays(1));
    } else {
        bool noinf = m_showNoInformation && (task->notScheduled() || (m_project ? m_project->notScheduled() : false /*hmmm, no project?*/));
        item->setShowNoInformation(noinf);
        item->setStartTime(task->startTime());
        item->setEndTime(task->endTime());
    }
    //item->setOpen(true);
    QString text;
    if (m_showTaskName) {
        text = task->name();
    }
    if (m_showResources && !task->notScheduled()) {
        Q3PtrList<Appointment> lst = task->appointments();
        if (lst.count() > 0) {
            if (!text.isEmpty())
                text += ' ';
            text += '(';
            Q3PtrListIterator<Appointment> it = lst;
            for (bool first=true; it.current(); ++it) {
                if (!first)
                    text += ", ";
                text += it.current()->resource()->resource()->name();
                first = false;
            }
            text += ')';
        }
    }
    item->setText(text);
#ifdef __GNUC__
#warning what happened to KDGanttViewItem::setProgress and setFloatStart/EndTime?
#endif
#if 0
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
#endif
    QString w = i18n("Name: %1", task->name());
    if (!task->notScheduled()) {
        w += "\n"; w += i18n("Start: %1", locale->formatDateTime(task->startTime()));
        w += "\n"; w += i18n("End: %1", locale->formatDateTime(task->endTime()));
        if (m_showProgress) {
            w += "\n"; w += i18n("Completion: %1%", task->progress().percentFinished);
        }
        if (task->positiveFloat() > Duration::zeroDuration) {
            w += "\n" + i18n("Float: %1", task->positiveFloat().toString(Duration::Format_i18nDayTime));
        }
        if (task->inCriticalPath()) {
            w += "\n" + i18n("Critical path");
        } else if (task->isCritical()) {
            w += "\n" + i18n("Critical");
        }
    }
    QString sts;
    bool ok = true;
    if (task->notScheduled()) {
        sts += "\n" + i18n("Not scheduled");
        ok = false;
    } else {
        if (task->resourceError()) {
            sts += "\n" + i18n("No resource assigned");
            ok = false;
        }
        if (task->resourceOverbooked()) {
            sts += "\n" + i18n("Resource overbooked");
            ok = false;
        }
        if (task->resourceNotAvailable()) {
            sts += "\n" + i18n("Resource not available");
            ok = false;
        }
        if (task->schedulingError()) {
            sts += "\n" + i18n("Scheduling conflict");
            ok = false;
        }
        if (!m_showNoInformation && m_project && m_project->notScheduled()) {
            ok = false;
        }
    }
    if (ok) {
        QColor c(Qt::green);
        item->setColors(c,c,c);
    } else {
        w += sts;
        QColor c(Qt::yellow);
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

void GanttView::modifyMilestone(KDGanttViewItem *item, Task *task)
{
    //kDebug()<<k_funcinfo<<endl;
    KLocale *locale = KGlobal::locale();
    //kDebug()<<k_funcinfo<<task->name()<<": "<<task->currentSchedule()<<", "<<task->notScheduled()<<", "<<(m_project ? m_project->notScheduled() : false)<<endl;
    if (task->currentSchedule() == 0) {
        item->setShowNoInformation(m_showNoInformation);
        item->setStartTime(task->projectNode()->startTime());
    } else {
        bool noinf = m_showNoInformation && (task->notScheduled() || (m_project ? m_project->notScheduled() : false /*hmmm, no project?*/));
        item->setShowNoInformation(noinf);
        item->setStartTime(task->startTime());
    }
    item->setListViewText(task->name());
    item->setListViewText(1, task->wbs());
    //item->setOpen(true);
    if (m_showTaskName) {
        item->setText(task->name());
    } else {
        item->setText(QString());
    }
#ifdef __GNUC__
#warning what happened to KDGanttViewItem::setFloatStart/EndTime?
#endif
#if 0
    if (m_showPositiveFloat) {
        DateTime t = task->startTime() + task->positiveFloat();
        //kDebug()<<k_funcinfo<<task->name()<<" float: "<<t.toString()<<endl;
        if (t.isValid() && t > task->startTime()) {
            item->setFloatEndTime(t);
        } else {
            item->setFloatEndTime(QDateTime());
        }
    } else {
        item->setFloatStartTime(QDateTime());
        item->setFloatEndTime(QDateTime());
    }
#endif
    //TODO: Show progress

    QString w = i18n("Name: %1", task->name());
    if (!task->notScheduled()) {
        w += "\n" + i18n("Time: %1", locale->formatDateTime(task->startTime()));

        if (task->positiveFloat() > Duration::zeroDuration) {
            w += "\n" + i18n("Float: %1", task->positiveFloat().toString(Duration::Format_i18nDayTime));
        }
        if (task->inCriticalPath()) {
            w += "\n" + i18n("Critical path");
        } else if (task->isCritical()) {
            w += "\n" + i18n("Critical");
        }
    }
    bool ok = true;
    if (task->notScheduled()) {
        w += "\n" + i18n("Not scheduled");
        ok = false;
    } else {
        if (task->schedulingError()) {
            w += "\n" + i18n("Scheduling conflict");
            ok = false;
        }
        if (!m_showNoInformation && m_project && m_project->notScheduled()) {
            ok = false;
        }
    }
    if (ok) {
        QColor c(Qt::blue);
        item->setColors(c,c,c);
    } else {
        QColor c(Qt::yellow);
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

KDGanttViewItem *GanttView::addNode( KDGanttViewItem *parentItem, Node *node, KDGanttViewItem *after)
{
    //kDebug()<<k_funcinfo<<endl;
    if (node->type() == Node::Type_Project) {
        return addProject(parentItem, node, after);
    }
    if (node->type() == Node::Type_Subproject) {
        return addSubProject(parentItem, node, after);
    }
    if (node->type() == Node::Type_Summarytask) {
        return addSummaryTask(parentItem, static_cast<Task *>(node), after);
    }
    if (node->type() == Node::Type_Task) {
        return addTask(parentItem, static_cast<Task *>(node), after);
    }
    if (node->type() == Node::Type_Milestone) {
        return addMilestone(parentItem, static_cast<Task *>(node), after);
    }
    return 0;
}

KDGanttViewItem *GanttView::addProject(KDGanttViewItem *parentItem, Node *node, KDGanttViewItem *after)
{
    //kDebug()<<k_funcinfo<<endl;
    GanttViewSummaryItem *item;
    if ( parentItem) {
        item = new GanttViewSummaryItem(parentItem, node);
    } else {
        // we are on the top level
        item = new GanttViewSummaryItem(m_gantt, node);
    }
    if (after)
        item->moveItem(after);
    modifyProject(item, node);
    return item;
}

KDGanttViewItem *GanttView::addSubProject(KDGanttViewItem *parentItem, Node *node, KDGanttViewItem *after)
{
    //kDebug()<<k_funcinfo<<endl;
    return addProject(parentItem, node, after);
}

KDGanttViewItem *GanttView::addSummaryTask(KDGanttViewItem *parentItem, Task *task, KDGanttViewItem *after)
{
    //kDebug()<<k_funcinfo<<endl;
    // display summary item
    GanttViewSummaryItem *item;
    if ( parentItem) {
        item = new GanttViewSummaryItem(parentItem, task);
    } else {
        // we are on the top level
        item = new GanttViewSummaryItem(m_gantt, task);
    }
    if (after)
        item->moveItem(after);
    modifySummaryTask(item, task);
    return item;
}

KDGanttViewItem *GanttView::addTask(KDGanttViewItem *parentItem, Task *task, KDGanttViewItem *after)
{
    //kDebug()<<k_funcinfo<<endl;
    // display task item
    GanttViewTaskItem *item;
    if ( parentItem ) {
        item = new GanttViewTaskItem(parentItem, task);
    }
    else {
        // we are on the top level
        item = new GanttViewTaskItem(m_gantt, task);
    }
    if (after)
        item->moveItem(after);
    modifyTask(item, task);
    return item;
}

KDGanttViewItem *GanttView::addMilestone(KDGanttViewItem *parentItem, Task *task, KDGanttViewItem *after)
{
    //kDebug()<<k_funcinfo<<endl;
    GanttViewEventItem *item;
    if ( parentItem ) {
        item = new GanttViewEventItem(parentItem, task);
    } else {
        // we are on the top level
        item = new GanttViewEventItem(m_gantt, task);
    }
    if (after)
        item->moveItem(after);
    modifyMilestone(item, task);
    return item;
}

void GanttView::drawChildren(KDGanttViewItem *parentItem, Node &parentNode)
{
    //kDebug()<<k_funcinfo<<endl;
	Q3PtrListIterator<Node> nit(parentNode.childNodeIterator());
	for ( nit.toLast(); nit.current(); --nit )
	{
		Node *n = nit.current();
		if (n->type() == Node::Type_Project)
	        drawProject(parentItem, n);
		else if (n->type() == Node::Type_Subproject)
		    drawSubProject(parentItem, n);
		else if (n->type() == Node::Type_Summarytask) {
            Task *t = dynamic_cast<Task *>(n);
		    drawSummaryTask(parentItem, t);
		} else if (n->type() == Node::Type_Task) {
            Task *t = dynamic_cast<Task *>(n);
		    drawTask(parentItem, t);
        } else if (n->type() == Node::Type_Milestone) {
            Task *t = dynamic_cast<Task *>(n);
			drawMilestone(parentItem, t);
        }
		else
		    kDebug()<<k_funcinfo<<"Node type "<<n->type()<<" not implemented yet"<<endl;

	}
}


void GanttView::drawProject(KDGanttViewItem *parentItem, Node *node)
{
    //kDebug()<<k_funcinfo<<endl;
    GanttViewSummaryItem *item = dynamic_cast<GanttViewSummaryItem*>(addProject(parentItem, node));
    drawChildren(item, *node);
}

void GanttView::drawSubProject(KDGanttViewItem *parentItem, Node *node)
{
    //kDebug()<<k_funcinfo<<endl;
    GanttViewSummaryItem *item = dynamic_cast<GanttViewSummaryItem*>(addSubProject(parentItem, node));
    drawChildren(item, *node);
}

void GanttView::drawSummaryTask(KDGanttViewItem *parentItem, Task *task)
{
    //kDebug()<<k_funcinfo<<endl;
    GanttViewSummaryItem *item = dynamic_cast<GanttViewSummaryItem*>(addSummaryTask(parentItem, task));
    drawChildren(item, *task);
}

void GanttView::drawTask(KDGanttViewItem *parentItem, Task *task)
{
    //kDebug()<<k_funcinfo<<endl;
    addTask(parentItem, task);
}

void GanttView::drawMilestone(KDGanttViewItem *parentItem, Task *task)
{
    //kDebug()<<k_funcinfo<<endl;
    addMilestone(parentItem, task);
}

void GanttView::addTaskLink(KDGanttViewTaskLink *link) {
    //kDebug()<<k_funcinfo<<endl;
    m_taskLinks.append(link);
}

void GanttView::drawRelations()
{
    if (!m_showTaskLinks)
        return;
    KDGanttViewItem *item = m_gantt->firstChild();
    //kDebug()<<k_funcinfo<<"First: "<<(item ? item->listViewText() : "nil")<<endl;
    for (; item; item = item->nextSibling())
    {
        drawRelations(item);
        drawChildRelations(item->firstChild());
    }
}

void GanttView::drawChildRelations(KDGanttViewItem *item)
{
    //kDebug()<<k_funcinfo<<"item: "<<(item ? item->listViewText() : "nil")<<endl;
    for (; item; item = item->nextSibling())
    {
        drawRelations(item);
        drawChildRelations(item->firstChild());
    }
}

void GanttView::drawRelations(KDGanttViewItem *item)
{
    //kDebug()<<k_funcinfo<<endl;
    if (!item) return;

    GanttViewSummaryItem *summaryItem = dynamic_cast<GanttViewSummaryItem *>(item);
    if (summaryItem)
    {
        //kDebug()<<k_funcinfo<<"Summary item: "<<summaryItem->listViewText()<<endl;
        summaryItem->insertRelations(this);
        return;
    }
    GanttViewTaskItem *taskItem = dynamic_cast<GanttViewTaskItem *>(item);
    if (taskItem)
    {
        //kDebug()<<k_funcinfo<<"Task item: "<<taskItem->listViewText()<<endl;
        taskItem->insertRelations(this);
        return;
    }
    GanttViewEventItem *milestoneItem = dynamic_cast<GanttViewEventItem *>(item);
    if (milestoneItem)
    {
        //kDebug()<<k_funcinfo<<"Milestone item: "<<milestoneItem->listViewText()<<endl;
        milestoneItem->insertRelations(this);
        return;
    }
    kDebug()<<k_funcinfo<<"Unknown item type: "<<item->listViewText()<<endl;
}

void GanttView::currentItemChanged(KDGanttViewItem* item)
{
    //kDebug()<<k_funcinfo<<(item ? item->listViewText() : "null")<<endl;
    m_taskView->clear();
    m_gantt->setSelected(m_currentItem, false);
    m_currentItem = item;
    if (item) {
        m_gantt->setSelected(item, true);
        if (m_showAppointments) {
            m_taskView->show();
            GanttViewTaskItem *taskItem = dynamic_cast<GanttViewTaskItem *>(item);
            if (taskItem) {
                m_taskView->draw(taskItem->getTask());
            } else {
                GanttViewEventItem *msItem = dynamic_cast<GanttViewEventItem *>(item);
                if (msItem)
                    m_taskView->draw(msItem->getTask());
            }
        } else {
            m_taskView->hide();
        }
    }
    emit enableActions(true);
}

Node *GanttView::currentNode() const
{
    return getNode(m_currentItem);
}

void GanttView::popupMenuRequested(KDGanttViewItem * item, const QPoint & pos, int)
{
    //kDebug()<<k_funcinfo<<(item?item->listViewText(0):"0")<<endl;
    if (item == 0) {
        kDebug()<<"No item selected"<<endl;
        return;
    }
    Node *n = getNode(item);
    if (n == 0) {
        kDebug()<<"No node selected"<<endl;
        return;
    }
    Task *t =  dynamic_cast<Task*>(n);
    if (t && (t->type() == Node::Type_Task || t->type() == Node::Type_Milestone)) {
        emit requestPopupMenu("task_popup",pos);
//         QPopupMenu *menu = m_mainview->popupMenu("task_popup");
//         if (menu)
//         {
//             /*int id =*/ menu->exec(pos);
            //kDebug()<<k_funcinfo<<"id="<<id<<endl;
//         }
        return;
    }
    if (t && t->type() == Node::Type_Summarytask) {
        emit requestPopupMenu("summarytask_popup",pos);
//         QPopupMenu *menu = m_mainview->popupMenu("summarytask_popup");
//         if (menu)
//         {
//             /*int id =*/ menu->exec(pos);
            //kDebug()<<k_funcinfo<<"id="<<id<<endl;
//         }
        return;
    }
    //TODO: Other nodetypes
}

void GanttView::slotItemDoubleClicked(Q3ListViewItem* item) {
    //kDebug()<<k_funcinfo<<endl;
    if (item == 0 || item->childCount() > 0) {
        // FIXME: How else to avoid interference wirh expanding/collapsing summary items?
        return;
    }
    emit itemDoubleClicked();
}

//TODO: 1) make it koffice compliant,
//      2) allow printing on multiple pages
void GanttView::print(KPrinter &prt) {
    //kDebug()<<k_funcinfo<<endl;

    KDGanttViewItem *selItem = m_gantt->selectedItem();
    if (selItem)
        selItem->setSelected(false);

    //Comment from KWord
    //   We don't get valid metrics from the printer - and we want a better resolution
    //   anyway (it's the PS driver that takes care of the printer resolution).
    //But KSpread uses fixed 300 dpis, so we can use it.

    uint top, left, bottom, right;
    prt.margins(&top, &left, &bottom, &right);
    //kDebug()<<m.width()<<"x"<<m.height()<<" : "<<top<<", "<<left<<", "<<bottom<<", "<<right<<" : "<<size()<<endl;

    // get the size of the desired output for scaling.
    // here we want to print: ListView and TimeLine (default)
    // for this purpose, we call drawContents() with a 0 pointer as painter
    QSize size = m_gantt->drawContents(0);

    QPainter p;
    p.begin( &prt );
    p.setViewport(left, top, prt.width()-left-right, prt.height()-top-bottom);
    p.setClipRect(left, top, prt.width()-left-right, prt.height()-top-bottom);

    // Make a simple header
    p.drawRect(0,0,prt.width(),prt.height());
    QString text;
    int hei = 0;
    text = KGlobal::locale()->formatDateTime(QDateTime::currentDateTime());
    QRect r = p.boundingRect(prt.width()-1,0,0,0, Qt::AlignRight, text );
    p.drawText( r, Qt::AlignRight, text );
    hei = r.height();
    //kDebug()<<"Date r="<<r.left()<<","<<r.top()<<" "<<r.width()<<"x"<<r.height()<<endl;
    if (m_project)
    {
      QRect re = p.boundingRect(1,0,0,0, Qt::AlignLeft, text );
      re.setWidth(prt.width()-r.width()-5); // don't print on top of date
      p.drawText( re, Qt::AlignLeft, m_project->name() );
      hei = r.height();
      //kDebug()<<"Project r="<<re.left()<<","<<re.top()<<" "<<re.width()<<"x"<<re.height()<<": "<<endl;
      hei = qMax(hei, re.height());
    }

    hei++;
    p.drawLine(0,hei,prt.width(),hei);
    hei += 3;
    // compute the scale
    float dx = (float) (prt.width()-2)  / (float)size.width();
    float dy  = (float)(prt.height()-hei) / (float)size.height();
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

void GanttView::slotItemRenamed(KDGanttViewItem* item, int col, const QString& str) {
    //kDebug()<<k_funcinfo<<(item ? item->listViewText(col) : "null")<<": "<<str<<endl;
    if (col == 0) {
        emit itemRenamed(getNode(item), str);
    }
}

 void GanttView::slotGvItemClicked(KDGanttViewItem *) {
}

// testing
bool GanttView::exportGantt(QIODevice* device) {
    kDebug()<<k_funcinfo<<endl;
    return m_gantt->saveProject(device);
}

void GanttView::slotLinkItems(KDGanttViewItem* from, KDGanttViewItem* to, int linkType) {
    //kDebug()<<k_funcinfo<<(from?from->listViewText():"null")<<" to "<<(to?to->listViewText():"null")<<" linkType="<<linkType<<endl;
    Node *par = getNode(from);
    Node *child = getNode(to);
    if (!par || !child || !(par->legalToLink(child))) {
        KMessageBox::sorry(this, i18n("Cannot link these nodes"));
        return;
    }
    Relation *rel = child->findRelation(par);
    if (rel)
        emit modifyRelation(rel, linkTypeToRelation(linkType));
    else
        emit addRelation(par, child, linkTypeToRelation(linkType));

    return;
}

int GanttView::linkTypeToRelation(int linkType) {
    switch (linkType) {
        case KDGanttViewTaskLink::FinishStart:
            return Relation::FinishStart;
            break;
        case KDGanttViewTaskLink::StartStart:
            return Relation::StartStart;
            break;
        case KDGanttViewTaskLink::FinishFinish:
            return Relation::FinishFinish;
            break;
        case KDGanttViewTaskLink::StartFinish:
        default:
            return -1;
            break;
    }
}

void GanttView::slotModifyLink(KDGanttViewTaskLink* link) {
    //kDebug()<<k_funcinfo<<link<<endl;
    // we support only one from/to item in each link
    Node *par = getNode(link->from().first());
    Relation *rel = par->findRelation(getNode(link->to().first()));
    if (rel)
        emit modifyRelation(rel);
}

bool GanttView::setContext(Context::Ganttview &context, Project& /*project*/) {
    //kDebug()<<k_funcinfo<<endl;

    Q3ValueList<int> list = sizes();
    list[0] = context.ganttviewsize;
    list[1] = context.taskviewsize;
    setSizes(list);

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
//         KDGanttViewItem *item = findItem(project.findNode(*it));
//         if (item) {
//             item->setOpen(false);
//         }
//     }
    return true;
}

void GanttView::getContext(Context::Ganttview &context) const {
    //kDebug()<<k_funcinfo<<endl;
    context.ganttviewsize = sizes()[0];
    context.taskviewsize = sizes()[1];
    //kDebug()<<k_funcinfo<<"sizes="<<sizes()[0]<<","<<sizes()[1]<<endl;
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
    context.showNoInformation = m_showNoInformation;
    getContextClosedNodes(context, m_gantt->firstChild());
}

void GanttView::getContextClosedNodes(Context::Ganttview &context, KDGanttViewItem *item) const {
    if (item == 0)
        return;
    for (KDGanttViewItem *i = item; i; i = i->nextSibling()) {
        if (!i->isOpen()) {
            context.closedNodes.append(getNode(i)->id());
            //kDebug()<<k_funcinfo<<"add closed "<<i->listViewText()<<endl;
        }
        getContextClosedNodes(context, i->firstChild());
    }
}

void GanttView::setReadWriteMode(bool on) {
    m_readWrite = on;
    disconnect(m_gantt, SIGNAL(linkItems(KDGanttViewItem*, KDGanttViewItem*, int)), this, SLOT(slotLinkItems(KDGanttViewItem*, KDGanttViewItem*, int)));
    disconnect(m_gantt, SIGNAL(taskLinkDoubleClicked(KDGanttViewTaskLink*)), this, SLOT(slotModifyLink(KDGanttViewTaskLink*)));
#ifdef __GNUC__
#warning setLinkItemsEnabled missing
#endif
#if 0
    m_gantt->setLinkItemsEnabled(on);
#endif

    if (on) {
        connect(m_gantt, SIGNAL(linkItems(KDGanttViewItem*, KDGanttViewItem*, int)), SLOT(slotLinkItems(KDGanttViewItem*, KDGanttViewItem*, int)));

        connect(m_gantt, SIGNAL(taskLinkDoubleClicked(KDGanttViewTaskLink*)), SLOT(slotModifyLink(KDGanttViewTaskLink*)));
    }
    setRenameEnabled(m_gantt->firstChild(), on);
}

void GanttView::setRenameEnabled(Q3ListViewItem *item, bool on) {
    if (item == 0)
        return;
    for (Q3ListViewItem *i = item; i; i = i->nextSibling()) {
        i->setRenameEnabled(0, on);
        setRenameEnabled(i->firstChild(), on);
    }
}

}  //KPlato namespace

#include "kptganttview.moc"
