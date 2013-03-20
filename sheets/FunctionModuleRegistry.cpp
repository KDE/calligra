/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "FunctionModuleRegistry.h"

#include "Function.h"
#include "FunctionRepository.h"

#include <KoPluginLoader.h>

#include <KDebug>
#include <KGlobal>
#include <KStandardDirs>

using namespace Calligra::Sheets;

class FunctionModuleRegistry::Private
{
public:
    void registerFunctionModule(FunctionModule* module);
    void removeFunctionModule(FunctionModule* module);

public:
    bool repositoryInitialized;
};

void FunctionModuleRegistry::Private::registerFunctionModule(FunctionModule* module)
{
    const QList<QSharedPointer<Function> > functions = module->functions();
    for (int i = 0; i < functions.count(); ++i) {
        FunctionRepository::self()->add(functions[i]);
    }
    Q_ASSERT(!module->descriptionFileName().isEmpty());
    const KStandardDirs* dirs = KGlobal::activeComponent().dirs();
    const QString fileName = dirs->findResource("functions", module->descriptionFileName());
    if (fileName.isEmpty()) {
        kDebug(36002) << module->descriptionFileName() << "not found.";
        return;
    }
    FunctionRepository::self()->loadFunctionDescriptions(fileName);
}

void FunctionModuleRegistry::Private::removeFunctionModule(FunctionModule* module)
{
    const QList<QSharedPointer<Function> > functions = module->functions();
    for (int i = 0; i < functions.count(); ++i) {
        FunctionRepository::self()->remove(functions[i]);
    }
}


FunctionModuleRegistry::FunctionModuleRegistry()
        : d(new Private)
{
    d->repositoryInitialized = false;
}

FunctionModuleRegistry::~FunctionModuleRegistry()
{
    foreach(const QString &id, keys()) {
        get(id)->deleteLater();
    }
    qDeleteAll(doubleEntries());
    delete d;
}

FunctionModuleRegistry* FunctionModuleRegistry::instance()
{
    K_GLOBAL_STATIC(FunctionModuleRegistry, s_instance)
    return s_instance;
}

void FunctionModuleRegistry::loadFunctionModules()
{
    const quint32 minKSpreadVersion = CALLIGRA_MAKE_VERSION(2, 1, 0);

    QList<QObject*> functionPlugins = KoPluginLoader::instance()->retrievePlugins(0,
                                                                                  QLatin1String("CalligraSheets/Plugin"),
                                                                                  QLatin1String("([X-CalligraSheets-InterfaceVersion] == 0) and "
                                                                                                "([X-KDE-PluginInfo-Category] == 'FunctionModule')"));

    foreach(QObject *plugin, functionPlugins) {

        if ((quint32)plugin->property("plugin-loader:version").toInt() < minKSpreadVersion) {
            kDebug(36002) << plugin->property("plugin-info:name")
                          << "was built against Caligra Sheets" << plugin->property("plugin-loader:version")
                          << "; required version >=" << minKSpreadVersion;
            continue;

        }

        QString pluginName = plugin->property("plugin-info:name").toString();

        if (plugin->property("plugin-info:enabled").toBool()) {

            if (contains(pluginName)) {
                continue;
            }

            FunctionModule* const module = qobject_cast<FunctionModule*>(plugin);
            if (!module) {
                kDebug(36002) << "Unable to create function module for" << pluginName;
                continue;
            }

            add(pluginName, module);
            // Delays the function registration until the user needs one.
            if (d->repositoryInitialized) {
                d->registerFunctionModule(module);
            }
        }
        else {
            if (!contains(pluginName)) {
                continue;
            }
            // Plugin disabled, but registered. Remove it.
            FunctionModule* const module = get(pluginName);

            // Delay the function registration until the user needs one.
            if (d->repositoryInitialized) {
                d->removeFunctionModule(module);
            }

            remove(pluginName);

            if (module->isRemovable()) {
                delete module;
                delete plugin;
            }
            else {
                // Put it back in.
                add(pluginName, module);
                // Delay the function registration until the user needs one.
                if (d->repositoryInitialized) {
                    d->registerFunctionModule(module);
                }
            }
        }
    }
}

void FunctionModuleRegistry::registerFunctions()
{
    d->repositoryInitialized = true;
    const QList<FunctionModule*> modules = values();
    for (int i = 0; i < modules.count(); ++i) {
        d->registerFunctionModule(modules[i]);
    }
}
