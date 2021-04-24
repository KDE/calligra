/*
   SPDX-FileCopyrightText: 2017 Dag Andersen <danders@get2net.dk>
   SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ChartDebug.h"

const QLoggingCategory &CHART_LOG()
{
    static const QLoggingCategory category("calligra.plugin.chart.shape");
    return category;
}

const QLoggingCategory &CHARTPARSE_LOG()
{
    static const QLoggingCategory category("calligra.plugin.chart.parse");
    return category;
}

const QLoggingCategory &CHARTLAYOUT_LOG()
{
    static const QLoggingCategory category("calligra.plugin.chart.layout");
    return category;
}

const QLoggingCategory &CHARTODF_LOG()
{
    static const QLoggingCategory category("calligra.plugin.chart.odf");
    return category;
}

const QLoggingCategory &CHARTTOOL_LOG()
{
    static const QLoggingCategory category("calligra.plugin.chart.tool");
    return category;
}

const QLoggingCategory &CHARTAXIS_LOG()
{
    static const QLoggingCategory category("calligra.plugin.chart.axis");
    return category;
}

const QLoggingCategory &CHARTDADASET_LOG()
{
    static const QLoggingCategory category("calligra.plugin.chart.dataset");
    return category;
}

const QLoggingCategory &CHARTUI_TITLES_LOG()
{
    static const QLoggingCategory category("calligra.plugin.chart.ui.titles");
    return category;
}

const QLoggingCategory &CHARTUI_LEGEND_LOG()
{
    static const QLoggingCategory category("calligra.plugin.chart.ui.legend");
    return category;
}

const QLoggingCategory &CHARTUI_PLOTAREA_LOG()
{
    static const QLoggingCategory category("calligra.plugin.chart.ui.plotarea");
    return category;
}

const QLoggingCategory &CHARTUI_AXES_LOG()
{
    static const QLoggingCategory category("calligra.plugin.chart.ui.axes");
    return category;
}

const QLoggingCategory &CHARTUI_DATASET_LOG()
{
    static const QLoggingCategory category("calligra.plugin.chart.ui.dataset");
    return category;
}

const QLoggingCategory &CHARTUI_BUBBLE_LOG()
{
    static const QLoggingCategory category("calligra.plugin.chart.ui.bubble");
    return category;
}

const QLoggingCategory &CHARTUI_SCATTER_LOG()
{
    static const QLoggingCategory category("calligra.plugin.chart.ui.scatter");
    return category;
}

const QLoggingCategory &CHARTUI_STOCK_LOG()
{
    static const QLoggingCategory category("calligra.plugin.chart.ui.stock");
    return category;
}
