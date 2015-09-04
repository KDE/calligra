/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#include "TestCustomSlideShows.h"

#include "KPrDocument.h"
#include "KoPAMasterPage.h"
#include "KoPAPage.h"
#include "PAMock.h"
#include <KPrCustomSlideShows.h>

#include <QTest>


void TestCustomSlideShows::populateDoc(MockDocument &doc, QList<KoPAPageBase*> &slideList1, QList<KoPAPageBase*> &slideList2,
                                       QString &customShowName1, QString &customShowName2)
{
    KoPAMasterPage *master1 = new KoPAMasterPage();
    doc.insertPage(master1, 0);

    KoPAPage *page1 = new KoPAPage(master1);
    doc.insertPage(page1, 0);

    KoPAPage *p1 = dynamic_cast<KoPAPage *>(doc.pageByIndex(0, false));
    KoPAMasterPage * m1 = dynamic_cast<KoPAMasterPage *>(doc.pageByIndex(0, true));

    QVERIFY(p1 != 0);
    QVERIFY(m1 != 0);

    KoPAPage *page2 = new KoPAPage(master1);
    doc.insertPage(page2, 0);

    KoPAPage *page3 = new KoPAPage(master1);
    doc.insertPage(page3, 0);

    KoPAPage *page4 = new KoPAPage(master1);
    doc.insertPage(page4, 0);

    KoPAPage *page5 = new KoPAPage(master1);
    doc.insertPage(page5, 0);

    //Create List 1 2 4 1 5
    slideList1.append(page1);
    slideList1.append(page2);
    slideList1.append(page4);
    slideList1.append(page1);
    slideList1.append(page5);

    //Create List 1 2 3 4 5
    slideList2.append(page1);
    slideList2.append(page2);
    slideList2.append(page3);
    slideList2.append(page4);
    slideList2.append(page5);

    customShowName1 = "test1";
    customShowName2 = "test2";

    doc.customSlideShows()->insert(customShowName1, slideList1);
    doc.customSlideShows()->insert(customShowName2, slideList2);
}

void TestCustomSlideShows::insertCustomSlideShow()
{
    // create some slide shows and insert then then test if you can access them again
    MockDocument doc;
    QList<KoPAPageBase*> slideList1;
    QList<KoPAPageBase*> slideList2;
    QString customShowName1;
    QString customShowName2;
    populateDoc(doc, slideList1, slideList2, customShowName1, customShowName2);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName1), slideList1);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName2), slideList2);
}

void TestCustomSlideShows::removeCustomSlideShow()
{
    // create some slide shows and insert then then test if you can access them again
    // then remove a´the slideshows again and test of they are no longer there
    MockDocument doc;
    QList<KoPAPageBase*> slideList1;
    QList<KoPAPageBase*> slideList2;
    QString customShowName1;
    QString customShowName2;
    populateDoc(doc, slideList1, slideList2, customShowName1, customShowName2);
    doc.customSlideShows()->remove(customShowName1);
    QList<QString> resultList;
    resultList.append(customShowName2);
    QCOMPARE(doc.customSlideShows()->names(), resultList);
}

void TestCustomSlideShows::updateCustomSlideShow()
{
    // create some slide shows and insert then then test if you can access them again
    // modify a slide show and update it
    // test if you get the updated slide show
    MockDocument doc;
    QList<KoPAPageBase*> slideList1;
    QList<KoPAPageBase*> slideList2;
    QString customShowName1;
    QString customShowName2;
    populateDoc(doc, slideList1, slideList2, customShowName1, customShowName2);

    QCOMPARE(doc.customSlideShows()->getByName(customShowName1), slideList1);

    doc.customSlideShows()->update(customShowName1, slideList2);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName1), slideList2);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName2), slideList2);
}

void TestCustomSlideShows::customSlideShowsNames()
{
    // insert different slide shows
    // test if you get the correct name of the slide shows
    MockDocument doc;
    QList<KoPAPageBase*> slideList1;
    QList<KoPAPageBase*> slideList2;
    QString customShowName1;
    QString customShowName2;
    populateDoc(doc, slideList1, slideList2, customShowName1, customShowName2);

    QList<QString> resultList;
    resultList.append(customShowName1);
    resultList.append(customShowName2);

    QCOMPARE(doc.customSlideShows()->names(), resultList);
}

void TestCustomSlideShows::getCustomSlideShowByName()
{
    // insert some slide shows
    // test if you can get each slide show correctly
}

void TestCustomSlideShows::addSlideToAllCustomSlideShows()
{
    MockDocument doc;

    KoPAMasterPage *master1 = new KoPAMasterPage();
    doc.insertPage(master1, 0);

    KoPAPage *page1 = new KoPAPage(master1);
    doc.insertPage(page1, 0);

    KoPAPage *p1 = dynamic_cast<KoPAPage *>(doc.pageByIndex(0, false));
    KoPAMasterPage * m1 = dynamic_cast<KoPAMasterPage *>(doc.pageByIndex(0, true));

    QVERIFY(p1 != 0);
    QVERIFY(m1 != 0);

    KoPAPage *page2 = new KoPAPage(master1);
    doc.insertPage(page2, 0);

    KoPAPage *page3 = new KoPAPage(master1);
    doc.insertPage(page3, 0);

    KoPAPage *page4 = new KoPAPage(master1);
    doc.insertPage(page4, 0);

    KoPAPage *page5 = new KoPAPage(master1);
    doc.insertPage(page5, 0);

    QList<KoPAPageBase*> slideList1;
    QList<KoPAPageBase*> slideList2;

    //Create List 1 2 4 1
    slideList1.append(page1);
    slideList1.append(page2);
    slideList1.append(page4);
    slideList1.append(page1);

    //Create List 1 2 3 4
    slideList2.append(page1);
    slideList2.append(page2);
    slideList2.append(page3);
    slideList2.append(page4);

    QString customShowName1 = "test 1";
    QString customShowName2 = "test 2";

    doc.customSlideShows()->insert(customShowName1, slideList1);
    doc.customSlideShows()->insert(customShowName2, slideList2);

    doc.customSlideShows()->addSlideToAll(page5, 0);

    slideList1.insert(0, page5);
    slideList2.insert(0, page5);

    QCOMPARE(doc.customSlideShows()->getByName(customShowName1), slideList1);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName2), slideList2);
}

void TestCustomSlideShows::addSlidesToAllCustomSlideShows()
{
    MockDocument doc;

    KoPAMasterPage *master1 = new KoPAMasterPage();
    doc.insertPage(master1, 0);

    KoPAPage *page1 = new KoPAPage(master1);
    doc.insertPage(page1, 0);

    KoPAPage *p1 = dynamic_cast<KoPAPage *>(doc.pageByIndex(0, false));
    KoPAMasterPage * m1 = dynamic_cast<KoPAMasterPage *>(doc.pageByIndex(0, true));

    QVERIFY(p1 != 0);
    QVERIFY(m1 != 0);

    KoPAPage *page2 = new KoPAPage(master1);
    doc.insertPage(page2, 0);

    KoPAPage *page3 = new KoPAPage(master1);
    doc.insertPage(page3, 0);

    KoPAPage *page4 = new KoPAPage(master1);
    doc.insertPage(page4, 0);

    KoPAPage *page5 = new KoPAPage(master1);
    doc.insertPage(page5, 0);

    QList<KoPAPageBase*> slideList1;
    QList<KoPAPageBase*> slideList2;

    //Create List 1 2 1
    slideList1.append(page1);
    slideList1.append(page2);
    slideList1.append(page1);

    //Create List 1 2 3
    slideList2.append(page1);
    slideList2.append(page2);
    slideList2.append(page3);

    QString customShowName1 = "test 1";
    QString customShowName2 = "test 2";

    doc.customSlideShows()->insert(customShowName1, slideList1);
    doc.customSlideShows()->insert(customShowName2, slideList2);

    QList<KoPAPageBase*> slideAddList;
    slideAddList.append(page4);
    slideAddList.append(page5);

    doc.customSlideShows()->addSlidesToAll(slideAddList, 0);

    slideList1.insert(0, page5);
    slideList1.insert(0, page4);
    slideList2.insert(0, page5);
    slideList2.insert(0, page4);

    QCOMPARE(doc.customSlideShows()->getByName(customShowName1), slideList1);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName2), slideList2);
}

void TestCustomSlideShows::removeSlideFromAllCustomSlideShows()
{
    MockDocument doc;

    KoPAMasterPage *master1 = new KoPAMasterPage();
    doc.insertPage(master1, 0);

    KoPAPage *page1 = new KoPAPage(master1);
    doc.insertPage(page1, 0);

    KoPAPage *p1 = dynamic_cast<KoPAPage *>(doc.pageByIndex(0, false));
    KoPAMasterPage * m1 = dynamic_cast<KoPAMasterPage *>(doc.pageByIndex(0, true));

    QVERIFY(p1 != 0);
    QVERIFY(m1 != 0);

    KoPAPage *page2 = new KoPAPage(master1);
    doc.insertPage(page2, 0);

    KoPAPage *page3 = new KoPAPage(master1);
    doc.insertPage(page3, 0);

    KoPAPage *page4 = new KoPAPage(master1);
    doc.insertPage(page4, 0);

    KoPAPage *page5 = new KoPAPage(master1);
    doc.insertPage(page5, 0);

    QList<KoPAPageBase*> slideList1;
    QList<KoPAPageBase*> slideList2;

    //Create List 1 2 4 1 5
    slideList1.append(page1);
    slideList1.append(page2);
    slideList1.append(page4);
    slideList1.append(page1);
    slideList1.append(page5);

    //Create List 1 2 3 4 5
    slideList2.append(page1);
    slideList2.append(page2);
    slideList2.append(page3);
    slideList2.append(page4);
    slideList2.append(page5);

    QString customShowName1 = "test 1";
    QString customShowName2 = "test 2";

    doc.customSlideShows()->insert(customShowName1, slideList1);
    doc.customSlideShows()->insert(customShowName2, slideList2);

    doc.customSlideShows()->removeSlideFromAll(page1);
    slideList1.removeAll(page1);
    slideList2.removeAll(page1);

    QCOMPARE(doc.customSlideShows()->getByName(customShowName1), slideList1);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName2), slideList2);
}

void TestCustomSlideShows::removeSlidesFromAllCustomSlideShows()
{
    MockDocument doc;

    KoPAMasterPage *master1 = new KoPAMasterPage();
    doc.insertPage(master1, 0);

    KoPAPage *page1 = new KoPAPage(master1);
    doc.insertPage(page1, 0);

    KoPAPage *p1 = dynamic_cast<KoPAPage *>(doc.pageByIndex(0, false));
    KoPAMasterPage * m1 = dynamic_cast<KoPAMasterPage *>(doc.pageByIndex(0, true));

    QVERIFY(p1 != 0);
    QVERIFY(m1 != 0);

    KoPAPage *page2 = new KoPAPage(master1);
    doc.insertPage(page2, 0);

    KoPAPage *page3 = new KoPAPage(master1);
    doc.insertPage(page3, 0);

    KoPAPage *page4 = new KoPAPage(master1);
    doc.insertPage(page4, 0);

    KoPAPage *page5 = new KoPAPage(master1);
    doc.insertPage(page5, 0);

    QList<KoPAPageBase*> slideList1;
    QList<KoPAPageBase*> slideList2;

    //Create List 1 2 4 1 5
    slideList1.append(page1);
    slideList1.append(page2);
    slideList1.append(page4);
    slideList1.append(page1);
    slideList1.append(page5);

    //Create List 1 2 3 4 5
    slideList2.append(page1);
    slideList2.append(page2);
    slideList2.append(page3);
    slideList2.append(page4);
    slideList2.append(page5);


    QString customShowName1 = "test 1";
    QString customShowName2 = "test 2";

    doc.customSlideShows()->insert(customShowName1, slideList1);
    doc.customSlideShows()->insert(customShowName2, slideList2);

    QList<KoPAPageBase*> slideRemoveList;
    slideRemoveList.append(page4);
    slideRemoveList.append(page5);

    doc.customSlideShows()->removeSlidesFromAll(slideRemoveList);

    slideList1.removeAll(page4);
    slideList1.removeAll(page5);
    slideList2.removeAll(page4);
    slideList2.removeAll(page5);

    QCOMPARE(doc.customSlideShows()->getByName(customShowName1), slideList1);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName2), slideList2);
}

QTEST_MAIN(TestCustomSlideShows)
