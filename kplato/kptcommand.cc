/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kptcommand.h"
#include "kptproject.h"
#include "kptcalendar.h"

#include <kdebug.h>
#include <klocale.h>


KPTCalendarAddCmd::KPTCalendarAddCmd(KPTProject &project,KPTCalendar *cal, QString name)
    : KNamedCommand(name),
      m_project(project),
      m_cal(cal),
      m_added(false) {
    cal->setDeleted(true);  
    //kdDebug()<<k_funcinfo<<cal->name()<<endl;
}

void KPTCalendarAddCmd::execute() {
    if (!m_added) {
        m_project.addCalendar(m_cal);
        m_added = true;
    }
    m_cal->setDeleted(false);
    //kdDebug()<<k_funcinfo<<m_cal->name()<<endl;
}

void KPTCalendarAddCmd::unexecute() {
    m_cal->setDeleted(true);
    //kdDebug()<<k_funcinfo<<m_cal->name()<<endl;
}

KPTCalendarDeleteCmd::KPTCalendarDeleteCmd(KPTCalendar *cal, QString name)
    : KNamedCommand(name) {
    m_cal = cal;
}

void KPTCalendarDeleteCmd::execute() {
    m_cal->setDeleted(true);
}

void KPTCalendarDeleteCmd::unexecute() {
    m_cal->setDeleted(false);
}


KPTNodeAddCmd::KPTNodeAddCmd(KPTProject &project, KPTNode *node, KPTNode *position,  QString name)
    : KNamedCommand(name),
      m_project(project),
      m_node(node),
      m_position(position),
      m_added(false) {
    node->setDeleted(true);
}

void KPTNodeAddCmd::execute() {
    m_node->setDeleted(false);
}

void KPTNodeAddCmd::unexecute() {
    m_node->setDeleted(true);
}


KPTNodeDeleteCmd::KPTNodeDeleteCmd(KPTNode *node, QString name)
    : KNamedCommand(name),
      m_node(node) {
}

void KPTNodeDeleteCmd::execute() {
    m_node->setDeleted(true);
}

void KPTNodeDeleteCmd::unexecute() {
    m_node->setDeleted(false);
}


KPTTaskAddCmd::KPTTaskAddCmd(KPTProject &project, KPTNode *node, KPTNode *position,  QString name)
    : KPTNodeAddCmd(project, node, position, name) {
}

void KPTTaskAddCmd::execute() {
    //kdDebug()<<k_funcinfo<<endl;
    if (!m_added) {
        m_project.addTask(m_node, m_position);
        m_added = true;
    }
    KPTNodeAddCmd::execute();
}

KPTSubtaskAddCmd::KPTSubtaskAddCmd(KPTProject &project, KPTNode *node, KPTNode *position,  QString name)
    : KPTNodeAddCmd(project, node, position, name) {   
}

void KPTSubtaskAddCmd::execute() {
    if (!m_added) {
        m_project.addSubTask(m_node, m_position);
        m_added = true;
    }
    KPTNodeAddCmd::execute();
}


KPTNodeModifyNameCmd::KPTNodeModifyNameCmd(KPTNode &node, QString nodename, QString name)
    : KNamedCommand(name),
      m_node(node),
      newName(nodename),
      oldName(node.name()) {

}
void KPTNodeModifyNameCmd::execute() {
    m_node.setName(newName);
}
void KPTNodeModifyNameCmd::unexecute() {
    m_node.setName(oldName);
}

KPTNodeModifyLeaderCmd::KPTNodeModifyLeaderCmd(KPTNode &node, QString leader, QString name)
    : KNamedCommand(name),
      m_node(node),
      newLeader(leader),
      oldLeader(node.leader()) {

}
void KPTNodeModifyLeaderCmd::execute() {
    m_node.setLeader(newLeader);
}
void KPTNodeModifyLeaderCmd::unexecute() {
    m_node.setLeader(oldLeader);
}

KPTNodeModifyDescriptionCmd::KPTNodeModifyDescriptionCmd(KPTNode &node, QString description, QString name)
    : KNamedCommand(name),
      m_node(node),
      newDescription(description),
      oldDescription(node.description()) {

}
void KPTNodeModifyDescriptionCmd::execute() {
    m_node.setDescription(newDescription);
}
void KPTNodeModifyDescriptionCmd::unexecute() {
    m_node.setDescription(oldDescription);
}

KPTNodeModifyConstraintCmd::KPTNodeModifyConstraintCmd(KPTNode &node, KPTNode::ConstraintType c, QString name)
    : KNamedCommand(name),
      m_node(node),
      newConstraint(c),
      oldConstraint(static_cast<KPTNode::ConstraintType>(node.constraint())) {

}
void KPTNodeModifyConstraintCmd::execute() {
    m_node.setConstraint(newConstraint);
}
void KPTNodeModifyConstraintCmd::unexecute() {
    m_node.setConstraint(oldConstraint);
}

KPTNodeModifyConstraintTimeCmd::KPTNodeModifyConstraintTimeCmd(KPTNode &node, QDateTime dt, QString name)
    : KNamedCommand(name),
      m_node(node),
      newTime(dt),
      oldTime(node.constraintTime()) {

}
void KPTNodeModifyConstraintTimeCmd::execute() {
    m_node.setConstraintTime(newTime);
}
void KPTNodeModifyConstraintTimeCmd::unexecute() {
    m_node.setConstraintTime(oldTime);
}

