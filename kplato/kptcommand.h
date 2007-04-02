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

#include <k3command.h>

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

class NamedCommand : public K3NamedCommand
{
public:
    NamedCommand( Part *part, const QString& name )
            : K3NamedCommand( name ), m_part( part )
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

class KPLATO_TEST_EXPORT CalendarAddCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT CalendarRemoveCmd : public NamedCommand
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
    K3MacroCommand *m_cmd;
};

class KPLATO_TEST_EXPORT CalendarModifyNameCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT CalendarModifyParentCmd : public NamedCommand
{
public:
    CalendarModifyParentCmd( Part *part, Project *project, Calendar *cal, Calendar *newvalue, const QString& name = QString() );
    ~CalendarModifyParentCmd();
    void execute();
    void unexecute();

private:
    Project *m_project;
    Calendar *m_cal;
    Calendar *m_newvalue;
    Calendar *m_oldvalue;
    K3MacroCommand *m_cmd;
};

class KPLATO_TEST_EXPORT CalendarModifyTimeZoneCmd : public NamedCommand
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
    K3MacroCommand *m_cmd;
};

class KPLATO_TEST_EXPORT CalendarAddDayCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT CalendarRemoveDayCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT CalendarModifyDayCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT CalendarModifyStateCmd : public NamedCommand
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
    K3MacroCommand *m_cmd;
};

class KPLATO_TEST_EXPORT CalendarModifyTimeIntervalCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT CalendarAddTimeIntervalCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT CalendarRemoveTimeIntervalCmd : public CalendarAddTimeIntervalCmd
{
public:
    CalendarRemoveTimeIntervalCmd( Part *part, Calendar *calendar, CalendarDay *day, TimeInterval *value, const QString& name = QString() );
    
    void execute();
    void unexecute();
};

class KPLATO_TEST_EXPORT CalendarModifyWeekdayCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT CalendarModifyDateCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ProjectModifyDefaultCalendarCmd : public NamedCommand
{
public:
    ProjectModifyDefaultCalendarCmd( Part *part, Project *project, Calendar *cal, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Project *m_project;
    Calendar *m_newvalue, *m_oldvalue;
};


class KPLATO_TEST_EXPORT NodeDeleteCmd : public NamedCommand
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
    K3MacroCommand *m_cmd;
};

class KPLATO_TEST_EXPORT TaskAddCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT SubtaskAddCmd : public NamedCommand
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
    K3MacroCommand *m_cmd;
};


class KPLATO_TEST_EXPORT NodeModifyNameCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT NodeModifyLeaderCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT NodeModifyDescriptionCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT NodeModifyConstraintCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT NodeModifyConstraintStartTimeCmd : public NamedCommand
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
class KPLATO_TEST_EXPORT NodeModifyConstraintEndTimeCmd : public NamedCommand
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
class KPLATO_TEST_EXPORT NodeModifyStartTimeCmd : public NamedCommand
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
class KPLATO_TEST_EXPORT NodeModifyEndTimeCmd : public NamedCommand
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
class KPLATO_TEST_EXPORT NodeModifyIdCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT NodeIndentCmd : public NamedCommand
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
    K3MacroCommand *m_cmd;
};

class KPLATO_TEST_EXPORT NodeUnindentCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT NodeMoveUpCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT NodeMoveDownCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT NodeMoveCmd : public NamedCommand
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
    K3MacroCommand *m_cmd;
};

class KPLATO_TEST_EXPORT AddRelationCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT DeleteRelationCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ModifyRelationTypeCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ModifyRelationLagCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT AddResourceRequestCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT RemoveResourceRequestCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ModifyEffortCmd : public NamedCommand
{
public:
    ModifyEffortCmd( Part *part, Node &node, Duration oldvalue, Duration newvalue, const QString& name = QString() );
    ~ModifyEffortCmd();
    void execute();
    void unexecute();

private:
    Effort *m_effort;
    Duration m_oldvalue, m_newvalue;
    K3MacroCommand *m_cmd;

};

class KPLATO_TEST_EXPORT EffortModifyOptimisticRatioCmd : public NamedCommand
{
public:
    EffortModifyOptimisticRatioCmd( Part *part, Node &node, int oldvalue, int newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Effort *m_effort;
    int m_oldvalue, m_newvalue;

};

class KPLATO_TEST_EXPORT EffortModifyPessimisticRatioCmd : public NamedCommand
{
public:
    EffortModifyPessimisticRatioCmd( Part *part, Node &node, int oldvalue, int newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Effort *m_effort;
    int m_oldvalue, m_newvalue;

};

class KPLATO_TEST_EXPORT ModifyEffortTypeCmd : public NamedCommand
{
public:
    ModifyEffortTypeCmd( Part *part, Node &node, int oldvalue, int newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Effort *m_effort;
    int m_oldvalue, m_newvalue;

};

class KPLATO_TEST_EXPORT ModifyEffortUnitCmd : public NamedCommand
{
public:
    ModifyEffortUnitCmd( Part *part, Node &node, Duration::Unit oldvalue, Duration::Unit newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Effort *m_effort;
    Duration::Unit m_oldvalue, m_newvalue;
};

class KPLATO_TEST_EXPORT EffortModifyRiskCmd : public NamedCommand
{
public:
    EffortModifyRiskCmd( Part *part, Node &node, int oldvalue, int newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Effort *m_effort;
    int m_oldvalue, m_newvalue;

};

class KPLATO_TEST_EXPORT AddResourceGroupRequestCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT RemoveResourceGroupRequestCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT AddResourceCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT RemoveResourceCmd : public AddResourceCmd
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

class KPLATO_TEST_EXPORT ModifyResourceNameCmd : public NamedCommand
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
class KPLATO_TEST_EXPORT ModifyResourceInitialsCmd : public NamedCommand
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
class KPLATO_TEST_EXPORT ModifyResourceEmailCmd : public NamedCommand
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
class KPLATO_TEST_EXPORT ModifyResourceTypeCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ModifyResourceUnitsCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ModifyResourceAvailableFromCmd : public NamedCommand
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
class KPLATO_TEST_EXPORT ModifyResourceAvailableUntilCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ModifyResourceNormalRateCmd : public NamedCommand
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
class KPLATO_TEST_EXPORT ModifyResourceOvertimeRateCmd : public NamedCommand
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
class KPLATO_TEST_EXPORT ModifyResourceCalendarCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT RemoveResourceGroupCmd : public NamedCommand
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
    K3MacroCommand *m_cmd;
};

class KPLATO_TEST_EXPORT AddResourceGroupCmd : public RemoveResourceGroupCmd
{
public:
    AddResourceGroupCmd( Part *part, Project *project, ResourceGroup *group, const QString& name = QString() );
    void execute();
    void unexecute();
};

class KPLATO_TEST_EXPORT ModifyResourceGroupNameCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ModifyResourceGroupTypeCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ModifyCompletionStartedCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ModifyCompletionFinishedCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ModifyCompletionStartTimeCmd : public NamedCommand
{
public:
    ModifyCompletionStartTimeCmd( Part *part, Completion &completion, const QDateTime &value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Completion &m_completion;
    DateTime oldvalue;
    QDateTime newvalue;
    KDateTime::Spec m_spec;
};

class KPLATO_TEST_EXPORT ModifyCompletionFinishTimeCmd : public NamedCommand
{
public:
    ModifyCompletionFinishTimeCmd( Part *part, Completion &completion, const QDateTime &value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Completion &m_completion;
    DateTime oldvalue;
    QDateTime newvalue;
    KDateTime::Spec m_spec;
};

class KPLATO_TEST_EXPORT AddCompletionEntryCmd : public NamedCommand
{
public:
    AddCompletionEntryCmd( Part *part, Completion &completion, const QDate &date, Completion::Entry *value, const QString& name = QString() );
    ~AddCompletionEntryCmd();
    void execute();
    void unexecute();

private:
    Completion &m_completion;
    QDate m_date;
    Completion::Entry *oldvalue;
    Completion::Entry *newvalue;
    bool m_newmine, m_oldmine;
};

class KPLATO_TEST_EXPORT AddCompletionUsedEffortCmd : public NamedCommand
{
public:
    AddCompletionUsedEffortCmd( Part *part, Completion &completion, const Resource *resource, Completion::UsedEffort *value, const QString& name = QString() );
    ~AddCompletionUsedEffortCmd();
    void execute();
    void unexecute();

private:
    Completion &m_completion;
    const Resource *m_resource;
    Completion::UsedEffort *oldvalue;
    Completion::UsedEffort *newvalue;
    bool m_newmine, m_oldmine;
};

class KPLATO_TEST_EXPORT AddCompletionActualEffortCmd : public NamedCommand
{
public:
    AddCompletionActualEffortCmd( Part *part, Completion::UsedEffort &ue, const QDate &date, Completion::UsedEffort::ActualEffort *value, const QString& name = QString() );
    ~AddCompletionActualEffortCmd();
    void execute();
    void unexecute();

private:
    Completion::UsedEffort &m_usedEffort;
    QDate m_date;
    Completion::UsedEffort::ActualEffort *oldvalue;
    Completion::UsedEffort::ActualEffort *newvalue;
    bool m_newmine, m_oldmine;
};


class KPLATO_TEST_EXPORT AddAccountCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT RemoveAccountCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT RenameAccountCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ModifyAccountDescriptionCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT NodeModifyStartupCostCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT NodeModifyShutdownCostCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT NodeModifyRunningAccountCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT NodeModifyStartupAccountCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT NodeModifyShutdownAccountCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ModifyDefaultAccountCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ProjectModifyConstraintCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ProjectModifyStartTimeCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ProjectModifyEndTimeCmd : public NamedCommand
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


class KPLATO_TEST_EXPORT AddScheduleManagerCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT DeleteScheduleManagerCmd : public AddScheduleManagerCmd
{
public:
    DeleteScheduleManagerCmd( Part *part, Project &project, ScheduleManager *sm, const QString& name = 0 );
    void execute();
    void unexecute();
};

class KPLATO_TEST_EXPORT ModifyScheduleManagerNameCmd : public NamedCommand
{
public:
    ModifyScheduleManagerNameCmd( Part *part,ScheduleManager &sm, const QString& value, const QString& name = 0 );
    void execute();
    void unexecute();

private:
    ScheduleManager &m_sm;
    QString oldvalue, newvalue;
};

class KPLATO_TEST_EXPORT ModifyScheduleManagerAllowOverbookingCmd : public NamedCommand
{
public:
    ModifyScheduleManagerAllowOverbookingCmd( Part *part,ScheduleManager &sm, bool value, const QString& name = 0 );
    void execute();
    void unexecute();

private:
    ScheduleManager &m_sm;
    bool oldvalue, newvalue;
};

class KPLATO_TEST_EXPORT ModifyScheduleManagerDistributionCmd : public NamedCommand
{
public:
    ModifyScheduleManagerDistributionCmd( Part *part,ScheduleManager &sm, bool value, const QString& name = 0 );
    void execute();
    void unexecute();

private:
    ScheduleManager &m_sm;
    bool oldvalue, newvalue;
};

class KPLATO_TEST_EXPORT ModifyScheduleManagerCalculateAllCmd : public NamedCommand
{
public:
    ModifyScheduleManagerCalculateAllCmd( Part *part,ScheduleManager &sm, bool value, const QString& name = 0 );
    void execute();
    void unexecute();

private:
    ScheduleManager &m_sm;
    bool oldvalue, newvalue;
};

class KPLATO_TEST_EXPORT CalculateScheduleCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ModifyStandardWorktimeYearCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ModifyStandardWorktimeMonthCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ModifyStandardWorktimeWeekCmd : public NamedCommand
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

class KPLATO_TEST_EXPORT ModifyStandardWorktimeDayCmd : public NamedCommand
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
