/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kptcommand.h"
#include "kptpart.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptcalendar.h"
#include "kptrelation.h"

#include <kdebug.h>
#include <klocale.h>


KPTCalendarAddCmd::KPTCalendarAddCmd(KPTPart *part, KPTProject *project,KPTCalendar *cal, QString name)
    : KNamedCommand(name),
      m_part(part),
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
    if (m_part)
        m_part->setCommandType(0);
    //kdDebug()<<k_funcinfo<<m_cal->name()<<" added to: "<<m_project->name()<<endl;
}

void KPTCalendarAddCmd::unexecute() {
    m_cal->setDeleted(true);
    if (m_part)
        m_part->setCommandType(0);
    //kdDebug()<<k_funcinfo<<m_cal->name()<<endl;
}

KPTCalendarDeleteCmd::KPTCalendarDeleteCmd(KPTPart *part, KPTCalendar *cal, QString name)
    : KNamedCommand(name),
      m_part(part),
      m_cal(cal) {
}

void KPTCalendarDeleteCmd::execute() {
    m_cal->setDeleted(true);
    if (m_part)
        m_part->setCommandType(0);
}

void KPTCalendarDeleteCmd::unexecute() {
    m_cal->setDeleted(false);
    if (m_part)
        m_part->setCommandType(0);
}

KPTNodeDeleteCmd::KPTNodeDeleteCmd(KPTPart *part, KPTNode *node, QString name)
    : KNamedCommand(name),
      m_part(part),
      m_node(node),
       m_index(-1) {
    
    m_parent = node->getParent();
    if (m_parent)
        m_index = m_parent->findChildNode(node);
    m_mine = false;
}
KPTNodeDeleteCmd::~KPTNodeDeleteCmd() {
    if (m_mine)
        delete m_node;
}
void KPTNodeDeleteCmd::execute() {
    if (m_parent) {
        //kdDebug()<<k_funcinfo<<m_node->name()<<" "<<m_index<<endl;
        m_parent->delChildNode(m_node, false/*take*/);
        m_mine = true;
        if (m_part)
        if (m_part)
            m_part->setCommandType(1);
    }
}
void KPTNodeDeleteCmd::unexecute() {
    if (m_parent) {
        //kdDebug()<<k_funcinfo<<m_node->name()<<" "<<m_index<<endl;
        m_parent->insertChildNode(m_index, m_node);
        m_mine = false;
        if (m_part)
            m_part->setCommandType(1);
    }
}

KPTTaskAddCmd::KPTTaskAddCmd(KPTPart *part, KPTProject *project, KPTNode *node, KPTNode *after,  QString name)
    : KNamedCommand(name),
      m_part(part),
      m_project(project),
      m_node(node),
      m_after(after),
      m_added(false) {
}
KPTTaskAddCmd::~KPTTaskAddCmd() {
    if (!m_added)
        delete m_node;
}
void KPTTaskAddCmd::execute() {
    //kdDebug()<<k_funcinfo<<m_node->name()<<endl;
    m_project->addTask(m_node, m_after);
    m_added = true;
    if (m_part)
        m_part->setCommandType(1);
}
void KPTTaskAddCmd::unexecute() {
    m_node->getParent()->delChildNode(m_node, false/*take*/);
    m_added = false;
    if (m_part)
        m_part->setCommandType(1);
}

KPTSubtaskAddCmd::KPTSubtaskAddCmd(KPTPart *part, KPTProject *project, KPTNode *node, KPTNode *parent,  QString name)
    : KNamedCommand(name),
      m_part(part),
      m_project(project),
      m_node(node),
      m_parent(parent),
      m_added(false) {
}
KPTSubtaskAddCmd::~KPTSubtaskAddCmd() {
    if (!m_added)
        delete m_node;
}
void KPTSubtaskAddCmd::execute() {
    m_project->addSubTask(m_node, m_parent);
    m_added = true;
    if (m_part)
        m_part->setCommandType(1);
}
void KPTSubtaskAddCmd::unexecute() {
    m_parent->delChildNode(m_node, false/*take*/);
    m_added = false;
    if (m_part)
        m_part->setCommandType(1);
}

KPTNodeModifyNameCmd::KPTNodeModifyNameCmd(KPTPart *part, KPTNode &node, QString nodename, QString name)
    : KNamedCommand(name),
      m_part(part),
      m_node(node),
      newName(nodename),
      oldName(node.name()) {

}
void KPTNodeModifyNameCmd::execute() {
    m_node.setName(newName);
    if (m_part)
        m_part->setCommandType(0);
}
void KPTNodeModifyNameCmd::unexecute() {
    m_node.setName(oldName);
    if (m_part)
        m_part->setCommandType(0);
}

KPTNodeModifyLeaderCmd::KPTNodeModifyLeaderCmd(KPTPart *part, KPTNode &node, QString leader, QString name)
    : KNamedCommand(name),
      m_part(part),
      m_node(node),
      newLeader(leader),
      oldLeader(node.leader()) {

}
void KPTNodeModifyLeaderCmd::execute() {
    m_node.setLeader(newLeader);
    if (m_part)
        m_part->setCommandType(0);
}
void KPTNodeModifyLeaderCmd::unexecute() {
    m_node.setLeader(oldLeader);
    if (m_part)
        m_part->setCommandType(0);
}

KPTNodeModifyDescriptionCmd::KPTNodeModifyDescriptionCmd(KPTPart *part, KPTNode &node, QString description, QString name)
    : KNamedCommand(name),
      m_part(part),
      m_node(node),
      newDescription(description),
      oldDescription(node.description()) {

}
void KPTNodeModifyDescriptionCmd::execute() {
    m_node.setDescription(newDescription);
    if (m_part)
        m_part->setCommandType(0);
}
void KPTNodeModifyDescriptionCmd::unexecute() {
    m_node.setDescription(oldDescription);
    if (m_part)
        m_part->setCommandType(0);
}

KPTNodeModifyConstraintCmd::KPTNodeModifyConstraintCmd(KPTPart *part, KPTNode &node, KPTNode::ConstraintType c, QString name)
    : KNamedCommand(name),
      m_part(part),
      m_node(node),
      newConstraint(c),
      oldConstraint(static_cast<KPTNode::ConstraintType>(node.constraint())) {

}
void KPTNodeModifyConstraintCmd::execute() {
    m_node.setConstraint(newConstraint);
    if (m_part)
        m_part->setCommandType(1);
}
void KPTNodeModifyConstraintCmd::unexecute() {
    m_node.setConstraint(oldConstraint);
    if (m_part)
        m_part->setCommandType(1);
}

KPTNodeModifyConstraintTimeCmd::KPTNodeModifyConstraintTimeCmd(KPTPart *part, KPTNode &node, QDateTime dt, QString name)
    : KNamedCommand(name),
      m_part(part),
      m_node(node),
      newTime(dt),
      oldTime(node.constraintTime()) {

}
void KPTNodeModifyConstraintTimeCmd::execute() {
    m_node.setConstraintTime(newTime);
    if (m_part)
        m_part->setCommandType(1);
}
void KPTNodeModifyConstraintTimeCmd::unexecute() {
    m_node.setConstraintTime(oldTime);
    if (m_part)
        m_part->setCommandType(1);
}

KPTNodeIndentCmd::KPTNodeIndentCmd(KPTPart *part, KPTNode &node, QString name)
    : KNamedCommand(name),
      m_part(part),
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
    if (m_part)
        m_part->setCommandType(1);
}
void KPTNodeIndentCmd::unexecute() {
    if (m_newindex != -1) {
        m_newparent->delChildNode(m_newindex, false);
        m_oldparent->insertChildNode(m_oldindex, &m_node);
        m_node.setParent(m_oldparent);
        m_newindex = -1;
    }
    if (m_part)
        m_part->setCommandType(1);
}

KPTNodeUnindentCmd::KPTNodeUnindentCmd(KPTPart *part, KPTNode &node, QString name)
    : KNamedCommand(name),
      m_part(part),
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
    if (m_part)
        m_part->setCommandType(1);
}
void KPTNodeUnindentCmd::unexecute() {
    if (m_newindex != -1) {
        m_newparent->delChildNode(m_newindex, false);
        m_oldparent->insertChildNode(m_oldindex, &m_node);
        m_node.setParent(m_oldparent);
        m_newindex = -1;
    }
    if (m_part)
        m_part->setCommandType(1);
}

KPTNodeMoveUpCmd::KPTNodeMoveUpCmd(KPTPart *part, KPTNode &node, QString name)
    : KNamedCommand(name),
      m_part(part),
      m_node(node), 
      m_newindex(-1) {
}
void KPTNodeMoveUpCmd::execute() {
    m_oldindex = m_node.getParent()->findChildNode(&m_node);
    KPTProject *p = dynamic_cast<KPTProject *>(m_node.projectNode());
    if (p && p->moveTaskUp(&m_node)) {
        m_newindex = m_node.getParent()->findChildNode(&m_node);
    }
    if (m_part)
        m_part->setCommandType(0);
}
void KPTNodeMoveUpCmd::unexecute() {
    if (m_newindex != -1) {
        m_node.getParent()->delChildNode(m_newindex, false);
        m_node.getParent()->insertChildNode(m_oldindex, &m_node);
        m_newindex = -1;
    }
    if (m_part)
        m_part->setCommandType(0);
}

KPTNodeMoveDownCmd::KPTNodeMoveDownCmd(KPTPart *part, KPTNode &node, QString name)
    : KNamedCommand(name),
      m_part(part),
      m_node(node), 
      m_newindex(-1) {
}
void KPTNodeMoveDownCmd::execute() {
    m_oldindex = m_node.getParent()->findChildNode(&m_node);
    KPTProject *p = dynamic_cast<KPTProject *>(m_node.projectNode());
    if (p && p->moveTaskDown(&m_node)) {
        m_newindex = m_node.getParent()->findChildNode(&m_node);
    }
    if (m_part)
        m_part->setCommandType(0);
}
void KPTNodeMoveDownCmd::unexecute() {
    if (m_newindex != -1) {
        m_node.getParent()->delChildNode(m_newindex, false);
        m_node.getParent()->insertChildNode(m_oldindex, &m_node);
        m_newindex = -1;
    }
    if (m_part)
        m_part->setCommandType(0);
}

KPTAddRelationCmd::KPTAddRelationCmd(KPTPart *part, KPTRelation *rel, QString name)
    : KNamedCommand(name),
      m_part(part),
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
    if (m_part)
        m_part->setCommandType(1);
}
void KPTAddRelationCmd::unexecute() {
    m_taken = true;
    m_rel->parent()->takeDependChildNode(m_rel);
    m_rel->child()->takeDependParentNode(m_rel);
    if (m_part)
        m_part->setCommandType(1);
}

KPTDeleteRelationCmd::KPTDeleteRelationCmd(KPTPart *part, KPTRelation *rel, QString name)
    : KNamedCommand(name),
      m_part(part),
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
    if (m_part)
        m_part->setCommandType(1);
}
void KPTDeleteRelationCmd::unexecute() {
    m_taken = false;
    m_rel->parent()->addDependChildNode(m_rel);
    m_rel->child()->addDependParentNode(m_rel);
    if (m_part)
        m_part->setCommandType(1);
}

KPTModifyTimingRelationCmd::KPTModifyTimingRelationCmd(KPTPart *part, KPTRelation *rel, TimingRelation type, QString name)
    : KNamedCommand(name),
      m_part(part),
      m_rel(rel),
      m_newtype(type) {
    
    m_oldtype = rel->timingRelation();
}
void KPTModifyTimingRelationCmd::execute() {
    m_rel->setTimingRelation(m_newtype);
    if (m_part)
        m_part->setCommandType(1);
}
void KPTModifyTimingRelationCmd::unexecute() {
    m_rel->setTimingRelation(m_oldtype);
    if (m_part)
        m_part->setCommandType(1);
}

KPTAddResourceRequestCmd::KPTAddResourceRequestCmd(KPTPart *part, KPTResourceGroupRequest *group, KPTResourceRequest *request, QString name)
    : KNamedCommand(name),
      m_part(part),
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
    if (m_part)
        m_part->setCommandType(1);
}
void KPTAddResourceRequestCmd::unexecute() {
    //kdDebug()<<k_funcinfo<<"group="<<m_group<<" req="<<m_request<<endl;
    m_group->takeResourceRequest(m_request);
    m_mine = true;
    if (m_part)
        m_part->setCommandType(1);
}

KPTRemoveResourceRequestCmd::KPTRemoveResourceRequestCmd(KPTPart *part,  KPTResourceGroupRequest *group, KPTResourceRequest *request, QString name)
    : KNamedCommand(name),
      m_part(part),
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
    if (m_part)
        m_part->setCommandType(1);
}
void KPTRemoveResourceRequestCmd::unexecute() {
    m_group->addResourceRequest(m_request);
    m_mine = false;
    if (m_part)
        m_part->setCommandType(1);
}

KPTModifyEffortCmd::KPTModifyEffortCmd(KPTPart *part, KPTEffort *effort, KPTDuration oldvalue, KPTDuration newvalue, QString name)
    : KNamedCommand(name),
      m_part(part),
      m_effort(effort),
      m_oldvalue(oldvalue),
      m_newvalue(newvalue) {
}
void KPTModifyEffortCmd::execute() {
    m_effort->set(m_newvalue);
    if (m_part)
        m_part->setCommandType(1);
}
void KPTModifyEffortCmd::unexecute() {
    m_effort->set(m_oldvalue);
    if (m_part)
        m_part->setCommandType(1);
}

KPTModifyEffortTypeCmd::KPTModifyEffortTypeCmd(KPTPart *part, KPTEffort *effort, int oldvalue, int newvalue, QString name)
    : KNamedCommand(name),
      m_part(part),
      m_effort(effort),
      m_oldvalue(oldvalue),
      m_newvalue(newvalue) {
}
void KPTModifyEffortTypeCmd::execute() {
    m_effort->setType(static_cast<KPTEffort::Type>(m_newvalue));
    if (m_part)
        m_part->setCommandType(1);
}
void KPTModifyEffortTypeCmd::unexecute() {
    m_effort->setType(static_cast<KPTEffort::Type>(m_oldvalue));
    if (m_part)
        m_part->setCommandType(1);
}

KPTAddResourceGroupRequestCmd::KPTAddResourceGroupRequestCmd(KPTPart *part, KPTTask &task, KPTResourceGroupRequest *request, QString name)
    : KNamedCommand(name),
      m_part(part),
      m_task(task),
      m_request(request) {
      
    m_mine = true;
}
void KPTAddResourceGroupRequestCmd::execute() {
    //kdDebug()<<k_funcinfo<<"group="<<m_request<<endl;
    m_task.addRequest(m_request);
    m_mine = false;
    if (m_part)
        m_part->setCommandType(1);
}
void KPTAddResourceGroupRequestCmd::unexecute() {
    //kdDebug()<<k_funcinfo<<"group="<<m_request<<endl;
    m_task.takeRequest(m_request); // group should now be empty of resourceRequests
    m_mine = true;
    if (m_part)
        m_part->setCommandType(1);
}

KPTRemoveResourceGroupRequestCmd::KPTRemoveResourceGroupRequestCmd(KPTPart *part, KPTTask &task, KPTResourceGroupRequest *request, QString name)
    : KNamedCommand(name),
      m_part(part),
      m_task(task),
      m_request(request) {
      
    m_mine = false;
}
void KPTRemoveResourceGroupRequestCmd::execute() {
    //kdDebug()<<k_funcinfo<<"group="<<m_request<<endl;
    m_task.takeRequest(m_request); // group should now be empty of resourceRequests
    m_mine = true;
    if (m_part)
        m_part->setCommandType(1);
}
void KPTRemoveResourceGroupRequestCmd::unexecute() {
    //kdDebug()<<k_funcinfo<<"group="<<m_request<<endl;
    m_task.addRequest(m_request);
    m_mine = false;
    if (m_part)
        m_part->setCommandType(1);
}

