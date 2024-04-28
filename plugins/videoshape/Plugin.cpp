/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "Plugin.h"
#include "VideoShapeFactory.h"
#include "VideoToolFactory.h"

#include <KoShapeRegistry.h>
#include <KoToolRegistry.h>

#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "calligra_shape_video.json", registerPlugin<Plugin>();)

Plugin::Plugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoShapeRegistry::instance()->add(new VideoShapeFactory());
    KoToolRegistry::instance()->add(new VideoToolFactory());
}

#include <Plugin.moc>
