/*
   Copyright (c) 2017 Dag Andersen <danders@get2net.dk>*
   Copyright (c) 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef CHART_DEBUG_H
#define CHART_DEBUG_H

#include <QDebug>
#include <QLoggingCategory>


extern const QLoggingCategory &CHART_LOG();

#define debugChart qCDebug(CHART_LOG)
#define warnChart qCWarning(CHART_LOG)
#define errorChart qCCritical(CHART_LOG)

extern const QLoggingCategory &CHARTPARSE_LOG();

#define debugChartParse qCDebug(CHARTPARSE_LOG)
#define warnChartParse qCWarning(CHARTPARSE_LOG)
#define errorChartParse qCCritical(CHARTPARSE_LOG)

extern const QLoggingCategory &CHARTLAYOUT_LOG();

#define debugChartLayout qCDebug(CHARTLAYOUT_LOG)<<Q_FUNC_INFO
#define warnChartLayout qCWarning(CHARTLAYOUT_LOG)
#define errorChartLayout qCCritical(CHARTLAYOUT_LOG)

extern const QLoggingCategory &CHARTODF_LOG();

#define debugChartOdf qCDebug(CHARTODF_LOG)<<Q_FUNC_INFO
#define warnChartOdf qCWarning(CHARTODF_LOG)
#define errorChartOdf qCCritical(CHARTODF_LOG)

extern const QLoggingCategory &CHARTTOOL_LOG();

#define debugChartTool qCDebug(CHARTTOOL_LOG)<<Q_FUNC_INFO
#define warnChartTool qCWarning(CHARTTOOL_LOG)
#define errorChartTool qCCritical(CHARTTOOL_LOG)

extern const QLoggingCategory &CHARTAXIS_LOG();

#define debugChartAxis qCDebug(CHARTAXIS_LOG)<<Q_FUNC_INFO
#define warnChartAxis qCWarning(CHARTAXIS_LOG)
#define errorChartAxis qCCritical(CHARTAXIS_LOG)

extern const QLoggingCategory &CHARTDADASET_LOG();

#define debugChartDataset qCDebug(CHARTDADASET_LOG)<<Q_FUNC_INFO
#define warnChartDataset qCWarning(CHARTDADASET_LOG)
#define errorChartDataset qCCritical(CHARTDADASET_LOG)

extern const QLoggingCategory &CHARTUI_TITLES_LOG();

#define debugChartUiTitles qCDebug(CHARTUI_TITLES_LOG)<<Q_FUNC_INFO
#define warnChartUiTitles qCWarning(CHARTUI_TITLES_LOG)
#define errorChartUiTitles qCCritical(CHARTUI_TITLES_LOG)

extern const QLoggingCategory &CHARTUI_LEGEND_LOG();

#define debugChartUiLegend qCDebug(CHARTUI_LEGEND_LOG)<<Q_FUNC_INFO
#define warnChartUiLegend qCWarning(CHARTUI_LEGEND_LOG)
#define errorChartUiLegend qCCritical(CHARTUI_LEGEND_LOG)

extern const QLoggingCategory &CHARTUI_PLOTAREA_LOG();

#define debugChartUiPlotArea qCDebug(CHARTUI_PLOTAREA_LOG)<<Q_FUNC_INFO
#define warnChartUiPlotArea qCWarning(CHARTUI_PLOTAREA_LOG)
#define errorChartUiPlotArea qCCritical(CHARTUI_PLOTAREA_LOG)

extern const QLoggingCategory &CHARTUI_AXES_LOG();

#define debugChartUiAxes qCDebug(CHARTUI_AXES_LOG)<<Q_FUNC_INFO
#define warnChartUiAxes qCWarning(CHARTUI_AXES_LOG)
#define errorChartUiAxes qCCritical(CHARTUI_AXES_LOG)

extern const QLoggingCategory &CHARTUI_DATASET_LOG();

#define debugChartUiDataSet qCDebug(CHARTUI_DATASET_LOG)<<Q_FUNC_INFO
#define warnChartUiDataSet qCWarning(CHARTUI_DATASET_LOG)
#define errorChartUiDataSet qCCritical(CHARTUI_DATASET_LOG)

extern const QLoggingCategory &CHARTUI_BUBBLE_LOG();

#define debugChartUiBubble qCDebug(CHARTUI_BUBBLE_LOG)<<Q_FUNC_INFO
#define warnChartUiBubble qCWarning(CHARTUI_BUBBLE_LOG)
#define errorChartUiBubble qCCritical(CHARTUI_BUBBLE_LOG)

extern const QLoggingCategory &CHARTUI_SCATTER_LOG();

#define debugChartUiScatter qCDebug(CHARTUI_SCATTER_LOG)<<Q_FUNC_INFO
#define warnChartUiScatter qCWarning(CHARTUI_SCATTER_LOG)
#define errorChartUiScatter qCCritical(CHARTUI_SCATTER_LOG)

extern const QLoggingCategory &CHARTUI_STOCK_LOG();

#define debugChartUiStock qCDebug(CHARTUI_STOCK_LOG)<<Q_FUNC_INFO
#define warnChartUiStock qCWarning(CHARTUI_STOCK_LOG)
#define errorChartUiStock qCCritical(CHARTUI_STOCK_LOG)

#endif
