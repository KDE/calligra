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

#include "ToolRegistry.h"

#include "CellTool.h"
#include "CellToolFactory.h"

#include <KSharedConfig>
#include <KConfigGroup>
#include <KPluginFactory>
#include <QGlobalStatic>

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
