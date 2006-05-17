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
#include <KWFrameView.h>
#include <defs.h> // for mousemeaning

using namespace KUnitTest;

KUNITTEST_MODULE(kunittest_FrameViewTester, "FrameView Tester");
KUNITTEST_MODULE_REGISTER_TESTER(KWFrameViewTester);

void KWFrameViewTester::allTests() {
    testListeners();
    testGetView();
    testMouseMeaning();
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
    void frameMoved(KWFrame *f) {
        lastF=f;
    }
    void frameResized(KWFrame *f) {
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
    // frameSetAdded
    viewManager->slotFrameSetAdded(fs);
    CHECK(listener->fsa, 0);
    CHECK(viewManager->m_fireEventsRequested, true);
    viewManager->doFireEvents();
    CHECK(listener->fsa, 1);
    CHECK(listener->fsr, 0);
    CHECK(listener->fa, 0);
    CHECK(listener->fr, 0);
    CHECK(listener->lastFs, fs);

    // reset
    listener->fsa = 0;
    viewManager->m_fireEventsRequested = false;
    // frameSetRemoved
    viewManager->slotFrameSetRemoved(fs);
    CHECK(viewManager->m_fireEventsRequested, true);
    viewManager->doFireEvents();
    CHECK(listener->fsa, 0);
    CHECK(listener->fsr, 1);
    CHECK(listener->fa, 0);
    CHECK(listener->fr, 0);
    CHECK(listener->lastFs, fs);

    // reset
    listener->fsr = 0;
    viewManager->m_fireEventsRequested = false;
    // frameSetRemoved
    viewManager->slotFrameAdded(f);
    CHECK(viewManager->m_fireEventsRequested, true);
    viewManager->doFireEvents();
    CHECK(listener->fsr, 0);
    CHECK(listener->fsa, 0);
    CHECK(listener->fa, 1);
    CHECK(listener->fr, 0);
    CHECK(listener->lastF, f);

    // reset
    listener->fa = 0;
    viewManager->m_fireEventsRequested = false;
    // frameSetRemoved
    viewManager->slotFrameRemoved(f);
    CHECK(viewManager->m_fireEventsRequested, true);
    viewManager->doFireEvents();
    CHECK(listener->fsr, 0);
    CHECK(listener->fsa, 0);
    CHECK(listener->fa, 0);
    CHECK(listener->fr, 1);
    CHECK(listener->lastF, f);

    // reset
    listener->fr = 0;
    viewManager->m_fireEventsRequested = false;
    // multiple
    KWFrame *f2 = new KWFrame(fs, 0, 1, 2, 3);
    viewManager->slotFrameRemoved(f);
    viewManager->slotFrameSetAdded(fs);
    viewManager->slotFrameAdded(f);
    viewManager->slotFrameAdded(f);
    viewManager->slotFrameAdded(f2);
    viewManager->doFireEvents();
    CHECK(listener->fsr, 0);
    CHECK(listener->fsa, 1);
    CHECK(listener->fa, 3);
    CHECK(listener->fr, 1);
    CHECK(listener->lastF, f2);

    viewManager->slotFrameSetAdded(fs);
    viewManager->removeKWFramesListener(listener);
    viewManager->doFireEvents();
    CHECK(listener->fsr, 0);

    // frame moved
    // frame resized
}

void KWFrameViewTester::testGetView() {
    TestFrameViewManager fvm;
    KWFrameSet *fs = new TestFrameSet();
    KWFrame *frame = new KWFrame(fs, 0, 0, 100, 100);
    fs->addFrame(frame);

    fvm.slotFrameAdded(frame);
    KWFrameView *fv = fvm.view(frame);
    CHECK(fv != 0, true);
    if(!fv) return;
    CHECK(fv->frame(), frame);

    fv = fvm.view( KoPoint(0,0), KWFrameViewManager::frameOnTop);
    CHECK(fv != 0, true); // should exist
    if(!fv) return;
    CHECK(fv->frame(), frame);

    fv = fvm.view( KoPoint(1,1), KWFrameViewManager::frameOnTop);
    CHECK(fv != 0, true);
    if(!fv) return;
    CHECK(fv->frame(), frame);

    fv = fvm.view( KoPoint(110,110), KWFrameViewManager::frameOnTop);
    CHECK(fv == 0, true);

    fv = fvm.view( KoPoint(100,100), KWFrameViewManager::frameOnTop); // note that 100,100 _is_ inside 0,0 100x100
    CHECK(fv != 0, true);
    if(!fv) return;
    CHECK(fv->frame(), frame);

    fv->setSelected(true);
    fv = fvm.view( KoPoint(1,1), KWFrameViewManager::unselected);
    CHECK(fv == 0, true);

    frame->setZOrder(4);
    KWFrame *frame2 = new KWFrame(fs, 80, 50, 60, 60);
    frame2->setZOrder(2);
    fvm.slotFrameAdded(frame2);
    KWFrame *frame3 = new KWFrame(fs, 30, 60, 60, 60);
    frame3->setZOrder(5);
    fvm.slotFrameAdded(frame3);
    KWFrame *frame4 = new KWFrame(fs, 200, 0, 100, 100);
    fvm.slotFrameAdded(frame4);
    frame4->setZOrder(1);

kdDebug() << "frame:" << frame << ", frame2:" << frame2<< ", frame2:" << frame3<< ", frame2:" << frame4 << endl;
    fv = fvm.view( KoPoint(90,60), KWFrameViewManager::frameOnTop);
    CHECK(fv != 0, true);
    if(!fv) return;
    CHECK(fv->frame(), frame3);
    fv = fvm.view( KoPoint(90,60), KWFrameViewManager::frameOnTop); // second time should give the same results
    CHECK(fv != 0, true);
    if(!fv) return;
    CHECK(fv->frame(), frame3);
    fv = fvm.view( KoPoint(90,60), KWFrameViewManager::selected);
    CHECK(fv != 0, true);
    if(!fv) return;
    CHECK(fv->frame(), frame);
    fv = fvm.view( KoPoint(90,60), KWFrameViewManager::selected); // second time should give the same results
    CHECK(fv != 0, true);
    if(!fv) return;
    CHECK(fv->frame(), frame);

    fv = fvm.view( KoPoint(90,60), KWFrameViewManager::nextUnselected);
    CHECK(fv != 0, true);
    if(!fv) return;
    CHECK(fv->frame(), frame2); // the one directly below the selected frame
}

void KWFrameViewTester::testMouseMeaning() {
    TestFrameViewManager fvm;
    KWFrameSet *fs = new TestFrameSet();
    KWFrame *frame = new KWFrame(fs, 50, 50, 50, 50);

    fvm.slotFrameAdded(frame);
    KWFrameView *fv = fvm.view(frame);
    if(fv == 0) return; // means previous test already failed a lot.

    MouseMeaning answers[9 * 3] = { MEANING_TOPLEFT, MEANING_LEFT, MEANING_BOTTOMLEFT,
                     MEANING_TOPLEFT, MEANING_LEFT, MEANING_BOTTOMLEFT,
                     MEANING_TOPLEFT, MEANING_LEFT, MEANING_BOTTOMLEFT,
                    MEANING_TOP, MEANING_MOUSE_INSIDE_TEXT, MEANING_BOTTOM,
                    MEANING_TOP, MEANING_MOUSE_INSIDE_TEXT, MEANING_BOTTOM,
                    MEANING_TOP, MEANING_MOUSE_INSIDE_TEXT, MEANING_BOTTOM,
                    MEANING_TOPRIGHT, MEANING_RIGHT, MEANING_BOTTOMRIGHT,
                    MEANING_TOPRIGHT, MEANING_RIGHT, MEANING_BOTTOMRIGHT,
                    MEANING_TOPRIGHT, MEANING_RIGHT, MEANING_BOTTOMRIGHT };
    int count=0;

    KoPoint point(10, 10);
    CHECK(fv->mouseMeaning(point, 0), MEANING_NONE);
    for(int x = 50; x <= 100; x += 25) {
        for(int offset=-6; offset <= 6; offset+=6) {
            point.setX(x + offset);
            for(int y = 50; y <= 100; y += 25) {
                for(int offset2=-6; offset2 <= 6; offset2+=6) {
                    point.setY(y + offset2);
                    CHECK(fv->contains(point, true), true);
                    if(x < 50 || x > 100 || y < 50 || y > 100)
                        CHECK(fv->contains(point), false);
                    else
                        CHECK(fv->contains(point), true);
                    if(x > 60 && x < 90 && y > 60 && y < 90)
                        CHECK(fv->isBorderHit(point), false);
                    else
                        CHECK(fv->isBorderHit(point), true);
                    CHECK(fv->mouseMeaning(point, 0), answers[count]);
                }
                count++;
            }
        }
    }

    point.setX(50);
    point.setY(120);
    CHECK(fv->mouseMeaning(point, 0), MEANING_NONE);

}
//    QCursor mouseCursor( const KoPoint &point, int keyState ) const;
