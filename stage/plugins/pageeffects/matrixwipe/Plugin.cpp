/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2008 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "Plugin.h"
#include <KPluginFactory>
#include <KPrPageEffectRegistry.h>

#include "boxsnakes/KPrBoxSnakesWipeEffectFactory.h"
#include "parallelsnakes/KPrParallelSnakesWipeEffectFactory.h"
#include "snakewipe/KPrSnakeWipeEffectFactory.h"
#include "spiralwipe/KPrSpiralWipeEffectFactory.h"
#include "waterfallwipe/KPrWaterfallWipeEffectFactory.h"

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "kpr_pageeffect_matrixwipe.json", registerPlugin<Plugin>();)

Plugin::Plugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KPrPageEffectRegistry::instance()->add(new KPrSnakeWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrSpiralWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrParallelSnakesWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrBoxSnakesWipeEffectFactory());
    KPrPageEffectRegistry::instance()->add(new KPrWaterfallWipeEffectFactory());
}

#include "Plugin.moc"
