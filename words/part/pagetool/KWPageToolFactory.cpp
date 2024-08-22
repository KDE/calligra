/*
 *  SPDX-FileCopyrightText: 2011 Jignesh Kakadiya <jigneshhk1992@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KWPageToolFactory.h"
#include "KWPageTool.h"
#include <KoCanvasBase.h>
#include <KoIcon.h>

#include <KLocalizedString>

KWPageToolFactory::KWPageToolFactory()
    : KoToolFactoryBase("PageToolFactory_ID")
{
    setToolTip(i18n("Page layout"));
    setToolType(dynamicToolType() + ",calligrawords, calligraauthor");
    setIconName(koIconName("tool_pagelayout"));
    setPriority(25);
    setActivationShapeId("flake/always");
}

KWPageToolFactory::~KWPageToolFactory() = default;

KoToolBase *KWPageToolFactory::createTool(KoCanvasBase *canvas)
{
    return new KWPageTool(canvas);
}
