/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2010 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
     Contact: Suresh Chande suresh.chande@nokia.com

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "TestLoading.h"

// KoChart
#include "ChartShape.h"
#include "Legend.h"
#include "PlotArea.h"

using namespace KoChart;

TestLoading::TestLoading()
    : TestLoadingBase()
{
}

void TestLoading::testInternalTable()
{
    testHasOnlyInternalTable();
    testInternalTableSize(5, 4);
}
void TestLoading::testPlotArea()
{
    testElementIsVisible(m_chart->plotArea(), true);
}

void TestLoading::testLegend()
{
    testElementIsVisible(m_chart->legend(), true);
    testLegendElements(QStringList() << "Row 1"
                                     << "Row 2"
                                     << "Row 3"
                                     << "Row 4");
}

QTEST_MAIN(TestLoading)
