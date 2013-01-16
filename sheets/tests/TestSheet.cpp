/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

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
#include "TestSheet.h"

#include <QPainter>

#include <KoViewConverter.h>
#include <KoShape.h>
#include <KoShapeSavingContext.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoPart.h>

#include <part/Doc.h> // FIXME detach from part
#include <Map.h>
#include <Sheet.h>
#include <CellStorage.h>
#include <OdfSavingContext.h>

#include <qtest_kde.h>

using namespace Calligra::Sheets;

class MockShape : public KoShape
{
public:
    MockShape() : KoShape() {}
    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &) {
        Q_UNUSED(painter);
        Q_UNUSED(converter);
    }
    virtual void saveOdf(KoShapeSavingContext &) const {}
    virtual bool loadOdf(const KoXmlElement &, KoShapeLoadingContext &) {
        return true;
    }
};

void SheetTest::init()
{
    m_doc = new Doc(new MockPart);
    m_doc->map()->addNewSheet();
    m_sheet = m_doc->map()->sheet(0);
    m_sheet->map()->setDefaultRowHeight(10.0);
    m_sheet->map()->setDefaultColumnWidth(10.0);
}

void SheetTest::cleanup()
{
    delete m_doc;
}

void SheetTest::testRemoveRows_data()
{
    QTest::addColumn<QString>("formula");
    QTest::addColumn<int>("rowToRemove");
    QTest::addColumn<QString>("result");

    QTest::newRow("exact row")   << "=C4" << 4 << "=#Dependency!";
    QTest::newRow("earlier row") << "=C4" << 3 << "=C3";
    QTest::newRow("later row")   << "=C4" << 5 << "=C4";

    QTest::newRow("range before start") << "=SUM(C4:C7)" << 3 << "=SUM(C3:C6)";
    QTest::newRow("range start row")    << "=SUM(C4:C7)" << 4 << "=SUM(C4:C6)";
    QTest::newRow("range middle row")   << "=SUM(C4:C7)" << 5 << "=SUM(C4:C6)";
    QTest::newRow("range end row")      << "=SUM(C4:C7)" << 7 << "=SUM(C4:C6)";
    QTest::newRow("range after end")    << "=SUM(C4:C7)" << 8 << "=SUM(C4:C7)";
    QTest::newRow("entire range")       << "=SUM(C4:C4)" << 4 << "=SUM(#Dependency!:#Dependency!)";

    QTest::newRow("2d range before start") << "=SUM(C4:E7)" << 3 << "=SUM(C3:E6)";
    QTest::newRow("2d range start row")    << "=SUM(C4:E7)" << 4 << "=SUM(C4:E6)";
    QTest::newRow("2d range middle row")   << "=SUM(C4:E7)" << 5 << "=SUM(C4:E6)";
    QTest::newRow("2d range end row")      << "=SUM(C4:E7)" << 7 << "=SUM(C4:E6)";
    QTest::newRow("2d range after end")    << "=SUM(C4:E7)" << 8 << "=SUM(C4:E7)";
}

void SheetTest::testRemoveRows()
{
    QFETCH(QString, formula);
    QFETCH(int, rowToRemove);
    QFETCH(QString, result);

    Cell cell(m_sheet, 1, 1);
    cell.setUserInput(formula);
    m_sheet->removeRows(rowToRemove, 1);

    QCOMPARE(cell.userInput(), result);
}

void SheetTest::testRemoveColumns_data()
{
    QTest::addColumn<QString>("formula");
    QTest::addColumn<int>("columnToRemove");
    QTest::addColumn<QString>("result");

    QTest::newRow("exact col")   << "=C4" << 3 << "=#Dependency!";
    QTest::newRow("earlier col") << "=C4" << 2 << "=B4";
    QTest::newRow("later col")   << "=C4" << 4 << "=C4";

    QTest::newRow("range before start") << "=SUM(C4:E4)" << 2 << "=SUM(B4:D4)";
    QTest::newRow("range start row")    << "=SUM(C4:E4)" << 3 << "=SUM(C4:D4)";
    QTest::newRow("range middle row")   << "=SUM(C4:E4)" << 4 << "=SUM(C4:D4)";
    QTest::newRow("range end row")      << "=SUM(C4:E4)" << 5 << "=SUM(C4:D4)";
    QTest::newRow("range after end")    << "=SUM(C4:E4)" << 6 << "=SUM(C4:E4)";
    QTest::newRow("entire range")       << "=SUM(C4:C4)" << 3 << "=SUM(#Dependency!:#Dependency!)";

    QTest::newRow("2d range before start") << "=SUM(C4:E7)" << 2 << "=SUM(B4:D7)";
    QTest::newRow("2d range start row")    << "=SUM(C4:E7)" << 3 << "=SUM(C4:D7)";
    QTest::newRow("2d range middle row")   << "=SUM(C4:E7)" << 4 << "=SUM(C4:D7)";
    QTest::newRow("2d range end row")      << "=SUM(C4:E7)" << 5 << "=SUM(C4:D7)";
    QTest::newRow("2d range after end")    << "=SUM(C4:E7)" << 6 << "=SUM(C4:E7)";
}

void SheetTest::testRemoveColumns()
{
    QFETCH(QString, formula);
    QFETCH(int, columnToRemove);
    QFETCH(QString, result);

    Cell cell(m_sheet, 1, 1);
    cell.setUserInput(formula);
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

// test if embedded objects are propare taken into account (tests for bug 287997)
void SheetTest::testCompareRows()
{
    CellStorage* storage = m_sheet->cellStorage();
    storage->insertRows(1, 15);

    QBuffer buf;
    buf.open(QIODevice::ReadWrite);
    KoXmlWriter xmlWriter(&buf);
    KoGenStyles mainStyles;
    KoEmbeddedDocumentSaver embeddedSaver;
    KoShapeSavingContext shapeContext(xmlWriter, mainStyles, embeddedSaver);
    OdfSavingContext tableContext(shapeContext);

    MockShape shape;
    tableContext.insertCellAnchoredShape(m_sheet, 20, 5, &shape);

    QCOMPARE(m_sheet->compareRows(12,19,1024,tableContext), true);
    QCOMPARE(m_sheet->compareRows(12,20,1024,tableContext), false);
    QCOMPARE(m_sheet->compareRows(12,21,1024,tableContext), true);

    storage->insertRows(1, 15);

    QCOMPARE(m_sheet->compareRows(12,19,1024,tableContext), true);
    QCOMPARE(m_sheet->compareRows(12,20,1024,tableContext), false);
    QCOMPARE(m_sheet->compareRows(12,21,1024,tableContext), true);
}

QTEST_KDEMAIN(SheetTest, GUI)

#include "TestSheet.moc"
