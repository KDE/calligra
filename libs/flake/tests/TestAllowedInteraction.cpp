/*
 *  This file is part of Calligra tests
 *
 *  Copyright (C) 2018 Dag Andersen <danders@get2net.dk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "TestAllowedInteraction.h"
#include <MockShapes.h>

#include <KoSelection.h>
#include <FlakeDebug.h>

#include <QTest>

void TestAllowedInteraction::testVisible()
{
    MockShape *shape = new MockShape();

    QVERIFY(shape->isVisible());
    shape->setVisible(false);
    QVERIFY(!shape->isVisible());
    shape->setVisible(true);
    QVERIFY(shape->isVisible());

    KoShapeContainer *parent = new MockContainer();
    parent->addShape(shape);
    
    parent->setVisible(false);
    QVERIFY(!shape->isVisible(true));
    parent->setVisible(true);
    QVERIFY(shape->isVisible(true));
}

void TestAllowedInteraction::testSelectable()
{
    MockShape *shape = new MockShape();
    
    QVERIFY(shape->isSelectable());
    shape->setSelectable(false);
    QVERIFY(!shape->isSelectable());
    shape->setSelectable(true);
    QVERIFY(shape->isSelectable());

    // visible does not affect selectable
    shape->setVisible(false);
    QVERIFY(shape->isSelectable());
    shape->setVisible(true);
    QVERIFY(shape->isSelectable());
    
    KoShapeContainer *parent = new MockContainer();
    parent->addShape(shape);

    // parent does not affect selectable
    parent->setSelectable(false);
    QVERIFY(shape->isSelectable());
    parent->setVisible(true);
    QVERIFY(shape->isSelectable());

    parent->setVisible(false);
    QVERIFY(shape->isSelectable());
    parent->setVisible(true);
    QVERIFY(shape->isSelectable());
}

void TestAllowedInteraction::testDeletable()
{
    MockShape *shape = new MockShape();
    
    QVERIFY(shape->isDeletable());
    shape->setDeletable(false);
    QVERIFY(!shape->isDeletable());
    shape->setDeletable(true);
    QVERIFY(shape->isDeletable());
    
    // visible does not affect isDeletable
    shape->setVisible(false);
    QVERIFY(shape->isDeletable());
    shape->setVisible(true);
    QVERIFY(shape->isDeletable());
    
    KoShapeContainer *parent = new MockContainer();
    parent->addShape(shape);
    
    // parent does not affect isDeletable
    parent->setDeletable(false);
    QVERIFY(shape->isDeletable());
    parent->setDeletable(true);
    QVERIFY(shape->isDeletable());
    
    parent->setVisible(false);
    QVERIFY(shape->isDeletable());
    parent->setDeletable(true);
    QVERIFY(shape->isSelectable());
}

void TestAllowedInteraction::testGeometryProtected()
{
    MockShape *shape = new MockShape();
    
    QVERIFY(!shape->isGeometryProtected());
    shape->setGeometryProtected(true);
    QVERIFY(shape->isGeometryProtected());
    shape->setGeometryProtected(false);
    QVERIFY(!shape->isGeometryProtected());

    // visible does not affect isGeometryProtected
    shape->setVisible(false);
    QVERIFY(!shape->isGeometryProtected());
    shape->setVisible(true);
    QVERIFY(!shape->isGeometryProtected());

    KoShapeContainer *parent = new MockContainer();
    parent->addShape(shape);
    
    // parent does not affect isGeometryProtected
    parent->setGeometryProtected(false);
    QVERIFY(!shape->isGeometryProtected());
    parent->setGeometryProtected(true);
    QVERIFY(!shape->isGeometryProtected());
    
    parent->setVisible(false);
    QVERIFY(!shape->isGeometryProtected());
    parent->setVisible(true);
    QVERIFY(!shape->isGeometryProtected());
}

void TestAllowedInteraction::testContentProtected()
{
    MockShape *shape = new MockShape();
    
    QVERIFY(!shape->isContentProtected());
    shape->setContentProtected(true);
    QVERIFY(shape->isContentProtected());
    shape->setContentProtected(false);
    QVERIFY(!shape->isContentProtected());
    
    // visible does not affect isContentProtected
    shape->setVisible(false);
    QVERIFY(!shape->isContentProtected());
    shape->setVisible(true);
    QVERIFY(!shape->isContentProtected());
    
    KoShapeContainer *parent = new MockContainer();
    parent->addShape(shape);
    
    // parent does not affect isContentProtected
    parent->setContentProtected(true);
    QVERIFY(!shape->isContentProtected());
    parent->setContentProtected(false);
    QVERIFY(!shape->isContentProtected());
    
    parent->setVisible(false);
    QVERIFY(!shape->isContentProtected());
    parent->setVisible(true);
    QVERIFY(!shape->isContentProtected());
}

void TestAllowedInteraction::testEditable()
{
    MockShape *shape = new MockShape();
    
    QVERIFY(shape->isSelectable());
    shape->setGeometryProtected(true);
    QVERIFY(!shape->isEditable());
    shape->setGeometryProtected(false);
    QVERIFY(shape->isEditable());

    shape->setVisible(false);
    QVERIFY(!shape->isEditable());
    shape->setVisible(true);
    QVERIFY(shape->isEditable());

    KoShapeContainer *parent = new MockContainer();
    parent->addShape(shape);
    
    parent->setGeometryProtected(true);
    QEXPECT_FAIL("", "Should check parent according to api doc", Continue);
    QVERIFY(!shape->isEditable());
    parent->setGeometryProtected(false);
    QVERIFY(shape->isEditable());
    
    parent->setVisible(false);
    QEXPECT_FAIL("", "Should check parent according to api doc", Continue);
    QVERIFY(!shape->isEditable());
    parent->setVisible(true);
    QVERIFY(shape->isEditable());
}

QTEST_GUILESS_MAIN(TestAllowedInteraction)
