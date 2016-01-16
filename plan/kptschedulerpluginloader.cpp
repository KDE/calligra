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

#include <KoPluginLoader.h>

#include <QPluginLoader>
#include <QLocale>


namespace KPlato
{

SchedulerPluginLoader::SchedulerPluginLoader(QObject * parent)
  : QObject(parent)
{
}
 
SchedulerPluginLoader::~SchedulerPluginLoader()
{
}

static
QJsonValue readLocalValue(const QJsonObject &json, const QString &key)
{
    // start with language_country
    const QString localeName = QLocale().name();

    QString localKey = key + QLatin1Char('[') + localeName + QLatin1Char(']');
    QJsonObject::ConstIterator it = json.constFind(localKey);
    if (it != json.constEnd()) {
        return it.value();
    }

    // drop _country
    const int separatorIndex = localeName.indexOf(QLatin1Char('_'));
    if (separatorIndex != -1) {
        const int localKeySeparatorIndex = key.length() + 1 + separatorIndex;
        localKey[localKeySeparatorIndex] = QLatin1Char(']');
        localKey.truncate(localKeySeparatorIndex + 1);
       it = json.constFind(localKey);
        if (it != json.constEnd()) {
            return it.value();
        }
    }

    // default to unlocalized value
    return json.value(key);
}


void SchedulerPluginLoader::loadAllPlugins()
{
    debugPlan << "Load all plugins";
    const QList<QPluginLoader *> offers = KoPluginLoader::pluginLoaders(QStringLiteral("Plan/SchedulerPlugin"));

    foreach(QPluginLoader *pluginLoader, offers) {
        KPluginFactory *factory = qobject_cast<KPluginFactory*>(pluginLoader->instance());
 
        if (!factory)
        {
            errorPlan << "KPluginFactory could not load the plugin:" << pluginLoader->fileName();
            continue;
        }
 
        SchedulerPlugin *plugin = factory->create<SchedulerPlugin>(this);
 
        if (plugin) {
            QJsonObject json = pluginLoader->metaData().value("MetaData").toObject();
            json = json.value("KPlugin").toObject();
            const QString name = readLocalValue(json, QLatin1String("Name")).toString();
            const QString comment = readLocalValue(json, QLatin1String("Description")).toString();

            debugPlan << "Load plugin:" << name << ", " << comment;
            plugin->setName( name );
            plugin->setComment( comment );
            emit pluginLoaded( pluginLoader->fileName(), plugin);
        } else {
           debugPlan << "KPluginFactory could not create SchedulerPlugin:" << pluginLoader->fileName();
        }
    }
}

} //namespace KPlato
