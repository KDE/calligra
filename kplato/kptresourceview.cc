/* This file is part of the KDE project
   Copyright (C) 2003 Dag Andersen kplato@kde.org>

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

#include "kptresourceview.h"

#include "kptpart.h"
#include "kptview.h"
#include "kptproject.h"
#include "kpttask.h"

#include <KDGanttViewSummaryItem.h>
#include <KDGanttViewTaskItem.h>
#include <qlistview.h>

#include <kdebug.h>


KPTResourceView::KPTResourceView( KPTView *view, QWidget *parent )
    : KDGanttView( parent, "Resource gantt view" ),
    m_mainview( view )
{
    setScale(KDGanttView::Day);
    addColumn("Start");
    addColumn("End");
	draw(view->getPart()->getProject());

}

void KPTResourceView::zoom(double zoom)
{
}

void KPTResourceView::draw(KPTProject &project)
{
    kdDebug()<<k_funcinfo<<endl;
	setUpdateEnabled(false);
	clear();

    QPtrListIterator<KPTResourceGroup> it(project.resourceGroups());
    for (; it.current(); ++it) {
        KDGanttViewSummaryItem *item = new KDGanttViewSummaryItem(this, it.current()->name());
        item->setStartTime(project.startTime().dateTime());
        item->setEndTime(project.endTime().dateTime());
        item->setListViewText(1, item->startTime().toString());
        item->setListViewText(2, item->endTime().toString());
	    item->setOpen(true);
        drawResources(item, it.current());
    }
	setUpdateEnabled(true);
}


void KPTResourceView::drawResources(KDGanttViewItem *parent, KPTResourceGroup *group)
{
    kdDebug()<<k_funcinfo<<"group: "<<group->name()<<endl;
    QPtrListIterator<KPTResource> it(group->resources());
    for (; it.current(); ++it) {
        KPTResource *r = it.current();
        KDGanttViewSummaryItem *item = new KDGanttViewSummaryItem(parent, r->name());
        item->setStartTime(r->availableFrom().dateTime());
        item->setEndTime(r->availableUntil().dateTime());
        item->setListViewText(1, item->startTime().toString());
        item->setListViewText(2, item->endTime().toString());
	    item->setOpen(true);
        drawAppointments(item, r);
    }
}

void KPTResourceView::drawAppointments(KDGanttViewItem *parent, KPTResource *resource)
{
    kdDebug()<<k_funcinfo<<"Appointmnets for resource: "<<resource->name()<<endl;
    QPtrListIterator<KPTAppointment> it(resource->appointments());
    for (; it.current(); ++it) {
        KPTAppointment *a = it.current();
        KDGanttViewTaskItem *item = new KDGanttViewTaskItem(parent, a->task()->name());
        KPTDuration time(a->startTime().dateTime());
    	item->setStartTime(time.dateTime());
	    time.add(a->duration());
	    item->setEndTime(time.dateTime());
        item->setListViewText(1, item->startTime().toString());
        item->setListViewText(2, item->endTime().toString());
    }
}


#include "kptresourceview.moc"
