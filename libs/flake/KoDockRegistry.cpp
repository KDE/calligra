/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoDockRegistry.h"

#include <QDebug>
#include <QFontDatabase>
#include <QGlobalStatic>

#include <KConfigGroup>
#include <KSharedConfig>

#include "KoPluginLoader.h"

Q_GLOBAL_STATIC(KoDockRegistry, s_instance)

KoDockRegistry::KoDockRegistry()
    : d(nullptr)
{
}

void KoDockRegistry::init()
{
    KoPluginLoader::PluginsConfig config;
    config.whiteList = "DockerPlugins";
    config.blacklist = "DockerPluginsDisabled";
    config.group = "calligra";
    KoPluginLoader::load(QStringLiteral("calligra/dockers"), config);
}

KoDockRegistry::~KoDockRegistry()
{
    qDeleteAll(doubleEntries());
    qDeleteAll(values());
}

KoDockRegistry *KoDockRegistry::instance()
{
    if (!s_instance.exists()) {
        s_instance->init();
    }
    return s_instance;
}

QFont KoDockRegistry::dockFont()
{
    KConfigGroup group(KSharedConfig::openConfig(), "GUI");
    QFont dockWidgetFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    QFont smallFont = QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont);

    int pointSize = group.readEntry("palettefontsize", dockWidgetFont.pointSize());

    // Not set by the user
    if (pointSize == dockWidgetFont.pointSize()) {
        // and there is no setting for the smallest readable font, calculate something small
        if (smallFont.pointSize() >= pointSize) {
            smallFont.setPointSizeF(pointSize * 0.9);
        }
    } else {
        // paletteFontSize was set, use that
        smallFont.setPointSize(pointSize);
    }
    return smallFont;
}
