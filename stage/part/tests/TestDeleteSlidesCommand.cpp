#include "TestDeleteSlidesCommand.h"

#include "KPrDocument.h"
#include "KoPAMasterPage.h"
#include "KoPAPage.h"
#include "PAMock.h"
#include "commands/KPrDeleteSlidesCommand.h"
#include "KPrCustomSlideShows.h"
#include <qtest_kde.h>


void TestDeleteSlidesCommand::delSlide()
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

    QList<KoPAPageBase*> slideList;

    slideList.append(page1);
    slideList.append(page2);
    slideList.append(page3);

    QString customShowName1 = "test 1";
    QString customShowName2 = "test 2";

    doc.customSlideShows()->insert(customShowName1, slideList);
    doc.customSlideShows()->insert(customShowName2, slideList);

    QCOMPARE(doc.customSlideShows()->names().count(), 2);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName1).count(), 3);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName2).count(), 3);

    KoPAPage *p2 = dynamic_cast<KoPAPage *>(doc.pageByIndex(1, false));
    KoPAPage *p3 = dynamic_cast<KoPAPage *>(doc.pageByIndex(2, false));
    QVERIFY(p2 != 0);
    QVERIFY(p3 != 0);

    KPrDeleteSlidesCommand cmd(&doc, page2);

    cmd.redo();
    //Page removed from document
    QVERIFY(!doc.pages(false).contains(p2));
    //Page removed from custom slide shows
    QCOMPARE(doc.customSlideShows()->getByName(customShowName1).count(), 2);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName2).count(), 2);
    QVERIFY(!doc.customSlideShows()->getByName(customShowName1).contains(page2));
    QVERIFY(!doc.customSlideShows()->getByName(customShowName2).contains(page2));

    cmd.undo();
    QCOMPARE(doc.customSlideShows()->getByName(customShowName1).count(), 3);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName2).count(), 3);
    QVERIFY(doc.customSlideShows()->getByName(customShowName1).contains(page2));
    QVERIFY(doc.customSlideShows()->getByName(customShowName2).contains(page2));
    QVERIFY(doc.pages(false).contains(p2));
}

void TestDeleteSlidesCommand::delSlideWithCopies()
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

    QList<KoPAPageBase*> slideList;

    slideList.append(page1);
    slideList.append(page2);
    slideList.append(page2);
    slideList.append(page3);

    QString customShowName1 = "test 1";

    doc.customSlideShows()->insert(customShowName1, slideList);

    QCOMPARE(doc.customSlideShows()->names().count(), 1);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName1).count(), 4);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName1).count(page2), 2);

    KoPAPage *p2 = dynamic_cast<KoPAPage *>(doc.pageByIndex(1, false));
    KoPAPage *p3 = dynamic_cast<KoPAPage *>(doc.pageByIndex(2, false));
    QVERIFY(p2 != 0);
    QVERIFY(p3 != 0);

    KPrDeleteSlidesCommand cmd(&doc, page2);

    cmd.redo();
    //page removed from document
    QVERIFY(!doc.pages(false).contains(p2));
    //page removed from custom shows
    QCOMPARE(doc.customSlideShows()->getByName(customShowName1).count(), 2);
    QVERIFY(!doc.customSlideShows()->getByName(customShowName1).contains(page2));

    cmd.undo();
    QCOMPARE(doc.customSlideShows()->getByName(customShowName1).count(), 4);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName1).count(page2), 2);
    QVERIFY(doc.pages(false).contains(p2));
}

void TestDeleteSlidesCommand::delMultipleSlides()
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

    QList<KoPAPageBase*> slideList;

    slideList.append(page1);
    slideList.append(page2);
    slideList.append(page3);

    QString customShowName1 = "test 1";
    QString customShowName2 = "test 2";

    doc.customSlideShows()->insert(customShowName1, slideList);
    doc.customSlideShows()->insert(customShowName2, slideList);

    QCOMPARE(doc.customSlideShows()->names().count(), 2);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName1).count(), 3);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName2).count(), 3);

    KoPAPage *p2 = dynamic_cast<KoPAPage *>(doc.pageByIndex(1, false));
    KoPAPage *p3 = dynamic_cast<KoPAPage *>(doc.pageByIndex(2, false));
    QVERIFY(p2 != 0);
    QVERIFY(p3 != 0);

    QList<KoPAPageBase*> deleteList;
    deleteList.append(page1);
    deleteList.append(page2);

    KPrDeleteSlidesCommand cmd(&doc, deleteList);

    cmd.redo();
    //pages removed from document
    QVERIFY(!doc.pages(false).contains(p1));
    QVERIFY(!doc.pages(false).contains(p2));
    //pages removed from custom shows
    QCOMPARE(doc.customSlideShows()->getByName(customShowName1).count(), 1);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName2).count(), 1);
    QVERIFY(!doc.customSlideShows()->getByName(customShowName1).contains(page1));
    QVERIFY(!doc.customSlideShows()->getByName(customShowName2).contains(page1));
    QVERIFY(!doc.customSlideShows()->getByName(customShowName1).contains(page2));
    QVERIFY(!doc.customSlideShows()->getByName(customShowName2).contains(page2));

    cmd.undo();
    QVERIFY(doc.pages(false).contains(p1));
    QVERIFY(doc.pages(false).contains(p2));
    QCOMPARE(doc.customSlideShows()->getByName(customShowName1).count(), 3);
    QCOMPARE(doc.customSlideShows()->getByName(customShowName2).count(), 3);
    QVERIFY(doc.customSlideShows()->getByName(customShowName1).contains(page1));
    QVERIFY(doc.customSlideShows()->getByName(customShowName2).contains(page1));
    QVERIFY(doc.customSlideShows()->getByName(customShowName1).contains(page2));
    QVERIFY(doc.customSlideShows()->getByName(customShowName2).contains(page2));
}

QTEST_KDEMAIN(TestDeleteSlidesCommand, GUI)
#include "TestDeleteSlidesCommand.moc"
