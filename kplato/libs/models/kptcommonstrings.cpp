/* This file is part of the KDE project
  Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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

    QString ToolTip::nodeName() { return i18n( "The name of the task" ); }
    QString ToolTip::nodeType() { return i18n( "Task type" ); }
    QString ToolTip::nodeResponsible() { return i18n( "The responsible person for this task" ); }
    QString ToolTip::allocation() { return i18n( "List of resource allocations" ); }
    QString ToolTip::nodeConstraint() { return i18n( "The timing constraint type" ); }
    QString ToolTip::nodeConstraintStart() { return i18n( "Constraint start time" ); }
    QString ToolTip::nodeConstraintEnd() { return i18n( "Constraint end time" ); }
    QString ToolTip::nodeDescription() { return i18n( "Task notes" ); }
    QString ToolTip::nodeWBS() { return i18n( "Work Breakdown Structure Code" ); }
    QString ToolTip::nodeLevel() { return i18n( "Node level" ); }
    QString ToolTip::nodeRisk() { return i18n( "Risk controls the PERT distribution used when calculating the actual estimate for this task" ); }
    QString ToolTip::nodeRunningAccount() { return i18n( "Account for running costs" ); }
    QString ToolTip::nodeStartupAccount() { return i18n( "Account for cost incurred at startup of the task" ); }
    QString ToolTip::nodeStartupCost() { return i18n( "The cost incurred at startup of the task" ); }
    QString ToolTip::nodeShutdownAccount() { return i18n( "Account for cost incurred at shutdown of the task" ); }
    QString ToolTip::nodeShutdownCost() { return i18n( "The cost incurred at shutdown of the task" ); }

    QString ToolTip::nodeStartTime() { return i18n( "Planned start time" ); }
    QString ToolTip::nodeEndTime() { return i18n( "Planned finish time" ); }
    QString ToolTip::nodeEarlyStart() { return i18n( "Earliest start time allowed by dependencies" ); }
    QString ToolTip::nodeEarlyFinish() { return i18n( "Earliest finish time allowed by dependencies" ); }
    QString ToolTip::nodeLateStart() { return i18n( "Latest start time allowed by dependencies" ); }
    QString ToolTip::nodeLateFinish() { return i18n( "Latest finish time allowed by dependencies" ); }
    
    QString ToolTip::nodeDuration() { return i18n( "The planned duration" ); }
    QString ToolTip::nodeVarianceDuration() { return i18n( "The variance of the duration" ); }
    QString ToolTip::nodeOptimisticDuration() { return i18n( "The optimistic duration" ); }
    QString ToolTip::nodePessimisticDuration() { return i18n( "The pessimistic duration" ); }

    QString ToolTip::nodePositiveFloat() { return i18n( "The duration by which a tasks start can be delayed without affecting the project completion time" ); }
    
    QString ToolTip::nodeNegativeFloat() { return i18n( "The duration by which the duration of a task or path has to be reduced in order to fulfil a timing constraint" ); }
    QString WhatsThis::nodeNegativeFloat() { return i18nc( "@info:whatsthis", "Negative float is the duration by which the duration of a task or path has to be reduced in order to fullfill a timing constraint." ); }

    QString ToolTip::nodeFreeFloat() { return i18n( "The duration by which a task can be delayed or extended without affecting the start of any succeeding task" ); }
    QString WhatsThis::nodeFreeFloat() { return i18nc( "@info:whatsthis", "Free float is the duration by which a task can be delayed or extended without affecting the start of any succeeding task." ); }

    QString ToolTip::nodeStartFloat() { return i18n( "The duration from Early Start to Late Start" ); }
    QString WhatsThis::nodeStartFloat() { return i18nc( "@info:whatsthis", "Start float is the duration from Early Start to Late Start." ); }

    QString ToolTip::nodeFinishFloat() { return i18n( "The duration from Early Finish to Late Finish" ); }
    QString WhatsThis::nodeFinishFloat() { return i18nc( "@info:whatsthis", "Finish float is the duration from Early Finish to Late Finish." ); }

    QString ToolTip::nodeAssignment() { return i18n( "The resources assigned to the task" ); }

    QString ToolTip::nodeStatus() { return i18n( "Task status" ); }
    QString ToolTip::nodeCompletion() { return i18n( "Task completion" ); }
    QString ToolTip::nodePlannedEffortTo() { return i18n( "Planned effort" ); }
    QString ToolTip::nodeActualEffortTo() { return i18n( "Actual effort" ); }
    QString ToolTip::nodeRemainingEffort() { return i18n( "Remaining effort" ); }
    QString ToolTip::nodePlannedCostTo() { return i18n( "Planned cost" ); }
    QString ToolTip::nodeActualCostTo() { return i18n( "Actual cost" ); }
    QString ToolTip::completionStartedTime() { return i18n( "Time when task was actually started" ); }
    QString ToolTip::completionStarted() { return i18n( "Shows if the task is started" ); }
    QString ToolTip::completionFinishedTime() { return i18n( "Time when task was actually finished" ); }
    QString ToolTip::completionFinished() { return i18n( "Shows if the task is finished" ); }
    QString ToolTip::completionStatusNote() { return i18n( "Status Note" ); }
    
    QString ToolTip::estimateExpected() { return i18n( "Calculated expected estimate" ); }
    QString ToolTip::estimateVariance() { return i18n( "Calculated estimate variance" ); }
    QString ToolTip::estimateOptimistic() { return i18n( "Optimistic estimate" ); }
    QString ToolTip::estimatePessimistic() { return i18n( "Pessimistic estimate" ); }
    QString ToolTip::estimateType() { return i18n( "Type of estimate" ); }
    QString ToolTip::estimateCalendar() { return i18n( "The calendar used when estimate type is Duration" ); }
    QString ToolTip::estimate() { return i18n( "The most likely estimate" ); }
    QString ToolTip::optimisticRatio() { return i18n( "Optimistic estimate" ); }
    QString ToolTip::pessimisticRatio() { return i18n( "Pessimistic estimate" ); }
    QString ToolTip::riskType() { return i18n( "Type of risk" ); }

    QString ToolTip::nodeNotScheduled() { return i18n( "The node has not been scheduled" ); }
    QString ToolTip::nodeAssignmentMissing() { return i18n( "An effort has been estimated, but no resource has been assigned" ); }
    QString ToolTip::nodeResourceOverbooked() { return i18n( "A resource assigned to this task is overbooked" ); }
    QString ToolTip::nodeResourceUnavailable() { return i18n( "A resource assigned to this task is not available" ); }
    QString ToolTip::nodeConstraintsError() { return i18n( "A timing constraint could not be met" ); }
    QString ToolTip::nodeEffortNotMet() { return i18n( "The assigned resource could not meet the estimated effort" ); }

    QString ToolTip::nodeBCWS() { return i18n( "Budgeted Cost of Work Scheduled" ); }
    QString ToolTip::nodeBCWP() { return i18n( "Budgeted Cost of Work Performed" ); }
    QString ToolTip::nodeACWP() { return i18n( "Actual Cost of Work Performed" ); }
    QString ToolTip::nodePerformanceIndex() { return i18n( "Schedule performance index (BCWP/BCWS)" ); }

    QString ToolTip::resourceName() { return i18n( "The name of the resource or resource group" ); }
    QString ToolTip::resourceType() { return i18n( "The type of the resource or resource group" ); }
    QString ToolTip::resourceInitials() { return i18n( "The initials of the resource" ); }
    QString ToolTip::resourceEMail() { return i18n( "The e-mail address of the resource" ); }
    QString ToolTip::resourceCalendar() { return i18n( "The calendar defines when the resource is working" ); }
    QString ToolTip::resourceUnits() { return i18n( "The maximum load that can be assigned" ); }
    QString ToolTip::resourceAvailableFrom() { return i18n( "Defines when the resource is available to the project" ); }
    QString ToolTip::resourceAvailableUntil() { return i18n( "Defines when the resource is available to the project" ); }
    QString ToolTip::resourceNormalRate() { return i18n( "The cost pr hour, normal hours" ); }
    QString ToolTip::resourceOvertimeRate() { return i18n( "The cost pr hour, overtime hours" ); }
    QString ToolTip::resourceFixedCost() { return i18n( "The fixed cost" ); }

    QString ToolTip::accountName() { return i18n( "The name of the account" ); }
    QString ToolTip::accountDescription() { return i18n( "The description of the account" ); }

    QString ToolTip::scheduleName() { return i18n( "The name of the schedule" ); }
    QString ToolTip::scheduleState() { return i18n( "The schedules state" ); }

    QString ToolTip::scheduleOverbooking() { return i18n( "Controles resource overbooking when scheduling" ); }
    QString WhatsThis::scheduleOverbooking() { return i18nc( "@info:whatsthis", 
            "<para>Controles resource overbooking when scheduling.</para>"
            "<para>If overbooking is allowed, a resource may be booked (on working days) to work more than it is available. This can happen if the resource is allocated to multiple tasks or are booked on other projects.</para>"
            "<para>If overbooking is to be avoided, resources will not be booked more than they are availabile. On resource conflict, tasks will be delayed until the resource is available.</para>"
            );
    }

    QString ToolTip::scheduleDistribution() { return i18n( "The distribution to be used during scheduling" ); }
    QString WhatsThis::scheduleDistribution() { return i18nc( "@info:whatsthis",
        "<para>The distribution to be used during scheduling</para>"
        "<para>If distribution is 'None', the tasks estimate is used as is during scheduling.</para>"
        "<para>If distribution is 'PERT', the estimate used is calculated based on the entered optimistic- (O), pessimistic-(P) and most likely (M) estimate. The formulay used for this is (O + 4 * M + P) / 6.<para>"
        );
    }

    QString ToolTip::scheduleCalculate() { return i18n( "Defines the schedules to be calculated" ); }
    QString ToolTip::scheduleStart() { return i18n( "The scheduled start time" ); }
    QString ToolTip::scheduleFinish() { return i18n( "The scheduled finish time" ); }
    QString ToolTip::schedulingDirection() { return i18n( "The scheduling direction" ); }
    QString WhatsThis::schedulingDirection() { return i18nc( "@info:whatsthis",
        "<para>The scheduling direction.</para>"
        "<para>If direction is Forward, the project is scheduled starting at the projects earliest start time specified in the main project dialog.</para>"
        "<para>If direction is Backward, the project is scheduled starting at the projects latest finish time specified in the main project dialog.</para>"
        );
    }

    QString ToolTip::scheduleScheduler() { return i18n( "The scheduler used for calculating the project schedule" ); }
    QString WhatsThis::scheduleScheduler() { return i18nc( "@info:whatsthis", 
        "<para>The scheduler used for calculating the project schedule.</para>"
        "<para>The default built-in scheduler is the Network Scheduler.</para>"
        "<para>Other schedulers presently available is RCPS if libRCPS is installed on your system. "
        "RCPS is a genetics based resource constrained project scheduler.</para>"
        );
    }

    QString ToolTip::documentUrl() { return i18n( "The url of the document" ); }
    QString ToolTip::documentType() { return i18n( "The type of the document" ); }
    QString ToolTip::documentStatus() { return i18n( "The status of the document" ); }
    QString ToolTip::documentSendAs() { return i18n( "Defines how this document is send" ); }

    QString ToolTip::calendarName() { return i18n( "The name of the calendar" ); }
    QString ToolTip::calendarTimeZone() { return i18n( "The timezone of the calendar" ); }

    QString ToolTip::relationParent() { return i18n( "The name of the required task" ); }
    QString ToolTip::relationChild() { return i18n( "The name of the dependent task" ); }
    QString ToolTip::relationType() { return i18n( "The type of relation" ); }
    QString ToolTip::relationLag() { return i18n( "The relations time lag" ); }


} //namespace KPlato

