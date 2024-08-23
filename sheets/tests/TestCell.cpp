/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2011 Sebastian Sauer <sebastian.sauer@kdab.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "TestCell.h"

#include <KoDocumentResourceManager.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoShapeLoadingContext.h>
#include <KoStore.h>

#include <core/CellStorage.h>
#include <core/Map.h>
#include <core/Sheet.h>
#include <core/odf/OdfLoadingContext.h>
#include <core/odf/SheetsOdf.h>
#include <engine/Value.h>

#include <QBuffer>
#include <QTest>

using namespace Calligra::Sheets;

KoXmlDocument CellTest::xmlDocument(const QString &content)
{
    KoXmlDocument document;
    QString xml =
        "<table:table-cell xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" "
        "xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" "
        "xmlns:number=\"urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0\" >"
        + content + "</table:table-cell>";
    bool ok = document.setContent(xml, true);
    return ok ? document : KoXmlDocument();
}

void CellTest::testRichText()
{
    KoOdfStylesReader stylesReader;

    QBuffer buffer;
    buffer.open(QIODevice::ReadOnly);
    KoStore *store = KoStore::createStore(&buffer, KoStore::Read);

    KoOdfLoadingContext odfContext(stylesReader, store);
    Odf::OdfLoadingContext context(odfContext);

    KoDocumentResourceManager documentResources;
    KoShapeLoadingContext shapeContext(odfContext, &documentResources);
    context.shapeContext = &shapeContext;

    Styles autoStyles;
    QString cellStyleName;

    Map map;
    Sheet *sheet = dynamic_cast<Sheet *>(map.addNewSheet());
    CellStorage *storage = sheet->fullCellStorage();
    storage->setValue(1, 1, Value(1));

    Cell cell = storage->firstInRow(1);
    QVERIFY(!cell.isNull());

    { // Test the simple case. Only one paragraph with some simple text.
        KoXmlDocument doc = xmlDocument("<text:p>Some text</text:p>");
        KoXmlElement e = doc.documentElement();
        QVERIFY(!e.isNull());
        Odf::loadCellText(&cell, e, context, autoStyles, cellStyleName);
        QVERIFY(!cell.isNull());
        QVERIFY(!cell.richText());
        QVERIFY(cell.userInput().split('\n').count() == 1);
    }

    { // Text in the paragraph and in a child text:span means rich-text.
        KoXmlDocument doc = xmlDocument("<text:p>First<text:span>Second<text:span>Theird</text:span></text:span></text:p>");
        KoXmlElement e = doc.documentElement();
        QVERIFY(!e.isNull());
        Odf::loadCellText(&cell, e, context, autoStyles, cellStyleName);
        QVERIFY(!cell.isNull());
        QVERIFY(cell.richText());
        QVERIFY(cell.userInput().split('\n').count() == 1);
    }

    { // The text:line-break should be translated into a \n newline and since there is no other rich-text it should not be detected as such.
        KoXmlDocument doc = xmlDocument("<text:p>First<text:line-break/>Second</text:p>");
        KoXmlElement e = doc.documentElement();
        QVERIFY(!e.isNull());
        Odf::loadCellText(&cell, e, context, autoStyles, cellStyleName);
        QVERIFY(!cell.isNull());
        QVERIFY(!cell.richText());
        QVERIFY(cell.userInput().split('\n').count() == 2);
    }

    { // The text:s and text:tab should be translated into space and tabulator. No rich-text else.
        KoXmlDocument doc = xmlDocument("<text:p>First<text:s/>Second<text:tab/>Theird</text:p>");
        KoXmlElement e = doc.documentElement();
        QVERIFY(!e.isNull());
        Odf::loadCellText(&cell, e, context, autoStyles, cellStyleName);
        QVERIFY(!cell.isNull());
        QVERIFY(!cell.richText());
        QVERIFY(cell.userInput().split('\n').count() == 1);
    }
}

QTEST_MAIN(CellTest)
