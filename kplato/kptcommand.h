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

#ifndef KPTCOMMAND_H
#define KPTCOMMAND_H

#include <kcommand.h>
#include "kptnode.h"
#include "defs.h"

class QString;
class KPTProject;
class KPTCalendar;
class KPTPart;
class KPTRelation;

class KPTCalendarAddCmd : public KNamedCommand
{
public:
    KPTCalendarAddCmd(KPTPart *part, KPTProject *project, KPTCalendar *cal, QString name=0);
    void execute();
    void unexecute();

private:
    KPTPart *m_part;
    KPTProject *m_project;
    KPTCalendar *m_cal;
    bool m_added;
};

class KPTCalendarDeleteCmd : public KNamedCommand
{
public:
    KPTCalendarDeleteCmd(KPTPart *part, KPTCalendar *cal, QString name=0);
    void execute();
    void unexecute();

private:
    KPTPart *m_part;
    KPTCalendar *m_cal;
};

class KPTNodeDeleteCmd : public KNamedCommand
{
public:
    KPTNodeDeleteCmd(KPTPart *part, KPTNode *node, QString name=0);
    ~KPTNodeDeleteCmd();
    void execute();
    void unexecute();

private:
    KPTPart *m_part;
    KPTNode *m_node;
    KPTNode *m_parent;
    int m_index;
    bool m_mine;
};

class KPTTaskAddCmd : public KNamedCommand
{
public:
    KPTTaskAddCmd(KPTPart *part, KPTProject *project, KPTNode *node, KPTNode *after,  QString name=0);
    ~KPTTaskAddCmd();
    void execute();
    void unexecute();

private:
    KPTPart *m_part;
    KPTProject *m_project;
    KPTNode *m_node;
    KPTNode *m_after;
    bool m_added;
};

class KPTSubtaskAddCmd : public KNamedCommand
{
public:
    KPTSubtaskAddCmd(KPTPart *part, KPTProject *project, KPTNode *node, KPTNode *parent,  QString name=0);
    ~KPTSubtaskAddCmd();
    void execute();
    void unexecute();

private:
    KPTPart *m_part;
    KPTProject *m_project;
    KPTNode *m_node;
    KPTNode *m_parent;
    bool m_added;
};


class KPTNodeModifyNameCmd : public KNamedCommand
{
public:
    KPTNodeModifyNameCmd(KPTPart *part, KPTNode &node, QString nodename, QString name=0);
    void execute();
    void unexecute();

private:
    KPTPart *m_part;
    KPTNode &m_node;
    QString newName;
    QString oldName;
};

class KPTNodeModifyLeaderCmd : public KNamedCommand
{
public:
    KPTNodeModifyLeaderCmd(KPTPart *part, KPTNode &node, QString leader, QString name=0);
    void execute();
    void unexecute();

private:
    KPTPart *m_part;
    KPTNode &m_node;
    QString newLeader;
    QString oldLeader;
};

class KPTNodeModifyDescriptionCmd : public KNamedCommand
{
public:
    KPTNodeModifyDescriptionCmd(KPTPart *part, KPTNode &node, QString description, QString name=0);
    void execute();
    void unexecute();

private:
    KPTPart *m_part;
    KPTNode &m_node;
    QString newDescription;
    QString oldDescription;
};

class KPTNodeModifyConstraintCmd : public KNamedCommand
{
public:
    KPTNodeModifyConstraintCmd(KPTPart *part, KPTNode &node, KPTNode::ConstraintType c, QString name=0);
    void execute();
    void unexecute();

private:
    KPTPart *m_part;
    KPTNode &m_node;
    KPTNode::ConstraintType newConstraint;
    KPTNode::ConstraintType oldConstraint;
};

class KPTNodeModifyConstraintTimeCmd : public KNamedCommand
{
public:
    KPTNodeModifyConstraintTimeCmd(KPTPart *part, KPTNode &node, QDateTime dt, QString name=0);
    void execute();
    void unexecute();

private:
    KPTPart *m_part;
    KPTNode &m_node;
    QDateTime newTime;
    QDateTime oldTime;
};

class KPTNodeIndentCmd : public KNamedCommand
{
public:
    KPTNodeIndentCmd(KPTPart *part, KPTNode &node, QString name=0);
    void execute();
    void unexecute();

private:
    KPTPart *m_part;
    KPTNode &m_node;
    KPTNode *m_oldparent, *m_newparent;
    int m_oldindex, m_newindex;
};

class KPTNodeUnindentCmd : public KNamedCommand
{
public:
    KPTNodeUnindentCmd(KPTPart *part, KPTNode &node, QString name=0);
    void execute();
    void unexecute();

private:
    KPTPart *m_part;
    KPTNode &m_node;
    KPTNode *m_oldparent, *m_newparent;
    int m_oldindex, m_newindex;
};

class KPTNodeMoveUpCmd : public KNamedCommand
{
public:
    KPTNodeMoveUpCmd(KPTPart *part, KPTNode &node, QString name=0);
    void execute();
    void unexecute();

private:
    KPTPart *m_part;
    KPTNode &m_node;
    int m_oldindex, m_newindex;
};

class KPTNodeMoveDownCmd : public KNamedCommand
{
public:
    KPTNodeMoveDownCmd(KPTPart *part, KPTNode &node, QString name=0);
    void execute();
    void unexecute();

private:
    KPTPart *m_part;
    KPTNode &m_node;
    int m_oldindex, m_newindex;
};

class KPTAddRelationCmd : public KNamedCommand
{
public:
    KPTAddRelationCmd(KPTPart *part, KPTRelation *rel, QString name=0);
    ~KPTAddRelationCmd();
    void execute();
    void unexecute();

private:
    KPTPart *m_part;
    KPTRelation *m_rel;
    bool m_taken;
};

class KPTDeleteRelationCmd : public KNamedCommand
{
public:
    KPTDeleteRelationCmd(KPTPart *part, KPTRelation *rel, QString name=0);
    ~KPTDeleteRelationCmd();
    void execute();
    void unexecute();

private:
    KPTPart *m_part;
    KPTRelation *m_rel;
    bool m_taken;
};

class KPTModifyTimingRelationCmd : public KNamedCommand
{
public:
    KPTModifyTimingRelationCmd(KPTPart *part, KPTRelation *rel, TimingRelation type, QString name=0);
    void execute();
    void unexecute();

private:
    KPTPart *m_part;
    KPTRelation *m_rel;
    TimingRelation m_newtype;
    TimingRelation m_oldtype;
};

#endif //KPTCOMMAND_H
