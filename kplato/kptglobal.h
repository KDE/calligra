/* This file is part of the KDE project
  Copyright (C) 2006 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation;
  version 2 of the License, or (at your option) any later version..

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#ifndef KPLATO_GLOBAL_H
#define KPLATO_GLOBAL_H

#include <QtCore>

#include <klocale.h>

namespace KPlato
{

namespace Role
{
    enum Roles {
        EnumList = Qt::UserRole + 1,
        EnumListValue,
        List,
        ListValues,
        DurationUnit,
        DurationScales,
        Maximum,
        Minimum,
        EditorType
    };
} //namespace Role

namespace ToolTip
{
    static QString NodeName = i18n( "The name of the task." );
    static QString NodeType = i18n( "Task type." );
    static QString NodeResponsible = i18n( "The responsible person for this task." );
    static QString Allocation = i18n( "List of resource allocations." );
    static QString NodeConstraint = i18n( "The scheduling constraint type." );
    static QString NodeConstraintStart = i18n( "Constraint start time." );
    static QString NodeConstraintEnd = i18n( "Constraint end time." );
    static QString NodeDescription = i18n( "Task notes." );
    static QString NodeWBS = i18n( "Work Breakdown Structure Code" );
    static QString NodeRisk = i18n( "Risk controles the PERT distribution used when calculating the actual estimate for this task." );
    static QString NodeRunningAccount = i18n( "Account for running costs." );
    static QString NodeStartupAccount = i18n( "Account for cost incurred at startup of the task." );
    static QString NodeStartupCost = i18n( "The cost incurred at startup of the task." );
    static QString NodeShutdownAccount = i18n( "Account for cost incurred at shutdown of the task." );
    static QString NodeShutdownCost = i18n( "The cost incurred at shutdown of the task." );
                
    static QString NodeEarlyStart = i18n( "Early start" );
    static QString NodeEarlyFinish = i18n( "Early start" );
    static QString NodeLateStart = i18n( "Late start" );
    static QString NodeLateFinish = i18n( "Late finish" );
    
    static QString  NodePositiveFloat = i18n( "The duration by which a tasks start can be delayed  without affecting the project completion time." );
    static QString  NodeNegativeFloat = i18n( "The duration by which the duration of a task or path has to be reduced in order to fullfill a timing constraint." );
    static QString  NodeFreeFloat = i18n( "The duration by which a task can be delayed or extended without affecting the start of any succeeding task." );
    static QString  NodeStartFloat = i18n( "The duration from Early Start to Late Start." );
    static QString  NodeFinishFloat = i18n( "The duration from Early Finish to Late Finish." );

    static QString EstimateType = i18n( "Type of estimate." );
    static QString Estimate = i18n( "The expected estimate." );
    static QString OptimisticRatio = i18n( "Optimistic estimate." );
    static QString PessimisticRatio = i18n( "Pessimistic estimate." );
    static QString RiskType = i18n( "Type of risk." );

    static QString ResourceName = i18n( "The name of the resource or resource group." );
    static QString ResourceType = i18n( "The type of the resource or resource group." );
    static QString ResourceInitials = i18n( "The initials of the resource.." );
    static QString ResourceEMail = i18n( "The e-mail address of the resource." );
    static QString ResourceCalendar = i18n( "The calendar defines when the resource is working." );
    static QString ResourceUnits = i18n( "The maximum load that can be assigned." );
    static QString ResourceAvailableFrom = i18n( "Defines when the resource is available to the project." );
    static QString ResourceAvailableUntil = i18n( "Defines when the resource is available to the project." );
    static QString ResourceNormalRate = i18n( "The cost pr hour, normal hours." );
    static QString ResourceOvertimeRate = i18n( "The cost pr hour, overtime hours." );
    static QString ResourceFixedCost = i18n( "The fixed cost." );

    static QString AccountName = i18n( "The name of the account" );
    static QString AccountDescription = i18n( "The description of the account" );

    static QString ScheduleName = i18n( "The name of the schedule." );
    static QString ScheduleState = i18n( "The schedules state." );
    static QString ScheduleOverbooking = i18n( "Allow or avoid resource overbooking when calculating." );
    static QString ScheduleDistribution = i18n( "The distribution to be used during calculation." );
    static QString ScheduleCalculate = i18n( "Defines the schedules to be calculated." );

} //namespace ToolTip

namespace WhatsThis
{
    static QString  NodeNegativeFloat = i18n( "Negative float is the duration by which the duration of a task or path has to be reduced in order to fullfill a timing constraint." );
    static QString  NodeFreeFloat = i18n( "Free float is the duration by which a task can be delayed or extended without affecting the start of any succeeding task." );
    static QString  NodeStartFloat = i18n( "Start float is the duration from Early Start to Late Start." );
    static QString  NodeFinishFloat = i18n( "Finish float is the duration from Early Finish to Late Finish." );

} //namespace WhatsThis


} //namespace KPlato

#endif
