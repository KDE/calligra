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

#include "SheetsDebug.h"
#include "Function.h"
#include "FunctionRepository.h"

#include <QGlobalStatic>
#include <KSharedConfig>

#ifndef SHEETS_NO_PLUGINMODULES
#include <kplugininfo.h>
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
    const QString fileName = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                    QStringLiteral("calligrasheets/functions/")+module->descriptionFileName());
    if (fileName.isEmpty()) {
        debugSheetsFormula << module->descriptionFileName() << "not found.";
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
    return s_instance;
}

void FunctionModuleRegistry::loadFunctionModules()
{
#ifndef SHEETS_NO_PLUGINMODULES
    const QList<QPluginLoader *> offers = KoPluginLoader::pluginLoaders(QStringLiteral("calligrasheets/functions"));
    debugSheetsFormula << offers.count() << "function modules found.";

    const KConfigGroup pluginsConfigGroup = KSharedConfig::openConfig()->group("Plugins");
    foreach (QPluginLoader *loader, offers) {

        QJsonObject metaData = loader->metaData().value("MetaData").toObject();
        int version = metaData.value("X-CalligraSheets-InterfaceVersion").toInt();
        if (version != 0) {
            debugSheetsFormula << "Skipping" << loader->fileName() << ", because interface version is" << version;
            continue;
        }
        QJsonObject pluginData = metaData.value("KPlugin").toObject();
        QString category = pluginData.value("Category").toString();
        if (category != "FunctionModule") {
            debugSheetsFormula << "Skipping" << loader->fileName() << ", because category is " << category;
            continue;
        }

        const QString pluginId = pluginData.value("Id").toString();
        const QString pluginConfigEnableKey = pluginId + QLatin1String("Enabled");
        const bool isPluginEnabled = pluginsConfigGroup.hasKey(pluginConfigEnableKey) ?
            pluginsConfigGroup.readEntry(pluginConfigEnableKey, true) :
            pluginData.value("EnabledByDefault").toBool(true);

        if (isPluginEnabled) {
            if(contains(pluginId)) {
                continue;
            }
            // Plugin enabled, but not registered. Add it.
            KPluginFactory* const factory = qobject_cast<KPluginFactory *>(loader->instance());
            if (!factory) {
                debugSheetsFormula << "Unable to create plugin factory for" << loader->fileName();
                continue;
            }
            FunctionModule* const module = qobject_cast<FunctionModule *>(factory->create());
            if (!module) {
                debugSheetsFormula << "Unable to create function module for" << loader->fileName();
                continue;
            }

            add(pluginId, module);
            debugSheetsFormula << "Loaded" << pluginId;

            // Delays the function registration until the user needs one.
            if (d->repositoryInitialized) {
                d->registerFunctionModule(module);
            }
        } else {
            if (!contains(pluginId)) {
                continue;
            }
            // Plugin disabled, but registered. Remove it.
            FunctionModule* const module = get(pluginId);
            // Delay the function registration until the user needs one.
            if (d->repositoryInitialized) {
                d->removeFunctionModule(module);
            }
            remove(pluginId);
            if (module->isRemovable()) {
                delete module;
                KPluginFactory* factory = qobject_cast<KPluginFactory *>(loader->instance());
                delete factory;
                loader->unload();
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
    qDeleteAll(offers);
#else
    QList<FunctionModule*> modules;
    QObject *parent = 0;

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

    Q_FOREACH(FunctionModule* module, modules) {
        add(module->id(), module);
        d->registerFunctionModule(module);
    }
#endif
}

void FunctionModuleRegistry::registerFunctions()
{
    d->repositoryInitialized = true;
    const QList<FunctionModule*> modules = values();
    for (int i = 0; i < modules.count(); ++i) {
        d->registerFunctionModule(modules[i]);
    }
}
