/* This file is part of the KDE project
 * Copyright (C) 2008 Pierre Stirnweiss \pierre.stirnweiss_calligra@gadz.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "ReviewToolFactory.h"
#include "ReviewTool.h"
#include "TextShape.h"
#include "AnnotationTextShape.h"

#include <KoCanvasBase.h>
#include <KoShapeController.h>
#include <KoDocumentResourceManager.h>

#include <KoIcon.h>
#include <klocalizedstring.h>

#include <QDebug>

ReviewToolFactory::ReviewToolFactory()
        : KoToolFactoryBase("ReviewToolFactory_ID")
{
    setToolTip(i18n("Review"));
    setToolType(dynamicToolType()+",calligrawords,calligraauthor");
    setIconName(koIconName("tool_review"));
    setPriority(30);

    const QString apps("calligrawords,calligraauthor");
    if (apps.split(',').contains(qApp->applicationName())) {
        setActivationShapeId(TextShape_SHAPEID);
    } else {
        // HACK: non-existing id to disable this tool
        setActivationShapeId(QStringLiteral("xxxReviewToolFactory_IDxxx"));
    }
}

ReviewToolFactory::~ReviewToolFactory()
{
}

KoToolBase *ReviewToolFactory::createTool(KoCanvasBase *canvas)
{
    return new ReviewTool(canvas);
}
