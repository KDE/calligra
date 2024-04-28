/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Vidhyapria Arunkumar <vidhyapria.arunkumar@nokia.com>
 * SPDX-FileCopyrightText: 2010 Amit Aggarwal <amit.5.aggarwal@nokia.com>

 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "Plugin.h"
#include "PluginShapeFactory.h"

#include <KoShapeRegistry.h>

#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "calligra_shape_plugin.json", registerPlugin<Plugin>();)

Plugin::Plugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoShapeRegistry::instance()->add(new PluginShapeFactory());
}

#include <Plugin.moc>
