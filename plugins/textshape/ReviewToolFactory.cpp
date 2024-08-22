/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Pierre Stirnweiss \pierre.stirnweiss_calligra@gadz.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ReviewToolFactory.h"
#include "AnnotationTextShape.h"
#include "ReviewTool.h"
#include "TextShape.h"

#include <KoCanvasBase.h>
#include <KoDocumentResourceManager.h>
#include <KoShapeController.h>

#include <KLocalizedString>
#include <KoIcon.h>

#include <QDebug>

ReviewToolFactory::ReviewToolFactory()
    : KoToolFactoryBase("ReviewToolFactory_ID")
{
    setToolTip(i18n("Review"));
    setToolType(dynamicToolType() + ",calligrawords,calligraauthor");
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

ReviewToolFactory::~ReviewToolFactory() = default;

KoToolBase *ReviewToolFactory::createTool(KoCanvasBase *canvas)
{
    return new ReviewTool(canvas);
}
