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
#include <KWFrameList.h>
#include <KWDocument.h>
#include <kunittest/runner.h>
#include <kunittest/module.h>
#include <qapplication.h>

using namespace KUnitTest;

KUNITTEST_MODULE(kunittest_FrameTester, "Frame Tester");
KUNITTEST_MODULE_REGISTER_TESTER(KWFrameTester);

void KWFrameTester::allTests() {
    int argc = 0;
    char** argv = 0;
    QApplication app( argc, argv ); // needed for KWDocument, due to KWCommandHistory -> KToolBarPopupAction -> QPopupMenu -> QWidget

    testSimpleFrame();
    testFrameList();
    testDeleteAllCopies();
    testNestedFrames();
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

void KWFrameTester::testFrameList()
{
    KWDocument* doc = new KWDocument();

    KWFrameSet *fs = new TestFrameSet( doc );
    // f2 is above f1, f3 doesn't intersect with either
    KWFrame *f1 = new KWFrame(fs, 0, 1, 10, 11);
    f1->setZOrder( 1 );
    fs->addFrame( f1 );
    KWFrame *f2 = new KWFrame(fs, 1, 5, 10, 12);
    f2->setZOrder( 10 );
    fs->addFrame( f2 );
    KWFrame *f3 = new KWFrame(fs, 100, 101, 10, 12);
    f3->setZOrder( 5 );
    fs->addFrame( f3 );

    CHECK( (int)fs->frameCount(), 3 );

    doc->addFrameSet( fs ); // creates frame lists

    KWFrameList* f1List = f1->frameStack();
    CHECK( f1List != 0, true );
    CHECK( f1List->framesBelow().isEmpty(), true );
    CHECK( f1List->framesOnTop().count(), (size_t)1 );
    CHECK( f1List->framesOnTop().first(), f2 );

    KWFrameList* f2List = f2->frameStack();
    CHECK( f2List->framesBelow().count(), (size_t)1 );
    CHECK( f2List->framesBelow().first(), f1 );
    CHECK( f2List->framesOnTop().isEmpty(), true );

    KWFrameList* f3List = f3->frameStack();
    CHECK( f3List->framesBelow().isEmpty(), true );
    CHECK( f3List->framesOnTop().isEmpty(), true );

    delete doc;
}

void KWFrameTester::testDeleteAllCopies() {
    KWFrameSet *fs = new TestFrameSet();
    KWFrame *f = new KWFrame(fs, 0, 1, 10, 11);
    fs->addFrame(f);

    fs->deleteAllCopies();
    CHECK(fs->frameCount(), (unsigned int) 1);
    CHECK(fs->frame(0), f);

    KWFrame *f2 = f->getCopy();
    f2->setCopy(true);
    fs->addFrame(f2);

    CHECK(fs->frameCount(), (unsigned int) 2);
    CHECK(fs->frame(0), f);
    CHECK(fs->frame(1), f2);

    fs->deleteAllCopies();
    f2=0;
    CHECK(fs->frameCount(), (unsigned int) 1);
    CHECK(fs->frame(0), f);
}

void KWFrameTester::testNestedFrames() {
    KWDocument *doc = new KWDocument();
    CHECK(doc->pageManager() != 0, true);

    KWTextFrameSet *base = new TestTextFrameSet(doc, "base");
    base->setPageManager(doc->pageManager());
    KWFrame *f1 = new KWFrame(base, 0, 1, 10, 11);
    base->addFrame(f1);
    KWTextFrameSet *nested = new TestTextFrameSet(doc, "nested");
    nested->setPageManager(doc->pageManager());
    KWFrame *f2 = new KWFrame(nested, 0, 1, 10, 11);
    nested->addFrame(f2);

    nested->setAnchorFrameset(base);

    KWFrameSet *deepNested = new TestTextFrameSet(doc, "deepNested");
    deepNested->setPageManager(doc->pageManager());
    KWFrame *f3 = new KWFrame(deepNested, 0, 1, 10, 11);
    deepNested->addFrame(f3);

    deepNested->setAnchorFrameset(nested);

    doc->addFrameSet(base, false);
    doc->addFrameSet(nested, false);
    doc->addFrameSet(deepNested, false);

    CHECK(f1->frameStack() != 0, true);
    CHECK(f2->frameStack() != 0, true);
    CHECK(f3->frameStack() != 0, true);

    // since nested and deepNested are inline they are 'inside' the base frame and thuse
    // all above and below lists should be empty.
    CHECK(f1->frameStack()->framesBelow().isEmpty(), true);
    CHECK(f2->frameStack()->framesBelow().isEmpty(), true);
    CHECK(f3->frameStack()->framesBelow().isEmpty(), true);
    CHECK(f1->frameStack()->framesOnTop().isEmpty(), true);
    CHECK(f2->frameStack()->framesOnTop().isEmpty(), true);
    CHECK(f3->frameStack()->framesOnTop().isEmpty(), true);
}
