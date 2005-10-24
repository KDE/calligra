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
#include <kunittest/runner.h>
#include <kunittest/module.h>
#include <kdebug.h>

#include "FrameViewTester.h"
#include "FrameTester.h"

#include <KWFrame.h>
#include <KWFrameSet.h>
#include <KWFrameViewManager.h>

using namespace KUnitTest;

KUNITTEST_MODULE(kunittest_FrameViewTester, "FrameView Tester");
KUNITTEST_MODULE_REGISTER_TESTER(KWFrameViewTester);

void KWFrameViewTester::allTests() {
    testListeners();
}

class MockListener : public KWFramesListener {
public:
    int fsa, fsr, fa,fr;
    KWFrameSet *lastFs;
    KWFrame *lastF;
    MockListener() {
        fsa=0;
        fsr=0;
        fa=0;
        fr=0;
        lastFs=0;
        lastF=0;
    }
    void frameSetAdded(KWFrameSet *fs) {
        fsa++;
        lastFs=fs;
    }
    void frameSetRemoved(KWFrameSet *fs) {
        fsr++;
        lastFs=fs;
    }
    void frameAdded(KWFrame *f) {
        fa++;
        lastF=f;
    }
    void frameRemoved(KWFrame *f) {
        fr++;
        lastF=f;
    }
};

class TestFrameViewManager : public KWFrameViewManager {
public:
    TestFrameViewManager() {
        m_fireEventsRequested = false;
    }
    bool m_fireEventsRequested;

    void doFireEvents() { fireEvents(); }
protected:
    void requestFireEvents() {
        m_fireEventsRequested = true;
    }
};

void KWFrameViewTester::testListeners() {
    KWFrameSet *fs = new TestFrameSet();
    KWFrame *f = new KWFrame(fs, 0, 1, 10, 11);

    TestFrameViewManager *viewManager = new TestFrameViewManager();
    MockListener *listener = new MockListener();
    viewManager->addKWFramesListener(listener);
    CHECK(listener->fsa, 0);
    viewManager->slotFrameSetAdded(fs);
    CHECK(listener->fsa, 0);
    CHECK(viewManager->m_fireEventsRequested, true);
    viewManager->doFireEvents();
    CHECK(listener->fsa, 1);
}
