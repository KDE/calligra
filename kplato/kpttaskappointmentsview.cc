/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

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

#include "kpttaskappointmentsview.h"

#include "kptappointment.h"
#include "kpttask.h"

#include <qapplication.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3PtrList>
#include <kcalendarsystem.h>
#include <kglobal.h>
#include <klocale.h>

#include <q3header.h>

namespace KPlato
{

TaskAppointmentsView::ResourceItem::ResourceItem(Resource *r, Q3ListView *parent, bool highlight)
    : DoubleListViewBase::MasterListItem(parent, r->name(), highlight),
      resource(r) {

      setFormat(0, 'f', 1);
    //kDebug()<<k_funcinfo<<endl;
}
TaskAppointmentsView::ResourceItem::ResourceItem(Resource *r, Q3ListViewItem *p, bool highlight)
    : DoubleListViewBase::MasterListItem(p, r->name(), highlight),
      resource(r) {

      setFormat(0, 'f', 1);
    //kDebug()<<k_funcinfo<<endl;
}

TaskAppointmentsView::ResourceItem::ResourceItem(QString text, Q3ListViewItem *parent, bool highlight)
    : DoubleListViewBase::MasterListItem(parent, text, highlight),
      resource(0) {

      setFormat(0, 'f', 1);
    //kDebug()<<k_funcinfo<<endl;
}

//-------------------------------------------
TaskAppointmentsView::TaskAppointmentsView(QWidget *parent)
    : DoubleListViewBase(parent),
      m_task(0) {

    setNameHeader(i18n("Resource"));


    Q3ValueList<int> list = sizes();
    int tot = list[0] + list[1];
    list[0] = qMin(35, tot);
    list[1] = tot-list[0];
    setSizes(list);
}

void TaskAppointmentsView::zoom(double zoom) {
    Q_UNUSED(zoom);
}


void TaskAppointmentsView::draw(Task *task) {
    m_task = task;
    draw();
}

void TaskAppointmentsView::draw() {
    //kDebug()<<k_funcinfo<<endl;
    clearLists();
    if (!m_task)
        return;

    Q3PtrList<Appointment> lst = m_task->appointments();
    Q3PtrListIterator<Appointment> it(lst);
    for (; it.current(); ++it) {
        Resource *r = it.current()->resource()->resource();
        TaskAppointmentsView::ResourceItem *item = new TaskAppointmentsView::ResourceItem(r, masterListView());

        item->effortMap = it.current()->plannedPrDay(m_task->startTime().date(), m_task->endTime().date());
    }
    slotUpdate();
}

void TaskAppointmentsView::slotUpdate() {
    //kDebug()<<k_funcinfo<<endl;
    if (!m_task)
        return;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    createSlaveItems();
    KLocale *locale = KGlobal::locale();
    const KCalendarSystem *cal = locale->calendar();

    // Add columns for selected period/periods
    QDate start = m_task->startTime().date();
    QDate end = m_task->endTime().date();
    //kDebug()<<k_funcinfo<<start.toString()<<" - "<<end.toString()<<endl;
    int c=0;
    for (QDate dt = start; dt <= end; dt = cal->addDays(dt, 1), ++c) {
        QString df = locale->formatDate(dt, true);
        addSlaveColumn(df);
    }
    Q3ListViewItemIterator it(masterListView());
    for (;it.current(); ++it) {
        TaskAppointmentsView::ResourceItem *item = static_cast<TaskAppointmentsView::ResourceItem*>(it.current());
        if (!item) {
            continue;
        }
        double eff;
        int col=0;
        for (QDate d=start; d <= end; d = cal->addDays(d, 1), ++col) {
            eff = (double)(item->effortMap.effortOnDate(d).minutes())/60.0;
            item->setSlaveItem(col, eff);
            item->addToTotal(eff);
        }
    }
    calculate();
    QApplication::restoreOverrideCursor();
}


void TaskAppointmentsView::print(KPrinter &/*printer*/) {
    kDebug()<<k_funcinfo<<endl;
}

// bool TaskAppointmentsView::setContext(Context::TaskAppointmentsView &context) {
//     //kDebug()<<k_funcinfo<<endl;
//
//     QValueList<int> list;
//     list << context.accountsviewsize << context.periodviewsize;
//     m_dlv->setSizes(list);
//     m_date = context.date;
//     if (!m_date.isValid())
//         m_date = QDate::currentDate();
//     m_period = context.period;
//     m_cumulative = context.cumulative;
//
//     return true;
// }
//
// void TaskAppointmentsView::getContext(Context::TaskAppointmentsView &context) const {
//     //kDebug()<<k_funcinfo<<endl;
//     context.accountsviewsize = m_dlv->sizes()[0];
//     context.periodviewsize = m_dlv->sizes()[1];
//     context.date = m_date;
//     context.period = m_period;
//     context.cumulative = m_cumulative;
//     //kDebug()<<k_funcinfo<<"sizes="<<sizes()[0]<<","<<sizes()[1]<<endl;
// }

void TaskAppointmentsView::clear() {
    clearLists();
}

void TaskAppointmentsView::createSlaveItems() {
    DoubleListViewBase::createSlaveItems();
    setSlaveFormat(0, 'f', 1);
}

}  //KPlato namespace

#include "kpttaskappointmentsview.moc"
