/* This file is part of the KDE project
   Copyright (C) 2001 Thomas zander <zander@kde.org>

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

#include "kptresource.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptdatetime.h"
#include "kptcalendar.h"

#include <kdebug.h>

KPTResourceGroup::KPTResourceGroup(KPTProject *project) {
    m_project = project;
    m_id = -1;
    m_type = Type_Work;
    m_resources.setAutoDelete(true);
}

KPTResourceGroup::~KPTResourceGroup() {
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
    m_id = element.attribute("id", "-1").toInt();
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

    if (m_project)
        m_id = m_project->groupId(); // get a fresh id

    me.setAttribute("id", m_id);
    me.setAttribute("name", m_name);

    QPtrListIterator<KPTResource> it(m_resources);
    for ( ; it.current(); ++it ) {
        it.current()->save(me);
    }
}

void KPTResourceGroup::saveAppointments(QDomElement &element) const {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<KPTResource> it(m_resources);
    for ( ; it.current(); ++it ) {
        it.current()->saveAppointments(element);
    }
}

QPtrList<KPTAppointment> KPTResourceGroup::appointments(const KPTNode *node) const {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrList<KPTAppointment> a;
    QPtrListIterator<KPTResource> rit(m_resources);
    for ( ; rit.current(); ++rit ) {
        // hmmm, isn't it a better way?
        QPtrList<KPTAppointment> list = rit.current()->appointments(node);
        QPtrListIterator<KPTAppointment> it(list);
        for (; it.current(); ++it) {
            //kdDebug()<<k_funcinfo<<"Adding appointment"<<endl;
            a.append(it.current());
        }
    }
    //kdDebug()<<k_funcinfo<<"Now have: "<<a.count()<<" appointments"<<endl;
    return a;
}

void KPTResourceGroup::clearAppointments() {
    QPtrListIterator<KPTResource> it(m_resources);
    for (; it.current(); ++it) {
        it.current()->clearAppointments();
        it.current()->setOverbooked(false);
    }
    QPtrListIterator<KPTNode> nodes(m_nodes);
    for (; nodes.current(); ++nodes) {
        nodes.current()->setResourceOverbooked(false);
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

KPTResource::KPTResource(KPTProject *project) : m_project(project), m_appointments(), m_workingHours(), m_overbooked(false) {
    m_availableFrom = QTime(8,0,0);
    m_availableUntil = QTime(18,0,0);
    m_id = -1;
    m_type = Type_Work;
    m_units = 100; // %

    cost.normalRate = 100;
    cost.overtimeRate = 200;
    cost.fixed = 0;
    m_calendar = 0;
}

KPTResource::~KPTResource() {
    QPtrListIterator<KPTResourceRequest> it = m_requests;
    for (; it.current(); ++it) {
        it.current()->setResource(0); // avoid the request to mess with my list
        it.current()->parent()->removeResourceRequest(it.current()); // deletes the request
    }
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
    
    m_calendar = resource->calendar();
}

void KPTResource::addWorkingHour(QTime from, QTime until) {
    //kdDebug()<<k_funcinfo<<endl;
    m_workingHours.append(new QTime(from));
    m_workingHours.append(new QTime(until));
}

KPTCalendar *KPTResource::calendar() const {
    return m_calendar; //hmmm, default calendar?
}

KPTCalendar *KPTResource::calendar(int id) const {
    if (!m_project)
        return 0;
    return m_project->calendar(id); //hmmm, default calendar?
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
    m_id = element.attribute("id", "-1").toInt();
    m_name = element.attribute("name");
    m_calendar = calendar(element.attribute("calendar-id", "-1").toInt());
    return true;
}

void KPTResource::save(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    QDomElement me = element.ownerDocument().createElement("resource");
    element.appendChild(me);

    if (m_project)
        m_id = m_project->resourceId(); // get a fresh id

    if (m_calendar)
        me.setAttribute("calendar-id", m_calendar->id());
    me.setAttribute("id", m_id);
    me.setAttribute("name", m_name);

/* We save these after tasks...
    QPtrListIterator<KPTAppointment> it(m_appointments);
    for (; it.current(); ++it) {
        it.current()->save(me);
    }*/
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

QPtrList<KPTAppointment> KPTResource::appointments(const KPTNode *node) const {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrList<KPTAppointment> a;
    QPtrListIterator<KPTAppointment> it(m_appointments);
    for ( ; it.current(); ++it ) {
        if (it.current()->task() == node) {
            a.append(it.current());
            //kdDebug()<<k_funcinfo<<"Appointment added, resource="<<name()<<" node="<<node->name()<<endl;
        }
    }
    return a;
}


void KPTResource::addAppointment(KPTAppointment *a) {
    m_appointments.append(a);
}

void KPTResource::clearAppointments() {
    m_appointments.clear();
}

void KPTResource::makeAppointment(KPTDateTime &start, KPTDuration &duration, KPTTask *task) {
    //TODO: units and moderated by availability, and standard non-working days
    KPTDateTime time = start;
    KPTDateTime end = start+duration;
    while (time < end) {
        //kdDebug()<<k_funcinfo<<time.toString()<<" to "<<end.toString()<<endl;
        if (!m_calendar->hasInterval(time, end)) {
            //kdDebug()<<time.toString()<<" to "<<end.toString()<<": No (more) interval(s)"<<endl;
            return; // nothing more to do
        }
        QPair<KPTDateTime, KPTDateTime> i = m_calendar->interval(time, end);
        if (time == i.second)
            return; // hmmm, didn't get a new interval, avoid loop
        KPTAppointment *a = new KPTAppointment(i.first, i.second - i.first, this, task);
        m_appointments.append(a);
        //kdDebug()<<i.first.toString()<<" to "<<i.second.toString()<<": Made appointment"<<endl;
        time = i.second;
    }
}

void KPTResource::saveAppointments(QDomElement &element) const {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<KPTAppointment> it(m_appointments);
    for ( ; it.current(); ++it ) {
        it.current()->save(element);
    }
}

// the amount of effort we can do within the duration
KPTDuration KPTResource::effort(const KPTDateTime &start, const KPTDuration &duration) const {
    //kdDebug()<<k_funcinfo<<start.date().toString()<<" for duration "<<duration.toString(KPTDuration::Format_Day)<<endl;
    KPTDuration e;
    if (m_calendar)
        e = (m_calendar->effort(start, duration) * m_units)/100;
    
    //kdDebug()<<k_funcinfo<<"e="<<e.toString(KPTDuration::Format_Day)<<" ("<<m_units<<")"<<endl;
    return e;
}

KPTAppointment::KPTAppointment(KPTDateTime startTime, KPTDuration duration, KPTResource *resource, KPTTask *taskNode) :m_extraRepeats(), m_skipRepeats() {
    m_startTime=startTime;
    m_duration=duration;
    m_task=taskNode;
    m_resource=resource;
    m_repeatInterval=KPTDuration();
    m_repeatCount=0;
}

KPTAppointment::~KPTAppointment() {
}

void KPTAppointment::deleteAppointmentFromRepeatList(KPTDateTime time) {
}

void KPTAppointment::addAppointmentToRepeatList(KPTDateTime time) {
}

bool KPTAppointment::isBusy(const KPTDateTime &start, const KPTDateTime &end) {
    KPTDateTime finish(m_startTime + m_duration);
    return !(start > finish || end < m_startTime);
}

bool KPTAppointment::load(QDomElement &element, KPTProject &project) {
    //kdDebug()<<k_funcinfo<<endl;
    int id  = element.attribute("resource-id").toInt();
    if (!(m_resource = project.resource(id))) {
        kdError()<<k_funcinfo<<"The referenced resource does not exists: resource id="<<id<<endl;
        return false;
    }
    id  = element.attribute("task-id").toInt();
    if (!(m_task = dynamic_cast<KPTTask *>(project.node(id)))) {
        kdError()<<k_funcinfo<<"The referenced task does not exists: task id="<<id<<endl;
        return false;
    }
    m_startTime = KPTDateTime::fromString(element.attribute("start"));
    m_duration = KPTDuration::fromString(element.attribute("duration"));

    m_resource->addAppointment(this);
    return true;
}

void KPTAppointment::save(QDomElement &element) {
    QDomElement me = element.ownerDocument().createElement("appointment");
    element.appendChild(me);

    me.setAttribute("resource-id", m_resource->id());
    me.setAttribute("task-id", m_task->id());
    me.setAttribute("start", m_startTime.toString());
    me.setAttribute("duration", m_duration.toString());
}

// TODO: calendar stuff
double KPTAppointment::cost() {
    return m_duration.hours()/*FIXME*/ * m_resource->normalRate() + m_resource->fixedCost();
}

double KPTAppointment::cost(const KPTDateTime &dt) {
    if (dt < m_startTime)
        return 0;

    KPTDuration dur;
    if (dt >= m_startTime + m_duration)
        dur = m_duration;
    else
        dur = dt - m_startTime;

    return dur.hours()/*FIXME*/ * m_resource->normalRate() + m_resource->fixedCost();
}

int KPTAppointment::work() {
    return m_duration.hours(); /*FIXME*/
}

int KPTAppointment::work(const KPTDateTime &dt) {
    if (dt < m_startTime)
        return 0;

    KPTDuration dur;
    if (dt >= m_startTime + m_duration)
        dur = m_duration;
    else
        dur = dt - m_startTime;

    return dur.hours(); /*FIXME*/
}

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
    int id  = element.attribute("resource-id").toInt();
    if (!(m_resource = project->resource(id))) {
        kdDebug()<<k_funcinfo<<"The referenced resource does not exist: resource id="<<id<<endl;
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
    //kdDebug()<<k_funcinfo<<"units="<<m_units<<endl;
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
    m_resourceRequests.setAutoDelete(true);
}

KPTResourceGroupRequest::~KPTResourceGroupRequest() {
    //kdDebug()<<k_funcinfo<<"Group: "<<m_group->name()<<endl;
    m_resourceRequests.clear();
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
    int id  = element.attribute("group-id").toInt();
    if (!(m_group = project->group(id))) {
        kdDebug()<<k_funcinfo<<"The referenced resource group does not exist: group id="<<id<<endl;
        return false;
    }
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
KPTDuration KPTResourceGroupRequest::duration(const KPTDateTime &start, const KPTDuration &effort) {
    //kdDebug()<<k_funcinfo<<"effort: "<<effort.toString(KPTDuration::Format_Day)<<endl;
    KPTDuration dur = effort; // have to start somewhere
    KPTDuration down = dur/2;
    KPTDuration up = dur;
    KPTDuration e;
    bool match = false;
    for (int i=0; !match && i < 50; ++i) {
        // calculate new effort
        e = KPTDuration::zeroDuration;
        QPtrListIterator<KPTResourceRequest> it = m_resourceRequests;
        for (; it.current(); ++it) {
            e += it.current()->resource()->effort(start, dur);
            
        }
        //kdDebug()<<k_funcinfo<<"now e["<<i<<"]: "<<e.toString()<<" match: "<<effort.toString()<<endl;
        // TODO: we need to make this smarter
        if (e.isCloseTo(effort)) {
            match = true;
        } else {
            if (e > effort) {
                dur -= down;
                //kdDebug()<<k_funcinfo<<"down["<<i<<"]: "<<down.toString(KPTDuration::Format_Day)<<" new dur: "<<dur.toString(KPTDuration::Format_Day)<<endl;
                up = down/2;
                down = up;
            } else {
                dur += up;
                //kdDebug()<<k_funcinfo<<"up["<<i<<"]: "<<up.toString(KPTDuration::Format_Day)<<" new dur: "<<dur.toString(KPTDuration::Format_Day)<<endl;
            }
        }
    }
    if (!match) {
        kdError()<<k_funcinfo<<"Could not match effort."<<" Want: "<<effort.toString(KPTDuration::Format_Day)<<" got: "<<e.toString(KPTDuration::Format_Day)<<endl;
    }
    m_start = start;
    m_duration = dur;
    return dur;   
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

/////////
KPTResourceRequestCollection::KPTResourceRequestCollection() {
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

bool KPTResourceRequestCollection::load(QDomElement &element, KPTProject *project) {
    //kdDebug()<<k_funcinfo<<endl;
    return true;
}

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
KPTDuration KPTResourceRequestCollection::duration(const KPTDateTime &start, const KPTDuration &effort) {
    //kdDebug()<<k_funcinfo<<"start="<<start.toString()<<" effort="<<effort.toString(KPTDuration::Format_Day)<<endl;
    KPTDuration dur;
    int units = workUnits();
    if (units == 0)
        units = 100; //hmmmm
    QPtrListIterator<KPTResourceGroupRequest> it(m_requests);
    for (; it.current(); ++it) {
        if (it.current()->group()->type() == KPTResourceGroup::Type_Work) {
            KPTDuration d = it.current()->duration(start, effort*(it.current()->workUnits())/units);
            if (d > dur)
                dur = d;
        } else {
            //TODO
            if (dur == KPTDuration::zeroDuration)
                dur = effort;
        }
    }
    return dur;
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
    kdDebug()<<indent<<"  + Resource: "<<m_name<<" id="<<m_id<<" Overbooked="<<m_overbooked<<endl;
    QPtrListIterator<KPTAppointment> it(m_appointments);
    indent += "  !";
    for (; it.current(); ++it)
        it.current()->printDebug(indent);
    indent += "  !";
}

void KPTAppointment::printDebug(QString indent)
{
    kdDebug()<<indent<<"  + Appointment to task: "<<m_task->name()<<endl;
    indent += "  !";
    kdDebug()<<indent<<"      From: "<<m_startTime.toString()<<endl;
    kdDebug()<<indent<<"  Duration: "<<m_duration.toString()<<endl;
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

