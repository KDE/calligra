/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoZoomToolFactory.h"
#include "KoZoomTool.h"

#include <KLocalizedString>
#include <KoIcon.h>

KoZoomToolFactory::KoZoomToolFactory()
    : KoToolFactoryBase("KoZoomToolId")
{
    setToolTip(i18n("Zoom"));
    setToolType(navigationToolType());
    setPriority(5);
    setIconName(koIconName("zoom-original"));
    setActivationShapeId("flake/always");
}

KoToolBase *KoZoomToolFactory::createTool(KoCanvasBase *canvas)
{
    return new KoZoomTool(canvas);
}
