/*
 *  This file is part of Calligra tests
 *
 *  SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "TestShapeAt.h"
#include <MockShapes.h>

#include <FlakeDebug.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoShapeShadow.h>
#include <KoShapeStroke.h>

#include <QTest>

void TestShapeAt::test()
{
    MockShape shape1;
    MockShape shape2;
    MockShape shape3;

    MockCanvas canvas;
    KoShapeManager manager(&canvas);
    shape1.setPosition(QPointF(100, 100));
    shape1.setSize(QSizeF(50, 50));
    shape1.setZIndex(0);
    manager.addShape(&shape1);

    QVERIFY(manager.shapeAt(QPointF(90, 90)) == nullptr);
    QVERIFY(manager.shapeAt(QPointF(110, 140)) != nullptr);
    QVERIFY(manager.shapeAt(QPointF(100, 100)) != nullptr);
    QVERIFY(manager.shapeAt(QPointF(100, 100), KoFlake::Selected) == nullptr);
    QVERIFY(manager.shapeAt(QPointF(100, 100), KoFlake::Unselected) != nullptr);
    QVERIFY(manager.shapeAt(QPointF(100, 100), KoFlake::NextUnselected) != nullptr);

    shape2.setPosition(QPointF(80, 80));
    shape2.setSize(QSizeF(50, 50));
    shape2.setZIndex(1);
    manager.addShape(&shape2);

    QVERIFY(manager.shapeAt(QPointF(200, 200)) == nullptr);
    QCOMPARE(manager.shapeAt(QPointF(90, 90)), &shape2);
    QCOMPARE(manager.shapeAt(QPointF(105, 105)), &shape2); // the one on top
    KoShape *dummy = nullptr;
    QCOMPARE(manager.shapeAt(QPointF(105, 105), KoFlake::Selected), dummy);
    QCOMPARE(manager.shapeAt(QPointF(105, 105), KoFlake::Unselected), &shape2); // the one on top
    QCOMPARE(manager.shapeAt(QPointF(105, 105), KoFlake::NextUnselected), &shape2);

    manager.selection()->select(&shape2);

    QVERIFY(manager.shapeAt(QPointF(200, 200)) == nullptr);
    QCOMPARE(manager.shapeAt(QPointF(90, 90)), &shape2);
    QCOMPARE(manager.shapeAt(QPointF(105, 105)), &shape2); // the one on top
    QCOMPARE(manager.shapeAt(QPointF(105, 105), KoFlake::Selected), &shape2);
    QCOMPARE(manager.shapeAt(QPointF(105, 105), KoFlake::Unselected), &shape1);
    QCOMPARE(manager.shapeAt(QPointF(105, 105), KoFlake::NextUnselected), &shape1);

    shape3.setPosition(QPointF(120, 80));
    shape3.setSize(QSizeF(50, 50));
    shape3.setZIndex(2);
    manager.addShape(&shape3);

    QVERIFY(manager.shapeAt(QPointF(200, 200)) == nullptr);
    QCOMPARE(manager.shapeAt(QPointF(90, 90)), &shape2);

    QVERIFY(manager.shapeAt(QPointF(200, 200)) == nullptr);
    QCOMPARE(manager.shapeAt(QPointF(90, 90)), &shape2);
    QCOMPARE(manager.shapeAt(QPointF(105, 145)), &shape1);
    QCOMPARE(manager.shapeAt(QPointF(165, 90)), &shape3);

    QCOMPARE(manager.shapeAt(QPointF(125, 105)), &shape3); // the one on top
    QCOMPARE(manager.shapeAt(QPointF(105, 105), KoFlake::Selected), &shape2);
    QCOMPARE(manager.shapeAt(QPointF(105, 105), KoFlake::Unselected), &shape1);
    QCOMPARE(manager.shapeAt(QPointF(105, 105), KoFlake::NextUnselected), &shape1);

    QCOMPARE(manager.shapeAt(QPointF(125, 105), KoFlake::Selected), &shape2);
    QCOMPARE(manager.shapeAt(QPointF(125, 105), KoFlake::Unselected), &shape3);
    QCOMPARE(manager.shapeAt(QPointF(125, 105), KoFlake::NextUnselected), &shape1);

    // test omitHiddenShapes
    QCOMPARE(manager.shapeAt(QPointF(125, 105), KoFlake::Selected, true), &shape2);
    QCOMPARE(manager.shapeAt(QPointF(125, 105), KoFlake::Unselected, true), &shape3);
    QCOMPARE(manager.shapeAt(QPointF(125, 105), KoFlake::NextUnselected, true), &shape1);

    shape3.setVisible(false);
    QCOMPARE(manager.shapeAt(QPointF(125, 105), KoFlake::Selected, true), &shape2);
    QCOMPARE(manager.shapeAt(QPointF(125, 105), KoFlake::Unselected, true), &shape1);
    QCOMPARE(manager.shapeAt(QPointF(125, 105), KoFlake::NextUnselected, true), &shape1);
}

void TestShapeAt::testShadow()
{
    QRectF bbox(20, 30, 50, 70);

    MockShape shape;
    shape.setPosition(bbox.topLeft());
    shape.setSize(bbox.size());
    QCOMPARE(shape.boundingRect(), bbox);

    KoShapeStroke *stroke = new KoShapeStroke();
    stroke->setLineWidth(20); // which means the shape grows 10 in all directions.
    shape.setStroke(stroke);
    KoInsets strokeInsets;
    stroke->strokeInsets(&shape, strokeInsets);
    bbox.adjust(-strokeInsets.left, -strokeInsets.top, strokeInsets.right, strokeInsets.bottom);
    QCOMPARE(shape.boundingRect(), bbox);

    KoShapeShadow *shadow = new KoShapeShadow();
    shadow->setOffset(QPointF(5, 9));
    shape.setShadow(shadow);
    KoInsets shadowInsets;
    shadow->insets(shadowInsets);
    bbox.adjust(-shadowInsets.left, -shadowInsets.top, shadowInsets.right, shadowInsets.bottom);
    QCOMPARE(shape.boundingRect(), bbox);
}

QTEST_MAIN(TestShapeAt)
