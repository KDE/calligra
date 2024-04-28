// This file is part of the KDE project
// SPDX-FileCopyrightText: 2012 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#include "TestPasteCommand.h"

#include <QMimeData>
#include <QTest>

#include "MockPart.h"

#include "core/Map.h"
#include "core/Sheet.h"
#include "part/CanvasItem.h"
#include "part/Doc.h"
#include "ui/Selection.h"
#include "ui/commands/PasteCommand.h"
#include <engine/Value.h>

using namespace Calligra::Sheets;

void PasteCommandTest::testKSpreadSnippet()
{
    Doc doc(new MockPart);
    Map *map = doc.map();
    Sheet *sheet = new Sheet(map, "Sheet1");
    map->addSheet(sheet);
    Cell(sheet, 1, 3).setCellValue(Value(3));
    Cell(sheet, 1, 4).setUserInput("=2*A3+7*A3");
    Cell(sheet, 1, 5).setCellValue(Value(71));
    Cell(sheet, 2, 3).setCellValue(Value(4));
    map->flushDamages();
    CanvasItem canvas(&doc);
    Selection selection(&canvas);
    selection.setActiveSheet(sheet);
    selection.initialize(QPoint(2, 4), sheet);

    QMimeData *mimedata = new QMimeData();
    mimedata->setData("application/x-calligra-sheets-snippet", "range 1 4 1 5 Sheet1\n");

    PasteCommand *command = new PasteCommand();
    command->setSheet(selection.activeSheet());
    command->add(selection);
    command->setMimeData(mimedata, true);
    command->setPasteFC(true);
    qDebug() << (*command);
    command->execute(&canvas);
    map->flushDamages();

    qDebug() << Cell(sheet, 2, 3).value() << Cell(sheet, 1, 3).value();
    qDebug() << Cell(sheet, 2, 4).value() << Cell(sheet, 1, 4).value();
    QCOMPARE(Cell(sheet, 2, 4).value(), Value(36));
    QCOMPARE(Cell(sheet, 2, 4).userInput(), QString("=2*B3+7*B3"));
    QCOMPARE(Cell(sheet, 2, 5).value(), Value(71));
}

QTEST_MAIN(PasteCommandTest)
