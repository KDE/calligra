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

#include "kptaccount.h"
#include "kptwbsdefinition.h"
#include "kptresource.h"

#include <qptrlist.h>
#include <qdom.h>

#include <kdebug.h>

namespace KPlato
{

Node::Node(Node *parent) : m_nodes(), m_dependChildNodes(), m_dependParentNodes() {
    m_parent = parent;
    init();
    m_id = QString(); // Not mapped
}

Node::Node(Node &node, Node *parent) 
    : m_nodes(), 
      m_dependChildNodes(), 
      m_dependParentNodes() {
    m_parent = parent;
    init();
    m_name = node.name();
    m_leader = node.leader();
    m_startTime = node.startTime();
    m_endTime = node.endTime();
    m_duration = node.duration();
    earliestStart = node.earliestStart;
    latestFinish = node.latestFinish;
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
}

Node::~Node() {
    Relation *rel = 0;
    while ((rel = m_dependParentNodes.getFirst())) {
        delete rel;
    }
    while ((rel = m_dependChildNodes.getFirst())) {
        delete rel;
    }
    Appointment *a;
    while ((a = m_appointments.getFirst())) {
        delete a;
    }
    if (m_runningAccount)
        m_runningAccount->removeRunning(*this);
    if (m_startupAccount)
        m_startupAccount->removeStartup(*this);
    if (m_shutdownAccount)
        m_shutdownAccount->removeShutdown(*this);
}

void Node::init() {
    m_nodes.setAutoDelete(true);
    m_name="";
    m_startTime = DateTime::currentDateTime();
    m_endTime = m_startTime.addDays(1);
    earliestStart = m_startTime;
    latestFinish = m_endTime;
    m_duration = m_endTime - m_startTime;
    m_constraint = Node::ASAP;
    m_effort = 0;
    m_resourceOverbooked = false;
    m_resourceError = false;
    m_resourceNotAvailable = false;
    m_schedulingError = false;
    m_notScheduled = true;
    m_visitedForward = false;
    m_visitedBackward = false;
    m_inCriticalPath = false;
    
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
    if (!m_effort) {
        kdError()<<k_funcinfo<<"m_effort = 0"<<endl;
        return Duration::zeroDuration;
    }
    Duration effort = m_effort->effort(use);
    if (effort == Duration::zeroDuration) {
        return Duration::zeroDuration;
    }
    Duration dur = effort; // use effort as default duration
    if (m_effort->type() == Effort::Type_Effort ||
        m_effort->type() == Effort::Type_FixedDuration) {
        dur = calcDuration(time, effort, backward);
        if (dur == Duration::zeroDuration) {
            kdWarning()<<k_funcinfo<<"zero duration: Resource not available"<<endl;
            m_resourceNotAvailable = true;
            return effort;
        }
    } else {
        // error
        kdError()<<k_funcinfo<<"Unsupported effort type: "<<m_effort->type()<<endl;
    }
    // TODO: handle risc

    return dur;
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

void Node::saveRelations(QDomElement &element) {
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
    earliestStart = time;
    //kdDebug()<<k_funcinfo<<m_name<<": "<<earliestStart.toString()<<endl;
    QPtrListIterator<Node> it = m_nodes;
    for (; it.current(); ++it) {
        it.current()->propagateEarliestStart(time);
    }
}

void Node::propagateLatestFinish(DateTime &time) {
    latestFinish = time;
    //kdDebug()<<k_funcinfo<<m_name<<": "<<latestFinish.toString()<<endl;
    QPtrListIterator<Node> it = m_nodes;
    for (; it.current(); ++it) {
        it.current()->propagateLatestFinish(time);
    }
}

void Node::moveEarliestStart(DateTime &time) {
    if (earliestStart < time)
        earliestStart = time;
    QPtrListIterator<Node> it = m_nodes;
    for (; it.current(); ++it) {
        it.current()->moveEarliestStart(time);
    }
}

void Node::moveLatestFinish(DateTime &time) {
    if (latestFinish > time)
        latestFinish = time;
    QPtrListIterator<Node> it = m_nodes;
    for (; it.current(); ++it) {
        it.current()->moveLatestFinish(time);
    }
}

void Node::initiateCalculation() {
    m_visitedForward = false;
    m_visitedBackward = false;
    m_resourceError = false;
    m_resourceOverbooked = false;
    m_schedulingError = false;
    m_inCriticalPath = false;
    clearProxyRelations();
    QPtrListIterator<Node> it = m_nodes;
    for (; it.current(); ++it) {
        it.current()->initiateCalculation();
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
    Node *n = findNode();
    if (n == this) {
        //kdDebug()<<k_funcinfo<<"My id found, remove it"<<endl;
        removeId();
    } else if (n) {
        //Hmmm, shouldn't happen
        kdError()<<k_funcinfo<<"My id '"<<m_id<<"' already used for different node: "<<n->name()<<endl;
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
    m_startTime = startTime; 
    m_dateOnlyStartDate = startTime.date();
}

void Node::setEndTime(DateTime endTime) { 
    m_endTime = endTime; 
    m_dateOnlyEndDate = endTime.date();
    if (endTime.time().isNull() && m_dateOnlyEndDate > m_dateOnlyStartDate)
        m_dateOnlyEndDate = m_dateOnlyEndDate.addDays(-1);
}

Appointment *Node::findAppointment(Resource *resource) {
    QPtrListIterator<Appointment> it = m_appointments;
    for (; it.current(); ++it) {
        if (it.current()->resource() == resource)
            return it.current();
    }
    return 0;
}

bool Node::addAppointment(Appointment *appointment) {
    if (m_appointments.findRef(appointment) != -1) {
        kdError()<<k_funcinfo<<"Appointment allready exists"<<endl;
        return false;
    }
    m_appointments.append(appointment);
    return true;
        
}

void Node::addAppointment(Resource *resource, DateTime &start, DateTime &end, double load) {
    Appointment *a = findAppointment(resource);
    if (a != 0) {
        a->addInterval(start, end, load);
        return;
    }
    a = new Appointment(resource, this, start, end, load);
    if (resource->addAppointment(a)) {
        m_appointments.append(a);
    } else {
        delete a;
    }
}

void Node::removeAppointment(Appointment *appointment) {
    takeAppointment(appointment);
    delete appointment;
}

void Node::takeAppointment(Appointment *appointment) {
    int i = m_appointments.findRef(appointment);
    if (i != -1) {
        m_appointments.take(i);
        //kdDebug()<<k_funcinfo<<"Taken: "<<appointment<<endl;
        if (appointment->resource())
            appointment->resource()->takeAppointment(appointment);
    } else {
        //kdDebug()<<k_funcinfo<<"Couldn't find appointment: "<<appointment<<endl;
    }
}


bool Node::calcCriticalPath() {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (!isCritical()) {
        return false;
    }
    if (isStartNode()) {
        m_inCriticalPath = true;
        return true;
    }
    QPtrListIterator<Relation> pit(m_dependParentNodes);
    for (; pit.current(); ++pit) {
        if (pit.current()->parent()->calcCriticalPath()) {
            m_inCriticalPath = true;
        }
    }
    return m_inCriticalPath;
}

int Node::level() {
    Node *n = getParent();
    return n ? n->level() + 1 : 0;
}

void Node::generateWBS(int count, WBSDefinition &def, QString wbs) {
    m_wbs = wbs + def.code(count, level());
    kdDebug()<<k_funcinfo<<m_name<<" wbs: "<<m_wbs<<endl;
    QString w = wbs + def.wbs(count, level());
    QPtrListIterator<Node> it = m_nodes;
    for (int i=0; it.current(); ++it) {
        it.current()->generateWBS(++i, def, w);
    }

}

//////////////////////////   Effort   /////////////////////////////////

Effort::Effort( Duration e, Duration p, Duration o) {
  m_expectedEffort = e;
  m_pessimisticEffort = p;
  m_optimisticEffort = o;
  m_type = Type_Effort;
}

Effort::Effort(const Effort &effort) {
    set(effort.expected(), effort.pessimistic(), effort.optimistic());
    setType(effort.type());
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

bool Effort::load(QDomElement &element) {
    m_expectedEffort = Duration::fromString(element.attribute("expected"));
    m_optimisticEffort = Duration::fromString(element.attribute("optimistic"));
    m_pessimisticEffort = Duration::fromString(element.attribute("pessimistic"));
    setType(element.attribute("type", "WorkBased"));
    return true;
}

void Effort::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("effort");
    element.appendChild(me);
    me.setAttribute("expected", m_expectedEffort.toString());
    me.setAttribute("optimistic", m_optimisticEffort.toString());
    me.setAttribute("pessimistic", m_pessimisticEffort.toString());
    me.setAttribute("type", typeToString());
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
    kdDebug()<<indent<<s<<" ("<<constraintStartTime().toString()<<")"<<endl;
    kdDebug()<<indent<<s<<" ("<<constraintEndTime().toString()<<")"<<endl;
    //kdDebug()<<indent<<"  Duration: "<<m_duration.toString()<<endl;
    kdDebug()<<indent<<"  Duration: "<<m_duration.seconds()<<QCString(" secs")<<" ("<<m_duration.toString()<<")"<<endl;
    kdDebug()<<indent<<"  Start time: "<<m_startTime.toString()<<endl;
    kdDebug()<<indent<<"  End time: " <<m_endTime.toString()<<endl;
    kdDebug()<<indent<<"  Earliest start: "<<earliestStart.toString()<<endl;
    kdDebug()<<indent<<"  Latest finish: " <<latestFinish.toString()<<endl;
    kdDebug()<<indent<<"  Parent: "<<(m_parent ? m_parent->name() : QString("None"))<<endl;
    kdDebug()<<indent<<"  Level: "<<level()<<endl;
//    kdDebug()<<indent<<"  Predecessors="<<start_node()->predecessors.number<<" unvisited="<<start_node()->predecessors.unvisited<<endl;
    //kdDebug()<<indent<<"  Successors="<<start_node()->successors.number<<" unvisited="<<start_node()->successors.unvisited<<endl;


    QPtrListIterator<Relation> pit(m_dependParentNodes);
    //kdDebug()<<indent<<"  Dependant parents="<<pit.count()<<endl;
    if (pit.count() > 0) {
        for ( ; pit.current(); ++pit ) {
            pit.current()->printDebug(indent);
        }
    }

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
    kdDebug()<<indent<<"  Effort: "<<m_expectedEffort.toString()<<endl;
    indent += "  ";
    kdDebug()<<indent<<"  Expected: "<<m_expectedEffort.toString()<<endl;
    kdDebug()<<indent<<"  Optimistic: "<<m_optimisticEffort.toString()<<endl;
    kdDebug()<<indent<<"  Pessimistic: "<<m_pessimisticEffort.toString()<<endl;
}
#endif

}  //KPlato namespace
