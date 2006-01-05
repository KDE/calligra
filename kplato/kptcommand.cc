/* This file is part of the KDE project
   Copyright (C) 2004, 2005 Dag Andersen <danders@get2net.dk>

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
 * Boston, MA 02110-1301, USA.
*/

#include "kptcommand.h"
#include "kptaccount.h"
#include "kptappointment.h"
#include "kptpart.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptcalendar.h"
#include "kptrelation.h"
#include "kptresource.h"

#include <kdebug.h>
#include <klocale.h>

namespace KPlato
{

CalendarAddCmd::CalendarAddCmd(Part *part, Project *project,Calendar *cal, QString name)
    : NamedCommand(part, name),
      m_project(project),
      m_cal(cal),
      m_added(false) {
    cal->setDeleted(true);  
    //kdDebug()<<k_funcinfo<<cal->name()<<endl;
}

void CalendarAddCmd::execute() {
    if (!m_added && m_project) {
        m_project->addCalendar(m_cal);
        m_added = true;
    }
    m_cal->setDeleted(false);
    
    setCommandType(1);
    //kdDebug()<<k_funcinfo<<m_cal->name()<<" added to: "<<m_project->name()<<endl;
}

void CalendarAddCmd::unexecute() {
    m_cal->setDeleted(true);
    
    setCommandType(1);
    //kdDebug()<<k_funcinfo<<m_cal->name()<<endl;
}

CalendarDeleteCmd::CalendarDeleteCmd(Part *part, Calendar *cal, QString name)
    : NamedCommand(part, name),
      m_cal(cal) {
}

void CalendarDeleteCmd::execute() {
    m_cal->setDeleted(true);
    
    setCommandType(1);
}

void CalendarDeleteCmd::unexecute() {
    m_cal->setDeleted(false);
    
    setCommandType(1);
}

CalendarModifyNameCmd::CalendarModifyNameCmd(Part *part, Calendar *cal, QString newvalue, QString name)
    : NamedCommand(part, name),
      m_cal(cal) {
      
    m_oldvalue = cal->name();
    m_newvalue = newvalue;
    //kdDebug()<<k_funcinfo<<cal->name()<<endl;
}
void CalendarModifyNameCmd::execute() {
    m_cal->setName(m_newvalue);
    setCommandType(0);
    //kdDebug()<<k_funcinfo<<m_cal->name()<<endl;
}
void CalendarModifyNameCmd::unexecute() {
    m_cal->setName(m_oldvalue);
    setCommandType(0);
    //kdDebug()<<k_funcinfo<<m_cal->name()<<endl;
}

CalendarModifyParentCmd::CalendarModifyParentCmd(Part *part, Calendar *cal, Calendar *newvalue, QString name)
    : NamedCommand(part, name),
      m_cal(cal) {
      
    m_oldvalue = cal->parent();
    m_newvalue = newvalue;
    //kdDebug()<<k_funcinfo<<cal->name()<<endl;
}
void CalendarModifyParentCmd::execute() {
    m_cal->setParent(m_newvalue);
    setCommandType(1);
}
void CalendarModifyParentCmd::unexecute() {
    m_cal->setParent(m_oldvalue);
    setCommandType(1);
}

CalendarAddDayCmd::CalendarAddDayCmd(Part *part, Calendar *cal, CalendarDay *newvalue, QString name)
    : NamedCommand(part, name),
      m_cal(cal),
      m_mine(true) {
      
    m_newvalue = newvalue;
    //kdDebug()<<k_funcinfo<<cal->name()<<endl;
}
CalendarAddDayCmd::~CalendarAddDayCmd() {
    //kdDebug()<<k_funcinfo<<endl;
    if (m_mine)
        delete m_newvalue;
}
void CalendarAddDayCmd::execute() {
    //kdDebug()<<k_funcinfo<<m_cal->name()<<endl;
    m_cal->addDay(m_newvalue);
    m_mine = false;
    setCommandType(1);
}
void CalendarAddDayCmd::unexecute() {
    //kdDebug()<<k_funcinfo<<m_cal->name()<<endl;
    m_cal->takeDay(m_newvalue);
    m_mine = true;
    setCommandType(1);
}

CalendarRemoveDayCmd::CalendarRemoveDayCmd(Part *part, Calendar *cal, const QDate &day, QString name)
    : CalendarAddDayCmd(part, cal, cal->findDay(day), name) {
    //kdDebug()<<k_funcinfo<<cal->name()<<endl;
}
void CalendarRemoveDayCmd::execute() {
    //kdDebug()<<k_funcinfo<<m_cal->name()<<endl;
    CalendarAddDayCmd::unexecute();
}
void CalendarRemoveDayCmd::unexecute() {
    //kdDebug()<<k_funcinfo<<m_cal->name()<<endl;
    CalendarAddDayCmd::execute();
}

CalendarModifyDayCmd::CalendarModifyDayCmd(Part *part, Calendar *cal, CalendarDay *value, QString name)
    : NamedCommand(part, name),
      m_cal(cal),
      m_mine(true) {
      
    m_newvalue = value;
    m_oldvalue = cal->findDay(value->date());
    //kdDebug()<<k_funcinfo<<cal->name()<<" old:("<<m_oldvalue<<") new:("<<m_newvalue<<")"<<endl;
}
CalendarModifyDayCmd::~CalendarModifyDayCmd() {
    //kdDebug()<<k_funcinfo<<endl;
    if (m_mine) {
        delete m_newvalue;
    } else {
        delete m_oldvalue;
    }
}
void CalendarModifyDayCmd::execute() {
    //kdDebug()<<k_funcinfo<<endl;
    m_cal->takeDay(m_oldvalue);
    m_cal->addDay(m_newvalue);
    m_mine = false;
    setCommandType(1);
}
void CalendarModifyDayCmd::unexecute() {
    //kdDebug()<<k_funcinfo<<endl;
    m_cal->takeDay(m_newvalue);
    m_cal->addDay(m_oldvalue);
    m_mine = true;
    setCommandType(1);
}

CalendarModifyWeekdayCmd::CalendarModifyWeekdayCmd(Part *part, Calendar *cal, int weekday, CalendarDay *value, QString name)
    : NamedCommand(part, name),
      m_weekday(weekday),
      m_cal(cal),
      m_mine(true) {
      
    m_value = value;
    kdDebug()<<k_funcinfo<<cal->name()<<" ("<<value<<")"<<endl;
}
CalendarModifyWeekdayCmd::~CalendarModifyWeekdayCmd() {
    kdDebug()<<k_funcinfo<<m_weekday<<": "<<m_value<<endl;
    delete m_value;
    
}
void CalendarModifyWeekdayCmd::execute() {
    kdDebug()<<k_funcinfo<<"set "<<m_weekday<<": "<<m_value<<endl;
    m_value = m_cal->weekdays()->replace(m_weekday, m_value);
    kdDebug()<<k_funcinfo<<"taken "<<m_weekday<<": "<<m_value<<endl;
    setCommandType(1);
}
void CalendarModifyWeekdayCmd::unexecute() {
    execute();
}

NodeDeleteCmd::NodeDeleteCmd(Part *part, Node *node, QString name)
    : NamedCommand(part, name),
      m_node(node),
       m_index(-1) {
    
    m_parent = node->getParent();
    if (m_parent)
        m_index = m_parent->findChildNode(node);
    m_mine = false;
    m_appointments.setAutoDelete(true);
}
NodeDeleteCmd::~NodeDeleteCmd() {
    if (m_mine)
        delete m_node;
}
void NodeDeleteCmd::execute() {
    if (m_parent) {
        //kdDebug()<<k_funcinfo<<m_node->name()<<" "<<m_index<<endl;
        QPtrListIterator<Appointment> it = m_node->appointments();
        for (; it.current(); ++it) {
            it.current()->detach();
            m_appointments.append(it.current());
        }
        m_parent->delChildNode(m_node, false/*take*/);
        m_mine = true;
        
        setCommandType(1);
    }
}
void NodeDeleteCmd::unexecute() {
    if (m_parent) {
        //kdDebug()<<k_funcinfo<<m_node->name()<<" "<<m_index<<endl;
        m_parent->insertChildNode(m_index, m_node);
        Appointment *a;
        for (a = m_appointments.first(); a != 0; m_appointments.take()) {
            a->attach();
        }
        m_mine = false;
        
        setCommandType(1);
    }
}

TaskAddCmd::TaskAddCmd(Part *part, Project *project, Node *node, Node *after,  QString name)
    : NamedCommand(part, name),
      m_project(project),
      m_node(node),
      m_after(after),
      m_added(false) {
      
    // set some reasonable defaults for normally calculated values
    if (after && after->getParent() && after->getParent() != project) {
        node->setStartTime(after->getParent()->startTime());
        node->setEndTime(node->startTime() + node->duration());
    } else {
        if (project->constraint() == Node::MustFinishOn) {
            node->setEndTime(project->endTime());
            node->setStartTime(node->endTime() - node->duration());
        } else {
            node->setStartTime(project->startTime());
            node->setEndTime(node->startTime() + node->duration());
        }
    }
    node->setEarliestStart(node->startTime());
    node->setLatestFinish(node->endTime());
    node->setWorkStartTime(node->startTime());
    node->setWorkEndTime(node->endTime());
}
TaskAddCmd::~TaskAddCmd() {
    if (!m_added)
        delete m_node;
}
void TaskAddCmd::execute() {
    //kdDebug()<<k_funcinfo<<m_node->name()<<endl;
    m_project->addTask(m_node, m_after);
    m_added = true;
    
    setCommandType(1);
}
void TaskAddCmd::unexecute() {
    m_node->getParent()->delChildNode(m_node, false/*take*/);
    m_added = false;
    
    setCommandType(1);
}

SubtaskAddCmd::SubtaskAddCmd(Part *part, Project *project, Node *node, Node *parent,  QString name)
    : NamedCommand(part, name),
      m_project(project),
      m_node(node),
      m_parent(parent),
      m_added(false) {

    // set some reasonable defaults for normally calculated values
    node->setStartTime(parent->startTime());
    node->setEndTime(node->startTime() + node->duration());
    node->setEarliestStart(node->startTime());
    node->setLatestFinish(node->endTime());
    node->setWorkStartTime(node->startTime());
    node->setWorkEndTime(node->endTime());
}
SubtaskAddCmd::~SubtaskAddCmd() {
    if (!m_added)
        delete m_node;
}
void SubtaskAddCmd::execute() {
    m_project->addSubTask(m_node, m_parent);
    m_added = true;
    
    setCommandType(1);
}
void SubtaskAddCmd::unexecute() {
    m_parent->delChildNode(m_node, false/*take*/);
    m_added = false;
    
    setCommandType(1);
}

NodeModifyNameCmd::NodeModifyNameCmd(Part *part, Node &node, QString nodename, QString name)
    : NamedCommand(part, name),
      m_node(node),
      newName(nodename),
      oldName(node.name()) {

}
void NodeModifyNameCmd::execute() {
    m_node.setName(newName);
    
    setCommandType(0);
}
void NodeModifyNameCmd::unexecute() {
    m_node.setName(oldName);
    
    setCommandType(0);
}

NodeModifyLeaderCmd::NodeModifyLeaderCmd(Part *part, Node &node, QString leader, QString name)
    : NamedCommand(part, name),
      m_node(node),
      newLeader(leader),
      oldLeader(node.leader()) {

}
void NodeModifyLeaderCmd::execute() {
    m_node.setLeader(newLeader);
    
    setCommandType(0);
}
void NodeModifyLeaderCmd::unexecute() {
    m_node.setLeader(oldLeader);
    
    setCommandType(0);
}

NodeModifyDescriptionCmd::NodeModifyDescriptionCmd(Part *part, Node &node, QString description, QString name)
    : NamedCommand(part, name),
      m_node(node),
      newDescription(description),
      oldDescription(node.description()) {

}
void NodeModifyDescriptionCmd::execute() {
    m_node.setDescription(newDescription);
    
    setCommandType(0);
}
void NodeModifyDescriptionCmd::unexecute() {
    m_node.setDescription(oldDescription);
    
    setCommandType(0);
}

NodeModifyConstraintCmd::NodeModifyConstraintCmd(Part *part, Node &node, Node::ConstraintType c, QString name)
    : NamedCommand(part, name),
      m_node(node),
      newConstraint(c),
      oldConstraint(static_cast<Node::ConstraintType>(node.constraint())) {

}
void NodeModifyConstraintCmd::execute() {
    m_node.setConstraint(newConstraint);
    
    setCommandType(1);
}
void NodeModifyConstraintCmd::unexecute() {
    m_node.setConstraint(oldConstraint);
    
    setCommandType(1);
}

NodeModifyConstraintStartTimeCmd::NodeModifyConstraintStartTimeCmd(Part *part, Node &node, QDateTime dt, QString name)
    : NamedCommand(part, name),
      m_node(node),
      newTime(dt),
      oldTime(node.constraintStartTime()) {

}
void NodeModifyConstraintStartTimeCmd::execute() {
    m_node.setConstraintStartTime(newTime);
    
    setCommandType(1);
}
void NodeModifyConstraintStartTimeCmd::unexecute() {
    m_node.setConstraintStartTime(oldTime);
    
    setCommandType(1);
}

NodeModifyConstraintEndTimeCmd::NodeModifyConstraintEndTimeCmd(Part *part, Node &node, QDateTime dt, QString name)
    : NamedCommand(part, name),
      m_node(node),
      newTime(dt),
      oldTime(node.constraintEndTime()) {

}
void NodeModifyConstraintEndTimeCmd::execute() {
    m_node.setConstraintEndTime(newTime);
    
    setCommandType(1);
}
void NodeModifyConstraintEndTimeCmd::unexecute() {
    m_node.setConstraintEndTime(oldTime);
    
    setCommandType(1);
}

NodeModifyStartTimeCmd::NodeModifyStartTimeCmd(Part *part, Node &node, QDateTime dt, QString name)
    : NamedCommand(part, name),
      m_node(node),
      newTime(dt),
      oldTime(node.startTime()) {

}
void NodeModifyStartTimeCmd::execute() {
    m_node.setStartTime(newTime);
    
    setCommandType(1);
}
void NodeModifyStartTimeCmd::unexecute() {
    m_node.setStartTime(oldTime);
    
    setCommandType(1);
}

NodeModifyEndTimeCmd::NodeModifyEndTimeCmd(Part *part, Node &node, QDateTime dt, QString name)
    : NamedCommand(part, name),
      m_node(node),
      newTime(dt),
      oldTime(node.endTime()) {

}
void NodeModifyEndTimeCmd::execute() {
    m_node.setEndTime(newTime);
    
    setCommandType(1);
}
void NodeModifyEndTimeCmd::unexecute() {
    m_node.setEndTime(oldTime);
    
    setCommandType(1);
}

NodeModifyIdCmd::NodeModifyIdCmd(Part *part, Node &node, QString id, QString name)
    : NamedCommand(part, name),
      m_node(node),
      newId(id),
      oldId(node.id()) {

}
void NodeModifyIdCmd::execute() {
    m_node.setId(newId);
    
    setCommandType(0);
}
void NodeModifyIdCmd::unexecute() {
    m_node.setId(oldId);
    
    setCommandType(0);
}

NodeIndentCmd::NodeIndentCmd(Part *part, Node &node, QString name)
    : NamedCommand(part, name),
      m_node(node), 
      m_newparent(0), 
      m_newindex(-1) {

}
void NodeIndentCmd::execute() {
    m_oldparent = m_node.getParent();
    m_oldindex = m_oldparent->findChildNode(&m_node);
    Project *p = dynamic_cast<Project *>(m_node.projectNode());
    if (p && p->indentTask(&m_node)) {
        m_newparent = m_node.getParent();
        m_newindex = m_newparent->findChildNode(&m_node);
        m_node.setParent(m_newparent);
    }
    
    setCommandType(1);
}
void NodeIndentCmd::unexecute() {
    if (m_newindex != -1) {
        m_newparent->delChildNode(m_newindex, false);
        m_oldparent->insertChildNode(m_oldindex, &m_node);
        m_node.setParent(m_oldparent);
        m_newindex = -1;
    }
    
    setCommandType(1);
}

NodeUnindentCmd::NodeUnindentCmd(Part *part, Node &node, QString name)
    : NamedCommand(part, name),
      m_node(node), 
      m_newparent(0),  
      m_newindex(-1) {
}
void NodeUnindentCmd::execute() {
    m_oldparent = m_node.getParent();
    m_oldindex = m_oldparent->findChildNode(&m_node);
    Project *p = dynamic_cast<Project *>(m_node.projectNode());
    if (p && p->unindentTask(&m_node)) {
        m_newparent = m_node.getParent();
        m_newindex = m_newparent->findChildNode(&m_node);
        m_node.setParent(m_newparent);
    }
    
    setCommandType(1);
}
void NodeUnindentCmd::unexecute() {
    if (m_newindex != -1) {
        m_newparent->delChildNode(m_newindex, false);
        m_oldparent->insertChildNode(m_oldindex, &m_node);
        m_node.setParent(m_oldparent);
        m_newindex = -1;
    }
    
    setCommandType(1);
}

NodeMoveUpCmd::NodeMoveUpCmd(Part *part, Node &node, QString name)
    : NamedCommand(part, name),
      m_node(node), 
      m_newindex(-1) {
}
void NodeMoveUpCmd::execute() {
    m_oldindex = m_node.getParent()->findChildNode(&m_node);
    Project *p = dynamic_cast<Project *>(m_node.projectNode());
    if (p && p->moveTaskUp(&m_node)) {
        m_newindex = m_node.getParent()->findChildNode(&m_node);
    }
    
    setCommandType(0);
}
void NodeMoveUpCmd::unexecute() {
    if (m_newindex != -1) {
        m_node.getParent()->delChildNode(m_newindex, false);
        m_node.getParent()->insertChildNode(m_oldindex, &m_node);
        m_newindex = -1;
    }
    
    setCommandType(0);
}

NodeMoveDownCmd::NodeMoveDownCmd(Part *part, Node &node, QString name)
    : NamedCommand(part, name),
      m_node(node), 
      m_newindex(-1) {
}
void NodeMoveDownCmd::execute() {
    m_oldindex = m_node.getParent()->findChildNode(&m_node);
    Project *p = dynamic_cast<Project *>(m_node.projectNode());
    if (p && p->moveTaskDown(&m_node)) {
        m_newindex = m_node.getParent()->findChildNode(&m_node);
    }
    
    setCommandType(0);
}
void NodeMoveDownCmd::unexecute() {
    if (m_newindex != -1) {
        m_node.getParent()->delChildNode(m_newindex, false);
        m_node.getParent()->insertChildNode(m_oldindex, &m_node);
        m_newindex = -1;
    }
    
    setCommandType(0);
}

AddRelationCmd::AddRelationCmd(Part *part, Relation *rel, QString name)
    : NamedCommand(part, name),
      m_rel(rel) {
    
    m_taken = true;
}
AddRelationCmd::~AddRelationCmd() {
    if (m_taken)
        delete m_rel;
}
void AddRelationCmd::execute() {
    //kdDebug()<<k_funcinfo<<m_rel->parent()<<" to "<<m_rel->child()<<endl;
    m_taken = false;
    m_rel->parent()->addDependChildNode(m_rel);
    m_rel->child()->addDependParentNode(m_rel);
    
    setCommandType(1);
}
void AddRelationCmd::unexecute() {
    m_taken = true;
    m_rel->parent()->takeDependChildNode(m_rel);
    m_rel->child()->takeDependParentNode(m_rel);
    
    setCommandType(1);
}

DeleteRelationCmd::DeleteRelationCmd(Part *part, Relation *rel, QString name)
    : NamedCommand(part, name),
      m_rel(rel) {
    
    m_taken = false;
}
DeleteRelationCmd::~DeleteRelationCmd() {
    if (m_taken)
        delete m_rel;
}
void DeleteRelationCmd::execute() {
    //kdDebug()<<k_funcinfo<<m_rel->parent()<<" to "<<m_rel->child()<<endl;
    m_taken = true;
    m_rel->parent()->takeDependChildNode(m_rel);
    m_rel->child()->takeDependParentNode(m_rel);
    
    setCommandType(1);
}
void DeleteRelationCmd::unexecute() {
    m_taken = false;
    m_rel->parent()->addDependChildNode(m_rel);
    m_rel->child()->addDependParentNode(m_rel);
    
    setCommandType(1);
}

ModifyRelationTypeCmd::ModifyRelationTypeCmd(Part *part, Relation *rel, Relation::Type type, QString name)
    : NamedCommand(part, name),
      m_rel(rel),
      m_newtype(type) {
    
    m_oldtype = rel->type();
}
void ModifyRelationTypeCmd::execute() {
    m_rel->setType(m_newtype);
    
    setCommandType(1);
}
void ModifyRelationTypeCmd::unexecute() {
    m_rel->setType(m_oldtype);
    
    setCommandType(1);
}

ModifyRelationLagCmd::ModifyRelationLagCmd(Part *part, Relation *rel, Duration lag, QString name)
    : NamedCommand(part, name),
      m_rel(rel),
      m_newlag(lag) {
    
    m_oldlag = rel->lag();
}
void ModifyRelationLagCmd::execute() {
    m_rel->setLag(m_newlag);
    
    setCommandType(1);
}
void ModifyRelationLagCmd::unexecute() {
    m_rel->setLag(m_oldlag);
    
    setCommandType(1);
}

AddResourceRequestCmd::AddResourceRequestCmd(Part *part, ResourceGroupRequest *group, ResourceRequest *request, QString name)
    : NamedCommand(part, name),
      m_group(group),
      m_request(request) {
    
    m_mine = true;
}
AddResourceRequestCmd::~AddResourceRequestCmd() {
    if (m_mine)
        delete m_request;
}
void AddResourceRequestCmd::execute() {
    //kdDebug()<<k_funcinfo<<"group="<<m_group<<" req="<<m_request<<endl;
    m_group->addResourceRequest(m_request);
    m_mine = false;
    
    setCommandType(1);
}
void AddResourceRequestCmd::unexecute() {
    //kdDebug()<<k_funcinfo<<"group="<<m_group<<" req="<<m_request<<endl;
    m_group->takeResourceRequest(m_request);
    m_mine = true;
    
    setCommandType(1);
}

RemoveResourceRequestCmd::RemoveResourceRequestCmd(Part *part,  ResourceGroupRequest *group, ResourceRequest *request, QString name)
    : NamedCommand(part, name),
      m_group(group),
      m_request(request) {
    
    m_mine = false;
    //kdDebug()<<k_funcinfo<<"group="<<group<<" req="<<request<<endl;
}
RemoveResourceRequestCmd::~RemoveResourceRequestCmd() {
    if (m_mine)
        delete m_request;
}
void RemoveResourceRequestCmd::execute() {
    m_group->takeResourceRequest(m_request);
    m_mine = true;
    
    setCommandType(1);
}
void RemoveResourceRequestCmd::unexecute() {
    m_group->addResourceRequest(m_request);
    m_mine = false;
    
    setCommandType(1);
}

ModifyEffortCmd::ModifyEffortCmd(Part *part, Effort *effort, Duration oldvalue, Duration newvalue, QString name)
    : NamedCommand(part, name),
      m_effort(effort),
      m_oldvalue(oldvalue),
      m_newvalue(newvalue) {
}
void ModifyEffortCmd::execute() {
    m_effort->set(m_newvalue);
    
    setCommandType(1);
}
void ModifyEffortCmd::unexecute() {
    m_effort->set(m_oldvalue);
    
    setCommandType(1);
}

EffortModifyOptimisticRatioCmd::EffortModifyOptimisticRatioCmd(Part *part, Effort *effort, int oldvalue, int newvalue, QString name)
    : NamedCommand(part, name),
      m_effort(effort),
      m_oldvalue(oldvalue),
      m_newvalue(newvalue) {
}
void EffortModifyOptimisticRatioCmd::execute() {
    m_effort->setOptimisticRatio(m_newvalue);
    
    setCommandType(1);
}
void EffortModifyOptimisticRatioCmd::unexecute() {
    m_effort->setOptimisticRatio(m_oldvalue);
    
    setCommandType(1);
}

EffortModifyPessimisticRatioCmd::EffortModifyPessimisticRatioCmd(Part *part, Effort *effort, int oldvalue, int newvalue, QString name)
    : NamedCommand(part, name),
      m_effort(effort),
      m_oldvalue(oldvalue),
      m_newvalue(newvalue) {
}
void EffortModifyPessimisticRatioCmd::execute() {
    m_effort->setPessimisticRatio(m_newvalue);
    
    setCommandType(1);
}
void EffortModifyPessimisticRatioCmd::unexecute() {
    m_effort->setPessimisticRatio(m_oldvalue);
    
    setCommandType(1);
}

ModifyEffortTypeCmd::ModifyEffortTypeCmd(Part *part, Effort *effort, int oldvalue, int newvalue, QString name)
    : NamedCommand(part, name),
      m_effort(effort),
      m_oldvalue(oldvalue),
      m_newvalue(newvalue) {
}
void ModifyEffortTypeCmd::execute() {
    m_effort->setType(static_cast<Effort::Type>(m_newvalue));
    
    setCommandType(1);
}
void ModifyEffortTypeCmd::unexecute() {
    m_effort->setType(static_cast<Effort::Type>(m_oldvalue));
    
    setCommandType(1);
}

AddResourceGroupRequestCmd::AddResourceGroupRequestCmd(Part *part, Task &task, ResourceGroupRequest *request, QString name)
    : NamedCommand(part, name),
      m_task(task),
      m_request(request) {
      
    m_mine = true;
}
void AddResourceGroupRequestCmd::execute() {
    //kdDebug()<<k_funcinfo<<"group="<<m_request<<endl;
    m_task.addRequest(m_request);
    m_mine = false;
    
    setCommandType(1);
}
void AddResourceGroupRequestCmd::unexecute() {
    //kdDebug()<<k_funcinfo<<"group="<<m_request<<endl;
    m_task.takeRequest(m_request); // group should now be empty of resourceRequests
    m_mine = true;
    
    setCommandType(1);
}

RemoveResourceGroupRequestCmd::RemoveResourceGroupRequestCmd(Part *part, ResourceGroupRequest *request, QString name)
    : NamedCommand(part, name),
      m_task(request->parent()->task()),
      m_request(request) {
      
    m_mine = false;
}

RemoveResourceGroupRequestCmd::RemoveResourceGroupRequestCmd(Part *part, Task &task, ResourceGroupRequest *request, QString name)
    : NamedCommand(part, name),
      m_task(task),
      m_request(request) {
      
    m_mine = false;
}
void RemoveResourceGroupRequestCmd::execute() {
    //kdDebug()<<k_funcinfo<<"group="<<m_request<<endl;
    m_task.takeRequest(m_request); // group should now be empty of resourceRequests
    m_mine = true;
    
    setCommandType(1);
}
void RemoveResourceGroupRequestCmd::unexecute() {
    //kdDebug()<<k_funcinfo<<"group="<<m_request<<endl;
    m_task.addRequest(m_request);
    m_mine = false;
    
    setCommandType(1);
}

AddResourceCmd::AddResourceCmd(Part *part, ResourceGroup *group, Resource *resource, QString name)
    : NamedCommand(part, name),
      m_group(group),
      m_resource(resource) {
      
    m_mine = true;
}
AddResourceCmd::~AddResourceCmd() {
    if (m_mine) {
        //kdDebug()<<k_funcinfo<<"delete: "<<m_resource<<endl;
        delete m_resource;
    }
}
void AddResourceCmd::execute() {
    m_group->addResource(m_resource, 0/*risk*/); 
    m_mine = false;
    //kdDebug()<<k_funcinfo<<"added: "<<m_resource<<endl;
    setCommandType(0);
}
void AddResourceCmd::unexecute() {
    m_group->takeResource(m_resource);
    //kdDebug()<<k_funcinfo<<"removed: "<<m_resource<<endl;
    m_mine = true;
    
    setCommandType(0);
}

RemoveResourceCmd::RemoveResourceCmd(Part *part, ResourceGroup *group, Resource *resource, QString name)
    : AddResourceCmd(part, group, resource, name) {
    //kdDebug()<<k_funcinfo<<resource<<endl;
    m_mine = false;
    m_requests = m_resource->requests();
    
    
}
RemoveResourceCmd::~RemoveResourceCmd() {
    m_appointments.setAutoDelete(true);
}
void RemoveResourceCmd::execute() {
    QPtrListIterator<ResourceRequest> it = m_requests;
    for (; it.current(); ++it) {
        it.current()->parent()->takeResourceRequest(it.current());
        //kdDebug()<<"Remove request for"<<it.current()->resource()->name()<<endl;
    }
    QPtrListIterator<Appointment> ait = m_resource->appointments();
    for (; ait.current(); ++ait) {
        m_appointments.append(ait.current());
    }
    QPtrListIterator<Appointment> mit = m_appointments;
    for (; mit.current(); ++mit) {
        mit.current()->detach(); //NOTE: removes from m_resource->appointments()
        //kdDebug()<<k_funcinfo<<"detached: "<<mit.current()<<endl;
    }
    AddResourceCmd::unexecute();
}
void RemoveResourceCmd::unexecute() {
    m_appointments.first();
    while (m_appointments.current()) {
        //kdDebug()<<k_funcinfo<<"attach: "<<m_appointments.current()<<endl;
        m_appointments.take()->attach();
    }
    QPtrListIterator<ResourceRequest> it = m_requests;
    for (; it.current(); ++it) {
        it.current()->parent()->addResourceRequest(it.current());
        //kdDebug()<<"Add request for "<<it.current()->resource()->name()<<endl;
    }
    AddResourceCmd::execute();
}

ModifyResourceNameCmd::ModifyResourceNameCmd(Part *part, Resource *resource, QString value, QString name)
    : NamedCommand(part, name),
      m_resource(resource),
      m_newvalue(value) {
    m_oldvalue = resource->name();
}
void ModifyResourceNameCmd::execute() {
    m_resource->setName(m_newvalue);
    
    setCommandType(0);
}
void ModifyResourceNameCmd::unexecute() {
    m_resource->setName(m_oldvalue);
    
    setCommandType(0);
}
ModifyResourceInitialsCmd::ModifyResourceInitialsCmd(Part *part, Resource *resource, QString value, QString name)
    : NamedCommand(part, name),
      m_resource(resource),
      m_newvalue(value) {
    m_oldvalue = resource->initials();
}
void ModifyResourceInitialsCmd::execute() {
    m_resource->setInitials(m_newvalue);
    
    setCommandType(0);
}
void ModifyResourceInitialsCmd::unexecute() {
    m_resource->setInitials(m_oldvalue);
    
    setCommandType(0);
}
ModifyResourceEmailCmd::ModifyResourceEmailCmd(Part *part, Resource *resource, QString value, QString name)
    : NamedCommand(part, name),
      m_resource(resource),
      m_newvalue(value) {
    m_oldvalue = resource->email();
}
void ModifyResourceEmailCmd::execute() {
    m_resource->setEmail(m_newvalue);
    
    setCommandType(0);
}
void ModifyResourceEmailCmd::unexecute() {
    m_resource->setEmail(m_oldvalue);
    
    setCommandType(0);
}
ModifyResourceTypeCmd::ModifyResourceTypeCmd(Part *part, Resource *resource, int value, QString name)
    : NamedCommand(part, name),
      m_resource(resource),
      m_newvalue(value) {
    m_oldvalue = resource->type();
}
void ModifyResourceTypeCmd::execute() {
    m_resource->setType((Resource::Type)m_newvalue);
    
    setCommandType(0); //FIXME
}
void ModifyResourceTypeCmd::unexecute() {
    m_resource->setType((Resource::Type)m_oldvalue);
    
    setCommandType(0); //FIXME
}

ModifyResourceAvailableFromCmd::ModifyResourceAvailableFromCmd(Part *part, Resource *resource, DateTime value, QString name)
    : NamedCommand(part, name),
      m_resource(resource),
      m_newvalue(value) {
    m_oldvalue = resource->availableFrom();
}
void ModifyResourceAvailableFromCmd::execute() {
    m_resource->setAvailableFrom(m_newvalue);
    
    setCommandType(1); //FIXME
}
void ModifyResourceAvailableFromCmd::unexecute() {
    m_resource->setAvailableFrom(m_oldvalue);
    
    setCommandType(1); //FIXME
}

ModifyResourceAvailableUntilCmd::ModifyResourceAvailableUntilCmd(Part *part, Resource *resource, DateTime value, QString name)
    : NamedCommand(part, name),
      m_resource(resource),
      m_newvalue(value) {
    m_oldvalue = resource->availableUntil();
}
void ModifyResourceAvailableUntilCmd::execute() {
    m_resource->setAvailableUntil(m_newvalue);
    
    setCommandType(1); //FIXME
}
void ModifyResourceAvailableUntilCmd::unexecute() {
    m_resource->setAvailableUntil(m_oldvalue);
    
    setCommandType(1); //FIXME
}

ModifyResourceNormalRateCmd::ModifyResourceNormalRateCmd(Part *part, Resource *resource, double value, QString name)
    : NamedCommand(part, name),
      m_resource(resource),
      m_newvalue(value) {
    m_oldvalue = resource->normalRate();
}
void ModifyResourceNormalRateCmd::execute() {
    m_resource->setNormalRate(m_newvalue);
    
    setCommandType(0);
}
void ModifyResourceNormalRateCmd::unexecute() {
    m_resource->setNormalRate(m_oldvalue);
    
    setCommandType(0);
}
ModifyResourceOvertimeRateCmd::ModifyResourceOvertimeRateCmd(Part *part, Resource *resource, double value, QString name)
    : NamedCommand(part, name),
      m_resource(resource),
      m_newvalue(value) {
    m_oldvalue = resource->overtimeRate();
}
void ModifyResourceOvertimeRateCmd::execute() {
    m_resource->setOvertimeRate(m_newvalue);
    
    setCommandType(0);
}
void ModifyResourceOvertimeRateCmd::unexecute() {
    m_resource->setOvertimeRate(m_oldvalue);
    
    setCommandType(0);
}

ModifyResourceCalendarCmd::ModifyResourceCalendarCmd(Part *part, Resource *resource, Calendar *value, QString name)
    : NamedCommand(part, name),
      m_resource(resource),
      m_newvalue(value) {
    m_oldvalue = resource->calendar(true);
}
void ModifyResourceCalendarCmd::execute() {
    m_resource->setCalendar(m_newvalue);
    
    setCommandType(1);
}
void ModifyResourceCalendarCmd::unexecute() {
    m_resource->setCalendar(m_oldvalue);
    
    setCommandType(1);
}

RemoveResourceGroupCmd::RemoveResourceGroupCmd(Part *part, ResourceGroup *group, QString name)
    : NamedCommand(part, name),
      m_group(group) {
            
    m_mine = false;
}
RemoveResourceGroupCmd::~RemoveResourceGroupCmd() {
    if (m_mine)
        delete m_group;
}
void RemoveResourceGroupCmd::execute() {
    // remove all requests to this group
    int c=0;
    QPtrListIterator<ResourceGroupRequest> it = m_group->requests();
    for (; it.current(); ++it) {
        if (it.current()->parent()) {
            it.current()->parent()->takeRequest(it.current());
        }
        c = 1;
    }
    if (m_group->project())
        m_group->project()->takeResourceGroup(m_group);
    m_mine = true;
    
    setCommandType(c);
}
void RemoveResourceGroupCmd::unexecute() {
    // add all requests
    int c=0;
    QPtrListIterator<ResourceGroupRequest> it = m_group->requests();
    for (; it.current(); ++it) {
        if (it.current()->parent()) {
            it.current()->parent()->addRequest(it.current());
        }
        c = 1;
    }
    if (m_group->project())
        m_group->project()->addResourceGroup(m_group);
        
    m_mine = false;
    
    setCommandType(c);
}

AddResourceGroupCmd::AddResourceGroupCmd(Part *part, ResourceGroup *group, QString name)
    : RemoveResourceGroupCmd(part, group, name) {
                
    m_mine = true;
}
void AddResourceGroupCmd::execute() {
    RemoveResourceGroupCmd::unexecute();
}
void AddResourceGroupCmd::unexecute() {
    RemoveResourceGroupCmd::execute();
}

ModifyResourceGroupNameCmd::ModifyResourceGroupNameCmd(Part *part, ResourceGroup *group, QString value, QString name)
    : NamedCommand(part, name),
      m_group(group),
      m_newvalue(value) {
    m_oldvalue = group->name();
}
void ModifyResourceGroupNameCmd::execute() {
    m_group->setName(m_newvalue);
    
    setCommandType(0);
}
void ModifyResourceGroupNameCmd::unexecute() {
    m_group->setName(m_oldvalue);
    
    setCommandType(0);
}

TaskModifyProgressCmd::TaskModifyProgressCmd(Part *part, Task &task, struct Task::Progress &value, QString name)
    : NamedCommand(part, name),
      m_task(task),
      m_newvalue(value) {
    m_oldvalue = task.progress();
}
void TaskModifyProgressCmd::execute() {
    m_task.progress() = m_newvalue;
    
    setCommandType(0);
}
void TaskModifyProgressCmd::unexecute() {
    m_task.progress() = m_oldvalue;
    
    setCommandType(0);
}

ProjectModifyBaselineCmd::ProjectModifyBaselineCmd(Part *part, Project &project, bool value, QString name)
    : NamedCommand(part, name),
      m_project(project),
      m_newvalue(value) {
    m_oldvalue = project.isBaselined();
}
void ProjectModifyBaselineCmd::execute() {
    m_project.setBaselined(m_newvalue);
    
    setCommandType(2);
}
void ProjectModifyBaselineCmd::unexecute() {
    m_project.setBaselined(m_oldvalue);
    
    setCommandType(2);
}

AddAccountCmd::AddAccountCmd(Part *part, Project &project, Account *account, QString parent, QString name)
    : NamedCommand(part, name),
      m_project(project),
      m_account(account),
      m_parent(0),
      m_parentName(parent) {
    m_mine = true;
}

AddAccountCmd::AddAccountCmd(Part *part, Project &project, Account *account, Account *parent, QString name)
    : NamedCommand(part, name),
      m_project(project),
      m_account(account),
      m_parent(parent) {
    m_mine = true;
}

AddAccountCmd::~AddAccountCmd() {
    if (m_mine)
        delete m_account;
}

void AddAccountCmd::execute() {
    if (m_parent == 0 && !m_parentName.isEmpty()) {
        m_parent = m_project.accounts().findAccount(m_parentName);
    }
    if (m_parent)
        m_parent->append(m_account);
    else
        m_project.accounts().append(m_account);
    
    setCommandType(0);
    m_mine = false;
}
void AddAccountCmd::unexecute() {
    if (m_parent)
        m_parent->take(m_account);
    else
        m_project.accounts().take(m_account);
    
    setCommandType(0);
    m_mine = true;
}

RemoveAccountCmd::RemoveAccountCmd(Part *part, Project &project, Account *account, QString name)
    : NamedCommand(part, name),
      m_project(project),
      m_account(account) {
    m_mine = false;
    m_isDefault = account == project.accounts().defaultAccount();
}

RemoveAccountCmd::~RemoveAccountCmd() {
    if (m_mine)
        delete m_account;
}

void RemoveAccountCmd::execute() {
    if (m_isDefault) {
        m_project.accounts().setDefaultAccount(0);
    }
    if (m_account->parent())
        m_account->parent()->take(m_account);
    else
        m_project.accounts().take(m_account);
    
    setCommandType(0);
    m_mine = true;
}
void RemoveAccountCmd::unexecute() {
    if (m_account->parent())
        m_account->parent()->append(m_account);
    else
        m_project.accounts().append(m_account);
    
    if (m_isDefault)
        m_project.accounts().setDefaultAccount(m_account);
    
    setCommandType(0);
    m_mine = false;
}

RenameAccountCmd::RenameAccountCmd(Part *part, Account *account, QString value, QString name)
    : NamedCommand(part, name),
      m_account(account) {
    m_oldvalue = account->name();
    m_newvalue = value;
}

void RenameAccountCmd::execute() {
    m_account->setName(m_newvalue);        
    setCommandType(0);
}
void RenameAccountCmd::unexecute() {
    m_account->setName(m_oldvalue);
    setCommandType(0);
}

ModifyAccountDescriptionCmd::ModifyAccountDescriptionCmd(Part *part, Account *account, QString value, QString name)
    : NamedCommand(part, name),
      m_account(account) {
    m_oldvalue = account->description();
    m_newvalue = value;
}

void ModifyAccountDescriptionCmd::execute() {
    m_account->setDescription(m_newvalue);        
    setCommandType(0);
}
void ModifyAccountDescriptionCmd::unexecute() {
    m_account->setDescription(m_oldvalue);
    setCommandType(0);
}


NodeModifyStartupCostCmd::NodeModifyStartupCostCmd(Part *part, Node &node, double value, QString name)
    : NamedCommand(part, name),
      m_node(node) {
    m_oldvalue = node.startupCost();
    m_newvalue = value;
}

void NodeModifyStartupCostCmd::execute() {
    m_node.setStartupCost(m_newvalue);        
    setCommandType(0);
}
void NodeModifyStartupCostCmd::unexecute() {
    m_node.setStartupCost(m_oldvalue);
    setCommandType(0);
}

NodeModifyShutdownCostCmd::NodeModifyShutdownCostCmd(Part *part, Node &node, double value, QString name)
    : NamedCommand(part, name),
      m_node(node) {
    m_oldvalue = node.startupCost();
    m_newvalue = value;
}

void NodeModifyShutdownCostCmd::execute() {
    m_node.setShutdownCost(m_newvalue);        
    setCommandType(0);
}
void NodeModifyShutdownCostCmd::unexecute() {
    m_node.setShutdownCost(m_oldvalue);
    setCommandType(0);
}

NodeModifyRunningAccountCmd::NodeModifyRunningAccountCmd(Part *part, Node &node, Account *oldvalue, Account *newvalue, QString name)
    : NamedCommand(part, name),
      m_node(node) {
    m_oldvalue = oldvalue;
    m_newvalue = newvalue;
    //kdDebug()<<k_funcinfo<<endl;
}
void NodeModifyRunningAccountCmd::execute() {
    //kdDebug()<<k_funcinfo<<endl;
    if (m_oldvalue) {
        m_oldvalue->removeRunning(m_node);
    }
    if (m_newvalue) {
        m_newvalue->addRunning(m_node);        
    }
    setCommandType(0);
}
void NodeModifyRunningAccountCmd::unexecute() {
    //kdDebug()<<k_funcinfo<<endl;
    if (m_newvalue) {
        m_newvalue->removeRunning(m_node);        
    }
    if (m_oldvalue) {
        m_oldvalue->addRunning(m_node);
    }
    setCommandType(0);
}

NodeModifyStartupAccountCmd::NodeModifyStartupAccountCmd(Part *part, Node &node, Account *oldvalue, Account *newvalue, QString name)
    : NamedCommand(part, name),
      m_node(node) {
    m_oldvalue = oldvalue;
    m_newvalue = newvalue;
    //kdDebug()<<k_funcinfo<<endl;
}

void NodeModifyStartupAccountCmd::execute() {
    //kdDebug()<<k_funcinfo<<endl;
    if (m_oldvalue) {
        m_oldvalue->removeStartup(m_node);
    }
    if (m_newvalue) {
        m_newvalue->addStartup(m_node);        
    }
    setCommandType(0);
}
void NodeModifyStartupAccountCmd::unexecute() {
    //kdDebug()<<k_funcinfo<<endl;
    if (m_newvalue) {
        m_newvalue->removeStartup(m_node);
    }
    if (m_oldvalue) {
        m_oldvalue->addStartup(m_node);        
    }
    setCommandType(0);
}

NodeModifyShutdownAccountCmd::NodeModifyShutdownAccountCmd(Part *part, Node &node, Account *oldvalue, Account *newvalue, QString name)
    : NamedCommand(part, name),
      m_node(node) {
    m_oldvalue = oldvalue;
    m_newvalue = newvalue;
    //kdDebug()<<k_funcinfo<<endl;
}

void NodeModifyShutdownAccountCmd::execute() {
    //kdDebug()<<k_funcinfo<<endl;
    if (m_oldvalue) {
        m_oldvalue->removeShutdown(m_node);
    }
    if (m_newvalue) {
        m_newvalue->addShutdown(m_node);
    }
    setCommandType(0);
}
void NodeModifyShutdownAccountCmd::unexecute() {
    //kdDebug()<<k_funcinfo<<endl;
    if (m_newvalue) {
        m_newvalue->removeShutdown(m_node);
    }
    if (m_oldvalue) {
        m_oldvalue->addShutdown(m_node);
    }
    setCommandType(0);
}

ModifyDefaultAccountCmd::ModifyDefaultAccountCmd(Part *part, Accounts &acc, Account *oldvalue, Account *newvalue, QString name)
    : NamedCommand(part, name),
      m_accounts(acc) {
    m_oldvalue = oldvalue;
    m_newvalue = newvalue;
    //kdDebug()<<k_funcinfo<<endl;
}

void ModifyDefaultAccountCmd::execute() {
    //kdDebug()<<k_funcinfo<<endl;
    m_accounts.setDefaultAccount(m_newvalue);
    setCommandType(0);
}
void ModifyDefaultAccountCmd::unexecute() {
    //kdDebug()<<k_funcinfo<<endl;
    m_accounts.setDefaultAccount(m_oldvalue);
    setCommandType(0);
}


}  //KPlato namespace
