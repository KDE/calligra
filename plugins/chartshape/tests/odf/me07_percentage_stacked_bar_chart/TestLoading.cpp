/* This file is part of the KDE project

   @@COPYRIGHT@@

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "TestLoading.h"

// Qt
#include <QStandardItemModel>

// KoChart
#include "ChartShape.h"
#include "Legend.h"
#include "TableSource.h"

using namespace KoChart;

TestLoading::TestLoading()
    : TestLoadingBase()
{
}

void TestLoading::initTestCase()
{
    // Fake sheet data from embedding document
    m_sheet.setRowCount(6);
    m_sheet.setColumnCount(8);
    // Categories
    m_sheet.setData(m_sheet.index(3, 4), "Pass");
    m_sheet.setData(m_sheet.index(3, 5), "Fail");
    m_sheet.setData(m_sheet.index(3, 6), "NA");
    // Series label
    m_sheet.setData(m_sheet.index(4, 3), "Week");
    QVERIFY(tableSource());
    tableSource()->add("Sheet1", &m_sheet);
    // No actual data needed

    // Tell the chart it's embedded
    m_chart->setUsesInternalModelOnly(false);

    TestLoadingBase::initTestCase();
}

void TestLoading::testInternalTable()
{
    QVERIFY(internalTable());
}

void TestLoading::testDataSets()
{
    TableSource *source = tableSource();
    QVERIFY(source);
    // y data
    testDataSetCellRegions(0,
                           CellRegion(source, "Sheet1.E5:G5"),
                           // series label
                           CellRegion(source, "Sheet1.D5"),
                           // categories (specified in x-axis)
                           CellRegion(source, "Sheet1.E4:G4"));
}

void TestLoading::testLegend()
{
    testElementIsVisible(m_chart->legend(), true);
    testLegendElements(QStringList() << "Week");
}

QTEST_MAIN(TestLoading)
