/* This file is part of the KDE project
 * Copyright (c) 2006 Boudewijn Rempt (boud@valdyas.org)
 * Copyright (c) 2016 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KO_PLUGIN_LOADER_H
#define KO_PLUGIN_LOADER_H

#include <QStringList>

#include "koplugin_export.h"

class QObject;
class QPluginLoader;
class KPluginFactory;

/**
 * The pluginloader singleton is responsible for loading the plugins
 * that it's asked to load. It keeps track of which servicetypes it
 * has seen and doesn't reload them. The plugins need to inherit
 * a QObject with a default constructor. Inside the default
 * constructor you can create whatever object you want and add it to
 * whatever registry you prefer. After having been constructed, your plugin
 * will be deleted, so do all you need in the constructor.  Things like
 * adding a factory to a registry make sense there.
 * Example header file;
@code
#include <QObject>

class MyPlugin : public QObject {
    Q_OBJECT
public:
    MyPlugin(QObject *parent, const QVariantList & );
    ~MyPlugin() {}
};
@endcode
 * Example cpp file;
@code
#include "MyPlugin.h"
#include <kpluginfactory.h>

K_PLUGIN_FACTORY_WITH_JSON(MyPluginFactory, "myplugin.json", registerPlugin<MyPlugin>();)

MyPlugin::MyPlugin( QObject *parent, const QVariantList& ) : QObject(parent) {
    // do stuff like creating a factory and adding it to the
    // registry instance.
}
#include <MyPlugin.moc>
@endcode
 */
namespace KoPluginLoader
{
    /**
     * Config object for load()
     * It is possible to limit which plugins will be loaded in the KConfig configuration file by
     * stating explicitly which plugins are wanted.
     */
    struct KOPLUGIN_EXPORT PluginsConfig {
        PluginsConfig() : group(0), whiteList(0), blacklist(0) {}
        /**
         * The properties are retrieved from the config using the following construct;
         * /code
         *  KConfigGroup configGroup =  KSharedConfig::openConfig()->group(config.group);
         * /endcode
         * For most cases you can pass the string "calligra" into this variable.
         */
        const char * group;
        /// This contains the variable name for the list of plugins (by library name) the user wants to load
        const char * whiteList;
        /// This contains the variable name for the list of plugins (by library name) that will not be loaded
        const char * blacklist;
        /// A registry can state it wants to load a default set of plugins instead of all plugins
        /// when the application starts the first time.  Append all such plugin (library) names to this list.
        QStringList defaults;
    };

    /**
     * Load all plugins that are located in the specified directory,
     * for instance:
     * KoPluginLoader::load(QStringLiteral("calligra/flakes"));
     * If you pass a PluginsConfig struct only those plugins are loaded that are specified in the
     * application config file.  New plugins found since last start will be automatically loaded.
     * @param directory The directory to search for plugins, as relative path.
     * All entries of QCoreApplication::libraryPaths() will be checked with @p directory appended as a
     * subdirectory.
     * @param config when passing a valid config only the wanted plugins are actually loaded
     * @return a list of services (by library name) that were not know in the config
     */
    KOPLUGIN_EXPORT void load(const QString & directory, const PluginsConfig &config = PluginsConfig(), QObject* owner = 0);

    /**
     * Load all plugins that are located in the specified directory and return their KPluginFactory objects.
     * @param directory The directory to search for plugins, as relative path.
     * All entries of QCoreApplication::libraryPaths() will be checked with @p directory appended as a
     * subdirectory.
     * @return a list of plugin factories from the found plugins matching the servicetype
     */
    KOPLUGIN_EXPORT QList<KPluginFactory *> instantiatePluginFactories(const QString & directory);

    /**
     * Creates and returns pluginLoaders for all plugins that are located in the specified directory.
     * @param directory The directory to search for plugins, as relative path.
     * All entries of QCoreApplication::libraryPaths() will be checked with @p directory appended as a
     * subdirectory.
     * @param mimeType The string used to identify the plugins.
     * @return a list of plugin loaders from the found plugins matching the mimetype, ownership is transferred to the caller
     */
    KOPLUGIN_EXPORT QList<QPluginLoader *> pluginLoaders(const QString &directory, const QString &mimeType = QString());
}

#endif // KO_PLUGIN_LOADER_H
