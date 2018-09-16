/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 * Copyright ( C ) 2012 Paul Mendez <paulestebanms@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "TestShapeAnimations.h"

#include "MockAnimation.h"
#include <KPrShapeAnimations.h>
#include <MockShapes.h>
#include "../animations/KPrShapeAnimation.h"
#include "../animations/KPrAnimationStep.h"
#include "../animations/KPrAnimationSubStep.h"
#include "ModelTest.h"
#include "PAMock.h"
#include "MockShapeAnimation.h"

#include <QTest>

const int ANIMATIONS_COUNT = 9;

void TestShapeAnimations::initTestCase()
{
    //Initialize Animations
    QTextBlockUserData *textBlockUserData = 0;
    for (int i = 0; i < 9; i++) {
        MockShape *shape = new MockShape();
        shape->setSize(QSizeF(100, 100));
        shapes.append(shape);
        MockShapeAnimation *animation = new MockShapeAnimation(shape, textBlockUserData);
        animation->setPresetClass(KPrShapeAnimation::Entrance);
        m_animation.append(animation);
    }
}

void TestShapeAnimations::addRemove()
{
    MockDocument doc;
    KPrShapeAnimations animations(&doc);
    new ModelTest(&animations, this);
    cleanStepSubStepData();
    animations.add(m_animation[1]);
    animations.add(m_animation[2]);
    animations.add(m_animation[3]);
    animations.add(m_animation[4]);
    animations.add(m_animation[6]);

    // Test animationByRowOutPut
    QCOMPARE (animations.animationByRow(0), m_animation[1]);
    QCOMPARE (animations.animationByRow(1), m_animation[2]);
    QCOMPARE (animations.animationByRow(2), m_animation[3]);
    QCOMPARE (animations.animationByRow(3), m_animation[4]);
    QCOMPARE (animations.animationByRow(4), m_animation[6]);

    //Test Order is updated
    animations.remove(m_animation[1]);
    animations.remove(m_animation[3]);

    QCOMPARE (animations.animationByRow(0), m_animation[2]);
    QCOMPARE (animations.animationByRow(1), m_animation[4]);
    QCOMPARE (animations.animationByRow(2), m_animation[6]);

    QVERIFY(animations.rowCount() == 3);

    //Remove all animations
    animations.remove(m_animation[2]);
    animations.remove(m_animation[4]);
    animations.remove(m_animation[6]);

    QVERIFY(animations.rowCount() == 0);
}

void TestShapeAnimations::replaceSwap()
{
    MockDocument doc;
    KPrShapeAnimations animations(&doc);
    new ModelTest(&animations, this);
    cleanStepSubStepData();
    animations.add(m_animation[1]);
    animations.add(m_animation[2]);
    animations.add(m_animation[3]);

    //Test swap animations
    animations.swapAnimations(m_animation[1], m_animation[3]);
    QCOMPARE (animations.animationByRow(0), m_animation[3]);
    QCOMPARE (animations.animationByRow(2), m_animation[1]);
    QVERIFY(animations.rowCount() == 3);

    //Test replace animation
    animations.replaceAnimation(m_animation[2], m_animation[5]);
    QCOMPARE(animations.animationByRow(1), m_animation[5]);
    QCOMPARE(m_animation[2]->step(), m_animation[5]->step());
    QCOMPARE(m_animation[2]->subStep(), m_animation[5]->subStep());
    QVERIFY(animations.rowCount() == 3);
}

void TestShapeAnimations::helperMethods()
{
    MockDocument doc;
    KPrShapeAnimations animations(&doc);
    new ModelTest(&animations, this);
    cleanStepSubStepData();
    animations.add(m_animation[1]);
    animations.add(m_animation[2]);
    animations.add(m_animation[4]);

    //Test shapeByIndex
    QCOMPARE(animations.shapeByIndex(animations.index(0, 0)), m_animation[1]->shape());
    QCOMPARE(animations.shapeByIndex(animations.index(1, 0)), m_animation[2]->shape());
    QCOMPARE(animations.shapeByIndex(animations.index(2, 0)), m_animation[4]->shape());

    //Test indexByShape
    QVERIFY(animations.indexByShape(m_animation[1]->shape()).isValid());
    QCOMPARE(animations.index(0, 0), animations.indexByShape(m_animation[1]->shape()));

    QVERIFY(animations.indexByShape(m_animation[2]->shape()).isValid());
    QCOMPARE(animations.index(1, 0), animations.indexByShape(m_animation[2]->shape()));

    QVERIFY(animations.indexByShape(m_animation[4]->shape()).isValid());
    QCOMPARE(animations.index(2, 0), animations.indexByShape(m_animation[4]->shape()));

    //Test indexByAnimation
    QVERIFY(animations.indexByAnimation(m_animation[1]).isValid());
    QCOMPARE(animations.index(0, 0), animations.indexByAnimation(m_animation[1]));
    QVERIFY(animations.indexByAnimation(m_animation[2]).isValid());
    QCOMPARE(animations.index(1, 0), animations.indexByAnimation(m_animation[2]));
    QVERIFY(animations.indexByAnimation(m_animation[4]).isValid());
    QCOMPARE(animations.index(2, 0), animations.indexByAnimation(m_animation[4]));
}

void TestShapeAnimations::getTriggerEvent()
{
    MockDocument doc;
    KPrShapeAnimations animations(&doc);
    new ModelTest(&animations, this);
    createAnimationTree(&animations);
    // Test Trigger Event
    QCOMPARE(animations.data(animations.index(0, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(1, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(2, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(3, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(4, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(5, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(6, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(7, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(8, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    // Test group
    QCOMPARE(animations.data(animations.index(0, KPrShapeAnimations::Group)).toInt(), 1);
    QCOMPARE(animations.data(animations.index(1, KPrShapeAnimations::Group)).toInt(), 1);
    QCOMPARE(animations.data(animations.index(2, KPrShapeAnimations::Group)).toInt(), 1);
    QCOMPARE(animations.data(animations.index(3, KPrShapeAnimations::Group)).toInt(), 1);
    QCOMPARE(animations.data(animations.index(4, KPrShapeAnimations::Group)).toInt(), 1);
    QCOMPARE(animations.data(animations.index(5, KPrShapeAnimations::Group)).toInt(), 2);
    QCOMPARE(animations.data(animations.index(6, KPrShapeAnimations::Group)).toInt(), 2);
    QCOMPARE(animations.data(animations.index(7, KPrShapeAnimations::Group)).toInt(), 3);
    QCOMPARE(animations.data(animations.index(8, KPrShapeAnimations::Group)).toInt(), 3);

    checkOrder(&animations);
}

void TestShapeAnimations::setTriggerEvent()
{
    MockDocument doc;
    KPrShapeAnimations animations(&doc);
    new ModelTest(&animations, this);
    createAnimationTree(&animations);
    // From On click
    // To After Previous
    animations.setNodeType(m_animation[5], KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(0, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(1, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(2, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(3, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(4, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(5, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(6, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(7, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(8, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    checkOrder(&animations);
    QVERIFY(animations.rowCount() == ANIMATIONS_COUNT);

    // To With Previous
    animations.setNodeType(m_animation[7], KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(0, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(1, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(2, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(3, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(4, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(5, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(6, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(7, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(8, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    checkOrder(&animations);
    QVERIFY(animations.rowCount() == ANIMATIONS_COUNT);

    // From After Previous
    // To On click
    animations.setNodeType(m_animation[3], KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(0, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(1, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(2, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(3, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(4, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(5, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(6, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(7, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(8, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    checkOrder(&animations);
    QVERIFY(animations.rowCount() == ANIMATIONS_COUNT);

    // To With previous
    animations.setNodeType(m_animation[6], KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(0, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(1, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(2, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(3, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(4, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(5, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(6, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(7, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(8, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    checkOrder(&animations);
    QVERIFY(animations.rowCount() == ANIMATIONS_COUNT);

    // From with previous
    // To On click
    animations.setNodeType(m_animation[1], KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(0, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(1, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(2, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(3, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(4, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(5, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(6, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(7, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(8, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    checkOrder(&animations);
    QVERIFY(animations.rowCount() == ANIMATIONS_COUNT);

    // To after previous
    animations.setNodeType(m_animation[6], KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(0, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(1, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(2, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(3, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(4, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(5, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(6, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(7, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(8, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    checkOrder(&animations);
    QVERIFY(animations.rowCount() == ANIMATIONS_COUNT);

    // From On click
    // To After Previous (with children)
    animations.setNodeType(m_animation[3], KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(0, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(1, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(2, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(3, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(4, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(5, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(6, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(7, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(8, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    checkOrder(&animations);
    QVERIFY(animations.rowCount() == ANIMATIONS_COUNT);

    //From On Click
    // To after previous (invald for the first animation)
    animations.setNodeType(m_animation[0], KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(0, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(1, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(2, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(3, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(4, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(5, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(6, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(7, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(8, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    checkOrder(&animations);
    QVERIFY(animations.rowCount() == ANIMATIONS_COUNT);

    //To with previous (invalid for the first item)
    animations.setNodeType(m_animation[0], KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(0, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(1, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::OnClick);
    QCOMPARE(animations.data(animations.index(2, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(3, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(4, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(5, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(6, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::AfterPrevious);
    QCOMPARE(animations.data(animations.index(7, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    QCOMPARE(animations.data(animations.index(8, KPrShapeAnimations::NodeType)).toInt(),
             (int)KPrShapeAnimation::WithPrevious);
    checkOrder(&animations);
    QVERIFY(animations.rowCount() == ANIMATIONS_COUNT);
}

void TestShapeAnimations::timeHelperMethods()
{
    MockDocument doc;
    KPrShapeAnimations animations(&doc);
    new ModelTest(&animations, this);
    createAnimationTree(&animations);

    //Previous animation Begin
    QCOMPARE(animations.animationStart(animations.index(0, 0)), 0);
    QCOMPARE(animations.animationStart(animations.index(1, 0)), 0);
    QCOMPARE(animations.animationStart(animations.index(2, 0)), 0);
    QCOMPARE(animations.animationStart(animations.index(3, 0)), 5000);
    QCOMPARE(animations.animationStart(animations.index(4, 0)), 5000);
    QCOMPARE(animations.animationStart(animations.index(5, 0)), 1000);
    QCOMPARE(animations.animationStart(animations.index(6, 0)), 3000);
    QCOMPARE(animations.animationStart(animations.index(7, 0)), 0);
    QCOMPARE(animations.animationStart(animations.index(8, 0)), 0);

    //Previous animation End
    QCOMPARE(animations.animationEnd(animations.index(0, 0)), 4000);
    QCOMPARE(animations.animationEnd(animations.index(1, 0)), 2000);
    QCOMPARE(animations.animationEnd(animations.index(2, 0)), 5000);
    QCOMPARE(animations.animationEnd(animations.index(3, 0)), 6000);
    QCOMPARE(animations.animationEnd(animations.index(4, 0)), 6000);
    QCOMPARE(animations.animationEnd(animations.index(5, 0)), 3000);
    QCOMPARE(animations.animationEnd(animations.index(6, 0)), 6000);
    QCOMPARE(animations.animationEnd(animations.index(7, 0)), 5000);
    QCOMPARE(animations.animationEnd(animations.index(8, 0)), 6000);
}

void TestShapeAnimations::cleanupTestCase()
{
    qDeleteAll(shapes);
    qDeleteAll(m_animation);
}

/* Tree structure:
  Step 1
  |_ SubStep1                           1   2   3   4   5   6
        |_Anim0         On click        HHHHHHHHHHHHH
        |_Anim1         With Previous   HHHHH
        |_Anim2         With Previous       HHHHHHHHHHHHH
  |_ SubStep2
        |_Anim3         After Previous                  HHHHHH
        |_Anim4         With Previous                   HHHHHH
  Step 2
  |_ SubStep3
        |_Anim5         On click            HHHHHHHHH
  |_ SubStep4
        |_Anim6         After Previous                  HHHHH
  Step 3
  |_ SubStep5
        |_Anim7         On click        HHHHHHHHHHHHHHHHH
        |_Anim8         With Previous               HHHHHHHHHHHH

  */
void TestShapeAnimations::createAnimationTree(KPrShapeAnimations *animations)
{
    KPrAnimationStep *step1 = new KPrAnimationStep();
    KPrAnimationStep *step2 = new KPrAnimationStep();
    KPrAnimationStep *step3 = new KPrAnimationStep();
    KPrAnimationSubStep *subStep1 = new KPrAnimationSubStep();
    KPrAnimationSubStep *subStep2 = new KPrAnimationSubStep();
    KPrAnimationSubStep *subStep3 = new KPrAnimationSubStep();
    KPrAnimationSubStep *subStep4 = new KPrAnimationSubStep();
    KPrAnimationSubStep *subStep5 = new KPrAnimationSubStep();
    step1->addAnimation(subStep1);
    step1->addAnimation(subStep2);
    step2->addAnimation(subStep3);
    step2->addAnimation(subStep4);
    step3->addAnimation(subStep5);
    subStep1->addAnimation(m_animation[0]);
    subStep1->addAnimation(m_animation[1]);
    subStep1->addAnimation(m_animation[2]);
    subStep2->addAnimation(m_animation[3]);
    subStep2->addAnimation(m_animation[4]);
    subStep3->addAnimation(m_animation[5]);
    subStep4->addAnimation(m_animation[6]);
    subStep5->addAnimation(m_animation[7]);
    subStep5->addAnimation(m_animation[8]);
    foreach(MockShapeAnimation *animation, m_animation) {
        animation->setBeginTime(0);
        animation->setGlobalDuration(1);
    }
    //Set times
    m_animation[0]->setGlobalDuration(4000);
    m_animation[1]->setGlobalDuration(2000);
    m_animation[2]->setBeginTime(2000);
    m_animation[2]->setGlobalDuration(3000);
    m_animation[3]->setGlobalDuration(1000);
    m_animation[4]->setGlobalDuration(1000);
    m_animation[5]->setBeginTime(1000);
    m_animation[5]->setGlobalDuration(2000);
    m_animation[6]->setBeginTime(2000);
    m_animation[6]->setGlobalDuration(1000);
    m_animation[7]->setGlobalDuration(5000);
    m_animation[8]->setBeginTime(4000);
    m_animation[8]->setGlobalDuration(2000);

    QList<KPrAnimationStep *> stepsList;
    stepsList.append(step1);
    stepsList.append(step2);
    stepsList.append(step3);
    animations->init(stepsList);
    animations->resyncStepsWithAnimations();

}

void TestShapeAnimations::cleanStepSubStepData()
{
    foreach(MockShapeAnimation *animation, m_animation) {
        animation->setStep(0);
        animation->setSubStep(0);
        animation->setStepIndex(-1);
        animation->setSubStepIndex(-1);
    }
}

void TestShapeAnimations::checkOrder(KPrShapeAnimations *animations)
{
    for (int i = 0; i < m_animation.count(); i++) {
        QCOMPARE (animations->animationByRow(i), m_animation[i]);
    }
}

QTEST_MAIN(TestShapeAnimations)
