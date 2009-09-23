/* This file is part of the KDE project
   Copyright (C) 2005 - 2007 Dag Andersen <danders@get2net.dk>

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

#include "kptappointment.h"

#include "kptproject.h"
#include "kpttask.h"
#include "kptdatetime.h"
#include "kptcalendar.h"
#include "kpteffortcostmap.h"
#include "kptschedule.h"
#include "kptxmlloaderobject.h"

#include <kdebug.h>

#include <qdatetime.h>
#include <QMutableMapIterator>

namespace KPlato
{

class Resource;

AppointmentInterval::AppointmentInterval() {
    //kDebug()<<this;
    m_load = 100.0;
}
AppointmentInterval::AppointmentInterval(const AppointmentInterval &interval) {
    //kDebug()<<this;
    m_start = interval.startTime();
    m_end = interval.endTime();
    m_load = interval.load();
}
AppointmentInterval::AppointmentInterval(const DateTime &start, const DateTime &end, double load) {
    //kDebug()<<this;
    m_start = start;
    m_end = end;
    m_load = load;
}
AppointmentInterval::~AppointmentInterval() {
    //kDebug()<<this;
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
    DateTime t( time, QTime(), m_start.timeSpec() );
    //kDebug()<<time<<upto<<t<<m_start<<m_end;
    if (upto) {
        if (t <= m_start) {
            return Duration::zeroDuration;
        }
        DateTime e = (t < m_end ? t : m_end);
        Duration d = (e - m_start) * m_load / 100;
        //kDebug()<<d.toString();
        return d;
    }
    // from time till end
    if (t >= m_end) {
        return Duration::zeroDuration;
    }
    DateTime s = (t > m_start ? t : m_start);
    return (m_end - s) * m_load / 100;
}

bool AppointmentInterval::loadXML(KoXmlElement &element, XMLLoaderObject &status) {
    //kDebug();
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
    //kDebug()<<interval.startTime().toString()<<" -"<<interval.endTime().toString()<<" from="<<from.toString();
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
    //kDebug()<<a.startTime().toString()<<" -"<<a.endTime().toString()<<" load="<<a.load();
    return a;
}

bool AppointmentInterval::operator ==(const AppointmentInterval &interval ) const
{
    return m_start == interval.m_start && m_end == interval.m_end && m_load == interval.m_load;
}

//-----------------------
AppointmentIntervalList &AppointmentIntervalList::operator+=( const AppointmentIntervalList &lst )
{
    if ( lst.isEmpty() ) {
        return *this;
    }
    foreach ( const AppointmentInterval &ai, lst ) {
        add( ai );
    }
    return *this;
}

void AppointmentIntervalList::add( const AppointmentInterval &ai )
{
    add( ai.startTime(), ai.endTime(), ai.load() );
}

void AppointmentIntervalList::add( const DateTime &st, const DateTime &et, double load )
{
    //kDebug()<<st<<et<<load;
    Q_ASSERT( st < et );
    DateTime s = st;
    DateTime e = et;
    if ( isEmpty() || et < values().first().startTime() || st > values().last().endTime() ) {
        inSort( AppointmentInterval( s, e, load ) );
    } else {
        // see if we have overlapping intervals
        AppointmentIntervalList lst;
        QMutableMapIterator<QString, AppointmentInterval> it( *this );
        while ( it.hasNext() ) {
            AppointmentInterval &ai = it.next().value();
            DateTime ist = ai.startTime();
            DateTime iet = ai.endTime();
            //kDebug()<<"Check:"<<s<<ist<<e<<iet;
            if ( s == ist && e == iet ) {
                //kDebug()<<"Exact match";
                ai.setLoad( ai.load() + load );
                return;
            }
            if ( e <= ist ) {
                //kDebug()<<"Before first or in between";
                inSort( AppointmentInterval( s, e, load ) );
                break;
            }
            if ( s >= iet ) {
                continue;
            }
            if ( s > ist ) {
                //kDebug()<<"Start in the middle";
                double l = ai.load();
                lst.inSort( AppointmentInterval( ist, s, l ) );
                if ( e < iet ) {
                    //kDebug()<<"End in the midle";
                    lst.inSort( AppointmentInterval( s, e, l + load ) );
                    lst.inSort( AppointmentInterval( e, iet, l ) );
                    it.remove();
                    break;
                }
                if ( e == iet ) {
                    //kDebug()<<"End at end";
                    lst.inSort( AppointmentInterval( s, e, l + load ) );
                    it.remove();
                    break;
                }
                lst.inSort( AppointmentInterval( s, iet, l + load ) );
                if ( ! it.hasNext() ) {
                    //kDebug()<<"e > iet -> at last interval";
                    lst.inSort( AppointmentInterval( iet, e, l + load ) );
                    it.remove();
                    break;
                }
                //kDebug()<<"e > iet -> may overlap next interval";
                s = iet;
                it.remove();
                continue;
            }
            if ( s < ist ) {
                // Add the part that's before this interval
                //kDebug()<<"start before";
                lst.inSort( AppointmentInterval( s, ist, load ) );
                s = ist;
            }
            if ( s == ist ) {
                //kDebug()<<"Start now at start";
                double l = ai.load();
                if ( e < iet ) {
                    //kDebug()<<"End in the midle";
                    lst.inSort( AppointmentInterval( s, e, l + load ) );
                    lst.inSort( AppointmentInterval( e, iet, l ) );
                    it.remove();
                    break;
                }
                if ( e == iet ) {
                    //kDebug()<<"End at end";
                    ai.setLoad( l + load );
                    break;
                }
                ai.setLoad( l + load );
                if ( ! it.hasNext() ) {
                    //kDebug()<<"e > iet -> at last interval";
                    lst.inSort( AppointmentInterval( iet, e, l ) );
                    break;
                }
                //kDebug()<<"e > iet -> may overlap next interval";
                s = iet;
                continue;
            }
        }
        foreach ( const AppointmentInterval &a, lst ) {
            //kDebug()<<"Add:"<<a;
            inSort( a );
        }
    }
}

void AppointmentIntervalList::inSort(const AppointmentInterval &a)
{
    Q_ASSERT( a.startTime() < a.endTime() );
    insert( a.startTime().toString( KDateTime::ISODate ) + a.endTime().toString( KDateTime::ISODate ), a );
}

void AppointmentIntervalList::saveXML( QDomElement &element ) const
{
    foreach (const AppointmentInterval &i, *this ) {
        i.saveXML(element);
    }
}

bool AppointmentIntervalList::loadXML( KoXmlElement &element, XMLLoaderObject &status )
{
    KoXmlElement e;
    forEachElement(e, element) {
        if (e.tagName() == "interval") {
            AppointmentInterval a;
            if (a.loadXML(e, status)) {
                inSort(a);
            } else {
                kError()<<"Could not load interval";
            }
        }
    }
    return true;
}


////
Appointment::Appointment()
    : m_extraRepeats(), m_skipRepeats() {
    //kDebug()<<"("<<this<<")";
    m_resource=0;
    m_node=0;
    m_calculationMode = Schedule::Scheduling;
    m_repeatInterval=Duration();
    m_repeatCount=0;
}

Appointment::Appointment(Schedule *resource, Schedule *node, DateTime start, DateTime end, double load)
    : m_extraRepeats(),
      m_skipRepeats() {
    //kDebug()<<"("<<this<<")";
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
    //kDebug()<<"("<<this<<")";
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
    //kDebug()<<"("<<this<<")";
    detach();
}

void Appointment::clear()
{
    m_intervals.clear();
}

AppointmentIntervalList Appointment::intervals( const DateTime &start, const DateTime &end ) const
{
    //kDebug()<<start<<end;
    AppointmentIntervalList lst;
    foreach ( const AppointmentInterval &i, m_intervals ) {
        if ( i.startTime() >= end || i.endTime() <= start ) {
            continue;
        }
        AppointmentInterval ai( i );
        if ( ai.startTime() < start ) {
            ai.setStartTime( start );
        }
        if ( ai.endTime() > end ) {
            ai.setEndTime( end );
        }
        lst.inSort( ai );
        //kDebug()<<ai.startTime().toString()<<ai.endTime().toString();
    }
    return lst;
}

void Appointment::setIntervals(const AppointmentIntervalList &lst) {
    m_intervals.clear();
    foreach( const AppointmentInterval &i, lst ) {
	m_intervals.inSort( i );
    }
}

void Appointment::addInterval(const AppointmentInterval &a) {
    Q_ASSERT( a.startTime() < a.endTime() );
    m_intervals.inSort(a);
    //if ( m_resource && m_resource->resource() && m_node && m_node->node() ) kDebug()<<"Mode="<<m_calculationMode<<":"<<m_resource->resource()->name()<<" to"<<m_node->node()->name()<<""<<a.startTime()<<a.endTime();
}
void Appointment::addInterval(const DateTime &start, const DateTime &end, double load) {
    Q_ASSERT( start < end );
    addInterval(AppointmentInterval(start, end, load));
}

void Appointment::addInterval(const DateTime &start, const Duration &duration, double load) {
    DateTime e = start+duration;
    addInterval(start, e, load);
}

double Appointment::maxLoad() const {
    double v = 0.0;
    foreach (const AppointmentInterval &i, m_intervals ) {
        if (v < i.load())
            v = i.load();
    }
    return v;
}

DateTime Appointment::startTime() const {
    if ( isEmpty() ) {
        //kDebug()<<"empty list";
        return DateTime();
    }
    return m_intervals.values().first().startTime();
}

DateTime Appointment::endTime() const {
    if ( isEmpty() ) {
        //kDebug()<<"empty list";
        return DateTime();
    }
    return m_intervals.values().last().endTime();
}

void Appointment::deleteAppointmentFromRepeatList(DateTime) {
}

void Appointment::addAppointmentToRepeatList(DateTime) {
}

bool Appointment::isBusy(const DateTime &/*start*/, const DateTime &/*end*/) {
    return false;
}

bool Appointment::loadXML(KoXmlElement &element, XMLLoaderObject &status, Schedule &sch) {
    //kDebug()<<project.name();
    Node *node = status.project().findNode(element.attribute("task-id"));
    if (node == 0) {
        kError()<<"The referenced task does not exists: "<<element.attribute("task-id");
        return false;
    }
    Resource *res = status.project().resource(element.attribute("resource-id"));
    if (res == 0) {
        kError()<<"The referenced resource does not exists: resource id="<<element.attribute("resource-id");
        return false;
    }
    if (!res->addAppointment(this, sch)) {
        kError()<<"Failed to add appointment to resource: "<<res->name();
        return false;
    }
    if (!node->addAppointment(this, sch)) {
        kError()<<"Failed to add appointment to node: "<<node->name();
        m_resource->takeAppointment(this);
        return false;
    }
    //kDebug()<<"res="<<m_resource<<" node="<<m_node;
    m_intervals.loadXML( element, status );
    if (isEmpty()) {
        return false;
    }
    return true;
}

void Appointment::saveXML(QDomElement &element) const {
    if (isEmpty()) {
        kError()<<"Incomplete appointment data: No intervals";
    }
    if (m_resource == 0 || m_resource->resource() == 0) {
        kError()<<"Incomplete appointment data: No resource";
        return;
    }
    if (m_node == 0 || m_node->node() == 0) {
        kError()<<"Incomplete appointment data: No node";
        return; // shouldn't happen
    }
    //kDebug();
    QDomElement me = element.ownerDocument().createElement("appointment");
    element.appendChild(me);

    me.setAttribute("resource-id", m_resource->resource()->id());
    me.setAttribute("task-id", m_node->node()->id());
    m_intervals.saveXML( me );
}

// Returns the total planned effort for this appointment
Duration Appointment::plannedEffort() const {
    Duration d;
    foreach (const AppointmentInterval &i, m_intervals ) {
        d += i.effort();
    }
    return d;
}

// Returns the planned effort on the date
Duration Appointment::plannedEffort(const QDate &date) const {
    Duration d;
    QDateTime s(date);
    QDateTime e(date.addDays(1));
    foreach (const AppointmentInterval &i, m_intervals ) {
        d += i.effort(DateTime(s, i.startTime().timeSpec()), DateTime(e, i.startTime().timeSpec()));
    }
    return d;
}

// Returns the planned effort upto and including the date
Duration Appointment::plannedEffortTo(const QDate& date) const {
    Duration d;
    QDate e(date.addDays(1));
    foreach (const AppointmentInterval &i, m_intervals ) {
        d += i.effort(e, true); // upto e, not including
    }
    //kDebug()<<date<<d.toString();
    return d;
}

// Returns a list of efforts pr day for interval start, end inclusive
// The list only includes days with any planned effort
EffortCostMap Appointment::plannedPrDay(const QDate& pstart, const QDate& pend) const {
    //kDebug()<<m_node->id()<<","<<m_resource->id();
    EffortCostMap ec;
    Duration eff;
    QDate start = pstart.isValid() ? pstart : startTime().date();
    QDate end = pend.isValid() ? pend : endTime().date();
    QDate dt(start);
    QDate ndt(dt.addDays(1));
    double rate = m_resource ? m_resource->normalRatePrHour() : 0.0;
    //kDebug()<<rate<<m_intervals.count();
    foreach (const AppointmentInterval &i, m_intervals ) {
        DateTime st = i.startTime();
        DateTime e = i.endTime();
        if (end < st.date()) {
            break;
        }
        if (dt < st.date()) {
            dt = st.date();
        }
        ndt = dt.addDays(1);
        while (dt <= e.date() && dt <= end ) {
            //kDebug()<<start<<end<<dt;
            eff = i.effort(DateTime(dt, QTime( 0, 0, 0 ), st.timeSpec()), DateTime(ndt, QTime( 0, 0, 0 ), st.timeSpec()));
            ec.add(dt, eff, eff.toDouble(Duration::Unit_h) * rate);
            if (dt < e.date() && dt < end) {
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


EffortCost Appointment::plannedCost() const {
    EffortCost ec;
    ec.setEffort( plannedEffort() );
    if (m_resource && m_resource->resource()) {
        ec.setCost( ec.hours() * m_resource->resource()->normalRate() ); //FIXME overtime
    }
    return ec;
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

bool Appointment::attach() {
    //kDebug()<<"("<<this<<")";
    if (m_resource && m_node) {
        m_resource->attatch(this);
        m_node->attatch(this);
        return true;
    }
    kWarning()<<"Failed: "<<(m_resource ? "" : "resource=0 ")
                                       <<(m_node ? "" : "node=0");
    return false;
}

void Appointment::detach() {
    //kDebug()<<"("<<this<<")"<<m_calculationMode<<":"<<m_resource<<","<<m_node;
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
    foreach (const AppointmentInterval &i, m_intervals ) {
        d += i.effort(start, end);
    }
    return d;
}
// Returns the effort from start for the duration
Duration Appointment::effort(const DateTime &start, const Duration &duration) const {
    Duration d;
    foreach (const AppointmentInterval &i, m_intervals ) {
        d += i.effort(start, start+duration);
    }
    return d;
}
// Returns the effort upto time / from time
Duration Appointment::effortFrom(const QDate &time) const {
    Duration d;
    foreach (const AppointmentInterval &i, m_intervals ) {
        d += i.effort(time, false);
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

    m_intervals.clear();
    foreach (const AppointmentInterval &i, app.intervals() ) {
        addInterval(i);
    }
}

void Appointment::merge(const Appointment &app) {
    //kDebug()<<this<<(m_node ? m_node->node()->name() : "no node")<<(app.node() ? app.node()->node()->name() : "no node");
    if ( app.isEmpty() ) {
	return;
    }
    if ( isEmpty() ) {
	setIntervals( app.intervals() );
	return;
    }
    QList<AppointmentInterval> result;
    QList<AppointmentInterval> lst1 = m_intervals.values();
    AppointmentInterval i1;
    QList<AppointmentInterval> lst2 = app.intervals().values();
    //kDebug()<<"add"<<lst1.count()<<" intervals to"<<lst2.count()<<" intervals";
    AppointmentInterval i2;
    int index1 = 0, index2 = 0;
    DateTime from;
    while (index1 < lst1.size() || index2 < lst2.size()) {
        if (index1 >= lst1.size()) {
            i2 = lst2[index2];
            if (!from.isValid() || from < i2.startTime())
                from = i2.startTime();
            result.append(AppointmentInterval(from, i2.endTime(), i2.load()));
            //kDebug()<<"Interval+ (i2):"<<from<<" -"<<i2.endTime();
            from = i2.endTime();
            ++index2;
            continue;
        }
        if (index2 >= lst2.size()) {
            i1 = lst1[index1];
            if (!from.isValid() || from < i1.startTime())
                from = i1.startTime();
            result.append(AppointmentInterval(from, i1.endTime(), i1.load()));
            //kDebug()<<"Interval+ (i1):"<<from<<" -"<<i1.endTime();
            from = i1.endTime();
            ++index1;
            continue;
        }
        i1 = lst1[index1];
        i2 = lst2[index2];
        AppointmentInterval i =  i1.firstInterval(i2, from);
        if (!i.isValid()) {
            break;
        }
        result.append(AppointmentInterval(i)); 
        from = i.endTime();
        //kDebug()<<"Interval+ (i):"<<i.startTime()<<" -"<<i.endTime()<<" load="<<i.load();
        if (i.endTime() >= i1.endTime()) {
            ++index1;
        }
        if (i.endTime() >= i2.endTime()) {
            ++index2;
        }
    }
    m_intervals.clear();
    foreach ( const AppointmentInterval &i, result ) {
        m_intervals.inSort( i );
    }
    //kDebug()<<this<<":"<<m_intervals.count();
    return;
}

#ifndef NDEBUG
void Appointment::printDebug(const QString& _indent)
{
    QString indent = _indent;
    //kDebug()<<indent<<"  + Appointment:"<<this;
    bool err = false;
    if (m_node == 0) {
        //kDebug()<<indent<<"   No node schedule";
        err = true;
    } else if (m_node->node() == 0) {
        //kDebug()<<indent<<"   No node";
        err = true;
    }
    if (m_resource == 0) {
        //kDebug()<<indent<<"   No resource schedule";
        err = true;
    } else if (m_resource->resource() == 0) {
        //kDebug()<<indent<<"   No resource";
        err = true;
    }
    if (!err) {
        kDebug()<<indent<<"  + Appointment to schedule:"<<m_node->name()<<" ("<<m_node->type()<<"):"<<" task="<<m_node->node()->name()<<", resource="<<m_resource->resource()->name();
    } else {
        kDebug()<<indent<<"  +"<<m_intervals.count()<<" appointment intervals:";
    }
    indent += "  + ";
    foreach (const AppointmentInterval &i, m_intervals ) {
        kDebug()<<indent<<"----"<<i.startTime().toString()<<" -"<<i.endTime().toString()<<" load="<<i.load();
    }
}
#endif

}  //KPlato namespace
