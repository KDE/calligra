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

#ifndef KPTCOMMAND_H
#define KPTCOMMAND_H

#include <kcommand.h>
#include "kptnode.h"
#include "kptduration.h"
#include "kptpart.h"
#include "kpttask.h"

class QString;

namespace KPlato
{

class KPTAccount;
class KPTProject;
class KPTTask;
class KPTCalendar;
class KPTRelation;
class KPTResourceGroupRequest;
class KPTResourceRequest;
class KPTResourceGroup;
class KPTResource;

class KPTNamedCommand : public KNamedCommand
{
public:
    KPTNamedCommand(KPTPart *part, QString name)
    : KNamedCommand(name), m_part(part)
    {}
    
public:
    void setCommandType(int type) {
        if (m_part) 
            m_part->setCommandType(type);
    }
private:
    KPTPart *m_part;
};

class KPTCalendarAddCmd : public KPTNamedCommand
{
public:
    KPTCalendarAddCmd(KPTPart *part, KPTProject *project, KPTCalendar *cal, QString name=0);
    void execute();
    void unexecute();

private:
    KPTProject *m_project;
    KPTCalendar *m_cal;
    bool m_added;
};

class KPTCalendarDeleteCmd : public KPTNamedCommand
{
public:
    KPTCalendarDeleteCmd(KPTPart *part, KPTCalendar *cal, QString name=0);
    void execute();
    void unexecute();

private:
    KPTCalendar *m_cal;
};

class KPTNodeDeleteCmd : public KPTNamedCommand
{
public:
    KPTNodeDeleteCmd(KPTPart *part, KPTNode *node, QString name=0);
    ~KPTNodeDeleteCmd();
    void execute();
    void unexecute();

private:
    KPTNode *m_node;
    KPTNode *m_parent;
    int m_index;
    bool m_mine;
    QPtrList<KPTAppointment> m_appointments;
};

class KPTTaskAddCmd : public KPTNamedCommand
{
public:
    KPTTaskAddCmd(KPTPart *part, KPTProject *project, KPTNode *node, KPTNode *after,  QString name=0);
    ~KPTTaskAddCmd();
    void execute();
    void unexecute();

private:
    KPTProject *m_project;
    KPTNode *m_node;
    KPTNode *m_after;
    bool m_added;
};

class KPTSubtaskAddCmd : public KPTNamedCommand
{
public:
    KPTSubtaskAddCmd(KPTPart *part, KPTProject *project, KPTNode *node, KPTNode *parent,  QString name=0);
    ~KPTSubtaskAddCmd();
    void execute();
    void unexecute();

private:
    KPTProject *m_project;
    KPTNode *m_node;
    KPTNode *m_parent;
    bool m_added;
};


class KPTNodeModifyNameCmd : public KPTNamedCommand
{
public:
    KPTNodeModifyNameCmd(KPTPart *part, KPTNode &node, QString nodename, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode &m_node;
    QString newName;
    QString oldName;
};

class KPTNodeModifyLeaderCmd : public KPTNamedCommand
{
public:
    KPTNodeModifyLeaderCmd(KPTPart *part, KPTNode &node, QString leader, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode &m_node;
    QString newLeader;
    QString oldLeader;
};

class KPTNodeModifyDescriptionCmd : public KPTNamedCommand
{
public:
    KPTNodeModifyDescriptionCmd(KPTPart *part, KPTNode &node, QString description, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode &m_node;
    QString newDescription;
    QString oldDescription;
};

class KPTNodeModifyConstraintCmd : public KPTNamedCommand
{
public:
    KPTNodeModifyConstraintCmd(KPTPart *part, KPTNode &node, KPTNode::ConstraintType c, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode &m_node;
    KPTNode::ConstraintType newConstraint;
    KPTNode::ConstraintType oldConstraint;
};

class KPTNodeModifyConstraintStartTimeCmd : public KPTNamedCommand
{
public:
    KPTNodeModifyConstraintStartTimeCmd(KPTPart *part, KPTNode &node, QDateTime dt, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode &m_node;
    QDateTime newTime;
    QDateTime oldTime;
};
class KPTNodeModifyConstraintEndTimeCmd : public KPTNamedCommand
{
public:
    KPTNodeModifyConstraintEndTimeCmd(KPTPart *part, KPTNode &node, QDateTime dt, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode &m_node;
    QDateTime newTime;
    QDateTime oldTime;
};
class KPTNodeModifyStartTimeCmd : public KPTNamedCommand
{
public:
    KPTNodeModifyStartTimeCmd(KPTPart *part, KPTNode &node, QDateTime dt, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode &m_node;
    QDateTime newTime;
    QDateTime oldTime;
};
class KPTNodeModifyEndTimeCmd : public KPTNamedCommand
{
public:
    KPTNodeModifyEndTimeCmd(KPTPart *part, KPTNode &node, QDateTime dt, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode &m_node;
    QDateTime newTime;
    QDateTime oldTime;
};
class KPTNodeModifyIdCmd : public KPTNamedCommand
{
public:
    KPTNodeModifyIdCmd(KPTPart *part, KPTNode &node, QString id, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode &m_node;
    QString newId;
    QString oldId;
};

class KPTNodeIndentCmd : public KPTNamedCommand
{
public:
    KPTNodeIndentCmd(KPTPart *part, KPTNode &node, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode &m_node;
    KPTNode *m_oldparent, *m_newparent;
    int m_oldindex, m_newindex;
};

class KPTNodeUnindentCmd : public KPTNamedCommand
{
public:
    KPTNodeUnindentCmd(KPTPart *part, KPTNode &node, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode &m_node;
    KPTNode *m_oldparent, *m_newparent;
    int m_oldindex, m_newindex;
};

class KPTNodeMoveUpCmd : public KPTNamedCommand
{
public:
    KPTNodeMoveUpCmd(KPTPart *part, KPTNode &node, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode &m_node;
    int m_oldindex, m_newindex;
};

class KPTNodeMoveDownCmd : public KPTNamedCommand
{
public:
    KPTNodeMoveDownCmd(KPTPart *part, KPTNode &node, QString name=0);
    void execute();
    void unexecute();

private:
    KPTNode &m_node;
    int m_oldindex, m_newindex;
};

class KPTAddRelationCmd : public KPTNamedCommand
{
public:
    KPTAddRelationCmd(KPTPart *part, KPTRelation *rel, QString name=0);
    ~KPTAddRelationCmd();
    void execute();
    void unexecute();

private:
    KPTRelation *m_rel;
    bool m_taken;
};

class KPTDeleteRelationCmd : public KPTNamedCommand
{
public:
    KPTDeleteRelationCmd(KPTPart *part, KPTRelation *rel, QString name=0);
    ~KPTDeleteRelationCmd();
    void execute();
    void unexecute();

private:
    KPTRelation *m_rel;
    bool m_taken;
};

class KPTModifyRelationTypeCmd : public KPTNamedCommand
{
public:
    KPTModifyRelationTypeCmd(KPTPart *part, KPTRelation *rel, KPTRelation::Type type, QString name=0);
    void execute();
    void unexecute();

private:
    KPTRelation *m_rel;
    KPTRelation::Type m_newtype;
    KPTRelation::Type m_oldtype;
};

class KPTModifyRelationLagCmd : public KPTNamedCommand
{
public:
    KPTModifyRelationLagCmd(KPTPart *part, KPTRelation *rel, KPTDuration lag, QString name=0);
    void execute();
    void unexecute();

private:
    KPTRelation *m_rel;
    KPTDuration m_newlag;
    KPTDuration m_oldlag;
};

class KPTAddResourceRequestCmd : public KPTNamedCommand
{
public:
    KPTAddResourceRequestCmd(KPTPart *part, KPTResourceGroupRequest *group, KPTResourceRequest *request, QString name=0);
    ~KPTAddResourceRequestCmd();
    void execute();
    void unexecute();

private:
    KPTResourceGroupRequest *m_group;
    KPTResourceRequest *m_request;
    bool m_mine;
};

class KPTRemoveResourceRequestCmd : public KPTNamedCommand
{
public:
    KPTRemoveResourceRequestCmd(KPTPart *part, KPTResourceGroupRequest *group, KPTResourceRequest *request, QString name=0);
    ~KPTRemoveResourceRequestCmd();
    void execute();
    void unexecute();

private:
    KPTResourceGroupRequest *m_group;
    KPTResourceRequest *m_request;
    bool m_mine;
};

class KPTModifyResourceRequestAccountCmd : public KPTNamedCommand
{
public:
    KPTModifyResourceRequestAccountCmd(KPTPart *part, KPTResourceRequest *request, QString account, QString name=0);
    ~KPTModifyResourceRequestAccountCmd();
    void execute();
    void unexecute();

private:
    KPTResourceRequest *m_request;
    KPTAccount *m_oldaccount;
    KPTAccount *m_newaccount;
};

class KPTModifyEffortCmd : public KPTNamedCommand
{
public:
    KPTModifyEffortCmd(KPTPart *part, KPTEffort *effort, KPTDuration oldvalue, KPTDuration newvalue, QString name=0);
    void execute();
    void unexecute();

private:
    KPTEffort *m_effort;
    KPTDuration m_oldvalue, m_newvalue;
};

class KPTEffortModifyOptimisticRatioCmd : public KPTNamedCommand
{
public:
    KPTEffortModifyOptimisticRatioCmd(KPTPart *part, KPTEffort *effort, int oldvalue, int newvalue, QString name=0);
    void execute();
    void unexecute();

private:
    KPTEffort *m_effort;
    int m_oldvalue, m_newvalue;
};

class KPTEffortModifyPessimisticRatioCmd : public KPTNamedCommand
{
public:
    KPTEffortModifyPessimisticRatioCmd(KPTPart *part, KPTEffort *effort, int oldvalue, int newvalue, QString name=0);
    void execute();
    void unexecute();

private:
    KPTEffort *m_effort;
    int m_oldvalue, m_newvalue;
};

class KPTModifyEffortTypeCmd : public KPTNamedCommand
{
public:
    KPTModifyEffortTypeCmd(KPTPart *part, KPTEffort *effort, int oldvalue, int newvalue, QString name=0);
    void execute();
    void unexecute();

private:
    KPTEffort *m_effort;
    int m_oldvalue, m_newvalue;
};

class KPTAddResourceGroupRequestCmd : public KPTNamedCommand
{
public:
    KPTAddResourceGroupRequestCmd(KPTPart *part, KPTTask &task, KPTResourceGroupRequest *request, QString name=0);
    void execute();
    void unexecute();

private:
    KPTTask &m_task;
    KPTResourceGroupRequest *m_request;
    bool m_mine;
};

class KPTRemoveResourceGroupRequestCmd : public KPTNamedCommand
{
public:
    KPTRemoveResourceGroupRequestCmd(KPTPart *part, KPTResourceGroupRequest *request, QString name=0);
    KPTRemoveResourceGroupRequestCmd(KPTPart *part, KPTTask &task, KPTResourceGroupRequest *request, QString name=0);
    void execute();
    void unexecute();

private:
    KPTTask &m_task;
    KPTResourceGroupRequest *m_request;
    bool m_mine;
};

class KPTAddResourceCmd : public KPTNamedCommand
{
public:
    KPTAddResourceCmd(KPTPart *part, KPTResourceGroup *group, KPTResource *resource, QString name=0);
    ~KPTAddResourceCmd();
    void execute();
    void unexecute();

protected:

    KPTResourceGroup *m_group;
    KPTResource *m_resource;
    bool m_mine;
};

class KPTRemoveResourceCmd : public KPTAddResourceCmd
{
public:
    KPTRemoveResourceCmd(KPTPart *part, KPTResourceGroup *group, KPTResource *resource, QString name=0);
    void execute();
    void unexecute();

private:
    QPtrList<KPTResourceRequest> m_requests;
    QPtrList<KPTAppointment> m_appointments;
};

class KPTModifyResourceNameCmd : public KPTNamedCommand
{
public:
    KPTModifyResourceNameCmd(KPTPart *part, KPTResource *resource, QString value, QString name=0);
    void execute();
    void unexecute();

private:

    KPTResource *m_resource;
    QString m_newvalue;
    QString m_oldvalue;
};
class KPTModifyResourceInitialsCmd : public KPTNamedCommand
{
public:
    KPTModifyResourceInitialsCmd(KPTPart *part, KPTResource *resource, QString value, QString name=0);
    void execute();
    void unexecute();

private:
    KPTResource *m_resource;
    QString m_newvalue;
    QString m_oldvalue;
};
class KPTModifyResourceEmailCmd : public KPTNamedCommand
{
public:
    KPTModifyResourceEmailCmd(KPTPart *part, KPTResource *resource, QString value, QString name=0);
    void execute();
    void unexecute();

private:
    KPTResource *m_resource;
    QString m_newvalue;
    QString m_oldvalue;
};
class KPTModifyResourceTypeCmd : public KPTNamedCommand
{
public:
    KPTModifyResourceTypeCmd(KPTPart *part, KPTResource *resource, int value, QString name=0);
    void execute();
    void unexecute();

private:
    KPTResource *m_resource;
    int m_newvalue;
    int m_oldvalue;
};

class KPTModifyResourceNormalRateCmd : public KPTNamedCommand
{
public:
    KPTModifyResourceNormalRateCmd(KPTPart *part, KPTResource *resource, double value, QString name=0);
    void execute();
    void unexecute();

private:
    KPTResource *m_resource;
    double m_newvalue;
    double m_oldvalue;
};
class KPTModifyResourceOvertimeRateCmd : public KPTNamedCommand
{
public:
    KPTModifyResourceOvertimeRateCmd(KPTPart *part, KPTResource *resource, double value, QString name=0);
    void execute();
    void unexecute();

private:
    KPTResource *m_resource;
    double m_newvalue;
    double m_oldvalue;
};
class KPTModifyResourceFixedCostCmd : public KPTNamedCommand
{
public:
    KPTModifyResourceFixedCostCmd(KPTPart *part, KPTResource *resource, double value, QString name=0);
    void execute();
    void unexecute();

private:
    KPTResource *m_resource;
    double m_newvalue;
    double m_oldvalue;
};

class KPTRemoveResourceGroupCmd : public KPTNamedCommand
{
public:
    KPTRemoveResourceGroupCmd(KPTPart *part, KPTResourceGroup *group, QString name=0);
    ~KPTRemoveResourceGroupCmd();
    void execute();
    void unexecute();

protected:

    KPTResourceGroup *m_group;
    bool m_mine;
};

class KPTAddResourceGroupCmd : public KPTRemoveResourceGroupCmd
{
public:
    KPTAddResourceGroupCmd(KPTPart *part, KPTResourceGroup *group, QString name=0);
    void execute();
    void unexecute();
};

class KPTModifyResourceGroupNameCmd : public KPTNamedCommand
{
public:
    KPTModifyResourceGroupNameCmd(KPTPart *part, KPTResourceGroup *group, QString value, QString name=0);
    void execute();
    void unexecute();

private:
    KPTResourceGroup *m_group;
    QString m_newvalue;
    QString m_oldvalue;
};

class KPTTaskModifyProgressCmd : public KPTNamedCommand
{
public:
    KPTTaskModifyProgressCmd(KPTPart *part, KPTTask &task, struct KPTTask::Progress &value, QString name=0);
    void execute();
    void unexecute();

private:
    KPTTask &m_task;
    struct KPTTask::Progress m_newvalue;
    struct KPTTask::Progress m_oldvalue;
};

class KPTProjectModifyBaselineCmd : public KPTNamedCommand
{
public:
    KPTProjectModifyBaselineCmd(KPTPart *part, KPTProject &project, bool value, QString name=0);
    void execute();
    void unexecute();

private:
    KPTProject &m_project;
    bool m_newvalue;
    bool m_oldvalue;
};

class KPTAddAccountCmd : public KPTNamedCommand
{
public:
    KPTAddAccountCmd(KPTPart *part, KPTProject &project, KPTAccount *account,  KPTAccount *parent=0, QString name=0);
    KPTAddAccountCmd(KPTPart *part, KPTProject &project, KPTAccount *account,  QString parent, QString name=0);
    ~KPTAddAccountCmd();
    void execute();
    void unexecute();

protected:
    bool m_mine;
    
private:
    KPTProject &m_project;
    KPTAccount *m_account;
    KPTAccount *m_parent;
    QString m_parentName;
};

class KPTRemoveAccountCmd : public KPTNamedCommand
{
public:
    KPTRemoveAccountCmd(KPTPart *part, KPTProject &project, KPTAccount *account, QString name=0);
    ~KPTRemoveAccountCmd();
    void execute();
    void unexecute();
    
private:
    KPTProject &m_project;
    KPTAccount *m_account;
    bool m_mine;

};

class KPTRenameAccountCmd : public KPTNamedCommand
{
public:
    KPTRenameAccountCmd(KPTPart *part, KPTAccount *account, QString value, QString name=0);
    void execute();
    void unexecute();

private:
    KPTAccount *m_account;
    QString m_oldvalue;
    QString m_newvalue;
};

class KPTModifyAccountDescriptionCmd : public KPTNamedCommand
{
public:
    KPTModifyAccountDescriptionCmd(KPTPart *part, KPTAccount *account, QString value, QString name=0);
    void execute();
    void unexecute();

private:
    KPTAccount *m_account;
    QString m_oldvalue;
    QString m_newvalue;
};


}  //KPlato namespace

#endif //KPTCOMMAND_H
