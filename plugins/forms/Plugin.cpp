/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "Plugin.h"

#include "KoFormShapeFactory.h"
#include "KoFormTool.h"
#include <KoToolRegistry.h>

#include <KPluginFactory>
#include <KoShapeRegistry.h>

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "calligra_shape_forms.json", registerPlugin<Plugin>();)

Plugin::Plugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoShapeRegistry::instance()->add(new KoFormShapeFactory());
    KoToolRegistry::instance()->add(new KoFormToolFactory());
}

#include "Plugin.moc"
