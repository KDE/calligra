/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "VectorShapePlugin.h"

// KF5
#include <KPluginFactory>

// Calligra libs
#include <KoShapeRegistry.h>
#include <KoToolRegistry.h>

// VectorShape
#include "VectorShapeFactory.h"
#include "VectorToolFactory.h"

K_PLUGIN_FACTORY_WITH_JSON(VectorShapePluginFactory, "calligra_shape_vector.json", registerPlugin<VectorShapePlugin>();)

VectorShapePlugin::VectorShapePlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoToolRegistry::instance()->add(new VectorToolFactory());
    KoShapeRegistry::instance()->add(new VectorShapeFactory());
}

#include <VectorShapePlugin.moc>
