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
    : TestMultipleFiles()
{
    documents << "invalidOODoc" // OO produce(d) invalid cell range address format
              << "validOODoc"; // Same document where cell range format has been fixed
}

void TestLoading::testInvalidOOChart()
{
    testLabels();
    testInternalTable();
    testDataSets();
    testPlotArea();
    testLegend();
    testAxes();
}

void TestLoading::testValidOOChart()
{
    testLabels();
    testInternalTable();
    testDataSets();
    testPlotArea();
    testLegend();
    testAxes();
}

// privates:
void TestLoading::testLabels()
{
    testElementIsVisible(m_chart->title(), false);
    testElementIsVisible(m_chart->subTitle(), false);
    testElementIsVisible(m_chart->footer(), false);
}

void TestLoading::testInternalTable()
{
    testHasOnlyInternalTable();
    testInternalTableSize(5, 5);
}

void TestLoading::testDataSets()
{
    Table *table = internalTable();
    QVERIFY(table);
    // y data
    testDataSetCellRegions(0,
                           CellRegion(table, QRect(2, 2, 1, 4)),
                           // series label
                           CellRegion(table, QRect(3, 1, 1, 1)),
                           // categories (specified in x-axis)
                           CellRegion(table, QRect(1, 2, 1, 4)),
                           // x data
                           CellRegion(),
                           // bubble widths
                           CellRegion(table, QRect(3, 2, 1, 4)));

    testDataSetCellRegions(1,
                           CellRegion(table, QRect(4, 2, 1, 4)),
                           CellRegion(table, QRect(5, 1, 1, 1)),
                           CellRegion(table, QRect(1, 2, 1, 4)),
                           CellRegion(),
                           CellRegion(table, QRect(5, 2, 1, 4)));
}

void TestLoading::testPlotArea()
{
    testElementIsVisible(m_chart->plotArea(), true);
}

void TestLoading::testLegend()
{
    testElementIsVisible(m_chart->legend(), true);
    testLegendElements(QStringList() << "Series 1"
                                     << "Series 2");
}

void TestLoading::testAxes()
{
    testElementIsVisible(m_chart->plotArea()->xAxis()->title(), false);
    testElementIsVisible(m_chart->plotArea()->yAxis()->title(), false);
}

QTEST_MAIN(TestLoading)
