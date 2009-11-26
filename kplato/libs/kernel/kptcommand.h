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

#include "kplatokernel_export.h"

#include <QUndoCommand>

#include "kptappointment.h"
#include "kptnode.h"
#include "kptduration.h"
#include "kpttask.h"
#include "kptwbsdefinition.h"

class QString;
/**
 * @file
 * This file includes undo/redo commands for kernel data structures
 */

/// The main namespace
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

class KPLATOKERNEL_EXPORT NamedCommand : public QUndoCommand
{
public:
    NamedCommand( const QString& name )
        : QUndoCommand( name )
    {}
    virtual void redo() { execute(); }
    virtual void undo() { unexecute(); }

    virtual void execute() = 0;
    virtual void unexecute() = 0;

protected:
    void setSchDeleted();
    void setSchDeleted( bool state );
    void setSchScheduled();
    void setSchScheduled( bool state );
    void addSchScheduled( Schedule *sch );
    void addSchDeleted( Schedule *sch );

    QMap<Schedule*, bool> m_schedules;

};

class KPLATOKERNEL_EXPORT MacroCommand : public QUndoCommand
{
public:
    MacroCommand( const QString& name = QString() )
        : QUndoCommand( name )
    {}
    ~MacroCommand();
    
    void addCommand( QUndoCommand *cmd );
    
    virtual void redo() { execute(); }
    virtual void undo() { unexecute(); }

    virtual void execute();
    virtual void unexecute();

    bool isEmpty() const { return cmds.isEmpty(); }
    
protected:
    QList<QUndoCommand*> cmds;
};


class KPLATOKERNEL_EXPORT CalendarAddCmd : public NamedCommand
{
public:
    CalendarAddCmd( Project *project, Calendar *cal, Calendar *parent, const QString& name = QString() );
    ~CalendarAddCmd();
    void execute();
    void unexecute();

private:
    Project *m_project;
    Calendar *m_cal;
    Calendar *m_parent;
    bool m_mine;
};

class KPLATOKERNEL_EXPORT CalendarRemoveCmd : public NamedCommand
{
public:
    CalendarRemoveCmd( Project *project, Calendar *cal, const QString& name = QString() );
    ~CalendarRemoveCmd();
    void execute();
    void unexecute();

private:
    Project *m_project;
    Calendar *m_parent;
    Calendar *m_cal;
    bool m_mine;
    MacroCommand *m_cmd;
};

class KPLATOKERNEL_EXPORT CalendarModifyNameCmd : public NamedCommand
{
public:
    CalendarModifyNameCmd( Calendar *cal, const QString& newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Calendar *m_cal;
    QString m_newvalue;
    QString m_oldvalue;
};

class KPLATOKERNEL_EXPORT CalendarModifyParentCmd : public NamedCommand
{
public:
    CalendarModifyParentCmd( Project *project, Calendar *cal, Calendar *newvalue, const QString& name = QString() );
    ~CalendarModifyParentCmd();
    void execute();
    void unexecute();

private:
    Project *m_project;
    Calendar *m_cal;
    Calendar *m_newvalue;
    Calendar *m_oldvalue;
    MacroCommand *m_cmd;
};

class KPLATOKERNEL_EXPORT CalendarModifyTimeZoneCmd : public NamedCommand
{
public:
    CalendarModifyTimeZoneCmd( Calendar *cal, const KTimeZone &value, const QString& name = QString() );
    ~CalendarModifyTimeZoneCmd();
    void execute();
    void unexecute();

private:
    Calendar *m_cal;
    KTimeZone m_newvalue;
    KTimeZone m_oldvalue;
    MacroCommand *m_cmd;
};

class KPLATOKERNEL_EXPORT CalendarAddDayCmd : public NamedCommand
{
public:
    CalendarAddDayCmd( Calendar *cal, CalendarDay *newvalue, const QString& name = QString() );
    ~CalendarAddDayCmd();
    void execute();
    void unexecute();

protected:
    Calendar *m_cal;
    CalendarDay *m_newvalue;
    bool m_mine;
};

class KPLATOKERNEL_EXPORT CalendarRemoveDayCmd : public NamedCommand
{
public:
    CalendarRemoveDayCmd( Calendar *cal, CalendarDay *day, const QString& name = QString() );
    CalendarRemoveDayCmd( Calendar *cal, const QDate &day, const QString& name = QString() );
    void execute();
    void unexecute();

protected:
    Calendar *m_cal;
    CalendarDay *m_value;
    bool m_mine;

private:
    void init();
};

class KPLATOKERNEL_EXPORT CalendarModifyDayCmd : public NamedCommand
{
public:
    CalendarModifyDayCmd( Calendar *cal, CalendarDay *value, const QString& name = QString() );
    ~CalendarModifyDayCmd();
    void execute();
    void unexecute();

private:
    Calendar *m_cal;
    CalendarDay *m_newvalue;
    CalendarDay *m_oldvalue;
    bool m_mine;
};

class KPLATOKERNEL_EXPORT CalendarModifyStateCmd : public NamedCommand
{
public:
    CalendarModifyStateCmd( Calendar *calendar, CalendarDay *day, CalendarDay::State value, const QString& name = QString() );
    ~CalendarModifyStateCmd();
    void execute();
    void unexecute();

private:
    Calendar *m_calendar;
    CalendarDay *m_day;
    CalendarDay::State m_newvalue;
    CalendarDay::State m_oldvalue;
    MacroCommand *m_cmd;
};

class KPLATOKERNEL_EXPORT CalendarModifyTimeIntervalCmd : public NamedCommand
{
public:
    CalendarModifyTimeIntervalCmd( Calendar *calendar, TimeInterval &newvalue, TimeInterval *value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Calendar *m_calendar;
    TimeInterval *m_value;
    TimeInterval m_newvalue;
    TimeInterval m_oldvalue;
};

class KPLATOKERNEL_EXPORT CalendarAddTimeIntervalCmd : public NamedCommand
{
public:
    CalendarAddTimeIntervalCmd( Calendar *calendar, CalendarDay *day, TimeInterval *value, const QString& name = QString() );
    ~CalendarAddTimeIntervalCmd();
    void execute();
    void unexecute();

protected:
    Calendar *m_calendar;
    CalendarDay *m_day;
    TimeInterval *m_value;
    bool m_mine;
};

class KPLATOKERNEL_EXPORT CalendarRemoveTimeIntervalCmd : public CalendarAddTimeIntervalCmd
{
public:
    CalendarRemoveTimeIntervalCmd( Calendar *calendar, CalendarDay *day, TimeInterval *value, const QString& name = QString() );
    
    void execute();
    void unexecute();
};

class KPLATOKERNEL_EXPORT CalendarModifyWeekdayCmd : public NamedCommand
{
public:
    CalendarModifyWeekdayCmd( Calendar *cal, int weekday, CalendarDay *value, const QString& name = QString() );
    ~CalendarModifyWeekdayCmd();
    void execute();
    void unexecute();

private:
    int m_weekday;
    Calendar *m_cal;
    CalendarDay *m_value;
    CalendarDay m_orig;
};

class KPLATOKERNEL_EXPORT CalendarModifyDateCmd : public NamedCommand
{
public:
    CalendarModifyDateCmd( Calendar *cal, CalendarDay *day, QDate &value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Calendar *m_cal;
    CalendarDay *m_day;
    QDate m_newvalue, m_oldvalue;
};

class KPLATOKERNEL_EXPORT ProjectModifyDefaultCalendarCmd : public NamedCommand
{
public:
    ProjectModifyDefaultCalendarCmd( Project *project, Calendar *cal, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Project *m_project;
    Calendar *m_newvalue, *m_oldvalue;
};


class KPLATOKERNEL_EXPORT NodeDeleteCmd : public NamedCommand
{
public:
    explicit NodeDeleteCmd( Node *node, const QString& name = QString() );
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
    MacroCommand *m_cmd;
};

class KPLATOKERNEL_EXPORT TaskAddCmd : public NamedCommand
{
public:
    TaskAddCmd( Project *project, Node *node, Node *after, const QString& name = QString() );
    ~TaskAddCmd();
    void execute();
    void unexecute();

private:
    Project *m_project;
    Node *m_node;
    Node *m_after;
    bool m_added;
};

class KPLATOKERNEL_EXPORT SubtaskAddCmd : public NamedCommand
{
public:
    SubtaskAddCmd( Project *project, Node *node, Node *parent, const QString& name = QString() );
    ~SubtaskAddCmd();
    void execute();
    void unexecute();

private:
    Project *m_project;
    Node *m_node;
    Node *m_parent;
    bool m_added;
    MacroCommand *m_cmd;
};


class KPLATOKERNEL_EXPORT NodeModifyNameCmd : public NamedCommand
{
public:
    NodeModifyNameCmd( Node &node, const QString& nodename, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    QString newName;
    QString oldName;
};

class KPLATOKERNEL_EXPORT NodeModifyLeaderCmd : public NamedCommand
{
public:
    NodeModifyLeaderCmd( Node &node, const QString& leader, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    QString newLeader;
    QString oldLeader;
};

class KPLATOKERNEL_EXPORT NodeModifyDescriptionCmd : public NamedCommand
{
public:
    NodeModifyDescriptionCmd( Node &node, const QString& description, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    QString newDescription;
    QString oldDescription;
};

class KPLATOKERNEL_EXPORT NodeModifyConstraintCmd : public NamedCommand
{
public:
    NodeModifyConstraintCmd( Node &node, Node::ConstraintType c, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    Node::ConstraintType newConstraint;
    Node::ConstraintType oldConstraint;

};

class KPLATOKERNEL_EXPORT NodeModifyConstraintStartTimeCmd : public NamedCommand
{
public:
    NodeModifyConstraintStartTimeCmd( Node &node, const QDateTime& dt, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    QDateTime newTime;
    DateTime oldTime;
    KDateTime::Spec m_spec;
};
class KPLATOKERNEL_EXPORT NodeModifyConstraintEndTimeCmd : public NamedCommand
{
public:
    NodeModifyConstraintEndTimeCmd( Node &node, const QDateTime& dt, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    QDateTime newTime;
    DateTime oldTime;
    KDateTime::Spec m_spec;
};
class KPLATOKERNEL_EXPORT NodeModifyStartTimeCmd : public NamedCommand
{
public:
    NodeModifyStartTimeCmd( Node &node, const QDateTime& dt, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    QDateTime newTime;
    DateTime oldTime;
    KDateTime::Spec m_spec;
};
class KPLATOKERNEL_EXPORT NodeModifyEndTimeCmd : public NamedCommand
{
public:
    NodeModifyEndTimeCmd( Node &node, const QDateTime& dt, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    QDateTime newTime;
    DateTime oldTime;
    KDateTime::Spec m_spec;
};
class KPLATOKERNEL_EXPORT NodeModifyIdCmd : public NamedCommand
{
public:
    NodeModifyIdCmd( Node &node, const QString& id, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    QString newId;
    QString oldId;
};

class KPLATOKERNEL_EXPORT NodeIndentCmd : public NamedCommand
{
public:
    explicit NodeIndentCmd( Node &node, const QString& name = QString() );
    ~NodeIndentCmd();
    void execute();
    void unexecute();

private:
    Node &m_node;
    Node *m_oldparent, *m_newparent;
    int m_oldindex, m_newindex;
    MacroCommand *m_cmd;
};

class KPLATOKERNEL_EXPORT NodeUnindentCmd : public NamedCommand
{
public:
    explicit NodeUnindentCmd( Node &node, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    Node *m_oldparent, *m_newparent;
    int m_oldindex, m_newindex;
};

class KPLATOKERNEL_EXPORT NodeMoveUpCmd : public NamedCommand
{
public:
    explicit NodeMoveUpCmd( Node &node, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    Project *m_project;
    bool m_moved;
};

class KPLATOKERNEL_EXPORT NodeMoveDownCmd : public NamedCommand
{
public:
    explicit NodeMoveDownCmd( Node &node, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    Project *m_project;
    bool m_moved;
};

class KPLATOKERNEL_EXPORT NodeMoveCmd : public NamedCommand
{
public:
    NodeMoveCmd( Project *project, Node *node, Node *newParent, int newPos, const QString& name = QString() );
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
    MacroCommand *m_cmd;
};

class KPLATOKERNEL_EXPORT AddRelationCmd : public NamedCommand
{
public:
    AddRelationCmd( Project &project, Relation *rel, const QString& name = QString() );
    ~AddRelationCmd();
    void execute();
    void unexecute();

private:
    Relation *m_rel;
    Project &m_project;
    bool m_taken;

};

class KPLATOKERNEL_EXPORT DeleteRelationCmd : public NamedCommand
{
public:
    DeleteRelationCmd( Project &project, Relation *rel, const QString& name = QString() );
    ~DeleteRelationCmd();
    void execute();
    void unexecute();

private:
    Relation *m_rel;
    Project &m_project;
    bool m_taken;

};

class KPLATOKERNEL_EXPORT ModifyRelationTypeCmd : public NamedCommand
{
public:
    ModifyRelationTypeCmd( Relation *rel, Relation::Type type, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Project *m_project;
    Relation *m_rel;
    Relation::Type m_newtype;
    Relation::Type m_oldtype;

};

class KPLATOKERNEL_EXPORT ModifyRelationLagCmd : public NamedCommand
{
public:
    ModifyRelationLagCmd( Relation *rel, Duration lag, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Project *m_project;
    Relation *m_rel;
    Duration m_newlag;
    Duration m_oldlag;

};

class KPLATOKERNEL_EXPORT AddResourceRequestCmd : public NamedCommand
{
public:
    AddResourceRequestCmd( ResourceGroupRequest *group, ResourceRequest *request, const QString& name = QString() );
    ~AddResourceRequestCmd();
    void execute();
    void unexecute();

private:
    ResourceGroupRequest *m_group;
    ResourceRequest *m_request;
    bool m_mine;

};

class KPLATOKERNEL_EXPORT RemoveResourceRequestCmd : public NamedCommand
{
public:
    RemoveResourceRequestCmd( ResourceGroupRequest *group, ResourceRequest *request, const QString& name = QString() );
    ~RemoveResourceRequestCmd();
    void execute();
    void unexecute();

private:
    ResourceGroupRequest *m_group;
    ResourceRequest *m_request;
    bool m_mine;

};

class KPLATOKERNEL_EXPORT ModifyResourceRequestUnitsCmd : public NamedCommand
{
public:
    ModifyResourceRequestUnitsCmd( ResourceRequest *request, int oldvalue, int newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    ResourceRequest *m_request;
    int m_oldvalue, m_newvalue;

};

class KPLATOKERNEL_EXPORT ModifyResourceRequestRequiredCmd : public NamedCommand
{
public:
    ModifyResourceRequestRequiredCmd( ResourceRequest *request, const QList<Resource*> &value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    ResourceRequest *m_request;
    QList<Resource*> m_oldvalue, m_newvalue;

};

class KPLATOKERNEL_EXPORT ModifyResourceGroupRequestUnitsCmd : public NamedCommand
{
public:
    ModifyResourceGroupRequestUnitsCmd( ResourceGroupRequest *request, int oldvalue, int newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    ResourceGroupRequest *m_request;
    int m_oldvalue, m_newvalue;

};

class KPLATOKERNEL_EXPORT ModifyEstimateCmd : public NamedCommand
{
public:
    ModifyEstimateCmd( Node &node, double oldvalue, double newvalue, const QString& name = QString() );
    ~ModifyEstimateCmd();
    void execute();
    void unexecute();

private:
    Estimate *m_estimate;
    double m_oldvalue, m_newvalue;
    MacroCommand *m_cmd;

};

class KPLATOKERNEL_EXPORT EstimateModifyOptimisticRatioCmd : public NamedCommand
{
public:
    EstimateModifyOptimisticRatioCmd( Node &node, int oldvalue, int newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Estimate *m_estimate;
    int m_oldvalue, m_newvalue;

};

class KPLATOKERNEL_EXPORT EstimateModifyPessimisticRatioCmd : public NamedCommand
{
public:
    EstimateModifyPessimisticRatioCmd( Node &node, int oldvalue, int newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Estimate *m_estimate;
    int m_oldvalue, m_newvalue;

};

class KPLATOKERNEL_EXPORT ModifyEstimateTypeCmd : public NamedCommand
{
public:
    ModifyEstimateTypeCmd( Node &node, int oldvalue, int newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Estimate *m_estimate;
    int m_oldvalue, m_newvalue;

};

class KPLATOKERNEL_EXPORT ModifyEstimateUnitCmd : public NamedCommand
{
public:
    ModifyEstimateUnitCmd( Node &node, Duration::Unit oldvalue, Duration::Unit newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Estimate *m_estimate;
    Duration::Unit m_oldvalue, m_newvalue;
};

class KPLATOKERNEL_EXPORT EstimateModifyRiskCmd : public NamedCommand
{
public:
    EstimateModifyRiskCmd( Node &node, int oldvalue, int newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Estimate *m_estimate;
    int m_oldvalue, m_newvalue;

};

class KPLATOKERNEL_EXPORT ModifyEstimateCalendarCmd : public NamedCommand
{
public:
    ModifyEstimateCalendarCmd( Node &node, Calendar *oldvalue, Calendar *newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Estimate *m_estimate;
    Calendar *m_oldvalue, *m_newvalue;

};


class KPLATOKERNEL_EXPORT AddResourceGroupRequestCmd : public NamedCommand
{
public:
    AddResourceGroupRequestCmd( Task &task, ResourceGroupRequest *request, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Task &m_task;
    ResourceGroupRequest *m_request;
    bool m_mine;
};

class KPLATOKERNEL_EXPORT RemoveResourceGroupRequestCmd : public NamedCommand
{
public:
    explicit RemoveResourceGroupRequestCmd( ResourceGroupRequest *request, const QString& name = QString() );
    RemoveResourceGroupRequestCmd( Task &task, ResourceGroupRequest *request, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Task &m_task;
    ResourceGroupRequest *m_request;
    bool m_mine;
};

class KPLATOKERNEL_EXPORT AddResourceCmd : public NamedCommand
{
public:
    AddResourceCmd( ResourceGroup *group, Resource *resource, const QString& name = QString() );
    ~AddResourceCmd();
    void execute();
    void unexecute();

protected:

    ResourceGroup *m_group;
    Resource *m_resource;
    int m_index;
    bool m_mine;
};

class KPLATOKERNEL_EXPORT RemoveResourceCmd : public AddResourceCmd
{
public:
    RemoveResourceCmd( ResourceGroup *group, Resource *resource, const QString& name = QString() );
    ~RemoveResourceCmd();
    void execute();
    void unexecute();

private:
    QList<ResourceRequest*> m_requests;
    QList<Appointment*> m_appointments;
};

class KPLATOKERNEL_EXPORT MoveResourceCmd : public NamedCommand
{
public:
    MoveResourceCmd( ResourceGroup *group, Resource *resource, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Project &m_project;
    Resource *m_resource;
    ResourceGroup *m_oldvalue, *m_newvalue;
    MacroCommand cmd;
};

class KPLATOKERNEL_EXPORT ModifyResourceNameCmd : public NamedCommand
{
public:
    ModifyResourceNameCmd( Resource *resource, const QString& value, const QString& name = QString() );
    void execute();
    void unexecute();

private:

    Resource *m_resource;
    QString m_newvalue;
    QString m_oldvalue;
};
class KPLATOKERNEL_EXPORT ModifyResourceInitialsCmd : public NamedCommand
{
public:
    ModifyResourceInitialsCmd( Resource *resource, const QString& value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    QString m_newvalue;
    QString m_oldvalue;
};
class KPLATOKERNEL_EXPORT ModifyResourceEmailCmd : public NamedCommand
{
public:
    ModifyResourceEmailCmd( Resource *resource, const QString& value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    QString m_newvalue;
    QString m_oldvalue;
};
class KPLATOKERNEL_EXPORT ModifyResourceTypeCmd : public NamedCommand
{
public:
    ModifyResourceTypeCmd( Resource *resource, int value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    int m_newvalue;
    int m_oldvalue;
};

class KPLATOKERNEL_EXPORT ModifyResourceUnitsCmd : public NamedCommand
{
public:
    ModifyResourceUnitsCmd( Resource *resource, int value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    int m_newvalue;
    int m_oldvalue;
};

class KPLATOKERNEL_EXPORT ModifyResourceAvailableFromCmd : public NamedCommand
{
public:
    ModifyResourceAvailableFromCmd( Resource *resource, const QDateTime& value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    QDateTime m_newvalue;
    DateTime m_oldvalue;
    KDateTime::Spec m_spec;
};
class KPLATOKERNEL_EXPORT ModifyResourceAvailableUntilCmd : public NamedCommand
{
public:
    ModifyResourceAvailableUntilCmd( Resource *resource, const QDateTime& value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    QDateTime m_newvalue;
    DateTime m_oldvalue;
    KDateTime::Spec m_spec;
};

class KPLATOKERNEL_EXPORT ModifyResourceNormalRateCmd : public NamedCommand
{
public:
    ModifyResourceNormalRateCmd( Resource *resource, double value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    double m_newvalue;
    double m_oldvalue;
};
class KPLATOKERNEL_EXPORT ModifyResourceOvertimeRateCmd : public NamedCommand
{
public:
    ModifyResourceOvertimeRateCmd( Resource *resource, double value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    double m_newvalue;
    double m_oldvalue;
};
class KPLATOKERNEL_EXPORT ModifyResourceCalendarCmd : public NamedCommand
{
public:
    ModifyResourceCalendarCmd( Resource *resource, Calendar *value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    Calendar *m_newvalue;
    Calendar *m_oldvalue;
};
class KPLATOKERNEL_EXPORT ModifyRequiredResourcesCmd : public NamedCommand
{
public:
    ModifyRequiredResourcesCmd( Resource *resource, const QList<Resource*> &value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Resource *m_resource;
    QList<Resource*> m_newvalue;
    QList<Resource*> m_oldvalue;
};

class KPLATOKERNEL_EXPORT RemoveResourceGroupCmd : public NamedCommand
{
public:
    RemoveResourceGroupCmd( Project *project, ResourceGroup *group, const QString& name = QString() );
    ~RemoveResourceGroupCmd();
    void execute();
    void unexecute();

protected:

    ResourceGroup *m_group;
    Project *m_project;
    int m_index;
    bool m_mine;
    MacroCommand *m_cmd;
};

class KPLATOKERNEL_EXPORT AddResourceGroupCmd : public RemoveResourceGroupCmd
{
public:
    AddResourceGroupCmd( Project *project, ResourceGroup *group, const QString& name = QString() );
    void execute();
    void unexecute();
};

class KPLATOKERNEL_EXPORT ModifyResourceGroupNameCmd : public NamedCommand
{
public:
    ModifyResourceGroupNameCmd( ResourceGroup *group, const QString& value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    ResourceGroup *m_group;
    QString m_newvalue;
    QString m_oldvalue;
};

class KPLATOKERNEL_EXPORT ModifyResourceGroupTypeCmd : public NamedCommand
{
    public:
        ModifyResourceGroupTypeCmd( ResourceGroup *group, int value, const QString& name = QString() );
        void execute();
        void unexecute();

    private:
        ResourceGroup *m_group;
        int m_newvalue;
        int m_oldvalue;
};

class KPLATOKERNEL_EXPORT ModifyCompletionEntrymodeCmd : public NamedCommand
{
public:
    ModifyCompletionEntrymodeCmd( Completion &completion, Completion::Entrymode value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Completion &m_completion;
    Completion::Entrymode oldvalue;
    Completion::Entrymode newvalue;
};

class KPLATOKERNEL_EXPORT ModifyCompletionStartedCmd : public NamedCommand
{
public:
    ModifyCompletionStartedCmd( Completion &completion, bool value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Completion &m_completion;
    bool oldvalue;
    bool newvalue;
};

class KPLATOKERNEL_EXPORT ModifyCompletionFinishedCmd : public NamedCommand
{
public:
    ModifyCompletionFinishedCmd( Completion &completion, bool value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Completion &m_completion;
    bool oldvalue;
    bool newvalue;
};

class KPLATOKERNEL_EXPORT ModifyCompletionStartTimeCmd : public NamedCommand
{
public:
    ModifyCompletionStartTimeCmd( Completion &completion, const QDateTime &value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Completion &m_completion;
    DateTime oldvalue;
    QDateTime newvalue;
    KDateTime::Spec m_spec;
};

class KPLATOKERNEL_EXPORT ModifyCompletionFinishTimeCmd : public NamedCommand
{
public:
    ModifyCompletionFinishTimeCmd( Completion &completion, const QDateTime &value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Completion &m_completion;
    DateTime oldvalue;
    QDateTime newvalue;
    KDateTime::Spec m_spec;
};

class KPLATOKERNEL_EXPORT AddCompletionEntryCmd : public NamedCommand
{
public:
    AddCompletionEntryCmd( Completion &completion, const QDate &date, Completion::Entry *value, const QString& name = QString() );
    ~AddCompletionEntryCmd();
    void execute();
    void unexecute();

private:
    Completion &m_completion;
    QDate m_date;
    Completion::Entry *newvalue;
    bool m_newmine;
};

class KPLATOKERNEL_EXPORT RemoveCompletionEntryCmd : public NamedCommand
{
public:
    RemoveCompletionEntryCmd( Completion &completion, const QDate& date, const QString& name = QString() );
    ~RemoveCompletionEntryCmd();
    void execute();
    void unexecute();

private:
    Completion &m_completion;
    QDate m_date;
    Completion::Entry *value;
    bool m_mine;
};

class KPLATOKERNEL_EXPORT ModifyCompletionEntryCmd : public NamedCommand
{
public:
    ModifyCompletionEntryCmd( Completion &completion, const QDate &date, Completion::Entry *value, const QString& name = QString() );
    ~ModifyCompletionEntryCmd();
    void execute();
    void unexecute();

private:
    MacroCommand *cmd;
};

class KPLATOKERNEL_EXPORT ModifyCompletionPercentFinishedCmd : public NamedCommand
{
public:
    ModifyCompletionPercentFinishedCmd( Completion &completion, const QDate &date, int value, const QString& name = QString() );
    
    void execute();
    void unexecute();

private:
    Completion &m_completion;
    QDate m_date;
    int m_newvalue, m_oldvalue;
    MacroCommand cmd;
};

class KPLATOKERNEL_EXPORT ModifyCompletionRemainingEffortCmd : public NamedCommand
{
public:
    ModifyCompletionRemainingEffortCmd( Completion &completion, const QDate &date, const Duration &value, const QString &name = QString() );

    void execute();
    void unexecute();

private:
    Completion &m_completion;
    QDate m_date;
    Duration m_newvalue, m_oldvalue;
    MacroCommand cmd;
};

class KPLATOKERNEL_EXPORT ModifyCompletionActualEffortCmd : public NamedCommand
{
public:
    ModifyCompletionActualEffortCmd( Completion &completion, const QDate &date, const Duration &value, const QString &name = QString() );

    void execute();
    void unexecute();

private:
    Completion &m_completion;
    QDate m_date;
    Duration m_newvalue, m_oldvalue;
    MacroCommand cmd;
};

class KPLATOKERNEL_EXPORT AddCompletionUsedEffortCmd : public NamedCommand
{
public:
    AddCompletionUsedEffortCmd( Completion &completion, const Resource *resource, Completion::UsedEffort *value, const QString& name = QString() );
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

class KPLATOKERNEL_EXPORT AddCompletionActualEffortCmd : public NamedCommand
{
public:
    AddCompletionActualEffortCmd( Completion::UsedEffort &ue, const QDate &date, Completion::UsedEffort::ActualEffort *value, const QString& name = QString() );
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


class KPLATOKERNEL_EXPORT AddAccountCmd : public NamedCommand
{
public:
    AddAccountCmd( Project &project, Account *account, Account *parent = 0, const QString& name = QString() );
    AddAccountCmd( Project &project, Account *account, const QString& parent, const QString& name = QString() );
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

class KPLATOKERNEL_EXPORT RemoveAccountCmd : public NamedCommand
{
public:
    RemoveAccountCmd( Project &project, Account *account, const QString& name = QString() );
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

class KPLATOKERNEL_EXPORT RenameAccountCmd : public NamedCommand
{
public:
    RenameAccountCmd( Account *account, const QString& value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Account *m_account;
    QString m_oldvalue;
    QString m_newvalue;
};

class KPLATOKERNEL_EXPORT ModifyAccountDescriptionCmd : public NamedCommand
{
public:
    ModifyAccountDescriptionCmd( Account *account, const QString& value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Account *m_account;
    QString m_oldvalue;
    QString m_newvalue;
};

class KPLATOKERNEL_EXPORT NodeModifyStartupCostCmd : public NamedCommand
{
public:
    NodeModifyStartupCostCmd( Node &node, double value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    double m_oldvalue;
    double m_newvalue;
};

class KPLATOKERNEL_EXPORT NodeModifyShutdownCostCmd : public NamedCommand
{
public:
    NodeModifyShutdownCostCmd( Node &node, double value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    double m_oldvalue;
    double m_newvalue;
};

class KPLATOKERNEL_EXPORT NodeModifyRunningAccountCmd : public NamedCommand
{
public:
    NodeModifyRunningAccountCmd( Node &node, Account *oldvalue, Account *newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    Account *m_oldvalue;
    Account *m_newvalue;
};

class KPLATOKERNEL_EXPORT NodeModifyStartupAccountCmd : public NamedCommand
{
public:
    NodeModifyStartupAccountCmd( Node &node, Account *oldvalue, Account *newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    Account *m_oldvalue;
    Account *m_newvalue;
};

class KPLATOKERNEL_EXPORT NodeModifyShutdownAccountCmd : public NamedCommand
{
public:
    NodeModifyShutdownAccountCmd( Node &node, Account *oldvalue, Account *newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Node &m_node;
    Account *m_oldvalue;
    Account *m_newvalue;
};

class KPLATOKERNEL_EXPORT ModifyDefaultAccountCmd : public NamedCommand
{
public:
    ModifyDefaultAccountCmd( Accounts &acc, Account *oldvalue, Account *newvalue, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Accounts &m_accounts;
    Account *m_oldvalue;
    Account *m_newvalue;
};

class KPLATOKERNEL_EXPORT ProjectModifyConstraintCmd : public NamedCommand
{
public:
    ProjectModifyConstraintCmd( Project &node, Node::ConstraintType c, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Project &m_node;
    Node::ConstraintType newConstraint;
    Node::ConstraintType oldConstraint;

};

class KPLATOKERNEL_EXPORT ProjectModifyStartTimeCmd : public NamedCommand
{
public:
    ProjectModifyStartTimeCmd( Project &node, const QDateTime& dt, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Project &m_node;
    QDateTime newTime;
    DateTime oldTime;
    KDateTime::Spec m_spec;
};

class KPLATOKERNEL_EXPORT ProjectModifyEndTimeCmd : public NamedCommand
{
public:
    ProjectModifyEndTimeCmd( Project &project, const QDateTime& dt, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    Project &m_node;
    QDateTime newTime;
    DateTime oldTime;
    KDateTime::Spec m_spec;
};


class KPLATOKERNEL_EXPORT AddScheduleManagerCmd : public NamedCommand
{
public:
    AddScheduleManagerCmd( Project &project, ScheduleManager *sm, const QString& name = 0 );
    AddScheduleManagerCmd( ScheduleManager *parent, ScheduleManager *sm, const QString& name = 0 );
    ~AddScheduleManagerCmd();
    void execute();
    void unexecute();

protected:
    Project &m_node;
    ScheduleManager *m_parent;
    ScheduleManager *m_sm;
    int m_index;
    MainSchedule *m_exp, *m_opt, *m_pess;
    bool m_mine;
};

class KPLATOKERNEL_EXPORT DeleteScheduleManagerCmd : public AddScheduleManagerCmd
{
public:
    DeleteScheduleManagerCmd( Project &project, ScheduleManager *sm, const QString& name = 0 );
    void execute();
    void unexecute();

private:
    MacroCommand cmd;
};

class KPLATOKERNEL_EXPORT ModifyScheduleManagerNameCmd : public NamedCommand
{
public:
    ModifyScheduleManagerNameCmd( ScheduleManager &sm, const QString& value, const QString& name = 0 );
    void execute();
    void unexecute();

private:
    ScheduleManager &m_sm;
    QString oldvalue, newvalue;
};

class KPLATOKERNEL_EXPORT ModifyScheduleManagerAllowOverbookingCmd : public NamedCommand
{
public:
    ModifyScheduleManagerAllowOverbookingCmd( ScheduleManager &sm, bool value, const QString& name = 0 );
    void execute();
    void unexecute();

private:
    ScheduleManager &m_sm;
    bool oldvalue, newvalue;
};

class KPLATOKERNEL_EXPORT ModifyScheduleManagerDistributionCmd : public NamedCommand
{
public:
    ModifyScheduleManagerDistributionCmd( ScheduleManager &sm, bool value, const QString& name = 0 );
    void execute();
    void unexecute();

private:
    ScheduleManager &m_sm;
    bool oldvalue, newvalue;
};

class KPLATOKERNEL_EXPORT ModifyScheduleManagerCalculateAllCmd : public NamedCommand
{
public:
    ModifyScheduleManagerCalculateAllCmd( ScheduleManager &sm, bool value, const QString& name = 0 );
    void execute();
    void unexecute();

private:
    ScheduleManager &m_sm;
    bool oldvalue, newvalue;
};

class KPLATOKERNEL_EXPORT CalculateScheduleCmd : public NamedCommand
{
public:
    CalculateScheduleCmd( Project &project, ScheduleManager &sm, const QString& name = 0 );
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

class KPLATOKERNEL_EXPORT BaselineScheduleCmd : public NamedCommand
{
public:
    explicit BaselineScheduleCmd( ScheduleManager &sm, const QString& name = 0 );
    void execute();
    void unexecute();

private:
    ScheduleManager &m_sm;
};

class KPLATOKERNEL_EXPORT ResetBaselineScheduleCmd : public NamedCommand
{
public:
    explicit ResetBaselineScheduleCmd( ScheduleManager &sm, const QString& name = 0 );
    void execute();
    void unexecute();

private:
    ScheduleManager &m_sm;
};

class KPLATOKERNEL_EXPORT ModifyScheduleManagerSchedulingDirectionCmd : public NamedCommand
{
public:
    ModifyScheduleManagerSchedulingDirectionCmd( ScheduleManager &sm, bool value, const QString& name = 0 );
    void execute();
    void unexecute();

private:
    ScheduleManager &m_sm;
    bool oldvalue, newvalue;
};

class KPLATOKERNEL_EXPORT ModifyScheduleManagerSchedulerCmd : public NamedCommand
{
public:
    ModifyScheduleManagerSchedulerCmd( ScheduleManager &sm, int value, const QString& name = 0 );
    void execute();
    void unexecute();

private:
    ScheduleManager &m_sm;
    int oldvalue, newvalue;
};

class KPLATOKERNEL_EXPORT ModifyStandardWorktimeYearCmd : public NamedCommand
{
public:
    ModifyStandardWorktimeYearCmd( StandardWorktime *wt, double oldvalue, double newvalue, const QString& name = QString() );
    void execute();
    void unexecute();
private:
    StandardWorktime *swt;
    double m_oldvalue;
    double m_newvalue;
};

class KPLATOKERNEL_EXPORT ModifyStandardWorktimeMonthCmd : public NamedCommand
{
public:
    ModifyStandardWorktimeMonthCmd( StandardWorktime *wt, double oldvalue, double newvalue, const QString& name = QString() );
    void execute();
    void unexecute();
private:
    StandardWorktime *swt;
    double m_oldvalue;
    double m_newvalue;
};

class KPLATOKERNEL_EXPORT ModifyStandardWorktimeWeekCmd : public NamedCommand
{
public:
    ModifyStandardWorktimeWeekCmd( StandardWorktime *wt, double oldvalue, double newvalue, const QString& name = QString() );
    void execute();
    void unexecute();
private:
    StandardWorktime *swt;
    double m_oldvalue;
    double m_newvalue;
};

class KPLATOKERNEL_EXPORT ModifyStandardWorktimeDayCmd : public NamedCommand
{
public:
    ModifyStandardWorktimeDayCmd( StandardWorktime *wt, double oldvalue, double newvalue, const QString& name = QString() );
    void execute();
    void unexecute();
private:
    StandardWorktime *swt;
    double m_oldvalue;
    double m_newvalue;
};

class KPLATOKERNEL_EXPORT DocumentAddCmd : public NamedCommand
{
public:
    DocumentAddCmd( Documents& docs, Document *value, const QString& name = QString() );
    ~DocumentAddCmd();
    void execute();
    void unexecute();
private:
    Documents& m_docs;
    Document *m_value;
    bool m_mine;
};

class KPLATOKERNEL_EXPORT DocumentRemoveCmd : public NamedCommand
{
public:
    DocumentRemoveCmd( Documents& docs, Document *value, const QString& name = QString() );
    ~DocumentRemoveCmd();
    void execute();
    void unexecute();
private:
    Documents& m_docs;
    Document *m_value;
    bool m_mine;
};

class KPLATOKERNEL_EXPORT DocumentModifyUrlCmd : public NamedCommand
{
public:
    DocumentModifyUrlCmd( Document *doc, const KUrl &url, const QString& name = QString() );
    void execute();
    void unexecute();
private:
    Document *m_doc;
    KUrl m_value;
    KUrl m_oldvalue;
};

class KPLATOKERNEL_EXPORT DocumentModifyTypeCmd : public NamedCommand
{
public:
    DocumentModifyTypeCmd( Document *doc, Document::Type value, const QString& name = QString() );
    void execute();
    void unexecute();
private:
    Document *m_doc;
    Document::Type m_value;
    Document::Type m_oldvalue;
};

class KPLATOKERNEL_EXPORT DocumentModifyStatusCmd : public NamedCommand
{
public:
    DocumentModifyStatusCmd( Document *doc, const QString &value, const QString& name = QString() );
    void execute();
    void unexecute();
private:
    Document *m_doc;
    QString  m_value;
    QString  m_oldvalue;
};

class KPLATOKERNEL_EXPORT DocumentModifySendAsCmd : public NamedCommand
{
    public:
        DocumentModifySendAsCmd( Document *doc, const Document::SendAs value, const QString& name = QString() );
        void execute();
        void unexecute();
    private:
        Document *m_doc;
        Document::SendAs m_value;
        Document::SendAs m_oldvalue;
};

class KPLATOKERNEL_EXPORT WBSDefinitionModifyCmd : public NamedCommand
{
public:
    WBSDefinitionModifyCmd( Project &project, const WBSDefinition value, const QString& name = QString() );
    void execute();
    void unexecute();
private:
    Project &m_project;
    WBSDefinition m_newvalue, m_oldvalue;
};

class KPLATOKERNEL_EXPORT InsertProjectCmd : public MacroCommand
{
public:
    InsertProjectCmd( Project &project, Node *parent, Node *after, const QString& name = QString() );

    void execute();
    void unexecute();

protected:
    void addCalendars( Calendar *calendar, Calendar *parent, QMap<Calendar*, Calendar*> &map );
    void addChildNodes( Node *node );

private:
    Project *m_project;
    Node *m_parent;
    Node *m_after;

};

class KPLATOKERNEL_EXPORT WorkPackageAddCmd : public NamedCommand
{
public:
    WorkPackageAddCmd( Project *project, Node *node, WorkPackage *wp, const QString& name = QString() );
    ~WorkPackageAddCmd();
    void execute();
    void unexecute();
private:
    Project *m_project;
    Node *m_node;
    WorkPackage *m_wp;
    bool m_mine;

};

class KPLATOKERNEL_EXPORT ModifyProjectLocaleCmd : public MacroCommand
{
public:
    ModifyProjectLocaleCmd( Project &project, const QString &name );
    void execute();
    void unexecute();
private:
    Project &m_project;
};

class KPLATOKERNEL_EXPORT ModifyCurrencySymolCmd : public NamedCommand
{
public:
    ModifyCurrencySymolCmd(  KLocale *locale, const QString &value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    KLocale *m_locale;
    QString m_newvalue;
    QString m_oldvalue;
};

class  KPLATOKERNEL_EXPORT ModifyCurrencyFractionalDigitsCmd : public NamedCommand
{
public:
    ModifyCurrencyFractionalDigitsCmd(  KLocale *locale, int value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    KLocale *m_locale;
    int m_newvalue;
    int m_oldvalue;
};

class  KPLATOKERNEL_EXPORT ModifyPositivePrefixCurrencySymolCmd : public NamedCommand
{
public:
    ModifyPositivePrefixCurrencySymolCmd(  KLocale *locale, bool value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    KLocale *m_locale;
    bool m_newvalue;
    bool m_oldvalue;
};

class  KPLATOKERNEL_EXPORT ModifyNegativePrefixCurrencySymolCmd : public NamedCommand
{
public:
    ModifyNegativePrefixCurrencySymolCmd(  KLocale *locale, bool value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    KLocale *m_locale;
    bool m_newvalue;
    bool m_oldvalue;
};

class  KPLATOKERNEL_EXPORT ModifyPositiveMonetarySignPositionCmd : public NamedCommand
{
public:
    ModifyPositiveMonetarySignPositionCmd( KLocale *locale, int value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    KLocale *m_locale;
    int m_newvalue;
    int m_oldvalue;
};

class  KPLATOKERNEL_EXPORT ModifyNegativeMonetarySignPositionCmd : public NamedCommand
{
public:
    ModifyNegativeMonetarySignPositionCmd( KLocale *locale, int value, const QString& name = QString() );
    void execute();
    void unexecute();

private:
    KLocale *m_locale;
    int m_newvalue;
    int m_oldvalue;
};

}  //KPlato namespace

#endif //COMMAND_H
