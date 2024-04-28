/*
  SPDX-FileCopyrightText: 2008 Brad Hards <bradh@frogmouth.net>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef PYEMF_TESTS_H
#define PYEMF_TESTS_H

#include <QtTest>

class PyEmfTests : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void test1();
    void testArcChordPie();
    void testDeleteObject();
    void testDrawing1();
    void testFontBackground();
    void testOptimize16Bit();
    void testPaths1();
    void testPoly1();
    void testPoly2();
    void testSetClipPath();
    void testSetPixel();
    void testViewportWindowOrigin();
    void testWorldTransform1();
};

#endif
