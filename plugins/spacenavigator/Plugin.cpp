/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "Plugin.h"
#include "SpaceNavigatorDevice.h"

#include <KoInputDeviceHandlerRegistry.h>

#include <kpluginfactory.h>

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "calligra_device_spacenavigator.json",
                           registerPlugin<Plugin>();)

Plugin::Plugin(QObject * parent, const QVariantList &)
    : QObject(parent)
{
    KoInputDeviceHandlerRegistry::instance()->add(new SpaceNavigatorDevice(parent));
}

#include <Plugin.moc>
