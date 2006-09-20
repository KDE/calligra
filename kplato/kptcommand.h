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

#include "kptappointment.h"
#include "kptnode.h"
#include "kptduration.h"
#include "kptpart.h"
#include "kpttask.h"
//Added by qt3to4:
#include <Q3PtrList>

class QString;

namespace KPlato
{

class Account;
class Accounts;
class Project;
class Task;
class Calendar;
class CalendarDay;
class Relation;
class ResourceGroupRequest;
class ResourceRequest;
class ResourceGroup;
class Resource;
class Schedule;
class StandardWorktime;

class NamedCommand : public KNamedCommand
{
public:
    NamedCommand(Part *part, QString name)
    : KNamedCommand(name), m_part(part)
    {}
    
    void setCommandType(int type);
    
protected:
    void setSchDeleted();
    void setSchDeleted(bool state);
    void setSchScheduled();
    void setSchScheduled(bool state);
    void addSchScheduled(Schedule *sch);
    void addSchDeleted(Schedule *sch);
    
    QMap<Schedule*, bool> m_schedules;
private:
    Part *m_part;
};

class CalendarAddCmd : public NamedCommand
{
public:
    CalendarAddCmd(Part *part, Project *project, Calendar *cal, QString name=0);
    void execute();
    void unexecute();

private:
    Project *m_project;
    Calendar *m_cal;
    bool m_added;
};

class CalendarDeleteCmd : public NamedCommand
{
public:
    CalendarDeleteCmd(Part *part, Calendar *cal, QString name=0);
    void execute();
    void unexecute();

private:
    Calendar *m_cal;
};

class CalendarModifyNameCmd : public NamedCommand
{
public:
    CalendarModifyNameCmd(Part *part, Calendar *cal, QString newvalue, QString name=0);
    void execute();
    void unexecute();

private:
    Calendar *m_cal;
    QString m_newvalue;
    QString m_oldvalue;
};

class CalendarModifyParentCmd : public NamedCommand
{
public:
    CalendarModifyParentCmd(Part *part, Calendar *cal, Calendar *newvalue, QString name=0);
    void execute();
    void unexecute();

private:
    Calendar *m_cal;
    Calendar *m_newvalue;
    Calendar *m_oldvalue;
};

class CalendarAddDayCmd : public NamedCommand
{
public:
    CalendarAddDayCmd(Part *part, Calendar *cal, CalendarDay *newvalue, QString name=0);
    ~CalendarAddDayCmd();
    void execute();
    void unexecute();

protected:
    Calendar *m_cal;
    CalendarDay *m_newvalue;
    bool m_mine;
};

class CalendarRemoveDayCmd : public NamedCommand
{
public:
    CalendarRemoveDayCmd(Part *part, Calendar *cal, const QDate &day, QString name=0);
    void execute();
    void unexecute();

protected:
    Calendar *m_cal;
    CalendarDay *m_value;
    bool m_mine;
};

class CalendarModifyDayCmd : public NamedCommand
{
public:
    CalendarModifyDayCmd(Part *part, Calendar *cal, CalendarDay *value, QString name=0);
    ~CalendarModifyDayCmd();
    void execute();
    void unexecute();

private:
    Calendar *m_cal;
    CalendarDay *m_newvalue;
    CalendarDay *m_oldvalue;
    bool m_mine;
};

class CalendarModifyWeekdayCmd : public NamedCommand
{
public:
    CalendarModifyWeekdayCmd(Part *part, Calendar *cal, int weekday, CalendarDay *value, QString name=0);
    ~CalendarModifyWeekdayCmd();
    void execute();
    void unexecute();

private:
    int m_weekday;
    Calendar *m_cal;
    CalendarDay *m_value;
    bool m_mine;
};


class NodeDeleteCmd : public NamedCommand
{
public:
    NodeDeleteCmd(Part *part, Node *node, QString name=0);
    ~NodeDeleteCmd();
    void execute();
    void unexecute();

private:
    Node *m_node;
    Node *m_parent;
    Project *m_project;
    int m_index;
    bool m_mine;
    Q3PtrList<Appointment> m_appointments;

};

class TaskAddCmd : public NamedCommand
{
public:
    TaskAddCmd(Part *part, Project *project, Node *node, Node *after,  QString name=0);
    ~TaskAddCmd();
    void execute();
    void unexecute();

private:
    Project *m_project;
    Node *m_node;
    Node *m_after;
    bool m_added;
};

class SubtaskAddCmd : public NamedCommand
{
public:
    SubtaskAddCmd(Part *part, Project *project, Node *node, Node *parent,  QString name=0);
    ~SubtaskAddCmd();
    void execute();
    void unexecute();

private:
    Project *m_project;
    Node *m_node;
    Node *m_parent;
    bool m_added;
};


class NodeModifyNameCmd : public NamedCommand
{
public:
    NodeModifyNameCmd(Part *part, Node &node, QString nodename, QString name=0);
    void execute();
    void unexecute();

private:
    Node &m_node;
    QString newName;
    QString oldName;
};

class NodeModifyLeaderCmd : public NamedCommand
{
public:
    NodeModifyLeaderCmd(Part *part, Node &node, QString leader, QString name=0);
    void execute();
    void unexecute();

private:
    Node &m_node;
    QString newLeader;
    QString oldLeader;
};

class NodeModifyDescriptionCmd : public NamedCommand
{
public:
    NodeModifyDescriptionCmd(Part *part, Node &node, QString description, QString name=0);
    void execute();
    void unexecute();

private:
    Node &m_node;
    QString newDescription;
    QString oldDescription;
};

class NodeModifyConstraintCmd : public NamedCommand
{
public:
    NodeModifyConstraintCmd(Part *part, Node &node, Node::ConstraintType c, QString name=0);
    void execute();
    void unexecute();

private:
    Node &m_node;
    Node::ConstraintType newConstraint;
    Node::ConstraintType oldConstraint;

};

class NodeModifyConstraintStartTimeCmd : public NamedCommand
{
public:
    NodeModifyConstraintStartTimeCmd(Part *part, Node &node, QDateTime dt, QString name=0);
    void execute();
    void unexecute();

private:
    Node &m_node;
    QDateTime newTime;
    QDateTime oldTime;

};
class NodeModifyConstraintEndTimeCmd : public NamedCommand
{
public:
    NodeModifyConstraintEndTimeCmd(Part *part, Node &node, QDateTime dt, QString name=0);
    void execute();
    void unexecute();

private:
    Node &m_node;
    QDateTime newTime;
    QDateTime oldTime;

};
class NodeModifyStartTimeCmd : public NamedCommand
{
public:
    NodeModifyStartTimeCmd(Part *part, Node &node, QDateTime dt, QString name=0);
    void execute();
    void unexecute();

private:
    Node &m_node;
    QDateTime newTime;
    QDateTime oldTime;
};
class NodeModifyEndTimeCmd : public NamedCommand
{
public:
    NodeModifyEndTimeCmd(Part *part, Node &node, QDateTime dt, QString name=0);
    void execute();
    void unexecute();

private:
    Node &m_node;
    QDateTime newTime;
    QDateTime oldTime;
};
class NodeModifyIdCmd : public NamedCommand
{
public:
    NodeModifyIdCmd(Part *part, Node &node, QString id, QString name=0);
    void execute();
    void unexecute();

private:
    Node &m_node;
    QString newId;
    QString oldId;
};

class NodeIndentCmd : public NamedCommand
{
public:
    NodeIndentCmd(Part *part, Node &node, QString name=0);
    void execute();
    void unexecute();

private:
    Node &m_node;
    Node *m_oldparent, *m_newparent;
    int m_oldindex, m_newindex;
};

class NodeUnindentCmd : public NamedCommand
{
public:
    NodeUnindentCmd(Part *part, Node &node, QString name=0);
    void execute();
    void unexecute();

private:
    Node &m_node;
    Node *m_oldparent, *m_newparent;
    int m_oldindex, m_newindex;
};

class NodeMoveUpCmd : public NamedCommand
{
public:
    NodeMoveUpCmd(Part *part, Node &node, QString name=0);
    void execute();
    void unexecute();

private:
    Node &m_node;
    Project *m_project;
    bool m_moved;
};

class NodeMoveDownCmd : public NamedCommand
{
public:
    NodeMoveDownCmd(Part *part, Node &node, QString name=0);
    void execute();
    void unexecute();

private:
    Node &m_node;
    Project *m_project;
    bool m_moved;
};

class AddRelationCmd : public NamedCommand
{
public:
    AddRelationCmd(Part *part, Relation *rel, QString name=0);
    ~AddRelationCmd();
    void execute();
    void unexecute();

private:
    Relation *m_rel;
    bool m_taken;

};

class DeleteRelationCmd : public NamedCommand
{
public:
    DeleteRelationCmd(Part *part, Relation *rel, QString name=0);
    ~DeleteRelationCmd();
    void execute();
    void unexecute();

private:
    Relation *m_rel;
    bool m_taken;

};

class ModifyRelationTypeCmd : public NamedCommand
{
public:
    ModifyRelationTypeCmd(Part *part, Relation *rel, Relation::Type type, QString name=0);
    void execute();
    void unexecute();

private:
    Relation *m_rel;
    Relation::Type m_newtype;
    Relation::Type m_oldtype;

};

class ModifyRelationLagCmd : public NamedCommand
{
public:
    ModifyRelationLagCmd(Part *part, Relation *rel, Duration lag, QString name=0);
    void execute();
    void unexecute();

private:
    Relation *m_rel;
    Duration m_newlag;
    Duration m_oldlag;

};

class AddResourceRequestCmd : public NamedCommand
{
public:
    AddResourceRequestCmd(Part *part, ResourceGroupRequest *group, ResourceRequest *request, QString name=0);
    ~AddResourceRequestCmd();
    void execute();
    void unexecute();

private:
    ResourceGroupRequest *m_group;
    ResourceRequest *m_request;
    bool m_mine;

};

class RemoveResourceRequestCmd : public NamedCommand
{
public:
    RemoveResourceRequestCmd(Part *part, ResourceGroupRequest *group, ResourceRequest *request, QString name=0);
    ~RemoveResourceRequestCmd();
    void execute();
    void unexecute();

private:
    ResourceGroupRequest *m_group;
    ResourceRequest *m_request;
    bool m_mine;

};

class ModifyEffortCmd : public NamedCommand
{
public:
    ModifyEffortCmd(Part *part, Node &node, Duration oldvalue, Duration newvalue, QString name=0);
    void execute();
    void unexecute();

private:
    Effort *m_effort;
    Duration m_oldvalue, m_newvalue;

};

class EffortModifyOptimisticRatioCmd : public NamedCommand
{
public:
    EffortModifyOptimisticRatioCmd(Part *part, Node &node, int oldvalue, int newvalue, QString name=0);
    void execute();
    void unexecute();

private:
    Effort *m_effort;
    int m_oldvalue, m_newvalue;

};

class EffortModifyPessimisticRatioCmd : public NamedCommand
{
public:
    EffortModifyPessimisticRatioCmd(Part *part, Node &node, int oldvalue, int newvalue, QString name=0);
    void execute();
    void unexecute();

private:
    Effort *m_effort;
    int m_oldvalue, m_newvalue;

};

class ModifyEffortTypeCmd : public NamedCommand
{
public:
    ModifyEffortTypeCmd(Part *part, Node &node, int oldvalue, int newvalue, QString name=0);
    void execute();
    void unexecute();

private:
    Effort *m_effort;
    int m_oldvalue, m_newvalue;

};

class EffortModifyRiskCmd : public NamedCommand
{
public:
    EffortModifyRiskCmd(Part *part, Node &node, int oldvalue, int newvalue, QString name=0);
    void execute();
    void unexecute();

private:
    Effort *m_effort;
    int m_oldvalue, m_newvalue;

};

class AddResourceGroupRequestCmd : public NamedCommand
{
public:
    AddResourceGroupRequestCmd(Part *part, Task &task, ResourceGroupRequest *request, QString name=0);
    void execute();
    void unexecute();

private:
    Task &m_task;
    ResourceGroupRequest *m_request;
    bool m_mine;
};

class RemoveResourceGroupRequestCmd : public NamedCommand
{
public:
    RemoveResourceGroupRequestCmd(Part *part, ResourceGroupRequest *request, QString name=0);
    RemoveResourceGroupRequestCmd(Part *part, Task &task, ResourceGroupRequest *request, QString name=0);
    void execute();
    void unexecute();

private:
    Task &m_task;
    ResourceGroupRequest *m_request;
    bool m_mine;
};

class AddResourceCmd : public NamedCommand
{
public:
    AddResourceCmd(Part *part, ResourceGroup *group, Resource *resource, QString name=0);
    ~AddResourceCmd();
    void execute();
    void unexecute();

protected:

    ResourceGroup *m_group;
    Resource *m_resource;
    bool m_mine;
};

class RemoveResourceCmd : public AddResourceCmd
{
public:
    RemoveResourceCmd(Part *part, ResourceGroup *group, Resource *resource, QString name=0);
    ~RemoveResourceCmd();
    void execute();
    void unexecute();

private:
    Q3PtrList<ResourceRequest> m_requests;
    Q3PtrList<Appointment> m_appointments;
};

class ModifyResourceNameCmd : public NamedCommand
{
public:
    ModifyResourceNameCmd(Part *part, Resource *resource, QString value, QString name=0);
    void execute();
    void unexecute();

private:

    Resource *m_resource;
    QString m_newvalue;
    QString m_oldvalue;
};
class ModifyResourceInitialsCmd : public NamedCommand
{
public:
    ModifyResourceInitialsCmd(Part *part, Resource *resource, QString value, QString name=0);
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    QString m_newvalue;
    QString m_oldvalue;
};
class ModifyResourceEmailCmd : public NamedCommand
{
public:
    ModifyResourceEmailCmd(Part *part, Resource *resource, QString value, QString name=0);
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    QString m_newvalue;
    QString m_oldvalue;
};
class ModifyResourceTypeCmd : public NamedCommand
{
public:
    ModifyResourceTypeCmd(Part *part, Resource *resource, int value, QString name=0);
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    int m_newvalue;
    int m_oldvalue;
};

class ModifyResourceUnitsCmd : public NamedCommand
{
public:
    ModifyResourceUnitsCmd(Part *part, Resource *resource, int value, QString name=0);
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    int m_newvalue;
    int m_oldvalue;
};

class ModifyResourceAvailableFromCmd : public NamedCommand
{
public:
    ModifyResourceAvailableFromCmd(Part *part, Resource *resource, DateTime value, QString name=0);
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    DateTime m_newvalue;
    DateTime m_oldvalue;
};
class ModifyResourceAvailableUntilCmd : public NamedCommand
{
public:
    ModifyResourceAvailableUntilCmd(Part *part, Resource *resource, DateTime value, QString name=0);
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    DateTime m_newvalue;
    DateTime m_oldvalue;
};

class ModifyResourceNormalRateCmd : public NamedCommand
{
public:
    ModifyResourceNormalRateCmd(Part *part, Resource *resource, double value, QString name=0);
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    double m_newvalue;
    double m_oldvalue;
};
class ModifyResourceOvertimeRateCmd : public NamedCommand
{
public:
    ModifyResourceOvertimeRateCmd(Part *part, Resource *resource, double value, QString name=0);
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    double m_newvalue;
    double m_oldvalue;
};
class ModifyResourceCalendarCmd : public NamedCommand
{
public:
    ModifyResourceCalendarCmd(Part *part, Resource *resource, Calendar *value, QString name=0);
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    Calendar *m_newvalue;
    Calendar *m_oldvalue;
};

class RemoveResourceGroupCmd : public NamedCommand
{
public:
    RemoveResourceGroupCmd(Part *part, ResourceGroup *group, QString name=0);
    ~RemoveResourceGroupCmd();
    void execute();
    void unexecute();

protected:

    ResourceGroup *m_group;
    bool m_mine;
};

class AddResourceGroupCmd : public RemoveResourceGroupCmd
{
public:
    AddResourceGroupCmd(Part *part, ResourceGroup *group, QString name=0);
    void execute();
    void unexecute();
};

class ModifyResourceGroupNameCmd : public NamedCommand
{
public:
    ModifyResourceGroupNameCmd(Part *part, ResourceGroup *group, QString value, QString name=0);
    void execute();
    void unexecute();

private:
    ResourceGroup *m_group;
    QString m_newvalue;
    QString m_oldvalue;
};

class TaskModifyProgressCmd : public NamedCommand
{
public:
    TaskModifyProgressCmd(Part *part, Task &task, struct Task::Progress &value, QString name=0);
    void execute();
    void unexecute();

private:
    Task &m_task;
    struct Task::Progress m_newvalue;
    struct Task::Progress m_oldvalue;
};

class ProjectModifyBaselineCmd : public NamedCommand
{
public:
    ProjectModifyBaselineCmd(Part *part, Project &project, bool value, QString name=0);
    void execute();
    void unexecute();

private:
    Project &m_project;
    bool m_newvalue;
    bool m_oldvalue;
};

class AddAccountCmd : public NamedCommand
{
public:
    AddAccountCmd(Part *part, Project &project, Account *account,  Account *parent=0, QString name=0);
    AddAccountCmd(Part *part, Project &project, Account *account,  QString parent, QString name=0);
    ~AddAccountCmd();
    void execute();
    void unexecute();

protected:
    bool m_mine;
    
private:
    Project &m_project;
    Account *m_account;
    Account *m_parent;
    QString m_parentName;
};

class RemoveAccountCmd : public NamedCommand
{
public:
    RemoveAccountCmd(Part *part, Project &project, Account *account, QString name=0);
    ~RemoveAccountCmd();
    void execute();
    void unexecute();
    
private:
    Project &m_project;
    Account *m_account;
    bool m_isDefault;
    bool m_mine;

};

class RenameAccountCmd : public NamedCommand
{
public:
    RenameAccountCmd(Part *part, Account *account, QString value, QString name=0);
    void execute();
    void unexecute();

private:
    Account *m_account;
    QString m_oldvalue;
    QString m_newvalue;
};

class ModifyAccountDescriptionCmd : public NamedCommand
{
public:
    ModifyAccountDescriptionCmd(Part *part, Account *account, QString value, QString name=0);
    void execute();
    void unexecute();

private:
    Account *m_account;
    QString m_oldvalue;
    QString m_newvalue;
};

class NodeModifyStartupCostCmd : public NamedCommand
{
public:
    NodeModifyStartupCostCmd(Part *part, Node &node, double value, QString name=0);
    void execute();
    void unexecute();

private:
    Node &m_node;
    double m_oldvalue;
    double m_newvalue;
};

class NodeModifyShutdownCostCmd : public NamedCommand
{
public:
    NodeModifyShutdownCostCmd(Part *part, Node &node, double value, QString name=0);
    void execute();
    void unexecute();

private:
    Node &m_node;
    double m_oldvalue;
    double m_newvalue;
};

class NodeModifyRunningAccountCmd : public NamedCommand
{
public:
    NodeModifyRunningAccountCmd(Part *part, Node &node, Account *oldvalue, Account *newvalue, QString name=0);
    void execute();
    void unexecute();

private:
    Node &m_node;
    Account *m_oldvalue;
    Account *m_newvalue;
};

class NodeModifyStartupAccountCmd : public NamedCommand
{
public:
    NodeModifyStartupAccountCmd(Part *part, Node &node, Account *oldvalue, Account *newvalue, QString name=0);
    void execute();
    void unexecute();

private:
    Node &m_node;
    Account *m_oldvalue;
    Account *m_newvalue;
};

class NodeModifyShutdownAccountCmd : public NamedCommand
{
public:
    NodeModifyShutdownAccountCmd(Part *part, Node &node, Account *oldvalue, Account *newvalue, QString name=0);
    void execute();
    void unexecute();

private:
    Node &m_node;
    Account *m_oldvalue;
    Account *m_newvalue;
};

class ModifyDefaultAccountCmd : public NamedCommand
{
public:
    ModifyDefaultAccountCmd(Part *part, Accounts &acc, Account *oldvalue, Account *newvalue, QString name=0);
    void execute();
    void unexecute();

private:
    Accounts &m_accounts;
    Account *m_oldvalue;
    Account *m_newvalue;
};

class ProjectModifyConstraintCmd : public NamedCommand
{
public:
    ProjectModifyConstraintCmd(Part *part, Project &node, Node::ConstraintType c, QString name=0);
    void execute();
    void unexecute();

private:
    Project &m_node;
    Node::ConstraintType newConstraint;
    Node::ConstraintType oldConstraint;

};

class ProjectModifyStartTimeCmd : public NamedCommand
{
public:
    ProjectModifyStartTimeCmd(Part *part, Project &node, QDateTime dt, QString name=0);
    void execute();
    void unexecute();

private:
    Project &m_node;
    QDateTime newTime;
    QDateTime oldTime;

};

class ProjectModifyEndTimeCmd : public NamedCommand
{
public:
    ProjectModifyEndTimeCmd(Part *part, Project &project, QDateTime dt, QString name=0);
    void execute();
    void unexecute();

private:
    Project &m_node;
    QDateTime newTime;
    QDateTime oldTime;

};


class CalculateProjectCmd : public NamedCommand
{
public:
    CalculateProjectCmd(Part *part, Project &project, QString tname, int type, QString name=0);
    void execute();
    void unexecute();

private:
    Project &m_node;
    QString m_typename;
    int m_type;
    Schedule *newSchedule;
    Schedule *oldCurrent;
};

class RecalculateProjectCmd : public NamedCommand
{
public:
    RecalculateProjectCmd(Part *part, Project &project, Schedule &sch, QString name=0);
    void execute();
    void unexecute();

private:
    Project &m_node;
    Schedule &oldSchedule;
    Schedule *newSchedule;
    bool oldDeleted;
    Schedule *oldCurrent;
};

class ModifyStandardWorktimeYearCmd : public NamedCommand
{
public:
    ModifyStandardWorktimeYearCmd(Part *part, StandardWorktime *wt, double oldvalue, double newvalue, QString name=0);
    void execute();
    void unexecute();
private:
    StandardWorktime *swt;
    double m_oldvalue;
    double m_newvalue;
};

class ModifyStandardWorktimeMonthCmd : public NamedCommand
{
public:
    ModifyStandardWorktimeMonthCmd(Part *part, StandardWorktime *wt, double oldvalue, double newvalue, QString name=0);
    void execute();
    void unexecute();
private:
    StandardWorktime *swt;
    double m_oldvalue;
    double m_newvalue;
};

class ModifyStandardWorktimeWeekCmd : public NamedCommand
{
public:
    ModifyStandardWorktimeWeekCmd(Part *part, StandardWorktime *wt, double oldvalue, double newvalue, QString name=0);
    void execute();
    void unexecute();
private:
    StandardWorktime *swt;
    double m_oldvalue;
    double m_newvalue;
};

class ModifyStandardWorktimeDayCmd : public NamedCommand
{
public:
    ModifyStandardWorktimeDayCmd(Part *part, StandardWorktime *wt, double oldvalue, double newvalue, QString name=0);
    void execute();
    void unexecute();
private:
    StandardWorktime *swt;
    double m_oldvalue;
    double m_newvalue;
};


}  //KPlato namespace

#endif //COMMAND_H
