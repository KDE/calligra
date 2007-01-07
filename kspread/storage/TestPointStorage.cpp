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

#include "../tests/BenchmarkHelper.h"
#include "PointStorage.h"

#include "TestPointStorage.h"

using namespace KSpread;

void PointStorageTest::testInsertion()
{
    PointStorage<int> storage;
//     int counter = 1;
//     for ( int col = 1; col <= 5; ++col )
//     for ( int row = 1; row <= 5; ++row )
//     cout << qPrintable( QString( "storage.insert( %1, %2, %3 );" ).arg(col,2).arg(row,2).arg(counter++,2) ) << endl;    storage.insert(  1,  1, 1 );
    storage.insert(  1,  1,  1 );
    storage.insert(  1,  2,  2 );
    storage.insert(  1,  3,  3 );
    storage.insert(  1,  4,  4 );
    storage.insert(  1,  5,  5 );
    storage.insert(  2,  1,  6 );
    storage.insert(  2,  2,  7 );
    storage.insert(  2,  3,  8 );
    storage.insert(  2,  4,  9 );
    storage.insert(  2,  5, 10 );
    storage.insert(  3,  1, 11 );
    storage.insert(  3,  2, 12 );
    storage.insert(  3,  3, 13 );
    storage.insert(  3,  4, 14 );
    storage.insert(  3,  5, 15 );
    storage.insert(  4,  1, 16 );
    storage.insert(  4,  2, 17 );
    storage.insert(  4,  3, 18 );
    storage.insert(  4,  4, 19 );
    storage.insert(  4,  5, 20 );
    storage.insert(  5,  1, 21 );
    storage.insert(  5,  2, 22 );
    storage.insert(  5,  3, 23 );
    storage.insert(  5,  4, 24 );
    storage.insert(  5,  5, 25 );
    // overwrite
    int old = storage.insert(  5,  5, 30 );
    QCOMPARE( old, 25 );
    // ( 1, 6,11,16,21)
    // ( 2, 7,12,17,22)
    // ( 3, 8,13,18,23)
    // ( 4, 9,14,19,24)
    // ( 5,10,15,20,25)

    const QVector<int> data( QVector<int>() << 1<< 6 << 11 << 16 << 21
                                        << 2 << 7 << 12 << 17 << 22
                                        << 3 << 8 << 13 << 18 << 23
                                        << 4 << 9 << 14 << 19 << 24
                                        << 5 << 10 << 15 << 20 << 30 );
    const QVector<int> rows( QVector<int>() << 0 << 5 << 10 << 15 << 20 );
    const QVector<int> cols( QVector<int>() << 1 << 2 << 3 << 4 << 5
                                        << 1 << 2 << 3 << 4 << 5
                                        << 1 << 2 << 3 << 4 << 5
                                        << 1 << 2 << 3 << 4 << 5
                                        << 1 << 2 << 3 << 4 << 5 );

    QCOMPARE( storage.m_data, data );
    QCOMPARE( storage.m_rows, rows );
    QCOMPARE( storage.m_cols, cols );
}

void PointStorageTest::testLookup()
{
    PointStorage<int> storage;
    storage.m_data << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    storage.m_rows << 0 << 3 << 6 << 9 << 10;
    storage.m_cols << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4 << 1 << 5;
    // ( 1, 2,  ,  , 3)
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,12)

    QCOMPARE( storage.lookup( 1, 1),  1 );
    QCOMPARE( storage.lookup( 1, 2),  4 );
    QCOMPARE( storage.lookup( 1, 3),  0 );
    QCOMPARE( storage.lookup( 1, 4),  0 );
    QCOMPARE( storage.lookup( 1, 5), 11 );
    QCOMPARE( storage.lookup( 2, 1),  2 );
    QCOMPARE( storage.lookup( 2, 2),  5 );
    QCOMPARE( storage.lookup( 2, 3),  7 );
    QCOMPARE( storage.lookup( 2, 4),  0 );
    QCOMPARE( storage.lookup( 2, 5),  0 );
    QCOMPARE( storage.lookup( 3, 1),  0 );
    QCOMPARE( storage.lookup( 3, 2),  6 );
    QCOMPARE( storage.lookup( 3, 3),  8 );
    QCOMPARE( storage.lookup( 3, 4),  0 );
    QCOMPARE( storage.lookup( 3, 5),  0 );
    QCOMPARE( storage.lookup( 4, 1),  0 );
    QCOMPARE( storage.lookup( 4, 2),  0 );
    QCOMPARE( storage.lookup( 4, 3),  0 );
    QCOMPARE( storage.lookup( 4, 4), 10 );
    QCOMPARE( storage.lookup( 4, 5),  0 );
    QCOMPARE( storage.lookup( 5, 1),  3 );
    QCOMPARE( storage.lookup( 5, 2),  0 );
    QCOMPARE( storage.lookup( 5, 3),  9 );
    QCOMPARE( storage.lookup( 5, 4),  0 );
    QCOMPARE( storage.lookup( 5, 5), 12 );
}

void PointStorageTest::testDeletion()
{
    PointStorage<int> storage;
    storage.m_data << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    storage.m_rows << 0 << 3 << 6 << 9 << 10;
    storage.m_cols << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4 << 1 << 5;
    // ( 1, 2,  ,  , 3)
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,12)
    int old = storage.take( 4, 4 );
    QCOMPARE( old, 10 );
    old = storage.take( 5, 1 );
    QCOMPARE( old,  3 );
    old = storage.take( 2, 2 );
    QCOMPARE( old,  5 );
    // ( 1, 2,  ,  ,  )
    // ( 4,  , 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,  ,  )
    // (11,  ,  ,  ,12)

    const QVector<int> data( QVector<int>() << 1 << 2 << 4 << 6 << 7 << 8 << 9 << 11 << 12 );
    const QVector<int> rows( QVector<int>() << 0 << 2 << 4 << 7 << 7 );
    const QVector<int> cols( QVector<int>() << 1 << 2 << 1 << 3 << 2 << 3 << 5 << 1 << 5);
    QCOMPARE( storage.m_data, data );
    QCOMPARE( storage.m_rows, rows );
    QCOMPARE( storage.m_cols, cols );
}

void PointStorageTest::testInsertColumns()
{
    PointStorage<int> storage;
    storage.m_data << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    storage.m_rows << 0 << 3 << 6 << 9 << 10;
    storage.m_cols << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4 << 1 << 5;
    // ( 1, 2,  ,  , 3)
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,12)

    storage.insertColumns( 2, 2 ); // in the middle
    storage.insertColumns( 9, 1 ); // beyond the current end
    // ( 1,  ,  , 2,  ,  , 3)
    // ( 4,  ,  , 5, 6,  ,  )
    // (  ,  ,  , 7, 8,  , 9)
    // (  ,  ,  ,  ,  ,10,  )
    // (11,  ,  ,  ,  ,  ,12)

    const QVector<int> data( QVector<int>() << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12 );
    const QVector<int> rows( QVector<int>() << 0 << 3 << 6 << 9 << 10 );
    const QVector<int> cols( QVector<int>() << 1 << 4 << 7 << 1 << 4 << 5 << 4 << 5 << 7 << 6 << 1 << 7 );
    QCOMPARE( storage.m_data, data );
    QCOMPARE( storage.m_rows, rows );
    QCOMPARE( storage.m_cols, cols );
}

void PointStorageTest::testDeleteColumns()
{
    PointStorage<int> storage;
    storage.m_data << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    storage.m_rows << 0 << 3 << 6 << 9 << 10;
    storage.m_cols << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4 << 1 << 5;
    // ( 1, 2,  ,  , 3)
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,12)

    storage.removeColumns( 2, 2 ); // in the middle
    storage.removeColumns( 3, 1 ); // beyond the current end
    // ( 1,  )
    // ( 4,  )
    // (  ,  )
    // (  ,10)
    // (11,  )

    const QVector<int> data( QVector<int>() << 1 << 4 << 10 << 11 );
    const QVector<int> rows( QVector<int>() << 0 << 1 << 2 << 2 << 3 );
    const QVector<int> cols( QVector<int>() << 1 << 1 << 2 << 1 );
    QCOMPARE( storage.m_data, data );
    QCOMPARE( storage.m_rows, rows );
    QCOMPARE( storage.m_cols, cols );
}

void PointStorageTest::testInsertRows()
{
    PointStorage<int> storage;
    storage.m_data << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    storage.m_rows << 0 << 3 << 6 << 9 << 10;
    storage.m_cols << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4 << 1 << 5;
    // ( 1, 2,  ,  , 3)
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,12)

    storage.insertRows( 2, 2 ); // in the middle
    storage.insertRows( 9, 1 ); // beyond the current end
    // ( 1, 2,  ,  , 3)
    // (  ,  ,  ,  ,  )
    // (  ,  ,  ,  ,  )
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,12)

    const QVector<int> data( QVector<int>() << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12 );
    const QVector<int> rows( QVector<int>() << 0 << 3 << 3 << 3 << 6 << 9 << 10 );
    const QVector<int> cols( QVector<int>() << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4 << 1 << 5 );
    QCOMPARE( storage.m_data, data );
    QCOMPARE( storage.m_rows, rows );
    QCOMPARE( storage.m_cols, cols );
}

void PointStorageTest::testDeleteRows()
{
    PointStorage<int> storage;
    storage.m_data << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    storage.m_rows << 0 << 3 << 6 << 9 << 10;
    storage.m_cols << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4 << 1 << 5;
    // ( 1, 2,  ,  , 3)
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,12)

    storage.removeRows( 2, 2 ); // in the middle
    storage.removeRows( 3, 1 ); // at the current end
    // ( 1, 2,  ,  , 3)
    // (  ,  ,  ,10,  )

    const QVector<int> data( QVector<int>() << 1 << 2 << 3 << 10 );
    const QVector<int> rows( QVector<int>() << 0 << 3 );
    const QVector<int> cols( QVector<int>() << 1 << 2 << 5 << 4 );
    QCOMPARE( storage.m_data, data );
    QCOMPARE( storage.m_rows, rows );
    QCOMPARE( storage.m_cols, cols );
}

void PointStorageTest::testShiftLeft()
{
    PointStorage<int> storage;
    storage.m_data << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    storage.m_rows << 0 << 3 << 6 << 9 << 10;
    storage.m_cols << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4 << 1 << 5;
    // ( 1, 2,  ,  , 3)
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,12)

    storage.removeShiftLeft( QRect( 2, 2, 2, 2 ) );
    storage.removeShiftLeft( QRect( 5, 5, 1, 1 ) );
    // ( 1, 2,  ,  , 3)
    // ( 4,  ,  ,  ,  )
    // (  ,  , 9,  ,  )
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,  )

    const QVector<int> data( QVector<int>() << 1 << 2 << 3 << 4 << 9 << 10 << 11 );
    const QVector<int> rows( QVector<int>() << 0 << 3 << 4 << 5 << 6 );
    const QVector<int> cols( QVector<int>() << 1 << 2 << 5 << 1 << 3 << 4 << 1 );
    QCOMPARE( storage.m_data, data );
    QCOMPARE( storage.m_rows, rows );
    QCOMPARE( storage.m_cols, cols );
}

void PointStorageTest::testShiftRight()
{
    PointStorage<int> storage;
    storage.m_data << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    storage.m_rows << 0 << 3 << 6 << 9 << 10;
    storage.m_cols << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4 << 1 << 5;
    // ( 1, 2,  ,  , 3)
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,12)

    storage.insertShiftRight( QRect( 2, 2, 2, 2 ) );
    storage.insertShiftRight( QRect( 5, 5, 1, 1 ) );
    // ( 1, 2,  ,  , 3,  ,  )
    // ( 4,  ,  , 5, 6,  ,  )
    // (  ,  ,  , 7, 8,  , 9)
    // (  ,  ,  ,10,  ,  ,  )
    // (11,  ,  ,  ,  ,12,  )

    const QVector<int> data( QVector<int>() << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12 );
    const QVector<int> rows( QVector<int>() << 0 << 3 << 6 << 9 << 10 );
    const QVector<int> cols( QVector<int>() << 1 << 2 << 5 << 1 << 4 << 5 << 4 << 5 << 7 << 4 << 1 << 6 );
    QCOMPARE( storage.m_data, data );
    QCOMPARE( storage.m_rows, rows );
    QCOMPARE( storage.m_cols, cols );
}

void PointStorageTest::testShiftUp()
{
    PointStorage<int> storage;
    storage.m_data << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    storage.m_rows << 0 << 3 << 6 << 9 << 10;
    storage.m_cols << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4 << 1 << 5;
    // ( 1, 2,  ,  , 3)
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,12)

    storage.removeShiftUp( QRect( 2, 2, 2, 2 ) );
    storage.removeShiftUp( QRect( 5, 5, 1, 1 ) );
    // ( 1, 2,  ,  , 3)
    // ( 4,  ,  ,  ,  )
    // (  ,  ,  ,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,  )
    qDebug() << endl << qPrintable( storage.dump() );

    const QVector<int> data( QVector<int>() << 1 << 2 << 3 << 4 << 9 << 10 << 11 );
    const QVector<int> rows( QVector<int>() << 0 << 3 << 4 << 5 << 6 );
    const QVector<int> cols( QVector<int>() << 1 << 2 << 5 << 1 << 5 << 4 << 1 );
    qDebug() << "data result: " << storage.m_data;
    qDebug() << "data expect: " << data;
    qDebug() << "rows result: " << storage.m_rows;
    qDebug() << "rows expect: " << rows;
    qDebug() << "cols result: " << storage.m_cols;
    qDebug() << "cols expect: " << cols;
    QCOMPARE( storage.m_data, data );
    QCOMPARE( storage.m_rows, rows );
    QCOMPARE( storage.m_cols, cols );
}

void PointStorageTest::testShiftDown()
{
    PointStorage<int> storage;
    storage.m_data << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    storage.m_rows << 0 << 3 << 6 << 9 << 10;
    storage.m_cols << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4 << 1 << 5;
    // ( 1, 2,  ,  , 3)
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,12)

    storage.insertShiftDown( QRect( 2, 2, 2, 2 ) );
    storage.insertShiftDown( QRect( 5, 5, 1, 1 ) );
    // ( 1, 2,  ,  , 3)
    // ( 4,  ,  ,  ,  )
    // (  ,  ,  ,  , 9)
    // (  , 5, 6,10,  )
    // (11, 7, 8,  ,  )
    // (  ,  ,  ,  ,12)
    qDebug() << endl << qPrintable( storage.dump() );

    const QVector<int> data( QVector<int>() << 1 << 2 << 3 << 4 << 9 << 5 << 6 << 10 << 11 << 7 << 8 << 12 );
    const QVector<int> rows( QVector<int>() << 0 << 3 << 4 << 5 << 8 << 11 << 12 );
    const QVector<int> cols( QVector<int>() << 1 << 2 << 5 << 1 << 5 << 2 << 3 << 4 << 1 << 2 << 3 << 5 );
    qDebug() << "data result: " << storage.m_data;
    qDebug() << "data expect: " << data;
    qDebug() << "rows result: " << storage.m_rows;
    qDebug() << "rows expect: " << rows;
    qDebug() << "cols result: " << storage.m_cols;
    qDebug() << "cols expect: " << cols;
    QCOMPARE( storage.m_data, data );
    QCOMPARE( storage.m_rows, rows );
    QCOMPARE( storage.m_cols, cols );
}

void PointStorageTest::testFirstColumnData()
{
    PointStorage<int> storage;
    storage.m_data << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    storage.m_rows << 0 << 3 << 6 << 9 << 10;
    storage.m_cols << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4 << 1 << 5;
    // ( 1, 2,  ,  , 3)
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,12)

    QCOMPARE( storage.firstColumnData( 1 ),  1 );
    QCOMPARE( storage.firstColumnData( 2 ),  2 );
    QCOMPARE( storage.firstColumnData( 3 ),  6 );
    QCOMPARE( storage.firstColumnData( 4 ), 10 );
    QCOMPARE( storage.firstColumnData( 5 ),  3 );
}

void PointStorageTest::testFirstRowData()
{
    PointStorage<int> storage;
    storage.m_data << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    storage.m_rows << 0 << 3 << 6 << 9 << 10;
    storage.m_cols << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4 << 1 << 5;
    // ( 1, 2,  ,  , 3)
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,12)

    QCOMPARE( storage.firstRowData( 1 ),  1 );
    QCOMPARE( storage.firstRowData( 2 ),  4 );
    QCOMPARE( storage.firstRowData( 3 ),  7 );
    QCOMPARE( storage.firstRowData( 4 ), 10 );
    QCOMPARE( storage.firstRowData( 5 ), 11 );
}

void PointStorageTest::testNextColumnData()
{
    PointStorage<int> storage;
    storage.m_data << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    storage.m_rows << 0 << 3 << 6 << 9 << 10;
    storage.m_cols << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4 << 1 << 5;
    // ( 1, 2,  ,  , 3)
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,12)

    QCOMPARE( storage.nextColumnData( 1, 3 ), 11 );
    QCOMPARE( storage.nextColumnData( 2, 3 ),  0 );
    QCOMPARE( storage.nextColumnData( 3, 3 ),  0 );
    QCOMPARE( storage.nextColumnData( 4, 3 ), 10 );
    QCOMPARE( storage.nextColumnData( 5, 3 ), 12 );
}

void PointStorageTest::testNextRowData()
{
    PointStorage<int> storage;
    storage.m_data << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    storage.m_rows << 0 << 3 << 6 << 9 << 10;
    storage.m_cols << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4 << 1 << 5;
    // ( 1, 2,  ,  , 3)
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,12)

    QCOMPARE( storage.nextRowData( 3, 1 ),  3 );
    QCOMPARE( storage.nextRowData( 3, 2 ),  0 );
    QCOMPARE( storage.nextRowData( 3, 3 ),  9 );
    QCOMPARE( storage.nextRowData( 3, 4 ), 10 );
    QCOMPARE( storage.nextRowData( 3, 5 ), 12 );
}

void PointStorageTest::testIteration()
{
    PointStorage<int> storage;
    storage.m_data << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    storage.m_rows << 0 << 3 << 6 << 9 << 10;
    storage.m_cols << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4 << 1 << 5;
    // ( 1, 2,  ,  , 3)
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,12)

    QCOMPARE( storage.data(  0 ),  1 );
    QCOMPARE( storage.data(  1 ),  2 );
    QCOMPARE( storage.data(  2 ),  3 );
    QCOMPARE( storage.data(  3 ),  4 );
    QCOMPARE( storage.data(  4 ),  5 );
    QCOMPARE( storage.data(  5 ),  6 );
    QCOMPARE( storage.data(  6 ),  7 );
    QCOMPARE( storage.data(  7 ),  8 );
    QCOMPARE( storage.data(  8 ),  9 );
    QCOMPARE( storage.data(  9 ), 10 );
    QCOMPARE( storage.data( 10 ), 11 );
    QCOMPARE( storage.data( 11 ), 12 );

    QCOMPARE( storage.col(  0 ),  1 );
    QCOMPARE( storage.col(  1 ),  2 );
    QCOMPARE( storage.col(  2 ),  5 );
    QCOMPARE( storage.col(  3 ),  1 );
    QCOMPARE( storage.col(  4 ),  2 );
    QCOMPARE( storage.col(  5 ),  3 );
    QCOMPARE( storage.col(  6 ),  2 );
    QCOMPARE( storage.col(  7 ),  3 );
    QCOMPARE( storage.col(  8 ),  5 );
    QCOMPARE( storage.col(  9 ),  4 );
    QCOMPARE( storage.col( 10 ),  1 );
    QCOMPARE( storage.col( 11 ),  5 );

    QCOMPARE( storage.row(  0 ),  1 );
    QCOMPARE( storage.row(  1 ),  1 );
    QCOMPARE( storage.row(  2 ),  1 );
    QCOMPARE( storage.row(  3 ),  2 );
    QCOMPARE( storage.row(  4 ),  2 );
    QCOMPARE( storage.row(  5 ),  2 );
    QCOMPARE( storage.row(  6 ),  3 );
    QCOMPARE( storage.row(  7 ),  3 );
    QCOMPARE( storage.row(  8 ),  3 );
    QCOMPARE( storage.row(  9 ),  4 );
    QCOMPARE( storage.row( 10 ),  5 );
    QCOMPARE( storage.row( 11 ),  5 );
}

void PointStorageTest::testInsertionPerformance()
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

void PointStorageTest::testLookupPerformance()
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

void PointStorageTest::testInsertColumnsPerformance()
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

void PointStorageTest::testDeleteColumnsPerformance()
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

void PointStorageTest::testInsertRowsPerformance()
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

void PointStorageTest::testDeleteRowsPerformance()
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

void PointStorageTest::testShiftLeftPerformance()
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

void PointStorageTest::testShiftRightPerformance()
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


QTEST_MAIN(PointStorageTest)

#include "moc_TestPointStorage.cpp"
