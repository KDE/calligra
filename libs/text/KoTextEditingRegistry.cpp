/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTextEditingRegistry.h"

#include <KoPluginLoader.h>
#include <QGlobalStatic>

Q_GLOBAL_STATIC(KoTextEditingRegistry, s_instance)

void KoTextEditingRegistry::init()
{
    KoPluginLoader::PluginsConfig config;
    config.whiteList = "TextEditingPlugins";
    config.blacklist = "TextEditingPluginsDisabled";
    config.group = "calligra";
    KoPluginLoader::load(QStringLiteral("calligra/textediting"), config);
}

KoTextEditingRegistry *KoTextEditingRegistry::instance()
{
    if (!s_instance.exists()) {
        s_instance->init();
    }
    return s_instance;
}

KoTextEditingRegistry::~KoTextEditingRegistry()
{
    qDeleteAll(doubleEntries());
    qDeleteAll(values());
}
