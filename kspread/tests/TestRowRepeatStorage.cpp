/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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
#include "TestRowRepeatStorage.h"

#include <QVector>

#include <qtest_kde.h>

#include "../RowRepeatStorage.h"
#include "../kspread_limits.h"

using namespace KSpread;

void TestRowRepeatStorage::testEmptyStorage()
{
    RowRepeatStorage s;
    for (int i = 1; i <= KS_rowMax; i++) {
        QCOMPARE(s.rowRepeat(i), 1);
        QCOMPARE(s.firstIdenticalRow(i), i);
    }
}

void TestRowRepeatStorage::testSimpleSetRowRepeat()
{
    // test simple non-overlapping ranges
    RowRepeatStorage s;
    s.setRowRepeat(10, 5);
    for (int i = 1; i <= KS_rowMax; i++) {
        if (i >= 10 && i < 15) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 10);
        } else {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        }
    }
    s.setRowRepeat(44, 7);
    for (int i = 1; i <= KS_rowMax; i++) {
        if (i >= 10 && i < 15) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 10);
        } else if (i >= 44 && i < 51) {
            QCOMPARE(s.rowRepeat(i), 7);
            QCOMPARE(s.firstIdenticalRow(i), 44);
        } else {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        }
    }
    s.setRowRepeat(15, 4);
    for (int i = 1; i <= KS_rowMax; i++) {
        if (i >= 10 && i < 15) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 10);
        } else if (i >= 15 && i < 19) {
            QCOMPARE(s.rowRepeat(i), 4);
            QCOMPARE(s.firstIdenticalRow(i), 15);
        } else if (i >= 44 && i < 51) {
            QCOMPARE(s.rowRepeat(i), 7);
            QCOMPARE(s.firstIdenticalRow(i), 44);
        } else {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        }
    }
}

void TestRowRepeatStorage::testOverlappingRanges_data()
{
    QTest::addColumn<int>("firststart");
    QTest::addColumn<int>("firstcount");
    QTest::addColumn<int>("secondstart");
    QTest::addColumn<int>("secondcount");

    QTest::newRow("non overlapping") << 5 << 5 << 10 << 5;
    QTest::newRow("overlap end 1") << 5 << 5 << 9 << 5;
    QTest::newRow("overlap end 2") << 5 << 5 << 8 << 5;
    QTest::newRow("overlap start 1") << 10 << 5 << 6 << 5;
    QTest::newRow("overlap start 2") << 10 << 5 << 7 << 5;
    QTest::newRow("overlap middle") << 5 << 10 << 7 << 5;
    QTest::newRow("fully cover") << 7 << 5 << 5 << 10;
}

void TestRowRepeatStorage::testOverlappingRanges()
{
    RowRepeatStorage s;

    QFETCH(int, firststart);
    QFETCH(int, firstcount);
    QFETCH(int, secondstart);
    QFETCH(int, secondcount);

    s.setRowRepeat(firststart, firstcount);
    s.setRowRepeat(secondstart, secondcount);

    for (int i = 1; i <= KS_rowMax; i++) {
        if (i >= secondstart && i < secondstart + secondcount) {
            QCOMPARE(s.rowRepeat(i), secondcount);
            QCOMPARE(s.firstIdenticalRow(i), secondstart);
        } else if (i >= firststart && i < firststart + firstcount) {
            if (i < secondstart) {
                QCOMPARE(s.rowRepeat(i), qMin(firstcount, secondstart - firststart));
                QCOMPARE(s.firstIdenticalRow(i), firststart);
            } else {
                QCOMPARE(s.rowRepeat(i), qMin(firstcount, firststart + firstcount - secondstart - secondcount));
                QCOMPARE(s.firstIdenticalRow(i), qMax(firststart, secondstart + secondcount));
            }
        } else {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        }
    }
}

void TestRowRepeatStorage::testComplexSetRowRepeat()
{
    RowRepeatStorage s;

    s.setRowRepeat(1, 1000);
    s.setRowRepeat(5, 100);
    s.setRowRepeat(10, 10);
    s.setRowRepeat(90, 50);
    s.setRowRepeat(15, 100);

    for (int i = 1; i <= KS_rowMax; i++) {
        if (i < 5) {
            QCOMPARE(s.rowRepeat(i), 4);
            QCOMPARE(s.firstIdenticalRow(i), 1);
        } else if (i < 10) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 5);
        } else if (i < 15) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 10);
        } else if (i < 115) {
            QCOMPARE(s.rowRepeat(i), 100);
            QCOMPARE(s.firstIdenticalRow(i), 15);
        } else if (i < 140) {
            QCOMPARE(s.rowRepeat(i), 25);
            QCOMPARE(s.firstIdenticalRow(i), 115);
        } else if (i <= 1000) {
            QCOMPARE(s.rowRepeat(i), 861);
            QCOMPARE(s.firstIdenticalRow(i), 140);
        } else {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        }
    }
}

void TestRowRepeatStorage::testInsertRowsEmpty()
{
    RowRepeatStorage s;
    s.insertRows(10, 5);

    for (int i = 1; i <= KS_rowMax; i++) {
        if (i >= 10 && i < 15) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 10);
        } else {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        }
    }
}

void TestRowRepeatStorage::testInsertRowsBetween()
{
    RowRepeatStorage s;

    s.setRowRepeat(5, 10);
    s.setRowRepeat(40, 10);
    s.insertRows(20, 10);

    for (int i = 1; i <= KS_rowMax; i++) {
        if (i < 5 || i >= 60) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 15) {
            QCOMPARE(s.rowRepeat(i), 10);
            QCOMPARE(s.firstIdenticalRow(i), 5);
        } else if (i < 20) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 30) {
            QCOMPARE(s.rowRepeat(i), 10);
            QCOMPARE(s.firstIdenticalRow(i), 20);
        } else if (i < 50) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 60) {
            QCOMPARE(s.rowRepeat(i), 10);
            QCOMPARE(s.firstIdenticalRow(i), 50);
        }
    }
}

void TestRowRepeatStorage::testInsertRowsMiddle()
{
    RowRepeatStorage s;
    s.setRowRepeat(10, 20);
    s.insertRows(20, 7);

    for (int i = 1; i <= KS_rowMax; i++) {
        if (i < 10 || i >= 37) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 20) {
            QCOMPARE(s.rowRepeat(i), 10);
            QCOMPARE(s.firstIdenticalRow(i), 10);
        } else if (i < 27) {
            QCOMPARE(s.rowRepeat(i), 7);
            QCOMPARE(s.firstIdenticalRow(i), 20);
        } else if (i < 37) {
            QCOMPARE(s.rowRepeat(i), 10);
            QCOMPARE(s.firstIdenticalRow(i), 27);
        }
    }
}

void TestRowRepeatStorage::testRemoveRowsEmpty()
{
    RowRepeatStorage s;
    s.removeRows(10, 20);

    for (int i = 1; i <= KS_rowMax; i++) {
        QCOMPARE(s.rowRepeat(i), 1);
        QCOMPARE(s.firstIdenticalRow(i), i);
    }
}

void TestRowRepeatStorage::testRemoveRowsBetween()
{
    RowRepeatStorage s;
    s.setRowRepeat(5, 10);
    s.setRowRepeat(50, 20);
    s.removeRows(25, 5);

    for (int i = 0; i < KS_rowMax; i++) {
        if (i < 5 || i >= 65) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 15) {
            QCOMPARE(s.rowRepeat(i), 10);
            QCOMPARE(s.firstIdenticalRow(i), 5);
        } else if (i < 45) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 65) {
            QCOMPARE(s.rowRepeat(i), 20);
            QCOMPARE(s.firstIdenticalRow(i), 45);
        }
    }
}

void TestRowRepeatStorage::testRemoveRowsOverlap()
{
    RowRepeatStorage s;
    s.setRowRepeat(5, 15);
    s.setRowRepeat(30, 10);
    s.setRowRepeat(12, 4);
    s.removeRows(10, 22);

    for (int i = 0; i < KS_rowMax; i++) {
        if (i < 5 || i >= 18) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 10) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 5);
        } else if (i < 18) {
            QCOMPARE(s.rowRepeat(i), 8);
            QCOMPARE(s.firstIdenticalRow(i), 10);
        }
    }
}

void TestRowRepeatStorage::testInsertShiftDown1()
{
    // entire rect inside one row-repeat, with a smaller and a larger row repeat after it
    RowRepeatStorage s;
    s.setRowRepeat(10, 20);
    s.setRowRepeat(100, 5);
    s.setRowRepeat(200, 50);
    s.insertShiftDown(QRect(5, 15, 10, 10));

    for (int i = 1; i <= KS_rowMax; i++) {
        if (i < 10) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 15) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 10);
        } else if (i < 25) {
            QCOMPARE(s.rowRepeat(i), 10);
            QCOMPARE(s.firstIdenticalRow(i), 15);
        } else if (i < 30) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 25);
        } else if (i < 210) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 250) {
            QCOMPARE(s.rowRepeat(i), 40);
            QCOMPARE(s.firstIdenticalRow(i), 210);
        } else {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        }
    }
}

void TestRowRepeatStorage::testInsertShiftDown2()
{
    // rect overlapping the end of a row-repeat, with a smaller and a larger row repeat after it
    RowRepeatStorage s;
    s.setRowRepeat(10, 20);
    s.setRowRepeat(100, 5);
    s.setRowRepeat(200, 50);
    s.insertShiftDown(QRect(5, 25, 10, 10));

    for (int i = 1; i <= KS_rowMax; i++) {
        if (i < 10) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 25) {
            QCOMPARE(s.rowRepeat(i), 15);
            QCOMPARE(s.firstIdenticalRow(i), 10);
        } else if (i < 30) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 25);
        } else if (i < 210) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 250) {
            QCOMPARE(s.rowRepeat(i), 40);
            QCOMPARE(s.firstIdenticalRow(i), 210);
        } else {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        }
    }
}

void TestRowRepeatStorage::testInsertShiftDown3()
{
    // rect overlapping the start of a row-repeat, with a smaller and a larger row repeat after it
    RowRepeatStorage s;
    s.setRowRepeat(10, 20);
    s.setRowRepeat(100, 5);
    s.setRowRepeat(200, 50);
    s.insertShiftDown(QRect(5, 5, 10, 10));

    for (int i = 1; i <= KS_rowMax; i++) {
        if (i < 10) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 15) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 10);
        } else if (i < 20) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 30) {
            QCOMPARE(s.rowRepeat(i), 10);
            QCOMPARE(s.firstIdenticalRow(i), 20);
        } else if (i < 210) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 250) {
            QCOMPARE(s.rowRepeat(i), 40);
            QCOMPARE(s.firstIdenticalRow(i), 210);
        } else {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        }
    }
}

void TestRowRepeatStorage::testInsertShiftDown4()
{
    // rect overlapping the start and end of a row-repeat, with a smaller and a larger row repeat after it
    RowRepeatStorage s;
    s.setRowRepeat(10, 20);
    s.setRowRepeat(35, 30);
    s.setRowRepeat(100, 5);
    s.setRowRepeat(200, 50);
    s.insertShiftDown(QRect(5, 15, 10, 25));

    for (int i = 1; i <= KS_rowMax; i++) {
        if (i < 10) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 15) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 10);
        } else if (i < 30) {
            QCOMPARE(s.rowRepeat(i), 15);
            QCOMPARE(s.firstIdenticalRow(i), 15);
        } else if (i < 35) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 40) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 35);
        } else if (i < 55) {
            QCOMPARE(s.rowRepeat(i), 15);
            QCOMPARE(s.firstIdenticalRow(i), 40);
        } else if (i < 60) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 65) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 60);
        } else if (i < 225) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 250) {
            QCOMPARE(s.rowRepeat(i), 25);
            QCOMPARE(s.firstIdenticalRow(i), 225);
        } else {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        }
    }
}

void TestRowRepeatStorage::testRemoveShiftUp1()
{
    // entire rect inside one row-repeat, with a smaller and a larger row repeat after it
    RowRepeatStorage s;
    s.setRowRepeat(10, 20);
    s.setRowRepeat(100, 5);
    s.setRowRepeat(200, 50);
    s.removeShiftUp(QRect(5, 15, 10, 10));

    for (int i = 1; i <= KS_rowMax; i++) {
        if (i < 10) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 15) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 10);
        } else if (i < 20) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 15);
        } else if (i < 200) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 240) {
            QCOMPARE(s.rowRepeat(i), 40);
            QCOMPARE(s.firstIdenticalRow(i), 200);
        } else {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        }
    }
}

void TestRowRepeatStorage::testRemoveShiftUp2()
{
    // rect overlapping the end of a row-repeat, with a smaller and a larger row repeat after it
    RowRepeatStorage s;
    s.setRowRepeat(10, 20);
    s.setRowRepeat(100, 5);
    s.setRowRepeat(200, 50);
    s.removeShiftUp(QRect(5, 25, 10, 10));

    for (int i = 1; i <= KS_rowMax; i++) {
        if (i < 10) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 25) {
            QCOMPARE(s.rowRepeat(i), 15);
            QCOMPARE(s.firstIdenticalRow(i), 10);
        } else if (i < 200) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 240) {
            QCOMPARE(s.rowRepeat(i), 40);
            QCOMPARE(s.firstIdenticalRow(i), 200);
        } else {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        }
    }
}

void TestRowRepeatStorage::testRemoveShiftUp3()
{
    // rect overlapping the start of a row-repeat, with a smaller and a larger row repeat after it
    RowRepeatStorage s;
    s.setRowRepeat(10, 20);
    s.setRowRepeat(100, 5);
    s.setRowRepeat(200, 50);
    s.removeShiftUp(QRect(5, 5, 10, 10));

    for (int i = 1; i <= KS_rowMax; i++) {
        if (i < 10) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 20) {
            QCOMPARE(s.rowRepeat(i), 10);
            QCOMPARE(s.firstIdenticalRow(i), 10);
        } else if (i < 200) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 240) {
            QCOMPARE(s.rowRepeat(i), 40);
            QCOMPARE(s.firstIdenticalRow(i), 200);
        } else {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        }
    }
}

void TestRowRepeatStorage::testRemoveShiftUp4()
{
    // rect overlapping the start and end of a row-repeat, with a smaller and a larger row repeat after it
    RowRepeatStorage s;
    s.setRowRepeat(10, 20);
    s.setRowRepeat(35, 30);
    s.setRowRepeat(100, 5);
    s.setRowRepeat(200, 50);
    s.removeShiftUp(QRect(5, 15, 10, 25));

    for (int i = 1; i <= KS_rowMax; i++) {
        if (i < 10) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 15) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 10);
        } else if (i < 30) {
            QCOMPARE(s.rowRepeat(i), 15);
            QCOMPARE(s.firstIdenticalRow(i), 15);
        } else if (i < 35) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 40) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 35);
        } else if (i < 200) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 225) {
            QCOMPARE(s.rowRepeat(i), 25);
            QCOMPARE(s.firstIdenticalRow(i), 200);
        } else {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        }
    }
}

void TestRowRepeatStorage::testInsertShiftRight()
{
    RowRepeatStorage s;
    s.setRowRepeat(5, 10);
    s.setRowRepeat(20, 10);
    s.setRowRepeat(35, 10);
    s.insertShiftRight(QRect(5, 10, 10, 30));

    for (int i = 1; i <= KS_rowMax; i++) {
        if (i < 5) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 10) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 5);
        } else if (i < 15) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 10);
        } else if (i < 20) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 30) {
            QCOMPARE(s.rowRepeat(i), 10);
            QCOMPARE(s.firstIdenticalRow(i), 20);
        } else if (i < 35) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 40) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 35);
        } else if (i < 45) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 40);
        } else {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        }
    }
}

void TestRowRepeatStorage::testRemoveShiftLeft()
{
    RowRepeatStorage s;
    s.setRowRepeat(5, 10);
    s.setRowRepeat(20, 10);
    s.setRowRepeat(35, 10);
    s.removeShiftLeft(QRect(5, 10, 10, 30));

    for (int i = 1; i <= KS_rowMax; i++) {
        if (i < 5) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 10) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 5);
        } else if (i < 15) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 10);
        } else if (i < 20) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 30) {
            QCOMPARE(s.rowRepeat(i), 10);
            QCOMPARE(s.firstIdenticalRow(i), 20);
        } else if (i < 35) {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        } else if (i < 40) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 35);
        } else if (i < 45) {
            QCOMPARE(s.rowRepeat(i), 5);
            QCOMPARE(s.firstIdenticalRow(i), 40);
        } else {
            QCOMPARE(s.rowRepeat(i), 1);
            QCOMPARE(s.firstIdenticalRow(i), i);
        }
    }
}

QTEST_KDEMAIN(TestRowRepeatStorage, NoGUI)

#include "TestRowRepeatStorage.moc"
