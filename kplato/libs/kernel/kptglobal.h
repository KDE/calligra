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


#include <klocale.h>

// The KPlato file syntax is used in parts of the KPlatoWork file, so:
// * If you change KPLATO_FILE_SYNTAX_VERSION, change KPLATOWORK_FILE_SYNTAX_VERSION too!
// * You don't need to change KPLATO_FILE_SYNTAX_VERSION when you change KPLATOWORK_FILE_SYNTAX_VERSION
#define KPLATO_FILE_SYNTAX_VERSION "0.6.5"
#define KPLATOWORK_FILE_SYNTAX_VERSION "0.6.5"

#define CURRENTSCHEDULE     -1
#define NOTSCHEDULED        -2
#define BASELINESCHEDULE    -3
#define ANYSCHEDULED        -4

namespace KPlato
{

enum ObjectType {
    OT_None = 0,
    OT_Project,
    OT_Task,
    OT_Summarytask,
    OT_ResourceGroup,
    OT_Resource,
    OT_Appointment,
    OT_External,
    OT_Interval,
    OT_ScheduleManager,
    OT_Schedule,
    OT_Calendar,
    OT_CalendarWeek,
    OT_CalendarDay
};

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
        EditorType,
        ReadWrite,
        ObjectType,
        InternalAppointments,
        ExternalAppointments
    };
} //namespace Role


struct KPLATOKERNEL_EXPORT SchedulingState
{
    static QString deleted();
    static QString notScheduled();
    static QString scheduled();
    static QString resourceOverbooked();
    static QString resourceNotAvailable();
    static QString resourceNotAllocated();
    static QString constraintsNotMet();

}; //namespace WhatsThis


} //namespace KPlato

#endif
