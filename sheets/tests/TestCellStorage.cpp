/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "TestCellStorage.h"

#include <core/CellStorage.h>
#include <core/Map.h>
#include <core/Sheet.h>
#include <engine/Value.h>

#include <QTest>

using namespace Calligra::Sheets;

void CellStorageTest::testMergedCellsInsertRowBug()
{
    Map map;
    Sheet *sheet = dynamic_cast<Sheet *>(map.addNewSheet());
    CellStorage *storage = sheet->fullCellStorage();

    // | 1 | 4 |
    // |-------|
    // |   2   |
    // |-------|
    // |   | 5 |
    // | 3 |---|
    // |   | 6 |
    // |-------|
    // | 7 | 8 |
    storage->setValue(1, 1, Value(1));
    storage->setValue(2, 1, Value(4));
    storage->setValue(1, 2, Value(2));
    storage->setValue(1, 3, Value(3));
    storage->setValue(2, 3, Value(5));
    storage->setValue(2, 4, Value(6));
    storage->setValue(1, 5, Value(7));
    storage->setValue(2, 5, Value(8));
    storage->mergeCells(1, 2, 1, 0);
    storage->mergeCells(1, 3, 0, 1);
    QCOMPARE(storage->isPartOfMerged(2, 2), true);
    QCOMPARE(storage->isPartOfMerged(3, 2), false);
    QCOMPARE(storage->isPartOfMerged(1, 4), true);
    QCOMPARE(storage->isPartOfMerged(1, 5), false);
    QCOMPARE(storage->mergedXCells(1, 2), 1);
    QCOMPARE(storage->mergedYCells(1, 2), 0);
    QCOMPARE(storage->mergedXCells(1, 3), 0);
    QCOMPARE(storage->mergedYCells(1, 3), 1);

    // insert a row
    storage->insertRows(5, 1);

    // validate result
    QCOMPARE(storage->value(1, 1), Value(1));
    QCOMPARE(storage->value(2, 1), Value(4));
    QCOMPARE(storage->value(1, 2), Value(2));
    QCOMPARE(storage->value(1, 3), Value(3));
    QCOMPARE(storage->value(2, 3), Value(5));
    QCOMPARE(storage->value(2, 4), Value(6));
    QCOMPARE(storage->value(1, 6), Value(7));
    QCOMPARE(storage->value(2, 6), Value(8));
    QCOMPARE(storage->isPartOfMerged(2, 2), true);
    QCOMPARE(storage->isPartOfMerged(3, 2), false);
    QCOMPARE(storage->isPartOfMerged(1, 4), true);
    QCOMPARE(storage->isPartOfMerged(1, 5), false);
    QCOMPARE(storage->mergedXCells(1, 2), 1);
    QCOMPARE(storage->mergedYCells(1, 2), 0);
    QCOMPARE(storage->mergedXCells(1, 3), 0);
    QCOMPARE(storage->mergedYCells(1, 3), 1);
}

QTEST_MAIN(CellStorageTest)
