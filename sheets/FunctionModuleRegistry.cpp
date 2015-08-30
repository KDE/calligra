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

#include <kdebug.h>
#include <kglobal.h>
#include <KSharedConfig>
#include <kstandarddirs.h>
#include <kcomponentdata.h>

#ifndef SHEETS_NO_PLUGINMODULES
#include <kplugininfo.h>
#include <KPluginFactory>
#include "KoJsonTrader.h"
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
    const KStandardDirs* dirs = KGlobal::dirs();
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
#ifndef SHEETS_NO_PLUGINMODULES
    QList<QPluginLoader *> offers = KoJsonTrader::self()->query("CalligraSheets/Plugin", QString());
    kDebug(36002) << offers.count() << "function modules found.";
    foreach (QPluginLoader *loader, offers) {

        QJsonObject meta = loader->metaData().value("MetaData").toObject().value("KPlugin").toObject();
        int version = meta.value("X-CalligraSheets-InterfaceVersion").toInt();
        if (version != 0) {
            kDebug(36002) << "Skipping" << loader->fileName() << ", because interface version is" << version;
            continue;
        }
        QString category = meta.value("Category").toString();
        if (category != "FunctionModule") {
            kDebug(36002) << "Skipping" << loader->fileName() << ", because category is " << category;
            continue;
        }

        // TODO: the kde4 version supported enabling/disabling of plugins, do we want that?
        KPluginFactory* factory = qobject_cast<KPluginFactory *>(loader->instance());
        FunctionModule* module = qobject_cast<FunctionModule *>(factory->create());
        if (!module) {
            kDebug(36002) << "Unable to create function module for" << loader->fileName();
            continue;
        }
        QString name = meta.value("Name").toString();
        add(name, module);
        kDebug(36002) << "Loaded" << name;

        // Delays the function registration until the user needs one.
        if (d->repositoryInitialized) {
            d->registerFunctionModule(module);
        }
    }
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
