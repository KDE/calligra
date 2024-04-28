/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "TextToolFactory.h"
#include "TextTool.h"
#include "TextShape.h"
#include "AnnotationTextShape.h"

#include <KoIcon.h>
#include <KLocalizedString>

TextToolFactory::TextToolFactory()
        : KoToolFactoryBase("TextToolFactory_ID")
{
    setToolTip(i18n("Text editing"));
    setToolType(dynamicToolType()+",calligrawords,calligraauthor");
    setIconName(koIconName("tool-text"));
    setPriority(1);
    setActivationShapeId(TextShape_SHAPEID "," AnnotationShape_SHAPEID);
}

TextToolFactory::~TextToolFactory()
{
}

KoToolBase * TextToolFactory::createTool(KoCanvasBase *canvas)
{
    return new TextTool(canvas);
}
