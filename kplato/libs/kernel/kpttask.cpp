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
#include "kptduration.h"
#include "kptrelation.h"
#include "kptdatetime.h"
#include "kptcalendar.h"
#include "kpteffortcostmap.h"
#include "kptschedule.h"
#include "kptxmlloaderobject.h"

#include <KoXmlReader.h>

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
      m_workPackage( *this )
{
    //kDebug()<<"("<<this<<")";
    Duration d(1, 0, 0);
    m_estimate = new Estimate();
    m_estimate->setOptimisticRatio(-10);
    m_estimate->setPessimisticRatio(20);
    m_estimate->setParentNode( this );
    
    m_requests = 0;

    if (m_parent)
        m_leader = m_parent->leader();
}

Task::Task(Task &task, Node *parent) 
    : Node(task, parent), 
      m_resource(),
      m_workPackage( *this )
{
    //kDebug()<<"("<<this<<")";
    m_requests = 0;
    
    delete m_estimate;
    if ( task.estimate() ) {
        m_estimate = new Estimate( *( task.estimate() ) );
    } else {
        m_estimate = new Estimate();
    }
    m_estimate->setParentNode( this );
}


Task::~Task() {
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
    else if ( m_estimate->expectedEstimate() == 0.0 ) {
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
    //kDebug()<<m_name<<request<<request->group()<<request->group()->id()<<request->group()->name();
    if (!m_requests) {
        m_requests = new ResourceRequestCollection(*this);
    }
    m_requests->addRequest(request);
}

void Task::takeRequest(ResourceGroupRequest *request) {
    //kDebug()<<request;
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

QList<Resource*> Task::requestedResources() const {
    QList<Resource*> lst;
    if ( m_requests ) {
        lst << m_requests->requestedResources();
    }
    return lst;
}

bool Task::containsRequest( const QString &identity ) const {
    return m_requests == 0 ? false : m_requests->contains( identity );
}

ResourceRequest *Task::resourceRequest( const QString &name ) const {
    return m_requests == 0 ? 0 : m_requests->resourceRequest( name );
}

QStringList Task::assignedNameList( long id) const {
    Schedule *s = schedule( id );
    if ( s == 0 ) {
        return QStringList();
    }
    return s->resourceNameList();
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
            //kDebug()<<m_name<<":"<<m_currentSchedule->startTime<<","<<m_currentSchedule->endTime<<";"<<m_currentSchedule->duration.toString();
            m_requests->makeAppointments(m_currentSchedule);
            //kDebug()<<m_name<<":"<<m_currentSchedule->startTime<<","<<m_currentSchedule->endTime<<";"<<m_currentSchedule->duration.toString();
        }
    } else if (type() == Node::Type_Summarytask) {
        foreach (Node *n, m_nodes) {
            n->makeAppointments();
        }
    } else if (type() == Node::Type_Milestone) {
        //kDebug()<<"Milestone not implemented";
        // Well, shouldn't have resources anyway...
    }
}

void Task::copySchedule()
{
    if ( m_currentSchedule == 0 || type() != Node::Type_Task ) {
        return;
    }
    int id = m_currentSchedule->parentScheduleId();
    NodeSchedule *ns = static_cast<NodeSchedule*>( findSchedule( id ) );
    if ( ns == 0 ) {
        return;
    }
    if ( type() == Node::Type_Task ) {
        copyAppointments( ns->startTime, ns->endTime );
    }
    m_currentSchedule->startTime = ns->startTime;
    m_currentSchedule->earlyStart = ns->earlyStart;
    m_currentSchedule->endTime = ns->endTime;
    m_currentSchedule->lateFinish = ns->lateFinish;
    m_currentSchedule->duration = ns->duration;
    // TODO: status flags, etc
    kDebug();
}

void Task::copyAppointments()
{
    copyAppointments( DateTime(), m_currentSchedule->startTime );
}

void Task::copyAppointments( const DateTime &start, const DateTime &end )
{
    if ( m_currentSchedule == 0 || type() != Node::Type_Task ) {
        return;
    }
    int id = m_currentSchedule->parentScheduleId();
    NodeSchedule *ns = static_cast<NodeSchedule*>( findSchedule( id ) );
    if ( ns == 0 ) {
        return;
    }
    DateTime st = start.isValid() ? start : ns->startTime;
    DateTime et = end.isValid() ? end : ns->endTime;
    kDebug()<<m_name<<st.toString()<<et.toString();
    foreach ( Appointment *a, ns->appointments() ) {
        Resource *r = a->resource() == 0 ? 0 : a->resource()->resource();
        if ( r == 0 ) {
            kError()<<"No resource";
            continue;
        }
        AppointmentIntervalList lst = a->intervals(  st, et );
        if ( lst.isEmpty() ) {
            kDebug()<<"No intervals to copy from"<<a;
            continue;
        }
        Appointment *curr = 0;
        foreach ( Appointment *c, m_currentSchedule->appointments() ) {
            if ( c->resource()->resource() == r ) {
                kDebug()<<"Found current appointment to"<<a->resource()->resource()->name();
                curr = c;
                break;
            }
        }
        if ( curr == 0 ) {
            curr = new Appointment();
            m_currentSchedule->add( curr );
            curr->setNode( m_currentSchedule );
            kDebug()<<"Created new appointment";
        }
        ResourceSchedule *rs = static_cast<ResourceSchedule*>( r->findSchedule( m_currentSchedule->id() ) );
        if ( rs == 0 ) {
            rs = r->createSchedule( m_currentSchedule->parent() );
            rs->setId( m_currentSchedule->id() );
            rs->setName( m_currentSchedule->name() );
            rs->setType( m_currentSchedule->type() );
            kDebug()<<"Resource schedule not found, id="<<m_currentSchedule->id();
        }
        if ( ! rs->appointments().contains( curr ) ) {
            rs->add( curr );
            curr->setResource( rs );
        }
        Appointment app;
        app.setIntervals( lst );
        foreach ( AppointmentInterval *i, curr->intervals() ) {
            kDebug()<<i->startTime().toString()<<i->endTime().toString();
        }
        curr->merge( app );
        kDebug()<<"Appointments added";
    }
    m_currentSchedule->startTime = ns->startTime;
    m_currentSchedule->earlyStart = ns->earlyStart;
}

void Task::calcResourceOverbooked() {
    if (m_currentSchedule)
        m_currentSchedule->calcResourceOverbooked();
}

// A new constraint means start/end times and duration must be recalculated
void Task::setConstraint(Node::ConstraintType type) {
    m_constraint = type;
}


bool Task::load(KoXmlElement &element, XMLLoaderObject &status ) {
    QString s;
    bool ok = false;
    m_id = element.attribute("id");
    
    m_name = element.attribute("name");
    m_leader = element.attribute("leader");
    m_description = element.attribute("description");
    //kDebug()<<m_name<<": id="<<m_id;

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
    
    // Load the task children
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
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
        } else if (e.tagName() == "estimate" || 
                   ( /*status.version() < "0.6" &&*/ e.tagName() == "effort" ) ) {
            //  Load the estimate
            m_estimate->load(e, status);
        } else if (e.tagName() == "resourcegroup-request") {
            // Load the resource request
            // Handle multiple requests to same group gracefully (Not really allowed)
            if ( m_requests == 0 ) {
                m_requests = new ResourceRequestCollection( *this );
            }
            ResourceGroupRequest *r = m_requests->findGroupRequestById( e.attribute("group-id") );
            if ( r ) {
                kWarning()<<"Multiple requests to same group, loading into existing group";
                if ( ! r->load( e, status.project() ) ) {
                    kError()<<"Failed to load resource request"<<endl;
                }
            } else {
                r = new ResourceGroupRequest();
                if (r->load(e, status.project())) {
                    addRequest(r);
                } else {
                    kError()<<"Failed to load resource request"<<endl;
                    delete r;
                }
            }
        } else if (e.tagName() == "progress") {
            completion().loadXML( e, status );
        } else if (e.tagName() == "schedules") {
            KoXmlNode n = e.firstChild();
            for ( ; ! n.isNull(); n = n.nextSibling() ) {
                if ( ! n.isElement() ) {
                    continue;
                }
                KoXmlElement el = n.toElement();
                if (el.tagName() == "schedule") {
                    NodeSchedule *sch = new NodeSchedule();
                    if (sch->loadXML(el, status)) {
                        sch->setNode(this);
                        addSchedule(sch);
                    } else {
                        kError()<<"Failed to load schedule"<<endl;
                        delete sch;
                    }
                }
            }
        } else if (e.tagName() == "documents") {
            m_documents.load( e, status );
        }
    }
    //kDebug()<<m_name<<" loaded";
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
    
    me.setAttribute("wbs", wbsCode()); //NOTE: included for information
    
    m_estimate->save(me);

    completion().saveXML( me );
    
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
    
    m_documents.save( me );
    
    for (int i=0; i<numChildren(); i++) {
        childNode(i)->save(me);
    }
}

void Task::saveAppointments(QDomElement &element, long id) const {
    //kDebug()<<m_name<<" id="<<id;
    Schedule *sch = findSchedule(id);
    if (sch) {
        sch->saveAppointments(element);
    }
    foreach (Node *n, m_nodes) {
        n->saveAppointments(element, id);
    }
}

void Task::saveWorkPackageXML(QDomElement &element, long id )  const
{
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
    
    me.setAttribute("wbs", wbsCode()); // NOTE: included for information
    
    m_estimate->save(me);

    completion().saveXML( me );
    
    if ( m_schedules.contains( id ) && ! m_schedules[ id ]->isDeleted() ) {
        QDomElement schs = me.ownerDocument().createElement("schedules");
        me.appendChild(schs);
        m_schedules[ id ]->saveXML( schs );
    }
    m_documents.save( me ); // TODO: copying documents
}

EffortCostMap Task::plannedEffortCostPrDay(const QDate &start, const QDate &end, long id ) const {
    //kDebug()<<m_name;
    Schedule *s = schedule( id );
    if ( s ) {
        return s->plannedEffortCostPrDay(start, end);
    }
    return EffortCostMap();
}

// Returns the total planned effort for this task (or subtasks) 
Duration Task::plannedEffort( long id ) const {
   //kDebug();
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            eff += n->plannedEffort( id );
        }
        return eff;
    }
    Schedule *s = schedule( id );
    if ( s ) {
        eff = s->plannedEffort();
    }
    return eff;
}

// Returns the total planned effort for this task (or subtasks) on date
Duration Task::plannedEffort(const QDate &date, long id ) const {
   //kDebug();
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            eff += n->plannedEffort(date, id);
        }
        return eff;
    }
    Schedule *s = schedule( id );
    if ( s ) {
        eff = s->plannedEffort(date);
    }
    return eff;
}

// Returns the total planned effort for this task (or subtasks) upto and including date
Duration Task::plannedEffortTo(const QDate &date, long id) const {
    //kDebug();
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            eff += n->plannedEffortTo(date, id);
        }
        return eff;
    }
    Schedule *s = schedule( id );
    if ( s ) {
        eff = s->plannedEffortTo(date);
    }
    return eff;
}

// Returns the total actual effort for this task (or subtasks) 
Duration Task::actualEffort() const {
   //kDebug();
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            eff += n->actualEffort();
        }
    }
    return completion().actualEffort();
}

// Returns the total actual effort for this task (or subtasks) on date
Duration Task::actualEffort( const QDate &date ) const {
   //kDebug();
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            eff += n->actualEffort( date );
        }
        return eff;
    }
    return completion().actualEffort( date );
}

// Returns the total actual effort for this task (or subtasks) to date
Duration Task::actualEffortTo( const QDate &date ) const {
   //kDebug();
    Duration eff;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            eff += n->actualEffortTo( date );
        }
        return eff;
    }
    return completion().actualEffortTo( date );
}

double Task::plannedCost( long id ) const {
    //kDebug();
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            c += n->plannedCost( id );
        }
        return c;
    }
    Schedule *s = schedule( id );
    if ( s ) {
        c = s->plannedCost();
    }
    return c;
}

double Task::plannedCost(const QDate &date, long id) const {
    //kDebug();
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            c += n->plannedCost(date, id);
        }
        return c;
    }
    Schedule *s = schedule( id );
    if ( s ) {
        c = s->plannedCost(date);
    }
    return c;
}

double Task::plannedCostTo(const QDate &date, long id) const {
    //kDebug();
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            c += n->plannedCostTo(date, id);
        }
        return c;
    }
    Schedule *s = schedule( id );
    if ( s ) {
        c = s->plannedCostTo(date);
    }
    return c;
}

double Task::actualCost() const {
    //kDebug();
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            c += n->actualCost();
        }
        return c;
    }
    return completion().actualCost();
}

double Task::actualCost( const QDate &date ) const {
    //kDebug();
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            c += n->actualCost( date );
        }
        return c;
    }
    return completion().actualCost( date );
}

double Task::actualCostTo( const QDate &date ) const {
    //kDebug();
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            c += n->actualCostTo( date );
        }
        return c;
    }
    return completion().actualCostTo( date );
}

double Task::bcwp( long id ) const
{
    //kDebug();
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            c += n->bcwp(id);
        }
        return c;
    }
    return plannedCost( id ) * (double)completion().percentFinished() / 100.0;
}

double Task::bcwp( const QDate &date, long id ) const
{
    //kDebug();
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            c += n->bcwp( date, id );
        }
        return c;
    }
    c = plannedCostTo( date, id ) * (double)completion().percentFinished( date ) / 100.0;
    //kDebug()<<m_name<<"("<<id<<")"<<date<<"="<<c;
    return c;
}


//FIXME Handle summarytasks
double Task::effortPerformanceIndex(const QDate &date, bool *error) const {
    double res = 0.0;
    Duration ae = actualEffortTo(date);
    
    bool e = (ae == Duration::zeroDuration || completion().percentFinished() == 0);
    if (error) {
        *error = e;
    }
    if (!e) {
        res = (plannedEffortTo(date).toDouble() * ((double)completion().percentFinished()/100.0) / ae.toDouble());
    }
    return res;
}

//FIXME Handle summarytasks
double Task::costPerformanceIndex(const QDate &date, bool *error) const {
    double res = 0.0;
    double ac = actualCostTo(date);
    
    bool e = (ac == 0.0 || completion().percentFinished() == 0);
    if (error) {
        *error = e;
    }
    if (!e) {
        res = (plannedCostTo(date) * completion().percentFinished())/(100 * ac);
    }
    return res;
}

void Task::initiateCalculation(MainSchedule &sch) {
    //kDebug()<<m_name<<" schedule:"<<sch.name()<<" id="<<sch.id();
    m_visitedForward = false;
    m_visitedBackward = false;
    m_currentSchedule = createSchedule(&sch);
    m_currentSchedule->initiateCalculation();
    clearProxyRelations();
    Node::initiateCalculation(sch);
}


void Task::initiateCalculationLists(MainSchedule &sch) {
    //kDebug()<<m_name;
    if (type() == Node::Type_Summarytask) {
        sch.insertSummaryTask(this);
        // propagate my relations to my children and dependent nodes
        foreach (Node *n, m_nodes) {
            if (!dependParentNodes().isEmpty()) {
                n->addParentProxyRelations( dependParentNodes() );
            }
            if (!dependChildNodes().isEmpty()) {
                n->addChildProxyRelations( dependChildNodes() );
            }
            n->initiateCalculationLists(sch);
        }
    } else {
        if (isEndNode()) {
            sch.insertEndNode(this);
            //kDebug()<<"endnodes append:"<<m_name;
        }
        if (isStartNode()) {
            sch.insertStartNode(this);
            //kDebug()<<"startnodes append:"<<m_name;
        }
        if ( ( m_constraint == Node::MustStartOn && m_estimate->type() == Estimate::Type_FixedDuration ) ||
            ( m_constraint == Node::MustFinishOn && m_estimate->type() == Estimate::Type_FixedDuration ) ||
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
            //kDebug()<<"Skip summarytask:"<<it.current()->parent()->name();
            continue; // skip summarytasks
        }
        DateTime t = r->parent()->calculateForward(use); // early finish
        switch (r->type()) {
            case Relation::StartStart:
                // I can't start earlier than my predesseccor
                t = r->parent()->earlyStart() + r->lag();
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
    //kDebug()<<time.toString()<<""<<m_name<<" calculatePredeccessors() ("<<list.count()<<")";
    return time;
}

DateTime Task::calculateForward(int use) {
    if (m_currentSchedule == 0) {
        return DateTime();
    }
    Schedule *cs = m_currentSchedule;
    cs->setCalculationMode( Schedule::CalculateForward );
    // calculate all predecessors
    if (!dependParentNodes().isEmpty()) {
        DateTime time = calculatePredeccessors(dependParentNodes(), use);
        if (time.isValid() && time > cs->earlyStart) {
            cs->earlyStart = time;
        }
    }
    if (!m_parentProxyRelations.isEmpty()) {
        DateTime time = calculatePredeccessors(m_parentProxyRelations, use);
        if (time.isValid() && time > cs->earlyStart) {
            cs->earlyStart = time;
        }
    }
    return calculateEarlyFinish( use );
}

DateTime Task::calculateEarlyFinish(int use) {
    //kDebug()<<m_name;
    if (m_currentSchedule == 0) {
        return DateTime();
    }
    Schedule *cs = m_currentSchedule;
    bool pert = cs->usePert();
    cs->setCalculationMode( Schedule::CalculateForward );
    if (m_visitedForward) {
        //kDebug()<<earliestStart.toString()<<" +"<<m_durationBackward.toString()<<""<<m_name<<" calculateForward() (visited)";
        return cs->earlyFinish;
    }
    //kDebug()<<"------>"<<m_name<<""<<cs->earlyStart;
    if (type() == Node::Type_Task) {
        m_durationForward = m_estimate->value(use, pert);
        switch (constraint()) {
            case Node::ASAP:
            case Node::ALAP:
            {
                //kDebug()<<m_name<<" ASAP/ALAP:"<<cs->earlyStart;
                m_durationForward = duration(cs->earlyStart, use, false);
                cs->earlyFinish = cs->earlyStart + m_durationForward;
                //kDebug()<<m_name<<" ASAP/ALAP:"<<cs->earlyStart<<"+"<<m_durationForward.toString()<<"="<<cs->earlyFinish;
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = cs->earlyStart;
                    cs->endTime = cs->earlyFinish;
                    makeAppointments();
                }
                break;
            }
            case Node::MustFinishOn:
            {
                m_durationForward = duration(m_constraintEndTime, use, true);
                if ( cs->earlyStart >  m_constraintEndTime - m_durationForward ) {
                    m_durationForward = duration( cs->earlyStart, use, false );
                    cs->earlyFinish = cs->earlyStart + m_durationForward;
                } else {
                    cs->earlyFinish = m_constraintEndTime;
                    m_durationForward = m_constraintEndTime - cs->earlyStart;
                }
                //kDebug()<<"MustFinishOn:"<<m_constraintEndTime<<cs->earlyStart<<cs->earlyFinish;
                if (cs->earlyFinish > m_constraintEndTime) {
                    cs->schedulingError = true;
                    cs->negativeFloat = cs->earlyFinish - m_constraintEndTime;
                    //kWarning()<<"m_constraintEndTime < cs->earlyFinish";
                }
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = cs->earlyStart;
                    cs->endTime = cs->earlyFinish;
                    makeAppointments();
                }
                break;
            }
            case Node::FinishNotLater:
                m_durationForward = duration(cs->earlyStart, use, false);
                cs->earlyFinish = cs->earlyStart + m_durationForward;
                //kDebug()<<"FinishNotLater:"<<m_constraintEndTime<<cs->earlyStart<<cs->earlyFinish<<workStartAfter( cs->earlyStart );
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = cs->earlyStart;
                    cs->endTime = cs->earlyStart + m_durationForward;
                    makeAppointments();
                }
                break;
            case Node::MustStartOn:
            {
                //kDebug()<<"MustStartOn:"<<m_constraintStartTime<<cs->earlyStart;
                m_earlyStart = workStartAfter( cs->earlyStart );
                if ( cs->earlyStart > m_constraintStartTime ) {
                    cs->schedulingError = true;
                    m_durationForward = duration( cs->earlyStart, use, false );
                    cs->startTime = cs->earlyStart;
                    cs->endTime = cs->startTime + m_durationForward;
                } else {
                    // Always use duration from earlyStart
                    m_durationForward = duration( m_constraintStartTime, use, false );
                    cs->startTime = m_constraintStartTime;
                    cs->endTime = cs->startTime + m_durationForward;
                    m_durationForward += m_constraintStartTime - cs->earlyStart;
                }
                if ( !cs->allowOverbooking() ) {
                    makeAppointments();
                }
                break;
            }
            case Node::StartNotEarlier:
                //kDebug()<<"StartNotEarlier:"<<m_constraintStartTime<<cs->earlyStart;
                if ( cs->earlyStart > m_constraintStartTime ) {
                    m_durationForward = duration(cs->earlyStart, use, false);
                } else {
                    m_durationForward = duration(m_constraintStartTime, use, false);
                    cs->earlyFinish = m_constraintStartTime + m_durationForward;
                    m_durationForward = cs->earlyFinish - cs->earlyStart;
                }
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = cs->earlyStart;
                    cs->endTime = cs->earlyFinish;
                    makeAppointments();
                }
                break;
            case Node::FixedInterval: {
                if ( cs->earlyStart > m_constraintStartTime ) {
                    cs->schedulingError = true;
                }
                //cs->earlyStart = m_constraintStartTime;
                m_durationForward = m_constraintEndTime - cs->earlyStart;
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = m_constraintStartTime;
                    cs->endTime = m_constraintEndTime;
                    makeAppointments();
                }
                break;
            }
        }
    } else if (type() == Node::Type_Milestone) {
        m_durationForward = Duration::zeroDuration;
        switch (constraint()) {
            case Node::MustFinishOn:
                //kDebug()<<"MustFinishOn:"<<m_constraintEndTime<<cs->earlyStart;
                if ( cs->earlyStart < m_constraintEndTime ) {
                    m_durationForward = m_constraintEndTime - cs->earlyStart;
                }
                break;
            case Node::FinishNotLater:
                //kDebug()<<"FinishNotLater:"<<m_constraintEndTime<<cs->earlyStart;
                if ( cs->earlyStart < m_constraintEndTime ) {
                    m_durationForward = m_constraintEndTime - cs->earlyStart;
                }
                break;
            case Node::MustStartOn:
                //kDebug()<<"MustStartOn:"<<m_constraintStartTime<<cs->earlyStart;
                if ( cs->earlyStart < m_constraintStartTime ) {
                    m_durationForward = m_constraintStartTime - cs->earlyStart;
                }
                break;
            case Node::StartNotEarlier:
                //kDebug()<<"StartNotEarlier:"<<m_constraintStartTime<<cs->earlyStart;
                if ( cs->earlyStart < m_constraintStartTime ) {
                    m_durationForward = m_constraintStartTime - cs->earlyStart;
                }
                break;
            case Node::FixedInterval:
                break;
            default:
                break;
        }
        //kDebug()<<m_name<<""<<earliestStart.toString()<<endl
    } else if (type() == Node::Type_Summarytask) {
        kWarning()<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    } else { // ???
        m_durationForward = Duration::zeroDuration;
    }
    m_visitedForward = true;
    cs->insertForwardNode( this );
    cs->earlyFinish = cs->earlyStart + m_durationForward;
    //kDebug()<<cs->earlyStart<<"+"<<m_durationForward.toString()<<"="<<cs->earlyFinish<<""<<m_name;
    return cs->earlyFinish;
}

DateTime Task::calculateSuccessors(const QList<Relation*> &list, int use) {
    DateTime time;
    foreach (Relation *r, list) {
        if (r->child()->type() == Type_Summarytask) {
            //kDebug()<<"Skip summarytask:"<<r->parent()->name();
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
                t = r->child()->lateFinish() -  r->lag();
                break;
            default:
                t -= r->lag();
                break;
        }
        if (!time.isValid() || t < time)
            time = t;
    }
    //kDebug()<<time.toString()<<""<<m_name<<" calculateSuccessors() ("<<list.count()<<")";
    return time;
}

DateTime Task::calculateBackward(int use) {
    //kDebug()<<m_name;
    if (m_currentSchedule == 0) {
        return DateTime();
    }
    Schedule *cs = m_currentSchedule;
    cs->setCalculationMode( Schedule::CalculateBackward );
    // calculate all successors
    if (!dependChildNodes().isEmpty()) {
        DateTime time = calculateSuccessors(dependChildNodes(), use);
        if (time.isValid() && time < cs->lateFinish) {
            cs->lateFinish = time;
        }
    }
    if (!m_childProxyRelations.isEmpty()) {
        DateTime time = calculateSuccessors(m_childProxyRelations, use);
        if (time.isValid() && time < cs->lateFinish) {
            cs->lateFinish = time;
        }
    }
    return calculateLateStart( use );
}

DateTime Task::calculateLateStart(int use) {
    //kDebug()<<m_name;
    if (m_currentSchedule == 0) {
        return DateTime();
    }
    Schedule *cs = m_currentSchedule;
    cs->setCalculationMode( Schedule::CalculateBackward );
    bool pert = cs->usePert();
    if (m_visitedBackward) {
        //kDebug()<<latestFinish.toString()<<" -"<<m_durationBackward.toString()<<""<<m_name<<" calculateBackward() (visited)";
        return cs->lateStart;
    }
    //kDebug()<<m_name<<" id="<<cs->id()<<" mode="<<cs->calculationMode()<<": latestFinish="<<cs->lateFinish;
    if (type() == Node::Type_Task) {
        m_durationBackward = m_estimate->value(use, pert);
        switch (constraint()) {
            case Node::ASAP:
            case Node::ALAP:
                //kDebug()<<m_name<<" ASAP/ALAP:"<<cs->lateFinish;
                m_durationBackward = duration(cs->lateFinish, use, true);
                cs->lateStart = cs->lateFinish - m_durationBackward;
                //kDebug()<<m_name<<" ASAP/ALAP:"<<cs->lateFinish<<"-"<<m_durationBackward.toString()<<"="<<cs->lateStart;
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = cs->lateStart;
                    cs->endTime = cs->lateFinish;
                    makeAppointments();
                }
                break;
            case Node::MustStartOn:
            {
                //kDebug()<<"MustStartOn:"<<m_constraintStartTime<<cs->lateFinish;
                m_durationBackward = duration(cs->lateFinish, use, true);
                cs->lateStart = cs->lateFinish - m_durationBackward;
                if ( cs->lateStart > m_constraintStartTime || cs->lateFinish < m_constraintStartTime) {
                    cs->schedulingError = true;
                }
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = cs->lateStart;
                    cs->endTime = cs->lateFinish;
                    makeAppointments();
                }
                break;
            }
            case Node::StartNotEarlier:
            {
                //kDebug()<<"StartNotEarlier:"<<m_constraintStartTime<<cs->lateFinish;
                m_durationBackward = duration(cs->lateFinish, use, true);
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = cs->lateFinish - m_durationBackward;
                    cs->endTime = cs->lateFinish;
                    makeAppointments();
                }
                break;
            }
            case Node::MustFinishOn:
                //kDebug()<<"MustFinishOn:"<<m_constraintEndTime<<cs->lateFinish;
                if ( cs->lateFinish < m_constraintEndTime ) {
                    cs->schedulingError = true;
                    //kWarning()<<"cs->lateFinish < m_constraintEndTime";
                    m_durationBackward = duration(cs->lateFinish, use, true);
                    cs->endTime = cs->lateFinish;
                } else {
                    m_durationBackward = duration(m_constraintEndTime, use, true);
                    cs->lateStart = m_constraintEndTime - m_durationBackward;
                    m_durationBackward = cs->lateFinish - cs->lateStart;
                    cs->endTime = m_constraintEndTime;
                }
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = cs->lateStart;
                    makeAppointments();
                }
                break;
            case Node::FinishNotLater:
                //kDebug()<<"FinishNotLater:"<<m_constraintEndTime<<cs->lateFinish;
                if ( m_constraintEndTime < cs->lateFinish ) {
                    m_durationBackward = duration(m_constraintEndTime, use, true);
                    cs->lateStart = m_constraintEndTime - m_durationBackward;
                    m_durationBackward = cs->lateFinish - cs->lateStart;
                } else {
                    m_durationBackward = duration(cs->lateFinish, use, true);
                }
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = cs->lateStart;
                    cs->endTime = cs->lateFinish;
                    makeAppointments();
                }
                break;
            case Node::FixedInterval: {
                //cs->lateFinish = m_constraintEndTime;
                if ( cs->lateFinish < m_constraintEndTime ) {
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
                //kDebug()<<"MustFinishOn:"<<m_constraintEndTime<<cs->lateFinish;
                if ( m_constraintEndTime < cs->lateFinish ) {
                    m_durationBackward = cs->lateFinish - m_constraintEndTime;
                }
                break;
            case Node::FinishNotLater:
                //kDebug()<<"FinishNotLater:"<<m_constraintEndTime<<cs->lateFinish;
                if ( m_constraintEndTime < cs->lateFinish ) {
                    m_durationBackward = cs->lateFinish - m_constraintEndTime;
                }
                break;
            case Node::MustStartOn:
                //kDebug()<<"MustStartOn:"<<m_constraintStartTime<<cs->lateFinish;
                if ( m_constraintStartTime < cs->lateFinish ) {
                    m_durationBackward = cs->lateFinish - m_constraintStartTime;
                }
                break;
            case Node::StartNotEarlier:
                //kDebug()<<"MustStartOn:"<<m_constraintStartTime<<cs->lateFinish;
                if ( m_constraintStartTime < cs->lateFinish ) {
                    m_durationBackward = cs->lateFinish - m_constraintStartTime;
                }
                break;
            case Node::FixedInterval:
                break;
            default:
                break;
        }
        m_lateFinish = cs->lateFinish;
        //kDebug()<<m_name<<""<<cs->lateFinish;
    } else if (type() == Node::Type_Summarytask) {
        kWarning()<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    } else { // ???
        m_durationBackward = Duration::zeroDuration;
    }
    m_visitedBackward = true;
    cs->insertBackwardNode( this );
    cs->lateStart = cs->lateFinish - m_durationBackward;
    //kDebug()<<cs->lateFinish<<"-"<<m_durationBackward.toString()<<"="<<cs->lateStart<<" "<<m_name;
    return cs->lateStart;
}

DateTime Task::schedulePredeccessors(const QList<Relation*> &list, int use) {
    DateTime time;
    foreach (Relation *r, list) {
        if (r->parent()->type() == Type_Summarytask) {
            //kDebug()<<"Skip summarytask:"<<r->parent()->name();
            continue; // skip summarytasks
        }
        // schedule the predecessors
        DateTime earliest = r->parent()->earlyStart();
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
    //kDebug()<<time.toString()<<""<<m_name<<" schedulePredeccessors()";
    return time;
}

DateTime Task::scheduleForward(const DateTime &earliest, int use) {
    if (m_currentSchedule == 0) {
        return DateTime();
    }
    Schedule *cs = m_currentSchedule;
    cs->setCalculationMode( Schedule::Scheduling );
    cs->startTime = earliest > cs->earlyStart ? earliest : cs->earlyStart;
    // First, calculate all my own predecessors
    DateTime time = schedulePredeccessors(dependParentNodes(), use);
    if (time.isValid() && time > cs->startTime) {
        cs->startTime = time;
        //kDebug()<<m_name<<" new startime="<<cs->startTime;
    }
    // Then my parents
    time = schedulePredeccessors(m_parentProxyRelations, use);
    if (time.isValid() && time > cs->startTime) {
        cs->startTime = time;
        //kDebug()<<m_name<<" new startime="<<cs->startTime;
    }
    return scheduleFromStartTime( use );
}

DateTime Task::scheduleFromStartTime(int use) {
    //kDebug()<<m_name;
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
        cs->startTime = cs->earlyStart;
    }
    //kDebug()<<m_name<<" startTime="<<cs->startTime;
    if(type() == Node::Type_Task) {
        if ( cs->recalculate() && completion().isFinished() ) {
            copySchedule();
            m_visitedForward = true;
            return cs->endTime;
        }
        cs->duration = m_estimate->value(use, pert);
        switch (m_constraint) {
        case Node::ASAP:
            // cs->startTime calculated above
            //kDebug()<<m_name<<"ASAP:"<<cs->startTime<<"earliest:"<<cs->earlyStart;
            if ( m_estimate->type() != Estimate::Type_FixedDuration ) {
                cs->startTime = workStartAfter( cs->startTime );
            }
            cs->duration = duration(cs->startTime, use, false);
            cs->endTime = cs->startTime + cs->duration;
            makeAppointments();
            if ( cs->recalculate() && completion().isStarted() ) {
                // copy start times + appointments from parent schedule
                copyAppointments();
                cs->duration = cs->endTime - cs->startTime;
            }
            if ( m_estimate->type() == Estimate::Type_FixedDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            } else {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            }
            break;
        case Node::ALAP:
            // cs->startTime calculated above
            //kDebug()<<m_name<<"ALAP:"<<cs->startTime<<cs->endTime<<" latest="<<cs->lateFinish;
            if ( estimate()->type() != Estimate::Type_FixedDuration ) {
                cs->endTime = workFinishBefore( cs->lateFinish );
            }
            cs->duration = duration(cs->endTime, use, true);
            cs->startTime = cs->endTime - cs->duration;
            //kDebug()<<m_name<<" endTime="<<cs->endTime<<" latest="<<cs->lateFinish;
            makeAppointments();
            if ( m_estimate->type() == Estimate::Type_FixedDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            } else {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            }
            if ( cs->recalculate() && completion().isStarted() ) {
                cs->earlyStart = cs->startTime = completion().startTime();
            }
            break;
        case Node::StartNotEarlier:
            // cs->startTime calculated above
            //kDebug()<<"StartNotEarlier:"<<m_constraintStartTime<<cs->startTime<<cs->lateStart;
            if ( cs->startTime < m_constraintStartTime ) {
                cs->startTime = m_constraintStartTime;
            }
            if ( estimate()->type() != Estimate::Type_FixedDuration ) {
                cs->startTime = workStartAfter( cs->startTime );
            }
            cs->duration = duration(cs->startTime, use, false);
            cs->endTime = cs->startTime + cs->duration;
            if (cs->endTime > cs->lateFinish) {
                cs->schedulingError = true;
                cs->negativeFloat = cs->endTime - cs->lateFinish;
            }
            makeAppointments();
            if ( m_estimate->type() == Estimate::Type_FixedDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            } else {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            }
            break;
        case Node::FinishNotLater:
            // cs->startTime calculated above
            //kDebug()<<"FinishNotLater:"<<m_constraintEndTime<<cs->startTime;
            if ( m_estimate->type() != Estimate::Type_FixedDuration ) {
                cs->startTime = workStartAfter( cs->startTime );
            }
            cs->duration = duration(cs->startTime, use, false);
            cs->endTime = cs->startTime + cs->duration;
            if (cs->endTime > m_constraintEndTime) {
                //kWarning()<<"cs->endTime > m_constraintEndTime";
                cs->schedulingError = true;
                cs->negativeFloat = cs->endTime - m_constraintEndTime;
            }
            makeAppointments();
            if ( m_estimate->type() == Estimate::Type_FixedDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            } else {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            }
            break;
        case Node::MustStartOn:
            // cs->startTime calculated above as predecessors endTime
            if ( m_estimate->type() != Estimate::Type_FixedDuration ) {
                cs->startTime = workStartAfter( cs->startTime );
            }
            //kDebug()<<"MustStartOn="<<m_constraintStartTime<<"<"<<cs->startTime;
            if (m_constraintStartTime < cs->startTime ) {
                cs->schedulingError = true;
            }
            cs->startTime = m_constraintStartTime; //Hmmmm
            cs->duration = duration(cs->startTime, use, false);
            cs->endTime = cs->startTime + cs->duration;
            makeAppointments();
            if ( m_estimate->type() == Estimate::Type_FixedDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            } else {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            }
            break;
        case Node::MustFinishOn:
            if ( m_estimate->type() != Estimate::Type_FixedDuration ) {
                cs->endTime = workFinishBefore( m_constraintEndTime );
            }
            if ( m_constraintEndTime != cs->endTime ) {
                cs->schedulingError = true;
                //kWarning()<<"cs->lateFinish != m_constraintEndTime";
            }
            cs->duration = duration(cs->endTime, use, true);
            if ( cs->startTime > cs->endTime - cs->duration ) {
                cs->schedulingError = true;
                cs->negativeFloat = cs->startTime - cs->endTime - cs->duration;
                cs->startTime = workStartAfter( cs->startTime );
                cs->duration = duration(cs->startTime, use, false );
                cs->endTime = cs->startTime + cs->duration;
            } else {
                cs->startTime = cs->endTime - cs->duration;
            }
            //kDebug()<<"MustFinishOn:"<<m_constraintEndTime<<","<<cs->lateFinish<<":"<<cs->startTime<<cs->endTime;
            makeAppointments();
            // TODO check
            if ( m_estimate->type() == Estimate::Type_FixedDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            } else {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            }
            break;
        case Node::FixedInterval: {
            // cs->startTime calculated above
            //kDebug()<<"FixedInterval="<<m_constraintStartTime<<""<<cs->startTime;
            if ( m_constraintStartTime < cs->startTime ) {
                cs->schedulingError = true;
                cs->negativeFloat = cs->startTime - m_constraintStartTime;
            }
            cs->startTime = m_constraintStartTime;
            cs->endTime = m_constraintEndTime;
            cs->duration = cs->endTime - cs->startTime;
            if ( cs->endTime > cs->lateFinish ) {
                cs->schedulingError = true;
                cs->negativeFloat = cs->endTime - cs->lateFinish;
            }
            cs->workStartTime = m_constraintStartTime;
            cs->workEndTime = m_constraintEndTime;
            //kDebug()<<"FixedInterval="<<cs->startTime<<","<<cs->endTime;
            makeAppointments();
            // TODO check
            if ( m_estimate->type() == Estimate::Type_FixedDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            } else {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            }
            break;
        }
        default:
            break;
        }
    } else if (type() == Node::Type_Milestone) {
        if ( cs->recalculate() && completion().isFinished() ) {
            cs->startTime = completion().startTime();
            cs->endTime = completion().finishTime();
            m_visitedForward = true;
            return cs->endTime;
        }
        switch (m_constraint) {
        case Node::ASAP: {
            cs->endTime = cs->startTime;
            // TODO check, do we need to check succeccors earliestStart?
            cs->positiveFloat = cs->lateFinish - cs->endTime;
            break;
        }
        case Node::ALAP: {
            cs->startTime = cs->lateFinish;
            cs->endTime = cs->lateFinish;
            cs->positiveFloat = Duration::zeroDuration;
            break;
        }
        case Node::MustStartOn:
        case Node::FixedInterval:
            //kDebug()<<"MustStartOn:"<<m_constraintStartTime<<cs->startTime;
            if (m_constraintStartTime < cs->startTime ) {
                cs->schedulingError = true;
                cs->negativeFloat = cs->startTime - m_constraintStartTime;
            } else if ( m_constraintStartTime > cs->lateFinish ) {
                cs->schedulingError = true;
                cs->negativeFloat = m_constraintStartTime - cs->lateFinish;
            } else {
                cs->startTime = m_constraintStartTime;
            }
            cs->endTime = cs->startTime;
            cs->positiveFloat = cs->lateFinish - cs->endTime; // FIXME: Work??
            break;
        case Node::MustFinishOn:
            if (m_constraintEndTime < cs->startTime ||
                m_constraintEndTime > cs->lateFinish) {
                cs->schedulingError = true;
            }
            cs->startTime = m_constraintEndTime;
            cs->endTime = m_constraintEndTime;
            cs->positiveFloat = cs->lateFinish - cs->endTime;
            break;
        case Node::StartNotEarlier:
            if (cs->startTime < m_constraintStartTime) {
                cs->startTime = m_constraintStartTime < cs->lateStart ? m_constraintStartTime : cs->lateStart;
            }
            if ( cs->startTime < m_constraintStartTime ) {
                cs->schedulingError = true;
                cs->negativeFloat = m_constraintStartTime - cs->startTime; //???
            }
            cs->endTime = cs->startTime;
            cs->positiveFloat = cs->lateFinish - cs->endTime;
            break;
        case Node::FinishNotLater:
            //kDebug()<<m_constraintEndTime<<cs->startTime;
            if (cs->startTime < m_constraintEndTime) {
                cs->startTime = m_constraintEndTime < cs->lateStart ? m_constraintEndTime : cs->lateStart;
            }
            if (cs->startTime > m_constraintEndTime) {
                cs->schedulingError = true;
            }
            cs->endTime = cs->startTime;
            cs->positiveFloat = cs->lateFinish - cs->endTime;
            break;
        default:
            break;
        }
        cs->duration = Duration::zeroDuration;
        //kDebug()<<m_name<<":"<<cs->startTime<<","<<cs->endTime;
    } else if (type() == Node::Type_Summarytask) {
        //shouldn't come here
        cs->endTime = cs->startTime;
        cs->duration = cs->endTime - cs->startTime;
        kWarning()<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    }
    //kDebug()<<cs->startTime<<" :"<<cs->endTime<<""<<m_name<<" scheduleForward()";
    m_visitedForward = true;
    return cs->endTime;
}

DateTime Task::scheduleSuccessors(const QList<Relation*> &list, int use) {
    DateTime time;
    foreach (Relation *r, list) {
        if (r->child()->type() == Type_Summarytask) {
            //kDebug()<<"Skip summarytask:"<<r->child()->name();
            continue;
        }
        // get the successors starttime
        DateTime latest = r->child()->lateFinish();
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
    cs->endTime = latest < cs->lateFinish ? latest : cs->lateFinish;
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
    //kDebug()<<m_name;
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
        cs->endTime = cs->lateFinish;
    }
    //kDebug()<<"------>"<<m_name<<""<<cs->endTime;
    if (type() == Node::Type_Task) {
        cs->duration = m_estimate->value(use, pert);
        switch (m_constraint) {
        case Node::ASAP: {
            // cs->endTime calculated above
            //kDebug()<<m_name<<": end="<<cs->endTime<<"  early="<<cs->earlyStart;
            if ( m_estimate->type() != Estimate::Type_FixedDuration ) {
                cs->startTime = workStartAfter( cs->earlyStart );
            }
            cs->duration = duration(cs->startTime, use, false);
            DateTime e = cs->startTime + cs->duration;
            if (e > cs->endTime) {
                cs->schedulingError = true;
                cs->negativeFloat = e - cs->endTime;
            }
            cs->endTime = e;
            //kDebug()<<m_name<<": start="<<cs->startTime<<"+"<<cs->duration.toString()<<"="<<e<<" -> end="<<cs->endTime;
            makeAppointments();
            if ( m_estimate->type() == Estimate::Type_FixedDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            } else {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            }
            break;
        }
        case Node::ALAP:
        {
            // cs->endTime calculated above
            //kDebug()<<m_name<<": end="<<cs->endTime<<"  late="<<cs->lateFinish<<endl;
            if ( m_estimate->type() != Estimate::Type_FixedDuration ) {
                cs->endTime = workFinishBefore(cs->endTime);
            }
            cs->duration = duration(cs->endTime, use, true);
            cs->startTime = cs->endTime - cs->duration;
            if ( cs->startTime < cs->earlyStart ) {
                cs->schedulingError = true;
                cs->negativeFloat = cs->earlyStart - cs->startTime;
            }
            //kDebug()<<m_name<<": lateStart="<<cs->startTime;
            makeAppointments();
            if ( m_estimate->type() == Estimate::Type_FixedDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            } else {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            }
            break;
        }
        case Node::StartNotEarlier:
            // cs->endTime calculated above
            //kDebug()<<"StartNotEarlier:"<<m_constraintStartTime<<cs->endTime;
            if ( m_estimate->type() != Estimate::Type_FixedDuration ) {
                cs->endTime = workFinishBefore( cs->endTime );
            }
            cs->duration = duration(cs->endTime, use, true);
            cs->startTime = cs->endTime - cs->duration;
            if ( m_constraintStartTime > cs->startTime ) {
                //kWarning()<<"m_constraintStartTime > cs->lateStart";
                cs->schedulingError = true;
                cs->negativeFloat = m_constraintStartTime - cs->startTime;
            }
            if ( cs->startTime < cs->earlyStart || cs->startTime > cs->lateStart ) {
                //kWarning()<<"startTime outside dependencies";
                cs->schedulingError = true;
            }
            makeAppointments();
            if ( m_estimate->type() == Estimate::Type_FixedDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            } else {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            }
            break;
        case Node::FinishNotLater:
            // cs->endTime calculated above
            //kDebug()<<"FinishNotLater:"<<m_constraintEndTime<<cs->endTime;
            if (cs->endTime > m_constraintEndTime) {
                cs->endTime = m_constraintEndTime;
            }
            if ( m_estimate->type() != Estimate::Type_FixedDuration ) {
                cs->endTime = workFinishBefore( cs->endTime );
            }
            if ( cs->endTime > cs->lateFinish ) {
                cs->endTime = cs->lateFinish;
            }
            cs->duration = duration(cs->endTime, use, true);
            cs->startTime = cs->endTime - cs->duration;
            if ( cs->startTime < cs->earlyStart ) {
                cs->negativeFloat = cs->earlyStart - cs->startTime;
            }
            makeAppointments();
            if ( m_estimate->type() == Estimate::Type_FixedDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            } else {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            }
            break;
        case Node::MustStartOn:
            // cs->endTime calculated above
            //kDebug()<<"MustStartOn="<<m_constraintStartTime.toString()<<""<<cs->startTime.toString();
            if (m_constraintStartTime > cs->lateFinish - m_durationBackward) {
                cs->schedulingError = true;
                cs->startTime = cs->lateFinish - m_durationBackward;
                if ( m_estimate->type() != Estimate::Type_FixedDuration ) {
                    cs->endTime = workFinishBefore(cs->endTime);
                }
            } else {
                cs->startTime = m_constraintStartTime;
                cs->duration = duration(cs->startTime, use, false);
                if ( cs->endTime < cs->startTime + cs->duration ) {
                    cs->schedulingError = true;
                }
                cs->endTime = cs->startTime + cs->duration;
            }
            makeAppointments();
            if ( m_estimate->type() == Estimate::Type_FixedDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            } else {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            }
            break;
        case Node::MustFinishOn:
            // cs->endTime calculated above
            //kDebug()<<"MustFinishOn:"<<m_constraintEndTime<<cs->endTime<<cs->earlyFinish;
            if (m_constraintEndTime > cs->endTime ) {
                cs->negativeFloat = m_constraintEndTime - cs->endTime;
                cs->schedulingError = true;
                //kWarning()<<"m_constraintEndTime > cs->endTime";
            } else if (m_constraintEndTime < cs->earlyFinish) {
                cs->schedulingError = true;
                cs->negativeFloat = cs->earlyFinish - m_constraintEndTime;
                cs->endTime = cs->earlyFinish;
                //kWarning()<<"m_constraintEndTime < cs->earlyFinish";
            } else {
                cs->endTime = m_constraintEndTime;
            }
            cs->duration = duration(cs->endTime, use, true);
            cs->startTime = cs->endTime - cs->duration;
            makeAppointments();
            if ( m_estimate->type() == Estimate::Type_FixedDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            } else {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            }
            break;
        case Node::FixedInterval: {
            // cs->endTime calculated above
            //kDebug()<<"FixedInterval="<<m_constraintEndTime<<""<<cs->endTime;
            if (m_constraintEndTime > cs->endTime) {
                cs->schedulingError = true;
                //kDebug()<<"FixedInterval error:"<<m_constraintEndTime<<" >"<<cs->endTime;
            }
            cs->startTime = m_constraintStartTime;
            cs->endTime = m_constraintEndTime;
            cs->duration = cs->endTime - cs->startTime;
            cs->workStartTime = m_constraintStartTime;
            cs->workEndTime = m_constraintEndTime;
            makeAppointments();
            if ( m_estimate->type() == Estimate::Type_FixedDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            } else {
                // TODO check
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            }
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
            if ( cs->endTime < cs->earlyStart ) {
                cs->schedulingError = true;
                cs->negativeFloat = cs->earlyStart - cs->endTime;
                cs->endTime = cs->earlyStart;
            } else {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            }
            cs->startTime = cs->endTime;
            break;
        case Node::ALAP:
            cs->startTime = cs->endTime;
            cs->positiveFloat = cs->lateFinish - cs->endTime;
            break;
        case Node::MustStartOn:
        case Node::FixedInterval:
            if (m_constraintStartTime < cs->earlyStart ||
                m_constraintStartTime > cs->endTime) {
                cs->schedulingError = true;
            }
            cs->startTime = cs->earlyStart;
            cs->endTime = cs->earlyStart;
            cs->positiveFloat = cs->lateFinish - cs->endTime;
            break;
        case Node::MustFinishOn:
            if (m_constraintEndTime < cs->earlyStart ||
                m_constraintEndTime > cs->endTime) {
                cs->schedulingError = true;
            }
            cs->startTime = m_constraintEndTime;
            cs->endTime = m_constraintEndTime;
            cs->positiveFloat = cs->lateFinish - cs->endTime;
            break;
        case Node::StartNotEarlier:
            if ( m_constraintStartTime < cs->endTime ) {
                cs->endTime = m_constraintStartTime > cs->earlyStart ? m_constraintStartTime : cs->earlyStart;
            }
            cs->startTime = cs->endTime;
            if (m_constraintStartTime > cs->startTime) {
                cs->schedulingError = true;
                cs->negativeFloat = m_constraintStartTime - cs->startTime;
            }
            cs->positiveFloat = cs->lateFinish - cs->endTime;
            break;
        case Node::FinishNotLater:
            if ( m_constraintEndTime < cs->endTime ) {
                cs->endTime = m_constraintEndTime > cs->earlyStart ? m_constraintEndTime : cs->earlyStart;
            }
            if (m_constraintEndTime < cs->endTime) {
                cs->schedulingError = true;
                cs->negativeFloat = cs->endTime - m_constraintEndTime;
            }
            cs->startTime = cs->endTime;
            cs->positiveFloat = cs->lateFinish - cs->endTime;
            break;
        default:
            break;
        }
        cs->duration = Duration::zeroDuration;
    } else if (type() == Node::Type_Summarytask) {
        //shouldn't come here
        cs->startTime = cs->endTime;
        cs->duration = cs->endTime - cs->startTime;
        kWarning()<<"Summarytasks should not be calculated here: "<<m_name<<endl;
    }
    //kDebug()<<m_name<<":"<<cs->startTime<<" :"<<cs->endTime;
    m_visitedBackward = true;
    return cs->startTime;
}

void Task::adjustSummarytask() {
    if (m_currentSchedule == 0)
        return;
    if (type() == Type_Summarytask) {
        DateTime start = m_currentSchedule->lateFinish;
        DateTime end = m_currentSchedule->earlyStart;
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
        //kDebug()<<cs->name<<":"<<m_currentSchedule->startTime.toString()<<" :"<<m_currentSchedule->endTime.toString();
    }
}

Duration Task::duration(const DateTime &time, int use, bool backward) {
    //kDebug();
    // TODO: handle risc
    if (!time.isValid()) {
        kError()<<"Time is invalid"<<endl;
        return Duration::zeroDuration;
    }
    if (m_currentSchedule == 0) {
        return Duration::zeroDuration;
        kError()<<"No current schedule"<<endl;
    }
    //kDebug()<<m_name<<": Use="<<use;
    Duration eff;
    if ( m_currentSchedule->recalculate() && completion().isStarted() ) {
        eff = completion().remainingEffort();
        //kDebug()<<m_name<<": recalculate, effort="<<eff.toDouble(Duration::Unit_h);
        if ( eff == 0 || completion().isFinished() ) {
            return eff;
        }
    } else {
        eff = m_estimate->value(use, m_currentSchedule->usePert());
    }
    return calcDuration(time, eff, backward);
}


Duration Task::calcDuration(const DateTime &time, const Duration &effort, bool backward) {
    //kDebug()<<"--------> calcDuration"<<(backward?"(B)":"(F)")<<m_name<<" time="<<time<<" effort="<<effort.toString(Duration::Format_Day);
    
    // Allready checked: m_currentSchedule and time.
    Duration dur = effort; // use effort as default duration
    if (m_estimate->type() == Estimate::Type_Effort) {
        if (m_requests == 0 || m_requests->isEmpty()) {
            m_currentSchedule->resourceError = true;
            return effort;
        }
        dur = m_requests->duration(time, effort, m_currentSchedule, backward);
        if (dur == Duration::zeroDuration) {
            kWarning()<<"zero duration: Resource not available"<<endl;
            m_currentSchedule->resourceNotAvailable = true;
            dur = effort; //???
        }
        return dur;
    }
    if (m_estimate->type() == Estimate::Type_FixedDuration) {
        //TODO: Different types of fixed duration
        return dur; //
    }
    kError()<<"Unsupported estimate type: "<<m_estimate->type()<<endl;
    return dur;
}

void Task::clearProxyRelations() {
    m_parentProxyRelations.clear();
    m_childProxyRelations.clear();
}

void Task::addParentProxyRelations( const QList<Relation*> &list )
{
    //kDebug()<<m_name;
    if (type() == Type_Summarytask) {
        // propagate to my children
        //kDebug()<<m_name<<" is summary task";
        foreach (Node *n, m_nodes) {
            n->addParentProxyRelations(list);
            n->addParentProxyRelations(dependParentNodes());
        }        
    } else {
        // add 'this' as child relation to the relations parent
        //kDebug()<<m_name<<" is not summary task";
        foreach (Relation *r, list) {
            r->parent()->addChildProxyRelation(this, r);
            // add a parent relation to myself
            addParentProxyRelation(r->parent(), r);
        }
    }
}

void Task::addChildProxyRelations( const QList<Relation*> &list) {
    //kDebug()<<m_name;
    if (type() == Type_Summarytask) {
        // propagate to my children
        //kDebug()<<m_name<<" is summary task";
        foreach (Node *n, m_nodes) {
            n->addChildProxyRelations(list);
            n->addChildProxyRelations(dependChildNodes());
        }        
    } else {
        // add 'this' as parent relation to the relations child
        //kDebug()<<m_name<<" is not summary task";
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
            //kDebug()<<"Add parent proxy from my children"<<m_name<<" to"<<node->name();
            foreach (Node *n, m_nodes) {
                n->addParentProxyRelation(node, rel);
            }
        } else {
            //kDebug()<<"Add parent proxy from"<<node->name()<<" to (me)"<<m_name;
            m_parentProxyRelations.append(new ProxyRelation(node, this, rel->type(), rel->lag()));
        }
    }
}

void Task::addChildProxyRelation(Node *node, const Relation *rel) {
    if (node->type() != Type_Summarytask) {
        if (type() == Type_Summarytask) {
            //kDebug()<<"Add child proxy from my children"<<m_name<<" to"<<node->name();
            foreach (Node *n, m_nodes) {
                n->addChildProxyRelation(node, rel);
            }
        } else {
            //kDebug()<<"Add child proxy from (me)"<<m_name<<" to"<<node->name();
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
        //kDebug()<<"id="<<m_currentSchedule->id()<<" mode="<<m_currentSchedule->calculationMode()<<":"<<m_name<<dt<<t;
        return t.isValid() ? t : dt;
    }
    return dt;
}

DateTime Task::workFinishBefore(const DateTime &dt)
{
    if (m_requests) {
        DateTime t = m_requests->availableBefore(dt, m_currentSchedule);
        //kDebug()<<"id="<<m_currentSchedule->id()<<" mode="<<m_currentSchedule->calculationMode()<<":"<<m_name<<dt<<t;
        return t.isValid() ? t : dt;
    }
    return dt;
}

Duration Task::positiveFloat( long id ) const
{
    Schedule *s = schedule( id );
    return s == 0 ? Duration::zeroDuration : s->positiveFloat;
}

Duration Task::negativeFloat( long id ) const
{
    Schedule *s = schedule( id );
    return s == 0 ? Duration::zeroDuration : s->negativeFloat;
}

Duration Task::freeFloat( long id ) const
{
    Schedule *s = schedule( id );
    return s == 0 ? Duration::zeroDuration : s->freeFloat;
}

Duration Task::startFloat( long id ) const
{
    Schedule *s = schedule( id );
    return s == 0 ? Duration::zeroDuration : ( s->earlyStart - s->lateStart );
}

Duration Task::finishFloat( long id ) const
{
    Schedule *s = schedule( id );
    return s == 0 ? Duration::zeroDuration : ( s->lateFinish - s->earlyFinish );
}

bool Task::isCritical( long id ) const
{
    Schedule *s = schedule( id );
    return s == 0 ? false : s->isCritical();
}

bool Task::calcCriticalPath(bool fromEnd)
{
    if (m_currentSchedule == 0)
        return false;
    //kDebug()<<m_name<<" fromEnd="<<fromEnd<<" cp="<<m_currentSchedule->inCriticalPath;
    if (m_currentSchedule->inCriticalPath) {
        return true; // path already calculated
    }
    if (!isCritical()) {
        return false;
    }
    if (fromEnd) {
        if (isEndNode()) {
            m_currentSchedule->inCriticalPath = true;
            //kDebug()<<m_name<<" end node";
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
            //kDebug()<<m_name<<" start node";
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
    //kDebug()<<m_name<<" return cp="<<m_currentSchedule->inCriticalPath;
    return m_currentSchedule->inCriticalPath;
}

void Task::calcFreeFloat()
{
    //kDebug()<<m_name<<endl;
    if ( type() == Node::Type_Summarytask ) {
        Node::calcFreeFloat();
        return;
    }
    Schedule *cs = m_currentSchedule;
    if ( cs == 0 ) {
        return;
    }
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
        //kDebug()<<m_name<<": "<<cs->freeFloat.toString()<<endl;
    }
}

void Task::setCurrentSchedule(long id)
{
    setCurrentSchedulePtr(findSchedule(id));
    Node::setCurrentSchedule(id);
}

bool Task::effortMetError( long id ) const
{
    Schedule *s = schedule( id );
    if (s == 0 || s->notScheduled || m_estimate->type() != Estimate::Type_Effort) {
        return false;
    }
    return s->plannedEffort() < estimate()->value(static_cast<Estimate::Use>(s->type()),  s->usePert());
}

uint Task::state( long id ) const
{
    int st = Node::State_None;
    if ( completion().isFinished() ) {
        st |= Node::State_Finished;
        if ( completion().finishTime() > endTime( id ) ) {
            st |= State_FinishedLate;
        }
        if ( completion().finishTime() < endTime( id ) ) {
            st |= State_FinishedEarly;
        }
    } else if ( completion().isStarted() ) {
        if ( completion().percentFinished() == 0 ) {
            st |= Node::State_Started;
            if ( completion().startTime() > startTime( id ) ) {
                st |= State_StartedLate;
            }
            if ( completion().startTime() < startTime( id ) ) {
                st |= State_StartedEarly;
            }
        } else {
            st |= State_Running;
        }
    }
    st |= State_ReadyToStart;
    //TODO: check proxy relations
    foreach ( const Relation *r, m_dependParentNodes ) {
        if ( ! static_cast<Task*>( r->parent() )->completion().isFinished() ) {
            st &= ~Node::State_ReadyToStart;
            st |= Node::State_NotReadyToStart;
            break;
        }
    }
    return st;
}

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


//------------------------------------------

Completion::Completion( Node *node )
    : m_node( node ),
      m_started( false ),
      m_finished( false ),
      m_entrymode( EnterCompleted )
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
    m_entrymode = p.entrymode();
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

void Completion::setPercentFinished( const QDate &date, int value )
{
    Entry *e = 0;
    if ( m_entries.contains( date ) ) {
        e = m_entries[ date ];
    } else {
        e = new Entry();
        m_entries[ date ] = e;
    }
    e->percentFinished = value;
    changed();
}

void Completion::setRemainingEffort( const QDate &date, const Duration &value )
{
    Entry *e = 0;
    if ( m_entries.contains( date ) ) {
        e = m_entries[ date ];
    } else {
        e = new Entry();
        m_entries[ date ] = e;
    }
    e->remainingEffort = value;
    changed();
}

void Completion::setActualEffort( const QDate &date, const Duration &value )
{
    Entry *e = 0;
    if ( m_entries.contains( date ) ) {
        e = m_entries[ date ];
    } else {
        e = new Entry();
        m_entries[ date ] = e;
    }
    e->totalPerformed = value;
    changed();
}

void Completion::addEntry( const QDate &date, Entry *entry )
{
     m_entries.insert( date, entry );
     //kDebug()<<m_entries.count()<<" added:"<<date;
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

int Completion::percentFinished( const QDate &date ) const
{
    int x = 0;
    foreach ( QDate d, m_entries.keys() ) {
        if ( d <= date ) {
            x = m_entries[ d ]->percentFinished;
        }
        if ( d >= date ) {
            break;
        }
    }
    return x;
}

Duration Completion::remainingEffort() const
{
    return m_entries.isEmpty() ? Duration::zeroDuration : m_entries.values().last()->remainingEffort;
}

Duration Completion::remainingEffort( const QDate &date ) const
{
    Duration x;
    foreach ( QDate d, m_entries.keys() ) {
        if ( d <= date ) {
            x = m_entries[ d ]->remainingEffort;
        }
        if ( d >= date ) {
            break;
        }
    }
    return x;
}


Duration Completion::actualEffort() const
{
    Duration eff;
    if ( m_entrymode == EnterEffortPerResource ) {
        foreach( UsedEffort *ue, m_usedEffort.values() ) {
            foreach ( QDate d, ue->actualEffortMap().keys() ) {
                eff += ue->actualEffortMap()[ d ]->effort();
            }
        }
    } else if ( ! m_entries.isEmpty() ) {
        eff = m_entries.values().last()->totalPerformed;
    }
    return eff;
}

Duration Completion::actualEffort( const QDate &date ) const
{
    Duration eff;
    if ( m_entrymode == EnterEffortPerResource ) {
        foreach( UsedEffort *ue, m_usedEffort.values() ) {
            if ( ue && ue->actualEffortMap().contains( date ) ) {
                eff += ue->actualEffortMap().value( date )->effort();
            }
        }
    } else {
        //FIXME: How to know on a specific date?
    }
    return eff;
}

Duration Completion::actualEffortTo( const QDate &date ) const
{
    //kDebug()<<date<<endl;
    Duration eff;
    if ( m_entrymode == EnterEffortPerResource ) {
        foreach( UsedEffort *ue, m_usedEffort.values() ) {
            eff += ue->effortTo( date );
        }
    } else {
        QListIterator<QDate> it( m_entries.uniqueKeys() );
        it.toBack();
        while ( it.hasPrevious() ) {
            QDate d = it.previous();
            if ( d <= date ) {
                eff = m_entries[ d ]->totalPerformed;
                break;
            }
        }
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

double Completion::actualCost( const QDate &date ) const
{
    //kDebug()<<date;
    double c = 0.0;
    foreach ( const Resource *r, m_usedEffort.keys() ) {
        double nc = r->normalRate();
        double oc = r->overtimeRate();
        if ( m_usedEffort[ r ]->actualEffortMap().contains( date ) ) {
            UsedEffort::ActualEffort *a = m_usedEffort[ r ]->effort( date );
            c += a->normalEffort().toDouble( Duration::Unit_h ) * nc;
            c += a->overtimeEffort().toDouble( Duration::Unit_h ) * oc;
        }
    }
    return c;
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

double Completion::actualCostTo( const QDate &date ) const
{
    //kDebug()<<date;
    double c = 0.0;
    foreach ( const Resource *r, m_usedEffort.keys() ) {
        double nc = r->normalRate();
        double oc = r->overtimeRate();
        foreach ( QDate d, m_usedEffort[ r ]->actualEffortMap().keys() ) {
            if ( d > date ) {
                break;
            }
            UsedEffort::ActualEffort *a = m_usedEffort[ r ]->actualEffortMap()[ d ];
            if ( a ) {
                c += a->normalEffort().toDouble( Duration::Unit_h ) * nc;
                c += a->overtimeEffort().toDouble( Duration::Unit_h ) * oc;
            }
        }
    }
    return c;
}

bool Completion::loadXML( KoXmlElement &element, XMLLoaderObject &status )
{
    //kDebug();
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
        KoXmlElement e;
        forEachElement(e, element) {
                if (e.tagName() == "completion-entry") {
                    QDate date;
                    s = e.attribute("date");
                    if ( !s.isEmpty() ) {
                        date = QDate::fromString( s, Qt::ISODate );
                    }
                    if ( !date.isValid() ) {
                        kWarning()<<"Invalid date: "<<date<<s<<endl;
                        continue;
                    }
                    Entry *entry = new Entry( e.attribute("percent-finished", "0").toInt(), Duration::fromString(e.attribute("remaining-effort")),  Duration::fromString(e.attribute("performed-effort")) );
                    addEntry( date, entry );
                } else if (e.tagName() == "used-effort") {
                    KoXmlElement el;
                    forEachElement(el, e) {
                            if (el.tagName() == "resource") {
                                QString id = el.attribute( "id" );
                                Resource *r = status.project().resource( id );
                                if ( r == 0 ) {
                                    kWarning()<<"Cannot find resource, id="<<id<<endl;
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

Duration Completion::UsedEffort::effortTo( const QDate &date ) const
{
    Duration eff;
    foreach ( QDate d, m_actual.keys() ) {
        if ( d > date ) {
            break;
        }
        eff += m_actual[ d ]->effort();
    }
    return eff;
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

bool Completion::UsedEffort::loadXML(KoXmlElement &element, XMLLoaderObject & )
{
    //kDebug();
    KoXmlElement e;
    forEachElement(e, element) {
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
WorkPackage::WorkPackage( Task &task )
    : m_task( task ),
    m_manager( 0 )
{
    m_completion.setNode( &task );
}

WorkPackage::~WorkPackage()
{
}

bool WorkPackage::loadXML(KoXmlElement &element, XMLLoaderObject &status )
{
    return true;
}

void WorkPackage::saveXML(QDomElement &element) const
{
}

QList<Resource*> WorkPackage::fetchResources()
{
    kDebug()<<m_task.name();
    QList<Resource*> lst;
    if ( id() == -1 ) {
        kDebug()<<"No schedule";
        lst << m_task.requestedResources();
    } else {
        lst = m_task.assignedResources( id() );
        foreach ( const Resource *r, m_completion.resources() ) {
            if ( ! lst.contains( const_cast<Resource*>( r ) ) ) {
                lst << const_cast<Resource*>( r );
            }
        }
    }
    kDebug()<<lst;
    return lst;
}

Completion &WorkPackage::completion()
{
    return m_completion;
}

const Completion &WorkPackage::completion() const
{
    return m_completion;
}

QMap<Resource*, WorkPackage::ResourceStatus> &WorkPackage::resourceStatus()
{
    foreach ( Resource *r, fetchResources() ) {
        if ( ! m_resourceStatus.contains( r ) ) {
            m_resourceStatus.insert( r, ResourceStatus() );
        }
    }
    return m_resourceStatus;
}

const QMap<Resource*, WorkPackage::ResourceStatus> &WorkPackage::resourceStatus() const
{
    return m_resourceStatus;
}

void WorkPackage::setScheduleManager( ScheduleManager *sm )
{
    m_manager = sm;
    resourceStatus();
}

WorkPackage::WPControlStatus WorkPackage::controlStatus() const
{
    foreach ( Resource *r, m_resourceStatus.keys() ) {
        if ( const_cast<WorkPackage*>( this )->p_controlStatus( r ) == CS_Work ) {
            return CS_Work;
        }
    }
    return CS_KPlato;
}

WorkPackage::WPControlStatus WorkPackage::controlStatus( const Resource *r ) const
{
    return const_cast<WorkPackage*>( this )->p_controlStatus( const_cast<Resource*>( r ) );
}

WorkPackage::WPControlStatus WorkPackage::controlStatus( Resource *r )
{
    return p_controlStatus( r );
}

WorkPackage::WPControlStatus WorkPackage::p_controlStatus( Resource *r )
{
    if ( ! m_resourceStatus.contains( r ) ) {
        return CS_None;
    }
    return m_resourceStatus[ r ].controlStatus;
}

QString WorkPackage::controlStatusToString( WorkPackage::WPControlStatus sts, bool trans )
{
    QString s = trans ? i18n( "None" ) : "None";
    switch ( sts ) {
        case CS_KPlato: 
            s = trans ? i18n( "KPlato" ) : "KPlato";
            break;
        case CS_Work:
            s = trans ? i18n( "Work" ) : "Work";
            break;
        default:
            break;
    }
    return s;
}

WorkPackage::WPSendStatus WorkPackage::sendStatus( const Resource *r ) const
{
    return const_cast<WorkPackage*>( this )->p_sendStatus( const_cast<Resource*>( r ) );
}

WorkPackage::WPSendStatus WorkPackage::sendStatus( Resource *r )
{
    return p_sendStatus( r );
}

WorkPackage::WPSendStatus WorkPackage::p_sendStatus( Resource *r )
{
    if ( ! m_resourceStatus.contains( r ) ) {
        return SS_None;
    }
    return m_resourceStatus[ r ].sendStatus;
}

QString WorkPackage::sendStatusToString( WorkPackage::WPSendStatus sts, bool trans )
{
    QString s = trans ? i18n( "None" ) : "None";
    switch ( sts ) {
        case SS_ForInformation: 
            s = trans ? i18n( "ForInformation" ) : "ForInformation";
            break;
        case SS_ForEstimation:
            s = trans ? i18n( "ForEstimation" ) : "ForEstimation";
            break;
        case SS_TentativeSchedule:
            s = trans ? i18n( "TentativeSchedule" ) : "TentativeSchedule";
            break;
        case SS_Scheduled:
            s = trans ? i18n( "Scheduled" ) : "Scheduled";
            break;
        case SS_Execute:
            s = trans ? i18n( "Execute" ) : "Execute";
            break;
        case SS_Cancel:
            s = trans ? i18n( "Cancel" ) : "Cancel";
            break;
        default:
            break;
    }
    return s;
}

WorkPackage::WPResponseStatus WorkPackage::responseStatus( const Resource *r ) const
{
    return const_cast<WorkPackage*>( this )->p_responseStatus( const_cast<Resource*>( r ) );
}

WorkPackage::WPResponseStatus WorkPackage::responseStatus( Resource *r )
{
    return p_responseStatus( r );
}

WorkPackage::WPResponseStatus WorkPackage::p_responseStatus( Resource *r )
{
    if ( ! m_resourceStatus.contains( r ) ) {
        return RS_None;
    }
    return m_resourceStatus[ r ].responseStatus;
}

QString WorkPackage::responseStatusToString( WorkPackage::WPResponseStatus sts, bool trans )
{
    QString s = trans ? i18n( "None" ) : "None";
    switch ( sts ) {
        case RS_Accepted: 
            s = trans ? i18n( "Accepted" ) : "Accepted";
            break;
        case RS_Tentative:
            s = trans ? i18n( "Tentative" ) : "Tentative";
            break;
        case RS_Refused:
            s = trans ? i18n( "Refused" ) : "Refused";
            break;
        case RS_Update:
            s = trans ? i18n( "Update" ) : "Update";
            break;
        default:
            break;
    }
    return s;
}

WorkPackage::WPResponseType WorkPackage::responseType(  const Resource *r ) const
{
    return const_cast<WorkPackage*>( this )->p_responseType( const_cast<Resource*>( r ) );
}

WorkPackage::WPResponseType WorkPackage::responseType(  Resource *r )
{
    return p_responseType( r );
}

WorkPackage::WPResponseType WorkPackage::p_responseType(  Resource *r )
{
    if ( ! m_resourceStatus.contains( r ) ) {
        return RT_None;
    }
    return m_resourceStatus[ r ].responseType;
}

QString WorkPackage::responseTypeToString( WorkPackage::WPResponseType rsp, bool trans )
{
    QString s = trans ? i18n( "None" ) : "None";
    switch ( rsp ) {
        case RT_Required: 
            s = trans ? i18n( "Required" ) : "Required";
            break;
        default:
            break;
    }
    return s;
}
    
WorkPackage::WPActionType WorkPackage::actionType(  const Resource *r ) const
{
    return const_cast<WorkPackage*>( this )->p_actionType( const_cast<Resource*>( r ) );
}

WorkPackage::WPActionType WorkPackage::actionType(  Resource *r )
{
    return p_actionType( r );
}

WorkPackage::WPActionType WorkPackage::p_actionType(  Resource *r )
{
    if ( ! m_resourceStatus.contains( r ) ) {
        return AT_None;
    }
    return m_resourceStatus[ r ].actionType;
}

QString WorkPackage::actionTypeToString( WorkPackage::WPActionType type, bool trans )
{
    QString s = trans ? i18n( "None" ) : "None";
    switch ( type ) {
        case AT_Send: 
            s = trans ? i18n( "Send" ) : "Send";
            break;
        case AT_Receive: 
            s = trans ? i18n( "Receive" ) : "Receive";
            break;
        case AT_Load: 
            s = trans ? i18n( "Load" ) : "Load";
            break;
        default:
            break;
    }
    return s;
}
    
//----------------------------------
#ifndef NDEBUG
void Task::printDebug(bool children, const QByteArray& _indent) {
    QByteArray indent = _indent;
    kDebug()<<indent<<"+ Task node:"<<name()<<" type="<<type();
    indent += "!  ";
    kDebug()<<indent<<"Requested resources (total):"<<units()<<"%";
    kDebug()<<indent<<"Requested resources (work):"<<workUnits()<<"%";
    if (m_requests)
        m_requests->printDebug(indent);
    
    completion().printDebug( indent );
    
    Node::printDebug(children, indent);

}

void Completion::printDebug(const QByteArray& _indent) const {
    QByteArray indent = _indent;
    kDebug()<<indent<<"+ Completion: ("<<m_entries.count()<<" entries)";
    indent += "!  ";
    kDebug()<<indent<<"Started:"<<m_started<<""<<m_startTime.toString();
    kDebug()<<indent<<"Finished:"<<m_finished<<""<<m_finishTime.toString();
    indent += "  ";
    foreach( QDate d, m_entries.keys() ) {
        Entry *e = m_entries[ d ];
        kDebug()<<indent<<"Date:"<<d;
        kDebug()<<(indent+" !")<<"% Finished:"<<e->percentFinished;
        kDebug()<<(indent+" !")<<"Remainig:"<<e->remainingEffort.toString();
        kDebug()<<(indent+" !")<<"Performed:"<<e->totalPerformed.toString();
    }
}

#endif


}  //KPlato namespace

#include "kpttask.moc"
