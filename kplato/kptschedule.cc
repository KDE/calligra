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
   Boston, MA 02110-1301, USA.
*/

#include "kptschedule.h"

#include "kptappointment.h"
#include "kptdatetime.h"
#include "kptduration.h"
#include "kptnode.h"

#include <qdom.h>
#include <qstring.h>

#include <klocale.h>
#include <kdebug.h>

namespace KPlato
{

Schedule::Schedule()
    : m_type(Expected),
      m_id(0),
      m_deleted(false) {
}

Schedule::Schedule(QString name, Type type, long id)
    : m_name(name),
      m_type(type),
      m_id(id),
      m_deleted(false),
      m_appointments() {

    //kdDebug()<<k_funcinfo<<"("<<this<<") Name: '"<<name<<"' Type="<<type<<" id="<<id<<endl;
    m_appointments.setAutoDelete(true);
}

Schedule::~Schedule() {
}

void Schedule::setType(const QString type) {
    m_type = Expected;
    if (type == "Expected")
        m_type = Expected;
    else if (type == "Optimistic")
        m_type = Optimistic;
    else if (type == "Pessimistic")
        m_type = Pessimistic;
}

QString Schedule::typeToString(bool translate) const {
    if (translate) {
        if (m_type == Expected)
            return i18n("Expected");
        if (m_type == Optimistic)
            return i18n("Optimistic");
        if (m_type == Pessimistic)
            return i18n("Pessimistic");
        return i18n("Expected");
    } else {
        if (m_type == Expected)
            return "Expected";
        if (m_type == Optimistic)
            return "Optimistic";
        if (m_type == Pessimistic)
            return "Pessimistic";
        return "Expected";
    }
}

bool Schedule::loadXML(const QDomElement &sch) {
    m_name = sch.attribute("name");
    setType(sch.attribute("type"));
    m_id = sch.attribute("id").toLong();
    
    return true;
}

void Schedule::saveXML(QDomElement &element) const {
    QDomElement sch = element.ownerDocument().createElement("schedule");
    element.appendChild(sch);
    saveCommonXML(sch);
}

void Schedule::saveCommonXML(QDomElement &element) const {
    //kdDebug()<<k_funcinfo<<m_name<<" save schedule"<<endl;
    element.setAttribute("name", m_name);
    element.setAttribute("type", typeToString());
    element.setAttribute("id", m_id);
}

void Schedule::saveAppointments(QDomElement &element) const {
    //kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<Appointment> it = m_appointments;
    for (; it.current(); ++it) {
        it.current()->saveXML(element);
    }
}
// Schedule *Schedule::find(const QIntDict<Schedule> &sch, Schedule::Type type) {
//     QIntDictIterator<Schedule> it = sch;
//     for (; it.current(); ++it) {
//         if (it.current()->isDeleted())
//             continue;
//         if (it.current()->type() == type)
//             return it.current();
//     }
//     return 0;
// }
// 
// Schedule *Schedule::find(const QIntDict<Schedule> &sch, QString name, Schedule::Type type) {
//     if (name.isEmpty()) {
//         return find(sch, type);
//     }
//     QIntDictIterator<Schedule> it = sch;
//     for (; it.current(); ++it) {
//         if (it.current()->isDeleted())
//             continue;
//         if (it.current()->type() == type && it.current()->name() == name)
//             return it.current();
//     }
//     return 0;
// }

bool Schedule::add(Appointment *appointment) {
    if (m_appointments.findRef(appointment) != -1) {
        kdError()<<k_funcinfo<<"Appointment allready exists"<<endl;
        return false;
    }
    m_appointments.append(appointment);
    //if (resource()) kdDebug()<<k_funcinfo<<"For resource '"<<resource()->name()<<"'"<<endl;
    //if (node()) kdDebug()<<k_funcinfo<<"For node '"<<node()->name()<<"'"<<endl;
    return true;
}

void Schedule::removeAppointment(Appointment *appointment) {
    takeAppointment(appointment);
    delete appointment;
}

void Schedule::takeAppointment(Appointment *appointment) {
    int i = m_appointments.findRef(appointment);
    if (i != -1) {
        m_appointments.take(i);
        //kdDebug()<<k_funcinfo<<"Taken: "<<appointment<<endl;
        if (appointment->node())
            appointment->node()->takeAppointment(appointment);
    } else {
        //kdDebug()<<k_funcinfo<<"Couldn't find appointment: "<<appointment<<endl;
    }
}

Appointment *Schedule::findAppointment(Schedule *resource, Schedule *node) {
    QPtrListIterator<Appointment> it = m_appointments;
    for (; it.current(); ++it) {
        if (it.current()->node() == node && it.current()->resource() == resource)
            return it.current();
    }
    return 0;
}

EffortCostMap Schedule::plannedEffortCostPrDay(const QDate &start, const QDate &end) const {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    EffortCostMap ec;
    QPtrListIterator<Appointment> it(m_appointments);
    for (; it.current(); ++it) {
        //kdDebug()<<k_funcinfo<<m_name<<endl;
        ec += it.current()->plannedPrDay(start, end);
    }
    return ec;
}

Duration Schedule::plannedEffort() const {
   //kdDebug()<<k_funcinfo<<endl;
    Duration eff;
    QPtrListIterator<Appointment> it(m_appointments);
    for (; it.current(); ++it) {
        eff += it.current()->plannedEffort();
    }
    return eff;
}

Duration Schedule::plannedEffort(const QDate &date) const {
   //kdDebug()<<k_funcinfo<<endl;
    Duration eff;
    QPtrListIterator<Appointment> it(m_appointments);
    for (; it.current(); ++it) {
        eff += it.current()->plannedEffort(date);
    }
    return eff;
}

Duration Schedule::plannedEffortTo(const QDate &date) const {
    //kdDebug()<<k_funcinfo<<endl;
    Duration eff;
    QPtrListIterator<Appointment> it(m_appointments);
    for (; it.current(); ++it) {
        eff += it.current()->plannedEffortTo(date);
    }
    return eff;
}

Duration Schedule::actualEffort() const {
    //kdDebug()<<k_funcinfo<<endl;
    Duration eff;
    QPtrListIterator<Appointment> it(m_appointments);
    for (; it.current(); ++it) {
        eff += it.current()->actualEffort();
    }
    return eff;
}

Duration Schedule::actualEffort(const QDate &date) const {
    //kdDebug()<<k_funcinfo<<endl;
    Duration eff;
    QPtrListIterator<Appointment> it(m_appointments);
    for (; it.current(); ++it) {
        eff += it.current()->actualEffort(date);
    }
    return eff;
}

Duration Schedule::actualEffortTo(const QDate &date) const {
    //kdDebug()<<k_funcinfo<<endl;
    Duration eff;
    QPtrListIterator<Appointment> it(m_appointments);
    for (; it.current(); ++it) {
        eff += it.current()->actualEffortTo(date);
    }
    return eff;
}

double Schedule::plannedCost() const {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    QPtrListIterator<Appointment> it(m_appointments);
    for (; it.current(); ++it) {
        c += it.current()->plannedCost();
    }
    return c;
}

double Schedule::plannedCost(const QDate &date) const {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    QPtrListIterator<Appointment> it(m_appointments);
    for (; it.current(); ++it) {
        c += it.current()->plannedCost(date);
    }
    return c;
}

double Schedule::plannedCostTo(const QDate &date) const {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    QPtrListIterator<Appointment> it(m_appointments);
    for (; it.current(); ++it) {
        c += it.current()->plannedCostTo(date);
    }
    return c;
}

double Schedule::actualCost() const {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    QPtrListIterator<Appointment> it(m_appointments);
    for (; it.current(); ++it) {
        c += it.current()->actualCost();
    }
    return c;
}

double Schedule::actualCost(const QDate &date) const {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    QPtrListIterator<Appointment> it(m_appointments);
    for (; it.current(); ++it) {
        c += it.current()->actualCost(date);
    }
    return c;
}

double Schedule::actualCostTo(const QDate &date) const {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    QPtrListIterator<Appointment> it(m_appointments);
    for (; it.current(); ++it) {
        c += it.current()->actualCostTo(date);
    }
    return c;
}


//----------------------------------------
NodeSchedule::NodeSchedule()
    : Schedule(),
      m_node(0) {
    //kdDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    init();
}

NodeSchedule::NodeSchedule(Node *node, QString name, Schedule::Type type, long id)
    : Schedule(name, type, id),
      m_node(node) {
    //kdDebug()<<k_funcinfo<<"node name: "<<node->name()<<endl;
    init();
}

NodeSchedule::~NodeSchedule() {
    //kdDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
}

void NodeSchedule::init() {
    resourceError = false;
    resourceOverbooked = false;
    resourceNotAvailable = false;
    schedulingError = false;
    notScheduled = true;
    inCriticalPath = false;
}

NodeSchedule *NodeSchedule::find(const QIntDict<NodeSchedule> &sch, Schedule::Type type) {
    QIntDictIterator<NodeSchedule> it = sch;
    for (; it.current(); ++it) {
        if (it.current()->isDeleted())
            continue;
        if (it.current()->type() == type)
            return it.current();
    }
    return 0;
}

NodeSchedule *NodeSchedule::find(const QIntDict<NodeSchedule> &sch, QString name, Schedule::Type type) {
    if (name.isEmpty()) {
        return find(sch, type);
    }
    QIntDictIterator<NodeSchedule> it = sch;
    for (; it.current(); ++it) {
        if (it.current()->isDeleted())
            continue;
        if (it.current()->type() == type && it.current()->name() == name)
            return it.current();
    }
    return 0;
}

bool NodeSchedule::loadXML(const QDomElement &sch) {
    //kdDebug()<<k_funcinfo<<endl;
    QString s;
    Schedule::loadXML(sch);
    s = sch.attribute("earlieststart");
    if (s != "")
        earliestStart = DateTime::fromString(s);
    s = sch.attribute("latestfinish");
    if (s != "")
        latestFinish = DateTime::fromString(s);
    s = sch.attribute("start");
    if (s != "")
        startTime = DateTime::fromString(s);
    s = sch.attribute("end");
    if (s != "")
        endTime = DateTime::fromString(s);
    s = sch.attribute("start-work");
    if (s != "")
        workStartTime = DateTime::fromString(s);
    s = sch.attribute("end-work");
    if (s != "")
        workEndTime = DateTime::fromString(s);
    duration = Duration::fromString(sch.attribute("duration"));
    
    inCriticalPath = sch.attribute("in-critical-path", "0").toInt();
    resourceError = sch.attribute("resource-error", "0").toInt();
    resourceOverbooked = sch.attribute("resource-overbooked", "0").toInt();
    schedulingError = sch.attribute("scheduling-conflict", "0").toInt();
    notScheduled = sch.attribute("not-scheduled", "1").toInt();

    return true;
}

void NodeSchedule::saveXML(QDomElement &element) const {
    //kdDebug()<<k_funcinfo<<endl;
    QDomElement sch = element.ownerDocument().createElement("schedule");
    element.appendChild(sch);
    saveCommonXML(sch);
    
    if (earliestStart.isValid())
        sch.setAttribute("earlieststart",earliestStart.toString());
    if (latestFinish.isValid())
        sch.setAttribute("latestfinish",latestFinish.toString());
    if (startTime.isValid())
        sch.setAttribute("start",startTime.toString());
    if (endTime.isValid())
        sch.setAttribute("end",endTime.toString());
    if (workStartTime.isValid())
        sch.setAttribute("start-work", workStartTime.toString());
    if (workEndTime.isValid())
        sch.setAttribute("end-work", workEndTime.toString());
    
    sch.setAttribute("duration",duration.toString());

    sch.setAttribute("in-critical-path",inCriticalPath);
    sch.setAttribute("resource-error",resourceError);
    sch.setAttribute("resource-overbooked",resourceOverbooked);
    sch.setAttribute("scheduling-conflict",schedulingError);
    sch.setAttribute("not-scheduled",notScheduled);
}

bool NodeSchedule::loadProjectXML(const QDomElement &sch, Project &project) {
    //kdDebug()<<k_funcinfo<<endl;
    QString s;
    Schedule::loadXML(sch);
    
    s = sch.attribute("start");
    if (s != "")
        startTime = DateTime::fromString(s);
    s = sch.attribute("end");
    if (s != "")
        endTime = DateTime::fromString(s);
    
    QDomNodeList al = sch.childNodes();
    //kdDebug()<<k_funcinfo<<"No of appointments: "<<al.count()<<endl;
    for (unsigned int i=0; i<al.count(); ++i) {
        if (al.item(i).isElement()) {
            QDomElement app = al.item(i).toElement();
            if (app.tagName() == "appointment") {
                // Load the appointments. 
                // Resources and tasks must allready loaded
                Appointment *child = new Appointment();
                if (!child->loadXML(app, project, *this)) {
                    // TODO: Complain about this
                    kdError()<<k_funcinfo<<"Failed to load appointment"<<endl;
                    delete child;
                }
            }
        }
    }
    return true;
}

void NodeSchedule::saveProjectXML(QDomElement &element) const {
    saveCommonXML(element);
    
    element.setAttribute("start",startTime.toString());
    element.setAttribute("end",endTime.toString());
}

void NodeSchedule::initiateCalculation() {
    resourceError = false;
    resourceOverbooked = false;
    schedulingError = false;
    inCriticalPath = false;
    workStartTime = DateTime();
    workEndTime = DateTime();
}

void NodeSchedule::addAppointment(Schedule *resource, DateTime &start, DateTime &end, double load) {
    //kdDebug()<<k_funcinfo<<endl;
    Appointment *a = findAppointment(resource, this);
    if (a != 0) {
        //kdDebug()<<k_funcinfo<<"Add interval"<<endl;
        a->addInterval(start, end, load);
        return;
    }
    a = new Appointment(resource, this, start, end, load);
    if (!add(a)) {
        delete a;
    }
    if (!resource->add(a)) {
        delete a;
    }
}


void NodeSchedule::calcResourceOverbooked() {
    resourceOverbooked = false;
    QPtrListIterator<Appointment> it = m_appointments;
    for (; it.current(); ++it) {
        if (it.current()->resource()->isOverbooked(startTime, endTime)) {
            resourceOverbooked = true;
            break;
        }
    }
}


//----------------------------------------
ResourceSchedule::ResourceSchedule()
    : Schedule(),
      m_resource(0) {
    //kdDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
}

ResourceSchedule::ResourceSchedule(Resource *resource, QString name, Schedule::Type type, long id)
    : Schedule(name, type, id),
      m_resource(resource) {
    //kdDebug()<<k_funcinfo<<"resource: "<<resource->name()<<endl;
}

ResourceSchedule::~ResourceSchedule() {
    //kdDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
}

void ResourceSchedule::addAppointment(Schedule *node, DateTime &start, DateTime &end, double load) {
    //kdDebug()<<k_funcinfo<<endl;
    Appointment *a = findAppointment(this, node);
    if (a != 0) {
        //kdDebug()<<k_funcinfo<<"Add interval"<<endl;
        a->addInterval(start, end, load);
        return;
    }
    a = new Appointment(this, node, start, end, load);
    if (!add(a)) {
        delete a;
    }
    if (!node->add(a)) {
        delete a;
    }
}

bool ResourceSchedule::isOverbooked() const {
    return false;
}

bool ResourceSchedule::isOverbooked(const DateTime &start, const DateTime &end) const {
    if (m_resource == 0)
        return false;
    //kdDebug()<<k_funcinfo<<start.toString()<<" - "<<end.toString()<<endl;
    Appointment a = appointmentIntervals();
    QPtrListIterator<AppointmentInterval> it = a.intervals();
    for (; it.current(); ++it) {
        if ((!end.isValid() || it.current()->startTime() < end) && 
            (!start.isValid() || it.current()->endTime() > start)) 
        {
            if (it.current()->load() > m_resource->units()) {
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

Appointment ResourceSchedule::appointmentIntervals() const {
    Appointment a;
    QPtrListIterator<Appointment> it = m_appointments;
    for (; it.current(); ++it) {
        a += *(it.current());
    }
    return a;
}

double ResourceSchedule::normalRatePrHour() const {
    return m_resource ? m_resource->normalRate() : 0.0;
}

#ifndef NDEBUG
void Schedule::printDebug(QString indent) {
    kdDebug()<<indent<<"Schedule["<<m_id<<"] '"<<m_name<<"' type: "<<typeToString()<<" ("<<m_type<<")"<<endl;
    if (m_deleted)
        kdDebug()<<indent<<"Deleted"<<endl;
}
void NodeSchedule::printDebug(QString indent) {
    Schedule::printDebug(indent);
    indent += "!  ";
    if (!notScheduled) {
        if (node()) kdDebug()<<indent<<"Node: "<<node()->name()<<endl;
        else kdDebug()<<indent<<"No parent node!"<<endl;
    }
    kdDebug()<<indent<<"Not scheduled="<<notScheduled<<endl;
    kdDebug()<<indent<<"Start time: "<<startTime.toString()<<endl;
    kdDebug()<<indent<<"End time: " <<endTime.toString()<<endl;
    kdDebug()<<indent<<"Duration: "<<duration.seconds()<<QCString(" secs")<<" ("<<duration.toString()<<")"<<endl;
    kdDebug()<<indent<<"Earliest start: "<<earliestStart.toString()<<endl;
    kdDebug()<<indent<<"Latest finish: " <<latestFinish.toString()<<endl;

    kdDebug()<<indent<<"Resource overbooked="<<resourceOverbooked<<endl;
    kdDebug()<<indent<<"resourceError="<<resourceError<<endl;
    kdDebug()<<indent<<"schedulingError="<<schedulingError<<endl;
    kdDebug()<<indent<<"resourceNotAvailable="<<resourceNotAvailable<<endl;
    kdDebug()<<indent<<"inCriticalPath="<<inCriticalPath<<endl;
    kdDebug()<<indent<<endl;
    kdDebug()<<indent<<"workStartTime="<<workStartTime.toString()<<endl;
    kdDebug()<<indent<<"workEndTime="<<workEndTime.toString()<<endl;
    kdDebug()<<indent<<endl;
    kdDebug()<<indent<<"Appointments: "<<m_appointments.count()<<endl;
    QPtrListIterator<Appointment> it = m_appointments;
    for (; it.current(); ++it) {
        it.current()->printDebug(indent + "  ");
    }
}
void ResourceSchedule::printDebug(QString indent) {
    Schedule::printDebug(indent);
    indent += "!  ";
    if (resource()) kdDebug()<<indent<<"Resource: "<<resource()->name()<<endl;
    else kdDebug()<<indent<<"No parent resource!"<<endl;
    kdDebug()<<indent<<endl;
    kdDebug()<<indent<<"Appointments: "<<m_appointments.count()<<endl;
}
#endif

} //namespace KPlato

