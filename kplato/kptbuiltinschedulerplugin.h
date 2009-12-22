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

#ifndef KPTBUILTINSCHEDULERPLUGIN_H
#define KPTBUILTINSCHEDULERPLUGIN_H

#include "kplato_export.h"
#include "kptschedulerplugin.h"


namespace KPlato
{

class Project;
class ScheduleManager;
 
class KPLATO_EXPORT BuiltinSchedulerPlugin : public SchedulerPlugin
{
    Q_OBJECT
public:
    BuiltinSchedulerPlugin(QObject *parent);
    virtual ~BuiltinSchedulerPlugin();

    /// Calculate the project
    virtual void calculate( Project &project, ScheduleManager *sm, bool nothread = false );
};

} //namespace KPlato

#endif
