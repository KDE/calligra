/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KarbonToolsPlugin.h"
#include "CalligraphyTool/KarbonCalligraphicShapeFactory.h"
#include "CalligraphyTool/KarbonCalligraphyToolFactory.h"
#include "KarbonFilterEffectsToolFactory.h"
#include "KarbonGradientToolFactory.h"
#include "KarbonPatternToolFactory.h"

#include <KoShapeRegistry.h>
#include <KoToolRegistry.h>

#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(KarbonToolsPluginFactory, "karbon_tools.json", registerPlugin<KarbonToolsPlugin>();)

KarbonToolsPlugin::KarbonToolsPlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoToolRegistry::instance()->add(new KarbonCalligraphyToolFactory());
    KoToolRegistry::instance()->add(new KarbonGradientToolFactory());
    KoToolRegistry::instance()->add(new KarbonPatternToolFactory());
    KoToolRegistry::instance()->add(new KarbonFilterEffectsToolFactory());

    KoShapeRegistry::instance()->add(new KarbonCalligraphicShapeFactory());
}
#include <KarbonToolsPlugin.moc>
