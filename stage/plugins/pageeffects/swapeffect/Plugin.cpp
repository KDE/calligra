/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "Plugin.h"
#include "KPrSwapEffectFactory.h"
#include <KPluginFactory>
#include <KPrPageEffectRegistry.h>

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "kpr_pageeffect_swapeffect.json", registerPlugin<Plugin>();)

Plugin::Plugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KPrPageEffectRegistry::instance()->add(new KPrSwapEffectFactory());
}

#include "Plugin.moc"
