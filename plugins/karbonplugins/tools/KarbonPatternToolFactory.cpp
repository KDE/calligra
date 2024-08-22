/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KarbonPatternToolFactory.h"
#include "KarbonPatternTool.h"

#include <KLocalizedString>
#include <KoIcon.h>
#include <QDebug>

KarbonPatternToolFactory::KarbonPatternToolFactory()
    : KoToolFactoryBase("KarbonPatternTool")
{
    setToolTip(i18n("Pattern editing"));
    setToolType("karbon");
    setIconName(koIconName("pattern"));
    setPriority(3);
}

KarbonPatternToolFactory::~KarbonPatternToolFactory() = default;

KoToolBase *KarbonPatternToolFactory::createTool(KoCanvasBase *canvas)
{
    return new KarbonPatternTool(canvas);
}
