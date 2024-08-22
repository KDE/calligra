/* This file is part of the KDE project
 *
 *  SPDX-FileCopyrightText: 2008 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrAnimationToolFactory.h"

#include "KPrAnimationTool.h"
#include "KPrView.h"
#include "StageDebug.h"

#include <KoIcon.h>

#include <KLocalizedString>

KPrAnimationToolFactory::KPrAnimationToolFactory()
    : KoToolFactoryBase("Animation Tool")
{
    setToolTip(i18n("Animation"));
    setToolType("calligrastage");
    setPriority(40);
    setIconName(koIconName("animation-stage"));
    setActivationShapeId("flake/edit");
}

KPrAnimationToolFactory::~KPrAnimationToolFactory() = default;

KoToolBase *KPrAnimationToolFactory::createTool(KoCanvasBase *canvas)
{
    return new KPrAnimationTool(canvas);
}
