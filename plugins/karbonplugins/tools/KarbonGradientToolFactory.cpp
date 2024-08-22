/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KarbonGradientToolFactory.h"
#include "KarbonGradientTool.h"

#include <KLocalizedString>
#include <KoIcon.h>
#include <QDebug>

KarbonGradientToolFactory::KarbonGradientToolFactory()
    : KoToolFactoryBase("KarbonGradientTool")
{
    setToolTip(i18n("Gradient editing"));
    setToolType("karbon");
    setIconName(koIconName("gradient"));
    // or probably rather "fill-gradient", please request that icon on TechBase
    setPriority(3);
}

KarbonGradientToolFactory::~KarbonGradientToolFactory() = default;

KoToolBase *KarbonGradientToolFactory::createTool(KoCanvasBase *canvas)
{
    return new KarbonGradientTool(canvas);
}
