/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "Plugin.h"
#include "KPrFadeEffectFactory.h"
#include <KPluginFactory>
#include <KPrPageEffectRegistry.h>

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "kpr_pageeffect_fade.json", registerPlugin<Plugin>();)

Plugin::Plugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KPrPageEffectRegistry::instance()->add(new KPrFadeEffectFactory());
}

#include "Plugin.moc"
