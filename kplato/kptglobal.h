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

namespace KPlato
{

namespace Role
{
    enum Roles {
        EnumList = Qt::UserRole + 1,
        EnumListValue = Qt::UserRole + 2,
        List = Qt::UserRole + 3,
        ListValues = Qt::UserRole + 4
    };
} //namespace Role

namespace ToolTip
{
    static QString NodeName = "The name of the task.";
    static QString NodeType = "Task type.";
    static QString NodeResponsible = "The responsible person for this task.";
    static QString NodeConstraint = "The scheduling constraint type.";
    static QString NodeConstraintStart = "Constraint start time.";
    static QString NodeConstraintEnd = "Constraint end time.";
    static QString NodeEstimateType = "Type of estimate.";
    static QString NodeDescription = "Task notes.";
    static QString NodeWBS = "Work Breakdown Structure Code";
    static QString NodeRisk = "Risk controles the PERT distribution used when calculating the actual estimate for this task.";

} //namespace ToolTip


} //namespace KPlato

#endif
