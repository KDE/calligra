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

#include "kptproject.h"
#include "kpttask.h"
#include "kptprojectdialog.h"
#include "kptdatetime.h"
#include "kptpart.h"
#include "kptconfig.h"
#include "kpteffortcostmap.h"

#include <qdom.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qbrush.h>
#include <qcanvas.h>
#include <qptrlist.h>

#include <kdatetimewidget.h>
#include <kdebug.h>

namespace KPlato
{

/// Use for main projects
Project::Project(Node *parent)
    : Node(parent),
      m_accounts(*this),
      m_baselined(false) {
    m_constraint = Node::MustStartOn;
    m_standardWorktime = new StandardWorktime();
    m_defaultCalendar = new Calendar(*m_standardWorktime);
    m_defaultCalendar->setProject(this);
    init();
}

void Project::init() {
    if (m_parent == 0) {
        // set sensible defaults for a project wo parent
        m_startTime = DateTime(QDate::currentDate(),m_standardWorktime->startOfDay(QDate::currentDate()));
        m_endTime = DateTime(QDate::currentDate(),m_standardWorktime->endOfDay(QDate::currentDate()));
        m_duration = m_endTime - m_startTime;
        if (m_duration == Duration::zeroDuration)
            m_duration.addDays(1);
    }    
    m_calendars.setAutoDelete(true);
}


Project::~Project() {
    m_resourceGroups.setAutoDelete(true);
    m_resourceGroups.clear();
    delete m_standardWorktime;
}

int Project::type() const { return Node::Type_Project; }

void Project::calculate(Effort::Use use) {
    //kdDebug()<<k_funcinfo<<"Node="<<m_name<<" Start="<<m_startTime.toString()<<endl;
    // clear all resource appointments
    QPtrListIterator<ResourceGroup> git(m_resourceGroups);
    for ( ; git.current(); ++git ) {
        git.current()->clearAppointments();
    }
    if (type() == Type_Project) {
        initiateCalculation();
        if (m_constraint == Node::MustStartOn) {
            // Calculate from start time
            propagateEarliestStart(m_startTime);
            m_endTime = calculateForward(use);
            propagateLatestFinish(m_endTime);
            calculateBackward(use);
            scheduleForward(m_startTime, use);
        } else {
            // Calculate from end time
            propagateLatestFinish(m_endTime);
            m_startTime = calculateBackward(use);
            propagateEarliestStart(m_startTime);
            calculateForward(use);
            scheduleBackward(m_endTime, use);
        }
        calcCriticalPath();
        makeAppointments();
        calcResourceOverbooked();
    } else if (type() == Type_Subproject) {
        kdWarning()<<k_funcinfo<<"Subprojects not implemented"<<endl;
    } else {
        kdError()<<k_funcinfo<<"Illegal project type: "<<type()<<endl;
    }
}

bool Project::calcCriticalPath() {
    kdDebug()<<k_funcinfo<<endl;
    QPtrListIterator<Node> endnodes = m_endNodes;
    for (; endnodes.current(); ++endnodes) {
        endnodes.current()->calcCriticalPath();
    }
    return false;
}

Duration *Project::getExpectedDuration() {
    //kdDebug()<<k_funcinfo<<endl;
    return new Duration(getLatestFinish() - getEarliestStart());
}

Duration *Project::getRandomDuration() {
    return 0L;
}

DateTime Project::calculateForward(int use) {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (type() == Node::Type_Project) {
        // Follow *parent* relations back and
        // calculate forwards following the child relations
        DateTime finish;
        DateTime time;
        QPtrListIterator<Node> endnodes = m_endNodes;
        for (; endnodes.current(); ++endnodes) {
            time = endnodes.current()->calculateForward(use);
            if (!finish.isValid() || time > finish)
                finish = time;
        }
        //kdDebug()<<k_funcinfo<<m_name<<" finish="<<finish.toString()<<endl;
        return finish;
    } else {
        //TODO: subproject
    }
    return DateTime();
}

DateTime Project::calculateBackward(int use) {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (type() == Node::Type_Project) {
        // Follow *child* relations back and
        // calculate backwards following parent relation
        DateTime start;
        DateTime time;
        QPtrListIterator<Node> startnodes = m_startNodes;
        for (; startnodes.current(); ++startnodes) {
            time = startnodes.current()->calculateBackward(use);
            if (!start.isValid() || time < start)
                start = time;
        }
        //kdDebug()<<k_funcinfo<<m_name<<" start="<<start.toString()<<endl;
        return start;
    } else {
        //TODO: subproject
    }
    return DateTime();
}

DateTime &Project::scheduleForward(DateTime &earliest, int use) {
    resetVisited();
    QPtrListIterator<Node> it(m_endNodes);
    for (; it.current(); ++it) {
        it.current()->scheduleForward(earliest, use);
    }
    // Fix summarytasks
    adjustSummarytask();
    return m_endTime;
}

DateTime &Project::scheduleBackward(DateTime &latest, int use) {
    resetVisited();
    QPtrListIterator<Node> it(m_startNodes);
    for (; it.current(); ++it) {
        it.current()->scheduleBackward(latest, use);
    }
    // Fix summarytasks
    adjustSummarytask();
    return m_startTime;
}

void Project::adjustSummarytask() {
    QPtrListIterator<Node> it(m_summarytasks);
    for (; it.current(); ++it) {
        it.current()->adjustSummarytask();
    }
}

void Project::initiateCalculation() {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    Node::initiateCalculation();
    m_startNodes.clear();
    m_endNodes.clear();
    m_summarytasks.clear();
    initiateCalculationLists(m_startNodes, m_endNodes, m_summarytasks);
}

void Project::initiateCalculationLists(QPtrList<Node> &startnodes, QPtrList<Node> &endnodes, QPtrList<Node> &summarytasks) {
    //kdDebug()<<k_funcinfo<<m_name<<endl;
    if (type() == Node::Type_Project) {
        QPtrListIterator<Node> it = childNodeIterator();
        for (; it.current(); ++it) {
            it.current()->initiateCalculationLists(startnodes, endnodes, summarytasks);
        }
    } else {
        //TODO: subproject
    }
}

bool Project::load(QDomElement &element) {
    // Maybe TODO: Delete old stuff here
    bool ok = false;
    QString id = element.attribute("id");
    if (!setId(id)) {
        kdWarning()<<k_funcinfo<<"Id must be unique: "<<id<<endl;
    }
    m_name = element.attribute("name");
    m_leader = element.attribute("leader");
    m_description = element.attribute("description");
    
    //m_baselined = (bool)element.attribute("baselined","0").toInt(&ok);FIXME: Removed for this release  
    
    // Allow for both numeric and text
    QString c = element.attribute("scheduling","0");
    m_constraint = (Node::ConstraintType)c.toInt(&ok);
    if (!ok)
        setConstraint(c);
    if (m_constraint != Node::MustStartOn &&
        m_constraint != Node::MustFinishOn) {
        kdError()<<k_funcinfo<<"Illegal constraint: "<<constraintToString()<<endl;
        setConstraint(Node::MustStartOn);
    }

    // Load the project children
    // Must do these first
    QDomNodeList list = element.childNodes();
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "calendar") {
                // Load the calendar.
                // References by resources
                Calendar *child = new Calendar();
                child->setProject(this);
                if (child->load(e)) {
                    addCalendar(child);
                } else {
                    // TODO: Complain about this
                    kdError()<<k_funcinfo<<"Failed to load calendar"<<endl;
                    delete child;
                }
            } else if (e.tagName() == "standard-worktime") {
                // Load standard worktime
                StandardWorktime *child = new StandardWorktime();
                if (child->load(e)) {
                    addStandardWorktime(child);
                    addDefaultCalendar(child);
                } else {
                    kdError()<<k_funcinfo<<"Failed to load standard worktime"<<endl;
                    delete child;
                }
            }
        }
    }
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
    
            if (e.tagName() == "resource-group") {
                // Load the resources
                // References calendars
                ResourceGroup *child = new ResourceGroup(this);
                if (child->load(e)) {
                        addResourceGroup(child);
                } else {
                    // TODO: Complain about this
                    delete child;
                }
            }
        }
    }
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
    
            if (e.tagName() == "project") {
                // Load the subproject
                Project *child = new Project(this);
                if (child->load(e)) {
                    addChildNode(child);
                } else {
                    // TODO: Complain about this
                    delete child;
                }
            } else if (e.tagName() == "task") {
                // Load the task (and resourcerequests). 
                // Depends on resources already loaded
                Task *child = new Task(this);
                if (child->load(e, *this)) {
                    addChildNode(child);
                } else {
                    // TODO: Complain about this
                    delete child;
                }
            }
        }
    }
    // These go last
    for (unsigned int i=0; i<list.count(); ++i) {
        if (list.item(i).isElement()) {
            QDomElement e = list.item(i).toElement();
            if (e.tagName() == "accounts") {
                // Load accounts
                // References tasks
                if (!m_accounts.load(e, *this)) {
                    kdError()<<k_funcinfo<<"Failed to load accounts"<<endl;
                }
            } else if (e.tagName() == "relation") {
                // Load the relation
                // References tasks
                Relation *child = new Relation();
                if (!child->load(e, *this)) {
                    // TODO: Complain about this
                    kdError()<<k_funcinfo<<"Failed to load relation"<<endl;
                    delete child;
                }
            } else if (e.tagName() == "schedules") {
                // Prepare for multiple schedules
                // References tasks and resources
                QDomNodeList lst = e.childNodes();
                for (unsigned int i=0; i<lst.count(); ++i) {
                    if (lst.item(i).isElement()) {
                        QDomElement sch = lst.item(i).toElement();
                        if (sch.tagName() == "schedule") {
                            DateTime dt( QDateTime::currentDateTime() );
                            dt = dt.fromString(sch.attribute("project-start", dt.toString()) );
                            //kdDebug()<<k_funcinfo<<"Start="<<dt.toString()<<endl;
                            setStartTime(dt);
                        
                            // Use project-start as default
                            dt = dt.fromString(sch.attribute("project-end", dt.toString()) );
                            //kdDebug()<<k_funcinfo<<"End="<<dt.toString()<<endl;
                            setEndTime(dt);
                            
                            QDomNodeList al = sch.childNodes();
                            for (unsigned int i=0; i<al.count(); ++i) {
                                if (al.item(i).isElement()) {
                                    QDomElement app = al.item(i).toElement();
                                    if (app.tagName() == "appointment") {
                                        // Load the appointments. 
                                        // Resources and tasks must allready loaded
                                        Appointment *child = new Appointment();
                                        if (!child->loadXML(app, *this)) {
                                            // TODO: Complain about this
                                            kdError()<<k_funcinfo<<"Failed to load appointment"<<endl;
                                            delete child;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // calendars references calendars in arbritary saved order
    QPtrListIterator<Calendar> calit(m_calendars);
    for (; calit.current(); ++calit) {
        if (calit.current()->id() == calit.current()->parentId()) {
            kdError()<<k_funcinfo<<"Calendar want itself as parent"<<endl;
            continue;
        }
        calit.current()->setParent(calendar(calit.current()->parentId()));
    }
    return true;
}


void Project::save(QDomElement &element)  {
    QDomElement me = element.ownerDocument().createElement("project");
    element.appendChild(me);

    me.setAttribute("name", m_name);
    me.setAttribute("leader", m_leader);
    me.setAttribute("id", m_id);
    me.setAttribute("description", m_description);
    
    //me.setAttribute("baselined",(int)m_baselined); FIXME: Removed for this release  

    me.setAttribute("scheduling",constraintToString());    
    
    m_accounts.save(me);
    
    // save calendars
    QPtrListIterator<Calendar> calit(m_calendars);
    for (; calit.current(); ++calit) {
        calit.current()->save(me);
    }
    // save standard worktime
    if (m_standardWorktime)
        m_standardWorktime->save(me);
    
    // save project resources, must be after calendars
    QPtrListIterator<ResourceGroup> git(m_resourceGroups);
    for ( ; git.current(); ++git ) {
        git.current()->save(me);
    }

    // Only save parent relations
    QPtrListIterator<Relation> it(m_dependParentNodes);
    for ( ; it.current(); ++it ) {
        it.current()->save(me);
    }

    for (int i=0; i<numChildren(); i++)
    // Save all children
    getChildNode(i)->save(me);

    // Now we can save relations assuming no tasks have relations outside the project
    QPtrListIterator<Node> nodes(m_nodes);
    for ( ; nodes.current(); ++nodes ) {
        nodes.current()->saveRelations(me);
    }
    
    // Prepare for multiple schedules (expected, optimistic, pessmistic...)
    // This goes into separate class later.
    QDomElement schs = me.ownerDocument().createElement("schedules");
    me.appendChild(schs);
    QDomElement sch = schs.ownerDocument().createElement("schedule");
    {
        schs.appendChild(sch);
        sch.setAttribute("name", "Standard");
        sch.setAttribute("type", "Expected");
        sch.setAttribute("project-start",startTime().toString());
        sch.setAttribute("project-end",endTime().toString());
        // save appointments
        QPtrListIterator<ResourceGroup> rgit(m_resourceGroups);
        for ( ; rgit.current(); ++rgit ) {
            rgit.current()->saveAppointments(sch);
        }
    }
}

void Project::addResourceGroup(ResourceGroup * group) {
    m_resourceGroups.append(group);
}


void Project::removeResourceGroup(ResourceGroup * group){
    m_resourceGroups.remove(group);
}


void Project::removeResourceGroup(int /* number */){
   // always auto remove
}


void Project::insertResourceGroup( unsigned int /* index */,
			      ResourceGroup * /* resource */) {
}

QPtrList<ResourceGroup> &Project::resourceGroups() {
     return m_resourceGroups;
}

bool Project::addTask( Node* task, Node* position )
{
	// we want to add a task at the given position. => the new node will
	// become next sibling right after position.
	if ( 0 == position ) {
      kdError()<<k_funcinfo<<"position=0, could not add task: "<<task->name()<<endl;
	  return false;
	}
    //kdDebug()<<k_funcinfo<<"Add "<<task->name()<<" after "<<position->name()<<endl;
	// in case we want to add to the main project, we make it child element
	// of the root element.
	if ( Node::Type_Project == position->type() ) {
        return addSubTask(task, position);
	}
	// find the position
	// we have to tell the parent that we want to delete one of its children
	Node* parentNode = position->getParent();
	if ( !parentNode ) {
		kdDebug()<<k_funcinfo<<"parent node not found???"<<endl;
		return false;
	}
	int index = parentNode->findChildNode( position );
	if ( -1 == index ) {
		// ok, it does not exist
		kdDebug()<<k_funcinfo<<"Task not found???"<<endl;
		return false;
	}
	parentNode->insertChildNode( index+1, task );
    return true;
}

bool Project::addSubTask( Node* task, Node* position )
{
	// we want to add a subtask to the node "position". It will become
	// position's last child.
	if ( 0 == position ) {
      kdError()<<k_funcinfo<<"No parent, can not add subtask: "<<task->name()<<endl;
	  return false;
	}
	position->addChildNode(task);
    return true;
}

bool Project::canIndentTask(Node* node)
{
    if (0 == node) {
        // should always be != 0. At least we would get the Project,
        // but you never know who might change that, so better be careful
        return false;
    }
    if (node->type() == Node::Type_Project) {
        kdDebug()<<k_funcinfo<<"The root node cannot be indented"<<endl;
        return false;
    }
    // we have to find the parent of task to manipulate its list of children
    Node* parentNode = node->getParent();
    if ( !parentNode ) {
        return false;
    }
    if (parentNode->findChildNode(node) == -1) {
        kdError()<<k_funcinfo<<"Tasknot found???"<<endl;
        return false;
    }
    Node *sib = node->siblingBefore();
    if (!sib) {
        kdDebug()<<k_funcinfo<<"new parent node not found"<<endl;
        return false;
    }
    if (node->findParentRelation(sib) || node->findChildRelation(sib)) {
        kdDebug()<<k_funcinfo<<"Cannot have relations to parent"<<endl;
        return false;
    }
    return true;
}

bool Project::indentTask( Node* node )
{
    if (canIndentTask(node)) {
        Node *newParent = node->siblingBefore();
        node->getParent()->delChildNode(node, false/*do not delete objekt*/);
        newParent->addChildNode(node);
        return true;
    }
    return false;
}

bool Project::canUnindentTask( Node* node )
{
    if ( 0 == node ) {
        // is always != 0. At least we would get the Project, but you
        // never know who might change that, so better be careful
        return false;
    }
    if ( Node::Type_Project == node->type() ) {
        kdDebug()<<k_funcinfo<<"The root node cannot be unindented"<<endl;
        return false;
    }
    // we have to find the parent of task to manipulate its list of children
    // and we need the parent's parent too
    Node* parentNode = node->getParent();
    if ( !parentNode ) {
        return false;
    }
    Node* grandParentNode = parentNode->getParent();
    if ( !grandParentNode ) {
        kdDebug()<<k_funcinfo<<"This node already is at the top level"<<endl;
        return false;
    }
    int index = parentNode->findChildNode( node );
    if ( -1 == index ) {
        kdError()<<k_funcinfo<<"Tasknot found???"<<endl;
        return false;
    }
    return true;
}

bool Project::unindentTask( Node* node )
{
    if (canUnindentTask(node)) {
        Node *parentNode = node->getParent();
        Node *grandParentNode = parentNode->getParent();
        parentNode->delChildNode(node, false/*do not delete objekt*/);
        grandParentNode->addChildNode(node,parentNode);
        return true;
    }
    return false;
}

bool Project::canMoveTaskUp( Node* node )
{
    // we have to find the parent of task to manipulate its list of children
    Node* parentNode = node->getParent();
    if (!parentNode) {
        kdDebug()<<k_funcinfo<<"No parent found"<<endl;
        return false;
    }
    if (parentNode->findChildNode(node) == -1) {
        kdError()<<k_funcinfo<<"Tasknot found???"<<endl;
        return false;
    }
    if (node->siblingBefore()) {
        return true;
    }
    return false;
}

bool Project::moveTaskUp( Node* node )
{
    if (canMoveTaskUp(node)) {
        return node->getParent()->moveChildUp(node);
    }
    return false;
}

bool Project::canMoveTaskDown( Node* node )
{
    // we have to find the parent of task to manipulate its list of children
    Node* parentNode = node->getParent();
    if (!parentNode) {
        return false;
    }
    if (parentNode->findChildNode(node) == -1) {
        kdError()<<k_funcinfo<<"Tasknot found???"<<endl;
        return false;
    }
    if (node->siblingAfter()) {
        return true;
    }
    return false;
}

bool Project::moveTaskDown( Node* node )
{
    if (canMoveTaskDown(node)) {
        return node->getParent()->moveChildDown(node);
    }
    return false;
}

ResourceGroup *Project::group(QString id) {
    return findResourceGroup(id);
}

Resource *Project::resource(QString id) {
    return findResource(id);
}

// TODO
EffortCostMap Project::plannedEffortCostPrDay(const QDate &start, const QDate &end) const {
    kdDebug()<<k_funcinfo<<endl;
    EffortCostMap ec;
    return ec;

}

// Returns the total planned effort for this project (or subproject) 
Duration Project::plannedEffort() {
   //kdDebug()<<k_funcinfo<<endl;
    Duration eff;
    QPtrListIterator<Node> it(childNodeIterator());
    for (; it.current(); ++it) {
        eff += it.current()->plannedEffort();
    }
    return eff;
}

// Returns the total planned effort for this project (or subproject) on date
Duration Project::plannedEffort(const QDate &date) {
   //kdDebug()<<k_funcinfo<<endl;
    Duration eff;
    QPtrListIterator<Node> it(childNodeIterator());
    for (; it.current(); ++it) {
        eff += it.current()->plannedEffort(date);
    }
    return eff;
}

// Returns the total planned effort for this project (or subproject) upto and including date
Duration Project::plannedEffortTo(const QDate &date) {
   //kdDebug()<<k_funcinfo<<endl;
    Duration eff;
    QPtrListIterator<Node> it(childNodeIterator());
    for (; it.current(); ++it) {
        eff += it.current()->plannedEffortTo(date);
    }
    return eff;
}

// Returns the total actual effort for this project (or subproject) 
Duration Project::actualEffort() {
   //kdDebug()<<k_funcinfo<<endl;
    Duration eff;
    QPtrListIterator<Node> it(childNodeIterator());
    for (; it.current(); ++it) {
        eff += it.current()->actualEffort();
    }
    return eff;
}

// Returns the total actual effort for this project (or subproject) on date
Duration Project::actualEffort(const QDate &date) {
   //kdDebug()<<k_funcinfo<<endl;
    Duration eff;
    QPtrListIterator<Node> it(childNodeIterator());
    for (; it.current(); ++it) {
        eff += it.current()->actualEffort(date);
    }
    return eff;
}

// Returns the total actual effort for this project (or subproject) upto and including date
Duration Project::actualEffortTo(const QDate &date) {
   //kdDebug()<<k_funcinfo<<endl;
    Duration eff;
    QPtrListIterator<Node> it(childNodeIterator());
    for (; it.current(); ++it) {
        eff += it.current()->actualEffortTo(date);
    }
    return eff;
}

double Project::plannedCost() {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    QPtrListIterator<Node> it(childNodeIterator());
    for (; it.current(); ++it) {
        c += it.current()->plannedCost();
    }
    return c;
}

// Returns the total planned effort for this project (or subproject) on date
double Project::plannedCost(const QDate &date) {
   //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    QPtrListIterator<Node> it(childNodeIterator());
    for (; it.current(); ++it) {
        c += it.current()->plannedCost(date);
    }
    return c;
}

// Returns the total planned effort for this project (or subproject) upto and including date
double Project::plannedCostTo(const QDate &date) {
   //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    QPtrListIterator<Node> it(childNodeIterator());
    for (; it.current(); ++it) {
        c += it.current()->plannedCostTo(date);
    }
    return c;
}

double Project::actualCost() {
    //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    QPtrListIterator<Node> it(childNodeIterator());
    for (; it.current(); ++it) {
        c += it.current()->actualCost();
    }
    return c;
}

// Returns the total planned effort for this project (or subproject) on date
double Project::actualCost(const QDate &date) {
   //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    QPtrListIterator<Node> it(childNodeIterator());
    for (; it.current(); ++it) {
        c += it.current()->actualCost(date);
    }
    return c;
}

// Returns the total planned effort for this project (or subproject) upto and including date
double Project::actualCostTo(const QDate &date) {
   //kdDebug()<<k_funcinfo<<endl;
    double c = 0;
    QPtrListIterator<Node> it(childNodeIterator());
    for (; it.current(); ++it) {
        c += it.current()->actualCostTo(date);
    }
    return c;
}

void Project::addCalendar(Calendar *calendar) {
    //kdDebug()<<k_funcinfo<<calendar->name()<<endl;
    m_calendars.append(calendar);
}

Calendar *Project::calendar(const QString id) const {
    return findCalendar(id);
}

QPtrList<Calendar> Project::calendars() {
    QPtrList<Calendar> list;
    QPtrListIterator<Calendar> it = m_calendars;
    for (; it.current(); ++it) {
        kdDebug()<<k_funcinfo<<it.current()->name()<<" deleted="<<it.current()->isDeleted()<<endl;
        if (!it.current()->isDeleted()) {
            list.append(it.current());
        }
    }
    return list;
}

void Project::addStandardWorktime(StandardWorktime * worktime) {
    if (m_standardWorktime != worktime) {
        delete m_standardWorktime; 
        m_standardWorktime = worktime; 
    }
}

void Project::addDefaultCalendar(StandardWorktime * worktime) {
    delete m_defaultCalendar;
    m_defaultCalendar = new Calendar(*worktime);
    m_defaultCalendar->setProject(this);
}

bool Project::legalToLink(Node *par, Node *child) {
    //kdDebug()<<k_funcinfo<<par.name()<<" ("<<par.numDependParentNodes()<<" parents) "<<child.name()<<" ("<<child.numDependChildNodes()<<" children)"<<endl;
    
    if (!child || par->isDependChildOf(child)) {
        return false;
    }
    bool legal = true;
    // see if par/child is related
    if (par->isParentOf(child) || child->isParentOf(par)) {
        legal = false;
    }
    if (legal)
        legal = legalChildren(par, child);
    if (legal)
        legal = legalParents(par, child);
    
    return legal;
}

bool Project::legalParents(Node *par, Node *child) {
    bool legal = true;
    //kdDebug()<<k_funcinfo<<par->name()<<" ("<<par->numDependParentNodes()<<" parents) "<<child->name()<<" ("<<child->numDependChildNodes()<<" children)"<<endl;
    for (int i=0; i < par->numDependParentNodes() && legal; ++i) {
        Node *pNode = par->getDependParentNode(i)->parent();
        if (child->isParentOf(pNode) || pNode->isParentOf(child)) {
            //kdDebug()<<k_funcinfo<<"Found: "<<pNode->name()<<" is related to "<<child->name()<<endl;
            legal = false;
        } else {
            legal = legalChildren(pNode, child);
        }
        if (legal)
            legal = legalParents(pNode, child);
    }
    return legal;
}

bool Project::legalChildren(Node *par, Node *child) {
    bool legal = true;
    //kdDebug()<<k_funcinfo<<par->name()<<" ("<<par->numDependParentNodes()<<" parents) "<<child->name()<<" ("<<child->numDependChildNodes()<<" children)"<<endl;
    for (int j=0; j < child->numDependChildNodes() && legal; ++j) {
        Node *cNode = child->getDependChildNode(j)->child();
        if (par->isParentOf(cNode) || cNode->isParentOf(par)) {
            //kdDebug()<<k_funcinfo<<"Found: "<<par->name()<<" is related to "<<cNode->name()<<endl;
            legal = false;
        } else {
            legal = legalChildren(par, cNode);
        }
    }
    return legal;
}

void Project::generateWBS(int count, WBSDefinition &def, QString wbs) {
    if (type() == Type_Subproject || def.level0Enabled()) {
        Node::generateWBS(count, def, wbs);
    } else {
        QPtrListIterator<Node> it = m_nodes;
        for (int i=0; it.current(); ++it) {
            it.current()->generateWBS(++i, def, m_wbs);
        }
    }
}


#ifndef NDEBUG
void Project::printDebug(bool children, QCString indent) {

    kdDebug()<<indent<<"+ Project node: "<<name()<<endl;
    indent += "!";
    QPtrListIterator<ResourceGroup> it(resourceGroups());
    for ( ; it.current(); ++it)
        it.current()->printDebug(indent);

    Node::printDebug(children, indent);
}
void Project::printCalendarDebug(QCString indent) {
    kdDebug()<<indent<<"-------- Calendars debug printout --------"<<endl;
    QPtrListIterator<Calendar> it = m_calendars;
    for (; it.current(); ++it) {
        it.current()->printDebug(indent + "--");
        kdDebug()<<endl;
    }
    if (m_standardWorktime)
        m_standardWorktime->printDebug();
}
#endif

}  //KPlato namespace
