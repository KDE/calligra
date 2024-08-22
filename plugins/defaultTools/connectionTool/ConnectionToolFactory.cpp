/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ConnectionToolFactory.h"
#include "ConnectionTool.h"

#include <KLocalizedString>
#include <KoIcon.h>
#include <QDebug>

ConnectionToolFactory::ConnectionToolFactory()
    : KoToolFactoryBase(ConnectionTool_ID)
{
    setToolTip(i18n("Connect shapes"));
    setIconName(koIconName("x-shape-connection"));
    setToolType(mainToolType());
    setPriority(1);
    setActivationShapeId("flake/always");
}

ConnectionToolFactory::~ConnectionToolFactory() = default;

KoToolBase *ConnectionToolFactory::createTool(KoCanvasBase *canvas)
{
    return new ConnectionTool(canvas);
}
