/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "Plugin.h"
#include "CommentShapeFactory.h"
#include "CommentToolFactory.h"

#include <KoShapeRegistry.h>
#include <KoToolRegistry.h>

#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "calligra_shape_comment.json", registerPlugin<Plugin>();)

Plugin::Plugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoShapeRegistry::instance()->add(new CommentShapeFactory());
    KoToolRegistry::instance()->add(new CommentToolFactory());
}

#include <Plugin.moc>
