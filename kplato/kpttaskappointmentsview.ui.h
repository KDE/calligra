/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include <kdebug.h>

namespace KPlato
{

void KPTTaskAppointmentsView::clear()
{
    if (m_appList) m_appList->clear();
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

    QPtrListIterator<KPTAppointment> it(task->appointments());
    for (; it.current(); ++it) {
        KPTResource *r = it.current()->resource();
        QListViewItem *item = new QListViewItem(m_appList, r->name());
 int i = 1;
        item->setText(i++, r->typeToString());
        item->setText(i++, it.current()->startTime().date().toString(ISODate));
        item->setText(i++, it.current()->endTime().date().toString(ISODate));
        item->setText(i++, it.current()->effort().toString(KPTDuration::Format_Hour));
        item->setText(i++, KGlobal::locale()->formatMoney(r->normalRate()));
        item->setText(i++, KGlobal::locale()->formatMoney(r->overtimeRate()));
        item->setText(i++, KGlobal::locale()->formatMoney(r->fixedCost()));
        QPtrListIterator<KPTAppointmentInterval> ait = it.current()->intervals();
        for (; ait.current(); ++ait) {
            QListViewItem *sub = new QListViewItem(item, "");
     i = 1;
            sub->setText(i++, "");
            sub->setText(i++, ait.current()->startTime().date().toString(ISODate));
            sub->setText(i++, ait.current()->endTime().date().toString(ISODate));
            sub->setText(i++, ait.current()->effort().toString(KPTDuration::Format_Hour));
        }
        
    }
}


void KPTTaskAppointmentsView::init()
{
    m_appList->setColumnAlignment(1, AlignHCenter);
    m_appList->setColumnAlignment(3, AlignRight);
    m_appList->setColumnAlignment(4, AlignRight);
    m_appList->setColumnAlignment(5, AlignRight);
    m_appList->setColumnAlignment(6, AlignRight);
}

}  //KPlato namespace
