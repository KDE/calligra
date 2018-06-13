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

void TestAllowedInteraction::testShape()
{
    MockShape *shape = new MockShape();

    QVERIFY(shape->allowedInteractions().testFlag(KoShape::MoveAllowed));
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::MoveAllowed, false));
    QVERIFY(!shape->allowedInteractions().testFlag(KoShape::MoveAllowed));
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::MoveAllowed, true));
    QVERIFY(shape->allowedInteractions().testFlag(KoShape::MoveAllowed));

    QVERIFY(shape->allowedInteractions().testFlag(KoShape::ResizeAllowed));
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::ResizeAllowed, false));
    QVERIFY(!shape->allowedInteractions().testFlag(KoShape::ResizeAllowed));
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::ResizeAllowed, true));
    QVERIFY(shape->allowedInteractions().testFlag(KoShape::ResizeAllowed));
    
    QVERIFY(shape->allowedInteractions().testFlag(KoShape::ShearingAllowed));
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::ShearingAllowed, false));
    QVERIFY(!shape->allowedInteractions().testFlag(KoShape::ShearingAllowed));
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::ShearingAllowed, true));
    QVERIFY(shape->allowedInteractions().testFlag(KoShape::ShearingAllowed));

    QVERIFY(shape->allowedInteractions().testFlag(KoShape::RotationAllowed));
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::RotationAllowed, false));
    QVERIFY(!shape->allowedInteractions().testFlag(KoShape::RotationAllowed));
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::RotationAllowed, true));
    QVERIFY(shape->allowedInteractions().testFlag(KoShape::RotationAllowed));
    
    QVERIFY(shape->allowedInteractions().testFlag(KoShape::SelectionAllowed));
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::SelectionAllowed, false));
    QVERIFY(!shape->allowedInteractions().testFlag(KoShape::SelectionAllowed));
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::SelectionAllowed, true));
    QVERIFY(shape->allowedInteractions().testFlag(KoShape::SelectionAllowed));

    QVERIFY(shape->allowedInteractions().testFlag(KoShape::ContentChangeAllowed));
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::ContentChangeAllowed, false));
    QVERIFY(!shape->allowedInteractions().testFlag(KoShape::ContentChangeAllowed));
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::ContentChangeAllowed, true));
    QVERIFY(shape->allowedInteractions().testFlag(KoShape::ContentChangeAllowed));

    QVERIFY(shape->allowedInteractions().testFlag(KoShape::DeletionAllowed));
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::DeletionAllowed, false));
    QVERIFY(!shape->allowedInteractions().testFlag(KoShape::DeletionAllowed));
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::DeletionAllowed, true));
    QVERIFY(shape->allowedInteractions().testFlag(KoShape::DeletionAllowed));

    QVERIFY(shape->isSelectable());
    shape->setSelectable(false);
    QVERIFY(!shape->isSelectable());
    shape->setSelectable(true);
    QVERIFY(shape->isSelectable());
    
    QVERIFY(shape->isDeletable());
    shape->setDeletable(false);
    QVERIFY(!shape->isDeletable());
    shape->setDeletable(true);
    QVERIFY(shape->isDeletable());
    
    QVERIFY(!shape->isGeometryProtected());
    shape->setGeometryProtected(true);
    QVERIFY(shape->isGeometryProtected());
    shape->setGeometryProtected(false);
    QVERIFY(!shape->isGeometryProtected());
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::MoveAllowed, false));
    QVERIFY(shape->isGeometryProtected());
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::MoveAllowed, true));
    QVERIFY(!shape->isGeometryProtected());
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::ResizeAllowed, false));
    QVERIFY(shape->isGeometryProtected());
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::ResizeAllowed, true));
    QVERIFY(!shape->isGeometryProtected());
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::MoveAllowed, false));
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::ResizeAllowed, false));
    QVERIFY(shape->isGeometryProtected());
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::MoveAllowed, true));
    shape->setAllowedInteractions(shape->allowedInteractions().setFlag(KoShape::ResizeAllowed, true));
    QVERIFY(!shape->isGeometryProtected());
    
    QVERIFY(!shape->isContentProtected());
    shape->setContentProtected(true);
    QVERIFY(shape->isContentProtected());
    shape->setContentProtected(false);
    QVERIFY(!shape->isContentProtected());

}

void TestAllowedInteraction::testParentAllowedInteractions()
{
    MockShape *shape = new MockShape();
    KoShapeContainer *parent = new MockContainer();
    QVERIFY(shape->allowedInteractions() == parent->allowedInteractions(shape));
    parent->addShape(shape);
    QVERIFY(shape->allowedInteractions() == parent->allowedInteractions(shape));
    parent->setAllowedInteractions(0);
    QVERIFY(shape->allowedInteractions() == parent->allowedInteractions(shape));
    QVERIFY(!(shape->allowedInteractions() & KoShape::MoveAllowed));
    QVERIFY(!(shape->allowedInteractions() & KoShape::ResizeAllowed));
    QVERIFY(!(shape->allowedInteractions() & KoShape::ShearingAllowed));
    QVERIFY(!(shape->allowedInteractions() & KoShape::RotationAllowed));
    QVERIFY(!(shape->allowedInteractions() & KoShape::SelectionAllowed));
    QVERIFY(!(shape->allowedInteractions() & KoShape::ContentChangeAllowed));
    QVERIFY(!(shape->allowedInteractions() & KoShape::DeletionAllowed));

    parent->setAllowedInteractions(KoShape::MoveAllowed);
    QVERIFY(shape->allowedInteractions() & KoShape::MoveAllowed);

    parent->setAllowedInteractions(KoShape::ResizeAllowed);
    QVERIFY(shape->allowedInteractions() & KoShape::ResizeAllowed);

    parent->setAllowedInteractions(KoShape::ShearingAllowed);
    QVERIFY(shape->allowedInteractions() & KoShape::ShearingAllowed);

    parent->setAllowedInteractions(KoShape::RotationAllowed);
    QVERIFY(shape->allowedInteractions() & KoShape::RotationAllowed);

    parent->setAllowedInteractions(KoShape::SelectionAllowed);
    QVERIFY(shape->allowedInteractions() & KoShape::SelectionAllowed);

    parent->setAllowedInteractions(KoShape::ContentChangeAllowed);
    QVERIFY(shape->allowedInteractions() & KoShape::ContentChangeAllowed);

    parent->setAllowedInteractions(KoShape::DeletionAllowed);
    QVERIFY(shape->allowedInteractions() & KoShape::DeletionAllowed);
}

void TestAllowedInteraction::testParentAllowedInteraction()
{
    MockShape *shape = new MockShape();
    KoShapeContainer *parent = new MockContainer();
    QVERIFY(shape->allowedInteractions() == parent->allowedInteractions(shape));
    parent->addShape(shape);
    QVERIFY(shape->allowedInteractions() == parent->allowedInteractions(shape));

    parent->setAllowedInteraction(KoShape::MoveAllowed, true);
    QVERIFY(shape->allowedInteraction(KoShape::MoveAllowed));
    
    parent->setAllowedInteraction(KoShape::ResizeAllowed, true);
    QVERIFY(shape->allowedInteraction(KoShape::ResizeAllowed));
    
    parent->setAllowedInteraction(KoShape::ShearingAllowed, true);
    QVERIFY(shape->allowedInteraction(KoShape::ShearingAllowed));
    
    parent->setAllowedInteraction(KoShape::RotationAllowed, true);
    QVERIFY(shape->allowedInteraction(KoShape::RotationAllowed));
    
    parent->setAllowedInteraction(KoShape::SelectionAllowed, true);
    QVERIFY(shape->allowedInteraction(KoShape::SelectionAllowed));
    
    parent->setAllowedInteraction(KoShape::ContentChangeAllowed, true);
    QVERIFY(shape->allowedInteraction(KoShape::ContentChangeAllowed));
    
    parent->setAllowedInteraction(KoShape::DeletionAllowed, true);
    QVERIFY(shape->allowedInteraction(KoShape::DeletionAllowed));
    
    parent->setAllowedInteraction(KoShape::MoveAllowed, false);
    QVERIFY(!shape->allowedInteraction(KoShape::MoveAllowed));
    
    parent->setAllowedInteraction(KoShape::ResizeAllowed, false);
    QVERIFY(!shape->allowedInteraction(KoShape::ResizeAllowed));
    
    parent->setAllowedInteraction(KoShape::ShearingAllowed, false);
    QVERIFY(!shape->allowedInteraction(KoShape::ShearingAllowed));
    
    parent->setAllowedInteraction(KoShape::RotationAllowed, false);
    QVERIFY(!shape->allowedInteraction(KoShape::RotationAllowed));
    
    parent->setAllowedInteraction(KoShape::SelectionAllowed, false);
    QVERIFY(!shape->allowedInteraction(KoShape::SelectionAllowed));
    
    parent->setAllowedInteraction(KoShape::ContentChangeAllowed, false);
    QVERIFY(!shape->allowedInteraction(KoShape::ContentChangeAllowed));
    
    parent->setAllowedInteraction(KoShape::DeletionAllowed, false);
    QVERIFY(!shape->allowedInteraction(KoShape::DeletionAllowed));
}

void TestAllowedInteraction::testGrandParentAllowedInteractions()
{
    MockShape *shape = new MockShape();
    KoShapeContainer *parent = new MockContainer();
    KoShapeContainer *grandParent = new MockContainer();

    QVERIFY(shape->allowedInteractions() == grandParent->allowedInteractions());
    parent->addShape(shape);
    grandParent->addShape(parent);
    QVERIFY(shape->allowedInteractions() == parent->allowedInteractions(shape));

    grandParent->setAllowedInteractions(0);
    QVERIFY(shape->allowedInteractions() == grandParent->allowedInteractions());
    QVERIFY(!(shape->allowedInteractions() & KoShape::MoveAllowed));
    QVERIFY(!(shape->allowedInteractions() & KoShape::ResizeAllowed));
    QVERIFY(!(shape->allowedInteractions() & KoShape::ShearingAllowed));
    QVERIFY(!(shape->allowedInteractions() & KoShape::RotationAllowed));
    QVERIFY(!(shape->allowedInteractions() & KoShape::SelectionAllowed));
    QVERIFY(!(shape->allowedInteractions() & KoShape::ContentChangeAllowed));
    QVERIFY(!(shape->allowedInteractions() & KoShape::DeletionAllowed));
    
    grandParent->setAllowedInteractions(KoShape::MoveAllowed);
    QVERIFY(shape->allowedInteractions() & KoShape::MoveAllowed);
    
    grandParent->setAllowedInteractions(KoShape::ResizeAllowed);
    QVERIFY(shape->allowedInteractions() & KoShape::ResizeAllowed);
    
    grandParent->setAllowedInteractions(KoShape::ShearingAllowed);
    QVERIFY(shape->allowedInteractions() & KoShape::ShearingAllowed);
    
    grandParent->setAllowedInteractions(KoShape::RotationAllowed);
    QVERIFY(shape->allowedInteractions() & KoShape::RotationAllowed);
    
    grandParent->setAllowedInteractions(KoShape::SelectionAllowed);
    QVERIFY(shape->allowedInteractions() & KoShape::SelectionAllowed);
    
    grandParent->setAllowedInteractions(KoShape::ContentChangeAllowed);
    QVERIFY(shape->allowedInteractions() & KoShape::ContentChangeAllowed);
    
    grandParent->setAllowedInteractions(KoShape::DeletionAllowed);
    QVERIFY(shape->allowedInteractions() & KoShape::DeletionAllowed);
}

void TestAllowedInteraction::testGrandParentAllowedInteraction()
{
    MockShape *shape = new MockShape();
    KoShapeContainer *parent = new MockContainer();
    KoShapeContainer *grandParent = new MockContainer();

    parent->addShape(shape);
    grandParent->addShape(parent);

    QVERIFY(shape->allowedInteractions() == grandParent->allowedInteractions());
    
    grandParent->setAllowedInteraction(KoShape::MoveAllowed, true);
    QVERIFY(shape->allowedInteraction(KoShape::MoveAllowed));
    
    grandParent->setAllowedInteraction(KoShape::ResizeAllowed, true);
    QVERIFY(shape->allowedInteraction(KoShape::ResizeAllowed));
    
    grandParent->setAllowedInteraction(KoShape::ShearingAllowed, true);
    QVERIFY(shape->allowedInteraction(KoShape::ShearingAllowed));
    
    grandParent->setAllowedInteraction(KoShape::RotationAllowed, true);
    QVERIFY(shape->allowedInteraction(KoShape::RotationAllowed));
    
    grandParent->setAllowedInteraction(KoShape::SelectionAllowed, true);
    QVERIFY(shape->allowedInteraction(KoShape::SelectionAllowed));
    
    grandParent->setAllowedInteraction(KoShape::ContentChangeAllowed, true);
    QVERIFY(shape->allowedInteraction(KoShape::ContentChangeAllowed));
    
    grandParent->setAllowedInteraction(KoShape::DeletionAllowed, true);
    QVERIFY(shape->allowedInteraction(KoShape::DeletionAllowed));
    
    grandParent->setAllowedInteraction(KoShape::MoveAllowed, false);
    QVERIFY(!shape->allowedInteraction(KoShape::MoveAllowed));
    
    grandParent->setAllowedInteraction(KoShape::ResizeAllowed, false);
    QVERIFY(!shape->allowedInteraction(KoShape::ResizeAllowed));
    
    grandParent->setAllowedInteraction(KoShape::ShearingAllowed, false);
    QVERIFY(!shape->allowedInteraction(KoShape::ShearingAllowed));
    
    grandParent->setAllowedInteraction(KoShape::RotationAllowed, false);
    QVERIFY(!shape->allowedInteraction(KoShape::RotationAllowed));
    
    grandParent->setAllowedInteraction(KoShape::SelectionAllowed, false);
    QVERIFY(!shape->allowedInteraction(KoShape::SelectionAllowed));
    
    grandParent->setAllowedInteraction(KoShape::ContentChangeAllowed, false);
    QVERIFY(!shape->allowedInteraction(KoShape::ContentChangeAllowed));
    
    grandParent->setAllowedInteraction(KoShape::DeletionAllowed, false);
    QVERIFY(!shape->allowedInteraction(KoShape::DeletionAllowed));
}

QTEST_GUILESS_MAIN(TestAllowedInteraction)
