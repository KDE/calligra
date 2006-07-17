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
#include "TestPageManager.h"

#include <KWPageManager.h>
#include <KWPage.h>

#include <QRectF>
#include <kdebug.h>
#include <kinstance.h>

void TestPageManager::init() {
    new KInstance ("TestPageManager");
}

void TestPageManager::getAddPages() {
    KWPageManager *pageManager = new KWPageManager();
    pageManager->setStartPage(1);
    pageManager->appendPage();
    KWPage *page = pageManager->page(0);
    QCOMPARE(page == 0, true);
    page = pageManager->page(2);
    QCOMPARE(page == 0, true);

    page = pageManager->page(1);
    QCOMPARE(page == 0, false);
    QCOMPARE(page->pageNumber(), 1);
    QCOMPARE(page->pageSide(), KWPage::Right);

    page->setWidth(134.2);
    page->setHeight(521.4);
    QCOMPARE(page->width(), 134.2);
    QCOMPARE(page->height(), 521.4);

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
}

void TestPageManager::getAddPages2() {
    KWPageManager *pageManager = new KWPageManager();
    pageManager->setStartPage(1);
    KWPage *page = pageManager->appendPage();
    page->setWidth(200);
    page->setHeight(200);

    MockShape shape1;
    shape1.setPosition(QPointF(0,0));
    shape1.resize(QSizeF(2,2));
    QCOMPARE(pageManager->pageNumber(&shape1), 1);

    MockShape shape2;
    shape2.setPosition(QPointF(90,90));
    shape2.resize(QSizeF(2,2));
    QCOMPARE(pageManager->pageNumber(&shape2), 1);

    MockShape shape3;
    shape3.setPosition(QPointF(190,190));
    shape3.resize(QSizeF(9,9));
    QCOMPARE(pageManager->pageNumber(&shape3), 1);

    page = pageManager->appendPage();
    page->setWidth(600);
    page->setHeight(600);
    QCOMPARE(pageManager->pageNumber(&shape1), 1);
    QCOMPARE(pageManager->pageNumber(&shape2), 1);
    QCOMPARE(pageManager->pageNumber(&shape3), 1);

    shape1.setPosition(QPointF(201,201));
    QCOMPARE(pageManager->pageNumber(&shape1), 2);
    shape1.setPosition(QPointF(300,3));
    QCOMPARE(pageManager->pageNumber(&shape1), 1); // right of page 1
    shape3.setPosition(QPointF(2,690));
    QCOMPARE(pageManager->pageNumber(&shape3), 2);
    shape3.setPosition(QPointF(300,300));
    QCOMPARE(pageManager->pageNumber(&shape3), 2);
    //QCOMPARE(pageManager->pageNumber(QRectF(300,300,10,10)), 2);
    shape3.setPosition(QPointF(600,700));
    QCOMPARE(pageManager->pageNumber(&shape3), 2);
    //QCOMPARE(pageManager->pageNumber(QRectF(600,700,10,10)), 2); // right of page 2

    // KoPoint based
    QCOMPARE(pageManager->pageNumber(QPointF(201,201)), 2);
    // Y based
    QCOMPARE(pageManager->pageNumber(201.0), 2);
    QCOMPARE(pageManager->pageNumber(900.0), 2);
}

void TestPageManager::createInsertPages() {
    KWPageManager *pageManager = new KWPageManager();
    pageManager->setStartPage(1);
    QCOMPARE(pageManager->pageCount(), 0);
    KWPage *page1 = pageManager->appendPage();
    QCOMPARE(pageManager->pageCount(), 1);
    KWPage *page3 = pageManager->appendPage();
    QCOMPARE(pageManager->pageCount(), 2);
    QCOMPARE(page3->pageNumber(), 2);

    KWPage *page2 = pageManager->insertPage(2);
    QCOMPARE(pageManager->pageCount(), 3);
    QCOMPARE(page1->pageNumber(), 1);
    QCOMPARE(page2->pageNumber(), 2);
    QCOMPARE(page3->pageNumber(), 3);

    KWPage *page4 = pageManager->insertPage(100);
    QCOMPARE(pageManager->pageCount(), 4);
    QCOMPARE(page1->pageNumber(), 1);
    QCOMPARE(page2->pageNumber(), 2);
    QCOMPARE(page3->pageNumber(), 3);
    QCOMPARE(page4->pageNumber(), 4);

    KWPage *page = pageManager->insertPage(0);
    QCOMPARE(pageManager->pageCount(), 5);
    QCOMPARE(page->pageNumber(), 1);
    QCOMPARE(page1->pageNumber(), 2);
    QCOMPARE(page2->pageNumber(), 3);
    QCOMPARE(page3->pageNumber(), 4);
    QCOMPARE(page4->pageNumber(), 5);

    pageManager->setOnlyAllowAppend(true);
    KWPage *page6 = pageManager->insertPage(0);
    QCOMPARE(pageManager->pageCount(), 6);
    QCOMPARE(page1->pageNumber(), 2);
    QCOMPARE(page6->pageNumber(), 6);
}

void TestPageManager::removePages() {
    KWPageManager *pageManager = new KWPageManager();
    pageManager->setStartPage(1);
    KWPage *page1 = pageManager->appendPage();
    pageManager->appendPage();
    KWPage *page2 = pageManager->appendPage();
    /*KWPage *page3 =*/ pageManager->appendPage();
    KWPage *page4 = pageManager->appendPage();

    pageManager->removePage(2);
    QCOMPARE(page1->pageNumber(), 1);
    QCOMPARE(page2->pageNumber(), 2);
    QCOMPARE(page4->pageNumber(), 4);
    pageManager->removePage(page2);
    QCOMPARE(page1->pageNumber(), 1);
    QCOMPARE(page4->pageNumber(), 3);

    /* todo: bool tryRemovingPages(); */
}

void TestPageManager::pageInfo() {
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

    QCOMPARE(pageManager->topOfPage(3), 300.0);
    QCOMPARE(pageManager->bottomOfPage(3), 900.0);
    layout.ptHeight = 500;
    pageManager->setDefaultPage(layout);
    QCOMPARE(pageManager->bottomOfPage(3), 800.0);
    page2->setHeight(-1);
    QCOMPARE(pageManager->bottomOfPage(3), 1200.0);

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
    QCOMPARE(lay.ptHeight, 500.0);
    QCOMPARE(lay.ptWidth, 50.0);
    QCOMPARE(lay.ptTop, 9.0);
    QCOMPARE(lay.ptLeft, 10.0);
    QCOMPARE(lay.ptBottom, 11.0);
    QCOMPARE(lay.ptRight, 12.0);

    lay = pageManager->pageLayout(3);
    QCOMPARE(lay.ptHeight, 500.0);
    QCOMPARE(lay.ptWidth, 300.0);
    QCOMPARE(lay.ptTop, 5.0);
    QCOMPARE(lay.ptLeft, 6.0);
    QCOMPARE(lay.ptBottom, 7.0);
    QCOMPARE(lay.ptRight, 8.0);

    lay.ptRight = 90; // should have no effect, since its a copy
    QCOMPARE(page3->rightMargin(), 8.0);


    // Page Edge / Page Margin
    page1->setPageEdgeMargin(14.0);
    QCOMPARE(page1->pageSide(), KWPage::Right);
    QCOMPARE(page1->rightMargin(), 14.0);
    page1->setMarginClosestBinding(15.0);
    QCOMPARE(page1->rightMargin(), 14.0);
    QCOMPARE(page1->leftMargin(), 15.0);

    QCOMPARE(page2->rightMargin(), 12.0); // unchanged due to changes in page1
    QCOMPARE(page2->leftMargin(), 10.0);
    page2->setPageEdgeMargin(16.0);
    QCOMPARE(page2->pageSide(), KWPage::Left);
    QCOMPARE(page2->leftMargin(), 16.0);
    page2->setMarginClosestBinding(17.0);
    QCOMPARE(page2->leftMargin(), 16.0);
    QCOMPARE(page2->rightMargin(), 17.0);

    page2->setLeftMargin(18);
    QCOMPARE(page2->leftMargin(), 18.0);
    page2->setRightMargin(19);
    QCOMPARE(page2->rightMargin(), 19.0);
    QCOMPARE(page2->leftMargin(), 18.0);
}

void TestPageManager::testClipToDocument() {
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

    QPointF p(10,10);

    QPointF result = pageManager->clipToDocument(p);
    QCOMPARE(p == result, true);

    p.setX(110);
    result = pageManager->clipToDocument(p);
    QCOMPARE(p.y(), result.y());
    QCOMPARE(result.x(), 100.0);

    p.setY(210);
    result = pageManager->clipToDocument(p);
    QCOMPARE(p.y(), result.y());
    QCOMPARE(result.x(), 50.0);

    p.setY(330);
    result = pageManager->clipToDocument(p);
    QCOMPARE(p == result, true);

    p.setY(910);
    p.setX(310);
    result = pageManager->clipToDocument(p);
    QCOMPARE(result.y(), 900.0);
    QCOMPARE(result.x(), 300.0);
}

QTEST_MAIN(TestPageManager)
#include "TestPageManager.moc"
