/*
 * This file is part of KOffice tests
 *
 * Copyright (C) 2005-2010 Thomas Zander <zander@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "TestPageCommands.h"

#include <KWPage.h>
#include <KWDocument.h>
#include <commands/KWPageInsertCommand.h>
#include <commands/KWPageRemoveCommand.h>
#include <commands/KWPageStylePropertiesCommand.h>
#include <commands/KWNewPageStyleCommand.h>
#include <frames/KWTextFrame.h>
#include <frames/KWTextFrameSet.h>
#include <KoTextShapeData.h>

#include <tests/MockShapes.h> // from flake

void TestPageCommands::init()
{
}

void TestPageCommands::documentPages()
{
    KWDocument document;
    QCOMPARE(document.pageCount(), 0);
    KWPage page1 = document.appendPage("pagestyle1");
    QVERIFY(page1.isValid());
    QCOMPARE(page1.pageStyle().name(), QString("Standard")); // doesn't auto-create styles
    QCOMPARE(page1.pageNumber(), 1);
    KWPage page2 = document.appendPage("pagestyle1");
    QVERIFY(page2.isValid());
    QCOMPARE(page2.pageStyle().name(), QString("Standard"));
    QCOMPARE(page2.pageNumber(), 2);
    KWPage page3 = document.appendPage("pagestyle2");
    QCOMPARE(page3.pageStyle().name(), QString("Standard"));
    QCOMPARE(page3.pageNumber(), 3);
    KWPage page4 = document.appendPage("pagestyle2");
    QCOMPARE(page4.pageStyle().name(), QString("Standard"));
    QCOMPARE(page4.pageNumber(), 4);
    KWPage page5 = document.insertPage(-99);
    QVERIFY(page5.isValid());
    QCOMPARE(page5.pageNumber(), 5);
    KWPage page6 = document.insertPage(99);
    QVERIFY(page6.isValid());
    QCOMPARE(page6.pageNumber(), 6);
    KWPage page7 = document.appendPage("pagestyle2");
    QVERIFY(page7.isValid());
    QCOMPARE(page7.pageNumber(), 7);
    KWPage page8 = document.appendPage("pagestyle1");
    QVERIFY(page8.isValid());
    QCOMPARE(page8.pageNumber(), 8);
    KWPage page9 = document.appendPage();
    QCOMPARE(page9.pageNumber(), 9);
    QCOMPARE(document.pageCount(), 9);

    document.removePage(3);
    QCOMPARE(document.pageCount(), 8);
    QVERIFY(! page3.isValid());
    QCOMPARE(document.pageManager()->page(2), page2);
    QCOMPARE(document.pageManager()->page(5), page6);
    document.removePage(3);
    QVERIFY(! page3.isValid());
    QVERIFY(! page4.isValid());
    QCOMPARE(document.pageCount(), 7);
    QCOMPARE(document.pageManager()->page(2), page2);
    QCOMPARE(document.pageManager()->page(4), page6);

    page4 = document.insertPage(2); // kwdocument uses 'after' instead of 'as' that the pageManager uses
    QCOMPARE(page4.pageNumber(), 3);
    page3 = document.insertPage(2);
    QCOMPARE(page3.pageNumber(), 3);
    QCOMPARE(page4.pageNumber(), 4);
    QCOMPARE(document.pageManager()->page(2), page2);
    QCOMPARE(document.pageManager()->page(3), page3);
    QCOMPARE(document.pageManager()->page(4), page4);
    QCOMPARE(document.pageManager()->page(5), page5);

    document.removePage(-16);
    document.removePage(16);
    QCOMPARE(document.pageCount(), 9);

    for (int i = document.pageCount() - 1; i >= 1; --i)
        document.removePage(1);
    QCOMPARE(document.pageCount(), 1);
    QCOMPARE(document.pageManager()->page(1), page9);

    QCOMPARE(document.pageCount(), 1);
    document.removePage(1); //we can't remove the last page
    QCOMPARE(document.pageCount(), 1);
}

void TestPageCommands::testInsertPageCommand() // move of frames
{
    KWDocument document;
    KWPageInsertCommand command1(&document, 0);
    QCOMPARE(document.pageCount(), 0);
    QCOMPARE(document.frameSetCount(), 0);
    command1.redo();
    QCOMPARE(document.pageCount(), 1);
    QCOMPARE(document.frameSetCount(), 0);

    KWFrameSet *fs = new KWFrameSet();
    MockShape *shape = new MockShape();
    KWFrame *frame = new KWFrame(shape, fs);
    Q_UNUSED(frame);
    document.addFrameSet(fs);
    QPointF startPos = shape->position();

    KWPageInsertCommand command2(&document, 0);
    QCOMPARE(document.pageCount(), 1);
    QCOMPARE(document.frameSetCount(), 1);
    command2.redo();
    QCOMPARE(document.pageCount(), 2);
    QCOMPARE(document.frameSetCount(), 1);
    QCOMPARE(fs->frameCount(), 1);
    QCOMPARE(command2.page().pageNumber(), 1);
    QCOMPARE(command1.page().pageNumber(), 2);
    QPointF newPos = shape->position();
    QCOMPARE(newPos, QPointF(0, command2.page().height()) + startPos); // it moved ;)

    KWPageInsertCommand command3(&document, 2);
    command3.redo();
    QCOMPARE(document.pageCount(), 3);
    QCOMPARE(document.frameSetCount(), 1);
    QCOMPARE(fs->frameCount(), 1);
    QCOMPARE(newPos, shape->position()); // it has not moved from page 2

    command3.undo();
    QCOMPARE(document.pageCount(), 2);
    QCOMPARE(document.frameSetCount(), 1);
    QCOMPARE(fs->frameCount(), 1);
    QCOMPARE(newPos, shape->position()); // it has not moved from page 2
    QCOMPARE(command2.page().pageNumber(), 1);
    QCOMPARE(command1.page().pageNumber(), 2);

    command2.undo();
    QCOMPARE(document.pageCount(), 1);
    QCOMPARE(document.frameSetCount(), 1);
    QCOMPARE(fs->frameCount(), 1);
    QCOMPARE(command1.page().pageNumber(), 1);
    QCOMPARE(startPos, shape->position()); // it has been moved back

    command2.redo();
    QCOMPARE(document.pageCount(), 2);
    QCOMPARE(document.frameSetCount(), 1);
    QCOMPARE(fs->frameCount(), 1);
    QCOMPARE(command2.page().pageNumber(), 1);
    QCOMPARE(command1.page().pageNumber(), 2);
    QCOMPARE(QPointF(0, command2.page().height()) + startPos, newPos); // it moved again ;)
}

void TestPageCommands::testInsertPageCommand2() // auto remove of frames
{
    KWDocument document;
    KWFrameSet *fs = new KWFrameSet();
    document.addFrameSet(fs);
    KWTextFrameSet *tfs = new KWTextFrameSet(&document, KWord::MainTextFrameSet);
    document.addFrameSet(tfs);

    KWPageInsertCommand command1(&document, 0);
    command1.redo();

    MockShape *shape1 = new MockShape();
    new KWFrame(shape1, fs);

    MockShape *shape2 = new MockShape();
    shape2->setUserData(new KoTextShapeData());
    new KWTextFrame(shape2, tfs);

    KWPageInsertCommand command2(&document, 1); // append a page
    command2.redo();
    QCOMPARE(document.pageCount(), 2);
    QCOMPARE(document.frameSetCount(), 2);
    QCOMPARE(fs->frameCount(), 1);
    QCOMPARE(tfs->frameCount(), 1);

    // add a new frame for the page we just created.
    MockShape *shape3 = new MockShape();
    QPointF position(30, command2.page().offsetInDocument());
    shape3->setPosition(position);
    new KWTextFrame(shape3, tfs);
    QCOMPARE(tfs->frameCount(), 2);

    command2.undo(); // remove the page again.
    QCOMPARE(document.pageCount(), 1);
    QCOMPARE(document.frameSetCount(), 2);
    QCOMPARE(fs->frameCount(), 1);
    QCOMPARE(tfs->frameCount(), 1); // the text frame is an auto-generated one, so it should be removed.
}

void TestPageCommands::testInsertPageCommand3() // restore all properties
{
    KWDocument document;
    KWPageInsertCommand command1(&document, 0);
    command1.redo();

    KWPage page = command1.page();
    KWPageStyle style = page.pageStyle();
    style.setHasMainTextFrame(false);
    style.setFootnoteDistance(10);
    KoPageLayout layout;
    layout.width = 400;
    layout.height = 300;
    layout.leftMargin = 4;
    layout.rightMargin = 6;
    layout.topMargin = 7;
    layout.bottomMargin = 5;
    style.setPageLayout(layout);
    page.setPageStyle(style);

    KWPageInsertCommand command2(&document, 1); // append one page.
    command2.redo();

    QCOMPARE(command2.page().pageStyle(), style);
    QCOMPARE(command2.page().width(), 400.);

    // undo and redo, remember order is important
    command2.undo();
    command1.undo();
    command1.redo();
    command2.redo();

    QVERIFY(command1.page() != page);
    QCOMPARE(command1.page().pageNumber(), 1);
    KWPageStyle style2 = command1.page().pageStyle();
    QCOMPARE(style2, style);
    QCOMPARE(style2.hasMainTextFrame(), false);
    QCOMPARE(style2.footnoteDistance(), 10.);
    KoPageLayout layout2 = style2.pageLayout();
    QCOMPARE(layout2, layout);

    QCOMPARE(command2.page().pageStyle(), style);
    QCOMPARE(command2.page().width(), 400.);
}

void TestPageCommands::testRemovePageCommand() // move of frames
{
    KWDocument document;
    KWPageInsertCommand insertCommand(&document, 0);
    insertCommand.redo();

    KWFrameSet *fs = new KWFrameSet();
    MockShape *shape = new MockShape();
    KWFrame *frame = new KWFrame(shape, fs);
    Q_UNUSED(frame);
    document.addFrameSet(fs);
    KWPageInsertCommand insertCommand2(&document, 1);
    insertCommand2.redo();
    MockShape *shape2 = new MockShape();
    QPointF pos = QPointF(20, insertCommand2.page().offsetInDocument() + 10);
    shape2->setPosition(pos);
    KWFrame *frame2 = new KWFrame(shape2, fs);
    Q_UNUSED(frame2);
    QCOMPARE(document.pageCount(), 2);
    QCOMPARE(document.frameSetCount(), 1);
    QCOMPARE(fs->frameCount(), 2);

    // remove page2
    KWPageRemoveCommand command1(&document, insertCommand2.page());
    command1.redo();

    QCOMPARE(insertCommand.page().pageNumber(), 1);
    QCOMPARE(insertCommand2.page().isValid(), false);

    QCOMPARE(document.pageCount(), 1);
    QCOMPARE(document.frameSetCount(), 1);
    QCOMPARE(fs->frameCount(), 1);

    QCOMPARE(shape2->position(), pos); // shapes are not deleted, just removed from the document

    command1.undo();
    QCOMPARE(insertCommand.page().pageNumber(), 1);
    QCOMPARE(document.pageCount(), 2);
    QCOMPARE(document.frameSetCount(), 1);
    QCOMPARE(fs->frameCount(), 2);

    QCOMPARE(shape2->position(), pos); // not moved.

    // remove page 1
    KWPageRemoveCommand command2(&document, insertCommand.page());
    command2.redo();

    QCOMPARE(insertCommand.page().isValid(), false);
    QCOMPARE(document.pageCount(), 1);
    QCOMPARE(document.frameSetCount(), 1);
    QCOMPARE(fs->frameCount(), 1);

    QCOMPARE(shape->position(), QPointF(0,0));
    QCOMPARE(shape2->position(), QPointF(20, 10)); // moved!

    command2.undo();

    QCOMPARE(document.pageCount(), 2);
    QCOMPARE(document.frameSetCount(), 1);
    QCOMPARE(fs->frameCount(), 2);
    QCOMPARE(shape->position(), QPointF(0,0));
    QCOMPARE(shape2->position(), pos); // moved back!
}

void TestPageCommands::testRemovePageCommand2() // auto remove of frames
{
    // In contrary to the insert command the remove command will remove frames
    // of all types and reinsert only non-auto-generated ones.
    // lets make sure we it does that.
    KWDocument document;
    KWFrameSet *fs = new KWFrameSet();
    document.addFrameSet(fs);
    KWTextFrameSet *tfs = new KWTextFrameSet(&document, KWord::MainTextFrameSet);
    document.addFrameSet(tfs);

    KWPageInsertCommand insertCommand(&document, 0);
    insertCommand.redo();

    MockShape *shape1 = new MockShape();
    new KWFrame(shape1, fs);

    MockShape *shape2 = new MockShape();
    shape2->setUserData(new KoTextShapeData());
    new KWTextFrame(shape2, tfs);

    KWPageRemoveCommand command(&document, insertCommand.page());
    QCOMPARE(document.frameSetCount(), 2);
    command.redo();

    QCOMPARE(document.frameSetCount(), 1); // only the main frameset is left
    QCOMPARE(document.frameSets().first(), tfs);
    QCOMPARE(fs->frameCount(), 0);
    QCOMPARE(tfs->frameCount(), 0);

    command.undo();

    QCOMPARE(document.frameSetCount(), 2);
    QCOMPARE(fs->frameCount(), 1);
    QCOMPARE(tfs->frameCount(), 0); // doesn't get auto-added
}

void TestPageCommands::testRemovePageCommand3() // test restore all properties
{
    KWDocument document;
    KWPageInsertCommand insertCommand(&document, 0);
    insertCommand.redo();

    KWPage page = insertCommand.page();
    KWPageStyle style = page.pageStyle();
    style.setHasMainTextFrame(false);
    style.setFootnoteDistance(10);
    KoPageLayout layout;
    layout.width = 400;
    layout.height = 300;
    layout.leftMargin = 4;
    layout.rightMargin = 6;
    layout.topMargin = 7;
    layout.bottomMargin = 5;
    style.setPageLayout(layout);
    page.setPageStyle(style);

    KWPageRemoveCommand command(&document, page);
    command.redo();
    QVERIFY(!page.isValid());
    command.undo();
    page = document.pageManager()->begin();
    QVERIFY(page.isValid());

    QVERIFY(insertCommand.page() != page);
    QCOMPARE(page.pageNumber(), 1);
    KWPageStyle style2 = page.pageStyle();
    QCOMPARE(style2, style);
    QCOMPARE(style2.hasMainTextFrame(), false);
    QCOMPARE(style2.footnoteDistance(), 10.);
    KoPageLayout layout2 = style2.pageLayout();
    QCOMPARE(layout2, layout);

    QCOMPARE(page.pageStyle(), style);
    QCOMPARE(page.width(), 400.);
}
    // question; how do I make sure that upon removal I invalidate *all* following pages so their auto-generated frames are re-generated

void TestPageCommands::testRemovePageCommand4() // auto remove of frames
{
    KWDocument document;
    KWPageInsertCommand insertCommand(&document, 0);
    insertCommand.redo();

    KWFrameSet *fs = new KWFrameSet();
    document.addFrameSet(fs);
    MockShape *shape1 = new MockShape();
    new KWFrame(shape1, fs);

    KWTextFrameSet *tfs = new KWTextFrameSet(&document, KWord::MainTextFrameSet);
    document.addFrameSet(tfs);
    MockShape *shape2 = new MockShape();
    shape2->setUserData(new KoTextShapeData());
    new KWTextFrame(shape2, tfs);

    KWTextFrameSet *header = new KWTextFrameSet(&document, KWord::EvenPagesHeaderTextFrameSet);
    document.addFrameSet(header);
    MockShape *shape3 = new MockShape();
    shape3->setUserData(new KoTextShapeData());
    new KWTextFrame(shape3, header);

    KWPageRemoveCommand command(&document, insertCommand.page());
    QCOMPARE(document.frameSetCount(), 3);
    command.redo();

    QCOMPARE(document.frameSetCount(), 2); // only the main&header framesets are left
    QVERIFY(document.frameSets().contains(tfs));
    QVERIFY(document.frameSets().contains(header));
    QCOMPARE(fs->frameCount(), 0);
    QCOMPARE(tfs->frameCount(), 0);
    QCOMPARE(header->frameCount(), 0);

    command.undo();

    QCOMPARE(document.frameSetCount(), 3);
    QCOMPARE(fs->frameCount(), 1);
    QCOMPARE(tfs->frameCount(), 0); // doesn't get auto-added
    QCOMPARE(header->frameCount(), 0); // doesn't get auto-added
}

void TestPageCommands::testPageStylePropertiesCommand() // basic properties change
{
    KWDocument document;
    KWPageManager *manager = document.pageManager();

    KWPageStyle style("pagestyle1");
    KoPageLayout oldLayout;
    oldLayout.format = KoPageFormat::IsoA4Size;
    oldLayout.width = 101;
    oldLayout.height = 102;
    oldLayout.leftMargin = -1;
    oldLayout.rightMargin = -1;
    oldLayout.pageEdge = 7;
    oldLayout.bindingSide = 13;
    style.setPageLayout(oldLayout);

    KoColumns oldColumns;
    oldColumns.columns = 4;
    oldColumns.columnSpacing = 21;
    style.setColumns(oldColumns);
    KWPage page1 = manager->appendPage(style);
    page1.setDirectionHint(KoText::LeftRightTopBottom);

    QCOMPARE(page1.pageNumber(), 1);
    QCOMPARE(page1.width(), 101.);
    QCOMPARE(page1.height(), 102.);
    QCOMPARE(page1.leftMargin(), 13.); // its a right-sided page
    QCOMPARE(page1.pageEdgeMargin(), 7.);
    QCOMPARE(page1.directionHint(), KoText::LeftRightTopBottom);
    QCOMPARE(page1.pageStyle().columns().columns, 4);
    QCOMPARE(page1.pageSide(), KWPage::Right);

    // new ;)
    KWPageStyle style2("pagestyle2");
    KoPageLayout newLayout;
    newLayout.width = 401;
    newLayout.height = 405;
    newLayout.leftMargin = 11;
    newLayout.rightMargin = 18;
    newLayout.pageEdge = -1;
    newLayout.bindingSide = -1;
    style2.setPageLayout(newLayout);
    KoColumns newColumns;
    newColumns.columns = 2;
    newColumns.columnSpacing = 12;
    style2.setColumns(newColumns);
    style2.setDirection(KoText::RightLeftTopBottom);

    KWPageStylePropertiesCommand command1(&document, style, style2);

    // nothing changed before the redo
    QCOMPARE(page1.width(), 101.);
    QCOMPARE(page1.height(), 102.);
    QCOMPARE(page1.leftMargin(), 13.); // its a right-sided page
    QCOMPARE(page1.pageEdgeMargin(), 7.);
    QCOMPARE(page1.directionHint(), KoText::LeftRightTopBottom);

    // redo
    command1.redo();
    QCOMPARE(page1.pageStyle().name(), QString("pagestyle1")); // name didn't change
    QCOMPARE(page1.width(), 401.);
    QCOMPARE(page1.height(), 405.);
    QCOMPARE(page1.leftMargin(), 11.);
    QCOMPARE(page1.pageEdgeMargin(), -1.); // its a right-sided page
    QCOMPARE(page1.pageStyle().direction(), KoText::RightLeftTopBottom);
    QCOMPARE(page1.directionHint(), KoText::LeftRightTopBottom);
    page1.setDirectionHint(KoText::InheritDirection); // reset to what the style says
    QCOMPARE(page1.directionHint(), KoText::RightLeftTopBottom);
    QCOMPARE(style.pageLayout().width, 401.); // style changed
    QCOMPARE(page1.pageStyle().columns().columns, 2);
    QCOMPARE(page1.pageNumber(), 1);
    QCOMPARE(page1.pageSide(), KWPage::Right);

    QCOMPARE(manager->pageCount(), 1);
    QVERIFY(manager->page(1).isValid());
    QVERIFY(!manager->page(2).isValid());

    KWPage page2 = manager->appendPage(style);
    QCOMPARE(manager->pageCount(), 2);
    QVERIFY(manager->page(1).isValid());
    QVERIFY(manager->page(2).isValid());
    QVERIFY(!manager->page(3).isValid());
    QCOMPARE(page2.pageNumber(), 2);
    QCOMPARE(page2.pageSide(), KWPage::Left);

    // undo
    command1.undo();
    QCOMPARE(page1.pageStyle().name(), QString("pagestyle1")); // name didn't change
    QCOMPARE(page1.width(), 101.);
    QCOMPARE(page1.height(), 102.);
    QCOMPARE(page1.leftMargin(), 13.);
    QCOMPARE(page1.directionHint(), KoText::AutoDirection);
    QCOMPARE(style.pageLayout().width, 101.);
    QCOMPARE(page1.pageStyle().columns().columns, 4);
    QCOMPARE(page1.pageNumber(), 1);
    QCOMPARE(page1.pageSide(), KWPage::Right);
    QCOMPARE(manager->pageCount(), 2);
    QVERIFY(manager->page(1).isValid());
    QVERIFY(manager->page(2).isValid());
    QVERIFY(!manager->page(3).isValid());
    QCOMPARE(page2.pageNumber(), 2);
    QCOMPARE(page2.pageSide(), KWPage::Left);
    QCOMPARE(page2.width(), 101.); // same style
    QCOMPARE(page2.height(), 102.);
    QCOMPARE(page2.leftMargin(), 7.);
    QCOMPARE(page2.directionHint(), KoText::AutoDirection);
}

void TestPageCommands::testPageStylePropertiesCommand2()
{
    /*
      make setup where multiple, interspaced pages use the style we want to change.
      add some frames.
      check if the frames moved properly when the page gets a new size
      check that all pages got their new size
      check that the name of the style did not change
    */
    KWDocument document;
    KWPageManager *manager = document.pageManager();

    KWPageStyle style("pagestyle1");
    KoPageLayout lay = style.pageLayout();
    lay.width = 300;
    lay.height = 500;
    style.setPageLayout(lay);

    KWPageStyle style2("pagestyle2");
    lay = style.pageLayout();
    lay.width = 400;
    lay.height = 400;
    style2.setPageLayout(lay);
    KWPage p1 = manager->appendPage(style2);
    KWPage p2 = manager->appendPage(style);
    KWPage p3 = manager->appendPage(style);
    KWPage p4 = manager->appendPage(style2);
    KWPage p5 = manager->appendPage(style2);
    KWPage p6 = manager->appendPage(style);
    KWPage p7 = manager->appendPage(style);
    KWPage p8 = manager->appendPage(style);
    KWPage p9 = manager->appendPage(style2);
    QCOMPARE(manager->pageCount(), 9);

    KWFrameSet *fs = new KWFrameSet();
    for (int i = 1; i <= manager->pageCount(); ++i) {
        // create one frame per page. Positioned relative to the top of the page.
        KWPage page = manager->page(i);
        QVERIFY(page.isValid());
        MockShape *shape = new MockShape();
        new KWFrame(shape, fs);
        shape->setPosition(QPointF(-10, page.offsetInDocument() + 10));
    }
    document.addFrameSet(fs);

    // lets prepare to change the style to a new page size.
    KWPageStyle newStyle("foo");
    lay = newStyle.pageLayout();
    lay.width = 350;
    lay.height = 1000;
    newStyle.setPageLayout(lay);

    KWPageStylePropertiesCommand command(&document, style, newStyle);

    // nothing happens in constructor before redo;
    for (int i = 1; i <= manager->pageCount(); ++i) {
        KWPage page = manager->page(i);
        QVERIFY(page.height() <= 500); // not 1000 yet
        QVERIFY(page.isValid());
        KoShape *shape = fs->frames().at(i-1)->shape();
        QCOMPARE(shape->position(), QPointF(-10, page.offsetInDocument() + 10));
    }

    const qreal lastPageOffset = p9.offsetInDocument();

    command.redo();
    QCOMPARE(manager->pageCount(), 9);
    QCOMPARE(p1.height(), 400.);
    QCOMPARE(p4.height(), 400.);
    QCOMPARE(p5.height(), 400.);
    QCOMPARE(p9.height(), 400.);
    QCOMPARE(p2.height(), 1000.);
    QCOMPARE(p3.height(), 1000.);
    QCOMPARE(p6.height(), 1000.);
    QCOMPARE(p7.height(), 1000.);
    QCOMPARE(p8.height(), 1000.);
    QCOMPARE(lastPageOffset + 2500, p9.offsetInDocument());
    for (int i = 1; i <= manager->pageCount(); ++i) {
        KWPage page = manager->page(i);
        QVERIFY(page.isValid());
        KoShape *shape = fs->frames().at(i-1)->shape();
        QCOMPARE(shape->position(), QPointF(-10, page.offsetInDocument() + 10));
    }
}

void TestPageCommands::testPageSpread()
{
    // setup a doc with multiple pages like;
    //   1: S1, 2: S1, 3: S1, 4: S2, 5: S1, 6:S2, 7: S2

    KWDocument document;
    KWPageManager *manager = document.pageManager();

    KWPageStyle style("pagestyle1");
    KoPageLayout lay = style.pageLayout();
    lay.width = 300;
    lay.height = 500;
    style.setPageLayout(lay);

    KWPageStyle style2("pagestyle2");
    lay = style.pageLayout();
    lay.width = 400;
    lay.height = 400;
    style2.setPageLayout(lay);
    KWPage p1 = manager->appendPage(style);
    KWPage p2 = manager->appendPage(style);
    KWPage p3 = manager->appendPage(style);
    KWPage p4 = manager->appendPage(style2);
    KWPage p5 = manager->appendPage(style);
    KWPage p6 = manager->appendPage(style2);
    KWPage p7 = manager->appendPage(style2);

    KWFrameSet *fs = new KWFrameSet();
    for (int i = 1; i <= manager->pageCount(); ++i) {
        // create one frame per page. Positioned relative to the top of the page.
        KWPage page = manager->page(i);
        QVERIFY(page.isValid());
        MockShape *shape = new MockShape();
        new KWFrame(shape, fs);
        shape->setPosition(QPointF(-10, page.offsetInDocument() + 10));
    }
    document.addFrameSet(fs);

    // when changing S1 from normal to pagespread I expect
    //  page 1 to stay the same.
    //  page 2 to become a pagespread
    //  page 3 to be deleted
    //  page 4 to stay the same.
    //  page 5 to become a page spread and gets pageNumber 6
    //  page 6 to become page 7 and become a Right page
    //  page 7 to become page 8 and become a Left page
    //  there will be a new KWPage for page 5
    // all frames to be moved so they are still at the same relative location
    // to their original page' top

    KWPageStyle style3("dummy");
    lay.leftMargin = -1;
    lay.rightMargin = -1;
    lay.pageEdge = 7;
    lay.bindingSide = 13;
    style2.setPageLayout(lay);
    KWPageStylePropertiesCommand cmd(&document, style, style3);
    cmd.redo();

    QCOMPARE(manager->pageCount(), 7);
QEXPECT_FAIL("", "Not done yet", Abort);
    QCOMPARE(p1.height(), 500.);
    QCOMPARE(p1.width(), 300.);
    QCOMPARE(p1.pageSide(), KWPage::Right);
    QCOMPARE(p1.pageNumber(), 1);
    QCOMPARE(p2.height(), 500.);
    QCOMPARE(p2.width(), 600.);
    QCOMPARE(p2.pageSide(), KWPage::PageSpread);
    QCOMPARE(p2.pageNumber(), 2);
    QVERIFY(!p3.isValid());
    QCOMPARE(p4.pageSide(), KWPage::Left);
    QCOMPARE(p4.width(), 300.);
    QCOMPARE(p4.height(), 500.);
    QCOMPARE(p4.pageNumber(), 4);
    QCOMPARE(p5.width(), 600.);
    QCOMPARE(p5.height(), 500.);
    QCOMPARE(p5.pageSide(), KWPage::PageSpread);
    QCOMPARE(p5.pageNumber(), 6);
    QCOMPARE(p6.pageSide(), KWPage::Right);
    QCOMPARE(p6.width(), 400.);
    QCOMPARE(p6.pageNumber(), 7);
    QCOMPARE(p7.pageSide(), KWPage::Left);
    QCOMPARE(p7.width(), 400.);
    QCOMPARE(p7.pageNumber(), 8);

    KWPage newPage5 = manager->page(5);
    QCOMPARE(newPage5.pageStyle(), style);
    QCOMPARE(newPage5.width(), 400.);
    QCOMPARE(newPage5.height(), 400.);
    QCOMPARE(newPage5.pageSide(), KWPage::Left);

    QCOMPARE(fs->frames()[0]->shape()->position(), QPointF(-10, 10));
    // TODO figure out what the actual numbers should be below
    QCOMPARE(fs->frames()[1]->shape()->position(), QPointF(-10, 10));
    QCOMPARE(fs->frames()[2]->shape()->position(), QPointF(-10, 10));
    QCOMPARE(fs->frames()[3]->shape()->position(), QPointF(-10, 10));
    QCOMPARE(fs->frames()[4]->shape()->position(), QPointF(-10, 10));
    QCOMPARE(fs->frames()[5]->shape()->position(), QPointF(-10, 10));
    QCOMPARE(fs->frames()[6]->shape()->position(), QPointF(-10, 10));
}

void TestPageCommands::testMakePageSpread()
{
    KWDocument document;
    KWPageManager *manager = document.pageManager();

    KWPageStyle style("pagestyle1");
    const KoPageLayout oldLayout = style.pageLayout();
    KoPageLayout layout = style.pageLayout();

    manager->addPageStyle(style);
    KWPage page1 = manager->appendPage(style);
    QCOMPARE(page1.width(), layout.width);
    QCOMPARE(page1.pageNumber(), 1);
    QCOMPARE(page1.pageSide(), KWPage::Right);
    QCOMPARE(manager->pageCount(), 1);

    // make it a pagespread
    KWPageStyle pageSpread = style;
    pageSpread.detach("dummy");
    layout.leftMargin = -1;
    layout.rightMargin = -1;
    layout.pageEdge = 7;
    layout.bindingSide = 13;
    pageSpread.setPageLayout(layout);

    KWPageStylePropertiesCommand cmd1(&document, style, pageSpread);
    cmd1.redo();
    QCOMPARE(page1.pageNumber(), 1);
    QCOMPARE(page1.pageSide(), KWPage::Right);
    QCOMPARE(page1.width(), style.pageLayout().width);
    QCOMPARE(manager->pageCount(), 1);
    KoPageLayout newLayout = style.pageLayout();
    QCOMPARE(newLayout.width, layout.width);
    QCOMPARE(newLayout.leftMargin, layout.leftMargin);
    QCOMPARE(newLayout.rightMargin, layout.rightMargin);
    QCOMPARE(newLayout.pageEdge, layout.pageEdge);
    QCOMPARE(newLayout.bindingSide, layout.bindingSide);

    cmd1.undo();
    QCOMPARE(page1.width(), oldLayout.width);
    QCOMPARE(page1.pageNumber(), 1);
    QCOMPARE(page1.pageSide(), KWPage::Right);
    QCOMPARE(manager->pageCount(), 1);

    // create another page. So we have 2 single sided pages. (Right/Left)
    KWPage page2 = manager->appendPage(style);
    QCOMPARE(page2.width(), oldLayout.width);
    QCOMPARE(page2.pageNumber(), 2);
    QCOMPARE(page2.pageSide(), KWPage::Left);
    QCOMPARE(manager->pageCount(), 2);

    // avoid reusing cmd1 as that assumes the constructor doens't do anything. Which is
    // not a restriction we put on the command. (i.e. that doesn't *have* to work)
    KWPageStylePropertiesCommand cmd2(&document, style, pageSpread);
    cmd2.redo();

    QCOMPARE(page1.width(), style.pageLayout().width);
    QCOMPARE(page1.pageNumber(), 1);
    QCOMPARE(page1.pageSide(), KWPage::Right);
    QCOMPARE(page2.pageNumber(), 2);
QEXPECT_FAIL("", "Not done yet", Abort);
    QCOMPARE(page2.pageSide(), KWPage::PageSpread);
    QCOMPARE(page2.width(), style.pageLayout().width * 2);
    QCOMPARE(manager->pageCount(), 3);

    cmd2.undo();
    // test for page side etc.
    QCOMPARE(page1.width(), oldLayout.width);
    QCOMPARE(page1.pageNumber(), 1);
    QCOMPARE(page1.pageSide(), KWPage::Right);
    QCOMPARE(page2.pageNumber(), 2);
    QCOMPARE(page2.pageSide(), KWPage::Left);
    QCOMPARE(page2.width(), oldLayout.width);
    QCOMPARE(manager->pageCount(), 2);
}

void TestPageCommands::testNewPageStyleCommand()
{
    KWDocument document;
    KWPageManager *manager = document.pageManager();

    KWPageStyle style("pagestyle1");
    KWNewPageStyleCommand cmd(&document, style);

    QCOMPARE(manager->pageStyle("pagestyle1").isValid(), false);
    cmd.redo();
    QVERIFY(manager->pageStyle("pagestyle1").isValid());
    QCOMPARE(manager->pageStyle("pagestyle1"), style);
    cmd.undo();
    QCOMPARE(manager->pageStyle("pagestyle1").isValid(), false);
}

QTEST_KDEMAIN(TestPageCommands, GUI)
#include <TestPageCommands.moc>
