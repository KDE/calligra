/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Montel Laurent <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <KLocalizedString>
#include <KoIcon.h>

#include "PictureShape.h"
#include "PictureTool.h"

#include "PictureToolFactory.h"

PictureToolFactory::PictureToolFactory()
    : KoToolFactoryBase("PictureToolFactoryId")
{
    setToolTip(i18n("Picture editing"));
    setIconName(koIconName("x-shape-image"));
    setToolType(dynamicToolType());
    setPriority(1);
    setActivationShapeId(PICTURESHAPEID);
}

PictureToolFactory::~PictureToolFactory() = default;

KoToolBase *PictureToolFactory::createTool(KoCanvasBase *canvas)
{
    return new PictureTool(canvas);
}
