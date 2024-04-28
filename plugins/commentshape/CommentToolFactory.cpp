/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "CommentToolFactory.h"

#include "CommentShapeFactory.h"
#include "CommentTool.h"

#include <KoIcon.h>

CommentToolFactory::CommentToolFactory()
    : KoToolFactoryBase("CommentToolFactory")
{
    setToolTip(i18n("Comment"));
    setToolType(mainToolType());
    setIconName(koIconName("tool_comment"));
    setActivationShapeId(COMMENTSHAPEID);
}

CommentToolFactory::~CommentToolFactory()
{
}

KoToolBase *CommentToolFactory::createTool(KoCanvasBase *canvas)
{
    return new CommentTool(canvas);
}

bool CommentToolFactory::canCreateTool(KoCanvasBase *canvas) const
{
    return KoToolFactoryBase::canCreateTool(canvas);
}
