/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Pierre Stirnweiss \pierre.stirnweiss_calligra@gadz.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ReferencesToolFactory.h"
#include "ReferencesTool.h"
#include "TextShape.h"

#include <KoCanvasBase.h>
#include <KoIcon.h>

#include <KLocalizedString>

ReferencesToolFactory::ReferencesToolFactory()
    : KoToolFactoryBase("ReferencesToolFactory_ID")
{
    setToolTip(i18n("References"));
    setToolType("calligrawords,calligraauthor");
    setIconName(koIconName("tool_references"));
    setPriority(20);
    setActivationShapeId(TextShape_SHAPEID);
}

ReferencesToolFactory::~ReferencesToolFactory() = default;

KoToolBase *ReferencesToolFactory::createTool(KoCanvasBase *canvas)
{
    return new ReferencesTool(canvas);
}
