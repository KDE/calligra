/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPanToolFactory.h"
#include "KoPanTool.h"

#include <KLocalizedString>
#include <KoIcon.h>

KoPanToolFactory::KoPanToolFactory()
    : KoToolFactoryBase(KoPanTool_ID)
{
    setToolTip(i18n("Pan"));
    setToolType(navigationToolType());
    setPriority(5);
    setIconName(koIconName("hand"));
    setActivationShapeId("flake/always");
}

KoToolBase *KoPanToolFactory::createTool(KoCanvasBase *canvas)
{
    return new KoPanTool(canvas);
}
