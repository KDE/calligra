/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoCreateShapesToolFactory.h"
#include "KoCreateShapesTool.h"

#include <KLocalizedString>

KoCreateShapesToolFactory::KoCreateShapesToolFactory()
    : KoToolFactoryBase(KoCreateShapesTool_ID)
{
    setToolTip(i18n("Create object"));
    setToolType(mainToolType());
    setPriority(1);
}

KoCreateShapesToolFactory::~KoCreateShapesToolFactory() = default;

KoToolBase *KoCreateShapesToolFactory::createTool(KoCanvasBase *canvas)
{
    return new KoCreateShapesTool(canvas);
}
