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

#include "KWPageManagerTester.h"

#include <KWPageManager.h>
#include <KWPage.h>

using namespace KUnitTest;

KUNITTEST_MODULE(kunittest_KWPageManagerTester, "PageManager Tester");
KUNITTEST_MODULE_REGISTER_TESTER(KWPageManagerTester);

void KWPageManagerTester::allTests() {
    getAddPages();
    getAddPages2();
    createInsertPages();
    removePages();
    pageInfo();
    testClipToDocument();
}

void KWPageManagerTester::getAddPages() {
    KWPageManager *pageManager = new KWPageManager();
    pageManager->setStartPage(1);
    pageManager->appendPage();
    KWPage *page = pageManager->page(0);
    CHECK(page == 0, true);
    page = pageManager->page(2);
    CHECK(page == 0, true);

    page = pageManager->page(1);
    CHECK(page == 0, false);
    CHECK(page->pageNumber(), 1);
    CHECK(page->pageSide(), KWPage::Right);

    page->setWidth(134.2);
    page->setHeight(521.4);
    CHECK(page->width(), 134.2);
    CHECK(page->height(), 521.4);

    // test setStartPage
    pageManager->setStartPage(4);
    page = pageManager->page(0);
    CHECK(page == 0, true);
    page = pageManager->page(3);
    CHECK(page == 0, true);
    page = pageManager->page(5);
    CHECK(page == 0, true);
    page = pageManager->page(4);
    CHECK(page == 0, false);
    CHECK(page->pageNumber(), 4);
    CHECK(page->pageSide(), KWPage::Left);

    pageManager->setStartPage(1);
    page = pageManager->page(0);
    CHECK(page == 0, true);
    page = pageManager->page(3);
    CHECK(page == 0, true);
    page = pageManager->page(2);
    CHECK(page == 0, true);
    page = pageManager->page(1);
    CHECK(page == 0, false);
    CHECK(page->pageNumber(), 1);
    CHECK(page->pageSide(), KWPage::Right);

    // adding pages
    CHECK(pageManager->pageCount(), 1);
    CHECK(pageManager->lastPageNumber(), 1);
    pageManager->setStartPage(40);
    CHECK(pageManager->pageCount(), 1);
    CHECK(pageManager->lastPageNumber(), 40);
    page = pageManager->appendPage();
    CHECK(pageManager->pageCount(), 2);
    CHECK(pageManager->lastPageNumber(), 41);

    CHECK(page == 0, false);
    CHECK(page->pageNumber(), 41);
    CHECK(page->pageSide(), KWPage::Right);
}

void KWPageManagerTester::getAddPages2() {
    KWPageManager *pageManager = new KWPageManager();
    pageManager->setStartPage(1);
    KWPage *page = pageManager->appendPage();
    page->setWidth(200);
    page->setHeight(200);

    CHECK(pageManager->pageNumber(KoRect(0,0,2,2)), 1);
    CHECK(pageManager->pageNumber(KoRect(90,90,2,2)), 1);
    CHECK(pageManager->pageNumber(KoRect(190,190,9,9)), 1);

    page = pageManager->appendPage();
    page->setWidth(600);
    page->setHeight(600);
    CHECK(pageManager->pageNumber(KoRect(0,0,2,2)), 1);
    CHECK(pageManager->pageNumber(KoRect(90,90,2,2)), 1);
    CHECK(pageManager->pageNumber(KoRect(190,190,9,9)), 1);

    CHECK(pageManager->pageNumber(KoRect(201,201,2,2)), 2);
#ifdef STRICT_PAGECHECKS // was true before KWPageManager.cpp -r511705:511706
    CHECK(pageManager->pageNumber(KoRect(300,3,2,2)), -1); // right of page 1
#else
    CHECK(pageManager->pageNumber(KoRect(300,3,2,2)), 1); // right of page 1
#endif
    CHECK(pageManager->pageNumber(KoRect(2, 690,9,9)), 2);
    CHECK(pageManager->pageNumber(KoRect(300,300,10,10)), 2);
#ifdef STRICT_PAGECHECKS // was true before KWPageManager.cpp -r511705:511706
    CHECK(pageManager->pageNumber(KoRect(600,700,10,10)), -1); // right of page 2
#else
    CHECK(pageManager->pageNumber(KoRect(600,700,10,10)), 2); // right of page 2
#endif

    // KoPoint based
    CHECK(pageManager->pageNumber(KoPoint(201,201)), 2);
    // Y based
    CHECK(pageManager->pageNumber(201.0), 2);
#ifdef STRICT_PAGECHECKS // was true before KWPageManager.cpp -r511705:511706
    CHECK(pageManager->pageNumber(900.0), -1);
#else
    CHECK(pageManager->pageNumber(900.0), 2);
#endif
}

void KWPageManagerTester::createInsertPages() {
    KWPageManager *pageManager = new KWPageManager();
    pageManager->setStartPage(1);
    CHECK(pageManager->pageCount(), 0);
    KWPage *page1 = pageManager->appendPage();
    CHECK(pageManager->pageCount(), 1);
    KWPage *page3 = pageManager->appendPage();
    CHECK(pageManager->pageCount(), 2);
    CHECK(page3->pageNumber(), 2);

    KWPage *page2 = pageManager->insertPage(2);
    CHECK(pageManager->pageCount(), 3);
    CHECK(page1->pageNumber(), 1);
    CHECK(page2->pageNumber(), 2);
    CHECK(page3->pageNumber(), 3);

    KWPage *page4 = pageManager->insertPage(100);
    CHECK(pageManager->pageCount(), 4);
    CHECK(page1->pageNumber(), 1);
    CHECK(page2->pageNumber(), 2);
    CHECK(page3->pageNumber(), 3);
    CHECK(page4->pageNumber(), 4);

    KWPage *page = pageManager->insertPage(0);
    CHECK(pageManager->pageCount(), 5);
    CHECK(page->pageNumber(), 1);
    CHECK(page1->pageNumber(), 2);
    CHECK(page2->pageNumber(), 3);
    CHECK(page3->pageNumber(), 4);
    CHECK(page4->pageNumber(), 5);

    pageManager->setOnlyAllowAppend(true);
    KWPage *page6 = pageManager->insertPage(0);
    CHECK(pageManager->pageCount(), 6);
    CHECK(page1->pageNumber(), 2);
    CHECK(page6->pageNumber(), 6);
}

void KWPageManagerTester::removePages() {
    KWPageManager *pageManager = new KWPageManager();
    pageManager->setStartPage(1);
    KWPage *page1 = pageManager->appendPage();
    pageManager->appendPage();
    KWPage *page2 = pageManager->appendPage();
    /*KWPage *page3 =*/ pageManager->appendPage();
    KWPage *page4 = pageManager->appendPage();

    pageManager->removePage(2);
    CHECK(page1->pageNumber(), 1);
    CHECK(page2->pageNumber(), 2);
    CHECK(page4->pageNumber(), 4);
    pageManager->removePage(page2);
    CHECK(page1->pageNumber(), 1);
    CHECK(page4->pageNumber(), 3);

    /* todo: bool tryRemovingPages(); */
}

void KWPageManagerTester::pageInfo() {
    KWPageManager *pageManager = new KWPageManager();
    KoPageLayout layout;
    layout.ptWidth = 300;
    layout.ptHeight = 600;
    layout.format = PG_DIN_A4;
    pageManager->setDefaultPage(layout);
    pageManager->setStartPage(1);
    KWPage *page1 = pageManager->appendPage();
    page1->setWidth(100);
    page1->setHeight(200);
    KWPage *page2 = pageManager->appendPage();
    page2->setWidth(50);
    page2->setHeight(100);
    KWPage *page3 = pageManager->appendPage();

    CHECK(pageManager->topOfPage(3), 300.0);
    CHECK(pageManager->bottomOfPage(3), 900.0);
    layout.ptHeight = 500;
    pageManager->setDefaultPage(layout);
    CHECK(pageManager->bottomOfPage(3), 800.0);
    page2->setHeight(-1);
    CHECK(pageManager->bottomOfPage(3), 1200.0);

    layout.ptTop = 5;
    layout.ptLeft = 6;
    layout.ptBottom = 7;
    layout.ptRight = 8;
    layout.ptBindingSide = -1;
    layout.ptPageEdge = -1;
    pageManager->setDefaultPage(layout);
    page2->setTopMargin(9);
    page2->setLeftMargin(10);
    page2->setBottomMargin(11);
    page2->setRightMargin(12);

    KoPageLayout lay = pageManager->pageLayout(2);
    CHECK(lay.ptHeight, 500.0);
    CHECK(lay.ptWidth, 50.0);
    CHECK(lay.ptTop, 9.0);
    CHECK(lay.ptLeft, 10.0);
    CHECK(lay.ptBottom, 11.0);
    CHECK(lay.ptRight, 12.0);

    lay = pageManager->pageLayout(3);
    CHECK(lay.ptHeight, 500.0);
    CHECK(lay.ptWidth, 300.0);
    CHECK(lay.ptTop, 5.0);
    CHECK(lay.ptLeft, 6.0);
    CHECK(lay.ptBottom, 7.0);
    CHECK(lay.ptRight, 8.0);

    lay.ptRight = 90; // should have no effect, since its a copy
    CHECK(page3->rightMargin(), 8.0);


    // Page Edge / Page Margin
    page1->setPageEdgeMargin(14.0);
    CHECK(page1->pageSide(), KWPage::Right);
    CHECK(page1->rightMargin(), 14.0);
    page1->setMarginClosestBinding(15.0);
    CHECK(page1->rightMargin(), 14.0);
    CHECK(page1->leftMargin(), 15.0);

    CHECK(page2->rightMargin(), 12.0); // unchanged due to changes in page1
    CHECK(page2->leftMargin(), 10.0);
    page2->setPageEdgeMargin(16.0);
    CHECK(page2->pageSide(), KWPage::Left);
    CHECK(page2->leftMargin(), 16.0);
    page2->setMarginClosestBinding(17.0);
    CHECK(page2->leftMargin(), 16.0);
    CHECK(page2->rightMargin(), 17.0);

    page2->setLeftMargin(18);
    CHECK(page2->leftMargin(), 18.0);
    page2->setRightMargin(19);
    CHECK(page2->rightMargin(), 19.0);
    CHECK(page2->leftMargin(), 18.0);
}

void KWPageManagerTester::testClipToDocument() {
    KWPageManager *pageManager = new KWPageManager();
    KoPageLayout lay;
    lay.ptWidth = 300;
    lay.ptHeight = 600;
    lay.format = PG_DIN_A4;
    pageManager->setDefaultPage(lay);
    KWPage *page1 = pageManager->appendPage();
    page1->setWidth(100);
    page1->setHeight(200);
    KWPage *page2 = pageManager->appendPage();
    page2->setWidth(50);
    page2->setHeight(100);
    /*KWPage *page3 =*/ pageManager->appendPage();

    KoPoint p(10,10);

    KoPoint result = pageManager->clipToDocument(p);
    CHECK(p == result, true);

    p.setX(110);
    result = pageManager->clipToDocument(p);
    CHECK(p.y(), result.y());
    CHECK(result.x(), 100.0);

    p.setY(210);
    result = pageManager->clipToDocument(p);
    CHECK(p.y(), result.y());
    CHECK(result.x(), 50.0);

    p.setY(330);
    result = pageManager->clipToDocument(p);
    CHECK(p == result, true);

    p.setY(910);
    p.setX(310);
    result = pageManager->clipToDocument(p);
    CHECK(result.y(), 900.0);
    CHECK(result.x(), 300.0);
}
