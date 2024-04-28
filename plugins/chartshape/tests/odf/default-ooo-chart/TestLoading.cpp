/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2010 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
     Contact: Suresh Chande suresh.chande@nokia.com

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "TestLoading.h"

// KoChart
#include "CellRegion.h"
#include "Legend.h"
#include "PlotArea.h"
#include "TableSource.h"

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
    testInternalTableSize(5, 4);
}

void TestLoading::testDataSets()
{
    Table *table = internalTable();
    QVERIFY(table);
    // y data
    testDataSetCellRegions(0,
                           CellRegion(table, QRect(2, 2, 1, 4)),
                           // series label
                           CellRegion(table, QRect(2, 1, 1, 1)),
                           // categories (specified in x-axis)
                           CellRegion(table, QRect(1, 2, 1, 4)));

    testDataSetCellRegions(1, CellRegion(table, QRect(3, 2, 1, 4)), CellRegion(table, QRect(3, 1, 1, 1)), CellRegion(table, QRect(1, 2, 1, 4)));

    testDataSetCellRegions(2, CellRegion(table, QRect(4, 2, 1, 4)), CellRegion(table, QRect(4, 1, 1, 1)), CellRegion(table, QRect(1, 2, 1, 4)));
}

void TestLoading::testPlotArea()
{
    testElementIsVisible(m_chart->plotArea(), true);
}

void TestLoading::testLegend()
{
    testElementIsVisible(m_chart->legend(), true);
    testLegendElements(QStringList() << "Spalte 1"
                                     << "Spalte 2"
                                     << "Spalte 3");
}

QTEST_MAIN(TestLoading)
