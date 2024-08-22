/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "GuidesToolFactory.h"
#include "GuidesTool.h"
#include <KLocalizedString>

GuidesToolFactory::GuidesToolFactory()
    : KoToolFactoryBase(GuidesToolId)
{
    setToolTip(i18n("Edit guidelines"));
    setToolType("never");
    setPriority(1);
    setActivationShapeId("itShouldNeverBeActivated");
}

GuidesToolFactory::~GuidesToolFactory() = default;

KoToolBase *GuidesToolFactory::createTool(KoCanvasBase *canvas)
{
    return new GuidesTool(canvas);
}
