/* This file is part of the KDE project

   Copyright 2010 Johannes Simon <johannes.simon@gmail.com>

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

#include "kochart_global.h"

namespace KoChart {

bool isPolar(ChartType type)
{
    switch (type)
    {
    case CircleChartType:
    case RingChartType:
    case RadarChartType:
    case FilledRadarChartType:
        return true;
    default:
        return false;
    }
    return false;
}

bool isCartesian(ChartType type)
{
    return !isPolar(type);
}

ChartSubtype defaultChartSubtype(ChartType type)
{
    switch (type) {
        case BarChartType:
        case LineChartType:
        case AreaChartType:
            return NormalChartSubtype;
        case CircleChartType:
        case RingChartType:
            return NoChartSubtype;
        case RadarChartType:
        case FilledRadarChartType:
            return NormalChartSubtype;
        case ScatterChartType:
            return NoChartSubtype;
        case SurfaceChartType:
            return NoChartSubtype;
        case BubbleChartType:
            return NoChartSubtype;
        case StockChartType:
            return HighLowCloseChartSubtype;
        default:
            break;
    }
    return NoChartSubtype;
}

int numDimensions(ChartType type, ChartSubtype subtype)
{
    int dimensions = 1;

    switch (type) {
    case BarChartType:
    case LineChartType:
    case AreaChartType:
    case CircleChartType:
    case RingChartType:
    case RadarChartType:
    case FilledRadarChartType:
        dimensions = 1;
        break;
    case ScatterChartType:
    case SurfaceChartType:
        dimensions = 2;
        break;
    case BubbleChartType:
        dimensions = 3;
        break;
    case StockChartType:
        dimensions = 1;
        break;
    case GanttChartType:
        // FIXME: Figure out correct number of dimensions
        dimensions = 1;
        break;
    case LastChartType:
        dimensions = 1;
    }

    return dimensions;
}

QMap<QPair<qint64, qint64>, QLatin1String> chartTypeIconMap;

void insertIcon(ChartType type, ChartSubtype subtype, const QLatin1String &name)
{
    chartTypeIconMap[QPair<qint64, qint64>((qint64)(type), (qint64)(subtype))] = name;
}

void initchartTypeIconMap()
{
    // Bar charts
    insertIcon(BarChartType, NoChartSubtype, QLatin1String("office-chart-bar"));
    insertIcon(BarChartType, NormalChartSubtype, QLatin1String("office-chart-bar"));
    insertIcon(BarChartType, StackedChartSubtype, QLatin1String("office-chart-bar-stacked"));
    insertIcon(BarChartType, PercentChartSubtype, QLatin1String("office-chart-bar-percentage"));

    // Line charts
    insertIcon(LineChartType, NoChartSubtype, QLatin1String("office-chart-line"));
    insertIcon(LineChartType, NormalChartSubtype, QLatin1String("office-chart-line"));
    insertIcon(LineChartType, StackedChartSubtype, QLatin1String("office-chart-line-stacked"));
    insertIcon(LineChartType, PercentChartSubtype, QLatin1String("office-chart-line-percentage"));

    // Area charts
    insertIcon(AreaChartType, NoChartSubtype, QLatin1String("office-chart-area"));
    insertIcon(AreaChartType, NormalChartSubtype, QLatin1String("office-chart-area"));
    insertIcon(AreaChartType, StackedChartSubtype, QLatin1String("office-chart-area-stacked"));
    insertIcon(AreaChartType, PercentChartSubtype, QLatin1String("office-chart-area-percentage"));

    // Circular charts: pie and ring
    insertIcon(CircleChartType, NoChartSubtype, QLatin1String("office-chart-pie"));
    insertIcon(CircleChartType, NormalChartSubtype, QLatin1String("office-chart-pie"));

    insertIcon(RingChartType, NoChartSubtype, QLatin1String("office-chart-ring"));
    insertIcon(RingChartType, NormalChartSubtype, QLatin1String("office-chart-ring"));

    // Polar charts: radar
    insertIcon(RadarChartType, NoChartSubtype, QLatin1String("office-chart-polar"));
    insertIcon(RadarChartType, NormalChartSubtype, QLatin1String("office-chart-polar"));
    insertIcon(RadarChartType, StackedChartSubtype, QLatin1String("office-chart-polar-stacked"));
    insertIcon(RadarChartType, PercentChartSubtype, QLatin1String("office-chart-polar"));

    insertIcon(FilledRadarChartType, NoChartSubtype, QLatin1String("office-chart-polar-filled"));
    insertIcon(FilledRadarChartType, NormalChartSubtype, QLatin1String("office-chart-polar-filled"));
    insertIcon(FilledRadarChartType, StackedChartSubtype, QLatin1String("office-chart-polar-filled"));
    insertIcon(FilledRadarChartType, PercentChartSubtype, QLatin1String("office-chart-polar-filled"));

    // X/Y charts: scatter and bubble
    insertIcon(ScatterChartType, NoChartSubtype, QLatin1String("office-chart-scatter"));
    insertIcon(ScatterChartType, NormalChartSubtype, QLatin1String("office-chart-scatter"));

    insertIcon(BubbleChartType, NoChartSubtype, QLatin1String("skg-chart-bubble"));
    insertIcon(BubbleChartType, NormalChartSubtype, QLatin1String("skg-chart-bubble"));

    // Stock Charts
    insertIcon(StockChartType, NoChartSubtype, QLatin1String("office-chart-stock-candlestick"));
    insertIcon(StockChartType, HighLowCloseChartSubtype, QLatin1String("office-chart-stock-hlc"));
    insertIcon(StockChartType, OpenHighLowCloseChartSubtype, QLatin1String("office-chart-stock-ohlc"));
    insertIcon(StockChartType, CandlestickChartSubtype, QLatin1String("office-chart-stock-candlestick"));

    // TODO surface/gantt
}

QLatin1String chartTypeIconName(ChartType type, ChartSubtype subtype)
{
    QPair<qint64, qint64> entry((qint64)(type), (qint64)(subtype));
    if (chartTypeIconMap.isEmpty()) {
        initchartTypeIconMap();
    }
    return chartTypeIconMap.value(entry);
}

} // namespace KoChart

QDebug operator<<(QDebug dbg, KoChart::Position p)
{
    switch (p) {
    case KoChart::StartPosition: dbg << "(StartPosition)"; break;
    case KoChart::TopPosition: dbg << "(TopPosition)"; break;
    case KoChart::EndPosition: dbg << "(EndPosition)"; break;
    case KoChart::BottomPosition: dbg << "(BottomPosition)"; break;
    case KoChart::TopStartPosition: dbg << "(BottomPosition)"; break;
    case KoChart::TopEndPosition: dbg << "(TopEndPosition)"; break;
    case KoChart::BottomStartPosition: dbg << "(BottomStartPosition)"; break;
    case KoChart::BottomEndPosition: dbg << "(BottomEndPosition)"; break;
    case KoChart::CenterPosition: dbg << "(CenterPosition)"; break;
    case KoChart::FloatingPosition: dbg << "(FloatingPosition)"; break;
    default: break;
        Q_ASSERT(false); // Unknown position
    }
    return dbg;
}

QDebug operator<<(QDebug dbg, KoChart::ChartType ct)
{
    switch (ct) {
        case KoChart::BarChartType: dbg << "(BarChartType)"; break;
        case KoChart::LineChartType: dbg << "(LineChartType)"; break;
        case KoChart::AreaChartType: dbg << "(AreaChartType)"; break;
        case KoChart::CircleChartType: dbg << "(CircleChartType)"; break;
        case KoChart::RingChartType: dbg << "(RingChartType)"; break;
        case KoChart::RadarChartType: dbg << "(RadarChartType)"; break;
        case KoChart::FilledRadarChartType: dbg << "(FilledRadarChartType)"; break;
        case KoChart::ScatterChartType: dbg << "(ScatterChartType)"; break;
        case KoChart::SurfaceChartType: dbg << "(SurfaceChartType)"; break;
        case KoChart::BubbleChartType: dbg << "(BubbleChartType)"; break;
        case KoChart::StockChartType: dbg << "(StockChartType)"; break;
        case KoChart::GanttChartType: dbg << "(StockChartType)"; break;
        case KoChart::LastChartType: dbg << "(LastChartType)"; break;
    }
    return dbg;
}

QDebug operator<<(QDebug dbg, KoChart::ChartSubtype st)
{
    switch (st) {
        case KoChart::NoChartSubtype: dbg << "(NoChartSubtype)"; break;
        case KoChart::NormalChartSubtype: dbg << "(NormalChartSubtype)"; break;
        case KoChart::StackedChartSubtype: dbg << "(StackedChartSubtype)"; break;
        case KoChart::PercentChartSubtype: dbg << "(PercentChartSubtype)"; break;
        case KoChart::HighLowCloseChartSubtype: dbg << "(HighLowCloseChartSubtype)"; break;
        case KoChart::OpenHighLowCloseChartSubtype: dbg << "(OpenHighLowCloseChartSubtype)"; break;
        case KoChart::CandlestickChartSubtype: dbg << "(CandlestickChartSubtype)"; break;
    }
    return dbg;
}
