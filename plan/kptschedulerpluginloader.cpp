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

#include <KoJsonTrader.h>

#include <kdebug.h>

#include <QPluginLoader>


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
    const QList<QPluginLoader *> offers = KoJsonTrader::self()->query("Plan/SchedulerPlugin", QString());

    foreach(QPluginLoader *pluginLoader, offers) {
        KPluginFactory *factory = qobject_cast<KPluginFactory*>(pluginLoader->instance());
 
        if (!factory)
        {
            kError() << "KPluginFactory could not load the plugin:" << pluginLoader->fileName();
            continue;
        }
 
        SchedulerPlugin *plugin = factory->create<SchedulerPlugin>(this);
 
        if (plugin) {
            QJsonObject json = pluginLoader->metaData().value("MetaData").toObject();
            json = json.value("KPlugin").toObject();
            // QT5TODO: get localized variant of Name & Description
            const QString name = json.value("Name").toString();
            const QString comment = json.value("Description").toString();

            kDebug(planDbg()) << "Load plugin:" << name << ", " << comment;
            plugin->setName( name );
            plugin->setComment( comment );
            emit pluginLoaded( pluginLoader->fileName(), plugin);
        } else {
           kDebug(planDbg()) << "KPluginFactory could not create SchedulerPlugin:" << pluginLoader->fileName();
        }
    }
}

} //namespace KPlato
