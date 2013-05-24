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

#include <klocale.h>

namespace KPlato
{

    QString ToolTip::nodeName() { return i18nc( "@info:tooltip", "The name of the task" ); }
    QString ToolTip::nodeType() { return i18nc( "@info:tooltip", "Task type" ); }
    QString ToolTip::nodeResponsible() { return i18nc( "@info:tooltip", "The person responsible for this task" ); }
    QString ToolTip::allocation() { return i18nc( "@info:tooltip", "List of resource allocations" ); }
    QString ToolTip::nodeConstraint() { return i18nc( "@info:tooltip", "The timing constraint type" ); }
    QString ToolTip::nodeConstraintStart() { return i18nc( "@info:tooltip", "Constraint start time" ); }
    QString ToolTip::nodeConstraintEnd() { return i18nc( "@info:tooltip", "Constraint end time" ); }
    QString ToolTip::nodeDescription() { return i18nc( "@info:tooltip", "Task notes" ); }
    QString ToolTip::nodeWBS() { return i18nc( "@info:tooltip", "Work Breakdown Structure Code" ); }
    QString ToolTip::nodeLevel() { return i18nc( "@info:tooltip", "Node level" ); }
    QString ToolTip::nodeRisk() { return i18nc( "@info:tooltip", "Risk controls the PERT distribution used when calculating the actual estimate for this task" ); }
    QString ToolTip::nodeRunningAccount() { return i18nc( "@info:tooltip", "Account for running costs" ); }
    QString ToolTip::nodeStartupAccount() { return i18nc( "@info:tooltip", "Account for cost incurred at startup of the task" ); }
    QString ToolTip::nodeStartupCost() { return i18nc( "@info:tooltip", "The cost incurred at startup of the task" ); }
    QString ToolTip::nodeShutdownAccount() { return i18nc( "@info:tooltip", "Account for cost incurred at shutdown of the task" ); }
    QString ToolTip::nodeShutdownCost() { return i18nc( "@info:tooltip", "The cost incurred at shutdown of the task" ); }

    QString ToolTip::nodeStartTime() { return i18nc( "@info:tooltip", "Planned start time" ); }
    QString ToolTip::nodeEndTime() { return i18nc( "@info:tooltip", "Planned finish time" ); }
    QString ToolTip::nodeEarlyStart() { return i18nc( "@info:tooltip", "Earliest start time allowed by dependencies" ); }
    QString ToolTip::nodeEarlyFinish() { return i18nc( "@info:tooltip", "Earliest finish time allowed by dependencies" ); }
    QString ToolTip::nodeLateStart() { return i18nc( "@info:tooltip", "Latest start time allowed by dependencies" ); }
    QString ToolTip::nodeLateFinish() { return i18nc( "@info:tooltip", "Latest finish time allowed by dependencies" ); }
    
    QString ToolTip::nodeDuration() { return i18nc( "@info:tooltip", "The planned duration" ); }
    QString ToolTip::nodeVarianceDuration() { return i18nc( "@info:tooltip", "The variance of the duration" ); }
    QString ToolTip::nodeOptimisticDuration() { return i18nc( "@info:tooltip", "The optimistic duration" ); }
    QString ToolTip::nodePessimisticDuration() { return i18nc( "@info:tooltip", "The pessimistic duration" ); }

    QString ToolTip::nodePositiveFloat() { return i18nc( "@info:tooltip", "The duration by which a tasks start can be delayed without affecting the project completion time" ); }
    
    QString ToolTip::nodeNegativeFloat() { return i18nc( "@info:tooltip", "The duration by which the duration of a task or path has to be reduced in order to fulfill a timing constraint" ); }
    QString WhatsThis::nodeNegativeFloat() { return i18nc( "@info:whatsthis", "Negative float is the duration by which the duration of a task or path has to be reduced in order to fulfill a timing constraint." ); }

    QString ToolTip::nodeFreeFloat() { return i18nc( "@info:tooltip", "The duration by which a task can be delayed or extended without affecting the start of any succeeding task" ); }
    QString WhatsThis::nodeFreeFloat() { return i18nc( "@info:whatsthis", "Free float is the duration by which a task can be delayed or extended without affecting the start of any succeeding task." ); }

    QString ToolTip::nodeStartFloat() { return i18nc( "@info:tooltip", "The duration from Early Start to Late Start" ); }
    QString WhatsThis::nodeStartFloat() { return i18nc( "@info:whatsthis", "Start float is the duration from Early Start to Late Start." ); }

    QString ToolTip::nodeFinishFloat() { return i18nc( "@info:tooltip", "The duration from Early Finish to Late Finish" ); }
    QString WhatsThis::nodeFinishFloat() { return i18nc( "@info:whatsthis", "Finish float is the duration from Early Finish to Late Finish." ); }

    QString ToolTip::nodeAssignment() { return i18nc( "@info:tooltip", "The resources assigned to the task" ); }

    QString ToolTip::nodeStatus() { return i18nc( "@info:tooltip", "Task status" ); }
    QString ToolTip::nodeCompletion() { return i18nc( "@info:tooltip", "Task completion" ); }
    QString ToolTip::nodePlannedEffortTo() { return i18nc( "@info:tooltip", "Planned effort" ); }
    QString ToolTip::nodeActualEffortTo() { return i18nc( "@info:tooltip", "Actual effort" ); }
    QString ToolTip::nodeRemainingEffort() { return i18nc( "@info:tooltip", "Remaining effort" ); }
    QString ToolTip::nodePlannedCostTo() { return i18nc( "@info:tooltip", "Planned cost" ); }
    QString ToolTip::nodeActualCostTo() { return i18nc( "@info:tooltip", "Actual cost" ); }
    QString ToolTip::completionStartedTime() { return i18nc( "@info:tooltip", "Time when task was actually started" ); }
    QString ToolTip::completionStarted() { return i18nc( "@info:tooltip", "Shows if the task is started" ); }
    QString ToolTip::completionFinishedTime() { return i18nc( "@info:tooltip", "Time when task was actually finished" ); }
    QString ToolTip::completionFinished() { return i18nc( "@info:tooltip", "Shows if the task is finished" ); }
    QString ToolTip::completionStatusNote() { return i18nc( "@info:tooltip", "Status Note" ); }
    
    QString ToolTip::estimateExpected() { return i18nc( "@info:tooltip", "Calculated expected estimate" ); }
    QString ToolTip::estimateVariance() { return i18nc( "@info:tooltip", "Calculated estimate variance" ); }
    QString ToolTip::estimateOptimistic() { return i18nc( "@info:tooltip", "Optimistic estimate" ); }
    QString ToolTip::estimatePessimistic() { return i18nc( "@info:tooltip", "Pessimistic estimate" ); }
    QString ToolTip::estimateType() { return i18nc( "@info:tooltip", "Type of estimate" ); }
    QString ToolTip::estimateCalendar() { return i18nc( "@info:tooltip", "The calendar used when estimate type is Duration" ); }
    QString ToolTip::estimate() { return i18nc( "@info:tooltip", "The most likely estimate" ); }
    QString ToolTip::optimisticRatio() { return i18nc( "@info:tooltip", "Optimistic estimate" ); }
    QString ToolTip::pessimisticRatio() { return i18nc( "@info:tooltip", "Pessimistic estimate" ); }
    QString ToolTip::riskType() { return i18nc( "@info:tooltip", "Type of risk" ); }

    QString ToolTip::nodeSchedulingStatus() { return i18nc( "@info:tooltip", "Shows the tasks scheduling status" ); }
    QString ToolTip::nodeNotScheduled() { return i18nc( "@info:tooltip", "The task has not been scheduled" ); }
    QString ToolTip::nodeAssignmentMissing() { return i18n( "An effort has been estimated, but no resource has been assigned" ); }
    QString ToolTip::nodeResourceOverbooked() { return i18nc( "@info:tooltip", "A resource assigned to this task is overbooked" ); }
    QString ToolTip::nodeResourceUnavailable() { return i18nc( "@info:tooltip", "A resource assigned to this task is not available" ); }
    QString ToolTip::nodeConstraintsError() { return i18nc( "@info:tooltip", "A timing constraint could not be met" ); }
    QString ToolTip::nodeEffortNotMet() { return i18nc( "@info:tooltip", "The assigned resource could not meet the estimated effort" ); }
    QString ToolTip::nodeSchedulingError() { return i18nc( "@info:tooltip", "A scheduling error occurred" ); }

    QString ToolTip::nodeBCWS() { return i18nc( "@info:tooltip", "Budgeted Cost of Work Scheduled" ); }
    QString ToolTip::nodeBCWP() { return i18nc( "@info:tooltip", "Budgeted Cost of Work Performed" ); }
    QString ToolTip::nodeACWP() { return i18nc( "@info:tooltip", "Actual Cost of Work Performed" ); }
    QString ToolTip::nodePerformanceIndex() { return i18n( "Schedule performance index (BCWP/BCWS)" ); }

    QString ToolTip::resourceName() { return i18nc( "@info:tooltip", "The name of the resource or resource group" ); }
    QString ToolTip::resourceType() { return i18nc( "@info:tooltip", "The type of the resource or resource group" ); }
    QString ToolTip::resourceInitials() { return i18nc( "@info:tooltip", "The initials of the resource" ); }
    QString ToolTip::resourceEMail() { return i18nc( "@info:tooltip", "The e-mail address of the resource" ); }
    QString ToolTip::resourceCalendar() { return i18nc( "@info:tooltip", "The calendar defines when the resource is working" ); }
    QString ToolTip::resourceUnits() { return i18nc( "@info:tooltip", "The maximum load that can be assigned" ); }
    QString ToolTip::resourceAvailableFrom() { return i18nc( "@info:tooltip", "Defines when the resource is available to the project" ); }
    QString ToolTip::resourceAvailableUntil() { return i18nc( "@info:tooltip", "Defines when the resource is available to the project" ); }
    QString ToolTip::resourceNormalRate() { return i18nc( "@info:tooltip", "The cost pr hour, normal hours" ); }
    QString ToolTip::resourceOvertimeRate() { return i18nc( "@info:tooltip", "The cost pr hour, overtime hours" ); }
    QString ToolTip::resourceFixedCost() { return i18nc( "@info:tooltip", "The fixed cost" ); }
    QString ToolTip::resourceAccount() { return i18nc( "@info:tooltip", "The account where the resource cost is accumulated" ); }

    QString ToolTip::accountName() { return i18nc( "@info:tooltip", "The name of the account" ); }
    QString ToolTip::accountDescription() { return i18nc( "@info:tooltip", "The description of the account" ); }

    QString ToolTip::scheduleName() { return i18nc( "@info:tooltip", "The name of the schedule" ); }
    QString ToolTip::scheduleState() { return i18nc( "@info:tooltip", "The schedules state" ); }

    QString ToolTip::scheduleOverbooking() { return i18nc( "@info:tooltip", "Controls resource overbooking when scheduling" ); }
    QString WhatsThis::scheduleOverbooking() { return i18nc( "@info:whatsthis", 
            "<para>Controls resource overbooking when scheduling.</para>"
            "<para>If overbooking is allowed, a resource may be booked (on working days) to work more than it is available. This can happen if the resource is allocated to multiple tasks or are booked on other projects.</para>"
            "<para>If overbooking is to be avoided, resources will not be booked more than they are available. On resource conflict, tasks will be delayed until the resource is available.</para>"
            );
    }

    QString ToolTip::scheduleDistribution() { return i18nc( "@info:tooltip", "The distribution to be used during scheduling" ); }
    QString WhatsThis::scheduleDistribution() { return i18nc( "@info:whatsthis",
        "<para>The distribution to be used during scheduling</para>"
        "<para>If distribution is 'None', the tasks estimate is used as is during scheduling.</para>"
        "<para>If distribution is 'PERT', the estimate used is calculated based on the entered optimistic- (O), pessimistic-(P) and most likely (M) estimate. The formula used for this is (O + 4 * M + P) / 6.<para>"
        );
    }

    QString ToolTip::scheduleCalculate() { return i18nc( "@info:tooltip", "Defines the schedules to be calculated" ); }
    QString ToolTip::scheduleStart() { return i18nc( "@info:tooltip", "The scheduled start time" ); }
    QString ToolTip::scheduleFinish() { return i18nc( "@info:tooltip", "The scheduled finish time" ); }
    QString ToolTip::schedulingDirection() { return i18nc( "@info:tooltip", "The scheduling direction" ); }
    QString WhatsThis::schedulingDirection() { return i18nc( "@info:whatsthis",
        "<para>The scheduling direction.</para>"
        "<para>If direction is Forward, the project is scheduled starting at the projects earliest start time specified in the main project dialog.</para>"
        "<para>If direction is Backward, the project is scheduled starting at the projects latest finish time specified in the main project dialog.</para>"
        );
    }

    QString ToolTip::scheduleScheduler() { return i18nc( "@info:tooltip", "The scheduler used for calculating the project schedule" ); }
    QString WhatsThis::scheduleScheduler() { return i18nc( "@info:whatsthis", 
        "<para>The scheduler used for calculating the project schedule.</para>"
        "<para>The default built-in scheduler is the Network Scheduler.</para>"
        "<para>Other schedulers presently available is RCPS if libRCPS is installed on your system. "
        "RCPS is a genetics based resource constrained project scheduler.</para>"
        );
    }

    QString ToolTip::scheduleGranularity() { return i18nc( "@info:tooltip", "The granularity used when calculating the project schedule" ); }

    QString ToolTip::documentUrl() { return i18nc( "@info:tooltip", "The url of the document" ); }
    QString ToolTip::documentType() { return i18nc( "@info:tooltip", "The type of the document" ); }
    QString ToolTip::documentStatus() { return i18nc( "@info:tooltip", "The status of the document" ); }
    QString ToolTip::documentSendAs() { return i18nc( "@info:tooltip", "Defines how this document is sent" ); }

    QString ToolTip::calendarName() { return i18nc( "@info:tooltip", "The name of the calendar" ); }
    QString ToolTip::calendarTimeZone() { return i18nc( "@info:tooltip", "The timezone of the calendar" ); }

    QString ToolTip::relationParent() { return i18nc( "@info:tooltip", "The name of the required task" ); }
    QString ToolTip::relationChild() { return i18nc( "@info:tooltip", "The name of the dependent task" ); }
    QString ToolTip::relationType() { return i18nc( "@info:tooltip", "The type of relation" ); }
    QString ToolTip::relationLag() { return i18nc( "@info:tooltip", "The relations time lag" ); }


    // Work around string freeze
    QString UndoText::removeDocument() { return i18nc( "(qtundo-format)", "Remove document" ); }

} //namespace KPlato

