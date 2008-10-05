/* This file is part of the KOffice project
 * Copyright (C) 2005,2008 Thomas Zander <zander@kde.org>
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
#include "TestPageCommands.h"

#include <KWPage.h>
#include <KWDocument.h>

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

QTEST_KDEMAIN(TestPageCommands, GUI)
#include "TestPageCommands.moc"
