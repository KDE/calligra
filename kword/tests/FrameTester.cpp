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
