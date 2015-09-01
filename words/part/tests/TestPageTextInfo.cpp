/* This file is part of the Calligra project
 * Copyright (C) 2008 Thomas Zander <zander@kde.org>
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
#include "TestPageTextInfo.h"

#include <KWPageManager.h>
#include <KWPage.h>
#include <KWPageTextInfo.h>

void TestPageTextInfo::init()
{
}

void TestPageTextInfo::testPageNumber()
{
    KWPageManager manager;
    QCOMPARE(manager.pageCount(), 0);

    KWPage page1 = manager.appendPage();
    KWPage page2 = manager.appendPage();
    KWPage page3 = manager.appendPage();
    KWPage page4 = manager.appendPage();
    KWPage page5 = manager.appendPage();

    KWPageStyle style2("foo");
    manager.addPageStyle(style2);
    KWPage page40 = manager.appendPage(style2);
    QCOMPARE(page40.pageNumber(), 6);
    page40.setPageNumber(40);
    QCOMPARE(page5.pageNumber(), 5);
    QCOMPARE(page40.pageNumber(), 40);
    KWPage page41 = manager.appendPage();
    QCOMPARE(page41.pageNumber(), 41);

    KWPageTextInfo info1(page1);
    QCOMPARE(info1.pageNumber(KoTextPage::CurrentPage, 0), 1);
    QCOMPARE(info1.pageNumber(KoTextPage::PreviousPage, 0), -1);
    QCOMPARE(info1.pageNumber(KoTextPage::NextPage, 0), 2);

    QCOMPARE(info1.pageNumber(KoTextPage::CurrentPage, 4), 5);
    QCOMPARE(info1.pageNumber(KoTextPage::CurrentPage, 5), -1);
    QCOMPARE(info1.pageNumber(KoTextPage::CurrentPage, 40), 41);
    QCOMPARE(info1.pageNumber(KoTextPage::PreviousPage, 1), -1);
    QCOMPARE(info1.pageNumber(KoTextPage::NextPage, 3), 5);
    QCOMPARE(info1.pageNumber(KoTextPage::NextPage, 4), -1);

    KWPageTextInfo info2(page2);
    QCOMPARE(info2.pageNumber(KoTextPage::CurrentPage, 0), 2);
    QCOMPARE(info2.pageNumber(KoTextPage::PreviousPage, 0), 1);
    QCOMPARE(info2.pageNumber(KoTextPage::NextPage, 0), 3);

    KWPageTextInfo info3(page5);
    QCOMPARE(info3.pageNumber(KoTextPage::CurrentPage, 0), 5);
    QCOMPARE(info3.pageNumber(KoTextPage::PreviousPage, 0), 4);
    QCOMPARE(info3.pageNumber(KoTextPage::NextPage, 0), 40);

    KWPageTextInfo info40(page40);
    QCOMPARE(info40.pageNumber(KoTextPage::CurrentPage, 0), 40);
    QCOMPARE(info40.pageNumber(KoTextPage::PreviousPage, 0), 5);
    QCOMPARE(info40.pageNumber(KoTextPage::NextPage, 0), 41);

    KWPageTextInfo info41(page41);
    QCOMPARE(info41.pageNumber(KoTextPage::CurrentPage, 0), 41);
    QCOMPARE(info41.pageNumber(KoTextPage::PreviousPage, 0), 40);
    QCOMPARE(info41.pageNumber(KoTextPage::NextPage, 0), -1);
}

QTEST_KDEMAIN(TestPageTextInfo, GUI)
