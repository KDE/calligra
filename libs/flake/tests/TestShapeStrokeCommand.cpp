/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "TestShapeStrokeCommand.h"

#include "KoShapeStroke.h"
#include "KoShapeStrokeCommand.h"
#include "KoShapeStrokeModel.h"
#include <MockShapes.h>

#include <QTest>

void TestShapeStrokeCommand::refCounting()
{
    MockShape *shape1 = new MockShape();
    KoShapeStrokeModel *whiteStroke = new KoShapeStroke(1.0, QColor(Qt::white));
    KoShapeStrokeModel *blackStroke = new KoShapeStroke(1.0, QColor(Qt::black));
    KoShapeStrokeModel *redStroke = new KoShapeStroke(1.0, QColor(Qt::red));

    shape1->setStroke(whiteStroke);
    QVERIFY(shape1->stroke() == whiteStroke);
    QCOMPARE(whiteStroke->useCount(), 1);

    // old stroke is white, new stroke is black
    KUndo2Command *cmd1 = new KoShapeStrokeCommand(shape1, blackStroke);
    cmd1->redo();
    QVERIFY(shape1->stroke() == blackStroke);

    // change stroke back to white stroke
    cmd1->undo();
    QVERIFY(shape1->stroke() == whiteStroke);

    // old stroke is white, new stroke is red
    KUndo2Command *cmd2 = new KoShapeStrokeCommand(shape1, redStroke);
    cmd2->redo();
    QVERIFY(shape1->stroke() == redStroke);

    // this command has the white stroke as the old stroke
    delete cmd1;

    // set stroke back to white stroke
    cmd2->undo();
    QVERIFY(shape1->stroke() == whiteStroke);

    // if white is deleted when deleting cmd1 this will crash
    KoInsets insets;
    whiteStroke->strokeInsets(shape1, insets);

    delete cmd2;
    delete shape1;
}

QTEST_MAIN(TestShapeStrokeCommand)
