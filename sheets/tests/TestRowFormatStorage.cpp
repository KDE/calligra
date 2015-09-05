/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

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
#include "TestRowFormatStorage.h"

#include <QVector>
#include <QDebug>
#include <QTest>

#include "../RowFormatStorage.h"
#include "../calligra_sheets_limits.h"
#include "../Map.h"
#include "../Sheet.h"
#include "../RowColumnFormat.h"

using namespace Calligra::Sheets;

void TestRowFormatStorage::initTestCase()
{
    m_map = new Map();
    m_sheet = m_map->addNewSheet();
}

void TestRowFormatStorage::cleanupTestCase()
{
    delete m_map;
}

void TestRowFormatStorage::testRowHeight()
{
    RowFormatStorage s(m_sheet);

    s.setRowHeight(10, 100, 42.0);

    for (int row = 1; row < 10000; row++) {
        QCOMPARE(s.rowHeight(row), row >= 10 && row <= 100 ? 42.0 : m_map->defaultRowFormat()->height());
    }
    int firstRow, lastRow;
    s.rowHeight(5, &lastRow);
    QCOMPARE(lastRow, 9);
    s.rowHeight(5, 0, &firstRow);
    QCOMPARE(firstRow, 1);
    s.rowHeight(50, &lastRow, &firstRow);
    QCOMPARE(firstRow, 10);
    QCOMPARE(lastRow, 100);
    s.rowHeight(150, &lastRow);
    QCOMPARE(lastRow, KS_rowMax);
}

void TestRowFormatStorage::testHidden()
{
    RowFormatStorage s(m_sheet);

    s.setHidden(10, 20, true);
    int first, last;
    QVERIFY(s.isHidden(15, &last, &first));
    QCOMPARE(first, 10);
    QCOMPARE(last, 20);
    QVERIFY(!s.isHidden(5, &last));
    QCOMPARE(last, 9);
    QVERIFY(!s.isHidden(25, 0, &first));
    QCOMPARE(first, 21);
}

void TestRowFormatStorage::testFiltered()
{
    RowFormatStorage s(m_sheet);

    s.setFiltered(10, 20, true);
    int first, last;
    QVERIFY(s.isFiltered(15, &last, &first));
    QCOMPARE(first, 10);
    QCOMPARE(last, 20);
    QVERIFY(!s.isFiltered(5, &last));
    QCOMPARE(last, 9);
    QVERIFY(!s.isFiltered(25, 0, &first));
    QCOMPARE(first, 21);
}

void TestRowFormatStorage::testHiddenOrFiltered()
{
    RowFormatStorage s(m_sheet);

    s.setHidden(5, 10, true);
    s.setHidden(20, 30, true);
    s.setFiltered(2, 7, true);
    s.setFiltered(10, 22, true);
    for (int row = 1; row < 10000; row++) {
        if (row < 2 || row > 30) {
            QVERIFY(!s.isHiddenOrFiltered(row));
        } else {
            QVERIFY(s.isHiddenOrFiltered(row));
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

void TestRowFormatStorage::testVisibleHeight()
{
    RowFormatStorage s(m_sheet);

    s.setRowHeight(20, 100, 42.0);
    s.setFiltered(10, 30, true);
    s.setHidden(90, 110, true);

    int first, last;
    QCOMPARE(s.visibleHeight(25, &last, &first), 0.0);
    QCOMPARE(first, 10);
    QCOMPARE(last, 30);
    QCOMPARE(s.visibleHeight(55, &last, &first), 42.0);
    QCOMPARE(first, 31);
    QCOMPARE(last, 89);
    QCOMPARE(s.visibleHeight(97, &last, &first), 0.0);
    QCOMPARE(first, 90);
    QCOMPARE(last, 110);
}

void TestRowFormatStorage::testTotalRowHeight()
{
    RowFormatStorage s(m_sheet);

    s.setRowHeight(1, KS_rowMax, 10.0);
    s.setRowHeight(10, 19, 13.0);
    s.setRowHeight(100, 199, 17.0);

    QCOMPARE(s.totalRowHeight(10, 19), 130.0);
    QCOMPARE(s.totalRowHeight(9, 10), 23.0);
    QCOMPARE(s.totalRowHeight(1, 19), 220.0);
    QCOMPARE(s.totalRowHeight(1, KS_rowMax), (KS_rowMax - 110) * 10.0 + 130.0 + 1700.0);
}

void TestRowFormatStorage::testTotalVisibleRowHeight()
{
    RowFormatStorage s(m_sheet);

    s.setRowHeight(1, KS_rowMax, 10.0);
    s.setRowHeight(10, 19, 13.0);
    s.setRowHeight(100, 199, 17.0);
    s.setHidden(5, 5, true);
    s.setHidden(13, 13, true);
    s.setFiltered(110, 110, true);

    QCOMPARE(s.totalVisibleRowHeight(10, 19), 130.0 - 13.0);
    QCOMPARE(s.totalVisibleRowHeight(1, 19), 220.0 - 23.0);
    QCOMPARE(s.totalVisibleRowHeight(1, KS_rowMax), (KS_rowMax - 110) * 10.0 + 130.0 + 1700.0 - 40.0);
}

void TestRowFormatStorage::testRowForPosition()
{
    RowFormatStorage s(m_sheet);

    s.setRowHeight(1, KS_rowMax, 10.0);
    s.setRowHeight(10, 19, 13.0);
    s.setHidden(5, 5, true);

    qreal top;
    QCOMPARE(s.rowForPosition(5.0), 1);
    QCOMPARE(s.rowForPosition(15.0), 2);
    QCOMPARE(s.rowForPosition(45.0), 6);
    QCOMPARE(s.rowForPosition(55.0, &top), 7);
    QCOMPARE(top, 50.0);
    QCOMPARE(s.rowForPosition(95.0, &top), 11);
    QCOMPARE(top, 93.0);
}

void TestRowFormatStorage::testPageBreak()
{
    RowFormatStorage s(m_sheet);

    s.setPageBreak(10, 20, true);
    int first, last;
    QVERIFY(s.hasPageBreak(15, &last, &first));
    QCOMPARE(first, 10);
    QCOMPARE(last, 20);
    QVERIFY(!s.hasPageBreak(5, &last));
    QCOMPARE(last, 9);
    QVERIFY(!s.hasPageBreak(25, 0, &first));
    QCOMPARE(first, 21);
}

void TestRowFormatStorage::testIsDefaultRow()
{
    RowFormatStorage s(m_sheet);

    s.setFiltered(10, 13, true);
    s.setHidden(15, 18, true);
    s.setPageBreak(21, 25, true);
    s.setRowHeight(23, 30, 42.0);

    int first, last;
    QVERIFY(s.isDefaultRow(5, &last, &first));
    QCOMPARE(first, 1);
    QCOMPARE(last, 9);
    QVERIFY(!s.isDefaultRow(13, &last, &first));
    QCOMPARE(first, 10);
    QCOMPARE(last, 13);
    QVERIFY(s.isDefaultRow(14, &last, &first));
    QCOMPARE(first, 14);
    QCOMPARE(last, 14);
    QVERIFY(!s.isDefaultRow(22, &last, &first));
    QCOMPARE(first, 21);
    QCOMPARE(last, 22);
    QVERIFY(!s.isDefaultRow(24, &last, &first));
    QCOMPARE(first, 23);
    QCOMPARE(last, 25);
    QVERIFY(!s.isDefaultRow(27, &last, &first));
    QCOMPARE(first, 26);
    QCOMPARE(last, 30);
}

void TestRowFormatStorage::testSetDefault()
{
    RowFormatStorage s(m_sheet);

    s.setFiltered(10, 13, true);
    s.setHidden(15, 18, true);
    s.setPageBreak(21, 25, true);
    s.setRowHeight(23, 30, 42.0);

    s.setDefault(5, 16);
    s.setDefault(24, 28);
    QVERIFY(s.isDefaultRow(5));
    QVERIFY(s.isDefaultRow(16));
    QVERIFY(!s.isDefaultRow(17));
    QVERIFY(!s.isDefaultRow(23));
    QVERIFY(s.isDefaultRow(24));
    QVERIFY(s.isDefaultRow(28));
    QVERIFY(!s.isDefaultRow(29));
}

void TestRowFormatStorage::testLastNonDefaultRow()
{
    RowFormatStorage s(m_sheet);

    s.setFiltered(10, 13, true);
    QCOMPARE(s.lastNonDefaultRow(), 13);
    s.setHidden(15, 18, true);
    QCOMPARE(s.lastNonDefaultRow(), 18);
    s.setPageBreak(23, 25, true);
    QCOMPARE(s.lastNonDefaultRow(), 25);
    s.setRowHeight(15, 30, 42.0);
    QCOMPARE(s.lastNonDefaultRow(), 30);
}

void TestRowFormatStorage::testRowsAreEqual()
{
    RowFormatStorage s(m_sheet);

    s.setFiltered(10, 13, true);
    s.setHidden(15, 18, true);
    s.setPageBreak(21, 25, true);
    s.setRowHeight(23, 30, 42.0);

    QVERIFY(!s.rowsAreEqual(9, 10));
    QVERIFY(s.rowsAreEqual(10, 12));
    QVERIFY(!s.rowsAreEqual(3, 15));
    QVERIFY(s.rowsAreEqual(16, 18));
    QVERIFY(!s.rowsAreEqual(20, 21));
    QVERIFY(s.rowsAreEqual(21, 22));
    QVERIFY(!s.rowsAreEqual(22, 23));
    QVERIFY(s.rowsAreEqual(23, 25));
    QVERIFY(!s.rowsAreEqual(24, 26));
    QVERIFY(s.rowsAreEqual(27, 28));
    QVERIFY(!s.rowsAreEqual(30, 31));
    QVERIFY(s.rowsAreEqual(5, 55));
}

void TestRowFormatStorage::testInsertRows()
{
    RowFormatStorage s(m_sheet);

    s.setRowHeight(10, 100, 42.0);

    s.insertRows(20, 5);
    s.insertRows(7, 5);
    s.insertRows(150, 5);

    for (int row = 1; row < 10000; row++) {
        QCOMPARE(s.rowHeight(row), row >= 15 && row <= 110 ? 42.0 : m_map->defaultRowFormat()->height());
    }
}

void TestRowFormatStorage::testRemoveRows()
{
    RowFormatStorage s(m_sheet);

    s.setRowHeight(10, 100, 42.0);

    s.removeRows(20, 5);
    s.removeRows(7, 5);
    s.removeRows(85, 10);

    for (int row = 1; row < 10000; row++) {
        QCOMPARE(s.rowHeight(row), row >= 7 && row < 85 ? 42.0 : m_map->defaultRowFormat()->height());
    }
}

QTEST_MAIN(TestRowFormatStorage)
