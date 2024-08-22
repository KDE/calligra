/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "MusicToolFactory.h"

#include "MusicShape.h"
#include "MusicTool.h"

#include <KLocalizedString>
#include <KoIcon.h>

MusicToolFactory::MusicToolFactory()
    : KoToolFactoryBase("MusicToolFactoryId")
{
    setToolTip(i18n("Music editing, parts"));
    setIconName(koIconName("musicshape"));
    setToolType(dynamicToolType());
    setPriority(2);
    setActivationShapeId(MusicShapeId);
}

MusicToolFactory::~MusicToolFactory() = default;

KoToolBase *MusicToolFactory::createTool(KoCanvasBase *canvas)
{
    return new MusicTool(canvas);
}
