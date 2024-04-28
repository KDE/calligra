/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2008 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "Plugin.h"
#include "KPrSlideWipeEffectFactory.h"
#include <KPluginFactory>
#include <KPrPageEffectRegistry.h>

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "kpr_pageeffect_slidewipe.json", registerPlugin<Plugin>();)

Plugin::Plugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KPrPageEffectRegistry::instance()->add(new KPrSlideWipeEffectFactory());
}

#include "Plugin.moc"
