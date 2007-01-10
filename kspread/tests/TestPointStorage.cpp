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

    const QVector<int> data( QVector<int>() << 1 << 2 << 3 << 4 << 9 << 10 << 11 );
    const QVector<int> rows( QVector<int>() << 0 << 3 << 4 << 5 << 6 );
    const QVector<int> cols( QVector<int>() << 1 << 2 << 5 << 1 << 5 << 4 << 1 );
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

void PointStorageTest::testFirstInColumn()
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

    QCOMPARE( storage.firstInColumn( 1 ),  1 );
    QCOMPARE( storage.firstInColumn( 2 ),  2 );
    QCOMPARE( storage.firstInColumn( 3 ),  6 );
    QCOMPARE( storage.firstInColumn( 4 ), 10 );
    QCOMPARE( storage.firstInColumn( 5 ),  3 );
}

void PointStorageTest::testFirstInRow()
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

    QCOMPARE( storage.firstInRow( 1 ),  1 );
    QCOMPARE( storage.firstInRow( 2 ),  4 );
    QCOMPARE( storage.firstInRow( 3 ),  7 );
    QCOMPARE( storage.firstInRow( 4 ), 10 );
    QCOMPARE( storage.firstInRow( 5 ), 11 );
}

void PointStorageTest::testNextInColumn()
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

    QCOMPARE( storage.nextInColumn( 1, 3 ), 11 );
    QCOMPARE( storage.nextInColumn( 2, 3 ),  0 );
    QCOMPARE( storage.nextInColumn( 3, 3 ),  0 );
    QCOMPARE( storage.nextInColumn( 4, 3 ), 10 );
    QCOMPARE( storage.nextInColumn( 5, 3 ), 12 );
}

void PointStorageTest::testNextInRow()
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

    QCOMPARE( storage.nextInRow( 3, 1 ),  3 );
    QCOMPARE( storage.nextInRow( 3, 2 ),  0 );
    QCOMPARE( storage.nextInRow( 3, 3 ),  9 );
    QCOMPARE( storage.nextInRow( 3, 4 ), 10 );
    QCOMPARE( storage.nextInRow( 3, 5 ), 12 );
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


QTEST_MAIN(PointStorageTest)

#include "TestPointStorage.moc"
