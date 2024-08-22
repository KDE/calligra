/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include <KLocalizedString>
#include <KoIcon.h>

#include "MusicShape.h"
#include "SimpleEntryTool.h"

#include "SimpleEntryToolFactory.h"

SimpleEntryToolFactory::SimpleEntryToolFactory()
    : KoToolFactoryBase("SimpleEntryToolFactoryId")
{
    setToolTip(i18n("Music editing"));
    setIconName(koIconName("music-note-16th"));
    setToolType(dynamicToolType());
    setPriority(1);
    setActivationShapeId(MusicShapeId);
}

SimpleEntryToolFactory::~SimpleEntryToolFactory() = default;

KoToolBase *SimpleEntryToolFactory::createTool(KoCanvasBase *canvas)
{
    return new SimpleEntryTool(canvas);
}
