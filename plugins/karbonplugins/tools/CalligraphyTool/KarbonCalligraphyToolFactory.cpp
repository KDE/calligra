/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KarbonCalligraphyToolFactory.h"
#include "KarbonCalligraphyTool.h"

#include <KoToolRegistry.h>

#include <KLocalizedString>
#include <KoIcon.h>
#include <QDebug>

KarbonCalligraphyToolFactory::KarbonCalligraphyToolFactory()
    : KoToolFactoryBase("KarbonCalligraphyTool")
{
    setToolTip(i18n("Calligraphy"));
    setToolType("karbon");
    setIconName(koIconName("calligraphy"));
    setPriority(3);
    setActivationShapeId("flake/edit");
}

KarbonCalligraphyToolFactory::~KarbonCalligraphyToolFactory() = default;

KoToolBase *KarbonCalligraphyToolFactory::createTool(KoCanvasBase *canvas)
{
    return new KarbonCalligraphyTool(canvas);
}
