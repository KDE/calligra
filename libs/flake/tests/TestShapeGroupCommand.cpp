/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "TestShapeGroupCommand.h"
#include "MockShapes.h"
#include <KoShapeGroup.h>
#include <KoShapeGroupCommand.h>
#include <KoShapeShadow.h>
#include <KoShapeStroke.h>
#include <kundo2command.h>

#include <QTest>

#include <algorithm>

TestShapeGroupCommand::TestShapeGroupCommand()
    : toplevelGroup(nullptr)
    , sublevelGroup(nullptr)
    , strokeGroup(nullptr)
    , cmd1(nullptr)
    , cmd2(nullptr)
    , strokeCmd(nullptr)
    , toplevelShape1(nullptr)
    , toplevelShape2(nullptr)
    , sublevelShape1(nullptr)
    , sublevelShape2(nullptr)
    , extraShape1(nullptr)
    , extraShape2(nullptr)
    , strokeShape1(nullptr)
    , strokeShape2(nullptr)
{
}

TestShapeGroupCommand::~TestShapeGroupCommand() = default;

void TestShapeGroupCommand::init()
{
    toplevelShape1 = new MockShape();
    toplevelShape1->setPosition(QPointF(50, 50));
    toplevelShape1->setSize(QSize(50, 50));

    toplevelShape2 = new MockShape();
    toplevelShape2->setPosition(QPointF(50, 150));
    toplevelShape2->setSize(QSize(50, 50));

    sublevelShape1 = new MockShape();
    sublevelShape1->setPosition(QPointF(150, 150));
    sublevelShape1->setSize(QSize(50, 50));

    sublevelShape2 = new MockShape();
    sublevelShape2->setPosition(QPointF(250, 150));
    sublevelShape2->setSize(QSize(50, 50));

    extraShape1 = new MockShape();
    extraShape1->setPosition(QPointF(150, 50));
    extraShape1->setSize(QSize(50, 50));

    extraShape2 = new MockShape();
    extraShape2->setPosition(QPointF(250, 50));
    extraShape2->setSize(QSize(50, 50));

    toplevelGroup = new KoShapeGroup();
    sublevelGroup = new KoShapeGroup();

    strokeShape1 = new MockShape();
    strokeShape1->setSize(QSizeF(50, 50));
    strokeShape1->setPosition(QPointF(0, 0));

    strokeShape2 = new MockShape();
    strokeShape2->setSize(QSizeF(50, 50));
    strokeShape2->setPosition(QPointF(25, 25));

    strokeGroup = new KoShapeGroup();
    strokeGroup->setStroke(new KoShapeStroke(2.0f));
    strokeGroup->setShadow(new KoShapeShadow());
}

void TestShapeGroupCommand::cleanup()
{
    if (toplevelShape1->parent() == nullptr) {
        delete toplevelShape1;
    }
    toplevelShape1 = nullptr;
    if (toplevelShape2->parent() == nullptr) {
        delete toplevelShape2;
    }
    toplevelShape2 = nullptr;
    if (sublevelShape1->parent() == nullptr) {
        delete sublevelShape1;
    }
    sublevelShape1 = nullptr;
    if (sublevelShape2->parent() == nullptr) {
        delete sublevelShape2;
    }
    sublevelShape2 = nullptr;
    if (extraShape1->parent() == nullptr) {
        delete extraShape1;
    }
    extraShape1 = nullptr;
    if (extraShape2->parent() == nullptr) {
        delete extraShape2;
    }
    extraShape2 = nullptr;
    if (strokeShape1->parent() == nullptr) {
        delete strokeShape1;
    }
    strokeShape1 = nullptr;
    if (strokeShape2->parent() == nullptr) {
        delete strokeShape2;
    }
    strokeShape2 = nullptr;
    if (sublevelGroup->parent() == nullptr) {
        delete sublevelGroup;
    }
    sublevelGroup = nullptr;
    if (strokeGroup->parent() == nullptr) {
        delete strokeGroup;
        strokeGroup = nullptr;
    }
    delete toplevelGroup;
    toplevelGroup = nullptr;
    delete cmd1;
    cmd1 = nullptr;
    delete cmd2;
    cmd2 = nullptr;
    delete strokeCmd;
    strokeCmd = nullptr;
}

void TestShapeGroupCommand::testToplevelGroup()
{
    QList<KoShape *> toplevelShapes;
    toplevelShapes << toplevelShape1 << toplevelShape2;
    cmd1 = KoShapeGroupCommand::createCommand(toplevelGroup, toplevelShapes);

    cmd1->redo();
    QCOMPARE(toplevelShape1->parent(), toplevelGroup);
    QCOMPARE(toplevelShape1->absolutePosition(KoFlake::TopLeftCorner), QPointF(50, 50));
    QCOMPARE(toplevelShape1->position(), QPointF(0, 0));
    QCOMPARE(toplevelShape2->parent(), toplevelGroup);
    QCOMPARE(toplevelShape2->absolutePosition(KoFlake::TopLeftCorner), QPointF(50, 150));
    QCOMPARE(toplevelShape2->position(), QPointF(0, 100));
    QCOMPARE(toplevelGroup->position(), QPointF(50, 50));

    cmd1->undo();
    QVERIFY(toplevelShape1->parent() == nullptr);
    QCOMPARE(toplevelShape1->absolutePosition(KoFlake::TopLeftCorner), QPointF(50, 50));
    QCOMPARE(toplevelShape1->position(), QPointF(50, 50));
    QVERIFY(toplevelShape2->parent() == nullptr);
    QCOMPARE(toplevelShape2->absolutePosition(KoFlake::TopLeftCorner), QPointF(50, 150));
    QCOMPARE(toplevelShape2->position(), QPointF(50, 150));
}

void TestShapeGroupCommand::testSublevelGroup()
{
    QList<KoShape *> toplevelShapes;
    toplevelShapes << toplevelShape1 << toplevelShape2;
    cmd1 = KoShapeGroupCommand::createCommand(toplevelGroup, toplevelShapes);

    QList<KoShape *> sublevelShapes;
    sublevelShapes << sublevelShape1 << sublevelShape2;
    sublevelShape1->setZIndex(1);
    sublevelShape2->setZIndex(2);
    sublevelShape2->setParent(strokeGroup);
    strokeGroup->setZIndex(-1);

    KoShapeGroupCommand::createCommand(sublevelGroup, sublevelShapes, cmd1);
    KoShapeGroupCommand::createCommand(toplevelGroup, QList<KoShape *>() << sublevelGroup, cmd1);

    cmd1->redo();

    QCOMPARE(toplevelShape1->parent(), toplevelGroup);
    QCOMPARE(toplevelShape1->absolutePosition(KoFlake::TopLeftCorner), QPointF(50, 50));
    QCOMPARE(toplevelShape1->position(), QPointF(0, 0));
    QCOMPARE(toplevelShape2->parent(), toplevelGroup);
    QCOMPARE(toplevelShape2->absolutePosition(KoFlake::TopLeftCorner), QPointF(50, 150));
    QCOMPARE(toplevelShape2->position(), QPointF(0, 100));
    QCOMPARE(toplevelGroup->position(), QPointF(50, 50));

    QCOMPARE(sublevelShape1->parent(), sublevelGroup);
    QCOMPARE(sublevelShape1->absolutePosition(KoFlake::TopLeftCorner), QPointF(150, 150));
    QCOMPARE(sublevelShape1->position(), QPointF(0, 0));
    QCOMPARE(sublevelShape2->parent(), sublevelGroup);
    QCOMPARE(sublevelShape2->absolutePosition(KoFlake::TopLeftCorner), QPointF(250, 150));
    QCOMPARE(sublevelShape2->position(), QPointF(100, 0));
    QCOMPARE(sublevelGroup->absolutePosition(KoFlake::TopLeftCorner), QPointF(150, 150));
    QCOMPARE(sublevelGroup->position(), QPointF(100, 100));

    // check that the shapes are added in the correct order
    QList<KoShape *> childOrder(sublevelGroup->shapes());
    std::sort(childOrder.begin(), childOrder.end(), KoShape::compareShapeZIndex);
    QList<KoShape *> expectedOrder;
    expectedOrder << sublevelShape2 << sublevelShape1;
    QCOMPARE(childOrder, expectedOrder);
    // check that the group has the zIndex/parent of its added top shape
    QCOMPARE(toplevelGroup->parent(), static_cast<KoShapeContainer *>(nullptr));
    QCOMPARE(toplevelGroup->zIndex(), 1);
}

void TestShapeGroupCommand::testAddToToplevelGroup()
{
    QList<KoShape *> toplevelShapes;
    toplevelShapes << toplevelShape1 << toplevelShape2;
    cmd1 = KoShapeGroupCommand::createCommand(toplevelGroup, toplevelShapes);
    cmd1->redo();

    cmd2 = KoShapeGroupCommand::createCommand(toplevelGroup, QList<KoShape *>() << extraShape1);
    cmd2->redo();

    QVERIFY(extraShape1->parent() == toplevelGroup);
    QCOMPARE(extraShape1->absolutePosition(KoFlake::TopLeftCorner), QPointF(150, 50));
    QCOMPARE(extraShape1->position(), QPointF(100, 0));
    QCOMPARE(toplevelGroup->position(), QPointF(50, 50));

    cmd2->undo();

    QVERIFY(extraShape1->parent() == nullptr);
    QCOMPARE(extraShape1->absolutePosition(KoFlake::TopLeftCorner), QPointF(150, 50));
    QCOMPARE(extraShape1->position(), QPointF(150, 50));
    QCOMPARE(toplevelGroup->position(), QPointF(50, 50));
}

void TestShapeGroupCommand::testAddToSublevelGroup()
{
    QList<KoShape *> toplevelShapes;
    toplevelShapes << toplevelShape1 << toplevelShape2;
    cmd1 = new KoShapeGroupCommand(toplevelGroup, toplevelShapes);

    QList<KoShape *> sublevelShapes;
    sublevelShapes << sublevelShape1 << sublevelShape2;
    new KoShapeGroupCommand(sublevelGroup, sublevelShapes, cmd1);
    new KoShapeGroupCommand(toplevelGroup, QList<KoShape *>() << sublevelGroup, cmd1);
    cmd1->redo();

    cmd2 = new KoShapeGroupCommand(sublevelGroup, QList<KoShape *>() << extraShape2);
    cmd2->redo();

    QVERIFY(extraShape2->parent() == sublevelGroup);
    QCOMPARE(extraShape2->absolutePosition(KoFlake::TopLeftCorner), QPointF(250, 50));
    QCOMPARE(extraShape2->position(), QPointF(100, 0));
    QCOMPARE(sublevelShape1->absolutePosition(KoFlake::TopLeftCorner), QPointF(150, 150));
    QCOMPARE(sublevelShape1->position(), QPointF(0, 100));
    QCOMPARE(sublevelShape2->absolutePosition(KoFlake::TopLeftCorner), QPointF(250, 150));
    QCOMPARE(sublevelShape2->position(), QPointF(100, 100));
    QCOMPARE(sublevelGroup->absolutePosition(KoFlake::TopLeftCorner), QPointF(150, 50));
    QCOMPARE(sublevelGroup->position(), QPointF(100, 0));

    cmd2->undo();

    QVERIFY(extraShape2->parent() == nullptr);
    QCOMPARE(extraShape2->absolutePosition(KoFlake::TopLeftCorner), QPointF(250, 50));
    QCOMPARE(extraShape2->position(), QPointF(250, 50));
    QCOMPARE(sublevelShape1->absolutePosition(KoFlake::TopLeftCorner), QPointF(150, 150));
    QCOMPARE(sublevelShape1->position(), QPointF(0, 0));
    QCOMPARE(sublevelShape2->absolutePosition(KoFlake::TopLeftCorner), QPointF(250, 150));
    QCOMPARE(sublevelShape2->position(), QPointF(100, 0));
    QCOMPARE(sublevelGroup->absolutePosition(KoFlake::TopLeftCorner), QPointF(150, 150));
    QCOMPARE(sublevelGroup->position(), QPointF(100, 100));
}

void TestShapeGroupCommand::testGroupStrokeShapes()
{
    QList<KoShape *> strokeShapes;
    strokeShapes << strokeShape2 << strokeShape1;
    strokeCmd = new KoShapeGroupCommand(strokeGroup, strokeShapes);
    strokeCmd->redo();
    QCOMPARE(strokeShape1->size(), QSizeF(50, 50));
    QCOMPARE(strokeShape2->size(), QSizeF(50, 50));
}

QTEST_MAIN(TestShapeGroupCommand)
