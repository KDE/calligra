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

void KPTResourceGroup::makeAppointments() {
    //kdDebug()<<k_funcinfo<<endl;
    if (m_resources.count() == 0) {
        // TODO: Notify all nodes of resource shortage
        //kdDebug()<<k_funcinfo<<"No resources in this group: '"<<m_name<<"'"<<endl;
        return;
    }
    KPTResourceGroupRequest *request;
    QPtrListIterator<KPTNode> nodes(m_nodes);
    //kdDebug()<<k_funcinfo<<"No of nodes: "<<nodes.count()<<endl;
    for (; nodes.current(); ++nodes) {
        KPTTask *task = dynamic_cast<KPTTask *>(nodes.current());
        if (!task) {
            kdError()<<k_funcinfo<<"Node is not a task: '"<<nodes.current()->name()<<"'"<<endl;
            continue;
        }
        if ((request = task->resourceGroupRequest(this))) {
            // treat requests for specific resources first
            QPtrListIterator<KPTResourceRequest> reqs(request->resourceRequests());
            for (; reqs.current(); ++reqs) {
                if (!reqs.current()->resource()->isAvailable(task)) {
                    reqs.current()->resource()->setOverbooked(true);
                    task->setResourceOverbooked(true);
                }
                //kdDebug()<<k_funcinfo<<"Make app: "<<reqs.current()->resource()->name()<<" to "<<task->name()<<endl;
                reqs.current()->resource()->makeAppointment(task);
            }
            // Now the unspecified
            for (int i = 0; i < request->units(); ++i) { //FIXME: may not be in whole units in future
                bool madeIt = false;
                QPtrListIterator<KPTResource> rit(m_resources);
                for (; rit.current(); ++rit) {
                    if (rit.current()->isAvailable(task)) {
                        rit.current()->makeAppointment(task); // make appointment to task
                        madeIt = true;
                        //kdDebug()<<k_funcinfo<<"'"<<rit.current()->name()<<"' made appointment"<<endl;
                        break;
                    }
                }
                if (!madeIt) {
                    // No resource available, so... We book anyway and mark overbooked
                    int pos = 0;
                    if (i < m_resources.count())
                        pos = i;
                    m_resources.at(pos)->makeAppointment(task);
                    m_resources.at(pos)->setOverbooked(true);
                    task->setResourceOverbooked(true);
                    // TODO: tell the other task(s) also
                    //kdDebug()<<k_funcinfo<<"'"<<m_resources.at(pos)->name()<<"' overbooked"<<endl;
                }
            }
        } else {
            // hmmm, should not happen
            kdError()<<k_funcinfo<<"No resource requested!!"<<endl;
        }
    }
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
}

KPTResource::~KPTResource() {
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

    m_units = resource->units(); // avalable units in percent
    m_overbooked = resource->isOverbooked();

    m_type = resource->type();

    cost.normalRate = resource->normalRate();
    cost.overtimeRate = resource->overtimeRate();
    cost.fixed = resource->fixedCost();
}

void KPTResource::addWorkingHour(QTime from, QTime until) {
    //kdDebug()<<k_funcinfo<<endl;
    m_workingHours.append(new QTime(from));
    m_workingHours.append(new QTime(until));
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
    return true;
}

void KPTResource::save(QDomElement &element) {
    //kdDebug()<<k_funcinfo<<endl;
    QDomElement me = element.ownerDocument().createElement("resource");
    element.appendChild(me);

    if (m_project)
        m_id = m_project->resourceId(); // get a fresh id

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

void KPTResource::makeAppointment(KPTTask *task) {
    KPTAppointment *a = new KPTAppointment(task->startTime(), task->getDuration(), this, task);
    m_appointments.append(a);
}

void KPTResource::saveAppointments(QDomElement &element) const {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<KPTAppointment> it(m_appointments);
    for ( ; it.current(); ++it ) {
        it.current()->save(element);
    }
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
    m_units(units) {
    //kdDebug()<<k_funcinfo<<"Request to: "<<(resource ? resource->name() : QString("None"))<<endl;
}

KPTResourceRequest::~KPTResourceRequest() {
    //kdDebug()<<k_funcinfo<<"resource: "<<m_resource->name()<<endl;
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

KPTResourceGroupRequest::KPTResourceGroupRequest(KPTResourceGroup *group, int numResources)
    : m_group(group), m_units(numResources) {

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
    if (numResources() == 0)
        return;
    QDomElement me = element.ownerDocument().createElement("resourcegroup-request");
    element.appendChild(me);
    me.setAttribute("group-id", m_group->id());
    me.setAttribute("units", m_units);
    QPtrListIterator<KPTResourceRequest> it(m_resourceRequests);
    for (; it.current(); ++it)
        it.current()->save(me);
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
    kdDebug()<<indent<<"  + Request to group: "<<(m_group ? m_group->name() : "None")<<" units="<<m_units<<endl;
    indent += "  !";
    QPtrListIterator<KPTResourceRequest> it(m_resourceRequests);
    for (; it.current(); ++it) {
        it.current()->printDebug(indent);
    }
}

void KPTResourceRequest::printDebug(QString indent)
{
    kdDebug()<<indent<<"  + Request to resource: "<<(m_resource ? m_resource->name() : "None")<<" units="<<m_units<<endl;
}
#endif
