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

#include "kptnode.h"

#include <kcommand.h>

class QString;
class KPTProject;
class KPTCalendar;

class KPTCalendarAddCmd : public KNamedCommand
{
public:
    KPTCalendarAddCmd(KPTProject &project, KPTCalendar *cal, QString name=0);
    void execute();
    void unexecute();

private:
    KPTCalendar *m_cal;
};

class KPTCalendarDeleteCmd : public KNamedCommand
{
public:
    KPTCalendarDeleteCmd(KPTCalendar *cal, QString name=0);
    void execute();
    void unexecute();

private:
    KPTCalendar *m_cal;
};

class KPTNodeAddCmd : public KNamedCommand
{
public:
    KPTNodeAddCmd(KPTProject &project, KPTNode *node, KPTNode *position, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode *m_node;
};

class KPTNodeDeleteCmd : public KNamedCommand
{
public:
    KPTNodeDeleteCmd(KPTNode *node, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode *m_node;
};

class KPTTaskAddCmd : public KPTNodeAddCmd
{
public:
    KPTTaskAddCmd(KPTProject &project, KPTNode *node, KPTNode *position,  QString name=0);
};

class KPTSubtaskAddCmd : public KPTNodeAddCmd
{
public:
    KPTSubtaskAddCmd(KPTProject &project, KPTNode *node, KPTNode *position,  QString name=0);
};


class KPTNodeModifyNameCmd : public KNamedCommand
{
public:
    KPTNodeModifyNameCmd(KPTNode &node, QString nodename, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode &m_node;
    QString newName;
    QString oldName;
};

class KPTNodeModifyLeaderCmd : public KNamedCommand
{
public:
    KPTNodeModifyLeaderCmd(KPTNode &node, QString leader, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode &m_node;
    QString newLeader;
    QString oldLeader;
};

class KPTNodeModifyDescriptionCmd : public KNamedCommand
{
public:
    KPTNodeModifyDescriptionCmd(KPTNode &node, QString description, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode &m_node;
    QString newDescription;
    QString oldDescription;
};

class KPTNodeModifyConstraintCmd : public KNamedCommand
{
public:
    KPTNodeModifyConstraintCmd(KPTNode &node, KPTNode::ConstraintType c, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode &m_node;
    KPTNode::ConstraintType newConstraint;
    KPTNode::ConstraintType oldConstraint;
};

class KPTNodeModifyConstraintTimeCmd : public KNamedCommand
{
public:
    KPTNodeModifyConstraintTimeCmd(KPTNode &node, QDateTime dt, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode &m_node;
    QDateTime newTime;
    QDateTime oldTime;
};

#endif //KPTCOMMAND_H
