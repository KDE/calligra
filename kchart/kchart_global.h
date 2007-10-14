/* This file is part of the KDE project
   Copyright (C) 2007 Inge Wallin <inge@lysator.liu.se>

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


#ifndef KCHART_GLOBAL_H
#define KCHART_GLOBAL_H


namespace KChart
{

// Chart types for OpenDocument

typedef enum {
    BarChartType,
    LineChartType,
    AreaChartType,
    CircleChartType,		// Pie in KDChart
    RingChartType,
    ScatterChartType,
    RadarChartType,		// Polar in KDChart
    StockChartType,
    BubbleChartType,
    SurfaceChartType,
    GanttChartType,

    LastChartType               // Not an actual type, just a place holder
} OdfChartType;
const int NUM_CHARTTYPES = int ( LastChartType );


// Chart subtypes, applicable to Bar, Line, Area, and Radar
typedef enum {
    NoChartSubtype,             // for charts with no subtypes
    NormalChartSubtype,         // For bar, line and area charts
    StackedChartSubtype,
    PercentChartSubtype
} OdfChartSubtype;

// Data direction
typedef  enum {
    DataRowsDirection    = 0,
    DataColumnsDirection = 1
} DataDirection;


}  //KChart namespace

#endif
