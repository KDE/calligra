/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoToolRegistry.h"

#include <FlakeDebug.h>
#include <KConfigGroup>
#include <KSharedConfig>

#include "KoToolManager.h"
#include "tools/KoCreateShapesTool.h"
#include "tools/KoCreateShapesToolFactory.h"
#include "tools/KoPanTool.h"
#include "tools/KoPanToolFactory.h"
#include "tools/KoPathToolFactory.h"
#include "tools/KoZoomTool.h"
#include "tools/KoZoomToolFactory.h"
#include <KoPluginLoader.h>

#include <QGlobalStatic>

Q_GLOBAL_STATIC(KoToolRegistry, s_instance)

KoToolRegistry::KoToolRegistry()
    : d(nullptr)
{
}

void KoToolRegistry::init()
{
    KoPluginLoader::PluginsConfig config;
    config.group = "calligra";
    config.whiteList = "ToolPlugins";
    config.blacklist = "ToolPluginsDisabled";
    KoPluginLoader::load(QStringLiteral("calligra/tools"), config);

    // register generic tools
    add(new KoCreateShapesToolFactory());
    add(new KoPathToolFactory());
    add(new KoZoomToolFactory());
    add(new KoPanToolFactory());

    KConfigGroup cfg = KSharedConfig::openConfig()->group("calligra");
    QStringList toolsBlacklist = cfg.readEntry("ToolsBlacklist", QStringList());
    foreach (const QString &toolID, toolsBlacklist) {
        delete value(toolID);
        remove(toolID);
    }
}

KoToolRegistry::~KoToolRegistry()
{
    qDeleteAll(doubleEntries());
    qDeleteAll(values());
}

KoToolRegistry *KoToolRegistry::instance()
{
    if (!s_instance.exists()) {
        s_instance->init();
    }
    return s_instance;
}

void KoToolRegistry::addDeferred(KoToolFactoryBase *toolFactory)
{
    add(toolFactory);
    KoToolManager::instance()->addDeferredToolFactory(toolFactory);
}
