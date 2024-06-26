/*
 *  This file is part of Calligra tests
 *
 *  SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "TestShapePainting.h"

#include "KoShapeContainer.h"
#include "KoShapeManager.h"
#include "KoShapePaintingContext.h"

#include <MockShapes.h>

#include <QTest>

#include <algorithm>

void TestShapePainting::testPaintShape()
{
    MockShape *shape1 = new MockShape();
    MockShape *shape2 = new MockShape();
    MockContainer *container = new MockContainer();

    container->addShape(shape1);
    container->addShape(shape2);
    QCOMPARE(shape1->parent(), container);
    QCOMPARE(shape2->parent(), container);
    container->setClipped(shape1, false);
    container->setClipped(shape2, false);
    QCOMPARE(container->isClipped(shape1), false);
    QCOMPARE(container->isClipped(shape2), false);

    MockCanvas canvas;
    KoShapeManager manager(&canvas);
    manager.addShape(container);
    QCOMPARE(manager.shapes().count(), 3);

    QImage image(100, 100, QImage::Format_Mono);
    QPainter painter(&image);
    KoViewConverter vc;
    manager.paint(painter, vc, false);

    // with the shape not being clipped, the shapeManager will paint it for us.
    QCOMPARE(shape1->paintedCount, 1);
    QCOMPARE(shape2->paintedCount, 1);
    QCOMPARE(container->paintedCount, 1);

    // the container should thus not paint the shape
    shape1->paintedCount = 0;
    shape2->paintedCount = 0;
    container->paintedCount = 0;
    KoShapePaintingContext paintContext;
    container->paint(painter, vc, paintContext);
    QCOMPARE(shape1->paintedCount, 0);
    QCOMPARE(shape2->paintedCount, 0);
    QCOMPARE(container->paintedCount, 1);

    container->setClipped(shape1, false);
    container->setClipped(shape2, true);
    QCOMPARE(container->isClipped(shape1), false);
    QCOMPARE(container->isClipped(shape2), true);

    shape1->paintedCount = 0;
    shape2->paintedCount = 0;
    container->paintedCount = 0;
    manager.paint(painter, vc, false);

    // with this shape not being clipped, the shapeManager will paint the container and this shape
    QCOMPARE(shape1->paintedCount, 1);
    // with this shape being clipped, the container will paint it for us.
    QCOMPARE(shape2->paintedCount, 1);
    QCOMPARE(container->paintedCount, 1);

    delete container;
}

void TestShapePainting::testPaintHiddenShape()
{
    MockShape *shape = new MockShape();
    MockContainer *fourth = new MockContainer();
    MockContainer *thirth = new MockContainer();
    MockContainer *second = new MockContainer();
    MockContainer *top = new MockContainer();

    top->addShape(second);
    second->addShape(thirth);
    thirth->addShape(fourth);
    fourth->addShape(shape);

    second->setVisible(false);

    MockCanvas canvas;
    KoShapeManager manager(&canvas);
    manager.addShape(top);
    QCOMPARE(manager.shapes().count(), 5);

    QImage image(100, 100, QImage::Format_Mono);
    QPainter painter(&image);
    KoViewConverter vc;
    manager.paint(painter, vc, false);

    QCOMPARE(top->paintedCount, 1);
    QCOMPARE(second->paintedCount, 0);
    QCOMPARE(thirth->paintedCount, 0);
    QCOMPARE(fourth->paintedCount, 0);
    QCOMPARE(shape->paintedCount, 0);

    delete top;
}

void TestShapePainting::testPaintOrder()
{
    // the stacking order determines the painting order so things on top
    // get their paint called last.
    // Each shape has a zIndex and within the children a container has
    // it determines the stacking order. Its important to realize that
    // the zIndex is thus local to a container, if you have layer1 and layer2
    // with both various child shapes the stacking order of the layer shapes
    // is most important, then within this the child shape index is used.

    class OrderedMockShape : public MockShape
    {
    public:
        OrderedMockShape(QList<MockShape *> &list)
            : order(list)
        {
        }
        void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext) override
        {
            order.append(this);
            MockShape::paint(painter, converter, paintcontext);
        }
        QList<MockShape *> &order;
    };

    QList<MockShape *> order;

    MockContainer *top = new MockContainer();
    top->setZIndex(2);
    OrderedMockShape *shape1 = new OrderedMockShape(order);
    shape1->setZIndex(5);
    OrderedMockShape *shape2 = new OrderedMockShape(order);
    shape2->setZIndex(0);
    top->addShape(shape1);
    top->addShape(shape2);

    MockContainer *bottom = new MockContainer();
    bottom->setZIndex(1);
    OrderedMockShape *shape3 = new OrderedMockShape(order);
    shape3->setZIndex(-1);
    OrderedMockShape *shape4 = new OrderedMockShape(order);
    shape4->setZIndex(9);
    bottom->addShape(shape3);
    bottom->addShape(shape4);

    MockCanvas canvas;
    KoShapeManager manager(&canvas);
    manager.addShape(top);
    manager.addShape(bottom);
    QCOMPARE(manager.shapes().count(), 6);

    QImage image(100, 100, QImage::Format_Mono);
    QPainter painter(&image);
    KoViewConverter vc;
    manager.paint(painter, vc, false);
    QCOMPARE(top->paintedCount, 1);
    QCOMPARE(bottom->paintedCount, 1);
    QCOMPARE(shape1->paintedCount, 1);
    QCOMPARE(shape2->paintedCount, 1);
    QCOMPARE(shape3->paintedCount, 1);
    QCOMPARE(shape4->paintedCount, 1);

    QCOMPARE(order.count(), 4);
    QVERIFY(order[0] == shape3); // lowest first
    QVERIFY(order[1] == shape4);
    QVERIFY(order[2] == shape2);
    QVERIFY(order[3] == shape1);

    // again, with clipping.
    order.clear();
    painter.setClipRect(0, 0, 100, 100);
    manager.paint(painter, vc, false);
    QCOMPARE(top->paintedCount, 2);
    QCOMPARE(bottom->paintedCount, 2);
    QCOMPARE(shape1->paintedCount, 2);
    QCOMPARE(shape2->paintedCount, 2);
    QCOMPARE(shape3->paintedCount, 2);
    QCOMPARE(shape4->paintedCount, 2);

    QCOMPARE(order.count(), 4);
    QVERIFY(order[0] == shape3); // lowest first
    QVERIFY(order[1] == shape4);
    QVERIFY(order[2] == shape2);
    QVERIFY(order[3] == shape1);

    order.clear();

    MockContainer *root = new MockContainer();
    root->setZIndex(0);

    MockContainer *branch1 = new MockContainer();
    branch1->setZIndex(1);
    OrderedMockShape *child1_1 = new OrderedMockShape(order);
    child1_1->setZIndex(1);
    OrderedMockShape *child1_2 = new OrderedMockShape(order);
    child1_2->setZIndex(2);
    branch1->addShape(child1_1);
    branch1->addShape(child1_2);

    MockContainer *branch2 = new MockContainer();
    branch2->setZIndex(2);
    OrderedMockShape *child2_1 = new OrderedMockShape(order);
    child2_1->setZIndex(1);
    OrderedMockShape *child2_2 = new OrderedMockShape(order);
    child2_2->setZIndex(2);
    branch2->addShape(child2_1);
    branch2->addShape(child2_2);

    root->addShape(branch1);
    root->addShape(branch2);

    QList<KoShape *> sortedShapes;
    sortedShapes.append(root);
    sortedShapes.append(branch1);
    sortedShapes.append(branch2);
    sortedShapes.append(branch1->shapes());
    sortedShapes.append(branch2->shapes());

    std::sort(sortedShapes.begin(), sortedShapes.end(), KoShape::compareShapeZIndex);
    QCOMPARE(sortedShapes.count(), 7);
    QVERIFY(sortedShapes[0] == root);
    QVERIFY(sortedShapes[1] == branch1);
    QVERIFY(sortedShapes[2] == child1_1);
    QVERIFY(sortedShapes[3] == child1_2);
    QVERIFY(sortedShapes[4] == branch2);
    QVERIFY(sortedShapes[5] == child2_1);
    QVERIFY(sortedShapes[6] == child2_2);

    delete top;
    delete bottom;
    delete root;
}

QTEST_MAIN(TestShapePainting)
