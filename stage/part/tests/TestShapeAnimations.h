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

#ifndef TESTSHAPEANIMATIONS_H
#define TESTSHAPEANIMATIONS_H

#include <QObject>

class MockShapeAnimation;
class KPrShapeAnimations;
class MockShape;

class TestShapeAnimations : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void addRemove();
    void replaceSwap();
    void helperMethods();
    void getTriggerEvent();
    void setTriggerEvent();
    void timeHelperMethods();
    void cleanupTestCase();

private:
    void createAnimationTree(KPrShapeAnimations *animations);
    void cleanStepSubStepData();
    void checkOrder(KPrShapeAnimations *animations);
    QList<MockShapeAnimation *> m_animation;
    QList<MockShape *> shapes;
};

#endif // TESTSHAPEANIMATIONS_H

