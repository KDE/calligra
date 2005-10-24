/* This file is part of the KOffice project
 * Copyright (C) 2005 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; version 2.

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
#include "FrameTester.h"
#include <KWFrame.h>
#include <kunittest/runner.h>
#include <kunittest/module.h>

using namespace KUnitTest;

KUNITTEST_MODULE(kunittest_FrameTester, "Frame Tester");
KUNITTEST_MODULE_REGISTER_TESTER(KWFrameTester);

void KWFrameTester::allTests() {
    testSimpleFrame();
}

void KWFrameTester::testSimpleFrame() {
    KWFrameSet *fs = new TestFrameSet();
    KWFrame *f = new KWFrame(fs, 0, 1, 10, 11);

    KWFrame *f2 = f->getCopy();
    CHECK(f2->x(), 0.0);
    CHECK(f2->y(), 1.0);
    CHECK(f2->width(), 10.0);
    CHECK(f2->height(), 11.0);
}
