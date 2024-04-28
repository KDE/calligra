/* This file is part of the KDE project

   @@COPYRIGHT@@

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "TestLoading.h"

// KoChart
#include "Axis.h"
#include "ChartShape.h"
#include "Legend.h"
#include "PlotArea.h"

using namespace KoChart;

TestLoading::TestLoading()
    : TestLoadingBase()
{
}

void TestLoading::testLabels()
{
    testElementIsVisible(m_chart->title(), false);
    testElementIsVisible(m_chart->subTitle(), false);
    testElementIsVisible(m_chart->footer(), false);
}

void TestLoading::testInternalTable()
{
    testHasOnlyInternalTable();
    testInternalTableSize(4, 5);
}

void TestLoading::testDataSets()
{
    Table *table = internalTable();
    QVERIFY(table);
    // y data
    testDataSetCellRegions(0,
                           CellRegion(table, QRect(2, 2, 1, 3)),
                           // series label
                           CellRegion(table, QRect(2, 1, 1, 1)),
                           // categories (specified in x-axis)
                           CellRegion(table, QRect(1, 2, 1, 3)));

    testDataSetCellRegions(1, CellRegion(table, QRect(3, 2, 1, 3)), CellRegion(table, QRect(3, 1, 1, 1)), CellRegion(table, QRect(1, 2, 1, 3)));

    testDataSetCellRegions(2, CellRegion(table, QRect(4, 2, 1, 3)), CellRegion(table, QRect(4, 1, 1, 1)), CellRegion(table, QRect(1, 2, 1, 3)));

    testDataSetCellRegions(3, CellRegion(table, QRect(5, 2, 1, 3)), CellRegion(table, QRect(5, 1, 1, 1)), CellRegion(table, QRect(1, 2, 1, 3)));
}

void TestLoading::testPlotArea()
{
    testElementIsVisible(m_chart->plotArea(), true);
}

void TestLoading::testLegend()
{
    testElementIsVisible(m_chart->legend(), true);
    testLegendElements(QStringList() << "Column 1"
                                     << "Column 2"
                                     << "Column 3"
                                     << "Column 4");
}

void TestLoading::testAxes()
{
    testElementIsVisible(m_chart->plotArea()->xAxis()->title(), true);
    testAxisTitle(m_chart->plotArea()->xAxis(), "Month");
    testElementIsVisible(m_chart->plotArea()->yAxis()->title(), true);
    testAxisTitle(m_chart->plotArea()->yAxis(), "Growth in %");
}

QTEST_MAIN(TestLoading)
