/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2007 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "KChartConvertions.h"

// KChart
#include <KChartCartesianAxis>
#include <KChartEnums>

namespace KoChart
{

KChart::CartesianAxis::Position PositionToKChartAxisPosition(Position position)
{
    switch (position) {
    case BottomPosition:
        return KChart::CartesianAxis::Bottom;
    case TopPosition:
        return KChart::CartesianAxis::Top;
    case EndPosition:
        return KChart::CartesianAxis::Right;
    case StartPosition:
        return KChart::CartesianAxis::Left;
    }

    Q_ASSERT("Unknown KChart::CartesianAxis::Position!");
    return KChart::CartesianAxis::Bottom;
}

// Used to save e.g. legend-position attribute to ODF. Do not change these strings.
QString PositionToString(Position position)
{
    switch (position) {
    case StartPosition:
        return QString("start");
    case TopPosition:
        return QString("top");
    case BottomPosition:
        return QString("bottom");
    case TopStartPosition:
        return QString("top-start");
    case BottomStartPosition:
        return QString("bottom-start");
    case TopEndPosition:
        return QString("top-end");
    case BottomEndPosition:
        return QString("bottom-end");
    case EndPosition:
        return QString("end");
    case CenterPosition:
        return QString("center");
    case FloatingPosition:
        return QString();
    }

    Q_ASSERT("Unknown Position!");
    return QString();
}

KChartEnums::PositionValue PositionToKChartPositionValue(Position position)
{
    switch (position) {
    case StartPosition:
        return KChartEnums::PositionWest;
    case TopPosition:
        return KChartEnums::PositionNorth;
    case BottomPosition:
        return KChartEnums::PositionSouth;
    case TopStartPosition:
        return KChartEnums::PositionNorthWest;
    case BottomStartPosition:
        return KChartEnums::PositionSouthWest;
    case TopEndPosition:
        return KChartEnums::PositionNorthEast;
    case BottomEndPosition:
        return KChartEnums::PositionSouthEast;
    case EndPosition:
        return KChartEnums::PositionEast;
    case CenterPosition:
        return KChartEnums::PositionCenter;
    case FloatingPosition:
        return KChartEnums::PositionFloating;
    }

    Q_ASSERT("Unknown Position!");
    return KChartEnums::PositionEast;
}

Position KChartPositionValueToPosition(KChartEnums::PositionValue position)
{
    switch (position) {
    case KChartEnums::PositionNorthWest:
        return TopStartPosition;
    case KChartEnums::PositionNorth:
        return TopPosition;
    case KChartEnums::PositionNorthEast:
        return TopEndPosition;
    case KChartEnums::PositionEast:
        return EndPosition;
    case KChartEnums::PositionSouthEast:
        return BottomEndPosition;
    case KChartEnums::PositionSouth:
        return BottomPosition;
    case KChartEnums::PositionSouthWest:
        return BottomStartPosition;
    case KChartEnums::PositionWest:
        return StartPosition;
    case KChartEnums::PositionCenter:
        return CenterPosition;
    case KChartEnums::PositionFloating:
        return FloatingPosition;

    // These are unsupported values
    case KChartEnums::PositionUnknown:
        return FloatingPosition;
    }

    Q_ASSERT("Unknown KChartEnums::PositionValue!");
    return FloatingPosition;
}

Qt::Orientation LegendExpansionToQtOrientation(LegendExpansion expansion)
{
    switch (expansion) {
    case HighLegendExpansion:
        return Qt::Vertical;
    case WideLegendExpansion:
        return Qt::Horizontal;
    case BalancedLegendExpansion:
        // KChart doesn't allow a balanced expansion
        return Qt::Vertical;
    case CustomLegendExpansion:
        // KChart doesn't allow a custom expansion
        return Qt::Vertical;
    }

    Q_ASSERT("Unknown Qt::Orientation!");
    return Qt::Vertical;
}

LegendExpansion QtOrientationToLegendExpansion(Qt::Orientation orientation)
{
    switch (orientation) {
    case Qt::Horizontal:
        return WideLegendExpansion;
    case Qt::Vertical:
        return HighLegendExpansion;
    }

    Q_ASSERT("Unknown LegendExpansion!");
    return HighLegendExpansion;
}

} // Namespace KoChart
