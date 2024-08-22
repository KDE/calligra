/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "VectorToolFactory.h"

#include "VectorShape.h"
#include "VectorTool.h"

#include <KLocalizedString>
#include <KoIcon.h>

VectorToolFactory::VectorToolFactory()
    : KoToolFactoryBase("VectorToolFactoryId")
{
    setToolTip(i18n("Vector Image (EMF/WMF/SVM/SVG) tool"));
    setIconName(koIconNameNeededWithSubs("a generic vector image icon", "x-shape-vectorimage", "application-x-wmf"));
    setToolType(dynamicToolType());
    setPriority(1);
    setActivationShapeId(VectorShape_SHAPEID);
}

VectorToolFactory::~VectorToolFactory() = default;

KoToolBase *VectorToolFactory::createTool(KoCanvasBase *canvas)
{
    return new VectorTool(canvas);
}
