/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "ThreedShapePlugin.h"

// KF5
#include <KPluginFactory>

// Calligra libs
#include <KoShapeRegistry.h>
#include <KoToolRegistry.h>

// 3D shape
#include "ThreedShapeFactory.h"
// #include "ThreedToolFactory.h"

K_PLUGIN_FACTORY_WITH_JSON(ThreedShapePluginFactory, "calligra_shape_threed.json", registerPlugin<ThreedShapePlugin>();)

ThreedShapePlugin::ThreedShapePlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoShapeRegistry::instance()->add(new ThreedShapeFactory());
    // KoToolRegistry::instance()->add( new ThreedToolFactory() );
}

#include <ThreedShapePlugin.moc>
