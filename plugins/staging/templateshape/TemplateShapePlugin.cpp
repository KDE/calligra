/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "TemplateShapePlugin.h"

// KF5
#include <KPluginFactory>

// Calligra libs
#include <KoShapeRegistry.h>
#include <KoToolRegistry.h>

// This shape
#include "TemplateShapeFactory.h"
#include "TemplateToolFactory.h"

K_PLUGIN_FACTORY_WITH_JSON(TemplateShapePluginFactory, "calligra_shape_template.json", registerPlugin<TemplateShapePlugin>();)

TemplateShapePlugin::TemplateShapePlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoShapeRegistry::instance()->add(new TemplateShapeFactory());

    // Template: Disable this if you don't need editing.
    KoToolRegistry::instance()->add(new TemplateToolFactory());
}

#include <TemplateShapePlugin.moc>
