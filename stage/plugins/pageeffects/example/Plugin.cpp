/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2008 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "Plugin.h"
#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "kpr_pageeffect_example.json", registerPlugin<Plugin>();)

Plugin::Plugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
}

#include "Plugin.moc"
