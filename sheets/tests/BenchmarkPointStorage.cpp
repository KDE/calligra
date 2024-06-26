/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "BenchmarkPointStorage.h"

#include "engine/PointStorage.h"
#include "engine/calligra_sheets_limits.h"

#include <QTest>
#include <QUuid>

using namespace Calligra::Sheets;

void PointStorageBenchmark::testInsertionPerformance_loadingLike()
{
    PointStorage<int> storage;
    int col = 1;
    int row = 1;
    int cols = 100;
    int rows = 10000;
    QBENCHMARK {
        for (int r = row; r <= rows; ++r) {
            for (int c = col; c <= cols; c += 1) {
                storage.insert(c, r, c);
            }
        }
    }
}

void PointStorageBenchmark::testInsertionPerformance_singular()
{
    PointStorage<int> storage;
    QBENCHMARK {
        int col = 1 + rand() % 1000;
        int row = 1 + rand() % 1000;
        int cols = col + 1;
        int rows = row + 1;
        for (int r = row; r <= rows; ++r) {
            for (int c = col; c <= cols; c += 1) {
                storage.insert(c, r, c);
            }
        }
    }
}

void PointStorageBenchmark::testLookupPerformance_data()
{
    QTest::addColumn<int>("maxrow");
    QTest::addColumn<int>("maxcol");

    QTest::newRow("very small") << 5 << 5;
    QTest::newRow("fit to screen") << 30 << 20;
    QTest::newRow("medium") << 100 << 100;
    QTest::newRow("large") << 1000 << 1000;
    QTest::newRow("typical data: more rows") << 10000 << 100;
    QTest::newRow("20 times larger") << 10000 << 2000;
    QTest::newRow("not really typical: more columns") << 100 << 10000;
    QTest::newRow("hopelessly large") << 8000 << 8000;
    QTest::newRow("some complete columns; KS_colMax-10, because of max lookup range of width 10 below") << 10 << 32757;
    QTest::newRow("some complete rows; KS_rowMax-10, because of max lookup range of height 10 below") << 32757 << 10;
}

void PointStorageBenchmark::testLookupPerformance()
{
    PointStorage<int> storage;
    QFETCH(int, maxrow);
    QFETCH(int, maxcol);

    for (int r = 0; r < maxrow; ++r) {
        for (int c = 0; c < maxcol; ++c) {
            storage.m_data << c;
            storage.m_cols << (c + 1);
        }
        storage.m_rows << r * maxcol;
    }

    //     qDebug() << endl << qPrintable( storage.dump() );

    int v;
    int col = 0;
    int row = 0;
    int cols = 0;
    int rows = 0;
    QBENCHMARK {
        col = 1 + rand() % maxcol;
        row = 1 + rand() % maxrow;
        cols = col + 1 * (rand() % 10);
        rows = row + rand() % 10;
        for (int r = row; r <= rows; ++r) {
            for (int c = col; c <= cols; c += 1) {
                v = storage.lookup(c, r);
            }
        }
    }
    Q_UNUSED(v); // not fully unused but GCC thinks so, so let us just tell it so
}

void PointStorageBenchmark::testInsertColumnsPerformance()
{
    PointStorage<int> storage;
    for (int c = 0; c < KS_colMax; ++c) {
        storage.m_data << 1;
        storage.m_cols << 1;
    }
    storage.m_rows << 0;
    QBENCHMARK {
        storage.insertColumns(42, 3);
    }
}

void PointStorageBenchmark::testDeleteColumnsPerformance()
{
    PointStorage<int> storage;
    for (int c = 0; c < KS_colMax; ++c) {
        storage.m_data << 1;
        storage.m_cols << 1;
    }
    storage.m_rows << 0;
    QBENCHMARK {
        storage.removeColumns(42, 3);
    }
}

void PointStorageBenchmark::testInsertRowsPerformance()
{
    PointStorage<int> storage;
    for (int r = 0; r < KS_rowMax; ++r) {
        storage.m_data << 1;
        storage.m_cols << 1;
        storage.m_rows << r;
    }
    QBENCHMARK {
        storage.insertRows(42, 3);
    }
}

void PointStorageBenchmark::testDeleteRowsPerformance()
{
    PointStorage<int> storage;
    for (int r = 0; r < KS_rowMax; ++r) {
        storage.m_data << 1;
        storage.m_cols << 1;
        storage.m_rows << r;
    }
    QBENCHMARK {
        storage.removeRows(42, 3);
    }
}

void PointStorageBenchmark::testShiftLeftPerformance()
{
    PointStorage<int> storage;
    for (int c = 0; c < KS_colMax; ++c) {
        storage.m_data << 1;
        storage.m_cols << 1;
    }
    storage.m_rows << 0;
    QBENCHMARK {
        storage.removeShiftLeft(QRect(42, 1, 3, 1));
    }
}

void PointStorageBenchmark::testShiftRightPerformance()
{
    PointStorage<int> storage;
    for (int c = 0; c < KS_colMax; ++c) {
        storage.m_data << 1;
        storage.m_cols << 1;
    }
    storage.m_rows << 0;
    QBENCHMARK {
        storage.insertShiftRight(QRect(42, 1, 3, 1));
    }
}

void PointStorageBenchmark::testShiftUpPerformance()
{
    PointStorage<int> storage;
    for (int r = 0; r < KS_rowMax; ++r) {
        storage.m_data << 1;
        storage.m_cols << 1;
        storage.m_rows << r;
    }
    QBENCHMARK {
        storage.removeShiftUp(QRect(1, 42, 1, 3));
    }
}

void PointStorageBenchmark::testShiftDownPerformance()
{
    PointStorage<int> storage;
    for (int r = 0; r < KS_rowMax; ++r) {
        storage.m_data << 1;
        storage.m_cols << 1;
        storage.m_rows << r;
    }
    storage.m_rows << 0;
    QBENCHMARK {
        storage.insertShiftDown(QRect(1, 42, 1, 3));
    }
}

void PointStorageBenchmark::testIterationPerformance_data()
{
    QTest::addColumn<int>("maxrow");
    QTest::addColumn<int>("maxcol");

    QTest::newRow("very small") << 5 << 5;
    QTest::newRow("fit to screen") << 30 << 20;
    QTest::newRow("medium") << 100 << 100;
    QTest::newRow("large") << 1000 << 1000;
    QTest::newRow("typical data: more rows") << 10000 << 100;
    QTest::newRow("20 times larger") << 10000 << 2000;
    QTest::newRow("not really typical: more columns") << 100 << 10000;
    QTest::newRow("hopelessly large") << 8000 << 8000;
#if 0
    QTest::newRow("some complete columns; KS_colMax-10, because of max lookup range of width 10 below") << 10 << 32757;
    QTest::newRow("some complete rows; KS_rowMax-10, because of max lookup range of height 10 below") << 32757 << 10;
#endif
}

void PointStorageBenchmark::testIterationPerformance()
{
    PointStorage<int> storage;

    QFETCH(int, maxrow);
    QFETCH(int, maxcol);

    storage.clear();
    for (int r = 0; r < maxrow; ++r) {
        for (int c = 0; c < maxcol; ++c) {
            storage.m_data << c;
            storage.m_cols << (c + 1);
        }
        storage.m_rows << r * maxcol;
    }

    //     qDebug() << endl << qPrintable( storage.dump() );

    int v;
    QBENCHMARK {
        for (int i = 0; i < storage.count(); ++i) {
            v = storage.data(i);
        }
    }
    Q_UNUSED(v); // Not fully unused, but GCC thinks so
}

QTEST_MAIN(PointStorageBenchmark)
