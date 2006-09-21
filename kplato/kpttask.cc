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

#include "kpttask.h"
#include "kptproject.h"
#include "kpttaskdialog.h"
#include "kptduration.h"
#include "kptrelation.h"
#include "kptdatetime.h"
#include "kptcalendar.h"
#include "kpteffortcostmap.h"
#include "kptschedule.h"

#include <qdom.h>
#include <qbrush.h>
//Added by qt3to4:
#include <Q3PtrList>
#include <kdebug.h>

namespace KPlato
{

Task::Task(Node *parent) : Node(parent), m_resource() {
    //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    m_resource.setAutoDelete(true);
    Duration d(1, 0, 0);
    m_effort = new Effort(d);
    m_effort->setOptimisticRatio(-10);
    m_effort->setPessimisticRatio(20);
    m_requests = 0;

    if (m_parent)
        m_leader = m_parent->leader();
    
    m_schedules.setAutoDelete(true);
    m_parentProxyRelations.setAutoDelete(true);
    m_childProxyRelations.setAutoDelete(true);
}

Task::Task(Task &task, Node *parent) 
    : Node(task, parent), 
      m_resource() {
    //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    m_resource.setAutoDelete(true);
    
    m_parentProxyRelations.setAutoDelete(true);
    m_childProxyRelations.setAutoDelete(true);
    m_requests = 0;
    
    m_effort = task.effort() ? new Effort(*(task.effort())) 
                             : new Effort(); // Avoid crash, (shouldn't be zero)
}


Task::~Task() {
    delete m_effort;
}

int Task::type() const {
	if ( numChildren() > 0) {
	  return Node::Type_Summarytask;
	}
	else if ( 0 == effort()->expected().seconds() ) {
		return Node::Type_Milestone;
	}
	else {
		return Node::Type_Task;
	}
}



Duration *Task::getExpectedDuration() {
    //kDebug()<<k_funcinfo<<endl;
    // Duration should already be calculated
    return m_currentSchedule ? new Duration(m_currentSchedule->duration) : new Duration();
}

Duration *Task::getRandomDuration() {
    return 0L;
}

ResourceGroupRequest *Task::resourceGroupRequest(ResourceGroup *group) const {
    if (m_requests)
        return m_requests->find(group);
    return 0;
}

void Task::clearResourceRequests() {
    if (m_requests)
        m_requests->clear();
}

void Task::addRequest(ResourceGroup *group, int numResources) {
    addRequest(new ResourceGroupRequest(group, numResources));
}

void Task::addRequest(ResourceGroupRequest *request) {
    if (!m_requests)
        m_requests = new ResourceRequestCollection(*this);
    m_requests->addRequest(request);
}

void Task::takeRequest(ResourceGroupRequest *request) {
    if (m_requests) {
        m_requests->takeRequest(request);
        if (m_requests->isEmpty()) {
            delete m_requests;
            m_requests = 0;
        }
    }
}

int Task::units() const {
    if (!m_requests)
        return 0;
    return m_requests->units();
}

int Task::workUnits() const {
    if (!m_requests)
        return 0;
    return m_requests->workUnits();
}

void Task::makeAppointments() {
    if (m_currentSchedule == 0)
        return;
    if (type() == Node::Type_Task) {
        if (m_requests) {
            //kDebug()<<k_funcinfo<<m_name<<": "<<m_currentSchedule->startTime<<", "<<m_currentSchedule->endTime<<"; "<<m_currentSchedule->duration.toString()<<endl;
            m_requests->makeAppointments(m_currentSchedule);
            //kDebug()<<k_funcinfo<<m_name<<": "<<m_currentSchedule->startTime<<", "<<m_currentSchedule->endTime<<"; "<<m_currentSchedule->duration.toString()<<endl;
        }
    } else if (type() == Node::Type_Summarytask) {
        Q3PtrListIterator<Node> nit(m_nodes);
        for ( ; nit.current(); ++nit ) {
            nit.current()->makeAppointments();
        }
    } else if (type() == Node::Type_Milestone) {
        //kDebug()<<k_funcinfo<<"Milestone not implemented"<<endl;
        // Well, shouldn't have resources anyway...
    }
}

void Task::calcResourceOverbooked() {
    if (m_currentSchedule)
        m_currentSchedule->calcResourceOverbooked();
}

// A new constraint means start/end times and duration must be recalculated
void Task::setConstraint(Node::ConstraintType type) {
    m_constraint = type;
}


bool Task::load(QDomElement &element, Project &project) {
    // Load attributes (TODO: Handle different types of tasks, milestone, summary...)
    QString s;
    bool ok = false;
    m_id = element.attribute("id");
    
    m_name = element.attribute("name");
    m_leader = element.attribute("leader");
    m_description = element.attribute("description");
    //kDebug()<<k_funcinfo<<m_name<<": id="<<m_id<<endl;

    // Allow for both numeric and text
    QString constraint = element.attribute("scheduling","0");
    m_constraint = (Node::ConstraintType)constraint.toInt(&ok);
    if (!ok)
        Node::setConstraint(constraint); // hmmm, why do I need Node::?

    s = element.attribute("constraint-starttime");
    if (s != "")
        m_constraintStartTime = DateTime::fromString(s);
    s = element.attribute("constraint-endtime");
    if ( s != "")
        m_constraintEndTime = DateTime::fromString(s);
    
    m_startupCost = element.attribute("startup-cost", "0.0").toDouble();
    m_shutdownCost = element.attribute("shutdown-cost", "0.0").toDouble();
    
    m_wbs = element.attribute("wbs", "");
    
    // Load the project children
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();

            if (e.tagName() == "project") {
                // Load the subproject
                Project *child = new Project(this);
                if (child->load(e)) {
                    if (!project.addSubTask(child, this)) {
                        delete child;  // TODO: Complain about this
                    }
                } else {
                    // TODO: Complain about this
                    delete child;
                }
            } else if (e.tagName() == "task") {
                // Load the task
                Task *child = new Task(this);
                if (child->load(e, project)) {
                    if (!project.addSubTask(child, this)) {
                        delete child;  // TODO: Complain about this
                    }
                } else {
                    // TODO: Complain about this
                    delete child;
                }
            } else if (e.tagName() == "resource") {
                // TODO: Load the resource (projects don't have resources yet)
            } else if (e.tagName() == "effort") {
                //  Load the effort
                m_effort->load(e);
            } else if (e.tagName() == "resourcegroup-request") {
                // Load the resource request
                ResourceGroupRequest *r = new ResourceGroupRequest();
                if (r->load(e, project)) {
                    addRequest(r);
                } else {
                    kError()<<k_funcinfo<<"Failed to load resource request"<<endl;
                    delete r;
                }
            } else if (e.tagName() == "progress") {
                m_progress.started = (bool)e.attribute("started", "0").toInt();
                m_progress.finished = (bool)e.attribute("finished", "0").toInt();
                
                s = e.attribute("startTime");
                if (s != "")
                    m_progress.startTime = DateTime::fromString(s);
                s = e.attribute("finishTime");
                if (s != "")
                    m_progress.finishTime = DateTime::fromString(s);
                m_progress.percentFinished = e.attribute("percent-finished", "0").toInt();
                m_progress.remainingEffort = Duration::fromString(e.attribute("remaining-effort"));
                m_progress.totalPerformed = Duration::fromString(e.attribute("performed-effort"));
            } else if (e.tagName() == "schedules") {
                QDomNodeList lst = e.childNodes();
                for (unsigned int i=0; i<lst.count(); ++i) {
                    if (lst.item(i).isElement()) {
                        QDomElement el = lst.item(i).toElement();
                        if (el.tagName() == "schedule") {
                            NodeSchedule *sch = new NodeSchedule();
                            if (sch->loadXML(el)) {
                                sch->setNode(this);
                                addSchedule(sch);
                            } else {
                                kError()<<k_funcinfo<<"Failed to load schedule"<<endl;
                                delete sch;
                            }
                        }
                    }
                }
            }
        }
    }
    //kDebug()<<k_funcinfo<<m_name<<" loaded"<<endl;
    return true;
}


void Task::save(QDomElement &element)  const {
    QDomElement me = element.ownerDocument().createElement("task");
    element.appendChild(me);

    //TODO: Handle different types of tasks, milestone, summary...
    me.setAttribute("id", m_id);
    me.setAttribute("name", m_name);
    me.setAttribute("leader", m_leader);
    me.setAttribute("description", m_description);

    me.setAttribute("scheduling",constraintToString());
    me.setAttribute("constraint-starttime",m_constraintStartTime.toString(Qt::ISODate));
    me.setAttribute("constraint-endtime",m_constraintEndTime.toString(Qt::ISODate));    

    me.setAttribute("startup-cost", m_startupCost);
    me.setAttribute("shutdown-cost", m_shutdownCost);
    
    me.setAttribute("wbs", m_wbs);
    
    m_effort->save(me);

    QDomElement el = me.ownerDocument().createElement("progress");
    me.appendChild(el);
    el.setAttribute("started", m_progress.started);
    el.setAttribute("finished", m_progress.finished);
    el.setAttribute("startTime", m_progress.startTime.toString(Qt::ISODate));
    el.setAttribute("finishTime", m_progress.finishTime.toString(Qt::ISODate));
    el.setAttribute("percent-finished", m_progress.percentFinished);
    el.setAttribute("remaining-effort", m_progress.remainingEffort.toString());
    el.setAttribute("performed-effort", m_progress.totalPerformed.toString());
    
    if (!m_schedules.isEmpty()) {
        QDomElement schs = me.ownerDocument().createElement("schedules");
        me.appendChild(schs);
        Q3IntDictIterator<Schedule> it = m_schedules;
        for (; it.current(); ++it) {
            if (!it.current()->isDeleted()) {
                it.current()->saveXML(schs);
            }
        }
    }
    if (m_requests) {
        m_requests->save(me);
    }
    for (int i=0; i<numChildren(); i++) {
        getChildNode(i)->save(me);
    }
}

void Task::saveAppointments(QDomElement &element, long id) const {
    //kDebug()<<k_funcinfo<<m_name<<" id="<<id<<endl;
    Schedule *sch = findSchedule(id);
    if (sch) {
        sch->saveAppointments(element);
    }
    Q3PtrListIterator<Node> it(m_nodes);
    for (; it.current(); ++it ) {
        it.current()->saveAppointments(element, id);
    }
}

EffortCostMap Task::plannedEffortCostPrDay(const QDate &start, const QDate &end) const {
    //kDebug()<<k_funcinfo<<m_name<<endl;
    if (m_currentSchedule) {
        return m_currentSchedule->plannedEffortCostPrDay(start, end);
    }
    return EffortCostMap();
}

// Returns the total planned effort for this task (or subtasks) 
Duration Task::plannedEffort() {
   //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        Q3PtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            eff += it.current()->plannedEffort();
        }
    } else if (m_currentSchedule) {
        eff = m_currentSchedule->plannedEffort();
    }
    return eff;
}

// Returns the total planned effort for this task (or subtasks) on date
Duration Task::plannedEffort(const QDate &date) {
   //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        Q3PtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            eff += it.current()->plannedEffort(date);
        }
    } else if (m_currentSchedule) {
        eff = m_currentSchedule->plannedEffort(date);
    }
    return eff;
}

// Returns the total planned effort for this task (or subtasks) upto and including date
Duration Task::plannedEffortTo(const QDate &date) {
    //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        Q3PtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            eff += it.current()->plannedEffortTo(date);
        }
    } else if (m_currentSchedule) {
        eff = m_currentSchedule->plannedEffortTo(date);
    }
    return eff;
}

// Returns the total planned effort for this task (or subtasks) 
Duration Task::actualEffort() {
   //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        Q3PtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            eff += it.current()->actualEffort();
        }
    } else {
        eff = m_progress.totalPerformed;
    }
    /* If we want to register pr resource...
    } else if (m_currentSchedule) {
        eff = m_currentSchedule->actualEffort();
    }*/
    return eff;
}

// Returns the total planned effort for this task (or subtasks) on date
Duration Task::actualEffort(const QDate &date) {
   //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        Q3PtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            eff += it.current()->actualEffort(date);
        }
    } else if (m_currentSchedule) {
        eff = m_currentSchedule->actualEffort(date);
    }
    return eff;
}

// Returns the total planned effort for this task (or subtasks) on date
Duration Task::actualEffortTo(const QDate &date) {
   //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        Q3PtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            eff += it.current()->actualEffortTo(date);
        }
    } else if (m_currentSchedule) {
        eff = m_currentSchedule->actualEffortTo(date);
    }
    return eff;
}

double Task::plannedCost() {
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        Q3PtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            c += it.current()->plannedCost();
        }
    } else if (m_currentSchedule) {
        c = m_currentSchedule->plannedCost();
    }
    return c;
}

double Task::plannedCost(const QDate &date) {
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        Q3PtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            c += it.current()->plannedCost(date);
        }
    } else if (m_currentSchedule) {
        c = m_currentSchedule->plannedCost(date);
    }
    return c;
}

double Task::plannedCostTo(const QDate &date) {
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        Q3PtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            c += it.current()->plannedCostTo(date);
        }
    } else if (m_currentSchedule) {
        c = m_currentSchedule->plannedCostTo(date);
    }
    return c;
}

double Task::actualCost() {
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        Q3PtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            c += it.current()->actualCost();
        }
    } else if (m_currentSchedule) {
        c = m_currentSchedule->actualCost();
    }
    return c;
}

double Task::actualCost(const QDate &date) {
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        Q3PtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            c += it.current()->actualCost(date);
        }
    } else if (m_currentSchedule) {
        c = m_currentSchedule->actualCost(date);
    }
    return c;
}

double Task::actualCostTo(const QDate &date) {
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        Q3PtrListIterator<Node> it(childNodeIterator());
        for (; it.current(); ++it) {
            c += it.current()->actualCostTo(date);
        }
    } else if (m_currentSchedule) {
        c = m_currentSchedule->actualCostTo(date);
    }
    return c;
}

//FIXME Handle summarytasks
double Task::effortPerformanceIndex(const QDate &date, bool *error) {
    double res = 0.0;
    Duration ae = actualEffortTo(date);
    
    bool e = (ae == Duration::zeroDuration || m_progress.percentFinished == 0);
    if (error) {
        *error = e;
    }
    if (!e) {
        res = (plannedEffortTo(date).toDouble() * ((double)m_progress.percentFinished/100.0) / ae.toDouble());
    }
    return res;
}

//FIXME Handle summarytasks
double Task::costPerformanceIndex(const QDate &date, bool *error) {
    double res = 0.0;
    Duration ac = qint64(actualCostTo(date));
    
    bool e = (ac == Duration::zeroDuration || m_progress.percentFinished == 0);
    if (error) {
        *error = e;
    }
    if (!e) {
        res = (plannedCostTo(date) * m_progress.percentFinished)/(100 * actualCostTo(date));
    }
    return res;
}

void Task::initiateCalculation(Schedule &sch) {
    //kDebug()<<k_funcinfo<<m_name<<" schedule: "<<(sch?sch->name():"None")<<" id="<<(sch?sch->id():-1)<<endl;
    m_visitedForward = false;
    m_visitedBackward = false;
    m_currentSchedule = createSchedule(&sch);
    m_currentSchedule->initiateCalculation();
    clearProxyRelations();
    Node::initiateCalculation(sch);
}


void Task::initiateCalculationLists(Q3PtrList<Node> &startnodes, Q3PtrList<Node> &endnodes, Q3PtrList<Node> &summarytasks/*, QPtrList<Node> &milestones*/) {
    //kDebug()<<k_funcinfo<<m_name<<endl;
    if (type() == Node::Type_Summarytask) {
        summarytasks.append(this);
        // propagate my relations to my children and dependent nodes
        
        Q3PtrListIterator<Node> nodes = m_nodes;
        for (; nodes.current(); ++nodes) {
            if (!dependParentNodes().isEmpty()) 
                nodes.current()->addParentProxyRelations(dependParentNodes());
            if (!dependChildNodes().isEmpty()) 
                nodes.current()->addChildProxyRelations(dependChildNodes());
            nodes.current()->initiateCalculationLists(startnodes, endnodes, summarytasks);
        }        
    } else {
        if (isEndNode()) {
            endnodes.append(this);
            //kDebug()<<k_funcinfo<<"endnodes append: "<<m_name<<endl;
        }
        if (isStartNode()) {
            startnodes.append(this);
            //kDebug()<<k_funcinfo<<"startnodes append: "<<m_name<<endl;
        }
    }
}

DateTime Task::calculatePredeccessors(const Q3PtrList<Relation> &list, int use) {
    DateTime time;
    Q3PtrListIterator<Relation> it = list;
    for (; it.current(); ++it) {
        if (it.current()->parent()->type() == Type_Summarytask) {
            //kDebug()<<k_funcinfo<<"Skip summarytask: "<<it.current()->parent()->name()<<endl;
            continue; // skip summarytasks
        }
        DateTime t = it.current()->parent()->calculateForward(use); // early finish
        switch (it.current()->type()) {
            case Relation::StartStart:
                // I can't start earlier than my predesseccor
                t = it.current()->parent()->getEarliestStart() + it.current()->lag();
                break;
            case Relation::FinishFinish:
                // I can't finish earlier than my predeccessor, so
                // I can't start earlier than it's (earlyfinish+lag)- my duration
                t += it.current()->lag();
                t -= duration(t, use, true);
                break;
            default:
                t += it.current()->lag();
                break;
        }
        if (!time.isValid() || t > time)
            time = t;
    }
    //kDebug()<<time.toString()<<"                  "<<m_name<<" calculatePredeccessors() ("<<list.count()<<")"<<endl;
    return time;
}
DateTime Task::calculateForward(int use) {
    //kDebug()<<k_funcinfo<<m_name<<<<endl;
    if (m_currentSchedule == 0) {
        return DateTime();
    }
    Schedule *cs = m_currentSchedule;
    if (m_visitedForward) {
    //kDebug()<<earliestStart.toString()<<" + "<<m_durationBackward.toString()<<" "<<m_name<<" calculateForward() (visited)"<<endl;
        return cs->earliestStart + m_durationForward;
    }
    // First, calculate all predecessors
    if (!dependParentNodes().isEmpty()) {
        DateTime time = calculatePredeccessors(dependParentNodes(), use);
        if (time.isValid() && time > cs->earliestStart) {
            cs->earliestStart = time;
        }
    }
    if (!m_parentProxyRelations.isEmpty()) {
        DateTime time = calculatePredeccessors(m_parentProxyRelations, use);
        if (time.isValid() && time > cs->earliestStart) {
            cs->earliestStart = time;
        }
    }
    if (type() == Node::Type_Task) {
        m_durationForward = m_effort->effort(use);
        switch (constraint()) {
            case Node::ASAP:
            case Node::ALAP:
                if (m_effort->type() == Effort::Type_Effort) {
                    DateTime t = workStartAfter(cs->earliestStart);
                    if (t.isValid())
                         cs->earliestStart = t;
                }
                m_durationForward = duration(cs->earliestStart, use, false);
                //kDebug()<<k_funcinfo<<m_name<<": "<<cs->earliestStart<<"+"<<m_durationForward.toString()<<"="<<(cs->earliestStart+m_durationForward)<<endl;
                break;
            case Node::MustFinishOn:
                m_durationForward = duration(m_constraintEndTime, use, true);
                cs->earliestStart = m_constraintEndTime - m_durationForward;
                break;
            case Node::FinishNotLater:
                m_durationForward = duration(cs->earliestStart, use, false);
                if (cs->earliestStart + m_durationForward > m_constraintEndTime) {
                    m_durationForward = duration(m_constraintEndTime, use, true);
                    cs->earliestStart = m_constraintEndTime - m_durationForward;
                }
                break;
            case Node::MustStartOn:
                cs->earliestStart = m_constraintStartTime;
                m_durationForward = duration(cs->earliestStart, use, false);
                break;
            case Node::StartNotEarlier:
                if (cs->earliestStart < m_constraintStartTime) {
                    cs->earliestStart = m_constraintStartTime;
                }
                m_durationForward = duration(cs->earliestStart, use, false);
                break;
            case Node::FixedInterval: {
                cs->earliestStart = m_constraintStartTime;
                m_durationForward = m_constraintEndTime - m_constraintStartTime;
                break;
            }
        }
    } else if (type() == Node::Type_Milestone) {
        m_durationForward = Duration::zeroDuration;
        switch (constraint()) {
            case Node::MustFinishOn:
                cs->earliestStart = m_constraintEndTime;
                break;
            case Node::FinishNotLater:
                if (cs->earliestStart > m_constraintEndTime) {
                    cs->earliestStart = m_constraintEndTime;
                }
                break;
            case Node::MustStartOn:
                cs->earliestStart = m_constraintStartTime;
                break;
            case Node::StartNotEarlier:
                if (cs->earliestStart < m_constraintStartTime) {
                    cs->earliestStart = m_constraintStartTime;
                }
                break;
            case Node::FixedInterval:
                cs->earliestStart = m_constraintStartTime;
                break;
            default:
                break;
        }
        //kDebug()<<k_funcinfo<<m_name<<" "<<earliestStart.toString()<<endl
    } else if (type() == Node::Type_Summarytask) {
        kWarning()<<k_funcinfo<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    } else { // ???
        m_durationForward = Duration::zeroDuration;
    }
    
    //kDebug()<<"Earlyfinish: "<<cs->earliestStart<<"+"<<m_durationForward.toString()<<"="<<(cs->earliestStart+m_durationForward)<<" "<<m_name<<" calculateForward()"<<endl;
    m_visitedForward = true;
    return cs->earliestStart + m_durationForward;
}

DateTime Task::calculateSuccessors(const Q3PtrList<Relation> &list, int use) {
    DateTime time;
    Q3PtrListIterator<Relation> it = list;
    for (; it.current(); ++it) {
        if (it.current()->child()->type() == Type_Summarytask) {
            //kDebug()<<k_funcinfo<<"Skip summarytask: "<<it.current()->parent()->name()<<endl;
            continue; // skip summarytasks
        }
        DateTime t = it.current()->child()->calculateBackward(use);
        switch (it.current()->type()) {
            case Relation::StartStart:
                // I must start before my successor, so
                // I can't finish later than it's (starttime-lag) + my duration
                t -= it.current()->lag();
                t += duration(t, use, false);
                break;
            case Relation::FinishFinish:
                // My successor cannot finish before me, so
                // I can't finish later than it's latest finish - lag
                t = it.current()->child()->getLatestFinish() -  it.current()->lag();
                break;
            default:
                t -= it.current()->lag();
                break;
        }
        if (!time.isValid() || t < time)
            time = t;
    }
    //kDebug()<<time.toString()<<"                  "<<m_name<<" calculateSuccessors() ("<<list.count()<<")"<<endl;
    return time;
}
DateTime Task::calculateBackward(int use) {
    //kDebug()<<k_funcinfo<<m_name<<endl;
    if (m_currentSchedule == 0) {
        return DateTime();
    }
    Schedule *cs = m_currentSchedule;
    if (m_visitedBackward) {
    //kDebug()<<latestFinish.toString()<<" - "<<m_durationBackward.toString()<<" "<<m_name<<" calculateBackward() (visited)"<<endl;
        return cs->latestFinish - m_durationBackward;
    }
    // First, calculate all successors
    if (!dependChildNodes().isEmpty()) {
        DateTime time = calculateSuccessors(dependChildNodes(), use);
        if (time.isValid() && time < cs->latestFinish) {
            cs->latestFinish = time;
        }
    }
    if (!m_childProxyRelations.isEmpty()) {
        DateTime time = calculateSuccessors(m_childProxyRelations, use);
        if (time.isValid() && time < cs->latestFinish) {
            cs->latestFinish = time;
        }
    }
    //kDebug()<<k_funcinfo<<m_name<<": latestFinish="<<cs->latestFinish<<endl;
    if (type() == Node::Type_Task) {
        m_durationBackward = m_effort->effort(use);
        switch (constraint()) {
            case Node::ASAP:
            case Node::ALAP:
                if (m_effort->type() == Effort::Type_Effort) {
                    DateTime t = workFinishBefore(cs->latestFinish);
                    //kDebug()<<k_funcinfo<<m_name<<": latestFinish="<<cs->latestFinish<<" t="<<t<<endl;
                    if (t.isValid()) {
                        cs->latestFinish = t;
                    }
                }
                m_durationBackward = duration(cs->latestFinish, use, true);
                break;
            case Node::MustStartOn:
                m_durationBackward = duration(m_constraintStartTime, use, false);
                cs->latestFinish = m_constraintStartTime + m_durationBackward;
                break;
            case Node::StartNotEarlier:
                m_durationBackward = duration(cs->latestFinish, use, true);
                if (cs->latestFinish - m_durationBackward < m_constraintStartTime) {
                    m_durationBackward = duration(m_constraintStartTime, use, false);
                    cs->latestFinish = m_constraintStartTime + m_durationBackward;
                }
                break;
            case Node::MustFinishOn:
                cs->latestFinish = m_constraintEndTime;
                m_durationBackward = duration(cs->latestFinish, use, true);
                break;
            case Node::FinishNotLater:
                if (cs->latestFinish > m_constraintEndTime) {
                    cs->latestFinish = m_constraintEndTime;
                }
                m_durationBackward = duration(cs->latestFinish, use, true);
                break;
            case Node::FixedInterval: {
                cs->latestFinish = m_constraintEndTime;
                m_durationBackward = m_constraintEndTime - m_constraintStartTime;
                break;
            }
        }
    } else if (type() == Node::Type_Milestone) {
        m_durationBackward = Duration::zeroDuration;
        switch (constraint()) {
            case Node::MustFinishOn:
                cs->latestFinish = m_constraintEndTime;
                break;
            case Node::FinishNotLater:
                if (cs->latestFinish > m_constraintEndTime) {
                    cs->latestFinish = m_constraintEndTime;
                }
                break;
            case Node::MustStartOn:
                cs->latestFinish = m_constraintStartTime;
                break;
            case Node::StartNotEarlier:
                if (cs->latestFinish < m_constraintStartTime) {
                    cs->latestFinish = m_constraintStartTime;
                }
                break;
            case Node::FixedInterval:
                cs->latestFinish = m_constraintEndTime;
                break;
            default:
                break;
        }
        //kDebug()<<k_funcinfo<<m_name<<" "<<cs->latestFinish<<endl;
    } else if (type() == Node::Type_Summarytask) {
        kWarning()<<k_funcinfo<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    } else { // ???
        m_durationBackward = Duration::zeroDuration;
    }
    //kDebug()<<"Latestart: "<<cs->latestFinish<<"-"<<m_durationBackward.toString()<<"="<<(cs->latestFinish-m_durationBackward).toString()<<" "<<m_name<<" calculateBackward()"<<endl;
    m_visitedBackward = true;
    return cs->latestFinish - m_durationBackward;
}

DateTime Task::schedulePredeccessors(const Q3PtrList<Relation> &list, int use) {
    DateTime time;
    Q3PtrListIterator<Relation> it = list;
    for (; it.current(); ++it) {
        if (it.current()->parent()->type() == Type_Summarytask) {
            //kDebug()<<k_funcinfo<<"Skip summarytask: "<<it.current()->parent()->name()<<endl;
            continue; // skip summarytasks
        }
        // schedule the predecessors
        DateTime earliest = it.current()->parent()->getEarliestStart();
        DateTime t = it.current()->parent()->scheduleForward(earliest, use);
        switch (it.current()->type()) {
            case Relation::StartStart:
                // I can't start before my predesseccor
                t = it.current()->parent()->startTime() + it.current()->lag();
                break;
            case Relation::FinishFinish:
                // I can't end before my predecessor, so
                // I can't start before it's endtime - my duration
                t -= duration(t + it.current()->lag(), use, true);
                break;
            default:
                t += it.current()->lag();
                break;
        }
        if (!time.isValid() || t > time)
            time = t;
    }
    //kDebug()<<time.toString()<<" "<<m_name<<" schedulePredeccessors()"<<endl;
    return time;
}

DateTime Task::scheduleForward(const DateTime &earliest, int use) {
    //kDebug()<<k_funcinfo<<m_name<<" earliest="<<earliest<<endl;
    if (m_currentSchedule == 0) {
        return DateTime();
    }
    Schedule *cs = m_currentSchedule;
    if (m_visitedForward) {
        return cs->endTime;
    }
    cs->notScheduled = false;
    cs->startTime = earliest > cs->earliestStart ? earliest : cs->earliestStart;
    // First, calculate all my own predecessors
    DateTime time = schedulePredeccessors(dependParentNodes(), use);
    if (time.isValid() && time > cs->startTime) {
        cs->startTime = time;
        //kDebug()<<k_funcinfo<<m_name<<" new startime="<<cs->startTime<<endl;
    }
    // Then my parents
    time = schedulePredeccessors(m_parentProxyRelations, use);
    if (time.isValid() && time > cs->startTime) {
        cs->startTime = time;
        //kDebug()<<k_funcinfo<<m_name<<" new startime="<<cs->startTime<<endl;
    }
    //kDebug()<<k_funcinfo<<m_name<<" startTime="<<cs->startTime<<endl;
    if(type() == Node::Type_Task) {
        cs->duration = m_effort->effort(use);
        switch (m_constraint) {
        case Node::ASAP:
            // cs->startTime calculated above
            //kDebug()<<k_funcinfo<<m_name<<" startTime="<<cs->startTime<<endl;
            if (m_effort->type() == Effort::Type_Effort) {
                DateTime t = workStartAfter(cs->startTime);
                if (t.isValid())
                    cs->startTime = t;
            }
            cs->duration = duration(cs->startTime, use, false);
            cs->endTime = cs->startTime + cs->duration;
            //kDebug()<<k_funcinfo<<m_name<<" startTime="<<cs->startTime<<endl;
            break;
        case Node::ALAP:
            // cd->startTime calculated above
            cs->duration = duration(cs->latestFinish, use, true);
            cs->endTime = cs->latestFinish;
            cs->startTime = cs->endTime - cs->duration;
            //kDebug()<<k_funcinfo<<m_name<<" endTime="<<cs->endTime<<" latest="<<cs->latestFinish<<endl;
            break;
        case Node::StartNotEarlier:
            // cs->startTime calculated above
            //kDebug()<<"StartNotEarlier="<<m_constraintStartTime.toString()<<" "<<cd->startTime.toString()<<endl;            
            if (cs->startTime < m_constraintStartTime) {
                cs->startTime = m_constraintStartTime;
            }
            if (m_effort->type() == Effort::Type_Effort) {
                DateTime t = workStartAfter(cs->startTime);
                if (t.isValid())
                    cs->startTime = t;
            }
            cs->duration = duration(cs->startTime, use, false);
            cs->endTime = cs->startTime + cs->duration;
            if (cs->endTime > cs->latestFinish) {
                cs->schedulingError = true;
            }
            break;
        case Node::FinishNotLater:
            // cs->startTime calculated above
            //kDebug()<<"FinishNotLater="<<m_constraintEndTime.toString()<<" "<<cs->startTime.toString()<<endl;
            cs->duration = duration(cs->startTime, use, false);
            cs->endTime = cs->startTime + cs->duration;
            if (cs->endTime > m_constraintEndTime) {
                cs->schedulingError = true;
                cs->endTime = m_constraintEndTime;
                cs->duration = duration(cs->endTime, use, true);
                cs->startTime = cs->endTime - cs->duration;
            }
            break;
        case Node::MustStartOn:
            // cs->startTime calculated above
            //kDebug()<<"MustStartOn="<<m_constraintStartTime.toString()<<" "<<cs->startTime.toString()<<endl;
            if (m_constraintStartTime < cs->startTime ||
                m_constraintStartTime > cs->latestFinish - m_durationBackward) {
                cs->schedulingError = true;
            }
            cs->startTime = m_constraintStartTime;
            cs->duration = duration(cs->startTime, use, false);
            cs->endTime = cs->startTime + cs->duration;
            break;
        case Node::MustFinishOn:
            // cs->startTime calculated above
            //kDebug()<<"MustFinishOn="<<m_constraintEndTime.toString()<<" "<<cs->startTime.toString()<<endl;
            if (m_constraintEndTime > cs->latestFinish ||
                m_constraintEndTime < cs->earliestStart + m_durationForward) {
                cs->schedulingError = true;
            }
            cs->endTime = m_constraintEndTime;
            cs->duration = duration(cs->endTime, use, true);
            cs->startTime = cs->endTime - cs->duration;
            break;
        case Node::FixedInterval: {
            // cs->startTime calculated above
            //kDebug()<<"FixedInterval="<<m_constraintStartTime<<" "<<cs->startTime<<endl;
            if (cs->startTime < cs->earliestStart) {
                cs->schedulingError = true;
            }
            cs->startTime = m_constraintStartTime;
            cs->endTime = m_constraintEndTime;
            cs->duration = cs->endTime - cs->startTime;
            cs->workStartTime = m_constraintStartTime;
            cs->workEndTime = m_constraintEndTime;
            //kDebug()<<"FixedInterval="<<cs->startTime<<", "<<cs->endTime<<endl;
            break;
        }
        default:
            break;
        }
        if (m_requests) {
            m_requests->reserve(cs->startTime, cs->duration);
        }
    } else if (type() == Node::Type_Milestone) {
        switch (m_constraint) {
        case Node::ASAP: {
            cs->endTime = cs->startTime;
            break;
        }
        case Node::ALAP: {
            cs->startTime = cs->latestFinish;
            cs->endTime = cs->latestFinish;
            break;
        }
        case Node::MustStartOn:
        case Node::FixedInterval:
            //kDebug()<<"Forw, MustStartOn: "<<m_constraintStartTime.toString()<<" "<<cs->startTime.toString()<<endl;
            if (m_constraintStartTime < cs->startTime ||
                m_constraintStartTime > cs->latestFinish) {
                cs->schedulingError = true;
            }
            cs->startTime = m_constraintStartTime;
            cs->endTime = m_constraintStartTime;
            break;
        case Node::MustFinishOn:
            if (m_constraintEndTime < cs->startTime ||
                m_constraintEndTime > cs->latestFinish) {
                cs->schedulingError = true;
            }
            cs->startTime = m_constraintEndTime;
            cs->endTime = m_constraintEndTime;
            break;
        case Node::StartNotEarlier:
            if (cs->startTime < m_constraintStartTime) {
                cs->schedulingError = true;
            }
            cs->endTime = cs->startTime;
            break;
        case Node::FinishNotLater:
            if (cs->startTime > m_constraintEndTime) {
                cs->schedulingError = true;
            }
            cs->endTime = cs->startTime;
            break;
        default:
            break;
        }
        cs->duration = Duration::zeroDuration;
        //kDebug()<<k_funcinfo<<m_name<<": "<<cs->startTime<<", "<<cs->endTime<<endl;
    } else if (type() == Node::Type_Summarytask) {
        //shouldn't come here
        cs->endTime = cs->startTime;
        cs->duration = cs->endTime - cs->startTime;
        kWarning()<<k_funcinfo<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    }
    //kDebug()<<cs->startTime.toString()<<" : "<<cs->endTime.toString()<<" "<<m_name<<" scheduleForward()"<<endl;
    m_visitedForward = true;
    return cs->endTime;
}

DateTime Task::scheduleSuccessors(const Q3PtrList<Relation> &list, int use) {
    DateTime time;
    Q3PtrListIterator<Relation> it = list;
    for (; it.current(); ++it) {
        if (it.current()->child()->type() == Type_Summarytask) {
            //kDebug()<<k_funcinfo<<"Skip summarytask: "<<it.current()->child()->name()<<endl;
            continue;
        }
        // get the successors starttime
        DateTime latest = it.current()->child()->getLatestFinish();
        DateTime t = it.current()->child()->scheduleBackward(latest, use);
        switch (it.current()->type()) {
            case Relation::StartStart:
                // I can't start before my successor, so
                // I can't finish later than it's starttime + my duration
                t += duration(t - it.current()->lag(), use, false);
                break;
            case Relation::FinishFinish:
                t = it.current()->child()->endTime() - it.current()->lag();
                break;
            default:
                t -= it.current()->lag();
                break;
        }
        if (!time.isValid() || t < time)
            time = t;
   }
   return time;
}
DateTime Task::scheduleBackward(const DateTime &latest, int use) {
    //kDebug()<<k_funcinfo<<m_name<<": latest="<<latest<<endl;
    if (m_currentSchedule == 0) {
        return DateTime();
    }
    Schedule *cs = m_currentSchedule;
    if (m_visitedBackward) {
        return cs->startTime;
    }
    cs->notScheduled = false;
    cs->endTime = latest < cs->latestFinish ? latest : cs->latestFinish;
    // First, calculate all my own successors
    DateTime time = scheduleSuccessors(dependChildNodes(), use);
    if (time.isValid() && time < cs->endTime) {
        cs->endTime = time;
    }
    // Then my parents
    time = scheduleSuccessors(m_childProxyRelations, use);
    if (time.isValid() && time < cs->endTime) {
        cs->endTime = time;
    }
    if (type() == Node::Type_Task) {
        cs->duration = m_effort->effort(use);
        switch (m_constraint) {
        case Node::ASAP: {
            // cs->endTime calculated above
            //kDebug()<<k_funcinfo<<m_name<<": end="<<cs->endTime<<"  early="<<cs->earliestStart<<endl;
            if (m_effort->type() == Effort::Type_Effort) {
                DateTime t = workFinishBefore(cs->endTime);
                //kDebug()<<k_funcinfo<<m_name<<": end="<<cs->endTime<<" t="<<t<<endl;
                if (t.isValid())
                    cs->endTime = t;
            }
            cs->duration = duration(cs->earliestStart, use, false);
            cs->startTime = cs->earliestStart;
            DateTime e = cs->startTime + cs->duration;
            if (e > cs->endTime) {
                cs->schedulingError = true;
            }
            cs->endTime = e;
            //kDebug()<<k_funcinfo<<m_name<<": start="<<cs->startTime<<"+"<<cs->duration.toString()<<"="<<e<<" -> end="<<cs->endTime<<endl;
            break;
        }
        case Node::ALAP:
            // cs->endTime calculated above
            //kDebug()<<k_funcinfo<<m_name<<": end="<<cs->endTime<<"  late="<<cs->latestFinish<<endl;
            if (m_effort->type() == Effort::Type_Effort) {
                DateTime t = workFinishBefore(cs->endTime);
                //kDebug()<<k_funcinfo<<m_name<<": end="<<cs->endTime<<" t="<<t<<endl;
                if (t.isValid())
                    cs->endTime = t;
            }
            cs->duration = duration(cs->endTime, use, true);
            cs->startTime = cs->endTime - cs->duration;
            //kDebug()<<k_funcinfo<<m_name<<": lateStart="<<cs->startTime<<endl;
            break;
        case Node::StartNotEarlier:
            // cs->endTime calculated above
            //kDebug()<<"StartNotEarlier="<<m_constraintStartTime.toString()<<" "<<cs->endTime.toString()<<endl;
            if (m_effort->type() == Effort::Type_Effort) {
                DateTime t = workFinishBefore(cs->endTime);
                //kDebug()<<k_funcinfo<<m_name<<": end="<<cs->endTime<<" t="<<t<<endl;
                if (t.isValid())
                    cs->endTime = t;
            }
            cs->duration = duration(cs->endTime, use, true);
            cs->startTime = cs->endTime - cs->duration;
            if (cs->startTime < m_constraintStartTime) {
                cs->schedulingError = true;
                cs->startTime = m_constraintStartTime;
                cs->duration = duration(cs->startTime, use, false);
                cs->endTime = cs->startTime + cs->duration;
            }
            break;
        case Node::FinishNotLater:
            // cs->endTime calculated above
            //kDebug()<<"FinishNotLater="<<m_constraintEndTime.toString()<<" "<<cs->endTime.toString()<<endl;            
            if (cs->endTime > m_constraintEndTime) {
                cs->schedulingError = true;
                cs->endTime = m_constraintEndTime;
            }
            if (m_effort->type() == Effort::Type_Effort) {
                DateTime t = workFinishBefore(cs->endTime);
                //kDebug()<<k_funcinfo<<m_name<<": end="<<cs->endTime<<" t="<<t<<endl;
                if (t.isValid())
                    cs->endTime = t;
            }
            cs->duration = duration(cs->endTime, use, true);
            cs->startTime = cs->endTime - cs->duration;
            break;
        case Node::MustStartOn:
            // cs->endTime calculated above
            //kDebug()<<"MustStartOn="<<m_constraintStartTime.toString()<<" "<<cs->startTime.toString()<<endl;
            if (m_constraintStartTime < cs->earliestStart ||
                m_constraintStartTime > cs->latestFinish - m_durationBackward) {
                cs->schedulingError = true;
            }
            cs->startTime = m_constraintStartTime;
            cs->duration = duration(cs->startTime, use, false);
            cs->endTime = cs->startTime + cs->duration;
            break;
        case Node::MustFinishOn:
            // cs->endTime calculated above
            //kDebug()<<"MustFinishOn="<<m_constraintEndTime.toString()<<" "<<cs->startTime.toString()<<endl;
            if (m_constraintEndTime > cs->latestFinish ||
                m_constraintEndTime < cs->earliestStart + m_durationForward) {
                cs->schedulingError = true;
            }
            cs->endTime = m_constraintEndTime;
            cs->duration = duration(cs->endTime, use, true);
            cs->startTime = cs->endTime - cs->duration;
            break;
        case Node::FixedInterval: {
            // cs->endTime calculated above
            //kDebug()<<k_funcinfo<<"FixedInterval="<<m_constraintEndTime<<" "<<cs->endTime<<endl;
            if (m_constraintEndTime > cs->endTime) {
                cs->schedulingError = true;
                //kDebug()<<k_funcinfo<<"FixedInterval error: "<<m_constraintEndTime<<" >  "<<cs->endTime<<endl;
            }
            cs->startTime = m_constraintStartTime;
            cs->endTime = m_constraintEndTime;
            cs->duration = cs->endTime - cs->startTime;
            cs->workStartTime = m_constraintStartTime;
            cs->workEndTime = m_constraintEndTime;
            break;
        }
        default:
            break;
        }
        if (m_requests) {
            m_requests->reserve(cs->startTime, cs->duration);
        }
    } else if (type() == Node::Type_Milestone) {
        switch (m_constraint) {
        case Node::ASAP:
            cs->startTime = cs->earliestStart;
            cs->endTime = cs->earliestStart;
            break;
        case Node::ALAP:
            cs->startTime = cs->latestFinish;
            cs->endTime = cs->latestFinish;
            break;
        case Node::MustStartOn:
        case Node::FixedInterval:
            if (m_constraintStartTime < cs->earliestStart ||
                m_constraintStartTime > cs->endTime) {
                cs->schedulingError = true;
            }
            cs->startTime = cs->earliestStart;
            cs->endTime = cs->earliestStart;
            break;
        case Node::MustFinishOn:
            if (m_constraintEndTime < cs->earliestStart ||
                m_constraintEndTime > cs->endTime) {
                cs->schedulingError = true;
            }
            cs->startTime = cs->earliestStart;
            cs->endTime = cs->earliestStart;
            break;
        case Node::StartNotEarlier:
            if (m_constraintStartTime > cs->endTime) {
                cs->schedulingError = true;
            }
            cs->startTime = cs->endTime;
            break;
        case Node::FinishNotLater:
            if (m_constraintEndTime < cs->endTime) {
                cs->schedulingError = true;
            }
            cs->startTime = cs->endTime;
            break;
        default:
            break;
        }
        cs->duration = Duration::zeroDuration;
    } else if (type() == Node::Type_Summarytask) {
        //shouldn't come here
        cs->startTime = cs->endTime;
        cs->duration = cs->endTime - cs->startTime;
        kWarning()<<k_funcinfo<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    }
    //kDebug()<<k_funcinfo<<m_name<<": "<<cs->startTime.toString()<<" : "<<cs->endTime.toString()<<endl;
    m_visitedBackward = true;
    return cs->startTime;
}

void Task::adjustSummarytask() {
    if (m_currentSchedule == 0)
        return;
    if (type() == Type_Summarytask) {
        DateTime start = m_currentSchedule->latestFinish;
        DateTime end = m_currentSchedule->earliestStart;
        Q3PtrListIterator<Node> it(m_nodes);
        for (; it.current(); ++it) {
            it.current()->adjustSummarytask();
            if (it.current()->startTime() < start)
                start = it.current()->startTime();
            if (it.current()->endTime() > end)
                end = it.current()->endTime();
        }
        m_currentSchedule->startTime = start;
        m_currentSchedule->endTime = end;
        m_currentSchedule->duration = end - start;
        m_currentSchedule->notScheduled = false;
        //kDebug()<<k_funcinfo<<cs->name<<": "<<m_currentSchedule->startTime.toString()<<" : "<<m_currentSchedule->endTime.toString()<<endl;
    }
}

Duration Task::calcDuration(const DateTime &time, const Duration &effort, bool backward) {
    //kDebug()<<"--------> calcDuration "<<(backward?"(B) ":"(F) ")<<m_name<<" time="<<time<<" effort="<<effort.toString(Duration::Format_Day)<<endl;
    
    // Allready checked: m_effort, m_currentSchedule and time.
    Duration dur = effort; // use effort as default duration
    if (m_effort->type() == Effort::Type_Effort) {
        if (m_requests == 0 || m_requests->isEmpty()) {
            m_currentSchedule->resourceError = true;
            return effort;
        }
        dur = m_requests->duration(time, effort, backward);
        if (dur == Duration::zeroDuration) {
            kWarning()<<k_funcinfo<<"zero duration: Resource not available"<<endl;
            m_currentSchedule->resourceNotAvailable = true;
            dur = effort; //???
        }
        return dur;
    }
    if (m_effort->type() == Effort::Type_FixedDuration) {
        //TODO: Different types of fixed duration
        return dur; //
    }
    kError()<<k_funcinfo<<"Unsupported effort type: "<<m_effort->type()<<endl;
    return dur;
}

void Task::clearProxyRelations() {
    m_parentProxyRelations.clear();
    m_childProxyRelations.clear();
}

void Task::addParentProxyRelations(Q3PtrList<Relation> &list) {
    //kDebug()<<k_funcinfo<<m_name<<endl;
    if (type() == Type_Summarytask) {
        // propagate to my children
        //kDebug()<<k_funcinfo<<m_name<<" is summary task"<<endl;
        Q3PtrListIterator<Node> nodes = m_nodes;
        for (; nodes.current(); ++nodes) {
            nodes.current()->addParentProxyRelations(list);
            nodes.current()->addParentProxyRelations(dependParentNodes());
        }        
    } else {
        // add 'this' as child relation to the relations parent
        //kDebug()<<k_funcinfo<<m_name<<" is not summary task"<<endl;
        Q3PtrListIterator<Relation> it = list;
        for (; it.current(); ++it) {
            it.current()->parent()->addChildProxyRelation(this, it.current());
            // add a parent relation to myself
            addParentProxyRelation(it.current()->parent(), it.current());
        }
    }
}

void Task::addChildProxyRelations(Q3PtrList<Relation> &list) {
    //kDebug()<<k_funcinfo<<m_name<<endl;
    if (type() == Type_Summarytask) {
        // propagate to my children
        //kDebug()<<k_funcinfo<<m_name<<" is summary task"<<endl;
        Q3PtrListIterator<Node> nodes = m_nodes;
        for (; nodes.current(); ++nodes) {
            nodes.current()->addChildProxyRelations(list);
            nodes.current()->addChildProxyRelations(dependChildNodes());
        }        
    } else {
        // add 'this' as parent relation to the relations child
        //kDebug()<<k_funcinfo<<m_name<<" is not summary task"<<endl;
        Q3PtrListIterator<Relation> it = list;
        for (; it.current(); ++it) {
            it.current()->child()->addParentProxyRelation(this, it.current());
            // add a child relation to myself
            addChildProxyRelation(it.current()->child(), it.current());
        }
    }
}

void Task::addParentProxyRelation(Node *node, const Relation *rel) {
    if (node->type() != Type_Summarytask) {
        if (type() == Type_Summarytask) {
            //kDebug()<<"Add parent proxy from my children "<<m_name<<" to "<<node->name()<<endl;
            Q3PtrListIterator<Node> nodes = m_nodes;
            for (; nodes.current(); ++nodes) {
                nodes.current()->addParentProxyRelation(node, rel);
            }
        } else {
            //kDebug()<<"Add parent proxy from "<<node->name()<<" to (me) "<<m_name<<endl;
            m_parentProxyRelations.append(new ProxyRelation(node, this, rel->type(), rel->lag()));
        }
    }
}

void Task::addChildProxyRelation(Node *node, const Relation *rel) {
    if (node->type() != Type_Summarytask) {
        if (type() == Type_Summarytask) {
            //kDebug()<<"Add child proxy from my children "<<m_name<<" to "<<node->name()<<endl;
            Q3PtrListIterator<Node> nodes = m_nodes;
            for (; nodes.current(); ++nodes) {
                nodes.current()->addChildProxyRelation(node, rel);
            }
        } else {
            //kDebug()<<"Add child proxy from (me) "<<m_name<<" to "<<node->name()<<endl;
            m_childProxyRelations.append(new ProxyRelation(this, node, rel->type(), rel->lag()));
        }
    }
}

bool Task::isEndNode() const {
    Q3PtrListIterator<Relation> it = m_dependChildNodes;
    for (; it.current(); ++it) {
        if (it.current()->type() == Relation::FinishStart)
            return false;
    }
    Q3PtrListIterator<Relation> pit = m_childProxyRelations;
    for (; pit.current(); ++pit) {
        if (pit.current()->type() == Relation::FinishStart)
            return false;
    }
    return true;
}
bool Task::isStartNode() const {
    Q3PtrListIterator<Relation> it = m_dependParentNodes;
    for (; it.current(); ++it) {
        if (it.current()->type() == Relation::FinishStart ||
            it.current()->type() == Relation::StartStart)
            return false;
    }
    Q3PtrListIterator<Relation> pit = m_parentProxyRelations;
    for (; pit.current(); ++pit) {
        if (pit.current()->type() == Relation::FinishStart ||
            pit.current()->type() == Relation::StartStart)
            return false;
    }
    return true;
}

DateTime Task::workStartTime() const {
    if (m_currentSchedule == 0)
         return DateTime();
    if (m_requests)
        return m_currentSchedule->workStartTime;
    return m_currentSchedule->startTime;
}

DateTime Task::workEndTime() const {
    if (m_currentSchedule == 0)
         return DateTime();
    return m_currentSchedule->endTime;
}

DateTime Task::workStartAfter(const DateTime &dt) {
    if (m_requests) {
        DateTime t = m_requests->availableAfter(dt);
        return t.isValid() ? t : dt;
    }
    return dt;
}

DateTime Task::workFinishBefore(const DateTime &dt) {
    if (m_requests) {
        return m_requests->availableBefore(dt);
    }
    return dt;
}

Duration Task::positiveFloat() {
    if (m_currentSchedule == 0 || 
        m_currentSchedule->schedulingError ||
        effortMetError()) {
        return Duration::zeroDuration;
    }
    Duration f;
    if (type() == Node::Type_Milestone) {
        if (m_currentSchedule->startTime < m_currentSchedule->latestFinish) {
            f = m_currentSchedule->latestFinish - m_currentSchedule->startTime;
        }
    } else if (m_effort->type() == Effort::Type_FixedDuration) {
        if (m_currentSchedule->endTime.isValid()) {
            if (m_currentSchedule->endTime < m_currentSchedule->latestFinish) {
                f = m_currentSchedule->latestFinish - m_currentSchedule->endTime;
            }
        }
    } else {
        if (m_currentSchedule->workEndTime.isValid())
            if (m_currentSchedule->workEndTime < m_currentSchedule->latestFinish) {
            f = m_currentSchedule->latestFinish - m_currentSchedule->workEndTime;
        } else if (m_currentSchedule->endTime.isValid()) {
            if (m_currentSchedule->endTime < m_currentSchedule->latestFinish) {
                f = m_currentSchedule->latestFinish - m_currentSchedule->endTime;
            }
        }
    }
    //kDebug()<<k_funcinfo<<f.toString()<<endl;
    return f;
}

bool Task::isCritical() {
    Schedule *cs = m_currentSchedule;
    if (cs == 0) {
        return false;
    }
    return cs->earliestStart >= cs->startTime && cs->latestFinish <= cs->endTime;
}

bool Task::calcCriticalPath(bool fromEnd) {
    if (m_currentSchedule == 0)
        return false;
    //kDebug()<<k_funcinfo<<m_name<<" fromEnd="<<fromEnd<<" cp="<<m_currentSchedule->inCriticalPath<<endl;
    if (m_currentSchedule->inCriticalPath) {
        return true; // path allready calculated
    }
    if (!isCritical()) {
        return false;
    }
    if (fromEnd) {
        if (isEndNode()) {
            m_currentSchedule->inCriticalPath = true;
            //kDebug()<<k_funcinfo<<m_name<<" end node"<<endl;
            return true;
        }
        Q3PtrListIterator<Relation> it(m_childProxyRelations);
        for (; it.current(); ++it) {
            if (it.current()->child()->calcCriticalPath(fromEnd)) {
                m_currentSchedule->inCriticalPath = true;
            }
        }
        Q3PtrListIterator<Relation> pit(m_dependChildNodes);
        for (; pit.current(); ++pit) {
            if (pit.current()->child()->calcCriticalPath(fromEnd)) {
                m_currentSchedule->inCriticalPath = true;
            }
        }
    } else {
        if (isStartNode()) {
            m_currentSchedule->inCriticalPath = true;
            //kDebug()<<k_funcinfo<<m_name<<" start node"<<endl;
            return true;
        }
        Q3PtrListIterator<Relation> it(m_parentProxyRelations);
        for (; it.current(); ++it) {
            if (it.current()->parent()->calcCriticalPath(fromEnd)) {
                m_currentSchedule->inCriticalPath = true;
            }
        }
        Q3PtrListIterator<Relation> pit(m_dependParentNodes);
        for (; pit.current(); ++pit) {
            if (pit.current()->parent()->calcCriticalPath(fromEnd)) {
                m_currentSchedule->inCriticalPath = true;
            }
        }
    }
    //kDebug()<<k_funcinfo<<m_name<<" return cp="<<m_currentSchedule->inCriticalPath<<endl;
    return m_currentSchedule->inCriticalPath;
}

void Task::setCurrentSchedule(long id) {
    setCurrentSchedulePtr(findSchedule(id));
    Node::setCurrentSchedule(id);
}

bool Task::effortMetError() const {
    if (m_currentSchedule->notScheduled) {
        return false;
    }
    return m_currentSchedule->plannedEffort() < effort()->effort(static_cast<Effort::Use>(m_currentSchedule->type()));
}

#ifndef NDEBUG
void Task::printDebug(bool children, QByteArray indent) {
    kDebug()<<indent<<"+ Task node: "<<name()<<" type="<<type()<<endl;
    indent += "!  ";
    kDebug()<<indent<<"Requested resources (total): "<<units()<<"%"<<endl;
    kDebug()<<indent<<"Requested resources (work): "<<workUnits()<<"%"<<endl;
    if (m_requests)
        m_requests->printDebug(indent);
    
    Node::printDebug(children, indent);

}

#endif

}  //KPlato namespace
