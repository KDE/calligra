/* This file is part of the KDE project
   Copyright 2011 Juan Aquino <utcl95@gmail.com>

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
#include "TestSort.h"

#include <QTest>

#include <KoCanvasBase.h>

#include "CellStorage.h"
#include "Map.h"
#include "Region.h"
#include "Sheet.h"
#include "ui/Selection.h"
#include "../commands/SortManipulator.h"

using namespace Calligra::Sheets;

void TestSort::AscendingOrder()
{
    Map map;
    Sheet* sheet = new Sheet(&map, "Sheet1");
    map.addSheet(sheet);

    Value cellvalue;

    KoCanvasBase* canvas = 0;
    Selection* selection = new Selection(canvas);

    selection->setActiveSheet(sheet);

    CellStorage* storage = sheet->cellStorage();
    // Data to sort...
    // Ascending
    // A1 3     
    // A2 Empty 
    // A3 1     
    // more...

    // Ascending
    storage->setValue(1,1, Value(3));
    storage->setValue(1,2, Value());
    storage->setValue(1,3, Value(1));

    // Selection
    selection->clear();
    selection->initialize(QRect(1,1,1,3), sheet);
    QCOMPARE(selection->name(), QString("Sheet1!A1:A3"));

    // Sort Manipulator
    SortManipulator *const command = new SortManipulator();
    command->setRegisterUndo(0);
    command->setSheet(sheet);

    // Parameters.
    command->setSortRows(Qt::Vertical);
    command->setSkipFirst(false);
    command->setCopyFormat(false);

    command->addCriterion(0, Qt::AscendingOrder, Qt::CaseInsensitive);

    command->add(selection->lastRange());
    QCOMPARE(selection->lastRange(), QRect(1,1,1,3));

    // Execute sort
    command->execute(selection->canvas());

    QCOMPARE(storage->value(1,1),Value(1));
    QCOMPARE(storage->value(1,2),Value(3));
    QCOMPARE(storage->value(1,3),Value());
}

void TestSort::DescendingOrder()
{
    Map map;
    Sheet* sheet = new Sheet(&map, "Sheet1");
    map.addSheet(sheet);

    Value cellvalue;

    KoCanvasBase* canvas = 0;
    Selection* selection = new Selection(canvas);

    selection->setActiveSheet(sheet);

    CellStorage* storage = sheet->cellStorage();
    // Data to sort...
    // Descending
    // B1 1 
    // B2 Empty 
    // B3 3

    // Descending 
    storage->setValue(2,1, Value(1));
    storage->setValue(2,2, Value());
    storage->setValue(2,3, Value(3));

    // Selection
    selection->clear();
    selection->initialize(QRect(2,1,1,3), sheet);
    QCOMPARE(selection->name(), QString("Sheet1!B1:B3"));

    // Sort Manipulator
    SortManipulator *const command = new SortManipulator();
    command->setRegisterUndo(0);
    command->setSheet(sheet);

    // Parameters.
    command->setSortRows(Qt::Vertical);
    command->setSkipFirst(false);
    command->setCopyFormat(false);

    command->addCriterion(0, Qt::DescendingOrder, Qt::CaseInsensitive);

    command->add(selection->lastRange());
    QCOMPARE(selection->lastRange(), QRect(2,1,1,3));

    // Execute sort
    command->execute(selection->canvas());

    QCOMPARE(storage->value(2,1),Value(3));
    QCOMPARE(storage->value(2,2),Value(1));
    QCOMPARE(storage->value(2,3),Value());
}

QTEST_MAIN(TestSort)
