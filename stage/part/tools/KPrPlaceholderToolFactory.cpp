// This file is part of the KDE project
// SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "KPrPlaceholderToolFactory.h"

#include "KPrPlaceholderTool.h"
#include "KPrPlaceholderShape.h"

#include <klocalizedstring.h>

KPrPlaceholderToolFactory::KPrPlaceholderToolFactory()
    : KoToolFactoryBase("Layout Tool")
{
    setToolTip(i18n("Layout"));
    setToolType(dynamicToolType());
    setPriority(0);
    setActivationShapeId(KPrPlaceholderShapeId);
}

KPrPlaceholderToolFactory::~KPrPlaceholderToolFactory()
{
}

KoToolBase * KPrPlaceholderToolFactory::createTool(KoCanvasBase *canvas)
{
    return new KPrPlaceholderTool(canvas);
}
