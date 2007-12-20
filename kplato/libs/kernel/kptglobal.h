/* This file is part of the KDE project
  Copyright (C) 2006, 2007 Dag Andersen <danders@get2net.dk>

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

#ifndef KPLATO_GLOBAL_H
#define KPLATO_GLOBAL_H

#include "kplatokernel_export.h"

#include <QtCore>

#include <klocale.h>

// This is used both for *.kplato and *.kplatowork files as the syntax is identical
#define XML_FILE_SYNTAX_VERSION "0.6.2"

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

struct KPLATOKERNEL_EXPORT ToolTip
{
    static QString nodeName();
    static QString nodeType();
    static QString nodeResponsible();
    static QString allocation();
    static QString nodeConstraint();
    static QString nodeConstraintStart();
    static QString nodeConstraintEnd();
    static QString nodeDescription();
    static QString nodeWBS();
    static QString nodeRisk();
    static QString nodeRunningAccount();
    static QString nodeStartupAccount();
    static QString nodeStartupCost();
    static QString nodeShutdownAccount();
    static QString nodeShutdownCost();

    static QString nodeStartTime();
    static QString nodeEndTime();
    static QString nodeEarlyStart();
    static QString nodeEarlyFinish();
    static QString nodeLateStart();
    static QString nodeLateFinish();
    
    static QString nodeDuration();
    static QString nodeVarianceDuration();
    static QString nodeOptimisticDuration();
    static QString nodePessimisticDuration();

    static QString nodePositiveFloat();
    static QString nodeNegativeFloat();
    static QString nodeFreeFloat();
    static QString nodeStartFloat();
    static QString nodeFinishFloat();
    static QString nodeAssignment();

    static QString nodeStatus();
    static QString nodeCompletion();
    static QString nodePlannedEffortTo();
    static QString nodeActualEffortTo();
    static QString nodeRemainingEffort();
    static QString nodePlannedCostTo();
    static QString nodeActualCostTo();
    static QString completionStartedTime();
    static QString completionFinishedTime();
    static QString completionStatusNote();
    
    static QString estimateExpected();
    static QString estimateVariance();
    static QString estimateOptimistic();
    static QString estimatePessimistic();
    static QString estimateType();
    static QString estimate();
    static QString optimisticRatio();
    static QString pessimisticRatio();
    static QString riskType();

    static QString nodeNotScheduled();
    static QString nodeAssigmentMissing();
    static QString nodeResourceOverbooked();
    static QString nodeResourceUnavailable();
    static QString nodeConstraintsError();
    static QString nodeEffortNotMet();

    static QString resourceName();
    static QString resourceType();
    static QString resourceInitials();
    static QString resourceEMail();
    static QString resourceCalendar();
    static QString resourceUnits();
    static QString resourceAvailableFrom();
    static QString resourceAvailableUntil();
    static QString resourceNormalRate();
    static QString resourceOvertimeRate();
    static QString resourceFixedCost();

    static QString accountName();
    static QString accountDescription();

    static QString scheduleName();
    static QString scheduleState();
    static QString scheduleOverbooking();
    static QString scheduleDistribution();
    static QString scheduleCalculate();
    static QString scheduleStart();
    static QString scheduleFinish();

    static QString documentUrl();
    static QString documentType();
    static QString documentStatus();
    static QString documentSendAs();

    static QString calendarName();
    static QString calendarTimeZone();

    static QString relationParent();
    static QString relationChild();
    static QString relationType();
    static QString relationLag();

}; //namespace ToolTip

struct KPLATOKERNEL_EXPORT WhatsThis
{
    static QString  nodeNegativeFloat();
    static QString  nodeFreeFloat();
    static QString  nodeStartFloat();
    static QString  nodeFinishFloat();

}; //namespace WhatsThis


} //namespace KPlato

#endif
