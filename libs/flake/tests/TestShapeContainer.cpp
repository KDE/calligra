/*
 *  This file is part of Calligra tests
 *
 *  SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *  SPDX-FileCopyrightText: 2010 Adam Celarek <kdedev@xibo.at>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "TestShapeContainer.h"
#include <MockShapes.h>

#include <KoSelection.h>
#include <KoShape.h>
#include <KoShapeGroup.h>
#include <KoShapeGroupCommand.h>
#include <KoShapeTransformCommand.h>
#include <KoShapeUngroupCommand.h>

#include <QTest>

void TestShapeContainer::testModel()
{
    MockContainerModel *model = new MockContainerModel();
    MockContainer container(model);
    MockShape *shape1 = new MockShape();

    container.addShape(shape1);
    QCOMPARE(model->containerChangedCalled(), 0);
    QCOMPARE(model->childChangedCalled(), 1);
    QCOMPARE(model->proposeMoveCalled(), 0);

    shape1->setPosition(QPointF(300, 300));
    QCOMPARE(model->containerChangedCalled(), 0);
    QCOMPARE(model->childChangedCalled(), 2);
    QCOMPARE(model->proposeMoveCalled(), 0);

    shape1->rotate(10);
    QCOMPARE(model->containerChangedCalled(), 0);
    QCOMPARE(model->childChangedCalled(), 3);
    QCOMPARE(model->proposeMoveCalled(), 0);

    shape1->setAbsolutePosition(shape1->absolutePosition() + QPointF(10., 40.));
    QCOMPARE(model->containerChangedCalled(), 0);
    QCOMPARE(model->childChangedCalled(), 5); // we get a generic Matrix as well as a position change...
    QCOMPARE(model->proposeMoveCalled(), 0);

    shape1->setTransformation(QTransform());
    QCOMPARE(model->containerChangedCalled(), 0);
    QCOMPARE(model->childChangedCalled(), 6);
    QCOMPARE(model->proposeMoveCalled(), 0);

    model->resetCounts();
    container.setPosition(QPointF(30, 30));
    QCOMPARE(model->containerChangedCalled(), 1);
    QCOMPARE(model->childChangedCalled(), 0);
    QCOMPARE(model->proposeMoveCalled(), 0);
}

void TestShapeContainer::testSetParent()
{
    MockContainerModel *model1 = new MockContainerModel();
    MockContainer container1(model1);
    MockContainerModel *model2 = new MockContainerModel();
    MockContainer container2(model2);
    MockShape shape;
    // init test
    QCOMPARE(model1->shapes().count(), 0);
    QCOMPARE(model2->shapes().count(), 0);

    shape.setParent(&container1);
    QCOMPARE(model1->shapes().count(), 1);
    QCOMPARE(model2->shapes().count(), 0);
    QCOMPARE(shape.parent(), &container1);

    shape.setParent(&container2);
    QCOMPARE(model1->shapes().count(), 0);
    QCOMPARE(container1.shapes().count(), 0);
    QCOMPARE(model2->shapes().count(), 1);
    QCOMPARE(container2.shapes().count(), 1);
    QCOMPARE(shape.parent(), &container2);
}

void TestShapeContainer::testSetParent2()
{
    MockContainerModel *model = new MockContainerModel();
    MockContainer container(model);
    MockShape *shape = new MockShape();
    shape->setParent(&container);
    QCOMPARE(model->shapes().count(), 1);

    shape->setParent(nullptr);
    QCOMPARE(model->shapes().count(), 0);
}

void TestShapeContainer::testScaling()
{
    KoShape *shape1 = new MockShape();
    KoShape *shape2 = new MockShape();

    shape1->setSize(QSizeF(10., 10.));
    shape1->setPosition(QPointF(20., 20.));

    shape2->setSize(QSizeF(30., 10.));
    shape2->setPosition(QPointF(10., 40.));

    QList<KoShape *> groupedShapes;
    groupedShapes.append(shape1);
    groupedShapes.append(shape2);

    KoShapeGroup *group = new KoShapeGroup();
    KoShapeGroupCommand *groupCommand = KoShapeGroupCommand::createCommand(group, groupedShapes);
    groupCommand->redo();

    QList<KoShape *> transformShapes;
    transformShapes.append(groupedShapes);

    QTransform matrix;
    matrix.scale(0.5, 0.5);

    QVector<QTransform> oldTransformations;
    QVector<QTransform> newTransformations;
    foreach (const KoShape *shape, transformShapes) {
        QTransform oldTransform = shape->transformation();
        oldTransformations.append(oldTransform);
        QTransform globalTransform = shape->absoluteTransformation(nullptr);
        QTransform localTransform = globalTransform * matrix * globalTransform.inverted();
        newTransformations.append(localTransform * oldTransform);
    }

    QVector<QPointF> oldPositions;
    for (int i = 0; i < transformShapes.size(); i++) {
        oldPositions.append(transformShapes.at(i)->absolutePosition(KoFlake::TopLeftCorner));
    }

    KoShapeTransformCommand *transformCommand;
    transformCommand = new KoShapeTransformCommand(transformShapes, oldTransformations, newTransformations);
    transformCommand->redo();

    for (int i = 0; i < transformShapes.size(); i++) {
        QCOMPARE(transformShapes.at(i)->absolutePosition(KoFlake::TopLeftCorner), oldPositions.at(i) * 0.5);
    }

    transformShapes.takeLast();
    KoShapeUngroupCommand *ungroupCmd = new KoShapeUngroupCommand(group, transformShapes);
    ungroupCmd->redo();

    for (int i = 0; i < transformShapes.size(); i++) {
        QCOMPARE(transformShapes.at(i)->absolutePosition(KoFlake::TopLeftCorner), oldPositions.at(i) * 0.5);
    }
}

void TestShapeContainer::testScaling2()
{
    KoShape *shape1 = new MockShape();
    KoShape *shape2 = new MockShape();

    shape1->setPosition(QPointF(20., 20.));
    shape1->setSize(QSizeF(10., 10.));

    shape2->setPosition(QPointF(10., 40.));
    shape2->setSize(QSizeF(30., 10.));

    QList<KoShape *> groupedShapes;
    groupedShapes.append(shape1);
    groupedShapes.append(shape2);

    KoShapeGroup *group = new KoShapeGroup();
    KoShapeGroupCommand *groupCommand = KoShapeGroupCommand::createCommand(group, groupedShapes);
    groupCommand->redo();

    KoSelection *selection = new KoSelection();
    selection->select(shape1, true);

    QList<KoShape *> transformShapes;
    transformShapes.append(selection->selectedShapes());

    QTransform matrix;
    matrix.scale(0.5, 0.5);

    QVector<QTransform> oldTransformations;
    QVector<QTransform> newTransformations;
    foreach (const KoShape *shape, transformShapes) {
        QTransform oldTransform = shape->transformation();
        oldTransformations.append(oldTransform);
        newTransformations.append(oldTransform * matrix);
    }

    QVector<QPointF> oldPositions;
    for (int i = 0; i < transformShapes.size(); i++) {
        oldPositions.append(transformShapes.at(i)->absolutePosition(KoFlake::TopLeftCorner));
    }

    KoShapeTransformCommand *transformCommand;
    transformCommand = new KoShapeTransformCommand(transformShapes, oldTransformations, newTransformations);
    transformCommand->redo();

    QRectF r1(shape1->absolutePosition(KoFlake::TopLeftCorner), shape1->absolutePosition(KoFlake::BottomRightCorner));
    QRectF r2(shape2->absolutePosition(KoFlake::TopLeftCorner), shape2->absolutePosition(KoFlake::BottomRightCorner));
    QSizeF shapeSize = r1.united(r2).size();

    selection = new KoSelection();
    selection->select(shape1, true);
    QSizeF selecSize = selection->size();

    bool works = false;
    if (qFuzzyCompare(selecSize.width(), shapeSize.width()) && qFuzzyCompare(selecSize.height(), shapeSize.height()))
        works = true;
    QCOMPARE(works, true);
}

QTEST_MAIN(TestShapeContainer)
