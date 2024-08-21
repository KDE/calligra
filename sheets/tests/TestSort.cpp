/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Juan Aquino <utcl95@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "TestSort.h"

#include <QTest>

#include "core/Map.h"
#include "core/Sheet.h"
#include "engine/CellBaseStorage.h"
#include "engine/Region.h"
#include "ui/Selection.h"
#include "ui/actions/Sort.h"

using namespace Calligra::Sheets;

void TestSort::AscendingOrder()
{
    m_map = new Map;
    m_map->addNewSheet();
    Sheet *sheet = dynamic_cast<Sheet *>(m_map->sheet(0));

    Value cellvalue;

    KoCanvasBase *canvas = nullptr;
    Selection *selection = new Selection(canvas);

    selection->setActiveSheet(sheet);

    CellBaseStorage *storage = sheet->cellStorage();
    // Data to sort...
    // Ascending
    // A1 3
    // A2 Empty
    // A3 1
    // more...

    // Ascending
    storage->setValue(1, 1, Value(3));
    storage->setValue(1, 2, Value());
    storage->setValue(1, 3, Value(1));

    // Selection
    selection->clear();
    selection->initialize(QRect(1, 1, 1, 3), sheet);
    QCOMPARE(selection->name(), QString("Sheet1!A1:A3"));

    // Sort Manipulator
    SortManipulator *const command = new SortManipulator();
    command->setRegisterUndo(0);
    command->setSheet(sheet);

    // Parameters.
    command->setSortRows(true);
    command->setSkipFirst(false);
    command->setCopyFormat(false);

    command->addCriterion(0, Qt::AscendingOrder, Qt::CaseInsensitive);

    command->add(*selection);
    QCOMPARE(selection->lastRange(), QRect(1, 1, 1, 3));

    // Execute sort
    command->execute(selection->canvas());

    QCOMPARE(storage->value(1, 1), Value(1));
    QCOMPARE(storage->value(1, 2), Value(3));
    QCOMPARE(storage->value(1, 3), Value());
}

void TestSort::DescendingOrder()
{
    m_map = new Map;
    m_map->addNewSheet();
    Sheet *sheet = dynamic_cast<Sheet *>(m_map->sheet(0));

    Value cellvalue;

    KoCanvasBase *canvas = nullptr;
    Selection *selection = new Selection(canvas);

    selection->setActiveSheet(sheet);

    CellBaseStorage *storage = sheet->cellStorage();
    // Data to sort...
    // Descending
    // B1 1
    // B2 Empty
    // B3 3

    // Descending
    storage->setValue(2, 1, Value(1));
    storage->setValue(2, 2, Value());
    storage->setValue(2, 3, Value(3));

    // Selection
    selection->clear();
    selection->initialize(QRect(2, 1, 1, 3), sheet);
    QCOMPARE(selection->name(), QString("Sheet1!B1:B3"));

    // Sort Manipulator
    SortManipulator *const command = new SortManipulator();
    command->setRegisterUndo(0);
    command->setSheet(sheet);

    // Parameters.
    command->setSortRows(true);
    command->setSkipFirst(false);
    command->setCopyFormat(false);

    command->addCriterion(0, Qt::DescendingOrder, Qt::CaseInsensitive);

    command->add(*selection);
    QCOMPARE(selection->lastRange(), QRect(2, 1, 1, 3));

    // Execute sort
    command->execute(selection->canvas());

    QCOMPARE(storage->value(2, 1), Value(3));
    QCOMPARE(storage->value(2, 2), Value(1));
    QCOMPARE(storage->value(2, 3), Value());
}

QTEST_MAIN(TestSort)
