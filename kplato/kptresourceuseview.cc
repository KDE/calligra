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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kptresourceuseview.h"

#include "kptresource.h"
#include "chart/kptchartview.h"
#include "chart/kptchartdataset.h"
#include "kptproject.h"
#include "kptresource.h"
#include "kptview.h"
#include "kptcontext.h"

#include <kactioncollection.h>

#include <qpainter.h>
#include <qpoint.h>
#include <qpopupmenu.h>

#include <klistview.h>
#include <kglobal.h>
#include <klocale.h>
#include <kaction.h>
#include <kdebug.h>

namespace KPlato
{

class ResourceGroupItemPrivate : public QListViewItem {
public:
    ResourceGroupItemPrivate(KPTResourceGroup *g, QListViewItem *parent)
        : QListViewItem(parent, g->name()),
        group(g) {}

    KPTResourceGroup *group;
};

class ResourceItemUseViewPrivate : public QListViewItem {
public:
    ResourceItemUseViewPrivate(KPTResource *r, QListViewItem *parent)
        : QListViewItem(parent, r->name()),
        resource(r) {}

    KPTResource *resource;
};


KPTResourceUseView::KPTResourceUseView(KPTView *view, QWidget* parent, const char* name )
    : QSplitter(parent, name),
      m_mainview(view),
      m_selectedItem(0),
      m_dataShown(LoadPrDay)
{
    setOrientation(QSplitter::Horizontal);

    m_resourcelist = new KListView(this, "Resource list");
    m_resourcelist->setRootIsDecorated(true);
    m_resourcelist->addColumn(i18n("Name"));

    m_chartview = new KPTChartView(this, "Chart view");
    
    connect(m_resourcelist, SIGNAL(selectionChanged(QListViewItem*)), SLOT(executed(QListViewItem*)));
    connect(m_resourcelist, SIGNAL(currentChanged(QListViewItem*)), SLOT(executed(QListViewItem*)));
    
    connect(m_chartview, SIGNAL(chartMenuRequest(const QPoint&)), SLOT(slotChartMenuRequested(const QPoint&)));

}

KPTResourceUseView::~KPTResourceUseView()
{
}

void KPTResourceUseView::draw(KPTProject &project)
{
    //kdDebug()<<k_funcinfo<<endl;
    m_project = &project;
    m_selectedItem=0;
    m_resourcelist->clear();
    QListViewItem *top = new QListViewItem(m_resourcelist, i18n("All resources"));
    top->setOpen(true);
    QPtrListIterator<KPTResourceGroup> it(project.resourceGroups());
    for (; it.current(); ++it) {
        ResourceGroupItemPrivate *item = new ResourceGroupItemPrivate(it.current(), top);
        item->setOpen(true);
        drawResources(item);
    }
    if (m_selectedItem) {
        m_resourcelist->setSelected(m_selectedItem, true);
    }
    drawChart();
}

void KPTResourceUseView::drawResources(ResourceGroupItemPrivate *parent)
{
    //kdDebug()<<k_funcinfo<<"group: "<<parent->group->name()<<endl;
    QPtrListIterator<KPTResource> it(parent->group->resources());
    for (; it.current(); ++it) {
        KPTResource *r = it.current();
        ResourceItemUseViewPrivate *item = new ResourceItemUseViewPrivate(r, parent);
        if (!m_selectedItem) {
            m_selectedItem = item;
        }
    }
}

void KPTResourceUseView::drawChart() {
    QTime t; t.start();
    m_chartview->clear();
    if (!m_selectedItem) {
        kdDebug()<<k_funcinfo<<"No selected item"<<endl;
        // Empty chart
        return;
    }
    if (m_selectedItem->parent() == 0) {
        //kdDebug()<<k_funcinfo<<"All selected"<<endl;
        drawLineAccumulated();
        return;
    }
    ResourceGroupItemPrivate *gi = dynamic_cast<ResourceGroupItemPrivate*>(m_selectedItem);
    if (gi) {
        //kdDebug()<<k_funcinfo<<gi->group->name()<<endl;
        drawLineAccumulated(gi->group);
        return;
    }
    ResourceItemUseViewPrivate *ri = dynamic_cast<ResourceItemUseViewPrivate*>(m_selectedItem);
    if (ri) {
        drawBarLoad(ri->resource);
        //drawBarPeekLoad(ri->resource);
        //drawLineAccumulated(ri->resource);
    }
}

void KPTResourceUseView::drawBarPeekLoad(KPTResource *resource) {
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

KPTChartDataSet *KPTResourceUseView::drawBarLoadPrMinute(KPTResource *resource) {
    m_chartview->setDescription(resource->name()+": Load pr minute");
    m_chartview->setYScaleUnit("%");
    m_chartview->enableYZeroLine(true);    
    KPTChartDataSetItem *item = new KPTChartDataSetItem(100.0, new QPen(QColor(red)));
    item->setPrio(100.0);
    m_chartview->addHorisontalLine(item);
        
    KPTAppointment a = resource->appointmentIntervals();
    KPTDateTime start = KPTDateTime(a.startTime().date(), QTime(a.startTime().time().hour(), a.startTime().time().addSecs(-60).minute(), 0));
    KPTDateTime end = KPTDateTime(a.endTime().date(), QTime(a.endTime().time().hour(), a.startTime().time().addSecs(60).minute(), 0));
    
    KPTChartDataSet *set = new KPTChartDataSet();
    double load=0.0;
    double maxLoad=100.0;
    KPTDuration aMinute = KPTDuration(0,0,1);
    KPTDuration gap = KPTDuration(0,0,0,6);
    for (KPTDateTime dt = start; dt < end; dt += aMinute) {
        KPTDuration e = resource->effort(dt, aMinute);
        KPTDuration l = a.effort(dt, aMinute);
        if (e == KPTDuration::zeroDuration) {
            load = 0.0;
        } else {
            load = l*100/e;
        }
        set->append(new KPTChartDataSetItem(dt+gap, dt+aMinute-gap, 0.0, load));
        if (load > maxLoad)
            maxLoad = load;
    }
    m_chartview->setTimeScaleRange(start, end);
    m_chartview->setYScaleRange(0, maxLoad, 10.0);
    return set;
}

KPTChartDataSet *KPTResourceUseView::drawBarLoadPrHour(KPTResource *resource) {
    m_chartview->setDescription(resource->name()+": Load pr hour");
    m_chartview->setYScaleUnit("%");
    m_chartview->enableYZeroLine(true);    
    KPTChartDataSetItem *item = new KPTChartDataSetItem(100.0, new QPen(QColor(red)));
    item->setPrio(100.0);
    m_chartview->addHorisontalLine(item);
        
    KPTAppointment a = resource->appointmentIntervals();
    KPTDateTime start = KPTDateTime(a.startTime().date(), QTime(a.startTime().time().addSecs(-3600).hour(), 0, 0));
    KPTDateTime end = KPTDateTime(a.endTime().date(), QTime(a.endTime().time().addSecs(3600).hour(), 0, 0));
    
    KPTChartDataSet *set = new KPTChartDataSet();
    double load=0.0;
    double maxLoad=100.0;
    KPTDuration aHour = KPTDuration(0,1,0);
    KPTDuration gap = KPTDuration(0,0,6);
    for (KPTDateTime dt = start; dt < end; dt += aHour) {
        KPTDuration e = resource->effort(dt, aHour);
        KPTDuration l = a.effort(dt, aHour);
        if (e == KPTDuration::zeroDuration) {
            load = 0.0;
        } else {
            load = l*100/e;
        }
        set->append(new KPTChartDataSetItem(dt+gap, dt+aHour-gap, 0.0, load));
        if (load > maxLoad)
            maxLoad = load;
    }
    m_chartview->setTimeScaleRange(start, end);
    m_chartview->setYScaleRange(0, maxLoad, 10.0);
    return set;
}

KPTChartDataSet *KPTResourceUseView::drawBarLoadPrDay(KPTResource *resource) {
    m_chartview->setDescription(resource->name()+": Load pr day");
    m_chartview->setYScaleUnit("%");
    m_chartview->enableYZeroLine(true);    
    KPTChartDataSetItem *item = new KPTChartDataSetItem(100.0, new QPen(QColor(red)));
    item->setPrio(100.0);
    m_chartview->addHorisontalLine(item);
    
    KPTAppointment a = resource->appointmentIntervals();
    KPTDateTime start = KPTDateTime(a.startTime().date(), QTime());
    KPTDateTime end = KPTDateTime(a.endTime().date().addDays(1),QTime());
    KPTChartDataSet *set = new KPTChartDataSet();
    double load=0.0;
    double maxLoad=100.0;
    for (KPTDateTime dt = start; dt < end; dt = dt.addDays(1)) {
        KPTDuration e = resource->effort(dt, KPTDuration(1,0,0));
        KPTDuration l = a.plannedEffort(dt.date());
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
    return set;
}

KPTChartDataSet *KPTResourceUseView::drawBarLoadPrWeek(KPTResource *resource) {
    m_chartview->setDescription(resource->name()+": Load pr week");
    m_chartview->setYScaleUnit("%");
    m_chartview->enableYZeroLine(true);    
    KPTChartDataSetItem *item = new KPTChartDataSetItem(100.0, new QPen(QColor(red)));
    item->setPrio(100.0);
    m_chartview->addHorisontalLine(item);
        
    KPTAppointment a = resource->appointmentIntervals();
    KPTDateTime start = KPTDateTime(a.startTime().date(), QTime());
    while (start.date().dayOfWeek() != KGlobal::locale()->weekStartDay()) {
        start = start.addDays(-1);
    }
    KPTDateTime end = KPTDateTime(a.endTime().date(),QTime());
    while (end.date().dayOfWeek() != KGlobal::locale()->weekStartDay()) {
        end = end.addDays(1);
    }
    end = end.addDays(-1);
    
    KPTChartDataSet *set = new KPTChartDataSet();
    double load=0.0;
    double maxLoad=100.0;
    KPTDuration aWeek = KPTDuration(7,0,0);
    for (KPTDateTime dt = start; dt < end; dt += aWeek) {
        KPTDuration e = resource->effort(dt, aWeek);
        KPTDuration l = a.effort(dt, aWeek);
        if (e == KPTDuration::zeroDuration) {
            load = 0.0;
        } else {
            load = l*100/e;
        }
        set->append(new KPTChartDataSetItem(dt.addDays(1), dt.addDays(6), 0.0, load));
        if (load > maxLoad)
            maxLoad = load;
    }
    m_chartview->setTimeScaleRange(start, end);
    m_chartview->setYScaleRange(0, maxLoad, 10.0);
    return set;
}

KPTChartDataSet *KPTResourceUseView::drawBarLoadPrMonth(KPTResource *resource) {
    m_chartview->setDescription(resource->name()+": Load pr month");
    m_chartview->setYScaleUnit("%");
    m_chartview->enableYZeroLine(true);    
    KPTChartDataSetItem *item = new KPTChartDataSetItem(100.0, new QPen(QColor(red)));
    item->setPrio(100.0);
    m_chartview->addHorisontalLine(item);
        
    KPTAppointment a = resource->appointmentIntervals();
    KPTDateTime start = KPTDateTime(QDate(a.startTime().date().year(), a.startTime().date().month(), 1).addMonths(-1), QTime());
    KPTDateTime end = KPTDateTime(QDate(a.endTime().date().year(), a.endTime().date().month(), a.endTime().date().daysInMonth()).addMonths(1),QTime());
    
    KPTChartDataSet *set = new KPTChartDataSet();
    double load=0.0;
    double maxLoad=100.0;
    for (KPTDateTime dt = start; dt < end; dt = dt.addMonths(1)) {
        KPTDuration aMonth = KPTDuration(dt.date().daysInMonth(), 0, 0);
        KPTDuration e = resource->effort(dt, aMonth);
        KPTDuration l = a.effort(dt, aMonth);
        //kdDebug()<<dt.date().toString()<<": e="<<e.toString()<<" l="<<l.toString()<<endl;
        if (e == KPTDuration::zeroDuration) {
            load = 0.0;
        } else {
            load = l*100/e;
        }
        set->append(new KPTChartDataSetItem(dt.addDays(7), dt.addMonths(1).addDays(-7), 0.0, load));
        if (load > maxLoad)
            maxLoad = load;
    }
    m_chartview->setTimeScaleRange(start, end);
    m_chartview->setYScaleRange(0, maxLoad, 10.0);
    return set;
}

void KPTResourceUseView::drawBarLoad(KPTResource *resource) {
    kdDebug()<<k_funcinfo<<resource->name()<<endl;
    QTime t; t.start();
    KPTChartDataSet *set = new KPTChartDataSet();
    switch (m_dataShown) {
        case LoadPrMinute:
            set = drawBarLoadPrMinute(resource);
            break;
        case LoadPrHour:
            set = drawBarLoadPrHour(resource);
            break;
        case LoadPrDay:
            set = drawBarLoadPrDay(resource);
            break;
        case LoadPrWeek:
            set = drawBarLoadPrWeek(resource);
            break;
        case LoadPrMonth:
            set = drawBarLoadPrMonth(resource);
            break;
        default:
            break;
    }
    m_chartview->addData(set);
    m_chartview->draw();
    kdDebug()<<k_funcinfo<<" takes: "<<t.elapsed()<<" ms"<<endl;
}

void KPTResourceUseView::drawLineAccumulated(KPTResource *resource) {
    //kdDebug()<<k_funcinfo<<resource->name()<<endl;
    m_chartview->setDescription(resource->name()+": Accumulated (Hours)");
    m_chartview->setYScaleUnit("Hours");
    KPTAppointment a = resource->appointmentIntervals();        
    KPTChartDataSet *set = new KPTChartDataSet();
    set->setDrawMode(KPTChartDataSet::Mode_Line);
    double currLoad = 0.0;
    QPtrListIterator<KPTAppointmentInterval> it = a.intervals();
    KPTAppointmentInterval *lastInterval = 0;
    for (; it.current(); ++it) {
        double endLoad = currLoad+((double)it.current()->effort().seconds()/3600.0);
        set->append(new KPTChartDataSetItem(it.current()->startTime(), it.current()->endTime(), currLoad, endLoad));
        if (lastInterval != 0) {
            set->append(new KPTChartDataSetItem(lastInterval->endTime(), it.current()->startTime(),  currLoad, currLoad));
            if (lastInterval->endTime() > it.current()->startTime()) {
                kdWarning()<<lastInterval->endTime()<<" > "<<it.current()->startTime()<<endl;
            }
        }
        currLoad = endLoad;
        lastInterval = it.current();
    }
    m_chartview->addData(set);
    m_chartview->setTimeScaleRange(KPTDateTime(a.startTime().date(), QTime()), KPTDateTime(a.endTime().date().addDays(1),QTime()));
    m_chartview->setYScaleRange(0, currLoad, 8.0);
    m_chartview->enableYZeroLine(true);    
    m_chartview->draw();
}

void KPTResourceUseView::drawLineAccumulated(KPTResourceGroup *resource) {
    //kdDebug()<<k_funcinfo<<resource->name()<<endl;
    m_chartview->setDescription(resource->name()+": Accumulated (Hours)");
    m_chartview->setYScaleUnit("Hours");
    KPTAppointment a = resource->appointmentIntervals();        
    KPTChartDataSet *set = new KPTChartDataSet();
    set->setDrawMode(KPTChartDataSet::Mode_Line);
    double currLoad = 0.0;
    QPtrListIterator<KPTAppointmentInterval> it = a.intervals();
    KPTAppointmentInterval *lastInterval = 0;
    for (; it.current(); ++it) {
        double endLoad = currLoad+((double)it.current()->effort().seconds()/3600.0);
        set->append(new KPTChartDataSetItem(it.current()->startTime(), it.current()->endTime(), currLoad, endLoad));
        if (lastInterval != 0) {
            set->append(new KPTChartDataSetItem(lastInterval->endTime(), it.current()->startTime(),  currLoad, currLoad));
            if (lastInterval->endTime() > it.current()->startTime()) {
                kdWarning()<<lastInterval->endTime()<<" > "<<it.current()->startTime()<<endl;
            }
        }
        currLoad = endLoad;
        lastInterval = it.current();
    }
    m_chartview->addData(set);
    m_chartview->setTimeScaleRange(KPTDateTime(a.startTime().date(), QTime()), KPTDateTime(a.endTime().date().addDays(1),QTime()));
    m_chartview->setYScaleRange(0, currLoad, 8.0);
    m_chartview->enableYZeroLine(true);    
    m_chartview->draw();
}

void KPTResourceUseView::drawLineAccumulated() {
    //kdDebug()<<k_funcinfo<<resource->name()<<endl;
    m_chartview->setDescription("Accumulated (Hours)");
    m_chartview->setYScaleUnit("Hours");
    KPTAppointment a;
    QPtrListIterator<KPTResourceGroup> rit = m_project->resourceGroups();
    for (; rit.current(); ++rit) {
        a += rit.current()->appointmentIntervals();
    }
    KPTChartDataSet *set = new KPTChartDataSet();
    set->setDrawMode(KPTChartDataSet::Mode_Line);
    double currLoad = 0.0;
    QPtrListIterator<KPTAppointmentInterval> it = a.intervals();
    KPTAppointmentInterval *lastInterval = 0;
    for (; it.current(); ++it) {
        double endLoad = currLoad+((double)it.current()->effort().seconds()/3600.0);
        set->append(new KPTChartDataSetItem(it.current()->startTime(), it.current()->endTime(), currLoad, endLoad));
        if (lastInterval != 0) {
            set->append(new KPTChartDataSetItem(lastInterval->endTime(), it.current()->startTime(),  currLoad, currLoad));
            if (lastInterval->endTime() > it.current()->startTime()) {
                kdWarning()<<lastInterval->endTime()<<" > "<<it.current()->startTime()<<endl;
            }
        }
        currLoad = endLoad;
        lastInterval = it.current();
    }
    m_chartview->addData(set);
    m_chartview->setTimeScaleRange(KPTDateTime(a.startTime().date(), QTime()), KPTDateTime(a.endTime().date().addDays(1),QTime()));
    m_chartview->setYScaleRange(0, currLoad, 8.0);
    m_chartview->enableYZeroLine(true);    
    m_chartview->draw();
}

void KPTResourceUseView::executed(QListViewItem *item) {
    if (m_selectedItem == item)
        return;
    m_selectedItem = item;
    drawChart();
}

void KPTResourceUseView::slotChartMenuRequested(const QPoint &pos) {
    //kdDebug()<<k_funcinfo<<pos.x()<<","<<pos.y()<<endl;
    QPopupMenu *menu = m_mainview->popupMenu("resourceuse_popup");
    if (menu) {
        (void)menu->exec(pos);
    }
}

void KPTResourceUseView::slotLoadPrMinute() {
    //kdDebug()<<k_funcinfo<<endl;
    m_dataShown = LoadPrMinute;
    m_chartview->clear();
    KPTResource *r = currentResource();
    if (r) {
        drawBarLoad(r);
        return;
    }
    KPTResourceGroup *g = currentGroup();
    if (g) {
        //kdDebug()<<k_funcinfo<<g->name()<<endl;
        drawLineAccumulated(g);
        return;
    }
}

void KPTResourceUseView::slotLoadPrHour() {
    //kdDebug()<<k_funcinfo<<endl;
    m_dataShown = LoadPrHour;
    m_chartview->clear();
    
    KPTResource *r = currentResource();
    if (r) {
        drawBarLoad(r);
        return;
    }
    KPTResourceGroup *g = currentGroup();
    if (g) {
        //kdDebug()<<k_funcinfo<<g->name()<<endl;
        drawLineAccumulated(g);
        return;
    }
}

void KPTResourceUseView::slotLoadPrDay() {
    //kdDebug()<<k_funcinfo<<endl;
    m_dataShown = LoadPrDay;
    m_chartview->clear();
    
    KPTResource *r = currentResource();
    if (r) {
        drawBarLoad(r);
        return;
    }
    KPTResourceGroup *g = currentGroup();
    if (g) {
        //kdDebug()<<k_funcinfo<<g->name()<<endl;
        drawLineAccumulated(g);
        return;
    }
}

void KPTResourceUseView::slotLoadPrWeek() {
    //kdDebug()<<k_funcinfo<<endl;
    m_dataShown = LoadPrWeek;
    m_chartview->clear();
    KPTResource *r = currentResource();
    if (r) {
        drawBarLoad(r);
        return;
    }
    KPTResourceGroup *g = currentGroup();
    if (g) {
        //kdDebug()<<k_funcinfo<<g->name()<<endl;
        drawLineAccumulated(g);
        return;
    }
}

void KPTResourceUseView::slotLoadPrMonth() {
    //kdDebug()<<k_funcinfo<<endl;
    m_dataShown = LoadPrMonth;
    m_chartview->clear();
    KPTResource *r = currentResource();
    if (r) {
        drawBarLoad(r);
        return;
    }
    KPTResourceGroup *g = currentGroup();
    if (g) {
        //kdDebug()<<k_funcinfo<<g->name()<<endl;
        drawLineAccumulated(g);
        return;
    }
}

void KPTResourceUseView::slotAccumulated() {
    //kdDebug()<<k_funcinfo<<endl;
    m_chartview->clear();
    KPTResource *r = currentResource();
    if (r) {
        drawLineAccumulated(r);
        return;
    }
    KPTResourceGroup *g = currentGroup();
    if (g) {
        //kdDebug()<<k_funcinfo<<g->name()<<endl;
        drawLineAccumulated(g);
        return;
    }
    drawLineAccumulated();
}

void KPTResourceUseView::initMenu(KActionCollection *collection) {
    //kdDebug()<<k_funcinfo<<endl;
    actionResourceuseLoadMinute = new KAction(i18n("Load pr Minute"), 0, 0, this, SLOT(slotLoadPrMinute()), collection, "resourceuse_load_minute");
    actionResourceuseLoadHour = new KAction(i18n("Load pr Hour"), 0, 0, this, SLOT(slotLoadPrHour()), collection, "resourceuse_load_hour");
    actionResourceuseLoadDay = new KAction(i18n("Load pr Day"), 0, 0, this, SLOT(slotLoadPrDay()), collection, "resourceuse_load_day");
    actionResourceuseLoadWeek = new KAction(i18n("Load pr Week"), 0, 0, this, SLOT(slotLoadPrWeek()), collection, "resourceuse_load_week");
    actionResourceuseLoadMonth = new KAction(i18n("Load pr Month"), 0, 0, this, SLOT(slotLoadPrMonth()), collection, "resourceuse_load_month");
    actionResourceuseAccumulated = new KAction(i18n("Accumulated"), 0, 0, this, SLOT(slotAccumulated()), collection, "resourceuse_accumulated");

}

KPTResource *KPTResourceUseView::currentResource() {
    ResourceItemUseViewPrivate *r = dynamic_cast<ResourceItemUseViewPrivate*>(m_selectedItem);
    return r == 0 ? 0 : r->resource;
}

KPTResourceGroup *KPTResourceUseView::currentGroup() {
    ResourceGroupItemPrivate *g = dynamic_cast<ResourceGroupItemPrivate*>(m_selectedItem);
    return g == 0 ? 0 : g->group;
}

bool KPTResourceUseView::setContext(KPTContext &context) {
    kdDebug()<<k_funcinfo<<endl;
    return true;
}

void KPTResourceUseView::getContext(KPTContext &context) const {
    kdDebug()<<k_funcinfo<<endl;
}

} //KPlato namespace

#include "kptresourceuseview.moc"
