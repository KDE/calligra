/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
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
#include "PointStorage.h"

#include "BenchmarkPointStorage.h"

using namespace KSpread;

void PointStorageBenchmark::testInsertionPerformance()
{
    PointStorage<int> storage;
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
            storage.insert( c, r, c );
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
                storage.insert( c, r, c );
                counter += 1;
            }
        }
        ticks += Time::elapsed( start );
    }
    qDebug() << qPrintable( Time::printAverage( ticks, counter ) );
}

void PointStorageBenchmark::testLookupPerformance()
{
    // row x column
    const int scenarios[] = {
        5, 5,          // very small
        30, 20,        // fit to screen
        100, 100,      // medium
        1000, 1000,    // large
        10000, 100,    // typical data: more rows
        10000, 2000,   // and 20 times larger
        100, 10000,    // not really typical: more columns
        8000, 8000     // hopelessly large
        };

    PointStorage<int> storage;

    for (uint sc = 0; sc < sizeof(scenarios)/sizeof(scenarios[0])/2; sc++)
    {
        int maxrow = scenarios[sc*2];
        int maxcol = scenarios[sc*2+1];

        storage.clear();
        for ( int r = 0; r < maxrow; ++r )
        {
            for ( int c = 0; c < maxcol; ++c )
            {
                storage.m_data << c;
                storage.m_cols << ( c + 1 );
            }
            storage.m_rows << r*maxcol;
        }
    //     qDebug() << endl << qPrintable( storage.dump() );
        QString prefix = QString("%1 x %2").arg(maxrow).arg(maxcol);
        qDebug() << "start measuring..." << prefix;

        Time::tval start = 0;
        Time::tval ticks = 0;
        int v;
        int col = 0;
        int row = 0;
        int cols = 0;
        int rows = 0;
        int counter = 0;
        const int iterations = 100000;
        while ( counter < iterations )
        {
            col = 1 + rand() % maxcol;
            row = 1 + rand() % maxrow;
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
        qDebug() << qPrintable( Time::printAverage( ticks, counter, prefix ) );
    }
}

void PointStorageBenchmark::testInsertColumnsPerformance()
{
    PointStorage<int> storage;
    for ( int c = 0; c < KS_colMax; ++c )
    {
        storage.m_data << 1;
        storage.m_cols << 1;
    }
    storage.m_rows << 0;
    qDebug() << "start measuring...";
    Time::tval start = Time::stamp();
    for ( int i = 1; i < 10000; ++i )
        storage.insertColumns( 42, 3 );
    qDebug() << qPrintable( Time::printAverage( Time::elapsed( start ), 10000 ) );
}

void PointStorageBenchmark::testDeleteColumnsPerformance()
{
    PointStorage<int> storage;
    for ( int c = 0; c < KS_colMax; ++c )
    {
        storage.m_data << 1;
        storage.m_cols << 1;
    }
    storage.m_rows << 0;
    qDebug() << "start measuring...";
    Time::tval start = Time::stamp();
    for ( int i = 1; i < 10000; ++i )
        storage.removeColumns( 42, 3 );
    qDebug() << qPrintable( Time::printAverage( Time::elapsed( start ), 10000 ) );
}

void PointStorageBenchmark::testInsertRowsPerformance()
{
    PointStorage<int> storage;
    for ( int r = 0; r < KS_rowMax; ++r )
    {
        storage.m_data << 1;
        storage.m_cols << 1;
        storage.m_rows << r;
    }
    qDebug() << "start measuring...";
    Time::tval start = Time::stamp();
    for ( int i = 1; i < 10000; ++i )
        storage.insertRows( 42, 3 );
    qDebug() << qPrintable( Time::printAverage( Time::elapsed( start ), 10000 ) );
}

void PointStorageBenchmark::testDeleteRowsPerformance()
{
    PointStorage<int> storage;
    for ( int r = 0; r < KS_rowMax; ++r )
    {
        storage.m_data << 1;
        storage.m_cols << 1;
        storage.m_rows << r;
    }
    qDebug() << "start measuring...";
    Time::tval start = Time::stamp();
    for ( int i = 1; i < 10000; ++i )
        storage.removeRows( 42, 3 );
    qDebug() << qPrintable( Time::printAverage( Time::elapsed( start ), 10000 ) );
}

void PointStorageBenchmark::testShiftLeftPerformance()
{
    PointStorage<int> storage;
    for ( int c = 0; c < KS_colMax; ++c )
    {
        storage.m_data << 1;
        storage.m_cols << 1;
    }
    storage.m_rows << 0;
    qDebug() << "start measuring...";
    Time::tval start = Time::stamp();
    for ( int i = 1; i < 10000; ++i )
        storage.removeShiftLeft( QRect( 42, 1, 3, 1) );
    qDebug() << qPrintable( Time::printAverage( Time::elapsed( start ), 10000 ) );
}

void PointStorageBenchmark::testShiftRightPerformance()
{
    PointStorage<int> storage;
    for ( int c = 0; c < KS_colMax; ++c )
    {
        storage.m_data << 1;
        storage.m_cols << 1;
    }
    storage.m_rows << 0;
    qDebug() << "start measuring...";
    Time::tval start = Time::stamp();
    for ( int i = 1; i < 10000; ++i )
        storage.insertShiftRight( QRect( 42, 1, 3, 1) );
    qDebug() << qPrintable( Time::printAverage( Time::elapsed( start ), 10000 ) );
}


QTEST_MAIN(PointStorageBenchmark)

#include "BenchmarkPointStorage.moc"
