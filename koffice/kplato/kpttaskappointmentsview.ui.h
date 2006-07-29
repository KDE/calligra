/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

/* This file is part of the KDE project
   Copyright (C) 2004 - 2005 Dag Andersen <danders@get2net.dk>

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


#include <kdebug.h>

namespace KPlato
{

void TaskAppointmentsView::clear()
{
    if (m_appList) m_appList->clear();
    if (m_taskName) m_taskName->clear();
    if (m_plannedCost) m_plannedCost->clear();
    if (m_plannedCostTotal) m_plannedCostTotal->clear();
    if (m_actualCost) m_actualCost->clear();
    if (m_plannedEffort) m_plannedEffort->clear();
    if (m_plannedEffortTotal) m_plannedEffortTotal->clear();
    if (m_epi) m_epi->clear();
    if (m_cpi) m_cpi->clear();
}

void TaskAppointmentsView::draw(Task *task)
{
    //kdDebug()<<k_funcinfo<<endl;
    m_task = task;
    clear();
    if (!task)
        return;
    m_taskName->setText(task->name());

    QPtrListIterator<Appointment> it(task->appointments());
    for (; it.current(); ++it) {
        Resource *r = it.current()->resource();
        QListViewItem *item = new QListViewItem(m_appList, r->name());
 int i = 1;
        item->setText(i++, r->typeToString());
        item->setText(i++, it.current()->startTime().date().toString(ISODate));
        item->setText(i++, it.current()->endTime().date().toString(ISODate));
        item->setText(i++, it.current()->plannedEffort().toString(Duration::Format_HourFraction));
        item->setText(i++, KGlobal::locale()->formatMoney(r->normalRate()));
        item->setText(i++, KGlobal::locale()->formatMoney(r->overtimeRate()));
        item->setText(i++, KGlobal::locale()->formatMoney(r->fixedCost()));
        QPtrListIterator<AppointmentInterval> ait = it.current()->intervals();
        for (; ait.current(); ++ait) {
            QListViewItem *sub = new QListViewItem(item, "");
            i = 1;
            sub->setText(i++, "");
            sub->setText(i++, ait.current()->startTime().date().toString(ISODate));
            sub->setText(i++, ait.current()->endTime().date().toString(ISODate));
            sub->setText(i++, ait.current()->effort().toString(Duration::Format_HourFraction));
        }
        
    }
    drawCostEffort();
}


void TaskAppointmentsView::init()
{
    m_appList->setColumnAlignment(1, AlignHCenter);
    m_appList->setColumnAlignment(3, AlignRight);
    m_appList->setColumnAlignment(4, AlignRight);
    m_appList->setColumnAlignment(5, AlignRight);
    m_appList->setColumnAlignment(6, AlignRight);
    
    m_task = 0;
    m_date->setDate(QDate::currentDate());
     
}

void TaskAppointmentsView::drawCostEffort()
{
    if (m_task == 0)
 return;
    m_actualCost->setText(KGlobal::locale()->formatMoney(m_task->actualCostTo(m_date->date())));
    m_plannedCost->setText(KGlobal::locale()->formatMoney(m_task->plannedCostTo(m_date->date())));
    m_plannedCostTotal->setText(KGlobal::locale()->formatMoney(m_task->plannedCost()));
    
    m_actualEffort->setText(m_task->actualEffortTo(m_date->date()).toString(Duration::Format_HourFraction));
    m_plannedEffort->setText(m_task->plannedEffortTo(m_date->date()).toString(Duration::Format_HourFraction));
    m_plannedEffortTotal->setText(m_task->plannedEffort().toString(Duration::Format_HourFraction));
    
    m_epi->setText(QString("%1").arg(m_task->effortPerformanceIndex(m_date->date()),3,'f',2));
    m_cpi->setText(QString("%1").arg(m_task->costPerformanceIndex(m_date->date()),3,'f',2));

}

}  // KPlato namespace
