/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "DefaultToolFactory.h"
#include "DefaultTool.h"

#include <KLocalizedString>
#include <KoIcon.h>

DefaultToolFactory::DefaultToolFactory()
    : KoToolFactoryBase(KoInteractionTool_ID)
{
    setToolTip(i18n("Shape handling"));
    setToolType(mainToolType());
    setPriority(0);
    setIconName(koIconName("select"));
    setActivationShapeId("flake/always");
}

DefaultToolFactory::~DefaultToolFactory() = default;

KoToolBase *DefaultToolFactory::createTool(KoCanvasBase *canvas)
{
    return new DefaultTool(canvas);
}
