/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2007 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "ChartToolFactory.h"

// KoChart
#include "ChartShape.h"
#include "ChartTool.h"

// Calligra
#include <KoIcon.h>

// KF5
#include <KLocalizedString>

using namespace KoChart;

ChartToolFactory::ChartToolFactory()
    : KoToolFactoryBase("ChartToolFactory_ID")
{
    setToolTip(i18n("Chart editing"));
    setToolType(dynamicToolType());
    setIconName(koIconNameNeededWithSubs("enables editing of a given chart of any type", "edit-chart", "office-chart-bar"));
    setPriority(1);
    setActivationShapeId(ChartShapeId);
}

ChartToolFactory::~ChartToolFactory() = default;

KoToolBase *ChartToolFactory::createTool(KoCanvasBase *canvas)
{
    return new ChartTool(canvas);
}
