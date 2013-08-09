/*  This file is part of the KDE project
    Copyright (C) 2007 Bernhard Loos <nhuh.put@web.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
#ifndef KPLUGINLOADER_H
#define KPLUGINLOADER_H

#include <kexportplugin.h>

#include <QtCore/QPluginLoader>
#include <QtCore/QtPlugin>

class KPluginFactory;
class KService;

class KPluginLoaderPrivate;

/**
 * \class KPluginLoader kpluginloader.h <KPluginLoader>
 *
 * This class can be used to dynamically load a plugin library at runtime.
 *
 * This class makes sure that the Qt and KDE versions used to compile this library aren't newer than
 * the ones currently used.
 *
 *
 * This class is reentrant, you can load plugins from different threads. You can also have multiple
 * PluginLoaders for one library without negative effects.
 * The object obtained with factory() or the inherited method QPluginLoader::instance() is
 * cached inside the library. If you call factory() or instance() multiple times, you will always get
 * the same object, even from different threads and different KPluginLoader instances.
 * You can delete this object easily, a new one will be created if factory() or instance() is called
 * afterwards. factory() uses instance() internally.
 *
 * KPluginLoader inherits QPluginLoader::unload(). It is safe to call this method if you loaded a plugin
 * and decide not to use it for some reason. But as soon as you start to use the factory from the plugin,
 * you should stay away from it. It's nearly impossible to keep track of all objects created directly or
 * indirectly from the plugin and all other pointers into plugin code. Using unload() in this case is asking
 * for trouble. If you really need to unload your plugins, you have to take care to convert the clipboard
 * content to text, because the plugin could have registered a custom mime source. You also have to delete
 * the factory of the plugin, otherwise you will create a leak.
 * The destructor of KPluginLoader doesn't call unload.
 *
 * Sample code:
 * \code
 *  KPluginLoader loader( ...library or kservice... );
 *  KPluginFactory* factory = loader.factory();
 *  if (!factory) {
 *      kWarning() << "Error loading plugin:" << loader.errorString();
 *  } else {
 *      MyInterface* obj = factory->create<MyInterface>();
 *      if (!obj) {
 *          kWarning() << "Error creating object";
 *      }
 *  }
 * \endcode
 *
 * \see KPluginFactory
 *
 * \author Bernhard Loos <nhuh.put@web.de>
 */
class KSERVICE_EXPORT KPluginLoader : public QPluginLoader
{
    Q_OBJECT
public:
    /**
     * Used this constructor to load a plugin with a given library name. Plugin libraries shouldn't have a 'lib' prefix.
     *
     * errorString() will be set if problems are encountered.
     *
     * \param plugin The name of the plugin library.
     * \param parent A parent object.
     */
    explicit KPluginLoader(const QString &plugin, QObject *parent = 0);

    /**
     * Used this constructor to load a plugin from a service. The service must contain a library.
     *
     * errorString() will be set if problems are encountered.
     *
     * \param service The service for which the library should be loaded.
     * \param parent A parent object.
     */
    explicit KPluginLoader(const KService &service, QObject *parent = 0);

    /**
     * Destroys the plugin loader.
     */
    ~KPluginLoader();

    /**
     * Used to obtain the factory object of the plugin. The returned object is usually created
     * from a K_PLUGIN_FACTORY invokation.
     *
     * \returns The factory of the plugin or 0 on error.
     */
    KPluginFactory *factory();

    /**
     * The name of this plugin as given to the constructor.
     * \returns the plugin name
     */
    QString pluginName() const;

    /**
     * Queries the plugin version.
     * \returns The version given to K_EXPORT_PLUGIN_VERSION or (quint32) -1 if not set.
     */
    quint32 pluginVersion() const;

    /**
     * Queries the last error.
     * \returns The description of the last error.
     */
    QString errorString() const;

    bool isLoaded() const;

protected:
    /**
     * Performs the loading of the plugin.
     */
    bool load();
private:
    Q_DECLARE_PRIVATE(KPluginLoader)
    Q_DISABLE_COPY(KPluginLoader)

    using QPluginLoader::setFileName;

    KPluginLoaderPrivate *const d_ptr;
};


#endif
