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
#include <kcomponentdata.h>

void TestPageManager::init() {
    new KComponentData("TestPageManager");
}

void TestPageManager::getAddPages() {
    KWPageManager *pageManager = new KWPageManager(&m_doc);

    QCOMPARE(pageManager->pageCount() == 0, true);
    QCOMPARE(pageManager->page(-10) == 0, true);
    QCOMPARE(pageManager->page(10) == 0, true);

    KWPage* page1 = pageManager->appendPage();
    KWPage* page3 = pageManager->appendPage();
    QCOMPARE(pageManager->pageCount() == 2, true);
    QCOMPARE(pageManager->page(0) == page1, true);
    QCOMPARE(pageManager->page(1) == page3, true);

    KWPage* page2 = pageManager->insertPage(1);
    QCOMPARE(pageManager->pageCount() == 3, true);
    QCOMPARE(pageManager->page(0) == page1, true);
    QCOMPARE(pageManager->page(1) == page2, true);
    QCOMPARE(pageManager->page(2) == page3, true);

    KWPage* page4 = pageManager->insertPage(3);
    QCOMPARE(pageManager->pageCount() == 4, true);
    QCOMPARE(pageManager->page(3) == page4, true);

    QCOMPARE(page1->pageNumber(), 0);
    QCOMPARE(page2->pageNumber(), 1);
    QCOMPARE(page3->pageNumber(), 2);
    QCOMPARE(page4->pageNumber(), 3);

#if 0
    page = pageManager->page(1);
    QCOMPARE(page == 0, false);
    QCOMPARE(page->pageNumber(), 1);
    QCOMPARE(page->pageSide(), KWPage::Right);
    KoPageLayout pageLayout = page->pageStyle()->pageLayout();
    pageLayout.width = 134.2;
    pageLayout.height = 521.4;
    page->pageStyle()->setPageLayout(pageLayout);
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
#endif
}

void TestPageManager::getAddPages2() {
    KWPageManager *pageManager = new KWPageManager(&m_doc);
#if 0
    pageManager->setStartPage(1);
    KWPage *page = pageManager->appendPage();
    KoPageLayout pageLayout = page->pageStyle()->pageLayout();
    pageLayout.width = 200;
    pageLayout.height = 200;
    page->pageStyle()->setPageLayout(pageLayout);

    MockShape shape1;
    shape1.setPosition(QPointF(0,0));
    shape1.setSize(QSizeF(2,2));
    QCOMPARE(pageManager->pageNumber(&shape1), 1);

    MockShape shape2;
    shape2.setPosition(QPointF(90,90));
    shape2.setSize(QSizeF(2,2));
    QCOMPARE(pageManager->pageNumber(&shape2), 1);

    MockShape shape3;
    shape3.setPosition(QPointF(190,190));
    shape3.setSize(QSizeF(9,9));
    QCOMPARE(pageManager->pageNumber(&shape3), 1);

    KWPageStyle *settingsPage2 = new KWPageStyle("page2");
    pageLayout = settingsPage2->pageLayout();
    pageLayout.width = 600;
    pageLayout.height = 600;
    settingsPage2->setPageLayout(pageLayout);
    page = pageManager->appendPage(settingsPage2);
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
#endif
}

void TestPageManager::createInsertPages() {
    KWPageManager *pageManager = new KWPageManager(&m_doc);

    QCOMPARE(pageManager->pageCount(), 0);
    KWPage *page1 = pageManager->appendPage();
    QCOMPARE(pageManager->pageCount(), 1);
    KWPage *page3 = pageManager->appendPage();
    QCOMPARE(pageManager->pageCount(), 2);
    QCOMPARE(page3->pageNumber(), 1);

    KWPage *page2 = pageManager->insertPage(1);
    QCOMPARE(pageManager->pageCount(), 3);
    QCOMPARE(page1->pageNumber(), 0);
    QCOMPARE(page2->pageNumber(), 1);
    QCOMPARE(page3->pageNumber(), 2);

    KWPage *page4 = pageManager->insertPage(-100);
    KWPage *page5 = pageManager->insertPage(100);
    QCOMPARE(pageManager->pageCount(), 5);
    QCOMPARE(page1->pageNumber(), 0);
    QCOMPARE(page2->pageNumber(), 1);
    QCOMPARE(page3->pageNumber(), 2);
    QCOMPARE(page4->pageNumber(), 3);
    QCOMPARE(page5->pageNumber(), 4);

    KWPage *page6 = pageManager->insertPage(0);
    KWPage *page7 = pageManager->insertPage(1);
    QCOMPARE(pageManager->pageCount(), 7);
    QCOMPARE(page6->pageNumber(), 0);
    QCOMPARE(page7->pageNumber(), 1);
    QCOMPARE(page1->pageNumber(), 2);
    QCOMPARE(page2->pageNumber(), 3);
    QCOMPARE(page3->pageNumber(), 4);
    QCOMPARE(page4->pageNumber(), 5);
}

void TestPageManager::removePages() {
    KWPageManager *pageManager = new KWPageManager(&m_doc);

    KWPage *page1 = pageManager->appendPage();
    KWPage *page2 = pageManager->appendPage();
    pageManager->appendPage();
    KWPage *page4 = pageManager->appendPage();

    pageManager->removePage(2);
    QCOMPARE(page1->pageNumber(), 0);
    QCOMPARE(page2->pageNumber(), 1);
    QCOMPARE(page4->pageNumber(), 2);

    pageManager->removePage(page2);
    QCOMPARE(page1->pageNumber(), 0);
    QCOMPARE(page4->pageNumber(), 1);

    /* todo: bool tryRemovingPages(); */
}

void TestPageManager::pageInfo() {
    KWPageManager *pageManager = new KWPageManager(&m_doc);

    KoPageLayout layout = pageManager->defaultPageStyle()->pageLayout();
    layout.width = 100;
    layout.height = 200;
    layout.format = KoPageFormat::IsoA4Size;
    pageManager->defaultPageStyle()->setPageLayout(layout);
    QCOMPARE(pageManager->defaultPageStyle()->pageLayout().width, 100.0);
    QCOMPARE(pageManager->defaultPageStyle()->pageLayout().format, KoPageFormat::IsoA4Size);

    KWPageStyle *pageStylePage2 = new KWPageStyle("Page 2");
    layout = pageStylePage2->pageLayout();
    layout.width = 50;
    layout.height = 100;
    pageStylePage2->setPageLayout(layout);
    pageManager->addPageStyle(pageStylePage2);
    QCOMPARE(pageManager->pageStyle("Page 2")->pageLayout().width, 50.0);

    KWPageStyle *pageStylePage3 = new KWPageStyle("Page 3");
    layout = pageStylePage3->pageLayout();
    layout.width = 300;
    layout.height = 600;
    pageStylePage3->setPageLayout(layout);
    pageManager->addPageStyle(pageStylePage3);
    QCOMPARE(pageManager->pageStyle("Page 3")->pageLayout().width, 300.0);

    KWPage *page1 = pageManager->appendPage();
    KWPage *page2 = pageManager->appendPage(pageStylePage2);
    KWPage *page3 = pageManager->appendPage(pageStylePage3);

    QCOMPARE(pageManager->pageCount(), 3);
    QCOMPARE(pageManager->page(0), page1);
    QCOMPARE(pageManager->page(0)->pageStyle(), pageManager->defaultPageStyle());
    QCOMPARE(pageManager->page(1)->pageStyle(), pageStylePage2);
    QCOMPARE(pageManager->page(2)->pageStyle(), pageStylePage3);

    QCOMPARE(pageManager->topOfPage(2), 300.0);
    QCOMPARE(pageManager->bottomOfPage(2), 900.0);

    layout = pageStylePage3->pageLayout();
    layout.height = 500;
    pageStylePage3->setPageLayout(layout);
    QCOMPARE(pageManager->bottomOfPage(3), 800.0);

    layout = pageManager->defaultPageStyle()->pageLayout();
    layout.top = 5;
    layout.left = 6;
    layout.bottom = 7;
    layout.right = 8;
    pageManager->defaultPageStyle()->setPageLayout(layout);

    layout = pageStylePage2->pageLayout();
    layout.top = 9;
    layout.left = 10;
    layout.bottom = 11;
    layout.right = 12;
    pageStylePage2->setPageLayout(layout);

    layout = page1->pageStyle()->pageLayout();
    layout.right = 14.0;
    page1->pageStyle()->setPageLayout(layout);
    QCOMPARE(page1->rightMargin(), 14.0);

#if 0
    // Page Edge / Page Margin
    layout = pageManager->defaultPageStyle()->pageLayout();
    layout.pageEdge = 14.0;
    pageManager->defaultPageStyle()->setPageLayout(layout);
    QCOMPARE(page1->pageSide(), KWPage::Right);
    QCOMPARE(page1->rightMargin(), 14.0);
   
    layout.bindingSide = 15.0;
    pageManager->defaultPageStyle()->setPageLayout(layout);
    QCOMPARE(page1->rightMargin(), 14.0);
    QCOMPARE(page1->leftMargin(), 15.0);
    //QCOMPARE(page2->rightMargin(), 12.0); // unchanged due to changes in page1
    //QCOMPARE(page2->leftMargin(), 10.0);
    
    layout = pageStylePage2->pageLayout();
    layout.pageEdge = 16.0;
    pageStylePage2->setPageLayout(layout);
    QCOMPARE(page2->pageSide(), KWPage::Left);
    QCOMPARE(page2->leftMargin(), 16.0);
    layout.bindingSide = 17.0;
    pageStylePage2->setPageLayout(layout);
    QCOMPARE(page2->leftMargin(), 16.0);
    QCOMPARE(page2->rightMargin(), 17.0);
    layout.left = 18;
    pageStylePage2->setPageLayout(layout);
    //QCOMPARE(page2->leftMargin(), 18.0);
    layout.right = 19;
    pageStylePage2->setPageLayout(layout);
    //QCOMPARE(page2->rightMargin(), 19.0);
    //QCOMPARE(page2->leftMargin(), 18.0);
#endif
}

void TestPageManager::testClipToDocument() {
    KWPageManager *pageManager = new KWPageManager(&m_doc);
    KoPageLayout lay;
    lay.width = 300;
    lay.height = 600;
    lay.format = KoPageFormat::IsoA4Size;
    pageManager->defaultPageStyle()->setPageLayout(lay);

    KWPageStyle *pageStyle1 = new KWPageStyle("page1");
    lay = pageStyle1->pageLayout();
    lay.width = 100;
    lay.height = 200;
    pageStyle1->setPageLayout(lay);
    pageManager->addPageStyle(pageStyle1);

    KWPageStyle *pageStyle2 = new KWPageStyle("page2");
    lay = pageStyle2->pageLayout();
    lay.width = 50;
    lay.height = 100;
    pageStyle2->setPageLayout(lay);
    pageManager->addPageStyle(pageStyle2);

    KWPage *page1 = pageManager->appendPage(pageStyle1);
    KWPage *page2 = pageManager->appendPage(pageStyle2);
    pageManager->appendPage(pageManager->defaultPageStyle());

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

QTEST_KDEMAIN(TestPageManager, GUI)
#include "TestPageManager.moc"
