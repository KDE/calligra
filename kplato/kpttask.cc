/* This file is part of the KDE project
   Copyright (C) 2001 Thomas zander <zander@kde.org>
   Copyright (C) 2004 - 2007 Dag Andersen <danders@get2net.dk>
   Copyright (C) 2007 Florian Piquemal <flotueur@yahoo.fr>
   Copyright (C) 2007 Alexis Ménard <darktears31@gmail.com>

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
#include "kptappointment.h"
#include "kptproject.h"
#include "kpttaskdialog.h"
#include "kptduration.h"
#include "kptrelation.h"
#include "kptdatetime.h"
#include "kptcalendar.h"
#include "kpteffortcostmap.h"
#include "kptschedule.h"
#include "kptxmlloaderobject.h"

#include <qdom.h>
#include <qbrush.h>
//Added by qt3to4:
#include <QList>
#include <kdebug.h>

namespace KPlato
{

Task::Task(Node *parent) 
    : Node(parent),
      m_resource(),
      m_completion( this )
{
    //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    Duration d(1, 0, 0);
    m_effort = new Effort(d);
    m_effort->setOptimisticRatio(-10);
    m_effort->setPessimisticRatio(20);
    m_requests = 0;

    if (m_parent)
        m_leader = m_parent->leader();
}

Task::Task(Task &task, Node *parent) 
    : Node(task, parent), 
      m_resource(),
      m_completion( this )
{
    //kDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    m_requests = 0;
    
    m_effort = task.effort() ? new Effort(*(task.effort())) 
                             : new Effort(); // Avoid crash, (shouldn't be zero)
}


Task::~Task() {
    delete m_effort;
    while (!m_resource.isEmpty()) {
        delete m_resource.takeFirst();
    }
    while (!m_parentProxyRelations.isEmpty()) {
        delete m_parentProxyRelations.takeFirst();
    }
    while (!m_childProxyRelations.isEmpty()) {
        delete m_childProxyRelations.takeFirst();
    }
    while (!m_schedules.isEmpty()) {
        foreach (long k, m_schedules.uniqueKeys()) {
            delete m_schedules.take(k);
        }
    }
    delete m_requests;
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

Duration *Task::getRandomDuration() {
    return 0L;
}

ResourceGroupRequest *Task::resourceGroupRequest(ResourceGroup *group) const {
    if (m_requests)
        return m_requests->find(group);
    return 0;
}

void Task::clearResourceRequests() {
    if (m_requests) {
        m_requests->clear();
        changed( this );
    }
}

void Task::addRequest(ResourceGroup *group, int numResources) {
    addRequest(new ResourceGroupRequest(group, numResources));
}

void Task::addRequest(ResourceGroupRequest *request) {
    //kDebug()<<k_funcinfo<<request<<endl;
    if (!m_requests) {
        m_requests = new ResourceRequestCollection(*this);
    }
    m_requests->addRequest(request);
}

void Task::takeRequest(ResourceGroupRequest *request) {
    //kDebug()<<k_funcinfo<<request<<endl;
    if (m_requests) {
        m_requests->takeRequest(request);
    }
}

QStringList Task::requestNameList() const {
    QStringList lst;
    if ( m_requests ) {
        lst << m_requests->requestNameList();
    }
    return lst;
}

bool Task::containsRequest( const QString &identity ) const {
    return m_requests == 0 ? false : m_requests->contains( identity );
}

ResourceRequest *Task::resourceRequest( const QString &name ) const {
    return m_requests == 0 ? 0 : m_requests->resourceRequest( name );
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
        foreach (Node *n, m_nodes) {
            n->makeAppointments();
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


bool Task::load(QDomElement &element, XMLLoaderObject &status ) {
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
    if (!s.isEmpty())
        m_constraintStartTime = DateTime::fromString(s, status.projectSpec());
    s = element.attribute("constraint-endtime");
    if (!s.isEmpty())
        m_constraintEndTime = DateTime::fromString(s, status.projectSpec());
    
    m_startupCost = element.attribute("startup-cost", "0.0").toDouble();
    m_shutdownCost = element.attribute("shutdown-cost", "0.0").toDouble();
    
    m_wbs = element.attribute("wbs", "");
    
    // Load the task children
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();

            if (e.tagName() == "project") {
                // Load the subproject
/*                Project *child = new Project(this, status);
                if (child->load(e)) {
                    if (!project.addSubTask(child, this)) {
                        delete child;  // TODO: Complain about this
                    }
                } else {
                    // TODO: Complain about this
                    delete child;
                }*/
            } else if (e.tagName() == "task") {
                // Load the task
                Task *child = new Task(this);
                if (child->load(e, status)) {
                    if (!status.project().addSubTask(child, this)) {
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
                if (r->load(e, status.project())) {
                    addRequest(r);
                } else {
                    kError()<<k_funcinfo<<"Failed to load resource request"<<endl;
                    delete r;
                }
            } else if (e.tagName() == "progress") {
                m_completion.loadXML( e, status );
            } else if (e.tagName() == "schedules") {
                QDomNodeList lst = e.childNodes();
                for (unsigned int i=0; i<lst.count(); ++i) {
                    if (lst.item(i).isElement()) {
                        QDomElement el = lst.item(i).toElement();
                        if (el.tagName() == "schedule") {
                            NodeSchedule *sch = new NodeSchedule();
                            if (sch->loadXML(el, status)) {
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

    me.setAttribute("id", m_id);
    me.setAttribute("name", m_name);
    me.setAttribute("leader", m_leader);
    me.setAttribute("description", m_description);

    me.setAttribute("scheduling",constraintToString());
    me.setAttribute("constraint-starttime",m_constraintStartTime.toString( KDateTime::ISODate ));
    me.setAttribute("constraint-endtime",m_constraintEndTime.toString( KDateTime::ISODate ));    

    me.setAttribute("startup-cost", m_startupCost);
    me.setAttribute("shutdown-cost", m_shutdownCost);
    
    me.setAttribute("wbs", m_wbs);
    
    m_effort->save(me);

    m_completion.saveXML( me );
    
    if (!m_schedules.isEmpty()) {
        QDomElement schs = me.ownerDocument().createElement("schedules");
        me.appendChild(schs);
        foreach (Schedule *s, m_schedules) {
            if (!s->isDeleted()) {
                s->saveXML(schs);
            }
        }
    }
    if (m_requests) {
        m_requests->save(me);
    }
    for (int i=0; i<numChildren(); i++) {
        childNode(i)->save(me);
    }
}

void Task::saveAppointments(QDomElement &element, long id) const {
    //kDebug()<<k_funcinfo<<m_name<<" id="<<id<<endl;
    Schedule *sch = findSchedule(id);
    if (sch) {
        sch->saveAppointments(element);
    }
    foreach (Node *n, m_nodes) {
        n->saveAppointments(element, id);
    }
}

EffortCostMap Task::plannedEffortCostPrDay(const QDate &start, const QDate &end, long id ) const {
    //kDebug()<<k_funcinfo<<m_name<<endl;
    Schedule *s = m_currentSchedule;
    if ( id != -1 ) {
        s = findSchedule( id );
    }
    if ( s ) {
        return s->plannedEffortCostPrDay(start, end);
    }
    return EffortCostMap();
}

// Returns the total planned effort for this task (or subtasks) 
Duration Task::plannedEffort( long id ) const {
   //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            eff += n->plannedEffort( id );
        }
        return eff;
    }
    Schedule *s = m_currentSchedule;
    if ( id != -1 ) {
        s = findSchedule( id );
    }
    if ( s ) {
        eff = s->plannedEffort();
    }
    return eff;
}

// Returns the total planned effort for this task (or subtasks) on date
Duration Task::plannedEffort(const QDate &date, long id ) const {
   //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            eff += n->plannedEffort(date, id);
        }
        return eff;
    }
    Schedule *s = m_currentSchedule;
    if ( id != -1 ) {
        s = findSchedule( id );
    }
    if ( s ) {
        eff = s->plannedEffort(date);
    }
    return eff;
}

// Returns the total planned effort for this task (or subtasks) upto and including date
Duration Task::plannedEffortTo(const QDate &date, long id) const {
    //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            eff += n->plannedEffortTo(date, id);
        }
        return eff;
    }
    Schedule *s = m_currentSchedule;
    if ( id != -1 ) {
        s = findSchedule( id );
    }
    if ( s ) {
        eff = s->plannedEffortTo(date);
    }
    return eff;
}

// Returns the total planned effort for this task (or subtasks) 
Duration Task::actualEffort( long id ) const {
   //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            eff += n->actualEffort(id);
        }
    } else {
        eff = m_completion.actualEffort();
    }
    /* If we want to register pr resource...
    } else if (m_currentSchedule) {
        eff = m_currentSchedule->actualEffort();
    }*/
    return eff;
}

// Returns the total actual effort for this task (or subtasks) on date
Duration Task::actualEffort(const QDate &date, long id) const {
   //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            eff += n->actualEffort(date, id);
        }
        return eff;
    }
    Schedule *s = m_currentSchedule;
    if ( id != -1 ) {
        s = findSchedule( id );
    }
    if ( s ) {
        eff = s->actualEffort(date);
    }
    return eff;
}

// Returns the total actual effort for this task (or subtasks) to date
Duration Task::actualEffortTo(const QDate &date, long id) const {
   //kDebug()<<k_funcinfo<<endl;
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            eff += n->actualEffortTo(date, id);
        }
        return eff;
    }
    return m_completion.actualEffortTo( date );
/*    Schedule *s = m_currentSchedule;
    if ( id != -1 ) {
        s = findSchedule( id );
    }
    if ( s ) {
        eff = s->actualEffortTo(date);
    }*/
    return eff;
}

double Task::plannedCost( long id ) const {
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            c += n->plannedCost( id );
        }
        return c;
    }
    Schedule *s = m_currentSchedule;
    if ( id != -1 ) {
        s = findSchedule( id );
    }
    if ( s ) {
        c = s->plannedCost();
    }
    return c;
}

double Task::plannedCost(const QDate &date, long id) const {
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            c += n->plannedCost(date, id);
        }
        return c;
    }
    Schedule *s = m_currentSchedule;
    if ( id != -1 ) {
        s = findSchedule( id );
    }
    if ( s ) {
        c = s->plannedCost(date);
    }
    return c;
}

double Task::plannedCostTo(const QDate &date, long id) const {
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            c += n->plannedCostTo(date, id);
        }
        return c;
    }
    Schedule *s = m_currentSchedule;
    if ( id != -1 ) {
        s = findSchedule( id );
    }
    if ( s ) {
        c = s->plannedCostTo(date);
    }
    return c;
}

double Task::actualCost( long id ) const {
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            c += n->actualCost( id );
        }
        return c;
    }
    Schedule *s = m_currentSchedule;
    if ( id != -1 ) {
        s = findSchedule( id );
    }
    if ( s ) {
        c = s->actualCost();
    }
    return c;
}

double Task::actualCost(const QDate &date, long id) const {
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            c += n->actualCost(date, id);
        }
        return c;
    }
    Schedule *s = m_currentSchedule;
    if ( id != -1 ) {
        s = findSchedule( id );
    }
    if ( s ) {
        c = s->actualCost(date);
    }
    return c;
}

double Task::actualCostTo(const QDate &date, long id) const {
    //kDebug()<<k_funcinfo<<endl;
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            c += n->actualCostTo(date, id);
        }
        return c;
    }
    Schedule *s = m_currentSchedule;
    if ( id != -1 ) {
        s = findSchedule( id );
    }
    if ( s ) {
        c = s->actualCostTo(date);
    }
    return c;
}

//FIXME Handle summarytasks
double Task::effortPerformanceIndex(const QDate &date, bool *error) const {
    double res = 0.0;
    Duration ae = actualEffortTo(date);
    
    bool e = (ae == Duration::zeroDuration || m_completion.percentFinished() == 0);
    if (error) {
        *error = e;
    }
    if (!e) {
        res = (plannedEffortTo(date).toDouble() * ((double)m_completion.percentFinished()/100.0) / ae.toDouble());
    }
    return res;
}

//FIXME Handle summarytasks
double Task::costPerformanceIndex(const QDate &date, bool *error) const {
    double res = 0.0;
    double ac = actualCostTo(date);
    
    bool e = (ac == 0.0 || m_completion.percentFinished() == 0);
    if (error) {
        *error = e;
    }
    if (!e) {
        res = (plannedCostTo(date) * m_completion.percentFinished())/(100 * ac);
    }
    return res;
}

void Task::initiateCalculation(MainSchedule &sch) {
    //kDebug()<<k_funcinfo<<m_name<<" schedule: "<<sch.name()<<" id="<<sch.id()<<endl;
    m_visitedForward = false;
    m_visitedBackward = false;
    m_currentSchedule = createSchedule(&sch);
    m_currentSchedule->initiateCalculation();
    clearProxyRelations();
    Node::initiateCalculation(sch);
}


void Task::initiateCalculationLists(MainSchedule &sch) {
    //kDebug()<<k_funcinfo<<m_name<<endl;
    if (type() == Node::Type_Summarytask) {
        sch.insertSummaryTask(this);
        // propagate my relations to my children and dependent nodes
        foreach (Node *n, m_nodes) {
            if (!dependParentNodes().isEmpty()) 
                n->addParentProxyRelations(dependParentNodes());
            if (!dependChildNodes().isEmpty()) 
                n->addChildProxyRelations(dependChildNodes());
            n->initiateCalculationLists(sch);
        }
    } else {
        if (isEndNode()) {
            sch.insertEndNode(this);
            //kDebug()<<k_funcinfo<<"endnodes append: "<<m_name<<endl;
        }
        if (isStartNode()) {
            sch.insertStartNode(this);
            //kDebug()<<k_funcinfo<<"startnodes append: "<<m_name<<endl;
        }
        if ( ( m_constraint == Node::MustStartOn && m_effort->type() == Effort::Type_FixedDuration ) ||
            ( m_constraint == Node::MustFinishOn && m_effort->type() == Effort::Type_FixedDuration ) ||
            ( m_constraint == Node::FixedInterval ) )
        {
            sch.insertHardConstraint( this );
        }
        else if ( ( m_constraint == Node::StartNotEarlier ) ||
                  ( m_constraint == Node::FinishNotLater ) )
        {
            sch.insertSoftConstraint( this );
        }
    }
}

DateTime Task::calculatePredeccessors(const QList<Relation*> &list, int use) {
    DateTime time;
    foreach (Relation *r, list) {
        if (r->parent()->type() == Type_Summarytask) {
            //kDebug()<<k_funcinfo<<"Skip summarytask: "<<it.current()->parent()->name()<<endl;
            continue; // skip summarytasks
        }
        DateTime t = r->parent()->calculateForward(use); // early finish
        switch (r->type()) {
            case Relation::StartStart:
                // I can't start earlier than my predesseccor
                t = r->parent()->getEarliestStart() + r->lag();
                break;
            case Relation::FinishFinish:
                // I can't finish earlier than my predeccessor, so
                // I can't start earlier than it's (earlyfinish+lag)- my duration
                t += r->lag();
                t -= duration(t, use, true);
                break;
            default:
                t += r->lag();
                break;
        }
        if (!time.isValid() || t > time)
            time = t;
    }
    //kDebug()<<time.toString()<<"                  "<<m_name<<" calculatePredeccessors() ("<<list.count()<<")"<<endl;
    return time;
}

DateTime Task::calculateForward(int use) {
    if (m_currentSchedule == 0) {
        return DateTime();
    }
    Schedule *cs = m_currentSchedule;
    bool pert = cs->usePert();
    cs->setCalculationMode( Schedule::CalculateForward );
    // calculate all predecessors
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
    return calculateEarlyFinish( use );
}

DateTime Task::calculateEarlyFinish(int use) {
    //kDebug()<<k_funcinfo<<m_name<<endl;
    if (m_currentSchedule == 0) {
        return DateTime();
    }
    Schedule *cs = m_currentSchedule;
    bool pert = cs->usePert();
    cs->setCalculationMode( Schedule::CalculateForward );
    if (m_visitedForward) {
        //kDebug()<<earliestStart.toString()<<" + "<<m_durationBackward.toString()<<" "<<m_name<<" calculateForward() (visited)"<<endl;
        return m_earlyFinish;
    }
    //kDebug()<<k_funcinfo<<"------> "<<m_name<<" "<<cs->earliestStart<<endl;
    if (type() == Node::Type_Task) {
        m_durationForward = m_effort->effort(use, pert);
        switch (constraint()) {
            case Node::ASAP:
            case Node::ALAP:
            {
                m_earlyStart = workStartAfter(cs->earliestStart);
                m_durationForward = duration(m_earlyStart, use, false);
                //kDebug()<<k_funcinfo<<m_name<<": "<<earlyStart<<"+"<<m_durationForward.toString()<<"="<<(earlyStart+m_durationForward)<<endl;
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = m_earlyStart;
                    cs->endTime = m_earlyStart + m_durationForward;
                    makeAppointments();
                }
                m_durationForward += cs->earliestStart - m_earlyStart;
                break;
            }
            case Node::MustFinishOn:
            {
                m_earlyStart = cs->earliestStart;
                m_durationForward = duration(m_earlyStart, use, false);
                DateTime earlyFinish = cs->earliestStart + m_durationForward;
                if (earlyFinish > m_constraintEndTime) {
                    cs->schedulingError = true;
                } else if (earlyFinish < m_constraintEndTime) {
                    earlyFinish = m_constraintEndTime;
                    m_durationForward = duration(m_constraintEndTime, use, true);
                    m_earlyStart = earlyFinish - m_durationForward;
                }
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = m_earlyStart;
                    cs->endTime = earlyFinish;
                    makeAppointments();
                }
                m_durationForward = earlyFinish - cs->earliestStart;
                break;
            }
            case Node::FinishNotLater:
                m_durationForward = duration(cs->earliestStart, use, false);
                if (cs->earliestStart + m_durationForward > m_constraintEndTime) {
                    cs->schedulingError = true;
                    m_durationForward = duration(m_constraintEndTime, use, true);
                    cs->earliestStart = m_constraintEndTime - m_durationForward;
                }
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = cs->earliestStart;
                    cs->endTime = cs->earliestStart + m_durationForward;
                    makeAppointments();
                }
                m_earlyStart = workTimeAfter( cs->earliestStart );
                break;
            case Node::MustStartOn:
            {
                //kDebug()<<k_funcinfo<<cs->earliestStart<<"> "<<m_constraintStartTime<<endl;
                DateTime earlyStart = workStartAfter( m_constraintStartTime );
                if ( cs->earliestStart > m_constraintStartTime ) {
                    cs->schedulingError = true;
                    m_earlyStart = workStartAfter( cs->earliestStart );
                }
                m_durationForward = duration( earlyStart, use, false );
                if ( m_visitedBackward && cs->latestFinish < earlyStart + m_durationForward ) {
                    m_earlyStart = cs->latestFinish - m_durationBackward;
                    m_durationForward = m_durationBackward;
                }
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = m_earlyStart;
                    cs->endTime = m_earlyStart + m_durationForward;
                    makeAppointments();
                }
                m_durationForward += m_earlyStart - cs->earliestStart;
                break;
            }
            case Node::StartNotEarlier:
                if ( cs->earliestStart > m_constraintStartTime ) { // TODO check
                    cs->schedulingError = true;
                }
                if (cs->earliestStart < m_constraintStartTime) {
                    cs->earliestStart = m_constraintStartTime;
                }
                m_durationForward = duration(cs->earliestStart, use, false);
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = cs->earliestStart;
                    cs->endTime = cs->earliestStart + m_durationForward;
                    makeAppointments();
                }
                m_earlyStart = cs->earliestStart;
                break;
            case Node::FixedInterval: {
                if ( cs->earliestStart > m_constraintStartTime ) {
                    cs->schedulingError = true;
                }
                //cs->earliestStart = m_constraintStartTime;
                m_durationForward = m_constraintEndTime - cs->earliestStart;
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = m_constraintStartTime;
                    cs->endTime = m_constraintEndTime;
                    makeAppointments();
                }
                m_earlyStart = m_constraintStartTime;
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
        m_earlyStart = cs->earliestStart;
        //kDebug()<<k_funcinfo<<m_name<<" "<<earliestStart.toString()<<endl
    } else if (type() == Node::Type_Summarytask) {
        kWarning()<<k_funcinfo<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    } else { // ???
        m_durationForward = Duration::zeroDuration;
    }
    
    //kDebug()<<"Earlyfinish: "<<cs->earliestStart<<"+"<<m_durationForward.toString()<<"="<<(cs->earliestStart+m_durationForward).toString()<<" "<<m_name<<" calculateForward()"<<endl;
    m_visitedForward = true;
    cs->insertForwardNode( this );
    m_earlyFinish = cs->earliestStart + m_durationForward;
    return m_earlyFinish;
}

DateTime Task::calculateSuccessors(const QList<Relation*> &list, int use) {
    DateTime time;
    foreach (Relation *r, list) {
        if (r->child()->type() == Type_Summarytask) {
            //kDebug()<<k_funcinfo<<"Skip summarytask: "<<r->parent()->name()<<endl;
            continue; // skip summarytasks
        }
        DateTime t = r->child()->calculateBackward(use);
        switch (r->type()) {
            case Relation::StartStart:
                // I must start before my successor, so
                // I can't finish later than it's (starttime-lag) + my duration
                t -= r->lag();
                t += duration(t, use, false);
                break;
            case Relation::FinishFinish:
                // My successor cannot finish before me, so
                // I can't finish later than it's latest finish - lag
                t = r->child()->getLatestFinish() -  r->lag();
                break;
            default:
                t -= r->lag();
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
    cs->setCalculationMode( Schedule::CalculateBackward );
    bool pert = cs->usePert();
    // calculate all successors
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
    return calculateLateStart( use );
}

DateTime Task::calculateLateStart(int use) {
    //kDebug()<<k_funcinfo<<m_name<<endl;
    if (m_currentSchedule == 0) {
        return DateTime();
    }
    Schedule *cs = m_currentSchedule;
    cs->setCalculationMode( Schedule::CalculateBackward );
    bool pert = cs->usePert();
    if (m_visitedBackward) {
        //kDebug()<<latestFinish.toString()<<" - "<<m_durationBackward.toString()<<" "<<m_name<<" calculateBackward() (visited)"<<endl;
        return m_lateStart;
    }
    //kDebug()<<k_funcinfo<<m_name<<" id="<<cs->id()<<" mode="<<cs->calculationMode()<<": latestFinish="<<cs->latestFinish<<endl;
    if (type() == Node::Type_Task) {
        m_durationBackward = m_effort->effort(use, pert);
        switch (constraint()) {
            case Node::ASAP:
            case Node::ALAP:
                cs->endTime = workFinishBefore(cs->latestFinish);
                m_durationBackward = duration(cs->endTime, use, true);
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = cs->endTime - m_durationBackward;
                    makeAppointments();
                }
                m_durationBackward += cs->latestFinish - cs->endTime;
                m_lateFinish = cs->endTime;
                break;
            case Node::MustStartOn:
            {
                DateTime lateStart = workStartAfter( m_constraintStartTime );
                m_durationBackward = duration(lateStart, use, false);
                m_lateFinish = lateStart + m_durationBackward;
                if ( m_lateFinish > cs->latestFinish ) {
                    cs->schedulingError = true;
                    m_lateFinish = cs->latestFinish;
                    m_durationBackward = duration( m_lateFinish, use, true );
                }
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = m_lateFinish - m_durationBackward;
                    cs->endTime = m_lateFinish;
                    makeAppointments();
                }
                break;
            }
            case Node::StartNotEarlier:
            {
                m_lateFinish = cs->latestFinish;
                m_durationBackward = duration(m_lateFinish, use, true);
                if (m_lateFinish - m_durationBackward < m_constraintStartTime) {
                    m_durationBackward = duration(m_constraintStartTime, use, false);
                    m_lateFinish = m_constraintStartTime + m_durationBackward;
                }
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = m_lateFinish - m_durationBackward;
                    cs->endTime = m_lateFinish;
                    makeAppointments();
                }
                m_durationBackward += cs->latestFinish - m_lateFinish;
                break;
            }
            case Node::MustFinishOn:
                //cs->latestFinish = m_constraintEndTime;
                //kDebug()<<m_name<<": "<<cs->latestFinish<<"< "<<m_constraintEndTime<<endl;
                if ( cs->latestFinish < m_constraintEndTime ) {
                    cs->schedulingError = true;
                }
                m_durationBackward = duration(cs->latestFinish, use, true);
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = cs->latestFinish - m_durationBackward;
                    cs->endTime = cs->latestFinish;
                    makeAppointments();
                }
                //m_durationBackward += cs->latestFinish - m_constraintEndTime;
                m_lateFinish = m_constraintEndTime;
                break;
            case Node::FinishNotLater:
                if (cs->latestFinish > m_constraintEndTime) {
                    cs->latestFinish = m_constraintEndTime;
                }
                m_durationBackward = duration(m_constraintEndTime, use, true);
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = m_constraintEndTime - m_durationBackward;
                    cs->endTime = m_constraintEndTime;
                    makeAppointments();
                }
                m_durationBackward += cs->latestFinish - m_constraintEndTime;
                m_lateFinish = m_constraintEndTime;
                break;
            case Node::FixedInterval: {
                //cs->latestFinish = m_constraintEndTime;
                if ( cs->latestFinish < m_constraintEndTime ) {
                    cs->schedulingError = true;
                }
                m_durationBackward = m_constraintEndTime - m_constraintStartTime;
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = m_constraintStartTime;
                    cs->endTime = m_constraintEndTime;
                    makeAppointments();
                }
                m_lateFinish = m_constraintEndTime;
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
        m_lateFinish = cs->latestFinish;
        //kDebug()<<k_funcinfo<<m_name<<" "<<cs->latestFinish<<endl;
    } else if (type() == Node::Type_Summarytask) {
        kWarning()<<k_funcinfo<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    } else { // ???
        m_durationBackward = Duration::zeroDuration;
    }
    m_visitedBackward = true;
    cs->insertBackwardNode( this );
    //kDebug()<<"Latestart: "<<cs->latestFinish<<"-"<<m_durationBackward.toString()<<"="<<(cs->latestFinish-m_durationBackward)<<" "<<m_name<<" calculateBackward()"<<endl;
    m_lateStart = cs->latestFinish - m_durationBackward;
    return m_lateStart;
}

DateTime Task::schedulePredeccessors(const QList<Relation*> &list, int use) {
    DateTime time;
    foreach (Relation *r, list) {
        if (r->parent()->type() == Type_Summarytask) {
            //kDebug()<<k_funcinfo<<"Skip summarytask: "<<r->parent()->name()<<endl;
            continue; // skip summarytasks
        }
        // schedule the predecessors
        DateTime earliest = r->parent()->getEarliestStart();
        DateTime t = r->parent()->scheduleForward(earliest, use);
        switch (r->type()) {
            case Relation::StartStart:
                // I can't start before my predesseccor
                t = r->parent()->startTime() + r->lag();
                break;
            case Relation::FinishFinish:
                // I can't end before my predecessor, so
                // I can't start before it's endtime - my duration
                t -= duration(t + r->lag(), use, true);
                break;
            default:
                t += r->lag();
                break;
        }
        if (!time.isValid() || t > time)
            time = t;
    }
    //kDebug()<<time.toString()<<" "<<m_name<<" schedulePredeccessors()"<<endl;
    return time;
}

DateTime Task::scheduleForward(const DateTime &earliest, int use) {
    if (m_currentSchedule == 0) {
        return DateTime();
    }
    Schedule *cs = m_currentSchedule;
    cs->setCalculationMode( Schedule::Scheduling );
    bool pert = cs->usePert();
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
    return scheduleFromStartTime( use );
}

DateTime Task::scheduleFromStartTime(int use) {
    //kDebug()<<k_funcinfo<<m_name<<endl;
    if (m_currentSchedule == 0) {
        return DateTime();
    }
    Schedule *cs = m_currentSchedule;
    cs->setCalculationMode( Schedule::Scheduling );
    bool pert = cs->usePert();
    if (m_visitedForward) {
        return cs->endTime;
    }
    cs->notScheduled = false;
    if ( !cs->startTime.isValid() ) {
        cs->startTime = cs->earliestStart;
    }
    //kDebug()<<k_funcinfo<<m_name<<" startTime="<<cs->startTime<<endl;
    if(type() == Node::Type_Task) {
        cs->duration = m_effort->effort(use, pert);
        switch (m_constraint) {
        case Node::ASAP:
            // cs->startTime calculated above
            cs->startTime = workStartAfter( cs->startTime );
            cs->duration = duration(cs->startTime, use, false);
            cs->endTime = cs->startTime + cs->duration;
            makeAppointments();
            cs->positiveFloat = workTimeBefore( cs->latestFinish ) - cs->endTime;
            break;
        case Node::ALAP:
            // cs->startTime calculated above
            cs->duration = duration(cs->latestFinish, use, true);
            cs->endTime = cs->latestFinish;
            cs->startTime = cs->endTime - cs->duration;
            //kDebug()<<k_funcinfo<<m_name<<" endTime="<<cs->endTime<<" latest="<<cs->latestFinish<<endl;
            makeAppointments();
            cs->positiveFloat = workTimeBefore( cs->latestFinish ) - cs->endTime;
            break;
        case Node::StartNotEarlier:
            // cs->startTime calculated above
            //kDebug()<<"StartNotEarlier="<<m_constraintStartTime<<cs->startTime<<endl;
            cs->startTime = workStartAfter(cs->startTime);
            if ( cs->startTime < m_constraintStartTime ) {
                cs->schedulingError = true;
            }
            cs->duration = duration(cs->startTime, use, false);
            cs->endTime = cs->startTime + cs->duration;
            if (cs->endTime > cs->latestFinish) {
                cs->schedulingError = true;
            }
            makeAppointments();
            cs->positiveFloat = workTimeBefore( cs->latestFinish ) - cs->endTime;
            break;
        case Node::FinishNotLater:
            // cs->startTime calculated above
            //kDebug()<<"FinishNotLater="<<m_constraintEndTime<<cs->startTime<<endl;
            cs->duration = duration(cs->startTime, use, false);
            cs->endTime = cs->startTime + cs->duration;
            if (cs->endTime > m_constraintEndTime) {
                cs->schedulingError = true;
/*                cs->endTime = m_constraintEndTime;
                cs->duration = duration(cs->endTime, use, true);
                cs->startTime = cs->endTime - cs->duration;*/
            }
            makeAppointments();
            cs->positiveFloat = workTimeBefore( cs->latestFinish ) - cs->endTime;
            break;
        case Node::MustStartOn:
            // cs->startTime calculated above
            cs->startTime = workStartAfter( m_constraintStartTime );
            //kDebug()<<"MustStartOn="<<m_constraintStartTime<<"< "<<cs->startTime<<endl;
            if (m_constraintStartTime != cs->startTime ) {
                cs->schedulingError = true;
            }
            cs->duration = duration(cs->startTime, use, false);
            cs->endTime = cs->startTime + cs->duration;
            makeAppointments();
            cs->positiveFloat = workTimeBefore( cs->latestFinish ) - cs->endTime;
            break;
        case Node::MustFinishOn:
            cs->endTime = workFinishBefore( m_constraintEndTime );
            if ( m_constraintEndTime != cs->endTime ) {
                cs->schedulingError = true;
            }
            cs->duration = duration(cs->endTime, use, true);
            if ( cs->startTime > cs->endTime - cs->duration ) {
                cs->schedulingError = true;
                cs->startTime = workStartAfter( cs->startTime );
                cs->duration = duration(cs->startTime, use, false );
                cs->endTime = cs->startTime + cs->duration;
            } else {
                cs->startTime = cs->endTime - cs->duration;
            }
            //kDebug()<<"MustFinishOn:"<<m_constraintEndTime<<", "<<cs->latestFinish<<": "<<cs->startTime<<cs->endTime<<endl;
            makeAppointments();
            // TODO check
            cs->positiveFloat = workTimeBefore( cs->latestFinish ) - cs->endTime;
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
            makeAppointments();
            // TODO check
            cs->positiveFloat = workTimeBefore( cs->latestFinish ) - cs->endTime;
            break;
        }
        default:
            break;
        }
    } else if (type() == Node::Type_Milestone) {
        switch (m_constraint) {
        case Node::ASAP: {
            cs->endTime = cs->startTime;
            // TODO check, do we need to check succeccors earliestStart?
            cs->positiveFloat = cs->latestFinish - cs->endTime;
            break;
        }
        case Node::ALAP: {
            cs->startTime = cs->latestFinish;
            cs->endTime = cs->latestFinish;
            cs->positiveFloat = Duration::zeroDuration;
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
            cs->positiveFloat = cs->latestFinish - cs->endTime;
            break;
        case Node::MustFinishOn:
            if (m_constraintEndTime < cs->startTime ||
                m_constraintEndTime > cs->latestFinish) {
                cs->schedulingError = true;
            }
            cs->startTime = m_constraintEndTime;
            cs->endTime = m_constraintEndTime;
            cs->positiveFloat = cs->latestFinish - cs->endTime;
            break;
        case Node::StartNotEarlier:
            if (cs->startTime < m_constraintStartTime) {
                cs->schedulingError = true;
            }
            cs->endTime = cs->startTime;
            cs->positiveFloat = cs->latestFinish - cs->endTime;
            break;
        case Node::FinishNotLater:
            if (cs->startTime > m_constraintEndTime) {
                cs->schedulingError = true;
            }
            cs->endTime = cs->startTime;
            cs->positiveFloat = cs->latestFinish - cs->endTime;
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
    cs->startFloat = m_lateStart - m_earlyStart;
    cs->finishFloat = m_lateFinish - m_earlyFinish;
    //kDebug()<<cs->startTime<<" : "<<cs->endTime<<" "<<m_name<<" scheduleForward()"<<endl;
    m_visitedForward = true;
    return cs->endTime;
}

DateTime Task::scheduleSuccessors(const QList<Relation*> &list, int use) {
    DateTime time;
    foreach (Relation *r, list) {
        if (r->child()->type() == Type_Summarytask) {
            //kDebug()<<k_funcinfo<<"Skip summarytask: "<<r->child()->name()<<endl;
            continue;
        }
        // get the successors starttime
        DateTime latest = r->child()->getLatestFinish();
        DateTime t = r->child()->scheduleBackward(latest, use);
        switch (r->type()) {
            case Relation::StartStart:
                // I can't start before my successor, so
                // I can't finish later than it's starttime + my duration
                t += duration(t - r->lag(), use, false);
                break;
            case Relation::FinishFinish:
                t = r->child()->endTime() - r->lag();
                break;
            default:
                t -= r->lag();
                break;
        }
        if (!time.isValid() || t < time)
            time = t;
   }
   return time;
}

DateTime Task::scheduleBackward(const DateTime &latest, int use) {
    if (m_currentSchedule == 0) {
        return DateTime();
    }
    Schedule *cs = m_currentSchedule;
    cs->setCalculationMode( Schedule::Scheduling );
    bool pert = cs->usePert();
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
    return scheduleFromEndTime( use );
}

DateTime Task::scheduleFromEndTime(int use) {
    //kDebug()<<k_funcinfo<<m_name<<endl;
    if (m_currentSchedule == 0) {
        return DateTime();
    }
    Schedule *cs = m_currentSchedule;
    cs->setCalculationMode( Schedule::Scheduling );
    bool pert = cs->usePert();
    if (m_visitedBackward) {
        return cs->startTime;
    }
    cs->notScheduled = false;
    if ( !cs->endTime.isValid() ) {
        cs->endTime = cs->latestFinish;
    }
    //kDebug()<<k_funcinfo<<"------> "<<m_name<<" "<<cs->endTime<<endl;
    if (type() == Node::Type_Task) {
        cs->duration = m_effort->effort(use, pert);
        switch (m_constraint) {
        case Node::ASAP: {
            // cs->endTime calculated above
            //kDebug()<<k_funcinfo<<m_name<<": end="<<cs->endTime<<"  early="<<cs->earliestStart<<endl;
            cs->endTime = workFinishBefore(cs->endTime);
            cs->startTime = workStartAfter( cs->earliestStart );
            cs->duration = duration(cs->startTime, use, false);
            DateTime e = cs->startTime + cs->duration;
            if (e > cs->endTime) {
                cs->schedulingError = true;
            }
            cs->endTime = e;
            //kDebug()<<k_funcinfo<<m_name<<": start="<<cs->startTime<<"+"<<cs->duration.toString()<<"="<<e<<" -> end="<<cs->endTime<<endl;
            makeAppointments();
            cs->positiveFloat = workTimeBefore( cs->latestFinish ) - cs->endTime;
            break;
        }
        case Node::ALAP:
        {
            // cs->endTime calculated above
            //kDebug()<<k_funcinfo<<m_name<<": end="<<cs->endTime<<"  late="<<cs->latestFinish<<endl;
            cs->endTime = workFinishBefore(cs->endTime);
            cs->duration = duration(cs->endTime, use, true);
            cs->startTime = cs->endTime - cs->duration;
            if ( cs->startTime < cs->earliestStart ) {
                cs->schedulingError = true;
            }
            //kDebug()<<k_funcinfo<<m_name<<": lateStart="<<cs->startTime<<endl;
            makeAppointments();
            cs->positiveFloat = workTimeBefore( cs->latestFinish ) - cs->endTime;
            break;
        }
        case Node::StartNotEarlier:
            // cs->endTime calculated above
            //kDebug()<<"StartNotEarlier="<<m_constraintStartTime.toString()<<" "<<cs->endTime.toString()<<endl;
            cs->endTime = workFinishBefore(cs->endTime);
            cs->duration = duration(cs->endTime, use, true);
            cs->startTime = cs->endTime - cs->duration;
            if (cs->startTime < m_constraintStartTime) {
                cs->schedulingError = true;
/*                cs->startTime = m_constraintStartTime;
                cs->duration = duration(cs->startTime, use, false);
                cs->endTime = cs->startTime + cs->duration;*/
            }
            makeAppointments();
            cs->positiveFloat = workTimeBefore( cs->latestFinish ) - cs->endTime;
            break;
        case Node::FinishNotLater:
            // cs->endTime calculated above
            //kDebug()<<"FinishNotLater="<<m_constraintEndTime.toString()<<" "<<cs->endTime.toString()<<endl;            
            if (cs->endTime > m_constraintEndTime) {
                cs->schedulingError = true;
                cs->endTime = m_constraintEndTime;
            }
            cs->endTime = workFinishBefore(cs->endTime);
            cs->duration = duration(cs->endTime, use, true);
            cs->startTime = cs->endTime - cs->duration;
            makeAppointments();
            cs->positiveFloat = workTimeBefore( cs->latestFinish ) - cs->endTime;
            break;
        case Node::MustStartOn:
            // cs->endTime calculated above
            //kDebug()<<"MustStartOn="<<m_constraintStartTime.toString()<<" "<<cs->startTime.toString()<<endl;
            if (m_constraintStartTime > cs->latestFinish - m_durationBackward) {
                cs->schedulingError = true;
                cs->startTime = cs->latestFinish - m_durationBackward;
                cs->endTime = workFinishBefore( cs->latestFinish );
            } else {
                cs->startTime = m_constraintStartTime;
                cs->duration = duration(cs->startTime, use, false);
                if ( cs->endTime < cs->startTime + cs->duration ) {
                    cs->schedulingError = true;
                }
                cs->endTime = cs->startTime + cs->duration;
            }
            makeAppointments();
            cs->positiveFloat = workTimeBefore( cs->latestFinish ) - cs->endTime;
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
            makeAppointments();
            cs->positiveFloat = workTimeBefore( cs->latestFinish ) - cs->endTime;
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
            makeAppointments();
            // TODO check
            cs->positiveFloat = workTimeBefore( cs->latestFinish ) - cs->endTime;
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
            cs->positiveFloat = cs->latestFinish - cs->endTime;
            break;
        case Node::ALAP:
            cs->startTime = cs->latestFinish;
            cs->endTime = cs->latestFinish;
            cs->positiveFloat = Duration::zeroDuration;
            break;
        case Node::MustStartOn:
        case Node::FixedInterval:
            if (m_constraintStartTime < cs->earliestStart ||
                m_constraintStartTime > cs->endTime) {
                cs->schedulingError = true;
            }
            cs->startTime = cs->earliestStart;
            cs->endTime = cs->earliestStart;
            cs->positiveFloat = cs->latestFinish - cs->endTime;
            break;
        case Node::MustFinishOn:
            if (m_constraintEndTime < cs->earliestStart ||
                m_constraintEndTime > cs->endTime) {
                cs->schedulingError = true;
            }
            cs->startTime = m_constraintEndTime;
            cs->endTime = m_constraintEndTime;
            cs->positiveFloat = cs->latestFinish - cs->endTime;
            break;
        case Node::StartNotEarlier:
            if (m_constraintStartTime > cs->endTime) {
                cs->schedulingError = true;
            }
            cs->startTime = cs->endTime;
            cs->positiveFloat = cs->latestFinish - cs->endTime;
            break;
        case Node::FinishNotLater:
            if (m_constraintEndTime < cs->endTime) {
                cs->schedulingError = true;
            }
            cs->startTime = cs->endTime;
            cs->positiveFloat = cs->latestFinish - cs->endTime;
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
    cs->startFloat = m_lateStart - m_earlyStart;
    cs->finishFloat = m_lateFinish - m_earlyFinish;
    //kDebug()<<k_funcinfo<<m_name<<": "<<cs->startTime<<" : "<<cs->endTime<<endl;
    m_visitedBackward = true;
    return cs->startTime;
}

void Task::adjustSummarytask() {
    if (m_currentSchedule == 0)
        return;
    if (type() == Type_Summarytask) {
        DateTime start = m_currentSchedule->latestFinish;
        DateTime end = m_currentSchedule->earliestStart;
        foreach (Node *n, m_nodes) {
            n->adjustSummarytask();
            if (n->startTime() < start)
                start = n->startTime();
            if (n->endTime() > end)
                end = n->endTime();
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
        dur = m_requests->duration(time, effort, m_currentSchedule, backward);
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

void Task::addParentProxyRelations(QList<Relation*> &list) {
    //kDebug()<<k_funcinfo<<m_name<<endl;
    if (type() == Type_Summarytask) {
        // propagate to my children
        //kDebug()<<k_funcinfo<<m_name<<" is summary task"<<endl;
        foreach (Node *n, m_nodes) {
            n->addParentProxyRelations(list);
            n->addParentProxyRelations(dependParentNodes());
        }        
    } else {
        // add 'this' as child relation to the relations parent
        //kDebug()<<k_funcinfo<<m_name<<" is not summary task"<<endl;
        foreach (Relation *r, list) {
            r->parent()->addChildProxyRelation(this, r);
            // add a parent relation to myself
            addParentProxyRelation(r->parent(), r);
        }
    }
}

void Task::addChildProxyRelations(QList<Relation*> &list) {
    //kDebug()<<k_funcinfo<<m_name<<endl;
    if (type() == Type_Summarytask) {
        // propagate to my children
        //kDebug()<<k_funcinfo<<m_name<<" is summary task"<<endl;
        foreach (Node *n, m_nodes) {
            n->addChildProxyRelations(list);
            n->addChildProxyRelations(dependChildNodes());
        }        
    } else {
        // add 'this' as parent relation to the relations child
        //kDebug()<<k_funcinfo<<m_name<<" is not summary task"<<endl;
        foreach (Relation *r, list) {
            r->child()->addParentProxyRelation(this, r);
            // add a child relation to myself
            addChildProxyRelation(r->child(), r);
        }
    }
}

void Task::addParentProxyRelation(Node *node, const Relation *rel) {
    if (node->type() != Type_Summarytask) {
        if (type() == Type_Summarytask) {
            //kDebug()<<"Add parent proxy from my children "<<m_name<<" to "<<node->name()<<endl;
            foreach (Node *n, m_nodes) {
                n->addParentProxyRelation(node, rel);
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
            foreach (Node *n, m_nodes) {
                n->addChildProxyRelation(node, rel);
            }
        } else {
            //kDebug()<<"Add child proxy from (me) "<<m_name<<" to "<<node->name()<<endl;
            m_childProxyRelations.append(new ProxyRelation(this, node, rel->type(), rel->lag()));
        }
    }
}

bool Task::isEndNode() const {
    foreach (Relation *r, m_dependChildNodes) {
        if (r->type() == Relation::FinishStart)
            return false;
    }
    foreach (Relation *r, m_childProxyRelations) {
        if (r->type() == Relation::FinishStart)
            return false;
    }
    return true;
}
bool Task::isStartNode() const {
    foreach (Relation *r, m_dependParentNodes) {
        if (r->type() == Relation::FinishStart ||
            r->type() == Relation::StartStart)
            return false;
    }
    foreach (Relation *r, m_parentProxyRelations) {
        if (r->type() == Relation::FinishStart ||
            r->type() == Relation::StartStart)
            return false;
    }
    return true;
}

DateTime Task::workTimeAfter(const DateTime &dt) const {
    DateTime t;
    if (m_requests) {
        t = m_requests->workTimeAfter(dt);
    }
    return t.isValid() ? t : dt;
}

DateTime Task::workTimeBefore(const DateTime &dt) const {
    DateTime t;
    if (m_requests) {
        t = m_requests->workTimeBefore(dt);
    }
    return t.isValid() ? t : dt;
}

DateTime Task::workStartAfter(const DateTime &dt) {
    if (m_requests) {
        DateTime t = m_requests->availableAfter(dt, m_currentSchedule);
        //kDebug()<<k_funcinfo<<"id="<<m_currentSchedule->id()<<" mode="<<m_currentSchedule->calculationMode()<<": "<<m_name<<dt<<t<<endl;
        return t.isValid() ? t : dt;
    }
    return dt;
}

DateTime Task::workFinishBefore(const DateTime &dt) {
    if (m_requests) {
        DateTime t = m_requests->availableBefore(dt, m_currentSchedule);
        //kDebug()<<k_funcinfo<<"id="<<m_currentSchedule->id()<<" mode="<<m_currentSchedule->calculationMode()<<": "<<m_name<<dt<<t<<endl;
        return t.isValid() ? t : dt;
    }
    return dt;
}

Duration Task::positiveFloat( long id ) const {
    Schedule *s = m_currentSchedule;
    if ( id != -1 ) {
        s = findSchedule( id );
    }
    return s == 0 ? Duration::zeroDuration : s->positiveFloat;
}

Duration Task::negativeFloat( long id ) const {
    Schedule *s = m_currentSchedule;
    if ( id != -1 ) {
        s = findSchedule( id );
    }
    return s == 0 ? Duration::zeroDuration : s->negativeFloat;
}

Duration Task::freeFloat( long id ) const {
    Schedule *s = m_currentSchedule;
    if ( id != -1 ) {
        s = findSchedule( id );
    }
    return s == 0 ? Duration::zeroDuration : s->freeFloat;
}

Duration Task::startFloat( long id ) const {
    Schedule *s = m_currentSchedule;
    if ( id != -1 ) {
        s = findSchedule( id );
    }
    return s == 0 ? Duration::zeroDuration : s->startFloat;
}

Duration Task::finishFloat( long id ) const {
    Schedule *s = m_currentSchedule;
    if ( id != -1 ) {
        s = findSchedule( id );
    }
    return s == 0 ? Duration::zeroDuration : s->finishFloat;
}

bool Task::isCritical( long id ) const {
    Schedule *s = m_currentSchedule;
    if ( id != -1 ) {
        s = findSchedule( id );
    }
    return s == 0 ? false : s->isCritical();
}

bool Task::calcCriticalPath(bool fromEnd) {
    if (m_currentSchedule == 0)
        return false;
    //kDebug()<<k_funcinfo<<m_name<<" fromEnd="<<fromEnd<<" cp="<<m_currentSchedule->inCriticalPath<<endl;
    if (m_currentSchedule->inCriticalPath) {
        return true; // path already calculated
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
        foreach (Relation *r, m_childProxyRelations) {
            if (r->child()->calcCriticalPath(fromEnd)) {
                m_currentSchedule->inCriticalPath = true;
            }
        }
        foreach (Relation *r, m_dependChildNodes) {
            if (r->child()->calcCriticalPath(fromEnd)) {
                m_currentSchedule->inCriticalPath = true;
            }
        }
    } else {
        if (isStartNode()) {
            m_currentSchedule->inCriticalPath = true;
            //kDebug()<<k_funcinfo<<m_name<<" start node"<<endl;
            return true;
        }
        foreach (Relation *r, m_parentProxyRelations) {
            if (r->parent()->calcCriticalPath(fromEnd)) {
                m_currentSchedule->inCriticalPath = true;
            }
        }
        foreach (Relation *r, m_dependParentNodes) {
            if (r->parent()->calcCriticalPath(fromEnd)) {
                m_currentSchedule->inCriticalPath = true;
            }
        }
    }
    //kDebug()<<k_funcinfo<<m_name<<" return cp="<<m_currentSchedule->inCriticalPath<<endl;
    return m_currentSchedule->inCriticalPath;
}

void Task::calcFreeFloat() {
    if ( type() == Node::Type_Summarytask ) {
        Node::calcFreeFloat();
        return;
    }
    Schedule *cs = m_currentSchedule;
    if ( cs == 0 ) {
        return;
    }
    cs->finishFloat = Duration::zeroDuration;
    DateTime t;
    foreach ( Relation *r, m_dependChildNodes ) {
        DateTime c = r->child()->startTime();
        if ( !t.isValid() || c < t ) {
            t = c;
        }
    }
    foreach ( Relation *r, m_childProxyRelations ) {
        DateTime c = r->child()->startTime();
        if ( !t.isValid() || c < t ) {
            t = c;
        }
    }
    if ( t.isValid() && t > cs->endTime ) {
        cs->freeFloat = t - cs->endTime;
    }
}


void Task::setCurrentSchedule(long id) {
    setCurrentSchedulePtr(findSchedule(id));
    Node::setCurrentSchedule(id);
}

bool Task::effortMetError( long id ) const {
    Schedule *s = m_currentSchedule;
    if ( id != -1 ) {
        s = findSchedule( id );
    }
    if (s == 0 || s->notScheduled || m_effort->type() != Effort::Type_Effort) {
        return false;
    }
    return s->plannedEffort() < effort()->effort(static_cast<Effort::Use>(s->type()),  s->usePert());
}

uint Task::state( long id ) const
{
    int st = Node::State_None;
    if ( m_completion.isFinished() ) {
        st |= Node::State_Finished;
        if ( m_completion.finishTime() > endTime( id ) ) {
            st |= State_FinishedLate;
        }
        if ( m_completion.finishTime() < endTime( id ) ) {
            st |= State_FinishedEarly;
        }
    } else if ( m_completion.isStarted() ) {
        if ( m_completion.percentFinished() == 0 ) {
            st |= Node::State_Started;
            if ( m_completion.startTime() > startTime( id ) ) {
                st |= State_StartedLate;
            }
            if ( m_completion.startTime() < startTime( id ) ) {
                st |= State_StartedEarly;
            }
        } else {
            st |= State_Running;
        }
    }
    return st;
}

//------------------------------------------

Completion::Completion( Node *node )
    : m_node( node ),
      m_started( false ),
      m_finished( false )
{}

Completion::Completion( const Completion &c )
{
    copy( c );
}

Completion::~Completion()
{ 
    qDeleteAll( m_entries );
    qDeleteAll( m_usedEffort );
}
    
void Completion::copy( const Completion &p )
{
    m_node = 0; //NOTE
    m_started = p.isStarted(); m_finished = p.isFinished();
    m_startTime = p.startTime(); m_finishTime = p.finishTime();
    foreach ( QDate d, p.entries().keys() ) {
        addEntry( d, new Entry( *(p.entries()[ d ]) ) );
    }
    foreach ( const Resource *r, p.usedEffortMap().keys() ) {
        addUsedEffort( r, new UsedEffort( *( p.usedEffortMap()[ r ] ) ) );
    }
}

bool Completion::operator==( const Completion &p )
{
    return m_started == p.isStarted() && m_finished == p.isFinished() &&
            m_startTime == p.startTime() && m_finishTime == p.finishTime() &&
            m_entries == p.entries() &&
            m_usedEffort == p.usedEffortMap();
}
Completion &Completion::operator=( const Completion &p )
{
    copy( p );
    return *this;
}

void Completion::changed()
{
    if ( m_node ) {
        m_node->changed();
    }
}

void Completion::setStarted( bool on )
{
     m_started = on;
     changed();
}

void Completion::setFinished( bool on )
{
     m_finished = on;
     changed();
}

void Completion::setStartTime( const DateTime &dt )
{
     m_startTime = dt;
     changed();
}

void Completion::setFinishTime( const DateTime &dt )
{
     m_finishTime = dt;
     changed();
}
    
void Completion::addEntry( const QDate &date, Entry *entry )
{
     m_entries.insert( date, entry );
     //kDebug()<<k_funcinfo<<m_entries.count()<<" added: "<<date<<endl;
     changed();
}
    
QDate Completion::entryDate() const
{
     return m_entries.isEmpty() ? QDate() : m_entries.keys().last();
}

int Completion::percentFinished() const
{
     return m_entries.isEmpty() ? 0 : m_entries.values().last()->percentFinished;
}
    
Duration Completion::remainingEffort() const
{
     return m_entries.isEmpty() ? Duration::zeroDuration : m_entries.values().last()->remainingEffort;
}

Duration Completion::actualEffort() const
{
    Duration tot;
    foreach( UsedEffort *ue, m_usedEffort.values() ) {
        tot += ue->effort();
    }
    return tot;
}

Duration Completion::actualEffortTo( const QDate &date ) const
{
    Duration eff;
    foreach ( QDate d, m_entries.keys() ) {
        if ( d > date ) {
            break;
        }
        eff = m_entries[ d ]->totalPerformed;
    }
    return eff;
}

void Completion::addUsedEffort( const Resource *resource, Completion::UsedEffort *value )
{
    UsedEffort *v = value == 0 ? new UsedEffort() : value;
    if ( m_usedEffort.contains( resource ) ) {
        m_usedEffort[ resource ]->mergeEffort( *v );
        delete v;
    } else {
        m_usedEffort.insert( resource, v );
    }
    changed();
}

QString Completion::note() const
{
    return m_entries.isEmpty() ? QString() : m_entries.values().last()->note;
}

void Completion::setNote( const QString &str )
{
    if ( ! m_entries.isEmpty() ) {
        m_entries.values().last()->note = str;
        changed();
    }
}

double Completion::actualCost() const
{
    double c = 0.0;
    foreach ( const Resource *r, m_usedEffort.keys() ) {
        c += actualCost( r );
    }
    return c;
}

double Completion::actualCost( const Resource *resource ) const
{
    double c = 0.0;
    double nc = resource->normalRate();
    double oc = resource->overtimeRate();
    foreach ( UsedEffort::ActualEffort *a, m_usedEffort.value( const_cast<Resource*>( resource )  )->actualEffortMap().values() ) {
        c += a->normalEffort().toDouble( Duration::Unit_h ) * nc;
        c += a->overtimeEffort().toDouble( Duration::Unit_h ) * oc;
    }
    return c;
}

bool Completion::loadXML( QDomElement &element, XMLLoaderObject &status )
{
    //kDebug()<<k_funcinfo<<endl;
    QString s;
    m_started = (bool)element.attribute("started", "0").toInt();
    m_finished = (bool)element.attribute("finished", "0").toInt();
    s = element.attribute("startTime");
    if (!s.isEmpty()) {
        m_startTime = DateTime::fromString(s, status.projectSpec());
    }
    s = element.attribute("finishTime");
    if (!s.isEmpty()) {
        m_finishTime = DateTime::fromString(s, status.projectSpec());
    }
    if (status.version() < "0.6") {
        if ( m_started ) {
            Entry *entry = new Entry( element.attribute("percent-finished", "0").toInt(), Duration::fromString(element.attribute("remaining-effort")),  Duration::fromString(element.attribute("performed-effort")) );
            entry->note = element.attribute("note");
            QDate date = m_startTime.date();
            if ( m_finished ) {
                date = m_finishTime.date();
            }
            // almost the best we can do ;)
            addEntry( date, entry );
        }
    } else {
        QDomNodeList list = element.childNodes();
        for (unsigned int i=0; i<list.count(); ++i) {
            if (list.item(i).isElement()) {
                QDomElement e = list.item(i).toElement();
                if (e.tagName() == "completion-entry") {
                    QDate date;
                    s = e.attribute("date");
                    if ( !s.isEmpty() ) {
                        date = QDate::fromString( s, Qt::ISODate );
                    }
                    if ( !date.isValid() ) {
                        kWarning()<<k_funcinfo<<"Illegal date: "<<date<<s<<endl;
                        continue;
                    }
                    Entry *entry = new Entry( e.attribute("percent-finished", "0").toInt(), Duration::fromString(e.attribute("remaining-effort")),  Duration::fromString(e.attribute("performed-effort")) );
                    addEntry( date, entry );
                } else if (e.tagName() == "used-effort") {
                    QDomNodeList list = e.childNodes();
                    for (unsigned int n=0; n<list.count(); ++n) {
                        if (list.item(n).isElement()) {
                            QDomElement el = list.item(n).toElement();
                            if (el.tagName() == "resource") {
                                QString id = el.attribute( "id" );
                                Resource *r = status.project().resource( id );
                                if ( r == 0 ) {
                                    kWarning()<<k_funcinfo<<"Cannot find resource, id="<<id<<endl;
                                    continue;
                                }
                                UsedEffort *ue = new UsedEffort();
                                addUsedEffort( r, ue );
                                ue->loadXML( el, status );
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

void Completion::saveXML(QDomElement &element )  const
{
    QDomElement el = element.ownerDocument().createElement("progress");
    element.appendChild(el);
    el.setAttribute("started", m_started);
    el.setAttribute("finished", m_finished);
    el.setAttribute("startTime", m_startTime.toString( KDateTime::ISODate ));
    el.setAttribute("finishTime", m_finishTime.toString( KDateTime::ISODate ));
    foreach( QDate date, m_entries.uniqueKeys() ) {
        QDomElement elm = el.ownerDocument().createElement("completion-entry");
        el.appendChild(elm);
        Entry *e = m_entries[ date ];
        elm.setAttribute( "date", date.toString( Qt::ISODate ) );
        elm.setAttribute( "percent-finished", e->percentFinished );
        elm.setAttribute( "remaining-effort", e->remainingEffort.toString() );
        elm.setAttribute( "performed-effort", e->totalPerformed.toString() );
        elm.setAttribute( "note", e->note );
    }
    if ( ! m_usedEffort.isEmpty() ) {
        QDomElement elm = el.ownerDocument().createElement("used-effort");
        el.appendChild(elm);
        ResourceUsedEffortMap::ConstIterator i = m_usedEffort.constBegin();
        for ( ; i != m_usedEffort.constEnd(); ++i ) {
            if ( i.value() == 0 ) {
                continue;
            }
            QDomElement e = elm.ownerDocument().createElement("resource");
            elm.appendChild(e);
            e.setAttribute( "id", i.key()->id() );
            i.value()->saveXML( e );
        }
    }
}

//--------------
Completion::UsedEffort::UsedEffort()
{
}

Completion::UsedEffort::UsedEffort( const UsedEffort &e )
{
    mergeEffort( e );
}

Completion::UsedEffort::~UsedEffort()
{
    qDeleteAll( m_actual );
}

void Completion::UsedEffort::mergeEffort( const Completion::UsedEffort &value )
{
    foreach ( QDate d, value.actualEffortMap().keys() ) {
        setEffort( d, new ActualEffort( *( value.actualEffortMap()[ d ] ) ) );
    }
}

void Completion::UsedEffort::setEffort( const QDate &date, ActualEffort *value )
{
    m_actual.insert( date, value );
}

Duration Completion::UsedEffort::effort() const
{
    Duration eff;
    foreach ( ActualEffort *e, m_actual.values() ) {
        eff += e->effort();
    }
    return eff;
}

bool Completion::UsedEffort::operator==( const Completion::UsedEffort &e ) const
{
    return m_actual == e.actualEffortMap();
}

bool Completion::UsedEffort::loadXML(QDomElement &element, XMLLoaderObject & )
{
    //kDebug()<<k_funcinfo<<endl;
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "actual-effort") {
                QDate date = QDate::fromString( e.attribute("date"), Qt::ISODate );
                if ( date.isValid() ) {
                    ActualEffort *a = new ActualEffort();
                    a->setNormalEffort( Duration::fromString( e.attribute( "normal-effort" ) ) );
                    a->setOvertimeEffort( Duration::fromString( e.attribute( "overtime-effort" ) ) );
                    setEffort( date, a );
                }
            }
        }
    }
    return true;
}

void Completion::UsedEffort::saveXML(QDomElement &element ) const
{
    if ( m_actual.isEmpty() ) {
        return;
    }
    DateUsedEffortMap::ConstIterator i = m_actual.constBegin();
    for ( ; i != m_actual.constEnd(); ++i ) {
        if ( i.value() == 0 ) {
            continue;
        }
        QDomElement el = element.ownerDocument().createElement("actual-effort");
        element.appendChild( el );
        el.setAttribute( "overtime-effort", i.value()->overtimeEffort().toString() );
        el.setAttribute( "normal-effort", i.value()->normalEffort().toString() );
        el.setAttribute( "date", i.key().toString( Qt::ISODate ) );
    }
}

//----------------------------------
#ifndef NDEBUG
void Task::printDebug(bool children, const QByteArray& _indent) {
    QByteArray indent = _indent;
    kDebug()<<indent<<"+ Task node: "<<name()<<" type="<<type()<<endl;
    indent += "!  ";
    kDebug()<<indent<<"Requested resources (total): "<<units()<<"%"<<endl;
    kDebug()<<indent<<"Requested resources (work): "<<workUnits()<<"%"<<endl;
    if (m_requests)
        m_requests->printDebug(indent);
    
    m_completion.printDebug( indent );
    
    Node::printDebug(children, indent);

}

void Completion::printDebug(const QByteArray& _indent) const {
    QByteArray indent = _indent;
    kDebug()<<indent<<"+ Completion: ("<<m_entries.count()<<" entries)"<<endl;
    indent += "!  ";
    kDebug()<<indent<<"Started: "<<m_started<<"  "<<m_startTime.toString()<<endl;
    kDebug()<<indent<<"Finished: "<<m_finished<<"  "<<m_finishTime.toString()<<endl;
    indent += "  ";
    foreach( QDate d, m_entries.keys() ) {
        Entry *e = m_entries[ d ];
        kDebug()<<indent<<"Date: "<<d<<endl;
        kDebug()<<(indent+" ! ")<<"% Finished: "<<e->percentFinished<<endl;
        kDebug()<<(indent+" ! ")<<"Remainig: "<<e->remainingEffort.toString()<<endl;
        kDebug()<<(indent+" ! ")<<"Performed: "<<e->totalPerformed.toString()<<endl;
    }
}


// Those method are used by the pert editor to link tasks between each others
void Task::addRequiredTask(Node * taskLinked){
    m_requiredTasks.append(taskLinked);
}

void Task::remRequiredTask(Node * taskLinked){
    int taskIndex;
    // Do nothing if the taskLinked is wrong
    taskIndex = m_requiredTasks.indexOf(taskLinked);
    if (taskIndex!=-1)
        m_requiredTasks.removeAt(taskIndex);
    else
        kDebug()<<"Task not found!"<<endl;
}

/*QList<Node *> Task::requiredTaskIterator() const{
    return m_requiredTasks;
}*/

DateTime Task::earlyStartDate()
{
    return m_earlyStartDate;
}

void Task::setEarlyStartDate(DateTime value)
{
    m_earlyStartDate=value;
}


DateTime Task::earlyFinishDate()
{
    return m_earlyFinishDate;
}

void Task::setEarlyFinishDate(DateTime value)
{
    m_earlyFinishDate=value;
}

DateTime Task::lateStartDate()
{
    return m_lateStartDate;
}

void Task::setLateStartDate(DateTime value)
{
    m_lateStartDate=value;
}

DateTime Task::lateFinishDate()
{
    return m_lateFinishDate;
}

void Task::setLateFinishDate(DateTime value)
{
    m_lateFinishDate=value;
}

int Task::activitySlack()
{
    return(m_activitySlack);
}

void Task::setActivitySlack(int value)
{
    m_activitySlack=value;
}

int Task::activityFreeMargin()
{
    return(m_activityFreeMargin);
}

void Task::setActivityFreeMargin(int value)
{
    m_activityFreeMargin=value;
}


#endif

}  //KPlato namespace
