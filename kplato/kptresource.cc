/* This file is part of the KDE project
   Copyright (C) 2001 Thomas zander <zander@kde.org>
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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

#include "kptresource.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptdatetime.h"
#include "kptcalendar.h"

#include <kdebug.h>

namespace KPlato
{

KPTResourceGroup::KPTResourceGroup(KPTProject *project) {
    m_project = project;
    m_type = Type_Work;
    m_resources.setAutoDelete(true);
    generateId();
}

KPTResourceGroup::~KPTResourceGroup() {
    removeId();
}

bool KPTResourceGroup::setId(QString id) {
    //kdDebug()<<k_funcinfo<<id<<endl;
    if (id.isEmpty()) {
        kdError()<<k_funcinfo<<"id is empty"<<endl;
        m_id = id;
        return false;
    }
    KPTResourceGroup *g = findId();
    if (g == this) {
        //kdDebug()<<k_funcinfo<<"My id found, remove it"<<endl;
        removeId();
    } else if (g) {
        //Hmmm, shouldn't happen
        kdError()<<k_funcinfo<<"My id '"<<m_id<<"' already used for different group: "<<g->name()<<endl;
    }
    if (findId(id)) {
        kdError()<<k_funcinfo<<"id '"<<id<<"' is already used for different group: "<<findId(id)->name()<<endl;
        m_id = QString(); // hmmm
        return false;
    }
    m_id = id;
    insertId(id);
    //kdDebug()<<k_funcinfo<<m_name<<": inserted id="<<id<<endl;
    return true;
}

void KPTResourceGroup::generateId() {
    if (!m_id.isEmpty()) {
        removeId();
    }
    for (int i=0; i<32000 ; ++i) {
        m_id = m_id.setNum(i);
        if (!findId()) {
            insertId(m_id);
            return;
        }
    }
    m_id = QString();
}

void KPTResourceGroup::addResource(KPTResource* resource, KPTRisk*) {
    m_resources.append(resource);
}

KPTResource* KPTResourceGroup::getResource(int) {
    return 0L;
}

KPTRisk* KPTResourceGroup::getRisk(int) {
    return 0L;
}

void KPTResourceGroup::removeResource(KPTResource *resource) {
    m_resources.removeRef(resource);
}

KPTResource *KPTResourceGroup::takeResource(KPTResource *resource) {
    return m_resources.take(m_resources.findRef(resource));
}

void KPTResourceGroup::removeResource(int) {
}

void KPTResourceGroup::addRequiredResource(KPTResourceGroup*) {
}

KPTResourceGroup* KPTResourceGroup::getRequiredResource(int) {
    return 0L;
}

void KPTResourceGroup::removeRequiredResource(int) {
}

bool KPTResourceGroup::load(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    setId(element.attribute("id"));
    m_name = element.attribute("name");

    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
    	if (list.item(i).isElement()) {
	        QDomElement e = list.item(i).toElement();
    	    if (e.tagName() == "resource") {
	    	    // Load the resource
		        KPTResource *child = new KPTResource(m_project);
    		    if (child->load(e))
	    	        addResource(child, 0);
		        else
		            // TODO: Complain about this
    		        delete child;
            }
        }
    }
    return true;
}

void KPTResourceGroup::save(QDomElement &element)  {
    //kdDebug()<<k_funcinfo<<endl;

    QDomElement me = element.ownerDocument().createElement("resource-group");
    element.appendChild(me);

    me.setAttribute("id", m_id);
    me.setAttribute("name", m_name);

    QPtrListIterator<KPTResource> it(m_resources);
    for ( ; it.current(); ++it ) {
        it.current()->save(me);
    }
}

void KPTResourceGroup::saveAppointments(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<KPTResource> it(m_resources);
    for ( ; it.current(); ++it ) {
        it.current()->saveAppointments(element);
    }
}

void KPTResourceGroup::clearAppointments() {
    QPtrListIterator<KPTResource> it(m_resources);
    for (; it.current(); ++it) {
        it.current()->clearAppointments();
    }
    clearNodes();
}

int KPTResourceGroup::units() {
    int u = 0;
    QPtrListIterator<KPTResource> it = m_resources;
    for (; it.current(); ++it) {
        u += it.current()->units();
    }
    return u;
}

KPTResourceGroup *KPTResourceGroup::findId(const QString &id) const {
    return m_project ? m_project->findResourceGroup(id) : 0;
}

bool KPTResourceGroup::removeId(const QString &id) { 
    return m_project ? m_project->removeResourceGroupId(id): false;
}

void KPTResourceGroup::insertId(const QString &id) { 
    if (m_project)
        m_project->insertResourceGroupId(id, this);
}

KPTAppointment KPTResourceGroup::appointmentIntervals() const {
    KPTAppointment a;
    QPtrListIterator<KPTResource> it = m_resources;
    for (; it.current(); ++it) {
        a += it.current()->appointmentIntervals();
    }
    return a;
}

KPTResource::KPTResource(KPTProject *project) : m_project(project), m_appointments(), m_workingHours(), m_overbooked(false) {
    m_type = Type_Work;
    m_units = 100; // %

    cost.normalRate = 100;
    cost.overtimeRate = 200;
    cost.fixed = 0;
    m_calendar = 0;

    generateId();
}

KPTResource::~KPTResource() {
    removeId();
    QPtrListIterator<KPTResourceRequest> it = m_requests;
    for (; it.current(); ++it) {
        it.current()->setResource(0); // avoid the request to mess with my list
        it.current()->parent()->removeResourceRequest(it.current()); // deletes the request
    }
    clearAppointments();
}

bool KPTResource::setId(QString id) {
    //kdDebug()<<k_funcinfo<<id<<endl;
    if (id.isEmpty()) {
        kdError()<<k_funcinfo<<"id is empty"<<endl;
        m_id = id;
        return false;
    }
    KPTResource *r = findId();
    if (r == this) {
        //kdDebug()<<k_funcinfo<<"My id found, remove it"<<endl;
        removeId();
    } else if (r) {
        //Hmmm, shouldn't happen
        kdError()<<k_funcinfo<<"My id '"<<m_id<<"' already used for different resource: "<<r->name()<<endl;
    }
    if (findId(id)) {
        kdError()<<k_funcinfo<<"id '"<<id<<"' is already used for different resource: "<<findId(id)->name()<<endl;
        m_id = QString(); // hmmm
        return false;
    }
    m_id = id;
    insertId(id);
    //kdDebug()<<k_funcinfo<<m_name<<": inserted id="<<id<<endl;
    return true;
}

void KPTResource::generateId() {
    if (!m_id.isEmpty()) {
        removeId();
    }
    for (int i=0; i<32000 ; ++i) {
        m_id = m_id.setNum(i);
        if (!findId()) {
            insertId(m_id);
            return;
        }
    }
    m_id = QString();
}

void KPTResource::setType(const QString &type) {
    if (type == "Work")
        m_type = Type_Work;
    else if (type == "Material")
        m_type = Type_Material;
}

QString KPTResource::typeToString() const {
    if (m_type == Type_Work)
        return QString("Work");
    else if (m_type == Type_Material)
        return QString("Material");

    return QString();
}

void KPTResource::copy(KPTResource *resource) {
    m_project = resource->project();
    //m_appointments = resource->appointments(); // Note
    m_id = resource->id();
    m_name = resource->name();
    m_initials = resource->initials();
    m_email = resource->email();
    m_availableFrom = resource->availableFrom();
    m_availableUntil = resource->availableUntil();
    m_workingHours.clear();
    m_workingHours = resource->workingHours();

    m_units = resource->units(); // available units in percent
    m_overbooked = resource->isOverbooked();

    m_type = resource->type();

    cost.normalRate = resource->normalRate();
    cost.overtimeRate = resource->overtimeRate();
    cost.fixed = resource->fixedCost();
    
    m_calendar = resource->m_calendar;
}

void KPTResource::addWorkingHour(QTime from, QTime until) {
    //kdDebug()<<k_funcinfo<<endl;
    m_workingHours.append(new QTime(from));
    m_workingHours.append(new QTime(until));
}

KPTCalendar *KPTResource::calendar() const {
    if (!m_calendar && project())
        return project()->defaultCalendar();
    return m_calendar;
}

KPTDateTime *KPTResource::getFirstAvailableTime(KPTDateTime /*after*/) {
    return 0L;
}

KPTDateTime *KPTResource::getBestAvailableTime(KPTDuration /*duration*/) {
    return 0L;
}

KPTDateTime *KPTResource::getBestAvailableTime(const KPTDateTime after, const KPTDuration duration) {
    return 0L;
}

bool KPTResource::load(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    setId(element.attribute("id"));
    m_name = element.attribute("name");
    m_initials = element.attribute("initials");
    m_email = element.attribute("email");
    setType(element.attribute("type"));
    m_calendar = findCalendar(element.attribute("calendar-id"));
    return true;
}

void KPTResource::save(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    QDomElement me = element.ownerDocument().createElement("resource");
    element.appendChild(me);

    if (m_calendar)
        me.setAttribute("calendar-id", m_calendar->id());
    me.setAttribute("id", m_id);
    me.setAttribute("name", m_name);
    me.setAttribute("initials", m_initials);
    me.setAttribute("email", m_email);
    me.setAttribute("type", typeToString());
}

bool KPTResource::isAvailable(KPTTask *task) {
    bool busy = false;
    QPtrListIterator<KPTAppointment> it(m_appointments);
    for (; it.current(); ++it) {
        if (it.current()->isBusy(task->startTime(), task->endTime())) {
            busy = true;
            break;
        }
    }
    return !busy;
}

KPTAppointment *KPTResource::findAppointment(KPTNode *node) {
    QPtrListIterator<KPTAppointment> it = m_appointments;
    for (; it.current(); ++it) {
        if (it.current()->node() == node)
            return it.current();
    }
    return 0;
}

bool KPTResource::addAppointment(KPTAppointment *appointment) {
    if (m_appointments.findRef(appointment) != -1) {
        kdError()<<k_funcinfo<<"Appointment allready exists"<<endl;
        return false;
    }
    m_appointments.append(appointment);
    return true;
        
}

void KPTResource::addAppointment(KPTNode *node, KPTDateTime &start, KPTDateTime &end, double load) {
    KPTAppointment *a = findAppointment(node);
    if (a != 0) {
        a->addInterval(start, end, load);
        return;
    }
    a = new KPTAppointment(this, node, start, end, load);
    if (node->addAppointment(a)) {
        m_appointments.append(a);
    } else {
        delete a;
    }
}


void KPTResource::removeAppointment(KPTAppointment *appointment) {
    takeAppointment(appointment);
    delete appointment;
}

void KPTResource::takeAppointment(KPTAppointment *appointment) {
    int i = m_appointments.findRef(appointment);
    if (i != -1) {
        m_appointments.take(i);
        if (appointment->node())
            appointment->node()->takeAppointment(appointment);
    }
}



void KPTResource::clearAppointments() {
    m_overbooked = false;
    KPTAppointment *a;
    while ((a = m_appointments.getFirst()))
        delete a;
}

void KPTResource::makeAppointment(KPTDateTime &start, KPTDuration &duration, KPTTask *task) {
    //kdDebug()<<k_funcinfo<<task->name()<<": "<<start.toString()<<" dur "<<duration.toString()<<endl;
    KPTCalendar *cal = calendar();
    if (!cal) {
        kdWarning()<<k_funcinfo<<m_name<<": No calendar defined"<<endl;
        return; 
    }
    //TODO: units and moderated by availability, and standard non-working days
    KPTDateTime time = start;
    KPTDateTime end = start+duration;
    while (time < end) {
        //kdDebug()<<k_funcinfo<<time.toString()<<" to "<<end.toString()<<endl;
        if (!cal->hasInterval(time, end)) {
            //kdDebug()<<time.toString()<<" to "<<end.toString()<<": No (more) interval(s)"<<endl;
            return; // nothing more to do
        }
        QPair<KPTDateTime, KPTDateTime> i = cal->interval(time, end);
        if (time == i.second)
            return; // hmmm, didn't get a new interval, avoid loop
        addAppointment(task, i.first, i.second);
        time = i.second;
    }
}

void KPTResource::saveAppointments(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<KPTAppointment> it(m_appointments);
    for ( ; it.current(); ++it ) {
        it.current()->saveXML(element);
    }
}

// the amount of effort we can do within the duration
KPTDuration KPTResource::effort(const KPTDateTime &start, const KPTDuration &duration, bool *ok) const {
    //kdDebug()<<k_funcinfo<<m_name<<": "<<start.date().toString()<<" for duration "<<duration.toString(KPTDuration::Format_Day)<<endl;
    bool sts=false;
    KPTDuration e;
    if ((!m_availableFrom.isValid() || start+duration >= m_availableFrom) && 
        (!m_availableUntil.isValid() || start < m_availableUntil)) 
    {
        KPTCalendar *cal = calendar();
        if (cal && cal->hasIntervalAfter(start)) {
            sts = true;
            e = (cal->effort(start, duration) * m_units)/100;
        }
    }
    //kdDebug()<<k_funcinfo<<"e="<<e.toString(KPTDuration::Format_Day)<<" ("<<m_units<<")"<<endl;
    if (ok) *ok = sts;
    return e;
}

KPTDateTime KPTResource::availableAfter(const KPTDateTime &time) {
    KPTDateTime t;
    KPTCalendar *cal = calendar();
    if (cal)
        t = cal->availableAfter(time);
    //kdDebug()<<k_funcinfo<<time.toString()<<"="<<t.toString()<<" "<<m_name<<endl;
    return t;
}

KPTDateTime KPTResource::availableBefore(const KPTDateTime &time) {
    KPTDateTime t;
    KPTCalendar *cal = calendar();
    if (cal)
        t = cal->availableBefore(time);
    return t;
}

KPTResource *KPTResource::findId(const QString &id) const { 
    return m_project ? m_project->findResource(id) : 0; 
}

bool KPTResource::removeId(const QString &id) { 
    return m_project ? m_project->removeResourceId(id) : false; 
}

void KPTResource::insertId(const QString &id) { 
    if (m_project)
        m_project->insertResourceId(id, this); 
}

KPTCalendar *KPTResource::findCalendar(const QString &id) const { 
    return (m_project ? m_project->findCalendar(id) : 0); 
}

bool KPTResource::isOverbooked() const {
    return isOverbooked(KPTDateTime(), KPTDateTime());
}

bool KPTResource::isOverbooked(const QDate &date) const {
    return isOverbooked(KPTDateTime(date), KPTDateTime(date.addDays(1)));
}

bool KPTResource::isOverbooked(const KPTDateTime &start, const KPTDateTime &end) const {
    //kdDebug()<<k_funcinfo<<start.toString()<<" - "<<end.toString()<<endl;
    KPTAppointment a = appointmentIntervals();
    QPtrListIterator<KPTAppointmentInterval> it = a.intervals();
    for (; it.current(); ++it) {
        if ((!end.isValid() || it.current()->startTime() < end) && 
            (!start.isValid() || it.current()->endTime() > start)) 
        {
            if (it.current()->load() > m_units) {
                //kdDebug()<<k_funcinfo<<m_name<<" overbooked"<<endl;
                return true;
            }
        }
        if (it.current()->startTime() >= end)
            break;
    }
    //kdDebug()<<k_funcinfo<<m_name<<" not overbooked"<<endl;
    return false; 
}

KPTAppointment KPTResource::appointmentIntervals() const {
    KPTAppointment a;
    QPtrListIterator<KPTAppointment> it = m_appointments;
    for (; it.current(); ++it) {
        a += *(it.current());
    }
    return a;
}

KPTDuration KPTResource::plannedEffort(const QDate &date) const {
    KPTDuration e;
    QPtrListIterator<KPTAppointment> it = m_appointments;
    for (; it.current(); ++it) {
        e += it.current()->plannedEffort(date);
    }
    return e;
}

//////

KPTAppointmentInterval::KPTAppointmentInterval() {
    m_load = 100.0; 
}
KPTAppointmentInterval::KPTAppointmentInterval(const KPTAppointmentInterval &interval) {
    //kdDebug()<<k_funcinfo<<endl;
    m_start = interval.startTime(); 
    m_end = interval.endTime(); 
    m_load = interval.load(); 
}
KPTAppointmentInterval::KPTAppointmentInterval(const KPTDateTime &start, const KPTDateTime end, double load) {
    //kdDebug()<<k_funcinfo<<endl;
    m_start = start; 
    m_end = end; 
    m_load = load; 
}
KPTAppointmentInterval::~KPTAppointmentInterval() {
    //kdDebug()<<k_funcinfo<<endl;
}

KPTDuration KPTAppointmentInterval::effort(const KPTDateTime &start, const KPTDateTime end) const {
    if (start >= m_end || end <= m_start) {
        return KPTDuration::zeroDuration;
    }
    KPTDateTime s = (start > m_start ? start : m_start);
    KPTDateTime e = (end < m_end ? end : m_end);
    return (e - s) * m_load / 100;
}

KPTDuration KPTAppointmentInterval::effort(const KPTDateTime &time, bool upto) const {
    if (upto) {
        if (time <= m_start) {
            return KPTDuration::zeroDuration;
        }
        KPTDateTime e = (time < m_end ? time : m_end);
        return (e - m_start) * m_load / 100;
    }
    // from time till end
    if (time >= m_end) {
        return KPTDuration::zeroDuration;
    }
    KPTDateTime s = (time > m_start ? time : m_start);
    return (m_end - s) * m_load / 100;
}

bool KPTAppointmentInterval::loadXML(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    bool ok;
    m_start = KPTDateTime::fromString(element.attribute("start"));
    m_end = KPTDateTime::fromString(element.attribute("end"));
    m_load = element.attribute("load", "100").toDouble(&ok);
    if (!ok) m_load = 100;
    return m_start.isValid() && m_end.isValid();
}

void KPTAppointmentInterval::saveXML(QDomElement &element) {
    QDomElement me = element.ownerDocument().createElement("interval");
    element.appendChild(me);

    me.setAttribute("start", m_start.toString());
    me.setAttribute("end", m_end.toString());
    me.setAttribute("load", m_load);
}

bool KPTAppointmentInterval::isValid() const {
    return m_start.isValid() && m_end.isValid();
}

KPTAppointmentInterval KPTAppointmentInterval::firstInterval(const KPTAppointmentInterval &interval, const KPTDateTime &from) const {
    //kdDebug()<<k_funcinfo<<interval.startTime().toString()<<" - "<<interval.endTime().toString()<<" from="<<from.toString()<<endl;
    KPTDateTime f = from;
    KPTDateTime s1 = m_start;
    KPTDateTime e1 = m_end;
    KPTDateTime s2 = interval.startTime();
    KPTDateTime e2 = interval.endTime();
    KPTAppointmentInterval a;
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

KPTAppointment::UsedEffortItem::UsedEffortItem(QDate date, KPTDuration effort, bool overtime) {
    m_date = date;
    m_effort = effort;
    m_overtime = overtime;
}
QDate KPTAppointment::UsedEffortItem::date() { 
    return m_date; 
}
KPTDuration KPTAppointment::UsedEffortItem::effort() {
    return m_effort; 
}
bool KPTAppointment::UsedEffortItem::isOvertime() { 
    return m_overtime; 
}

KPTAppointment::UsedEffort::UsedEffort() { 
    setAutoDelete(true); 
}

void KPTAppointment::UsedEffort::inSort(QDate date, KPTDuration effort, bool overtime) {
    UsedEffortItem *item = new UsedEffortItem(date, effort, overtime);
    QPtrList<UsedEffortItem>::inSort(item);
}

KPTDuration KPTAppointment::UsedEffort::usedEffort(bool includeOvertime) const {
    KPTDuration eff;
    QPtrListIterator<UsedEffortItem> it(*this);
    for (; it.current(); ++it) {
        if (includeOvertime || !it.current()->isOvertime()) {
            eff += it.current()->effort();
        }
    }
    return eff;
}
        
KPTDuration KPTAppointment::UsedEffort::usedEffort(const QDate &date, bool includeOvertime) const {
    KPTDuration eff;
    QPtrListIterator<UsedEffortItem> it(*this);
    for (; it.current(); ++it) {
        if ((includeOvertime || !it.current()->isOvertime()) && 
            it.current()->date() == date) {
            eff += it.current()->effort();
        }
    }
    return eff;
}

KPTDuration KPTAppointment::UsedEffort::usedEffortTo(const QDate &date, bool includeOvertime) const {
    KPTDuration eff;
    QPtrListIterator<UsedEffortItem> it(*this);
    for (; it.current(); ++it) {
        if ((includeOvertime || !it.current()->isOvertime()) && 
            it.current()->date() <= date) {
            eff += it.current()->effort();
        }
    }
    return eff;
}

KPTDuration KPTAppointment::UsedEffort::usedOvertime() const {
    UsedEffortItem *item = getFirst();
    return item==0 ? KPTDuration::zeroDuration : usedOvertime(item->date());
}

KPTDuration KPTAppointment::UsedEffort::usedOvertime(const QDate &date) const {
    KPTDuration eff;
    QPtrListIterator<UsedEffortItem> it(*this);
    for (; it.current(); ++it) {
        if (it.current()->isOvertime() && it.current()->date() == date) {
            eff += it.current()->effort();
        }
    }
    return eff;
}

KPTDuration KPTAppointment::UsedEffort::usedOvertimeTo(const QDate &date) const {
    KPTDuration eff;
    QPtrListIterator<UsedEffortItem> it(*this);
    for (; it.current(); ++it) {
        if (it.current()->isOvertime() && it.current()->date() <= date) {
            eff += it.current()->effort();
        }
    }
    return eff;
}

bool KPTAppointment::UsedEffort::load(QDomElement &element) {
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "actual-effort") {
                QDate date = QDate::fromString(e.attribute("date"), Qt::ISODate);
                KPTDuration eff = KPTDuration::fromString(e.attribute("effort"));
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

void KPTAppointment::UsedEffort::save(QDomElement &element) {
    if (isEmpty()) return;
    for (UsedEffortItem *item = first(); item; item = next()) {
        QDomElement me = element.ownerDocument().createElement("actual-effort");
        element.appendChild(me);
        me.setAttribute("date",item->date().toString(Qt::ISODate));
        me.setAttribute("effort",item->effort().toString());
        me.setAttribute("overtime",item->isOvertime());
    }
}

int KPTAppointment::UsedEffort::compareItems(QPtrCollection::Item item1, QPtrCollection::Item item2) {
    QDate d1 = static_cast<UsedEffortItem*>(item1)->date();
    QDate d2 = static_cast<UsedEffortItem*>(item2)->date();
    if (d1 > d2) return 1;
    if (d1 < d2) return -1;
    return 0;
}

////
KPTAppointment::KPTAppointment() 
    : m_extraRepeats(), m_skipRepeats() {
    m_resource=0;
    m_node=0;
    m_repeatInterval=KPTDuration();
    m_repeatCount=0;

    m_intervals.setAutoDelete(true);
}

KPTAppointment::KPTAppointment(KPTResource *resource, KPTNode *node, KPTDateTime start, KPTDateTime end, double load) 
    : m_extraRepeats(), m_skipRepeats() {
    
    m_node = node;
    m_resource = resource;
    m_repeatInterval = KPTDuration();
    m_repeatCount = 0;

    addInterval(start, end, load);

    m_intervals.setAutoDelete(true);
}

KPTAppointment::KPTAppointment(KPTResource *resource, KPTNode *node, KPTDateTime start, KPTDuration duration, double load) 
    : m_extraRepeats(), m_skipRepeats() {
    
    m_node = node;
    m_resource = resource;
    m_repeatInterval = KPTDuration();
    m_repeatCount = 0;

    addInterval(start, duration, load);
    
    m_intervals.setAutoDelete(true);
}

KPTAppointment::~KPTAppointment() {
    detach();
}

void KPTAppointment::addInterval(KPTAppointmentInterval *a) {
    //kdDebug()<<k_funcinfo<<m_resource->name()<<" to "<<m_node->name()<<endl;
    m_intervals.inSort(a);
}
void KPTAppointment::addInterval(const KPTDateTime &start, const KPTDateTime &end, double load) {
    addInterval(new KPTAppointmentInterval(start, end, load));
}
void KPTAppointment::addInterval(const KPTDateTime &start, const KPTDuration &duration, double load) {
    KPTDateTime e = start+duration;
    addInterval(start, e, load);
}

double KPTAppointment::maxLoad() const {
    double v = 0.0;
    QPtrListIterator<KPTAppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        if (v < it.current()->load())
            v = it.current()->load();
    }
    return v;
}

KPTDateTime KPTAppointment::startTime() const {
    KPTDateTime t;
    QPtrListIterator<KPTAppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        if (!t.isValid() || t > it.current()->startTime())
            t = it.current()->startTime();
    }
    return t;
}

KPTDateTime KPTAppointment::endTime() const {
    KPTDateTime t;
    QPtrListIterator<KPTAppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        if (!t.isValid() || t < it.current()->endTime())
            t = it.current()->endTime();
    }
    return t;
}

void KPTAppointment::deleteAppointmentFromRepeatList(KPTDateTime time) {
}

void KPTAppointment::addAppointmentToRepeatList(KPTDateTime time) {
}

bool KPTAppointment::isBusy(const KPTDateTime &start, const KPTDateTime &end) {
    return false;
}

bool KPTAppointment::loadXML(QDomElement &element, KPTProject &project) {
    //kdDebug()<<k_funcinfo<<endl;
    m_resource = project.resource(element.attribute("resource-id"));
    if (m_resource == 0) {
        kdError()<<k_funcinfo<<"The referenced resource does not exists: resource id="<<element.attribute("resource-id")<<endl;
        return false;
    }
    m_node = project.findNode(element.attribute("task-id"));
    if (m_node == 0) {
        kdError()<<k_funcinfo<<"The referenced task does not exists: "<<element.attribute("task-id")<<endl;
        return false;
    }

    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "interval") {
            KPTAppointmentInterval *a = new KPTAppointmentInterval();
                if (a->loadXML(e)) {
                    addInterval(a);
                } else {
                    kdError()<<k_funcinfo<<"Could not load interval"<<endl;
                    delete a;
                }
            }
        }
    }
    m_actualEffort.load(element);
    if (m_intervals.isEmpty()) {
        return false; 
    }
    return attach();
}

void KPTAppointment::saveXML(QDomElement &element) {
    QDomElement me = element.ownerDocument().createElement("appointment");
    element.appendChild(me);

    if (m_intervals.isEmpty()) {
        return; // shouldn't happen
    }
    me.setAttribute("resource-id", m_resource->id());
    me.setAttribute("task-id", m_node->id());
    QPtrListIterator<KPTAppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        it.current()->saveXML(me);
    }
    m_actualEffort.save(me);
}

// Returns the total actual effort for this appointment
KPTDuration KPTAppointment::plannedEffort() const {
    KPTDuration d;
    QPtrListIterator<KPTAppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        d += it.current()->effort();
    }
    return d;
}

// Returns the planned effort on the date
KPTDuration KPTAppointment::plannedEffort(const QDate &date) const {
    KPTDuration d;
    KPTDateTime s(date);
    KPTDateTime e(date.addDays(1));
    QPtrListIterator<KPTAppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        d += it.current()->effort(s, e);
    }
    return d;
}

// Returns the planned effort upto and including the date
KPTDuration KPTAppointment::plannedEffortTo(const QDate& date) const {
    KPTDuration d;
    KPTDateTime e(date.addDays(1));
    QPtrListIterator<KPTAppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        d += it.current()->effort(e, true);
    }
    return d;
}


// Returns the total actual effort for this appointment
KPTDuration KPTAppointment::actualEffort() const {
    return m_actualEffort.usedEffort();
}

// Returns the actual effort on the date
KPTDuration KPTAppointment::actualEffort(const QDate &date) const {
    return m_actualEffort.usedEffort(date);
}

// Returns the actual effort upto and including date
KPTDuration KPTAppointment::actualEffortTo(const QDate &date) const {
    return m_actualEffort.usedEffortTo(date);
}

double KPTAppointment::plannedCost() {
    return plannedEffort().toDouble(KPTDuration::Unit_h) * m_resource->normalRate(); //FIXME overtime
}

//Calculates the planned cost on date
double KPTAppointment::plannedCost(const QDate &date) {
    return plannedEffort(date).toDouble(KPTDuration::Unit_h) * m_resource->normalRate(); //FIXME overtime
}

//Calculates the planned cost upto and including date
double KPTAppointment::plannedCostTo(const QDate &date) {
    return plannedEffortTo(date).toDouble(KPTDuration::Unit_h) * m_resource->normalRate(); //FIXME overtime
}

// Calculates the total actual cost for this appointment
double KPTAppointment::actualCost() {
    //kdDebug()<<k_funcinfo<<m_actualEffort.usedEffort(false /*ex. overtime*/).toDouble(KPTDuration::Unit_h)<<endl;
    
    return (m_actualEffort.usedEffort(false /*ex. overtime*/).toDouble(KPTDuration::Unit_h)*m_resource->normalRate()) + (m_actualEffort.usedOvertime().toDouble(KPTDuration::Unit_h)*m_resource->overtimeRate());
}

// Calculates the actual cost on date
double KPTAppointment::actualCost(const QDate &date) {
    return (m_actualEffort.usedEffort(date, false /*ex. overtime*/).toDouble(KPTDuration::Unit_h)*m_resource->normalRate()) + (m_actualEffort.usedOvertime(date).toDouble(KPTDuration::Unit_h)*m_resource->overtimeRate());
}

// Calculates the actual cost upto and including date
double KPTAppointment::actualCostTo(const QDate &date) {
    return (m_actualEffort.usedEffortTo(date, false /*ex. overtime*/).toDouble(KPTDuration::Unit_h)*m_resource->normalRate()) + (m_actualEffort.usedOvertimeTo(date).toDouble(KPTDuration::Unit_h)*m_resource->overtimeRate());
}

void KPTAppointment::addActualEffort(QDate date, KPTDuration effort, bool overtime) {
    m_actualEffort.inSort(date, effort, overtime);
}

bool KPTAppointment::attach() { 
    if (m_resource && m_node) {
        m_resource->addAppointment(this);
        m_node->addAppointment(this);
        return true;
    }
    kdWarning()<<k_funcinfo<<"Failed: "<<(m_resource ? "" : "resource=0 ") 
                                       <<(m_node ? "" : "node=0")<<endl;
    return false;
}

void KPTAppointment::detach() {
    if (m_resource) {
        m_resource->takeAppointment(this); // takes from node also
    }
    if (m_node) {
        m_node->takeAppointment(this); // to make it robust
    }
}

// Returns the effort from start to end
KPTDuration KPTAppointment::effort(const KPTDateTime &start, const KPTDateTime &end) const {
    KPTDuration d;
    QPtrListIterator<KPTAppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        d += it.current()->effort(start, end);
    }
    return d;
}
// Returns the effort from start for the duration
KPTDuration KPTAppointment::effort(const KPTDateTime &start, const KPTDuration &duration) const {
    KPTDuration d;
    QPtrListIterator<KPTAppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        d += it.current()->effort(start, start+duration);
    }
    return d;
}
// Returns the effort upto time / from time 
KPTDuration KPTAppointment::effortFrom(const KPTDateTime &time) const {
    KPTDuration d;
    QPtrListIterator<KPTAppointmentInterval> it = m_intervals;
    for (; it.current(); ++it) {
        d += it.current()->effort(time, false);
    }
    return d;
}

KPTAppointment &KPTAppointment::operator=(const KPTAppointment &app) {
    m_resource = app.resource();
    m_node = app.node();
    m_repeatInterval = app.repeatInterval();
    m_repeatCount = app.repeatCount();

    m_intervals.clear();
    QPtrListIterator<KPTAppointmentInterval> it = app.intervals();
    for (; it.current(); ++it) {
        addInterval(new KPTAppointmentInterval(*(it.current())));
    }
    return *this;
}

KPTAppointment &KPTAppointment::operator+=(const KPTAppointment &app) {
    *this = *this + app;
    return *this;
}

KPTAppointment KPTAppointment::operator+(const KPTAppointment &app) {
    KPTAppointment a;
    KPTAppointmentIntervalList ai = app.intervals();
    KPTAppointmentInterval i;
    KPTAppointmentInterval *i1 = m_intervals.first();
    KPTAppointmentInterval *i2 = ai.first();
    KPTDateTime from;
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

/////////   KPTRisk   /////////
KPTRisk::KPTRisk(KPTNode *n, KPTResource *r, RiskType rt) {
    m_node=n;
    m_resource=r;
    m_riskType=rt;
}

KPTRisk::~KPTRisk() {
}

KPTResourceRequest::KPTResourceRequest(KPTResource *resource, int units)
    : m_resource(resource),
      m_units(units),
      m_parent(0) {
    //kdDebug()<<k_funcinfo<<"Request to: "<<(resource ? resource->name() : QString("None"))<<endl;
}

KPTResourceRequest::~KPTResourceRequest() {
    //kdDebug()<<k_funcinfo<<"resource: "<<m_resource->name()<<endl;
    if (m_resource)
        m_resource->unregisterRequest(this);
    m_resource = 0;
}

bool KPTResourceRequest::load(QDomElement &element, KPTProject *project) {
    //kdDebug()<<k_funcinfo<<endl;
    m_resource = project->resource(element.attribute("resource-id"));
    if (m_resource == 0) {
        kdDebug()<<k_funcinfo<<"The referenced resource does not exist: resource id="<<element.attribute("resource-id")<<endl;
        return false;
    }
    m_units  = element.attribute("units").toInt();
    return true;
}

void KPTResourceRequest::save(QDomElement &element) {
    QDomElement me = element.ownerDocument().createElement("resource-request");
    element.appendChild(me);
    me.setAttribute("resource-id", m_resource->id());
    me.setAttribute("units", m_units);
}

int KPTResourceRequest::units() const {
    //kdDebug()<<k_funcinfo<<m_resource->name()<<": units="<<m_units<<endl;
    return m_units;
}

int KPTResourceRequest::workUnits() const {
    if (m_resource->type() == KPTResource::Type_Work)
        return units();
        
    //kdDebug()<<k_funcinfo<<"units=0"<<endl;
    return 0;
}

/////////
KPTResourceGroupRequest::KPTResourceGroupRequest(KPTResourceGroup *group, int units)
    : m_group(group), m_units(units) {

    //kdDebug()<<k_funcinfo<<"Request to: "<<(group ? group->name() : QString("None"))<<endl;
    if (group)
        group->registerRequest(this);
    m_resourceRequests.setAutoDelete(true);
}

KPTResourceGroupRequest::~KPTResourceGroupRequest() {
    //kdDebug()<<k_funcinfo<<"Group: "<<m_group->name()<<endl;
    if (m_group)
        m_group->unregisterRequest(this);
    m_resourceRequests.clear();
}

void KPTResourceGroupRequest::addResourceRequest(KPTResourceRequest *request) {
    //kdDebug()<<k_funcinfo<<"Group: "<<m_group->name()<<endl;
    request->setParent(this);
    m_resourceRequests.append(request);
    request->registerRequest();
}

KPTResourceRequest *KPTResourceGroupRequest::takeResourceRequest(KPTResourceRequest *request) {
    if (request)
        request->unregisterRequest();
    return m_resourceRequests.take(m_resourceRequests.findRef(request)); 
}

KPTResourceRequest *KPTResourceGroupRequest::find(KPTResource *resource) const {
    QPtrListIterator<KPTResourceRequest> it(m_resourceRequests);
    for (; it.current(); ++it)
        if (it.current()->resource() == resource)
            return it.current();

    return 0;
}

bool KPTResourceGroupRequest::load(QDomElement &element, KPTProject *project) {
    //kdDebug()<<k_funcinfo<<endl;
    m_group = project->findResourceGroup(element.attribute("group-id"));
    if (m_group == 0) {
        kdDebug()<<k_funcinfo<<"The referenced resource group does not exist: group id="<<element.attribute("group-id")<<endl;
        return false;
    }
    m_group->registerRequest(this);
    
    m_units  = element.attribute("units").toInt();

    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
	    if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "resource-request") {
                KPTResourceRequest *r = new KPTResourceRequest();
                if (r->load(e, project))
                    addResourceRequest(r);
                else {
                    kdError()<<k_funcinfo<<"Failed to load resource request"<<endl;
                    delete r;
                }
            }
        }
    }
    return true;
}

void KPTResourceGroupRequest::save(QDomElement &element) {
    if (units() == 0)
        return;
    QDomElement me = element.ownerDocument().createElement("resourcegroup-request");
    element.appendChild(me);
    me.setAttribute("group-id", m_group->id());
    me.setAttribute("units", m_units);
    QPtrListIterator<KPTResourceRequest> it(m_resourceRequests);
    for (; it.current(); ++it)
        it.current()->save(me);
}

int KPTResourceGroupRequest::units() const {
    int units = m_units;
    QPtrListIterator<KPTResourceRequest> it = m_resourceRequests;
    for (; it.current(); ++it) {
        units += it.current()->units();
    }
    //kdDebug()<<k_funcinfo<<"units="<<units<<endl;
    return units;
}

int KPTResourceGroupRequest::workUnits() const {
    int units = 0;
    if (m_group->type() == KPTResourceGroup::Type_Work)
        units = m_units;
    QPtrListIterator<KPTResourceRequest> it = m_resourceRequests;
    for (; it.current(); ++it) {
        units += it.current()->workUnits();
    }
    //kdDebug()<<k_funcinfo<<"units="<<units<<endl;
    return units;
}

//TODO: handle nonspecific resources
KPTDuration KPTResourceGroupRequest::effort(const KPTDateTime &time, const KPTDuration &duration, bool *ok) const {
    KPTDuration e;
    bool sts=false;
    if (ok) *ok = sts;
    QPtrListIterator<KPTResourceRequest> it = m_resourceRequests;
    for (; it.current(); ++it) {
        e += it.current()->resource()->effort(time, duration, &sts);
        if (sts && ok) *ok = sts;
        //kdDebug()<<k_funcinfo<<it.current()->resource()->name()<<" e="<<e.toString()<<endl;
    }
    return e;
}

// FIXME: Handle 'any' duration, atm stops at a year.
KPTDuration KPTResourceGroupRequest::duration(const KPTDateTime &time, const KPTDuration &_effort, bool backward) {
    //kdDebug()<<k_funcinfo<<"--->"<<(backward?"(B) ":"(F) ")<<m_group->name()<<" "<<time.toString()<<": effort: "<<_effort.toString(KPTDuration::Format_Day)<<" ("<<_effort.milliseconds()<<")"<<endl;
    KPTDuration e;
    bool sts=false;
    bool match = false;
    KPTDateTime start = time;
    int inc = backward ? -1 : 1;
    KPTDateTime end = start;
    end.setTime(QTime());
    KPTDuration e1;
    for (int i=0; !match && i < 52; ++i) {
        // weeks
        end = end.addDays(inc*7);
        e1 = backward ? effort(end, start - end, &sts) 
                      : effort(start, end - start, &sts);
        if (e + e1 < _effort) {
            e += e1;
            if (!sts)
                break;
            start = end;
        } else if (e + e1 == _effort) {
            e += e1;
            match = true;
        } else {
            end = start;
            break;
        }
        //kdDebug()<<"duration(w)["<<i<<"]"<<(backward?"backward":"forward:")<<" date="<<start.date().toString()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")"<<endl;
    }
    //kdDebug()<<"duration "<<(backward?"backward":"forward: ")<<start.toString()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")  match="<<match<<" sts="<<sts<<endl;
    for (int i=0; !match && i < 7 && sts; ++i) {
        // days
        end = end.addDays(inc);
        e1 = backward ? effort(end, start - end, &sts) 
                      : effort(start, end - start, &sts);
        if (e + e1 < _effort) {
            e += e1;
            if (!sts)
                break;
            start = end;
        } else if (e + e1 == _effort) {
            e += e1;
            match = true;
        } else {
            end = start;
            break;
        }
        //kdDebug()<<"duration(d)["<<i<<"] "<<(backward?"backward":"forward:")<<"  date="<<start.date().toString()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")"<<endl;
    }
    //kdDebug()<<"duration "<<(backward?"backward ":"forward: ")<<start.toString()<<" - "<<end.toString()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")  match="<<match<<" sts="<<sts<<endl;
    for (int i=0; !match && i < 24 && sts; ++i) {
        // hours
        end = end.addSecs(inc*60*60);
        e1 = backward ? effort(end, start - end, &sts) 
                      : effort(start, end - start, &sts);
        if (e + e1 < _effort) {
            e += e1;
            if (!sts)
                break;
            start = end;
        } else if (e + e1 == _effort) {
            e += e1;
            match = true;
        } else {
            end = start;
            break;
        }
        //kdDebug()<<"duration(h)["<<i<<"]"<<(backward?"backward ":"forward:")<<" time="<<start.time().toString()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")"<<endl;
    }
    //kdDebug()<<"duration "<<(backward?"backward ":"forward: ")<<start.toString()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")  match="<<match<<" sts="<<sts<<endl;
    for (int i=0; !match && i < 60 && sts; ++i) {
        //minutes
        end = end.addSecs(inc*60);
        e1 = backward ? effort(end, start - end, &sts) 
                      : effort(start, end - start, &sts);
        if (e + e1 < _effort) {
            e += e1;
            if (!sts)
                break;
            start = end;
        } else if (e + e1 == _effort) {
            e += e1;
            match = true;
        } else if (e + e1 > _effort) {
            end = start;
            break;
        }
        //kdDebug()<<"duration(m) "<<(backward?"backward":"forward:")<<"  time="<<start.time().toString()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")"<<endl;
    }
    //kdDebug()<<"duration "<<(backward?"backward":"forward:")<<"  start="<<start.toString()<<" e="<<e.toString()<<" match="<<match<<" sts="<<sts<<endl;
    for (int i=0; !match && i < 60 && sts; ++i) {
        //seconds
        e1 = backward ? effort(end, start - end, &sts) 
                      : effort(start, end - start, &sts);
        if (e + e1 < _effort) {
            e += e1;
            if (!sts)
                break;
            start = end;
            end = end.addSecs(inc);
        } else if (e + e1 == _effort) {
            e += e1;
            match = true;
        } else if (e + e1 > _effort) {
            end = start;
            break;
        }
        //kdDebug()<<"duration(s)["<<i<<"]"<<(backward?"backward":"forward:")<<" time="<<start.time().toString()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")"<<endl;
    }
    for (int i=0; !match && i < 1000 && sts; ++i) {
        //milliseconds
        end.setTime(end.time().addMSecs(inc));
        e1 = backward ? effort(end, start - end, &sts) 
                      : effort(start, end - start, &sts);
        if (e + e1 < _effort) {
            e += e1;
            if (!sts)
                break;
            start = end;
        } else if (e + e1 == _effort) {
            e += e1;
            match = true;
        } else if (e + e1 > _effort) {
            end = start;
            break;
        }
        //kdDebug()<<"duration(ms)["<<i<<"]"<<(backward?"backward":"forward:")<<" time="<<start.time().toString()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")"<<endl;
    }
    if (!match) {
        kdError()<<k_funcinfo<<"Could not match effort."<<" Want: "<<_effort.toString(KPTDuration::Format_Day)<<" got: "<<e.toString(KPTDuration::Format_Day)<<" sts="<<sts<<endl;
    }
    end = backward ? availableAfter(end) : availableBefore(end);
    
    //kdDebug()<<k_funcinfo<<"<---"<<(backward?"(B) ":"(F) ")<<m_group->name()<<": "<<end.toString()<<"-"<<time.toString()<<"="<<(end - time).toString()<<" effort: "<<_effort.toString(KPTDuration::Format_Day)<<endl;
    return end - time;
}

KPTDateTime KPTResourceGroupRequest::availableAfter(const KPTDateTime &time) {
    KPTDateTime start;
    QPtrListIterator<KPTResourceRequest> it = m_resourceRequests;
    for (; it.current(); ++it) {
        KPTDateTime t = it.current()->resource()->availableAfter(time);
        if (t.isValid() && (!start.isValid() || t < start))
            start = t;
    }
    if (start.isValid() && start < time)
        start = time;
    //kdDebug()<<k_funcinfo<<time.toString()<<"="<<start.toString()<<" "<<m_group->name()<<endl;
    return start;
}

KPTDateTime KPTResourceGroupRequest::availableBefore(const KPTDateTime &time) {
    KPTDateTime end;
    QPtrListIterator<KPTResourceRequest> it = m_resourceRequests;
    for (; it.current(); ++it) {
        KPTDateTime t = it.current()->resource()->availableBefore(time);
        if (!end.isValid() || end > t)
            end = t;
    }
    if (!end.isValid() || end > time)
        end = time;
    return end;
}

void KPTResourceGroupRequest::makeAppointments(KPTTask *task) {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<KPTResourceRequest> it = m_resourceRequests;
    for (; it.current(); ++it) {
        it.current()->makeAppointment(m_start, m_duration, task);
    }
}

void KPTResourceGroupRequest::reserve(const KPTDateTime &start, const KPTDuration &duration) {
    m_start = start;
    m_duration = duration;
}

bool KPTResourceGroupRequest::isEmpty() const {
    return m_resourceRequests.isEmpty() && m_units == 0;
}

/////////
KPTResourceRequestCollection::KPTResourceRequestCollection(KPTTask &task)
    : m_task(task) {
    m_requests.setAutoDelete(true);
}

KPTResourceRequestCollection::~KPTResourceRequestCollection() {
    //kdDebug()<<k_funcinfo<<"Group: "<<m_group->name()<<endl;
    m_requests.clear();
}

KPTResourceGroupRequest *KPTResourceRequestCollection::find(KPTResourceGroup *group) const {
    QPtrListIterator<KPTResourceGroupRequest> it(m_requests);
    for (; it.current(); ++it) {
        if (it.current()->group() == group)
            return it.current(); // we assume only one request to the same group
    }
    return 0;
}


KPTResourceRequest *KPTResourceRequestCollection::find(KPTResource *resource) const {
    KPTResourceRequest *req = 0;
    QPtrListIterator<KPTResourceGroupRequest> it(m_requests);
    for (; !req && it.current(); ++it) {
        req = it.current()->find(resource);
    }
    return req;
}

// bool KPTResourceRequestCollection::load(QDomElement &element, KPTProject *project) {
//     //kdDebug()<<k_funcinfo<<endl;
//     return true;
// }

void KPTResourceRequestCollection::save(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<KPTResourceGroupRequest> it(m_requests);
    for ( ; it.current(); ++it ) {
        it.current()->save(element);
    }
}

int KPTResourceRequestCollection::units() const {
    //kdDebug()<<k_funcinfo<<endl;
    int units = 0;
    QPtrListIterator<KPTResourceGroupRequest> it = m_requests;
    for (; it.current(); ++it) {
        units += it.current()->units();
        //kdDebug()<<k_funcinfo<<" Group: "<<it.current()->group()->name()<<" now="<<units<<endl;
    }
    return units;
}

int KPTResourceRequestCollection::workUnits() const {
    //kdDebug()<<k_funcinfo<<endl;
    int units = 0;
    QPtrListIterator<KPTResourceGroupRequest> it(m_requests);
    for (; it.current(); ++it) {
        units += it.current()->workUnits();
    }
    //kdDebug()<<k_funcinfo<<" units="<<units<<endl;
    return units;
}

// Returns the longest duration needed by any of the groups.
// The effort is distributed on "work type" resourcegroups in proportion to
// the amount of resources requested for each group.
// "Material type" of resourcegroups does not (atm) affect the duration.
KPTDuration KPTResourceRequestCollection::duration(const KPTDateTime &time, const KPTDuration &effort, bool backward) {
    //kdDebug()<<k_funcinfo<<"time="<<time.toString()<<" effort="<<effort.toString(KPTDuration::Format_Day)<<" backward="<<backward<<endl;
    if (isEmpty()) {
        return effort;
    }
    KPTDuration dur;
    int units = workUnits();
    if (units == 0)
        units = 100; //hmmmm
    QPtrListIterator<KPTResourceGroupRequest> it(m_requests);
    for (; it.current(); ++it) {
        if (it.current()->isEmpty())
            continue;
        if (it.current()->group()->type() == KPTResourceGroup::Type_Work) {
            KPTDuration d = it.current()->duration(time, (effort*it.current()->workUnits())/units, backward);
            if (d > dur)
                dur = d;
        } else if (it.current()->group()->type() == KPTResourceGroup::Type_Material) {
            //TODO
            if (dur == KPTDuration::zeroDuration)
                dur = effort;
        }
    }
    return dur;
}

KPTDateTime KPTResourceRequestCollection::availableAfter(const KPTDateTime &time) {
    KPTDateTime start;
    QPtrListIterator<KPTResourceGroupRequest> it = m_requests;
    for (; it.current(); ++it) {
        KPTDateTime t = it.current()->availableAfter(time);
        if (!start.isValid() || t < start)
            start = t;
    }
    if (start.isValid() && start < time)
        start = time;
    //kdDebug()<<k_funcinfo<<time.toString()<<"="<<start.toString()<<endl;
    return start;
}

KPTDateTime KPTResourceRequestCollection::availableBefore(const KPTDateTime &time) {
    KPTDateTime end;
    QPtrListIterator<KPTResourceGroupRequest> it = m_requests;
    for (; it.current(); ++it) {
        KPTDateTime t = it.current()->availableBefore(time);
        if (!end.isValid() || end > t)
            end = t;
    }
    if (!end.isValid() || end > time)
        end = time;
    return end;
}


void KPTResourceRequestCollection::makeAppointments(KPTTask *task) {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<KPTResourceGroupRequest> it(m_requests);
    for (; it.current(); ++it) {
        it.current()->makeAppointments(task);
    }
}

void KPTResourceRequestCollection::reserve(const KPTDateTime &start, const KPTDuration &duration) {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<KPTResourceGroupRequest> it(m_requests);
    for (; it.current(); ++it) {
        it.current()->reserve(start, duration);
    }
}

bool KPTResourceRequestCollection::isEmpty() const {
    QPtrListIterator<KPTResourceGroupRequest> it(m_requests);
    for (; it.current(); ++it) {
        if (!it.current()->isEmpty())
            return false;
    }
    return true;
}
#ifndef NDEBUG

void KPTResourceGroup::printDebug(QString indent)
{
    kdDebug()<<indent<<"  + Resource group: "<<m_name<<" id="<<m_id<<endl;
    indent += "   !";
    QPtrListIterator<KPTResource> it(m_resources);
    for ( ; it.current(); ++it)
        it.current()->printDebug(indent);
}
void KPTResource::printDebug(QString indent)
{
    kdDebug()<<indent<<"  + Resource: "<<m_name<<" id="<<m_id<<" Overbooked="<<isOverbooked()<<endl;
    QPtrListIterator<KPTAppointment> it(m_appointments);
    indent += "  !";
    for (; it.current(); ++it)
        it.current()->printDebug(indent);
    indent += "  !";
}

void KPTAppointment::printDebug(QString indent)
{
    kdDebug()<<indent<<"  + Appointment to task: "<<m_node->name()<<endl;
    indent += "  !";
    QPtrListIterator<KPTAppointmentInterval> it = intervals();
    for (; it.current(); ++it) {
        kdDebug()<<indent<<it.current()->startTime().toString()<<endl;
        kdDebug()<<indent<<it.current()->endTime().toString()<<endl;
        kdDebug()<<indent<<it.current()->load()<<endl;
    }
}

void KPTResourceGroupRequest::printDebug(QString indent)
{
    kdDebug()<<indent<<"  + Request to group: "<<(m_group ? m_group->name() : "None")<<" units="<<m_units<<"%"<<endl;
    indent += "  !";
    QPtrListIterator<KPTResourceRequest> it(m_resourceRequests);
    for (; it.current(); ++it) {
        it.current()->printDebug(indent);
    }
}

void KPTResourceRequest::printDebug(QString indent)
{
    kdDebug()<<indent<<"  + Request to resource: "<<(m_resource ? m_resource->name() : "None")<<" units="<<m_units<<"%"<<endl;
}

void KPTResourceRequestCollection::printDebug(QString indent)
{
    kdDebug()<<indent<<"  + Resource requests:"<<endl;
    QPtrListIterator<KPTResourceGroupRequest> it = m_requests;
    for (; it.current(); ++it) {
        it.current()->printDebug(indent+"  ");
    }
}
#endif

}  //KPlato namespace
