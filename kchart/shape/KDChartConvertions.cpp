/* This file is part of the KDE project

   Copyright 2007 Inge Wallin <inge@lysator.liu.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


// Own
#include "KDChartConvertions.h"

// KDChart
#include <KDChartEnums>
#include <KDChartCartesianAxis>


namespace KChart {

KDChart::CartesianAxis::Position AxisPositionToKDChartAxisPosition( AxisPosition position )
{
    switch ( position ) {
    case BottomAxisPosition:
        return KDChart::CartesianAxis::Bottom;
    case TopAxisPosition:
        return KDChart::CartesianAxis::Top;
    case RightAxisPosition:
        return KDChart::CartesianAxis::Right;
    case LeftAxisPosition:
        return KDChart::CartesianAxis::Left;
    }
    
    Q_ASSERT( "Unknown KDChart::CartesianAxis::Position!" );
    return KDChart::CartesianAxis::Bottom;
}

AxisPosition KDChartAxisPositionToAxisPosition( KDChart::CartesianAxis::Position position )
{
    switch ( position ) {
    case KDChart::CartesianAxis::Bottom:
        return BottomAxisPosition;
    case KDChart::CartesianAxis::Top:
        return TopAxisPosition;
    case KDChart::CartesianAxis::Right:
        return RightAxisPosition;
    case KDChart::CartesianAxis::Left:
        return LeftAxisPosition;
    }
    
    Q_ASSERT( "Unknown AxisPosition!" );
    return BottomAxisPosition;
}

QString LegendPositionToString( LegendPosition position )
{
    switch ( position ) {
    case StartLegendPosition:
        return QString( "start" );
    case TopLegendPosition:
        return QString( "top" );
    case BottomLegendPosition:
        return QString( "bottom" );
    case TopStartLegendPosition:
        return QString( "top-start" );
    case BottomStartLegendPosition:
        return QString( "bottom-start" );
    case TopEndLegendPosition:
        return QString( "top-end" );
    case BottomEndLegendPosition:
        return QString( "bottom-end" );
    case EndLegendPosition:
        return QString( "end" );
    case FloatingLegendPosition:
        return QString();
    }
    
    Q_ASSERT( "Unknown LegendPosition!" );
    return QString();
}

KDChartEnums::PositionValue LegendPositionToKDChartPositionValue( LegendPosition position )
{
    switch ( position ) {
    case StartLegendPosition:
        return KDChartEnums::PositionWest;
    case TopLegendPosition:
        return KDChartEnums::PositionNorth;
    case BottomLegendPosition:
        return KDChartEnums::PositionSouth;
    case TopStartLegendPosition:
        return KDChartEnums::PositionNorthWest;
    case BottomStartLegendPosition:
        return KDChartEnums::PositionSouthWest;
    case TopEndLegendPosition:
        return KDChartEnums::PositionNorthEast;
    case BottomEndLegendPosition:
        return KDChartEnums::PositionSouthEast;
    case EndLegendPosition:
        return KDChartEnums::PositionEast;
    case FloatingLegendPosition:
        return KDChartEnums::PositionFloating;
    }
    
    Q_ASSERT( "Unknown LegendPosition!" );
    return KDChartEnums::PositionEast;
}

LegendPosition KDChartPositionValueToLegendPosition( KDChartEnums::PositionValue position )
{
    switch ( position ) {
    case KDChartEnums::PositionNorthWest:
        return TopStartLegendPosition;
    case KDChartEnums::PositionNorth:
        return TopLegendPosition;
    case KDChartEnums::PositionNorthEast:
        return TopEndLegendPosition;
    case KDChartEnums::PositionEast:
        return EndLegendPosition;
    case KDChartEnums::PositionSouthEast:
        return BottomEndLegendPosition;
    case KDChartEnums::PositionSouth:
        return BottomLegendPosition;
    case KDChartEnums::PositionSouthWest:
        return BottomStartLegendPosition;
    case KDChartEnums::PositionWest:
        return StartLegendPosition;
    case KDChartEnums::PositionFloating:
        return FloatingLegendPosition;
        
    // These are unsupported values
    case KDChartEnums::PositionUnknown:
    case KDChartEnums::PositionCenter:
        return FloatingLegendPosition;
    }
    
    Q_ASSERT( "Unknown KDChartEnums::PositionValue!" );
    return FloatingLegendPosition;
}

Qt::Orientation LegendExpansionToQtOrientation( LegendExpansion expansion )
{
    switch ( expansion ) {
    case WideLegendExpansion:
        return Qt::Horizontal;
    case HighLegendExpansion:
        return Qt::Vertical;
        
    // KDChart doesn't allow a balanced expansion
    case BalancedLegendExpansion:
        return Qt::Vertical;
    }
    
    Q_ASSERT( "Unknown Qt::Orientation!" );
    return Qt::Vertical;
}

LegendExpansion QtOrientationToLegendExpansion( Qt::Orientation orientation )
{
    switch ( orientation ) {
    case Qt::Horizontal:
        return WideLegendExpansion;
    case Qt::Vertical:
        return HighLegendExpansion;
    }
    
    Q_ASSERT( "Unknown LegendExpansion!" );
    return HighLegendExpansion;
}

} // Namespace KChart
