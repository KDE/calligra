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

#include "kptresourceuseview.h"

#include "kptresource.h"
#include "chart/kptchartview.h"
#include "chart/kptchartdataset.h"
#include "kptproject.h"
#include "kptresource.h"

#include <qpainter.h>

#include <klistview.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>

namespace KPlato
{

class ResourceGroupItemPrivate : public QListViewItem {
public:
    ResourceGroupItemPrivate(KPTResourceGroup *g, QListView *parent)
        : QListViewItem(parent, g->name()),
        group(g) {}

    KPTResourceGroup *group;
};

class ResourceItemPrivate : public QListViewItem {
public:
    ResourceItemPrivate(KPTResource *r, QListViewItem *parent)
        : QListViewItem(parent, r->name()),
        resource(r) {}

    KPTResource *resource;
};


KPTResourceUseView::KPTResourceUseView(KPTView *view, QWidget* parent, const char* name )
    : QSplitter(parent, name),
      m_mainview(view),
      m_selectedItem(0)
{
    setOrientation(QSplitter::Horizontal);

    m_resourcelist = new KListView(this, "Resource list");
    m_resourcelist->setRootIsDecorated(true);
    m_resourcelist->addColumn(i18n("Name"));

    m_chartview = new KPTChartView(this, "Chart view");
    
    connect(m_resourcelist, SIGNAL(selectionChanged(QListViewItem*)), SLOT(executed(QListViewItem*)));
    connect(m_resourcelist, SIGNAL(currentChanged(QListViewItem*)), SLOT(executed(QListViewItem*)));
}

KPTResourceUseView::~KPTResourceUseView()
{
}

void KPTResourceUseView::draw(KPTProject &project)
{
    //kdDebug()<<k_funcinfo<<endl;
    m_selectedItem=0;
    m_resourcelist->clear();
    QPtrListIterator<KPTResourceGroup> it(project.resourceGroups());
    for (; it.current(); ++it) {
        ResourceGroupItemPrivate *item = new ResourceGroupItemPrivate(it.current(), m_resourcelist);
        item->setOpen(true);
        drawResources(item);
    }
    if (m_selectedItem) {
        m_resourcelist->setSelected(m_selectedItem, true);
    }
    if (m_selectedItem) {
    
    }
    drawChart();
}

void KPTResourceUseView::drawResources(ResourceGroupItemPrivate *parent)
{
    //kdDebug()<<k_funcinfo<<"group: "<<parent->group->name()<<endl;
    QPtrListIterator<KPTResource> it(parent->group->resources());
    for (; it.current(); ++it) {
        KPTResource *r = it.current();
        ResourceItemPrivate *item = new ResourceItemPrivate(r, parent);
        if (!m_selectedItem) {
            m_selectedItem = item;
        }
    }
}

void KPTResourceUseView::drawChart() {
    if (!m_selectedItem) {
        kdDebug()<<k_funcinfo<<"No selected item"<<endl;
        // Empty chart
        m_chartview->clear();
        return;
    }
    ResourceGroupItemPrivate *gi = dynamic_cast<ResourceGroupItemPrivate*>(m_selectedItem);
    if (gi) {
        //kdDebug()<<k_funcinfo<<gi->group->name()<<endl;
        // Empty chart
        m_chartview->clear();
        return;
    }
    ResourceItemPrivate *ri = dynamic_cast<ResourceItemPrivate*>(m_selectedItem);
    if (ri) {
        m_chartview->clearData();
        drawLoadPrDay(ri->resource);
    }
}

void KPTResourceUseView::drawPeek(KPTResource *resource) {
    //kdDebug()<<k_funcinfo<<resource->name()<<endl;
    KPTAppointment a = resource->appointmentIntervals();
    
    KPTChartDataSet *set = new KPTChartDataSet();
    QPtrListIterator<KPTAppointmentInterval> it = a.intervals();
    for (; it.current(); ++it) {
        set->append(new KPTChartDataSetItem(it.current()->startTime(), it.current()->endTime(), 0.0, it.current()->load()));
    }
    m_chartview->setTimeScaleRange(a.startTime(), a.endTime());
    m_chartview->setYScaleRange(0, a.maxLoad(), 10.0);
    m_chartview->addData(set);
    m_chartview->draw();
}

void KPTResourceUseView::drawLoadPrDay(KPTResource *resource) {
    //kdDebug()<<k_funcinfo<<resource->name()<<endl;
    m_chartview->setDescription(resource->name()+": Load pr day (%)");
    KPTAppointment a = resource->appointmentIntervals();
    KPTDateTime start = KPTDateTime(a.startTime().date(), QTime());
    KPTDateTime end = KPTDateTime(a.endTime().date().addDays(1),QTime());

    m_chartview->enableYZeroLine(true);    
    
    KPTChartDataSetItem *item = new KPTChartDataSetItem(100.0, new QPen(QColor(red)));
    item->setPrio(100.0);
    m_chartview->addYMarkerLine(item);
    
    KPTChartDataSet *set = new KPTChartDataSet();
    double load=0.0;
    double maxLoad=100.0;
    for (KPTDateTime dt = start; dt < end; dt = dt.addDays(1)) {
        KPTDuration e = resource->effort(dt, KPTDuration(1,0,0));
        KPTDuration l = a.effort(dt.date());
        if (e == KPTDuration::zeroDuration) {
            load = 0.0;
        } else {
            load = l*100/e;
        }
        set->append(new KPTChartDataSetItem(dt.addSecs(7200), dt.addDays(1).addSecs(-7200), 0.0, load));
        if (load > maxLoad)
            maxLoad = load;

    }
    m_chartview->setTimeScaleRange(start, end);
    m_chartview->setYScaleRange(0, maxLoad, 10.0);
    m_chartview->addData(set);
    m_chartview->draw();
}

void KPTResourceUseView::executed(QListViewItem *item) {
    if (m_selectedItem == item)
        return;
    m_selectedItem = item;
    if (item == 0) {
        // clear data
        drawChart();
        return;
    }
    ResourceGroupItemPrivate *i = dynamic_cast<ResourceGroupItemPrivate*>(item);
    if (i) {
        //kdDebug()<<k_funcinfo<<i->group->name()<<endl;
        drawChart();
    }
    ResourceItemPrivate *ri = dynamic_cast<ResourceItemPrivate*>(item);
    if (ri) {
        //kdDebug()<<k_funcinfo<<ri->resource->name()<<endl;
        drawChart();
    }    
}

} //KPlato namespace

#include "kptresourceuseview.moc"
