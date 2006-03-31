/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

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

#include "kptappointment.h"

#include "kptproject.h"
#include "kpttask.h"
#include "kptdatetime.h"
#include "kptcalendar.h"
#include "kpteffortcostmap.h"
#include "kptschedule.h"

#include <kdebug.h>

namespace KPlato
{

class Resource;

AppointmentInterval::AppointmentInterval() {
    m_load = 100.0; 
}
AppointmentInterval::AppointmentInterval(const AppointmentInterval &interval) {
    //kdDebug()<<k_funcinfo<<endl;
    m_start = interval.startTime(); 
    m_end = interval.endTime(); 
    m_load = interval.load(); 
}
AppointmentInterval::AppointmentInterval(const DateTime &start, const DateTime end, double load) {
    //kdDebug()<<k_funcinfo<<endl;
    m_start = start; 
    m_end = end; 
    m_load = load; 
}
AppointmentInterval::~AppointmentInterval() {
    //kdDebug()<<k_funcinfo<<endl;
}

Duration AppointmentInterval::effort(const DateTime &start, const DateTime end) const {
    if (start >= m_end || end <= m_start) {
        return Duration::zeroDuration;
    }
    DateTime s = (start > m_start ? start : m_start);
    DateTime e = (end < m_end ? end : m_end);
    return (e - s) * m_load / 100;
}

Duration AppointmentInterval::effort(const DateTime &time, bool upto) const {
    if (upto) {
        if (time <= m_start) {
            return Duration::zeroDuration;
        }
        DateTime e = (time < m_end ? time : m_end);
        return (e - m_start) * m_load / 100;
    }
    // from time till end
    if (time >= m_end) {
        return Duration::zeroDuration;
    }
    DateTime s = (time > m_start ? time : m_start);
    return (m_end - s) * m_load / 100;
}

bool AppointmentInterval::loadXML(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    bool ok;
    QString s = element.attribute("start");
    if (s != "")
        m_start = DateTime::fromString(s);
    s = element.attribute("end");
    if (s != "")
        m_end = DateTime::fromString(s);
    m_load = element.attribute("load", "100").toDouble(&ok);
    if (!ok) m_load = 100;
    return m_start.isValid() && m_end.isValid();
}

void AppointmentInterval::saveXML(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("interval");
    element.appendChild(me);

    me.setAttribute("start", m_start.toString(Qt::ISODate));
    me.setAttribute("end", m_end.toString(Qt::ISODate));
    me.setAttribute("load", m_load);
}

bool AppointmentInterval::isValid() const {
    return m_start.isValid() && m_end.isValid();
}

AppointmentInterval AppointmentInterval::firstInterval(const AppointmentInterval &interval, const DateTime &from) const {
    //kdDebug()<<k_funcinfo<<interval.startTime().toString()<<" - "<<interval.endTime().toString()<<" from="<<from.toString()<<endl;
    DateTime f = from;
    DateTime s1 = m_start;
    DateTime e1 = m_end;
    DateTime s2 = interval.startTime();
    DateTime e2 = interval.endTime();
    AppointmentInterval a;
    if (f.isValid() && f >= e1 && f >= e2) {
        return a;
    }
    if (f.isValid()) {
        if (s1 < f && f < e1) {
            s1 = f;
        }
        if (s2 < f && f < e2) {
            s2 = f;
        }
    } else {
        f = s1 < s2 ? s1 : s2;
    }
    if (s1 < s2) {
        a.setStartTime(s1);
        if (e1 <= s2) {
            a.setEndTime(e1);
        } else {
            a.setEndTime(s2);
        }
        a.setLoad(m_load);
    } else if (s1 > s2) {
        a.setStartTime(s2);
        if (e2 <= s1) {
            a.setEndTime(e2);
        } else {
            a.setEndTime(s1);
        }
        a.setLoad(interval.load());
    } else {
        a.setStartTime(s1);
        if (e1 <= e2)
            a.setEndTime(e1);
        else 
            a.setEndTime(e2);
        a.setLoad(m_load + interval.load());
    }
    //kdDebug()<<k_funcinfo<<a.startTime().toString()<<" - "<<a.endTime().toString()<<" load="<<a.load()<<endl;
    return a;
}

//////

Appointment::UsedEffortItem::UsedEffortItem(QDate date, Duration effort, bool overtime) {
    m_date = date;
    m_effort = effort;
    m_overtime = overtime;
}
QDate Appointment::UsedEffortItem::date() { 
    return m_date; 
}
Duration Appointment::UsedEffortItem::effort() {
    return m_effort; 
}
bool Appointment::UsedEffortItem::isOvertime() { 
    return m_overtime; 
}

Appointment::UsedEffort::UsedEffort() { 
    setAutoDelete(true); 
}

void Appointment::UsedEffort::inSort(QDate date, Duration effort, bool overtime) {
    UsedEffortItem *item = new UsedEffortItem(date, effort, overtime);
    QPtrList<UsedEffortItem>::inSort(item);
}

Duration Appointment::UsedEffort::usedEffort(bool includeOvertime) const {
    Duration eff;
    QPtrListIterator<UsedEffortItem> it(*this);
    for (; it.current(); ++it) {
        if (includeOvertime || !it.current()->isOvertime()) {
            eff += it.current()->effort();
        }
    }
    return eff;
}
        
Duration Appointment::UsedEffort::usedEffort(const QDate &date, bool includeOvertime) const {
    Duration eff;
    QPtrListIterator<UsedEffortItem> it(*this);
    for (; it.current(); ++it) {
        if ((includeOvertime || !it.current()->isOvertime()) && 
            it.current()->date() == date) {
            eff += it.current()->effort();
        }
    }
    return eff;
}

Duration Appointment::UsedEffort::usedEffortTo(const QDate &date, bool includeOvertime) const {
    Duration eff;
    QPtrListIterator<UsedEffortItem> it(*this);
    for (; it.current(); ++it) {
        if ((includeOvertime || !it.current()->isOvertime()) && 
            it.current()->date() <= date) {
            eff += it.current()->effort();
        }
    }
    return eff;
}

Duration Appointment::UsedEffort::usedOvertime() const {
    UsedEffortItem *item = getFirst();
    return item==0 ? Duration::zeroDuration : usedOvertime(item->date());
}

Duration Appointment::UsedEffort::usedOvertime(const QDate &date) const {
    Duration eff;
    QPtrListIterator<UsedEffortItem> it(*this);
    for (; it.current(); ++it) {
        if (it.current()->isOvertime() && it.current()->date() == date) {
            eff += it.current()->effort();
        }
    }
    return eff;
}

Duration Appointment::UsedEffort::usedOvertimeTo(const QDate &date) const {
    Duration eff;
    QPtrListIterator<UsedEffortItem> it(*this);
    for (; it.current(); ++it) {
        if (it.current()->isOvertime() && it.current()->date() <= date) {
            eff += it.current()->effort();
        }
    }
    return eff;
}

bool Appointment::UsedEffort::load(QDomElement &element) {
    QString s;
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "actual-effort") {
                QDate date;
                s = e.attribute("date");
                if (s != "")
                    date = QDate::fromString(s, Qt::ISODate);
                Duration eff = Duration::fromString(e.attribute("effort"));
                bool ot = e.attribute("overtime", "0").toInt();
                if (date.isValid()) {
                    inSort(date, eff, ot);
                } else {
                    kdError()<<k_funcinfo<<"Load failed, illegal date: "<<e.attribute("date")<<endl;
                }
            }
        }   
    }
    return true;
}

void Appointment::UsedEffort::save(QDomElement &element) const {
    if (isEmpty()) return;
    QPtrListIterator<UsedEffortItem> it = *this;
    for (; it.current(); ++it) {
        QDomElement me = element.ownerDocument().createElement("actual-effort");
        element.appendChild(me);
        me.setAttribute("date",it.current()->date().toString(Qt::ISODate));
        me.setAttribute("effort",it.current()->effort().toString());
        me.setAttribute("overtime",it.current()->isOvertime());
    }
}

int Appointment::UsedEffort::compareItems(QPtrCollection::Item item1, QPtrCollection::Item item2) {
    QDate d1 = static_cast<UsedEffortItem*>(item1)->date();
    QDate d2 = static_cast<UsedEffortItem*>(item2)->date();
    if (d1 > d2) return 1;
    if (d1 < d2) return -1;
    return 0;
}

////
Appointment::Appointment() 
    : m_extraRepeats(), m_skipRepeats() {
    //kdDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    m_resource=0;
    m_node=0;
    m_repeatInterval=Duration();
    m_repeatCount=0;

    m_intervals.setAutoDelete(true);
}

Appointment::Appointment(Schedule *resource, Schedule *node, DateTime start, DateTime end, double load) 
    : m_extraRepeats(), 
      m_skipRepeats() {
    //kdDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    m_node = node;
    m_resource = resource;
    m_repeatInterval = Duration();
    m_repeatCount = 0;

    addInterval(start, end, load);

    m_intervals.setAutoDelete(true);
}

Appointment::Appointment(Schedule *resource, Schedule *node, DateTime start, Duration duration, double load) 
    : m_extraRepeats(), 
      m_skipRepeats() {
    //kdDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    m_node = node;
    m_resource = resource;
    m_repeatInterval = Duration();
    m_repeatCount = 0;

    addInterval(start, duration, load);
    
    m_intervals.setAutoDelete(true);
}

Appointment::~Appointment() {
    //kdDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    detach();
}

void Appointment::addInterval(AppointmentInterval *a) {
    //kdDebug()<<k_funcinfo<<m_resource->name()<<" to "<<m_node->name()<<endl;
    m_intervals.inSort(a);
}
void Appointment::addInterval(const DateTime &start, const DateTime &end, double load) {
    addInterval(new AppointmentInterval(start, end, load));
}
void Appointment::addInterval(const DateTime &start, const Duration &duration, double load) {
    DateTime e = start+duration;
    addInterval(start, e, load);
}

double Appointment::maxLoad() const {
    double v = 0.0;
    QPtrListIterator<AppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        if (v < it.current()->load())
            v = it.current()->load();
    }
    return v;
}

DateTime Appointment::startTime() const {
    DateTime t;
    QPtrListIterator<AppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        if (!t.isValid() || t > it.current()->startTime())
            t = it.current()->startTime();
    }
    return t;
}

DateTime Appointment::endTime() const {
    DateTime t;
    QPtrListIterator<AppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        if (!t.isValid() || t < it.current()->endTime())
            t = it.current()->endTime();
    }
    return t;
}

void Appointment::deleteAppointmentFromRepeatList(DateTime) {
}

void Appointment::addAppointmentToRepeatList(DateTime) {
}

bool Appointment::isBusy(const DateTime &/*start*/, const DateTime &/*end*/) {
    return false;
}

bool Appointment::loadXML(QDomElement &element, Project &project, Schedule &sch) {
    //kdDebug()<<k_funcinfo<<endl;
    QDictIterator<Node> it = project.nodeDict();
/*    for (; it.current(); ++it) {
        kdDebug()<<" Node="<<it.current()->name()<<" id="<<it.currentKey()<<endl;
    }*/
    Node *node = project.findNode(element.attribute("task-id"));
    if (node == 0) {
        kdError()<<k_funcinfo<<"The referenced task does not exists: "<<element.attribute("task-id")<<endl;
        return false;
    }
    Resource *res = project.resource(element.attribute("resource-id"));
    if (res == 0) {
        kdError()<<k_funcinfo<<"The referenced resource does not exists: resource id="<<element.attribute("resource-id")<<endl;
        return false;
    }
    if (!res->addAppointment(this, sch)) {
        kdError()<<k_funcinfo<<"Failed to add appointment to resource: "<<res->name()<<endl;
        return false;
    }
    if (!node->addAppointment(this, sch)) {
        kdError()<<k_funcinfo<<"Failed to add appointment to node: "<<node->name()<<endl;
        m_resource->takeAppointment(this);
        return false;
    }
    //kdDebug()<<k_funcinfo<<"res="<<m_resource<<" node="<<m_node<<endl;
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "interval") {
            AppointmentInterval *a = new AppointmentInterval();
                if (a->loadXML(e)) {
                    addInterval(a);
                } else {
                    kdError()<<k_funcinfo<<"Could not load interval"<<endl;
                    delete a;
                }
            }
        }
    }
    if (m_intervals.isEmpty()) {
        return false; 
    }
    m_actualEffort.load(element);
    return true;
}

void Appointment::saveXML(QDomElement &element) const {
    if (m_intervals.isEmpty()) {
        kdError()<<k_funcinfo<<"Incomplete appointment data: No intervals"<<endl;
    }
    if (m_resource == 0 || m_resource->resource() == 0) {
        kdError()<<k_funcinfo<<"Incomplete appointment data: No resource"<<endl;
        return;
    }
    if (m_node == 0 || m_node->node() == 0) {
        kdError()<<k_funcinfo<<"Incomplete appointment data: No node"<<endl;
        return; // shouldn't happen
    }
    //kdDebug()<<k_funcinfo<<endl;
    QDomElement me = element.ownerDocument().createElement("appointment");
    element.appendChild(me);

    me.setAttribute("resource-id", m_resource->resource()->id());
    me.setAttribute("task-id", m_node->node()->id());
    QPtrListIterator<AppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        it.current()->saveXML(me);
    }
    m_actualEffort.save(me);
}

// Returns the total actual effort for this appointment
Duration Appointment::plannedEffort() const {
    Duration d;
    QPtrListIterator<AppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        d += it.current()->effort();
    }
    return d;
}

// Returns the planned effort on the date
Duration Appointment::plannedEffort(const QDate &date) const {
    Duration d;
    DateTime s(date);
    DateTime e(date.addDays(1));
    QPtrListIterator<AppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        d += it.current()->effort(s, e);
    }
    return d;
}

// Returns the planned effort upto and including the date
Duration Appointment::plannedEffortTo(const QDate& date) const {
    Duration d;
    DateTime e(date.addDays(1));
    QPtrListIterator<AppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        d += it.current()->effort(e, true);
    }
    return d;
}

// Returns a list of efforts pr day for interval start, end inclusive
// The list only includes days with any planned effort
EffortCostMap Appointment::plannedPrDay(const QDate& start, const QDate& end) const {
    //kdDebug()<<k_funcinfo<<m_node->id()<<", "<<m_resource->id()<<endl;
    EffortCostMap ec;
    Duration eff;
    DateTime dt(start);
    DateTime ndt(dt.addDays(1));
    double rate = m_resource->normalRatePrHour();
    AppointmentIntervalListIterator it = m_intervals;
    for (; it.current(); ++it) {
        DateTime st = it.current()->startTime();
        DateTime e = it.current()->endTime();
        if (end < st.date())
            break;
        if (dt.date() < st.date()) {
            dt.setDate(st.date());
        }
        ndt = dt.addDays(1);
        // loop trough the interval (it may span dates)
        while (dt.date() <= e.date()) {
            eff = it.current()->effort(dt, ndt);
            ec.add(dt.date(), eff, eff.toDouble(Duration::Unit_h) * rate);
            dt = ndt;
            ndt = ndt.addDays(1);
        }
    }
    return ec;
}


// Returns the total actual effort for this appointment
Duration Appointment::actualEffort() const {
    return m_actualEffort.usedEffort();
}

// Returns the actual effort on the date
Duration Appointment::actualEffort(const QDate &date) const {
    return m_actualEffort.usedEffort(date);
}

// Returns the actual effort upto and including date
Duration Appointment::actualEffortTo(const QDate &date) const {
    return m_actualEffort.usedEffortTo(date);
}

double Appointment::plannedCost() {
    if (m_resource && m_resource->resource()) {
        return plannedEffort().toDouble(Duration::Unit_h) * m_resource->resource()->normalRate(); //FIXME overtime
    }
    return 0.0;
}

//Calculates the planned cost on date
double Appointment::plannedCost(const QDate &date) {
    if (m_resource && m_resource->resource()) {
        return plannedEffort(date).toDouble(Duration::Unit_h) * m_resource->resource()->normalRate(); //FIXME overtime
    }
    return 0.0;
}

//Calculates the planned cost upto and including date
double Appointment::plannedCostTo(const QDate &date) {
    if (m_resource && m_resource->resource()) {
        return plannedEffortTo(date).toDouble(Duration::Unit_h) * m_resource->resource()->normalRate(); //FIXME overtime
    }
    return 0.0;
}

// Calculates the total actual cost for this appointment
double Appointment::actualCost() {
    //kdDebug()<<k_funcinfo<<m_actualEffort.usedEffort(false /*ex. overtime*/).toDouble(Duration::Unit_h)<<endl;
    if (m_resource && m_resource->resource()) {
        return (m_actualEffort.usedEffort(false /*ex. overtime*/).toDouble(Duration::Unit_h)*m_resource->resource()->normalRate()) + (m_actualEffort.usedOvertime().toDouble(Duration::Unit_h)*m_resource->resource()->overtimeRate());
    }
    return 0.0;
}

// Calculates the actual cost on date
double Appointment::actualCost(const QDate &date) {
    if (m_resource && m_resource->resource()) {
        return (m_actualEffort.usedEffort(date, false /*ex. overtime*/).toDouble(Duration::Unit_h)*m_resource->resource()->normalRate()) + (m_actualEffort.usedOvertime(date).toDouble(Duration::Unit_h)*m_resource->resource()->overtimeRate());
    }
    return 0.0;
}

// Calculates the actual cost upto and including date
double Appointment::actualCostTo(const QDate &date) {
    if (m_resource && m_resource->resource()) {
        return (m_actualEffort.usedEffortTo(date, false /*ex. overtime*/).toDouble(Duration::Unit_h)*m_resource->resource()->normalRate()) + (m_actualEffort.usedOvertimeTo(date).toDouble(Duration::Unit_h)*m_resource->resource()->overtimeRate());
    }
    return 0.0;
}

void Appointment::addActualEffort(QDate date, Duration effort, bool overtime) {
    m_actualEffort.inSort(date, effort, overtime);
}

bool Appointment::attach() { 
    //kdDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    if (m_resource && m_node) {
        m_resource->add(this);
        m_node->add(this);
        return true;
    }
    kdWarning()<<k_funcinfo<<"Failed: "<<(m_resource ? "" : "resource=0 ") 
                                       <<(m_node ? "" : "node=0")<<endl;
    return false;
}

void Appointment::detach() {
    //kdDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    if (m_resource) {
        m_resource->takeAppointment(this); // takes from node also
    }
    if (m_node) {
        m_node->takeAppointment(this); // to make it robust
    }
}

// Returns the effort from start to end
Duration Appointment::effort(const DateTime &start, const DateTime &end) const {
    Duration d;
    QPtrListIterator<AppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        d += it.current()->effort(start, end);
    }
    return d;
}
// Returns the effort from start for the duration
Duration Appointment::effort(const DateTime &start, const Duration &duration) const {
    Duration d;
    QPtrListIterator<AppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        d += it.current()->effort(start, start+duration);
    }
    return d;
}
// Returns the effort upto time / from time 
Duration Appointment::effortFrom(const DateTime &time) const {
    Duration d;
    QPtrListIterator<AppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        d += it.current()->effort(time, false);
    }
    return d;
}

Appointment &Appointment::operator=(const Appointment &app) {
    m_resource = app.resource();
    m_node = app.node();
    m_repeatInterval = app.repeatInterval();
    m_repeatCount = app.repeatCount();

    m_intervals.clear();
    QPtrListIterator<AppointmentInterval> it = app.intervals();
    for (; it.current(); ++it) {
        addInterval(new AppointmentInterval(*(it.current())));
    }
    return *this;
}

Appointment &Appointment::operator+=(const Appointment &app) {
    *this = *this + app;
    return *this;
}

Appointment Appointment::operator+(const Appointment &app) {
    Appointment a;
    AppointmentIntervalList ai = app.intervals();
    AppointmentInterval i;
    AppointmentInterval *i1 = m_intervals.first();
    AppointmentInterval *i2 = ai.first();
    DateTime from;
    while (i1 || i2) {
        if (!i1) {
            if (!from.isValid() || from < i2->startTime())
                from = i2->startTime();
            a.addInterval(from, i2->endTime(), i2->load());
            //kdDebug()<<"Interval+ (i2): "<<from.toString()<<" - "<<i2->endTime().toString()<<endl;
            from = i2->endTime();
            i2 = ai.next();
            continue;
        }
        if (!i2) {
            if (!from.isValid() || from < i1->startTime())
                from = i1->startTime();
            a.addInterval(from, i1->endTime(), i1->load());
            //kdDebug()<<"Interval+ (i1): "<<from.toString()<<" - "<<i1->endTime().toString()<<endl;
            from = i1->endTime();
            i1 = m_intervals.next();
            continue;
        }
        i =  i1->firstInterval(*i2, from);
        if (!i.isValid()) {
            break;
        }
        a.addInterval(i);
        from = i.endTime();
        //kdDebug()<<"Interval+ (i): "<<i.startTime().toString()<<" - "<<i.endTime().toString()<<" load="<<i.load()<<endl;
        if (i1 && a.endTime() >= i1->endTime()) {
            i1 = m_intervals.next();
        }
        if (i2 && a.endTime() >= i2->endTime()) {
            i2 = ai.next();
        }
    }
    return a;
}

#ifndef NDEBUG
void Appointment::printDebug(QString indent)
{
    bool err = false;
    if (m_node == 0) {
        kdDebug()<<indent<<"   No node schedule"<<endl;
        err = true;
    } else if (m_node->node() == 0) {
        kdDebug()<<indent<<"   No node"<<endl;
        err = true;
    }
    if (m_resource == 0) {
        kdDebug()<<indent<<"   No resource schedule"<<endl;
        err = true;
    } else if (m_resource->resource() == 0) {
        kdDebug()<<indent<<"   No resource"<<endl;
        err = true;
    }
    if (err)
        return;
    kdDebug()<<indent<<"  + Appointment to schedule: "<<m_node->name()<<" ("<<m_node->type()<<")"<<" resource: "<<m_resource->resource()->name()<<endl;
    indent += "  ! ";
    QPtrListIterator<AppointmentInterval> it = intervals();
    for (; it.current(); ++it) {
        kdDebug()<<indent<<it.current()->startTime().toString()<<" - "<<it.current()->endTime().toString()<<" load="<<it.current()->load()<<endl;
    }
}
#endif

}  //KPlato namespace
