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

#ifndef TESTPAPAGEDELETECOMMAND_H
#define TESTPAPAGEDELETECOMMAND_H

#include <QtTest>

class KPrShapeAnimation;
class KPrShapeAnimations;
class MockShape;

class TestShapeAnimations : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void addRemove();
    void replaceSwap();
    void helperMethods();
    void getTriggerEvent();
    void cleanupTestCase();

private:
    void createAnimationTree(KPrShapeAnimations *animations);
    void cleanStepSubStepData();
    QList<KPrShapeAnimation *> m_animation;
    QList<MockShape *> shapes;
};

#endif // TESTPAPAGEDELETECOMMAND_H

