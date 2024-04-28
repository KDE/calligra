/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "Plugin.h"

#include "sound/KPrSoundEventActionFactory.h"
#include <KPluginFactory>
#include <KoEventActionRegistry.h>

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "calligrastageeventactions.json", registerPlugin<Plugin>();)

Plugin::Plugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoEventActionRegistry::instance()->addPresentationEventAction(new KPrSoundEventActionFactory());
}

#include "Plugin.moc"
