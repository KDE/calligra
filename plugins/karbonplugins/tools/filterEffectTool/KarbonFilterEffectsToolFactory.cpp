/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KarbonFilterEffectsToolFactory.h"
#include "KarbonFilterEffectsTool.h"

#include <KLocalizedString>
#include <KoIcon.h>

KarbonFilterEffectsToolFactory::KarbonFilterEffectsToolFactory()
    : KoToolFactoryBase("KarbonFilterEffectsTool")
{
    setToolTip(i18n("Filter effects editing"));
    setToolType("karbon");
    setIconName(koIconName("tool_imageeffects")); // TODO: better icon, e.g. black Fx bad on dark UI
    setPriority(3);
}

KarbonFilterEffectsToolFactory::~KarbonFilterEffectsToolFactory() = default;

KoToolBase *KarbonFilterEffectsToolFactory::createTool(KoCanvasBase *canvas)
{
    return new KarbonFilterEffectsTool(canvas);
}
