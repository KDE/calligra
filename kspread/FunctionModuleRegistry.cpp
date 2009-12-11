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

public:
    bool repositoryInitialized;
};

void FunctionModuleRegistry::Private::registerFunctionModule(FunctionModule* module)
{
    module->registerFunctions();
    Q_ASSERT(!module->descriptionFileName().isEmpty());
    const KStandardDirs* dirs = Factory::global().dirs();
    const QString fileName = dirs->findResource("functions", module->descriptionFileName());
    if (fileName.isEmpty()) {
        kDebug(36002) << module->descriptionFileName() << "not found.";
    }
    FunctionRepository::self()->loadFunctionDescriptions(fileName);
}


FunctionModuleRegistry::FunctionModuleRegistry()
        : d(new Private)
{
    d->repositoryInitialized = false;
    loadFunctions();
}

FunctionModuleRegistry::~FunctionModuleRegistry()
{
    delete d;
}

FunctionModuleRegistry* FunctionModuleRegistry::instance()
{
    return &s_singleton->instance;
}

void FunctionModuleRegistry::loadFunctions()
{
    const QString serviceType = QString::fromLatin1("KSpread/Plugin");
    const QString query = QString::fromLatin1("([X-KSpread-Version] >= 2) and "
                          "([X-KDE-PluginInfo-Category] == 'FunctionModule')");
    const KService::List offers = KServiceTypeTrader::self()->query(serviceType, query);
    const KConfigGroup moduleGroup = KGlobal::config()->group("Plugins");
    const KPluginInfo::List pluginInfos = KPluginInfo::fromServices(offers, moduleGroup);
    foreach(KPluginInfo pluginInfo, pluginInfos) {
        KPluginFactory *factory = KPluginLoader(*pluginInfo.service()).factory();
        if (!factory) {
            kDebug(36002) << "Unable to create plugin factory for" << pluginInfo.name();
            continue;
        }
        FunctionModule* module = factory->create<FunctionModule>(this);
        if (!module) {
            kDebug(36002) << "Unable to create function module for" << pluginInfo.name();
            continue;
        }
        pluginInfo.load(); // load activation state
        if (pluginInfo.isPluginEnabled()) {
            // Module already registered?
            if (contains(module->id())) {
                continue;
            }
            add(module);
            // Is the function repository already initialized?
            if (d->repositoryInitialized) {
                d->registerFunctionModule(module);
            }
        } else {
            // Module not registered?
            if (!contains(module->id())) {
                continue;
            }
            module->removeFunctions();
            remove(module->id());
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
