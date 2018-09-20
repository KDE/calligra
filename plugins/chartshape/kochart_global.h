/* This file is part of the KDE project

   Copyright 2017 Dag Andersen <danders@get2net.dk>
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


#ifndef KCHART_GLOBAL_H
#define KCHART_GLOBAL_H

#include <QLatin1String>
#include <QDebug>

#include <KoTextShapeData.h>
typedef KoTextShapeData TextLabelData;

namespace KoChart
{

// Chart types for OpenDocument
enum ChartType {
    BarChartType,
    LineChartType,
    AreaChartType,
    CircleChartType,		// Pie in KChart
    RingChartType,
    ScatterChartType,
    RadarChartType,		    // Polar in KChart
    FilledRadarChartType,   // Polar in KChart
    StockChartType,
    BubbleChartType,
    SurfaceChartType,
    GanttChartType,
    LastChartType               // Not an actual type, just a place holder
};
const int NUM_CHARTTYPES = int (LastChartType);

bool isPolar(ChartType type);
bool isCartesian(ChartType type);


// Chart subtypes, applicable to Bar, Line, Area, and Radar
enum ChartSubtype {
    NoChartSubtype,             // for charts with no subtypes
    NormalChartSubtype,         // For bar, line, area and radar charts
    StackedChartSubtype,
    PercentChartSubtype,
    HighLowCloseChartSubtype,               // For stock charts
    OpenHighLowCloseChartSubtype,
    CandlestickChartSubtype
};

ChartSubtype defaultChartSubtype(ChartType type);
int numDimensions(ChartType type, ChartSubtype subtype = NoChartSubtype);

QLatin1String chartTypeIconName(ChartType type, ChartSubtype subtype);
#define ICON1(charttype) QIcon::fromTheme(chartTypeIconName(charttype, NoChartSubtype))
#define ICON2(charttype, chartsubtype) QIcon::fromTheme(chartTypeIconName(charttype, chartsubtype))


enum AxisDimension {
    XAxisDimension,
    YAxisDimension,
    ZAxisDimension
};

struct ChartTypeOptions
{
    ChartSubtype subtype;
};

enum Position {
    StartPosition,
    TopPosition,
    EndPosition,
    BottomPosition,
    TopStartPosition,
    TopEndPosition,
    BottomStartPosition,
    BottomEndPosition,
    CenterPosition,

    FloatingPosition
};

enum LegendExpansion {
    HighLegendExpansion,
    WideLegendExpansion,
    BalancedLegendExpansion,
    CustomLegendExpansion
};

enum ErrorCategory {
    NoErrorCategory,
    VarianceErrorCategory,
    StandardDeviationErrorCategory,
    StandardErrorErrorCategory,
    PercentageErrorCategory,
    ErrorMarginErrorCategory,
    ConstantErrorCategory
};

enum ItemType {
    GenericItemType = 0,
    TitleLabelType = 1,
    SubTitleLabelType = 3,
    FooterLabelType = 5,
    PlotAreaType = 10,
    LegendType = 11,
    XAxisTitleType = 20,
    YAxisTitleType = 21,
    SecondaryXAxisTitleType = 22,
    SecondaryYAxisTitleType = 23
};

enum OdfSymbolType {
    NoSymbol,
    AutomaticSymbol,
    NamedSymbol,
    ImageSymbol
};

// From odf spec:
// The values of the chart:symbol-name attribute are:
// square, diamond, arrow-down, arrow-up, arrow-right, arrow-left, bow-tie, hourglass, circle, star, x, plus, asterisk, horizontal-bar or vertical-bar
enum OdfMarkerStyle {
    MarkerSquare         = 0,
    MarkerDiamond        = 1,
    MarkerArrowDown      = 2,
    MarkerArrowUp        = 3,
    MarkerArrowRight     = 4,
    MarkerArrowLeft      = 5,
    MarkerBowTie         = 6,
    MarkerHourGlass      = 7,
    MarkerCircle         = 8,
    MarkerStar           = 9,
    MarkerX              = 10,
    MarkerCross          = 11,
    MarkerAsterisk       = 12,
    MarkerHorizontalBar  = 13,
    MarkerVerticalBar    = 14,
    // NOTE: These are not odf
    MarkerRing           = 15,
    MarkerFastCross      = 16,
    Marker1Pixel         = 17,
    Marker4Pixels        = 18
};

} // Namespace KoChart

QDebug operator<<(QDebug dbg, KoChart::Position p);
QDebug operator<<(QDebug dbg, KoChart::ChartType ct);
QDebug operator<<(QDebug dbg, KoChart::ChartSubtype st);

#endif
