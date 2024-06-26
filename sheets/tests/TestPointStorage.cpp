// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-License-Identifier: LGPL-2.0-or-later

#define KS_colMax 10
#define KS_rowMax 10

#include "TestPointStorage.h"

#include "engine/PointStorage.h"

#include <QTest>

using namespace Calligra::Sheets;

void PointStorageTest::testInsertion()
{
    PointStorage<int> storage;
    //     int counter = 1;
    //     for ( int col = 1; col <= 5; ++col )
    //     for ( int row = 1; row <= 5; ++row )
    //     cout << qPrintable( QString( "storage.insert( %1, %2, %3 );" ).arg(col,2).arg(row,2).arg(counter++,2) ) << Qt::endl;    storage.insert(  1,  1, 1 );
    storage.insert(1, 1, 1);
    storage.insert(1, 2, 2);
    storage.insert(1, 3, 3);
    storage.insert(1, 4, 4);
    storage.insert(1, 5, 5);
    storage.insert(2, 1, 6);
    storage.insert(2, 2, 7);
    storage.insert(2, 3, 8);
    storage.insert(2, 4, 9);
    storage.insert(2, 5, 10);
    storage.insert(3, 1, 11);
    storage.insert(3, 2, 12);
    storage.insert(3, 3, 13);
    storage.insert(3, 4, 14);
    storage.insert(3, 5, 15);
    storage.insert(4, 1, 16);
    storage.insert(4, 2, 17);
    storage.insert(4, 3, 18);
    storage.insert(4, 4, 19);
    storage.insert(4, 5, 20);
    storage.insert(5, 1, 21);
    storage.insert(5, 2, 22);
    storage.insert(5, 3, 23);
    storage.insert(5, 4, 24);
    storage.insert(5, 5, 25);
    // overwrite
    int old = storage.insert(5, 5, 30);
    QCOMPARE(old, 25);
    // ( 1, 6,11,16,21)
    // ( 2, 7,12,17,22)
    // ( 3, 8,13,18,23)
    // ( 4, 9,14,19,24)
    // ( 5,10,15,20,25)
    //     qDebug() << endl << qPrintable( storage.dump() );

    QVector<int> data(QVector<int>() << 1 << 6 << 11 << 16 << 21 << 2 << 7 << 12 << 17 << 22 << 3 << 8 << 13 << 18 << 23 << 4 << 9 << 14 << 19 << 24 << 5 << 10
                                     << 15 << 20 << 30);
    QVector<int> rows(QVector<int>() << 0 << 5 << 10 << 15 << 20);
    QVector<int> cols(QVector<int>() << 1 << 2 << 3 << 4 << 5 << 1 << 2 << 3 << 4 << 5 << 1 << 2 << 3 << 4 << 5 << 1 << 2 << 3 << 4 << 5 << 1 << 2 << 3 << 4
                                     << 5);
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);
    //     qDebug() << "data result: " << storage.m_data;
    //     qDebug() << "data expect: " << data;
    //     qDebug() << "rows result: " << storage.m_rows;
    //     qDebug() << "rows expect: " << rows;
    //     qDebug() << "cols result: " << storage.m_cols;
    //     qDebug() << "cols expect: " << cols;

    // reverse filling
    storage.clear();
    storage.insert(2, 2, 4);
    storage.insert(1, 2, 3);
    storage.insert(2, 1, 2);
    storage.insert(1, 1, 1);
    // ( 1, 2)
    // ( 3, 4)

    data = QVector<int>() << 1 << 2 << 3 << 4;
    rows = QVector<int>() << 0 << 2;
    cols = QVector<int>() << 1 << 2 << 1 << 2;
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);
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

    QCOMPARE(storage.lookup(1, 1), 1);
    QCOMPARE(storage.lookup(1, 2), 4);
    QCOMPARE(storage.lookup(1, 3), 0);
    QCOMPARE(storage.lookup(1, 4), 0);
    QCOMPARE(storage.lookup(1, 5), 11);
    QCOMPARE(storage.lookup(2, 1), 2);
    QCOMPARE(storage.lookup(2, 2), 5);
    QCOMPARE(storage.lookup(2, 3), 7);
    QCOMPARE(storage.lookup(2, 4), 0);
    QCOMPARE(storage.lookup(2, 5), 0);
    QCOMPARE(storage.lookup(3, 1), 0);
    QCOMPARE(storage.lookup(3, 2), 6);
    QCOMPARE(storage.lookup(3, 3), 8);
    QCOMPARE(storage.lookup(3, 4), 0);
    QCOMPARE(storage.lookup(3, 5), 0);
    QCOMPARE(storage.lookup(4, 1), 0);
    QCOMPARE(storage.lookup(4, 2), 0);
    QCOMPARE(storage.lookup(4, 3), 0);
    QCOMPARE(storage.lookup(4, 4), 10);
    QCOMPARE(storage.lookup(4, 5), 0);
    QCOMPARE(storage.lookup(5, 1), 3);
    QCOMPARE(storage.lookup(5, 2), 0);
    QCOMPARE(storage.lookup(5, 3), 9);
    QCOMPARE(storage.lookup(5, 4), 0);
    QCOMPARE(storage.lookup(5, 5), 12);

    // empty row checking
    storage.clear();
    storage.insert(1, 1, 1);
    // ( 1)

    QCOMPARE(storage.lookup(1, 1), 1);
    QCOMPARE(storage.lookup(2, 1), 0);
    QCOMPARE(storage.lookup(1, 2), 0);
    QCOMPARE(storage.lookup(2, 2), 0);
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
    int old = storage.take(4, 4);
    QCOMPARE(old, 10);
    old = storage.take(5, 1);
    QCOMPARE(old, 3);
    old = storage.take(2, 2);
    QCOMPARE(old, 5);
    // ( 1, 2,  ,  ,  )
    // ( 4,  , 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,  ,  )
    // (11,  ,  ,  ,12)

    const QVector<int> data(QVector<int>() << 1 << 2 << 4 << 6 << 7 << 8 << 9 << 11 << 12);
    const QVector<int> rows(QVector<int>() << 0 << 2 << 4 << 7 << 7);
    const QVector<int> cols(QVector<int>() << 1 << 2 << 1 << 3 << 2 << 3 << 5 << 1 << 5);
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);

    // empty row checking
    storage.clear();
    storage.insert(1, 1, 1);
    // ( 1)

    old = storage.take(2, 2);
    QCOMPARE(old, 0);
    old = storage.take(1, 2);
    QCOMPARE(old, 0);

    old = storage.take(2, 1);
    QCOMPARE(old, 0);
    old = storage.take(1, 1);
    QCOMPARE(old, 1);
    QCOMPARE(storage.lookup(1, 1), 0);
    QCOMPARE(storage.lookup(2, 1), 0);
    QCOMPARE(storage.lookup(1, 2), 0);
    QCOMPARE(storage.lookup(2, 2), 0);
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

    QVector<QPair<QPoint, int>> old;
    storage.resetUndo();
    storage.storeUndo(true);
    storage.insertColumns(2, 2); // in the middle
    old = storage.undoData();
    QVERIFY(old.count() == 0);

    storage.resetUndo();
    storage.storeUndo(true);
    storage.insertColumns(9, 1); // beyond the current end
    old = storage.undoData();
    QVERIFY(old.count() == 0);
    // ( 1,  ,  , 2,  ,  , 3)
    // ( 4,  ,  , 5, 6,  ,  )
    // (  ,  ,  , 7, 8,  , 9)
    // (  ,  ,  ,  ,  ,10,  )
    // (11,  ,  ,  ,  ,  ,12)

    QVector<int> data(QVector<int>() << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12);
    QVector<int> rows(QVector<int>() << 0 << 3 << 6 << 9 << 10);
    QVector<int> cols(QVector<int>() << 1 << 4 << 7 << 1 << 4 << 5 << 4 << 5 << 7 << 6 << 1 << 7);
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);

    storage.resetUndo();
    storage.storeUndo(true);
    storage.insertColumns(6, 4); // shift the last column out of range
    old = storage.undoData();
    QVERIFY(old.count() == 3);
    QVERIFY(old.contains(qMakePair(QPoint(7, 1), 3)));
    QVERIFY(old.contains(qMakePair(QPoint(7, 3), 9)));
    QVERIFY(old.contains(qMakePair(QPoint(7, 5), 12)));
    // ( 1,  ,  , 2,  ,  ,  ,  ,  ,  )
    // ( 4,  ,  , 5, 6,  ,  ,  ,  ,  )
    // (  ,  ,  , 7, 8,  ,  ,  ,  ,  )
    // (  ,  ,  ,  ,  ,  ,  ,  ,  ,10)
    // (11,  ,  ,  ,  ,  ,  ,  ,  ,  )

    data = QVector<int>() << 1 << 2 << 4 << 5 << 6 << 7 << 8 << 10 << 11;
    rows = QVector<int>() << 0 << 2 << 5 << 7 << 8;
    cols = QVector<int>() << 1 << 4 << 1 << 4 << 5 << 4 << 5 << 10 << 1;
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);

    // empty row checking
    storage.clear();
    storage.insert(1, 1, 1);
    // ( 1)

    storage.resetUndo();
    storage.storeUndo(true);
    storage.insertColumns(1, 1);
    old = storage.undoData();
    QVERIFY(old.count() == 0);
    // (  , 1)

    data = QVector<int>() << 1;
    rows = QVector<int>() << 0;
    cols = QVector<int>() << 2;
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);
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

    QVector<QPair<QPoint, int>> old;
    storage.resetUndo();
    storage.storeUndo(true);
    storage.removeColumns(2, 2); // in the middle
    old = storage.undoData();
    QVERIFY(old.count() == 5);
    QVERIFY(old.contains(qMakePair(QPoint(2, 1), 2)));
    QVERIFY(old.contains(qMakePair(QPoint(2, 2), 5)));
    QVERIFY(old.contains(qMakePair(QPoint(3, 2), 6)));
    QVERIFY(old.contains(qMakePair(QPoint(2, 3), 7)));
    QVERIFY(old.contains(qMakePair(QPoint(3, 3), 8)));

    storage.resetUndo();
    storage.storeUndo(true);
    storage.removeColumns(3, 1); // beyond the current end
    old = storage.undoData();
    QVERIFY(old.count() == 3);
    QVERIFY(old.contains(qMakePair(QPoint(3, 1), 3)));
    QVERIFY(old.contains(qMakePair(QPoint(3, 3), 9)));
    QVERIFY(old.contains(qMakePair(QPoint(3, 5), 12)));

    // ( 1,  )
    // ( 4,  )
    // (  ,  )
    // (  ,10)
    // (11,  )

    const QVector<int> data(QVector<int>() << 1 << 4 << 10 << 11);
    const QVector<int> rows(QVector<int>() << 0 << 1 << 2 << 2 << 3);
    const QVector<int> cols(QVector<int>() << 1 << 1 << 2 << 1);
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);
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

    QVector<QPair<QPoint, int>> old;
    storage.resetUndo();
    storage.storeUndo(true);
    storage.insertRows(2, 2); // in the middle
    old = storage.undoData();
    QVERIFY(old.count() == 0);

    storage.resetUndo();
    storage.storeUndo(true);
    storage.insertRows(9, 1); // beyond the current end
    old = storage.undoData();
    QVERIFY(old.count() == 0);

    // ( 1, 2,  ,  , 3)
    // (  ,  ,  ,  ,  )
    // (  ,  ,  ,  ,  )
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,12)

    QVector<int> data(QVector<int>() << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12);
    QVector<int> rows(QVector<int>() << 0 << 3 << 3 << 3 << 6 << 9 << 10);
    QVector<int> cols(QVector<int>() << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4 << 1 << 5);
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);

    storage.resetUndo();
    storage.storeUndo(true);
    storage.insertRows(6, 4); // shift the last row out of range
    old = storage.undoData();
    QVERIFY(old.count() == 2);
    QVERIFY(old.contains(qMakePair(QPoint(1, 7), 11)));
    QVERIFY(old.contains(qMakePair(QPoint(5, 7), 12)));
    // ( 1, 2,  ,  , 3)
    // (  ,  ,  ,  ,  )
    // (  ,  ,  ,  ,  )
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,  ,  )
    // (  ,  ,  ,  ,  )
    // (  ,  ,  ,  ,  )
    // (  ,  ,  ,  ,  )
    // (  ,  ,  ,10,  )

    data = QVector<int>() << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10;
    rows = QVector<int>() << 0 << 3 << 3 << 3 << 6 << 9 << 9 << 9 << 9 << 9;
    cols = QVector<int>() << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4;
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);

    // first row checking
    storage.clear();
    storage.insert(1, 1, 1);
    // ( 1)

    storage.resetUndo();
    storage.storeUndo(true);
    storage.insertRows(1, 1);
    old = storage.undoData();
    QVERIFY(old.count() == 0);
    // (  )
    // ( 1)

    data = QVector<int>() << 1;
    rows = QVector<int>() << 0 << 0;
    cols = QVector<int>() << 1;
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);
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

    QVector<QPair<QPoint, int>> old;
    storage.resetUndo();
    storage.storeUndo(true);
    storage.removeRows(2, 2); // in the middle
    old = storage.undoData();
    QVERIFY(old.count() == 6);
    QVERIFY(old.contains(qMakePair(QPoint(1, 2), 4)));
    QVERIFY(old.contains(qMakePair(QPoint(2, 2), 5)));
    QVERIFY(old.contains(qMakePair(QPoint(3, 2), 6)));
    QVERIFY(old.contains(qMakePair(QPoint(2, 3), 7)));
    QVERIFY(old.contains(qMakePair(QPoint(3, 3), 8)));
    QVERIFY(old.contains(qMakePair(QPoint(5, 3), 9)));

    storage.resetUndo();
    storage.storeUndo(true);
    storage.removeRows(3, 1); // at the current end
    old = storage.undoData();
    QVERIFY(old.count() == 2);
    QVERIFY(old.contains(qMakePair(QPoint(1, 3), 11)));
    QVERIFY(old.contains(qMakePair(QPoint(5, 3), 12)));
    // ( 1, 2,  ,  , 3)
    // (  ,  ,  ,10,  )

    const QVector<int> data(QVector<int>() << 1 << 2 << 3 << 10);
    const QVector<int> rows(QVector<int>() << 0 << 3);
    const QVector<int> cols(QVector<int>() << 1 << 2 << 5 << 4);
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);
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

    QVector<QPair<QPoint, int>> old;
    storage.resetUndo();
    storage.storeUndo(true);
    storage.removeShiftLeft(QRect(2, 2, 2, 2));
    old = storage.undoData();
    QVERIFY(old.count() == 4);
    QVERIFY(old.contains(qMakePair(QPoint(2, 2), 5)));
    QVERIFY(old.contains(qMakePair(QPoint(3, 2), 6)));
    QVERIFY(old.contains(qMakePair(QPoint(2, 3), 7)));
    QVERIFY(old.contains(qMakePair(QPoint(3, 3), 8)));

    storage.resetUndo();
    storage.storeUndo(true);
    storage.removeShiftLeft(QRect(5, 5, 1, 1));
    old = storage.undoData();
    QVERIFY(old.count() == 1);
    QVERIFY(old.contains(qMakePair(QPoint(5, 5), 12)));
    // ( 1, 2,  ,  , 3)
    // ( 4,  ,  ,  ,  )
    // (  ,  , 9,  ,  )
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,  )

    const QVector<int> data(QVector<int>() << 1 << 2 << 3 << 4 << 9 << 10 << 11);
    const QVector<int> rows(QVector<int>() << 0 << 3 << 4 << 5 << 6);
    const QVector<int> cols(QVector<int>() << 1 << 2 << 5 << 1 << 3 << 4 << 1);
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);
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

    QVector<QPair<QPoint, int>> old;
    storage.resetUndo();
    storage.storeUndo(true);
    storage.insertShiftRight(QRect(2, 2, 2, 2));
    old = storage.undoData();
    QVERIFY(old.count() == 0);

    storage.resetUndo();
    storage.storeUndo(true);
    storage.insertShiftRight(QRect(5, 5, 1, 1));
    old = storage.undoData();
    QVERIFY(old.count() == 0);
    // ( 1, 2,  ,  , 3,  ,  )
    // ( 4,  ,  , 5, 6,  ,  )
    // (  ,  ,  , 7, 8,  , 9)
    // (  ,  ,  ,10,  ,  ,  )
    // (11,  ,  ,  ,  ,12,  )

    QVector<int> data(QVector<int>() << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12);
    QVector<int> rows(QVector<int>() << 0 << 3 << 6 << 9 << 10);
    QVector<int> cols(QVector<int>() << 1 << 2 << 5 << 1 << 4 << 5 << 4 << 5 << 7 << 4 << 1 << 6);
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);

    storage.resetUndo();
    storage.storeUndo(true);
    storage.insertShiftRight(QRect(4, 2, 6, 1)); // shift the 6 out of range
    old = storage.undoData();
    QVERIFY(old.count() == 1);
    QVERIFY(old.contains(qMakePair(QPoint(5, 2), 6)));
    // ( 1, 2,  ,  , 3,  ,  ,  ,  ,  )
    // ( 4,  ,  ,  ,  ,  ,  ,  ,  , 5)
    // (  ,  ,  , 7, 8,  , 9,  ,  ,  )
    // (  ,  ,  ,10,  ,  ,  ,  ,  ,  )
    // (11,  ,  ,  ,  ,12,  ,  ,  ,  )

    data = QVector<int>() << 1 << 2 << 3 << 4 << 5 << 7 << 8 << 9 << 10 << 11 << 12;
    rows = QVector<int>() << 0 << 3 << 5 << 8 << 9;
    cols = QVector<int>() << 1 << 2 << 5 << 1 << 10 << 4 << 5 << 7 << 4 << 1 << 6;
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);
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

    QRect rect;
    QVector<QPair<QPoint, int>> old;
    rect = QRect(2, 2, 2, 1);
    storage.resetUndo();
    storage.storeUndo(true);
    storage.removeShiftUp(rect);
    old = storage.undoData();
    //     qDebug() << "moved two filled cells up onto filled cells:"<<rect<< endl << qPrintable( storage.dump() );
    QVERIFY(old.count() == 2);
    QVERIFY(old.contains(qMakePair(QPoint(2, 2), 5)));
    QVERIFY(old.contains(qMakePair(QPoint(3, 2), 6)));

    rect = QRect(5, 5, 1, 1);
    storage.resetUndo();
    storage.storeUndo(true);
    storage.removeShiftUp(rect); // shift data from 6,5 -> 5,5
    old = storage.undoData();
    //     qDebug() << "moved 1 cell from non-existent row onto filled cell:"<<rect << endl << qPrintable( storage.dump() );
    QVERIFY(old.count() == 1);
    QVERIFY(old.contains(qMakePair(QPoint(5, 5), 12)));
    // ( 1, 2,  ,  , 3)
    // ( 4, 7, 8,  ,  )
    // (  ,  ,  ,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,  )

    QVector<int> data(QVector<int>() << 1 << 2 << 3 << 4 << 7 << 8 << 9 << 10 << 11);
    QVector<int> rows(QVector<int>() << 0 << 3 << 6 << 7 << 8);
    QVector<int> cols(QVector<int>() << 1 << 2 << 5 << 1 << 2 << 3 << 5 << 4 << 1);
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);

    rect = QRect(1, 4, 1, 1);
    storage.resetUndo();
    storage.storeUndo(true);
    storage.removeShiftUp(rect);
    old = storage.undoData();
    //     qDebug() << "moved 1 filled cell onto unfilled cell:"<<rect << endl << qPrintable( storage.dump() );
    QCOMPARE(old.count(), 0);

    // first row
    storage.clear();
    storage.m_data << 1 << 2 << 3 << 4;
    storage.m_rows << 0 << 1 << 3;
    storage.m_cols << 1 << 1 << 2 << 2;
    // ( 1,  )
    // ( 2, 3)
    // (  , 4)
    //     qDebug() << "start:" << endl << qPrintable( storage.dump() );

    storage.resetUndo();
    storage.storeUndo(true);
    storage.removeShiftUp(QRect(1, 1, 2, 2));
    old = storage.undoData();
    //     qDebug() << "moved 1 filled, 1 unfilled two rows up onto 1 row:"<<QRect(1, 1, 2, 2) << endl << qPrintable( storage.dump() );
    qDebug() << old;
    QVERIFY(old.count() == 3);
    QVERIFY(old.contains(qMakePair(QPoint(1, 1), 1)));
    QVERIFY(old.contains(qMakePair(QPoint(1, 2), 2)));
    QVERIFY(old.contains(qMakePair(QPoint(2, 2), 3)));
    // (  , 4)
    QCOMPARE(storage.rows(), 1);
    QCOMPARE(storage.columns(), 2);

    data = QVector<int>() << 4;
    rows = QVector<int>() << 0;
    cols = QVector<int>() << 2;
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);
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

    QVector<QPair<QPoint, int>> old;
    storage.resetUndo();
    storage.storeUndo(true);
    storage.insertShiftDown(QRect(2, 2, 2, 2));
    old = storage.undoData();
    QVERIFY(old.count() == 0);

    storage.resetUndo();
    storage.storeUndo(true);
    storage.insertShiftDown(QRect(5, 5, 1, 1));
    old = storage.undoData();
    QVERIFY(old.count() == 0);
    // ( 1, 2,  ,  , 3)
    // ( 4,  ,  ,  ,  )
    // (  ,  ,  ,  , 9)
    // (  , 5, 6,10,  )
    // (11, 7, 8,  ,  )
    // (  ,  ,  ,  ,12)

    QVector<int> data(QVector<int>() << 1 << 2 << 3 << 4 << 9 << 5 << 6 << 10 << 11 << 7 << 8 << 12);
    QVector<int> rows(QVector<int>() << 0 << 3 << 4 << 5 << 8 << 11);
    QVector<int> cols(QVector<int>() << 1 << 2 << 5 << 1 << 5 << 2 << 3 << 4 << 1 << 2 << 3 << 5);
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);

    storage.resetUndo();
    storage.storeUndo(true);
    storage.insertShiftDown(QRect(2, 4, 1, 6)); // shift the 7 out of range
    old = storage.undoData();
    QVERIFY(old.count() == 1);
    QVERIFY(old.contains(qMakePair(QPoint(2, 5), 7)));
    // ( 1, 2,  ,  , 3)
    // ( 4,  ,  ,  ,  )
    // (  ,  ,  ,  , 9)
    // (  ,  , 6,10,  )
    // (11,  , 8,  ,  )
    // (  ,  ,  ,  ,12)
    // (  ,  ,  ,  ,  )
    // (  ,  ,  ,  ,  )
    // (  ,  ,  ,  ,  )
    // (  ,  ,  ,  ,  )
    // (  , 5,  ,  ,  )

    data = QVector<int>() << 1 << 2 << 3 << 4 << 9 << 6 << 10 << 11 << 8 << 12 << 5;
    rows = QVector<int>() << 0 << 3 << 4 << 5 << 7 << 9 << 10 << 10 << 10 << 10;
    cols = QVector<int>() << 1 << 2 << 5 << 1 << 5 << 3 << 4 << 1 << 3 << 5 << 2;
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);

    // first row
    storage.clear();
    storage.m_data << 1 << 2 << 3 << 4;
    storage.m_rows << 0 << 1 << 3;
    storage.m_cols << 1 << 1 << 2 << 2;
    // ( 1,  )
    // ( 2, 3)
    // (  , 4)

    storage.resetUndo();
    storage.storeUndo(true);
    storage.insertShiftDown(QRect(1, 1, 2, 2));
    old = storage.undoData();
    QVERIFY(old.count() == 0);
    // (  ,  )
    // (  ,  )
    // ( 1,  )
    // ( 2, 3)
    // (  , 4)

    data = QVector<int>() << 1 << 2 << 3 << 4;
    rows = QVector<int>() << 0 << 0 << 0 << 1 << 3;
    cols = QVector<int>() << 1 << 1 << 2 << 2;
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);
}

void PointStorageTest::testShiftDownUp()
{
    PointStorage<int> storage;
    for (int row = 1; row < 6; ++row) {
        for (int col = 1; col < 6; ++col) {
            storage.m_data << (row * col);
            storage.m_cols << col;
        }
        storage.m_rows << (5 * (row - 1));
    }
    // qDebug() << "Origin:" << endl << qPrintable(storage.dump());
    // ( 1, 2, 3, 4, 5)
    // ( 2, 4, 6, 8,10)
    // ( 3, 6, 8,12,15)
    // ( 4, 8,12,16,20)
    // ( 5,10,15,20,25)

    QVector<int> data(QVector<int>() << 1 << 2 << 3 << 4 << 5 << 2 << 4 << 6 << 8 << 10 << 3 << 6 << 9 << 12 << 15 << 4 << 8 << 12 << 16 << 20 << 5 << 10 << 15
                                     << 20 << 25);
    QVector<int> rows(QVector<int>() << 0 << 5 << 10 << 15 << 20);
    QVector<int> cols(QVector<int>() << 1 << 2 << 3 << 4 << 5 << 1 << 2 << 3 << 4 << 5 << 1 << 2 << 3 << 4 << 5 << 1 << 2 << 3 << 4 << 5 << 1 << 2 << 3 << 4
                                     << 5);
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);

    QVector<QPair<QPoint, int>> old;
    storage.resetUndo();
    storage.storeUndo(true);
    storage.insertShiftDown(QRect(3, 2, 2, 2));
    old = storage.undoData();
    QVERIFY(old.count() == 0);

    // qDebug() << endl << qPrintable(storage.dump());
    // ( 1, 2, 3, 4, 5)
    // ( 2, 4,  ,  ,10)
    // ( 3, 6,  ,  ,15)
    // ( 4, 8, 6, 8,20)
    // ( 5,10, 9,12,25)
    // (  ,  ,12,16,  )
    // (  ,  ,15,20,  )

    data = QVector<int>() << 1 << 2 << 3 << 4 << 5 << 2 << 4 << 10 << 3 << 6 << 15 << 4 << 8 << 6 << 8 << 20 << 5 << 10 << 9 << 12 << 25 << 12 << 16 << 15
                          << 20;
    cols = QVector<int>() << 1 << 2 << 3 << 4 << 5 << 1 << 2 << 5 << 1 << 2 << 5 << 1 << 2 << 3 << 4 << 5 << 1 << 2 << 3 << 4 << 5 << 3 << 4 << 3 << 4;
    rows = QVector<int>() << 0 << 5 << 8 << 11 << 16 << 21 << 23;
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);

    storage.resetUndo();
    storage.storeUndo(true);
    storage.removeShiftUp(QRect(3, 2, 2, 2));
    old = storage.undoData();
    QVERIFY(old.count() == 0);

    data = QVector<int>() << 1 << 2 << 3 << 4 << 5 << 2 << 4 << 6 << 8 << 10 << 3 << 6 << 9 << 12 << 15 << 4 << 8 << 12 << 16 << 20 << 5 << 10 << 15 << 20
                          << 25;
    rows = QVector<int>() << 0 << 5 << 10 << 15 << 20;
    cols = QVector<int>() << 1 << 2 << 3 << 4 << 5 << 1 << 2 << 3 << 4 << 5 << 1 << 2 << 3 << 4 << 5 << 1 << 2 << 3 << 4 << 5 << 1 << 2 << 3 << 4 << 5;
    QCOMPARE(storage.m_data, data);
    QCOMPARE(storage.m_rows, rows);
    QCOMPARE(storage.m_cols, cols);
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

    int newRow = 0;
    QCOMPARE(storage.firstInColumn(1, &newRow), 1);
    QCOMPARE(newRow, 1);
    QCOMPARE(storage.firstInColumn(2, &newRow), 2);
    QCOMPARE(newRow, 1);
    QCOMPARE(storage.firstInColumn(3, &newRow), 6);
    QCOMPARE(newRow, 2);
    QCOMPARE(storage.firstInColumn(4, &newRow), 10);
    QCOMPARE(newRow, 4);
    QCOMPARE(storage.firstInColumn(5, &newRow), 3);
    QCOMPARE(newRow, 1);

    storage.clear();
    storage.m_data << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 11 << 12;
    storage.m_rows << 0 << 3 << 6 << 9 << 9;
    storage.m_cols << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 1 << 5;
    // ( 1, 2,  ,  , 3)
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,  ,  )
    // (11,  ,  ,  ,12)

    QCOMPARE(storage.firstInColumn(4, &newRow), 0);
    QCOMPARE(newRow, 0);
    QCOMPARE(storage.firstInColumn(5, &newRow), 3);
    QCOMPARE(newRow, 1);
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

    int newCol = 0;
    QCOMPARE(storage.firstInRow(1, &newCol), 1);
    QCOMPARE(newCol, 1);
    QCOMPARE(storage.firstInRow(2, &newCol), 4);
    QCOMPARE(newCol, 1);
    QCOMPARE(storage.firstInRow(3, &newCol), 7);
    QCOMPARE(newCol, 2);
    QCOMPARE(storage.firstInRow(4, &newCol), 10);
    QCOMPARE(newCol, 4);
    QCOMPARE(storage.firstInRow(5, &newCol), 11);
    QCOMPARE(newCol, 1);

    storage.clear();
    storage.m_data << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 11 << 12;
    storage.m_rows << 0 << 3 << 6 << 9 << 9;
    storage.m_cols << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 1 << 5;
    // ( 1, 2,  ,  , 3)
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,  ,  )
    // (11,  ,  ,  ,12)

    QCOMPARE(storage.firstInRow(4, &newCol), 0);
    QCOMPARE(newCol, 0);
    QCOMPARE(storage.firstInRow(5, &newCol), 11);
    QCOMPARE(newCol, 1);

    storage.clear();
    storage.m_data << 1;
    storage.m_rows << 0 << 0;
    storage.m_cols << 1;
    // (  )
    // ( 1)

    QCOMPARE(storage.firstInRow(1, &newCol), 0);
    QCOMPARE(newCol, 0);
    QCOMPARE(storage.firstInRow(2, &newCol), 1);
    QCOMPARE(newCol, 1);
}

void PointStorageTest::testLastInColumn()
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

    int newRow = 0;
    QCOMPARE(storage.lastInColumn(1, &newRow), 11);
    QCOMPARE(newRow, 5);
    QCOMPARE(storage.lastInColumn(2, &newRow), 7);
    QCOMPARE(newRow, 3);
    QCOMPARE(storage.lastInColumn(3, &newRow), 8);
    QCOMPARE(newRow, 3);
    QCOMPARE(storage.lastInColumn(4, &newRow), 10);
    QCOMPARE(newRow, 4);
    QCOMPARE(storage.lastInColumn(5, &newRow), 12);
    QCOMPARE(newRow, 5);
    QCOMPARE(storage.lastInColumn(6, &newRow), 0);
    QCOMPARE(newRow, 0);
}

void PointStorageTest::testLastInRow()
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

    int newCol = 0;
    QCOMPARE(storage.lastInRow(1, &newCol), 3);
    QCOMPARE(newCol, 5);
    QCOMPARE(storage.lastInRow(2, &newCol), 6);
    QCOMPARE(newCol, 3);
    QCOMPARE(storage.lastInRow(3, &newCol), 9);
    QCOMPARE(newCol, 5);
    QCOMPARE(storage.lastInRow(4, &newCol), 10);
    QCOMPARE(newCol, 4);
    QCOMPARE(storage.lastInRow(5, &newCol), 12);
    QCOMPARE(newCol, 5);
    QCOMPARE(storage.lastInRow(6, &newCol), 0);
    QCOMPARE(newCol, 0);
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

    int newRow = 0;
    QCOMPARE(storage.nextInColumn(1, 3, &newRow), 11);
    QCOMPARE(newRow, 5);
    QCOMPARE(storage.nextInColumn(2, 3, &newRow), 0);
    QCOMPARE(newRow, 0);
    QCOMPARE(storage.nextInColumn(3, 3, &newRow), 0);
    QCOMPARE(newRow, 0);
    QCOMPARE(storage.nextInColumn(4, 3, &newRow), 10);
    QCOMPARE(newRow, 4);
    QCOMPARE(storage.nextInColumn(5, 3, &newRow), 12);
    QCOMPARE(newRow, 5);
    QCOMPARE(storage.nextInColumn(6, 3, &newRow), 0);
    QCOMPARE(newRow, 0);

    //
    storage.clear();
    storage.m_data << 1 << 2 << 3 << 4;
    storage.m_rows << 0 << 0 << 0 << 1 << 2;
    storage.m_cols << 1 << 1 << 1 << 2;
    // (  ,  )
    // (  ,  )
    // ( 1,  )
    // ( 2,  )
    // ( 3, 4)

    QCOMPARE(storage.nextInColumn(1, 1, &newRow), 1);
    QCOMPARE(newRow, 3);
    QCOMPARE(storage.nextInColumn(2, 1, &newRow), 4);
    QCOMPARE(newRow, 5);
    QCOMPARE(storage.nextInColumn(1, 7, &newRow), 0);
    QCOMPARE(newRow, 0);
    QCOMPARE(storage.nextInColumn(2, 5, &newRow), 0);
    QCOMPARE(newRow, 0);
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

    int newCol = 0;
    QCOMPARE(storage.nextInRow(3, 1, &newCol), 3);
    QCOMPARE(newCol, 5);
    QCOMPARE(storage.nextInRow(3, 2, &newCol), 0);
    QCOMPARE(newCol, 0);
    QCOMPARE(storage.nextInRow(3, 3, &newCol), 9);
    QCOMPARE(newCol, 5);
    QCOMPARE(storage.nextInRow(3, 4, &newCol), 10);
    QCOMPARE(newCol, 4);
    QCOMPARE(storage.nextInRow(3, 5, &newCol), 12);
    QCOMPARE(newCol, 5);
    QCOMPARE(storage.nextInRow(3, 6, &newCol), 0);
    QCOMPARE(newCol, 0);
}

void PointStorageTest::testPrevInColumn()
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

    int newRow = 0;
    QCOMPARE(storage.prevInColumn(1, 3, &newRow), 4);
    QCOMPARE(newRow, 2);
    QCOMPARE(storage.prevInColumn(2, 3, &newRow), 5);
    QCOMPARE(newRow, 2);
    QCOMPARE(storage.prevInColumn(3, 3, &newRow), 6);
    QCOMPARE(newRow, 2);
    QCOMPARE(storage.prevInColumn(4, 3, &newRow), 0);
    QCOMPARE(newRow, 0);
    QCOMPARE(storage.prevInColumn(5, 3, &newRow), 3);
    QCOMPARE(newRow, 1);
    QCOMPARE(storage.prevInColumn(6, 3, &newRow), 0);
    QCOMPARE(newRow, 0);
    QCOMPARE(storage.prevInColumn(3, 7, &newRow), 8);
    QCOMPARE(newRow, 3);
}

void PointStorageTest::testPrevInRow()
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

    int newCol = 0;
    QCOMPARE(storage.prevInRow(3, 1, &newCol), 2);
    QCOMPARE(newCol, 2);
    QCOMPARE(storage.prevInRow(3, 2, &newCol), 5);
    QCOMPARE(newCol, 2);
    QCOMPARE(storage.prevInRow(3, 3, &newCol), 7);
    QCOMPARE(newCol, 2);
    QCOMPARE(storage.prevInRow(3, 4, &newCol), 0);
    QCOMPARE(newCol, 0);
    QCOMPARE(storage.prevInRow(3, 5, &newCol), 11);
    QCOMPARE(newCol, 1);
    QCOMPARE(storage.prevInRow(3, 6, &newCol), 0);
    QCOMPARE(newCol, 0);
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

    QCOMPARE(storage.data(0), 1);
    QCOMPARE(storage.data(1), 2);
    QCOMPARE(storage.data(2), 3);
    QCOMPARE(storage.data(3), 4);
    QCOMPARE(storage.data(4), 5);
    QCOMPARE(storage.data(5), 6);
    QCOMPARE(storage.data(6), 7);
    QCOMPARE(storage.data(7), 8);
    QCOMPARE(storage.data(8), 9);
    QCOMPARE(storage.data(9), 10);
    QCOMPARE(storage.data(10), 11);
    QCOMPARE(storage.data(11), 12);

    QCOMPARE(storage.col(0), 1);
    QCOMPARE(storage.col(1), 2);
    QCOMPARE(storage.col(2), 5);
    QCOMPARE(storage.col(3), 1);
    QCOMPARE(storage.col(4), 2);
    QCOMPARE(storage.col(5), 3);
    QCOMPARE(storage.col(6), 2);
    QCOMPARE(storage.col(7), 3);
    QCOMPARE(storage.col(8), 5);
    QCOMPARE(storage.col(9), 4);
    QCOMPARE(storage.col(10), 1);
    QCOMPARE(storage.col(11), 5);

    QCOMPARE(storage.row(0), 1);
    QCOMPARE(storage.row(1), 1);
    QCOMPARE(storage.row(2), 1);
    QCOMPARE(storage.row(3), 2);
    QCOMPARE(storage.row(4), 2);
    QCOMPARE(storage.row(5), 2);
    QCOMPARE(storage.row(6), 3);
    QCOMPARE(storage.row(7), 3);
    QCOMPARE(storage.row(8), 3);
    QCOMPARE(storage.row(9), 4);
    QCOMPARE(storage.row(10), 5);
    QCOMPARE(storage.row(11), 5);
}

void PointStorageTest::testColumnIteration()
{
    PointStorage<int> storage;
    storage.insert(1, 1, 27);

    int row = -1;
    QCOMPARE(storage.firstInColumn(1, &row), 27);
    QCOMPARE(row, 1);
    row = -1;
    //     QCOMPARE(storage.nextInColumn(1, 0, &row), 27);
    //     QCOMPARE(row, 1);
    QCOMPARE(storage.nextInColumn(1, 1, &row), 0);
    QCOMPARE(row, 0);

    storage.insert(1, 5, 5);

    QCOMPARE(storage.nextInColumn(1, 1, &row), 5);
    QCOMPARE(row, 5);
    QCOMPARE(storage.nextInColumn(5, 1, &row), 0);
    QCOMPARE(row, 0);
    row = -1;
    QCOMPARE(storage.nextInColumn(6, 1, &row), 0);
    QCOMPARE(row, 0);

    // reverse iteration
    QCOMPARE(storage.lastInColumn(1, &row), 5);
    QCOMPARE(row, 5);
    row = -1;
    //     QCOMPARE(storage.prevInColumn(1, KS_rowMax + 1, &row), 5);
    //     QCOMPARE(row, 5);
    QCOMPARE(storage.prevInColumn(1, 6, &row), 5);
    QCOMPARE(row, 5);
    QCOMPARE(storage.prevInColumn(1, 5, &row), 27);
    QCOMPARE(row, 1);
    QCOMPARE(storage.prevInColumn(1, 1, &row), 0);
    QCOMPARE(row, 0);
}

void PointStorageTest::testRowIteration()
{
    PointStorage<int> storage;
    storage.insert(1, 1, 27);

    int col = -1;
    QCOMPARE(storage.firstInRow(1, &col), 27);
    QCOMPARE(col, 1);
    col = -1;
    QCOMPARE(storage.nextInRow(1, 1, &col), 0);
    QCOMPARE(col, 0);

    storage.insert(5, 1, 5);
    //     qDebug() << qPrintable(storage.dump());
    QCOMPARE(storage.nextInRow(1, 1, &col), 5);
    QCOMPARE(col, 5);
    QCOMPARE(storage.nextInRow(1, 5, &col), 0);
    QCOMPARE(col, 0);
    col = -1;
    QCOMPARE(storage.nextInRow(1, 6, &col), 0);
    QCOMPARE(col, 0);

    // reverse iteration
    QCOMPARE(storage.lastInRow(1, &col), 5);
    QCOMPARE(col, 5);
    col = -1;
    //     QCOMPARE(storage.prevInRow(KS_colMax + 1, 1, &col), 5);
    //     QCOMPARE(col, 5);
    QCOMPARE(storage.prevInRow(6, 1, &col), 5);
    QCOMPARE(col, 5);
    QCOMPARE(storage.prevInRow(5, 1, &col), 27);
    QCOMPARE(col, 1);
    QCOMPARE(storage.prevInRow(1, 1, &col), 0);
    QCOMPARE(col, 0);
}

void PointStorageTest::testDimension()
{
    PointStorage<int> storage;
    QCOMPARE(storage.rows(), 0);
    QCOMPARE(storage.columns(), 0);
    storage.insert(1, 1, 27);
    QCOMPARE(storage.rows(), 1);
    QCOMPARE(storage.columns(), 1);
    storage.insert(3, 1, 27);
    QCOMPARE(storage.rows(), 1);
    QCOMPARE(storage.columns(), 3);
    storage.insert(3, 9, 27);
    QCOMPARE(storage.rows(), 9);
    QCOMPARE(storage.columns(), 3);
    storage.insert(9, 9, 27);
    QCOMPARE(storage.rows(), 9);
    QCOMPARE(storage.columns(), 9);
    storage.insert(10, 9, 27);
    QCOMPARE(storage.rows(), 9);
    QCOMPARE(storage.columns(), 10);
    storage.insert(10, 10, 27);
    QCOMPARE(storage.rows(), 10);
    QCOMPARE(storage.columns(), 10);
}

void PointStorageTest::testSubStorage()
{
    // #if 0
    PointStorage<int> storage;
    storage.m_data << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    storage.m_rows << 0 << 3 << 6 << 9 << 10;
    storage.m_cols << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4 << 1 << 5;
    // ( 1, 2,  ,  , 3)
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,12)

    PointStorage<int> subStorage;
    subStorage = storage.subStorage(Region(QRect(1, 1, 5, 5))); // all
    // ( 1, 2,  ,  , 3)
    // ( 4, 5, 6,  ,  )
    // (  , 7, 8,  , 9)
    // (  ,  ,  ,10,  )
    // (11,  ,  ,  ,12)

    QVector<int> data = QVector<int>() << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12;
    QVector<int> rows = QVector<int>() << 0 << 3 << 6 << 9 << 10;
    QVector<int> cols = QVector<int>() << 1 << 2 << 5 << 1 << 2 << 3 << 2 << 3 << 5 << 4 << 1 << 5;
    QCOMPARE(subStorage.m_data, data);
    QCOMPARE(subStorage.m_rows, rows);
    QCOMPARE(subStorage.m_cols, cols);

    subStorage = storage.subStorage(Region(QRect(1, 1, 3, 3))); // upper left
    // ( 1, 2,  )
    // ( 4, 5, 6)
    // (  , 7, 8)

    data = QVector<int>() << 1 << 2 << 4 << 5 << 6 << 7 << 8;
    rows = QVector<int>() << 0 << 2 << 5;
    cols = QVector<int>() << 1 << 2 << 1 << 2 << 3 << 2 << 3;
    QCOMPARE(subStorage.m_data, data);
    QCOMPARE(subStorage.m_rows, rows);
    QCOMPARE(subStorage.m_cols, cols);

    subStorage = storage.subStorage(Region(QRect(4, 4, 5, 5))); // lower right
    // (  ,  ,  ,  ,  )
    // (  ,  ,  ,  ,  )
    // (  ,  ,  ,  ,  )
    // (  ,  ,  ,10,  )
    // (  ,  ,  ,  ,12)

    data = QVector<int>() << 10 << 12;
    rows = QVector<int>() << 0 << 0 << 0 << 0 << 1;
    cols = QVector<int>() << 4 << 5;
    QCOMPARE(subStorage.m_data, data);
    QCOMPARE(subStorage.m_rows, rows);
    QCOMPARE(subStorage.m_cols, cols);
    // #endif
}

QTEST_MAIN(PointStorageTest)
