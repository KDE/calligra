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
      m_workPackage( this )
{
    //kDebug()<<"("<<this<<')';
    m_requests.setTask( this );
    Duration d(1, 0, 0);
    m_estimate = new Estimate();
    m_estimate->setOptimisticRatio(-10);
    m_estimate->setPessimisticRatio(20);
    m_estimate->setParentNode( this );
    
    if (m_parent)
        m_leader = m_parent->leader();
}

Task::Task(const Task &task, Node *parent) 
    : Node(task, parent), 
      m_resource(),
      m_workPackage( this )
{
    //kDebug()<<"("<<this<<')';
    m_requests.setTask( this );
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

ResourceGroupRequest *Task::resourceGroupRequest(const ResourceGroup *group) const {
    return m_requests.find(group);
}

void Task::clearResourceRequests() {
    m_requests.clear();
    changed( this );
}

void Task::addRequest(ResourceGroup *group, int numResources) {
    addRequest(new ResourceGroupRequest(group, numResources));
}

void Task::addRequest(ResourceGroupRequest *request) {
    //kDebug()<<m_name<<request<<request->group()<<request->group()->id()<<request->group()->name();
    m_requests.addRequest(request);
}

void Task::takeRequest(ResourceGroupRequest *request) {
    //kDebug()<<request;
    m_requests.takeRequest(request);
}

QStringList Task::requestNameList() const {
    return m_requests.requestNameList();
}

QList<Resource*> Task::requestedResources() const {
    return m_requests.requestedResources();
}

bool Task::containsRequest( const QString &identity ) const {
    return m_requests.contains( identity );
}

ResourceRequest *Task::resourceRequest( const QString &name ) const {
    return m_requests.resourceRequest( name );
}

QStringList Task::assignedNameList( long id) const {
    Schedule *s = schedule( id );
    if ( s == 0 ) {
        return QStringList();
    }
    return s->resourceNameList();
}

// int Task::units() const {
//     return m_requests.units();
// }

int Task::workUnits() const {
    return m_requests.workUnits();
}

void Task::makeAppointments() {
    if (m_currentSchedule == 0)
        return;
    if (type() == Node::Type_Task) {
        //kDebug()<<m_name<<":"<<m_currentSchedule->startTime<<","<<m_currentSchedule->endTime<<";"<<m_currentSchedule->duration.toString();
        m_requests.makeAppointments(m_currentSchedule);
        //kDebug()<<m_name<<":"<<m_currentSchedule->startTime<<","<<m_currentSchedule->endTime<<";"<<m_currentSchedule->duration.toString();
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
    //kDebug();
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
    //kDebug()<<m_name<<st.toString()<<et.toString()<<m_currentSchedule->calculationMode();
    foreach ( Appointment *a, ns->appointments() ) {
        Resource *r = a->resource() == 0 ? 0 : a->resource()->resource();
        if ( r == 0 ) {
            kError()<<"No resource";
            continue;
        }
        AppointmentIntervalList lst = a->intervals(  st, et );
        if ( lst.isEmpty() ) {
            //kDebug()<<"No intervals to copy from"<<a;
            continue;
        }
        Appointment *curr = 0;
        foreach ( Appointment *c, m_currentSchedule->appointments() ) {
            if ( c->resource()->resource() == r ) {
                //kDebug()<<"Found current appointment to"<<a->resource()->resource()->name()<<c;
                curr = c;
                break;
            }
        }
        if ( curr == 0 ) {
            curr = new Appointment();
            m_currentSchedule->add( curr );
            curr->setNode( m_currentSchedule );
            //kDebug()<<"Created new appointment"<<curr;
        }
        ResourceSchedule *rs = static_cast<ResourceSchedule*>( r->findSchedule( m_currentSchedule->id() ) );
        if ( rs == 0 ) {
            rs = r->createSchedule( m_currentSchedule->parent() );
            rs->setId( m_currentSchedule->id() );
            rs->setName( m_currentSchedule->name() );
            rs->setType( m_currentSchedule->type() );
            //kDebug()<<"Resource schedule not found, id="<<m_currentSchedule->id();
        }
        rs->setCalculationMode( m_currentSchedule->calculationMode() );
        if ( ! rs->appointments().contains( curr ) ) {
            //kDebug()<<"add to resource"<<rs<<curr;
            rs->add( curr );
            curr->setResource( rs );
        }
        Appointment app;
        app.setIntervals( lst );
        //foreach ( AppointmentInterval *i, curr->intervals() ) { kDebug()<<i->startTime().toString()<<i->endTime().toString(); }
        curr->merge( app );
        //kDebug()<<"Appointments added";
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
            ResourceGroupRequest *r = m_requests.findGroupRequestById( e.attribute("group-id") );
            if ( r ) {
                kWarning()<<"Multiple requests to same group, loading into existing group";
                if ( ! r->load( e, status.project() ) ) {
                    kError()<<"Failed to load resource request";
                }
            } else {
                r = new ResourceGroupRequest();
                if (r->load(e, status.project())) {
                    addRequest(r);
                } else {
                    kError()<<"Failed to load resource request";
                    delete r;
                }
            }
        } else if (e.tagName() == "workpackage") {
            m_workPackage.loadXML( e, status );
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
                        kError()<<"Failed to load schedule";
                        delete sch;
                    }
                }
            }
        } else if (e.tagName() == "documents") {
            m_documents.load( e, status );
        } else if (e.tagName() == "workpackage-log") {
            KoXmlNode n = e.firstChild();
            for ( ; ! n.isNull(); n = n.nextSibling() ) {
                if ( ! n.isElement() ) {
                    continue;
                }
                KoXmlElement el = n.toElement();
                if (el.tagName() == "workpackage") {
                    WorkPackage *wp = new WorkPackage( this );
                    if ( wp->loadLoggedXML( el, status ) ) {
                        m_packageLog << wp;
                    } else {
                        kError()<<"Failed to load logged workpackage";
                        delete wp;
                    }
                }
            }
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
    m_workPackage.saveXML(me);
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
    if ( ! m_requests.isEmpty() ) {
        m_requests.save(me);
    }
    
    m_documents.save( me );
    
    // The workpackage log
    if (!m_packageLog.isEmpty()) {
        QDomElement log = me.ownerDocument().createElement("workpackage-log");
        me.appendChild(log);
        foreach (WorkPackage *wp, m_packageLog) {
            wp->saveLoggedXML( log );
        }
    }

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
    if ( type() == Node::Type_Summarytask ) {
        EffortCostMap ec;
        QListIterator<Node*> it( childNodeIterator() );
        while ( it.hasNext() ) {
            ec += it.next() ->plannedEffortCostPrDay( start, end, id );
        }
        return ec;
    }
    Schedule *s = schedule( id );
    if ( s ) {
        return s->plannedEffortCostPrDay(start, end);
    }
    return EffortCostMap();
}

EffortCostMap Task::actualEffortCostPrDay(const QDate &start, const QDate &end, long id ) const {
    //kDebug()<<m_name;
    if ( type() == Node::Type_Summarytask ) {
        EffortCostMap ec;
        QListIterator<Node*> it( childNodeIterator() );
        while ( it.hasNext() ) {
            ec += it.next() ->actualEffortCostPrDay( start, end, id );
        }
        return ec;
    }
    switch ( completion().entrymode() ) {
        case Completion::FollowPlan:
            return plannedEffortCostPrDay( start, end, id );
        default:
            return completion().effortCostPrDay( start, end, id );
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

EffortCost Task::plannedCost( long id ) const {
    //kDebug();
    if (type() == Node::Type_Summarytask) {
        return Node::plannedCost( id );
    }
    EffortCost c;
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

EffortCost Task::actualCostTo( const QDate &date ) const {
    //kDebug();
    EffortCost c;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            c += n->actualCostTo( date );
        }
        return c;
    }
    return completion().actualCostTo( date );
}

double Task::bcws( const QDate &date, long id ) const
{
    //kDebug();
    double c = plannedCostTo( date, id );
    //kDebug()<<c;
    return c;
}

EffortCostMap Task::bcwsPrDay( long id ) const
{
    //kDebug();
    if (type() == Node::Type_Summarytask) {
        return Node::bcwsPrDay( id );
    }
    Schedule *s = schedule( id );
    if ( s == 0 ) {
        return EffortCostMap();
    }
    return s->bcwsPrDay();
}

EffortCostMap Task::bcwpPrDay( long id ) const
{
    //kDebug();
    if (type() == Node::Type_Summarytask) {
        return Node::bcwpPrDay( id );
    }
    Schedule *s = schedule( id );
    if ( s == 0 ) {
        return EffortCostMap();
    }
    EffortCostMap e = s->bcwsPrDay();
    double totEff = e.totalEffort().toDouble( Duration::Unit_h );
    double totCost = e.totalCost();
    EffortCostDayMap::const_iterator it;
    for(it = e.days().constBegin(); it != e.days().constEnd(); ++it) {
        double p = (double)(completion().percentFinished( it.key() )) / 100.0;
        const_cast<EffortCost&>(it.value()).setBcwpEffort( totEff  * p );
        const_cast<EffortCost&>(it.value()).setBcwpCost( totCost  * p );
    }
    return e;
}

Duration Task::budgetedWorkPerformed( const QDate &date, long id ) const
{
    //kDebug();
    Duration e;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            e += n->budgetedWorkPerformed( date, id );
        }
        return e;
    }
    
    e = plannedEffort( id ) * (double)completion().percentFinished( date ) / 100.0;
    //kDebug()<<m_name<<"("<<id<<")"<<date<<"="<<e.toString();
    return e;
}

double Task::budgetedCostPerformed( const QDate &date, long id ) const
{
    //kDebug();
    double c = 0.0;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            c += n->budgetedCostPerformed( date, id );
        }
        return c;
    }
    
    c = plannedCost( id ).cost() * (double)completion().percentFinished( date ) / 100.0;
    //kDebug()<<m_name<<"("<<id<<")"<<date<<"="<<e.toString();
    return c;
}

double Task::bcwp( long id ) const
{
    return bcwp( QDate::currentDate(), id );
}

double Task::bcwp( const QDate &date, long id ) const
{
    //kDebug();
    double c = 0;
    if (type() == Node::Type_Summarytask) {
        foreach (Node *n, childNodeIterator()) {
            c += n->bcwp( date, id ); //FIXME
        }
        return c;
    }
    c = plannedCost( id ).cost() * (double)completion().percentFinished( date ) / 100.0;
    //kDebug()<<m_name<<"("<<id<<")"<<date<<"="<<c;
    return c;
}

EffortCostMap Task::acwp( long id ) const
{
    if (type() == Node::Type_Summarytask) {
        return Node::acwp( id );
    }
    //kDebug()<<m_name<<completion().entrymode();
    switch ( completion().entrymode() ) {
        case Completion::FollowPlan:
            //TODO
            break;
        case Completion::EnterCompleted:
            //hmmm
        default:
            return completion().actualEffortCost( id );
    }
    return EffortCostMap();
}

EffortCost Task::acwp( const QDate &date, long id ) const
{
    //kDebug();
    if (type() == Node::Type_Summarytask) {
        return Node::acwp( date, id );
    }
    EffortCost c;
    c = completion().actualCostTo( date );
    return c;
}

double Task::schedulePerformanceIndex( const QDate &date, long id ) const {
    //kDebug();
    double r = 1.0;
    double s = bcws( date, id );
    double p = bcwp( date, id );
    if ( s > 0.0 ) {
        r = p / s;
    }
    return r;
}

double Task::effortPerformanceIndex( const QDate &date, long id ) const {
    //kDebug();
    double r = 1.0;
    Duration a, b;
    if ( m_estimate->type() == Estimate::Type_Effort ) {
        Duration b = budgetedWorkPerformed( date, id );
        if ( b == Duration::zeroDuration ) {
            return r;
        }
        Duration a = actualEffortTo( date );
        if ( b == Duration::zeroDuration ) {
            return 1.0;
        }
        r = b.toDouble() / a.toDouble();
    } else if ( m_estimate->type() == Estimate::Type_Duration ) {
        //TODO
    }
    return r;
}


//FIXME Handle summarytasks
double Task::costPerformanceIndex(const QDate &date, bool *error) const {
    double res = 0.0;
    double ac = actualCostTo(date).cost();
    
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
        if ( ( m_constraint == Node::MustStartOn && m_estimate->type() == Estimate::Type_Duration ) ||
            ( m_constraint == Node::MustFinishOn && m_estimate->type() == Estimate::Type_Duration ) ||
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
    // do them forward
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
    //cs->logDebug( "calculateForward: earlyStart=" + cs->earlyStart.toString() );
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
    //cs->logDebug( "calculateForward: earlyStart=" + cs->earlyStart.toString() );
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
        return m_earlyFinish;
    }
    KLocale *locale = KGlobal::locale();
    cs->logInfo( i18n( "Calculate early finish " ) );
    //kDebug()<<"------>"<<m_name<<""<<cs->earlyStart;
    if (type() == Node::Type_Task) {
        m_durationForward = m_estimate->value(use, pert);
        switch (constraint()) {
            case Node::ASAP:
            case Node::ALAP:
            {
                //kDebug()<<m_name<<" ASAP/ALAP:"<<cs->earlyStart;
                cs->earlyStart = workTimeAfter( cs->earlyStart );
                m_durationForward = duration(cs->earlyStart, use, false);
                m_earlyFinish = cs->earlyStart + m_durationForward;
                cs->logDebug("ASAP/ALAP: " + cs->earlyStart.toString() + "+" + m_durationForward.toString() + "=" + m_earlyFinish.toString() );
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = cs->earlyStart;
                    cs->endTime = m_earlyFinish;
                    makeAppointments();

                    // calculate duration wo checking booking = the earliest finish possible
                    cs->setAllowOverbooking( true );
                    m_durationForward = duration(cs->earlyStart, use, false);
                    cs->setAllowOverbooking( false );
                    cs->logDebug("ASAP/ALAP earliest possible: " + cs->earlyStart.toString() + "+" + m_durationForward.toString() + "=" + (cs->earlyStart+m_durationForward).toString() );
                }
                break;
            }
            case Node::MustFinishOn:
            case Node::FinishNotLater:
            {
                m_durationForward = duration(cs->earlyStart, use, false);
                cs->earlyFinish = cs->earlyStart + m_durationForward;
                //kDebug()<<"MustFinishOn:"<<m_constraintEndTime<<cs->earlyStart<<cs->earlyFinish;
                if (cs->earlyFinish > m_constraintEndTime) {
                    cs->schedulingError = true;
                    cs->logWarning( i18nc( "1=type of constraint", "%1: Failed to meet constraint", constraintToString( true ) ) );
                }
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = cs->earlyStart;
                    cs->endTime = cs->earlyFinish;
                    makeAppointments();
                }
                m_earlyFinish = cs->earlyStart + m_durationForward;
                break;
            }
            case Node::MustStartOn:
            case Node::StartNotEarlier:
            {
                //kDebug()<<"MustStartOn:"<<m_constraintStartTime<<cs->earlyStart;
                cs->earlyStart = workTimeAfter( cs->earlyStart );
                if ( cs->earlyStart > m_constraintStartTime ) {
                    cs->schedulingError = true;
                    cs->logWarning( i18nc( "1=type of constraint", "%1: Failed to meet constraint", constraintToString( true ) ) );
                }
                cs->startTime = qMax( cs->earlyStart, m_constraintStartTime );
                m_durationForward = duration( cs->startTime, use, false );
                m_earlyFinish = cs->startTime + m_durationForward;
                if ( !cs->allowOverbooking() ) {
                    cs->endTime = cs->startTime + m_durationForward;
                    makeAppointments();
                    
                    // calculate duration wo checking booking = the earliest finish possible
                    cs->setAllowOverbooking( true );
                    m_durationForward = duration(cs->earlyStart, use, false);
                    cs->setAllowOverbooking( false );
                    cs->logDebug("MSO/SNE earliest possible: " + cs->earlyStart.toString() + "+" + m_durationForward.toString() + "=" + (cs->earlyStart+m_durationForward).toString() );
                }
                break;
            }
            case Node::FixedInterval: {
                if ( cs->earlyStart > m_constraintStartTime ) {
                    cs->schedulingError = true;
                    cs->logWarning( i18nc( "1=type of constraint", "%1: Failed to meet constraint", constraintToString( true ) ) );
                }
                //cs->earlyStart = m_constraintStartTime;
                m_durationForward = m_constraintEndTime - m_constraintStartTime;
                if ( cs->earlyStart < m_constraintStartTime ) {
                    m_durationForward = m_constraintEndTime - cs->earlyStart;
                }
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = m_constraintStartTime;
                    cs->endTime = m_constraintEndTime;
                    makeAppointments();
                }
                m_earlyFinish = cs->earlyStart + m_durationForward;
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
                    cs->logWarning( i18nc( "1=type of constraint", "%1: Failed to meet constraint", constraintToString( true ) ) );
                }
                m_earlyFinish = cs->earlyStart + m_durationForward;
                break;
            case Node::FinishNotLater:
                //kDebug()<<"FinishNotLater:"<<m_constraintEndTime<<cs->earlyStart;
                if ( cs->earlyStart < m_constraintEndTime ) {
                    m_durationForward = m_constraintEndTime - cs->earlyStart;
                    cs->logWarning( i18nc( "1=type of constraint", "%1: Failed to meet constraint", constraintToString( true ) ) );
                }
                m_earlyFinish = cs->earlyStart + m_durationForward;
                break;
            case Node::MustStartOn:
                //kDebug()<<"MustStartOn:"<<m_constraintStartTime<<cs->earlyStart;
                if ( cs->earlyStart < m_constraintStartTime ) {
                    m_durationForward = m_constraintStartTime - cs->earlyStart;
                    cs->logWarning( i18nc( "1=type of constraint", "%1: Failed to meet constraint", constraintToString( true ) ) );
                }
                m_earlyFinish = cs->earlyStart + m_durationForward;
                break;
            case Node::StartNotEarlier:
                //kDebug()<<"StartNotEarlier:"<<m_constraintStartTime<<cs->earlyStart;
                if ( cs->earlyStart < m_constraintStartTime ) {
                    m_durationForward = m_constraintStartTime - cs->earlyStart;
                    cs->logWarning( i18nc( "1=type of constraint", "%1: Failed to meet constraint", constraintToString( true ) ) );
                }
                m_earlyFinish = cs->earlyStart + m_durationForward;
                break;
            case Node::FixedInterval:
                m_earlyFinish = cs->earlyStart + m_durationForward;
                break;
            default:
                m_earlyFinish = cs->earlyStart + m_durationForward;
                break;
        }
        //kDebug()<<m_name<<""<<earliestStart.toString();
    } else if (type() == Node::Type_Summarytask) {
        kWarning()<<"Summarytasks should not be calculated here: "<<m_name;
    } else { // ???
        m_durationForward = Duration::zeroDuration;
    }
    m_visitedForward = true;
    cs->insertForwardNode( this );
    cs->earlyFinish = cs->earlyStart + m_durationForward;
    foreach ( Appointment *a, cs->appointments( Schedule::CalculateForward ) ) {
        cs->logInfo( i18n( "Resource %1 booked from %2 to %3", a->resource()->resource()->name(), locale->formatDateTime( a->startTime() ), locale->formatDateTime( a->endTime() ) ) );
    }
    cs->logInfo( i18n( "Early finish calculated: %1", locale->formatDateTime( cs->earlyFinish ) ) );
    cs->incProgress();
    return m_earlyFinish;
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
    //cs->lateFinish = projectNode()->constraintEndTime();
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
        return m_lateStart;
    }
    KLocale *locale = KGlobal::locale();
    cs->logInfo( i18n( "Calculate late start" ) );
    //kDebug()<<m_name<<" id="<<cs->id()<<" mode="<<cs->calculationMode()<<": latestFinish="<<cs->lateFinish;
    if (type() == Node::Type_Task) {
        m_durationBackward = m_estimate->value(use, pert);
        switch (constraint()) {
            case Node::ASAP:
            case Node::ALAP:
                //kDebug()<<m_name<<" ASAP/ALAP:"<<cs->lateFinish;
                cs->lateFinish = workTimeBefore( cs->lateFinish );
                m_durationBackward = duration(cs->lateFinish, use, true);
                m_lateStart = cs->lateFinish - m_durationBackward;
                cs->logDebug("ASAP/ALAP: " + cs->lateFinish.toString() + "-" + m_durationBackward.toString() + "=" + m_lateStart.toString() );
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = m_lateStart;
                    cs->endTime = cs->lateFinish;
                    makeAppointments();

                    // calculate wo checking bookings = latest start possible
                    cs->setAllowOverbooking( true );
                    m_durationBackward = duration(cs->lateFinish, use, true);
                    cs->setAllowOverbooking( false );
                    cs->logDebug("ASAP/ALAP latest start possible: " + cs->lateFinish.toString() + "-" + m_durationBackward.toString() + "=" + (cs->lateFinish-m_durationBackward).toString() );
                }
                break;
            case Node::MustStartOn:
            case Node::StartNotEarlier:
            {
                //kDebug()<<"MustStartOn:"<<m_constraintStartTime<<cs->lateFinish;
                cs->lateFinish = workTimeBefore( cs->lateFinish );
                m_durationBackward = duration(cs->lateFinish, use, true);
                cs->lateStart = cs->lateFinish - m_durationBackward;
                if ( cs->lateStart < m_constraintStartTime) {
                    cs->schedulingError = true;
                    cs->logWarning( i18nc( "1=type of constraint", "%1: Failed to meet constraint", constraintToString( true ) ) );
                } else {
                    cs->lateStart = qMax( cs->earlyStart, m_constraintStartTime );
                }
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = cs->lateStart;
                    cs->endTime = cs->lateFinish;
                    makeAppointments();
                }
                m_lateStart = cs->lateFinish - m_durationBackward;
                break;
            }
            case Node::MustFinishOn:
            case Node::FinishNotLater:
                //kDebug()<<"MustFinishOn:"<<m_constraintEndTime<<cs->lateFinish;
                cs->lateFinish = workTimeBefore( cs->lateFinish );
                cs->endTime = cs->lateFinish;
                if ( cs->lateFinish < m_constraintEndTime ) {
                    cs->schedulingError = true;
                    cs->logWarning( i18nc( "1=type of constraint", "%1: Failed to meet constraint", constraintToString( true ) ) );
                } else {
                    cs->endTime = qMax( cs->earlyFinish, m_constraintEndTime );
                }
                m_durationBackward = duration(cs->endTime, use, true);
                cs->startTime = cs->endTime - m_durationBackward;
                if ( !cs->allowOverbooking() ) {
                    makeAppointments();
                }
                m_durationBackward = cs->lateFinish - cs->startTime;
                m_lateStart = cs->lateFinish - m_durationBackward;
                break;
            case Node::FixedInterval: {
                //cs->lateFinish = m_constraintEndTime;
                if ( cs->lateFinish < m_constraintEndTime ) {
                    cs->schedulingError = true;
                    cs->logWarning( i18nc( "1=type of constraint", "%1: Failed to meet constraint", constraintToString( true ) ) );
                }
                m_durationBackward = m_constraintEndTime - m_constraintStartTime;
                if ( !cs->allowOverbooking() ) {
                    cs->startTime = m_constraintStartTime;
                    cs->endTime = m_constraintEndTime;
                    makeAppointments();
                }
                m_lateStart = cs->lateFinish - m_durationBackward;
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
                m_lateStart = cs->lateFinish - m_durationBackward;
                break;
            case Node::FinishNotLater:
                //kDebug()<<"FinishNotLater:"<<m_constraintEndTime<<cs->lateFinish;
                if ( m_constraintEndTime < cs->lateFinish ) {
                    m_durationBackward = cs->lateFinish - m_constraintEndTime;
                }
                m_lateStart = cs->lateFinish - m_durationBackward;
                break;
            case Node::MustStartOn:
                //kDebug()<<"MustStartOn:"<<m_constraintStartTime<<cs->lateFinish;
                if ( m_constraintStartTime < cs->lateFinish ) {
                    m_durationBackward = cs->lateFinish - m_constraintStartTime;
                }
                m_lateStart = cs->lateFinish - m_durationBackward;
                break;
            case Node::StartNotEarlier:
                //kDebug()<<"MustStartOn:"<<m_constraintStartTime<<cs->lateFinish;
                if ( m_constraintStartTime < cs->lateFinish ) {
                    m_durationBackward = cs->lateFinish - m_constraintStartTime;
                }
                m_lateStart = cs->lateFinish - m_durationBackward;
                break;
            case Node::FixedInterval:
                m_lateStart = cs->lateFinish - m_durationBackward;
                break;
            default:
                m_lateStart = cs->lateFinish - m_durationBackward;
                break;
        }
        //kDebug()<<m_name<<""<<cs->lateFinish;
    } else if (type() == Node::Type_Summarytask) {
        kWarning()<<"Summarytasks should not be calculated here: "<<m_name;
    } else { // ???
        m_durationBackward = Duration::zeroDuration;
    }
    m_visitedBackward = true;
    cs->insertBackwardNode( this );
    cs->lateStart = cs->lateFinish - m_durationBackward;
    foreach ( Appointment *a, cs->appointments( Schedule::CalculateBackward ) ) {
        cs->logInfo( i18n( "Resource %1 booked from %2 to %3", a->resource()->resource()->name(), locale->formatDateTime( a->startTime() ), locale->formatDateTime( a->endTime() ) ) );
    }
    cs->logInfo( i18n( "Late start calculated: %1", locale->formatDateTime( cs->lateStart ) ) );
    cs->incProgress();
    return m_lateStart;
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
    if (m_visitedForward) {
        return cs->endTime;
    }
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
    KLocale *locale = KGlobal::locale();
    cs->logInfo( i18n( "Schedule from start %1", locale->formatDateTime( cs->startTime ) ) );
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
            cs->startTime = workTimeAfter( cs->startTime, cs );
            cs->logDebug( "ASAP: " + cs->startTime.toString() + " earliest: " + cs->earlyStart.toString() );
            cs->duration = duration(cs->startTime, use, false);
            cs->endTime = cs->startTime + cs->duration;
            makeAppointments();
            if ( cs->recalculate() && completion().isStarted() ) {
                // copy start times + appointments from parent schedule
                copyAppointments();
                cs->duration = cs->endTime - cs->startTime;
            }
            if ( cs->lateFinish > cs->endTime ) {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            } else {
                cs->positiveFloat = Duration::zeroDuration;
            }
            break;
        case Node::ALAP:
            // cs->startTime calculated above
            //kDebug()<<m_name<<"ALAP:"<<cs->startTime<<cs->endTime<<" latest="<<cs->lateFinish;
            cs->endTime = workTimeBefore( cs->lateFinish );
            cs->duration = duration(cs->endTime, use, true);
            cs->startTime = cs->endTime - cs->duration;
            //kDebug()<<m_name<<" endTime="<<cs->endTime<<" latest="<<cs->lateFinish;
            makeAppointments();
            if ( cs->lateFinish > cs->endTime ) {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            } else {
                cs->positiveFloat = Duration::zeroDuration;
            }
            if ( cs->recalculate() && completion().isStarted() ) {
                cs->earlyStart = cs->startTime = completion().startTime();
            }
            break;
        case Node::StartNotEarlier:
            // cs->startTime calculated above
            //kDebug()<<"StartNotEarlier:"<<m_constraintStartTime<<cs->startTime<<cs->lateStart;
            cs->startTime = workTimeAfter( cs->startTime );
            if ( cs->startTime < m_constraintStartTime ) {
                cs->startTime = workTimeAfter( qMin( m_constraintStartTime, cs->lateStart ) );
            }
            cs->duration = duration(cs->startTime, use, false);
            cs->endTime = cs->startTime + cs->duration;
            makeAppointments();
            if ( cs->recalculate() && completion().isStarted() ) {
                // copy start times + appointments from parent schedule
                copyAppointments();
                cs->duration = cs->endTime - cs->startTime;
            }
            if ( cs->lateFinish > cs->endTime ) {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            } else {
                cs->positiveFloat = Duration::zeroDuration;
            }
            if (cs->startTime < m_constraintStartTime) {
                cs->schedulingError = true;
                cs->negativeFloat = cs->startTime - m_constraintStartTime;
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to meet constraint. Negative float=%2", constraintToString( true ), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
            }
            break;
        case Node::FinishNotLater:
            // cs->startTime calculated above
            //kDebug()<<"FinishNotLater:"<<m_constraintEndTime<<cs->startTime;
            cs->startTime = workTimeAfter( cs->startTime );
            cs->duration = duration(cs->startTime, use, false);
            cs->endTime = cs->startTime + cs->duration;
            makeAppointments();
            if ( cs->recalculate() && completion().isStarted() ) {
                // copy start times + appointments from parent schedule
                copyAppointments();
                cs->duration = cs->endTime - cs->startTime;
            }
            if ( cs->lateFinish > cs->endTime ) {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            } else {
                cs->positiveFloat = Duration::zeroDuration;
            }
            if (cs->endTime > m_constraintEndTime) {
                //kWarning()<<"cs->endTime > m_constraintEndTime";
                cs->schedulingError = true;
                cs->negativeFloat = cs->endTime - m_constraintEndTime;
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to meet constraint. Negative float=%2", constraintToString( true ), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
            }
            break;
        case Node::MustStartOn:
            // cs->startTime calculated above as predecessors endTime
            cs->startTime = workTimeAfter( cs->startTime );
            if (m_constraintStartTime > cs->startTime ) {
                cs->startTime = qMin( m_constraintStartTime, cs->lateStart );
            }
            //kDebug()<<"MustStartOn="<<m_constraintStartTime<<"<"<<cs->startTime;
            cs->duration = duration(cs->startTime, use, false);
            cs->endTime = cs->startTime + cs->duration;
            makeAppointments();
            if ( cs->recalculate() && completion().isStarted() ) {
                // copy start times + appointments from parent schedule
                copyAppointments();
                cs->duration = cs->endTime - cs->startTime;
            }
            if ( cs->lateFinish > cs->endTime ) {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            } else {
                cs->positiveFloat = Duration::zeroDuration;
            }
            if (m_constraintStartTime < cs->startTime ) {
                cs->schedulingError = true;
                cs->negativeFloat = cs->startTime - m_constraintStartTime;
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to meet constraint. Negative float=%2", constraintToString( true ), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
            }
            break;
        case Node::MustFinishOn:
            cs->endTime = workTimeBefore( qMin( m_constraintEndTime, cs->lateFinish ) );
            cs->duration = duration(cs->endTime, use, true);
            cs->startTime = workTimeAfter( cs->startTime );
            if ( cs->startTime <= cs->endTime - cs->duration ) {
                cs->startTime = cs->endTime - cs->duration;
            } else {
                cs->duration = duration(cs->startTime, use, false);
                cs->endTime = cs->startTime + cs->duration;
            }
            //kDebug()<<"MustFinishOn:"<<m_constraintEndTime<<","<<cs->lateFinish<<":"<<cs->startTime<<cs->endTime;
            makeAppointments();
            if ( cs->recalculate() && completion().isStarted() ) {
                // copy start times + appointments from parent schedule
                copyAppointments();
                cs->duration = cs->endTime - cs->startTime;
            }
            if ( cs->lateFinish > cs->endTime ) {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            } else {
                cs->positiveFloat = Duration::zeroDuration;
            }
            if ( cs->endTime != m_constraintEndTime  ) {
                cs->schedulingError = true;
                cs->negativeFloat = cs->endTime - m_constraintEndTime;
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to meet constraint. Negative float=%2", constraintToString( true ), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
            }
            break;
        case Node::FixedInterval: {
            // cs->startTime calculated above
            //kDebug()<<"FixedInterval="<<m_constraintStartTime<<""<<cs->startTime;
            cs->duration = m_constraintEndTime - m_constraintStartTime;
            if ( m_constraintStartTime >= cs->earlyStart ) {
                cs->startTime = m_constraintStartTime;
                cs->endTime = m_constraintEndTime;
            } else {
                cs->startTime = cs->earlyStart;
                cs->endTime = cs->startTime + cs->duration;
                cs->schedulingError = true;
            }
            if ( m_constraintStartTime < cs->startTime ) {
                cs->schedulingError = true;
                cs->negativeFloat = cs->startTime - m_constraintStartTime;
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to meet constraint. Negative float=%2", constraintToString( true ), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
            }
            if ( cs->lateFinish > cs->endTime ) {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            } else {
                cs->positiveFloat = Duration::zeroDuration;
            }
            cs->workStartTime = m_constraintStartTime;
            cs->workEndTime = m_constraintEndTime;
            //kDebug()<<"FixedInterval="<<cs->startTime<<","<<cs->endTime;
            makeAppointments();
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
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to meet constraint. Negative float=%2", constraintToString( true ), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
            } else if ( m_constraintStartTime > cs->lateFinish ) {
                cs->schedulingError = true;
                cs->negativeFloat = m_constraintStartTime - cs->lateFinish;
            } else {
                cs->startTime = m_constraintStartTime;
            }
            cs->endTime = cs->startTime;
            if ( cs->negativeFloat == Duration::zeroDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime; // FIXME: Work??
            }
            break;
        case Node::MustFinishOn:
            if (m_constraintEndTime < cs->startTime ||
                m_constraintEndTime > cs->lateFinish) {
                cs->schedulingError = true;
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to meet constraint. Negative float=%2", constraintToString( true ), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
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
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to meet constraint. Negative float=%2", constraintToString( true ), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
            }
            cs->endTime = cs->startTime;
            if ( cs->negativeFloat == Duration::zeroDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            }
            break;
        case Node::FinishNotLater:
            //kDebug()<<m_constraintEndTime<<cs->startTime;
            if (cs->startTime < m_constraintEndTime) {
                cs->startTime = m_constraintEndTime < cs->lateStart ? m_constraintEndTime : cs->lateStart;
            }
            if (cs->startTime > m_constraintEndTime) {
                cs->schedulingError = true;
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to meet constraint. Negative float=%2", constraintToString( true ), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
            }
            cs->endTime = cs->startTime;
            if ( cs->negativeFloat == Duration::zeroDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            }
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
        kWarning()<<"Summarytasks should not be calculated here: "<<m_name;
    }
    //kDebug()<<cs->startTime<<" :"<<cs->endTime<<""<<m_name<<" scheduleForward()";
    if ( cs->startTime < projectNode()->constraintStartTime() || cs->endTime > projectNode()->constraintEndTime() ) {
        cs->logError( i18n( "Failed to schedule within project target time" ) );
    }
    foreach ( Appointment *a, cs->appointments() ) {
        cs->logInfo( i18n( "Resource %1 booked from %2 to %3", a->resource()->resource()->name(), locale->formatDateTime( a->startTime() ), locale->formatDateTime( a->endTime() ) ) );
    }
    if ( cs->startTime < cs->earlyStart ) {
        cs->logWarning( i18n( "Starting earlier than early start" ) );
    }
    if ( cs->endTime > cs->lateFinish ) {
        cs->logWarning( i18n( "Finishing later than late finish" ) );
    }
    cs->logInfo( i18n( "Scheduled: %1 to %2", locale->formatDateTime( cs->startTime ), locale->formatDateTime( cs->endTime ) ) );
    m_visitedForward = true;
    cs->incProgress();
    m_requests.resetDynamicAllocations();
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
    if (m_visitedBackward) {
        return cs->startTime;
    }
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
    KLocale *locale = KGlobal::locale();
    cs->logInfo( i18n( "Schedule from end time: %1", cs->endTime.toString() ) );
    if (type() == Node::Type_Task) {
        cs->duration = m_estimate->value(use, pert);
        switch (m_constraint) {
        case Node::ASAP: {
            // cs->endTime calculated above
            //kDebug()<<m_name<<": end="<<cs->endTime<<"  early="<<cs->earlyStart;
            //TODO: try to keep within projects constraint times
            cs->startTime = workTimeAfter( cs->earlyStart, cs );
            cs->duration = duration(cs->startTime, use, false);
            DateTime e = cs->startTime + cs->duration;
            if ( e > cs->lateFinish ) {
                cs->schedulingError = true;
                cs->negativeFloat = e - cs->lateFinish;
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to schedule within late finish. Negative float=%2", constraintToString(), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
                cs->logDebug( "ASAP: late finish=" + cs->lateFinish.toString() + " end time=" + e.toString() + " negativeFloat=" +  cs->negativeFloat.toString() );
            } else if ( e > cs->endTime ) {
                cs->schedulingError = true;
                cs->logWarning( i18nc( "1=type of constraint", "%1: Failed to schedule within successors start time",  constraintToString() ) );
                cs->logDebug( "ASAP: succ. start=" + cs->endTime.toString() + " end time=" + e.toString() );
            }
            if ( cs->lateFinish > e ) {
                DateTime w = workTimeBefore( cs->lateFinish );
                if ( w > e ) {
                    cs->positiveFloat = w - e;
                }
                cs->logDebug( "ASAP: positiveFloat=" + cs->positiveFloat.toString() );
            }
            cs->endTime = e;
            makeAppointments();
            break;
        }
        case Node::ALAP:
        {
            // cs->endTime calculated above
            //kDebug()<<m_name<<": end="<<cs->endTime<<"  late="<<cs->lateFinish;
            cs->endTime = workTimeBefore(cs->endTime);
            cs->duration = duration(cs->endTime, use, true);
            cs->startTime = cs->endTime - cs->duration;
            if ( cs->startTime < cs->earlyStart ) {
                cs->schedulingError = true;
                cs->negativeFloat = cs->earlyStart - cs->startTime;
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to schedule after early start. Negative float=%2", constraintToString(), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
                cs->logDebug( "ALAP: earlyStart=" + cs->earlyStart.toString() + " cs->startTime=" + cs->startTime.toString() + " negativeFloat=" +  cs->negativeFloat.toString() );
            } else if ( cs->lateFinish > cs->endTime ) {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
                cs->logDebug( "ALAP: positiveFloat=" + cs->positiveFloat.toString() );
            }
            //kDebug()<<m_name<<": lateStart="<<cs->startTime;
            makeAppointments();
            break;
        }
        case Node::StartNotEarlier:
            // cs->endTime calculated above
            //kDebug()<<"StartNotEarlier:"<<m_constraintStartTime<<cs->endTime;
            cs->endTime = workTimeBefore( cs->endTime );
            cs->duration = duration(cs->endTime, use, true);
            cs->startTime = cs->endTime - cs->duration;
            if ( m_constraintStartTime > cs->startTime ) {
                //kWarning()<<"m_constraintStartTime > cs->lateStart";
                cs->schedulingError = true;
                cs->negativeFloat = m_constraintStartTime - cs->startTime;
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to meet constraint. Negative float=%2", constraintToString( true ), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
            }
            makeAppointments();
            if ( cs->lateFinish > cs->endTime ) {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            }
            break;
        case Node::FinishNotLater:
            // cs->endTime calculated above
            //kDebug()<<"FinishNotLater:"<<m_constraintEndTime<<cs->endTime;
            if (cs->endTime > m_constraintEndTime) {
                cs->endTime = qMax( qMin( m_constraintEndTime, cs->lateFinish ), cs->earlyFinish );
            }
            cs->endTime = workTimeBefore( cs->endTime );
            cs->duration = duration(cs->endTime, use, true);
            cs->startTime = cs->endTime - cs->duration;
            if ( cs->endTime > m_constraintEndTime ) {
                cs->negativeFloat = cs->endTime - m_constraintEndTime;
                cs->schedulingError = true;
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to meet constraint. Negative float=%2", constraintToString( true ), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
            }
            makeAppointments();
            if ( cs->lateFinish > cs->endTime ) {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            }
            break;
        case Node::MustStartOn:
            // cs->endTime calculated above
            //kDebug()<<"MustStartOn="<<m_constraintStartTime.toString()<<""<<cs->startTime.toString();
            cs->startTime = workTimeAfter( qMax( m_constraintStartTime, cs->earlyStart ) );
            cs->duration = duration(cs->startTime, use, false);
            if ( cs->endTime >= cs->startTime + cs->duration ) {
                cs->endTime = cs->startTime + cs->duration;
            } else {
                cs->endTime = workTimeBefore( cs->endTime );
                cs->duration = duration(cs->endTime, use, true);
                cs->startTime = cs->endTime - cs->duration;
            }
            if (m_constraintStartTime != cs->startTime) {
                cs->schedulingError = true;
                cs->negativeFloat = m_constraintStartTime - cs->startTime;
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to meet constraint. Negative float=%2", constraintToString( true ), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
            }
            makeAppointments();
            if ( cs->lateFinish > cs->endTime ) {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            }
            break;
        case Node::MustFinishOn:
            // cs->endTime calculated above
            //kDebug()<<m_name<<"MustFinishOn:"<<m_constraintEndTime<<cs->endTime<<cs->earlyFinish;
            if (cs->endTime > m_constraintEndTime) {
                cs->endTime = qMax( qMin( m_constraintEndTime, cs->lateFinish ), cs->earlyFinish );
            }
            cs->endTime = workTimeBefore( cs->endTime );
            cs->duration = duration(cs->endTime, use, true);
            cs->startTime = cs->endTime - cs->duration;
            if (m_constraintEndTime != cs->endTime ) {
                cs->negativeFloat = m_constraintEndTime - cs->endTime;
                cs->schedulingError = true;
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to meet constraint. Negative float=%2", constraintToString( true ), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
                //kWarning()<<"m_constraintEndTime > cs->endTime";
            }
            makeAppointments();
            if ( cs->lateFinish > cs->endTime ) {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            }
            break;
        case Node::FixedInterval: {
            // cs->endTime calculated above
            //kDebug()<<m_name<<"FixedInterval="<<m_constraintEndTime<<""<<cs->endTime;
            cs->duration = m_constraintEndTime - m_constraintStartTime;
            if ( cs->endTime > m_constraintEndTime ) {
                cs->endTime = qMax( m_constraintEndTime, cs->earlyFinish );
            }
            cs->startTime = cs->endTime - cs->duration;
            if (m_constraintEndTime != cs->endTime) {
                cs->negativeFloat = m_constraintEndTime - cs->endTime;
                cs->schedulingError = true;
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to meet constraint. Negative float=%2", constraintToString( true ), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
            }
            cs->workStartTime = workTimeAfter( cs->startTime );
            cs->workEndTime = workTimeBefore( cs->endTime );
            makeAppointments();
            if ( cs->negativeFloat == Duration::zeroDuration ) {
                cs->positiveFloat = workTimeBefore( cs->lateFinish ) - cs->endTime;
            }
            break;
        }
        default:
            break;
        }
        m_requests.reserve(cs->startTime, cs->duration);
    } else if (type() == Node::Type_Milestone) {
        switch (m_constraint) {
        case Node::ASAP:
            if ( cs->endTime < cs->earlyStart ) {
                cs->schedulingError = true;
                cs->negativeFloat = cs->earlyStart - cs->endTime;
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to schedule after early start. Negative float=%2", constraintToString(), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
                cs->endTime = cs->earlyStart;
            } else {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            }
            //cs->endTime = cs->earlyStart; FIXME need to follow predeccessors. Defer scheduling?
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
                cs->negativeFloat = cs->earlyStart - m_constraintStartTime;
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to meet constraint. Negative float=%2", constraintToString( true ), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
            }
            cs->startTime = cs->earlyStart;
            cs->endTime = cs->earlyStart;
            if ( cs->negativeFloat == Duration::zeroDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            }
            break;
        case Node::MustFinishOn:
            if (m_constraintEndTime < cs->earlyStart ||
                m_constraintEndTime > cs->endTime) {
                cs->schedulingError = true;
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to meet constraint. Negative float=%2", constraintToString( true ), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
            }
            cs->startTime = m_constraintEndTime;
            cs->endTime = m_constraintEndTime;
            if ( cs->negativeFloat == Duration::zeroDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            }
            break;
        case Node::StartNotEarlier:
            if ( m_constraintStartTime < cs->endTime ) {
                cs->endTime = m_constraintStartTime > cs->earlyStart ? m_constraintStartTime : cs->earlyStart;
            }
            cs->startTime = cs->endTime;
            if (m_constraintStartTime > cs->startTime) {
                cs->schedulingError = true;
                cs->negativeFloat = m_constraintStartTime - cs->startTime;
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to meet constraint. Negative float=%2", constraintToString( true ), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
            }
            if ( cs->negativeFloat == Duration::zeroDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            }
            break;
        case Node::FinishNotLater:
            if ( m_constraintEndTime < cs->endTime ) {
                cs->endTime = m_constraintEndTime > cs->earlyStart ? m_constraintEndTime : cs->earlyStart;
            }
            if (m_constraintEndTime < cs->endTime) {
                cs->schedulingError = true;
                cs->negativeFloat = cs->endTime - m_constraintEndTime;
                cs->logError( i18nc( "1=type of constraint", "%1: Failed to meet constraint. Negative float=%2", constraintToString( true ), cs->negativeFloat.toString( Duration::Format_i18nHour ) ) );
            }
            cs->startTime = cs->endTime;
            if ( cs->negativeFloat == Duration::zeroDuration ) {
                cs->positiveFloat = cs->lateFinish - cs->endTime;
            }
            break;
        default:
            break;
        }
        cs->duration = Duration::zeroDuration;
    } else if (type() == Node::Type_Summarytask) {
        //shouldn't come here
        cs->startTime = cs->endTime;
        cs->duration = cs->endTime - cs->startTime;
        kWarning()<<"Summarytasks should not be calculated here: "<<m_name;
    }
    if ( cs->startTime < projectNode()->constraintStartTime() || cs->endTime > projectNode()->constraintEndTime() ) {
        cs->logError( i18n( "Failed to schedule within project target time" ) );
    }
    foreach ( Appointment *a, cs->appointments() ) {
        cs->logInfo( i18n( "Resource %1 booked from %2 to %3", a->resource()->resource()->name(), locale->formatDateTime( a->startTime() ), locale->formatDateTime( a->endTime() ) ) );
    }
    if ( cs->startTime < cs->earlyStart ) {
        cs->logWarning( i18n( "Starting earlier than early start" ) );
    }
    if ( cs->endTime > cs->lateFinish ) {
        cs->logWarning( i18n( "Finishing later than late finish" ) );
    }
    cs->logInfo( i18n( "Scheduled: %1 to %2", locale->formatDateTime( cs->startTime ), locale->formatDateTime( cs->endTime ) ) );
    m_visitedBackward = true;
    cs->incProgress();
    m_requests.resetDynamicAllocations();
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
    if (m_currentSchedule == 0) {
        kError()<<"No current schedule";
        return Duration::zeroDuration;
    }
    if (!time.isValid()) {
        m_currentSchedule->logDebug( "Calculate duration: Start time is not valid" );
        return Duration::zeroDuration;
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
    
    // Already checked: m_currentSchedule and time.
    Duration dur = effort; // use effort as default duration
    if (m_estimate->type() == Estimate::Type_Effort) {
        if (m_requests.isEmpty()) {
            m_currentSchedule->resourceError = true;
            m_currentSchedule->logError( i18n( "No resource has been allocated") );
            return effort;
        }
        dur = m_requests.duration(time, effort, m_currentSchedule, backward);
        if (dur == Duration::zeroDuration) {
            kWarning()<<"zero duration: Resource not available";
            m_currentSchedule->resourceNotAvailable = true;
            dur = effort; //???
        }
        return dur;
    }
    if (m_estimate->type() == Estimate::Type_Duration) {
        return length( time, dur, backward );
    }
    kError()<<"Unsupported estimate type: "<<m_estimate->type();
    return dur;
}

Duration Task::length(const DateTime &time, const Duration &duration, bool backward) {
    //kDebug()<<"--->"<<(backward?"(B)":"(F)")<<m_name<<""<<time.toString()<<": duration:"<<duration.toString(Duration::Format_Day)<<" ("<<duration.milliseconds()<<")";
    
    Duration l;
    if ( duration == Duration::zeroDuration ) {
        m_currentSchedule->logDebug( "Calculate length: estimate == 0" );
        return l;
    }
    Calendar *cal = m_estimate->calendar();
    if ( cal == 0) {
        m_currentSchedule->logDebug( "Calculate length: No calendar, return estimate " + duration.toString() );
        return duration;
    }
    m_currentSchedule->logDebug( "Calculate length from: " + time.toString() );
    DateTime logtime = time;
    bool sts=true;
    bool match = false;
    DateTime start = time;
    int inc = backward ? -1 : 1;
    DateTime end = start;
    Duration l1;
    int nDays = backward ? projectNode()->constraintStartTime().daysTo( time ) : time.daysTo( projectNode()->constraintEndTime() );
    for (int i=0; !match && i <= nDays; ++i) {
        // days
        end = end.addDays(inc);
        l1 = backward ? cal->effort(end, start) : cal->effort(start, end);
        //kDebug()<<"["<<i<<"of"<<nDays<<"]"<<(backward?"(B)":"(F):")<<"  start="<<start<<" l+l1="<<(l+l1).toString()<<" match"<<duration.toString();
        if (l + l1 < duration) {
            l += l1;
            start = end;
        } else if (l + l1 == duration) {
            l += l1;
            match = true;
        } else {
            end = start;
            break;
        }
    }
    if ( ! match ) {
        m_currentSchedule->logDebug( "Days: duration " + logtime.toString() + " - " + end.toString() + " = " + l.toString() + " (" + (duration - l).toString() + ')' );
        
        logtime = start;
        for (int i=0; !match && i < 24; ++i) {
            // hours
            end = end.addSecs(inc*60*60);
            l1 = backward ? cal->effort(end, start) : cal->effort(start, end);
            if (l + l1 < duration) {
                l += l1;
                start = end;
            } else if (l + l1 == duration) {
                l += l1;
                match = true;
            } else {
                end = start;
                break;
            }
            //kDebug()<<"duration(h)["<<i<<"]"<<(backward?"backward":"forward:")<<" time="<<start.time()<<" l="<<l.toString()<<" ("<<l.milliseconds()<<')';
        }
        //kDebug()<<"duration"<<(backward?"backward":"forward:")<<start.toString()<<" l="<<l.toString()<<" ("<<l.milliseconds()<<")  match="<<match<<" sts="<<sts;
    }
    if ( ! match ) {
        m_currentSchedule->logDebug( "Hours: duration " + logtime.toString() + " - " + end.toString() + " = " + l.toString() + " (" + (duration - l).toString() + ')' );
        
        logtime = start;
        for (int i=0; !match && i < 60; ++i) {
            //minutes
            end = end.addSecs(inc*60);
            l1 = backward ? cal->effort(end, start) : cal->effort(start, end);
            if (l + l1 < duration) {
                l += l1;
                start = end;
            } else if (l + l1 == duration) {
                l += l1;
                match = true;
            } else if (l + l1 > duration) {
                end = start;
                break;
            }
            //kDebug()<<"duration(m)"<<(backward?"backward":"forward:")<<"  time="<<start.time().toString()<<" l="<<l.toString()<<" ("<<l.milliseconds()<<')';
        }
        //kDebug()<<"duration"<<(backward?"backward":"forward:")<<"  start="<<start.toString()<<" l="<<l.toString()<<" match="<<match<<" sts="<<sts;
    }
    if ( ! match ) {
        m_currentSchedule->logDebug( "Minutes: duration " + logtime.toString() + " - " + end.toString() + " = " + l.toString() + " (" + (duration - l).toString() + ')' );
        
        logtime = start;
        for (int i=0; !match && i < 60 && sts; ++i) {
            //seconds
            end = end.addSecs(inc);
            l1 = backward ? cal->effort(end, start) : cal->effort(start, end);
            if (l + l1 < duration) {
                l += l1;
                start = end;
            } else if (l + l1 == duration) {
                l += l1;
                match = true;
            } else if (l + l1 > duration) {
                end = start;
                break;
            }
            //kDebug()<<"duration(s)["<<i<<"]"<<(backward?"backward":"forward:")<<" time="<<start.time().toString()<<" l="<<l.toString()<<" ("<<l.milliseconds()<<')';
        }
    }
    if ( ! match ) {
        m_currentSchedule->logDebug( "Seconds: duration " + logtime.toString() + " - " + end.toString() + " l " + l.toString() + " (" + (duration - l).toString() + ')' );
        
        for (int i=0; !match && i < 1000; ++i) {
            //milliseconds
            end.setTime(end.time().addMSecs(inc));
            l1 = backward ? cal->effort(end, start) : cal->effort(start, end);
            if (l + l1 < duration) {
                l += l1;
                start = end;
            } else if (l + l1 == duration) {
                l += l1;
                match = true;
            } else {
                m_currentSchedule->logDebug( "Got more than asked for, should not happen! Want: " + duration.toString(Duration::Format_Hour) + " got: " + l.toString(Duration::Format_Hour) );
                break;
            }
            //kDebug()<<"duration(ms)["<<i<<"]"<<(backward?"backward":"forward:")<<" time="<<start.time().toString()<<" l="<<l.toString()<<" ("<<l.milliseconds()<<')';
        }
    }
    if (!match) {
        m_currentSchedule->logError( i18n( "Could not match work duration. Want: %1 got: %2",  l.toString( Duration::Format_i18nHour ), duration.toString( Duration::Format_i18nHour ) ) );
    }
    DateTime t = end;
    if (l != Duration::zeroDuration) {
        if ( backward ) {
            if ( end < projectNode()->constraintEndTime() ) {
                t = cal->firstAvailableAfter(end, projectNode()->constraintEndTime());
            }
        } else {
            if ( end > projectNode()->constraintStartTime() ) {
                t = cal->firstAvailableBefore(end, projectNode()->constraintStartTime());
            }
        }
        m_currentSchedule->logDebug( "Moved end to work: " + end.toString() + " -> " + t.toString() );
    }
    end = t.isValid() ? t : time;
    //kDebug()<<"<---"<<(backward?"(B)":"(F)")<<m_name<<":"<<end.toString()<<"-"<<time.toString()<<"="<<(end - time).toString()<<" duration:"<<duration.toString(Duration::Format_Day);
    l = end>time ? end-time : time-end;
    if ( match ) {
        m_currentSchedule->logDebug( "Calculated length: " + time.toString() + " - " + end.toString() + " = " + l.toString() );
    }
    return l;
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

DateTime Task::workTimeAfter(const DateTime &dt, Schedule *sch) const {
    DateTime t;
    if ( m_estimate->type() == Estimate::Type_Duration ) {
        if ( m_estimate->calendar() ) {
            t = m_estimate->calendar()->firstAvailableAfter( dt, projectNode()->constraintEndTime() );
        }
    } else {
        t = m_requests.workTimeAfter(dt, sch);
    }
    return t.isValid() ? t : dt;
}

DateTime Task::workTimeBefore(const DateTime &dt, Schedule *sch) const {
    DateTime t;
    if ( m_estimate->type() == Estimate::Type_Duration ) {
        if ( m_estimate->calendar() ) {
            t = m_estimate->calendar()->firstAvailableBefore( dt, projectNode()->constraintStartTime() );
        }
    } else {
        t = m_requests.workTimeBefore(dt, sch);
    }
    return t.isValid() ? t : dt;
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
    return s == 0 || s->earlyStart > s->lateStart ? Duration::zeroDuration : ( s->earlyStart - s->lateStart );
}

Duration Task::finishFloat( long id ) const
{
    Schedule *s = schedule( id );
    return s == 0 || s->lateFinish < s->earlyFinish ? Duration::zeroDuration : ( s->lateFinish - s->earlyFinish );
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
    //kDebug()<<m_name;
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
        //kDebug()<<m_name<<": "<<cs->freeFloat.toString();
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
    if ( ! isScheduled( id ) ) {
        st |= State_NotScheduled;
    }
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

void Task::addWorkPackage( WorkPackage *wp )
{
    qDebug()<<"addWorkPackage:"<<m_name<<wp->ownerName();
    emit workPackageToBeAdded( this, m_packageLog.count() );
    m_packageLog.append( wp );
    emit workPackageAdded( this );
}

void Task::removeWorkPackage( WorkPackage *wp )
{
    qDebug()<<"removeWorkPackage:"<<m_name<<wp->ownerName();
    int index = m_packageLog.indexOf( wp );
    if ( index < 0 ) {
        return;
    }
    emit workPackageToBeRemoved( this, index );
    m_packageLog.removeAt( index );
    emit workPackageRemoved( this );
}

WorkPackage *Task::workPackageAt( int index ) const
{
    Q_ASSERT ( index >= 0 && index < m_packageLog.count() );
    return m_packageLog.at( index );
}

QString Task::wpOwnerName() const
{
    if ( m_packageLog.isEmpty() ) {
        return m_workPackage.ownerName();
    }
    return m_packageLog.last()->ownerName();
}

WorkPackage::WPTransmitionStatus Task::wpTransmitionStatus() const
{
    if ( m_packageLog.isEmpty() ) {
        return m_workPackage.transmitionStatus();
    }
    return m_packageLog.last()->transmitionStatus();
}

DateTime Task::wpTransmitionTime() const
{
    if ( m_packageLog.isEmpty() ) {
        return m_workPackage.transmitionTime();
    }
    return m_packageLog.last()->transmitionTime();
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
    foreach ( const QDate &d, p.entries().keys() ) {
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
    foreach ( const QDate &d, m_entries.keys() ) {
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
    foreach ( const QDate &d, m_entries.keys() ) {
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
        foreach( UsedEffort *ue, m_usedEffort ) {
            foreach ( const QDate &d, ue->actualEffortMap().keys() ) {
                eff += ue->actualEffortMap()[ d ]->effort();
            }
        }
    } else if ( ! m_entries.isEmpty() ) {
        eff = m_entries.values().last()->totalPerformed;
    }
    return eff;
}

Duration Completion::actualEffort( const Resource *resource, const QDate &date ) const
{
    UsedEffort *ue = usedEffort( resource );
    if ( ue == 0 ) {
        return Duration::zeroDuration;
    }
    UsedEffort::ActualEffort *ae = ue->effort( date );
    if ( ae == 0 ) {
        return Duration::zeroDuration;
    }
    return ae->effort();
}

Duration Completion::actualEffort( const QDate &date ) const
{
    Duration eff;
    if ( m_entrymode == EnterEffortPerResource ) {
        foreach( UsedEffort *ue, m_usedEffort ) {
            if ( ue && ue->actualEffortMap().contains( date ) ) {
                eff += ue->actualEffortMap().value( date )->effort();
            }
        }
    } else {
        // Hmmm: How to really knowon a specific date?
        if ( m_entries.contains( date ) ) {
            eff = m_entries[ date ]->totalPerformed;
        }
    }
    return eff;
}

Duration Completion::actualEffortTo( const QDate &date ) const
{
    //kDebug()<<date;
    Duration eff;
    if ( m_entrymode == EnterEffortPerResource ) {
        foreach( UsedEffort *ue, m_usedEffort ) {
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

double Completion::averageCostPrHour( const QDate &date, long id ) const
{
    Schedule *s = m_node->schedule( id );
    if ( s == 0 ) {
        return 0.0;
    }
    double cost = 0.0;
    double eff = 0.0;
    QList<double> cl;
    foreach ( Appointment *a, s->appointments() ) {
        cl << a->resource()->resource()->normalRate();
        double e = a->plannedEffort( date ).toDouble( Duration::Unit_h );
        if (  e > 0.0 ) {
            eff += e;
            cost += e * cl.last();
        }
    }
    if ( eff > 0.0 ) {
        cost /= eff;
    } else {
        foreach ( double c, cl ) {
            cost += c;
        }
        cost /= cl.count();
    }
    return cost;
}

EffortCostMap Completion::effortCostPrDay(const QDate &start, const QDate &end, long id ) const
{
    //kDebug()<<m_node->name()<<start<<end;
    EffortCostMap ec;
    if ( ! isStarted() ) {
        return ec;
    }
    switch ( m_entrymode ) {
        case FollowPlan:
            break;
        case EnterCompleted:
        case EnterEffortPerTask: {
            QDate st = start.isValid() ? start : m_startTime.date();
            QDate et = end.isValid() ? end : m_finishTime.date();
            Duration last;
            foreach ( const QDate &d, m_entries.uniqueKeys() ) {
                if ( d < st ) {
                    continue;
                }
                Duration e = m_entries[ d ]->totalPerformed;
                if ( e != Duration::zeroDuration && e != last ) {
                    Duration eff = e - last;
                    ec.insert( d, eff, eff.toDouble( Duration::Unit_h ) * averageCostPrHour( d, id ) );
                    last = e;
                }
                if ( et.isValid() && d > et ) {
                    break;
                }
            }
            break;
        }
        case EnterEffortPerResource: {
            QDate st = start.isValid() ? start : m_startTime.date();
            QDate et = end.isValid() ? end : m_finishTime.date();
            for ( QDate d = st; d <= et; d = d.addDays( 1 ) ) {
                ec.add( d, actualEffort( d ), actualCost( d ) );
            }
            break;
        }
    }
    return ec;
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
    foreach ( UsedEffort::ActualEffort *a, m_usedEffort.value( const_cast<Resource*>( resource )  )->actualEffortMap() ) {
        c += a->normalEffort().toDouble( Duration::Unit_h ) * nc;
        c += a->overtimeEffort().toDouble( Duration::Unit_h ) * oc;
    }
    return c;
}

EffortCostMap Completion::actualEffortCost( long id ) const
{
    //kDebug();
    EffortCostMap map;
    if ( ! isStarted() ) {
        return map;
    }
    QList< const QMap<QDate, UsedEffort::ActualEffort*>* > lst;
    QList< double > rate;
    QDate start, end;
    foreach ( const Resource *r, m_usedEffort.keys() ) {
        //kDebug()<<m_node->name()<<r->name();
        const QMap<QDate, UsedEffort::ActualEffort*> &m = usedEffort( r )->actualEffortMap();
        if ( m.isEmpty() ) {
            continue;
        }
        if ( ! start.isValid() || start > m.keys().first() ) {
            start = m.keys().first();
        }
        if ( ! end.isValid() || end < m.keys().last() ) {
            end = m.keys().last();
        }
        lst.append( &m );
        rate.append( r->normalRate() );
    }
    qDebug()<<"actualEffortCost:"<<m_node->name()<<start<<end<<lst;
    if ( ! lst.isEmpty() && start.isValid() && end.isValid() ) {
        for ( QDate d = start; d <= end; d = d.addDays( 1 ) ) {
            EffortCost c;
            for ( int i = 0; i < lst.count(); ++i ) {
                UsedEffort::ActualEffort *a = lst.at( i )->value( d );
                if ( a == 0 ) {
                    continue;
                }
                double nc = rate.at( i );
                Duration eff = a->normalEffort();
                double cost = eff.toDouble( Duration::Unit_h ) * nc;
                c.add( eff, cost );
                //kDebug()<<m_node->name()<<d<<eff.toDouble(Duration::Unit_h)<<nc<<cost<<"->"<<c.effort().toDouble(Duration::Unit_h)<<c.cost();
            }
            if ( c.effort() != Duration::zeroDuration || c.cost() != 0.0 ) {
                map.add( d, c );
                qDebug()<<"actualEffortCost: added"<<m_node->name()<<d<<c.hours()<<c.cost();
            }
        }
    } else if ( ! m_entries.isEmpty() ) {
        QDate st = start.isValid() ? start : m_startTime.date();
        QDate et = end.isValid() ? end : m_finishTime.date();
        Duration last;
        foreach ( const QDate &d, m_entries.uniqueKeys() ) {
            if ( d < st ) {
                continue;
            }
            Duration e = m_entries[ d ]->totalPerformed;
            if ( e != Duration::zeroDuration && e != last ) {
                //kDebug()<<m_node->name()<<d<<(e - last).toDouble(Duration::Unit_h);
                double eff = ( e - last ).toDouble( Duration::Unit_h );
                map.insert( d, e - last, eff * averageCostPrHour( d, id ) ); // try to guess cost
                last = e;
            }
            if ( et.isValid() && d > et ) {
                break;
            }
        }
    }
    return map;
}

EffortCost Completion::actualCostTo( const QDate &date ) const
{
    //kDebug()<<date;
    EffortCost c;
    foreach ( const Resource *r, m_usedEffort.keys() ) {
        double nc = r->normalRate();
        //double oc = r->overtimeRate();
        const QMap<QDate, UsedEffort::ActualEffort*> &map = usedEffort( r )->actualEffortMap();
        QMap<QDate, UsedEffort::ActualEffort*>::const_iterator it;
        for ( it = map.constBegin(); it != map.constEnd(); ++it ) {
            if ( it.key() > date ) {
                break;
            }
            UsedEffort::ActualEffort *a = it.value();
            Duration eff = a->normalEffort();
            double cost = eff.toDouble( Duration::Unit_h ) * nc;
            c.add( Duration::zeroDuration, cost );
            //kDebug()<<date<<r->name()<<it.key()<<c.cost();
        }
    }
    Duration eff = actualEffortTo( date );
    c.add( eff, 0.0 );
    //kDebug()<<date<<eff.toString()<<c.effort().toString()<<c.cost();
    return c;
}

QStringList Completion::entrymodeList() const
{
    return QStringList()
            << "FollowPlan"
            << "EnterCompleted"
            << "EnterEffortPerTask"
            << "EnterEffortPerResource";

}

void Completion::setEntrymode( const QString &mode )
{
    int m = entrymodeList().indexOf( mode );
    if ( m == -1 ) {
        m = EnterCompleted;
    }
    m_entrymode = static_cast<Entrymode>( m );
}
QString Completion::entryModeToString() const
{
    return entrymodeList().value( m_entrymode );
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
    setEntrymode( element.attribute( "entrymode" ) );
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
                        kWarning()<<"Invalid date: "<<date<<s;
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
                                    kWarning()<<"Cannot find resource, id="<<id;
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
    el.setAttribute("entrymode", entryModeToString());
    foreach( const QDate &date, m_entries.uniqueKeys() ) {
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
    foreach ( const QDate &d, value.actualEffortMap().keys() ) {
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
    foreach ( const QDate &d, m_actual.keys() ) {
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
    foreach ( ActualEffort *e, m_actual ) {
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
WorkPackage::WorkPackage( Task *task )
    : m_task( task ),
    m_manager( 0 ),
    m_transmitionStatus( TS_None )
{
    m_completion.setNode( task );
}

WorkPackage::WorkPackage( const WorkPackage &wp )
    : m_task( 0 ),
    m_manager( 0 ),
    m_completion( wp.m_completion ),
    m_ownerName( wp.m_ownerName ),
    m_ownerId( wp.m_ownerId ),
    m_transmitionStatus( wp.m_transmitionStatus ),
    m_transmitionTime( wp.m_transmitionTime )
{
}

WorkPackage::~WorkPackage()
{
}

bool WorkPackage::loadXML(KoXmlElement &element, XMLLoaderObject &status )
{
    m_ownerName = element.attribute( "owner" );
    m_ownerId = element.attribute( "owner-id" );
    return true;
}

void WorkPackage::saveXML(QDomElement &element) const
{
    QDomElement el = element.ownerDocument().createElement("workpackage");
    element.appendChild(el);
    el.setAttribute( "owner", m_ownerName );
    el.setAttribute( "owner-id", m_ownerId );
}

bool WorkPackage::loadLoggedXML(KoXmlElement &element, XMLLoaderObject &status )
{
    m_ownerName = element.attribute( "owner" );
    m_ownerId = element.attribute( "owner-id" );
    m_transmitionStatus = transmitionStatusFromString( element.attribute( "status" ) );
    m_transmitionTime = DateTime( KDateTime::fromString( element.attribute( "time" ) ) );
    return m_completion.loadXML( element, status );
}

void WorkPackage::saveLoggedXML(QDomElement &element) const
{
    QDomElement el = element.ownerDocument().createElement("workpackage");
    element.appendChild(el);
    el.setAttribute( "owner", m_ownerName );
    el.setAttribute( "owner-id", m_ownerId );
    el.setAttribute( "status", transmitionStatusToString( m_transmitionStatus ) );
    el.setAttribute( "time", m_transmitionTime.toString( KDateTime::ISODate ) );
    m_completion.saveXML( el );
}

QList<Resource*> WorkPackage::fetchResources()
{
    return fetchResources( id() );
}

QList<Resource*> WorkPackage::fetchResources( long id )
{
    //kDebug()<<m_task.name();
    QList<Resource*> lst;
    if ( id == NOTSCHEDULED ) {
        qDebug()<<"WorkPackage::fetchResources:"<<"No schedule";
        if ( m_task ) lst << m_task->requestedResources();
    } else {
        if ( m_task ) lst = m_task->assignedResources( id );
        foreach ( const Resource *r, m_completion.resources() ) {
            if ( ! lst.contains( const_cast<Resource*>( r ) ) ) {
                lst << const_cast<Resource*>( r );
            }
        }
    }
    qDebug()<<"WorkPackage::fetchResources:"<<lst;
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


void WorkPackage::setScheduleManager( ScheduleManager *sm )
{
    m_manager = sm;
}


QString WorkPackage::transmitionStatusToString( WorkPackage::WPTransmitionStatus sts, bool trans )
{
    QString s = trans ? i18n( "None" ) : "None";
    switch ( sts ) {
        case TS_Send: 
            s = trans ? i18n( "Send" ) : "Send";
            break;
        case TS_Receive:
            s = trans ? i18n( "Receive" ) : "Receive";
            break;
        default:
            break;
    }
    return s;
}

WorkPackage::WPTransmitionStatus WorkPackage::transmitionStatusFromString( const QString &sts )
{
    QStringList lst;
    lst << "None" << "Send" << "Receive";
    int s = lst.indexOf( sts );
    return s < 0 ? TS_None : static_cast<WPTransmitionStatus>( s );
}

//--------------------------------
WorkPackageSettings::WorkPackageSettings()
    : usedEffort( true ),
    progress( false ),
    remainingEffort( false ),
    documents( true )
{
}

void WorkPackageSettings::saveXML( QDomElement &element ) const
{
    QDomElement el = element.ownerDocument().createElement("settings");
    element.appendChild( el );
    el.setAttribute( "used-effort", usedEffort );
    el.setAttribute( "progress", progress );
    el.setAttribute( "remaining-effort", remainingEffort );
    el.setAttribute( "documents", documents );
}

bool WorkPackageSettings::loadXML( const KoXmlElement &element )
{
    qDebug()<<"WorkPackageSettings::loadXML:";
    usedEffort = (bool)element.attribute( "used-effort" ).toInt();
    progress = (bool)element.attribute( "progress" ).toInt();
    remainingEffort = (bool)element.attribute( "remaining-effort" ).toInt();
    documents = (bool)element.attribute( "documents" ).toInt();
    return true;
}

bool WorkPackageSettings::operator==( const WorkPackageSettings &s ) const
{
    return usedEffort == s.usedEffort &&
            progress == s.progress &&
            remainingEffort == s.remainingEffort &&
            documents == s.documents;
}

bool WorkPackageSettings::operator!=( const WorkPackageSettings &s ) const
{
    return ! operator==( s );
}


//----------------------------------
#ifndef NDEBUG
void Task::printDebug(bool children, const QByteArray& _indent) {
    QByteArray indent = _indent;
    qDebug()<<indent<<"+ Task node:"<<name()<<" type="<<typeToString()<<"("<<type()<<") id="<<id();
    indent += "!  ";
    qDebug()<<indent<<"Requested resources (work):"<<workUnits()<<"%";
    m_requests.printDebug(indent);
    
    completion().printDebug( indent );
    
    Node::printDebug(children, indent);

}

void Completion::printDebug(const QByteArray& _indent) const {
    QByteArray indent = _indent;
    qDebug()<<indent<<"+ Completion: ("<<m_entries.count()<<" entries)";
    indent += "!  ";
    qDebug()<<indent<<"Started:"<<m_started<<""<<m_startTime.toString();
    qDebug()<<indent<<"Finished:"<<m_finished<<""<<m_finishTime.toString();
    indent += "  ";
    foreach( const QDate &d, m_entries.keys() ) {
        Entry *e = m_entries[ d ];
        qDebug()<<indent<<"Date:"<<d;
        qDebug()<<(indent+" !")<<"% Finished:"<<e->percentFinished;
        qDebug()<<(indent+" !")<<"Remaining:"<<e->remainingEffort.toString();
        qDebug()<<(indent+" !")<<"Performed:"<<e->totalPerformed.toString();
    }
}

#endif


}  //KPlato namespace

#include "kpttask.moc"
