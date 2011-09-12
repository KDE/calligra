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

#include <QtTest/QtTest>

class MockAppearAnimation;
class MockDisappearAnimation;

class TestShapeAnimations : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void addRemove();
    void animations();
    void steps();
    void cleanupTestCase();

private:    
    MockAppearAnimation * m_animation1_2;
    MockAppearAnimation * m_animation2_0;
    MockDisappearAnimation * m_animation2_2;
    MockAppearAnimation * m_animation2_4;
    MockDisappearAnimation * m_animation3_3;
};

#endif // TESTPAPAGEDELETECOMMAND_H

