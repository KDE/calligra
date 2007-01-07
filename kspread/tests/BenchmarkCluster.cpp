/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

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
    int counter = 0;
    const int iterations = 100000;
    start = Time::stamp();
    for ( int r = row; r <= rows; ++r )
    {
        for ( int c = col; c <= cols; c += 1 )
        {
            storage.insert( cell, c, r );
            counter += 1;
        }
    }
    ticks = Time::elapsed( start );
    qDebug() << qPrintable( Time::printAverage( ticks, counter ) );

    qDebug() << "measuring random singular insertion...";
    storage.clear();
    counter = 0;
    while ( counter < iterations )
    {
        col = 1 + rand() % 1000;
        row = 1 + rand() % 1000;
        cols = col + 1;
        rows = row + 1;
        start = Time::stamp();
        for ( int r = row; r <= rows; ++r )
        {
            for ( int c = col; c <= cols && counter < iterations; c += 1 )
            {
                storage.insert( cell, c, r );
                counter += 1;
            }
        }
        ticks += Time::elapsed( start );
    }
    qDebug() << qPrintable( Time::printAverage( ticks, counter ) );
}

void ClusterBenchmark::testLookupPerformance()
{
    Cluster storage;
    Cell* cell = 0;
    for ( int r = 1; r <= 1000; ++r )
    {
        for ( int c = 1; c <= 1000; ++c )
        {
            storage.insert( cell, c, r );
        }
    }
//     qDebug() << endl << qPrintable( storage.dump() );
    qDebug() << "start measuring...";

    Time::tval start = 0;
    Time::tval ticks = 0;
    Cell* v;
    int col = 0;
    int row = 0;
    int cols = 0;
    int rows = 0;
    int counter = 0;
    const int iterations = 100000;
    while ( counter < iterations )
    {
        col = 1 + rand() % 1000;
        row = 1 + rand() % 1000;
        cols = col + 1 * ( rand() % 10 );
        rows = row + rand() % 10;
        start = Time::stamp();
        for ( int r = row; r <= rows && counter < iterations; ++r )
        {
            for ( int c = col; c <= cols && counter < iterations; c += 1 )
            {
                v = storage.lookup( c, r );
                counter += 1;
            }
        }
        ticks += Time::elapsed( start );
    }
    qDebug() << qPrintable( Time::printAverage( ticks, counter ) );
}

void ClusterBenchmark::testInsertColumnsPerformance()
{
    Cluster storage;
    Cell* cell = 0;
    for ( int c = 1; c <= KS_colMax; ++c )
        storage.insert( cell, c, 1 );
    qDebug() << "start measuring...";

    Time::tval start = Time::stamp();
    for ( int i = 1; i < 100; ++i )
        storage.insertColumn( 250 ); // near a cluster border
    qDebug() << qPrintable( Time::printAverage( Time::elapsed( start ), 100 ) );
}

void ClusterBenchmark::testDeleteColumnsPerformance()
{
    Cluster storage;
    Cell* cell = 0;
    for ( int c = 1; c <= KS_colMax; ++c )
        storage.insert( cell, c, 1 );
    qDebug() << "start measuring...";

    Time::tval start = Time::stamp();
    for ( int i = 1; i < 100; ++i )
        storage.removeColumn( 250 ); // near a cluster border
    qDebug() << qPrintable( Time::printAverage( Time::elapsed( start ), 100 ) );
}

void ClusterBenchmark::testInsertRowsPerformance()
{
    Cluster storage;
    Cell* cell = 0;
    for ( int r = 1; r <= KS_rowMax; ++r )
        storage.insert( cell, 1, r );
    qDebug() << "start measuring...";

    Time::tval start = Time::stamp();
    for ( int i = 1; i < 100; ++i )
        storage.insertRow( 250 ); // near a cluster border
    qDebug() << qPrintable( Time::printAverage( Time::elapsed( start ), 100 ) );
}

void ClusterBenchmark::testDeleteRowsPerformance()
{
    Cluster storage;
    Cell* cell = 0;
    for ( int r = 1; r <= KS_rowMax; ++r )
        storage.insert( cell, 1, r );
    qDebug() << "start measuring...";

    Time::tval start = Time::stamp();
    for ( int i = 1; i < 100; ++i )
        storage.removeRow( 250 ); // near a cluster border
    qDebug() << qPrintable( Time::printAverage( Time::elapsed( start ), 100 ) );
}

void ClusterBenchmark::testShiftLeftPerformance()
{
    Cluster storage;
    Cell* cell = 0;
    for ( int c = 1; c <= KS_colMax; ++c )
        storage.insert( cell, c, 1 );
    qDebug() << "start measuring...";

    Time::tval start = Time::stamp();
    for ( int i = 1; i < 1000; ++i )
        storage.unshiftRow( QPoint( 42, 1 ) );
    qDebug() << qPrintable( Time::printAverage( Time::elapsed( start ), 1000 ) );
}

void ClusterBenchmark::testShiftRightPerformance()
{
    Cluster storage;
    Cell* cell = 0;
    for ( int c = 1; c <= KS_colMax; ++c )
        storage.insert( cell, c, 1 );
    qDebug() << "start measuring...";

    Time::tval start = Time::stamp();
    for ( int i = 1; i < 1000; ++i )
        storage.shiftRow( QPoint( 42, 1 ) );
    qDebug() << qPrintable( Time::printAverage( Time::elapsed( start ), 1000 ) );
}

void ClusterBenchmark::testShiftUpPerformance()
{
    Cluster storage;
    Cell* cell = 0;
    for ( int r = 1; r <= KS_rowMax; ++r )
        storage.insert( cell, 1, r );
    qDebug() << "start measuring...";

    Time::tval start = Time::stamp();
    for ( int i = 1; i < 1000; ++i )
        storage.unshiftColumn( QPoint( 1, 42 ) );
    qDebug() << qPrintable( Time::printAverage( Time::elapsed( start ), 1000 ) );
}

void ClusterBenchmark::testShiftDownPerformance()
{
    Cluster storage;
    Cell* cell = 0;
    for ( int r = 1; r <= KS_rowMax; ++r )
        storage.insert( cell, 1, r );
    qDebug() << "start measuring...";

    Time::tval start = Time::stamp();
    for ( int i = 1; i < 1000; ++i )
        storage.shiftColumn( QPoint( 1, 42 ) );
    qDebug() << qPrintable( Time::printAverage( Time::elapsed( start ), 1000 ) );
}



QTEST_MAIN(ClusterBenchmark)

#include "BenchmarkCluster.moc"
