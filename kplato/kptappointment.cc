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
#include "kptxmlloaderobject.h"

#include <kdebug.h>

namespace KPlato
{

class Resource;

AppointmentInterval::AppointmentInterval() {
    //kDebug()<<k_funcinfo<<this<<endl;
    m_load = 100.0;
}
AppointmentInterval::AppointmentInterval(const AppointmentInterval &interval) {
    //kDebug()<<k_funcinfo<<this<<endl;
    m_start = interval.startTime();
    m_end = interval.endTime();
    m_load = interval.load();
}
AppointmentInterval::AppointmentInterval(const DateTime &start, const DateTime end, double load) {
    //kDebug()<<k_funcinfo<<this<<endl;
    m_start = start;
    m_end = end;
    m_load = load;
}
AppointmentInterval::~AppointmentInterval() {
    //kDebug()<<k_funcinfo<<this<<endl;
}

Duration AppointmentInterval::effort(const DateTime &start, const DateTime end) const {
    if (start >= m_end || end <= m_start) {
        return Duration::zeroDuration;
    }
    DateTime s = (start > m_start ? start : m_start);
    DateTime e = (end < m_end ? end : m_end);
    return (e - s) * m_load / 100;
}

Duration AppointmentInterval::effort(const QDate &time, bool upto) const {
    DateTime t( DateTime( time, m_start.timeSpec() ) );
    if (upto) {
        if (t <= m_start) {
            return Duration::zeroDuration;
        }
        DateTime e = (t < m_end ? t : m_end);
        return (e - m_start) * m_load / 100;
    }
    // from time till end
    if (t >= m_end) {
        return Duration::zeroDuration;
    }
    DateTime s = (t > m_start ? t : m_start);
    return (m_end - s) * m_load / 100;
}

bool AppointmentInterval::loadXML(QDomElement &element, XMLLoaderObject &status) {
    //kDebug()<<k_funcinfo<<endl;
    bool ok;
    QString s = element.attribute("start");
    if (!s.isEmpty())
        m_start = DateTime::fromString(s, status.projectSpec());
    s = element.attribute("end");
    if (!s.isEmpty())
        m_end = DateTime::fromString(s, status.projectSpec());
    m_load = element.attribute("load", "100").toDouble(&ok);
    if (!ok) m_load = 100;
    return m_start.isValid() && m_end.isValid();
}

void AppointmentInterval::saveXML(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("interval");
    element.appendChild(me);

    me.setAttribute("start", m_start.toString( KDateTime::ISODate ));
    me.setAttribute("end", m_end.toString( KDateTime::ISODate ));
    me.setAttribute("load", m_load);
}

bool AppointmentInterval::isValid() const {
    return m_start.isValid() && m_end.isValid();
}

AppointmentInterval AppointmentInterval::firstInterval(const AppointmentInterval &interval, const DateTime &from) const {
    //kDebug()<<k_funcinfo<<interval.startTime().toString()<<" - "<<interval.endTime().toString()<<" from="<<from.toString()<<endl;
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
    //kDebug()<<k_funcinfo<<a.startTime().toString()<<" - "<<a.endTime().toString()<<" load="<<a.load()<<endl;
    return a;
}

void AppointmentIntervalList::inSort(AppointmentInterval *a)
{
    Q_ASSERT( a->startTime() < a->endTime() );
    insert( a->startTime().toString( KDateTime::ISODate ) + a->endTime().toString( KDateTime::ISODate ), a );
}

//////

Appointment::UsedEffortItem::UsedEffortItem(const DateTime& date, Duration effort, bool overtime) {
    m_date = date;
    m_effort = effort;
    m_overtime = overtime;
}
DateTime Appointment::UsedEffortItem::date() {
    return m_date;
}
Duration Appointment::UsedEffortItem::effort() {
    return m_effort;
}
bool Appointment::UsedEffortItem::isOvertime() {
    return m_overtime;
}

Appointment::UsedEffort::UsedEffort() {
}

Appointment::UsedEffort::~UsedEffort() {
    while (!isEmpty())
        delete takeFirst();
}

void Appointment::UsedEffort::inSort(const DateTime& date, Duration effort, bool overtime) {
    UsedEffortItem *item = new UsedEffortItem(date, effort, overtime);
    this->append(item);
//TODO    qSort(*this);
}

Duration Appointment::UsedEffort::usedEffort(bool includeOvertime) const {
    Duration eff;
    QListIterator<UsedEffortItem*> it(*this);
    while (it.hasNext()) {
        UsedEffortItem *i = it.next();
        if (includeOvertime || !i->isOvertime()) {
            eff += i->effort();
        }
    }
    return eff;
}

Duration Appointment::UsedEffort::usedEffort(const QDate &date, bool includeOvertime) const {
    Duration eff;
    QListIterator<UsedEffortItem*> it(*this);
    while (it.hasNext()) {
        UsedEffortItem *i = it.next();
        if ((includeOvertime || !i->isOvertime()) &&
            i->date().date() == date) {
            eff += i->effort();
        }
    }
    return eff;
}

Duration Appointment::UsedEffort::usedEffortTo(const QDate &date, bool includeOvertime) const {
    Duration eff;
    QListIterator<UsedEffortItem*> it(*this);
    while (it.hasNext()) {
        UsedEffortItem *i = it.next();
        if ((includeOvertime || !i->isOvertime()) &&
            i->date().date() <= date) {
            eff += i->effort();
        }
    }
    return eff;
}

Duration Appointment::UsedEffort::usedOvertime() const {
    if (isEmpty()) {
        return Duration::zeroDuration;
    }
    return usedOvertime(first()->date().date());
}

Duration Appointment::UsedEffort::usedOvertime(const QDate &date) const {
    Duration eff;
    QListIterator<UsedEffortItem*> it(*this);
    while (it.hasNext()) {
        UsedEffortItem *i = it.next();
        if (i->isOvertime() && i->date().date() == date) {
            eff += i->effort();
        }
    }
    return eff;
}

Duration Appointment::UsedEffort::usedOvertimeTo(const QDate &date) const {
    Duration eff;
    QListIterator<UsedEffortItem*> it(*this);
    while (it.hasNext()) {
        UsedEffortItem *i = it.next();
        if (i->isOvertime() && i->date().date() <= date) {
            eff += i->effort();
        }
    }
    return eff;
}

bool Appointment::UsedEffort::load(QDomElement &element, XMLLoaderObject &status) {
    QString s;
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "actual-effort") {
                DateTime date = DateTime::fromString(e.attribute("date"), status.projectSpec());
                Duration eff = Duration::fromString(e.attribute("effort"));
                bool ot = e.attribute("overtime", "0").toInt();
                if (date.isValid()) {
                    inSort(date, eff, ot);
                } else {
                    kError()<<k_funcinfo<<"Load failed, illegal date: "<<e.attribute("date")<<endl;
                }
            }
        }
    }
    return true;
}

void Appointment::UsedEffort::save(QDomElement &element) const {
    if (isEmpty()) return;
    QListIterator<UsedEffortItem*> it = *this;
    while (it.hasNext()) {
        UsedEffortItem *i = it.next();
        QDomElement me = element.ownerDocument().createElement("actual-effort");
        element.appendChild(me);
        me.setAttribute("date",i->date().toString( KDateTime::ISODate ));
        me.setAttribute("effort",i->effort().toString());
        me.setAttribute("overtime",i->isOvertime());
    }
}

////
Appointment::Appointment()
    : m_extraRepeats(), m_skipRepeats() {
    //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    m_resource=0;
    m_node=0;
    m_calculationMode = Schedule::Scheduling;
    m_repeatInterval=Duration();
    m_repeatCount=0;
}

Appointment::Appointment(Schedule *resource, Schedule *node, DateTime start, DateTime end, double load)
    : m_extraRepeats(),
      m_skipRepeats() {
    //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    m_node = node;
    m_resource = resource;
    m_calculationMode = Schedule::Scheduling;
    m_repeatInterval = Duration();
    m_repeatCount = 0;

    addInterval(start, end, load);
}

Appointment::Appointment(Schedule *resource, Schedule *node, DateTime start, Duration duration, double load)
    : m_extraRepeats(),
      m_skipRepeats() {
    //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    m_node = node;
    m_resource = resource;
    m_calculationMode = Schedule::Scheduling;
    m_repeatInterval = Duration();
    m_repeatCount = 0;

    addInterval(start, duration, load);

}

Appointment::Appointment( const Appointment &app)
{
    copy( app );
}


Appointment::~Appointment() {
    //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    detach();
    qDeleteAll( m_intervals );
}

void Appointment::addInterval(AppointmentInterval *a) {
    Q_ASSERT( a->startTime() < a->endTime() );
    m_intervals.inSort(a);
    //if ( m_resource && m_resource->resource() && m_node && m_node->node() ) kDebug()<<k_funcinfo<<"Mode="<<m_calculationMode<<": "<<m_resource->resource()->name()<<" to "<<m_node->node()->name()<<" "<<a->startTime()<<a->endTime()<<endl;
}
void Appointment::addInterval(const DateTime &start, const DateTime &end, double load) {
    Q_ASSERT( start < end );
    addInterval(new AppointmentInterval(start, end, load));
}
void Appointment::addInterval(const DateTime &start, const Duration &duration, double load) {
    DateTime e = start+duration;
    addInterval(start, e, load);
}

double Appointment::maxLoad() const {
    double v = 0.0;
    foreach (AppointmentInterval *i, m_intervals.values() ) {
        if (v < i->load())
            v = i->load();
    }
    return v;
}

DateTime Appointment::startTime() const {
    if ( isEmpty() ) {
        //kDebug()<<k_funcinfo<<"empty list"<<endl;
        return DateTime();
    }
    return m_intervals.values().first()->startTime();
}

DateTime Appointment::endTime() const {
    if ( isEmpty() ) {
        //kDebug()<<k_funcinfo<<"empty list"<<endl;
        return DateTime();
    }
    return m_intervals.values().last()->endTime();
}

void Appointment::deleteAppointmentFromRepeatList(DateTime) {
}

void Appointment::addAppointmentToRepeatList(DateTime) {
}

bool Appointment::isBusy(const DateTime &/*start*/, const DateTime &/*end*/) {
    return false;
}

bool Appointment::loadXML(QDomElement &element, XMLLoaderObject &status, Schedule &sch) {
    //kDebug()<<k_funcinfo<<project.name()<<endl;
    Node *node = status.project().findNode(element.attribute("task-id"));
    if (node == 0) {
        kError()<<k_funcinfo<<"The referenced task does not exists: "<<element.attribute("task-id")<<endl;
        return false;
    }
    Resource *res = status.project().resource(element.attribute("resource-id"));
    if (res == 0) {
        kError()<<k_funcinfo<<"The referenced resource does not exists: resource id="<<element.attribute("resource-id")<<endl;
        return false;
    }
    if (!res->addAppointment(this, sch)) {
        kError()<<k_funcinfo<<"Failed to add appointment to resource: "<<res->name()<<endl;
        return false;
    }
    if (!node->addAppointment(this, sch)) {
        kError()<<k_funcinfo<<"Failed to add appointment to node: "<<node->name()<<endl;
        m_resource->takeAppointment(this);
        return false;
    }
    //kDebug()<<k_funcinfo<<"res="<<m_resource<<" node="<<m_node<<endl;
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "interval") {
            AppointmentInterval *a = new AppointmentInterval();
                if (a->loadXML(e, status)) {
                    addInterval(a);
                } else {
                    kError()<<k_funcinfo<<"Could not load interval"<<endl;
                    delete a;
                }
            }
        }
    }
    if (isEmpty()) {
        return false;
    }
    m_actualEffort.load(element, status);
    return true;
}

void Appointment::saveXML(QDomElement &element) const {
    if (isEmpty()) {
        kError()<<k_funcinfo<<"Incomplete appointment data: No intervals"<<endl;
    }
    if (m_resource == 0 || m_resource->resource() == 0) {
        kError()<<k_funcinfo<<"Incomplete appointment data: No resource"<<endl;
        return;
    }
    if (m_node == 0 || m_node->node() == 0) {
        kError()<<k_funcinfo<<"Incomplete appointment data: No node"<<endl;
        return; // shouldn't happen
    }
    //kDebug()<<k_funcinfo<<endl;
    QDomElement me = element.ownerDocument().createElement("appointment");
    element.appendChild(me);

    me.setAttribute("resource-id", m_resource->resource()->id());
    me.setAttribute("task-id", m_node->node()->id());
    foreach (AppointmentInterval *i, m_intervals.values() ) {
        i->saveXML(me);
    }
    m_actualEffort.save(me);
}

// Returns the total actual effort for this appointment
Duration Appointment::plannedEffort() const {
    Duration d;
    foreach (AppointmentInterval *i, m_intervals.values() ) {
        d += i->effort();
    }
    return d;
}

// Returns the planned effort on the date
Duration Appointment::plannedEffort(const QDate &date) const {
    Duration d;
    QDate s(date);
    QDate e(date.addDays(1));
    foreach (AppointmentInterval *i, m_intervals.values() ) {
        d += i->effort(DateTime(s, i->startTime().timeSpec()), DateTime(e, i->startTime().timeSpec()));
    }
    return d;
}

// Returns the planned effort upto and including the date
Duration Appointment::plannedEffortTo(const QDate& date) const {
    Duration d;
    QDate e(date.addDays(1));
    foreach (AppointmentInterval *i, m_intervals.values() ) {
        d += i->effort(e, true);
    }
    return d;
}

// Returns a list of efforts pr day for interval start, end inclusive
// The list only includes days with any planned effort
EffortCostMap Appointment::plannedPrDay(const QDate& start, const QDate& end) const {
    //kDebug()<<k_funcinfo<<m_node->id()<<", "<<m_resource->id()<<endl;
    EffortCostMap ec;
    Duration eff;
    QDate dt(start);
    QDate ndt(dt.addDays(1));
    double rate = m_resource->normalRatePrHour();
    foreach (AppointmentInterval *i, m_intervals.values() ) {
        DateTime st = i->startTime();
        DateTime e = i->endTime();
        if (end < st.date())
            break;
        if (dt < st.date()) {
            dt = st.date();
        }
        ndt = dt.addDays(1);
        while (dt <= e.date()) {
            eff = i->effort(DateTime(dt, st.timeSpec()), DateTime(ndt, st.timeSpec()));
            ec.add(dt, eff, eff.toDouble(Duration::Unit_h) * rate);
            if (dt < e.date()) {
                // loop trough the interval (it spans dates)
                dt = ndt;
                ndt = ndt.addDays(1);
            } else {
                break;
            }
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
    //kDebug()<<k_funcinfo<<m_actualEffort.usedEffort(false /*ex. overtime*/).toDouble(Duration::Unit_h)<<endl;
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

void Appointment::addActualEffort(const DateTime& date, Duration effort, bool overtime) {
    //kDebug()<<k_funcinfo<<endl;
    m_actualEffort.inSort(date, effort, overtime);
}

bool Appointment::attach() {
    //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    if (m_resource && m_node) {
        m_resource->attatch(this);
        m_node->attatch(this);
        return true;
    }
    kWarning()<<k_funcinfo<<"Failed: "<<(m_resource ? "" : "resource=0 ")
                                       <<(m_node ? "" : "node=0")<<endl;
    return false;
}

void Appointment::detach() {
    //kDebug()<<k_funcinfo<<"("<<this<<") "<<m_calculationMode<<": "<<m_resource<<", "<<m_node<<endl;
    if (m_resource) {
        m_resource->takeAppointment(this, m_calculationMode); // takes from node also
    }
    if (m_node) {
        m_node->takeAppointment(this, m_calculationMode); // to make it robust
    }
}

// Returns the effort from start to end
Duration Appointment::effort(const DateTime &start, const DateTime &end) const {
    Duration d;
    foreach (AppointmentInterval *i, m_intervals.values() ) {
        d += i->effort(start, end);
    }
    return d;
}
// Returns the effort from start for the duration
Duration Appointment::effort(const DateTime &start, const Duration &duration) const {
    Duration d;
    foreach (AppointmentInterval *i, m_intervals.values() ) {
        d += i->effort(start, start+duration);
    }
    return d;
}
// Returns the effort upto time / from time
Duration Appointment::effortFrom(const QDate &time) const {
    Duration d;
    foreach (AppointmentInterval *i, m_intervals.values() ) {
        d += i->effort(time, false);
    }
    return d;
}

Appointment &Appointment::operator=(const Appointment &app) {
    copy( app );
    return *this;
}

Appointment &Appointment::operator+=(const Appointment &app) {
    merge( app );
    return *this;
}

Appointment Appointment::operator+(const Appointment &app) {
    Appointment a( *this );
    a.merge(app);
    return a;
}

void Appointment::copy(const Appointment &app) {
    m_resource = 0; //app.resource(); // NOTE: Don't copy, the new appointment
    m_node = 0; //app.node();         // NOTE: doesn't belong to anyone yet.
    //TODO: incomplete but this is all we use atm
    m_calculationMode = app.calculationMode();
    
    //m_repeatInterval = app.repeatInterval();
    //m_repeatCount = app.repeatCount();

    qDeleteAll( m_intervals );
    m_intervals.clear();
    foreach (AppointmentInterval *i, app.intervals().values() ) {
        addInterval(new AppointmentInterval(*i));
    }
}

void Appointment::merge(const Appointment &app) {
    QList<AppointmentInterval*> result;
    QList<AppointmentInterval*> lst1 = m_intervals.values();
    AppointmentInterval *i1;
    QList<AppointmentInterval*> lst2 = app.intervals().values();
    //kDebug()<<k_funcinfo<<"add "<<lst1.count()<<" intervals to "<<lst2.count()<<" intervals"<<endl;
    AppointmentInterval *i2;
    int index1 = 0, index2 = 0;
    DateTime from;
    while (index1 < lst1.size() || index2 < lst2.size()) {
        if (index1 >= lst1.size()) {
            i2 = lst2[index2];
            if (!from.isValid() || from < i2->startTime())
                from = i2->startTime();
            result.append(new AppointmentInterval(from, i2->endTime(), i2->load()));
            //kDebug()<<"Interval+ (i2): "<<from<<" - "<<i2->endTime()<<endl;
            from = i2->endTime();
            ++index2;
            continue;
        }
        if (index2 >= lst2.size()) {
            i1 = lst1[index1];
            if (!from.isValid() || from < i1->startTime())
                from = i1->startTime();
            result.append(new AppointmentInterval(from, i1->endTime(), i1->load()));
            //kDebug()<<"Interval+ (i1): "<<from<<" - "<<i1->endTime()<<endl;
            from = i1->endTime();
            ++index1;
            continue;
        }
        i1 = lst1[index1];
        i2 = lst2[index2];
        AppointmentInterval i =  i1->firstInterval(*i2, from);
        if (!i.isValid()) {
            break;
        }
        result.append(new AppointmentInterval(i)); 
        from = i.endTime();
        //kDebug()<<"Interval+ (i): "<<i.startTime()<<" - "<<i.endTime()<<" load="<<i.load()<<endl;
        if (i.endTime() >= i1->endTime()) {
            ++index1;
        }
        if (i.endTime() >= i2->endTime()) {
            ++index2;
        }
    }
    qDeleteAll( m_intervals );
    m_intervals.clear();
    foreach ( AppointmentInterval *i, result ) {
        m_intervals.inSort( i );
    }
    //kDebug()<<k_funcinfo<<this<<": "<<m_intervals.count()<<endl;
    return;
}

#ifndef NDEBUG
void Appointment::printDebug(const QString& _indent)
{
    QString indent = _indent;
    //kDebug()<<indent<<"  + Appointment: "<<this<<endl;
    bool err = false;
    if (m_node == 0) {
        //kDebug()<<indent<<"   No node schedule"<<endl;
        err = true;
    } else if (m_node->node() == 0) {
        //kDebug()<<indent<<"   No node"<<endl;
        err = true;
    }
    if (m_resource == 0) {
        //kDebug()<<indent<<"   No resource schedule"<<endl;
        err = true;
    } else if (m_resource->resource() == 0) {
        //kDebug()<<indent<<"   No resource"<<endl;
        err = true;
    }
    if (!err) {
        kDebug()<<indent<<"  + Appointment to schedule: "<<m_node->name()<<" ("<<m_node->type()<<"):"<<" task="<<m_node->node()->name()<<", resource="<<m_resource->resource()->name()<<endl;
    } else {
        kDebug()<<indent<<"  + "<<m_intervals.count()<<" appointment intervals: "<<endl;
    }
    indent += "  + ";
    foreach (AppointmentInterval *i, m_intervals.values() ) {
        kDebug()<<indent<<"----"<<i->startTime().toString()<<" - "<<i->endTime().toString()<<" load="<<i->load()<<endl;
    }
}
#endif

}  //KPlato namespace
