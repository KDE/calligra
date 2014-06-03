/* This file is part of the KDE project
  Copyright (C) 2009, 2012 Dag Andersen <danders@get2net.dk>

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


#include "kptschedulerpluginloader.h"
 
#include "kptschedulerplugin.h"
#include "kptdebug.h"

#include <kservicetypetrader.h>
#include <kdebug.h>


namespace KPlato
{

SchedulerPluginLoader::SchedulerPluginLoader(QObject * parent)
  : QObject(parent)
{
}
 
SchedulerPluginLoader::~SchedulerPluginLoader()
{
}
 
void SchedulerPluginLoader::loadAllPlugins()
{
    kDebug(planDbg()) << "Load all plugins";
    KService::List offers = KServiceTypeTrader::self()->query("Plan/SchedulerPlugin");
 
    KService::List::const_iterator iter;
    for(iter = offers.constBegin(); iter < offers.constEnd(); ++iter)
    {
        QString error;
        KService::Ptr service = *iter;
 
        KPluginFactory *factory = KPluginLoader(service->library()).factory();
 
        if (!factory)
        {
            kError() << "KPluginFactory could not load the plugin:" << service->library();
            continue;
        }
 
        SchedulerPlugin *plugin = factory->create<SchedulerPlugin>(this);
 
        if (plugin) {
            kDebug(planDbg()) << "Load plugin:" << service->name()<<", "<<service->comment();
            plugin->setName( service->name() );
            plugin->setComment( service->comment() );
            emit pluginLoaded( service->library(), plugin);
        } else {
           kDebug(planDbg()) << error;
        }
    }
}

} //namespace KPlato
