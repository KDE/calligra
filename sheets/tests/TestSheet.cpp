/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "TestSheet.h"

#include <core/Cell.h>
#include <core/Map.h>
#include <core/Sheet.h>

#include <QTest>

using namespace Calligra::Sheets;

void SheetTest::init()
{
    m_map = new Map(nullptr);
    m_map->addNewSheet();
    m_sheet = dynamic_cast<Sheet *>(m_map->sheet(0));
    m_map->setDefaultRowHeight(10.0);
    m_map->setDefaultColumnWidth(10.0);
}

void SheetTest::cleanup()
{
    delete m_map;
}

void SheetTest::testRemoveRows_data()
{
    QTest::addColumn<int>("col");
    QTest::addColumn<int>("row");
    QTest::addColumn<QString>("formula");
    QTest::addColumn<int>("rowToRemove");
    QTest::addColumn<int>("numRows");
    QTest::addColumn<QString>("result");

    QTest::newRow("exact row") << 1 << 1 << "=C4" << 4 << 1 << "=#Dependency!";
    QTest::newRow("earlier row") << 1 << 1 << "=C4" << 3 << 1 << "=C3";
    QTest::newRow("later row") << 1 << 1 << "=C4" << 5 << 1 << "=C4";

    QTest::newRow("range before start") << 1 << 1 << "=SUM(C4:C7)" << 3 << 1 << "=SUM(C3:C6)";
    QTest::newRow("range start row") << 1 << 1 << "=SUM(C4:C7)" << 4 << 1 << "=SUM(C4:C6)";
    QTest::newRow("range middle row") << 1 << 1 << "=SUM(C4:C7)" << 5 << 1 << "=SUM(C4:C6)";
    QTest::newRow("range end row") << 1 << 1 << "=SUM(C4:C7)" << 7 << 1 << "=SUM(C4:C6)";
    QTest::newRow("range after end") << 1 << 1 << "=SUM(C4:C7)" << 8 << 1 << "=SUM(C4:C7)";
    QTest::newRow("entire range") << 1 << 1 << "=SUM(C4:C4)" << 4 << 1 << "=SUM(#Dependency!)";

    QTest::newRow("2d range before start") << 1 << 1 << "=SUM(C4:E7)" << 3 << 1 << "=SUM(C3:E6)";
    QTest::newRow("2d range start row") << 1 << 1 << "=SUM(C4:E7)" << 4 << 1 << "=SUM(C4:E6)";
    QTest::newRow("2d range middle row") << 1 << 1 << "=SUM(C4:E7)" << 5 << 1 << "=SUM(C4:E6)";
    QTest::newRow("2d range end row") << 1 << 1 << "=SUM(C4:E7)" << 7 << 1 << "=SUM(C4:E6)";
    QTest::newRow("2d range after end") << 1 << 1 << "=SUM(C4:E7)" << 8 << 1 << "=SUM(C4:E7)";

    QTest::newRow("refer to last deleted row") << 1 << 1 << "=A4" << 2 << 3 << "=#Dependency!";
    QTest::newRow("refer to first not deleted row") << 1 << 1 << "=A4" << 2 << 2 << "=A2";

    // Bug 313056
    QTest::newRow("bug313056_1") << 4 << 5 << "=A1" << 3 << 2 << "=A1";
    QTest::newRow("bug313056_2") << 2 << 32 << "=E9" << 5 << 26 << "=#Dependency!";
}

void SheetTest::testRemoveRows()
{
    QFETCH(int, col);
    QFETCH(int, row);
    QFETCH(QString, formula);
    QFETCH(int, rowToRemove);
    QFETCH(int, numRows);
    QFETCH(QString, result);

    Cell cell(m_sheet, col, row);
    cell.parseUserInput(formula);
    m_sheet->removeRows(rowToRemove, numRows);

    QCOMPARE(cell.userInput(), result);
}

void SheetTest::testRemoveColumns_data()
{
    QTest::addColumn<QString>("formula");
    QTest::addColumn<int>("columnToRemove");
    QTest::addColumn<QString>("result");

    QTest::newRow("exact col") << "=C4" << 3 << "=#Dependency!";
    QTest::newRow("earlier col") << "=C4" << 2 << "=B4";
    QTest::newRow("later col") << "=C4" << 4 << "=C4";

    QTest::newRow("range before start") << "=SUM(C4:E4)" << 2 << "=SUM(B4:D4)";
    QTest::newRow("range start row") << "=SUM(C4:E4)" << 3 << "=SUM(C4:D4)";
    QTest::newRow("range middle row") << "=SUM(C4:E4)" << 4 << "=SUM(C4:D4)";
    QTest::newRow("range end row") << "=SUM(C4:E4)" << 5 << "=SUM(C4:D4)";
    QTest::newRow("range after end") << "=SUM(C4:E4)" << 6 << "=SUM(C4:E4)";
    QTest::newRow("entire range") << "=SUM(C4:C4)" << 3 << "=SUM(#Dependency!)";

    QTest::newRow("2d range before start") << "=SUM(C4:E7)" << 2 << "=SUM(B4:D7)";
    QTest::newRow("2d range start row") << "=SUM(C4:E7)" << 3 << "=SUM(C4:D7)";
    QTest::newRow("2d range middle row") << "=SUM(C4:E7)" << 4 << "=SUM(C4:D7)";
    QTest::newRow("2d range end row") << "=SUM(C4:E7)" << 5 << "=SUM(C4:D7)";
    QTest::newRow("2d range after end") << "=SUM(C4:E7)" << 6 << "=SUM(C4:E7)";
}

void SheetTest::testRemoveColumns()
{
    QFETCH(QString, formula);
    QFETCH(int, columnToRemove);
    QFETCH(QString, result);

    Cell cell(m_sheet, 1, 1);
    cell.parseUserInput(formula);
    m_sheet->removeColumns(columnToRemove, 1);

    QCOMPARE(cell.userInput(), result);
}

void SheetTest::testDocumentToCellCoordinates_data()
{
    QTest::addColumn<QRectF>("area");
    QTest::addColumn<QRect>("result");

    QTest::newRow("simple") << QRectF(5, 5, 10, 10) << QRect(1, 1, 2, 2);
    QTest::newRow("bigger") << QRectF(5, 5, 200, 100) << QRect(1, 1, 21, 11);
    QTest::newRow("wide") << QRectF(5, 5, 300000, 10) << QRect(1, 1, 30001, 2);
    QTest::newRow("tall") << QRectF(5, 5, 10, 300000) << QRect(1, 1, 2, 30001);
    QTest::newRow("very tall") << QRectF(5, 5, 10, 10000000) << QRect(1, 1, 2, 1000001);
}

void SheetTest::testDocumentToCellCoordinates()
{
    QFETCH(QRectF, area);
    QFETCH(QRect, result);

    QCOMPARE(m_sheet->documentToCellCoordinates(area), result);
}

QTEST_MAIN(SheetTest)
