/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "TemplateToolFactory.h"

// KF5
#include <KLocalizedString>

// Calligra
#include <KoIcon.h>

// This Shape
#include "TemplateShape.h"
#include "TemplateTool.h"

TemplateToolFactory::TemplateToolFactory()
    : KoToolFactoryBase("TemplateToolFactoryId")
{
    setToolTip(i18n("Template shape editing"));
    setIconName(koIconName("x-template"));
    setToolType(dynamicToolType());
    setPriority(1);
    setActivationShapeId(TEMPLATESHAPEID);
}

TemplateToolFactory::~TemplateToolFactory()
{
}

KoToolBase *TemplateToolFactory::createTool(KoCanvasBase *canvas)
{
    return new TemplateTool(canvas);
}
