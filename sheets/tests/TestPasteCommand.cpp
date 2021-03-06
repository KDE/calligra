// This file is part of the KDE project
// SPDX-FileCopyrightText: 2012 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#include "TestPasteCommand.h"

#include <QTest>

#include "MockPart.h"

#include "part/CanvasItem.h"
#include "part/Doc.h"
#include "Map.h"
#include "ui/Selection.h"
#include "Sheet.h"
#include <Value.h>
#include "commands/PasteCommand.h"

using namespace Calligra::Sheets;

void PasteCommandTest::testKSpreadSnippet()
{
    Doc doc(new MockPart);
    Map *map = doc.map();
    Sheet* sheet = new Sheet(map, "Sheet1");
    map->addSheet(sheet);
    CanvasItem canvas(&doc);
    Selection selection(&canvas);
    selection.setActiveSheet(sheet);
    selection.initialize(QPoint(2, 4), sheet);

    QMimeData *mimedata = new QMimeData();
    mimedata->setData("application/x-kspread-snippet",
                      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                      "<!DOCTYPE spreadsheet-snippet>\n"
                      "<spreadsheet-snippet rows=\"1\" columns=\"1\">\n"
                      " <cell row=\"1\" column=\"1\">\n"
                      "  <text outStr=\"3\" dataType=\"Num\">3</text>\n"
                      " </cell>\n"
                      "</spreadsheet-snippet>\n");

    PasteCommand *command = new PasteCommand();
    command->setSheet(selection.activeSheet());
    command->add(selection);
    command->setMimeData(mimedata);
    command->setPasteFC(true);
    command->execute(&canvas);

    qDebug() << Cell(sheet, 2, 4).value() << Cell(sheet, 1, 3).value() << Value(3);
    QCOMPARE(Cell(sheet, 2, 4).value(), Value(3));
}

QTEST_MAIN(PasteCommandTest)
