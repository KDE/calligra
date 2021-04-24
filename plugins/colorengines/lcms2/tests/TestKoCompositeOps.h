/*
 *  SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KOCOMPOSITEOPS_H_
#define _KOCOMPOSITEOPS_H_

#include <QObject>

class TestKoCompositeOps : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void testCompositeOver();
    void testCompositeAlphaDarken();
    void testCompositeAdd();
    void testCompositeDivide();
    void testCompositeDodge();
    void testCompositeInversedSubtract();
    void testCompositeMulitply();
    void testCompositeOverlay();
    void testCompositeScreen();
    void testCompositeSubtract();
    void testCompositeCopy();
    void testCompositeCopy2();
};

#endif
