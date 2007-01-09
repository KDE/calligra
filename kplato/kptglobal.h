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
        DurationValue,
        DurationScales,
        Maximum,
        Minimum
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

    static QString EstimateType = i18n( "Type of estimate." );
    static QString Estimate = i18n( "The expected estimate." );
    static QString OptimisticRatio = i18n( "Optimistic estimate." );
    static QString PessimisticRatio = i18n( "Pessimistic estimate." );

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

} //namespace ToolTip


} //namespace KPlato

#endif
