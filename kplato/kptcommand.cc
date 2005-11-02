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

KPTCalendarAddCmd::KPTCalendarAddCmd(KPTPart *part, KPTProject *project,KPTCalendar *cal, QString name)
    : KPTNamedCommand(part, name),
      m_project(project),
      m_cal(cal),
      m_added(false) {
    cal->setDeleted(true);  
    //kdDebug()<<k_funcinfo<<cal->name()<<endl;
}

void KPTCalendarAddCmd::execute() {
    if (!m_added && m_project) {
        m_project->addCalendar(m_cal);
        m_added = true;
    }
    m_cal->setDeleted(false);
    
    setCommandType(0);
    //kdDebug()<<k_funcinfo<<m_cal->name()<<" added to: "<<m_project->name()<<endl;
}

void KPTCalendarAddCmd::unexecute() {
    m_cal->setDeleted(true);
    
    setCommandType(0);
    //kdDebug()<<k_funcinfo<<m_cal->name()<<endl;
}

KPTCalendarDeleteCmd::KPTCalendarDeleteCmd(KPTPart *part, KPTCalendar *cal, QString name)
    : KPTNamedCommand(part, name),
      m_cal(cal) {
}

void KPTCalendarDeleteCmd::execute() {
    m_cal->setDeleted(true);
    
    setCommandType(0);
}

void KPTCalendarDeleteCmd::unexecute() {
    m_cal->setDeleted(false);
    
    setCommandType(0);
}

KPTNodeDeleteCmd::KPTNodeDeleteCmd(KPTPart *part, KPTNode *node, QString name)
    : KPTNamedCommand(part, name),
      m_node(node),
       m_index(-1) {
    
    m_parent = node->getParent();
    if (m_parent)
        m_index = m_parent->findChildNode(node);
    m_mine = false;
    m_appointments.setAutoDelete(true);
}
KPTNodeDeleteCmd::~KPTNodeDeleteCmd() {
    if (m_mine)
        delete m_node;
}
void KPTNodeDeleteCmd::execute() {
    if (m_parent) {
        //kdDebug()<<k_funcinfo<<m_node->name()<<" "<<m_index<<endl;
        QPtrListIterator<KPTAppointment> it = m_node->appointments();
        for (; it.current(); ++it) {
            it.current()->detach();
            m_appointments.append(it.current());
        }
        m_parent->delChildNode(m_node, false/*take*/);
        m_mine = true;
        
        setCommandType(1);
    }
}
void KPTNodeDeleteCmd::unexecute() {
    if (m_parent) {
        //kdDebug()<<k_funcinfo<<m_node->name()<<" "<<m_index<<endl;
        m_parent->insertChildNode(m_index, m_node);
        KPTAppointment *a;
        for (a = m_appointments.first(); a != 0; m_appointments.take()) {
            a->attach();
        }
        m_mine = false;
        
        setCommandType(1);
    }
}

KPTTaskAddCmd::KPTTaskAddCmd(KPTPart *part, KPTProject *project, KPTNode *node, KPTNode *after,  QString name)
    : KPTNamedCommand(part, name),
      m_project(project),
      m_node(node),
      m_after(after),
      m_added(false) {
      
    // set some reasonable defaults for normally calculated values
    if (after && after->getParent() && after->getParent() != project) {
        node->setStartTime(after->getParent()->startTime());
        node->setEndTime(node->startTime() + node->duration());
    } else {
        if (project->constraint() == KPTNode::MustFinishOn) {
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
KPTTaskAddCmd::~KPTTaskAddCmd() {
    if (!m_added)
        delete m_node;
}
void KPTTaskAddCmd::execute() {
    //kdDebug()<<k_funcinfo<<m_node->name()<<endl;
    m_project->addTask(m_node, m_after);
    m_added = true;
    
    setCommandType(1);
}
void KPTTaskAddCmd::unexecute() {
    m_node->getParent()->delChildNode(m_node, false/*take*/);
    m_added = false;
    
    setCommandType(1);
}

KPTSubtaskAddCmd::KPTSubtaskAddCmd(KPTPart *part, KPTProject *project, KPTNode *node, KPTNode *parent,  QString name)
    : KPTNamedCommand(part, name),
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
KPTSubtaskAddCmd::~KPTSubtaskAddCmd() {
    if (!m_added)
        delete m_node;
}
void KPTSubtaskAddCmd::execute() {
    m_project->addSubTask(m_node, m_parent);
    m_added = true;
    
    setCommandType(1);
}
void KPTSubtaskAddCmd::unexecute() {
    m_parent->delChildNode(m_node, false/*take*/);
    m_added = false;
    
    setCommandType(1);
}

KPTNodeModifyNameCmd::KPTNodeModifyNameCmd(KPTPart *part, KPTNode &node, QString nodename, QString name)
    : KPTNamedCommand(part, name),
      m_node(node),
      newName(nodename),
      oldName(node.name()) {

}
void KPTNodeModifyNameCmd::execute() {
    m_node.setName(newName);
    
    setCommandType(0);
}
void KPTNodeModifyNameCmd::unexecute() {
    m_node.setName(oldName);
    
    setCommandType(0);
}

KPTNodeModifyLeaderCmd::KPTNodeModifyLeaderCmd(KPTPart *part, KPTNode &node, QString leader, QString name)
    : KPTNamedCommand(part, name),
      m_node(node),
      newLeader(leader),
      oldLeader(node.leader()) {

}
void KPTNodeModifyLeaderCmd::execute() {
    m_node.setLeader(newLeader);
    
    setCommandType(0);
}
void KPTNodeModifyLeaderCmd::unexecute() {
    m_node.setLeader(oldLeader);
    
    setCommandType(0);
}

KPTNodeModifyDescriptionCmd::KPTNodeModifyDescriptionCmd(KPTPart *part, KPTNode &node, QString description, QString name)
    : KPTNamedCommand(part, name),
      m_node(node),
      newDescription(description),
      oldDescription(node.description()) {

}
void KPTNodeModifyDescriptionCmd::execute() {
    m_node.setDescription(newDescription);
    
    setCommandType(0);
}
void KPTNodeModifyDescriptionCmd::unexecute() {
    m_node.setDescription(oldDescription);
    
    setCommandType(0);
}

KPTNodeModifyConstraintCmd::KPTNodeModifyConstraintCmd(KPTPart *part, KPTNode &node, KPTNode::ConstraintType c, QString name)
    : KPTNamedCommand(part, name),
      m_node(node),
      newConstraint(c),
      oldConstraint(static_cast<KPTNode::ConstraintType>(node.constraint())) {

}
void KPTNodeModifyConstraintCmd::execute() {
    m_node.setConstraint(newConstraint);
    
    setCommandType(1);
}
void KPTNodeModifyConstraintCmd::unexecute() {
    m_node.setConstraint(oldConstraint);
    
    setCommandType(1);
}

KPTNodeModifyConstraintStartTimeCmd::KPTNodeModifyConstraintStartTimeCmd(KPTPart *part, KPTNode &node, QDateTime dt, QString name)
    : KPTNamedCommand(part, name),
      m_node(node),
      newTime(dt),
      oldTime(node.constraintStartTime()) {

}
void KPTNodeModifyConstraintStartTimeCmd::execute() {
    m_node.setConstraintStartTime(newTime);
    
    setCommandType(1);
}
void KPTNodeModifyConstraintStartTimeCmd::unexecute() {
    m_node.setConstraintStartTime(oldTime);
    
    setCommandType(1);
}

KPTNodeModifyConstraintEndTimeCmd::KPTNodeModifyConstraintEndTimeCmd(KPTPart *part, KPTNode &node, QDateTime dt, QString name)
    : KPTNamedCommand(part, name),
      m_node(node),
      newTime(dt),
      oldTime(node.constraintEndTime()) {

}
void KPTNodeModifyConstraintEndTimeCmd::execute() {
    m_node.setConstraintEndTime(newTime);
    
    setCommandType(1);
}
void KPTNodeModifyConstraintEndTimeCmd::unexecute() {
    m_node.setConstraintEndTime(oldTime);
    
    setCommandType(1);
}

KPTNodeModifyStartTimeCmd::KPTNodeModifyStartTimeCmd(KPTPart *part, KPTNode &node, QDateTime dt, QString name)
    : KPTNamedCommand(part, name),
      m_node(node),
      newTime(dt),
      oldTime(node.startTime()) {

}
void KPTNodeModifyStartTimeCmd::execute() {
    m_node.setStartTime(newTime);
    
    setCommandType(1);
}
void KPTNodeModifyStartTimeCmd::unexecute() {
    m_node.setStartTime(oldTime);
    
    setCommandType(1);
}

KPTNodeModifyEndTimeCmd::KPTNodeModifyEndTimeCmd(KPTPart *part, KPTNode &node, QDateTime dt, QString name)
    : KPTNamedCommand(part, name),
      m_node(node),
      newTime(dt),
      oldTime(node.endTime()) {

}
void KPTNodeModifyEndTimeCmd::execute() {
    m_node.setEndTime(newTime);
    
    setCommandType(1);
}
void KPTNodeModifyEndTimeCmd::unexecute() {
    m_node.setEndTime(oldTime);
    
    setCommandType(1);
}

KPTNodeModifyIdCmd::KPTNodeModifyIdCmd(KPTPart *part, KPTNode &node, QString id, QString name)
    : KPTNamedCommand(part, name),
      m_node(node),
      newId(id),
      oldId(node.id()) {

}
void KPTNodeModifyIdCmd::execute() {
    m_node.setId(newId);
    
    setCommandType(0);
}
void KPTNodeModifyIdCmd::unexecute() {
    m_node.setId(oldId);
    
    setCommandType(0);
}

KPTNodeIndentCmd::KPTNodeIndentCmd(KPTPart *part, KPTNode &node, QString name)
    : KPTNamedCommand(part, name),
      m_node(node), 
      m_newparent(0), 
      m_newindex(-1) {

}
void KPTNodeIndentCmd::execute() {
    m_oldparent = m_node.getParent();
    m_oldindex = m_oldparent->findChildNode(&m_node);
    KPTProject *p = dynamic_cast<KPTProject *>(m_node.projectNode());
    if (p && p->indentTask(&m_node)) {
        m_newparent = m_node.getParent();
        m_newindex = m_newparent->findChildNode(&m_node);
        m_node.setParent(m_newparent);
    }
    
    setCommandType(1);
}
void KPTNodeIndentCmd::unexecute() {
    if (m_newindex != -1) {
        m_newparent->delChildNode(m_newindex, false);
        m_oldparent->insertChildNode(m_oldindex, &m_node);
        m_node.setParent(m_oldparent);
        m_newindex = -1;
    }
    
    setCommandType(1);
}

KPTNodeUnindentCmd::KPTNodeUnindentCmd(KPTPart *part, KPTNode &node, QString name)
    : KPTNamedCommand(part, name),
      m_node(node), 
      m_newparent(0),  
      m_newindex(-1) {
}
void KPTNodeUnindentCmd::execute() {
    m_oldparent = m_node.getParent();
    m_oldindex = m_oldparent->findChildNode(&m_node);
    KPTProject *p = dynamic_cast<KPTProject *>(m_node.projectNode());
    if (p && p->unindentTask(&m_node)) {
        m_newparent = m_node.getParent();
        m_newindex = m_newparent->findChildNode(&m_node);
        m_node.setParent(m_newparent);
    }
    
    setCommandType(1);
}
void KPTNodeUnindentCmd::unexecute() {
    if (m_newindex != -1) {
        m_newparent->delChildNode(m_newindex, false);
        m_oldparent->insertChildNode(m_oldindex, &m_node);
        m_node.setParent(m_oldparent);
        m_newindex = -1;
    }
    
    setCommandType(1);
}

KPTNodeMoveUpCmd::KPTNodeMoveUpCmd(KPTPart *part, KPTNode &node, QString name)
    : KPTNamedCommand(part, name),
      m_node(node), 
      m_newindex(-1) {
}
void KPTNodeMoveUpCmd::execute() {
    m_oldindex = m_node.getParent()->findChildNode(&m_node);
    KPTProject *p = dynamic_cast<KPTProject *>(m_node.projectNode());
    if (p && p->moveTaskUp(&m_node)) {
        m_newindex = m_node.getParent()->findChildNode(&m_node);
    }
    
    setCommandType(0);
}
void KPTNodeMoveUpCmd::unexecute() {
    if (m_newindex != -1) {
        m_node.getParent()->delChildNode(m_newindex, false);
        m_node.getParent()->insertChildNode(m_oldindex, &m_node);
        m_newindex = -1;
    }
    
    setCommandType(0);
}

KPTNodeMoveDownCmd::KPTNodeMoveDownCmd(KPTPart *part, KPTNode &node, QString name)
    : KPTNamedCommand(part, name),
      m_node(node), 
      m_newindex(-1) {
}
void KPTNodeMoveDownCmd::execute() {
    m_oldindex = m_node.getParent()->findChildNode(&m_node);
    KPTProject *p = dynamic_cast<KPTProject *>(m_node.projectNode());
    if (p && p->moveTaskDown(&m_node)) {
        m_newindex = m_node.getParent()->findChildNode(&m_node);
    }
    
    setCommandType(0);
}
void KPTNodeMoveDownCmd::unexecute() {
    if (m_newindex != -1) {
        m_node.getParent()->delChildNode(m_newindex, false);
        m_node.getParent()->insertChildNode(m_oldindex, &m_node);
        m_newindex = -1;
    }
    
    setCommandType(0);
}

KPTAddRelationCmd::KPTAddRelationCmd(KPTPart *part, KPTRelation *rel, QString name)
    : KPTNamedCommand(part, name),
      m_rel(rel) {
    
    m_taken = true;
}
KPTAddRelationCmd::~KPTAddRelationCmd() {
    if (m_taken)
        delete m_rel;
}
void KPTAddRelationCmd::execute() {
    //kdDebug()<<k_funcinfo<<m_rel->parent()<<" to "<<m_rel->child()<<endl;
    m_taken = false;
    m_rel->parent()->addDependChildNode(m_rel);
    m_rel->child()->addDependParentNode(m_rel);
    
    setCommandType(1);
}
void KPTAddRelationCmd::unexecute() {
    m_taken = true;
    m_rel->parent()->takeDependChildNode(m_rel);
    m_rel->child()->takeDependParentNode(m_rel);
    
    setCommandType(1);
}

KPTDeleteRelationCmd::KPTDeleteRelationCmd(KPTPart *part, KPTRelation *rel, QString name)
    : KPTNamedCommand(part, name),
      m_rel(rel) {
    
    m_taken = false;
}
KPTDeleteRelationCmd::~KPTDeleteRelationCmd() {
    if (m_taken)
        delete m_rel;
}
void KPTDeleteRelationCmd::execute() {
    //kdDebug()<<k_funcinfo<<m_rel->parent()<<" to "<<m_rel->child()<<endl;
    m_taken = true;
    m_rel->parent()->takeDependChildNode(m_rel);
    m_rel->child()->takeDependParentNode(m_rel);
    
    setCommandType(1);
}
void KPTDeleteRelationCmd::unexecute() {
    m_taken = false;
    m_rel->parent()->addDependChildNode(m_rel);
    m_rel->child()->addDependParentNode(m_rel);
    
    setCommandType(1);
}

KPTModifyRelationTypeCmd::KPTModifyRelationTypeCmd(KPTPart *part, KPTRelation *rel, KPTRelation::Type type, QString name)
    : KPTNamedCommand(part, name),
      m_rel(rel),
      m_newtype(type) {
    
    m_oldtype = rel->type();
}
void KPTModifyRelationTypeCmd::execute() {
    m_rel->setType(m_newtype);
    
    setCommandType(1);
}
void KPTModifyRelationTypeCmd::unexecute() {
    m_rel->setType(m_oldtype);
    
    setCommandType(1);
}

KPTModifyRelationLagCmd::KPTModifyRelationLagCmd(KPTPart *part, KPTRelation *rel, KPTDuration lag, QString name)
    : KPTNamedCommand(part, name),
      m_rel(rel),
      m_newlag(lag) {
    
    m_oldlag = rel->lag();
}
void KPTModifyRelationLagCmd::execute() {
    m_rel->setLag(m_newlag);
    
    setCommandType(1);
}
void KPTModifyRelationLagCmd::unexecute() {
    m_rel->setLag(m_oldlag);
    
    setCommandType(1);
}

KPTAddResourceRequestCmd::KPTAddResourceRequestCmd(KPTPart *part, KPTResourceGroupRequest *group, KPTResourceRequest *request, QString name)
    : KPTNamedCommand(part, name),
      m_group(group),
      m_request(request) {
    
    m_mine = true;
}
KPTAddResourceRequestCmd::~KPTAddResourceRequestCmd() {
    if (m_mine)
        delete m_request;
}
void KPTAddResourceRequestCmd::execute() {
    //kdDebug()<<k_funcinfo<<"group="<<m_group<<" req="<<m_request<<endl;
    m_group->addResourceRequest(m_request);
    m_mine = false;
    
    setCommandType(1);
}
void KPTAddResourceRequestCmd::unexecute() {
    //kdDebug()<<k_funcinfo<<"group="<<m_group<<" req="<<m_request<<endl;
    m_group->takeResourceRequest(m_request);
    m_mine = true;
    
    setCommandType(1);
}

KPTRemoveResourceRequestCmd::KPTRemoveResourceRequestCmd(KPTPart *part,  KPTResourceGroupRequest *group, KPTResourceRequest *request, QString name)
    : KPTNamedCommand(part, name),
      m_group(group),
      m_request(request) {
    
    m_mine = false;
    //kdDebug()<<k_funcinfo<<"group="<<group<<" req="<<request<<endl;
}
KPTRemoveResourceRequestCmd::~KPTRemoveResourceRequestCmd() {
    if (m_mine)
        delete m_request;
}
void KPTRemoveResourceRequestCmd::execute() {
    m_group->takeResourceRequest(m_request);
    m_mine = true;
    
    setCommandType(1);
}
void KPTRemoveResourceRequestCmd::unexecute() {
    m_group->addResourceRequest(m_request);
    m_mine = false;
    
    setCommandType(1);
}

KPTModifyResourceRequestAccountCmd::KPTModifyResourceRequestAccountCmd(KPTPart *part, KPTResourceRequest *request, QString account, QString name)
    : KPTNamedCommand(part, name),
      m_request(request) {    
    m_newaccount = part->getProject().accounts().findAccount(account);
    m_oldaccount = m_request->account();
    //kdDebug()<<k_funcinfo<<"group="<<group<<" req="<<request<<endl;
}

KPTModifyResourceRequestAccountCmd::~KPTModifyResourceRequestAccountCmd() {
}

void KPTModifyResourceRequestAccountCmd::execute() {
    m_request->setAccount(m_newaccount);
    
    setCommandType(1);
}
void KPTModifyResourceRequestAccountCmd::unexecute() {
    m_request->setAccount(m_oldaccount);
    
    setCommandType(1);
}

KPTModifyEffortCmd::KPTModifyEffortCmd(KPTPart *part, KPTEffort *effort, KPTDuration oldvalue, KPTDuration newvalue, QString name)
    : KPTNamedCommand(part, name),
      m_effort(effort),
      m_oldvalue(oldvalue),
      m_newvalue(newvalue) {
}
void KPTModifyEffortCmd::execute() {
    m_effort->set(m_newvalue);
    
    setCommandType(1);
}
void KPTModifyEffortCmd::unexecute() {
    m_effort->set(m_oldvalue);
    
    setCommandType(1);
}

KPTEffortModifyOptimisticRatioCmd::KPTEffortModifyOptimisticRatioCmd(KPTPart *part, KPTEffort *effort, int oldvalue, int newvalue, QString name)
    : KPTNamedCommand(part, name),
      m_effort(effort),
      m_oldvalue(oldvalue),
      m_newvalue(newvalue) {
}
void KPTEffortModifyOptimisticRatioCmd::execute() {
    m_effort->setOptimisticRatio(m_newvalue);
    
    setCommandType(1);
}
void KPTEffortModifyOptimisticRatioCmd::unexecute() {
    m_effort->setOptimisticRatio(m_oldvalue);
    
    setCommandType(1);
}

KPTEffortModifyPessimisticRatioCmd::KPTEffortModifyPessimisticRatioCmd(KPTPart *part, KPTEffort *effort, int oldvalue, int newvalue, QString name)
    : KPTNamedCommand(part, name),
      m_effort(effort),
      m_oldvalue(oldvalue),
      m_newvalue(newvalue) {
}
void KPTEffortModifyPessimisticRatioCmd::execute() {
    m_effort->setPessimisticRatio(m_newvalue);
    
    setCommandType(1);
}
void KPTEffortModifyPessimisticRatioCmd::unexecute() {
    m_effort->setPessimisticRatio(m_oldvalue);
    
    setCommandType(1);
}

KPTModifyEffortTypeCmd::KPTModifyEffortTypeCmd(KPTPart *part, KPTEffort *effort, int oldvalue, int newvalue, QString name)
    : KPTNamedCommand(part, name),
      m_effort(effort),
      m_oldvalue(oldvalue),
      m_newvalue(newvalue) {
}
void KPTModifyEffortTypeCmd::execute() {
    m_effort->setType(static_cast<KPTEffort::Type>(m_newvalue));
    
    setCommandType(1);
}
void KPTModifyEffortTypeCmd::unexecute() {
    m_effort->setType(static_cast<KPTEffort::Type>(m_oldvalue));
    
    setCommandType(1);
}

KPTAddResourceGroupRequestCmd::KPTAddResourceGroupRequestCmd(KPTPart *part, KPTTask &task, KPTResourceGroupRequest *request, QString name)
    : KPTNamedCommand(part, name),
      m_task(task),
      m_request(request) {
      
    m_mine = true;
}
void KPTAddResourceGroupRequestCmd::execute() {
    //kdDebug()<<k_funcinfo<<"group="<<m_request<<endl;
    m_task.addRequest(m_request);
    m_mine = false;
    
    setCommandType(1);
}
void KPTAddResourceGroupRequestCmd::unexecute() {
    //kdDebug()<<k_funcinfo<<"group="<<m_request<<endl;
    m_task.takeRequest(m_request); // group should now be empty of resourceRequests
    m_mine = true;
    
    setCommandType(1);
}

KPTRemoveResourceGroupRequestCmd::KPTRemoveResourceGroupRequestCmd(KPTPart *part, KPTResourceGroupRequest *request, QString name)
    : KPTNamedCommand(part, name),
      m_task(request->parent()->task()),
      m_request(request) {
      
    m_mine = false;
}

KPTRemoveResourceGroupRequestCmd::KPTRemoveResourceGroupRequestCmd(KPTPart *part, KPTTask &task, KPTResourceGroupRequest *request, QString name)
    : KPTNamedCommand(part, name),
      m_task(task),
      m_request(request) {
      
    m_mine = false;
}
void KPTRemoveResourceGroupRequestCmd::execute() {
    //kdDebug()<<k_funcinfo<<"group="<<m_request<<endl;
    m_task.takeRequest(m_request); // group should now be empty of resourceRequests
    m_mine = true;
    
    setCommandType(1);
}
void KPTRemoveResourceGroupRequestCmd::unexecute() {
    //kdDebug()<<k_funcinfo<<"group="<<m_request<<endl;
    m_task.addRequest(m_request);
    m_mine = false;
    
    setCommandType(1);
}

KPTAddResourceCmd::KPTAddResourceCmd(KPTPart *part, KPTResourceGroup *group, KPTResource *resource, QString name)
    : KPTNamedCommand(part, name),
      m_group(group),
      m_resource(resource) {
      
    m_mine = true;
}
KPTAddResourceCmd::~KPTAddResourceCmd() {
    if (m_mine)
        delete m_resource;
}
void KPTAddResourceCmd::execute() {
    m_group->addResource(m_resource, 0/*risk*/); 
    m_mine = false;
    
    setCommandType(0);
}
void KPTAddResourceCmd::unexecute() {
    m_group->takeResource(m_resource);
    m_mine = true;
    
    setCommandType(0);
}

KPTRemoveResourceCmd::KPTRemoveResourceCmd(KPTPart *part, KPTResourceGroup *group, KPTResource *resource, QString name)
    : KPTAddResourceCmd(part, group, resource, name) {

    m_mine = false;
    m_requests = m_resource->requests();
    
    m_appointments.setAutoDelete(true);
}
void KPTRemoveResourceCmd::execute() {
    QPtrListIterator<KPTResourceRequest> it = m_requests;
    for (; it.current(); ++it) {
        it.current()->parent()->takeResourceRequest(it.current());
        //kdDebug()<<"Remove request for"<<it.current()->resource()->name()<<endl;
    }
    QPtrListIterator<KPTAppointment> ait = m_resource->appointments();
    for (; ait.current(); ++ait) {
        ait.current()->detach();
        m_appointments.append(ait.current());
    }
    KPTAddResourceCmd::unexecute();
}
void KPTRemoveResourceCmd::unexecute() {
    KPTAppointment *a;
    for (a = m_appointments.first(); a != 0; m_appointments.take()) {
        a->attach();
    }
    QPtrListIterator<KPTResourceRequest> it = m_requests;
    for (; it.current(); ++it) {
        it.current()->parent()->addResourceRequest(it.current());
        //kdDebug()<<"Add request for "<<it.current()->resource()->name()<<endl;
    }
    KPTAddResourceCmd::execute();
}

KPTModifyResourceNameCmd::KPTModifyResourceNameCmd(KPTPart *part, KPTResource *resource, QString value, QString name)
    : KPTNamedCommand(part, name),
      m_resource(resource),
      m_newvalue(value) {
    m_oldvalue = resource->name();
}
void KPTModifyResourceNameCmd::execute() {
    m_resource->setName(m_newvalue);
    
    setCommandType(0);
}
void KPTModifyResourceNameCmd::unexecute() {
    m_resource->setName(m_oldvalue);
    
    setCommandType(0);
}
KPTModifyResourceInitialsCmd::KPTModifyResourceInitialsCmd(KPTPart *part, KPTResource *resource, QString value, QString name)
    : KPTNamedCommand(part, name),
      m_resource(resource),
      m_newvalue(value) {
    m_oldvalue = resource->initials();
}
void KPTModifyResourceInitialsCmd::execute() {
    m_resource->setInitials(m_newvalue);
    
    setCommandType(0);
}
void KPTModifyResourceInitialsCmd::unexecute() {
    m_resource->setInitials(m_oldvalue);
    
    setCommandType(0);
}
KPTModifyResourceEmailCmd::KPTModifyResourceEmailCmd(KPTPart *part, KPTResource *resource, QString value, QString name)
    : KPTNamedCommand(part, name),
      m_resource(resource),
      m_newvalue(value) {
    m_oldvalue = resource->email();
}
void KPTModifyResourceEmailCmd::execute() {
    m_resource->setEmail(m_newvalue);
    
    setCommandType(0);
}
void KPTModifyResourceEmailCmd::unexecute() {
    m_resource->setEmail(m_oldvalue);
    
    setCommandType(0);
}
KPTModifyResourceTypeCmd::KPTModifyResourceTypeCmd(KPTPart *part, KPTResource *resource, int value, QString name)
    : KPTNamedCommand(part, name),
      m_resource(resource),
      m_newvalue(value) {
    m_oldvalue = resource->type();
}
void KPTModifyResourceTypeCmd::execute() {
    m_resource->setType((KPTResource::Type)m_newvalue);
    
    setCommandType(0); //FIXME
}
void KPTModifyResourceTypeCmd::unexecute() {
    m_resource->setType((KPTResource::Type)m_oldvalue);
    
    setCommandType(0); //FIXME
}

KPTModifyResourceNormalRateCmd::KPTModifyResourceNormalRateCmd(KPTPart *part, KPTResource *resource, double value, QString name)
    : KPTNamedCommand(part, name),
      m_resource(resource),
      m_newvalue(value) {
    m_oldvalue = resource->normalRate();
}
void KPTModifyResourceNormalRateCmd::execute() {
    m_resource->setNormalRate(m_newvalue);
    
    setCommandType(0);
}
void KPTModifyResourceNormalRateCmd::unexecute() {
    m_resource->setNormalRate(m_oldvalue);
    
    setCommandType(0);
}
KPTModifyResourceOvertimeRateCmd::KPTModifyResourceOvertimeRateCmd(KPTPart *part, KPTResource *resource, double value, QString name)
    : KPTNamedCommand(part, name),
      m_resource(resource),
      m_newvalue(value) {
    m_oldvalue = resource->overtimeRate();
}
void KPTModifyResourceOvertimeRateCmd::execute() {
    m_resource->setOvertimeRate(m_newvalue);
    
    setCommandType(0);
}
void KPTModifyResourceOvertimeRateCmd::unexecute() {
    m_resource->setOvertimeRate(m_oldvalue);
    
    setCommandType(0);
}
KPTModifyResourceFixedCostCmd::KPTModifyResourceFixedCostCmd(KPTPart *part, KPTResource *resource, double value, QString name)
    : KPTNamedCommand(part, name),
      m_resource(resource),
      m_newvalue(value) {
    m_oldvalue = resource->normalRate();
}
void KPTModifyResourceFixedCostCmd::execute() {
    m_resource->setFixedCost(m_newvalue);
    
    setCommandType(0);
}
void KPTModifyResourceFixedCostCmd::unexecute() {
    m_resource->setFixedCost(m_oldvalue);
    
    setCommandType(0);
}

KPTRemoveResourceGroupCmd::KPTRemoveResourceGroupCmd(KPTPart *part, KPTResourceGroup *group, QString name)
    : KPTNamedCommand(part, name),
      m_group(group) {
            
    m_mine = false;
}
KPTRemoveResourceGroupCmd::~KPTRemoveResourceGroupCmd() {
    if (m_mine)
        delete m_group;
}
void KPTRemoveResourceGroupCmd::execute() {
    // remove all requests to this group
    int c=0;
    QPtrListIterator<KPTResourceGroupRequest> it = m_group->requests();
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
void KPTRemoveResourceGroupCmd::unexecute() {
    // add all requests
    int c=0;
    QPtrListIterator<KPTResourceGroupRequest> it = m_group->requests();
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

KPTAddResourceGroupCmd::KPTAddResourceGroupCmd(KPTPart *part, KPTResourceGroup *group, QString name)
    : KPTRemoveResourceGroupCmd(part, group, name) {
                
    m_mine = true;
}
void KPTAddResourceGroupCmd::execute() {
    KPTRemoveResourceGroupCmd::unexecute();
}
void KPTAddResourceGroupCmd::unexecute() {
    KPTRemoveResourceGroupCmd::execute();
}

KPTModifyResourceGroupNameCmd::KPTModifyResourceGroupNameCmd(KPTPart *part, KPTResourceGroup *group, QString value, QString name)
    : KPTNamedCommand(part, name),
      m_group(group),
      m_newvalue(value) {
    m_oldvalue = group->name();
}
void KPTModifyResourceGroupNameCmd::execute() {
    m_group->setName(m_newvalue);
    
    setCommandType(0);
}
void KPTModifyResourceGroupNameCmd::unexecute() {
    m_group->setName(m_oldvalue);
    
    setCommandType(0);
}

KPTTaskModifyProgressCmd::KPTTaskModifyProgressCmd(KPTPart *part, KPTTask &task, struct KPTTask::Progress &value, QString name)
    : KPTNamedCommand(part, name),
      m_task(task),
      m_newvalue(value) {
    m_oldvalue = task.progress();
}
void KPTTaskModifyProgressCmd::execute() {
    m_task.progress() = m_newvalue;
    
    setCommandType(0);
}
void KPTTaskModifyProgressCmd::unexecute() {
    m_task.progress() = m_oldvalue;
    
    setCommandType(0);
}

KPTProjectModifyBaselineCmd::KPTProjectModifyBaselineCmd(KPTPart *part, KPTProject &project, bool value, QString name)
    : KPTNamedCommand(part, name),
      m_project(project),
      m_newvalue(value) {
    m_oldvalue = project.isBaselined();
}
void KPTProjectModifyBaselineCmd::execute() {
    m_project.setBaselined(m_newvalue);
    
    setCommandType(2);
}
void KPTProjectModifyBaselineCmd::unexecute() {
    m_project.setBaselined(m_oldvalue);
    
    setCommandType(2);
}

KPTAddAccountCmd::KPTAddAccountCmd(KPTPart *part, KPTProject &project, KPTAccount *account, QString parent, QString name)
    : KPTNamedCommand(part, name),
      m_project(project),
      m_account(account),
      m_parent(0),
      m_parentName(parent) {
    m_mine = true;
}

KPTAddAccountCmd::KPTAddAccountCmd(KPTPart *part, KPTProject &project, KPTAccount *account, KPTAccount *parent, QString name)
    : KPTNamedCommand(part, name),
      m_project(project),
      m_account(account),
      m_parent(parent) {
    m_mine = true;
}

KPTAddAccountCmd::~KPTAddAccountCmd() {
    if (m_mine)
        delete m_account;
}

void KPTAddAccountCmd::execute() {
    if (m_parent == 0 && !m_parentName.isEmpty()) {
        m_parent = m_project.accounts().findAccount(m_parentName);
    }
    if (m_parent)
        m_parent->append(m_account);
    else
        m_project.accounts().append(m_account);
    
    setCommandType(2);
    m_mine = false;
}
void KPTAddAccountCmd::unexecute() {
    if (m_parent)
        m_parent->take(m_account);
    else
        m_project.accounts().take(m_account);
    
    setCommandType(2);
    m_mine = true;
}

KPTRemoveAccountCmd::KPTRemoveAccountCmd(KPTPart *part, KPTProject &project, KPTAccount *account, QString name)
    : KPTNamedCommand(part, name),
      m_project(project),
      m_account(account) {
    m_mine = false;
}

KPTRemoveAccountCmd::~KPTRemoveAccountCmd() {
    if (m_mine)
        delete m_account;
}

void KPTRemoveAccountCmd::execute() {
    if (m_account->parent())
        m_account->parent()->take(m_account);
    else
        m_project.accounts().take(m_account);
    
    setCommandType(0);
    m_mine = true;
}
void KPTRemoveAccountCmd::unexecute() {
    if (m_account->parent())
        m_account->parent()->append(m_account);
    else
        m_project.accounts().append(m_account);
    
    setCommandType(0);
    m_mine = false;
}

KPTRenameAccountCmd::KPTRenameAccountCmd(KPTPart *part, KPTAccount *account, QString value, QString name)
    : KPTNamedCommand(part, name),
      m_account(account) {
    m_oldvalue = account->name();
    m_newvalue = value;
}

void KPTRenameAccountCmd::execute() {
    m_account->setName(m_newvalue);        
    setCommandType(0);
}
void KPTRenameAccountCmd::unexecute() {
    m_account->setName(m_oldvalue);
    setCommandType(0);
}

KPTModifyAccountDescriptionCmd::KPTModifyAccountDescriptionCmd(KPTPart *part, KPTAccount *account, QString value, QString name)
    : KPTNamedCommand(part, name),
      m_account(account) {
    m_oldvalue = account->description();
    m_newvalue = value;
}

void KPTModifyAccountDescriptionCmd::execute() {
    m_account->setDescription(m_newvalue);        
    setCommandType(0);
}
void KPTModifyAccountDescriptionCmd::unexecute() {
    m_account->setDescription(m_oldvalue);
    setCommandType(0);
}


KPTNodeModifyStartupCostCmd::KPTNodeModifyStartupCostCmd(KPTPart *part, KPTNode &node, double value, QString name)
    : KPTNamedCommand(part, name),
      m_node(node) {
    m_oldvalue = node.startupCost();
    m_newvalue = value;
}

void KPTNodeModifyStartupCostCmd::execute() {
    m_node.setStartupCost(m_newvalue);        
    setCommandType(0);
}
void KPTNodeModifyStartupCostCmd::unexecute() {
    m_node.setStartupCost(m_oldvalue);
    setCommandType(0);
}

KPTNodeModifyShutdownCostCmd::KPTNodeModifyShutdownCostCmd(KPTPart *part, KPTNode &node, double value, QString name)
    : KPTNamedCommand(part, name),
      m_node(node) {
    m_oldvalue = node.startupCost();
    m_newvalue = value;
}

void KPTNodeModifyShutdownCostCmd::execute() {
    m_node.setShutdownCost(m_newvalue);        
    setCommandType(0);
}
void KPTNodeModifyShutdownCostCmd::unexecute() {
    m_node.setShutdownCost(m_oldvalue);
    setCommandType(0);
}

KPTNodeModifyRunningAccountCmd::KPTNodeModifyRunningAccountCmd(KPTPart *part, KPTNode &node, KPTAccount *value, QString name)
    : KPTNamedCommand(part, name),
      m_node(node) {
    m_oldvalue = node.runningAccount();
    m_newvalue = value;
}

void KPTNodeModifyRunningAccountCmd::execute() {
    m_node.setRunningAccount(m_newvalue);        
    setCommandType(0);
}
void KPTNodeModifyRunningAccountCmd::unexecute() {
    m_node.setRunningAccount(m_oldvalue);
    setCommandType(0);
}

KPTNodeModifyStartupAccountCmd::KPTNodeModifyStartupAccountCmd(KPTPart *part, KPTNode &node, KPTAccount *value, QString name)
    : KPTNamedCommand(part, name),
      m_node(node) {
    m_oldvalue = node.startupAccount();
    m_newvalue = value;
}

void KPTNodeModifyStartupAccountCmd::execute() {
    m_node.setStartupAccount(m_newvalue);        
    setCommandType(0);
}
void KPTNodeModifyStartupAccountCmd::unexecute() {
    m_node.setStartupAccount(m_oldvalue);
    setCommandType(0);
}

KPTNodeModifyShutdownAccountCmd::KPTNodeModifyShutdownAccountCmd(KPTPart *part, KPTNode &node, KPTAccount *value, QString name)
    : KPTNamedCommand(part, name),
      m_node(node) {
    m_oldvalue = node.shutdownAccount();
    m_newvalue = value;
}

void KPTNodeModifyShutdownAccountCmd::execute() {
    m_node.setShutdownAccount(m_newvalue);        
    setCommandType(0);
}
void KPTNodeModifyShutdownAccountCmd::unexecute() {
    m_node.setShutdownAccount(m_oldvalue);
    setCommandType(0);
}


}  //KPlato namespace
