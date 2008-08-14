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
    KoPageLayout pageLayout = page->pageSettings()->pageLayout();
    pageLayout.width = 134.2;
    pageLayout.height = 521.4;
    page->pageSettings()->setPageLayout(pageLayout);
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
    KWPageManager *pageManager = new KWPageManager(&m_doc);
    pageManager->setStartPage(1);
    KWPage *page = pageManager->appendPage();
    KoPageLayout pageLayout = page->pageSettings()->pageLayout();
    pageLayout.width = 200;
    pageLayout.height = 200;
    page->pageSettings()->setPageLayout(pageLayout);

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

    KWPageSettings *settingsPage2 = new KWPageSettings("page2");
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
}

void TestPageManager::createInsertPages() {
    KWPageManager *pageManager = new KWPageManager(&m_doc);
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
    KWPageManager *pageManager = new KWPageManager(&m_doc);
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
    KWPageManager *pageManager = new KWPageManager(&m_doc);

    KoPageLayout layout = pageManager->defaultPageSettings()->pageLayout();
    layout.width = 100;
    layout.height = 200;
    layout.format = KoPageFormat::IsoA4Size;
    pageManager->defaultPageSettings()->setPageLayout(layout);
    
    KWPageSettings *pageSettingsPage2 = new KWPageSettings("Page 2");
    layout = pageSettingsPage2->pageLayout();
    layout.width = 50;
    layout.height = 100;
    pageSettingsPage2->setPageLayout(layout);
    pageManager->addPageSettings(pageSettingsPage2);

    KWPageSettings *pageSettingsPage3 = new KWPageSettings("Page 3");
    layout = pageSettingsPage3->pageLayout();
    layout.width = 300;
    layout.height = 600;
    pageSettingsPage3->setPageLayout(layout);
    pageManager->addPageSettings(pageSettingsPage3);
    
    pageManager->setStartPage(1);
    KWPage *page1 = pageManager->appendPage();
    KWPage *page2 = pageManager->appendPage(pageSettingsPage2);
    KWPage *page3 = pageManager->appendPage(pageSettingsPage3);

    QCOMPARE(pageManager->topOfPage(3), 300.0);
    QCOMPARE(pageManager->bottomOfPage(3), 900.0);

    layout = pageSettingsPage3->pageLayout();
    layout.height = 500;
    pageSettingsPage3->setPageLayout(layout);
    QCOMPARE(pageManager->bottomOfPage(3), 800.0);

    layout = pageManager->defaultPageSettings()->pageLayout();
    layout.top = 5;
    layout.left = 6;
    layout.bottom = 7;
    layout.right = 8;
    pageManager->defaultPageSettings()->setPageLayout(layout);
    
    layout = pageSettingsPage2->pageLayout();
    layout.top = 9;
    layout.left = 10;
    layout.bottom = 11;
    layout.right = 12;
    pageSettingsPage2->setPageLayout(layout);
    
    // Page Edge / Page Margin
    layout = pageManager->defaultPageSettings()->pageLayout();
    layout.pageEdge = 14.0;
    pageManager->defaultPageSettings()->setPageLayout(layout);

    QCOMPARE(page1->pageSide(), KWPage::Right);
    QCOMPARE(page1->rightMargin(), 14.0);
   
    layout.bindingSide = 15.0;
    pageManager->defaultPageSettings()->setPageLayout(layout);
    QCOMPARE(page1->rightMargin(), 14.0);
    QCOMPARE(page1->leftMargin(), 15.0);
    //QCOMPARE(page2->rightMargin(), 12.0); // unchanged due to changes in page1
    //QCOMPARE(page2->leftMargin(), 10.0);
    
    layout = pageSettingsPage2->pageLayout();
    layout.pageEdge = 16.0;
    pageSettingsPage2->setPageLayout(layout);
    QCOMPARE(page2->pageSide(), KWPage::Left);
    QCOMPARE(page2->leftMargin(), 16.0);
    layout.bindingSide = 17.0;
    pageSettingsPage2->setPageLayout(layout);
    QCOMPARE(page2->leftMargin(), 16.0);
    QCOMPARE(page2->rightMargin(), 17.0);
    layout.left = 18;
    pageSettingsPage2->setPageLayout(layout);
    //QCOMPARE(page2->leftMargin(), 18.0);
    layout.right = 19;
    pageSettingsPage2->setPageLayout(layout);
    //QCOMPARE(page2->rightMargin(), 19.0);
    //QCOMPARE(page2->leftMargin(), 18.0);
}

void TestPageManager::testClipToDocument() {
    KWPageManager *pageManager = new KWPageManager(&m_doc);
    KoPageLayout lay;
    lay.width = 300;
    lay.height = 600;
    lay.format = KoPageFormat::IsoA4Size;
    pageManager->defaultPageSettings()->setPageLayout(lay);

    KWPageSettings *pageSettings1 = new KWPageSettings("page1");
    lay = pageSettings1->pageLayout();
    lay.width = 100;
    lay.height = 200;
    pageSettings1->setPageLayout(lay);
    pageManager->addPageSettings(pageSettings1);

    KWPageSettings *pageSettings2 = new KWPageSettings("page2");
    lay = pageSettings2->pageLayout();
    lay.width = 50;
    lay.height = 100;
    pageSettings2->setPageLayout(lay);
    pageManager->addPageSettings(pageSettings2);
    
    KWPage *page1 = pageManager->appendPage(pageSettings1);
    KWPage *page2 = pageManager->appendPage(pageSettings2);
    pageManager->appendPage(pageManager->defaultPageSettings());

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
