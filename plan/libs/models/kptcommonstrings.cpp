/* This file is part of the KDE project
  Copyright (C) 2009, 2011 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
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

#include "kptcommonstrings.h"

#include <kundo2magicstring.h>

#include <KLocalizedString>

namespace KPlato
{

    QString ToolTip::nodeName() { return xi18nc( "@info:tooltip", "The name of the task" ); }
    QString ToolTip::nodeType() { return xi18nc( "@info:tooltip", "Task type" ); }
    QString ToolTip::nodeResponsible() { return xi18nc( "@info:tooltip", "The person responsible for this task" ); }
    QString ToolTip::allocation() { return xi18nc( "@info:tooltip", "List of resource allocations" ); }
    QString ToolTip::nodeConstraint() { return xi18nc( "@info:tooltip", "The timing constraint type" ); }
    QString ToolTip::nodeConstraintStart() { return xi18nc( "@info:tooltip", "Constraint start time" ); }
    QString ToolTip::nodeConstraintEnd() { return xi18nc( "@info:tooltip", "Constraint end time" ); }
    QString ToolTip::nodeDescription() { return xi18nc( "@info:tooltip", "Task notes" ); }
    QString ToolTip::nodeWBS() { return xi18nc( "@info:tooltip", "Work Breakdown Structure Code" ); }
    QString ToolTip::nodeLevel() { return xi18nc( "@info:tooltip", "Node level" ); }
    QString ToolTip::nodeRisk() { return xi18nc( "@info:tooltip", "Risk controls the PERT distribution used when calculating the actual estimate for this task" ); }
    QString ToolTip::nodeRunningAccount() { return xi18nc( "@info:tooltip", "Account for running costs" ); }
    QString ToolTip::nodeStartupAccount() { return xi18nc( "@info:tooltip", "Account for cost incurred at startup of the task" ); }
    QString ToolTip::nodeStartupCost() { return xi18nc( "@info:tooltip", "The cost incurred at startup of the task" ); }
    QString ToolTip::nodeShutdownAccount() { return xi18nc( "@info:tooltip", "Account for cost incurred at shutdown of the task" ); }
    QString ToolTip::nodeShutdownCost() { return xi18nc( "@info:tooltip", "The cost incurred at shutdown of the task" ); }

    QString ToolTip::nodeStartTime() { return xi18nc( "@info:tooltip", "Planned start time" ); }
    QString ToolTip::nodeEndTime() { return xi18nc( "@info:tooltip", "Planned finish time" ); }
    QString ToolTip::nodeEarlyStart() { return xi18nc( "@info:tooltip", "Earliest start time allowed by dependencies" ); }
    QString ToolTip::nodeEarlyFinish() { return xi18nc( "@info:tooltip", "Earliest finish time allowed by dependencies" ); }
    QString ToolTip::nodeLateStart() { return xi18nc( "@info:tooltip", "Latest start time allowed by dependencies" ); }
    QString ToolTip::nodeLateFinish() { return xi18nc( "@info:tooltip", "Latest finish time allowed by dependencies" ); }
    
    QString ToolTip::nodeDuration() { return xi18nc( "@info:tooltip", "The planned duration" ); }
    QString ToolTip::nodeVarianceDuration() { return xi18nc( "@info:tooltip", "The variance of the duration" ); }
    QString ToolTip::nodeOptimisticDuration() { return xi18nc( "@info:tooltip", "The optimistic duration" ); }
    QString ToolTip::nodePessimisticDuration() { return xi18nc( "@info:tooltip", "The pessimistic duration" ); }

    QString ToolTip::nodePositiveFloat() { return xi18nc( "@info:tooltip", "The duration by which a tasks start can be delayed without affecting the project completion time" ); }
    
    QString ToolTip::nodeNegativeFloat() { return xi18nc( "@info:tooltip", "The duration by which the duration of a task or path has to be reduced in order to fulfill a timing constraint" ); }
    QString WhatsThis::nodeNegativeFloat() { return xi18nc( "@info:whatsthis", "Negative float is the duration by which the duration of a task or path has to be reduced in order to fulfill a timing constraint." ); }

    QString ToolTip::nodeFreeFloat() { return xi18nc( "@info:tooltip", "The duration by which a task can be delayed or extended without affecting the start of any succeeding task" ); }
    QString WhatsThis::nodeFreeFloat() { return xi18nc( "@info:whatsthis", "Free float is the duration by which a task can be delayed or extended without affecting the start of any succeeding task." ); }

    QString ToolTip::nodeStartFloat() { return xi18nc( "@info:tooltip", "The duration from Early Start to Late Start" ); }
    QString WhatsThis::nodeStartFloat() { return xi18nc( "@info:whatsthis", "Start float is the duration from Early Start to Late Start." ); }

    QString ToolTip::nodeFinishFloat() { return xi18nc( "@info:tooltip", "The duration from Early Finish to Late Finish" ); }
    QString WhatsThis::nodeFinishFloat() { return xi18nc( "@info:whatsthis", "Finish float is the duration from Early Finish to Late Finish." ); }

    QString ToolTip::nodeAssignment() { return xi18nc( "@info:tooltip", "The resources assigned to the task" ); }

    QString ToolTip::nodeStatus() { return xi18nc( "@info:tooltip", "Task status" ); }
    QString ToolTip::nodeCompletion() { return xi18nc( "@info:tooltip", "Task completion" ); }
    QString ToolTip::nodePlannedEffortTo() { return xi18nc( "@info:tooltip", "Planned effort" ); }
    QString ToolTip::nodeActualEffortTo() { return xi18nc( "@info:tooltip", "Actual effort" ); }
    QString ToolTip::nodeRemainingEffort() { return xi18nc( "@info:tooltip", "Remaining effort" ); }
    QString ToolTip::nodePlannedCostTo() { return xi18nc( "@info:tooltip", "Planned cost" ); }
    QString ToolTip::nodeActualCostTo() { return xi18nc( "@info:tooltip", "Actual cost" ); }
    QString ToolTip::completionStartedTime() { return xi18nc( "@info:tooltip", "Time when task was actually started" ); }
    QString ToolTip::completionStarted() { return xi18nc( "@info:tooltip", "Shows if the task is started" ); }
    QString ToolTip::completionFinishedTime() { return xi18nc( "@info:tooltip", "Time when task was actually finished" ); }
    QString ToolTip::completionFinished() { return xi18nc( "@info:tooltip", "Shows if the task is finished" ); }
    QString ToolTip::completionStatusNote() { return xi18nc( "@info:tooltip", "Status Note" ); }
    
    QString ToolTip::estimateExpected() { return xi18nc( "@info:tooltip", "Calculated expected estimate" ); }
    QString ToolTip::estimateVariance() { return xi18nc( "@info:tooltip", "Calculated estimate variance" ); }
    QString ToolTip::estimateOptimistic() { return xi18nc( "@info:tooltip", "Optimistic estimate" ); }
    QString ToolTip::estimatePessimistic() { return xi18nc( "@info:tooltip", "Pessimistic estimate" ); }
    QString ToolTip::estimateType() { return xi18nc( "@info:tooltip", "Type of estimate" ); }
    QString ToolTip::estimateCalendar() { return xi18nc( "@info:tooltip", "The calendar used when estimate type is Duration" ); }
    QString ToolTip::estimate() { return xi18nc( "@info:tooltip", "The most likely estimate" ); }
    QString ToolTip::optimisticRatio() { return xi18nc( "@info:tooltip", "Optimistic estimate" ); }
    QString ToolTip::pessimisticRatio() { return xi18nc( "@info:tooltip", "Pessimistic estimate" ); }
    QString ToolTip::riskType() { return xi18nc( "@info:tooltip", "Type of risk" ); }

    QString ToolTip::nodeSchedulingStatus() { return xi18nc( "@info:tooltip", "Shows the tasks scheduling status" ); }
    QString ToolTip::nodeNotScheduled() { return xi18nc( "@info:tooltip", "The task has not been scheduled" ); }
    QString ToolTip::nodeAssignmentMissing() { return i18n( "An effort has been estimated, but no resource has been assigned" ); }
    QString ToolTip::nodeResourceOverbooked() { return xi18nc( "@info:tooltip", "A resource assigned to this task is overbooked" ); }
    QString ToolTip::nodeResourceUnavailable() { return xi18nc( "@info:tooltip", "A resource assigned to this task is not available" ); }
    QString ToolTip::nodeConstraintsError() { return xi18nc( "@info:tooltip", "A timing constraint could not be met" ); }
    QString ToolTip::nodeEffortNotMet() { return xi18nc( "@info:tooltip", "The assigned resource could not meet the estimated effort" ); }
    QString ToolTip::nodeSchedulingError() { return xi18nc( "@info:tooltip", "A scheduling error occurred" ); }

    QString ToolTip::nodeBCWS() { return xi18nc( "@info:tooltip", "Budgeted Cost of Work Scheduled" ); }
    QString ToolTip::nodeBCWP() { return xi18nc( "@info:tooltip", "Budgeted Cost of Work Performed" ); }
    QString ToolTip::nodeACWP() { return xi18nc( "@info:tooltip", "Actual Cost of Work Performed" ); }
    QString ToolTip::nodePerformanceIndex() { return i18n( "Schedule performance index (BCWP/BCWS)" ); }

    QString ToolTip::resourceName() { return xi18nc( "@info:tooltip", "The name of the resource or resource group" ); }
    QString ToolTip::resourceType() { return xi18nc( "@info:tooltip", "The type of the resource or resource group" ); }
    QString ToolTip::resourceInitials() { return xi18nc( "@info:tooltip", "The initials of the resource" ); }
    QString ToolTip::resourceEMail() { return xi18nc( "@info:tooltip", "The e-mail address of the resource" ); }
    QString ToolTip::resourceCalendar() { return xi18nc( "@info:tooltip", "The calendar defines when the resource is working" ); }
    QString ToolTip::resourceUnits() { return xi18nc( "@info:tooltip", "The maximum load that can be assigned" ); }
    QString ToolTip::resourceAvailableFrom() { return xi18nc( "@info:tooltip", "Defines when the resource is available to the project" ); }
    QString ToolTip::resourceAvailableUntil() { return xi18nc( "@info:tooltip", "Defines when the resource is available to the project" ); }
    QString ToolTip::resourceNormalRate() { return xi18nc( "@info:tooltip", "The cost pr hour, normal hours" ); }
    QString ToolTip::resourceOvertimeRate() { return xi18nc( "@info:tooltip", "The cost pr hour, overtime hours" ); }
    QString ToolTip::resourceFixedCost() { return xi18nc( "@info:tooltip", "The fixed cost" ); }
    QString ToolTip::resourceAccount() { return xi18nc( "@info:tooltip", "The account where the resource cost is accumulated" ); }

    QString ToolTip::accountName() { return xi18nc( "@info:tooltip", "The name of the account" ); }
    QString ToolTip::accountDescription() { return xi18nc( "@info:tooltip", "The description of the account" ); }

    QString ToolTip::scheduleName() { return xi18nc( "@info:tooltip", "The name of the schedule" ); }
    QString ToolTip::scheduleState() { return xi18nc( "@info:tooltip", "The schedules state" ); }

    QString ToolTip::scheduleOverbooking() { return xi18nc( "@info:tooltip", "Controls resource overbooking when scheduling" ); }
    QString WhatsThis::scheduleOverbooking() { return xi18nc( "@info:whatsthis", 
            "<para>Controls resource overbooking when scheduling.</para>"
            "<para>If overbooking is allowed, a resource may be booked (on working days) to work more than it is available. This can happen if the resource is allocated to multiple tasks or are booked on other projects.</para>"
            "<para>If overbooking is to be avoided, resources will not be booked more than they are available. On resource conflict, tasks will be delayed until the resource is available.</para>"
            );
    }

    QString ToolTip::scheduleDistribution() { return xi18nc( "@info:tooltip", "The distribution to be used during scheduling" ); }
    QString WhatsThis::scheduleDistribution() { return xi18nc( "@info:whatsthis",
        "<para>The distribution to be used during scheduling</para>"
        "<para>If distribution is 'None', the tasks estimate is used as is during scheduling.</para>"
        "<para>If distribution is 'PERT', the estimate used is calculated based on the entered optimistic- (O), pessimistic-(P) and most likely (M) estimate. The formula used for this is (O + 4 * M + P) / 6.<para>"
        );
    }

    QString ToolTip::scheduleCalculate() { return xi18nc( "@info:tooltip", "Defines the schedules to be calculated" ); }
    QString ToolTip::scheduleStart() { return xi18nc( "@info:tooltip", "The scheduled start time" ); }
    QString ToolTip::scheduleFinish() { return xi18nc( "@info:tooltip", "The scheduled finish time" ); }
    QString ToolTip::schedulingDirection() { return xi18nc( "@info:tooltip", "The scheduling direction" ); }
    QString WhatsThis::schedulingDirection() { return xi18nc( "@info:whatsthis",
        "<para>The scheduling direction.</para>"
        "<para>If direction is Forward, the project is scheduled starting at the projects earliest start time specified in the main project dialog.</para>"
        "<para>If direction is Backward, the project is scheduled starting at the projects latest finish time specified in the main project dialog.</para>"
        );
    }

    QString ToolTip::scheduleScheduler() { return xi18nc( "@info:tooltip", "The scheduler used for calculating the project schedule" ); }
    QString WhatsThis::scheduleScheduler() { return xi18nc( "@info:whatsthis", 
        "<para>The scheduler used for calculating the project schedule.</para>"
        "<para>The default built-in scheduler is the Network Scheduler.</para>"
        "<para>Other schedulers presently available is RCPS if libRCPS is installed on your system. "
        "RCPS is a genetics based resource constrained project scheduler.</para>"
        );
    }

    QString ToolTip::scheduleGranularity() { return xi18nc( "@info:tooltip", "The granularity used when calculating the project schedule" ); }

    QString ToolTip::documentUrl() { return xi18nc( "@info:tooltip", "The url of the document" ); }
    QString ToolTip::documentType() { return xi18nc( "@info:tooltip", "The type of the document" ); }
    QString ToolTip::documentStatus() { return xi18nc( "@info:tooltip", "The status of the document" ); }
    QString ToolTip::documentSendAs() { return xi18nc( "@info:tooltip", "Defines how this document is sent" ); }

    QString ToolTip::calendarName() { return xi18nc( "@info:tooltip", "The name of the calendar" ); }
    QString ToolTip::calendarTimeZone() { return xi18nc( "@info:tooltip", "The timezone of the calendar" ); }

    QString ToolTip::relationParent() { return xi18nc( "@info:tooltip", "The name of the required task" ); }
    QString ToolTip::relationChild() { return xi18nc( "@info:tooltip", "The name of the dependent task" ); }
    QString ToolTip::relationType() { return xi18nc( "@info:tooltip", "The type of relation" ); }
    QString ToolTip::relationLag() { return xi18nc( "@info:tooltip", "The relations time lag" ); }


    // Work around string freeze
    KUndo2MagicString UndoText::removeDocument() { return kundo2_i18n( "Remove document" ); }

} //namespace KPlato

