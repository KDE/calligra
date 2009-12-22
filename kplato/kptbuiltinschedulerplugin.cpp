/* This file is part of the KDE project
  Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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
  Boston, MA 02110-1301, USA.
*/

#include "kptbuiltinschedulerplugin.h"

#include "kptproject.h"
#include "kptschedule.h"
 
namespace KPlato
{

BuiltinSchedulerPlugin::BuiltinSchedulerPlugin(QObject *parent)
    : SchedulerPlugin(parent)
{
    setName( i18n( "Network Scheduler" ) );
    setComment( i18n( "Built-in network based scheduler" ) );
}
 
BuiltinSchedulerPlugin::~BuiltinSchedulerPlugin()
{
}

void BuiltinSchedulerPlugin::calculate( Project &project, ScheduleManager *sm, bool )
{
    kDebug();
    project.calculate( *sm );
}

} //namespace KPlato

