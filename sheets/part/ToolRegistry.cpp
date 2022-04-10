/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ToolRegistry.h"

#include "CellToolFactory.h"

#include <engine/SheetsDebug.h>

#include <KSharedConfig>
#include <KConfigGroup>
#include <KPluginFactory>

#include <KoPluginLoader.h>
#include <KoToolRegistry.h>

Q_GLOBAL_STATIC(Calligra::Sheets::ToolRegistry, s_instance)

using namespace Calligra::Sheets;


class Q_DECL_HIDDEN ToolRegistry::Private
{
public:
};


ToolRegistry::ToolRegistry()
        : d(new Private)
{
    // Add the built-in cell tool.
    KoToolRegistry::instance()->add(new CellToolFactory("KSpreadCellToolId"));
    // Load the tool plugins.
    loadTools();
}

ToolRegistry::~ToolRegistry()
{
    delete d;
}

ToolRegistry* ToolRegistry::instance()
{
    return s_instance;
}

void ToolRegistry::loadTools()
{
    const QList<QPluginLoader *> offers = KoPluginLoader::pluginLoaders(QStringLiteral("calligrasheets/tools"));
    debugSheetsFormula << offers.count() << "tools found.";

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
        if (category != "Tool") {
            debugSheetsFormula << "Skipping" << loader->fileName() << ", because category is " << category;
            continue;
        }

        KPluginFactory* factory = qobject_cast<KPluginFactory *>(loader->instance());
        if (!factory) {
            debugSheetsFormula << "Unable to create plugin factory for" << loader->fileName();
            continue;
        }
        QObject *object = factory->create<QObject>(this, QVariantList());
        CellToolFactory *toolFactory = dynamic_cast<CellToolFactory*>(object);
        if (!toolFactory) {
            debugSheetsFormula << "Unable to create tool factory for" << loader->fileName();
            continue;
        }
        const QString pluginConfigEnableKey = pluginData.value("Id").toString() + QLatin1String("Enabled");
        const bool isPluginEnabled = pluginsConfigGroup.hasKey(pluginConfigEnableKey) ?
            pluginsConfigGroup.readEntry(pluginConfigEnableKey, true) :
            pluginData.value("EnabledByDefault").toBool(true);

        if (isPluginEnabled) {
            // Tool already registered?
            if (KoToolRegistry::instance()->contains(toolFactory->id())) {
                continue;
            }

            toolFactory->setIconName(pluginData.value("Icon").toString());
            toolFactory->setPriority(10);
            toolFactory->setToolTip(pluginData.value("Description").toString());
            KoToolRegistry::instance()->add(toolFactory);
        } else {
           // Tool not registered?
           if (!KoToolRegistry::instance()->contains(toolFactory->id())) {
               continue;
           }
           delete KoToolRegistry::instance()->value(toolFactory->id());
           KoToolRegistry::instance()->remove(toolFactory->id());
        }
    }
    qDeleteAll(offers);
}
