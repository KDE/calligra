/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPencilToolFactory.h"
#include "KoPencilTool.h"

#include <KoIcon.h>
#include <klocalizedstring.h>

KoPencilToolFactory::KoPencilToolFactory()
        : KoToolFactoryBase("KoPencilTool")
{
    setToolTip(i18n("Freehand path"));
    setToolType("karbon");
    setIconName(koIconName("draw-freehand"));
    setPriority(3);
    setActivationShapeId("flake/edit");
}

KoPencilToolFactory::~KoPencilToolFactory()
{
}

KoToolBase * KoPencilToolFactory::createTool(KoCanvasBase *canvas)
{
    return new KoPencilTool(canvas);
}

