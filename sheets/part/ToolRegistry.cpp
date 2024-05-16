/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ToolRegistry.h"

#include "CellToolFactory.h"

#include <engine/SheetsDebug.h>

#include <KConfigGroup>
#include <KPluginFactory>
#include <KSharedConfig>

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

ToolRegistry *ToolRegistry::instance()
{
    return s_instance;
}

void ToolRegistry::loadTools()
{
    const auto metaDatas = KoPluginLoader::pluginLoaders(QStringLiteral("calligrasheets/tools"));
    debugSheetsFormula << metaDatas.count() << "tools found.";

    const KConfigGroup pluginsConfigGroup = KSharedConfig::openConfig()->group("Plugins");
    for (const auto &metaData : metaDatas) {
        QJsonObject data = metaData.rawData();
        int version = metaData.value("X-CalligraSheets-InterfaceVersion").toInt();
        if (version != 0) {
            debugSheetsFormula << "Skipping" << metaData.fileName() << ", because interface version is" << version;
            continue;
        }
        const QString category = metaData.category();
        if (category != "Tool") {
            debugSheetsFormula << "Skipping" << metaData.fileName() << ", because category is " << category;
            continue;
        }

        const auto result = KPluginFactory::instantiatePlugin<QObject>(metaData, this);
        if (!result.plugin) {
            debugSheetsFormula << "Unable to create tool factory for" << metaData.fileName();
            continue;
        }
        const auto toolFactory = dynamic_cast<CellToolFactory *>(result.plugin);
        if (!toolFactory) {
            debugSheetsFormula << "Unable to create tool factory for" << metaData.fileName();
            continue;
        }
        const QString pluginConfigEnableKey = metaData.pluginId() + QLatin1String("Enabled");
        const bool isPluginEnabled =
            pluginsConfigGroup.hasKey(pluginConfigEnableKey) ? pluginsConfigGroup.readEntry(pluginConfigEnableKey, true) : metaData.isEnabledByDefault();

        if (isPluginEnabled) {
            // Tool already registered?
            if (KoToolRegistry::instance()->contains(toolFactory->id())) {
                continue;
            }

            toolFactory->setIconName(metaData.iconName());
            toolFactory->setPriority(10);
            toolFactory->setToolTip(metaData.description());
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
}
