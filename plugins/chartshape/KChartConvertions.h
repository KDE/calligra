/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_KDCHARTCONVERTIONS_H
#define KCHART_KDCHARTCONVERTIONS_H

// KChart
#include <KChartCartesianAxis>
#include <KChartEnums>

// KoChart
#include "kochart_global.h"

namespace KoChart
{

extern KChart::CartesianAxis::Position PositionToKChartAxisPosition(Position position);

extern QString PositionToString(Position position);
extern KChartEnums::PositionValue PositionToKChartPositionValue(Position position);
extern Position KChartPositionValueToPosition(KChartEnums::PositionValue position);

extern Qt::Orientation LegendExpansionToQtOrientation(LegendExpansion expansion);
extern LegendExpansion QtOrientationToLegendExpansion(Qt::Orientation orientation);

} // Namespace KoChart

#endif
