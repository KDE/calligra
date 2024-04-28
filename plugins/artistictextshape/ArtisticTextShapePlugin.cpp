/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ArtisticTextShapePlugin.h"
#include "ArtisticTextShapeFactory.h"
#include "ArtisticTextToolFactory.h"

#include <KoShapeRegistry.h>
#include <KoToolRegistry.h>

#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(ArtisticTextShapePluginFactory, "calligra_shape_artistictext.json", registerPlugin<ArtisticTextShapePlugin>();)

ArtisticTextShapePlugin::ArtisticTextShapePlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoShapeRegistry::instance()->add(new ArtisticTextShapeFactory());
    KoToolRegistry::instance()->add(new ArtisticTextToolFactory());
}

ArtisticTextShapePlugin::~ArtisticTextShapePlugin()
{
}

#include <ArtisticTextShapePlugin.moc>
