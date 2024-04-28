/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "StencilBoxPlugin.h"

#include "StencilBoxDockerFactory.h"

#include <KPluginFactory>
#include <KoDockRegistry.h>

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "calligra_docker_stencils.json", registerPlugin<StencilBoxPlugin>();)

StencilBoxPlugin::StencilBoxPlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    Q_UNUSED(parent);
    KoDockRegistry::instance()->add(new StencilBoxDockerFactory());
}
#include <StencilBoxPlugin.moc>
