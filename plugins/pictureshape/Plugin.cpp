/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "Plugin.h"
#include "PictureShapeFactory.h"
#include "PictureToolFactory.h"

#include <QPixmapCache>

#include <KoShapeRegistry.h>
#include <KoToolRegistry.h>

#include <KPluginFactory>

#define CACHE_SIZE 40960 // 5 images of 2048x1024 at 32bpp

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "calligra_shape_picture.json", registerPlugin<Plugin>();)

Plugin::Plugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    if (QPixmapCache::cacheLimit() < CACHE_SIZE) {
        QPixmapCache::setCacheLimit(CACHE_SIZE);
    }

    KoShapeRegistry::instance()->add(new PictureShapeFactory());
    KoToolRegistry::instance()->add(new PictureToolFactory());
}

#include <Plugin.moc>
