/* This file is part of the KOffice project
 * Copyright (C) 2005,2008,2010 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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
#include "TestPageManager.h"

#include <KWPageManager.h>
#include <KWPage.h>
#include <KoColorBackground.h>

#include <kdebug.h>

void TestPageManager::init()
{
}

void TestPageManager::getAddPages()
{
    KWPageManager *pageManager = new KWPageManager();

    QCOMPARE(pageManager->pageCount(), 0);
    QVERIFY(! pageManager->page(0).isValid());
    QVERIFY(! pageManager->page(1).isValid());
    QVERIFY(! pageManager->page(-10).isValid());
    QVERIFY(! pageManager->page(10).isValid());

    KWPage page1 = pageManager->appendPage();
    QCOMPARE(page1.pageNumber(), 1);
    KWPage page3 = pageManager->appendPage();
    QCOMPARE(page3.pageNumber(), 2);
    QCOMPARE(pageManager->pageCount(), 2);
    QCOMPARE(pageManager->page(1), page1);
    QCOMPARE(pageManager->page(2), page3);

    KWPage page2 = pageManager->insertPage(2);
    QCOMPARE(page2.pageNumber(), 2);
    QCOMPARE(pageManager->pageCount(), 3);
    QCOMPARE(pageManager->page(1), page1);
    QCOMPARE(pageManager->page(2), page2);
    QCOMPARE(pageManager->page(3), page3);

    KWPage page4 = pageManager->insertPage(4);
    QCOMPARE(pageManager->pageCount(), 4);
    QCOMPARE(pageManager->page(4), page4);

    QCOMPARE(page1.pageNumber(), 1);
    QCOMPARE(page2.pageNumber(), 2);
    QCOMPARE(page3.pageNumber(), 3);
    QCOMPARE(page4.pageNumber(), 4);

    // fetching
    KWPage page = pageManager->page(1);
    QVERIFY(page.isValid());
    QCOMPARE(page.pageNumber(), 1);
    QCOMPARE(page.pageSide(), KWPage::Right);
    QVERIFY(page.pageStyle().isValid());
    KoPageLayout pageLayout = page.pageStyle().pageLayout();
    pageLayout.width = 134.2;
    pageLayout.height = 521.4;
    page.pageStyle().setPageLayout(pageLayout);
    QCOMPARE(page.width(), 134.2);
    QCOMPARE(page.height(), 521.4);

#if 0       // TODO, missing feature :(
    // test setStartPage
    pageManager->setStartPage(4);
    page = pageManager->page(0);
    QCOMPARE(page == 0, true);
    page = pageManager->page(3);
    QCOMPARE(page == 0, true);
    page = pageManager->page(5);
    QCOMPARE(page == 0, true);
    page = pageManager->page(4);
    QCOMPARE(page == 0, false);
    QCOMPARE(page->pageNumber(), 4);
    QCOMPARE(page->pageSide(), KWPage::Left);

    pageManager->setStartPage(1);
    page = pageManager->page(0);
    QCOMPARE(page == 0, true);
    page = pageManager->page(3);
    QCOMPARE(page == 0, true);
    page = pageManager->page(2);
    QCOMPARE(page == 0, true);
    page = pageManager->page(1);
    QCOMPARE(page == 0, false);
    QCOMPARE(page->pageNumber(), 1);
    QCOMPARE(page->pageSide(), KWPage::Right);

    // adding pages
    QCOMPARE(pageManager->pageCount(), 1);
    QCOMPARE(pageManager->lastPageNumber(), 1);
    pageManager->setStartPage(40);
    QCOMPARE(pageManager->pageCount(), 1);
    QCOMPARE(pageManager->lastPageNumber(), 40);
    page = pageManager->appendPage();
    QCOMPARE(pageManager->pageCount(), 2);
    QCOMPARE(pageManager->lastPageNumber(), 41);

    QCOMPARE(page == 0, false);
    QCOMPARE(page->pageNumber(), 41);
    QCOMPARE(page->pageSide(), KWPage::Right);
#endif
}

void TestPageManager::getAddPages2()
{
    KWPageManager *pageManager = new KWPageManager();

    KWPage page = pageManager->appendPage();
    KoPageLayout pageLayout = page.pageStyle().pageLayout();
    pageLayout.width = 200;
    pageLayout.height = 200;
    page.pageStyle().setPageLayout(pageLayout);

    MockShape shape1;
    shape1.setPosition(QPointF(0, 0));
    shape1.setSize(QSizeF(2, 2));
    QCOMPARE(pageManager->pageNumber(&shape1), 1);

    MockShape shape2;
    shape2.setPosition(QPointF(90, 90));
    shape2.setSize(QSizeF(2, 2));
    QCOMPARE(pageManager->pageNumber(&shape2), 1);

    MockShape shape3;
    shape3.setPosition(QPointF(190, 190));
    shape3.setSize(QSizeF(9, 9));
    QCOMPARE(pageManager->pageNumber(&shape3), 1);

    KWPageStyle settingsPage2("page2");
    pageLayout = settingsPage2.pageLayout();
    pageLayout.width = 600;
    pageLayout.height = 600;
    settingsPage2.setPageLayout(pageLayout);
    page = pageManager->appendPage(settingsPage2);
    QCOMPARE(pageManager->pageNumber(&shape1), 1);
    QCOMPARE(pageManager->pageNumber(&shape2), 1);
    QCOMPARE(pageManager->pageNumber(&shape3), 1);

    shape1.setPosition(QPointF(201, 201));
    QCOMPARE(pageManager->pageNumber(&shape1), 2);
    shape1.setPosition(QPointF(300, 3));
    QCOMPARE(pageManager->pageNumber(&shape1), 1); // right of page 1
    shape3.setPosition(QPointF(2, 690));
    QCOMPARE(pageManager->pageNumber(&shape3), 2);
    shape3.setPosition(QPointF(300, 300));
    QCOMPARE(pageManager->pageNumber(&shape3), 2);
    shape3.setPosition(QPointF(600, 700));
    QCOMPARE(pageManager->pageNumber(&shape3), 2);

    // QPointF based
    QCOMPARE(pageManager->pageNumber(QPointF(201, 201)), 2);
    // Y based
    QCOMPARE(pageManager->pageNumber(201.0), 2);
    QCOMPARE(pageManager->pageNumber(900.0), 2);
}

void TestPageManager::createInsertPages()
{
    KWPageManager *pageManager = new KWPageManager();

    QCOMPARE(pageManager->pageCount(), 0);
    KWPage page1 = pageManager->appendPage();
    QCOMPARE(pageManager->pageCount(), 1);
    KWPage page3 = pageManager->appendPage();
    QCOMPARE(pageManager->pageCount(), 2);
    QCOMPARE(page3.pageNumber(), 2);

    KWPage page2 = pageManager->insertPage(2);
    QCOMPARE(pageManager->pageCount(), 3);
    QCOMPARE(page1.pageNumber(), 1);
    QCOMPARE(page2.pageNumber(), 2);
    QCOMPARE(page3.pageNumber(), 3);

    KWPage page4 = pageManager->insertPage(-100); // invalid numbers go at the end
    KWPage page5 = pageManager->insertPage(100);
    QCOMPARE(pageManager->pageCount(), 5);
    QCOMPARE(page1.pageNumber(), 1);
    QCOMPARE(page2.pageNumber(), 2);
    QCOMPARE(page3.pageNumber(), 3);
    QCOMPARE(page4.pageNumber(), 4);
    QCOMPARE(page5.pageNumber(), 5);

    KWPage page6 = pageManager->insertPage(1);
    KWPage page7 = pageManager->insertPage(2);
    QCOMPARE(pageManager->pageCount(), 7);
    QCOMPARE(page6.pageNumber(), 1);
    QCOMPARE(page7.pageNumber(), 2);
    QCOMPARE(page1.pageNumber(), 3);
    QCOMPARE(page2.pageNumber(), 4);
    QCOMPARE(page3.pageNumber(), 5);
    QCOMPARE(page4.pageNumber(), 6);
}

void TestPageManager::removePages()
{
    KWPageManager *pageManager = new KWPageManager();

    KWPage page1 = pageManager->appendPage();
    KWPage page2 = pageManager->appendPage();
    pageManager->appendPage();
    KWPage page4 = pageManager->appendPage();

    pageManager->removePage(3);
    QCOMPARE(page1.pageNumber(), 1);
    QCOMPARE(page2.pageNumber(), 2);
    QCOMPARE(page4.pageNumber(), 3);

    pageManager->removePage(page2);
    QCOMPARE(page1.pageNumber(), 1);
    QCOMPARE(page4.pageNumber(), 2);
}

void TestPageManager::pageInfo()
{
    KWPageManager *pageManager = new KWPageManager();

    KoPageLayout layout = pageManager->defaultPageStyle().pageLayout();
    layout.width = 100;
    layout.height = 200;
    layout.format = KoPageFormat::IsoA4Size;
    pageManager->defaultPageStyle().setPageLayout(layout);
    QCOMPARE(pageManager->defaultPageStyle().pageLayout().width, 100.0);
    QCOMPARE(pageManager->defaultPageStyle().pageLayout().format, KoPageFormat::IsoA4Size);

    KWPageStyle pageStylePage2("Page 2");
    layout = pageStylePage2.pageLayout();
    layout.width = 50;
    layout.height = 100;
    pageStylePage2.setPageLayout(layout);
    pageManager->addPageStyle(pageStylePage2);
    QCOMPARE(pageManager->pageStyle("Page 2").pageLayout().width, 50.0);

    KWPageStyle pageStylePage3("Page 3");
    layout = pageStylePage3.pageLayout();
    layout.width = 300;
    layout.height = 600;
    pageStylePage3.setPageLayout(layout);
    pageManager->addPageStyle(pageStylePage3);
    QCOMPARE(pageManager->pageStyle("Page 3").pageLayout().width, 300.0);

    KWPage page1r = pageManager->appendPage();
    KWPage page1l = pageManager->appendPage();
    KWPage page2 = pageManager->appendPage(pageStylePage2);
    KWPage page3 = pageManager->appendPage(pageStylePage3);
    QVERIFY(page3.isValid());

    QCOMPARE(pageManager->pageCount(), 4);
    QCOMPARE(pageManager->page(1), page1r);
    QCOMPARE(pageManager->page(2), page1l);
    QCOMPARE(pageManager->page(2).pageStyle(), pageManager->defaultPageStyle());
    QCOMPARE(pageManager->page(3).pageStyle(), pageStylePage2);
    QCOMPARE(pageManager->page(4).pageStyle(), pageStylePage3);

    QCOMPARE(pageManager->topOfPage(4), 500.0);
    QCOMPARE(pageManager->bottomOfPage(4), 1100.0);

    layout = pageStylePage3.pageLayout();
    layout.height = 500;
    pageStylePage3.setPageLayout(layout);
    QCOMPARE(pageManager->bottomOfPage(4), 1000.0);

    layout = pageManager->defaultPageStyle().pageLayout();
    layout.topMargin = 5;
    layout.leftMargin = 6;
    layout.bottomMargin = 7;
    layout.rightMargin = 8;
    pageManager->defaultPageStyle().setPageLayout(layout);

    layout = pageStylePage2.pageLayout();
    layout.topMargin = 9;
    layout.leftMargin = 10;
    layout.bottomMargin = 11;
    layout.rightMargin = 12;
    pageStylePage2.setPageLayout(layout);

    layout = page1l.pageStyle().pageLayout(); //layout is valid for page1l and page1r
    layout.rightMargin = 14.0;
    page1l.pageStyle().setPageLayout(layout);
    QCOMPARE(page1l.rightMargin(), 14.0);
    QCOMPARE(page1r.rightMargin(), 14.0);

    // Page Edge / Page Margin
    layout = pageManager->defaultPageStyle().pageLayout();
    layout.pageEdge = 14.0;
    pageManager->defaultPageStyle().setPageLayout(layout);
    QCOMPARE(page1l.pageSide(), KWPage::Left);
    QCOMPARE(page1r.pageSide(), KWPage::Right);
    QCOMPARE(page1l.rightMargin(), 14.0);
    QCOMPARE(page1l.leftMargin(), 14.0);

    layout.bindingSide = 15.0;
    pageManager->defaultPageStyle().setPageLayout(layout);
    QCOMPARE(page1l.leftMargin(), 14.0);
    QCOMPARE(page1l.rightMargin(), 15.0);
    QCOMPARE(page1r.leftMargin(), 15.0);
    QCOMPARE(page1r.rightMargin(), 14.0);

    QCOMPARE(page2.rightMargin(), 12.0); // unchanged due to changes in page1
    QCOMPARE(page2.leftMargin(), 10.0);

    layout = pageStylePage2.pageLayout();
    layout.pageEdge = 16.0;
    pageStylePage2.setPageLayout(layout);
    QCOMPARE(page2.pageStyle(), pageStylePage2);
    QCOMPARE(page2.pageStyle().pageLayout().width, 50.0);
    QCOMPARE(page2.pageStyle().pageLayout().height, 100.0);
    QCOMPARE(page2.pageSide(), KWPage::Right);
    QCOMPARE(page2.rightMargin(), 16.0);

    layout.bindingSide = 17.0;
    pageStylePage2.setPageLayout(layout);
    QCOMPARE(page2.rightMargin(), 16.0);
    QCOMPARE(page2.leftMargin(), 17.0);

    layout.leftMargin = 18;
    layout.rightMargin = 19;
    pageStylePage2.setPageLayout(layout);
    QCOMPARE(page2.rightMargin(), 16.0);
    QCOMPARE(page2.leftMargin(), 17.0);
}

void TestPageManager::testClipToDocument()
{
    KWPageManager *pageManager = new KWPageManager();
    KoPageLayout lay;
    lay.width = 300;
    lay.height = 600;
    lay.format = KoPageFormat::IsoA4Size;
    pageManager->defaultPageStyle().setPageLayout(lay);

    KWPageStyle pageStyle1("page1");
    lay = pageStyle1.pageLayout();
    lay.width = 100;
    lay.height = 200;
    pageStyle1.setPageLayout(lay);
    pageManager->addPageStyle(pageStyle1);

    KWPageStyle pageStyle2("page2");
    lay = pageStyle2.pageLayout();
    lay.width = 50;
    lay.height = 100;
    pageStyle2.setPageLayout(lay);
    pageManager->addPageStyle(pageStyle2);

    KWPage page1 = pageManager->appendPage(pageStyle1);
    QVERIFY(page1.isValid());
    KWPage page2 = pageManager->appendPage(pageStyle2);
    QVERIFY(page2.isValid());
    pageManager->appendPage(pageManager->defaultPageStyle());

    QPointF p(10, 10);

    QPointF result = pageManager->clipToDocument(p);
    QCOMPARE(result, p);

    p.setX(110);
    result = pageManager->clipToDocument(p);
    QCOMPARE(p.y(), result.y());
    QCOMPARE(result.x(), 100.0);

    p.setY(210);
    result = pageManager->clipToDocument(p);
    QCOMPARE(result.x(), 50.0);
    QCOMPARE(result.y(), p.y());

    p.setY(330);
    result = pageManager->clipToDocument(p);
    QCOMPARE(p == result, true);

    p.setY(910);
    p.setX(310);
    result = pageManager->clipToDocument(p);
    QCOMPARE(result.y(), 900.0);
    QCOMPARE(result.x(), 300.0);
}

void TestPageManager::testOrientationHint()
{
    KWPageManager pageManager;
    KWPage page = pageManager.appendPage();
    QCOMPARE(page.orientationHint(), KoPageFormat::Portrait);
    page.setOrientationHint(KoPageFormat::Landscape);
    QCOMPARE(page.orientationHint(), KoPageFormat::Landscape);
    page.setOrientationHint(KoPageFormat::Portrait);
    QCOMPARE(page.orientationHint(), KoPageFormat::Portrait);

    page.setOrientationHint(KoPageFormat::Landscape);
    KWPage page2 = pageManager.appendPage();
    QCOMPARE(page2.orientationHint(), KoPageFormat::Landscape); // inherit from last page
    page.setOrientationHint(KoPageFormat::Portrait);
    QCOMPARE(page2.orientationHint(), KoPageFormat::Landscape); // but separate
    QCOMPARE(page.orientationHint(), KoPageFormat::Portrait);
}

void TestPageManager::testDirectionHint()
{
    KWPageManager pageManager;
    KWPage page = pageManager.appendPage();
    QCOMPARE(page.directionHint(), KoText::AutoDirection);
    page.setDirectionHint(KoText::LeftRightTopBottom);
    QCOMPARE(page.directionHint(), KoText::LeftRightTopBottom);
    page.setDirectionHint(KoText::TopBottomRightLeft);
    QCOMPARE(page.directionHint(), KoText::TopBottomRightLeft);

    KWPage page2 = pageManager.appendPage();
    QCOMPARE(page2.directionHint(), KoText::TopBottomRightLeft); // inherit from last page
    page.setDirectionHint(KoText::LeftRightTopBottom);
    QCOMPARE(page2.directionHint(), KoText::TopBottomRightLeft); // but separate
    QCOMPARE(page.directionHint(), KoText::LeftRightTopBottom);
}

void TestPageManager::testPageNumber()
{
    KWPageManager pageManager;
    KWPage page = pageManager.appendPage();
    QCOMPARE(page.pageNumber(), 1);
    page.setPageNumber(1);
    QCOMPARE(page.pageNumber(), 1);
    QCOMPARE(page.pageSide(), KWPage::Right);
    page.setPageNumber(5);
    QCOMPARE(page.pageNumber(), 5);
    QCOMPARE(page.pageSide(), KWPage::Right);
    page.setPageNumber(2);
    QCOMPARE(page.pageNumber(), 2);
    QCOMPARE(page.pageSide(), KWPage::Left);
    page.setPageSide(KWPage::PageSpread);
    QCOMPARE(page.pageSide(), KWPage::PageSpread);
    QVERIFY(pageManager.page(2) == page);
    QVERIFY(pageManager.page(3) == page);
    KWPage page2 = pageManager.appendPage();
    QCOMPARE(page2.pageNumber(), 4);

    KWPage page3 = pageManager.appendPage();
    QCOMPARE(page3.pageNumber(), 5);

    page.setPageNumber(10); // should renumber stuff
    QCOMPARE(page.pageNumber(), 10);
    QCOMPARE(page.pageSide(), KWPage::PageSpread);
    QCOMPARE(page2.pageNumber(), 12);
    QCOMPARE(page3.pageNumber(), 13);

    page2.setPageNumber(20);
    QCOMPARE(page.pageNumber(), 10);
    QCOMPARE(page.pageSide(), KWPage::PageSpread);
    QCOMPARE(page2.pageNumber(), 20);
    QCOMPARE(page3.pageNumber(), 21);
}

void TestPageManager::testPageTraversal()
{
    KWPageManager manager;
    for (int i = 1; i < 6; ++i)
        manager.appendPage();
    KWPage page = manager.begin();
    QCOMPARE(page.pageNumber(), 1);
    page = page.next();
    QCOMPARE(page.pageNumber(), 2);
    page = page.next();
    QCOMPARE(page.pageNumber(), 3);
    page = page.next();
    QCOMPARE(page.pageNumber(), 4);
    page = page.next();
    QCOMPARE(page.pageNumber(), 5);
    QCOMPARE(page.isValid(), true);
    page = page.next();
    QCOMPARE(page.isValid(), false);
    page = manager.last();
    QCOMPARE(page.pageNumber(), 5);
    QCOMPARE(page.isValid(), true);
    page = page.previous();
    QCOMPARE(page.pageNumber(), 4);
    page = page.previous();
    QCOMPARE(page.pageNumber(), 3);
    page = page.previous();
    QCOMPARE(page.pageNumber(), 2);
    page = page.previous();
    QCOMPARE(page.pageNumber(), 1);
    QCOMPARE(page.isValid(), true);
    page = page.previous();
    QCOMPARE(page.isValid(), false);
}

void TestPageManager::testSetPageStyle()
{
    KWPageManager manager;
    KWPage page = manager.appendPage();

    KWPageStyle style("myStyle");
    KoPageLayout layout;
    layout.height = 100;
    style.setPageLayout(layout);
    page.setPageStyle(style);

    QCOMPARE(page.height(), 100.);
    QVERIFY(manager.pageStyle("myStyle") == style);
}

void TestPageManager::testPageCount()
{
    KWPageManager manager;
    QCOMPARE(manager.pageCount(), 0);
    KWPage page = manager.appendPage();
    QCOMPARE(manager.pageCount(), 1);
    KWPage page2 = manager.appendPage();
    QCOMPARE(manager.pageCount(), 2);
    KWPage page3 = manager.appendPage();
    QCOMPARE(manager.pageCount(), 3);
    page2.setPageSide(KWPage::PageSpread);
    QCOMPARE(manager.pageCount(), 4);
}

void TestPageManager::testPageSpreadLayout()
{
    KWPageManager manager;
    KWPage page = manager.appendPage();
    KoPageLayout layout = page.pageStyle().pageLayout();
    layout.width = 450;
    layout.height = 150;
    QCOMPARE(page.pageNumber(), 1);
    page.setPageSide(KWPage::PageSpread); // makes it page 2 (and 3)
    QCOMPARE(page.pageNumber(), 2);
    page.pageStyle().setPageLayout(layout);
    KWPage page2 = manager.appendPage();
    QCOMPARE(page2.pageNumber(), 4);
    page2.setDirectionHint(KoText::LeftRightTopBottom);
    layout.width = 200;
    page2.pageStyle().setPageLayout(layout);

    QCOMPARE(page.offsetInDocument(), 0.);
    QCOMPARE(page2.offsetInDocument(), 150.);

    QCOMPARE(manager.pageNumber(QPointF(10, 200)), 4);
    QCOMPARE(manager.pageNumber(QPointF(10, 151)), 4);


    KWPage four = page.next();
    QCOMPARE(four.pageNumber(), 4);
    KWPage invalid = four.next();
    QVERIFY(!invalid.isValid());
    QVERIFY(invalid.pageNumber() != 4);
    KWPage copy = invalid;
    QVERIFY(!copy.isValid());
    QVERIFY(copy.pageNumber() != 4);
}

void TestPageManager::testInsertPage()
{
    KWPageManager *pageManager = new KWPageManager();
    QCOMPARE(pageManager->pageCount(), 0);

    // inserting determines the position, not always the page number.
    KWPage page3 = pageManager->insertPage(10);
    QCOMPARE(page3.pageNumber(), 1);
    KWPage page1 = pageManager->insertPage(1);
    QCOMPARE(page1.pageNumber(), 1);
    QCOMPARE(page3.pageNumber(), 2);
    QCOMPARE(pageManager->pageCount(), 2);
    QCOMPARE(pageManager->page(1), page1);
    QCOMPARE(pageManager->page(2), page3);

    KWPage page2 = pageManager->insertPage(2);
    QCOMPARE(page2.pageNumber(), 2);
    QCOMPARE(pageManager->pageCount(), 3);
    QCOMPARE(pageManager->page(1), page1);
    QCOMPARE(pageManager->page(2), page2);
    QCOMPARE(pageManager->page(3), page3);

    KWPage page4 = pageManager->insertPage(4);
    QCOMPARE(pageManager->pageCount(), 4);
    QCOMPARE(pageManager->page(4), page4);

    QCOMPARE(page1.pageNumber(), 1);
    QCOMPARE(page2.pageNumber(), 2);
    QCOMPARE(page3.pageNumber(), 3);
    QCOMPARE(page4.pageNumber(), 4);
}

void TestPageManager::testPadding()
{
    // padding is the 'dead space' between actual pages.  This allows us to print using bleed to PDF.

    KWPageManager *pageManager = new KWPageManager();
    KoInsets padding(6, 7, 9, 13);
    pageManager->setPadding(padding);
    KoInsets padding2 = pageManager->padding();
    QCOMPARE(padding2.top, padding.top);
    QCOMPARE(padding2.bottom, padding.bottom);
    QCOMPARE(padding2.left, padding.left);
    QCOMPARE(padding2.right, padding.right);

    KoPageLayout lay;
    QVERIFY(lay.leftMargin >= 0);
    QVERIFY(lay.bindingSide == -1);
    lay.width = 100;
    lay.height = 50;
    KWPageStyle style("testStyle");
    style.setPageLayout(lay);

    KWPage page1 = pageManager->appendPage(style);
    QVERIFY(page1.isValid());
    QCOMPARE(page1.offsetInDocument(), 0.);
    QCOMPARE(page1.rect(), QRectF(0, 0, 100, 50));
    KWPage page2 = pageManager->appendPage(style);
    QCOMPARE(page2.offsetInDocument(), 50. + 6. + 9.);
    QCOMPARE(page2.rect(), QRectF(0, 65, 100, 50));
    KWPage page3 = pageManager->appendPage(style);
    QCOMPARE(page3.offsetInDocument(), 115 + 6. + 9.);
    QCOMPARE(page3.rect(), QRectF(0, 130, 100, 50));

    padding = KoInsets(1, 2, 3, 4);
    pageManager->setPadding(padding);
    QCOMPARE(page3.offsetInDocument(), 2 * (50. + 1. + 3.)); // they moved :)
    QCOMPARE(page3.rect(), QRectF(0, 108, 100, 50));
}

void TestPageManager::testPageOffset()
{
    KWPageManager *pageManager = new KWPageManager();
    for (int i=0; i < 500; ++i) {
        KWPage page = pageManager->appendPage();
    }

    KWPage page = pageManager->page(1);
    QVERIFY(page.isValid());
    QCOMPARE(page.pageNumber(), 1);
    QCOMPARE(page.offsetInDocument(), 0.);
    const qreal pageHeight = page.pageStyle().pageLayout().height;

    page = pageManager->page(50);
    QVERIFY(page.isValid());
    QCOMPARE(page.pageNumber(), 50);
    QCOMPARE(page.offsetInDocument(), pageHeight * 49);

    KoPageLayout layout = page.pageStyle().pageLayout();
    layout.height = 400;
    page.pageStyle().setPageLayout(layout);
    QCOMPARE(page.offsetInDocument(), (qreal) 400 * 49);
}

void TestPageManager::testBackgroundRefCount()
{
    KWPageStyle ps1("test");
    QVERIFY(ps1.background() == 0);
    KoColorBackground *background = new KoColorBackground(QColor(Qt::red));
    QVERIFY(background->ref());
    QCOMPARE(background->useCount(), 1);

    ps1.setBackground(background);
    QCOMPARE(background->useCount(), 2);

    {
        KWPageStyle ps2("test2");
        QCOMPARE(background->useCount(), 2);
        ps2 = ps1;
        QCOMPARE(background->useCount(), 2);
    }
    QCOMPARE(background->useCount(), 2);

    ps1 = ps1;
    QCOMPARE(background->useCount(), 2);
    ps1.setBackground(0);
    QCOMPARE(background->useCount(), 1);
    delete background;
}

void TestPageManager::testAppendPageSpread()
{
    KWPageManager manager;

    KWPageStyle style = manager.addPageStyle("pagestyle1");
    KoPageLayout layout = style.pageLayout();
    layout.leftMargin = -1;
    layout.rightMargin = -1;
    layout.pageEdge = 7;
    layout.bindingSide = 13;
    style.setPageLayout(layout);

    KWPage page1 = manager.appendPage(style);
    QCOMPARE(page1.pageNumber(), 1);
    QCOMPARE(page1.pageSide(), KWPage::Right);
    QCOMPARE(page1.width(), style.pageLayout().width);
    KWPage test = page1.next();
    QVERIFY(!test.isValid());

    KWPage page2 = manager.appendPage(style);
    QCOMPARE(page1.pageNumber(), 1);
    QCOMPARE(page1.pageSide(), KWPage::Right);
    QCOMPARE(page1.width(), style.pageLayout().width);
    QCOMPARE(page2.pageNumber(), 2);
    QCOMPARE(page2.pageSide(), KWPage::PageSpread);
    QCOMPARE(page2.width(), style.pageLayout().width * 2);
    test = page2.next();
    QVERIFY(!test.isValid());

    KWPage page3 = manager.insertPage(2, style);
    QCOMPARE(page1.pageNumber(), 1);
    QCOMPARE(page1.pageSide(), KWPage::Right);
    QCOMPARE(page1.width(), style.pageLayout().width);
    QCOMPARE(page2.pageNumber(), 4);
    QCOMPARE(page2.pageSide(), KWPage::PageSpread);
    QCOMPARE(page2.width(), style.pageLayout().width * 2);
    QCOMPARE(page3.pageNumber(), 2);
    QCOMPARE(page3.pageSide(), KWPage::PageSpread);
    QCOMPARE(page3.width(), style.pageLayout().width * 2);
    test = page2.next();
    QVERIFY(!test.isValid());

    KWPage realPage3 = manager.page(3);
    QVERIFY(realPage3.isValid());
    QCOMPARE(realPage3.pageNumber(), 2); // its still a pagespread
}

void TestPageManager::testRemovePageSpread()
{
    KWPageManager manager;

    KoPageLayout layout = manager.defaultPageStyle().pageLayout();
    layout.leftMargin = -1;
    layout.rightMargin = -1;
    layout.pageEdge = 7;
    layout.bindingSide = 13;
    manager.defaultPageStyle().setPageLayout(layout);

    KWPage page1 = manager.appendPage();
    KWPage pageSpread1 = manager.appendPage();
    KWPage pageSpread2 = manager.appendPage();
    QCOMPARE(pageSpread1.pageSide(), KWPage::PageSpread);
    QCOMPARE(pageSpread2.pageSide(), KWPage::PageSpread);
    QCOMPARE(manager.pageCount(), 5);

    manager.removePage(pageSpread2); // remove from end
    QVERIFY(pageSpread1.isValid());
    QCOMPARE(pageSpread1.pageSide(), KWPage::PageSpread);
    QCOMPARE(manager.pageCount(), 3);
    QVERIFY(!pageSpread2.isValid());
    QVERIFY(pageSpread2.pageNumber() < 0);

    // re-add so we can remove something in the middle
    pageSpread2 = manager.appendPage();
    QCOMPARE(pageSpread1.pageSide(), KWPage::PageSpread);
    QCOMPARE(pageSpread2.pageSide(), KWPage::PageSpread);
    QCOMPARE(manager.pageCount(), 5);


    manager.removePage(pageSpread1); // remove pages 2&3
    QVERIFY(!pageSpread1.isValid());
    QVERIFY(pageSpread1.pageNumber() < 0);
    QCOMPARE(pageSpread2.pageSide(), KWPage::PageSpread);
    QCOMPARE(manager.pageCount(), 3);
    QVERIFY(pageSpread2.isValid());
}

QTEST_KDEMAIN(TestPageManager, GUI)
#include <TestPageManager.moc>
