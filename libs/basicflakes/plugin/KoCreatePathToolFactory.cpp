/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoCreatePathToolFactory.h"
#include "KoCreatePathTool.h"

#include <KLocalizedString>
#include <KoIcon.h>

KoCreatePathToolFactory::KoCreatePathToolFactory()
    : KoToolFactoryBase(KoCreatePathTool_ID)
{
    setToolTip(i18n("Draw path"));
    setToolType(mainToolType());
    setPriority(2);
    setIconName(koIconName("createpath"));
    setActivationShapeId("flake/edit");
}

KoCreatePathToolFactory::~KoCreatePathToolFactory() = default;

KoToolBase *KoCreatePathToolFactory::createTool(KoCanvasBase *canvas)
{
    return new KoCreatePathTool(canvas);
}
