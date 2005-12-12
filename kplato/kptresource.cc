/* This file is part of the KDE project
   Copyright (C) 2001 Thomas zander <zander@kde.org>
   Copyright (C) 2004, 2005 Dag Andersen <danders@get2net.dk>

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
#include "kpteffortcostmap.h"

#include <kdebug.h>

namespace KPlato
{

ResourceGroup::ResourceGroup(Project *project) {
    m_project = project;
    m_type = Type_Work;
    m_resources.setAutoDelete(true);
    generateId();
    //kdDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
}

ResourceGroup::~ResourceGroup() {
    removeId();
    //kdDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
}

bool ResourceGroup::setId(QString id) {
    //kdDebug()<<k_funcinfo<<id<<endl;
    if (id.isEmpty()) {
        kdError()<<k_funcinfo<<"id is empty"<<endl;
        m_id = id;
        return false;
    }
    ResourceGroup *g = findId();
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

void ResourceGroup::generateId() {
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

void ResourceGroup::addResource(Resource* resource, Risk*) {
    m_resources.append(resource);
}

Resource* ResourceGroup::getResource(int) {
    return 0L;
}

Risk* ResourceGroup::getRisk(int) {
    return 0L;
}

void ResourceGroup::removeResource(Resource *resource) {
    m_resources.removeRef(resource);
}

Resource *ResourceGroup::takeResource(Resource *resource) {
    return m_resources.take(m_resources.findRef(resource));
}

void ResourceGroup::removeResource(int) {
}

void ResourceGroup::addRequiredResource(ResourceGroup*) {
}

ResourceGroup* ResourceGroup::getRequiredResource(int) {
    return 0L;
}

void ResourceGroup::removeRequiredResource(int) {
}

bool ResourceGroup::load(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    setId(element.attribute("id"));
    m_name = element.attribute("name");

    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
    	if (list.item(i).isElement()) {
	        QDomElement e = list.item(i).toElement();
    	    if (e.tagName() == "resource") {
	    	    // Load the resource
		        Resource *child = new Resource(m_project);
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

void ResourceGroup::save(QDomElement &element)  {
    //kdDebug()<<k_funcinfo<<endl;

    QDomElement me = element.ownerDocument().createElement("resource-group");
    element.appendChild(me);

    me.setAttribute("id", m_id);
    me.setAttribute("name", m_name);

    QPtrListIterator<Resource> it(m_resources);
    for ( ; it.current(); ++it ) {
        it.current()->save(me);
    }
}

void ResourceGroup::saveAppointments(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<Resource> it(m_resources);
    for ( ; it.current(); ++it ) {
        it.current()->saveAppointments(element);
    }
}

void ResourceGroup::clearAppointments() {
    QPtrListIterator<Resource> it(m_resources);
    for (; it.current(); ++it) {
        it.current()->clearAppointments();
    }
    clearNodes();
}

int ResourceGroup::units() {
    int u = 0;
    QPtrListIterator<Resource> it = m_resources;
    for (; it.current(); ++it) {
        u += it.current()->units();
    }
    return u;
}

ResourceGroup *ResourceGroup::findId(const QString &id) const {
    return m_project ? m_project->findResourceGroup(id) : 0;
}

bool ResourceGroup::removeId(const QString &id) { 
    return m_project ? m_project->removeResourceGroupId(id): false;
}

void ResourceGroup::insertId(const QString &id) { 
    if (m_project)
        m_project->insertResourceGroupId(id, this);
}

Appointment ResourceGroup::appointmentIntervals() const {
    Appointment a;
    QPtrListIterator<Resource> it = m_resources;
    for (; it.current(); ++it) {
        a += it.current()->appointmentIntervals();
    }
    return a;
}

Resource::Resource(Project *project) : m_project(project), m_appointments(), m_workingHours(), m_overbooked(false) {
    m_type = Type_Work;
    m_units = 100; // %

    cost.normalRate = 100;
    cost.overtimeRate = 200;
    cost.fixed = 0;
    m_calendar = 0;

    generateId();
    //kdDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
}

Resource::Resource(Resource *resource) { 
    //kdDebug()<<k_funcinfo<<"("<<this<<") from ("<<resource<<")"<<endl;
    copy(resource); 
}

Resource::~Resource() {
    //kdDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    removeId();
    QPtrListIterator<ResourceRequest> it = m_requests;
    for (; it.current(); ++it) {
        it.current()->setResource(0); // avoid the request to mess with my list
        it.current()->parent()->removeResourceRequest(it.current()); // deletes the request
    }
    clearAppointments();
}

bool Resource::setId(QString id) {
    //kdDebug()<<k_funcinfo<<id<<endl;
    if (id.isEmpty()) {
        kdError()<<k_funcinfo<<"id is empty"<<endl;
        m_id = id;
        return false;
    }
    Resource *r = findId();
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

void Resource::generateId() {
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

void Resource::setType(const QString &type) {
    if (type == "Work")
        m_type = Type_Work;
    else if (type == "Material")
        m_type = Type_Material;
}

QString Resource::typeToString() const {
    if (m_type == Type_Work)
        return QString("Work");
    else if (m_type == Type_Material)
        return QString("Material");

    return QString();
}

void Resource::copy(Resource *resource) {
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

void Resource::addWorkingHour(QTime from, QTime until) {
    //kdDebug()<<k_funcinfo<<endl;
    m_workingHours.append(new QTime(from));
    m_workingHours.append(new QTime(until));
}

Calendar *Resource::calendar() const {
    if (!m_calendar && project())
        return project()->defaultCalendar();
    return m_calendar;
}

DateTime *Resource::getFirstAvailableTime(DateTime /*after*/) {
    return 0L;
}

DateTime *Resource::getBestAvailableTime(Duration /*duration*/) {
    return 0L;
}

DateTime *Resource::getBestAvailableTime(const DateTime after, const Duration duration) {
    return 0L;
}

bool Resource::load(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    setId(element.attribute("id"));
    m_name = element.attribute("name");
    m_initials = element.attribute("initials");
    m_email = element.attribute("email");
    setType(element.attribute("type"));
    m_calendar = findCalendar(element.attribute("calendar-id"));
    return true;
}

void Resource::save(QDomElement &element) {
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

bool Resource::isAvailable(Task *task) {
    bool busy = false;
    QPtrListIterator<Appointment> it(m_appointments);
    for (; it.current(); ++it) {
        if (it.current()->isBusy(task->startTime(), task->endTime())) {
            busy = true;
            break;
        }
    }
    return !busy;
}

Appointment *Resource::findAppointment(Node *node) {
    QPtrListIterator<Appointment> it = m_appointments;
    for (; it.current(); ++it) {
        if (it.current()->node() == node)
            return it.current();
    }
    return 0;
}

bool Resource::addAppointment(Appointment *appointment) {
    if (m_appointments.findRef(appointment) != -1) {
        kdError()<<k_funcinfo<<"Appointment allready exists"<<endl;
        return false;
    }
    m_appointments.append(appointment);
    return true;
        
}

void Resource::addAppointment(Node *node, DateTime &start, DateTime &end, double load) {
    Appointment *a = findAppointment(node);
    if (a != 0) {
        a->addInterval(start, end, load);
        return;
    }
    a = new Appointment(this, node, start, end, load);
    if (node->addAppointment(a)) {
        m_appointments.append(a);
    } else {
        delete a;
    }
}


void Resource::removeAppointment(Appointment *appointment) {
    takeAppointment(appointment);
    delete appointment;
}

void Resource::takeAppointment(Appointment *appointment) {
    int i = m_appointments.findRef(appointment);
    if (i != -1) {
        m_appointments.take(i);
        if (appointment->node())
            appointment->node()->takeAppointment(appointment);
    }
}



void Resource::clearAppointments() {
    m_overbooked = false;
    Appointment *a;
    while ((a = m_appointments.getFirst()))
        delete a;
}

void Resource::makeAppointment(DateTime &start, Duration &duration, Task *task) {
    //kdDebug()<<k_funcinfo<<task->name()<<": "<<start.toString()<<" dur "<<duration.toString()<<endl;
    Calendar *cal = calendar();
    if (!cal) {
        kdWarning()<<k_funcinfo<<m_name<<": No calendar defined"<<endl;
        return; 
    }
    //TODO: units and moderated by availability, and standard non-working days
    DateTime time = start;
    DateTime end = start+duration;
    while (time < end) {
        //kdDebug()<<k_funcinfo<<time.toString()<<" to "<<end.toString()<<endl;
        if (!cal->hasInterval(time, end)) {
            //kdDebug()<<time.toString()<<" to "<<end.toString()<<": No (more) interval(s)"<<endl;
            return; // nothing more to do
        }
        QPair<DateTime, DateTime> i = cal->interval(time, end);
        if (time == i.second)
            return; // hmmm, didn't get a new interval, avoid loop
        addAppointment(task, i.first, i.second, 100); //FIXME
        time = i.second;
    }
}

void Resource::saveAppointments(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<Appointment> it(m_appointments);
    for ( ; it.current(); ++it ) {
        it.current()->saveXML(element);
    }
}

// the amount of effort we can do within the duration
Duration Resource::effort(const DateTime &start, const Duration &duration, bool *ok) const {
    //kdDebug()<<k_funcinfo<<m_name<<": "<<start.date().toString()<<" for duration "<<duration.toString(Duration::Format_Day)<<endl;
    bool sts=false;
    Duration e;
    if ((!m_availableFrom.isValid() || start+duration >= m_availableFrom) && 
        (!m_availableUntil.isValid() || start < m_availableUntil)) 
    {
        Calendar *cal = calendar();
        if (cal && cal->hasIntervalAfter(start)) {
            sts = true;
            e = (cal->effort(start, duration) * m_units)/100;
        }
    }
    //kdDebug()<<k_funcinfo<<"e="<<e.toString(Duration::Format_Day)<<" ("<<m_units<<")"<<endl;
    if (ok) *ok = sts;
    return e;
}

DateTime Resource::availableAfter(const DateTime &time) {
    DateTime t;
    Calendar *cal = calendar();
    if (cal)
        t = cal->availableAfter(time);
    //kdDebug()<<k_funcinfo<<time.toString()<<"="<<t.toString()<<" "<<m_name<<endl;
    return t;
}

DateTime Resource::availableBefore(const DateTime &time) {
    DateTime t;
    Calendar *cal = calendar();
    if (cal)
        t = cal->availableBefore(time);
    return t;
}

Resource *Resource::findId(const QString &id) const { 
    return m_project ? m_project->findResource(id) : 0; 
}

bool Resource::removeId(const QString &id) { 
    return m_project ? m_project->removeResourceId(id) : false; 
}

void Resource::insertId(const QString &id) { 
    if (m_project)
        m_project->insertResourceId(id, this); 
}

Calendar *Resource::findCalendar(const QString &id) const { 
    return (m_project ? m_project->findCalendar(id) : 0); 
}

bool Resource::isOverbooked() const {
    return isOverbooked(DateTime(), DateTime());
}

bool Resource::isOverbooked(const QDate &date) const {
    return isOverbooked(DateTime(date), DateTime(date.addDays(1)));
}

bool Resource::isOverbooked(const DateTime &start, const DateTime &end) const {
    //kdDebug()<<k_funcinfo<<start.toString()<<" - "<<end.toString()<<endl;
    Appointment a = appointmentIntervals();
    QPtrListIterator<AppointmentInterval> it = a.intervals();
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

Appointment Resource::appointmentIntervals() const {
    Appointment a;
    QPtrListIterator<Appointment> it = m_appointments;
    for (; it.current(); ++it) {
        a += *(it.current());
    }
    return a;
}

Duration Resource::plannedEffort(const QDate &date) const {
    Duration e;
    QPtrListIterator<Appointment> it = m_appointments;
    for (; it.current(); ++it) {
        e += it.current()->plannedEffort(date);
    }
    return e;
}

//////

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
    m_start = DateTime::fromString(element.attribute("start"));
    m_end = DateTime::fromString(element.attribute("end"));
    m_load = element.attribute("load", "100").toDouble(&ok);
    if (!ok) m_load = 100;
    return m_start.isValid() && m_end.isValid();
}

void AppointmentInterval::saveXML(QDomElement &element) {
    QDomElement me = element.ownerDocument().createElement("interval");
    element.appendChild(me);

    me.setAttribute("start", m_start.toString());
    me.setAttribute("end", m_end.toString());
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
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "actual-effort") {
                QDate date = QDate::fromString(e.attribute("date"), Qt::ISODate);
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

void Appointment::UsedEffort::save(QDomElement &element) {
    if (isEmpty()) return;
    for (UsedEffortItem *item = first(); item; item = next()) {
        QDomElement me = element.ownerDocument().createElement("actual-effort");
        element.appendChild(me);
        me.setAttribute("date",item->date().toString(Qt::ISODate));
        me.setAttribute("effort",item->effort().toString());
        me.setAttribute("overtime",item->isOvertime());
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
    m_resource=0;
    m_node=0;
    m_repeatInterval=Duration();
    m_repeatCount=0;

    m_intervals.setAutoDelete(true);
}

Appointment::Appointment(Resource *resource, Node *node, DateTime start, DateTime end, double load) 
    : m_extraRepeats(), 
      m_skipRepeats() {
    
    m_node = node;
    m_resource = resource;
    m_repeatInterval = Duration();
    m_repeatCount = 0;

    addInterval(start, end, load);

    m_intervals.setAutoDelete(true);
}

Appointment::Appointment(Resource *resource, Node *node, DateTime start, Duration duration, double load) 
    : m_extraRepeats(), 
      m_skipRepeats() {
    
    m_node = node;
    m_resource = resource;
    m_repeatInterval = Duration();
    m_repeatCount = 0;

    addInterval(start, duration, load);
    
    m_intervals.setAutoDelete(true);
}

Appointment::~Appointment() {
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

void Appointment::deleteAppointmentFromRepeatList(DateTime time) {
}

void Appointment::addAppointmentToRepeatList(DateTime time) {
}

bool Appointment::isBusy(const DateTime &start, const DateTime &end) {
    return false;
}

bool Appointment::loadXML(QDomElement &element, Project &project) {
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
    m_actualEffort.load(element);
    if (m_intervals.isEmpty()) {
        return false; 
    }
    return attach();
}

void Appointment::saveXML(QDomElement &element) {
    QDomElement me = element.ownerDocument().createElement("appointment");
    element.appendChild(me);

    if (m_intervals.isEmpty()) {
        return; // shouldn't happen
    }
    me.setAttribute("resource-id", m_resource->id());
    me.setAttribute("task-id", m_node->id());
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
    //kdDebug()<<k_funcinfo<<m_node->name()<<", "<<m_resource->name()<<endl;
    EffortCostMap ec;
    Duration eff;
    DateTime dt(start);
    DateTime ndt(dt.addDays(1));
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
            double cost = eff.toDouble(Duration::Unit_h) * m_resource->normalRate(); //TODO overtime and cost unit
            ec.insert(dt.date(), eff, cost);
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
    return plannedEffort().toDouble(Duration::Unit_h) * m_resource->normalRate(); //FIXME overtime
}

//Calculates the planned cost on date
double Appointment::plannedCost(const QDate &date) {
    return plannedEffort(date).toDouble(Duration::Unit_h) * m_resource->normalRate(); //FIXME overtime
}

//Calculates the planned cost upto and including date
double Appointment::plannedCostTo(const QDate &date) {
    return plannedEffortTo(date).toDouble(Duration::Unit_h) * m_resource->normalRate(); //FIXME overtime
}

// Calculates the total actual cost for this appointment
double Appointment::actualCost() {
    //kdDebug()<<k_funcinfo<<m_actualEffort.usedEffort(false /*ex. overtime*/).toDouble(Duration::Unit_h)<<endl;
    
    return (m_actualEffort.usedEffort(false /*ex. overtime*/).toDouble(Duration::Unit_h)*m_resource->normalRate()) + (m_actualEffort.usedOvertime().toDouble(Duration::Unit_h)*m_resource->overtimeRate());
}

// Calculates the actual cost on date
double Appointment::actualCost(const QDate &date) {
    return (m_actualEffort.usedEffort(date, false /*ex. overtime*/).toDouble(Duration::Unit_h)*m_resource->normalRate()) + (m_actualEffort.usedOvertime(date).toDouble(Duration::Unit_h)*m_resource->overtimeRate());
}

// Calculates the actual cost upto and including date
double Appointment::actualCostTo(const QDate &date) {
    return (m_actualEffort.usedEffortTo(date, false /*ex. overtime*/).toDouble(Duration::Unit_h)*m_resource->normalRate()) + (m_actualEffort.usedOvertimeTo(date).toDouble(Duration::Unit_h)*m_resource->overtimeRate());
}

void Appointment::addActualEffort(QDate date, Duration effort, bool overtime) {
    m_actualEffort.inSort(date, effort, overtime);
}

bool Appointment::attach() { 
    if (m_resource && m_node) {
        m_resource->addAppointment(this);
        m_node->addAppointment(this);
        return true;
    }
    kdWarning()<<k_funcinfo<<"Failed: "<<(m_resource ? "" : "resource=0 ") 
                                       <<(m_node ? "" : "node=0")<<endl;
    return false;
}

void Appointment::detach() {
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

/////////   Risk   /////////
Risk::Risk(Node *n, Resource *r, RiskType rt) {
    m_node=n;
    m_resource=r;
    m_riskType=rt;
}

Risk::~Risk() {
}

ResourceRequest::ResourceRequest(Resource *resource, int units)
    : m_resource(resource),
      m_units(units),
      m_parent(0) {
    //kdDebug()<<k_funcinfo<<"Request to: "<<(resource ? resource->name() : QString("None"))<<endl;
}

ResourceRequest::~ResourceRequest() {
    //kdDebug()<<k_funcinfo<<"resource: "<<m_resource->name()<<endl;
    if (m_resource)
        m_resource->unregisterRequest(this);
    m_resource = 0;
}

bool ResourceRequest::load(QDomElement &element, Project &project) {
    //kdDebug()<<k_funcinfo<<endl;
    m_resource = project.resource(element.attribute("resource-id"));
    if (m_resource == 0) {
        kdDebug()<<k_funcinfo<<"The referenced resource does not exist: resource id="<<element.attribute("resource-id")<<endl;
        return false;
    }
    m_units  = element.attribute("units").toInt();
    return true;
}

void ResourceRequest::save(QDomElement &element) {
    QDomElement me = element.ownerDocument().createElement("resource-request");
    element.appendChild(me);
    me.setAttribute("resource-id", m_resource->id());
    me.setAttribute("units", m_units);
}

int ResourceRequest::units() const {
    //kdDebug()<<k_funcinfo<<m_resource->name()<<": units="<<m_units<<endl;
    return m_units;
}

int ResourceRequest::workUnits() const {
    if (m_resource->type() == Resource::Type_Work)
        return units();
        
    //kdDebug()<<k_funcinfo<<"units=0"<<endl;
    return 0;
}

/////////
ResourceGroupRequest::ResourceGroupRequest(ResourceGroup *group, int units)
    : m_group(group), m_units(units) {

    //kdDebug()<<k_funcinfo<<"Request to: "<<(group ? group->name() : QString("None"))<<endl;
    if (group)
        group->registerRequest(this);
    m_resourceRequests.setAutoDelete(true);
}

ResourceGroupRequest::~ResourceGroupRequest() {
    //kdDebug()<<k_funcinfo<<"Group: "<<m_group->name()<<endl;
    if (m_group)
        m_group->unregisterRequest(this);
    m_resourceRequests.clear();
}

void ResourceGroupRequest::addResourceRequest(ResourceRequest *request) {
    //kdDebug()<<k_funcinfo<<"Group: "<<m_group->name()<<endl;
    request->setParent(this);
    m_resourceRequests.append(request);
    request->registerRequest();
}

ResourceRequest *ResourceGroupRequest::takeResourceRequest(ResourceRequest *request) {
    if (request)
        request->unregisterRequest();
    return m_resourceRequests.take(m_resourceRequests.findRef(request)); 
}

ResourceRequest *ResourceGroupRequest::find(Resource *resource) const {
    QPtrListIterator<ResourceRequest> it(m_resourceRequests);
    for (; it.current(); ++it)
        if (it.current()->resource() == resource)
            return it.current();

    return 0;
}

bool ResourceGroupRequest::load(QDomElement &element, Project &project) {
    //kdDebug()<<k_funcinfo<<endl;
    m_group = project.findResourceGroup(element.attribute("group-id"));
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
                ResourceRequest *r = new ResourceRequest();
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

void ResourceGroupRequest::save(QDomElement &element) {
    if (units() == 0)
        return;
    QDomElement me = element.ownerDocument().createElement("resourcegroup-request");
    element.appendChild(me);
    me.setAttribute("group-id", m_group->id());
    me.setAttribute("units", m_units);
    QPtrListIterator<ResourceRequest> it(m_resourceRequests);
    for (; it.current(); ++it)
        it.current()->save(me);
}

int ResourceGroupRequest::units() const {
    int units = m_units;
    QPtrListIterator<ResourceRequest> it = m_resourceRequests;
    for (; it.current(); ++it) {
        units += it.current()->units();
    }
    //kdDebug()<<k_funcinfo<<"units="<<units<<endl;
    return units;
}

int ResourceGroupRequest::workUnits() const {
    int units = 0;
    if (m_group->type() == ResourceGroup::Type_Work)
        units = m_units;
    QPtrListIterator<ResourceRequest> it = m_resourceRequests;
    for (; it.current(); ++it) {
        units += it.current()->workUnits();
    }
    //kdDebug()<<k_funcinfo<<"units="<<units<<endl;
    return units;
}

//TODO: handle nonspecific resources
Duration ResourceGroupRequest::effort(const DateTime &time, const Duration &duration, bool *ok) const {
    Duration e;
    bool sts=false;
    if (ok) *ok = sts;
    QPtrListIterator<ResourceRequest> it = m_resourceRequests;
    for (; it.current(); ++it) {
        e += it.current()->resource()->effort(time, duration, &sts);
        if (sts && ok) *ok = sts;
        //kdDebug()<<k_funcinfo<<it.current()->resource()->name()<<" e="<<e.toString()<<endl;
    }
    return e;
}

// FIXME: Handle 'any' duration, atm stops at 2 years.
Duration ResourceGroupRequest::duration(const DateTime &time, const Duration &_effort, bool backward) {
    //kdDebug()<<k_funcinfo<<"--->"<<(backward?"(B) ":"(F) ")<<m_group->name()<<" "<<time.toString()<<": effort: "<<_effort.toString(Duration::Format_Day)<<" ("<<_effort.milliseconds()<<")"<<endl;
    Duration e;
    if (_effort == Duration::zeroDuration) {
        return e;
    }
    bool sts=true;
    bool match = false;
    DateTime start = time;
    int inc = backward ? -1 : 1;
    DateTime end = start;
    Duration e1;
    for (int i=0; !match && i < 730 && sts; ++i) {
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
        end = end.addSecs(inc);
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
            break;
        }
        //kdDebug()<<"duration(ms)["<<i<<"]"<<(backward?"backward":"forward:")<<" time="<<start.time().toString()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")"<<endl;
    }
    if (!match) {
        kdError()<<k_funcinfo<<"Could not match effort."<<" Want: "<<_effort.toString(Duration::Format_Day)<<" got: "<<e.toString(Duration::Format_Day)<<" sts="<<sts<<endl;
    }
    end = backward ? availableAfter(end) : availableBefore(end);
    
    //kdDebug()<<k_funcinfo<<"<---"<<(backward?"(B) ":"(F) ")<<m_group->name()<<": "<<end.toString()<<"-"<<time.toString()<<"="<<(end - time).toString()<<" effort: "<<_effort.toString(Duration::Format_Day)<<endl;
    return end - time;
}

DateTime ResourceGroupRequest::availableAfter(const DateTime &time) {
    DateTime start;
    QPtrListIterator<ResourceRequest> it = m_resourceRequests;
    for (; it.current(); ++it) {
        DateTime t = it.current()->resource()->availableAfter(time);
        if (t.isValid() && (!start.isValid() || t < start))
            start = t;
    }
    if (start.isValid() && start < time)
        start = time;
    //kdDebug()<<k_funcinfo<<time.toString()<<"="<<start.toString()<<" "<<m_group->name()<<endl;
    return start;
}

DateTime ResourceGroupRequest::availableBefore(const DateTime &time) {
    DateTime end;
    QPtrListIterator<ResourceRequest> it = m_resourceRequests;
    for (; it.current(); ++it) {
        DateTime t = it.current()->resource()->availableBefore(time);
        if (!end.isValid() || end > t)
            end = t;
    }
    if (!end.isValid() || end > time)
        end = time;
    return end;
}

void ResourceGroupRequest::makeAppointments(Task *task) {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<ResourceRequest> it = m_resourceRequests;
    for (; it.current(); ++it) {
        it.current()->makeAppointment(m_start, m_duration, task);
    }
}

void ResourceGroupRequest::reserve(const DateTime &start, const Duration &duration) {
    m_start = start;
    m_duration = duration;
}

bool ResourceGroupRequest::isEmpty() const {
    return m_resourceRequests.isEmpty() && m_units == 0;
}

/////////
ResourceRequestCollection::ResourceRequestCollection(Task &task)
    : m_task(task) {
    m_requests.setAutoDelete(true);
}

ResourceRequestCollection::~ResourceRequestCollection() {
    //kdDebug()<<k_funcinfo<<"Group: "<<m_group->name()<<endl;
    m_requests.clear();
}

ResourceGroupRequest *ResourceRequestCollection::find(ResourceGroup *group) const {
    QPtrListIterator<ResourceGroupRequest> it(m_requests);
    for (; it.current(); ++it) {
        if (it.current()->group() == group)
            return it.current(); // we assume only one request to the same group
    }
    return 0;
}


ResourceRequest *ResourceRequestCollection::find(Resource *resource) const {
    ResourceRequest *req = 0;
    QPtrListIterator<ResourceGroupRequest> it(m_requests);
    for (; !req && it.current(); ++it) {
        req = it.current()->find(resource);
    }
    return req;
}

// bool ResourceRequestCollection::load(QDomElement &element, Project &project) {
//     //kdDebug()<<k_funcinfo<<endl;
//     return true;
// }

void ResourceRequestCollection::save(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<ResourceGroupRequest> it(m_requests);
    for ( ; it.current(); ++it ) {
        it.current()->save(element);
    }
}

int ResourceRequestCollection::units() const {
    //kdDebug()<<k_funcinfo<<endl;
    int units = 0;
    QPtrListIterator<ResourceGroupRequest> it = m_requests;
    for (; it.current(); ++it) {
        units += it.current()->units();
        //kdDebug()<<k_funcinfo<<" Group: "<<it.current()->group()->name()<<" now="<<units<<endl;
    }
    return units;
}

int ResourceRequestCollection::workUnits() const {
    //kdDebug()<<k_funcinfo<<endl;
    int units = 0;
    QPtrListIterator<ResourceGroupRequest> it(m_requests);
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
Duration ResourceRequestCollection::duration(const DateTime &time, const Duration &effort, bool backward) {
    //kdDebug()<<k_funcinfo<<"time="<<time.toString()<<" effort="<<effort.toString(Duration::Format_Day)<<" backward="<<backward<<endl;
    if (isEmpty()) {
        return effort;
    }
    Duration dur;
    int units = workUnits();
    if (units == 0)
        units = 100; //hmmmm
    QPtrListIterator<ResourceGroupRequest> it(m_requests);
    for (; it.current(); ++it) {
        if (it.current()->isEmpty())
            continue;
        if (it.current()->group()->type() == ResourceGroup::Type_Work) {
            Duration d = it.current()->duration(time, (effort*it.current()->workUnits())/units, backward);
            if (d > dur)
                dur = d;
        } else if (it.current()->group()->type() == ResourceGroup::Type_Material) {
            //TODO
            if (dur == Duration::zeroDuration)
                dur = effort;
        }
    }
    return dur;
}

DateTime ResourceRequestCollection::availableAfter(const DateTime &time) {
    DateTime start;
    QPtrListIterator<ResourceGroupRequest> it = m_requests;
    for (; it.current(); ++it) {
        DateTime t = it.current()->availableAfter(time);
        if (!start.isValid() || t < start)
            start = t;
    }
    if (start.isValid() && start < time)
        start = time;
    //kdDebug()<<k_funcinfo<<time.toString()<<"="<<start.toString()<<endl;
    return start;
}

DateTime ResourceRequestCollection::availableBefore(const DateTime &time) {
    DateTime end;
    QPtrListIterator<ResourceGroupRequest> it = m_requests;
    for (; it.current(); ++it) {
        DateTime t = it.current()->availableBefore(time);
        if (!end.isValid() || end > t)
            end = t;
    }
    if (!end.isValid() || end > time)
        end = time;
    return end;
}


void ResourceRequestCollection::makeAppointments(Task *task) {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<ResourceGroupRequest> it(m_requests);
    for (; it.current(); ++it) {
        it.current()->makeAppointments(task);
    }
}

void ResourceRequestCollection::reserve(const DateTime &start, const Duration &duration) {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<ResourceGroupRequest> it(m_requests);
    for (; it.current(); ++it) {
        it.current()->reserve(start, duration);
    }
}

bool ResourceRequestCollection::isEmpty() const {
    QPtrListIterator<ResourceGroupRequest> it(m_requests);
    for (; it.current(); ++it) {
        if (!it.current()->isEmpty())
            return false;
    }
    return true;
}
#ifndef NDEBUG

void ResourceGroup::printDebug(QString indent)
{
    kdDebug()<<indent<<"  + Resource group: "<<m_name<<" id="<<m_id<<endl;
    indent += "   !";
    QPtrListIterator<Resource> it(m_resources);
    for ( ; it.current(); ++it)
        it.current()->printDebug(indent);
}
void Resource::printDebug(QString indent)
{
    kdDebug()<<indent<<"  + Resource: "<<m_name<<" id="<<m_id<<" Overbooked="<<isOverbooked()<<endl;
    QPtrListIterator<Appointment> it(m_appointments);
    indent += "  !";
    for (; it.current(); ++it)
        it.current()->printDebug(indent);
    indent += "  !";
}

void Appointment::printDebug(QString indent)
{
    kdDebug()<<indent<<"  + Appointment to task: "<<m_node->name()<<endl;
    indent += "  !";
    QPtrListIterator<AppointmentInterval> it = intervals();
    for (; it.current(); ++it) {
        kdDebug()<<indent<<it.current()->startTime().toString()<<endl;
        kdDebug()<<indent<<it.current()->endTime().toString()<<endl;
        kdDebug()<<indent<<it.current()->load()<<endl;
    }
}

void ResourceGroupRequest::printDebug(QString indent)
{
    kdDebug()<<indent<<"  + Request to group: "<<(m_group ? m_group->name() : "None")<<" units="<<m_units<<"%"<<endl;
    indent += "  !";
    QPtrListIterator<ResourceRequest> it(m_resourceRequests);
    for (; it.current(); ++it) {
        it.current()->printDebug(indent);
    }
}

void ResourceRequest::printDebug(QString indent)
{
    kdDebug()<<indent<<"  + Request to resource: "<<(m_resource ? m_resource->name() : "None")<<" units="<<m_units<<"%"<<endl;
}

void ResourceRequestCollection::printDebug(QString indent)
{
    kdDebug()<<indent<<"  + Resource requests:"<<endl;
    QPtrListIterator<ResourceGroupRequest> it = m_requests;
    for (; it.current(); ++it) {
        it.current()->printDebug(indent+"  ");
    }
}
#endif

}  //KPlato namespace
