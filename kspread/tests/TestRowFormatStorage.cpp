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
#include "TestRowFormatStorage.h"

#include <QVector>
#include <QDebug>
#include <qtest_kde.h>

#include "../RowFormatStorage.h"
#include "../kspread_limits.h"
#include "../Map.h"
#include "../Sheet.h"
#include "../RowColumnFormat.h"

using namespace KSpread;

void TestRowFormatStorage::initTestCase()
{
    m_map = new Map();
    m_sheet = m_map->createSheet();
}

void TestRowFormatStorage::cleanupTestCase()
{
    delete m_map;
}

void TestRowFormatStorage::testSetRowHeight()
{
    RowFormatStorage s(m_sheet);

    s.setRowHeight(10, 100, 42.0);

    for (int row = 1; row < 10000; row++) {
        QCOMPARE(s.rowHeight(row), row >= 10 && row <= 100 ? 42.0 : m_map->defaultRowFormat()->height());
    }
}

void TestRowFormatStorage::testHidden()
{
    RowFormatStorage s(m_sheet);

    s.setHidden(10, 20, true);
    int first, last;
    QVERIFY(s.isHidden(15, &last, &first));
    QCOMPARE(first, 10);
    QCOMPARE(last, 20);
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

QTEST_KDEMAIN(TestRowFormatStorage, GUI)

#include "TestRowFormatStorage.moc"
