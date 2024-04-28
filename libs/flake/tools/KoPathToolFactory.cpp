/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPathToolFactory.h"
#include "KoPathTool.h"
#include "KoPathShape.h"

#include <KoIcon.h>
#include <KLocalizedString>

KoPathToolFactory::KoPathToolFactory()
        : KoToolFactoryBase("PathToolFactoryId")
{
    setToolTip(i18n("Path editing"));
    setToolType(dynamicToolType());
    setIconName(koIconName("editpath"));
    setPriority(2);
    setActivationShapeId(KoPathShapeId);
}

KoPathToolFactory::~KoPathToolFactory()
{
}

KoToolBase * KoPathToolFactory::createTool(KoCanvasBase *canvas)
{
    return new KoPathTool(canvas);
}
