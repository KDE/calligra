/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "Plugin.h"

#include <KoCreatePathToolFactory.h>
#include <KoPencilToolFactory.h>
#include <KoToolRegistry.h>

#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "calligra_tool_basicflakes.json", registerPlugin<Plugin>();)

Plugin::Plugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoToolRegistry::instance()->add(new KoCreatePathToolFactory());
    KoToolRegistry::instance()->add(new KoPencilToolFactory());
}

#include <Plugin.moc>
