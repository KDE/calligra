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
#include "kptappointment.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptdatetime.h"
#include "kptcalendar.h"
#include "kpteffortcostmap.h"
#include "kptschedule.h"

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

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
    if (findId() == this) {
        removeId(); // only remove myself (I may be just a working copy)
    }
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

void ResourceGroup::save(QDomElement &element)  const {
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

void ResourceGroup::initiateCalculation(Schedule &sch) {
    QPtrListIterator<Resource> it(m_resources);
    for (; it.current(); ++it) {
        it.current()->initiateCalculation(sch);
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

Resource::Resource(Project *project) : m_project(project), m_schedules(), m_workingHours() {
    m_type = Type_Work;
    m_units = 100; // %

    m_availableFrom = DateTime(QDate::currentDate());
    m_availableUntil = m_availableFrom.addYears(2);

    cost.normalRate = 100;
    cost.overtimeRate = 200;
    cost.fixed = 0;
    m_calendar = 0;
    m_currentSchedule = 0;
    generateId();
    //kdDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
}

Resource::Resource(Resource *resource) { 
    //kdDebug()<<k_funcinfo<<"("<<this<<") from ("<<resource<<")"<<endl;
    copy(resource); 
}

Resource::~Resource() {
    //kdDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    if (findId() == this) {
        removeId(); // only remove myself (I may be just a working copy)
    }
    QPtrListIterator<ResourceRequest> it = m_requests;
    for (; it.current(); ++it) {
        it.current()->setResource(0); // avoid the request to mess with my list
    }
    m_requests.first();
    for (; m_requests.current(); m_requests.next()) {
        m_requests.current()->parent()->removeResourceRequest(m_requests.current()); // deletes the request
    }
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
            //kdDebug()<<k_funcinfo<<m_name<<": inserted id="<<m_id<<endl;
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

Calendar *Resource::calendar(bool local) const {
    if (!local && project() != 0 && (m_calendar == 0 || m_calendar->isDeleted())) {
        return project()->defaultCalendar();
    }
    if (m_calendar && m_calendar->isDeleted()) {
        return 0;
    }
    return m_calendar;
}

DateTime Resource::getFirstAvailableTime(DateTime /*after*/) {
    return DateTime();
}

DateTime Resource::getBestAvailableTime(Duration /*duration*/) {
    return DateTime();
}

DateTime Resource::getBestAvailableTime(const DateTime /*after*/, const Duration /*duration*/) {
    return DateTime();
}

bool Resource::load(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    QString s;
    setId(element.attribute("id"));
    m_name = element.attribute("name");
    m_initials = element.attribute("initials");
    m_email = element.attribute("email");
    setType(element.attribute("type"));
    m_calendar = findCalendar(element.attribute("calendar-id"));
    m_units = element.attribute("units", "100").toInt();
    s = element.attribute("available-from");
    if (s != "")
        m_availableFrom = DateTime::fromString(s);
    s = element.attribute("available-until");
    if (s != "")
        m_availableUntil = DateTime::fromString(s);
        
    cost.normalRate = KGlobal::locale()->readMoney(element.attribute("normal-rate"));
    cost.overtimeRate = KGlobal::locale()->readMoney(element.attribute("overtime-rate"));
    return true;
}

void Resource::save(QDomElement &element) const {
    //kdDebug()<<k_funcinfo<<endl;
    QDomElement me = element.ownerDocument().createElement("resource");
    element.appendChild(me);

    if (calendar(true))
        me.setAttribute("calendar-id", m_calendar->id());
    me.setAttribute("id", m_id);
    me.setAttribute("name", m_name);
    me.setAttribute("initials", m_initials);
    me.setAttribute("email", m_email);
    me.setAttribute("type", typeToString());
    me.setAttribute("units", m_units);
    me.setAttribute("available-from", m_availableFrom.toString(Qt::ISODate));
    me.setAttribute("available-until", m_availableUntil.toString(Qt::ISODate));
    me.setAttribute("normal-rate", KGlobal::locale()->formatMoney(cost.normalRate));
    me.setAttribute("overtime-rate", KGlobal::locale()->formatMoney(cost.overtimeRate));
}

bool Resource::isAvailable(Task */*task*/) {
    bool busy = false;
/*    QPtrListIterator<Appointment> it(m_appointments);
    for (; it.current(); ++it) {
        if (it.current()->isBusy(task->startTime(), task->endTime())) {
            busy = true;
            break;
        }
    }*/
    return !busy;
}

QPtrList<Appointment> Resource::appointments() {
    QPtrList<Appointment> lst;
    if (m_currentSchedule)
        lst = m_currentSchedule->appointments();
    //kdDebug()<<k_funcinfo<<lst.count()<<endl;
    return lst;
}

Appointment *Resource::findAppointment(Node */*node*/) {
/*    QPtrListIterator<Appointment> it = m_appointments;
    for (; it.current(); ++it) {
        if (it.current()->node() == node)
            return it.current();
    }*/
    return 0;
}

bool Resource::addAppointment(Appointment *appointment) {
    if (m_currentSchedule)
        return m_currentSchedule->add(appointment);
    return false;
}

bool Resource::addAppointment(Appointment *appointment, Schedule &main) {
    Schedule *s = findSchedule(main.id());
    if (s == 0) {
        s = createSchedule(&main);
    }
    appointment->setResource(s);
    return s->add(appointment);
}

void Resource::addAppointment(Schedule *node, DateTime &start, DateTime &end, double load) {
    //kdDebug()<<k_funcinfo<<endl;
    Schedule *s = findSchedule(node->id());
    if (s == 0) {
        s = createSchedule(node->parent());
    }
    s->addAppointment(node, start, end, load);
}

void Resource::initiateCalculation(Schedule &sch) {
    m_currentSchedule = createSchedule(&sch);
}

void Resource::removeSchedule(Schedule *schedule) {
    takeSchedule(schedule);
    delete schedule;
}

void Resource::takeSchedule(const Schedule *schedule) {
    if (schedule == 0)
        return;
    if (m_currentSchedule == schedule)
        m_currentSchedule = 0;
    m_schedules.take(schedule->id());
}

void Resource::addSchedule(Schedule *schedule) {
    if (schedule == 0)
        return;
    m_schedules.replace(schedule->id(), schedule);
}

ResourceSchedule *Resource::createSchedule(QString name, int type, long id) {
    ResourceSchedule *sch = new ResourceSchedule(this, name, (Schedule::Type)type, id);
    addSchedule(sch);
    return sch;
}

ResourceSchedule *Resource::createSchedule(Schedule *parent) {
    ResourceSchedule *sch = new ResourceSchedule(parent, this);
    addSchedule(sch);
    return sch;
}

void Resource::makeAppointment(Schedule *node, const DateTime &from, const DateTime &end) {
    if (!from.isValid() || !end.isValid()) {
        kdWarning()<<k_funcinfo<<"Invalid time"<<endl;
        return;
    }
    Calendar *cal = calendar();
    if (cal == 0) {
        return;
    }
    DateTime time = from;
    while (time < end) {
        //kdDebug()<<k_funcinfo<<time.toString()<<" to "<<end.toString()<<endl;
        if (!time.isValid() || !end.isValid()) {
            kdWarning()<<k_funcinfo<<"Invalid time"<<endl;
            return;
        }
        if (!cal->hasInterval(time, end)) {
            //kdDebug()<<time.toString()<<" to "<<end.toString()<<": No (more) interval(s)"<<endl;
            kdWarning()<<k_funcinfo<<m_name<<": Resource only partially available"<<endl;
            //node->resourceNotAvailable = true;
            return; // nothing more to do
        }
        QPair<DateTime, DateTime> i = cal->firstInterval(time, end);
        if (!i.second.isValid()) {
            kdWarning()<<k_funcinfo<<"Invalid interval: "<<time<<", "<<end<<endl;
            return;
        }
        if (time == i.second)
            return; // hmmm, didn't get a new interval, avoid loop
        addAppointment(node, i.first, i.second, m_units);
        //kdDebug()<<k_funcinfo<<"Add :"<<i.first.toString()<<" to "<<i.second.toString()<<endl;
        if (!(node->workStartTime.isValid()) || i.first < node->workStartTime)
            node->workStartTime = i.first;
        if (!(node->workEndTime.isValid()) || i.second > node->workEndTime)
            node->workEndTime = i.second;
        time = i.second;
    }
    return;
}
void Resource::makeAppointment(Schedule *node) {
    //kdDebug()<<k_funcinfo<<m_name<< ": "<<node->node()->name()<<": "<<node->startTime.toString()<<" dur "<<node->duration.toString()<<endl;
    if (!node->startTime.isValid()) {
        kdWarning()<<k_funcinfo<<m_name<<": startTime invalid"<<endl;
        return;
    }
    if (!node->endTime.isValid()) {
        kdWarning()<<k_funcinfo<<m_name<<": endTime invalid"<<endl;
        return;
    }
    Calendar *cal = calendar();
    if (m_type == Type_Material) {
        DateTime from = availableAfter(node->startTime, node->endTime);
        DateTime end = availableBefore(node->endTime, node->startTime);
        if (!from.isValid() || !end.isValid()) {
            return;
        }
        if (cal == 0) {
            // Allocate the whole period
            addAppointment(node, from, end, m_units);
            return;
        }
        makeAppointment(node, from, end);
    }
    if (!cal) {
        kdWarning()<<k_funcinfo<<m_name<<": No calendar defined"<<endl;
        return; 
    }
    //TODO: units and standard non-working days
    DateTime time = node->startTime;
    DateTime end = node->endTime;
    time = availableAfter(time, end);
    if (!time.isValid()) {
        kdWarning()<<k_funcinfo<<m_name<<": Resource not available (after="<<node->startTime<<", "<<end<<")"<<endl;
        node->resourceNotAvailable = true;
        return;
    }
    end = availableBefore(end, time);
    if (!end.isValid()) {
        kdWarning()<<k_funcinfo<<m_name<<": Resource not available (before="<<node->endTime<<", "<<time<<")"<<endl;
        node->resourceNotAvailable = true;
        return;
    }
    //kdDebug()<<k_funcinfo<<time.toString()<<" to "<<end.toString()<<endl;
    makeAppointment(node, time, end);
}

// the amount of effort we can do within the duration
Duration Resource::effort(const DateTime &start, const Duration &duration, bool backward, bool *ok) const {
    //kdDebug()<<k_funcinfo<<m_name<<": "<<start.date().toString()<<" for duration "<<duration.toString(Duration::Format_Day)<<endl;
    bool sts=false;
    Duration e;
    if (duration == 0) {
        kdWarning()<<k_funcinfo<<"zero duration"<<endl;
        return e;
    }
    Calendar *cal = calendar();
    if (cal == 0) {
        kdWarning()<<k_funcinfo<<m_name<<": No calendar defined"<<endl;
        return e;
    }
    if (backward) {
        DateTime limit = start - duration;
        DateTime t = availableBefore(start, limit);
        if (t.isValid()) {
            sts = true;
            e = (cal->effort(limit, t) * m_units)/100;
        } else {
            //kdDebug()<<k_funcinfo<<m_name<<": Not available (start="<<start<<", "<<limit<<")"<<endl;
        }
    } else {
        DateTime limit = start + duration;
        DateTime t = availableAfter(start, limit);
        if (t.isValid()) {
            sts = true;
            e = (cal->effort(t, limit) * m_units)/100;
        }
    }
    //kdDebug()<<k_funcinfo<<start.toString()<<" e="<<e.toString(Duration::Format_Day)<<" ("<<m_units<<")"<<endl;
    if (ok) *ok = sts;
    return e;
}

DateTime Resource::availableAfter(const DateTime &time, const DateTime limit, bool checkAppointments) const {
    DateTime t;
    if (m_units == 0) {
        return t;
    }
    DateTime lmt = m_availableUntil;
    if (limit.isValid() && limit < lmt) {
        lmt = limit;
    }
    if (time >= lmt) {
        return t;
    }
    if (type() == Type_Material) {
        t = time > m_availableFrom ? time : m_availableFrom;
        //kdDebug()<<k_funcinfo<<time.toString()<<"="<<t.toString()<<" "<<m_name<<endl;
        return t;
    }
    Calendar *cal = calendar();
    if (cal == 0) {
        return t;
    }
    t = m_availableFrom > time ? m_availableFrom : time;
    t = cal->firstAvailableAfter(t, lmt);
    if (checkAppointments) {
        //TODO
    }
    //kdDebug()<<k_funcinfo<<time.toString()<<"="<<t.toString()<<" "<<m_name<<endl;
    return t;
}

DateTime Resource::availableBefore(const DateTime &time, const DateTime limit, bool checkAppointments) const {
    DateTime t;
    if (m_units == 0) {
        return t;
    }
    DateTime lmt = m_availableFrom;
    if (limit.isValid() && limit > lmt) {
        lmt = limit;
    }
    if (time <= lmt) {
        return t;
    }
    if (type() == Type_Material) {
        t = time < m_availableUntil ? time : m_availableUntil;
        //kdDebug()<<k_funcinfo<<time.toString()<<"="<<t.toString()<<" "<<m_name<<endl;
        return t;
    }
    Calendar *cal = calendar();
    if (cal == 0) {
        return t;
    }
    if (!m_availableUntil.isValid()) {
        kdWarning()<<k_funcinfo<<m_name<<": availabelUntil is invalid"<<endl;
        t = time;
    } else {
        t = m_availableUntil < time ? m_availableUntil : time;
    }
    //kdDebug()<<k_funcinfo<<t<<", "<<lmt<<endl;
    t = cal->firstAvailableBefore(t, lmt);
    if (checkAppointments) {
        //TODO
    }
    //kdDebug()<<k_funcinfo<<m_name<<" returns: "<<time<<"="<<t<<" "<<endl;
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
    //kdDebug()<<k_funcinfo<<m_name<<": "<<start.toString()<<" - "<<end.toString()<<" cs=("<<m_currentSchedule<<")"<<endl;
    return m_currentSchedule ? m_currentSchedule->isOverbooked(start, end) : false;
}

Appointment Resource::appointmentIntervals() const {
    Appointment a;
    if (m_currentSchedule == 0)
        return a;
    QPtrListIterator<Appointment> it = m_currentSchedule->appointments();
    for (; it.current(); ++it) {
        a += *(it.current());
    }
    return a;
}

Duration Resource::plannedEffort(const QDate &date) const {
    return m_currentSchedule ? m_currentSchedule->plannedEffort(date) : Duration::zeroDuration;
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
    //kdDebug()<<k_funcinfo<<"("<<this<<") Request to: "<<(resource ? resource->name() : QString("None"))<<endl;
}

ResourceRequest::~ResourceRequest() {
    //kdDebug()<<k_funcinfo<<"("<<this<<") resource: "<<(m_resource ? m_resource->name() : QString("None"))<<endl;
    if (m_resource)
        m_resource->unregisterRequest(this);
    m_resource = 0;
}

bool ResourceRequest::load(QDomElement &element, Project &project) {
    //kdDebug()<<k_funcinfo<<endl;
    m_resource = project.resource(element.attribute("resource-id"));
    if (m_resource == 0) {
        kdWarning()<<k_funcinfo<<"The referenced resource does not exist: resource id="<<element.attribute("resource-id")<<endl;
        return false;
    }
    m_units  = element.attribute("units").toInt();
    return true;
}

void ResourceRequest::save(QDomElement &element) const {
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

Task *ResourceRequest::task() const {
    return m_parent ? m_parent->task() : 0;
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
    //kdDebug()<<k_funcinfo<<"("<<request<<") to Group: "<<m_group->name()<<endl;
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
        //kdDebug()<<k_funcinfo<<"The referenced resource group does not exist: group id="<<element.attribute("group-id")<<endl;
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

void ResourceGroupRequest::save(QDomElement &element) const {
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
Duration ResourceGroupRequest::effort(const DateTime &time, const Duration &duration, bool backward, bool *ok) const {
    Duration e;
    bool sts=false;
    if (ok) *ok = sts;
    QPtrListIterator<ResourceRequest> it = m_resourceRequests;
    for (; it.current(); ++it) {
        e += it.current()->resource()->effort(time, duration, backward, &sts);
        if (sts && ok) *ok = sts;
        //kdDebug()<<k_funcinfo<<(backward?"(B)":"(F)" )<<it.current()->resource()->name()<<": time="<<time<<" dur="<<duration.toString()<<"gave e="<<e.toString()<<endl;
    }
    //kdDebug()<<k_funcinfo<<time.toString()<<"d="<<duration.toString()<<": e="<<e.toString()<<endl;
    return e;
}

int ResourceGroupRequest::numDays(const DateTime &time, bool backward) const {
    DateTime t1, t2 = time;
    if (backward) {
        QPtrListIterator<ResourceRequest> it = m_resourceRequests;
        for (; it.current(); ++it) {
            t1 = it.current()->resource()->availableFrom();
            if (!t2.isValid() || t2 > t1)
                t2 = t1;
        }
        //kdDebug()<<k_funcinfo<<"bw "<<time.toString()<<": "<<t2.daysTo(time)<<endl;
        return t2.daysTo(time);
    }
    QPtrListIterator<ResourceRequest> it = m_resourceRequests;
    for (; it.current(); ++it) {
        t1 = it.current()->resource()->availableUntil();
        if (!t2.isValid() || t2 < t1)
            t2 = t1;
    }
    //kdDebug()<<k_funcinfo<<"fw "<<time.toString()<<": "<<time.daysTo(t2)<<endl;
    return time.daysTo(t2);
}

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
    Duration d(1, 0, 0); // 1 day
    int nDays = numDays(time, backward) + 1;
    for (int i=0; !match && i <= nDays; ++i) {
        // days
        end = end.addDays(inc);
        e1 = effort(start, d, backward, &sts);
        //kdDebug()<<"["<<i<<"of"<<nDays<<"] "<<(backward?"(B)":"(F):")<<"  start="<<start<<" e+e1="<<(e+e1).toString()<<" match "<<_effort.toString()<<endl;
        if (e + e1 < _effort) {
            e += e1;
            start = end;
        } else if (e + e1 == _effort) {
            e += e1;
            match = true;
        } else {
            end = start;
            break;
        }
    }
    //kdDebug()<<"duration "<<(backward?"backward ":"forward: ")<<start.toString()<<" - "<<end.toString()<<" e="<<e.toString()<<" ("<<e.milliseconds()<<")  match="<<match<<" sts="<<sts<<endl;
    d = Duration(0, 1, 0); // 1 hour
    for (int i=0; !match && i < 24; ++i) {
        // hours
        end = end.addSecs(inc*60*60);
        e1 = effort(start, d, backward, &sts);
        if (e + e1 < _effort) {
            e += e1;
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
    d = Duration(0, 0, 1); // 1 minute
    for (int i=0; !match && i < 60; ++i) {
        //minutes
        end = end.addSecs(inc*60);
        e1 = effort(start, d, backward, &sts);
        if (e + e1 < _effort) {
            e += e1;
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
    d = Duration(0, 0, 0, 1); // 1 second
    for (int i=0; !match && i < 60 && sts; ++i) {
        //seconds
        end = end.addSecs(inc);
        e1 = effort(start, d, backward, &sts);
        if (e + e1 < _effort) {
            e += e1;
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
    d = Duration(0, 0, 0, 0, 1); // 1 millisecond
    for (int i=0; !match && i < 1000; ++i) {
        //milliseconds
        end.setTime(end.time().addMSecs(inc));
        e1 = effort(start, d, backward, &sts);
        if (e + e1 < _effort) {
            e += e1;
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
        kdError()<<k_funcinfo<<(task()?task()->name():"No task")<<" "<<time<<": Could not match effort."<<" Want: "<<_effort.toString(Duration::Format_Day)<<" got: "<<e.toString(Duration::Format_Day)<<" sts="<<sts<<endl;
    }
    DateTime t;
    if (e != Duration::zeroDuration) {
        t = backward ? availableAfter(end) : availableBefore(end);
    }
    end = t.isValid() ? t : time;
    //kdDebug()<<k_funcinfo<<"<---"<<(backward?"(B) ":"(F) ")<<m_group->name()<<": "<<end.toString()<<"-"<<time.toString()<<"="<<(end - time).toString()<<" effort: "<<_effort.toString(Duration::Format_Day)<<endl;
    return (end>time?end-time:time-end);
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
        if (t.isValid() && (!end.isValid() || t > end))
            end = t;
    }
    if (!end.isValid() || end > time)
        end = time;
    //kdDebug()<<k_funcinfo<<time.toString()<<"="<<end.toString()<<" "<<m_group->name()<<endl;
    return end;
}

void ResourceGroupRequest::makeAppointments(Schedule *schedule) {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<ResourceRequest> it = m_resourceRequests;
    for (; it.current(); ++it) {
        it.current()->makeAppointment(schedule);
    }
}

void ResourceGroupRequest::reserve(const DateTime &start, const Duration &duration) {
    m_start = start;
    m_duration = duration;
}

bool ResourceGroupRequest::isEmpty() const {
    return m_resourceRequests.isEmpty() && m_units == 0;
}

Task *ResourceGroupRequest::task() const {
    return m_parent ? &(m_parent->task()) : 0;
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

void ResourceRequestCollection::save(QDomElement &element) const {
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
        if (t.isValid() && (!start.isValid() || t < start))
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
        if (t.isValid() && (!end.isValid() ||t > end))
            end = t;
    }
    if (!end.isValid() || end > time)
        end = time;
    return end;
}


void ResourceRequestCollection::makeAppointments(Schedule *schedule) {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<ResourceGroupRequest> it(m_requests);
    for (; it.current(); ++it) {
        it.current()->makeAppointments(schedule);
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
    kdDebug()<<indent<<"  + Resource: "<<m_name<<" id="<<m_id/*<<" Overbooked="<<isOverbooked()*/<<endl;
    QIntDictIterator<Schedule> it(m_schedules);
    indent += "      ";
    for (; it.current(); ++it) {
        it.current()->printDebug(indent);
    }
    indent += "  !";
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
