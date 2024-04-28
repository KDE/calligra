/*
 *  This file is part of Calligra tests
 *
 *  SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "TestTextTool.h"

#include "../TextTool.h"
#include "MockTextShape.h"

#include <tests/MockShapes.h>

void TestTextTool::testCaretRect()
{
    TextTool tool(new MockCanvas());

    MockTextShape *shape1 = new MockTextShape();
    QVERIFY(shape1->userData());
    QVERIFY(qobject_cast<KoTextShapeData *>(shape1->userData()));

    tool.setShapeData(qobject_cast<KoTextShapeData *>(shape1->userData()));
    QVERIFY(tool.textEditor());

    QTextCursor cursor(qobject_cast<KoTextShapeData *>(shape1->userData())->document());
    cursor.insertText("foo\n");

    QCOMPARE(tool.caretRect(0), QRectF()); // invalid
    QCOMPARE(tool.caretRect(14), QRectF()); // invalid

    shape1->layout->layout();

    QCOMPARE(tool.caretRect(0).topLeft(), QPointF());
    // second line should be lower.
    // bug 213238
    QVERIFY(tool.caretRect(4).topLeft().y() > 0);
}

QTEST_KDEMAIN(TestTextTool, GUI)
