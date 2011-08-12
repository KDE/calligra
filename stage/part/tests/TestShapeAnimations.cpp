/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
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

void TestShapeAnimations::initTestCase()
{
    m_animation1_2 = new MockAppearAnimation( ( KoShape * )1, 2 );
    m_animation2_0 = new MockAppearAnimation( ( KoShape * )2, 0 );
    m_animation2_2 = new MockDisappearAnimation( ( KoShape * )2, 2 );
    m_animation2_4 = new MockAppearAnimation( ( KoShape * )2, 4 );
    m_animation3_3 = new MockDisappearAnimation( ( KoShape * )3, 3 );
}

void TestShapeAnimations::addRemove()
{
    KPrShapeAnimations animations;
    animations.add( m_animation1_2 );

    QMap<KoShape *, KPrShapeAnimationOld *> step2;
    step2.insert( m_animation1_2->shape(), m_animation1_2 );

    QVERIFY( step2 == animations.animations( 2 ) );

    MockDisappearAnimation animation1_2( ( KoShape * )1, 2 );

    QMap<KoShape *, KPrShapeAnimationOld *> step2replace;
    step2replace.insert( animation1_2.shape(), &animation1_2 );

    animations.add( &animation1_2 );

#if 0
    // this is for printing out the data in case something might be wrong
    QMap<KoShape *, KPrShapeAnimationOld *> animationData = animations.animations( 2 );
    QMap<KoShape *, KPrShapeAnimationOld *>::iterator it( animationData.begin() );
    for (  ; it != animationData.end(); ++it )
    {
        qDebug() << "s" << it.key() << (  it.value() ? (  it.value() )->step() : -1 ) << it.value()->type();
    }

    it = step2replace.begin();
    for (  ; it != step2replace.end(); ++it )
    {
        qDebug() << "s" << it.key() << (  it.value() ? (  it.value() )->step() : -1 ) << it.value()->type();
    }
#endif

    QVERIFY( step2replace == animations.animations( 2 ) );

    animations.remove( &animation1_2 );

    qDebug() << animations.animations( 2 ).size();

    QVERIFY( 0 == animations.animations( 2 ).size() );
}

void TestShapeAnimations::animations()
{
    KPrShapeAnimations animations;
    animations.add( m_animation1_2 );
    animations.add( m_animation2_0 );
    animations.add( m_animation2_2 );
    animations.add( m_animation2_4 );
    animations.add( m_animation3_3 );
    
    QVector<QMap<KoShape *, KPrShapeAnimationOld *> > animationVector;

    QMap<KoShape *, KPrShapeAnimationOld *> step0;
    step0.insert( m_animation1_2->shape(), 0 );
    step0.insert( m_animation2_0->shape(), m_animation2_0 );
    animationVector.push_back( step0 );

    QMap<KoShape *, KPrShapeAnimationOld *> step1;
    step1.insert( m_animation1_2->shape(), 0 );
    animationVector.push_back( step1 );

    QMap<KoShape *, KPrShapeAnimationOld *> step2;
    step2.insert( m_animation1_2->shape(), m_animation1_2 );
    step2.insert( m_animation2_2->shape(), m_animation2_2 );
    animationVector.push_back( step2 );

    QMap<KoShape *, KPrShapeAnimationOld *> step3;
    step3.insert( m_animation2_2->shape(), 0 );
    step3.insert( m_animation3_3->shape(), m_animation3_3 );
    animationVector.push_back( step3 );

    QMap<KoShape *, KPrShapeAnimationOld *> step4;
    step4.insert( m_animation2_4->shape(), m_animation2_4 ); 
    step4.insert( m_animation3_3->shape(), 0 );
    animationVector.push_back( step4 );

    for ( int i = 0; i < animationVector.size(); ++i )
    {
        QMap<KoShape *, KPrShapeAnimationOld *> animationData = animations.animations( i );
        QVERIFY( animationData == animationVector[i] );
    }
}

void TestShapeAnimations::steps()
{
    KPrShapeAnimations animations;

    QList<int> steps1;
    steps1 << 0;
    QVERIFY( steps1 == animations.steps() );

    animations.add( m_animation1_2 );

    QList<int> steps2;
    steps2 << 0 << 2;
    QVERIFY( steps2 == animations.steps() );

    animations.add( m_animation2_0 );

    QList<int> steps3;
    steps3 << 0 << 2;
    QVERIFY( steps3 == animations.steps() );
}

void TestShapeAnimations::cleanupTestCase()
{
    delete m_animation1_2;
    delete m_animation2_0;
    delete m_animation2_2;
    delete m_animation2_4;
    delete m_animation3_3;
}

QTEST_MAIN(TestShapeAnimations)
#include "TestShapeAnimations.moc"

