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
#include "part/Factory.h" // FIXME detach from part
#include "FunctionModule.h"
#include "Functions.h"

#include <KDebug>
#include <KGlobal>
#include <KPluginInfo>
#include <KServiceTypeTrader>
#include <KStandardDirs>

using namespace KSpread;

class FunctionModuleRegistrySingleton
{
public:
    FunctionModuleRegistry instance;
};

K_GLOBAL_STATIC(FunctionModuleRegistrySingleton, s_singleton)


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
    const KStandardDirs* dirs = Factory::global().dirs();
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
    delete d;
}

FunctionModuleRegistry* FunctionModuleRegistry::instance()
{
    return &s_singleton->instance;
}

void FunctionModuleRegistry::loadFunctionModules()
{
    const quint32 minKSpreadVersion = KOFFICE_MAKE_VERSION(2, 1, 0);
    const QString serviceType = QLatin1String("KSpread/Plugin");
    const QString query = QLatin1String("([X-KSpread-InterfaceVersion] == 0) and "
                                        "([X-KDE-PluginInfo-Category] == 'FunctionModule')");
    const KService::List offers = KServiceTypeTrader::self()->query(serviceType, query);
    const KConfigGroup moduleGroup = KGlobal::config()->group("Plugins");
    const KPluginInfo::List pluginInfos = KPluginInfo::fromServices(offers, moduleGroup);
    kDebug(36002) << pluginInfos.count() << "function modules found.";
    foreach(KPluginInfo pluginInfo, pluginInfos) {
        pluginInfo.load(); // load activation state
        KPluginLoader loader(*pluginInfo.service());
        // Let's be paranoid: do not believe the service type.
        if (loader.pluginVersion() < minKSpreadVersion) {
            kDebug(36002) << pluginInfo.name()
                          << "was built against KSpread" << loader.pluginVersion()
                          << "; required version >=" << minKSpreadVersion;
            continue;
        }
        if (pluginInfo.isPluginEnabled() && !contains(pluginInfo.pluginName())) {
            // Plugin enabled, but not registered. Add it.
            KPluginFactory* const factory = loader.factory();
            if (!factory) {
                kDebug(36002) << "Unable to create plugin factory for" << pluginInfo.name();
                continue;
            }
            FunctionModule* const module = factory->create<FunctionModule>(this);
            if (!module) {
                kDebug(36002) << "Unable to create function module for" << pluginInfo.name();
                continue;
            }
            add(pluginInfo.pluginName(), module);

            // Delays the function registration until the user needs one.
            if (d->repositoryInitialized) {
                d->registerFunctionModule(module);
            }
        } else if (!pluginInfo.isPluginEnabled() && contains(pluginInfo.pluginName())) {
            // Plugin disabled, but registered. Remove it.
            FunctionModule* const module = get(pluginInfo.pluginName());
            // Delay the function registration until the user needs one.
            if (d->repositoryInitialized) {
                d->removeFunctionModule(module);
            }
            remove(pluginInfo.pluginName());
            if (module->isRemovable()) {
                delete module;
                delete loader.factory();
                loader.unload();
            } else {
                // Put it back in.
                add(pluginInfo.pluginName(), module);
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
