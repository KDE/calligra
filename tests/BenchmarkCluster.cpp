/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "BenchmarkHelper.h"
#include "Cluster.h"
#include "Global.h"

#include "BenchmarkCluster.h"

using namespace KSpread;
class Cell;

void ClusterBenchmark::testInsertionPerformance()
{
    Cluster storage;
    Cell* cell = 0;
    qDebug() << "measuring loading-like insertion...";
    Time::tval start = 0;
    Time::tval ticks = 0;
    int col = 1;
    int row = 1;
    int cols = 100;
    int rows = 10000;
    long counter = 0;
    start = Time::stamp();
    for (int r = row; r <= rows; ++r) {
        for (int c = col; c <= cols; c += 1) {
            storage.insert(cell, c, r);
            counter += 1;
        }
    }
    ticks = Time::elapsed(start);
    qDebug() << qPrintable(Time::printAverage(ticks, counter));

    qDebug() << "measuring random singular insertion...";
    storage.clear();
    counter = 0;
    while (counter < Time::iterations) {
        col = 1 + rand() % 1000;
        row = 1 + rand() % 1000;
        cols = col + 1;
        rows = row + 1;
        start = Time::stamp();
        for (int r = row; r <= rows; ++r) {
            for (int c = col; c <= cols && counter < Time::iterations; c += 1) {
                storage.insert(cell, c, r);
                counter += 1;
            }
        }
        ticks += Time::elapsed(start);
    }
    qDebug() << qPrintable(Time::printAverage(ticks, counter));
}

void ClusterBenchmark::testLookupPerformance()
{
    // row x column
    const int scenarios[] = {
#if 1
        1000, 1000    // large
#else
        5, 5,          // very small
        30, 20,        // fit to screen
        100, 100,      // medium
        1000, 1000,    // large
        10000, 100,    // typical data: more rows
        10000, 2000,   // and 20 times larger
        100, 10000,    // not really typical: more columns
        8000, 8000     // hopelessly large
#endif
    };

    Cluster storage;
    Cell* cell = 0;

    for (uint sc = 0; sc < sizeof(scenarios) / sizeof(scenarios[0]) / 2; sc++) {
        int maxrow = scenarios[sc*2];
        int maxcol = scenarios[sc*2+1];

        storage.clear();
#if 0
        for (int r = 0; r < maxrow; ++r) {
            for (int c = 0; c < maxcol; ++c) {
                storage.insert(cell, c, r);
            }
        }
#else
        storage.insert(cell, 1, 1);
        storage.insert(cell, maxcol / 2, maxrow / 2);
        storage.insert(cell, maxcol / 3, maxrow / 3);
        storage.insert(cell, maxcol, maxrow);
#endif
        //     qDebug() << endl << qPrintable( storage.dump() );
        QString prefix = QString("%1 x %2").arg(maxrow).arg(maxcol);
        qDebug() << "start measuring..." << prefix;

        Time::tval start = 0;
        Time::tval ticks = 0;
        Cell* v;
        int col = 0;
        int row = 0;
        int cols = 0;
        int rows = 0;
        long counter = 0;
        while (counter < Time::iterations) {
            col = 1 + rand() % maxcol;
            row = 1 + rand() % maxrow;
            cols = col + 1 * (rand() % 10);
            rows = row + rand() % 10;
            start = Time::stamp();
            for (int r = row; r <= rows && counter < Time::iterations; ++r) {
                for (int c = col; c <= cols && counter < Time::iterations; c += 1) {
                    v = storage.lookup(c, r);
                    counter += 1;
                }
            }
            ticks += Time::elapsed(start);
        }
        qDebug() << qPrintable(Time::printAverage(ticks, counter, prefix));
    }
}

void ClusterBenchmark::testInsertColumnsPerformance()
{
    Cluster storage;
    Cell* cell = 0;
    for (int c = 1; c <= KS_colMax; ++c)
        storage.insert(cell, c, 1);
    qDebug() << "start measuring...";

    Time::tval start = Time::stamp();
    for (int i = 1; i < 100; ++i)
        storage.insertColumn(250);   // near a cluster border
    qDebug() << qPrintable(Time::printAverage(Time::elapsed(start), 100));
}

void ClusterBenchmark::testDeleteColumnsPerformance()
{
    Cluster storage;
    Cell* cell = 0;
    for (int c = 1; c <= KS_colMax; ++c)
        storage.insert(cell, c, 1);
    qDebug() << "start measuring...";

    Time::tval start = Time::stamp();
    for (int i = 1; i < 100; ++i)
        storage.removeColumn(250);   // near a cluster border
    qDebug() << qPrintable(Time::printAverage(Time::elapsed(start), 100));
}

void ClusterBenchmark::testInsertRowsPerformance()
{
    Cluster storage;
    Cell* cell = 0;
    for (int r = 1; r <= KS_rowMax; ++r)
        storage.insert(cell, 1, r);
    qDebug() << "start measuring...";

    Time::tval start = Time::stamp();
    for (int i = 1; i < 100; ++i)
        storage.insertRow(250);   // near a cluster border
    qDebug() << qPrintable(Time::printAverage(Time::elapsed(start), 100));
}

void ClusterBenchmark::testDeleteRowsPerformance()
{
    Cluster storage;
    Cell* cell = 0;
    for (int r = 1; r <= KS_rowMax; ++r)
        storage.insert(cell, 1, r);
    qDebug() << "start measuring...";

    Time::tval start = Time::stamp();
    for (int i = 1; i < 100; ++i)
        storage.removeRow(250);   // near a cluster border
    qDebug() << qPrintable(Time::printAverage(Time::elapsed(start), 100));
}

void ClusterBenchmark::testShiftLeftPerformance()
{
    Cluster storage;
    Cell* cell = 0;
    for (int c = 1; c <= KS_colMax; ++c)
        storage.insert(cell, c, 1);
    qDebug() << "start measuring...";

    Time::tval start = Time::stamp();
    for (int i = 1; i < 1000; ++i)
        storage.removeShiftLeft(QPoint(42, 1));
    qDebug() << qPrintable(Time::printAverage(Time::elapsed(start), 1000));
}

void ClusterBenchmark::testShiftRightPerformance()
{
    Cluster storage;
    Cell* cell = 0;
    for (int c = 1; c <= KS_colMax; ++c)
        storage.insert(cell, c, 1);
    qDebug() << "start measuring...";

    Time::tval start = Time::stamp();
    for (int i = 1; i < 1000; ++i)
        storage.insertShiftRight(QPoint(42, 1));
    qDebug() << qPrintable(Time::printAverage(Time::elapsed(start), 1000));
}

void ClusterBenchmark::testShiftUpPerformance()
{
    Cluster storage;
    Cell* cell = 0;
    for (int r = 1; r <= KS_rowMax; ++r)
        storage.insert(cell, 1, r);
    qDebug() << "start measuring...";

    Time::tval start = Time::stamp();
    for (int i = 1; i < 1000; ++i)
        storage.removeShiftUp(QPoint(1, 42));
    qDebug() << qPrintable(Time::printAverage(Time::elapsed(start), 1000));
}

void ClusterBenchmark::testShiftDownPerformance()
{
    Cluster storage;
    Cell* cell = 0;
    for (int r = 1; r <= KS_rowMax; ++r)
        storage.insert(cell, 1, r);
    qDebug() << "start measuring...";

    Time::tval start = Time::stamp();
    for (int i = 1; i < 1000; ++i)
        storage.insertShiftDown(QPoint(1, 42));
    qDebug() << qPrintable(Time::printAverage(Time::elapsed(start), 1000));
}



QTEST_MAIN(ClusterBenchmark)

#include "BenchmarkCluster.moc"
