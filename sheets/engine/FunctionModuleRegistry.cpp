// This file is part of the KDE project
// SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "FunctionModuleRegistry.h"

#include "Function.h"
#include "FunctionRepository.h"
#include "SheetsDebug.h"

#include <KConfigGroup>
#include <KSharedConfig>

#ifndef SHEETS_NO_PLUGINMODULES
#include <KPluginFactory>
#include <KoPluginLoader.h>
#else
#include "functions/BitOpsModule.h"
#include "functions/ConversionModule.h"
#include "functions/DatabaseModule.h"
#include "functions/DateTimeModule.h"
#include "functions/EngineeringModule.h"
#include "functions/FinancialModule.h"
#include "functions/InformationModule.h"
#include "functions/LogicModule.h"
#include "functions/MathModule.h"
#include "functions/ReferenceModule.h"
#include "functions/StatisticalModule.h"
#include "functions/TextModule.h"
#include "functions/TrigonometryModule.h"
#endif

Q_GLOBAL_STATIC(Calligra::Sheets::FunctionModuleRegistry, s_instance)

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN FunctionModuleRegistry::Private
{
public:
    void registerFunctionModule(FunctionModule *module);
    void removeFunctionModule(FunctionModule *module);

public:
    bool repositoryInitialized;
};

void FunctionModuleRegistry::Private::registerFunctionModule(FunctionModule *module)
{
    const QList<QSharedPointer<Function>> functions = module->functions();
    for (int i = 0; i < functions.count(); ++i) {
        FunctionRepository::self()->add(functions[i]);
    }
    Q_ASSERT(!module->descriptionFileName().isEmpty());
    const QString fileName =
        QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("calligrasheets/functions/") + module->descriptionFileName());
    if (fileName.isEmpty()) {
        debugSheetsFormula << module->descriptionFileName() << "not found.";
        return;
    }
    FunctionRepository::self()->loadFunctionDescriptions(fileName);
}

void FunctionModuleRegistry::Private::removeFunctionModule(FunctionModule *module)
{
    const QList<QSharedPointer<Function>> functions = module->functions();
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
    for (const QString &id : keys()) {
        get(id)->deleteLater();
    }
    qDeleteAll(doubleEntries());
    delete d;
}

FunctionModuleRegistry *FunctionModuleRegistry::instance()
{
    return s_instance;
}

void FunctionModuleRegistry::loadFunctionModules()
{
#ifndef SHEETS_NO_PLUGINMODULES
    const auto metaData = KoPluginLoader::pluginLoaders(QStringLiteral("calligrasheets/functions"));
    debugSheetsFormula << metaData.count() << "function modules found.";

    const KConfigGroup pluginsConfigGroup = KSharedConfig::openConfig()->group("Plugins");
    for (KPluginMetaData metaData : metaData) {
        int version = metaData.rawData().value("X-CalligraSheets-InterfaceVersion").toInt();
        if (version != 0) {
            debugSheetsFormula << "Skipping" << metaData.fileName() << ", because interface version is" << version;
            continue;
        }
        QString category = metaData.category();
        if (category != "FunctionModule") {
            debugSheetsFormula << "Skipping" << metaData.fileName() << ", because category is " << category;
            continue;
        }

        const QString pluginId = metaData.pluginId();
        const QString pluginConfigEnableKey = pluginId + QLatin1String("Enabled");
        const bool isPluginEnabled =
            pluginsConfigGroup.hasKey(pluginConfigEnableKey) ? pluginsConfigGroup.readEntry(pluginConfigEnableKey, true) : metaData.isEnabledByDefault();

        if (isPluginEnabled) {
            if (contains(pluginId)) {
                continue;
            }
            // Plugin enabled, but not registered. Add it.
            auto result = KPluginFactory::instantiatePlugin<FunctionModule>(metaData, nullptr, {});
            if (!result.plugin) {
                debugSheetsFormula << "Unable to create plugin for" << metaData.fileName();
                continue;
            }

            add(pluginId, result.plugin);
            debugSheetsFormula << "Loaded" << pluginId;

            // Delays the function registration until the user needs one.
            if (d->repositoryInitialized) {
                d->registerFunctionModule(result.plugin);
            }
        } else {
            if (!contains(pluginId)) {
                continue;
            }
            // Plugin disabled, but registered. Remove it.
            FunctionModule *const module = get(pluginId);
            // Delay the function registration until the user needs one.
            if (d->repositoryInitialized) {
                d->removeFunctionModule(module);
            }
            remove(pluginId);
            if (module->isRemovable()) {
                delete module;
                auto result = KPluginFactory::loadFactory(metaData);
                if (result.plugin) {
                    delete result.plugin;
                }
            } else {
                // Put it back in.
                add(pluginId, module);
                // Delay the function registration until the user needs one.
                if (d->repositoryInitialized) {
                    d->registerFunctionModule(module);
                }
            }
        }
    }

#else
    QList<FunctionModule *> modules;
    QObject *parent = nullptr;

    modules << new BitOpsModule(parent);
    modules << new ConversionModule(parent);
    modules << new DatabaseModule(parent);
    modules << new DateTimeModule(parent);
    modules << new EngineeringModule(parent);
    modules << new FinancialModule(parent);
    modules << new InformationModule(parent);
    modules << new LogicModule(parent);
    modules << new MathModule(parent);
    modules << new ReferenceModule(parent);
    modules << new StatisticalModule(parent);
    modules << new TextModule(parent);
    modules << new TrigonometryModule(parent);

    Q_FOREACH (FunctionModule *module, modules) {
        add(module->id(), module);
        d->registerFunctionModule(module);
    }
#endif
}

void FunctionModuleRegistry::registerFunctions()
{
    d->repositoryInitialized = true;
    const QList<FunctionModule *> modules = values();
    for (int i = 0; i < modules.count(); ++i) {
        d->registerFunctionModule(modules[i]);
    }
}
