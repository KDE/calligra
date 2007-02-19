/* This file is part of the KDE project
  Copyright (C) 2004-2007 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTCOMMAND_H
#define KPTCOMMAND_H

#include <kcommand.h>

#include "kptappointment.h"
#include "kptnode.h"
#include "kptduration.h"
#include "kptpart.h"
#include "kpttask.h"

#include <QList>

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
    NamedCommand( Part *part, const QString& name )
            : KNamedCommand( name ), m_part( part )
    {}

    void setCommandType( int type );

protected:
    void setSchDeleted();
    void setSchDeleted( bool state );
    void setSchScheduled();
    void setSchScheduled( bool state );
    void addSchScheduled( Schedule *sch );
    void addSchDeleted( Schedule *sch );

    QMap<Schedule*, bool> m_schedules;

protected:
    Part *m_part;
};

class CalendarAddCmd : public NamedCommand
{
public:
    CalendarAddCmd( Part *part, Project *project, Calendar *cal, Calendar *parent, const QString& name  = QString());
    ~CalendarAddCmd();
    void execute();
    void unexecute();

private:
    Project *m_project;
    Calendar *m_cal;
    Calendar *m_parent;
    bool m_mine;
};

class CalendarRemoveCmd : public NamedCommand
{
public:
    CalendarRemoveCmd( Part *part, Project *project, Calendar *cal, const QString& name = QString() );
    ~CalendarRemoveCmd();
    void execute();
    void unexecute();

private:
    Project *m_project;
    Calendar *m_parent;
    Calendar *m_cal;
    bool m_mine;
    KMacroCommand *m_cmd;
};

class CalendarModifyNameCmd : public NamedCommand
{
public:
    CalendarModifyNameCmd( Part *part, Calendar *cal, const QString& newvalue, const QString& name = QString() );
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
    CalendarModifyParentCmd( Part *part, Project *project, Calendar *cal, Calendar *newvalue, const QString& name = QString() );
    ~CalendarModifyParentCmd();
    void execute();
    void unexecute();

private:
    Calendar *m_cal;
    Project *m_project;
    Calendar *m_newvalue;
    Calendar *m_oldvalue;
    KMacroCommand *m_cmd;
};

class CalendarModifyTimeZoneCmd : public NamedCommand
{
public:
    CalendarModifyTimeZoneCmd( Part *part, Calendar *cal, const KTimeZone *value, const QString& name = QString() );
    ~CalendarModifyTimeZoneCmd();
    void execute();
    void unexecute();

private:
    Calendar *m_cal;
    const KTimeZone *m_newvalue;
    const KTimeZone *m_oldvalue;
    KMacroCommand *m_cmd;
};

class CalendarAddDayCmd : public NamedCommand
{
public:
    CalendarAddDayCmd( Part *part, Calendar *cal, CalendarDay *newvalue, const QString& name = QString() );
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
    CalendarRemoveDayCmd( Part *part, Calendar *cal, CalendarDay *day, const QString& name = QString() );
    CalendarRemoveDayCmd( Part *part, Calendar *cal, const QDate &day, const QString& name = QString() );
    void execute();
    void unexecute();

protected:
    Calendar *m_cal;
    CalendarDay *m_value;
    bool m_mine;

private:
    void init();
};

class CalendarModifyDayCmd : public NamedCommand
{
public:
    CalendarModifyDayCmd( Part *part, Calendar *cal, CalendarDay *value, const QString& name = QString() );
    ~CalendarModifyDayCmd();
    void execute();
    void unexecute();

private:
    Calendar *m_cal;
    CalendarDay *m_newvalue;
    CalendarDay *m_oldvalue;
    bool m_mine;
};

class CalendarModifyStateCmd : public NamedCommand
{
public:
    CalendarModifyStateCmd( Part *part, Calendar *calendar, CalendarDay *day, CalendarDay::State value, const QString& name = QString() );
    ~CalendarModifyStateCmd();
    void execute();
    void unexecute();

private:
    Calendar *m_calendar;
    CalendarDay *m_day;
    CalendarDay::State m_newvalue;
    CalendarDay::State m_oldvalue;
    KMacroCommand *m_cmd;
};

class CalendarModifyTimeIntervalCmd : public NamedCommand
{
public:
    CalendarModifyTimeIntervalCmd( Part *part, Calendar *calendar, TimeInterval &newvalue, TimeInterval *value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Calendar *m_calendar;
    TimeInterval *m_value;
    TimeInterval m_newvalue;
    TimeInterval m_oldvalue;
};

class CalendarAddTimeIntervalCmd : public NamedCommand
{
public:
    CalendarAddTimeIntervalCmd( Part *part, Calendar *calendar, CalendarDay *day, TimeInterval *value, const QString& name = QString() );
    ~CalendarAddTimeIntervalCmd();
    void execute();
    void unexecute();

protected:
    Calendar *m_calendar;
    CalendarDay *m_day;
    TimeInterval *m_value;
    bool m_mine;
};

class CalendarRemoveTimeIntervalCmd : public CalendarAddTimeIntervalCmd
{
public:
    CalendarRemoveTimeIntervalCmd( Part *part, Calendar *calendar, CalendarDay *day, TimeInterval *value, const QString& name = QString() );
    
    void execute();
    void unexecute();
};

class CalendarModifyWeekdayCmd : public NamedCommand
{
public:
    CalendarModifyWeekdayCmd( Part *part, Calendar *cal, int weekday, CalendarDay *value, const QString& name = QString() );
    ~CalendarModifyWeekdayCmd();
    void execute();
    void unexecute();

private:
    int m_weekday;
    Calendar *m_cal;
    CalendarDay *m_value;
    CalendarDay m_orig;
};

class CalendarModifyDateCmd : public NamedCommand
{
public:
    CalendarModifyDateCmd( Part *part, Calendar *cal, CalendarDay *day, QDate &value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Calendar *m_cal;
    CalendarDay *m_day;
    QDate m_newvalue, m_oldvalue;
};


class NodeDeleteCmd : public NamedCommand
{
public:
    NodeDeleteCmd( Part *part, Node *node, const QString& name = QString() );
    ~NodeDeleteCmd();
    void execute();
    void unexecute();

private:
    Node *m_node;
    Node *m_parent;
    Project *m_project;
    int m_index;
    bool m_mine;
    QList<Appointment*> m_appointments;
    KMacroCommand *m_cmd;
};

class TaskAddCmd : public NamedCommand
{
public:
    TaskAddCmd( Part *part, Project *project, Node *node, Node *after, const QString& name = QString() );
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
    SubtaskAddCmd( Part *part, Project *project, Node *node, Node *parent, const QString& name = QString() );
    ~SubtaskAddCmd();
    void execute();
    void unexecute();

private:
    Project *m_project;
    Node *m_node;
    Node *m_parent;
    bool m_added;
    KMacroCommand *m_cmd;
};


class NodeModifyNameCmd : public NamedCommand
{
public:
    NodeModifyNameCmd( Part *part, Node &node, const QString& nodename, const QString& name = QString() );
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
    NodeModifyLeaderCmd( Part *part, Node &node, const QString& leader, const QString& name = QString() );
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
    NodeModifyDescriptionCmd( Part *part, Node &node, const QString& description, const QString& name = QString() );
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
    NodeModifyConstraintCmd( Part *part, Node &node, Node::ConstraintType c, const QString& name = QString() );
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
    NodeModifyConstraintStartTimeCmd( Part *part, Node &node, const QDateTime& dt, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    QDateTime newTime;
    DateTime oldTime;
    KDateTime::Spec m_spec;
};
class NodeModifyConstraintEndTimeCmd : public NamedCommand
{
public:
    NodeModifyConstraintEndTimeCmd( Part *part, Node &node, const QDateTime& dt, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    QDateTime newTime;
    DateTime oldTime;
    KDateTime::Spec m_spec;
};
class NodeModifyStartTimeCmd : public NamedCommand
{
public:
    NodeModifyStartTimeCmd( Part *part, Node &node, const QDateTime& dt, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    QDateTime newTime;
    DateTime oldTime;
    KDateTime::Spec m_spec;
};
class NodeModifyEndTimeCmd : public NamedCommand
{
public:
    NodeModifyEndTimeCmd( Part *part, Node &node, const QDateTime& dt, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    QDateTime newTime;
    DateTime oldTime;
    KDateTime::Spec m_spec;
};
class NodeModifyIdCmd : public NamedCommand
{
public:
    NodeModifyIdCmd( Part *part, Node &node, const QString& id, const QString& name = QString() );
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
    NodeIndentCmd( Part *part, Node &node, const QString& name = QString() );
    ~NodeIndentCmd();
    void execute();
    void unexecute();

private:
    Node &m_node;
    Node *m_oldparent, *m_newparent;
    int m_oldindex, m_newindex;
    KMacroCommand *m_cmd;
};

class NodeUnindentCmd : public NamedCommand
{
public:
    NodeUnindentCmd( Part *part, Node &node, const QString& name = QString() );
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
    NodeMoveUpCmd( Part *part, Node &node, const QString& name = QString() );
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
    NodeMoveDownCmd( Part *part, Node &node, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    Project *m_project;
    bool m_moved;
};

class NodeMoveCmd : public NamedCommand
{
public:
    NodeMoveCmd( Part *part, Project *project, Node *node, Node *newParent, int newPos, const QString& name = QString() );
    ~NodeMoveCmd();
    void execute();
    void unexecute();

private:
    Project *m_project;
    Node *m_node;
    Node *m_newparent;
    Node *m_oldparent;
    int m_newpos;
    int m_oldpos;
    bool m_moved;
    KMacroCommand *m_cmd;
};

class AddRelationCmd : public NamedCommand
{
public:
    AddRelationCmd( Part *part, Relation *rel, const QString& name = QString() );
    ~AddRelationCmd();
    void execute();
    void unexecute();

private:
    Relation *m_rel;
    Project &m_project;
    bool m_taken;

};

class DeleteRelationCmd : public NamedCommand
{
public:
    DeleteRelationCmd( Part *part, Relation *rel, const QString& name = QString() );
    ~DeleteRelationCmd();
    void execute();
    void unexecute();

private:
    Relation *m_rel;
    Project &m_project;
    bool m_taken;

};

class ModifyRelationTypeCmd : public NamedCommand
{
public:
    ModifyRelationTypeCmd( Part *part, Relation *rel, Relation::Type type, const QString& name = QString() );
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
    ModifyRelationLagCmd( Part *part, Relation *rel, Duration lag, const QString& name = QString() );
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
    AddResourceRequestCmd( Part *part, ResourceGroupRequest *group, ResourceRequest *request, const QString& name = QString() );
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
    RemoveResourceRequestCmd( Part *part, ResourceGroupRequest *group, ResourceRequest *request, const QString& name = QString() );
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
    ModifyEffortCmd( Part *part, Node &node, Duration oldvalue, Duration newvalue, const QString& name = QString() );
    ~ModifyEffortCmd();
    void execute();
    void unexecute();

private:
    Effort *m_effort;
    Duration m_oldvalue, m_newvalue;
    KMacroCommand *m_cmd;

};

class EffortModifyOptimisticRatioCmd : public NamedCommand
{
public:
    EffortModifyOptimisticRatioCmd( Part *part, Node &node, int oldvalue, int newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Effort *m_effort;
    int m_oldvalue, m_newvalue;

};

class EffortModifyPessimisticRatioCmd : public NamedCommand
{
public:
    EffortModifyPessimisticRatioCmd( Part *part, Node &node, int oldvalue, int newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Effort *m_effort;
    int m_oldvalue, m_newvalue;

};

class ModifyEffortTypeCmd : public NamedCommand
{
public:
    ModifyEffortTypeCmd( Part *part, Node &node, int oldvalue, int newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Effort *m_effort;
    int m_oldvalue, m_newvalue;

};

class ModifyEffortUnitCmd : public NamedCommand
{
public:
    ModifyEffortUnitCmd( Part *part, Node &node, Duration::Unit oldvalue, Duration::Unit newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Effort *m_effort;
    Duration::Unit m_oldvalue, m_newvalue;
};

class EffortModifyRiskCmd : public NamedCommand
{
public:
    EffortModifyRiskCmd( Part *part, Node &node, int oldvalue, int newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Effort *m_effort;
    int m_oldvalue, m_newvalue;

};

class AddResourceGroupRequestCmd : public NamedCommand
{
public:
    AddResourceGroupRequestCmd( Part *part, Task &task, ResourceGroupRequest *request, const QString& name = QString() );
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
    RemoveResourceGroupRequestCmd( Part *part, ResourceGroupRequest *request, const QString& name = QString() );
    RemoveResourceGroupRequestCmd( Part *part, Task &task, ResourceGroupRequest *request, const QString& name = QString() );
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
    AddResourceCmd( Part *part, ResourceGroup *group, Resource *resource, const QString& name = QString() );
    ~AddResourceCmd();
    void execute();
    void unexecute();

protected:

    ResourceGroup *m_group;
    Resource *m_resource;
    int m_index;
    bool m_mine;
};

class RemoveResourceCmd : public AddResourceCmd
{
public:
    RemoveResourceCmd( Part *part, ResourceGroup *group, Resource *resource, const QString& name = QString() );
    ~RemoveResourceCmd();
    void execute();
    void unexecute();

private:
    QList<ResourceRequest*> m_requests;
    QList<Appointment*> m_appointments;
};

class ModifyResourceNameCmd : public NamedCommand
{
public:
    ModifyResourceNameCmd( Part *part, Resource *resource, const QString& value, const QString& name = QString() );
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
    ModifyResourceInitialsCmd( Part *part, Resource *resource, const QString& value, const QString& name = QString() );
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
    ModifyResourceEmailCmd( Part *part, Resource *resource, const QString& value, const QString& name = QString() );
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
    ModifyResourceTypeCmd( Part *part, Resource *resource, int value, const QString& name = QString() );
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
    ModifyResourceUnitsCmd( Part *part, Resource *resource, int value, const QString& name = QString() );
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
    ModifyResourceAvailableFromCmd( Part *part, Resource *resource, const QDateTime& value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    QDateTime m_newvalue;
    DateTime m_oldvalue;
    KDateTime::Spec m_spec;
};
class ModifyResourceAvailableUntilCmd : public NamedCommand
{
public:
    ModifyResourceAvailableUntilCmd( Part *part, Resource *resource, const QDateTime& value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    QDateTime m_newvalue;
    DateTime m_oldvalue;
    KDateTime::Spec m_spec;
};

class ModifyResourceNormalRateCmd : public NamedCommand
{
public:
    ModifyResourceNormalRateCmd( Part *part, Resource *resource, double value, const QString& name = QString() );
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
    ModifyResourceOvertimeRateCmd( Part *part, Resource *resource, double value, const QString& name = QString() );
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
    ModifyResourceCalendarCmd( Part *part, Resource *resource, Calendar *value, const QString& name = QString() );
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
    RemoveResourceGroupCmd( Part *part, Project *project, ResourceGroup *group, const QString& name = QString() );
    ~RemoveResourceGroupCmd();
    void execute();
    void unexecute();

protected:

    ResourceGroup *m_group;
    Project *m_project;
    int m_index;
    bool m_mine;
    KMacroCommand *m_cmd;
};

class AddResourceGroupCmd : public RemoveResourceGroupCmd
{
public:
    AddResourceGroupCmd( Part *part, Project *project, ResourceGroup *group, const QString& name = QString() );
    void execute();
    void unexecute();
};

class ModifyResourceGroupNameCmd : public NamedCommand
{
public:
    ModifyResourceGroupNameCmd( Part *part, ResourceGroup *group, const QString& value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    ResourceGroup *m_group;
    QString m_newvalue;
    QString m_oldvalue;
};

class ModifyResourceGroupTypeCmd : public NamedCommand
{
    public:
        ModifyResourceGroupTypeCmd( Part *part, ResourceGroup *group, int value, const QString& name = QString() );
        void execute();
        void unexecute();

    private:
        ResourceGroup *m_group;
        int m_newvalue;
        int m_oldvalue;
};

class ModifyCompletionStartedCmd : public NamedCommand
{
public:
    ModifyCompletionStartedCmd( Part *part, Completion &completion, bool value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Completion &m_completion;
    bool oldvalue;
    bool newvalue;
};

class ModifyCompletionFinishedCmd : public NamedCommand
{
public:
    ModifyCompletionFinishedCmd( Part *part, Completion &completion, bool value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Completion &m_completion;
    bool oldvalue;
    bool newvalue;
};

class ModifyCompletionStartTimeCmd : public NamedCommand
{
public:
    ModifyCompletionStartTimeCmd( Part *part, Completion &completion, QDateTime value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Completion &m_completion;
    DateTime oldvalue;
    QDateTime newvalue;
    KDateTime::Spec m_spec;
};

class ModifyCompletionFinishTimeCmd : public NamedCommand
{
public:
    ModifyCompletionFinishTimeCmd( Part *part, Completion &completion, QDateTime value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Completion &m_completion;
    DateTime oldvalue;
    QDateTime newvalue;
    KDateTime::Spec m_spec;
};

class AddCompletionEntryCmd : public NamedCommand
{
public:
    AddCompletionEntryCmd( Part *part, Completion &completion, const QDate &date, Completion::Entry *value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Completion &m_completion;
    QDate m_date;
    Completion::Entry *oldvalue;
    Completion::Entry *newvalue;
};


class AddAccountCmd : public NamedCommand
{
public:
    AddAccountCmd( Part *part, Project &project, Account *account, Account *parent = 0, const QString& name = QString() );
    AddAccountCmd( Part *part, Project &project, Account *account, const QString& parent, const QString& name = QString() );
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
    RemoveAccountCmd( Part *part, Project &project, Account *account, const QString& name = QString() );
    ~RemoveAccountCmd();
    void execute();
    void unexecute();

private:
    Project &m_project;
    Account *m_account;
    Account *m_parent;
    int m_index;
    bool m_isDefault;
    bool m_mine;

};

class RenameAccountCmd : public NamedCommand
{
public:
    RenameAccountCmd( Part *part, Account *account, const QString& value, const QString& name = QString() );
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
    ModifyAccountDescriptionCmd( Part *part, Account *account, const QString& value, const QString& name = QString() );
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
    NodeModifyStartupCostCmd( Part *part, Node &node, double value, const QString& name = QString() );
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
    NodeModifyShutdownCostCmd( Part *part, Node &node, double value, const QString& name = QString() );
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
    NodeModifyRunningAccountCmd( Part *part, Node &node, Account *oldvalue, Account *newvalue, const QString& name = QString() );
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
    NodeModifyStartupAccountCmd( Part *part, Node &node, Account *oldvalue, Account *newvalue, const QString& name = QString() );
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
    NodeModifyShutdownAccountCmd( Part *part, Node &node, Account *oldvalue, Account *newvalue, const QString& name = QString() );
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
    ModifyDefaultAccountCmd( Part *part, Accounts &acc, Account *oldvalue, Account *newvalue, const QString& name = QString() );
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
    ProjectModifyConstraintCmd( Part *part, Project &node, Node::ConstraintType c, const QString& name = QString() );
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
    ProjectModifyStartTimeCmd( Part *part, Project &node, const QDateTime& dt, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Project &m_node;
    QDateTime newTime;
    DateTime oldTime;
    KDateTime::Spec m_spec;
};

class ProjectModifyEndTimeCmd : public NamedCommand
{
public:
    ProjectModifyEndTimeCmd( Part *part, Project &project, const QDateTime& dt, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Project &m_node;
    QDateTime newTime;
    DateTime oldTime;
    KDateTime::Spec m_spec;
};


class AddScheduleManagerCmd : public NamedCommand
{
public:
    AddScheduleManagerCmd( Part *part, Project &project, ScheduleManager *sm, const QString& name = 0 );
    ~AddScheduleManagerCmd();
    void execute();
    void unexecute();

protected:
    Project &m_node;
    ScheduleManager *m_sm;
    MainSchedule *m_exp, *m_opt, *m_pess;
    bool m_mine;
};

class DeleteScheduleManagerCmd : public AddScheduleManagerCmd
{
public:
    DeleteScheduleManagerCmd( Part *part, Project &project, ScheduleManager *sm, const QString& name = 0 );
    void execute();
    void unexecute();
};

class ModifyScheduleManagerNameCmd : public NamedCommand
{
public:
    ModifyScheduleManagerNameCmd( Part *part,ScheduleManager &sm, const QString& value, const QString& name = 0 );
    void execute();
    void unexecute();

private:
    ScheduleManager &m_sm;
    QString oldvalue, newvalue;
};

class ModifyScheduleManagerAllowOverbookingCmd : public NamedCommand
{
public:
    ModifyScheduleManagerAllowOverbookingCmd( Part *part,ScheduleManager &sm, bool value, const QString& name = 0 );
    void execute();
    void unexecute();

private:
    ScheduleManager &m_sm;
    bool oldvalue, newvalue;
};

class ModifyScheduleManagerDistributionCmd : public NamedCommand
{
public:
    ModifyScheduleManagerDistributionCmd( Part *part,ScheduleManager &sm, bool value, const QString& name = 0 );
    void execute();
    void unexecute();

private:
    ScheduleManager &m_sm;
    bool oldvalue, newvalue;
};

class ModifyScheduleManagerCalculateAllCmd : public NamedCommand
{
public:
    ModifyScheduleManagerCalculateAllCmd( Part *part,ScheduleManager &sm, bool value, const QString& name = 0 );
    void execute();
    void unexecute();

private:
    ScheduleManager &m_sm;
    bool oldvalue, newvalue;
};

class CalculateScheduleCmd : public NamedCommand
{
public:
    CalculateScheduleCmd( Part *part, Project &project, ScheduleManager &sm, const QString& name = 0 );
    void execute();
    void unexecute();

private:
    Project &m_node;
    ScheduleManager &m_sm;
    bool m_first;
    MainSchedule *m_oldexpected;
    MainSchedule *m_oldoptimistic;
    MainSchedule *m_oldpessimistic;
    MainSchedule *m_newexpected;
    MainSchedule *m_newoptimistic;
    MainSchedule *m_newpessimistic;
};

class ModifyStandardWorktimeYearCmd : public NamedCommand
{
public:
    ModifyStandardWorktimeYearCmd( Part *part, StandardWorktime *wt, double oldvalue, double newvalue, const QString& name = QString() );
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
    ModifyStandardWorktimeMonthCmd( Part *part, StandardWorktime *wt, double oldvalue, double newvalue, const QString& name = QString() );
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
    ModifyStandardWorktimeWeekCmd( Part *part, StandardWorktime *wt, double oldvalue, double newvalue, const QString& name = QString() );
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
    ModifyStandardWorktimeDayCmd( Part *part, StandardWorktime *wt, double oldvalue, double newvalue, const QString& name = QString() );
    void execute();
    void unexecute();
private:
    StandardWorktime *swt;
    double m_oldvalue;
    double m_newvalue;
};


}  //KPlato namespace

#endif //COMMAND_H
