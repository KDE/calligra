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
#include "kptnode.h"

#include "kptappointment.h"
#include "kptaccount.h"
#include "kptwbsdefinition.h"
#include "kptresource.h"
#include "kptschedule.h"

#include <qptrlist.h>
#include <qdom.h>

#include <klocale.h>
#include <kdebug.h>

namespace KPlato
{

Node::Node(Node *parent) : m_nodes(), m_dependChildNodes(), m_dependParentNodes() {
    //kdDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    m_parent = parent;
    init();
    m_id = QString(); // Not mapped
}

Node::Node(Node &node, Node *parent) 
    : m_nodes(), 
      m_dependChildNodes(), 
      m_dependParentNodes() {
    //kdDebug()<<k_funcinfo<<"("<<this<<")"<<endl;
    m_parent = parent;
    init();
    m_name = node.name();
    m_leader = node.leader();
    m_description = node.description();
    m_constraint = (ConstraintType) node.constraint();
    m_constraintStartTime = node.constraintStartTime();
    m_constraintEndTime = node.constraintEndTime();
    
    m_dateOnlyStartDate = node.startDate();
    m_dateOnlyEndDate = node.endDate();
    
    m_runningAccount = node.runningAccount();
    m_startupAccount = node.startupAccount();
    m_shutdownAccount = node.shutdownAccount();

    m_startupCost = node.startupCost();
    m_shutdownCost = node.shutdownCost();
    
    m_schedules.setAutoDelete(node.m_schedules.autoDelete());
}

Node::~Node() {
    if (findNode() == this) {
        removeId(); // only remove myself (I may be just a working copy)
    }
    Relation *rel = 0;
    while ((rel = m_dependParentNodes.getFirst())) {
        delete rel;
    }
    while ((rel = m_dependChildNodes.getFirst())) {
        delete rel;
    }
    if (m_runningAccount)
        m_runningAccount->removeRunning(*this);
    if (m_startupAccount)
        m_startupAccount->removeStartup(*this);
    if (m_shutdownAccount)
        m_shutdownAccount->removeShutdown(*this);
}

void Node::init() {
    m_currentSchedule = 0;
    m_nodes.setAutoDelete(true);
    m_name="";
    m_constraint = Node::ASAP;
    m_effort = 0;
    m_visitedForward = false;
    m_visitedBackward = false;
    
    m_dateOnlyStartDate = m_dateOnlyEndDate = QDate::currentDate();
    m_dateOnlyDuration.addDays(1);
    
    m_runningAccount = 0;
    m_startupAccount = 0;
    m_shutdownAccount = 0;
    m_startupCost = 0.0;
    m_shutdownCost = 0.0;
}

Node *Node::projectNode() {
    if ((type() == Type_Project) || (type() == Type_Subproject)) {
        return this;
    }
    if (m_parent)
        return m_parent->projectNode();

    kdError()<<k_funcinfo<<"Ooops, no parent and no project found"<<endl;
    return 0;
}

void Node::delChildNode( Node *node, bool remove) {
    //kdDebug()<<k_funcinfo<<"find="<<m_nodes.findRef(node)<<endl;
    if ( m_nodes.findRef(node) != -1 ) {
        removeId(node->id());
        if(remove)
            m_nodes.remove();
        else
            m_nodes.take();
    }
}

void Node::delChildNode( int number, bool remove) {
    Node *n = m_nodes.at(number);
    if (n)
        removeId(n->id());
    if(remove)
        m_nodes.remove(number);
    else
        m_nodes.take(number);
}

void Node::insertChildNode( unsigned int index, Node *node) {
    if (!node->setId(node->id())) {
        kdError()<<k_funcinfo<<node->name()<<" Not unique id: "<<m_id<<endl;
    }
    m_nodes.insert(index,node);
    node->setParent(this);
}

void Node::addChildNode( Node *node, Node *after) {
    int index = m_nodes.findRef(after);
    if (index == -1) {
        if (!node->setId(node->id())) {
            kdError()<<k_funcinfo<<node->name()<<" Not unique id: "<<m_id<<endl;
        }
        m_nodes.append(node);
        node->setParent(this);
        return;
    }
    m_nodes.insert(index+1, node);
    node->setParent(this);
}

int Node::findChildNode( Node* node )
{
	return m_nodes.findRef( node );
}


const Node* Node::getChildNode(int number) const {
    // Work around missing const at() method in QPtrList
    const QPtrList<Node> &nodes = m_nodes;
    return (const_cast<QPtrList<Node> &>(nodes)).at(number);
}

Duration *Node::getDelay() {
    /* TODO
       Calculate the delay of this node. Use the calculated startTime and the setted startTime.
    */
    return 0L;
}

void Node::addDependChildNode( Node *node, Relation::Type p) {
    addDependChildNode(node,p,Duration());
}

void Node::addDependChildNode( Node *node, Relation::Type p, Duration lag) {
    Relation *relation = new Relation(this, node, p, lag);
    if (node->addDependParentNode(relation))
        m_dependChildNodes.append(relation);
    else
        delete relation;
}

void Node::insertDependChildNode( unsigned int index, Node *node, Relation::Type p) {
    Relation *relation = new Relation(this, node, p, Duration());
    if (node->addDependParentNode(relation))
        m_dependChildNodes.insert(index, relation);
    else
        delete relation;
}

bool Node::addDependChildNode( Relation *relation) {
    if(m_dependChildNodes.findRef(relation) != -1)
        return false;
    m_dependChildNodes.append(relation);
    return true;
}

// These delDepend... methods look suspicious to me, can someone review?
void Node::delDependChildNode( Node *node, bool remove) {
    if ( m_nodes.findRef(node) != -1 ) {
        if(remove)
            m_dependChildNodes.remove();
        else
            m_dependChildNodes.take();
    }
}

void Node::delDependChildNode( Relation *rel, bool remove) {
    if ( m_dependChildNodes.findRef(rel) != -1 ) {
        if(remove)
            m_dependChildNodes.remove();
        else
            m_dependChildNodes.take();
    }
}

void Node::delDependChildNode( int number, bool remove) {
    if(remove)
        m_dependChildNodes.remove(number);
    else
        m_dependChildNodes.take(number);
}

void Node::takeDependChildNode(Relation *rel) {
    if (m_dependChildNodes.findRef(rel) != -1) {
        m_dependChildNodes.take();
    }
}

void Node::addDependParentNode( Node *node, Relation::Type p) {
    addDependParentNode(node,p,Duration());
}

void Node::addDependParentNode( Node *node, Relation::Type p, Duration lag) {
    Relation *relation = new Relation(node, this, p, lag);
    if (node->addDependChildNode(relation))
        m_dependParentNodes.append(relation);
    else
        delete relation;
}

void Node::insertDependParentNode( unsigned int index, Node *node, Relation::Type p) {
    Relation *relation = new Relation(this, node, p, Duration());
    if (node->addDependChildNode(relation))
        m_dependParentNodes.insert(index,relation);
    else
        delete relation;
}

bool Node::addDependParentNode( Relation *relation) {
    if(m_dependParentNodes.findRef(relation) != -1)
        return false;
    m_dependParentNodes.append(relation);
    return true;
}

// These delDepend... methods look suspicious to me, can someone review?
void Node::delDependParentNode( Node *node, bool remove) {
    if ( m_nodes.findRef(node) != -1 ) {
        if(remove)
            m_dependParentNodes.remove();
        else
            m_dependParentNodes.take();
    }
}

void Node::delDependParentNode( Relation *rel, bool remove) {
    if ( m_dependParentNodes.findRef(rel) != -1 ) {
        if(remove)
            m_dependParentNodes.remove();
        else
            m_dependParentNodes.take();
    }
}

void Node::delDependParentNode( int number, bool remove) {
    if(remove)
        m_dependParentNodes.remove(number);
    else
        m_dependParentNodes.take(number);
}

void Node::takeDependParentNode(Relation *rel) {
    if (m_dependParentNodes.findRef(rel) != -1) {
        rel = m_dependParentNodes.take();
    }      
}

bool Node::isParentOf(Node *node) {
    if (m_nodes.findRef(node) != -1)
	    return true;

	QPtrListIterator<Node> nit(childNodeIterator());
	for ( ; nit.current(); ++nit ) {
		if (nit.current()->isParentOf(node))
		    return true;
	}
	return false;
}

Relation *Node::findParentRelation(Node *node) {
    for (int i=0; i<numDependParentNodes(); i++) {
        Relation *rel = getDependParentNode(i);
        if (rel->parent() == node)
            return rel;
    }
    return (Relation *)0;
}

Relation *Node::findChildRelation(Node *node) {
    for (int i=0; i<numDependChildNodes(); i++) {
        Relation *rel = getDependChildNode(i);
        if (rel->child() == node)
            return rel;
    }
    return (Relation *)0;
}

Relation *Node::findRelation(Node *node) {
    Relation *rel = findParentRelation(node);
    if (!rel)
        rel = findChildRelation(node);
    return rel;
}

bool Node::isDependChildOf(Node *node) {
    //kdDebug()<<k_funcinfo<<" '"<<m_name<<"' checking against '"<<node->name()<<"'"<<endl;
    for (int i=0; i<numDependParentNodes(); i++) {
        Relation *rel = getDependParentNode(i);
        if (rel->parent() == node)
            return true;
		if (rel->parent()->isDependChildOf(node))
		    return true;
    }
	return false;
}

Duration Node::duration(const DateTime &time, int use, bool backward) {
    //kdDebug()<<k_funcinfo<<endl;
    // TODO: handle risc
    if (!time.isValid()) {
        kdError()<<k_funcinfo<<"Time is invalid"<<endl;
        return Duration::zeroDuration;
    }
    if (m_effort == 0) {
        kdError()<<k_funcinfo<<"m_effort == 0"<<endl;
        return Duration::zeroDuration;
    }
    if (m_currentSchedule == 0) {
        return Duration::zeroDuration;
        kdError()<<k_funcinfo<<"No current schedule"<<endl;
    }
    kdDebug()<<k_funcinfo<<m_name<<": Use="<<use<<endl;
    return calcDuration(time, m_effort->effort(use), backward);
}

void Node::makeAppointments() {
    QPtrListIterator<Node> nit(m_nodes);
    for ( ; nit.current(); ++nit ) {
        nit.current()->makeAppointments();
    }
}

void Node::calcResourceOverbooked() {
    QPtrListIterator<Node> nit(m_nodes);
    for ( ; nit.current(); ++nit ) {
        nit.current()->calcResourceOverbooked();
    }
}

void Node::saveRelations(QDomElement &element) const {
    QPtrListIterator<Relation> it(m_dependChildNodes);
    for (; it.current(); ++it) {
        it.current()->save(element);
    }
    QPtrListIterator<Node> nodes(m_nodes);
    for ( ; nodes.current(); ++nodes ) {
        nodes.current()->saveRelations(element);
    }
}

void Node::setConstraint(QString &type) {
    // Do not i18n these, they are used in load()
    if (type == "ASAP")
        setConstraint(ASAP);
    else if (type == "ALAP")
        setConstraint(ALAP);
    else if (type == "StartNotEarlier")
        setConstraint(StartNotEarlier);
    else if (type == "FinishNotLater")
        setConstraint(FinishNotLater);
    else if (type == "MustStartOn")
        setConstraint(MustStartOn);
    else if (type == "MustFinishOn")
        setConstraint(MustFinishOn);
    else if (type == "FixedInterval")
        setConstraint(FixedInterval);
    else
        setConstraint(ASAP);  // default
}

QString Node::constraintToString() const {
    // Do not i18n these, they are used in save()
    if (m_constraint == ASAP)
        return QString("ASAP");
    else if (m_constraint == ALAP)
        return QString("ALAP");
    else if (m_constraint == StartNotEarlier)
        return QString("StartNotEarlier");
    else if (m_constraint == FinishNotLater)
        return QString("FinishNotLater");
    else if (m_constraint == MustStartOn)
        return QString("MustStartOn");
    else if (m_constraint == MustFinishOn)
        return QString("MustFinishOn");
    else if (m_constraint == FixedInterval)
        return QString("FixedInterval");

    return QString();
}

void Node::propagateEarliestStart(DateTime &time) {
    if (m_currentSchedule == 0)
        return;
    m_currentSchedule->earliestStart = time;
    //kdDebug()<<k_funcinfo<<m_name<<": "<<m_currentSchedule->earliestStart.toString()<<endl;
    QPtrListIterator<Node> it = m_nodes;
    for (; it.current(); ++it) {
        it.current()->propagateEarliestStart(time);
    }
}

void Node::propagateLatestFinish(DateTime &time) {
    if (m_currentSchedule == 0)
        return;
    m_currentSchedule->latestFinish = time;
    //kdDebug()<<k_funcinfo<<m_name<<": "<<m_currentSchedule->latestFinish<<endl;
    QPtrListIterator<Node> it = m_nodes;
    for (; it.current(); ++it) {
        it.current()->propagateLatestFinish(time);
    }
}

void Node::moveEarliestStart(DateTime &time) {
    if (m_currentSchedule == 0)
        return;
    if (m_currentSchedule->earliestStart < time)
        m_currentSchedule->earliestStart = time;
    QPtrListIterator<Node> it = m_nodes;
    for (; it.current(); ++it) {
        it.current()->moveEarliestStart(time);
    }
}

void Node::moveLatestFinish(DateTime &time) {
    if (m_currentSchedule == 0)
        return;
    if (m_currentSchedule->latestFinish > time)
        m_currentSchedule->latestFinish = time;
    QPtrListIterator<Node> it = m_nodes;
    for (; it.current(); ++it) {
        it.current()->moveLatestFinish(time);
    }
}

void Node::initiateCalculation(Schedule &sch) {
    QPtrListIterator<Node> it = m_nodes;
    for (; it.current(); ++it) {
        it.current()->initiateCalculation(sch);
    }
}

void Node::resetVisited() {
    m_visitedForward = false;
    m_visitedBackward = false;
    QPtrListIterator<Node> it = m_nodes;
    for (; it.current(); ++it) {
        it.current()->resetVisited();
    }
}

Node *Node::siblingBefore() {
    //kdDebug()<<k_funcinfo<<endl;
    if (getParent())
        return getParent()->childBefore(this);
    return 0;
}

Node *Node::childBefore(Node *node) {
    //kdDebug()<<k_funcinfo<<endl;
    int index = m_nodes.findRef(node);
    if (index > 0){
        return m_nodes.at(index-1);
    }
    return 0;
}

Node *Node::siblingAfter() {
    //kdDebug()<<k_funcinfo<<endl;
    if (getParent())
        return getParent()->childAfter(this);
    return 0;
}

Node *Node::childAfter(Node *node)
{
    //kdDebug()<<k_funcinfo<<endl;
    uint index = m_nodes.findRef(node);
    if (index < m_nodes.count()-1) {
        return m_nodes.at(index+1);    }
    return 0;
}

bool Node::moveChildUp(Node* node)
{
    if (findChildNode(node) == -1)
        return false; // not my node!
    Node *sib = node->siblingBefore();
    if (!sib)
        return false;
    sib = sib->siblingBefore();
    delChildNode(node, false);
    if (sib) {
        addChildNode(node, sib);
    } else {
        insertChildNode(0, node);
    }        
    return true;
}

bool Node::moveChildDown(Node* node)
{
    if (findChildNode(node) == -1)
        return false; // not my node!
    Node *sib = node->siblingAfter();
    if (!sib)
        return false;
    delChildNode(node, false);
    addChildNode(node, sib);
    return true;
}

bool Node::legalToLink(Node *node) {
    Node *p = projectNode();
    if (p)
        return p->legalToLink(this, node);
    return false;
}

bool Node::isEndNode() const {
    return m_dependChildNodes.isEmpty();
}
bool Node::isStartNode() const {
    return m_dependParentNodes.isEmpty();
}

bool Node::setId(QString id) {
    //kdDebug()<<k_funcinfo<<id<<endl;
    if (id.isEmpty()) {
        kdError()<<k_funcinfo<<"id is empty"<<endl;
        m_id = id;
        return false;
    }
    if (!m_id.isEmpty()) {
        Node *n = findNode();
        if (n == this) {
            //kdDebug()<<k_funcinfo<<"My id found, remove it"<<endl;
            removeId();
        } else if (n) {
            //Hmmm, shouldn't happen
            kdError()<<k_funcinfo<<"My id '"<<m_id<<"' already used for different node: "<<n->name()<<endl;
        }
    }
    if (findNode(id)) {
        kdError()<<k_funcinfo<<"id '"<<id<<"' is already used for different node: "<<findNode(id)->name()<<endl;
        m_id = QString(); // hmmm
        return false;
    }
    m_id = id;
    insertId(id);
    //kdDebug()<<k_funcinfo<<m_name<<": inserted id="<<id<<endl;
    return true;
}

void Node::setStartTime(DateTime startTime) { 
    if (m_currentSchedule)
        m_currentSchedule->startTime = startTime;
    m_dateOnlyStartDate = startTime.date();
}

void Node::setEndTime(DateTime endTime) { 
    if (m_currentSchedule)
        m_currentSchedule->endTime = endTime;
    
    m_dateOnlyEndDate = endTime.date();
    if (endTime.time().isNull() && m_dateOnlyEndDate > m_dateOnlyStartDate)
        m_dateOnlyEndDate = m_dateOnlyEndDate.addDays(-1);
}

void Node::saveAppointments(QDomElement &element, long id) const {
    //kdDebug()<<k_funcinfo<<m_name<<" id="<<id<<endl;
    QPtrListIterator<Node> it(m_nodes);
    for (; it.current(); ++it ) {
        it.current()->saveAppointments(element, id);
    }
}

QPtrList<Appointment> Node::appointments() {
    QPtrList<Appointment> lst;
    if (m_currentSchedule)
        lst = m_currentSchedule->appointments();
    return lst;
}

// Appointment *Node::findAppointment(Resource *resource) {
//     if (m_currentSchedule)
//         return m_currentSchedule->findAppointment(resource);
//     return 0;
// }
bool Node::addAppointment(Appointment *appointment) {
    if (m_currentSchedule)
        return m_currentSchedule->add(appointment);
    return false;
}

bool Node::addAppointment(Appointment *appointment, Schedule &main) {
    //kdDebug()<<k_funcinfo<<this<<endl;
    Schedule *s = findSchedule(main.id());
    if (s == 0) {
        s = createSchedule(&main);
    }
    appointment->setNode(s);
    return s->add(appointment);
}

void Node::addAppointment(ResourceSchedule *resource, DateTime &start, DateTime &end, double load) {
    Schedule *node = findSchedule(resource->id());
    if (node == 0) {
        node = createSchedule(resource->parent());
    }
    node->addAppointment(resource, start, end, load);
}

void Node::takeSchedule(const Schedule *schedule) {
    if (schedule == 0)
        return;
    if (m_currentSchedule == schedule)
        m_currentSchedule = 0;
    m_schedules.take(schedule->id());
}

void Node::addSchedule(Schedule *schedule) {
    if (schedule == 0)
        return;
    m_schedules.replace(schedule->id(), schedule);
}

Schedule *Node::createSchedule(QString name, Schedule::Type type, long id) {
    //kdDebug()<<k_funcinfo<<name<<" type="<<type<<" id="<<(int)id<<endl;
    NodeSchedule *sch = new NodeSchedule(this, name, type, id);
    addSchedule(sch);
    return sch;
}

Schedule *Node::createSchedule(Schedule *parent) {
    //kdDebug()<<k_funcinfo<<name<<" type="<<type<<" id="<<(int)id<<endl;
    NodeSchedule *sch = new NodeSchedule(parent, this);
    addSchedule(sch);
    return sch;
}

Schedule *Node::findSchedule(const QString name, const Schedule::Type type) const {
    QIntDictIterator<Schedule> it = m_schedules;
    for (; it.current(); ++it) {
        if (!it.current()->isDeleted() && 
            it.current()->name() == name && it.current()->type() == type)
            return it.current();
    }
    return 0;
}

Schedule *Node::findSchedule(const Schedule::Type type) const {
    //kdDebug()<<k_funcinfo<<m_name<<" find type="<<type<<" nr="<<m_schedules.count()<<endl;
    QIntDictIterator<Schedule> it = m_schedules;
    for (; it.current(); ++it) {
        if (!it.current()->isDeleted() && it.current()->type() == type) {
            return it.current();
        }
    }
    return 0;
}

void Node::setScheduleDeleted(long id, bool on) {
    Schedule *ns = findSchedule(id);
    if (ns == 0) {
        kdError()<<k_funcinfo<<m_name<<" Could not find schedule with id="<<id<<endl;
    } else {
        ns->setDeleted(on);
    }
}

void Node::setParentSchedule(Schedule *sch) {
    Schedule *s = findSchedule(sch->id());
    if (s) {
        s->setParent(sch);
    }
    QPtrListIterator<Node> it = m_nodes;
    for (; it.current(); ++it) {
        it.current()->setParentSchedule(sch);
    }
}

bool Node::calcCriticalPath(bool fromEnd) {
    if (m_currentSchedule == 0)
        return false;
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (!isCritical()) {
        return false;
    }
    if (!fromEnd && isStartNode()) {
        m_currentSchedule->inCriticalPath = true;
        return true;
    }
    if (fromEnd && isEndNode()) {
        m_currentSchedule->inCriticalPath = true;
        return true;
    }
    QPtrListIterator<Relation> pit(m_dependParentNodes);
    for (; pit.current(); ++pit) {
        if (pit.current()->parent()->calcCriticalPath(fromEnd)) {
            m_currentSchedule->inCriticalPath = true;
        }
    }
    return m_currentSchedule->inCriticalPath;
}

int Node::level() {
    Node *n = getParent();
    return n ? n->level() + 1 : 0;
}

void Node::generateWBS(int count, WBSDefinition &def, QString wbs) {
    m_wbs = wbs + def.code(count, level());
    //kdDebug()<<k_funcinfo<<m_name<<" wbs: "<<m_wbs<<endl;
    QString w = wbs + def.wbs(count, level());
    QPtrListIterator<Node> it = m_nodes;
    for (int i=0; it.current(); ++it) {
        it.current()->generateWBS(++i, def, w);
    }

}

void Node::setCurrentSchedule(long id) {
    QPtrListIterator<Node> it = m_nodes;
    for (; it.current(); ++it) {
        it.current()->setCurrentSchedule(id);
    }
    //kdDebug()<<k_funcinfo<<m_name<<" id: "<<id<<"="<<m_currentSchedule<<endl;
}
//////////////////////////   Effort   /////////////////////////////////

Effort::Effort( Duration e, Duration p, Duration o) {
  m_expectedEffort = e;
  m_pessimisticEffort = p;
  m_optimisticEffort = o;
  m_type = Type_Effort;
  m_risktype = Risk_None;
}

Effort::Effort(const Effort &effort) {
    set(effort.expected(), effort.pessimistic(), effort.optimistic());
    setType(effort.type());
    setRisktype(effort.risktype());
}

Effort::~Effort() {
}

const Effort Effort::zeroEffort( Duration::zeroDuration,
                       Duration::zeroDuration,
                       Duration::zeroDuration );

void Effort::set( Duration e, Duration p, Duration o ) {
    m_expectedEffort = e;
    m_pessimisticEffort = (p == Duration::zeroDuration) ? e :  p;
    m_optimisticEffort = (o == Duration::zeroDuration) ? e :  o;
    //kdDebug()<<k_funcinfo<<"   Expected: "<<m_expectedEffort.toString()<<endl;
}

void Effort::set( int e, int p, int o ) {
    m_expectedEffort = Duration(e);
    m_pessimisticEffort = (p < 0) ? Duration(e) :  Duration(p);
    m_optimisticEffort = (o < 0) ? Duration(e) :  Duration(o);
    //kdDebug()<<k_funcinfo<<"   Expected: "<<m_expectedEffort.toString()<<endl;
    //kdDebug()<<k_funcinfo<<"   Optimistic: "<<m_optimisticEffort.toString()<<endl;
    //kdDebug()<<k_funcinfo<<"   Pessimistic: "<<m_pessimisticEffort.toString()<<endl;

    //kdDebug()<<k_funcinfo<<"   Expected: "<<m_expectedEffort.duration()<<" manseconds"<<endl;
}

//TODO (?): effort is not really a duration, should maybe not use Duration for storage
void Effort::set(unsigned days, unsigned hours, unsigned minutes) {
    Duration dur(days, hours, minutes);
    set(dur);
    //kdDebug()<<k_funcinfo<<"effort="<<dur.toString()<<endl;
}

void Effort::expectedEffort(unsigned *days, unsigned *hours, unsigned *minutes) {
    m_expectedEffort.get(days, hours, minutes);
}

Duration Effort::variance() const {
    return (m_pessimisticEffort - m_optimisticEffort)/6;
}
Duration Effort::pertExpected() const {
    if (m_risktype == Risk_Low) {
        return (m_optimisticEffort + m_pessimisticEffort + (m_expectedEffort*4))/6;
    } else if (m_risktype == Risk_High) {
        return (m_optimisticEffort + (m_pessimisticEffort*2) + (m_expectedEffort*4))/7;
    }
    return m_expectedEffort; // risk==none
}
Duration Effort::pertOptimistic() const {
    if (m_risktype != Risk_None) {
        return pertExpected() - variance();
    }
    return m_optimisticEffort;
}
Duration Effort::pertPessimistic() const {
    if (m_risktype != Risk_None) {
        return pertExpected() + variance();
    }
    return m_pessimisticEffort;
}

Duration Effort::effort(int use) const {
    if (use == Effort::Use_Expected) {
        return pertExpected();
    } else if (use == Effort::Use_Optimistic) {
        return pertOptimistic();
    } else if (use == Effort::Use_Pessimistic)
        return pertPessimistic();
    
    return m_expectedEffort; // default
}

bool Effort::load(QDomElement &element) {
    m_expectedEffort = Duration::fromString(element.attribute("expected"));
    m_optimisticEffort = Duration::fromString(element.attribute("optimistic"));
    m_pessimisticEffort = Duration::fromString(element.attribute("pessimistic"));
    setType(element.attribute("type", "WorkBased"));
    setRisktype(element.attribute("risk"));
    return true;
}

void Effort::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("effort");
    element.appendChild(me);
    me.setAttribute("expected", m_expectedEffort.toString());
    me.setAttribute("optimistic", m_optimisticEffort.toString());
    me.setAttribute("pessimistic", m_pessimisticEffort.toString());
    me.setAttribute("type", typeToString());
    me.setAttribute("risk", risktypeToString());
}

QString Effort::typeToString() const {
    if (m_type == Type_Effort)
        return QString("Effort");
    if (m_type == Type_FixedDuration)
        return QString("Type_FixedDuration");

    return QString();
}

void Effort::setType(QString type) {
    if (type == "Effort")
        setType(Type_Effort);
    else if (type == "Type_FixedDuration")
        setType(Type_FixedDuration);
    else
        setType(Type_Effort); // default
}

QString Effort::risktypeToString() const {
    if (m_risktype == Risk_None)
        return QString("None");
    if (m_risktype == Risk_Low)
        return QString("Low");
    if (m_risktype == Risk_High)
        return QString("High");

    return QString();
}

void Effort::setRisktype(QString type) {
    if (type == "High")
        setRisktype(Risk_High);
    else if (type == "Low")
        setRisktype(Risk_Low);
    else
        setRisktype(Risk_None); // default
}

void Effort::setOptimisticRatio(int percent)
{
    int p = percent>0 ? -percent : percent;
    m_optimisticEffort = m_expectedEffort*(100+p)/100;
}

int Effort::optimisticRatio() const {
    if (m_expectedEffort == Duration::zeroDuration)
        return 0;
    return (m_optimisticEffort.milliseconds()*100/m_expectedEffort.milliseconds())-100;
}

void Effort::setPessimisticRatio(int percent) 
{
    int p = percent<0 ? -percent : percent;
    m_pessimisticEffort = m_expectedEffort*(100+p)/100;
}
int Effort::pessimisticRatio() const {
    if (m_expectedEffort == Duration::zeroDuration)
        return 0;
    return m_pessimisticEffort.milliseconds()*100/m_expectedEffort.milliseconds()-100;
}

// Debugging
#ifndef NDEBUG
void Node::printDebug(bool children, QCString indent) {
    kdDebug()<<indent<<"  Unique node identity="<<m_id<<endl;
    if (m_effort) m_effort->printDebug(indent);
    QString s = "  Constraint: " + constraintToString();
    if (m_constraint == MustStartOn || m_constraint == StartNotEarlier || m_constraint == FixedInterval)
        kdDebug()<<indent<<s<<" ("<<constraintStartTime().toString()<<")"<<endl;
    if (m_constraint == MustFinishOn || m_constraint == FinishNotLater || m_constraint == FixedInterval)
        kdDebug()<<indent<<s<<" ("<<constraintEndTime().toString()<<")"<<endl;
    Schedule *cs = m_currentSchedule; 
    if (cs) {
        kdDebug()<<indent<<"  Current schedule: "<<"id="<<cs->id()<<" '"<<cs->name()<<"' type: "<<cs->type()<<endl;
    } else {
        kdDebug()<<indent<<"  Current schedule: None"<<endl;
    }
    QIntDictIterator<Schedule> it = m_schedules;
    for (; it.current(); ++it) {
        it.current()->printDebug(indent+"  ");
    }
    kdDebug()<<indent<<"  Parent: "<<(m_parent ? m_parent->name() : QString("None"))<<endl;
    kdDebug()<<indent<<"  Level: "<<level()<<endl;
    kdDebug()<<indent<<"  No of predecessors: "<<m_dependParentNodes.count()<<endl;
    QPtrListIterator<Relation> pit(m_dependParentNodes);
    //kdDebug()<<indent<<"  Dependant parents="<<pit.count()<<endl;
    if (pit.count() > 0) {
        for ( ; pit.current(); ++pit ) {
            pit.current()->printDebug(indent);
        }
    }
    kdDebug()<<indent<<"  No of successors: "<<m_dependChildNodes.count()<<endl;
    QPtrListIterator<Relation> cit(m_dependChildNodes);
    //kdDebug()<<indent<<"  Dependant children="<<cit.count()<<endl;
    if (cit.count() > 0) {
        for ( ; cit.current(); ++cit ) {
            cit.current()->printDebug(indent);
        }
    }

    //kdDebug()<<indent<<endl;
    indent += "  ";
    if (children) {
        QPtrListIterator<Node> it(m_nodes);
        for ( ; it.current(); ++it ) {
            it.current()->printDebug(true,indent);
        }
    }

}
#endif


#ifndef NDEBUG
void Effort::printDebug(QCString indent) {
    kdDebug()<<indent<<"  Effort:"<<endl;
    indent += "  ";
    kdDebug()<<indent<<"  Expected:    "<<m_expectedEffort.toString()<<endl;
    kdDebug()<<indent<<"  Optimistic:  "<<m_optimisticEffort.toString()<<endl;
    kdDebug()<<indent<<"  Pessimistic: "<<m_pessimisticEffort.toString()<<endl;
    
    kdDebug()<<indent<<"  Risk: "<<risktypeToString()<<endl;
    kdDebug()<<indent<<"  Pert expected:    "<<pertExpected().toString()<<endl;
    kdDebug()<<indent<<"  Pert optimistic:  "<<pertOptimistic().toString()<<endl;
    kdDebug()<<indent<<"  Pert pessimistic: "<<pertPessimistic().toString()<<endl;
    kdDebug()<<indent<<"  Pert variance:    "<<variance().toString()<<endl;
}
#endif

}  //KPlato namespace
