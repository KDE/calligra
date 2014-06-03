/* This file is part of the KDE project
   Copyright (C) 2001 Thomas zander <zander@kde.org>
   Copyright (C) 2002 - 2010, 2012 Dag Andersen <danders@get2net.dk>

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
#include "kptxmlloaderobject.h"
#include "kptdebug.h"

#include <QList>
#include <QListIterator>
#include <QDomDocument>

#include <kglobal.h>
#include <klocale.h>


namespace KPlato
{

Node::Node(Node *parent) 
    : QObject( 0 ), // We don't use qobjects parent
      m_nodes(), m_dependChildNodes(), m_dependParentNodes(),
      m_estimate( 0 )
{
    //kDebug(planDbg())<<"("<<this<<")";
    m_parent = parent;
    init();
    m_id.clear(); // Not mapped
}

Node::Node(const Node &node, Node *parent) 
    : QObject( 0 ), // Don't set parent, we handle parent/child ourselves
      m_nodes(), 
      m_dependChildNodes(), 
      m_dependParentNodes(),
      m_estimate( 0 )
{
    //kDebug(planDbg())<<"("<<this<<")";
    m_parent = parent;
    init();
    m_name = node.name();
    m_leader = node.leader();
    m_description = node.description();
    m_constraint = (ConstraintType) node.constraint();
    m_constraintStartTime = node.constraintStartTime();
    m_constraintEndTime = node.constraintEndTime();
    
    m_runningAccount = node.runningAccount();
    m_startupAccount = node.startupAccount();
    m_shutdownAccount = node.shutdownAccount();

    m_startupCost = node.startupCost();
    m_shutdownCost = node.shutdownCost();
    
    setObjectName( node.objectName() );
}

Node::~Node() {
    //kDebug(planDbg())<<"("<<this<<")"<<m_name;
    delete m_estimate;
    while (!m_nodes.isEmpty())
        delete m_nodes.takeFirst();
    
    if (findNode() == this) {
        removeId(); // only remove myself (I may be just a working copy)
    }
    while (!m_dependParentNodes.isEmpty()) {
        delete m_dependParentNodes.takeFirst();
    }
    while (!m_dependChildNodes.isEmpty()) {
        delete m_dependChildNodes.takeFirst();
    }
    if (m_runningAccount)
        m_runningAccount->removeRunning(*this);
    if (m_startupAccount)
        m_startupAccount->removeStartup(*this);
    if (m_shutdownAccount)
        m_shutdownAccount->removeShutdown(*this);

    foreach (Schedule *s, m_schedules) {
        delete s;
    }
    m_schedules.clear();
    m_parent = 0; //safety
}

void Node::init() {
    m_documents.node = this;
    m_currentSchedule = 0;
    m_name="";
    m_constraint = Node::ASAP;
    m_estimate = 0;
    m_visitedForward = false;
    m_visitedBackward = false;
    
    m_runningAccount = 0;
    m_startupAccount = 0;
    m_shutdownAccount = 0;
    m_startupCost = 0.0;
    m_shutdownCost = 0.0;
}

QString Node::typeToString( bool trans ) const
{
    return typeToString( (Node::NodeTypes)type(), trans );
}

// static
QString Node::typeToString( Node::NodeTypes typ, bool trans )
{
    return typeToStringList( trans ).at( typ );
}

// static
QStringList Node::typeToStringList( bool trans )
{
    return QStringList() 
            << ( trans ? i18n( "None" ) : QString( "None" ) )
            << ( trans ? i18n( "Project" ) : QString( "Project" ) )
            << ( trans ? i18n( "Sub-Project" ) : QString( "Sub-Project" ) )
            << ( trans ? i18n( "Task" ) : QString( "Task" ) )
            << ( trans ? i18n( "Milestone" ) : QString( "Milestone" ) )
            << ( trans ? i18n( "Periodic" ) : QString( "Periodic" ) )
            << ( trans ? i18n( "Summary" ) : QString( "Summary-Task" ) );
}

void Node::setName(const QString &n) 
{
#ifndef NDEBUG
    setObjectName( n );
#endif
     m_name = n;
     changed(this);
}

void Node::setLeader(const QString &l)
{
     m_leader = l;
     changed(this);
}

void Node::setDescription(const QString &d)
{
     m_description = d;
     changed(this);
}

Node *Node::projectNode() {
    if ((type() == Type_Project) || (type() == Type_Subproject)) {
        return this;
    }
    if (m_parent)
        return m_parent->projectNode();

    // This happends for default tasks
    return 0;
}

const Node *Node::projectNode() const {
    if ((type() == Type_Project) || (type() == Type_Subproject)) {
        return this;
    }
    if (m_parent)
        return m_parent->projectNode();

    // This happends for default tasks
    return 0;
}

void Node::takeChildNode( Node *node) {
    //kDebug(planDbg())<<"find="<<m_nodes.indexOf(node);
    int t = type();
    int i = m_nodes.indexOf(node);
    if ( i != -1 ) {
        m_nodes.removeAt(i);
    }
    node->setParentNode(0);
    if ( t != type() ) {
        changed( Type );
    }
}

void Node::takeChildNode( int number ) {
    int t = type();
    if (number >= 0 && number < m_nodes.size()) {
        Node *n = m_nodes.takeAt(number);
        //kDebug(planDbg())<<(n?n->id():"null")<<" :"<<(n?n->name():"");
        if (n) {
            n->setParentNode( 0 );
        }
    }
    if ( t != type() ) {
        changed( Type );
    }
}

void Node::insertChildNode( int index, Node *node ) {
    int t = type();
    if (index == -1)
        m_nodes.append(node);
    else
        m_nodes.insert(index,node);
    node->setParentNode( this );
    if ( t != type() ) {
        changed( Type );
    }
}

void Node::addChildNode( Node *node, Node *after) {
    int t = type();
    int index = m_nodes.indexOf(after);
    if (index == -1) {
        m_nodes.append(node);
        node->setParentNode( this );
        if ( t != type() ) {
            changed( Type );
        }
        return;
    }
    m_nodes.insert(index+1, node);
    node->setParentNode(this);
    if ( t != type() ) {
        changed( Type );
    }
}

int Node::findChildNode( const Node* node ) const
{
    return m_nodes.indexOf( const_cast<Node*>( node ) );
}

bool Node::isChildOf( const Node* node ) const
{
    if ( node == 0 || m_parent == 0 ) {
        return false;
    }
    if ( node == m_parent ) {
        return true;
    }
    return m_parent->isChildOf( node );
}


Node* Node::childNode(int number)
{
    //kDebug(planDbg())<<number;
    return m_nodes.value( number );
}

const Node* Node::childNode(int number) const
{
    if ( number < 0 || number >= m_nodes.count() ) {
        return 0;
    }
    return m_nodes.at( number );
}

int Node::indexOf( const Node *node ) const
{
    return m_nodes.indexOf( const_cast<Node*>(node) );
}


Duration *Node::getDelay() {
    /* TODO
       Calculate the delay of this node. Use the calculated startTime and the set startTime.
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
    if(m_dependChildNodes.indexOf(relation) != -1)
        return false;
    m_dependChildNodes.append(relation);
    return true;
}

void Node::takeDependChildNode( Relation *rel ) {
    int i = m_dependChildNodes.indexOf(rel);
    if ( i != -1 ) {
        //kDebug(planDbg())<<m_name<<": ("<<rel<<")";
        m_dependChildNodes.removeAt(i);
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
    if(m_dependParentNodes.indexOf(relation) != -1)
        return false;
    m_dependParentNodes.append(relation);
    return true;
}

void Node::takeDependParentNode( Relation *rel ) {
    int i = m_dependParentNodes.indexOf(rel);
    if ( i != -1 ) {
        //kDebug(planDbg())<<m_name<<": ("<<rel<<")";
        m_dependParentNodes.removeAt(i);
    }
}

bool Node::isParentOf( const Node *node ) const
{
    if (m_nodes.indexOf( const_cast<Node*>( node ) ) != -1)
        return true;

    QListIterator<Node*> nit(childNodeIterator());
    while (nit.hasNext()) {
        if (nit.next()->isParentOf(node))
            return true;
    }
    return false;
}

Relation *Node::findParentRelation( const Node *node ) const
{
    for (int i=0; i<numDependParentNodes(); i++) {
        Relation *rel = getDependParentNode(i);
        if (rel->parent() == node)
            return rel;
    }
    return (Relation *)0;
}

Relation *Node::findChildRelation( const Node *node) const
{
    for (int i=0; i<numDependChildNodes(); i++) {
        Relation *rel = getDependChildNode(i);
        if (rel->child() == node)
            return rel;
    }
    return (Relation *)0;
}

Relation *Node::findRelation( const Node *node ) const
{
    Relation *rel = findParentRelation(node);
    if (!rel)
        rel = findChildRelation(node);
    return rel;
}

bool Node::isDependChildOf( const Node *node ) const
{
    //kDebug(planDbg())<<" '"<<m_name<<"' checking against '"<<node->name()<<"'";
    for (int i=0; i<numDependParentNodes(); i++) {
        Relation *rel = getDependParentNode(i);
        if (rel->parent() == node)
            return true;
        if (rel->parent()->isDependChildOf(node))
            return true;
    }
    return false;
}

QList<Node*> Node::getParentNodes()
{
    this->m_parentNodes.clear();
    foreach(Relation * currentRelation, this->dependParentNodes())
    {
        if (!this->m_parentNodes.contains(currentRelation->parent())) 
        {
            this->m_parentNodes.append(currentRelation->parent());
        }
    }
    return this->m_parentNodes;
}

bool Node::canMoveTo( const Node *newParent ) const
{
    if ( m_parent == newParent ) {
        return true;
    }
    if ( newParent->isChildOf( this ) ) {
        return false;
    }
    if ( isDependChildOf( newParent ) || newParent->isDependChildOf( this ) ) {
        kDebug(planDbg())<<"Can't move, node is dependent on new parent";
        return false;
    }
    foreach ( Node *n, m_nodes ) {
        if ( !n->canMoveTo( newParent ) ) {
            return false;
        }
    }
    return true;
}

void Node::makeAppointments() {
    QListIterator<Node*> nit(m_nodes);
    while (nit.hasNext()) {
        nit.next()->makeAppointments();
    }
}

void Node::calcResourceOverbooked() {
    QListIterator<Node*> nit(m_nodes);
    while (nit.hasNext()) {
        nit.next()->calcResourceOverbooked();
    }
}

// Returns the (previously) calculated duration
Duration Node::duration( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->duration : Duration::zeroDuration;
}

double Node::variance( long id, Duration::Unit unit ) const
{
    double d = deviation( id, unit );
    return d * d;
}

double Node::deviation( long id, Duration::Unit unit ) const
{
    Schedule *s = schedule( id );
    double d = 0.0;
    if ( s && m_estimate ) {
        d = s->duration.toDouble( unit );
        double o = ( d *  ( 100 + m_estimate->optimisticRatio() ) ) / 100;
        double p = ( d * ( 100 + m_estimate->pessimisticRatio() ) ) / 100;
        d =  ( p - o ) / 6;
    }
    return d;
}

DateTime Node::startTime( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->startTime : DateTime();
}
DateTime Node::endTime( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->endTime : DateTime();
}

DateTime Node::appointmentStartTime( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->appointmentStartTime() : DateTime();
}
DateTime Node::appointmentEndTime( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->appointmentEndTime() : DateTime();
}

void Node::setDuration(const Duration &duration, long id )
{
    Schedule *s = schedule( id );
    if ( s ) {
        s->duration = duration;
    }
}

void Node::setEarlyStart(const DateTime &dt, long id )
{
    Schedule *s = schedule( id );
    if ( s ) s->earlyStart = dt;
}

DateTime Node::earlyStart( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->earlyStart : DateTime();
}

void Node::setLateStart(const DateTime &dt, long id )
{
    Schedule *s = schedule( id );
    if ( s ) s->lateStart = dt;
}

DateTime Node::lateStart( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->lateStart : DateTime();
}
    
void Node::setEarlyFinish(const DateTime &dt, long id )
{
    Schedule *s = schedule( id );
    if ( s ) s->earlyFinish = dt;
}

DateTime Node::earlyFinish( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->earlyFinish : DateTime();
}

void Node::setLateFinish(const DateTime &dt, long id )
{
    Schedule *s = schedule( id );
    if ( s ) s->lateFinish = dt;
}

DateTime Node::lateFinish( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->lateFinish : DateTime();
}
    
DateTime Node::workStartTime( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->workStartTime : DateTime();
}

void Node::setWorkStartTime(const DateTime &dt, long id )
{
    Schedule *s = schedule( id );
    if ( s ) s->workStartTime = dt;
}

DateTime Node::workEndTime( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->workEndTime : DateTime();
}

void Node::setWorkEndTime(const DateTime &dt, long id )
{
    Schedule *s = schedule( id );
    if ( s ) s->workEndTime = dt;
}
    
bool Node::inCriticalPath( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->inCriticalPath : false;
}

QStringList Node::schedulingStatus( long id, bool trans ) const 
{
    Schedule *s = schedule( id );
    QStringList lst;
    if ( s ) {
        lst = s->state();
    }
    if ( lst.isEmpty() ) {
        lst.append( trans ? i18n( "Not scheduled" ) : QString( "Not scheduled" ) );
    }
    return lst;
}

bool Node::resourceError( long id ) const 
{
    Schedule *s = schedule( id );
    return s ? s->resourceError : false;
}
    
bool Node::resourceOverbooked( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->resourceOverbooked : false;
}
    
bool Node::resourceNotAvailable( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->resourceNotAvailable : false;
}

bool Node::constraintError( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->constraintError : false;
}

bool Node::schedulingError( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->schedulingError : false;
}

bool Node::notScheduled( long id ) const
{
    if ( type() == Type_Summarytask ) {
        // i am scheduled if al least on child is scheduled
        foreach ( Node *n, m_nodes ) {
            if ( ! n->notScheduled( id ) ) {
                return false;
            }
        }
        return true;
    }
    Schedule *s = schedule( id );
    return s == 0 || s->isDeleted() || s->notScheduled;
}

QStringList Node::overbookedResources( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->overbookedResources() : QStringList();
}

void Node::saveWorkPackageXML( QDomElement &, long ) const
{
    return;
}

void Node::saveRelations(QDomElement &element) const
{
    QListIterator<Relation*> it(m_dependChildNodes);
    while (it.hasNext()) {
        it.next()->save(element);
    }
    QListIterator<Node*> nodes(m_nodes);
    while (nodes.hasNext()) {
        nodes.next()->saveRelations(element);
    }
}

void Node::setConstraint(Node::ConstraintType type)
{ 
    m_constraint = type;
    changed( this );
}

void Node::setConstraint(QString &type) {
    // Do not i18n these, they are used in load()
    if (type == "ASAP")
        setConstraint(ASAP);
    else if (type == "ALAP")
        setConstraint(ALAP);
    else if (type == "MustStartOn")
        setConstraint(MustStartOn);
    else if (type == "MustFinishOn")
        setConstraint(MustFinishOn);
    else if (type == "StartNotEarlier")
        setConstraint(StartNotEarlier);
    else if (type == "FinishNotLater")
        setConstraint(FinishNotLater);
    else if (type == "FixedInterval")
        setConstraint(FixedInterval);
    else
        setConstraint(ASAP);  // default
}

QString Node::constraintToString( bool trans ) const {
    return constraintList( trans ).at( m_constraint );
}

QStringList Node::constraintList( bool trans ) {
    // keep theses in the same order as the enum!
    return QStringList() 
            << (trans ? i18n("As Soon As Possible") : QString("ASAP"))
            << (trans ? i18n("As Late As Possible") : QString("ALAP"))
            << (trans ? i18n("Must Start On") : QString("MustStartOn"))
            << (trans ? i18n("Must Finish On") : QString("MustFinishOn"))
            << (trans ? i18n("Start Not Earlier") : QString("StartNotEarlier"))
            << (trans ? i18n("Finish Not Later") : QString("FinishNotLater"))
            << (trans ? i18n("Fixed Interval") : QString("FixedInterval"));
}

void Node::propagateEarliestStart(DateTime &time) {
    if (m_currentSchedule == 0) {
        return;
    }
    if ( type() != Type_Project ) {
        m_currentSchedule->earlyStart = time;
        if ( m_currentSchedule->lateStart.isValid() && m_currentSchedule->lateStart < time ) {
            m_currentSchedule->lateStart = time;
        }
        //m_currentSchedule->logDebug( "propagateEarliestStart: " + time.toString() );
        switch ( m_constraint ) {
            case FinishNotLater:
            case MustFinishOn:
                if ( m_constraintEndTime < time ) {
                    m_currentSchedule->logWarning("Task constraint outside project constraint");
#ifndef PLAN_NLOGDEBUG
                    m_currentSchedule->logDebug( QString( "%1: end constraint %2 < %3" ).arg( constraintToString( true ) ).arg( m_constraintEndTime.toString() ).arg( time.toString() ) );
#endif
                }
                break;
            case MustStartOn:
            case FixedInterval:
                if ( m_constraintStartTime < time ) {
                    m_currentSchedule->logWarning("Task constraint outside project constraint");
#ifndef PLAN_NLOGDEBUG
                    m_currentSchedule->logDebug( QString( "%1: start constraint %2 < %3" ).arg( constraintToString( true ) ).arg( m_constraintEndTime.toString() ).arg( time.toString() ) );
#endif
                }
                break;
            default:
                break;
        }
    }
    //kDebug(planDbg())<<m_name<<":"<<m_currentSchedule->earlyStart;
    QListIterator<Node*> it = m_nodes;
    while (it.hasNext()) {
        it.next()->propagateEarliestStart(time);
    }
}

void Node::propagateLatestFinish(DateTime &time) {
    if (m_currentSchedule == 0) {
        return;
    }
    if ( type() != Type_Project ) {
        m_currentSchedule->lateFinish = time;
        if ( m_currentSchedule->earlyFinish.isValid() && m_currentSchedule->earlyFinish > time ) {
            m_currentSchedule->earlyFinish = time;
        }
        switch ( m_constraint ) {
            case StartNotEarlier:
            case MustStartOn:
                if ( m_constraintStartTime > time ) {
                    m_currentSchedule->logWarning("Task constraint outside project constraint");
#ifndef PLAN_NLOGDEBUG
                    m_currentSchedule->logDebug( QString( "%1: start constraint %2 < %3" ).arg( constraintToString( true ) ).arg( m_constraintEndTime.toString() ).arg( time.toString() ) );
#endif
                }
                break;
            case MustFinishOn:
            case FixedInterval:
                if ( m_constraintEndTime > time ) {
                    m_currentSchedule->logWarning("Task constraint outside project constraint");
#ifndef PLAN_NLOGDEBUG
                    m_currentSchedule->logDebug( QString( "%1: end constraint %2 > %3" ).arg( constraintToString( true ) ).arg( m_constraintEndTime.toString() ).arg( time.toString() ) );
#endif
                }
                break;
            default:
                break;
        }
    }
    //kDebug(planDbg())<<m_name<<":"<<m_currentSchedule->lateFinish;
    QListIterator<Node*> it = m_nodes;
    while (it.hasNext()) {
        it.next()->propagateLatestFinish(time);
    }
}

void Node::moveEarliestStart(DateTime &time) {
    if (m_currentSchedule == 0)
        return;
    if (m_currentSchedule->earlyStart < time) {
        //m_currentSchedule->logDebug( "moveEarliestStart: " + m_currentSchedule->earlyStart.toString() + " -> " + time.toString() );
        m_currentSchedule->earlyStart = time;
    }
    QListIterator<Node*> it = m_nodes;
    while (it.hasNext()) {
        it.next()->moveEarliestStart(time);
    }
}

void Node::moveLatestFinish(DateTime &time) {
    if (m_currentSchedule == 0)
        return;
    if (m_currentSchedule->lateFinish > time)
        m_currentSchedule->lateFinish = time;
    QListIterator<Node*> it = m_nodes;
    while (it.hasNext()) {
        it.next()->moveLatestFinish(time);
    }
}

void Node::initiateCalculation(MainSchedule &sch) {
    m_visitedForward = false;
    m_visitedBackward = false;
    m_durationForward = Duration::zeroDuration;
    m_durationBackward = Duration::zeroDuration;
    m_earlyStart = DateTime();
    m_earlyFinish = DateTime();
    m_lateFinish = DateTime();

    QListIterator<Node*> it = m_nodes;
    while (it.hasNext()) {
        it.next()->initiateCalculation(sch);
    }
}

void Node::resetVisited() {
    m_visitedForward = false;
    m_visitedBackward = false;
    QListIterator<Node*> it = m_nodes;
    while (it.hasNext()) {
        it.next()->resetVisited();
    }
}

Node *Node::siblingBefore() {
    //kDebug(planDbg());
    if (parentNode())
        return parentNode()->childBefore(this);
    return 0;
}

Node *Node::childBefore(Node *node) {
    //kDebug(planDbg());
    int index = m_nodes.indexOf(node);
    if (index > 0){
        return m_nodes.at(index-1);
    }
    return 0;
}

Node *Node::siblingAfter() {
    //kDebug(planDbg());
    if (parentNode())
        return parentNode()->childAfter(this);
    return 0;
}

Node *Node::childAfter(Node *node)
{
    //kDebug(planDbg());
    Q_ASSERT( m_nodes.contains( node ) );
    int index = m_nodes.indexOf(node);
    if (index < m_nodes.count()-1) {
        return m_nodes.at(index+1);
    }
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
    takeChildNode(node);
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
    takeChildNode(node);
    addChildNode(node, sib);
    return true;
}

bool Node::legalToLink( const Node *node ) const
{
    Node *p = const_cast<Node*>(this)->projectNode();
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

void Node::setId(const QString& id) {
    //kDebug(planDbg())<<id;
    m_id = id;
}

void Node::setStartTime(DateTime startTime, long id )
{ 
    Schedule *s = schedule( id );
    if ( s )
        s->startTime = startTime;
}

void Node::setEndTime(DateTime endTime, long id )
{
    Schedule *s = schedule( id );
    if ( s )
        s->endTime = endTime;
}

void Node::saveAppointments(QDomElement &element, long id) const {
    //kDebug(planDbg())<<m_name<<" id="<<id;
    QListIterator<Node*> it(m_nodes);
    while (it.hasNext()) {
        it.next()->saveAppointments(element, id);
    }
}

QList<Appointment*> Node::appointments( long id )
{
    Schedule *s = schedule( id );
    QList<Appointment*> lst;
    if ( s )
        lst = s->appointments();
    return lst;
}

QList<Resource*> Node::assignedResources( long id ) const {
    Schedule *s = schedule( id );
    QList<Resource*> res;
    if ( s ) {
        foreach ( Appointment *a, s->appointments() ) {
            res << a->resource()->resource();
        }
    }
    return res;
}


// Appointment *Node::findAppointment(Resource *resource) {
//     if (m_currentSchedule)
//         return m_currentSchedule->findAppointment(resource);
//     return 0;
// }
// bool Node::addAppointment(Appointment *appointment) {
//     if ( m_currentSchedule )
//         return m_currentSchedule->add(appointment);
//     return false;
// }
// 
// called from resource side when resource adds appointment
bool Node::addAppointment(Appointment *appointment, Schedule &main) {
    Schedule *s = findSchedule(main.id());
    if (s == 0) {
        s = createSchedule(&main);
    }
    appointment->setNode(s);
    //kDebug(planDbg())<<this<<":"<<appointment<<","<<s<<","<<s->id()<<","<<main.id();
    return s->add(appointment);
}

void Node::addAppointment(ResourceSchedule *resource, DateTime &start, DateTime &end, double load) {
    Schedule *node = findSchedule(resource->id());
    if (node == 0) {
        node = createSchedule(resource->parent());
    }
    node->setCalculationMode( resource->calculationMode() );
    node->addAppointment(resource, start, end, load);
}

bool Node::isBaselined( long id ) const
{
    Schedule *s = schedule( id );
    return s ? s->isBaselined() : false;
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
    m_schedules.insert(schedule->id(), schedule);
}

Schedule *Node::createSchedule(const QString& name, Schedule::Type type, long id) {
    //kDebug(planDbg())<<name<<" type="<<type<<" id="<<(int)id;
    NodeSchedule *sch = new NodeSchedule(this, name, type, id);
    addSchedule(sch);
    return sch;
}

Schedule *Node::createSchedule(Schedule *parent) {
    //kDebug(planDbg())<<name<<" type="<<type<<" id="<<(int)id;
    NodeSchedule *sch = new NodeSchedule(parent, this);
    addSchedule(sch);
    return sch;
}

Schedule *Node::schedule( long id ) const
{
    switch ( id ) {
        case ANYSCHEDULED: {
            foreach ( Schedule *s, m_schedules ) {
                if ( s->isScheduled() ) {
                    return s;
                }
            }
            return 0;
        }
        case CURRENTSCHEDULE:
            return m_currentSchedule;
        case NOTSCHEDULED:
            return 0;
        case BASELINESCHEDULE: {
            foreach ( Schedule *s, m_schedules ) {
                if ( s->isBaselined() ) {
                    return s;
                }
            }
            return 0;
        }
        default:
            break;
    }
    return findSchedule( id );
}

Schedule *Node::findSchedule( long id ) const
{
    return m_schedules.value( id );
}

Schedule *Node::findSchedule(const QString name, const Schedule::Type type) {
    QHash<long, Schedule*> it;
    foreach (Schedule *sch, it) {
        if (!sch->isDeleted() && 
            sch->name() == name && sch->type() == type)
            return sch;
    }
    return 0;
}

Schedule *Node::findSchedule(const QString name) {
    foreach (Schedule *sch, m_schedules) {
        if (!sch->isDeleted() && sch->name() == name)
            return sch;
    }
    return 0;
}


Schedule *Node::findSchedule(const Schedule::Type type) {
    //kDebug(planDbg())<<m_name<<" find type="<<type<<" nr="<<m_schedules.count();
    QHash<long, Schedule*> hash;
    foreach (Schedule *sch, hash) {
        if (!sch->isDeleted() && sch->type() == type) {
            return sch;
        }
    }
    return 0;
}

void Node::setScheduleDeleted(long id, bool on) {
    Schedule *ns = findSchedule(id);
    if (ns == 0) {
        kError()<<m_name<<" Could not find schedule with id="<<id;
    } else {
        ns->setDeleted(on);
    }
}

void Node::setParentSchedule(Schedule *sch) {
    Schedule *s = findSchedule(sch->id());
    if (s) {
        s->setParent(sch);
    }
    QListIterator<Node*> it = m_nodes;
    while (it.hasNext()) {
        it.next()->setParentSchedule(sch);
    }
}

bool Node::calcCriticalPath(bool fromEnd) {
    if (m_currentSchedule == 0)
        return false;
    //kDebug(planDbg())<<m_name;
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
    QListIterator<Relation*> pit(m_dependParentNodes);
    while (pit.hasNext()) {
        if (pit.next()->parent()->calcCriticalPath(fromEnd)) {
            m_currentSchedule->inCriticalPath = true;
        }
    }
    return m_currentSchedule->inCriticalPath;
}

void Node::calcFreeFloat() {
    foreach ( Node *n, m_nodes ) {
        n->calcFreeFloat();
    }
    return;
}

int Node::level() const {
    const Node *n = parentNode();
    return n ? n->level() + 1 : 0;
}

QString Node::generateWBSCode( QList<int> &indexes ) const {
    //kDebug(planDbg())<<m_name<<indexes;
    if ( m_parent == 0 ) {
        return QString();
    }
    indexes.insert( 0, m_parent->indexOf( this ) );
    return m_parent->generateWBSCode( indexes );
}

QString Node::wbsCode() const {
    //kDebug(planDbg())<<m_name;
    QList<int> indexes;
    return generateWBSCode( indexes );
}

bool Node::isScheduled( long id ) const
{
    Schedule *s = schedule( id );
    return s != 0 && s->isScheduled();
}

void Node::setCurrentSchedule(long id) {
    QListIterator<Node*> it = m_nodes;
    while (it.hasNext()) {
        it.next()->setCurrentSchedule(id);
    }
    //kDebug(planDbg())<<m_name<<" id:"<<id<<"="<<m_currentSchedule;
}

void Node::setStartupCost(double cost)
{
    m_startupCost = cost;
    changed(StartupCost);
}

void Node::setStartupAccount(Account *acc)
{
    //kDebug(planDbg())<<m_name<<"="<<acc;
    if ( m_startupAccount ) {
        m_startupAccount->removeStartup( *this );
    }
    m_startupAccount = acc;
    changed();
}

void Node::setShutdownCost(double cost)
{
    m_shutdownCost = cost;
    changed(ShutdownCost);
}

void Node::setShutdownAccount(Account *acc)
{
    //kDebug(planDbg())<<m_name<<"="<<acc;
    if ( m_shutdownAccount ) {
        m_shutdownAccount->removeShutdown( *this );
    }
    m_shutdownAccount = acc;
    changed();
}

void Node::setRunningAccount(Account *acc)
{
    //kDebug(planDbg())<<m_name<<"="<<acc;
    if ( m_runningAccount ) {
        m_runningAccount->removeRunning( *this );
    }
    m_runningAccount = acc;
    changed();
}

void Node::changed(Node *node, int property) {
    switch ( property) {
        case Type:
        case StartupCost:
        case ShutdownCost:
        case CompletionEntry:
        case CompletionStarted:
        case CompletionFinished:
        case CompletionStartTime:
        case CompletionFinishTime:
        case CompletionPercentage:
        case CompletionRemainingEffort:
        case CompletionActualEffort:
        case CompletionUsedEffort:
            foreach ( Schedule *s, m_schedules ) {
                s->clearPerformanceCache();
            }
        break;
        default: break;
    }
    if (m_parent) {
        m_parent->changed(node, property);
    }
}

Duration Node::plannedEffort( const Resource *resource, long id, EffortCostCalculationType type ) const
{
    Duration e;
    foreach ( Node *n, m_nodes ) {
        e += n->plannedEffort( resource, id, type );
    }
    return e;
}

Duration Node::plannedEffort( const Resource *resource, const QDate &date, long id, EffortCostCalculationType type ) const
{
    Duration e;
    foreach ( Node *n, m_nodes ) {
        e += n->plannedEffort( resource, date, id, type );
    }
    return e;
}

Duration Node::plannedEffortTo( const Resource *resource, const QDate &date, long id, EffortCostCalculationType type ) const
{
    Duration e;
    foreach ( Node *n, m_nodes ) {
        e += n->plannedEffortTo( resource, date, id, type );
    }
    return e;
}

EffortCost Node::plannedCost( long id, EffortCostCalculationType type ) const
{
    EffortCost ec;
    foreach ( Node *n, m_nodes ) {
        ec += n->plannedCost( id, type );
    }
    return ec;
}

EffortCostMap Node::bcwsPrDay( long int id, EffortCostCalculationType type ) const
{
    return const_cast<Node*>( this )->bcwsPrDay( id, type );
}

EffortCostMap Node::bcwsPrDay( long int id, EffortCostCalculationType type )
{
    Schedule *s = schedule( id );
    if ( s == 0 ) {
        return EffortCostMap();
    }
    EffortCostCache &ec = s->bcwsPrDayCache( type );
    if ( ! ec.cached ) {
        ec.effortcostmap = EffortCostMap();
        foreach ( Node *n, m_nodes ) {
            ec.effortcostmap += n->bcwsPrDay( id, type );
        }
        ec.cached = true;
    }
    return ec.effortcostmap;
}

EffortCostMap Node::bcwpPrDay( long int id, EffortCostCalculationType type ) const
{
    return const_cast<Node*>( this )->bcwpPrDay( id, type);
}

EffortCostMap Node::bcwpPrDay( long int id, EffortCostCalculationType type )
{
    Schedule *s = schedule( id );
    if ( s == 0 ) {
        return EffortCostMap();
    }
    EffortCostCache &ec = s->bcwpPrDayCache( type );
    if ( ! ec.cached ) {
        ec.effortcostmap = EffortCostMap();
        foreach ( Node *n, m_nodes ) {
            ec.effortcostmap += n->bcwpPrDay( id, type );
        }
        ec.cached = true;
    }
    return ec.effortcostmap;
}

EffortCostMap Node::acwp( long id, EffortCostCalculationType type ) const
{
    return const_cast<Node*>( this )->acwp( id, type );
}

EffortCostMap Node::acwp( long id, EffortCostCalculationType type )
{
    Schedule *s = schedule( id );
    if ( s == 0 ) {
        return EffortCostMap();
    }
    EffortCostCache &ec = s->acwpCache( type );
    if ( ! ec.cached ) {
        ec.effortcostmap = EffortCostMap();
        foreach ( Node *n, m_nodes ) {
            ec.effortcostmap += n->acwp( id, type );
        }
        ec.cached = true;
    }
    return ec.effortcostmap;
}

EffortCost Node::acwp( const QDate &date, long id ) const
{
    EffortCost ec;
    foreach ( Node *n, m_nodes ) {
        ec += n->acwp( date, id );
    }
    return ec;
}

void Node::slotStandardWorktimeChanged( StandardWorktime* )
{
    //kDebug(planDbg())<<m_estimate;
    if ( m_estimate ) {
        m_estimate->m_expectedCached = false;
        m_estimate->m_optimisticCached = false;
        m_estimate->m_pessimisticCached = false;
    }
}

void Node::emitDocumentAdded( Node *node, Document *doc, int idx )
{
    if ( m_parent ) {
        m_parent->emitDocumentAdded( node, doc, idx );
    }
}

void Node::emitDocumentRemoved( Node *node, Document *doc, int idx )
{
    if ( m_parent ) {
        m_parent->emitDocumentRemoved( node, doc, idx );
    }
}

void Node::emitDocumentChanged( Node *node, Document *doc, int idx )
{
    if ( m_parent ) {
        m_parent->emitDocumentChanged( node, doc, idx );
    }
}

//////////////////////////   Estimate   /////////////////////////////////

Estimate::Estimate( Node *parent )
    : m_parent( parent )
{
    m_pertCached = false;

    setUnit( Duration::Unit_h );
    setExpectedEstimate( 8.0 );
    setPessimisticEstimate( 8.0 );
    setOptimisticEstimate( 8.0 );
    
    m_type = Type_Effort;
    m_calendar = 0;
    m_risktype = Risk_None;
}

Estimate::Estimate(const Estimate &estimate, Node *parent)
    : m_parent( parent )
{
    copy( estimate );
}

Estimate::~Estimate()
{
}

void Estimate::clear()
{
    m_pertCached = false;

    setExpectedEstimate( 0.0 );
    setPessimisticEstimate( 0.0 );
    setOptimisticEstimate( 0.0 );
    
    m_type = Type_Effort;
    m_calendar = 0;
    m_risktype = Risk_None;
    m_unit = Duration::Unit_h;
    changed();
}

Estimate &Estimate::operator=( const Estimate &estimate )
{
    copy( estimate );
    return *this;
}

void Estimate::copy( const Estimate &estimate )
{
    //m_parent = 0; // don't touch
    m_expectedEstimate = estimate.m_expectedEstimate;
    m_optimisticEstimate = estimate.m_optimisticEstimate;
    m_pessimisticEstimate = estimate.m_pessimisticEstimate;
    
    m_expectedValue = estimate.m_expectedValue;
    m_optimisticValue = estimate.m_optimisticValue;
    m_pessimisticValue = estimate.m_pessimisticValue;
    
    m_expectedCached = estimate.m_expectedCached;
    m_optimisticCached = estimate.m_optimisticCached;
    m_pessimisticCached = estimate.m_pessimisticCached;
    
    m_pertExpected = estimate.m_pertExpected;
    m_pertCached = estimate.m_pertCached;

    m_type = estimate.m_type;
    m_calendar = estimate.m_calendar;
    m_risktype = estimate.m_risktype;
    m_unit = estimate.m_unit;
    changed();
}

double Estimate::variance() const
{
    double d = deviation();
    return d * d;
}

double Estimate::variance( Duration::Unit unit ) const
{
    double d = deviation( unit );
    return d * d;
}

double Estimate::deviation() const
{
    return ( m_pessimisticEstimate - m_optimisticEstimate ) / 6;
}

double Estimate::deviation( Duration::Unit unit ) const
{
    if ( unit == m_unit ) {
        return deviation();
    }
    double p = pessimisticValue().toDouble( unit );
    double o = optimisticValue().toDouble( unit );
    double v = ( p - o ) / 6;
    return v;
}

Duration Estimate::pertExpected() const {
    if (m_risktype == Risk_Low) {
        if ( ! m_pertCached ) {
            m_pertExpected = (optimisticValue() + pessimisticValue() + (expectedValue()*4))/6;
            m_pertCached = true;
        }
        return m_pertExpected;
    } else if (m_risktype == Risk_High) {
        if ( ! m_pertCached ) {
            m_pertExpected = (optimisticValue() + (pessimisticValue()*2) + (expectedValue()*4))/7;
            m_pertCached = true;
        }
        return m_pertExpected;
    }
    return expectedValue(); // risk==none
}

Duration Estimate::pertOptimistic() const {
    if (m_risktype != Risk_None) {
        return pertExpected() - Duration( variance( Duration::Unit_ms ) );
    }
    return optimisticValue();
}

Duration Estimate::pertPessimistic() const {
    if (m_risktype != Risk_None) {
        return pertExpected() + Duration( variance( Duration::Unit_ms ) );
    }
    return pessimisticValue();
}

Duration Estimate::value(int valueType, bool pert) const {
    if (valueType == Estimate::Use_Expected) {
        return pert ? pertExpected() : expectedValue();
    } else if (valueType == Estimate::Use_Optimistic) {
        return pert ? pertOptimistic() : optimisticValue();
    } else if (valueType == Estimate::Use_Pessimistic) {
        return pert ? pertPessimistic() : pessimisticValue();
    }
    return expectedValue();
}

void Estimate::setUnit( Duration::Unit unit )
{
    m_unit = unit;
    m_expectedCached = false;
    m_optimisticCached = false;
    m_pessimisticCached = false;
    m_pertCached = false;
    changed();
}

bool Estimate::load(KoXmlElement &element, XMLLoaderObject &status) {
    setType(element.attribute("type"));
    setRisktype(element.attribute("risk"));
    if ( status.version() <= "0.6" ) {
        m_unit = (Duration::Unit)(element.attribute("display-unit", QString().number(Duration::Unit_h) ).toInt());
        QList<qint64> s = status.project().standardWorktime()->scales();
        m_expectedEstimate = scale( Duration::fromString(element.attribute("expected")), m_unit, s );
        m_optimisticEstimate = scale(  Duration::fromString(element.attribute("optimistic")), m_unit, s );
        m_pessimisticEstimate = scale( Duration::fromString(element.attribute("pessimistic")), m_unit, s );
    } else {
        if ( status.version() <= "0.6.2" ) {
            // 0 pos in unit is now Unit_Y, so add 3 to get the correct new unit
            m_unit = (Duration::Unit)(element.attribute("unit", QString().number(Duration::Unit_ms - 3) ).toInt() + 3);
        } else {
            m_unit = Duration::unitFromString( element.attribute( "unit" ) );
        }
        m_expectedEstimate = element.attribute("expected", "0.0").toDouble();
        m_optimisticEstimate = element.attribute("optimistic", "0.0").toDouble();
        m_pessimisticEstimate = element.attribute("pessimistic", "0.0").toDouble();
        
        m_calendar = status.project().findCalendar(element.attribute("calendar-id"));
    }
    return true;
}

void Estimate::save(QDomElement &element) const {
    QDomElement me = element.ownerDocument().createElement("estimate");
    element.appendChild(me);
    me.setAttribute("expected", m_expectedEstimate);
    me.setAttribute("optimistic", m_optimisticEstimate);
    me.setAttribute("pessimistic", m_pessimisticEstimate);
    me.setAttribute("type", typeToString());
    if ( m_calendar ) {
        me.setAttribute("calendar-id", m_calendar->id() );
    }
    me.setAttribute("risk", risktypeToString());
    me.setAttribute("unit", Duration::unitToString( m_unit ) );
}

QString Estimate::typeToString( bool trans ) const {
    return typeToStringList( trans ).at( m_type );
}

QString Estimate::typeToString( Estimate::Type typ, bool trans )
{
    return typeToStringList( trans ).value( typ );
}

QStringList Estimate::typeToStringList( bool trans ) {
    return QStringList() 
            << (trans ? i18n("Effort") : QString("Effort"))
            << (trans ? i18n("Duration") : QString("Duration"));
}

void Estimate::setType(Type type)
{
    m_type = type;
    m_expectedCached = false;
    m_optimisticCached = false;
    m_pessimisticCached = false;
    m_pertCached = false;
    changed();
}

void Estimate::setType(const QString& type) {
    if (type == "Effort")
        setType(Type_Effort);
    else if (type == "Duration" || /*old format*/ type == "FixedDuration")
        setType(Type_Duration);
    else if (/*old format*/type == "Length")
        setType(Type_Duration);
    else if (type == "Type_FixedDuration") // Typo, keep old xml files working
        setType(Type_Duration);
    else
        setType(Type_Effort); // default
}

QString Estimate::risktypeToString( bool trans ) const {
    return risktypeToStringList( trans ).at( m_risktype );
}

QStringList Estimate::risktypeToStringList( bool trans ) {
    return QStringList() 
            << (trans ? i18n("None") : QString("None"))
            << (trans ? i18n("Low") : QString("Low"))
            << (trans ? i18n("High") : QString("High"));
}

void Estimate::setRisktype(const QString& type) {
    if (type == "High")
        setRisktype(Risk_High);
    else if (type == "Low")
        setRisktype(Risk_Low);
    else
        setRisktype(Risk_None); // default
}

void Estimate::setRisktype(Risktype type)
{
    m_pertCached = false;
    m_risktype = type;
    changed();
}

void Estimate::setCalendar( Calendar *calendar )
{
    m_calendar = calendar;
    m_expectedCached = false;
    m_optimisticCached = false;
    m_pessimisticCached = false;
    m_pertCached = false;
    changed();
}

void Estimate::setExpectedEstimate( double value)
{
    m_expectedEstimate = value;
    m_expectedCached = false;
    m_pertCached = false;
    changed();
}

void Estimate::setOptimisticEstimate( double value )
{
    m_optimisticEstimate = value;
    m_optimisticCached = false;
    m_pertCached = false;
    changed();
}

void Estimate::setPessimisticEstimate( double value )
{
    m_pessimisticEstimate = value;
    m_pessimisticCached = false;
    m_pertCached = false;
    changed();
}

void Estimate::setOptimisticRatio(int percent)
{
    int p = percent>0 ? -percent : percent;
    m_optimisticValue = expectedValue()*(100+p)/100;
    m_optimisticEstimate = scale( m_optimisticValue, m_unit, scales() );
    m_optimisticCached = true;
    m_pertCached = false;
    changed();
}

int Estimate::optimisticRatio() const {
    if (m_expectedEstimate == 0.0)
        return 0;
    return (int)((optimisticValue()*100)/expectedValue())-100;
}

void Estimate::setPessimisticRatio(int percent) 
{
    int p = percent<0 ? -percent : percent;
    m_pessimisticValue = expectedValue()*(100+p)/100;
    m_pessimisticEstimate = scale( m_pessimisticValue, m_unit, scales() );
    m_pessimisticCached = true;
    m_pertCached = false;
    changed();
}

int Estimate::pessimisticRatio() const {
    if (m_expectedEstimate == 0.0)
        return 0;
    return (int)((pessimisticValue()*100)/expectedValue())-100;
}

// internal
void Estimate::setOptimisticValue()
{
    m_optimisticValue = scale( m_optimisticEstimate, m_unit, scales() );
    m_optimisticCached = true;
    m_pertCached = false;
}

// internal
void Estimate::setExpectedValue()
{
    m_expectedValue = scale( m_expectedEstimate, m_unit, scales() );
    m_expectedCached = true;
    m_pertCached = false;
}

// internal
void Estimate::setPessimisticValue()
{
    m_pessimisticValue = scale( m_pessimisticEstimate, m_unit, scales() );
    m_pessimisticCached = true;
    m_pertCached = false;
}

Duration Estimate::optimisticValue() const
{
    if ( ! m_optimisticCached ) {
        const_cast<Estimate*>(this)->setOptimisticValue();
    }
    return m_optimisticValue;
}

Duration Estimate::pessimisticValue() const
{
    if ( ! m_pessimisticCached ) {
        const_cast<Estimate*>(this)->setPessimisticValue();
    }
    return m_pessimisticValue;
}

Duration Estimate::expectedValue() const
{
    if ( ! m_expectedCached ) {
        const_cast<Estimate*>(this)->setExpectedValue();
    }
    return m_expectedValue;
}

double Estimate::scale( const Duration &value, Duration::Unit unit, const QList<qint64> &scales )
{
    //kDebug(planDbg())<<value.toDouble( unit )<<","<<unit<<scales;
    QList<qint64> lst = scales;
    switch ( lst.count() ) {
        case Duration::Unit_Y:
            lst << (qint64)(365 * 24) * 60 * 60 * 1000; // add milliseconds in a year
        case Duration::Unit_M:
            lst << (qint64)(30 * 24) * 60 * 60 * 1000; // add milliseconds in a month
        case Duration::Unit_w:
            lst << (qint64)(7 * 24) * 60 * 60 * 1000; // add milliseconds in a week
        case Duration::Unit_d:
            lst << 24 * 60 * 60 * 1000; // add milliseconds in day
        case Duration::Unit_h:
            lst << 60 * 60 * 1000; // add milliseconds in hour
        case Duration::Unit_m:
            lst << 60 * 1000; // add milliseconds in minute
        case Duration::Unit_s:
            lst << 1000; // add milliseconds in second
        case Duration::Unit_ms:
            lst << 1; // add milliseconds in a millisecond
        default:
            break;
    }
    double v = ( double )( value.milliseconds() );
    v /= lst[ unit ];
    //kDebug(planDbg())<<value.toString()<<","<<unit<<"="<<v;
    return v;
}

Duration Estimate::scale( double value, Duration::Unit unit, const QList<qint64> &scales )
{
    //kDebug(planDbg())<<value<<","<<unit<<scales;
    QList<qint64> lst = scales;
    switch ( lst.count() ) {
        case Duration::Unit_Y:
            lst << (qint64)(365 * 24) * 60 * 60 * 1000; // add milliseconds in a year
        case Duration::Unit_M:
            lst << (qint64)(30 * 24) * 60 * 60 * 1000; // add milliseconds in a month
        case Duration::Unit_w:
            lst << (qint64)(7 * 24) * 60 * 60 * 1000; // add milliseconds in a week
        case Duration::Unit_d:
            lst << 24 * 60 * 60 * 1000; // add milliseconds in day
        case Duration::Unit_h:
            lst << 60 * 60 * 1000; // add milliseconds in hour
        case Duration::Unit_m:
            lst << 60 * 1000; // add milliseconds in minute
        case Duration::Unit_s:
            lst << 1000; // add milliseconds in second
        case Duration::Unit_ms:
            lst << 1; // add milliseconds in a millisecond
        default:
            break;
    }
    qint64 v = ( qint64 )( value * lst[ unit ] );
    //kDebug(planDbg())<<value<<","<<unit<<"="<<v;
    return Duration( v, Duration::Unit_ms );
}

//static
QList<qint64> Estimate::defaultScales()
{
    QList<qint64> lst;
    lst << (qint64)(365 * 24) * 60 * 60 * 1000  // add milliseconds in a year
        << (qint64)(30 * 24) * 60 * 60 * 1000   // add milliseconds in a month
        << (qint64)(7 * 24) * 60 * 60 * 1000    // add milliseconds in a week
        << 24 * 60 * 60 * 1000                  // add milliseconds in day
        << 60 * 60 * 1000                       // add milliseconds in hour
        << 60 * 1000                            // add milliseconds in minute
        << 1000                                 // add milliseconds in second
        << 1;                                   // add milliseconds in a millisecond
    return lst;
}

QList<qint64> Estimate::scales() const
{
    QList<qint64> s;
    if ( m_type == Type_Duration && m_calendar == 0 ) {
        return s; // Use default scaling ( 24h a day...)
    }
    if ( m_parent == 0 ) {
        return s;
    }
    Project *p = static_cast<Project*>( m_parent->projectNode() );
    if ( p == 0 ) {
        return s;
    }
    s << p->standardWorktime()->scales();
    return s;
}


}  //KPlato namespace

#include "kptnode.moc"
