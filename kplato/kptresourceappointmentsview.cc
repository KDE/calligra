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

#include "kptresourceappointmentsview.h"

#include "kptappointment.h"
#include "kptcalendar.h"
#include "kptnode.h"
#include "kptresource.h"
#include "kptview.h"

#include <qapplication.h>
#include <kcalendarsystem.h>
#include <kglobal.h>
#include <klocale.h>

#include <qdatetime.h>
#include <qheader.h>

namespace KPlato
{

ResourceAppointmentsView::NodeItem::NodeItem(Node *t, QListView *parent, bool highlight)
    : DoubleListViewBase::MasterListItem(parent, t->name(), highlight),
      node(t) {
      
      setFormat(0, 'f', 1);
    //kdDebug()<<k_funcinfo<<endl;
}
ResourceAppointmentsView::NodeItem::NodeItem(Node *t, QListViewItem *p, bool highlight)
    : DoubleListViewBase::MasterListItem(p, t->name(), highlight),
      node(t) {
      
      setFormat(0, 'f', 1);
    //kdDebug()<<k_funcinfo<<endl;
}

ResourceAppointmentsView::NodeItem::NodeItem(QString text, QListViewItem *parent, bool highlight)
    : DoubleListViewBase::MasterListItem(parent, text, highlight),
      node(0) {
      
      setFormat(0, 'f', 1);
    //kdDebug()<<k_funcinfo<<endl;
}

ResourceAppointmentsView::NodeItem::NodeItem(QString text, QListView *parent, bool highlight)
    : DoubleListViewBase::MasterListItem(parent, text, highlight),
      node(0) {
      
      setFormat(0, 'f', 1);
    //kdDebug()<<k_funcinfo<<endl;
}

//-------------------------------------------
ResourceAppointmentsView::ResourceAppointmentsView(View *view, QWidget *parent)
    : DoubleListViewBase(parent),
      m_mainview(view),
      m_resource(0),
      m_availItem(0),
      m_totalItem(0) {
    
    setNameHeader(i18n("Task"));
    
    
    QValueList<int> list = sizes();
    int tot = list[0] + list[1];
    list[0] = QMIN(35, tot);
    list[1] = tot-list[0];
    setSizes(list);
}

void ResourceAppointmentsView::zoom(double zoom) {
    Q_UNUSED(zoom);
}


void ResourceAppointmentsView::draw(Resource *resource, const QDate &start, const QDate &end) {
    m_resource = resource;
    m_start = start;
    m_end = end;
    draw();
}

void ResourceAppointmentsView::draw() {
    //kdDebug()<<k_funcinfo<<endl;
    clear();
    if (!m_resource)
        return;
    
    m_totalItem = new ResourceAppointmentsView::NodeItem(i18n("Total"), masterListView());
    m_totalItem->setExpandable(true);
    m_totalItem->setOpen(true);
    m_availItem = new ResourceAppointmentsView::NodeItem(i18n("Available"), masterListView());
    QPtrList<Appointment> lst = m_resource->appointments();
    //kdDebug()<<k_funcinfo<<lst.count()<<endl;
    QPtrListIterator<Appointment> it(lst);
    for (; it.current(); ++it) {
        //kdDebug()<<k_funcinfo<<endl;
        Node *n = it.current()->node()->node();
        ResourceAppointmentsView::NodeItem *item = new ResourceAppointmentsView::NodeItem(n, m_totalItem);
        
        item->effortMap = it.current()->plannedPrDay(m_start, m_end);
    }
    slotUpdate();
}

void ResourceAppointmentsView::slotUpdate() {
    //kdDebug()<<k_funcinfo<<endl;
    if (!m_resource)
        return;
    QApplication::setOverrideCursor(Qt::waitCursor);
    createSlaveItems();
    KLocale *locale = KGlobal::locale();
    const KCalendarSystem *cal = locale->calendar();
    const Calendar *resCal = m_resource->calendar();
    const QDateTime availFrom = m_resource->availableFrom();
    const QDateTime availUntil = m_resource->availableUntil();
    // Add columns for selected period/periods
    //kdDebug()<<k_funcinfo<<start.toString()<<" - "<<end.toString()<<endl;
    int c=0;
    for (QDate dt = m_start; dt <= m_end; dt = cal->addDays(dt, 1), ++c) {
        QString df = locale->formatDate(dt, true);
        addSlaveColumn(df);
    }
    if (m_totalItem) {
        m_totalItem->setHighlight(true);
        m_totalItem->setSlaveHighlight(true);
    }
    QListViewItemIterator it(masterListView());
    for (;it.current(); ++it) {
        ResourceAppointmentsView::NodeItem *item = static_cast<ResourceAppointmentsView::NodeItem*>(it.current());
        if (!item || item->firstChild()) {
            continue;
        }
        double eff;
        double avail;
        int col=0;
        for (QDate d=m_start; d <= m_end; d = cal->addDays(d, 1), ++col) {
            if (item == m_availItem && resCal) {
                QDateTime f(d);
                QDateTime u(d, QTime(23, 59, 59, 999));
                if (f >= availUntil || u <= availFrom) {
                    avail = 0.0;
                } else {
                    if (availFrom > f) {
                        f = availFrom;
                    }
                    if (availUntil < u) {
                        u = availUntil;
                    }
                    avail = ((double)(resCal->effort(f.date(), f.time(), u.time())*(double)(m_resource->units())/100.0).minutes()/60.0);
                }
                m_availItem->setSlaveItem(col, avail);
                m_availItem->addToTotal(avail);
                if (m_totalItem) {
                    m_totalItem->setSlaveLimit(col, avail);
                }
            }
            if (item != m_availItem) {
                eff = (double)(item->effortMap.effortOnDate(d).minutes())/60.0;
                item->setSlaveItem(col, eff);
                item->addToTotal(eff);
            }
        }
    }
    if (m_totalItem && m_availItem) {
        m_totalItem->setLimit(m_availItem->value());
        //kdDebug()<<k_funcinfo<<"avail="<<m_availItem->value()<<endl;
    }
    calculate();
    QApplication::restoreOverrideCursor();
}


void ResourceAppointmentsView::print(KPrinter &/*printer*/) {
    kdDebug()<<k_funcinfo<<endl;
}

// bool ResourceAppointmentsView::setContext(Context::ResourceAppointmentsView &context) {
//     //kdDebug()<<k_funcinfo<<endl;
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
// void ResourceAppointmentsView::getContext(Context::ResourceAppointmentsView &context) const {
//     //kdDebug()<<k_funcinfo<<endl;
//     context.accountsviewsize = m_dlv->sizes()[0];
//     context.periodviewsize = m_dlv->sizes()[1];
//     context.date = m_date;
//     context.period = m_period;
//     context.cumulative = m_cumulative;
//     //kdDebug()<<k_funcinfo<<"sizes="<<sizes()[0]<<","<<sizes()[1]<<endl;
// }

void ResourceAppointmentsView::clear() {
    clearLists();
    m_availItem = 0;
    m_totalItem = 0;
}

void ResourceAppointmentsView::createSlaveItems() {
    DoubleListViewBase::createSlaveItems();
    setSlaveFormat(0, 'f', 1);
}

}  //KPlato namespace

#include "kptresourceappointmentsview.moc"
