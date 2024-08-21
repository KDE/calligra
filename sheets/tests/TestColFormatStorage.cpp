// This file is part of the KDE project
// SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>
// SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "TestColFormatStorage.h"

#include <KLocalizedString>
#include <QDebug>
#include <QTest>
#include <QVector>

#include "core/ColFormatStorage.h"
#include "core/Map.h"
#include "core/Sheet.h"
#include "engine/calligra_sheets_limits.h"

using namespace Calligra::Sheets;

void TestColFormatStorage::initTestCase()
{
    KLocalizedString::setApplicationDomain("calligrasheets");
    m_map = new Map();
    m_sheet = dynamic_cast<Sheet *>(m_map->addNewSheet());
}

void TestColFormatStorage::cleanupTestCase()
{
    delete m_map;
}

void TestColFormatStorage::testColWidth()
{
    ColFormatStorage s(m_sheet);

    s.setColWidth(10, 100, 42.0);

    for (int col = 1; col < 10000; col++) {
        QCOMPARE(s.colWidth(col), col >= 10 && col <= 100 ? 42.0 : m_map->defaultColumnFormat().width);
    }
    int firstCol, lastCol;
    s.colWidth(5, &lastCol);
    QCOMPARE(lastCol, 9);
    s.colWidth(5, nullptr, &firstCol);
    QCOMPARE(firstCol, 1);
    s.colWidth(50, &lastCol, &firstCol);
    QCOMPARE(firstCol, 10);
    QCOMPARE(lastCol, 100);
    s.colWidth(150, &lastCol);
    QCOMPARE(lastCol, KS_colMax);
}

void TestColFormatStorage::testHidden()
{
    ColFormatStorage s(m_sheet);

    s.setHidden(10, 20, true);
    int first, last;
    QVERIFY(s.isHidden(15, &last, &first));
    QCOMPARE(first, 10);
    QCOMPARE(last, 20);
    QVERIFY(!s.isHidden(5, &last));
    QCOMPARE(last, 9);
    QVERIFY(!s.isHidden(25, nullptr, &first));
    QCOMPARE(first, 21);
}

void TestColFormatStorage::testFiltered()
{
    ColFormatStorage s(m_sheet);

    s.setFiltered(10, 20, true);
    int first, last;
    QVERIFY(s.isFiltered(15, &last, &first));
    QCOMPARE(first, 10);
    QCOMPARE(last, 20);
    QVERIFY(!s.isFiltered(5, &last));
    QCOMPARE(last, 9);
    QVERIFY(!s.isFiltered(25, nullptr, &first));
    QCOMPARE(first, 21);
}

void TestColFormatStorage::testHiddenOrFiltered()
{
    ColFormatStorage s(m_sheet);

    s.setHidden(5, 10, true);
    s.setHidden(20, 30, true);
    s.setFiltered(2, 7, true);
    s.setFiltered(10, 22, true);
    for (int col = 1; col < 10000; col++) {
        if (col < 2 || col > 30) {
            QVERIFY(!s.isHiddenOrFiltered(col));
        } else {
            QVERIFY(s.isHiddenOrFiltered(col));
        }
    }
    int first, last;
    QVERIFY(!s.isHiddenOrFiltered(1, &last, &first));
    QCOMPARE(first, 1);
    QCOMPARE(last, 1);
    QVERIFY(s.isHiddenOrFiltered(3, &last, &first));
    QCOMPARE(first, 2);
    QCOMPARE(last, 4);
    QVERIFY(s.isHiddenOrFiltered(7, &last, &first));
    QCOMPARE(first, 5);
    QCOMPARE(last, 7);
}

void TestColFormatStorage::testVisibleWidth()
{
    ColFormatStorage s(m_sheet);

    s.setColWidth(20, 100, 42.0);
    s.setFiltered(10, 30, true);
    s.setHidden(90, 110, true);

    int first, last;
    QCOMPARE(s.visibleWidth(25, &last, &first), 0.0);
    QCOMPARE(first, 10);
    QCOMPARE(last, 30);
    QCOMPARE(s.visibleWidth(55, &last, &first), 42.0);
    QCOMPARE(first, 31);
    QCOMPARE(last, 89);
    QCOMPARE(s.visibleWidth(97, &last, &first), 0.0);
    QCOMPARE(first, 90);
    QCOMPARE(last, 110);
}

void TestColFormatStorage::testTotalColWidth()
{
    ColFormatStorage s(m_sheet);

    s.setColWidth(1, KS_colMax, 10.0);
    s.setColWidth(10, 19, 13.0);
    s.setColWidth(100, 199, 17.0);

    QCOMPARE(s.totalColWidth(10, 19), 130.0);
    QCOMPARE(s.totalColWidth(9, 10), 23.0);
    QCOMPARE(s.totalColWidth(1, 19), 220.0);
    QCOMPARE(s.totalColWidth(1, KS_colMax), (KS_colMax - 110) * 10.0 + 130.0 + 1700.0);
}

void TestColFormatStorage::testTotalVisibleColWidth()
{
    ColFormatStorage s(m_sheet);

    s.setColWidth(1, KS_colMax, 10.0);
    s.setColWidth(10, 19, 13.0);
    s.setColWidth(100, 199, 17.0);
    s.setHidden(5, 5, true);
    s.setHidden(13, 13, true);
    s.setFiltered(110, 110, true);

    QCOMPARE(s.totalVisibleColWidth(10, 19), 130.0 - 13.0);
    QCOMPARE(s.totalVisibleColWidth(1, 19), 220.0 - 23.0);
    QCOMPARE(s.totalVisibleColWidth(1, KS_colMax), (KS_colMax - 110) * 10.0 + 130.0 + 1700.0 - 40.0);
}

void TestColFormatStorage::testColForPosition()
{
    ColFormatStorage s(m_sheet);

    s.setColWidth(1, KS_colMax, 10.0);
    s.setColWidth(10, 19, 13.0);
    s.setHidden(5, 5, true);

    qreal top;
    QCOMPARE(s.colForPosition(5.0), 1);
    QCOMPARE(s.colForPosition(15.0), 2);
    QCOMPARE(s.colForPosition(45.0), 6);
    QCOMPARE(s.colForPosition(55.0, &top), 7);
    QCOMPARE(top, 50.0);
    QCOMPARE(s.colForPosition(95.0, &top), 11);
    QCOMPARE(top, 93.0);
}

void TestColFormatStorage::testPageBreak()
{
    ColFormatStorage s(m_sheet);

    s.setPageBreak(10, 20, true);
    int first, last;
    QVERIFY(s.hasPageBreak(15, &last, &first));
    QCOMPARE(first, 10);
    QCOMPARE(last, 20);
    QVERIFY(!s.hasPageBreak(5, &last));
    QCOMPARE(last, 9);
    QVERIFY(!s.hasPageBreak(25, nullptr, &first));
    QCOMPARE(first, 21);
}

void TestColFormatStorage::testIsDefaultCol()
{
    ColFormatStorage s(m_sheet);

    s.setFiltered(10, 13, true);
    s.setHidden(15, 18, true);
    s.setPageBreak(21, 25, true);
    s.setColWidth(23, 30, 42.0);

    int first, last;
    QVERIFY(s.isDefaultCol(5, &last, &first));
    QCOMPARE(first, 1);
    QCOMPARE(last, 9);
    QVERIFY(!s.isDefaultCol(13, &last, &first));
    QCOMPARE(first, 10);
    QCOMPARE(last, 13);
    QVERIFY(s.isDefaultCol(14, &last, &first));
    QCOMPARE(first, 14);
    QCOMPARE(last, 14);
    QVERIFY(!s.isDefaultCol(22, &last, &first));
    QCOMPARE(first, 21);
    QCOMPARE(last, 22);
    QVERIFY(!s.isDefaultCol(24, &last, &first));
    QCOMPARE(first, 23);
    QCOMPARE(last, 25);
    QVERIFY(!s.isDefaultCol(27, &last, &first));
    QCOMPARE(first, 26);
    QCOMPARE(last, 30);
}

void TestColFormatStorage::testSetDefault()
{
    ColFormatStorage s(m_sheet);

    s.setFiltered(10, 13, true);
    s.setHidden(15, 18, true);
    s.setPageBreak(21, 25, true);
    s.setColWidth(23, 30, 42.0);

    s.setDefault(5, 16);
    s.setDefault(24, 28);
    QVERIFY(s.isDefaultCol(5));
    QVERIFY(s.isDefaultCol(16));
    QVERIFY(!s.isDefaultCol(17));
    QVERIFY(!s.isDefaultCol(23));
    QVERIFY(s.isDefaultCol(24));
    QVERIFY(s.isDefaultCol(28));
    QVERIFY(!s.isDefaultCol(29));
}

void TestColFormatStorage::testLastNonDefaultCol()
{
    ColFormatStorage s(m_sheet);

    s.setFiltered(10, 13, true);
    QCOMPARE(s.lastNonDefaultCol(), 13);
    s.setHidden(15, 18, true);
    QCOMPARE(s.lastNonDefaultCol(), 18);
    s.setPageBreak(23, 25, true);
    QCOMPARE(s.lastNonDefaultCol(), 25);
    s.setColWidth(15, 30, 42.0);
    QCOMPARE(s.lastNonDefaultCol(), 30);
}

void TestColFormatStorage::testColsAreEqual()
{
    ColFormatStorage s(m_sheet);

    s.setFiltered(10, 13, true);
    s.setHidden(15, 18, true);
    s.setPageBreak(21, 25, true);
    s.setColWidth(23, 30, 42.0);

    QVERIFY(!s.colsAreEqual(9, 10));
    QVERIFY(s.colsAreEqual(10, 12));
    QVERIFY(!s.colsAreEqual(3, 15));
    QVERIFY(s.colsAreEqual(16, 18));
    QVERIFY(!s.colsAreEqual(20, 21));
    QVERIFY(s.colsAreEqual(21, 22));
    QVERIFY(!s.colsAreEqual(22, 23));
    QVERIFY(s.colsAreEqual(23, 25));
    QVERIFY(!s.colsAreEqual(24, 26));
    QVERIFY(s.colsAreEqual(27, 28));
    QVERIFY(!s.colsAreEqual(30, 31));
    QVERIFY(s.colsAreEqual(5, 55));
}

void TestColFormatStorage::testInsertCols()
{
    ColFormatStorage s(m_sheet);

    s.setColWidth(10, 100, 42.0);

    s.insertCols(20, 5);
    s.insertCols(7, 5);
    s.insertCols(150, 5);

    for (int col = 1; col < 10000; col++) {
        QCOMPARE(s.colWidth(col), col >= 15 && col <= 110 ? 42.0 : m_map->defaultColumnFormat().width);
    }
}

void TestColFormatStorage::testRemoveCols()
{
    ColFormatStorage s(m_sheet);

    s.setColWidth(10, 100, 42.0);

    s.removeCols(20, 5);
    s.removeCols(7, 5);
    s.removeCols(85, 10);

    for (int col = 1; col < 10000; col++) {
        QCOMPARE(s.colWidth(col), col >= 7 && col < 85 ? 42.0 : m_map->defaultColumnFormat().width);
    }
}

QTEST_MAIN(TestColFormatStorage)
